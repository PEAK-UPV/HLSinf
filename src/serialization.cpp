#include "conv2D.h"

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
void serialize_and_filter(int I_ITER, int num_pixels, int channel_blocks, int channel_size, int offset, hls::stream<read_block_t> &in, hls::stream<data_type> &out, int first_channel, int I) {

  #ifdef DEBUG_SERIALIZE
  printf("SERIALIZE: starts (num_pixels = %d)\n", num_pixels);
  #endif

  int num_pixels_cnt;

  // Zero block initialization
  read_block_t data_zeros;
  for (int b=0; b<READ_BLOCK_SIZE; b++) {
    #pragma HLS UNROLL
    data_zeros.pixel[b] = 0;
  }

  int iters = I_ITER * channel_blocks * READ_BLOCK_SIZE;
  int b = 0;
  int p = 0;
  int iter = 0;
  int offset_ch = 0;
  int current_channel = first_channel;
  for (int i_iter=0; i_iter < iters; i_iter++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI * READ_BLOCK_SIZE * (W_REFERENCE * H_REFERENCE / READ_BLOCK_SIZE))
    #pragma HLS pipeline II=1
    // offset
    if ((b==0) && (p==0)) {
      offset_ch = (offset + (channel_size * CPI * iter)) % READ_BLOCK_SIZE;
      num_pixels_cnt = num_pixels;
    }
    read_block_t bx;
    DO_PRAGMA(HLS ARRAY_PARTITION variable=bx dim=0 complete)
    if (p==0) {
      if (current_channel < I) bx = in.read(); else bx = data_zeros;
    }
    if ((offset_ch==0) && (num_pixels_cnt !=0)) {
      out << bx.pixel[p];
      num_pixels_cnt = num_pixels_cnt - 1;
      #ifdef DEBUG_SERIALIZE
      printf("SERIALIZE: pixel forwarded %f\n", (float)bx.pixel[p]);
      #endif
    } else {
      offset_ch = offset_ch - 1;
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

