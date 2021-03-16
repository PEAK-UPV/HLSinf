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
	pixel_out_t pixel[KW_POOLING * KH_POOLING];
};


/*
* Convert module for the pooling layer
* Only works for fixed size of (KW=2, KH=2) and (SW=2, SH=2).
* 
* The output data format is (KH, KW, CPI)
* 
* Parameters:
* 	H   	: Height of the input image
* 	W		: Width of the input image
*	I_ITER 	: Input iterations (I / CPI)
* 	in		: Input stream (pixel_out_t) 
*	out 	: Output stream (frame_pool_t)
*/
static void cvt_pooling(int H, int W, int I_ITER, hls::stream<pixel_out_t> &in, hls::stream<frame_pool_t> &out) {

	frame_pool_t kernel;
	pixel_out_t buffer[KH][WMAX];
	int row0_write;
	int row0;
	int shift_frame, send_frame;
	int p, pin_row, pin_col;

	pixel_out_t p0, p1, p2, p3;
	pixel_out_t pix_b0, pix_b1;

	/* These pragmas were not tested */
	DO_PRAGMA(HLS data_pack variable=p0);
	DO_PRAGMA(HLS data_pack variable=p1);
	DO_PRAGMA(HLS data_pack variable=p2);
	DO_PRAGMA(HLS data_pack variable=p3);

	DO_PRAGMA(HLS data_pack variable=kernel);
	DO_PRAGMA(HLS data_pack variable=buffer);
	DO_PRAGMA(HLS ARRAY_PARTITION variable=buffer cyclic dim=2 factor=CPI);
	DO_PRAGMA(HLS ARRAY_PARTITION variable=buffer cyclic dim=1 factor=KH);

	int size_channel = H * W;
	int iterations = I_ITER * size_channel;

	for (int i=0; i < iterations; i++) {
		#pragma HLS PIPELINE II=1

		if (!p) {
			pin_row = 0;
			pin_col = 0;
		}

		pixel_out_t pixel;
		pixel = in.read();

		row0_write = (pin_row % KH_POOLING) == 0;
		row0 = (pin_row <= 1) | ((pin_row % KH_POOLING) == 1);

		buffer[row0_write][pin_col] = pixel;

		shift_frame = (pin_row > 0) & (pin_col > 1);
		send_frame = (pin_row > 0) & (pin_col > 0);
		send_frame &= ((pin_row & SH_POOLING == 1) && (pin_col % SW_POOLING == 1));

		pix_b0 = buffer[0][pin_col];
		pix_b1 = buffer[1][pin_col];

		/* p0 p1 */
		if (shift_frame) {p0 = p1; } else if (!pin_col) {if (row0) p0 = pix_b0; else p0 = pix_b1; }
		if (row0) p1 = pix_b0; else p1 = pix_b1;

		/* p2 p3 */
		if (shift_frame) {p2 = p3; } else if (!pin_col) {if (row0) p2 = pix_b1; else p1 = pix_b0; }
		if (row0) p3 = pix_b1; else p2 = pix_b0;

		if (send_frame) {
			kernel.pixel[0] = p0; kernel.pixel[1] = p1;
			kernel.pixel[2] = p2; kernel.pixel[3] = p3;

			out << kernel;
		}

		/* Control the iteration count */
		if (pin_col == W) {
			pin_col = 0;
			pin_row++;
		}
		
		p++;
		if (p == size_channel) p = 0;
	}
}

/*
* Pooling operation of the layer
* If USE_MAXPOOLING defined, the MaxPooling is done.
* If USE_AVGPOOLING defined, the AvgPooling is done.
*
* Parameters:
* 	HO		: Height of the output image
*	WO		: Width of the output image
*   I_ITER 	: Input iterations (I / CPO) CPI if CPI=CPO
*   in		: Input stream (frame_pool_t) from the cvt module
*   out		: Output stream (pixel_out_t)
*/
static void fn_pooling(int HO, int WO, int I_ITER, hls::stream<frame_pool_t> &in, hls::stream<pixel_out_t> &out) {
	
	frame_pool_t kernel;
	pixel_out_t out_pix;
	
	int size_out = HO * WO;
	int size_kernel = KH_POOLING * KW_POOLING;
	int iterations = I_ITER * size_out;

	for (int i=0; i < iterations; i++) {
		#pragma HLS PIPELINE II=1

		kernel = in.read();

		for (int cpo=0; cpo < CPO; cpo++) { /* We assume that CPI=CPO */
			data_type pool_value;

			for (int k=0; k < size_kernel; k++) {
				data_type value = kernel.pixel[k].pixel[cpo];

				#ifdef USE_MAXPOOLING
					pool_value = (value > pool_value) ? value;
				#endif
				#ifdef USE_AVGPOOLING
					pool_value += value;
				#endif
			}
			#ifdef USE_AVGPOOLING
				pool_value /= size_kernel;
			#endif

			out_pix.pixel[cpo] = pool_value;
		}
		out << out_pix;
	}
}

void pooling(int H, int W, int I_ITER, hls::stream<pixel_out_t> &input, hls::stream<pixel_out_t> &output) {
	#pragma HLS INTERFACE s_axilite port=H bundle=control
	#pragma HLS INTERFACE s_axilite port=W bundle=control
	#pragma HLS INTERFACE s_axilite port=I_ITER bundle=control
	#pragma HLS INTERFACE ap_fifo port=input
	#pragma HLS INTERFACE ap_fifo port=output

	static hls::stream<frame_pool_t> stream_pool ("stream_pool");

	#pragma HLS STREAM variable=str_pool

	int WO = (int) ((W - KW_POOLING)/SW_POOLING + 1);
	int HO = (int) ((H - KH_POOLING)/SH_POOLING + 1);

	#pragma HLS DATAFLOW
	cvt_pooling(H, W, I_ITER, input, stream_pool);
	fn_pooling(HO, WO, I_ITER, stream_pool, output);
}