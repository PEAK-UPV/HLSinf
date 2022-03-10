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
//   in                  : input streams, one per CPO channel
//   enable              : if not set the module just consumes the input stream and does not write memory, one per CPO channel
//
// On every cycle the module receives BLOCK_SIZE pixels to write into memory
//

void write_data_channels_gihwcpi(int num_pixels, int offset, write_block_t *ptr, hls::stream<dout_st> &in, int write_to_obuf, hls::stream<dout_st> &out) {

  #ifdef DEBUG_WRITE_DATA
  printf("WRITE_DATA: starts (gihwcpi data format)\n");
  printf("  num_pixels %d\n", num_pixels);
  printf("  offset %d\n", offset);
  printf("  write_to_obuf %d\n", write_to_obuf);
  #endif

  if (write_to_obuf) {
    write_to_obuff_loop_pixels:
    for (int i = 0; i < num_pixels; i++) {
	    DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE)
      #pragma HLS pipeline
	    dout_st bx = in.read();
      out << bx;
    }
  } else {
    write_data_channels_loop_pixels:
    for (int i = 0; i < num_pixels; i++) {
	  DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE)
      #pragma HLS pipeline
	  dout_st bx = in.read();
      ptr[offset+i] = bx;
    }
  }
}
