#include "conv2D.h"

// ---------------------------------------------------------------------------------------------------
// cvt_winograd: reads an input stream with an image of format (H, W, CPI) and writes an output stream
// in a 2D format based on (KW, KH). (SW=1, SH=1) stride is assumed and (PW=1, PH=1) padding is assumed.
// The function outputs data in the format (KH, KW, CPI).
//
// Arguments:
//   H      : Height of input channel
//   W      : Width of input channel
//   I_ITER : Number of input iterations (I / CPI)
//   in     : input stream (format pixel_in_t)
//   out    : output stream (format frame_d_2)
//
static void cvt_winograd(int H, int W, int I_ITER, hls::stream<pixel_in_t> &in, hls::stream<frame_d_2> &out) {

  #ifdef DEBUG_VERBOSE
  printf("cvt: start\n");
  #endif

  cvt_i_iter_loop:
  for(int i_iter = 0; i_iter < I_ITER; i_iter++){
	DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE/CPI)
	printf("ITERACION %d:\n", i_iter);
    // Now we process the input data and convert the data into frames
    // buffers (keep three rows) , necesitamos 3 filas para leer un trozo de 3x3, aqui se guardan las filas
    pixel_in_t buffer0[WMAX+2];
    pixel_in_t buffer1[WMAX+2];
    pixel_in_t buffer2[WMAX+2];
	pixel_in_t buffer3[WMAX+2];
    DO_PRAGMA(HLS ARRAY_PARTITION variable=buffer0 cyclic dim=1 factor=CPI)
    DO_PRAGMA(HLS ARRAY_PARTITION variable=buffer1 cyclic dim=1 factor=CPI)
    DO_PRAGMA(HLS ARRAY_PARTITION variable=buffer2 cyclic dim=1 factor=CPI)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=buffer3 cyclic dim=1 factor=CPI)

    // frame
    frame_d_2 frame;
    DO_PRAGMA(HLS ARRAY_PARTITION variable=frame dim=0)

    // We loop for every incoming pixel
    cvt_loop_1:
    for (int pin_row=0; pin_row < H+2; pin_row++) {
      DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE+2)
      cvt_loop_2:
      for (int pin_col=0; pin_col < W+2; pin_col++) {
    	  DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE+2)
        // get the pixel
        pixel_in_t pixel;
        pixel = in.read();
        // row buffer write (in which buffer row we write the pixel) -> en que buffer se escribe
        int row0_buffer_write = (pin_row % 4) == 0; // 0 4 8 -> bufffer0
        int row1_buffer_write = (pin_row % 4) == 1; // 1 4 7 -> buffer 1
		int row2_buffer_write = (pin_row % 4) == 2; //2 5 8 -> buffer 2

        // first row buffer -> decide la posicion
        int row0 = (pin_row <= 3) | ((pin_row % 4) == 3); // pin row = 0  1 2 5
        int row1 = !row0 & ((pin_row % 4) == 0); //  pinr row = 3
		int row2 = !row1 & ((pin_row % 4) == 1);
        // we write the pixel into the buffer
        if (row0_buffer_write) buffer0[pin_col] = pixel; else if (row1_buffer_write) buffer1[pin_col] = pixel;
		else if (row2_buffer_write) buffer2[pin_col] = pixel; else buffer3[pin_col] = pixel;
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
        // p0, p1, p2, p3
        if (shift_frame) {p0 = p1;} else if (pin_col==0) {if (row0) p0 = pixel_b0; else if (row1) p0 = pixel_b1; else if (row2) p0 = pixel_b2; else p0 = pixel_b3;}
        if (shift_frame) {p1 = p2;} else if (pin_col==1) {if (row0) p1 = pixel_b0; else if (row1) p1 = pixel_b1; else if (row2) p1 = pixel_b2; else p1 = pixel_b3;}
		if (shift_frame) {p2 = p3;} else if (pin_col==2) {if (row0) p2 = pixel_b0; else if (row1) p2 = pixel_b1; else if (row2) p2 = pixel_b2; else p2 = pixel_b3;}
        if (row0) p3 = pixel_b0; else if (row1) p3 = pixel_b1; else if (row2) p3 = pixel_b2; else p3 = pixel_b3;
        // p4, p5, p6, p7
        if (shift_frame) {p4 = p5;} else if (pin_col==0) {if (row0) p4 = pixel_b1; else if (row1) p4 = pixel_b2; else if (row2) p4 = pixel_b3; else p4 = pixel_b0;}
        if (shift_frame) {p5 = p6;} else if (pin_col==1) {if (row0) p5 = pixel_b1; else if (row1) p5 = pixel_b2; else if (row2) p5 = pixel_b3; else p5 = pixel_b0;}
		if (shift_frame) {p6 = p7;} else if (pin_col==2) {if (row0) p6 = pixel_b1; else if (row1) p6 = pixel_b2; else if (row2) p6 = pixel_b3; else p6 = pixel_b0;}
        if (row0) p7 = pixel_b1; else if (row1) p7 = pixel_b2; else if (row2) p7 = pixel_b3; else p7 = pixel_b0;
        // p8, p9, p10, p11
        if (shift_frame) {p8 = p9;} else if (pin_col==0) {if (row0) p8 = pixel_b2; else if (row1) p8 = pixel_b3; else if (row2) p8 = pixel_b0; else p8 = pixel_b1;}
        if (shift_frame) {p9 = p10;} else if (pin_col==1) {if (row0) p9 = pixel_b2; else if (row1) p9 = pixel_b3; else if (row2) p9 = pixel_b0; else p8 = pixel_b1;}
		if (shift_frame) {p10 = p11;} else if (pin_col==2) {if (row0) p10 = pixel_b2; else if (row1) p10 = pixel_b3; else if (row2) p10 = pixel_b0; else p10 = pixel_b0;}
        if (row0) p11 = pixel_b2; else if (row1) p11 = pixel_b3; else if (row2) p11 = pixel_b0; else p11 = pixel_b1;
		// p12, p13, p14, p15
        if (shift_frame) {p12 = p13;} else if (pin_col==0) {if (row0) p12 = pixel_b3; else if (row1) p12 = pixel_b0; else if (row2) p12 = pixel_b1; else p12 = pixel_b2;}
        if (shift_frame) {p13 = p14;} else if (pin_col==1) {if (row0) p13 = pixel_b3; else if (row1) p13 = pixel_b0; else if (row2) p13 = pixel_b1; else p13 = pixel_b2;}
		if (shift_frame) {p14 = p15;} else if (pin_col==2) {if (row0) p14 = pixel_b3; else if (row1) p14 = pixel_b0; else if (row2) p14 = pixel_b1; else p14 = pixel_b2;}
        if (row0) p15 = pixel_b3; else if (row1) p15 = pixel_b0; else if (row2) p15 = pixel_b1; else p15 = pixel_b2;

		//añadir otro paquete

        if (send_frame) {
			int pos = 0;
			for(int cpi = 0; cpi < CPI; cpi++){
			  frame.pixel[0].pixel[pos] = p0.pixel[cpi]; frame.pixel[1].pixel[pos] = p1.pixel[cpi]; frame.pixel[2].pixel[pos] = p2.pixel[cpi]; frame.pixel[3].pixel[pos] = p3.pixel[cpi];
			  frame.pixel[4].pixel[pos] = p4.pixel[cpi]; frame.pixel[5].pixel[pos] = p5.pixel[cpi]; frame.pixel[6].pixel[pos] = p6.pixel[cpi]; frame.pixel[7].pixel[pos] = p7.pixel[cpi];
			  frame.pixel[8].pixel[pos] = p8.pixel[cpi]; frame.pixel[9].pixel[pos] = p9.pixel[cpi]; frame.pixel[10].pixel[pos] = p10.pixel[cpi]; frame.pixel[11].pixel[pos] = p11.pixel[cpi];
			  frame.pixel[12].pixel[pos] = p12.pixel[cpi]; frame.pixel[13].pixel[pos] = p13.pixel[cpi]; frame.pixel[14].pixel[pos] = p14.pixel[cpi]; frame.pixel[15].pixel[pos] = p15.pixel[cpi];

			  if(pos == CPI/2 - 1){
				 out << frame;
				 pos=0;
				 #ifdef DEBUG_VERBOSE
				  printf("cvt: frame sent:\n");
				  for (int cpi=0; cpi<CPI/2; cpi++) {
					printf("  cpi %d:\n", cpi);
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(frame.pixel[0].pixel[cpi]), float(frame.pixel[1].pixel[cpi]), float(frame.pixel[2].pixel[cpi]), float(frame.pixel[3].pixel[cpi]));
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(frame.pixel[4].pixel[cpi]), float(frame.pixel[5].pixel[cpi]), float(frame.pixel[6].pixel[cpi]), float(frame.pixel[7].pixel[cpi]));
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(frame.pixel[8].pixel[cpi]), float(frame.pixel[9].pixel[cpi]), float(frame.pixel[10].pixel[cpi]), float(frame.pixel[11].pixel[cpi]));
					printf("    %6.4f %6.4f %6.4f %6.4f\n", float(frame.pixel[12].pixel[cpi]), float(frame.pixel[13].pixel[cpi]), float(frame.pixel[14].pixel[cpi]),  float(frame.pixel[15].pixel[cpi]));
				  }
				 #endif
			 }else{
				pos++;
			 }
			}
        }
      }
    }
  } //i_iter

  #ifdef DEBUG_VERBOSE
  printf("cvt: end\n");
  #endif
}

// ----------------------------------------------------------------------------------------
// mulData: This function performs the multiplication of an input frame with the stored kernels
// and sends the produced pixels. Before normal operation it receives its kernels
// Arguments:
//   H     : Height of the input channel
//   W     : Width of the input channel
//   I_ITER: Number of input iterations (I / CPI)
//   in    : input stream with incoming data frames
//   k_in  : input stream with kernels
//   out   : output stream
// mulKernels(H, W, I_ITER, k_in, kernels_multWise);
static void mulData(int H, int W, int I_ITER, hls::stream<frame_d_2> &d_in, hls::stream<frame_d_2> &out) {

	//printf("mulData: start\n");
	frame_d_2 data;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=data dim=0)
	frame_d_2 res;
	DO_PRAGMA(HLS ARRAY_PARTITION variable=res dim=0)
	data_type CTd[CPI][4][4];
	data_type CTdC[CPI][4][4];
	DO_PRAGMA(HLS ARRAY_PARTITION variable=CTd dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=CTdC dim=0)

	//#pragma HLS allocation instances=sub_Mul_data limit=50 function
	//#pragma HLS allocation instances=fadd limit=50 operation
	int cont = 0;
	for( int i_iter = 0; i_iter < I_ITER; i_iter++){
		DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE/CPI)
		 #pragma HLS loop_flatten off
		for (int i = 0; i < (H/2 * W); i++){
			DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE/2 * W_REFERENCE)
			#pragma HLS PIPELINE II=1
			data = d_in.read();
		    //multiplicamos Ct x data
			for(int i = 0; i<4; i++){//columnas de CTd
				#pragma HLS UNROLL
				for (int cpi=0; cpi < CPI/2; cpi++) {
					CTd[cpi][0][i] = data.pixel[i].pixel[cpi] - data.pixel[i+8].pixel[cpi]; //canviar las multiplicacionde los indices por su valor
					CTd[cpi][1][i] = data.pixel[i+4].pixel[cpi] + data.pixel[i+8].pixel[cpi];
					CTd[cpi][2][i] = data.pixel[i+8].pixel[cpi] - data.pixel[i+4].pixel[cpi];
					CTd[cpi][3][i] = data.pixel[i+4].pixel[cpi] - data.pixel[i+12].pixel[cpi];
				}
			}

			//calculamos CtdxC (4x4)
			for(int i = 0; i<4; i++){//columnas de CTdC
				#pragma HLS UNROLL
				for (int cpi=0; cpi < CPI/2; cpi++) {
					CTdC[cpi][i][0] = CTd[cpi][i][0] - CTd[cpi][i][2];
					CTdC[cpi][i][1] = CTd[cpi][i][1] + CTd[cpi][i][2];
					CTdC[cpi][i][2] = CTd[cpi][i][2] - CTd[cpi][i][1];
					CTdC[cpi][i][3] = CTd[cpi][i][1] - CTd[cpi][i][3];
				}
			}

			int pos = 0;
			for(int f = 0; f < 4; f++){//canviar las multiplicacionde los indices por su valor
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

//printf("mulDATA: ends %d\n", cont);
}

// ----------------------------------------------------------------------------------------
// mul: This function performs the multiplication of an input frame with the stored kernels
// and sends the produced pixels. Before normal operation it receives its kernels
// Arguments:
//   H     : Height of the input channel
//   W     : Width of the input channel
//   I_ITER: Number of input iterations (I / CPI)
//   in    : input stream with incoming data frames
//   k_in  : input stream with kernels
//   out   : output stream
// mulKernels(H, W, I_ITER, k_in, kernels_multWise);
static void mulKernels(int H, int W, int I_ITER, hls::stream<kernel_t> &k_in, hls::stream<frame_d> &out) {

  #ifdef DEBUG_VERBOSE
  printf("mul: start\n");
  #endif

  kernel_t kernel;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel dim=0)
  frame_d res;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=res dim=0)

    // now we read frames and produce the pixels
    data_type Gg[CPI][4][3];
	data_type GgGT[CPI][4][4];
	DO_PRAGMA(HLS ARRAY_PARTITION variable=Gg dim=0)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=GgGT dim=0)

  // Reading the kernels
  mul_i_iter_loop:
  for(int i_iter = 0; i_iter < I_ITER; i_iter++){
	DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE/CPI)
	printf("ITERACION %d:\n", i_iter);
    loop_mul_kernels_load_cpo:
    for (int cpo=0; cpo<CPO; cpo++) {
		#pragma HLS PIPELINE II=1
		kernel = k_in.read();


		#ifdef DEBUG_VERBOSE
		printf("mul: kernels received\n");
		for (int cpo=0; cpo < CPO; cpo++) {
		  for (int cpi=0; cpi < CPI; cpi++) {
			printf("  cpi=%d, cpo=%d:\n", cpi, cpo);
			printf("    %6.4f %6.4f %6.4f\n", float(kernel[cpo].pixel[0].pixel[cpi]), float(kernel[cpo].pixel[1].pixel[cpi]), float(kernel[cpo].pixel[2].pixel[cpi]));
			printf("    %6.4f %6.4f %6.4f\n", float(kernel[cpo].pixel[3].pixel[cpi]), float(kernel[cpo].pixel[4].pixel[cpi]), float(kernel[cpo].pixel[5].pixel[cpi]));
			printf("    %6.4f %6.4f %6.4f\n", float(kernel[cpo].pixel[6].pixel[cpi]), float(kernel[cpo].pixel[7].pixel[cpi]), float(kernel[cpo].pixel[8].pixel[cpi]));
		  }
		}
		#endif

		//cada cpo lo forman 9 pixel, y cada pixel contiene 4 elementos -> total 4 kernels x CPO

		//multiplicamos G x kenels(g)
		for(int i = 0; i<3; i++){//columnas de G
			#pragma HLS UNROLL
			for (int cpi=0; cpi < CPI; cpi++) {
//				Gg[cpi][0][i] = kernel.pixel[i].pixel[cpi];
//				Gg[cpi][1][i] = (kernel.pixel[i].pixel[cpi] / 2) + (kernel.pixel[i+KW].pixel[cpi]  / 2) + (kernel.pixel[i+6].pixel[cpi] / 2);
//				Gg[cpi][2][i] = (kernel.pixel[i].pixel[cpi] / 2) - (kernel.pixel[i+KW].pixel[cpi]  / 2) + (kernel.pixel[i+6].pixel[cpi] / 2);
//				Gg[cpi][3][i] = kernel.pixel[i+6].pixel[cpi];

                Gg[cpi][0][i] = kernel.pixel[cpo][cpi][i];
                Gg[cpi][1][i] = (kernel.pixel[cpo][cpi][i] / 2) + (kernel.pixel[cpo][cpi][i+KW]  / 2) + (kernel.pixel[cpo][cpi][i+6] / 2);
                Gg[cpi][2][i] = (kernel.pixel[cpo][cpi][i] / 2) - (kernel.pixel[cpo][cpi][i+KW]  / 2) + (kernel.pixel[cpo][cpi][i+6] / 2);
                Gg[cpi][3][i] = kernel.pixel[cpo][cpi][i+6];
			}
		}

		 //calculamos GgxGT (4x4)
		for(int i = 0; i<4; i++){//columnas de G
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



//mulWise(H, W, I_ITER, mult_data_res, kernels_multWise, mult_wise_res);
static void mulWise(int H, int W, int I_ITER, hls::stream<frame_d_2> &d_in, hls::stream<frame_d> &k_in, hls::stream<frame_d> &out) {
  //printf("mulwise: starts\n");
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
	int even = 0;
	// Reading the kernels
	#pragma HLS PIPELINE II=1
	loop_mul_kernels_load_cpo_kernels_wise:
    for (int cpo=0; cpo<CPO; cpo++) {
	  #pragma HLS PIPELINE II=1
      kernel[cpo] = k_in.read();
	}
	   // Reading data
	loop_mul_kernels_load_cpo_data_wise:
    for (int i = 0; i < (H/2 * W/2 * 2); i++) {
      DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE/2*W_REFERENCE)
	  #pragma HLS UNROLL
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
				res.pixel[pos].pixel[cpo] += data.pixel[pos].pixel[cpi] * kernel[cpo].pixel[pos].pixel[cpif];//inicializar a 0 res
			  }
		  }

		}
		if(even){
	      out << res;
/* 		  printf("frame sent:\n");
          for (int cpi=0; cpi<CPO; cpi++) {
            printf("  cpi %d:\n", cpi);
			printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[0].pixel[cpi]), float(res.pixel[1].pixel[cpi]), float(res.pixel[2].pixel[cpi]), float(res.pixel[3].pixel[cpi]));
            printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[4].pixel[cpi]), float(res.pixel[5].pixel[cpi]), float(res.pixel[6].pixel[cpi]), float(res.pixel[7].pixel[cpi]));
            printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[8].pixel[cpi]), float(res.pixel[9].pixel[cpi]), float(res.pixel[10].pixel[cpi]), float(res.pixel[11].pixel[cpi]));
			printf("    %6.4f %6.4f %6.4f %6.4f\n", float(res.pixel[12].pixel[cpi]), float(res.pixel[13].pixel[cpi]), float(res.pixel[14].pixel[cpi]), float(res.pixel[15].pixel[cpi]));

          } */
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
  //printf("mulwise: end\n");
}



static void mult_A_AT(int H, int W, int I_ITER, hls::stream<frame_d> &d_in, hls::stream<frame_m> &out) {

  frame_d data;
  frame_m res;
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
			DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE/2)

			#pragma HLS PIPELINE II=1
			data = d_in.read();
			if(d_in.empty()){
				printf("mult_A_AT: vacio\n");
			}

		   //multiplicamos AT x data
			for(int i = 0; i<4; i++){//columnas de AT
				#pragma HLS UNROLL
				for (int cpi=0; cpi < CPI; cpi++) {
					ATxWise_mult[cpi][0][i] = data.pixel[i].pixel[cpi] + data.pixel[i+4].pixel[cpi]+ data.pixel[i+8].pixel[cpi];
					ATxWise_mult[cpi][1][i] = data.pixel[i+4].pixel[cpi] - data.pixel[i+8].pixel[cpi]- data.pixel[i+12].pixel[cpi];
				}
			}


			//calculamos ATData x A
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
}

// -------------------------------------------------------------------------------
// add: This function performs the addition of all subpixels for the same channel.
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
static void add_winograd(int H, int W, int I_ITER, hls::stream<pixel_out_t> &b_in, hls::stream<frame_m> &in, hls::stream<pixel_out_t> &out) {

  #ifdef DEBUG_VERBOSE
  printf("add: start\n");
  #endif

  data_type bias[CPO];
   DO_PRAGMA(HLS ARRAY_PARTITION variable=bias dim=0 complete)

  // number of iterations by CPI || CPO channels
  int num_iterations = W * H;

  // Buffer for all data and CPO channels
  //pixel_out_t buff_o_channels[WHMAX];
  pixel_out_t buff_o_channels[HMAX][WMAX];
  DO_PRAGMA(HLS ARRAY_PARTITION variable=buff_o_channels dim=1 cyclic factor=2) //acceder a la vez a la pos 0 de WHMAX de cada cpo, (0,20), (1,20),  (2,20) y (3,20)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=buff_o_channels dim=2 cyclic factor=2)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=buff_o_channels dim=3 complete)
  // We receive bias in packs of CPO
  pixel_out_t p_out;
  p_out = b_in.read();
  add_load_bias_loop:
  for (int b=0; b<CPO; b++) {
    #pragma HLS PIPELINE II=1
    bias[b] = p_out.pixel[b];
  }

  #ifdef DEBUG_VERBOSE
  for (int b=0; b<CPO; b++) {
    printf("Bias[%d] = %6.4f \n", b, float(bias[b]));
  }
  printf("add: bias received\n");
  for(int cpo = 0; cpo<CPO; cpo++){
    printf("Channel cpo = %d: ", cpo);
    for(int it = 0; it<num_iterations; it++){
      printf("%6.2f ", float(buff_o_channels[it]));
    }
    printf("\n");
  }
  #endif

	add_copy_bias_loop:
	for (int h = 0; h < H; h++) {
		DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE)
		for (int w=0; w < W; w++) {
			DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
			#pragma HLS PIPELINE II=1
			for (int cpo=0; cpo < CPO; cpo++) {
				#pragma HLS UNROLL
				buff_o_channels[h][w].pixel[cpo]= bias[cpo];
			}
		}
	}

   frame_m data;
   DO_PRAGMA(HLS ARRAY_PARTITION variable=data dim=0 complete)


  // All input data have effect into output add
  add_i_iter_loop:
  for (int i_iter = 0; i_iter < I_ITER; i_iter++){//2
	#pragma HLS loop_flatten off
	loop_add_read_data:
    for (int h = 0; h < H; h+=2) { // de dos en dos, los bucles con iteracions H Y W no pueden ser unrolling o flatterns porque no son constantes, entonces pipeline en el bucle superior falla
    	DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE)
    	for (int w=0; w < W; w+=2) {	 //2
    		DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
			#pragma HLS PIPELINE II=1
			data = in.read();
			for (int cpi=0; cpi < CPI; cpi++) {
				#pragma HLS UNROLL
				buff_o_channels[h][w].pixel[cpi] += data.pixel[0].pixel[cpi];
				buff_o_channels[h][w+1].pixel[cpi] += data.pixel[1].pixel[cpi];
				buff_o_channels[h+1][w].pixel[cpi] += data.pixel[2].pixel[cpi];
				buff_o_channels[h+1][w+1].pixel[cpi] += data.pixel[3].pixel[cpi];
			}
		}
	}

	if(i_iter ==(I_ITER-1)){
		for (int h = 0; h < H; h++) {
			for (int w=0; w < W; w++) {
			 #pragma HLS PIPELINE II=1
			  out << buff_o_channels[h][w];
			}
		}
	}

  }





  	//enviamos los datos
/* 	for (int cpo=0; cpo<CPO; cpo++) {
		printf("CH %d: ", cpo);
		for (int it=0; it<num_iterations; it++) {
		  printf("%d ", int(buff_o_channels[it].pixel[cpo]));
		}
		printf("\n");
	} */

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
  static hls::stream<pixel_in_t>  str_pad_cvt;  // padding->cvt
  static hls::stream<frame_d_2>   str_cvt_mul_cTc;  // cvt->mul para obtener CtDC
  static hls::stream<frame_d>     kernels_multWise;  // GgGt con los kernels -> mult wise con CtDC
  static hls::stream<frame_d_2>   mult_data_res;  // CtD -> mult wise
  static hls::stream<frame_d>     mult_wise_res;  // GgGt con los kernels -> mult wise con CtDC
  static hls::stream<frame_m> 	  str_mul_add;  // mul->add
  DO_PRAGMA(HLS stream variable=str_pad_cvt depth=CPO)
  DO_PRAGMA(HLS stream variable=str_cvt_mul_cTc depth=CPO/2)
  DO_PRAGMA(HLS stream variable=kernels_multWise depth=CPO)
  DO_PRAGMA(HLS stream variable=mult_data_res depth=CPO/2)
  DO_PRAGMA(HLS stream variable=mult_wise_res depth=CPO)
  DO_PRAGMA(HLS stream variable=str_mul_add depth=CPO)

  // topology
  #pragma HLS dataflow
  padding(H, W, I_ITER, enable_upper_padding, enable_lower_padding, in, str_pad_cvt);   // padding
  cvt_winograd(H, W, I_ITER, str_pad_cvt, str_cvt_mul_cTc);       							// cvt
  mulKernels(H, W, I_ITER, k_in, kernels_multWise); // mulkernels
  mulData(H, W, I_ITER, str_cvt_mul_cTc, mult_data_res); // mul
  mulWise(H, W, I_ITER, mult_data_res, kernels_multWise, mult_wise_res); // mul
  mult_A_AT(H, W, I_ITER, mult_wise_res, str_mul_add);
  add_winograd(H, W, I_ITER, b_in, str_mul_add, out);         // add
}

