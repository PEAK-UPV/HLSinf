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
void add_data(int enable_add, int num_pixels, hls::stream<pixel_out_t> &in_r, hls::stream<pixel_out_t> &in_stm, hls::stream<pixel_out_t> &out) {

  #ifdef DEBUG_ADD_DATA
  printf("add_data: start\n");
  printf("  num_pixels : %d\n", num_pixels);
  #endif

  pixel_out_t data_in_r;
  pixel_out_t data_in_stm;
  pixel_out_t data_out;
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
		  	  #pragma HLS PIPELINE II=1

			  data_type v_in_a, v_in_b, v_out;
			  v_in_a = data_in_r.pixel[cpo];
			  v_in_b = data_in_stm.pixel[cpo];

			  v_out = v_in_a + v_in_b;
			  data_out.pixel[cpo] = v_out;
		  }
	  } else {
		  data_out = data_in_stm;
	  }

	#ifdef DEBUG_ADD_DATA
     printf("ADD_DATA (pixel %d):\n", i);
     for (int x=0; x<CPI; x++) {
      	printf("  cpi %d : in_a %f in_b %f out %f\n", x, float(data_in_r.pixel[x]),float(data_in_stm.pixel[x]), float(data_out.pixel[x]));
     }
	#endif

      out << data_out;

  #ifdef DEBUG_ADD_DATA
  printf("add_data: end\n");
  #endif
  }
}
