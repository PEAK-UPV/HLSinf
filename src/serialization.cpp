/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

#ifdef IHW_DATA_FORMAT
// -------------------------------------------------------------------
// serialize and filter
//
// This module performs serialization of the input data as first action. The input
// data is a stream of blocks of items (in). The module reads a block and serializes
// the items. Items are sent through the out stream.
// In addition, the module performs a filter. That is, not all items are forwarded.
// Depending on num_pixels and offset the first and last items are potentially discarded.
// This filtering is needed as input data may be unaligned to block boundaries. This is needed
// to allow block-level reading of input data which is much more efficient than reading
// item by item
//
// channel_blocks parameter indicates how many blocks to expect from the input.
// num_pixels parameter indicates how many pixels (items) need to be forwarded
// offset indicates the first item offset within a block in order to filter out first items
//
void serialize_and_filter(int I_ITER, int num_pixels, int channel_blocks, hls::stream<read_block_t> &in, hls::stream<data_type> &out, int first_channel, int I, int enable) {

  #ifdef DEBUG_SERIALIZE
  printf("SERIALIZE: starts (num_pixels = %d)\n", num_pixels);
  #endif

  int num_pixels_cnt;

  if (!enable) return;

  // Zero block initialization
  read_block_t data_zeros;
  data_zeros = 0;

  int iters = I_ITER * channel_blocks * READ_BLOCK_SIZE;
  int b = 0;
  int p = 0;
  int iter = 0;
  int current_channel = first_channel;

  serialize_and_filter_loop_i_iter:
  for (int i_iter=0; i_iter < iters; i_iter++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=(I_REFERENCE/CPI) * W_REFERENCE * H_REFERENCE)
    #pragma HLS pipeline II=1

    if ((b==0) && (p==0)) {
      num_pixels_cnt = num_pixels;
    }

    read_block_t bx;

    if (p==0) {
      if (current_channel < I) bx = in.read(); else bx = data_zeros;
    }
    if (num_pixels_cnt != 0) {
      ap_int<DATA_TYPE_WIDTH> aux = bx.range(DATA_TYPE_WIDTH-1, 0);

      data_type bx2 = *(data_type *)(&aux);
      out << bx2;
      num_pixels_cnt = num_pixels_cnt - 1;

      bx = bx >> DATA_TYPE_WIDTH;

      #ifdef DEBUG_SERIALIZE
      #ifdef DEBUG_VERBOSE
      printf("SERIALIZE: pixel forwarded %f\n", (float)bx2);
      #endif
      #endif
    }
    p = p + 1;
    if (p == READ_BLOCK_SIZE) {
      p = 0;
      b = b + 1;
      if (b == channel_blocks) {
        b = 0;
        iter = iter + 1;
        current_channel = current_channel + CPI;
      }
    }
  }

  #ifdef DEBUG_SERIALIZE
  printf("SERIALIZE: ends (remaining pixels to send %d)\n", num_pixels_cnt);
  #endif

}
#endif
