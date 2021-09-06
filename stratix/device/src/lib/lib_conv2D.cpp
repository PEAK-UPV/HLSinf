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
#include "HLS/stdio.h"

#include "lib_conv2D_commons.h"
#include "lib_hdrs_priv.h"

#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

#include <iostream>
#include <string.h>


#define HLS_DBG_SB_CAPACITY_W 1024 // stream buffer capacity in words

extern "C" {


// streams for read-from write to memroy and pass data between tasks
ihc::stream<pixel_in_t>   out_read_data;
ihc::stream<pixel_in_t>   out_read_data_from_input_buffer;
ihc::stream<kernel_t>     out_read_kernel;
ihc::stream<pixel_out_t>  out_read_bias;
ihc::stream<pixel_out_t>  out_conv;
ihc::stream<pixel_in_t>   str_pad_cvt;  // padding->cvt
ihc::stream<frame_t>      str_cvt_mul;  // cvt->mul
ihc::stream<pixel_out_t>  str_mul_add;  // mul->add
ihc::stream<pixel_out_t>  out_relu;
ihc::stream<pixel_out_t>  out_pooling;
ihc::stream<frame_pool_t> stream_pool;



/*
#define TASK_STREAM_BUFFER_CAPACITY_WORDS 32
// streams for read-from write to memroy and pass data between tasks
ihc::stream<pixel_in_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS> >  out_read_data;
ihc::stream<pixel_in_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>>   out_read_data_from_input_buffer;
ihc::stream<kernel_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>>     out_read_kernel;
ihc::stream<pixel_out_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>>  out_read_bias;
ihc::stream<pixel_out_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>>  out_conv;
ihc::stream<pixel_in_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>>   str_pad_cvt;  // padding->cvt
ihc::stream<frame_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>>      str_cvt_mul;  // cvt->mul
ihc::stream<pixel_out_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>>  str_mul_add;  // mul->add
ihc::stream<pixel_out_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>>  out_relu;
ihc::stream<pixel_out_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>>  out_pooling;
ihc::stream<frame_pool_t, ihc::buffer<TASK_STREAM_BUFFER_CAPACITY_WORDS>> stream_pool;


*/


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------


#ifdef HLS_DEBUG
//ihc::stream<pixel_in_t>    dbg_loop_stream_data_in;
ihc::stream<pixel_in_t,  ihc::buffer<HLS_DBG_SB_CAPACITY_W>>   dbg_loop_stream_data_input_buffer;
// direct convolution streams
ihc::stream<pixel_in_t,  ihc::buffer<HLS_DBG_SB_CAPACITY_W>>   dbg_loop_stream_data_dc_pad_out;  // pixels write to output stream of padding task
ihc::stream<frame_t,     ihc::buffer<HLS_DBG_SB_CAPACITY_W>>   dbg_loop_stream_data_dc_cvt_out;  // frames write to output stream of cvt task
ihc::stream<hls_cvt_sbs_control_t,   ihc::buffer<HLS_DBG_SB_CAPACITY_W>>   dbg_loop_stream_data_dc_cvt_sbs_control_out;  // frames write to output stream of cvt task
ihc::stream<frame_t,                 ihc::buffer<HLS_DBG_SB_CAPACITY_W>>   dbg_loop_stream_data_dc_cvt_sbs_frame_out;  // frames write to output stream of cvt task

ihc::stream<pixel_out_t, ihc::buffer<HLS_DBG_SB_CAPACITY_W>>   dbg_loop_stream_data_dc_mul_out;  // pixels write to output stream of mul task
//ihc::stream<pixel_out_t, ihc::buffer<HLS_DBG_SB_CAPACITY_W>    dbg_loop_stream_dc_add_out; directconv_out
ihc::stream<pixel_out_t, ihc::buffer<HLS_DBG_SB_CAPACITY_W>>   dbg_loop_stream_data_directconv_out;  // pixels write to output stream of dicrect convolution (add) task


unsigned long dbg_loop_stream_data_dc_pad_out_counter;
unsigned long dbg_loop_stream_data_dc_cvt_out_counter;
unsigned long dbg_loop_stream_data_dc_cvt_sbs_out_counter;
unsigned long dbg_loop_stream_data_dc_mul_out_counter;
unsigned long dbg_loop_stream_data_dc_add_out_counter;

unsigned long int dbg_elements_per_iter_data_in[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_kernel[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_bias[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_input_buffer[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_pad_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_cvt_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_cvt_sbs_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_mul_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_add_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_dc_direcconv_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_relu_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_pool_cvt_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_pool_cvt_sbs_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_pool_pooling_out[MAX_O_ITERS];
unsigned long int dbg_elements_per_iter_data_out[MAX_O_ITERS];
#endif



//// -------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void set_write_enables(int enable_write[CPO], int o_channel, int O) {
//  set_write_enables_loop:
//  #pragma unroll
//  for (int o = 0; o <CPO; o++) {
//    //DO_PRAGMA(HLS loop_tripcount min=1 max=CPO)
//    enable_write[o] = (o_channel + o) < O;
//  }
//}
//
//void set_reading_channel_offsets(int offset_read_data_channel_i[CPI], int offset_read_data_channel, int channel_offset) {
// set_reading_channel_offsets_loop:
//  #pragma unroll
//  for(int i=0; i<CPI; i++){
//    //DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
//    offset_read_data_channel_i[i] = (offset_read_data_channel + i * channel_offset) % READ_BLOCK_SIZE;
//  }
//}
//
//
//void set_channel_write_blocks(int num_channel_write_blocks[CPO], int H, int W) {
//  set_channel_write_blocks_loop:
//  #pragma unroll
//  for(int i=0; i<CPO; i++) {
//    num_channel_write_blocks[i] = ((H * W) + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
//  }
//}


// include sources forced
#include "join_split.cpp"
#include "add.cpp"
#include "cvt.cpp"
#include "mul.cpp"
#include "padding.cpp"
#include "pooling.cpp"
#include "relu.cpp"



#ifdef HLS_DEBUG
  data_type my_flt_bias = 0.0;
  data_type my_flt_krnl = 0.0;
  data_type my_flt_din  = 0.0;
  data_type my_flt_dout = 0.0;
#endif

  
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
      unsigned long                     my_val, // currently unused
      OCL_ADDRSP_GLOBAL unsigned long  *dbg_ptr_ul,
      OCL_ADDRSP_GLOBAL data_type      *dbg_ptr_dt,
      OCL_ADDRSP_GLOBAL read_block_t   *dbg_loop_ptr_data_in,
      OCL_ADDRSP_GLOBAL read_block_t   *dbg_loop_ptr_data_input_buffer,
      OCL_ADDRSP_GLOBAL read_block_t   *dbg_loop_ptr_data_dc_pad_out,
      OCL_ADDRSP_GLOBAL frame_t        *dbg_loop_ptr_data_dc_cvt_out,
      OCL_ADDRSP_GLOBAL hls_cvt_sbs_control_t   *dbg_loop_ptr_data_dc_cvt_sbs_control_out,
      OCL_ADDRSP_GLOBAL frame_t                 *dbg_loop_ptr_data_dc_cvt_sbs_frame_out,
      OCL_ADDRSP_GLOBAL write_block_t  *dbg_loop_ptr_data_dc_mul_out,
      OCL_ADDRSP_GLOBAL write_block_t  *dbg_loop_ptr_data_directconv_out
#endif
  ) {

 
  #ifdef HLS_DEBUG
 
  dbg_loop_stream_data_dc_pad_out_counter = (unsigned long)0;
  dbg_loop_stream_data_dc_cvt_out_counter = (unsigned long)0;
  dbg_loop_stream_data_dc_cvt_sbs_out_counter = (unsigned long) 0;
  dbg_loop_stream_data_dc_mul_out_counter = (unsigned long)0;
  dbg_loop_stream_data_dc_add_out_counter = (unsigned long)0;

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
    dbg_elements_per_iter_data_dc_cvt_sbs_out[ind] = 0;
    dbg_elements_per_iter_data_dc_mul_out[ind] = 0;
    dbg_elements_per_iter_data_dc_add_out[ind] = 0;
    dbg_elements_per_iter_data_dc_direcconv_out[ind] = 0;

    dbg_elements_per_iter_data_relu_out[ind] = 0;
    dbg_elements_per_iter_data_pool_cvt_out[ind] = 0;
    dbg_elements_per_iter_data_pool_pooling_out[ind] = 0;

    dbg_elements_per_iter_data_out[ind] = 0;

  }
  #endif

  // Here begins the real k_conv2D kernel
  
  // I did not find yet the way to include pritf/std::cout msgs for crossgen/libtool
  //   So let's disable all print msgs
  //  printf("kernel starts...\n");
  

  int O_ITER = o_iter_last - o_iter_first + 1;

  o_iter_loop:
  for (int o_iter = 0; o_iter<O_ITER; o_iter++) {
	  int o_channel = (o_iter + o_iter_first) << LOG2_CPO;  // current output channel (first one in this iteration)

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
    //int enable_write[CPO];
    //int offset_read_data_channel_i[CPI];
    //int num_channel_write_blocks[CPO];
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

  
    unsigned long int  hld_dbg_buffer_input_counter;


    // we first need to launch the tasks
    ihc::launch<input_buffer>( o_iter, read_pixels_total, write_to_input_buffer, read_from_input_buffer);
    
    //direct_conv(o_iter, rows, W, I_ITER, enable_upper_padding, enable_lower_padding, out_read_data_1, out_read_kernel, out_read_bias, out_conv);
    //void direct_conv(o_iter, H,  W, I_ITER, enable_upper_padding, enable_lower_padding, in, k_in, b_in, out) {
    ihc::launch<padding>(o_iter, H, W, I_ITER, enable_upper_padding, enable_lower_padding);   // padding
    ihc::launch<cvt>(o_iter, H, W, I_ITER);
    ihc::launch<mul>(o_iter, H, W, I_ITER);
    ihc::launch<add>(o_iter, H, W, I_ITER);

    ihc::launch<relu>(enable_relu, enable_clipping, enable_shift, min_clip, max_clip, dir_shift, pos_shift, rows, W);
    // Pooling: avgpooling or maxpooling
    //ihc::launch<pooling>(H, W, enable_maxpooling, enable_avgpooling);
    int WO = enable_pooling ? ((W - KW_POOLING)/SW_POOLING + 1) : W;
    int HO = enable_pooling ? ((H - KH_POOLING)/SH_POOLING + 1) : H;
    ihc::launch<pool_cvt>(H, W, enable_pooling);
    ihc::launch<pool_pooling>(HO, WO, enable_maxpooling, enable_avgpooling);


     
    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // read bias
    {
      pixel_out_t bias;
      memcpy((void*)&bias, (void*)&ptr_bias[offset_bias], sizeof(pixel_out_t));
      for (int i = 0; i < CPO; i++) {
        bias.pixel[i] = ptr_bias[offset_bias].pixel[i];
      }

      
      #ifdef HLS_DEBUG
      for (int lopi = 0; lopi < CPO; lopi++) {
        dbg_flt_bias_sum[o_iter] = dbg_flt_bias_sum[o_iter] + bias.pixel[lopi];
      }
      
      dbg_elements_per_iter_bias[o_iter] = dbg_elements_per_iter_bias[o_iter] + 1;
      #endif
         
      //#pragma HLS ARRAY_PARTITION variable=bias complete dim=0          
      out_read_bias.write(bias);
    }

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // read kernel (different version based on the type of convolution)
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
              k.pixel[cpo][cpi][p] = ptr_kernel[offset_kernel+cnt];
              //memcpy((void*)&k.pixel[cpo][cpi][p], (void*)&ptr_kernel[offset_kernel+cnt], sizeof(data_type));

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
        #ifdef laguasa //#ifdef HLS_DEBUG
        dbg_elements_per_iter_kernel[o_iter] = dbg_elements_per_iter_kernel[o_iter] + 1;
        #endif
      }
    }


    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Read and preparation of pixels, depending on the data format we select the modules to use


    // JM10 debug, la forma correcta és activar el if(enable read)
    //if (enable_read)
    { 
      unsigned long rdclp_bytes_to_copy = sizeof(pixel_in_t);

      read_data_channels_loop_pixels:
      for (int i = 0; i < read_pixels_total; i++) {
        pixel_in_t px_din;

        //DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max = I_REFERENCE * H_REFERENCE * W_REFERENCE / CPI)
        //DO_PRAGMA(HLS pipeline)
        //memcpy((void*)&px_din, (void*)&(ptr_data[offset_read_data_channel+i]), rdclp_bytes_to_copy);
        for (int j = 0; j < CPI; j++) {
          px_din.pixel[j] = ptr_data[offset_read_data_channel+i].pixel[j];
        }
        out_read_data.write(px_din);

        #ifdef HLS_DEBUG
        for (int lopi = 0; lopi < CPI; lopi++) {
          dbg_flt_din_sum[o_iter] = dbg_flt_din_sum[o_iter] + px_din.pixel[lopi];
        }
        dbg_elements_per_iter_data_in[o_iter] = dbg_elements_per_iter_data_in[o_iter] + 1;
        
        if (o_iter == HLS_O_ITER_MONITOR)
        {
          //dbg_loop_stream_data_in.write(px_din);    
          for (int j = 0; j < CPI; j++) {
            dbg_loop_ptr_data_in[i].pixel[j]= px_din.pixel[j];
          }
        }
        
        #endif
        
      }
    }
    //}


    //ihc::collect<input_buffer>();
    //ihc::collect<padding>();
    //ihc::collect<cvt>();
    //ihc::collect<mul>();
    //ihc::collect<add>();
    //ihc::collect<relu>();
    //ihc::collect<pool_cvt>();
    //ihc::collect<pool_pooling>();
    
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
        my_flt_dout = my_flt_dout + bx.pixel[0] + bx.pixel[1] + bx.pixel[2] + bx.pixel[3]; 
        #endif
        //memcpy((void*)&ptr_out[o_iter_write_offset+i], (void*)&bx, sizeof(struct pixel_out_t));
        //memcpy((void*)&(ptr_out[o_iter_write_offset+i]), (void*)&bx, sizeof(write_block_t));
        for(int j = 0; j < CPO; j++) {
          ptr_out[o_iter_write_offset+i].pixel[j] = bx.pixel[j];
        }
        dbg_elements_per_iter_data_out[o_iter] = dbg_elements_per_iter_data_out[o_iter] + 1;

      }
    //}

    dbg_elements_per_iter_data_input_buffer[o_iter] = ihc::collect<input_buffer>();
    dbg_elements_per_iter_data_dc_pad_out[o_iter]   = ihc::collect<padding>();
    dbg_elements_per_iter_data_dc_cvt_out[o_iter]     = ihc::collect<cvt>();
    //unsigned long cvt_ul                            = ihc::collect<cvt>();
    //dbg_elements_per_iter_data_dc_cvt_out[o_iter]     = ((0xFF00 & cvt_ul) >> 16) & (unsigned long)0x00FF;
    //dbg_elements_per_iter_data_dc_cvt_sbs_out[o_iter] = ( 0x00FF & cvt_ul)        & (unsigned long)0x00FF;
    dbg_elements_per_iter_data_dc_mul_out[o_iter]     = ihc::collect<mul>();
    dbg_elements_per_iter_data_dc_add_out[o_iter]     = ihc::collect<add>();
    dbg_elements_per_iter_data_relu_out[o_iter]       = ihc::collect<relu>();
    dbg_elements_per_iter_data_pool_cvt_out[o_iter]   = ihc::collect<pool_cvt>();
    dbg_elements_per_iter_data_pool_pooling_out[o_iter] = ihc::collect<pool_pooling>();

    dbg_elements_per_iter_data_dc_cvt_sbs_out[o_iter] = dbg_elements_per_iter_data_dc_pad_out[o_iter]; // will read the same number of pixels as the padding stage wrote to the channel


    


    //---------------------------------------------------------------------------------------- end of code for preparation and write



  
    } // end o_iter

    // #ifdef DEBUG_VERBOSE
    // printf("kernel finishes\n");
    // #endif

#ifdef laguasa //#ifdef HLS_DEBUG
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

#endif
#ifdef HLS_DEBUG

    dbg_ptr_ul[HLS_DBG_H_IND] = H;
    dbg_ptr_ul[HLS_DBG_W_IND] = W;
    dbg_ptr_ul[HLS_DBG_ROWS_IND] = rows;
    dbg_ptr_ul[HLS_DBG_I_INPUT_IND] = I;
    dbg_ptr_ul[HLS_DBG_O_OUTPUT_IND] = O;
    dbg_ptr_ul[HLS_DBG_I_ITER_IND] = I_ITER;
    dbg_ptr_ul[HLS_DBG_O_ITER_FIRST_IND] = o_iter_first;
    dbg_ptr_ul[HLS_DBG_O_ITER_LAST_IND] = o_iter_last;
    dbg_ptr_ul[HLS_DBG_ENABLE_RELU_IND] = enable_relu;
    dbg_ptr_ul[HLS_DBG_GLOBAL_OFFSET_IND] = global_offset;
    dbg_ptr_ul[HLS_DBG_ENABLE_UPPER_PADDING_IND] = enable_upper_padding;
    dbg_ptr_ul[HLS_DBG_ENABLE_LOWER_PADDING_IND] = enable_lower_padding;
    dbg_ptr_ul[HLS_DBG_ENABLE_MAX_POOLING_IND] = enable_maxpooling;
    dbg_ptr_ul[HLS_DBG_ENABLE_AVG_POOLING_IND] = enable_avgpooling;
    dbg_ptr_ul[HLS_DBG_ENABLE_CLIPPING_IND] = enable_clipping;
    dbg_ptr_ul[HLS_DBG_ENABLE_SHIFT_IND] = enable_shift;
    dbg_ptr_ul[HLS_DBG_MIN_CLIP_IND] = min_clip;
    dbg_ptr_ul[HLS_DBG_MAX_CLIP_IND] = max_clip;
    dbg_ptr_ul[HLS_DBG_DIR_SHIFT_IND] = dir_shift;
    dbg_ptr_ul[HLS_DBG_POS_SHIFT_IND] = pos_shift;

    dbg_ptr_ul[HLS_DBG_VALUES_write_to_out_read_data_stream_IND] = dbg_elements_per_iter_data_in[HLS_O_ITER_MONITOR];
    dbg_ptr_ul[HLS_DBG_VALUES_write_to_out_read_data_from_input_buffer_stream_IND] = dbg_elements_per_iter_data_input_buffer[HLS_O_ITER_MONITOR];
    //dbg_ptr_ul[HLS_DBG_VALUES_write_to_out_read_data_from_input_buffer_stream_IND] = hld_dbg_buffer_input_counter;
    
    dbg_ptr_ul[HLS_DBG_VALUES_write_to_pad_cvt_stream_IND]  = dbg_elements_per_iter_data_dc_pad_out[HLS_O_ITER_MONITOR];
    dbg_ptr_ul[HLS_DBG_VALUES_write_to_cvt_mul_stream_IND]  = dbg_elements_per_iter_data_dc_cvt_out[HLS_O_ITER_MONITOR];
    dbg_ptr_ul[HLS_DBG_VALUES_write_to_mul_add_stream_IND]  = dbg_elements_per_iter_data_dc_mul_out[HLS_O_ITER_MONITOR];
    dbg_ptr_ul[HLS_DBG_VALUES_write_to_out_conv_stream_IND] = dbg_elements_per_iter_data_dc_add_out[HLS_O_ITER_MONITOR];
    //dbg_ptr_ul[] = dbg_elements_per_iter_data_dc_direcconv_out[HLS_O_ITER_MONITOR];
    //dbg_ptr_ul[] = dbg_elements_per_iter_data_out[HLS_O_ITER_MONITOR];
    dbg_ptr_ul[HLS_DBG_VALUES_write_to_out_relu_stream_IND] = dbg_elements_per_iter_data_relu_out[HLS_O_ITER_MONITOR];
    dbg_ptr_ul[HLS_DBG_VALUES_write_to_stream_pool_stream_IND] = dbg_elements_per_iter_data_pool_cvt_out[HLS_O_ITER_MONITOR];
    dbg_ptr_ul[HLS_DBG_VALUES_write_to_out_pooling_stream_IND] = dbg_elements_per_iter_data_pool_pooling_out[HLS_O_ITER_MONITOR];
   
    dbg_ptr_ul[HLS_DBG_VALUES_write_to_data_out_IND] =dbg_elements_per_iter_data_out[HLS_O_ITER_MONITOR];   

    dbg_ptr_ul[HLS_DBG_VALUES_write_to_out_conv_sbs_stream_IND] =  dbg_elements_per_iter_data_dc_cvt_sbs_out[HLS_O_ITER_MONITOR]; 

    my_flt_bias = dbg_flt_bias_sum[HLS_O_ITER_MONITOR];
    my_flt_krnl = dbg_flt_krnl_sum[HLS_O_ITER_MONITOR];
    my_flt_din  = dbg_flt_din_sum[HLS_O_ITER_MONITOR];

    dbg_ptr_dt[HLS_DBG_DT_bias_sum_IND]   = my_flt_bias;
    dbg_ptr_dt[HLS_DBG_DT_kernel_sum_IND] = my_flt_krnl;
    dbg_ptr_dt[HLS_DBG_DT_din_sum_IND]  = my_flt_din;
    dbg_ptr_dt[HLS_DBG_DT_dout_sum_IND] = my_flt_dout;
    
#endif

#ifdef laguasa //#ifdef HLS_DEBUG
//        // the correct one is the complete, but it would require to send the number of reads back to the host app to know how many data read from memory
//        //if(enable_read)  or  if ( (o_iter == 0)|| (!enable_buffer))
//        {
//          // values read from memory
//          //// copy ptr_data buffer to output
//          //   for this case cpi and cpo have the same value 
//          //unsigned long rdclp_bytes_to_copy = sizeof(struct pixel_in_t);
//          unsigned long rdclp_bytes_to_copy = sizeof(read_block_t);
//          for (int i = 0; i < dbg_elements_per_iter_data_in[HLS_O_ITER_MONITOR]; i++) {
//            pixel_in_t px_loop;
//            px_loop = dbg_loop_stream_data_in.read();
//            //memcpy((void*)&(dbg_loop_ptr_data_in[i]), (void*)&(px_loop), rdclp_bytes_to_copy);
//            for (int j = 0; j < CPI; j++) {
//              dbg_loop_ptr_data_in[i].pixel[j] = px_loop.pixel[j];
//            }
//          }
//        }
#endif
#ifdef HLS_DEBUG
        // values after input_buffer state
        {
          //// copy ptr_data_input buffer to output
          //unsigned long rdclp_bytes_to_copy = sizeof(struct pixel_in_t);
          unsigned long rdclp_bytes_to_copy = sizeof(read_block_t);
          for (int i = 0; i < dbg_elements_per_iter_data_input_buffer[HLS_O_ITER_MONITOR]; i++) {
            pixel_in_t px_loop;
            px_loop = dbg_loop_stream_data_input_buffer.read();
            //memcpy((void*)&(dbg_loop_ptr_data_input_buffer[i]), (void*)&(px_loop), rdclp_bytes_to_copy);
            for (int j = 0; j < CPI; j++) {
              dbg_loop_ptr_data_input_buffer[i].pixel[j] = px_loop.pixel[j];
            }
          }
        }

       // values after padding stage
        {
           unsigned long bytes_to_copy = sizeof(read_block_t);
           int elements_to_copy =  I_ITER * (H + 2) * (W + 2); // value from padding.cp
           //int elements_to_copy =  dbg_elements_per_iter_data_dc_pad_out[o_iter];
           for (int i = 0; i < elements_to_copy; i++) {
             pixel_in_t px_loop;
             px_loop = dbg_loop_stream_data_dc_pad_out.read();
             //memcpy((void*)&dbg_loop_ptr_data_dc_pad_out[i], (void*)&(px_loop), bytes_to_copy);
             for (int j = 0; j < CPI; j++) {
               dbg_loop_ptr_data_dc_pad_out[i].pixel[j] = px_loop.pixel[j];
             }
           }
        }


        // values after cvt stage
        {
          unsigned long bytes_to_copy = sizeof(frame_t); // 9 pixels
          // Despite that the main loop in cvt.cpp runs for I_ITER * (H + 2) * (W + 2), it writes less data to output buffer
          //int elements_to_copy = I_ITER * (H) * (W); // value from cvt.cpp
          int elements_to_copy = dbg_elements_per_iter_data_dc_cvt_out[HLS_O_ITER_MONITOR];
          //int elements_to_copy = dbg_elements_per_iter_data_dc_cvt_out[o_iter];

          for (int i = 0; i < elements_to_copy; i++) {
            frame_t fr_loop;
            fr_loop = dbg_loop_stream_data_dc_cvt_out.read();
            //memcpy((void*)&dbg_loop_ptr_data_dc_cvt_out[i], (void*)&(fr_loop), bytes_to_copy);
            for (int j = 0; j < 9; j++)
              for (int k = 0; k < CPI; k++) // pixel_in_t
                dbg_loop_ptr_data_dc_cvt_out[i].pixel[j].pixel[k] = fr_loop.pixel[j].pixel[k];

          }
        }

        //step by step frame debug
        {
          int elements_to_copy = dbg_elements_per_iter_data_dc_cvt_sbs_out[HLS_O_ITER_MONITOR];

          for (int i = 0; i < elements_to_copy; i++) {
            hls_cvt_sbs_control_t ctrl_loop;
            ctrl_loop = dbg_loop_stream_data_dc_cvt_sbs_control_out.read();

            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].iter              = ctrl_loop.iter;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].pin_row           = ctrl_loop.pin_row;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].pin_col           = ctrl_loop.pin_col;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].row0_buffer_write = ctrl_loop.row0_buffer_write;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].row1_buffer_write = ctrl_loop.row1_buffer_write;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].row2_buffer_write = ctrl_loop.row2_buffer_write;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].p                 = ctrl_loop.p;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].pin_col0          = ctrl_loop.pin_col0;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].pin_col1          = ctrl_loop.pin_col1;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].row0              = ctrl_loop.row0;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].row1              = ctrl_loop.row1;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].shift_frame       = ctrl_loop.shift_frame;
            dbg_loop_ptr_data_dc_cvt_sbs_control_out[i].send_frame        = ctrl_loop.send_frame;
          }

          for (int i = 0; i < elements_to_copy; i++) {
            frame_t fr_loop;
            fr_loop = dbg_loop_stream_data_dc_cvt_sbs_frame_out.read();
            for (int j = 0; j < 9; j++)
              for (int k = 0; k < CPI; k++) // pixel_in_t
                dbg_loop_ptr_data_dc_cvt_sbs_frame_out[i].pixel[j].pixel[k] = fr_loop.pixel[j].pixel[k];
          }
        }


       

        // values after mul stage
        {
          unsigned long bytes_to_copy = sizeof(write_block_t);
          int elements_to_copy = I_ITER * H * W; // value from mul.cpp
          //int elements_to_copy = dbg_elements_per_iter_data_dc_mul_out[o_iter];
          for (int i = 0; i < elements_to_copy; i++) {
            pixel_out_t px_loop;
            px_loop = dbg_loop_stream_data_dc_mul_out.read();
            //memcpy((void*)&(dbg_loop_ptr_data_dc_mul_out[i]), (void*)&(px_loop), bytes_to_copy);
            for (int j = 0; j < CPO; j++) {
              dbg_loop_ptr_data_dc_mul_out[i].pixel[j] = px_loop.pixel[j];
            }

          }
        }

        // values after add stage
        // this is the same output as for the direct convolution
        {
          //// copy ptr_data buffer to output
          //   for this case cpi and cpo have the same value 
          //unsigned long rdclp_bytes_to_copy = sizeof(struct pixel_out_t);
          unsigned long rdclp_bytes_to_copy = sizeof(write_block_t);
          //int elements_to_copy = dbg_elements_per_iter_data_dc_add_out[o_iter];
          for (int i = 0; i < dbg_elements_per_iter_data_out[HLS_O_ITER_MONITOR]; i++) {
            pixel_out_t px_loop;
            px_loop = dbg_loop_stream_data_directconv_out.read();
            //memcpy((void*)&(dbg_loop_ptr_data_directconv_out[i]), (void*)&(px_loop), rdclp_bytes_to_copy);
            for (int j = 0; j < CPO; j++) {
              dbg_loop_ptr_data_directconv_out[i].pixel[j] = px_loop.pixel[j];
            }
          }
        }

#endif

  } // end of lib_conv2D 




}


