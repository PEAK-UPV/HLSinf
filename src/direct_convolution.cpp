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
void direct_conv(int H, int W, int PT, int PB, int PL, int PR, int SH, int SW, int num_output_conv_pixels, int I_ITER, hls::stream<din_st> &in, hls::stream<w_st> &k_in, hls::stream<b_st> &b_in, hls::stream<conv_st> &out) {

  // streams
  static hls::stream<din_st>  str_pad_cvt;  // padding->cvt
  static hls::stream<conv_cvt_st>     str_cvt_mul;  // cvt->mul
  static hls::stream<conv_mul_st> str_mul_add;  // mul->add
  DO_PRAGMA(HLS stream variable=str_pad_cvt depth=STREAMS_DEPTH)
  DO_PRAGMA(HLS stream variable=str_cvt_mul depth=STREAMS_DEPTH)
  DO_PRAGMA(HLS stream variable=str_mul_add depth=STREAMS_DEPTH)

  // topology
  #pragma HLS dataflow
  padding(H, W, PT, PB, PL, PR, I_ITER, in, str_pad_cvt);   // padding
  cvt(H+PT+PB, W+PL+PR, I_ITER, SH, SW, str_pad_cvt, str_cvt_mul);       									// cvt
  mul(num_output_conv_pixels, I_ITER, str_cvt_mul, k_in, str_mul_add); 									// mul
  add(num_output_conv_pixels, I_ITER, str_mul_add, b_in, out);         									// add
}
