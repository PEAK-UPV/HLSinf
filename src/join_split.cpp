#include "conv2D.h"

#include <hls_stream.h>


// ---------------------------------------------------------------------------------------
// join. Joins input streams of pixels and combines them to produce groups of pixels
//
// Arguments:
//   H, W                : Data channel height and width
//   I_ITER              : Number of input iterations (I / CPI)
//   in                  : input streams
//   out                 : output stream
//
// The input streams have width of BLOCK_SIZE elements whereas the output stream
// has width of CPI elements. This module gets the first elements of all input
// streams and produces an output data, then it takes the second elements of all
// input streams and produces a new output data, and so on... For every received
// input data from all streams the join module uses BLOCK_SIZE cycles to produce
// BLOCK_SIZE data items. All data items are sent through the output stream
//
void join(int H, int W, int I_ITER, int num_extra_rows, hls::stream<data_type> in[CPI], hls::stream<pixel_in_t> &out) {

  #ifdef DEBUG_JOIN
  printf("JOIN: starts\n");
  #endif

  int num_pixels = (H + num_extra_rows) * W;                    // pixels to read

  #ifdef DEBUG_JOIN
  printf("JOIN: Expected pixels = %d\n", num_pixels);
  #endif

  for (int i_iter = 0; i_iter < I_ITER; i_iter++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)

    join_loop:
    for (int r=0; r<num_pixels; r++) {
      DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
      #pragma HLS PIPELINE II=1
      pixel_in_t data;
      DO_PRAGMA(HLS ARRAY_PARTITION variable=data complete dim=0)
      for(int i=0; i<CPI; i++){
        DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
        #pragma HLS UNROLL
        data.pixel[i] = in[i].read();
        #ifdef DEBUG_JOIN
        printf("data.pixel[%d] = %6.2f  ", i, float(data.pixel[i]));
        #endif
      }
      #ifdef DEBUG_JOIN
      printf("\n");
      #endif
      out << data;
    }
  }

  #ifdef DEBUG_JOIN
  printf("JOIN: ends\n");
  #endif
}

// ---------------------------------------------------------------------------------------
// split. Splits incomming pixels grouped in pixel_out_t struct into eight output streams
// of size BLOCK_SIZE elements each.
//
// Arguments:
//   H, W                : data channel height and width
//   in                  : input stream
//   out0, ... out7      : output streams
//
// The input stream has CPO pixels per data item whereas each output stream has
// BLOCK_SIZE pixels per data item. Therefore, this module reads during BLOCK_SIZE
// cycles the input stream and assigns each pixel from each read data item into
// every output data item to be sent. After those cycles the out data items are
// sent through the corresponding output stream
//
void split(int H, int W, hls::stream<pixel_out_t> &in, hls::stream<data_type> out[CPO]) {

  #ifdef DEBUG_SPLIT
  printf("DEBUG_SPLIT: starts\n");
  #endif

  int num_pixels = H * W;                                       // pixels to receive per channel
  pixel_out_t data;												// received pixels
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data        complete dim=0)

  // We read input pixels and forward blocks
  split_loop_pixels:
  for (int r=0; r<num_pixels; r++) {
    DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE * H_REFERENCE)
    #pragma HLS PIPELINE

	// We read the pixels
    data = in.read();

    split_loop_cpo_1:
    for(int cpo=0; cpo<CPO; cpo++) {
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      #pragma HLS UNROLL

  	  data_type datum = data.pixel[cpo];

      out[cpo] << datum;
    }
  }

  #ifdef DEBUG_SPLIT
  printf("DEBUG_SPLIT: starts\n");
  #endif

}

void block_generate(int H, int W, hls::stream<data_type> &in, hls::stream<write_block_t> &out) {

#ifdef DEBUG_BLOCK
printf("DEBUG_BLOCK: starts\n");
#endif

  write_block_t bx;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=bx complete dim=0)

  int offset = 0;
  int num_pixels = H * W;
  int num_blocks = (num_pixels + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
  block_generate_blocks:
  for (int b=0; b < num_blocks; b++) {
	DO_PRAGMA(HLS loop_tripcount min=1 max=(W_REFERENCE * H_REFERENCE) / WRITE_BLOCK_SIZE)
    block_generate_pixels:
    for (int p=0; p < WRITE_BLOCK_SIZE; p++) {
	  DO_PRAGMA(HLS PIPELINE)
	  data_type dx;
#ifdef DEBUG_BLOCK
	  printf("to read pixel, pending %d\n", num_pixels);
#endif
	  if (num_pixels) dx = in.read();
#ifdef DEBUG_BLOCK
	  printf("read pixel, pending %d\n", num_pixels);
#endif
      int first = p * DATA_TYPE_WIDTH;
      int last = first + DATA_TYPE_WIDTH - 1;
      bx.range(last, first) = *(ap_uint<DATA_TYPE_WIDTH>*)(&dx);
	  if (num_pixels) num_pixels--;
    }
    out << bx;
  }

#ifdef DEBUG_BLOCK
printf("DEBUG_BLOCK: ends\n");
#endif

}
