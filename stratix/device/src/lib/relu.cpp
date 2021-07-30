#include "lib_hdrs_priv.h"

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

static unsigned long relu(int enable_relu, int enable_clipping, int enable_shift, int min_clip, int max_clip, int direction_shift, int pos_shift, int H, int W ) {

  #ifdef DEBUG_RELU
  printf("relu: start\n");
  #endif

  unsigned long cnt = 0;

  int data_size = W * H;
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in complete dim=0)
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=data_out complete dim=0)

  loop_relu_pixels:
  for (int i=0; i < data_size; i++) {
    pixel_out_t data_in;
    pixel_out_t data_out;
  
    //DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
    //#pragma HLS PIPELINE II=1

    // Let's read the input data
    data_in  = out_conv.read();

    loop_relu_cpo:
    #pragma unroll
    for(int cpo = 0; cpo<CPO; cpo++){
      //DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      //#pragma HLS UNROLL

	    data_type v_in, v_shift, v_clipping, v_relu;

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

      // clipping
#ifdef USE_CLIPPING
      v_clipping = v_shift;
      if (enable_clipping) {
    	  if (v_shift < min_clip) {
    		  v_clipping = min_clip;
    	  } else if (v_shift > max_clip) {
    		  v_clipping = max_clip;
    	  }
      }
#else
      v_clipping = v_shift;
#endif

      // relu
#ifdef USE_RELU
      v_relu = v_clipping;
      if(enable_relu && (v_relu < 0)) v_relu = 0;
#else
      v_relu = v_clipping;
#endif
      data_out.pixel[cpo] = v_relu;

    }

    //#ifdef DEBUG_RELU
    //#ifdef DEBUG_VERBOSE
    //printf("RELU (pixel %d):\n", i);
    //for (int x=0; x<CPI; x++) {
    //	printf("  cpi %d : in %f out %f\n", x, float(data_in.pixel[x]), float(data_out.pixel[x]));
    //}
    //#endif
    //#endif

    out_relu.write(data_out);
    cnt = cnt + 1;
}

//#ifdef DEBUG_RELU
//printf("relu: end\n");
//#endif
  return cnt;
}
