#include "conv2D.h"

#include <hls_stream.h>


// -------------------------------------------------------------------------------
// relu: module of ReLu function
//
// Arguments:
//   enable_relu: : Flag to enable ReLu function
//   H            : Height of the input channel
//   W            : Width of the input channel
//   in           : input data stream
//   out          : output data stream
//
// This module builds ReLu function by instantiatig streams and
// building the dataflow model with the corresponding modules
//
void relu(int enable_relu, int H, int W, hls::stream<pixel_out_t> &in, hls::stream<pixel_out_t> &out) {

#ifdef DEBUG_RELU
printf("relu: start\n");
#endif

pixel_out_t data;
int data_size = W * H;
for (int i=0; i < data_size; i++) {
  DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
  #pragma HLS PIPELINE II=1
  data  = in.read();
  for(int cpo = 0; cpo<CPO; cpo++){
    DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
    #pragma HLS UNROLL
    if(enable_relu & (data.pixel[cpo] < 0)) data.pixel[cpo] = data_type(0.f);
  }
  out << data;
}

#ifdef DEBUG_RELU
printf("relu: end\n");
#endif
}
