/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

#include <hls_stream.h>

//#include "add_data.cpp"

void set_write_enables(int enable_write[CPO], int o_channel, int O) {
  set_write_enables_loop:
  for (int o = 0; o <CPO; o++) {
    DO_PRAGMA(HLS loop_tripcount min=1 max=CPO)
    #pragma HLS UNROLL
    enable_write[o] = (o_channel + o) < O;
  }
}

void set_reading_channel_offsets(int offset_read_data_channel_i[CPI], int offset_read_data_channel, int channel_offset) {
 set_reading_channel_offsets_loop:
 for(int i=0; i<CPI; i++){
   DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
   #pragma HLS UNROLL
   offset_read_data_channel_i[i] = (offset_read_data_channel + i * channel_offset) % READ_BLOCK_SIZE;
 }
}


void set_channel_write_blocks(int num_channel_write_blocks[CPO], int H, int W) {
  set_channel_write_blocks_loop:
  for(int i=0; i<CPO; i++) {
    #pragma HLS UNROLL
	num_channel_write_blocks[i] = ((H * W) + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
  }
}

#ifdef USE_UPSIZE
template <class in_st, class out_st> void upsize(int enable, int H, int W, hls::stream<in_st> &in, hls::stream<out_st> &out) {

  out_st buff[WMAX];

  #ifdef DEBUG_UPSIZE
  printf("UPSIZE: begin\n");
  #endif

  if (enable) {
    for (int h = 0; h<H; h++) {
      DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE)
      for (int w = 0; w<W; w++) {
        DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
        in_st px;
        px = in.read();
        for (int cpo=0; cpo<CPO; cpo++) {
          DO_PRAGMA(HLS UNROLL)
          buff[w].pixel[cpo] = px.pixel[cpo];
        }
      }
      for (int r = 0; r < 2; r++) {
        for (int w = 0; w<2*W; w++) {
          DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
          out << buff[w/2];
          #ifdef DEBUG_UPSIZE
          #ifdef DEBUG_VERBOSE
          printf("UPSIZE: r %d w %d -> ", r, w);
          for (int c=0; c<CPO; c++) printf(" %f ", buff[w/2].pixel[c]);
          printf("\n");
          #endif
          #endif
        }
      }
    }
  } else {
    for (int h = 0; h<H; h++) {
      DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE)
      for (int w = 0; w<W; w++) {
        DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
        in_st px;
        out_st px_out;
        px = in.read();
        for (int cpo=0; cpo<CPO; cpo++) {
          DO_PRAGMA(HLS UNROLL)
          px_out.pixel[cpo] = px.pixel[cpo];
        }
	      out << px_out;
      }
    }
  }

  #ifdef DEBUG_UPSIZE
  printf("UPSIZE: end\n");
  #endif

}
#endif

extern "C" {
void k_conv2D(read_block_t *ptr_data, 
            #ifdef USE_ADD
            write_block_t *ptr_data_add, 
            #endif
            int H, int W, int HO, int WO, int rows, int PT, int PB, int PL, int PR, int SH, int SW, int I, int O, int I_ITER, int o_iter_first, int o_iter_last, 
            int enable_relu, int enable_stm, float relu_factor, 
            #ifdef USE_BATCH_NORM 
            int enable_batch_norm,
            #endif
            #ifdef DIRECT_CONV
            w_t *ptr_kernel, 
            #endif
            #ifdef DWS_CONV
  					w_t *ptr_dw_kernel, w_pw_t *ptr_pw_kernel,
            #endif
            b_st *ptr_bias, 
            #ifdef USE_BATCH_NORM
            bnp_st *b_ptr, 
            #endif
            write_block_t *ptr_out, int read_offset, int write_offset, int enable_maxpooling, int enable_avgpooling,
						int enable_clipping, int enable_shift, 
            #ifdef USE_ADD
            int enable_add, 
            #endif
            int min_clip, int max_clip, int dir_shift, int pos_shift, int enable_upsize,
            int write_to_weight_buffer, int read_from_weight_buffer, int first_row_weight_buffer) {

	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_data         depth=DATA_IN_PORT_DEPTH    offset=slave bundle=gmem)
  #ifdef DIRECT_CONV    
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_kernel       depth=KERNEL_PORT_DEPTH     offset=slave bundle=gmem1)
  #endif
  #ifdef DWS_CONV
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_dw_kernel    depth=DW_KERNEL_PORT_DEPTH  offset=slave bundle=gmem1)
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_pw_kernel    depth=PW_KERNEL_PORT_DEPTH  offset=slave bundle=gmem4)
  #endif
  #ifdef USE_ADD
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_data_add     depth=DATA_IN_PORT_DEPTH    offset=slave bundle=gmem5)
  #endif
  #ifdef USE_BATCH_NORM
	DO_PRAGMA(HLS INTERFACE m_axi port=b_ptr            depth=BATCH_MORM_VAL_DEPTH  offset=slave bundle=gmem6)
  #endif
  DO_PRAGMA(HLS INTERFACE m_axi port=ptr_bias         depth=BIAS_PORT_DEPTH       offset=slave bundle=gmem2)
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_out          depth=DATA_OUT_PORT_DEPTH   offset=slave bundle=gmem3)

	DO_PRAGMA(HLS shared variable=I)
	DO_PRAGMA(HLS shared variable=O)
	DO_PRAGMA(HLS shared variable=I_ITER)
	DO_PRAGMA(HLS shared variable=o_iter_first)
	DO_PRAGMA(HLS shared variable=o_iter_last)
	DO_PRAGMA(HLS shared variable=enable_maxpooling)
	DO_PRAGMA(HLS shared variable=enable_avgpooling)
	DO_PRAGMA(HLS shared variable=rows)

	DO_PRAGMA(HLS shared variable=H)
	DO_PRAGMA(HLS shared variable=W)
	DO_PRAGMA(HLS stable variable=I)
	DO_PRAGMA(HLS stable variable=O)
	DO_PRAGMA(HLS stable variable=I_ITER)
	DO_PRAGMA(HLS stable variable=o_iter_first)
	DO_PRAGMA(HLS stable variable=o_iter_last)
	DO_PRAGMA(HLS stable variable=H)
	DO_PRAGMA(HLS stable variable=W)
	DO_PRAGMA(HLS stable variable=enable_maxpooling)
	DO_PRAGMA(HLS stable variable=enable_avgpooling)
	DO_PRAGMA(HLS stable variable=rows)
	DO_PRAGMA(HLS stable variable=SH)
	DO_PRAGMA(HLS stable variable=SW)
	DO_PRAGMA(HLS stable variable=relu_factor)


  #ifdef DEBUG_VERBOSE
  printf("kernel starts...\n");
  #endif

  int O_ITER = o_iter_last - o_iter_first + 1;

  // output convolution geometry
  int HO_conv                  = (rows + PT + PB - KH + SH) / SH; // HO = ceil(H + padding - (KH-1) / SH)
  int WO_conv                  = (W + PL + PR - KW + SW) / SW; // WO = ceil(H + padding - (KW-1) / SW)
  int num_output_conv_pixels   = HO_conv * WO_conv;

  #ifdef USE_POOLING
  int enable_pooling           = enable_maxpooling | enable_avgpooling;
  int HI_pooling               = HO_conv;
  int WI_pooling               = WO_conv;
  int write_pixels             = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv;
  int read_pixels_add          = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv;
  int num_bn_pixels            = read_pixels_add;
  int num_add_pixels           = read_pixels_add;
  if (enable_upsize) write_pixels = write_pixels * 4;


  int write_rows               = enable_pooling ? HO_conv / 2 : HO_conv;
  int write_cols               = enable_pooling ? WO_conv / 2 : WO_conv;
  
  #else
  int write_pixels             = HO_conv * WO_conv;
  if (enable_upsize) write_pixels = write_pixels * 4;
  int read_pixels_add          = HO_conv * WO_conv;
  int num_bn_pixels            = read_pixels_add;
  int num_add_pixels           = read_pixels_add;

  int write_rows               = HO_conv;
  int write_cols               = WO_conv;
  #endif
 
  o_iter_loop:
  for (int o_iter = 0; o_iter<O_ITER; o_iter++) {
	  DO_PRAGMA(HLS loop_tripcount min=1 max=O_REFERENCE/CPO)
	  #pragma HLS dataflow

	  int o_channel = (o_iter + o_iter_first) * CPO; //<< LOG2_CPO;  // current output channel (first one in this iteration)

    // input and output streams
    static hls::stream<din_st>   out_read_data;
    static hls::stream<dout_st>   out_read_data_add;
    static hls::stream<din_st>   out_read_data_1;
    static hls::stream<din_st>   out_read_data_2;

    DO_PRAGMA(HLS STREAM variable=out_read_data     depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_read_data_add depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_read_data_1   depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_read_data_2   depth=STREAMS_DEPTH)

	  // DIRECT CONV
    #ifdef DIRECT_CONV
    static hls::stream<w_st>     out_read_kernel;
    DO_PRAGMA(HLS STREAM variable=out_read_kernel depth=STREAMS_DEPTH)
    static hls::stream<w_st>     out_read_kernel_2;
    DO_PRAGMA(HLS STREAM variable=out_read_kernel_2 depth=STREAMS_DEPTH)
    #endif
    #ifdef DWS_CONV
    static hls::stream<w_dw_st>     out_read_kernel_dw;
    static hls::stream<w_pw_st>     out_read_kernel_pw;
    DO_PRAGMA(HLS STREAM variable=out_read_kernel_dw depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_read_kernel_pw depth=STREAMS_DEPTH)
    #endif    

    static hls::stream<b_st>  out_read_bias;
    static hls::stream<conv_st>  out_conv;
    DO_PRAGMA(HLS STREAM variable=out_read_bias depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_conv depth=STREAMS_DEPTH)

    // RELU, CLIPPING, and SHIFT support
    static hls::stream<relu_st>  out_relu;
    DO_PRAGMA(HLS STREAM variable=out_relu depth=STREAMS_DEPTH)

    //STM support
    static hls::stream<stm_st>  out_stm;
    DO_PRAGMA(HLS STREAM variable=out_stm depth=STREAMS_DEPTH)

    // MAXPOOLING, AVGPOOLING
    #ifdef USE_POOLING
    static hls::stream<pool_st>  out_pooling;
    DO_PRAGMA(HLS STREAM variable=out_pooling depth=STREAMS_DEPTH)
    #endif

    //ADD support
    static hls::stream<dout_st>  out_add;
    DO_PRAGMA(HLS STREAM variable=out_add depth=STREAMS_DEPTH)

    #ifdef USE_UPSIZE
    static hls::stream<dout_st> out_write;
    DO_PRAGMA(HLS STREAM variable=out_write depth=STREAMS_DEPTH)
    #endif

    // BATCH NORM support
	  #ifdef USE_BATCH_NORM
    static hls::stream<dout_st>  out_batch_norm;
    DO_PRAGMA(HLS STREAM variable=out_batch_norm depth=STREAMS_DEPTH)

    static hls::stream<bnp_st>  out_read_batch_norm;
	  DO_PRAGMA(HLS STREAM variable=out_read_batch_norm depth=STREAMS_DEPTH)
	  #endif

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // variables to manage the input buffer
    int num_extra_rows           = 0;
    int read_pixels              = W * (rows + num_extra_rows);
    int read_pixels_total        = read_pixels * I_ITER;
    int offset_data_in_group_cpi = H * W;
    int offset_data_out_group_cpo = HO * WO;
    int offset_read_add_group_cpo = HO * WO;
    if (enable_upsize) offset_data_out_group_cpo = offset_data_out_group_cpo * 4;

    //printf("I_ITER %d W %d rows %d num_extra_rows %d read_pixels %d read_pixels_total %d\n", I_ITER, W, rows, num_extra_rows, read_pixels, read_pixels_total);
    int enable_buffer            = (read_pixels_total <= INPUT_BUFFER_SIZE);
    int write_to_input_buffer    = enable_buffer && (o_iter == 0) && (O_ITER>1);
    int read_from_input_buffer   = enable_buffer && (o_iter != 0);
    int enable_read              = (o_iter == 0) || !enable_buffer;
    //printf("enable_buffer %d write_to_input_buffer %d read_from_input_buffer %d enable_read %d\n", enable_buffer, write_to_input_buffer, read_from_input_buffer, enable_read);

    int enable_read_kernel = !read_from_weight_buffer;

    // variables
    int enable_write[CPO];
    int offset_read_data_channel_i[CPI];
    int num_channel_write_blocks[CPO];
    int offset_read_data_channel = read_offset;
    int channel_size             = H * W;

    int read_channel_offset      = (W * H);

    int o_iter_read_add_offset   = write_offset + (offset_read_add_group_cpo * (o_iter + o_iter_first));
    int o_iter_write_offset      = write_offset + (offset_data_out_group_cpo * (o_iter + o_iter_first));

    int read_channel_blocks      = (read_pixels + READ_BLOCK_SIZE - 1) / READ_BLOCK_SIZE;
    int offset_bias              = o_iter + o_iter_first;
    int offset_kernel            = (o_iter + o_iter_first) * ((I + CPI - 1) / CPI) * CPI * CPO * 9;
    #pragma HLS array_partition variable=enable_write dim=0 complete
    DO_PRAGMA(HLS ARRAY_PARTITION variable=offset_read_data_channel_i dim=0 complete)
    DO_PRAGMA(HLS ARRAY_PARTITION variable=offset_write_data_channel_i dim=0 complete)
    DO_PRAGMA(HLS ARRAY_PARTITION variable=block_offset_write_data_channel_i dim=0 complete)
	  DO_PRAGMA(HLS ARRAY_PARTITION variable=num_channel_write_blocks dim=0 complete)

    // we compute the enable_write signals
    set_write_enables(enable_write, o_channel, O);

    // channel offsets for reading
    set_reading_channel_offsets(offset_read_data_channel_i, offset_read_data_channel, read_channel_offset);

    // channel write blocks
    set_channel_write_blocks(num_channel_write_blocks, H, W);

    // offset weight buffer
    int offset_weight_buffer = first_row_weight_buffer + (o_iter * I_ITER);

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // read bias and kernel (filters)
    read_bias(offset_bias, ptr_bias, out_read_bias);
    #ifdef DIRECT_CONV
    read_kernel(enable_read_kernel, I_ITER, offset_kernel, ptr_kernel, out_read_kernel);
    weight_buffer(I_ITER, write_to_weight_buffer, read_from_weight_buffer, offset_weight_buffer, out_read_kernel, out_read_kernel_2);
    #endif
    #ifdef DWS_CONV
    dws_read_dw_kernel(I_ITER, o_iter, ptr_dw_kernel, out_read_kernel_dw);  // o_iter as argument to load all kernels in the first iteration (o_iter==0)
    dws_read_pw_kernel(I_ITER, O, o_iter + o_iter_first, ptr_pw_kernel, out_read_kernel_pw); // o_iter+o_iter_ifrst sent to let the module compute the offset to read the kernels
    #endif

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Read data and batch normalization vectors
    read_data_channels_gihwcpi(read_pixels, offset_read_data_channel, I_ITER, offset_data_in_group_cpi, ptr_data, out_read_data, enable_read);
    input_buffer(read_pixels_total, write_to_input_buffer, read_from_input_buffer, out_read_data, out_read_data_1);
    #ifdef USE_ADD
    read_input_add_gihwcpi(read_pixels_add, o_iter_read_add_offset, ptr_data_add, out_read_data_add, enable_add);
    #endif
    #ifdef USE_BATCH_NORM
    read_batch_norm(offset_bias, b_ptr, out_read_batch_norm);
    #endif

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // direct convolution
    #ifdef DIRECT_CONV
    direct_conv(rows, W, PT, PB, PL, PR, SH, SW, num_output_conv_pixels, I_ITER, out_read_data_1, out_read_kernel_2, out_read_bias, out_conv);
    #endif
    #ifdef DWS_CONV
    dws_conv(rows, W, PT, PB, PL, PR, SH, SW, num_output_conv_pixels, I_ITER, out_read_data_1, out_read_kernel_dw, out_read_kernel_pw, out_read_bias, out_conv);
    #endif


    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Relu, Clipping, shift, pooling, batch normalization, add, and upsize
    relu(enable_relu, enable_clipping, enable_shift, relu_factor, min_clip, max_clip, dir_shift, pos_shift, num_output_conv_pixels, out_conv, out_relu);
    stm(enable_stm, num_output_conv_pixels, out_relu, out_stm); 
    pooling(HI_pooling, WI_pooling, enable_maxpooling, enable_avgpooling, out_stm, out_pooling);
    #ifdef USE_BATCH_NORM
      batch_norm(enable_batch_norm, num_bn_pixels, out_pooling, out_read_batch_norm, out_batch_norm);
      #ifdef USE_ADD
        add_data<dout_st, dout_st, dout_st>(enable_add, num_add_pixels, out_read_data_add, out_batch_norm, out_add); 
        upsize<dout_st, dout_st>(enable_upsize, write_rows, write_cols, out_add, out_write);
      #else
        upsize(enable_upsize, write_rows, write_cols, out_pooling, out_write);
        upsize<pool_st, dout_st>(enable_upsize, write_rows, write_cols, out_pooling, out_write);
      #endif
    #else
      #ifdef USE_ADD
        add_data<dout_st, pool_st, dout_st>(enable_add, num_add_pixels, out_read_data_add, out_pooling, out_add); 
        upsize<dout_st, dout_st>(enable_upsize, write_rows, write_cols, out_add, out_write);
      #else
        upsize<pool_st, dout_st>(enable_upsize, write_rows, write_cols, out_pooling, out_write);
      #endif
    #endif
    
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // write to memory
	  write_data_channels_gihwcpi(write_pixels, o_iter_write_offset, ptr_out, out_write);

 } // end o_iter

 #ifdef DEBUG_VERBOSE
 printf("kernel finishes\n");
 #endif

}

} // extern "C"
