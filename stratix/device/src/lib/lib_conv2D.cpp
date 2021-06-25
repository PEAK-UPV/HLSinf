// --------------------------------------------------------------------------------------------------------------
// HLSinf kernels
// Version: 1.0
// copyright (c) 2021, Universidad Politècnica de València (UPV), GAP research group
// Date: May 2021
// Authors: GAP Research Group (UPV)
//     José Flich Cardo
//     Jorge García Martínez
//     Izan Catalán Gallarch
//     Carles Hernández Luz
//     Jose Maria Martinez
//
// contact: jflich@disca.upv.es
// All rights reserved
//
// 
//
// WARNING: 
// Libraries that target OpenCL* and are written in HLS cannot
// use streams or pipes as an interface between OpenCL code
// and the library written in HLS.
// However, the library in HLS can use streams or pipes if both
// endpoints are within the library (for example, a stream that
// connects two task functions).

#include "HLS/hls.h"
#include "HLS/math.h"
#include "HLS/ocl_types.h"
//#ifndef HLS_SYNTHESIS
//  #include <iostream>
//#endif

#include "HLS/stdio.h"


//ihc::stream<float> s0, s1;
//ihc::stream<float> s0;
//ihc::stream<float> s1;


#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)



#include "lib_conv2D_commons.h"




#include "file1.cpp"
#include "file2.cpp"

// HLS sources included files
#include "lib_hdrs_priv.h"


#include "add.cpp"
#include "cvt.cpp"
#include "direct_convolution.cpp"
#include "dws_convolution.cpp"
#include "join_split.cpp"
#include "mul.cpp"
#include "padding.cpp"
#include "pooling.cpp"
#include "read.cpp"
#include "relu.cpp"
#include "serialization.cpp"
//#include "winograd_convolution"
#include "write.cpp"

//extern void read_bias(int offset_bias, pixel_out_t *b_ptr, ihc::stream<pixel_out_t> &out);

void set_write_enables(int enable_write[CPO], int o_channel, int O) {
  set_write_enables_loop:
  #pragma unroll
  for (int o = 0; o <CPO; o++) {
    //DO_PRAGMA(HLS loop_tripcount min=1 max=CPO)
    enable_write[o] = (o_channel + o) < O;
  }
}

void set_reading_channel_offsets(int offset_read_data_channel_i[CPI], int offset_read_data_channel, int channel_offset) {
 set_reading_channel_offsets_loop:
  #pragma unroll
  for(int i=0; i<CPI; i++){
    //DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
    offset_read_data_channel_i[i] = (offset_read_data_channel + i * channel_offset) % READ_BLOCK_SIZE;
  }
}


void set_channel_write_blocks(int num_channel_write_blocks[CPO], int H, int W) {
  set_channel_write_blocks_loop:
  #pragma unroll
  for(int i=0; i<CPO; i++) {
    num_channel_write_blocks[i] = ((H * W) + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
  }
}


#include <iostream>
#include <string.h>

#ifdef HLS_DEBUG
ihc::stream<pixel_in_t>    dbg_loop_stream_data_in[MAX_O_ITERS];
ihc::stream<pixel_in_t>    dbg_loop_stream_data_input_buffer[MAX_O_ITERS];

// direct convolution streams
ihc::stream<pixel_out_t>   dbg_loop_stream_data_directconv_out[MAX_O_ITERS];
ihc::stream<pixel_in_t>    dbg_loop_stream_data_dc_pad_out[MAX_O_ITERS];
ihc::stream<frame_t>       dbg_loop_stream_data_dc_cvt_out[MAX_O_ITERS];
ihc::stream<pixel_out_t>   dbg_loop_stream_data_dc_mul_out[MAX_O_ITERS];
//ihc::stream<pixel_out_t>    dbg_loop_stream_dc_add_out; directconv_out

unsigned long dbg_loop_stream_data_dc_pad_out_counter;
unsigned long dbg_loop_stream_data_dc_cvt_out_counter;
unsigned long dbg_loop_stream_data_dc_mul_out_counter;
unsigned long dbg_loop_stream_data_dc_add_out_counter;

#endif


#ifdef HLS_DEBUG
unsigned long int dbg_elements_per_iter_data_in[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_kernel[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_bias[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_input_buffer[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_pad_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_cvt_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_mul_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_add_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_direcconv_out[MAX_O_ITERS];
#endif


extern "C" {
  
  // **********************************************************************************************
  // library main component 
  // ********************************************************************************************** 
HLS_EXTERNAL void lib_conv2D( 
      OCL_ADDRSP_GLOBAL read_block_t *ptr_data,
      int H, 
      int W, 
      int rows,
      int I,
      int O,
      int I_ITER,
      int o_iter_first,
      int o_iter_last,
      int enable_relu,
      #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
      OCL_ADDRSP_GLOBAL data_type *ptr_kernel,
      #endif
      #ifdef DWS_CONV
      OCL_ADDRSP_GLOBAL data_type *ptr_dw_kernel, 
      OCL_ADDRSP_GLOBAL read_kernel_pw_t *ptr_pw_kernel,
      #endif
      OCL_ADDRSP_GLOBAL pixel_out_t *ptr_bias,
      OCL_ADDRSP_GLOBAL write_block_t *ptr_out,
      int global_offset,
      int enable_upper_padding,
      int enable_lower_padding,
      int enable_maxpooling,
      int enable_avgpooling,
      int enable_clipping,
      int enable_shift,
      int min_clip,
      int max_clip,
      int dir_shift,
      int pos_shift
#ifdef HLS_DEBUG
        ,
      unsigned long                    my_val,
      OCL_ADDRSP_GLOBAL unsigned long  *my_ret,
      OCL_ADDRSP_GLOBAL unsigned long  *my_ret_2,
      OCL_ADDRSP_GLOBAL unsigned long  *my_ret_3,
      OCL_ADDRSP_GLOBAL unsigned long  *my_ret_4,
      OCL_ADDRSP_GLOBAL float          *my_flt_bias,
      OCL_ADDRSP_GLOBAL float          *my_flt_krnl,
      OCL_ADDRSP_GLOBAL float          *my_flt_din,
      OCL_ADDRSP_GLOBAL float          *my_flt_dout,
      OCL_ADDRSP_GLOBAL read_block_t   *dbg_loop_ptr_data_in,
      OCL_ADDRSP_GLOBAL read_block_t   *dbg_loop_ptr_data_input_buffer,

      OCL_ADDRSP_GLOBAL read_block_t   *dbg_loop_ptr_data_dc_pad_out,
      OCL_ADDRSP_GLOBAL frame_t        *dbg_loop_ptr_data_dc_cvt_out,
      OCL_ADDRSP_GLOBAL write_block_t  *dbg_loop_ptr_data_dc_mul_out,
      OCL_ADDRSP_GLOBAL write_block_t  *dbg_loop_ptr_data_directconv_out
#endif
  ) {

  #ifdef HLS_DEBUG
  *my_ret = my_val;
  *my_flt_bias = 0.0;
  *my_flt_krnl = 0.0;
  *my_flt_din  = 0.0;
  *my_flt_dout = 0.0;

  dbg_loop_stream_data_dc_pad_out_counter = 0;
  dbg_loop_stream_data_dc_cvt_out_counter = 0;
  dbg_loop_stream_data_dc_mul_out_counter = 0;
  dbg_loop_stream_data_dc_add_out_counter = 0;
  #endif

  #ifdef HLS_DEBUG
  data_type  dbg_flt_bias_sum[MAX_O_ITERS];
  data_type  dbg_flt_krnl_sum[MAX_O_ITERS];
  data_type  dbg_flt_din_sum[MAX_O_ITERS];
  data_type  dbg_flt_dout_sum[MAX_O_ITERS];

  for(int ind = 0; ind < MAX_O_ITERS; ind++) {
    dbg_flt_bias_sum[ind] = (data_type)0;
    dbg_flt_krnl_sum[ind] = (data_type)0;
    dbg_flt_din_sum[ind] = (data_type)0;
    dbg_flt_dout_sum[ind] = (data_type)0;
  }


  for(int ind = 0; ind < MAX_O_ITERS; ind++) {
    dbg_elements_per_iter_data_in[ind] = 0;
    dbg_elements_per_iter_data_input_buffer[ind] = 0;
    dbg_elements_per_iter_data_dc_pad_out[ind] = 0;
    dbg_elements_per_iter_data_dc_cvt_out[ind] = 0;
    dbg_elements_per_iter_data_dc_mul_out[ind] = 0;
    dbg_elements_per_iter_data_dc_add_out[ind] = 0;
    dbg_elements_per_iter_data_dc_direcconv_out[ind] = 0;
  }
  #endif

  // Here begins the real k_conv2D kernel
  
  // I did not find yet the way to include pritf/std::cout msgs for crossgen/libtool
  //   So let's disable all print msgs
  //  printf("kernel starts...\n");
  

  int O_ITER = o_iter_last - o_iter_first + 1;

  o_iter_loop:
  for (int o_iter = 0; o_iter<O_ITER; o_iter++) {
 
	//DO_PRAGMA(HLS loop_tripcount min=1 max=O_REFERENCE/CPO)
	//#pragma HLS dataflow

	  int o_channel = (o_iter + o_iter_first) << LOG2_CPO;  // current output channel (first one in this iteration)
    // input and output streams

    
    ihc::stream<pixel_in_t>   out_read_data;
    ihc::stream<pixel_in_t>   out_read_data_1;
    ihc::stream<pixel_in_t>   out_read_data_2;

 	// DIRECT CONV, WINOGRAD, DWS
    #ifdef DIRECT_CONV
    ihc::stream<kernel_t>     out_read_kernel;
    #endif
    #ifdef WINOGRAD_CONV
    ihc::stream<kernel_t>     out_read_kernel;
    #endif
    #ifdef DWS_CONV
    ihc::stream<kernel_dw_t>     str_dw_kernel;
    ihc::stream<kernel_pw_t>     str_pw_kernel;
    #endif

    ihc::stream<pixel_out_t>  out_read_bias;
    ihc::stream<pixel_out_t>  out_conv;

	// RELU, CLIPPING, and SHIFT support
    #if defined(USE_RELU) || defined(USE_CLIPPING) || defined(USE_SHIFT)
    ihc::stream<pixel_out_t>  out_relu;
    #endif

    // MAXPOOLING, AVGPOOLING
    #ifdef USE_POOLING
    ihc::stream<pixel_out_t>  out_pooling;
    #endif

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // streams for read and write data
    #ifdef IHW_DATA_FORMAT
    ihc::stream<read_block_t>  stream_data_ch_0[CPI];
    ihc::stream<data_type>     stream_data_ch_1[CPI];
    ihc::stream<data_type>     out_write_channel[CPO];
    ihc::stream<write_block_t> out_block_write_channel[CPO];
    #endif

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // variables to manage the input buffer
    int corrected_offset         = (enable_upper_padding==0)? W : 0;
    int num_extra_rows           = (enable_lower_padding == 0) + (enable_upper_padding == 0);
    int read_pixels              = W * (rows + num_extra_rows);
    int read_pixels_total        = read_pixels * I_ITER;
    //printf("I_ITER %d W %d rows %d num_extra_rows %d read_pixels %d read_pixels_total %d\n", I_ITER, W, rows, num_extra_rows, read_pixels, read_pixels_total);
    int enable_buffer            = (read_pixels * (I / CPI)) <= INPUT_BUFFER_SIZE;
    int write_to_input_buffer    = enable_buffer && (o_iter == 0) && (O_ITER>1);
    int read_from_input_buffer   = enable_buffer && (o_iter != 0);
    int enable_read              = (o_iter == 0) || !enable_buffer;
    //printf("enable_buffer %d write_to_input_buffer %d read_from_input_buffer %d enable_read %d\n", enable_buffer, write_to_input_buffer, read_from_input_buffer, enable_read);

    // variables
    int enable_write[CPO];
    int offset_read_data_channel_i[CPI];
    int num_channel_write_blocks[CPO];
    int offset_read_data_channel = global_offset - corrected_offset;
    int channel_size             = H * W;


    int enable_pooling           = enable_maxpooling | enable_avgpooling;

    int read_channel_offset      = (W * H);


    #ifdef USE_POOLING
    int write_pixels             = enable_pooling ? (rows * W / 4) : (rows * W);
    int write_rows               = enable_pooling ? rows/2 : rows;
    int write_cols               = enable_pooling ? W/2 : W;
    int write_channel_offset     = enable_pooling ? (W * H) / 4 : (W * H);
    #else
    int write_pixels             = rows * W;
    int write_rows               = rows;
    int write_cols               = W;
    int write_channel_offset     = (W * H);
    #endif

    #ifdef IHW_DATA_FORMAT
    int o_iter_write_offset      = (global_offset + (o_channel * write_channel_offset)) / WRITE_BLOCK_SIZE;
    #endif
    #ifdef GIHWCPI_DATA_FORMAT
    int o_iter_write_offset      = write_pixels * (o_iter + o_iter_first);
    #endif

    int read_channel_blocks      = (read_pixels + READ_BLOCK_SIZE - 1) / READ_BLOCK_SIZE;
    int offset_bias              = o_iter + o_iter_first;
    #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
    int offset_kernel            = (o_iter + o_iter_first) * ((I + CPI - 1) / CPI) * CPI * CPO * 9;
    #endif

    
    // we compute the enable_write signals
    set_write_enables(enable_write, o_channel, O);

    // channel offsets for reading
    set_reading_channel_offsets(offset_read_data_channel_i, offset_read_data_channel, read_channel_offset);

    // channel write blocks
    set_channel_write_blocks(num_channel_write_blocks, H, W);
    

     
    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // read bias
    //read_bias(offset_bias, ptr_bias, out_read_bias);
    //void read_bias(int offset_bias, pixel_out_t *b_ptr, ihc::stream<pixel_out_t> &out)
    //{
    //  pixel_out_t bias;
    //  //#pragma HLS ARRAY_PARTITION variable=bias complete dim=0
    //  bias =(pixel_out_t *) &b_ptr[offset_bias];
    //  out_read_bias.write(bias);
    //}
    {
      struct pixel_out_t bias;
      // original not working overload "="
      //bias =  ptr_bias[offset_bias];

      // sol 1
      //struct pixel_out_t *my_src;
      //my_src = (pixel_out_t *) &ptr_bias[offset_bias];
      //bias = *my_src;

      // sol 2
      memcpy((void*)&bias, (void*)&ptr_bias[offset_bias], sizeof(pixel_out_t));

      #ifdef HLS_DEBUG
      for (int lopi = 0; lopi < sizeof(pixel_out_t); lopi++) {
        dbg_flt_bias_sum[o_iter] = dbg_flt_bias_sum[o_iter] + bias.pixel[lopi];
      }
      
      dbg_elements_per_iter_bias[o_iter] = dbg_elements_per_iter_bias[o_iter] + 1;
      #endif
         
      //#pragma HLS ARRAY_PARTITION variable=bias complete dim=0          
      out_read_bias.write(bias);
    }

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // read kernel (different version based on the type of convolution)
    //#ifdef DIRECT_CONV
    //read_kernel(I_ITER, offset_kernel, ptr_kernel, out_read_kernel);
    //#endif
    //#ifdef WINOGRAD_CONV
    //read_kernel(I_ITER, offset_kernel, ptr_kernel, out_read_kernel);
    //#endif
    #if defined (DIRECT_CONV) || defined (WINOGRAD_CONV)
    //read_kernel(I_ITER, offset_kernel, ptr_kernel, out_read_kernel);
    //void read_kernel(int I_ITER, int offset_kernel, data_type *k_ptr, ihc::stream<kernel_t> &k_out)
    {
      int cnt = 0;
      //#pragma HLS array_partition variable=k complete dim=0

      for (int i=0; i<I_ITER; i++)
      {
        kernel_t k;

        //DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)
        //DO_PRAGMA(HLS LOOP_FLATTEN OFF)
        for (int cpo=0; cpo < CPO; cpo++) {
          for (int cpi=0; cpi < CPI; cpi++) {
            for (int p=0; p<9; p++) {
              //#pragma HLS pipeline II=1
              //k.pixel[cpo][cpi][p] = ptr_kernel[offset_kernel+cnt];
              memcpy((void*)&k.pixel[cpo][cpi][p], (void*)&ptr_kernel[offset_kernel+cnt], sizeof(data_type));

              #ifdef HLS_DEBUG
              // JM10 debug - accumulated value for kernel
              dbg_flt_krnl_sum[o_iter] = dbg_flt_krnl_sum[o_iter] + ptr_kernel[offset_kernel+cnt];
              // end of JM10 debug
              #endif

              cnt = cnt + 1;
            }
          }
        }
        out_read_kernel.write(k);
        #ifdef HLS_DEBUG
        dbg_elements_per_iter_kernel[o_iter] = dbg_elements_per_iter_kernel[o_iter] + 1;
        #endif
      }
    }
    #endif


    #ifdef DWS_CONV
    dws_read_dw_kernel(I_ITER, o_iter, ptr_dw_kernel, str_dw_kernel);  // o_iter as argument to load all kernels in the first iteration (o_iter==0)
    dws_read_pw_kernel(I_ITER, O, o_iter + o_iter_first, ptr_pw_kernel, str_pw_kernel); // o_iter+o_iter_ifrst sent to let the module compute the offset to read the kernels
    #endif

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Read and preparation of pixels, depending on the data format we select the modules to use
    #ifdef IHW_DATA_FORMAT
    read_data_channels(H, W, rows, I_ITER, ptr_data, offset_read_data_channel, num_extra_rows, read_channel_blocks, stream_data_ch_0, I, enable_read);
    ch_serialize_and_filter<CPI>(I_ITER, read_pixels, read_channel_blocks, stream_data_ch_0, stream_data_ch_1, I, enable_read);
    join(rows, W, I_ITER, num_extra_rows, enable_read, stream_data_ch_1,  out_read_data);
    input_buffer(read_pixels_total, write_to_input_buffer, read_from_input_buffer, out_read_data, out_read_data_1);
    #endif
    #ifdef GIHWCPI_DATA_FORMAT
    //read_data_channels_gihwcpi(  read_pixels_total, offset_read_data_channel,           ptr_data,                 out_read_data, enable_read);
    //void read_data_channels_gihwcpi(int num_pixels,               int offset,  read_block_t *ptr,  ihc::stream<pixel_in_t> &out,  int enable) 
    //{

    // for this configuration 
    //  read_block_t  is defined as struct pixel_in_t
    //  write_block_t is defined as struct pixel_out_t
    if (enable_read) { 
      unsigned long rdclp_bytes_to_copy = sizeof(struct pixel_in_t);

      read_data_channels_loop_pixels:
      for (int i = 0; i < read_pixels_total; i++) {
        struct pixel_in_t px_din;

        //DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max = I_REFERENCE * H_REFERENCE * W_REFERENCE / CPI)
        //DO_PRAGMA(HLS pipeline)
        memcpy((void*)&px_din, (void*)&(ptr_data[offset_read_data_channel+i]), rdclp_bytes_to_copy);
        out_read_data.write(px_din);

        #ifdef HLS_DEBUG
        for (int lopi = 0; lopi < CPI; lopi++) {
          dbg_flt_din_sum[o_iter] = dbg_flt_din_sum[o_iter] + px_din.pixel[lopi];
        }
        dbg_elements_per_iter_data_in[o_iter] = dbg_elements_per_iter_data_in[o_iter] + 1;
        dbg_loop_stream_data_in[o_iter].write(px_din);    
        
        #endif
        
      }
    }
    //}



    input_buffer( o_iter, read_pixels_total, write_to_input_buffer, read_from_input_buffer, out_read_data, out_read_data_1
        );
    #endif

    // JM10 debug- 2021_06_09  kernel
    //   all code seems woring fine upt to this point
    //   data_in, kernel and bias buffers are properly read 
    //   data_in is properly copied into out_read_data_1 stream


    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // convolution: Direct, Winograd, DWS
    #ifdef DIRECT_CONV
    direct_conv(o_iter, rows, W, I_ITER, enable_upper_padding, enable_lower_padding, out_read_data_1, out_read_kernel, out_read_bias, out_conv);
    #endif
    #ifdef WINOGRAD_CONV
    winograd_conv(rows, W, I_ITER, enable_upper_padding, enable_lower_padding, out_read_data_1, out_read_kernel, out_read_bias, out_conv);
    #endif
    #ifdef DWS_CONV
    dws_conv(rows, W, I_ITER, enable_upper_padding, enable_lower_padding, out_read_data_1, str_dw_kernel, str_pw_kernel, out_read_bias, out_conv);
    #endif
    

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    // Relu, Clipping, shift, and pooling
    #if defined(USE_RELU) || defined(USE_CLIPPING) || defined(USE_SHIFT)
      relu(enable_relu, enable_clipping, enable_shift, min_clip, max_clip, dir_shift, pos_shift, rows, W, out_conv, out_relu);
      // Pooling: avgpooling or maxpooling
      #ifdef USE_POOLING
        pooling(H, W, enable_maxpooling, enable_avgpooling, out_relu, out_pooling);
      #endif
    #else
      // Pooling: avgpooling or maxpooling
      #ifdef USE_POOLING
        pooling(H, W, enable_maxpooling, enable_avgpooling, out_conv, out_pooling);
      #endif
    #endif
    
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    // preparation of pixels and write on memory: Depending on the data format we build the modules structure
    #ifdef IHW_DATA_FORMAT
      #if defined(USE_RELU) || defined(USE_CLIPPING) || defined(USE_SHIFT)
        #ifdef USE_POOLING
        split(write_rows, write_cols, out_pooling, out_write_channel);
        ch_block_generate<CPO>(write_rows, write_cols, out_write_channel, out_block_write_channel);
        write_data_channels(write_pixels, o_iter_write_offset, ptr_out, out_block_write_channel, enable_write);
        #else
        split(write_rows, write_cols, out_relu, out_write_channel);
        ch_block_generate<CPO>(write_rows, write_cols, out_write_channel, out_block_write_channel);
        write_data_channels(write_pixels, o_iter_write_offset, ptr_out, out_block_write_channel, enable_write);
        #endif
      #else
        #ifdef USE_POOLING
        split(write_rows, write_cols, out_pooling, out_write_channel);
        ch_block_generate<CPO>(write_rows, write_cols, out_write_channel, out_block_write_channel);
        write_data_channels(write_pixels, o_iter_write_offset, ptr_out, out_block_write_channel, enable_write);
        #else
        split(write_rows, write_cols, out_conv, out_write_channel);
        ch_block_generate<CPO>(write_rows, write_cols, out_write_channel, out_block_write_channel);
        write_data_channels(write_pixels, o_iter_write_offset, ptr_out, out_block_write_channel, enable_write);
        #endif
      #endif
    #endif
    #ifdef GIHWCPI_DATA_FORMAT
      #if defined(USE_RELU) || defined(USE_CLIPPING) || defined(USE_SHIFT)
        #ifdef USE_POOLING

        // JM10 debug
        //#ifdef HLS_DEBUG
        //*my_ret_2 = write_pixels;
        //*my_ret_3 = o_iter_write_offset;
        //*my_ret_4 = CPO;
        //#endif
        // end of JM10 debug

        //write_data_channels_gihwcpi(write_pixels, o_iter_write_offset, ptr_out, out_pooling);
        //void write_data_channels_gihwcpi(int num_pixels, int offset, write_block_t *ptr, ihc::stream<pixel_out_t> &in)
        //{
          write_data_channels_loop_pixels:
          for (int i = 0; i < write_pixels; i++) {
            //DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE)
            //#pragma HLS pipeline
            write_block_t bx = out_pooling.read();
            //ptr_out[o_iter_write_offset+i] = bx;

            // JM10 debug trick out
            //bx.pixel[0] = 0.34;
            //for(int lopi = 1; lopi < CPO; i++){ bx.pixel[lopi]= 0.0;}
            
            #ifdef HLS_DEBUG
            *my_flt_dout = *my_flt_dout + bx.pixel[0] + bx.pixel[1] + bx.pixel[2] + bx.pixel[3]; 
            #endif

            //memcpy((void*)&ptr_out[o_iter_write_offset+i], (void*)&bx, sizeof(struct pixel_out_t));
            memcpy((void*)&(ptr_out[o_iter_write_offset+i]), (void*)&bx, sizeof(write_block_t));
          }
        //}
        #else
        write_data_channels_gihwcpi(write_pixels, o_iter_write_offset, ptr_out, out_relu, enable_write);
        #endif
      #else
        #ifdef USE_POOLING
        write_data_channels_gihwcpi(write_pixels, o_iter_write_offset, ptr_out, out_pooling, enable_write);
        #else
        write_data_channels_gihwcpi(write_pixels, o_iter_write_offset, ptr_out, out_conv, enable_write);
        #endif
      #endif
    #endif
        
    //---------------------------------------------------------------------------------------- end of code for preparation and write

    

#ifdef HLS_DEBUG

        //*my_ret   = dbg_loop_stream_data_dc_pad_out_counter;
        //*my_ret_2 = dbg_loop_stream_data_dc_cvt_out_counter;
        //*my_ret_3 = dbg_loop_stream_data_dc_mul_out_counter;
        //*my_ret_4 = dbg_loop_stream_data_dc_add_out_counter;
/*
        *my_ret   = dbg_elements_per_iter_data_in[HLS_O_ITER_MONITOR];
        *my_ret_2 = dbg_elements_per_iter_data_input_buffer[HLS_O_ITER_MONITOR];
        *my_ret_3 = dbg_elements_per_iter_data_dc_pad_out[HLS_O_ITER_MONITOR];
        *my_ret_4 = dbg_elements_per_iter_data_dc_cvt_out[HLS_O_ITER_MONITOR];

        // the correct one is the complete, but it would require to send the number of reads back to the host app to know how many data read from memory
        //if(enable_read)  or  if ( (o_iter == 0)|| (!enable_buffer))
        if(o_iter == HLS_O_ITER_MONITOR)
        {
          // values read from memory
          //// copy ptr_data buffer to output
          //   for this case cpi and cpo have the same value 
          //unsigned long rdclp_bytes_to_copy = sizeof(struct pixel_in_t);
          unsigned long rdclp_bytes_to_copy = sizeof(read_block_t);
          for (int i = 0; i < dbg_elements_per_iter_data_in[o_iter]; i++) {
            struct pixel_in_t px_loop;
            px_loop = dbg_loop_stream_data_in[o_iter].read();
            memcpy((void*)&(dbg_loop_ptr_data_in[i + (o_iter*read_channel_offset)]), (void*)&(px_loop), rdclp_bytes_to_copy);
          }
        }

       
        // values after input_buffer state
        if(o_iter == HLS_O_ITER_MONITOR)
        {
          //// copy ptr_data_input buffer to output
          //unsigned long rdclp_bytes_to_copy = sizeof(struct pixel_in_t);
          unsigned long rdclp_bytes_to_copy = sizeof(read_block_t);
          for (int i = 0; i < dbg_elements_per_iter_data_input_buffer[o_iter]; i++) {
            struct pixel_in_t px_loop;
            px_loop = dbg_loop_stream_data_input_buffer[o_iter].read();
            memcpy((void*)&(dbg_loop_ptr_data_input_buffer[i]), (void*)&(px_loop), rdclp_bytes_to_copy);
          }
        }

        
        // values after padding stage
        if(o_iter == HLS_O_ITER_MONITOR)
        {
           unsigned long bytes_to_copy = sizeof(read_block_t);
           int elements_to_copy =  I_ITER * (H + 2) * (W + 2); // value from padding.cp
           //int elements_to_copy =  dbg_elements_per_iter_data_dc_pad_out[o_iter];
           for (int i = 0; i < elements_to_copy; i++) {
             struct pixel_in_t px_loop;
             px_loop = dbg_loop_stream_data_dc_pad_out[o_iter].read();
             memcpy((void*)&dbg_loop_ptr_data_dc_pad_out[i], (void*)&(px_loop), bytes_to_copy);
           }
        }


        // values after cvt stage
        if(o_iter == HLS_O_ITER_MONITOR)
        {
          unsigned long bytes_to_copy = sizeof(frame_t); // 9 pixels
          // Despite that the main loop in cvt.cpp runs for I_ITER * (H + 2) * (W + 2), it writes less data to output buffer
          int elements_to_copy = I_ITER * (H) * (W); // value from cvt.cpp
          //int elements_to_copy = dbg_elements_per_iter_data_dc_cvt_out[o_iter];

          for (int i = 0; i < elements_to_copy; i++) {
             struct frame_t fr_loop;
             fr_loop = dbg_loop_stream_data_dc_cvt_out[o_iter].read();
             memcpy((void*)&dbg_loop_ptr_data_dc_cvt_out[i], (void*)&(fr_loop), bytes_to_copy);
           }
        }
        */


        /*
        // values after mul stage
        if(o_iter == HLS_O_ITER_MONITOR)
        {
          unsigned long bytes_to_copy = sizeof(write_block_t);
          int elements_to_copy = I_ITER * H * W; // value from mul.cpp
          //int elements_to_copy = dbg_elements_per_iter_data_dc_mul_out[o_iter];
          for (int i = 0; i < elements_to_copy; i++) {
            struct pixel_out_t px_loop;
            px_loop = dbg_loop_stream_data_dc_mul_out[o_iter].read();
            memcpy((void*)&(dbg_loop_ptr_data_dc_mul_out[i]), (void*)&(px_loop), bytes_to_copy);
          }
        }

        // values after add stage
        // this is the same output as for the direct convolution
        if(o_iter == HLS_O_ITER_MONITOR)
        {
          //// copy ptr_data buffer to output
          //   for this case cpi and cpo have the same value 
          //unsigned long rdclp_bytes_to_copy = sizeof(struct pixel_out_t);
          unsigned long rdclp_bytes_to_copy = sizeof(write_block_t);
          //int elements_to_copy = dbg_elements_per_iter_data_dc_add_out[o_iter];
          for (int i = 0; i < write_pixels; i++) {
            struct pixel_out_t px_loop;
            px_loop = dbg_loop_stream_data_directconv_out[o_iter].read();
            memcpy((void*)&(dbg_loop_ptr_data_directconv_out[i]), (void*)&(px_loop), rdclp_bytes_to_copy);
          }
        }
        */
        
#endif
  
  
    } // end o_iter

    // #ifdef DEBUG_VERBOSE
    // printf("kernel finishes\n");
    // #endif

#ifdef HLS_DEBUG
    *my_flt_bias = dbg_flt_bias_sum[HLS_O_ITER_MONITOR];
    *my_flt_krnl = dbg_flt_krnl_sum[HLS_O_ITER_MONITOR];
    *my_flt_din  = dbg_flt_din_sum[HLS_O_ITER_MONITOR];

//    *my_ret   = (int)dbg_elements_per_iter_data_in[0];
//    *my_ret_2 = (int)dbg_elements_per_iter_data_in[1];
//    *my_ret_3 = (int)dbg_elements_per_iter_data_in[2];
//    *my_ret_4 = (int)dbg_elements_per_iter_data_in[3];

//    *my_ret   = (int)dbg_elements_per_iter_kernel[0];
//    *my_ret_2 = (int)dbg_elements_per_iter_kernel[1];
//    *my_ret_3 = (int)dbg_elements_per_iter_kernel[2];
//    *my_ret_4 = (int)dbg_elements_per_iter_kernel[3];

//    *my_ret   = (int)dbg_elements_per_iter_bias[0];
//    *my_ret_2 = (int)dbg_elements_per_iter_bias[1];
//    *my_ret_3 = (int)dbg_elements_per_iter_bias[2];
//    *my_ret_4 = (int)dbg_elements_per_iter_bias[3];

//    *my_ret   = (int)dbg_elements_per_iter_data_input_buffer[0];
//    *my_ret_2 = (int)dbg_elements_per_iter_data_input_buffer[1];
//    *my_ret_3 = (int)dbg_elements_per_iter_data_input_buffer[2];
//    *my_ret_4 = (int)dbg_elements_per_iter_data_input_buffer[3];

//    *my_ret   = (int)dbg_elements_per_iter_data_dc_pad_out[0];
//    *my_ret_2 = (int)dbg_elements_per_iter_data_dc_pad_out[1];
//    *my_ret_3 = (int)dbg_elements_per_iter_data_dc_pad_out[2];
//    *my_ret_4 = (int)dbg_elements_per_iter_data_dc_pad_out[3];


//    *my_ret   = (int)dbg_elements_per_iter_data_dc_cvt_out[0];
//    *my_ret_2 = (int)dbg_elements_per_iter_data_dc_cvt_out[1];
//    *my_ret_3 = (int)dbg_elements_per_iter_data_dc_cvt_out[2];
//    *my_ret_4 = (int)dbg_elements_per_iter_data_dc_cvt_out[3];

//    *my_ret   = (int)dbg_elements_per_iter_data_dc_mul_out[0];
//    *my_ret_2 = (int)dbg_elements_per_iter_data_dc_mul_out[1];
//    *my_ret_3 = (int)dbg_elements_per_iter_data_dc_mul_out[2];
//    *my_ret_4 = (int)dbg_elements_per_iter_data_dc_mul_out[3];

//    *my_ret   = (int)dbg_elements_per_iter_data_dc_add_out[0];
//    *my_ret_2 = (int)dbg_elements_per_iter_data_dc_add_out[1];
//    *my_ret_3 = (int)dbg_elements_per_iter_data_dc_add_out[2];
//    *my_ret_4 = (int)dbg_elements_per_iter_data_dc_add_out[3];

//    *my_ret   = (int)dbg_elements_per_iter_data_dc_direcconv_out[0];
//    *my_ret_2 = (int)dbg_elements_per_iter_data_dc_direcconv_out[1];
//    *my_ret_3 = (int)dbg_elements_per_iter_data_dc_direcconv_out[2];
//    *my_ret_4 = (int)dbg_elements_per_iter_data_dc_direcconv_out[3];


        *my_ret   = dbg_elements_per_iter_data_in[HLS_O_ITER_MONITOR];
        *my_ret_2 = dbg_elements_per_iter_data_input_buffer[HLS_O_ITER_MONITOR];
        *my_ret_3 = dbg_elements_per_iter_data_dc_pad_out[HLS_O_ITER_MONITOR];
        *my_ret_4 = dbg_elements_per_iter_data_dc_cvt_out[HLS_O_ITER_MONITOR];

        // the correct one is the complete, but it would require to send the number of reads back to the host app to know how many data read from memory
        //if(enable_read)  or  if ( (o_iter == 0)|| (!enable_buffer))
        {
          // values read from memory
          //// copy ptr_data buffer to output
          //   for this case cpi and cpo have the same value 
          //unsigned long rdclp_bytes_to_copy = sizeof(struct pixel_in_t);
          unsigned long rdclp_bytes_to_copy = sizeof(read_block_t);
          for (int i = 0; i < dbg_elements_per_iter_data_in[HLS_O_ITER_MONITOR]; i++) {
            struct pixel_in_t px_loop;
            px_loop = dbg_loop_stream_data_in[HLS_O_ITER_MONITOR].read();
            memcpy((void*)&(dbg_loop_ptr_data_in[i]), (void*)&(px_loop), rdclp_bytes_to_copy);
          }
        }

        // values after input_buffer state
        {
          //// copy ptr_data_input buffer to output
          //unsigned long rdclp_bytes_to_copy = sizeof(struct pixel_in_t);
          unsigned long rdclp_bytes_to_copy = sizeof(read_block_t);
          for (int i = 0; i < dbg_elements_per_iter_data_input_buffer[HLS_O_ITER_MONITOR]; i++) {
            struct pixel_in_t px_loop;
            px_loop = dbg_loop_stream_data_input_buffer[HLS_O_ITER_MONITOR].read();
            memcpy((void*)&(dbg_loop_ptr_data_input_buffer[i]), (void*)&(px_loop), rdclp_bytes_to_copy);
          }
        }



#endif

  } // end of lib_conv2D 




}


