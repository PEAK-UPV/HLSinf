/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

#ifdef DWS_CONV
// -------------------------------------------------------------------------------
// dws_convolution: Convolutional kernel
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
// This module builds the deepwise separable convolutional operation by instantiating streams and
// building the dataflow model with the corresponding modules
//
void dws_conv(int H, int W, int I_ITER, hls::stream<pixel_in_t> &in, hls::stream<kernel_dw_t> &k_dw_in, hls::stream<kernel_pw_t> &k_pw_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out) {

  // streams
  static hls::stream<pixel_in_t>  str_pad_cvt;  // padding->cvt
  static hls::stream<frame_t>     str_cvt_mul;  // cvt->mul
  static hls::stream<pixel_out_t> str_mul_add;  // mul->add
  DO_PRAGMA(HLS stream variable=str_pad_cvt depth=DWS_STREAM_DEPTH)
  DO_PRAGMA(HLS stream variable=str_cvt_mul depth=DWS_STREAM_DEPTH)
  DO_PRAGMA(HLS stream variable=str_mul_add depth=DWS_STREAM_DEPTH)

  // topology
  #pragma HLS dataflow
  padding(H, W, I_ITER, in, str_pad_cvt);   // padding
  cvt(H, W, I_ITER, str_pad_cvt, str_cvt_mul);       									// cvt
  dws_mul(H, W, I_ITER, str_cvt_mul, k_dw_in, k_pw_in, str_mul_add);                    // dws_mul
  add(H, W, I_ITER, str_mul_add, b_in, out);         									// add
}
#endif
