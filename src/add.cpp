/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

// -------------------------------------------------------------------------------
// add: This function performs the addition of all subpixels for the same channel.
// It adds also the corresponding bias.
//
// Arguments:
//   num_pixels     : Height of input channel
//   I_ITER			: Number of input iterations (I / CPI)
//   O_ITER			: Number of output iterations (O /CPO)
//   in    			: input streams data
//   b_in  			: input stream bias
//   out   			: output stream
//

void add (int num_pixels, int I_ITER, int O_ITER, hls::stream<conv_mul_st> &in, hls::stream <b_st> &b_in, hls::stream<conv_st> &out) {
    #ifdef DEBUG_ADD
    std::cout << "add: start" << std::endl;
    #endif

    // number of iterations by CPO channels
    uint num_iterations = num_pixels * I_ITER * O_ITER;

    static conv_st buff_o_channels[(OMAX/CPO)];

    // All input data have effect into output add
    add_load_data_it_loop:
    for (uint it = 0, i_iter = 0, o_iter = 0; it < num_iterations; it++) {
		#pragma HLS DEPENDENCE variable=buff_o_channels type=inter dependent=false

		conv_st data_out;
		conv_mul_st px;
		b_st data_in;

		data_in = (i_iter == 0) ? b_in.read() : *(b_st *)&buff_o_channels[o_iter];


		#ifdef DEBUG_ADD
		#ifdef DEBUG_VERBOSE
		for (int b = 0; b < CPO; b++) {
			std::cout << "Bias[" << b << "] = " << float(bias.pixel[b]) << std::endl;
		}
		std::cout << "add: bias received" << std::endl;
		#endif
		#endif

		// Read pixels
		px = in.read();

		add_load_data_cpo_loop:
		for (int cpo = 0; cpo < CPO; cpo++) {
			#pragma HLS UNROLL
			data_out.pixel[cpo] = data_in.pixel[cpo] + px.pixel[cpo];
		}

		buff_o_channels[o_iter] = data_out;

		if (i_iter == (I_ITER-1)) {
			out << data_out;
			#ifdef DEBUG_ADD
			#ifdef DEBUG_VERBOSE
			printf("add: \n");
			for (int cpo = 0; cpo < CPO; cpo++) {
				printf("data_out[%d]: %6.2f ", cpo, float(data_out.pixel[cpo]));
			}
			std::cout << std::endl;
			#endif
			#endif
		}

		if (o_iter == (O_ITER-1)) {
			o_iter = 0;
			if (i_iter == (I_ITER-1)) {
				i_iter = 0;
			} else {
				i_iter++;
			}
		} else {
			o_iter++;
		}
    }

	#ifdef DEBUG_ADD
	std::cout << "add: end" << std::endl;
    #endif
}
