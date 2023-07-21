/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

// ---------------------------------------------------------------------------------------
// read_bias. Reading bias from memory and sending to conv module
//
// Arguments:
//   b_ptr               : pointer to bias
//   offset_bias         : offset to bias
//   b_out               : output stream
//
// All the bias are read and sent through the out stream
//

void read_bias(int O_ITER, int offset_bias, b_st *b_ptr, hls::stream<b_st> &out) {

  #ifdef DEBUG_READ_BIAS
  printf("READ_BIAS: start\n");
  #endif

  b_st bias;
  #pragma HLS ARRAY_PARTITION variable=bias complete dim=0

  for (uint iter = 0; iter < O_ITER; iter++) {
	  bias = b_ptr[offset_bias+iter];
	  out << bias;

	  #ifdef DEBUG_READ_BIAS
	  printf("READ_BIAS: offset_bias = %d\n", offset_bias);
	  printf("READ_BIAS: bias = ");
	  for (int c=0; c<CPO; c++) printf(" %f ", float(bias.pixel[c]));
	  printf("\n");
	  #endif
  }

  #ifdef DEBUG_READ_BIAS
  printf("READ_BIAS: end\n");
  #endif
}


// ---------------------------------------------------------------------------------------
// read_batch_norm. Reading batch normalization values from memory and sending to batch
// 					normalization module
//
// Arguments:
//   b_ptr               : pointer to batch normalization values
//   offset_batchnorm    : offset to batch normalization values
//   b_out               : output stream
//
// All the bias are read and sent through the out stream
//
void read_batch_norm(int enable_batch_norm, int offset_batchnorm, bnp_st *b_ptr, hls::stream<bnp_st> &out) {
  #ifdef DEBUG_READ_BATCH_NORM
  printf("DEBUG_READ_BATCH_NORM: start\n");
  #endif

  if (!enable_batch_norm) return;

  bnp_st batch_norm;
  #pragma HLS ARRAY_PARTITION variable=batch_norm complete dim=0

  batch_norm = b_ptr[offset_batchnorm];
  out << batch_norm;

  #ifdef DEBUG_READ_BATCH_NORM
  printf("DEBUG_READ_BATCH_NORM: value = ");
  for (int c=0; c<CPO*4; c++) {
	  printf(" %f ", float(batch_norm.values[c]));
  }
  printf("\n");
  #endif

  #ifdef DEBUG_READ_BATCH_NORM
  printf("DEBUG_READ_BATCH_NORM: end\n");
  #endif
}

// ---------------------------------------------------------------------------------------
// read_kernel. Reads kernels and sends them through the stream
//
// Arguments:
//   I_ITER              : Number of input iterations (I / CPI)
//   O_ITER				 : Number of output iterations (O / CPO)
//   k_ptr               : pointer to kernels
//   offset_kernel       : offset to kernels
//   k_out               : output stream
//
// kernels are stored in memory with the format GO x GI x CPO x CPI x KH x KW
// This storage formats lets the module to read memory sequentially and send all the
// kernels in the same order they are read through the output stream.
// kernels are sent in frame structures (3x3 grid)
//

void read_kernel(int I_ITER, int O_ITER, int offset_kernel, w_t *k_ptr, hls::stream<w_st> &k_out) {
  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL: start\n");
  #endif

  w_st k;
  int cnt = 0;
  #pragma HLS array_partition variable=k complete dim=0
  uint iters = I_ITER * O_ITER;

  for (int i=0; i<iters; i++) {
	  DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)
	  DO_PRAGMA(HLS LOOP_FLATTEN OFF)

	  // TODO: Could be optimize
	  for (int cpo=0; cpo < CPO; cpo++) {
		  for (int cpi=0; cpi < CPI; cpi++) {
			  for (int p=0; p<9; p++) {
				  #pragma HLS pipeline II=1
				  k.pixel[cpo][cpi][p] = k_ptr[offset_kernel+cnt];
				  cnt = cnt + 1;
			  }
		  }
	  }
	  k_out << k;
	  #ifdef DEBUG_READ_KERNEL
	  for (int cpo=0; cpo<CPO; cpo++) {
		  for (int cpi=0; cpi<CPI; cpi++) {
		      printf("READ_KERNEL: Kernel read for cpi=%d cpo=%d : ", cpi, cpo);
		      for (int p=0;p<9; p++) printf(" %6.4f ", float(k.pixel[cpo][cpi][p]));
		      printf("\n");
		  }
	  }
	  #endif
  }

  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL: end\n");
  #endif
}

// ---------------------------------------------------------------------------------------
// read_input. Reads all input data assuming HxWxI data format
// Read pixels are sent out through the output stream
//
// Arguments:
//   num_pixels          : Number of pixels to read in total (each pixel is I wide)
//   offset              : offsets within input data to read
//   I_ITER				 : Number of input iterations (I / CPI)
//   ptr                 : pointer to input data
//   out                 : output stream
//   enable              : enable for the read operation
//

void read_input(int num_pixels, int offset, int I_ITER, read_block_t *ptr, hls::stream<din_st> &out) {
	#ifdef DEBUG_READ_DATA
    std::cout << "READ_DATA: starts (HxWxI) format" << std::endl;
    std::cout << "num_pixels : " << num_pixels << std::endl;
    std::cout << "offset 	   : " << offset << std::endl;
    std::cout << "ptr 	   : " << ptr << std::endl;
    #endif

    uint total_num_pixels = num_pixels * I_ITER;

    read_data_channels_loop_i_iter:
    for (int i = 0; i < total_num_pixels; i++) {
    	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=H_REFERENCE*W_REFERENCE)
		#pragma HLS PIPELINE II=1

		din_st px;
    	px = ptr[offset+i];
    	out << px;

		#ifdef DEBUG_READ_DATA
		#ifdef VERBOSE
		std::cout << "data read: " << i << "   ";
		for (int cpi = 0; cpi < CPI; cpi++)
			std::cout << px.pixel[cpi] << " ";
		std::cout << std::endl;
    	#endif
		#endif
    }

	#ifdef DEBUG_READ_DATA
    std::cout << "READ_DATA: ends (HWI format)" << std::endl;
    #endif
}

// ---------------------------------------------------------------------------------------
// read_input_add_gihwcpi. Reads all input data assuming GIxHxWxCPO input data format
// Read pixels are sent out through the output stream
//
// Arguments:
//   num_pixels          : Number of pixels to read in total (each pixel is CPI wide)
//   offset              : offsets within input data to read (offset aligned to CPI wide pixels)
//   ptr                 : pointer to input data
//   out                 : output stream
//   enable              : enable for the read operation
//

void read_input_add_gihwcpi(int num_pixels, int offset, write_block_t *ptr, hls::stream<dout_st> &out, int enable) {

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: starts (gihwcpi format)\n");
  printf("  num_pixels : %d\n", num_pixels);
  printf("  offset : %d\n", offset);
  #endif

  if (!enable) return;

  read_data_channels_loop_pixels:
  for (int i = 0; i < num_pixels; i++) {
    DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max = I_REFERENCE * H_REFERENCE * W_REFERENCE / CPI)
    DO_PRAGMA(HLS pipeline)

    dout_st px;
  	px = ptr[offset+i];
    out << px;
    #ifdef DEBUG_READ_DATA
    #ifdef DEBUG_VERBOSE
    printf("data read : %d : ", i);
    for (int x=0; x<CPO; x++) printf("%f ", float(px.pixel[x]));
    printf("\n");
    #endif
    #endif
  }

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: ends (gihwcpi format)\n");
  #endif

}
