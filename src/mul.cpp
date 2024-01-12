/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

#ifdef DIRECT_CONV

#ifdef PRUEBA_MUL_2
conv_mul_t conv_op(conv_mul_t x0, conv_mul_t x1, conv_mul_t x2, conv_mul_t x3, conv_mul_t x4, conv_mul_t x5, conv_mul_t x6, conv_mul_t x7, conv_mul_t x8,
                   w_t w0, w_t w1, w_t w2, w_t w3, w_t w4, w_t w5, w_t w6, w_t w7, w_t w8) 
{
  conv_mul_t r1;
  conv_mul_t r2;

  #pragma HLS BIND_op variable=r1 op=mul impl=fabric
  r1 = (x0 * w0) + (x1 * w1) + (x2 * w2);
  #pragma HLS BIND_op variable=r2 op=mul impl=dsp
  r2 = (x3 * w3) + (x4 * w4) + (x5 * w5) + (x6 * w6) + (x7 * w7) + (x8 * w8);
  return r1 + r2;
}
#endif

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
void mul(int num_data_frames, int I_ITER, hls::stream<conv_cvt_st> &in, hls::stream<w_st> &k_in, hls::stream<conv_mul_st> &out) {

  #ifdef DEBUG_MUL
  printf("mul: start\n");
  #endif

  w_st kernel;
  w_in_st k;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel dim=0 complete)
  #pragma HLS array_partition variable=k dim=0 complete

  conv_cvt_st data_in;

  conv_mul_t sum[CPO];
  DO_PRAGMA(HLS ARRAY_PARTITION variable=sum dim=0 block factor=CPO)

  conv_mul_st p_out;

  int load_kernel = 0;
  int num_iter = I_ITER * num_data_frames;
  int iter_load_kernel = 0;

  mul_loop_1:
  for(int i = 0; i < num_iter; i++){
    DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE*I_REFERENCE/CPI)
    #pragma HLS PIPELINE II=1
    load_kernel = (iter_load_kernel == 0);
    if (load_kernel){
      kernel = k_in.read();
      #ifdef DEBUG_MUL
      //#ifdef DEBUG_VERBOSE
      printf("MUL: kernel read\n");
		for(int o=0; o<CPO; o++){
			for(int i=0; i<CPI; i++){
          printf("kernel cpi=%d cpo=%d: ", i, o);
          for (int p=0; p<9; p++){
            printf(" %f ", float(kernel.pixel[o][i][p]));
            //if((p+1)%3==0)printf("\n");
          }
          printf("\n");
        }
      }
      //#endif
      #endif
    }

    mul_loop_2:
    for(int i=0; i<CPO; i++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      #pragma HLS UNROLL
      sum[i] = 0;
    }

    data_in = in.read();
    #ifdef DEBUG_MUL
    for (int xx=0; xx<CPI; xx++) {
      for (int x=0; x<KW*KH; x++) printf("%6.4f ", float(data_in.pixel[x].pixel[xx]));
      printf(",");
    }
    #endif

#ifdef PRUEBA_MUL_2
    for (int cpi = 0; cpi < CPI; cpi++) {
      #pragma HLS UNROLL
      for (int cpo=0; cpo<CPO; cpo++) {
        #pragma HLS UNROLL
        sum[cpo] += conv_op(data_in.pixel[0].pixel[cpi], data_in.pixel[1].pixel[cpi], data_in.pixel[2].pixel[cpi],
                            data_in.pixel[3].pixel[cpi], data_in.pixel[4].pixel[cpi], data_in.pixel[5].pixel[cpi],
                            data_in.pixel[6].pixel[cpi], data_in.pixel[7].pixel[cpi], data_in.pixel[8].pixel[cpi],
                            kernel.pixel[cpo][cpi][0], kernel.pixel[cpo][cpi][1], kernel.pixel[cpo][cpi][2], 
                            kernel.pixel[cpo][cpi][3], kernel.pixel[cpo][cpi][4], kernel.pixel[cpo][cpi][5], 
                            kernel.pixel[cpo][cpi][6], kernel.pixel[cpo][cpi][7], kernel.pixel[cpo][cpi][8]);
      }
    }
#else
    

#ifdef PRUEBA_MUL
    struct conv_mul_st2 {conv_mul_t pixel[CPO][CPI][9]};
    struct conv_mul_st3 {conv_mul_t pixel[CPO][CPI];};
    conv_mul_st2 x;
    DO_PRAGMA(HLS ARRAY_PARTITION variable=x dim=0 complete)
    conv_mul_st3 xx;
    DO_PRAGMA(HLS ARRAY_PARTITION variable=xx dim=0 complete)
    loop_mul_cpo:
    for (int cpo=0; cpo<CPO; cpo = cpo + 1) {
      #pragma HLS UNROLL
      for (int cpi = 0; cpi < CPI; cpi++) {
        #pragma HLS UNROLL
        for (int j = 0; j < KH*KW; j++) {
          #pragma HLS UNROLL
          x.pixel[cpo][cpi][j] = data_in.pixel[j].pixel[cpi] * kernel.pixel[cpo][cpi][j];
        }
      }
    }
    for (int cpo=0; cpo<CPO; cpo = cpo + 1) {
      #pragma HLS UNROLL
      for (int cpi = 0; cpi < CPI; cpi++) {
        #pragma HLS UNROLL
        for (int j = 0; j < KH*KW; j++) {
          #pragma HLS UNROLL
          xx.pixel[cpo][cpi] += x.pixel[cpo][cpi][j];
        }
      }
    }
    for (int cpo=0; cpo<CPO; cpo = cpo + 1) {
      #pragma HLS UNROLL
      for (int cpi = 0; cpi < CPI; cpi++) {
        #pragma HLS UNROLL
        sum[cpo] += xx.pixel[cpo][cpi];
      }
    }


#else
    loop_mul_cpi:
    for (int cpi=0; cpi<CPI; cpi++) {
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
      #pragma HLS UNROLL
      loop_mul_j:
      for (int j=0; j<KW*KH; j++) {
        DO_PRAGMA(HLS loop_tripcount  min=1 max=KW*KH)
        #pragma HLS UNROLL
#ifdef DSP_OPTIMIZATION
        loop_mul_cpo:
        for (int cpo=0; cpo<CPO; cpo = cpo + 2) {
	        DO_PRAGMA(HLS loop_tripcount min=1 max=CPO/2)
          #pragma HLS UNROLL
	        ap_int<27> op1;
          op1.range(26, 18) = kernel.pixel[cpo][cpi][j];
          op1.range(17, 0) = 0;
	        ap_uint<27> op2;
	        op2 = kernel.pixel[cpo+1][cpi][j];
	        ap_int<45> result;
          #pragma HLS BIND_op variable=result op=mul impl=dsp
	        result = (op1 + op2) * data_in.pixel[j].pixel[cpi];
	        sum[cpo] += result.range(33, 18);
	        sum[cpo+1] += result.range(15, 0);
	}
#else
	loop_mul_cpo:
        for (int cpo=0; cpo<CPO; cpo++) {
          DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
          #pragma HLS UNROLL
	        conv_mul_t x;
	        #pragma HLS BIND_op variable = x op=mul impl=dsp
	        x = data_in.pixel[j].pixel[cpi] * kernel.pixel[cpo][cpi][j];
	        sum[cpo] += x;
          //sum[cpo] += data_in.pixel[j].pixel[cpi] * kernel.pixel[cpo][cpi][j];
        }
#endif
      }
    }

#endif
#endif

    for(int i=0; i<CPO; i++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      #pragma HLS UNROLL
      p_out.pixel[i] = sum[i];
    }
    #ifdef DEBUG_MUL
    //#ifdef DEBUG_VERBOSE
    printf("->");
    for(int i = 0;i<CPO;i++) {
      printf(" %6.2f", float(p_out.pixel[i]));
    }
    printf("\n");
    //#endif
    #endif
    out << p_out;
    iter_load_kernel++;
    if (iter_load_kernel == num_data_frames) iter_load_kernel = 0;
  }

  #ifdef DEBUG_MUL
  printf("mul: end\n");
  #endif
}
#endif

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
void dws_mul(int num_data_frames, int I_ITER, hls::stream<conv_cvt_st> &in, hls::stream<w_dw_st> &k_dw_in, hls::stream<w_pw_st> &k_pw_in, hls::stream<conv_mul_st> &out) {
	  
  #ifdef DEBUG_MUL
  printf("mul: start (I_ITER %d)\n", I_ITER);
  #endif

  w_dw_st     kernel_dw;
  w_pw_st     kernel_pw;
  conv_cvt_st data_in;
  conv_cvt_st data_mul;
  conv_mul_st data_sum;
  conv_mul_st pixel_out;
  conv_mul_st p_out;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel_dw complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel_pw complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in dim=0 complete)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_mul dim=0 complete)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_sum dim=0 complete)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=pixel_out dim=0 complete)

  int load_kernel      = 0;
  int num_iter         = I_ITER * num_data_frames;
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
	  data_sum.pixel[cpi] = 0;
	  dws_mul_loop_dw_reduce_p:
      for (int p=0; p<9; p++) {
	    DO_PRAGMA(HLS UNROLL)
	    data_sum.pixel[cpi] += data_mul.pixel[p].pixel[cpi];
	  }
	}
    #ifdef DEBUG_MUL
    #ifdef DEBUG_VERBOSE
    printf("MUL: DW_REDUCE\n");
    for (int cpi=0; cpi<CPI; cpi++) {
  	  printf("%f ", float(data_sum.pixel[cpi]));
	}
	printf("\n");
    #endif
    #endif

	// now we multiply and reduce the input for each output applying the point wise kernel
	dws_mul_loop_pd_reduce_cpo:
	for (int cpo=0; cpo<CPO; cpo++) {
	  DO_PRAGMA(HLS UNROLL)
  	  pixel_out.pixel[cpo] = 0;
  	  dws_mul_loop_pd_reduce_cpi:
	  for (int cpi=0; cpi<CPI; cpi++) {
		DO_PRAGMA(HLS UNROLL)
		pixel_out.pixel[cpo] += data_sum.pixel[cpi] * kernel_pw.pixel[cpo][cpi];
	  }
 	}
    #ifdef DEBUG_MUL
    #ifdef DEBUG_VERBOSE
    printf("MUL: PW_REDUCE\n");
    for (int cpo=0; cpo<CPO; cpo++) {
	  printf(" %f ", float(pixel_out.pixel[cpo]));
    }
    printf("\n");
    #endif
    #endif

	// now we send the frame out
	dws_mul_loop_send_cpo:
	for (int cpo=0; cpo<CPO; cpo++) {
		p_out.pixel[cpo] = pixel_out.pixel[cpo];
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
    if (iter_load_kernel == num_data_frames) iter_load_kernel = 0;
  }

  #ifdef DEBUG_MUL
  printf("mul: end\n");
  #endif
}

#endif
