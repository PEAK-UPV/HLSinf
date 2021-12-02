#include "conv2D.h"

#include <hls_stream.h>

// -------------------------------------------------------------------------------
// relu: module of ReLu function. It also provides support for clipping and shift
//
// Arguments:
//   enable_relu:    : Flag to enable ReLu operation
//   enable_clipping : Flag to enable clipping operation
//   enable_shift    : Flag to enable shift operation
//   min_clipping    : Minimum value to clip
//   max_clipping    : Maximum value to clip
//   direction_shift : Direction to shift (LEFT_DIRECTION, RIGHT_DIRECTION)
//   pos_shift       : Positions to shift
//   H               : Height of the input channel
//   W               : Width of the input channel
//   in              : input data stream
//   out             : output data stream
//
//

void relu(int enable_relu, int enable_clipping, int enable_shift, relu_t relu_factor, int min_clip, int max_clip, int direction_shift, int pos_shift,
		  int num_pixels, hls::stream<conv_st> &in, hls::stream<relu_st> &out) {

  #ifdef DEBUG_RELU
  printf("relu: start\n");
  #endif

  conv_st data_in;
  relu_st data_out;
  int data_size = num_pixels;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_out complete dim=0)

  loop_relu_pixels:
  for (int i=0; i < data_size; i++) {
    DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
    #pragma HLS PIPELINE II=1

	// Let's read the input data
    data_in  = in.read();

    loop_relu_cpo:
    for(int cpo = 0; cpo<CPO; cpo++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      #pragma HLS UNROLL

	conv_t v_in;
      conv_t v_shift, v_clipping;
      conv_t v_relu;

      v_in = data_in.pixel[cpo];

      // shift
#ifdef USE_SHIFT
      v_shift = v_in;
      if (enable_shift) {
    	  if (direction_shift == LEFT_DIRECTION) v_shift = v_in << pos_shift;
    	  if (direction_shift == RIGHT_DIRECTION) v_shift = v_in >> pos_shift;
      }
#else
      v_shift = v_in;
#endif

      #ifdef DEBUG_RELU
//      #ifdef DEBUG_VERBOSE
      printf("SHIFT(pixel %d, cpo %d): in %f out %f\n", i, cpo, float(v_in), float(v_shift));
//      #endif
      #endif
      // clipping
      v_clipping = v_shift;
      if (enable_clipping) {
    	  if (v_shift < min_clip) {
    		  v_clipping = min_clip;
    	  } else if (v_shift > max_clip) {
    		  v_clipping = max_clip;
    	  }
      }

      #ifdef DEBUG_RELU
  //    #ifdef DEBUG_VERBOSE
      printf("CLIP(pixel %d, cpo %d): in %f out %f\n", i, cpo, float(v_shift), float(v_clipping));
    //  #endif
      #endif

      // relu
#ifdef USE_RELU
      v_relu = v_clipping;
      if(enable_relu && (v_relu < 0)) v_relu = relu_factor * v_clipping;
#else
      v_relu = v_clipping;
#endif
      data_out.pixel[cpo] = v_relu;

      #ifdef DEBUG_RELU
      //#ifdef DEBUG_VERBOSE
      printf("RELU(pixel %d, cpo %d): in %f out %f\n", i, cpo, float(v_clipping), float(v_relu));
      //#endif
      #endif

    }

    #ifdef DEBUG_RELU
    //#ifdef DEBUG_VERBOSE
    printf("RELU (pixel %d):\n", i);
    for (int x=0; x<CPI; x++) {
    	printf("  cpi %d : in %f out %f\n", x, float(data_in.pixel[x]), float(data_out.pixel[x]));
    }
    //#endif
    #endif

    out << data_out;
}

#ifdef DEBUG_RELU
printf("relu: end\n");
#endif
}
