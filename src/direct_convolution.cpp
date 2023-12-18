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
// direct_conv: Convolutional kernel
//
// Arguments:
//   H                    : Height of the input channel
//   W                    : Width of the input channel
//   I_ITER               : Number of input iterations (I / CPI)
//   in                   : input data stream
//   k_in                 : input kernel stream
//   b_in                 : input bias stream
//   out                  : output data stream
//
// This module builds the direct convolutional operation by instantiating streams and
// building the dataflow model with the corresponding modules
//
#ifdef DIRECT_CONV
void direct_conv(int H, int W, int PT, int PB, int PL, int PR, int SH, int SW, int num_output_conv_pixels, int I_ITER, hls::stream<din_st> &in, hls::stream<w_st> &k_in, hls::stream<b_st> &b_in, hls::stream<conv_st> &out, int foultTolerance, int &errorFlag) {

  #pragma HLS dataflow

  // streams
  static hls::stream<din_st>      str_pad_cvt;  // padding->cvt
  static hls::stream<conv_cvt_st> str_cvt_mul;  // cvt->mul
#ifdef FAULT_DETECTION
  static hls::stream<conv_mul_st> str_mul_add;  // mul->add
#endif
#ifdef FAULT_CORRECTION
  static hls::stream<fc_add_st> str_mul_add;  // mul->add
  static hls::stream<fc_mul_st> str_mul_mul;  // mul->mul correction faults
  DO_PRAGMA(HLS stream variable=str_mul_mul depth=STREAMS_DEPTH)
#endif
  DO_PRAGMA(HLS stream variable=str_pad_cvt depth=STREAMS_DEPTH)
  DO_PRAGMA(HLS stream variable=str_cvt_mul depth=STREAMS_DEPTH)
  DO_PRAGMA(HLS stream variable=str_mul_add depth=STREAMS_DEPTH)

  int h = H+PT+PB;
  int w = W+PL+PR;

  // topology

  padding(H, W, PT, PB, PL, PR, I_ITER, in, str_pad_cvt);   // padding
  cvt(h, w, I_ITER, SH, SW, str_pad_cvt, str_cvt_mul);       									        // cvt

#ifdef FAULT_DETECTION
  mul(num_output_conv_pixels, I_ITER, str_cvt_mul, k_in, str_mul_add, foultTolerance, errorFlag);
#endif
#ifdef FAULT_CORRECTION
  mul(num_output_conv_pixels, I_ITER, str_cvt_mul, k_in, str_mul_mul, str_mul_add, foultTolerance, errorFlag); 	// mul
#endif
  add(num_output_conv_pixels, I_ITER, str_mul_add, b_in, out);         											// add
}
#endif

