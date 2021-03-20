#include "conv2D.h"

#include <hls_stream.h>

// ---------------------------------------------------------------------------------------
// write_data_channels. Writes data channels from the elements read from an input stream
//
// Arguments:
//   num_pixels          : Number of pixels for each channel
//   ptr                 : pointer to output buffer
//   offset              : offset within output buffer
//   in                  : input streams, one per CPO channel
//   enable              : if not set the module just consumes the input stream and does not write memory, one per CPO channel
//
// On every cycle the module receives BLOCK_SIZE pixels to write into memory
//

void write_data_channels(int num_pixels, ap_uint<512> *ptr, hls::stream<write_block_t> in[CPO], int *enable_write) {

  int num_blocks = (num_pixels + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
  int num_blocks_channel[CPO];
  write_block_t bx;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=num_blocks_channel dim=1)

  write_data_channels_loop_init:
  for (int cpo = 0; cpo < CPO; cpo++) {
	  DO_PRAGMA(HLS UNROLL)
    //  block_offset[cpo] = offset_i[cpo] / WRITE_BLOCK_SIZE;
	  int channel_is_block_aligned = ((num_pixels * cpo) % WRITE_BLOCK_SIZE) == 0;
	  if (channel_is_block_aligned)
		num_blocks_channel[cpo] = num_blocks;
	  else
		num_blocks_channel[cpo] = num_blocks-1;
  }

  #ifdef DEBUG_WRITE_DATA
  printf("WRITE_DATA: starts\n");
  printf("WRITE_DATA: num_pixels %d, num_blocks %d\n", num_pixels, num_blocks);
  for (int cpo=0; cpo<CPO; cpo++) printf("WRITE:DATA num_blocks_channel %d: %d\n", cpo, num_blocks_channel[cpo]);
  #endif

  int cpo = 0;
  int num_its = num_blocks * CPO;
  int num_blocks_ch;
  int read, write;
  int offset;

  write_data_channels_loop_its:
  for (int it = 0; it < num_its; it++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=(W_REFERENCE*H_REFERENCE/WRITE_BLOCK_SIZE) * CPO)
    #pragma HLS pipeline

	num_blocks_ch = (num_blocks_channel[cpo] > 0);
	read = num_blocks_ch;
	write = enable_write[cpo] && num_blocks_ch;

	if (read) bx = in[cpo].read();

	offset = bx.block_offset;

    ap_uint<512> data_out;
    for (int x = 0; x < WRITE_BLOCK_SIZE; x++) {
      DO_PRAGMA(HLS UNROLL)
      int first = x * DATA_TYPE_WIDTH;
      int last = first + DATA_TYPE_WIDTH - 1;
      data_type datum = bx.pixel[x];
      data_out.range(last, first) = *(ap_uint<DATA_TYPE_WIDTH>*)(&datum);
    }

    if (write) ptr[offset] = data_out;
    num_blocks_channel[cpo]--;

    #ifdef DEBUG_WRITE_DATA
    if (write) printf("WRITE_DATA: writing block cpo %d on block address %d\n", cpo, offset);
    #endif

    cpo = (cpo + 1) % CPO;

  }

}
