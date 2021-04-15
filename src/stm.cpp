#include "conv2D.h"
// -------------------------------------------------------------------------------
// STM: TODO
//
// Arguments:
//   H               : Height of the input channel
//   W               : Width of the input channel
//   in              : input data stream
//   out             : output data stream
//
//


void stm(int H, int W, hls::stream<pixel_out_t> &in, hls::stream<pixel_out_t> &out) {

  #ifdef DEBUG_STM
  printf("stm: start\n");
  #endif

  pixel_out_t data_in;
  pixel_out_t data_out;
  int data_size = W * H;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_in complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data_out complete dim=0)

  loop_stm_pixels:
  for (int i=0; i < data_size; i++) {
    DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
    #pragma HLS PIPELINE II=1

	// Let's read the input data
    data_in  = in.read();

    loop_stm_cpo:
    for(int cpo = 0; cpo<CPO; cpo++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      #pragma HLS UNROLL

	  //TODO cambiar el tipo
	  data_type v_in, v_soft, v_tanh, v_mult;
	  //float v_in, v_soft, v_tanh, v_mult;

      v_in = data_in.pixel[cpo];

      // softplus
      v_soft = hls::log(hls::exp(v_in) + 1);

      //tanh
      v_tanh = hls::tanh(v_soft);

      //mult
      v_mult = v_tanh * v_in;

      data_out.pixel[cpo] = v_mult;

    }

    #ifdef DEBUG_STM
    printf("STM (pixel %d):\n", i);
    /*for (int x=0; x<CPI; x++) {
    	printf("  cpi %d : in %f out %f\n", x, float(data_in.pixel[x]), float(data_out.pixel[x]));
    }*/
    #endif
    out << data_out;
}

#ifdef DEBUG_RELU
printf("stm: end\n");
#endif
}
