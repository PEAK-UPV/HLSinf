/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

#include <hls_stream.h>

// ---------------------------------------------------------------------------------------
// write_data_channels_gihwcpi. Writes data channels from the elements read from the stream
// Expected output format is GIxHxWxCPI
//
// Arguments:
//   num_pixels          : Number of pixels for each channel
//   ptr                 : pointer to output buffer
//   offset              : offset within output buffer
//   enable_outstream    : if enabled write the output to the stream instead of the memory interface
//   in                  : input streams, one per CPO channel
//   enable              : if not set the module just consumes the input stream and does not write memory, one per CPO channel
//
// On every cycle the module receives BLOCK_SIZE pixels to write into memory
//

void write_data(int num_pixels, int O_ITER, int offset, int enable_outstream, write_block_t *ptr, hls::stream<write_block_t> &output_stream, hls::stream<dout_st> &in) {

  #ifdef DEBUG_WRITE_DATA
  printf("WRITE_DATA: starts (gihwcpi data format)\n");
  printf("  num_pixels %d\n", num_pixels);
  printf("  offset %d\n", offset);
  printf("  enable_outstream %d\n", enable_outstream);
  #endif

  uint total_num_pixels = num_pixels * O_ITER;

  write_data_channels_loop_pixels:
  for (int i = 0; i < total_num_pixels; i++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE)
    #pragma HLS pipeline

	if (enable_outstream) {
		output_stream << in.read();
    } else {
    	dout_st bx = in.read();
    	ptr[offset+i] = bx;
    }

    #ifdef DEBUG_WRITE_DATA
    printf("data write : %d : ", i);
    for (int x=0; x<CPO; x++) printf("%f ", float(bx.pixel[x]));
    printf("\n");
    #endif
  }

}
