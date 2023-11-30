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

#ifdef FAULT_DETECTION
void add(int num_pixels, int I_ITER, hls::stream<conv_mul_st> &in, hls::stream<b_st> &b_in, hls::stream<conv_st> &out) {
#endif
#ifdef FAULT_CORRECTION
void add(int num_pixels, int I_ITER, hls::stream<fc_add_st> &in, hls::stream<b_st> &b_in, hls::stream<conv_st> &out) {
#endif


  #ifdef DEBUG_ADD
  printf("add: start\n");
  #endif

  b_st bias;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=bias dim=0 complete)

  // number of iterations by CPI || CPO channels
  int num_iterations = num_pixels;

  // Buffer for all data and CPO channels
  static conv_st buff_o_channels[WMAX*HMAX];
  DO_PRAGMA(HLS AGGREGATE variable=buff_o_channels)
  #ifdef ALVEO_U200
  DO_PRAGMA(HLS bind_storage variable=buff_o_channels type=ram_t2p impl=uram)
  #endif
  #ifdef ALVEO_U280
  DO_PRAGMA(HLS bind_storage variable=buff_o_channels type=ram_t2p impl=uram)
  #endif


  // We receive bias in packs of CPO
  bias = b_in.read();

  #ifdef DEBUG_ADD
  #ifdef DEBUG_VERBOSE
  for (int b=0; b<CPO; b++) {
    printf("Bias[%d] = %6.4f \n", b, float(bias.pixel[b]));
  }
  printf("add: bias received\n");
  for(int cpo = 0; cpo<CPO; cpo++){
    printf("Channel cpo = %d: ", cpo);
    for(int it = 0; it<num_iterations; it++){
      printf("%6.2f ", float(buff_o_channels[it].pixel[cpo]));
    }
    printf("\n");
  }
  #endif
  #endif

  // All input data have effect into output add
  add_i_iter_loop:
  for (int i_iter = 0; i_iter < I_ITER; i_iter++){
    DO_PRAGMA(HLS loop_tripcount  min=1 max=I_REFERENCE/CPI)
    conv_st data_out;
    #pragma HLS loop_flatten off
    add_load_data_it_loop:
    for(int it = 0; it<num_iterations; it++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
      conv_mul_st px;
      px = in.read();
      b_st data_in;
      conv_st data_out;

      if (i_iter == 0) data_in = bias; else data_in = *(b_st *)&buff_o_channels[it];

      add_load_data_cpo_loop:
      for (int cpo=0; cpo<CPO; cpo++) {
        DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
        #pragma HLS unroll
        data_out.pixel[cpo] = data_in.pixel[cpo] + px.pixel[cpo];
      }
      buff_o_channels[it] = data_out;

      if(i_iter ==(I_ITER-1)){

        out << data_out;
      }
    }
  } //i_iter

  #ifdef DEBUG_ADD
  #ifdef DEBUG_VERBOSE
  for (int cpo=0; cpo<CPO; cpo++) {
    printf("CH %d: ", cpo);
    for (int it=0; it<num_iterations; it++) {
      printf("%6.2f ", float(buff_o_channels[it].pixel[cpo]));
    }
    printf("\n");
  }
  #endif
  #endif

  #ifdef DEBUG_ADD
  printf("add: end\n");
  #endif
}
