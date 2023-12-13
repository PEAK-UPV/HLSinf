/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Polit�?cnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"


// ---------------------------------------------------------------------------------------------------
// cvt: reads an input stream with an image of format (H, W, CPI) and writes an output stream
// in a 2D format based on (KW, KH) and (SH, SW). (PW=1, PH=1) padding is assumed.
// The function outputs data in the format (KH, KW, CPI).
//
// Arguments:
//   H      : Height of input channel
//   W      : Width of input channel
//   I_ITER : Number of input iterations (I / CPI)
//   SH     : Vertical stride
//   SW     : Horizontal stride
//   in     : input stream (format pixel_in_t)
//   out    : output stream (format frame_t)
//


void cvt(int H, int W, int I_ITER, int SH, int SW, hls::stream<din_st> &in, hls::stream<conv_cvt_st> &out) {
#ifdef DEBUG_CVT
	std::cout << "cvt: start" << std::endl;
	std::cout << "  H: " << H << ", W: " << W << ", I_ITER: " << I_ITER << ", SH: " << SH << ", SW: " << SW << std::endl;
#endif

	// buffers (keep three rows)
	cvt_in_st buffer0[WMAX+2];
	cvt_in_st buffer1[WMAX+2];
	cvt_in_st buffer2[WMAX+2];

	conv_cvt_st frame;

	// We have to partition the buffers to increase ports to meet the II=1
	#pragma HLS ARRAY_RESHAPE variable=buffer0 type=complete dim=3
	#pragma HLS ARRAY_RESHAPE variable=buffer1 type=complete dim=3
	#pragma HLS ARRAY_RESHAPE variable=buffer2 type=complete dim=3

	#pragma HLS ARRAY_PARTITION variable=frame type=complete dim=0

	uint i_iter = 0;
	uint pin_col = 0;

	uint row0_buffer_write = 1;
	uint row0_bw_aux;
	uint row1_buffer_write = 0;
	uint row1_bw_aux;
	uint row2_buffer_write = 0;
	uint row2_bw_aux;

	uint row0 = 1;
	uint row1 = 0;
	uint row2 = 0;
	uint row0_aux, row1_aux, row2_aux;
	uint column = 1;

	uint num_iters = H * W * I_ITER;
	uint initial_interval = (2 * (W * I_ITER)) + (2 * I_ITER);
	uint row_interval = ((SH-1) * (W * I_ITER)) + (2 * I_ITER);
	uint col_interval = (I_ITER * (SW-1));

	uint row_interval_counter = 0;
	uint col_interval_counter = 0;

    cvt_loop:
	for (uint p = 0; p < num_iters; p++) {
		DO_PRAGMA(HLS loop_tripcount  min=1 max=(I_REFERENCE/CPI) * (H_REFERENCE+2)*(W_REFERENCE+2))
		#pragma HLS LOOP_FLATTEN off

		din_st pixel;

		#pragma HLS AGGREGATE variable=pixel

		// Read CPI values from stream
		pixel = in.read();

		#ifdef DEBUG_CVT
		std::cout << std::endl;
		std::cout << "Pixel p: " << p << std::endl;
		std::cout << "CVT: read data (i_iter: " << i_iter  << " pin_col: " << pin_col << " column: " << column << " row_interval_counter: " << row_interval_counter << ")" << std::endl;
		std::cout << "Pixel: " << pixel.pixel[0] << " " << pixel.pixel[1] << " " << pixel.pixel[2] << " " << pixel.pixel[3] << std::endl;
		#endif

		// Write pixel into the buffer
		if (row0_buffer_write) buffer0[pin_col].pixel[i_iter] = pixel;
		if (row1_buffer_write) buffer1[pin_col].pixel[i_iter] = pixel;
		if (row2_buffer_write) buffer2[pin_col].pixel[i_iter] = pixel;

		// build the frame
		din_st p0, p1, p2, p3, p4, p5, p6, p7, p8;


		// Send frame to the next module
		uint send_frame = (p >= initial_interval) & (row_interval_counter >= row_interval) & (col_interval_counter >= col_interval);

		row_interval_counter++;
		col_interval_counter++;

		if (send_frame) {
			if (row0) {
				p0 = buffer0[column-1].pixel[i_iter]; p1 = buffer0[column].pixel[i_iter]; p2 = buffer0[column+1].pixel[i_iter];
				p3 = buffer1[column-1].pixel[i_iter]; p4 = buffer1[column].pixel[i_iter]; p5 = buffer1[column+1].pixel[i_iter];
				p6 = buffer2[column-1].pixel[i_iter]; p7 = buffer2[column].pixel[i_iter]; p8 = pixel;
			}
			if (row1) {
				p0 = buffer1[column-1].pixel[i_iter]; p1 = buffer1[column].pixel[i_iter]; p2 = buffer1[column+1].pixel[i_iter];
				p3 = buffer2[column-1].pixel[i_iter]; p4 = buffer2[column].pixel[i_iter]; p5 = buffer2[column+1].pixel[i_iter];
				p6 = buffer0[column-1].pixel[i_iter]; p7 = buffer0[column].pixel[i_iter]; p8 = pixel;
			}
			if (row2) {
				p0 = buffer2[column-1].pixel[i_iter]; p1 = buffer2[column].pixel[i_iter]; p2 = buffer2[column+1].pixel[i_iter];
				p3 = buffer0[column-1].pixel[i_iter]; p4 = buffer0[column].pixel[i_iter]; p5 = buffer0[column+1].pixel[i_iter];
				p6 = buffer1[column-1].pixel[i_iter]; p7 = buffer1[column].pixel[i_iter]; p8 = pixel;
			}

			frame.pixel[0] = p0; frame.pixel[1] = p1; frame.pixel[2] = p2;
			frame.pixel[3] = p3; frame.pixel[4] = p4; frame.pixel[5] = p5;
			frame.pixel[6] = p6; frame.pixel[7] = p7; frame.pixel[8] = p8;

			out << frame;

			#ifdef DEBUG_CVT
			#ifdef DEBUG_VERBOSE
			std::cout << "cvt: frame sent:" << std::endl;
			for (int cpi = 0; cpi < CPI; cpi++) {
				std::cout << "  cpi : " << cpi << std::endl;
				std::cout << "  " << float(frame.pixel[0].pixel[cpi]) << " " << float(frame.pixel[1].pixel[cpi]) << " " << float(frame.pixel[2].pixel[cpi]) << std::endl;
				std::cout << "  " << float(frame.pixel[3].pixel[cpi]) << " " << float(frame.pixel[4].pixel[cpi]) << " " << float(frame.pixel[5].pixel[cpi]) << std::endl;
				std::cout << "  " << float(frame.pixel[6].pixel[cpi]) << " " << float(frame.pixel[7].pixel[cpi]) << " " << float(frame.pixel[8].pixel[cpi]) << std::endl;
			}
			#endif
			#endif
		}

		i_iter++;
		// Next column
		if (send_frame && (i_iter == I_ITER)) {
			col_interval_counter = 0;
			column = column + SW;
		}

		// Next row
		if (column >= W - 1) {
			column = 1;
			row_interval_counter = 0;

			row0_aux = row0;
			row1_aux = row1;
			row2_aux = row2;

			row0 = row2_aux;
			row1 = row0_aux;
			row2 = row1_aux;
		}

		if (i_iter == I_ITER) {
			i_iter = 0;
			pin_col++;
			if (pin_col == W) {
				pin_col = 0;

				// Row buffer write rotation
				row0_bw_aux = row0_buffer_write;
				row1_bw_aux = row1_buffer_write;
				row2_bw_aux = row2_buffer_write;

				row0_buffer_write = row2_bw_aux;
				row1_buffer_write = row0_bw_aux;
				row2_buffer_write = row1_bw_aux;
			}
		}
	} // p

	#ifdef DEBUG_CVT
	std::cout << "cvt: end" << std::endl;
	#endif
}

