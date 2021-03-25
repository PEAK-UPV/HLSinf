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

void write_data_channels(int num_pixels, int channel_offset, ap_uint<512> *ptr, hls::stream<write_block_t> in[CPO], int *enable_write) {

  int num_blocks_per_channel = (num_pixels + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
  write_block_t bx;

  #ifdef DEBUG_WRITE_DATA
  printf("WRITE_DATA: starts\n");
  printf("WRITE_DATA: num_pixels %d, num_blocks_per_channel %d\n", num_pixels, num_blocks_per_channel);
  #endif

  int cpo = 0;
  int offset = channel_offset;
  int num_its = num_blocks_per_channel * CPO;
  int write;

  write_data_channels_loop_its:
  for (int it = 0; it < num_its; it++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=(W_REFERENCE*H_REFERENCE/WRITE_BLOCK_SIZE) * CPO)
    #pragma HLS pipeline

	write = enable_write[cpo];

	bx = in[cpo].read();

	int write_offset = (cpo * num_blocks_per_channel) + offset;

    if (write) ptr[write_offset] = bx; //data_out;

    #ifdef DEBUG_WRITE_DATA
    if (write) printf("WRITE_DATA: writing block cpo %d on block address %d\n", cpo, offset);
    #endif

    cpo = (cpo + 1) % CPO;
    if (cpo==0) offset++;

  }

}
