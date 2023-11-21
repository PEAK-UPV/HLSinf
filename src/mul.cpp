/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

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
//
// This module is used in the Direct Convolution method
//
void mul(int num_data_frames, int I_ITER, hls::stream<conv_cvt_st> &in, hls::stream<w_st> &k_in, hls::stream<conv_mul_st> &out, int foultTolerance, int &flag) {

  #ifdef DEBUG_MUL
  printf("mul: start\n");
  #endif

  w_st kernel;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel dim=0 complete)
  w_st kernel_used;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel_used dim=0 complete)

  conv_cvt_st data_in;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in dim=0 complete)

  conv_mul_t sum[CPO];
  DO_PRAGMA(HLS ARRAY_PARTITION variable=sum dim=0 complete)

  conv_mul_st p_out;
  conv_mul_st d_out;

  int load_kernel = 0;
  int num_iter = foultTolerance ? I_ITER * num_data_frames *2 : I_ITER * num_data_frames;
  int iter_load_kernel = 0;
  int fallo = 0;
  int i = 0;
  int cpo_used = 0;



  mul_loop_1:
  while(i < num_iter){
    DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE*I_REFERENCE/CPI*2)
    #pragma HLS PIPELINE II=1

	bool impar = foultTolerance ? i & 0x1 : false;

	if ((iter_load_kernel == 0) && !impar){
	  kernel = k_in.read();
	  #ifdef DEBUG_MUL
	  #ifdef DEBUG_VERBOSE
	  printf("MUL: kernel read\n");
	  for(int i=0; i<CPI; i++){
		for(int o=0; o<CPO; o++){
		  printf("kernel cpi=%d cpo=%d\n", i, o);
		  for (int p=0; p<9; p++){
			printf(" %f ", float(kernel.pixel[o][i][p]));
			if((p+1)%3==0)printf("\n");
		  }
		  printf("\n");
		}
	  }
	  #endif
	  #endif
	}

    mul_loop_2:
    for(int a=0; a<CPO; a++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      #pragma HLS UNROLL
      sum[a] = 0;
    }

    if(!impar) data_in = in.read();

    loop_mul_cpi:
    for (int cpi=0; cpi<CPI; cpi++) {
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
      #pragma HLS UNROLL
      loop_mul_j:
      for (int j=0; j<KW*KH; j++) {
        DO_PRAGMA(HLS loop_tripcount  min=1 max=KW*KH)
        #pragma HLS UNROLL
		loop_mul_cpo:
		for (int cpo = 0; cpo < CPO; cpo++) {
			DO_PRAGMA(HLS loop_tripcount min=1 max=CPO)
			#pragma HLS UNROLL
	    	 cpo_used = foultTolerance ? (!impar ? cpo/2 : (cpo + CPO / 2) / 2 + (CPO / 4)) : cpo;
	    	 sum[cpo] += data_in.pixel[j].pixel[cpi] * kernel.pixel[cpo_used][cpi][j];
		}

      }
    }

    for(int s=0; s<CPO; s++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      #pragma HLS UNROLL
      p_out.pixel[s] = sum[s];
//      if(i < 2) {
//    	  printf("i: %d | sum[%d] = %6.2f\n", i, s, sum[s]);
//      }
    }
    #ifdef DEBUG_MUL
    #ifdef DEBUG_VERBOSE
    for(int i = 0;i<CPO;i++) {
      printf("mult: p_out.pixel[%d] = %6.2f  ", i, float(p_out.pixel[i]));
    }
    printf("\n");
    #endif
    #endif

    int check = 0;
    if (foultTolerance){
		loop_check_cpo:
		for(int p=0; p<CPO; p+=2){
		  DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO/2)
		  #pragma HLS UNROLL
		  if(p_out.pixel[p] != p_out.pixel[p+1]) check++;
		}
    }

    //actualizar flag al valor de check
    if (check) flag = check;

	loop_update_data:
	for(int c=0; c<CPO; c++){
	   DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
	   #pragma HLS UNROLL
	   if ((c < CPO/2) && !impar && foultTolerance) d_out.pixel[c] = p_out.pixel[c*2];
	   if ((c >= CPO/2) && impar && foultTolerance) d_out.pixel[c] = p_out.pixel[(c-CPO/2)*2];
	   if (!foultTolerance) d_out.pixel[c] = p_out.pixel[c];
	}

	if(impar || !foultTolerance){
//		if(i < 2){
//			printf("\ndout con i(%d):\n", i);
//			for(int c=0; c<CPO; c++){
//				printf("mult: p_out.pixel[%d] = %6.2f  ", i, float(d_out.pixel[c]));
//			}
//
//		}
	 out << d_out;
	 iter_load_kernel++;
	 if (iter_load_kernel == num_data_frames) iter_load_kernel = 0;
	}
	i++;
  }

  #ifdef DEBUG_MUL
  printf("mul: end\n");
  #endif
}

#ifdef DWS_CONV
// ----------------------------------------------------------------------------------------
// dws_mul: This function performs the deewise separable multiplication of an input frame
// with the stored kernels and sends the produced pixels. 
// Before normal operation it receives its kernels
// Arguments:
//   H        : Height of the input channel
//   W        : Width of the input channel
//   I_ITER   : Number of input iterations (I / CPI)
//   in       : input stream with incoming data frames
//   k_dw_in  : input stream with deepwise kernels
//   k_pw_in  : input stream with pointwise kernels
//   out      : output stream
//
// This module is used in the DWS Convolution method
//
void dws_mul(int H, int W, int I_ITER, hls::stream<frame_t> &in, hls::stream<kernel_dw_t> &k_dw_in, hls::stream<kernel_pw_t> &k_pw_in, hls::stream<pixel_out_t> &out) {
	  
  #ifdef DEBUG_MUL
  printf("mul: start (I_ITER %d)\n", I_ITER);
  #endif

  kernel_dw_t kernel_dw;
  kernel_pw_t kernel_pw;
  frame_t data_in;
  frame_t data_mul;
  data_type data_sum[CPI];
  data_type pixel_out[CPO];
  pixel_out_t p_out;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel_dw complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel_pw complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in dim=0 complete)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_mul dim=0 complete)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_sum dim=0 complete)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=pixel_out dim=0 complete)

  int load_kernel = 0;
  int num_iter = I_ITER * H * W;
  int iter_load_kernel = 0;

  dws_mul_loop_iter:
  for(int i = 0; i < num_iter; i++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=(I_REFERENCE / CPI) * W_REFERENCE * H_REFERENCE)
    #pragma HLS PIPELINE II=1

    load_kernel = (iter_load_kernel == 0);

	// we load the kernels
    if (load_kernel) {
      kernel_dw = k_dw_in.read();
      kernel_pw = k_pw_in.read();

      #ifdef DEBUG_MUL
      #ifdef DEBUG_VERBOSE
      printf("MUL: loaded dw kernel:\n");
      for (int cpi=0; cpi<CPI; cpi++) {
	    printf("cpi %d: ", cpi);
	    for (int p=0; p<9; p++) {
  		  printf("%f ", float(kernel_dw.pixel[cpi][p]));
	    }
	    printf("\n");
      }
      printf("MUL: loaded pw kernel  (cpi/cpo table):\n");
      for (int cpi=0; cpi<CPI; cpi++) {
    	for (int cpo=0; cpo<CPO; cpo++) {
    		printf(" %4.2f", float(kernel_pw.pixel[cpo][cpi]));
    	}
    	printf("\n");
      }
      #endif
      #endif
    }

    // we read the input frame
    data_in = in.read();

    #ifdef DEBUG_MUL
    #ifdef DEBUG_VERBOSE
    printf("MUL: read data:\n");
    for (int cpi=0; cpi<CPI; cpi++) {
    	printf("cpi %d: ", cpi);
    	for (int p=0; p<9; p++) {
    		printf("%f ", float(data_in.pixel[p].pixel[cpi]));
    	}
    	printf("\n");
    }
    #endif
    #endif


    // as a first step we multiply the dw kernel with the input frame
    dws_mul_loop_dw_mul_cpi:
	for (int cpi=0; cpi<CPI; cpi++) {
      DO_PRAGMA(HLS UNROLL)
	  dws_mul_loop_dw_mul_p:
	  for (int p=0; p<9; p++) {
	    DO_PRAGMA(HLS UNROLL)
	    data_mul.pixel[p].pixel[cpi] = data_in.pixel[p].pixel[cpi] * kernel_dw.pixel[cpi][p];
	  }
	}
    #ifdef DEBUG_MUL
    #ifdef DEBUG_VERBOSE
    printf("MUL: DW_MUL\n");
    for (int cpi=0; cpi<CPI; cpi++) {
    	for (int p=0; p<9; p++) {
    		printf("%f ", float(data_mul.pixel[p].pixel[cpi]));
    	}
    	printf("\n");
    }
    #endif
    #endif

	// now we reduce the dw output into cpo pixels
	dws_mul_loop_dw_reduce_cpi:
	for (int cpi=0; cpi<CPI; cpi++) {
	  DO_PRAGMA(HLS UNROLL)
	  data_sum[cpi] = 0;
	  dws_mul_loop_dw_reduce_p:
      for (int p=0; p<9; p++) {
	    DO_PRAGMA(HLS UNROLL)
	    data_sum[cpi] += data_mul.pixel[p].pixel[cpi];
	  }
	}
    #ifdef DEBUG_MUL
    #ifdef DEBUG_VERBOSE
    printf("MUL: DW_REDUCE\n");
    for (int cpi=0; cpi<CPI; cpi++) {
  	  printf("%f ", float(data_sum[cpi]));
	}
	printf("\n");
    #endif
    #endif

	// now we multiply and reduce the input for each output applying the point wise kernel
	dws_mul_loop_pd_reduce_cpo:
	for (int cpo=0; cpo<CPO; cpo++) {
	  DO_PRAGMA(HLS UNROLL)
  	  pixel_out[cpo] = 0;
  	  dws_mul_loop_pd_reduce_cpi:
	  for (int cpi=0; cpi<CPI; cpi++) {
		DO_PRAGMA(HLS UNROLL)
		pixel_out[cpo] += data_sum[cpi] * kernel_pw.pixel[cpo][cpi];
	  }
 	}
    #ifdef DEBUG_MUL
    #ifdef DEBUG_VERBOSE
    printf("MUL: PW_REDUCE\n");
    for (int cpo=0; cpo<CPO; cpo++) {
	  printf(" %f ", float(pixel_out[cpo]));
    }
    printf("\n");
    #endif
    #endif

	// now we send the frame out
	dws_mul_loop_send_cpo:
	for (int cpo=0; cpo<CPO; cpo++) {
		p_out.pixel[cpo] = pixel_out[cpo];
	}
    out << p_out;

    #ifdef DEBUG_MUL
    #ifdef DEBUG_VERBOSE
    for(int i = 0;i<CPO;i++) {
      printf("mult: p_out.pixel[%d] = %6.2f  ", i, float(p_out.pixel[i]));
    }
    printf("\n");
    #endif
    #endif

    iter_load_kernel++;
    if (iter_load_kernel == W*H) iter_load_kernel = 0;
  }

  #ifdef DEBUG_MUL
  printf("mul: end\n");
  #endif
}

#endif
