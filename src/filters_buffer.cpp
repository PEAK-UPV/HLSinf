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

	// Buffer declaration
	static w_st buffer[FILTERS_BUFFER_SIZE];

	#pragma HLS ARRAY_RESHAPE variable=buffer type=complete dim=4

	#ifdef ALVEO_U200
	#pragma HLS BIND_STORAGE variable=buffer type=ram_t2p impl=uram
	#endif

	uint num_k = I_ITER * O_ITER;
	uint num_iters = I_ITER * O_ITER * num_data_frames;

	filters_buffer_loop:
	for (uint it = 0, buf_read = 0; it < num_iters; it++) {
		// Read filters from stream
		if (it < num_k) {
			w_st k_in;
			k_in = in.read();
			buffer[it] = k_in;

			// Send filter through the stream
			out << k_in;
		} else {
			w_st k_out;
			k_out = buffer[buf_read];
			out << k_out;

			/*
			* This is more resource efficient than having -> uint k_offset = it % num_k
			*/
			if (buf_read == num_k-1) {
				buf_read = 0;
			} else {
				buf_read++;
			}
		}
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

	// Buffer declaration
	static b_st buffer[(OMAX/CPO)];

	#pragma HLS AGGREGATE variable=buffer

	#ifdef ALVEO_U200
	#pragma HLS BIND_STORAGE variable=buffer type=ram_t2p impl=uram
	#endif

	uint num_iter = O_ITER * num_data_frames;

	bias_buffer_load_loop:
	for (uint it = 0, b_offset = 0; it < num_iter; it++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE*(I_REFERENCE/CPI)
		#pragma HLS PIPELINE II=1
		if (it < O_ITER) {
			b_in = in.read();
			buffer[it] = b_in;
			out << b_in;
		} else {
			b_out = buffer[b_offset];
			out << b_out;
			/*
			 * This is more resource efficient than having -> b_offset = it % O_ITER
			*/
			if (b_offset == O_ITER-1) {
				b_offset = 0;
			} else {
				b_offset++;
			}
		}
	}

	#ifdef DEBUG_FILTERS_BUFFER
	std::cout << "BIAS_BUFFER: ends" << std::endl;
	#endif
}
