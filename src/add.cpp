#include "conv2D.h"

// -------------------------------------------------------------------------------
// add: This function performs the addition of all subpixels for the same channel.
// It adds also the corresponding bias.
//
// Arguments:
//   H     : Height of input channel
//   W     : Width of input channel
//   I_ITER: Number of input iterations (I / CPI)
//   in    : input streams data
//   b_in  : input stream bias
//   out   : output stream
//
void add(int H, int W, int I_ITER, hls::stream<pixel_out_t> &in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out) {

  #ifdef DEBUG_ADD
  printf("add: start\n");
  #endif

  pixel_out_t bias;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=bias dim=0 complete)

  // number of iterations by CPI || CPO channels
  int num_iterations = W * H;

  // Buffer for all data and CPO channels
  static pixel_out_t buff_o_channels[WMAX*HMAX];
  #ifndef __VIVADO_HLS__
  DO_PRAGMA(HLS AGGREGATE variable=buffer_o_channels)
  #else
  DO_PRAGMA(HLS data_pack variable=buffer_o_channels)
  #endif
  #ifdef ALVEO_U200
  DO_PRAGMA(HLS bind_storage variable=buffer_o_channels type=ram_t2p impl=uram)
  #endif


  // We receive bias in packs of CPO
  bias = b_in.read();

  #ifdef DEBUG_ADD
  #ifdef DEBUG_VERBOSE
  for (int b=0; b<CPO; b++) {
    printf("Bias[%d] = %6.4f \n", b, float(bias.pixel[b]));
  }
  printf("add: bias received\n");
  for(int cpo = 0; cpo<CPO; cpo++){
    printf("Channel cpo = %d: ", cpo);
    for(int it = 0; it<num_iterations; it++){
      printf("%6.2f ", float(buff_o_channels[it].pixel[cpo]));
    }
    printf("\n");
  }
  #endif
  #endif

  // All input data have effect into output add
  add_i_iter_loop:
  for (int i_iter = 0; i_iter < I_ITER; i_iter++){
    DO_PRAGMA(HLS loop_tripcount  min=1 max=I_REFERENCE/CPI)
    pixel_out_t data_out;
    #pragma HLS loop_flatten off
    add_load_data_it_loop:
    for(int it = 0; it<num_iterations; it++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
      pixel_out_t px;
      px = in.read();
      pixel_out_t data_in;
      pixel_out_t data_out;

      if (i_iter == 0) data_in = bias; else data_in = buff_o_channels[it];

      add_load_data_cpo_loop:
      for (int cpo=0; cpo<CPO; cpo++) {
        DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
        #pragma HLS unroll
        data_out.pixel[cpo] = data_in.pixel[cpo] + px.pixel[cpo];
      }
      buff_o_channels[it] = data_out;

      if(i_iter ==(I_ITER-1)){

        out << data_out;
      }
    }
  } //i_iter

  #ifdef DEBUG_ADD
  #ifdef DEBUG_VERBOSE
  for (int cpo=0; cpo<CPO; cpo++) {
    printf("CH %d: ", cpo);
    for (int it=0; it<num_iterations; it++) {
      printf("%6.2f ", float(buff_o_channels[it].pixel[cpo]));
    }
    printf("\n");
  }
  #endif
  #endif

  #ifdef DEBUG_ADD
  printf("add: end\n");
  #endif
}
