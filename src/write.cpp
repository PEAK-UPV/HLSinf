#include "conv2D.h"

#include <hls_stream.h>

#ifdef IHW_DATA_FORMAT
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

void write_data_channels(int num_pixels, int channel_offset, write_block_t *ptr, hls::stream<write_block_t> in[CPO], int *enable_write) {

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
#endif

#ifdef GIHWCPI_DATA_FORMAT
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

void write_data_channels_gihwcpi(int num_pixels, int offset, write_block_t *ptr, hls::stream<pixel_out_t> &in) {

  #ifdef DEBUG_WRITE_DATA
  printf("WRITE_DATA: starts (gihwcpi data format)\n");
  printf("WRITE_DATA: num_pixels %d\n", num_pixels);
  #endif

  write_data_channels_loop_pixels:
  for (int i = 0; i < num_pixels; i++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE)
    #pragma HLS pipeline
	//pixel_in_t aux;
	pixel_out_t bx = in.read();
//	for(int cpi=0;cpi<CPI;cpi++){
//		bx.pixel[cpi]=aux.pixel[cpi];
//	}
    ptr[offset+i] = bx;
//    printf("pos %d, valor0 %6.4f, valor1 %6.4f, valor2 %6.4f, valor3 %6.4f\n",
//    		offset+i, float(bx.pixel[0]), float(bx.pixel[1]), float(bx.pixel[2]) ,float(bx.pixel[3]));
  }

}
#endif
