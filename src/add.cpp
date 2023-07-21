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

void add (int num_pixels, int I_ITER, int O_ITER, hls::stream<conv_mul_st> &in, hls::stream <b_st> &b_in, hls::stream<dout_st> &out) {
    #ifdef DEBUG_ADD
    std::cout << "add: start" << std::endl;
    #endif

    b_st bias;
    #pragma HLS ARRAY_PARTITION variable=bias type=complete dim=0

    // number of iterations by CPO channels
    int num_iterations = num_pixels * O_ITER;

    //conv_st o_channels;
    dout_st o_channels;
    #pragma HLS AGGREGATE variable=o_channels

    // All input data have effect into output add
    add_i_iter_loop:
    for (int it = 0; it < num_iterations; it++) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE
        #pragma HLS LOOP_FLATTEN off

    	dout_st data_out;

	    // We receive bias in packs of CPO
	    bias = b_in.read();

	    #ifdef DEBUG_ADD
	    #ifdef DEBUG_VERBOSE
	    for (int b = 0; b < CPO; b++) {
		    std::cout << "Bias[" << b << "] = " << float(bias.pixel[b]) << std::endl;
	    }
	    std::cout << "add: bias received" << std::endl;
	    #endif
	    #endif


	    add_load_data_it_loop:
	    for (int i_iter = 0; i_iter < I_ITER; i_iter++) {
		    #pragma HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI
	    	conv_mul_st px;
	    	b_st data_in;
	    	dout_st data_out;
		    px = in.read();

		    if (i_iter == 0) data_in = bias; else data_in = *(b_st *)&o_channels;

		    add_load_data_cpo_loop:
		    for (int cpo = 0; cpo < CPO; cpo++) {
		        #pragma HLS UNROLL
			    data_out.pixel[cpo] = data_in.pixel[cpo] + px.pixel[cpo];
		    }

		    o_channels = data_out;
	    }
	    out << data_out;
    }
}
