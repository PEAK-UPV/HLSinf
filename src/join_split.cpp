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
void split(int H, int W, int *offset_channel, int *block_offset_channel, hls::stream<pixel_out_t> &in, hls::stream<write_block_t> out[CPO]) {

  #ifdef DEBUG_SPLIT
  printf("DEBUG_SPLIT: starts\n");

  #endif

  int num_pixels = H * W;                                       // pixels to receive per channel
  write_block_t cb_[CPO];										// current block buffer
  write_block_t lb_[CPO];										// last block buffer
  int offset_[CPO];												// block offset for incoming pixel
  int block_addr_[CPO];											// block address for incoming pixel
  int current_block_[CPO];										// current block id being built
  int fpa_[CPO];												// first pixel aligned flag
  pixel_out_t data;												// received pixels
  DO_PRAGMA(HLS ARRAY_PARTITION variable=cb_         complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=lb_         complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=curr_block_ complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=fpa_        complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=data        complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=offset_     complete dim=0)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=block_addr_ complete dim=0)

  // structs initialization
  for (int cpo=0; cpo<CPO; cpo++) {
	  DO_PRAGMA(HLS UNROLL)
	  offset_[cpo] = block_offset_channel[cpo];
	  block_addr_[cpo] = offset_channel[cpo] / WRITE_BLOCK_SIZE;
	  fpa_[cpo] = (offset_[cpo] == 0);
	  current_block_[cpo] = 0;
      #ifdef DEBUG_SPLIT
	  printf("DEBUG_SPLIT: cpo %d -> offset %d fpa %d current_block %d\n", cpo, offset_[cpo], fpa_[cpo], current_block_[cpo]);
      #endif
  }

  // buffers initialization
  split_loop_init_cpo:
  for (int cpo=0; cpo<CPO; cpo++) {
	DO_PRAGMA(HLS UNROLL)
	split_loop_init_p:
	for (int p=0; p<WRITE_BLOCK_SIZE; p++) {
	  DO_PRAGMA(HLS PIPELINE II=1)
	  lb_[cpo].pixel[p] = 0;
	  cb_[cpo].pixel[p] = 0;
	}
  }

  // Now we read input pixels and forward blocks
  split_loop_pixels:
  for (int r=0; r<num_pixels; r++) {
    DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE * H_REFERENCE)
    #pragma HLS PIPELINE

	// We read the pixels
    data = in.read();

    split_loop_cpo_1:
    for(int cpo=0; cpo<CPO; cpo++){
      DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      #pragma HLS UNROLL

  	  int offset = offset_[cpo];
  	  data_type datum = data.pixel[cpo];

	  if (fpa_[cpo]) {
		  cb_[cpo].pixel[offset] = datum;
	  } else {
		  lb_[cpo].pixel[offset] = datum;
	  }

	  if (offset == WRITE_BLOCK_SIZE-1) {

		  if (fpa_[cpo]) {
			  cb_[cpo].block_offset = block_addr_[cpo];
			  out[cpo] << cb_[cpo];
			  current_block_[cpo] = current_block_[cpo] + 1;
              #ifdef DEBUG_SPLIT
              printf("sending block: cpo %d (block address %d) -> ", cpo, block_addr_[cpo]);
              for (int pp=0; pp<WRITE_BLOCK_SIZE; pp++) printf("%6.4f ", float(cb_[cpo].pixel[pp]));
              printf("\n");
              #endif
		  }
		  fpa_[cpo] = 1;
		  block_addr_[cpo] = block_addr_[cpo] + 1;
	  }
      offset_[cpo] = (offset_[cpo] + 1) % WRITE_BLOCK_SIZE;
    }
  }

  // we send last block for each output channel, taking into account
  // possible overlaps between consecutive channels
  split_loop_last_block_cpo:
  for (int cpo=0; cpo<CPO; cpo++) {
	DO_PRAGMA(HLS UNROLL)
	if (offset_[cpo] != 0) {
		int cpo_next = (cpo + 1) % CPO;
  	    split_loop_last_block_p:
		for (int p=offset_[cpo]; p<WRITE_BLOCK_SIZE; p++) {
			DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=WRITE_BLOCK_SIZE)
			DO_PRAGMA(HLS PIPELINE II=1)
			cb_[cpo].pixel[p] = lb_[cpo_next].pixel[p];
		}
		cb_[cpo].block_offset = block_addr_[cpo];
        out[cpo] << cb_[cpo];
        #ifdef DEBUG_SPLIT
        printf("sending block: cpo %d (block address %d) -> ", cpo, block_addr_[cpo]);
        for (int pp=0; pp<WRITE_BLOCK_SIZE; pp++) printf("%6.4f ", cb_[cpo].pixel[pp]);
        printf("\n");
        #endif
	}
  }

}
