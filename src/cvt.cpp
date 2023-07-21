/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
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


void cvt(int H, int W, int I_ITER, int O_ITER, int SH, int SW, hls::stream<cvt_in_st> &in, hls::stream<conv_cvt_st> &out) {
#ifdef DEBUG_CVT
	std::cout << "cvt: start" << std::endl;
	std::cout << "  H: " << H << ", W: " << W << ", I_ITER: " << I_ITER << ", SH: " << SH << ", SW: " << SW << std::endl;
#endif

	uint HH = H;
	uint WW = W;

	// buffers (keep three rows)
	cvt_in_st buffer0[WMAX+2];
	cvt_in_st buffer1[WMAX+2];
	cvt_in_st buffer2[WMAX+2];
	conv_cvt_st frame;

	uint pin_row;
	uint pin_col;
	uint stride_row;
	uint stride_col;
	uint row0_buffer_write;
	uint row0_bw_aux;
	uint row1_buffer_write;
	uint row1_bw_aux;
	uint row2_buffer_write;
	uint row2_bw_aux;
	uint num_pixels = HH * WW;

    #pragma HLS ARRAY_PARTITION variable=frame type=complete dim=0
	// TODO: Optimize
	//#pragma HLS AGGREGATE variable=buffer0
	//#pragma HLS AGGREGATE variable=buffer1
	//#pragma HLS AGGREGATE variable=buffer2

    uint p = 0;

    cvt_loop:
	for (uint i_iter = 0; i_iter < num_pixels; i_iter++) {
		DO_PRAGMA(HLS loop_tripcount  min=1 max=(I_REFERENCE/CPI) * (H_REFERENCE+2)*(W_REFERENCE+2))
		#pragma HLS PIPELINE II=1

		if (p == 0) {
			pin_row = 0;
			pin_col = 0;
			row0_buffer_write = 1;
			row1_buffer_write = 0;
			row2_buffer_write = 0;
			stride_row = 2;
			stride_col = 2;
		}

		uint pin_col0 = (pin_col==0);
		uint pin_col1 = (pin_col==1);

		// get the pixel
		cvt_in_st pixel;
		#pragma HLS ARRAY_PARTITION variable=pixel type=complete dim=0
		pixel = in.read();

		#ifdef DEBUG_CVT
		std::cout << "CVT: read data (i_iter: " << i_iter << " pin_row: " << pin_row << " pin_col: " << pin_col << " stride_row: " << stride_col << " stride_col: " << stride_col << std::endl;
		#endif

		// row buffer write (in which buffer row we write the pixel)
		// first row buffer
		uint row0 = (pin_row <= 2) | ((pin_row % 3) == 2);
		uint row1 = !row0 & ((pin_row % 3) == 0);

		// write pixel into the buffer
		if (row0_buffer_write) buffer0[pin_col] = pixel;
		if (row1_buffer_write) buffer1[pin_col] = pixel;
		if (row2_buffer_write) buffer2[pin_col] = pixel;

		// build the frame
		cvt_in_st p0, p1, p2, p3, p4, p5, p6, p7, p8;

		uint shift_frame = (pin_row > 1) & (pin_col > 2);
		// Send frame to the next module
		uint send_frame = (pin_row > 1) & (pin_col > 1) & (stride_row == SH) & (stride_col == SW);

		cvt_in_st pixel_b0, pixel_b1, pixel_b2;
		pixel_b0 = buffer0[pin_col];
		pixel_b1 = buffer1[pin_col];
		pixel_b2 = buffer2[pin_col];
		pin_col++;

		if (stride_col == 1) stride_col = SW; else stride_col = stride_col - 1;
		int pin_col_curr = pin_col;

		// p0, p1, p2
		if (shift_frame) {p0 = p1;} else if (pin_col0) {if (row0) p0 = pixel_b0; else if (row1) p0 = pixel_b1; else p0 = pixel_b2;}
		if (shift_frame) {p1 = p2;} else if (pin_col1) {if (row0) p1 = pixel_b0; else if (row1) p1 = pixel_b1; else p1 = pixel_b2;}
		if (row0) p2 = pixel_b0; else if (row1) p2 = pixel_b1; else p2 = pixel_b2;
		// p3, p4, p5
		if (shift_frame) {p3 = p4;} else if (pin_col0) {if (row0) p3 = pixel_b1; else if (row1) p3 = pixel_b2; else p3 = pixel_b0;}
		if (shift_frame) {p4 = p5;} else if (pin_col1) {if (row0) p4 = pixel_b1; else if (row1) p4 = pixel_b2; else p4 = pixel_b0;}
		if (row0) p5 = pixel_b1; else if (row1) p5 = pixel_b2; else p5 = pixel_b0;
		// p6, p7, p8
		if (shift_frame) {p6 = p7;} else if (pin_col0) {if (row0) p6 = pixel_b2; else if (row1) p6 = pixel_b0; else p6 = pixel_b1;}
		if (shift_frame) {p7 = p8;} else if (pin_col1) {if (row0) p7 = pixel_b2; else if (row1) p7 = pixel_b0; else p7 = pixel_b1;}
		if (row0) p8 = pixel_b2; else if (row1) p8 = pixel_b0; else p8 = pixel_b1;
		if (send_frame) {
		  // TODO: Revisar esta parte
	      for (uint o = 0; o < O_ITER; o++) {
			  // #pragma HLS LOOP_FLATTEN
	    	  for (uint i = 0; i < I_ITER; i++) {
				  frame.pixel[0] = p0.pixel[i]; frame.pixel[1] = p1.pixel[i]; frame.pixel[2] = p2.pixel[i];
				  frame.pixel[3] = p3.pixel[i]; frame.pixel[4] = p4.pixel[i]; frame.pixel[5] = p5.pixel[i];
				  frame.pixel[6] = p6.pixel[i]; frame.pixel[7] = p7.pixel[i]; frame.pixel[8] = p8.pixel[i];

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
	      }
		}

		if (pin_col_curr == WW) {
			pin_col = 0;
			pin_row++;
			stride_col = 2;
			if (stride_row == 1) stride_row = SH; else stride_row = stride_row - 1;

		    // row buffer write rotation
			row0_bw_aux = row0_buffer_write;
			row1_bw_aux = row1_buffer_write;
			row2_bw_aux = row2_buffer_write;

			row0_buffer_write = row2_bw_aux;
			row1_buffer_write = row0_bw_aux;
			row2_buffer_write = row1_bw_aux;
		}

		p = p + 1;
	} // i_iter

	#ifdef DEBUG_CVT
	std::cout << "cvt: end" << std::endl;
	#endif
}

