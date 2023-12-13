/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

/**
 * Pooling layer 
 * 		
 * 		MaxPooling
 * 		AvgPooling
 *  
 * @file pooling.cpp
 * @author Raul Ferrando 
 */

#include "conv2D.h"

struct frame_pool_t {
	relu_st pixel[KW_POOLING * KH_POOLING];
};

struct pooling_st   {
	relu_st     pixel[(OMAX/CPO)];
};

/*
* Convert module for the pooling layer
* Only works for fixed size of (KW=2, KH=2) and (SW=2, SH=2).
* 
* The output data format is (KH, KW, CPI)
* 
* Parameters:
* 	H   	       : Height of the input image
* 	W		       : Width of the input image
* 	enable_pooling : Activates pooling
* 	in		       : Input stream (pixel_out_t)
*	out 	       : Output stream (frame_pool_t)
*
*	In case enable_pooling is not set, the module bypasses the input
*	to the output, sending the pixel on the first position of the output frame.
*/

static void pool_cvt(int H, int W, int O_ITER, int enable_pooling, hls::stream<relu_st> &in, hls::stream<frame_pool_t> &out) {

	frame_pool_t kernel;
	pooling_st  buffer0[WMAX];
	pooling_st  buffer1[WMAX]; // Quizas quitar

	uint          send_frame;
	uint          pin_col = 0;
	uint		  column = 1;

	uint          row0_write = 1;			// either 0 or 1 (we assume 2x2 kernel)
	uint		  row1_write = 0;
	uint		  row0_write_aux = 0;
	uint		  row1_write_aux = 0;

	uint          size_channel = H * W;
	uint          iterations = size_channel * O_ITER;

	uint 		  initial_interval =  (W * O_ITER) + (1 * O_ITER);
	uint		  ii_counter = 0;
	uint		  row_counter = 0;
	uint		  o_iter = 0;

	relu_st       pixel;

    #ifdef ALVEO_U200
	DO_PRAGMA(HLS bind_storage variable=buffer0 type=ram_2p impl=uram)
	DO_PRAGMA(HLS bind_storage variable=buffer1 type=ram_2p impl=uram)
    #endif
    #ifdef ALVEO_U280
    DO_PRAGMA(HLS bind_storage variable=buffer0 type=ram_2p impl=uram)
    DO_PRAGMA(HLS bind_storage variable=buffer1 type=ram_2p impl=uram)
    #endif

	#pragma HLS ARRAY_RESHAPE variable=buffer0 type=complete dim=3
	#pragma HLS ARRAY_RESHAPE variable=buffer1 type=complete dim=3

	#pragma HLS AGGREGATE variable=pixel
	#pragma HLS ARRAY_PARTITION variable=kernel type=complete

    #ifdef DEBUG_POOL
	printf("DEBUG_POOL: starts (cvt)\n");
	printf("  H %d W %d\n", H, W);
    #endif


	cvt_pooling_iterations:
	for (int it = 0; it < iterations; it++) {
		DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=(O_REFERENCE/CPO) * W_REFERENCE * H_REFERENCE)
		#pragma HLS LOOP_FLATTEN off

		// Let's read CPO pixels
		pixel = in.read();

        #ifdef DEBUG_POOL
        #ifdef DEBUG_VERBOSE
		printf("DEBUG_POOL: pixel read: ");
		for (int x=0; x<CPO; x++) printf(" %f ", float(pixel.pixel[x]));
		printf("\n");
        #endif
        #endif

		if (enable_pooling) {
			// Let's write on the buffer
			if (row0_write) buffer0[pin_col].pixel[o_iter] = pixel;
			if (row1_write) buffer1[pin_col].pixel[o_iter] = pixel;

			row_counter++;
			ii_counter++;

			uint send_frame = (ii_counter > initial_interval) & (row_counter > O_ITER);

		    if (send_frame) {
				kernel.pixel[0] = buffer0[column-1].pixel[o_iter]; kernel.pixel[1] = buffer0[column].pixel[o_iter];
				kernel.pixel[2] = buffer1[column-1].pixel[o_iter]; kernel.pixel[3] = pixel;

				out << kernel;

				#ifdef DEBUG_POOL
				#ifdef DEBUG_VERBOSE
				printf("DEBUG_POOL: Send Frame:\n");
				for (int x = 0; x < CPI; x++) {
					std::cout << "  cpo : " << x << std::endl;
					std::cout << "     " << float(kernel.pixel[0].pixel[x]) << " " << float(kernel.pixel[1].pixel[x]) << std::endl;
					std::cout << "     " << float(kernel.pixel[2].pixel[x]) << " " << float(kernel.pixel[3].pixel[x]) << std::endl;
				}
				#endif
				#endif
		    }

		    o_iter++;
		    if (send_frame && (o_iter == O_ITER)) {
		    	row_counter = 0;
		    	/* SW = 2 */
		    	column = column + 2;

		    	if (column > W) {
		    		column = 1;
		    		ii_counter = 0;
		    	}
		    }

			/* Control the iteration count */
		    if (o_iter == O_ITER) {
		    	o_iter = 0;
		    	pin_col++;
				if (pin_col == W) {
					pin_col = 0;

					row0_write_aux = row0_write;
					row1_write_aux = row1_write;

					row0_write = row1_write_aux;
					row1_write = row0_write_aux;
				}
		    }
		} else {
			kernel.pixel[0] = pixel;
			out << kernel;
		}
	}

    #ifdef DEBUG_POOL
    printf("DEBUG_POOL: ends (cvt)\n");
    #endif

}

/*
* Pooling operation of the layer
*
* Parameters:
* 	HO		          : Height of the output image
*	WO		          : Width of the output image
*	enable_maxpooling : Activates maxpooling operation
*	enable_avgpooling : Activates avgpooling operation
*   in		          : Input stream (frame_pool_t) from the cvt module
*   out		          : Output stream (pixel_out_t)
*
*   If no enable is active then the module bypasses the first pixel of the incomming frame to the output stream
*/
static void pool_pooling(int HO, int WO, int O_ITER, int enable_maxpooling, int enable_avgpooling, hls::stream<frame_pool_t> &in, hls::stream<pool_st> &out) {
	
	frame_pool_t kernel;
	pool_st out_pix;
	
	int size_out = HO * WO;
	int size_kernel = KH_POOLING * KW_POOLING;
	int iterations = size_out * O_ITER;
	int enable_pooling = enable_maxpooling | enable_avgpooling;

    #ifdef DEBUG_POOL
	printf("DEBUG_POOL: Starts (pooling)\n");
    #endif

	pooling_loop_iter:
	for (int i=0; i < iterations; i++) {
		DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=(W_REFERENCE * H_REFERENCE) / 4)
		#pragma HLS PIPELINE II=1

		// Let's read the input frame
		kernel = in.read();

        #ifdef DEBUG_POOL
        #ifdef DEBUG_VERBOSE
		printf("DEBUG_POOL: read \n");
		for (int x=0; x<CPO; x++) {
			printf("cpo %d: ", x);
			for (int xx=0; xx<size_kernel; xx++) printf("   %f", float(kernel.pixel[xx].pixel[x]));
			printf("\n");
		}
        #endif
        #endif

        pooling_loop_cpo:
		for (int cpo = 0; cpo < CPO; cpo++) {
          #pragma HLS UNROLL

		  pool_t maxpool_value = MIN_DATA_TYPE_VALUE;
		  pool_t avgpool_value = 0;

		  pooling_loop_kernel:
		  for (int k=0; k < size_kernel; k++) {
			DO_PRAGMA(HLS LOOP_TRIPCOUNT min=(KW_POOLING * KH_POOLING) max=(KW_POOLING * KH_POOLING))

			pool_t value = kernel.pixel[k].pixel[cpo];

			if (value > maxpool_value) maxpool_value = value;
			avgpool_value += value;
		  }
		  avgpool_value /= size_kernel;

		  out_pix.pixel[cpo] = enable_maxpooling ? maxpool_value : enable_avgpooling ? avgpool_value : kernel.pixel[0].pixel[cpo];
		}

		out << out_pix;
        #ifdef DEBUG_POOL
        #ifdef DEBUG_VERBOSE
		printf("DEBUG_POOL: send pixel: \n");
		for (int x=0; x<CPO; x++) printf(" %f", float(out_pix.pixel[x]));
        #endif
        #endif
	}

    #ifdef DEBUG_POOL
    printf("DEBUG_POOL: Ends (pooling)\n");
    #endif
}

void pooling(int H, int W, int O_ITER, int enable_maxpooling, int enable_avgpooling, hls::stream<relu_st> &input, hls::stream<pool_st> &output) {

	static hls::stream<frame_pool_t> stream_pool("pool");
    DO_PRAGMA(HLS STREAM variable=stream_pool depth=STREAMS_DEPTH)

	int enable_pooling = enable_maxpooling | enable_avgpooling;

	int WO = enable_pooling ? ((W - KW_POOLING)/SW_POOLING + 1) : W;
	int HO = enable_pooling ? ((H - KH_POOLING)/SH_POOLING + 1) : H;

	#pragma HLS DATAFLOW
	pool_cvt(H, W, O_ITER, enable_pooling, input, stream_pool);
	pool_pooling(HO, WO, O_ITER, enable_maxpooling, enable_avgpooling, stream_pool, output);
}
