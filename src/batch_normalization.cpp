#include "conv2D.h"

// -----------------------------------------------------------------------------------------------------------
// Batch normalization layer

#define EPS		hls::pow(1, -5)

void batch_norm(int enable_batch_norm, int num_pixels, hls::stream<pixel_out_t> &in, hls::stream<batch_norm_in_t> &bn_values, hls::stream<pixel_out_t> &out) {

	#ifdef DEBUG_BATCH_NORM
	printf("Batch Norm: start\n");
	#endif

	pixel_out_t data_in;
	batch_norm_in_t bn_values_in;
	pixel_out_t data_out;
	int data_size = num_pixels;

	DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=bn_values_in complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=data_out complete dim=0)

	// Reading batch normalization values
	bn_values_in = bn_values.read();

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

			data_type v_in, std, xn, v_batchnorm;
			v_in = data_in.pixel[cpo];

			if(enable_batch_norm) {
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
