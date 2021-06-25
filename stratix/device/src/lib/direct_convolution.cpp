#include "lib_hdrs_priv.h"


// -------------------------------------------------------------------------------
// direct_conv: Convolutional kernel
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
// This module builds the direct convolutional operation by instantiating streams and
// building the dataflow model with the corresponding modules
//
void direct_conv(int o_iter, int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, ihc::stream<pixel_in_t> &in, ihc::stream<kernel_t> &k_in, ihc::stream<pixel_out_t> &b_in, ihc::stream<pixel_out_t> &out) {

  // streams
  ihc::stream<pixel_in_t>  str_pad_cvt;  // padding->cvt
  ihc::stream<frame_t>     str_cvt_mul;  // cvt->mul
  ihc::stream<pixel_out_t> str_mul_add;  // mul->add
  //DO_PRAGMA(HLS stream variable=str_pad_cvt depth=STREAMS_DEPTH)
  //DO_PRAGMA(HLS stream variable=str_cvt_mul depth=STREAMS_DEPTH)
  //DO_PRAGMA(HLS stream variable=str_mul_add depth=STREAMS_DEPTH)

  // topology
  //#pragma HLS dataflow
  padding(o_iter, H, W, I_ITER, enable_upper_padding, enable_lower_padding, in, str_pad_cvt);   // padding
  cvt(o_iter, H, W, I_ITER, str_pad_cvt, str_cvt_mul);       									// cvt
  mul(o_iter, H, W, I_ITER, str_cvt_mul, k_in, str_mul_add); 									// mul
  add(o_iter, H, W, I_ITER, str_mul_add, b_in, out);         									// add
}
