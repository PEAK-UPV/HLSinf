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
// add_data: this function performs the element-wise addition on the input data
//
// Arguments:
//   H     : Height of input channel
//   W     : Width of input channel
//   I_ITER: Number of input iterations (I / CPI)
//   in_r  : input stream data from read module
//   in_stm: input stream data from stm module
//   out   : output stream
//
template <class in1_st, class in2_st, class out_st> void add_data(int enable_add, int num_pixels, int apply_relu, hls::stream<in1_st> &in_r, hls::stream<in2_st> &in_stm, hls::stream<out_st> &out) {

  #ifdef DEBUG_ADD_DATA
  printf("add_data: start\n");
  printf("  num_pixels : %d\n", num_pixels);
  #endif

  in1_st data_in_r;
  in2_st data_in_stm;
  out_st data_out;
  int data_size = num_pixels;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in_r complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in_stm complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_out complete dim=0)

  loop_add_data_pixels:
  for (int i=0; i < data_size; i++) {
    DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
    #pragma HLS PIPELINE II=1

    // Let's read the input data
    data_in_stm  = in_stm.read();
    if(enable_add) {
      data_in_r  = in_r.read();
      loop_add_data_cpo:
      for(int cpo = 0; cpo<CPO; cpo++){
        DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
        #pragma HLS UNROLL
        dout_t v_in_a;
        bn_t v_in_b;
        add_t v_out;
        v_in_a = data_in_r.pixel[cpo];
        v_in_b = data_in_stm.pixel[cpo];
        v_out = v_in_a + v_in_b;
	if (apply_relu && (v_out < 0)) v_out = 0;
        data_out.pixel[cpo] = v_out;
      }
    } else {
      for (int cpo=0; cpo<CPO; cpo++) {
        DO_PRAGMA(HLS UNROLL)
        data_out.pixel[cpo] = data_in_stm.pixel[cpo];
      }
    }

    #ifdef DEBUG_ADD_DATA
    printf("ADD_DATA (pixel %d):\n", i);
    for (int x=0; x<CPI; x++) {
      printf("  cpi %d : in_a %f in_b %f out %f\n", x, float(data_in_r.pixel[x]),float(data_in_stm.pixel[x]), float(data_out.pixel[x]));
    }
    #endif

    out << data_out;
  }
  #ifdef DEBUG_ADD_DATA
  printf("add_data: end\n");
  #endif
}
