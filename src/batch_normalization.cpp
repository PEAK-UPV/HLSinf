/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

// -----------------------------------------------------------------------------------------------------------
// Batch normalization layer

#define EPS		1e-5

void batch_norm(int enable_batch_norm, int num_pixels, hls::stream<pool_st> &in, hls::stream<bnp_st> &bn_values, hls::stream<dout_st> &out) {

	#ifdef DEBUG_BATCH_NORM
	printf("Batch Norm: start\n");
	#endif

	pool_st data_in;
	bnp_st bn_values_in;
	dout_st data_out;
	int data_size = num_pixels;

	DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=bn_values_in complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=data_out complete dim=0)

	if (enable_batch_norm) {
		// Reading batch normalization values
		bn_values_in = bn_values.read();
	}

	loop_batch_norm_pixels:
	for (int i = 0; i < data_size; i++) {
		DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE*H_REFERENCE)
		#pragma HLS PIPELINE II=1

		// Read input data
		data_in = in.read();

		loop_batch_norm_cpo:
		for (int cpo = 0; cpo < CPO; cpo++) {
			DO_PRAGMA(HLS loop_tripcount min=1 max=CPO)
			#pragma HLS UNROLL

			bn_t v_in, std, xn, v_batchnorm;
			v_in = data_in.pixel[cpo];

			if (enable_batch_norm) {
				// Apply normalization
				// std = sqrt(run_var + eps)
				std = hls::sqrtf(bn_values_in.values[(cpo*4)+3] + EPS);
				// xn = (prev_a - run_mean) / std
				xn = (v_in - bn_values_in.values[(cpo*4)+2]) / std;
				// y = gamma * xn - beta
				v_batchnorm = bn_values_in.values[(cpo*4)+1] * xn + bn_values_in.values[cpo*4];

				data_out.pixel[cpo] = v_batchnorm;
				
			} else {
				data_out.pixel[cpo] = v_in;
			}
		}

		#ifdef DEBUG_BATCH_NORM
		printf("Batch Norm (pixel %d): \n", i);
		for (int x = 0; x < CPI; x++) {
			printf("   cpi %d : in %f out %f\n", x, float(data_in.pixel[x]), float(data_out.pixel[x]));
		}
		#endif

		out << data_out;
	}

	#ifdef DEBUG_BATCH_NORM
	printf("Batch Norm: end\n");
	#endif
}
