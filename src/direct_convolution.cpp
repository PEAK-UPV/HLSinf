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
void direct_conv(int H, int W, int PH, int PW, int SH, int SW, int num_output_conv_pixels, int I_ITER, hls::stream<pixel_in_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out) {

  // streams
  static hls::stream<pixel_in_t>  str_pad_cvt;  // padding->cvt
  static hls::stream<frame_t>     str_cvt_mul;  // cvt->mul
  static hls::stream<pixel_out_t> str_mul_add;  // mul->add
  DO_PRAGMA(HLS stream variable=str_pad_cvt depth=STREAMS_DEPTH)
  DO_PRAGMA(HLS stream variable=str_cvt_mul depth=STREAMS_DEPTH)
  DO_PRAGMA(HLS stream variable=str_mul_add depth=STREAMS_DEPTH)

  // topology
  #pragma HLS dataflow
  padding(H, W, PH, PW, I_ITER, in, str_pad_cvt);   // padding
  cvt(H+PH+PH, W+PW+PW, I_ITER, SH, SW, str_pad_cvt, str_cvt_mul);       									// cvt
  mul(num_output_conv_pixels, I_ITER, str_cvt_mul, k_in, str_mul_add); 									// mul
  add(num_output_conv_pixels, I_ITER, str_mul_add, b_in, out);         									// add
}
