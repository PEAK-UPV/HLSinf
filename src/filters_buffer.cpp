/*
* HLSinf accelerator
* Version: 2.0
* copyright (c) 2023, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: July 2023
* Author: GAP Research Group (UPV), contact: drodagu@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

// ---------------------------------------------------------------------------------------
// kernel_buffer. Stores the filters in a kernels and feeds the mul module with the
// corresponding filters
//
// Arguments:
//   I_ITER	             : Number of iterations of the input channels (I/CPI)
//   in                  : Input buffer. From read_kernel function
//   out                 : Output stream. To mul module
//

void kernel_buffer(int num_data_frames, int I_ITER, int O_ITER,  hls::stream<w_st> &in, hls::stream<w_st> &out) {
	#ifdef DEBUG_FILTERS_BUFFER
	std::cout << "FILTERS_BUFFER: starts" << std::endl;
	#endif

	w_st k_in;
	w_st k_out;
	uint num_k = I_ITER * O_ITER;
	uint num_iter = I_ITER * O_ITER * num_data_frames;
	uint k_offset;

	// Buffer declaration
	static w_st buffer[FILTERS_BUFFER_SIZE];

	// TODO: Optimize
	//#pragma HLS AGGREGATE variable=buffer

	#ifdef ALVEO_U200
	#pragma HLS BIND_STORAGE variable=buffer type=ram_t2p impl=uram
	#endif
	// TODO: Optimize for Kintex

	filters_buffer_store_loop:
	for (uint w = 0; w < num_k; w++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=(IMAX/CPI)*(OMAX/CPO)
		#pragma HLS PIPELINE II=1

		k_in = in.read();
		buffer[w] = k_in;
	}

	filters_buffer_load_loop:
	for (uint it = 0; it < num_iter; it++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE*(I_REFERENCE/CPI)
		#pragma HLS PIPELINE II=1
		k_offset = it % num_k;
		k_out = buffer[k_offset];
		out << k_out;
	}

	#ifdef DEBUG_FILTERS_BUFFER
	std::cout << "FILTERS_BUFFER: ends" << std::endl;
	#endif
}

// ---------------------------------------------------------------------------------------
// bias_buffer. Stores the filters in a kernels and feeds the mul module with the
// corresponding filters
//
// Arguments:
//   num_data_frames	 : Number of data frames
//   O_ITER	             : Number of iterations of the output channels (O/CPO)
//   in                  : Input stream. From read_kernel module
//   out                 : Output stream. To mul module
//

void bias_buffer(int num_data_frames, int O_ITER, hls::stream<b_st> &in, hls::stream<b_st> &out) {
	#ifdef DEBUG_BIAS_BUFFER
	std::cout << "BIAS_BUFFER: starts" << std::endl;
	#endif

	b_st b_in;
	b_st b_out;
	uint b_offset;

	// Buffer declaration
	static b_st buffer[(OMAX/CPO)];

	#pragma HLS AGGREGATE variable=buffer

	#ifdef ALVEO_U200
	#pragma HLS BIND_STORAGE variable=buffer type=ram_t2p impl=uram
	#endif
	// TODO: Optimize for Kintex

	uint num_iter = O_ITER * num_data_frames;

	bias_buffer_store_loop:
	for (uint b = 0; b < O_ITER; b++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=O_REFERENCE/CPO
		#pragma HLS PIPELINE II=1

		b_in = in.read();
		buffer[b] = b_in;
	}

	bias_buffer_load_loop:
	for (uint it = 0; it < num_iter; it++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE*(I_REFERENCE/CPI)
		#pragma HLS PIPELINE II=1
		b_offset = it % O_ITER;
		b_out = buffer[b_offset];
		out << b_out;
	}

	#ifdef DEBUG_FILTERS_BUFFER
	std::cout << "BIAS_BUFFER: ends" << std::endl;
	#endif
}
