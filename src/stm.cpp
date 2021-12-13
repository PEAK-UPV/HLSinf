/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

// -------------------------------------------------------------------------------
// stm: This module performs the softplus and hyperbolic tangent functions of the
// input pixels and multiplies the result by the input.
//
// Arguments:
//   enable_stm      : Flag to enable STM functions
//   H               : Height of the input channel
//   W               : Width of the input channel
//   in              : input data stream
//   out             : output data stream
//
//


void stm(int enable_stm, int num_pixels, hls::stream<relu_st> &in, hls::stream<stm_st> &out) {

  #ifdef DEBUG_STM
  printf("stm: start\n");
  #endif

  relu_st data_in;
  stm_st data_out;
  int data_size = num_pixels;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_out complete dim=0)

  loop_stm_pixels:
  for (int i=0; i < data_size; i++) {
    DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
    #pragma HLS PIPELINE II=1

	// Let's read the input data
    data_in  = in.read();

    loop_stm_cpo:
    for(int cpo = 0; cpo<CPO; cpo++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      #pragma HLS UNROLL

      stm_t v_in, v_soft, v_tanh, v_mult;
      v_in = data_in.pixel[cpo];

#ifdef USE_STM
      if(enable_stm) {
    	  // softplus
    	  v_soft = hls::logf(hls::expf(v_in) + 1); //logf for single-precision natural logarithm

    	  //tanh
    	  v_tanh = hls::tanhf(v_soft);

    	  //mult
    	  v_mult = v_tanh * v_in;

    	  data_out.pixel[cpo] = v_mult;
      }
      else {
    	  data_out.pixel[cpo] = v_in;
      }
#else
      data_out.pixel[cpo] = v_in;
#endif
    }

    #ifdef DEBUG_STM
    printf("STM (pixel %d):\n", i);
    for (int x=0; x<CPI; x++) {
    	printf("  cpi %d : in %f out %f\n", x, float(data_in.pixel[x]), float(data_out.pixel[x]));
    }
    #endif
    out << data_out;
}

#ifdef DEBUG_STM
printf("stm: end\n");
#endif
}
