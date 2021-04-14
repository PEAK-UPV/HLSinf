#include "conv2D.h"

// -----------------------------------------------------------------------------------------------------------
// frames struct winograd
struct frame_winograd {
  pixel_in_t pixel[2 * 2];
};

// ---------------------------------------------------------------------------------------------------
// cvt_winograd: reads an input stream with an image of format (H, W, CPI) and writes an output stream
// in a 2D format based on (KW, KH). (SW=2, SH=2) stride is assumed and (PW=1, PH=1) padding is assumed.
// The function outputs data in the format frame (4, 4, CPI).
//
// Arguments:
//   H      : Height of input channel
//   W      : Width of input channel
//   I_ITER : Number of input iterations (I / CPI)
//   in     : input stream (format pixel_in_t)
//   out    : output stream (format frame_d)
//
static void cvt_winograd(int H, int W, int I_ITER, hls::stream<pixel_in_t> &in, hls::stream<frame_d> &out) {

	#ifdef DEBUG_CVT
	printf("cvt: winograd START\n");
	#endif

	int HH=H+2;
	int WW=W+2;
	// buffers (keep three rows)
	pixel_in_t buffer0[WMAX+2];
	pixel_in_t buffer1[WMAX+2];
	pixel_in_t buffer2[WMAX+2];
	pixel_in_t buffer3[WMAX+2];
	frame_d frame;
	int pin_row;
	int pin_col;
	int row0_buffer_write;
	int row1_buffer_write;
	int row2_buffer_write;
	int row3_buffer_write;
	int num_pixels = HH * WW;

	DO_PRAGMA(HLS ARRAY_PARTITION variable=frame complete dim=0)


	// manually flattened loop (for the purposes of getting the expected pipelined design)
	int p = 0;
	int num_iters = I_ITER * num_pixels;
	cvt_loop:
	for(int i_iter = 0; i_iter <= num_iters; i_iter++){
		DO_PRAGMA(HLS loop_tripcount  min=1 max=(I_REFERENCE/CPI) * (H_REFERENCE+2)*(W_REFERENCE+2))
		DO_PRAGMA(HLS PIPELINE II=1)

		if (p==0) {
		  pin_row = 0;
		  pin_col = 0;
		  row0_buffer_write = 1;
		  row1_buffer_write = 0;
		  row2_buffer_write = 0;
		  row3_buffer_write = 0;
		}

		int pin_col0 = (pin_col==0);
		int pin_col1 = (pin_col==1);
		int pin_col2 = (pin_col==2);

		// get the pixel
		pixel_in_t pixel;
		DO_PRAGMA(HLS ARRAY_PARTITION variable=pixel complete dim=0)
		if(i_iter != num_iters)
		pixel = in.read();

		// row buffer write (in which buffer row we write the pixel)
		// first row buffer
		int row0 = (pin_row <= 3) | ((pin_row % 4) == 3); // pin row = 0 1 2 5
		int row1 = !row0 & ((pin_row % 4) == 0); //  pinr row = 3
		int row2 = !row1 & ((pin_row % 4) == 1);

		// we write the pixel into the buffer
		if (row0_buffer_write) buffer0[pin_col] = pixel;
		if (row1_buffer_write) buffer1[pin_col] = pixel;
		if (row2_buffer_write) buffer2[pin_col] = pixel;
		if (row3_buffer_write) buffer3[pin_col] = pixel;

		// build the frame
		pixel_in_t p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;
		int shift_frame = (pin_row>2) & (pin_col > 3);//
		int send_frame = (pin_row>2) & (pin_col > 2); //stride 2, el frame se envia pasado 2 filas y 2 columnas
		send_frame = (send_frame) & ((pin_row%2 == 1) && (pin_col%2 == 1));
		pixel_in_t pixel_b0, pixel_b1, pixel_b2, pixel_b3;
		pixel_b0 = buffer0[pin_col];
		pixel_b1 = buffer1[pin_col];
		pixel_b2 = buffer2[pin_col];
		pixel_b3 = buffer3[pin_col];
		pin_col++;
		int pin_col_curr = pin_col;
		// p0, p1, p2, p3
		if (shift_frame) {p0 = p1;} else if (pin_col0) {if (row0) p0 = pixel_b0; else if (row1) p0 = pixel_b1; else if (row2) p0 = pixel_b2; else p0 = pixel_b3;}
		if (shift_frame) {p1 = p2;} else if (pin_col1) {if (row0) p1 = pixel_b0; else if (row1) p1 = pixel_b1; else if (row2) p1 = pixel_b2; else p1 = pixel_b3;}
		if (shift_frame) {p2 = p3;} else if (pin_col2) {if (row0) p2 = pixel_b0; else if (row1) p2 = pixel_b1; else if (row2) p2 = pixel_b2; else p2 = pixel_b3;}
		if (row0) p3 = pixel_b0; else if (row1) p3 = pixel_b1; else if (row2) p3 = pixel_b2; else p3 = pixel_b3;
		// p4, p5, p6, p7
		if (shift_frame) {p4 = p5;} else if (pin_col0) {if (row0) p4 = pixel_b1; else if (row1) p4 = pixel_b2; else if (row2) p4 = pixel_b3; else p4 = pixel_b0;}
		if (shift_frame) {p5 = p6;} else if (pin_col1) {if (row0) p5 = pixel_b1; else if (row1) p5 = pixel_b2; else if (row2) p5 = pixel_b3; else p5 = pixel_b0;}
		if (shift_frame) {p6 = p7;} else if (pin_col2) {if (row0) p6 = pixel_b1; else if (row1) p6 = pixel_b2; else if (row2) p6 = pixel_b3; else p6 = pixel_b0;}
		if (row0) p7 = pixel_b1; else if (row1) p7 = pixel_b2; else if (row2) p7 = pixel_b3; else p7 = pixel_b0;
		// p8, p9, p10, p11
		if (shift_frame) {p8 = p9;} else if (pin_col0) {if (row0) p8 = pixel_b2; else if (row1) p8 = pixel_b3; else if (row2) p8 = pixel_b0; else p8 = pixel_b1;}
		if (shift_frame) {p9 = p10;} else if (pin_col1) {if (row0) p9 = pixel_b2; else if (row1) p9 = pixel_b3; else if (row2) p9 = pixel_b0; else p8 = pixel_b1;}
		if (shift_frame) {p10 = p11;} else if (pin_col2) {if (row0) p10 = pixel_b2; else if (row1) p10 = pixel_b3; else if (row2) p10 = pixel_b0; else p10 = pixel_b0;}
		if (row0) p11 = pixel_b2; else if (row1) p11 = pixel_b3; else if (row2) p11 = pixel_b0; else p11 = pixel_b1;
		// p12, p13, p14, p15
		if (shift_frame) {p12 = p13;} else if (pin_col0) {if (row0) p12 = pixel_b3; else if (row1) p12 = pixel_b0; else if (row2) p12 = pixel_b1; else p12 = pixel_b2;}
		if (shift_frame) {p13 = p14;} else if (pin_col1) {if (row0) p13 = pixel_b3; else if (row1) p13 = pixel_b0; else if (row2) p13 = pixel_b1; else p13 = pixel_b2;}
		if (shift_frame) {p14 = p15;} else if (pin_col2) {if (row0) p14 = pixel_b3; else if (row1) p14 = pixel_b0; else if (row2) p14 = pixel_b1; else p14 = pixel_b2;}
		if (row0) p15 = pixel_b3; else if (row1) p15 = pixel_b0; else if (row2) p15 = pixel_b1; else p15 = pixel_b2;

		if (send_frame) {
			for(int cpi= 0; cpi < CPI; cpi++){
				frame.pixel[0].pixel[cpi] = p0.pixel[cpi];
				frame.pixel[1].pixel[cpi] = p1.pixel[cpi];
				frame.pixel[2].pixel[cpi] = p2.pixel[cpi];
				frame.pixel[3].pixel[cpi] = p3.pixel[cpi];
				frame.pixel[4].pixel[cpi] = p4.pixel[cpi];
				frame.pixel[5].pixel[cpi] = p5.pixel[cpi];
				frame.pixel[6].pixel[cpi] = p6.pixel[cpi];
				frame.pixel[7].pixel[cpi] = p7.pixel[cpi];
				frame.pixel[8].pixel[cpi] = p8.pixel[cpi];
				frame.pixel[9].pixel[cpi] = p9.pixel[cpi];
				frame.pixel[10].pixel[cpi] = p10.pixel[cpi];
				frame.pixel[11].pixel[cpi] = p11.pixel[cpi];
				frame.pixel[12].pixel[cpi] = p12.pixel[cpi];
				frame.pixel[13].pixel[cpi] = p13.pixel[cpi];
				frame.pixel[14].pixel[cpi] = p14.pixel[cpi];
				frame.pixel[15].pixel[cpi] = p15.pixel[cpi];
			}
		 out << frame;

		 #ifdef DEBUG_CVT
		  printf("cvt: frame sent1 %d:\n", i_iter);
		  for (int cpi=0; cpi<CPI/2; cpi++) {
			printf("  cpi %d:\n", cpi);
			printf("    %6.4f %6.4f %6.4f %6.4f\n", float(frame.pixel[0].pixel[cpi]), float(frame.pixel[1].pixel[cpi]), float(frame.pixel[2].pixel[cpi]), float(frame.pixel[3].pixel[cpi]));
			printf("    %6.4f %6.4f %6.4f %6.4f\n", float(frame.pixel[4].pixel[cpi]), float(frame.pixel[5].pixel[cpi]), float(frame.pixel[6].pixel[cpi]), float(frame.pixel[7].pixel[cpi]));
			printf("    %6.4f %6.4f %6.4f %6.4f\n", float(frame.pixel[8].pixel[cpi]), float(frame.pixel[9].pixel[cpi]), float(frame.pixel[10].pixel[cpi]), float(frame.pixel[11].pixel[cpi]));
			printf("    %6.4f %6.4f %6.4f %6.4f\n", float(frame.pixel[12].pixel[cpi]), float(frame.pixel[13].pixel[cpi]), float(frame.pixel[14].pixel[cpi]),  float(frame.pixel[15].pixel[cpi]));
		  }
		 #endif
		}

		if (pin_col_curr == WW) {
			pin_col = 0;
			pin_row++;
			// row buffer write rotation
			if (row0_buffer_write) {
				row0_buffer_write = 0;
				row1_buffer_write = 1;
			} else if (row1_buffer_write) {
				row1_buffer_write = 0;
				row2_buffer_write = 1;
			} else if (row2_buffer_write) {
				row2_buffer_write = 0;
				row3_buffer_write = 1;
			} else {
				row3_buffer_write = 0;
				row0_buffer_write = 1;
			}
		}
		p = p + 1;
		if (p == num_pixels) p = 0;
	} //i_iter


	#ifdef DEBUG_CVT
	printf("cvt: end\n");
	#endif
}

// ---------------------------------------------------------------------------------------------------
// frameConvert: reads an input stream with frames of format (4, 4, CPI) and writes an output stream
// data in the format frame (4, 4, CPI/2).
//
// Arguments:
//   H      : Height of input channel
//   W      : Width of input channel
//   I_ITER : Number of input iterations (I / CPI)
//   in     : input stream (format frame_d)
//   out    : output stream (format frame_d_2)
//
static void frameConvert(int H, int W, int I_ITER, hls::stream<frame_d> &d_in, hls::stream<frame_d_2> &out){
	
	frame_d_2 subframe1;
	frame_d_2 subframe2;
	int send1 = 0;
	int enable2=0;
	frame_d frame;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=frame     complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=subframe1 complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=subframe2 complete dim=0)

	// Reading data
	loop_i_iter_frameConvert:
    for (int i_iter = 0; i_iter < I_ITER; i_iter++) {
    	DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE/CPI)

		loop_frames_frameConvert:
		for (int p = 0; p < (H/2 * W/2 * 2); p++) {
			DO_PRAGMA(HLS loop_tripcount min=1 max=(H_REFERENCE/2) * (W_REFERENCE/2))
			DO_PRAGMA(HLS PIPELINE)

			if (p%2==0) {

				frame = d_in.read();
				for(int cpi= 0; cpi < CPI/2; cpi++){
					subframe1.pixel[0].pixel[cpi] = frame.pixel[0].pixel[cpi];
					subframe1.pixel[1].pixel[cpi] = frame.pixel[1].pixel[cpi];
					subframe1.pixel[2].pixel[cpi] = frame.pixel[2].pixel[cpi];
					subframe1.pixel[3].pixel[cpi] = frame.pixel[3].pixel[cpi];
					subframe1.pixel[4].pixel[cpi] = frame.pixel[4].pixel[cpi];
					subframe1.pixel[5].pixel[cpi] = frame.pixel[5].pixel[cpi];
					subframe1.pixel[6].pixel[cpi] = frame.pixel[6].pixel[cpi];
					subframe1.pixel[7].pixel[cpi] = frame.pixel[7].pixel[cpi];
					subframe1.pixel[8].pixel[cpi] = frame.pixel[8].pixel[cpi];
					subframe1.pixel[9].pixel[cpi] = frame.pixel[9].pixel[cpi];
					subframe1.pixel[10].pixel[cpi] = frame.pixel[10].pixel[cpi];
					subframe1.pixel[11].pixel[cpi] = frame.pixel[11].pixel[cpi];
					subframe1.pixel[12].pixel[cpi] = frame.pixel[12].pixel[cpi];
					subframe1.pixel[13].pixel[cpi] = frame.pixel[13].pixel[cpi];
					subframe1.pixel[14].pixel[cpi] = frame.pixel[14].pixel[cpi];
					subframe1.pixel[15].pixel[cpi] = frame.pixel[15].pixel[cpi];
				}
				for(int cpi2= CPI/2; cpi2 < CPI; cpi2++){
					subframe2.pixel[0].pixel[cpi2-CPI/2] = frame.pixel[0].pixel[cpi2];
					subframe2.pixel[1].pixel[cpi2-CPI/2] = frame.pixel[1].pixel[cpi2];
					subframe2.pixel[2].pixel[cpi2-CPI/2] = frame.pixel[2].pixel[cpi2];
					subframe2.pixel[3].pixel[cpi2-CPI/2] = frame.pixel[3].pixel[cpi2];
					subframe2.pixel[4].pixel[cpi2-CPI/2] = frame.pixel[4].pixel[cpi2];
					subframe2.pixel[5].pixel[cpi2-CPI/2] = frame.pixel[5].pixel[cpi2];
					subframe2.pixel[6].pixel[cpi2-CPI/2] = frame.pixel[6].pixel[cpi2];
					subframe2.pixel[7].pixel[cpi2-CPI/2] = frame.pixel[7].pixel[cpi2];
					subframe2.pixel[8].pixel[cpi2-CPI/2] = frame.pixel[8].pixel[cpi2];
					subframe2.pixel[9].pixel[cpi2-CPI/2] = frame.pixel[9].pixel[cpi2];
					subframe2.pixel[10].pixel[cpi2-CPI/2] = frame.pixel[10].pixel[cpi2];
					subframe2.pixel[11].pixel[cpi2-CPI/2] = frame.pixel[11].pixel[cpi2];
					subframe2.pixel[12].pixel[cpi2-CPI/2] = frame.pixel[12].pixel[cpi2];
					subframe2.pixel[13].pixel[cpi2-CPI/2] = frame.pixel[13].pixel[cpi2];
					subframe2.pixel[14].pixel[cpi2-CPI/2] = frame.pixel[14].pixel[cpi2];
					subframe2.pixel[15].pixel[cpi2-CPI/2] = frame.pixel[15].pixel[cpi2];
				}
				out << subframe1;

				#ifdef DEBUG_VERBOSE
					printf("cvt: frame sent1 %d:\n", i_iter);
					for (int cpi=0; cpi<CPI/2; cpi++) {
						printf("  cpi %d:\n", cpi);
						printf("    %6.4f %6.4f %6.4f %6.4f\n", float(subframe1.pixel[0].pixel[cpi]), float(subframe1.pixel[1].pixel[cpi]), float(subframe1.pixel[2].pixel[cpi]), float(subframe1.pixel[3].pixel[cpi]));
						printf("    %6.4f %6.4f %6.4f %6.4f\n", float(subframe1.pixel[4].pixel[cpi]), float(subframe1.pixel[5].pixel[cpi]), float(subframe1.pixel[6].pixel[cpi]), float(subframe1.pixel[7].pixel[cpi]));
						printf("    %6.4f %6.4f %6.4f %6.4f\n", float(subframe1.pixel[8].pixel[cpi]), float(subframe1.pixel[9].pixel[cpi]), float(subframe1.pixel[10].pixel[cpi]), float(subframe1.pixel[11].pixel[cpi]));
						printf("    %6.4f %6.4f %6.4f %6.4f\n", float(subframe1.pixel[12].pixel[cpi]), float(subframe1.pixel[13].pixel[cpi]), float(subframe1.pixel[14].pixel[cpi]),  float(subframe1.pixel[15].pixel[cpi]));
					}
				#endif

			}

			if (p%2==1) {
				out << subframe2;
				#ifdef DEBUG_VERBOSE
					printf("cvt: frame sent2 %d:\n", i_iter);
					for (int cpi=0; cpi<CPI/2; cpi++) {
						printf("  cpi %d:\n", cpi);
						printf("    %6.4f %6.4f %6.4f %6.4f\n", float(subframe2.pixel[0].pixel[cpi]), float(subframe2.pixel[1].pixel[cpi]), float(subframe2.pixel[2].pixel[cpi]), float(subframe2.pixel[3].pixel[cpi]));
						printf("    %6.4f %6.4f %6.4f %6.4f\n", float(subframe2.pixel[4].pixel[cpi]), float(subframe2.pixel[5].pixel[cpi]), float(subframe2.pixel[6].pixel[cpi]), float(subframe2.pixel[7].pixel[cpi]));
						printf("    %6.4f %6.4f %6.4f %6.4f\n", float(subframe2.pixel[8].pixel[cpi]), float(subframe2.pixel[9].pixel[cpi]), float(subframe2.pixel[10].pixel[cpi]), float(subframe2.pixel[11].pixel[cpi]));
						printf("    %6.4f %6.4f %6.4f %6.4f\n", float(subframe2.pixel[12].pixel[cpi]), float(subframe2.pixel[13].pixel[cpi]), float(subframe2.pixel[14].pixel[cpi]),  float(subframe2.pixel[15].pixel[cpi]));
					}
				#endif
			}

		}
    }

}

// ----------------------------------------------------------------------------------------------------------
// mulData: This function performs the multiplication of an input frame_d_2 of data with two matrix 
// to produce winograd convolution and sends the produced result frames in a frame_d_2 format (4, 4, CPI/2).
// However, these matrix are not loaded, the multiplication it is done directly assigning the combination
// of input data positions to the right exit position.
// Arguments:
//   H     : Height of the input channel
//   W     : Width of the input channel
//   I_ITER: Number of input iterations (I / CPI)
//   in    : input stream with incoming data frames
//   out   : output stream
//
static void mulData(int H, int W, int I_ITER, hls::stream<frame_d_2> &d_in, hls::stream<frame_d_2> &out) {

	#ifdef DEBUG_VERBOSE
	printf("mulData: start\n");
	#endif
	
	frame_d_2 data;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=data dim=0)
	frame_d_2 res;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=res dim=0)
	data_type CTd[CPI][4][4];
	data_type CTdC[CPI][4][4];
	DO_PRAGMA(HLS ARRAY_PARTITION variable=CTd dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=CTdC dim=0)

	int cont = 0;
	for( int i_iter = 0; i_iter < I_ITER; i_iter++){
		DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE/CPI)
		for (int i = 0; i < (H/2 * W); i++){
			DO_PRAGMA(HLS loop_tripcount min=1 max=(H_REFERENCE/2) * W_REFERENCE)
			#pragma HLS PIPELINE II=1
			data = d_in.read();

			for(int i = 0; i<4; i++){
				#pragma HLS UNROLL
				for (int cpi=0; cpi < CPI/2; cpi++) {
					CTd[cpi][0][i] = data.pixel[i].pixel[cpi] - data.pixel[i+8].pixel[cpi];
					CTd[cpi][1][i] = data.pixel[i+4].pixel[cpi] + data.pixel[i+8].pixel[cpi];
					CTd[cpi][2][i] = data.pixel[i+8].pixel[cpi] - data.pixel[i+4].pixel[cpi];
					CTd[cpi][3][i] = data.pixel[i+4].pixel[cpi] - data.pixel[i+12].pixel[cpi];
				}
			}

			for(int i = 0; i<4; i++){
				#pragma HLS UNROLL
				for (int cpi=0; cpi < CPI/2; cpi++) {
					CTdC[cpi][i][0] = CTd[cpi][i][0] - CTd[cpi][i][2];
					CTdC[cpi][i][1] = CTd[cpi][i][1] + CTd[cpi][i][2];
					CTdC[cpi][i][2] = CTd[cpi][i][2] - CTd[cpi][i][1];
					CTdC[cpi][i][3] = CTd[cpi][i][1] - CTd[cpi][i][3];
				}
			}

			int pos = 0;
			for(int f = 0; f < 4; f++){
				#pragma HLS UNROLL
				for(int c = 0; c < 4; c++){
					for (int cpi=0; cpi < CPI/2; cpi++) {
						res.pixel[pos].pixel[cpi] = CTdC[cpi][f][c];

					}
					pos++;
				}
			}
			out << res;
			cont++;

			#ifdef DEBUG_VERBOSE
				printf("frame sent:\n");
				for (int cpi=0; cpi<CPI; cpi++) {
					printf("  cpi %d:\n", cpi);
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[0].pixel[cpi]), float(res.pixel[1].pixel[cpi]), float(res.pixel[2].pixel[cpi]), float(res.pixel[3].pixel[cpi]));
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[4].pixel[cpi]), float(res.pixel[5].pixel[cpi]), float(res.pixel[6].pixel[cpi]), float(res.pixel[7].pixel[cpi]));
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[8].pixel[cpi]), float(res.pixel[9].pixel[cpi]), float(res.pixel[10].pixel[cpi]), float(res.pixel[11].pixel[cpi]));
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[12].pixel[cpi]), float(res.pixel[13].pixel[cpi]), float(res.pixel[14].pixel[cpi]), float(res.pixel[15].pixel[cpi]));

				}
			#endif

		}

	}
	
	#ifdef DEBUG_VERBOSE
	printf("mulDATA: ends %d\n", cont);
	#endif
}

// ------------------------------------------------------------------------------------------------------
// mulKernels: This function performs the multiplication of the input kernels with two matrix 
// and sends the produced results in a frame_d format (4, 4, CPI).
// However, these matrix are not loaded, the multiplication it is done directly assigning the combination
// of input data positions to the right exit position.
// Arguments:
//   H     : Height of the input channel
//   W     : Width of the input channel
//   I_ITER: Number of input iterations (I / CPI)
//   k_in  : input stream with kernels
//   out   : output stream
//
static void mulKernels(int I_ITER, hls::stream<kernel_t> &k_in, hls::stream<frame_d> &out) {

	#ifdef DEBUG_VERBOSE
	printf("mul: start\n");
	#endif

	kernel_t kernel;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel dim=0)
	frame_d res;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=res dim=0)

	data_type Gg[CPI][4][3];
	data_type GgGT[CPI][4][4];
	DO_PRAGMA(HLS ARRAY_PARTITION variable=Gg dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=GgGT dim=0)

	// Reading the kernels
	mul_i_iter_loop:
	for(int i_iter = 0; i_iter < I_ITER; i_iter++){
		DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)
		kernel = k_in.read();
		loop_mul_kernels_load_cpo:
		for (int cpo=0; cpo<CPO; cpo++) {
			#pragma HLS PIPELINE II=1

			for(int i = 0; i<3; i++){
				#pragma HLS UNROLL
				for (int cpi=0; cpi < CPI; cpi++) {
					Gg[cpi][0][i] = kernel.pixel[cpo][cpi][i];
					Gg[cpi][1][i] = (kernel.pixel[cpo][cpi][i] / 2) + (kernel.pixel[cpo][cpi][i+KW]  / 2) + (kernel.pixel[cpo][cpi][i+6] / 2);
					Gg[cpi][2][i] = (kernel.pixel[cpo][cpi][i] / 2) - (kernel.pixel[cpo][cpi][i+KW]  / 2) + (kernel.pixel[cpo][cpi][i+6] / 2);
					Gg[cpi][3][i] = kernel.pixel[cpo][cpi][i+6];
				}
			}

			for(int i = 0; i<4; i++){
				#pragma HLS UNROLL
				for (int cpi=0; cpi < CPI; cpi++) {
					GgGT[cpi][i][0] = Gg[cpi][i][0];
					GgGT[cpi][i][1] = (Gg[cpi][i][0]  / 2) + (Gg[cpi][i][1]  / 2) + (Gg[cpi][i][2] / 2);
					GgGT[cpi][i][2] = (Gg[cpi][i][0]  / 2) - (Gg[cpi][i][1]  / 2) + (Gg[cpi][i][2] / 2);
					GgGT[cpi][i][3] = Gg[cpi][i][2];
				}
			}
			int pos = 0;
			for(int f = 0; f < 4; f++){
				#pragma HLS UNROLL
				for(int c = 0; c < 4; c++){
					for (int cpi=0; cpi < CPI; cpi++) {
						res.pixel[pos].pixel[cpi] = GgGT[cpi][f][c];
					}
					pos++;
				}
			}

			  out << res;
			  #ifdef DEBUG_VERBOSE
				  printf("frame sent:\n");
				  for (int cpi=0; cpi<CPI; cpi++) {
					printf("  cpi %d:\n", cpi);
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[0].pixel[cpi]), float(res.pixel[1].pixel[cpi]), float(res.pixel[2].pixel[cpi]), float(res.pixel[3].pixel[cpi]));
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[4].pixel[cpi]), float(res.pixel[5].pixel[cpi]), float(res.pixel[6].pixel[cpi]), float(res.pixel[7].pixel[cpi]));
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[8].pixel[cpi]), float(res.pixel[9].pixel[cpi]), float(res.pixel[10].pixel[cpi]), float(res.pixel[11].pixel[cpi]));
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[12].pixel[cpi]), float(res.pixel[13].pixel[cpi]), float(res.pixel[14].pixel[cpi]), float(res.pixel[15].pixel[cpi]));

				  }
			  #endif

		}
	} //i_iter

	#ifdef DEBUG_VERBOSE
	printf("mul: end\n");
	#endif
}


// ------------------------------------------------------------------------------------------------------
// mulWise: This function performs the element wise multiplication between kernel frames in a frame_d format (4, 4, CPI)
// and data frames in a frame_d_2 format (4, 4, CPI/2).
// Arguments:
//   H     : Height of the input channel
//   W     : Width of the input channel
//   I_ITER: Number of input iterations (I / CPI)
//   k_in  : input stream with kernels frames
//   d_in  : input stream with data frames
//   out   : output stream
//
static void mulWise(int H, int W, int I_ITER, hls::stream<frame_d_2> &d_in, hls::stream<frame_d> &k_in, hls::stream<frame_d> &out) {
	
	#ifdef DEBUG_VERBOSE
	printf("mulwise: starts\n");
	#endif
	
	frame_d kernel[CPO];
	DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel dim=0 complete)
	frame_d res;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=res dim=0 complete)
	frame_d_2 data;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=data dim=0 complete)


	for(int i = 0; i<16; i++){
		#pragma HLS PIPELINE II=1
		for (int cpi=0; cpi<CPI; cpi++) {
			#pragma HLS UNROLL
			res.pixel[i].pixel[cpi] = 0.f;
		}
	}
	
	int cpif = 0;
	mul_i_iter_loop:
	for(int i_iter = 0; i_iter < I_ITER; i_iter++){
		DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE/CPI)
		DO_PRAGMA(HLS LOOP_FLATTEN off)

		int even = 0;
		
		// Reading the kernels
		loop_mul_kernels_load_cpo_kernels_wise:
		for (int cpo=0; cpo<CPO; cpo++) {
			#pragma HLS PIPELINE II=1
			kernel[cpo] = k_in.read();
		}
		
		// Reading data
		loop_mul_kernels_load_cpo_data_wise:
		for (int i = 0; i < (H * W) / 2; i++) {
			DO_PRAGMA(HLS loop_tripcount min=1 max=(H_REFERENCE/2)*W_REFERENCE)
			#pragma HLS PIPELINE
			DO_PRAGMA(HLS dependence variable=res inter false)

			data = d_in.read();
			for (int cpo=0; cpo<CPO; cpo++) {
				#pragma HLS UNROLL
				for(int pos = 0; pos < 16; pos++){
					#pragma HLS UNROLL
					for (int cpi=0; cpi < CPI/2; cpi++) {
						#pragma HLS UNROLL
						if(!even){
						  cpif=cpi;
						}else{
						  cpif=cpi+CPI/2;
						}
						res.pixel[pos].pixel[cpo] += data.pixel[pos].pixel[cpi] * kernel[cpo].pixel[pos].pixel[cpif];
					}
				}
			}
			if(even){
				out << res;
				for(int i = 0; i<16; i++){
					#pragma HLS UNROLL
					for (int cpo=0; cpo<CPO; cpo++) {
						#pragma HLS UNROLL
						res.pixel[i].pixel[cpo] = 0.f;
					}
				}
			}
			even=(even+1)%2;
		}
	}
	#ifdef DEBUG_VERBOSE
	printf("mulwise: end\n");
	#endif
}

// -------------------------------------------------------------------------------------------------------------------
// mult_A_AT: This function performs the multiplication of the final data that has been multiplied in multWise module
// in a frame_d format (4, 4, CPI) with two matrix and sends the produced results in a frame_m format (2, 2, CPI).
// However, these matrix are not loaded, the multiplication it is done directly assigning the combination
// of input data positions to the right exit position.
// Arguments:
//   H     : Height of the input channel
//   W     : Width of the input channel
//   I_ITER: Number of input iterations (I / CPI)
//   d_in  : input stream with data frames
//   out   : output stream
//
static void mult_A_AT(int H, int W, int I_ITER, hls::stream<frame_d> &d_in, hls::stream<frame_winograd> &out) {

	#ifdef DEBUG_VERBOSE
	printf("mult_A_AT: starts\n");
	#endif
	frame_d data;
	frame_winograd res;
	
	data_type ATxWise_mult[CPI][2][4];
	data_type resMULT[CPI][2][2];
	DO_PRAGMA(HLS ARRAY_PARTITION variable=ATxWise_mult dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=resMULT dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=data dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=res dim=0)
	
	// Reading data
	loop_mul_kernels_load_cpo_A_AT:
	for (int i_iter = 0; i_iter < I_ITER; i_iter++) {
		DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE/CPI)
		for (int p = 0; p < (H/2 * W/2); p++) {
			DO_PRAGMA(HLS loop_tripcount min=1 max=(H_REFERENCE/2) * (W_REFERENCE/2))

			#pragma HLS PIPELINE II=1
			data = d_in.read();

			for(int i = 0; i<4; i++){
				#pragma HLS UNROLL
				for (int cpi=0; cpi < CPI; cpi++) {
					ATxWise_mult[cpi][0][i] = data.pixel[i].pixel[cpi] + data.pixel[i+4].pixel[cpi]+ data.pixel[i+8].pixel[cpi];
					ATxWise_mult[cpi][1][i] = data.pixel[i+4].pixel[cpi] - data.pixel[i+8].pixel[cpi]- data.pixel[i+12].pixel[cpi];
				}
			}

			for(int i = 0; i<2; i++){
				#pragma HLS UNROLL
				for (int cpi=0; cpi < CPI; cpi++) {
					resMULT[cpi][i][0] = ATxWise_mult[cpi][i][0] + ATxWise_mult[cpi][i][1] + ATxWise_mult[cpi][i][2];
					resMULT[cpi][i][1] = ATxWise_mult[cpi][i][1] - ATxWise_mult[cpi][i][2] - ATxWise_mult[cpi][i][3];
				}
			}
			int pos = 0;
			for(int f = 0; f < 2; f++){
				#pragma HLS UNROLL
				for(int c = 0; c < 2; c++){
					for (int cpi=0; cpi < CPI; cpi++) {
						res.pixel[pos].pixel[cpi] = resMULT[cpi][f][c];
					}
					pos++;
				}
			}

		   out << res;

		}
	}
	#ifdef DEBUG_VERBOSE
	printf("mult_A_AT: ends\n");
	#endif
}

// -------------------------------------------------------------------------------
// add_winograd: This function performs the addition of all subpixels for the same channel. To do that
// it receives several frame_m frames in format (2, 2, CPI) and add these values to its correct pixel result.
// It adds also the corresponding bias.
//
// Arguments:
//   H     : Height of input channel
//   W     : Width of input channel
//   I_ITER: Number of input iterations (I / CPI)
//   in    : input streams data
//   b_in  : input stream bias
//   out   : output stream
//
static void add_winograd(int H, int W, int I_ITER, hls::stream<pixel_out_t> &b_in, hls::stream<frame_winograd> &in, hls::stream<pixel_out_t> &out) {

	#ifdef DEBUG_VERBOSE
	printf("add: start\n");
	#endif

	pixel_out_t bias;
	pixel_out_t p0;
	pixel_out_t p1;
	pixel_out_t p2;
	pixel_out_t p3;
	pixel_out_t buff_o_channels_0[HMAX/2][WMAX/2];
	pixel_out_t buff_o_channels_1[HMAX/2][WMAX/2];
	pixel_out_t buff_o_channels_2[HMAX/2][WMAX/2];
	pixel_out_t buff_o_channels_3[HMAX/2][WMAX/2];
	frame_winograd data;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=bias dim=0 complete)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=p0 complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=p1 complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=p2 complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=p3 complete dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=buff_o_channels_0 dim=3 complete)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=buff_o_channels_1 dim=3 complete)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=buff_o_channels_2 dim=3 complete)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=buff_o_channels_3 dim=3 complete)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=data dim=0 complete)

	// We read the bias
	bias = b_in.read();

	int h = 0;
	int w = 0;
	int hh, ww, pp;
	int pixels = H * W;

	// All input data have effect into output add
	add_winograd_loop_iter:
	for (int i_iter = 0; i_iter < I_ITER; i_iter++){
		DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE/CPI)

		add_winograd_loop_hw:
		for (int p = 0; p<pixels; p=p+1) {
			DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE * W_REFERENCE)
			DO_PRAGMA(HLS pipeline)
			DO_PRAGMA(HLS dependence variable=buff_o_channels_0 inter false)
			DO_PRAGMA(HLS dependence variable=buff_o_channels_1 inter false)
			DO_PRAGMA(HLS dependence variable=buff_o_channels_2 inter false)
			DO_PRAGMA(HLS dependence variable=buff_o_channels_3 inter false)

  		    ww = w / 2;
		  	hh = h / 2;
			if (((h % 2) == 0) && ((w % 2) == 0)) pp = 0;
			if (((h % 2) == 0) && ((w % 2) == 1)) pp = 1;
			if (((h % 2) == 1) && ((w % 2) == 0)) pp = 2;
			if (((h % 2) == 1) && ((w % 2) == 1)) pp = 3;

			// read four pixels from bias or from buffer
			if ((pp==0) && (i_iter == 0)) {
				p0 = bias;
				p1 = bias;
				p2 = bias;
				p3 = bias;
			} else {
				p0 = buff_o_channels_0[hh][ww];
				p1 = buff_o_channels_1[hh][ww];
				p2 = buff_o_channels_2[hh][ww];
				p3 = buff_o_channels_3[hh][ww];
			}

			if (pp == 0) {

				// read four input pixels
				data = in.read();

				// add operation and writing to the buffer
    			add_winograd_loop_cpo:
			    for (int cpo=0; cpo < CPO; cpo++) {
				  #pragma HLS UNROLL

				  p0.pixel[cpo] += data.pixel[0].pixel[cpo];
				  p1.pixel[cpo] += data.pixel[1].pixel[cpo];
				  p2.pixel[cpo] += data.pixel[2].pixel[cpo];
				  p3.pixel[cpo] += data.pixel[3].pixel[cpo];

				  buff_o_channels_0[hh][ww] = p0;
				  buff_o_channels_1[hh][ww] = p1;
				  buff_o_channels_2[hh][ww] = p2;
				  buff_o_channels_3[hh][ww] = p3;
			    }
			}

			// send pixels out
			if(i_iter == (I_ITER-1)){
			  pixel_out_t px;
			  if (pp == 0) px = p0;
			  if (pp == 1) px = p1;
			  if (pp == 2) px = p2;
			  if (pp == 3) px = p3;
			  out << px;
			}

			// pointers (h, w) update
			w = w + 1;
			if (w == W) {
				w = 0;
				h = h + 1;
				if (h == H) h = 0;
			}

		}

	}

	#ifdef DEBUG_VERBOSE
	printf("add: end\n");
	#endif
}


// -------------------------------------------------------------------------------
// winograd_conv: Convolutional kernel
//
// Arguments:
//   H                    : Height of the input channel
//   W                    : Width of the input channel
//   I_ITER               : Number of input iterations (I / CPI)
//   enable_upper_padding : Enables building the upper padding row
//   enable_lower_padding : Enables building the lower padding row
//   in                   : input data stream
//   k_in                 : input kernel stream
//   b_in                 : input bias stream
//   out                  : output data stream
//
// This module builds the winograd convolutional operation by instantiating streams and
// building the dataflow model with the corresponding modules
//
void winograd_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out) {

	// streams
	static hls::stream<pixel_in_t>      str_pad_cvt;  	 	// padding 	-> 	cvt
	static hls::stream<frame_d>   	    cvt_frameConvert; 	// cvt 		-> 	frameConvert
	static hls::stream<frame_d_2>       str_cvt_mul_cTc;    // cvt 		-> 	mulData
	static hls::stream<frame_d>         kernels_multWise;  	// mulKernels -> mulWise
	static hls::stream<frame_d_2>       mult_data_res;  	// mulData 	-> 	mult wise
	static hls::stream<frame_d>         mult_wise_res;  	// mulWise 	-> 	mult_A_AT
	static hls::stream<frame_winograd> 	str_mul_add;  		// mult_A_AT -> add_winograd
	DO_PRAGMA(HLS stream variable=str_pad_cvt      depth=STREAMS_DEPTH)
	DO_PRAGMA(HLS stream variable=cvt_frameConvert depth=STREAMS_DEPTH)
	DO_PRAGMA(HLS stream variable=str_cvt_mul_cTc  depth=STREAMS_DEPTH)
	DO_PRAGMA(HLS stream variable=kernels_multWise depth=STREAMS_DEPTH)
	DO_PRAGMA(HLS stream variable=mult_data_res    depth=STREAMS_DEPTH)
	DO_PRAGMA(HLS stream variable=mult_wise_res    depth=STREAMS_DEPTH)
	DO_PRAGMA(HLS stream variable=str_mul_add      depth=STREAMS_DEPTH)

	// topology
	#pragma HLS dataflow
	padding(H, W, I_ITER, enable_upper_padding, enable_lower_padding, in, str_pad_cvt); 
	cvt_winograd(H, W, I_ITER, str_pad_cvt, cvt_frameConvert);		
	frameConvert(H, W, I_ITER, cvt_frameConvert, str_cvt_mul_cTc);
	mulKernels(I_ITER, k_in, kernels_multWise);
	mulData(H, W, I_ITER, str_cvt_mul_cTc, mult_data_res);
	mulWise(H, W, I_ITER, mult_data_res, kernels_multWise, mult_wise_res);
	mult_A_AT(H, W, I_ITER, mult_wise_res, str_mul_add);
	add_winograd(H, W, I_ITER, b_in, str_mul_add, out);         
}

