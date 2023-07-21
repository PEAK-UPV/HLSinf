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

#ifdef USE_UPSIZE
void upsize(int enable, int H, int W, hls::stream<dout_st> &in, hls::stream<dout_st> &out) {
  dout_st buff[WMAX];

  #ifdef DEBUG_UPSIZE
  printf("UPSIZE: begin\n");
  #endif

  if (enable) {
    for (int h = 0; h<H; h++) {
      DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE)
      for (int w = 0; w<W; w++) {
        DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
        dout_st px;
        px = in.read();
        buff[w] = px;
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
        dout_st px;
        px = in.read();
	      out << px;
      }
    }
  }

  #ifdef DEBUG_UPSIZE
  printf("UPSIZE: end\n");
  #endif

}
#endif


extern "C" {
void k_conv2D(read_block_t *ptr_data, write_block_t *ptr_data_add, int H, int W, int HO, int WO, int rows, int PT, int PB, int PL, int PR,
		    int SH, int SW, int I, int O, int I_ITER, int o_iter_first, int o_iter_last, int O_ITER,
            int enable_relu, int enable_stm, float relu_factor,int enable_batch_norm,
            w_t *ptr_kernel,
            b_st *ptr_bias, bnp_st *b_ptr, write_block_t *ptr_out, int read_offset, int write_offset, int enable_maxpooling, int enable_avgpooling,
			int enable_clipping, int enable_shift, int enable_add, int min_clip, int max_clip, int dir_shift, int pos_shift, int enable_upsize) {

	// Increase max_read_burst_length to 256 for better performance when accessing memory
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_kernel    depth=KERNEL_PORT_DEPTH    max_widen_bitwidth=512	num_read_outstanding=16   max_read_burst_length=64  num_write_outstanding=1 max_write_burst_length=2	offset=slave bundle=gmem1)
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_data      depth=DATA_IN_PORT_DEPTH   max_widen_bitwidth=512	num_read_outstanding=16   max_read_burst_length=64	 num_write_outstanding=1 max_write_burst_length=2	offset=slave bundle=gmem)
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_data_add  depth=DATA_IN_PORT_DEPTH   max_widen_bitwidth=512	num_read_outstanding=16   max_read_burst_length=64  num_write_outstanding=1 max_write_burst_length=2	offset=slave bundle=gmem5)
	DO_PRAGMA(HLS INTERFACE m_axi port=b_ptr         depth=BATCH_MORM_VAL_DEPTH max_widen_bitwidth=512	num_read_outstanding=16   max_read_burst_length=64  num_write_outstanding=1 max_write_burst_length=2	offset=slave bundle=gmem6)
    DO_PRAGMA(HLS INTERFACE m_axi port=ptr_bias      depth=BIAS_PORT_DEPTH      max_widen_bitwidth=512	num_read_outstanding=16   max_read_burst_length=64  num_write_outstanding=1 max_write_burst_length=2	offset=slave bundle=gmem2)
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_out       depth=DATA_OUT_PORT_DEPTH  max_widen_bitwidth=512	num_write_outstanding=16  max_write_burst_length=64  num_read_outstanding=1  max_read_burst_length=2	offset=slave bundle=gmem3)

	#pragma HLS INTERFACE mode=s_axilite port=ptr_data bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=ptr_data_add bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=H bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=W bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=HO bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=WO bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=rows bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=PT bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=PB bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=PL bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=PR bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=SH bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=SW bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=I bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=O bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=I_ITER bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=o_iter_first bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=o_iter_last bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=O_ITER bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=enable_relu bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=enable_stm bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=relu_factor bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=enable_batch_norm bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=ptr_kernel bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=ptr_bias bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=b_ptr bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=ptr_out bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=read_offset bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=write_offset bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=enable_maxpooling bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=enable_avgpooling bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=enable_clipping bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=enable_shift bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=enable_add bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=min_clip bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=max_clip bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=dir_shift bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=pos_shift bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=enable_upsize bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=return bundle=control

	DO_PRAGMA(HLS stable variable=I)
	DO_PRAGMA(HLS stable variable=O)
	DO_PRAGMA(HLS stable variable=I_ITER)
	DO_PRAGMA(HLS stable variable=O_ITER)
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

	// output convolution geometry
	int HO_conv                  = (rows + PT + PB - KH + SH) / SH; 	// HO = ceil(H + padding - (KH-1) / SH)
	int WO_conv                  = (W + PL + PR - KW + SW) / SW; 		// WO = ceil(H + padding - (KW-1) / SW)

	int num_output_conv_pixels   = HO_conv * WO_conv;

	#ifdef USE_POOLING
	int enable_pooling           = enable_maxpooling | enable_avgpooling;
	int HI_pooling               = HO_conv;
	int WI_pooling               = WO_conv;
	int write_pixels             = (enable_pooling ? (enable_upsize ? ((HO_conv / 2) * (WO_conv / 2)) *4 : (HO_conv / 2) * (WO_conv / 2)) : (enable_upsize ? (HO_conv * WO_conv) * 4 : (HO_conv * WO_conv)));
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

	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// input and output streams
	static hls::stream<din_st>   out_read_data("read_data");
	//static hls::stream<dout_st>   out_read_data_add("read_data_add");

	DO_PRAGMA(HLS STREAM variable=out_read_data     depth=STREAMS_DEPTH)
	//DO_PRAGMA(HLS STREAM variable=out_read_data_add depth=STREAMS_DEPTH)
	static hls::stream<w_st>     out_read_kernel("read_kernel");
	DO_PRAGMA(HLS STREAM variable=out_read_kernel depth=STREAMS_DEPTH)
	// DIRECT CONV
	static hls::stream<b_st>	out_bias_buffer("bias_buffer");
	#pragma HLS STREAM variable=out_bias_buffer depth=STREAMS_DEPTH

	static hls::stream<w_st>	out_filters_buffer("kernel_buffer");
	DO_PRAGMA(HLS STREAM variable=out_filters_buffer depth=STREAMS_DEPTH)

	static hls::stream<b_st>  out_read_bias("read_bias");
	static hls::stream<conv_st>  out_conv("conv");
	DO_PRAGMA(HLS STREAM variable=out_read_bias depth=STREAMS_DEPTH)
	DO_PRAGMA(HLS STREAM variable=out_conv depth=STREAMS_DEPTH)

	// RELU, CLIPPING, and SHIFT support
	static hls::stream<relu_st>  out_relu("relu");
	DO_PRAGMA(HLS STREAM variable=out_relu depth=STREAMS_DEPTH)

	//STM support
	static hls::stream<stm_st>  out_stm("stm");
	DO_PRAGMA(HLS STREAM variable=out_stm depth=STREAMS_DEPTH)

	// MAXPOOLING, AVGPOOLING
	#ifdef USE_POOLING
	static hls::stream<pool_st>  out_pooling("pooling");
	DO_PRAGMA(HLS STREAM variable=out_pooling depth=STREAMS_DEPTH)
	#endif

	//ADD support
	static hls::stream<dout_st>  out_add("add");
	DO_PRAGMA(HLS STREAM variable=out_add depth=STREAMS_DEPTH)

	#ifdef USE_UPSIZE
	static hls::stream<dout_st> out_write("write");
	DO_PRAGMA(HLS STREAM variable=out_write depth=STREAMS_DEPTH)
	#endif

	// BATCH NORM support
	#ifdef USE_BATCH_NORM
	static hls::stream<dout_st>  out_batch_norm("batch_norm");
	DO_PRAGMA(HLS STREAM variable=out_batch_norm depth=STREAMS_DEPTH)

	static hls::stream<bnp_st>  out_read_batch_norm("read_batch_norm");
	DO_PRAGMA(HLS STREAM variable=out_read_batch_norm depth=STREAMS_DEPTH)
	#endif

	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// variables
	int read_pixels              = W * rows;
	int read_pixels_total        = read_pixels * I_ITER;
	int offset_data_out_group_cpo = HO * WO;
	int offset_read_add_group_cpo = HO * WO;
	int offset_read_data_channel = read_offset;

	int o_iter_read_add_offset   = write_offset + (offset_read_add_group_cpo * o_iter_first);
	int o_iter_write_offset      = write_offset + (offset_data_out_group_cpo * o_iter_first);

	int offset_bias              = o_iter_first;
	int offset_kernel            = o_iter_first * ((I + CPI - 1) / CPI) * CPI * CPO * 9;


	#pragma HLS dataflow
	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// read bias and kernels (filters)
	read_bias(O_ITER, offset_bias, ptr_bias, out_read_bias);
	bias_buffer(num_output_conv_pixels, O_ITER, out_read_bias, out_bias_buffer);
	read_kernel(I_ITER, O_ITER, offset_kernel, ptr_kernel, out_read_kernel);
	kernel_buffer(num_output_conv_pixels, I_ITER, O_ITER, out_read_kernel, out_filters_buffer);

	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Read data and batch normalization vectors
	read_input(read_pixels, offset_read_data_channel, I_ITER, ptr_data, out_read_data);
	//read_input_add_gihwcpi(read_pixels_add, o_iter_read_add_offset, ptr_data_add, out_read_data_add, enable_add);
	//read_batch_norm(enable_batch_norm, offset_bias, b_ptr, out_read_batch_norm);

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// direct convolution
	direct_conv(rows, W, PT, PB, PL, PR, SH, SW, num_output_conv_pixels, I_ITER, O_ITER, out_read_data, out_filters_buffer, out_bias_buffer, out_write);

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Relu, Clipping, shift, pooling, batch normalization, add, and upsize
	//    relu(enable_relu, enable_clipping, enable_shift, relu_factor, min_clip, max_clip, dir_shift, pos_shift, num_output_conv_pixels, out_conv, out_relu);
	//    stm(enable_stm, num_output_conv_pixels, out_relu, out_stm);
	//    pooling(HI_pooling, WI_pooling, enable_maxpooling, enable_avgpooling, out_stm, out_pooling);
	//    batch_norm(enable_batch_norm, num_bn_pixels, out_pooling, out_read_batch_norm, out_batch_norm);
	//    add_data(enable_add, num_add_pixels, out_read_data_add, out_batch_norm, out_add);
	//    upsize(enable_upsize, write_rows, write_cols, out_add, out_write);

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// write to memory
	write_data_channels_gihwcpi(write_pixels, O_ITER, o_iter_write_offset, ptr_out, out_write);

    //#ifdef DEBUG_VERBOSE
	//printf("kernel finishes\n");
 	//#endif

}
} // extern "C"
