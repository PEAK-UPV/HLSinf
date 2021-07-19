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
void padding(int H, int W, int PT, int PB, int PL, int PR, int I_ITER, hls::stream<pixel_in_t> &in, hls::stream<pixel_in_t> &out) {

  #ifdef DEBUG_PADDING
  printf("PADDING: start\n");
  #endif

  int num_iters;
  int h;
  int w;
  pixel_in_t data;
  pixel_in_t zero;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data complete)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=zero complete)

  padding_cpi_loop:
  for (int cpi=0; cpi<CPI; cpi++){
    DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
    #pragma HLS UNROLL
    zero.pixel[cpi] = 0.f;
  }

  num_iters = I_ITER * (H + PT + PB) * (W + PL + PR);
  h = 0;
  w = 0;
  padding_loop:
  for (int i = 0; i < num_iters; i++) {
	DO_PRAGMA(HLS loop_tripcount min=1 max=(I_REFERENCE/CPI) * W_REFERENCE * H_REFERENCE)
    #pragma HLS pipeline II=1
    int enable1 = h<PT;
    int enable2 = h >= H+PT;
    int enable3 = w < PL;
    int enable4 = (w >= W+PL);
    if (enable1 | enable2 | enable3 | enable4) data = zero; else data = in.read();
    out << data;
#ifdef DEBUG_PADDING
	printf("PADDING: send data (i %d, h %d, w %d, |enableX %d)\n", i, h, w, enable1|enable2|enable3|enable4);
#endif
	w = w+1;
	if (w == W+PL+PR) {
	  w = 0;
	  h = h + 1;
	  if (h == H+PT+PB) {
		h = 0;
	  }
	}
  }

  #ifdef DEBUG_PADDING
  printf("PADDING: end\n");
  #endif
}
