#include "conv2D.h"

// ---------------------------------------------------------------------------------------
// padding. Adds padding to the input and forwards it through the output
//
// Arguments:
//   H                 : Height of input channel
//   W                 : Width of input channel
//   I_ITER            : Number of input iterations (I / CPI)
//   in                : input stream
//   out               : output stream
//
void padding(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<pixel_in_t> &out) {

  #ifdef DEBUG_PADDING
  printf("PADDING: start\n");
  #endif

  int num_iters;
  int h;
  int w;
  const int W1 = W + 1;
  const int H1 = H + 1;
  pixel_in_t data;
  pixel_in_t zero;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data complete)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=zero complete)

  padding_cpi_loop:
  for (int cpi=0; cpi<CPI; cpi++){
    #pragma HLS UNROLL
    DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
    zero.pixel[cpi] = 0.f;
  }

  num_iters = I_ITER * (H + 2) * (W + 2);
  h = 0;
  w = 0;
  padding_loop:
  for (int i = 0; i < num_iters; i++) {
	DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE/CPI*W_REFERENCE*H_REFERENCE)
    #pragma HLS pipeline II=1
    int enable1 = enable_upper_padding & (h==0);
	int enable2 = enable_lower_padding & (h == H1); //(h == H+1);
	int enable3 = (w == 0);
	int enable4 = (w == W+1);
	if (enable1 | enable2 | enable3 | enable4) data = zero; else data = in.read();
    out << data;

    if (w == W1) {
    	w = 0;
    	if (h == H1) {
    		h = 0;
    	} else {
    		h++;
    	}
    } else {
    	w++;
    }
	//w = w+1;
	//if (w == W+2) {
	//  w = 0;
	//  h = h + 1;
	//  if (h == H+2) {
	//	h = 0;
	//  }
	//}
  }

  #ifdef DEBUG_PADDING
  printf("PADDING: end\n");
  #endif
}
