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

void write_data_channels(int num_pixels, ap_uint<512> *ptr, int *offset_i, hls::stream<write_block_t> in[CPO], int *enable_write) {

  int num_blocks = (num_pixels + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;

  #ifdef DEBUG_WRITE_DATA
  printf("WRITE_DATA: starts\n");
  printf("WRITE_DATA: num_pixels %d, num_blocks %d\n", num_pixels, num_blocks);
  #endif

write_block_t bx[CPO];
  int block_offset[CPO];
  DO_PRAGMA(HLS ARRAY_PARTITION variable=bx complete)
  DO_PRAGMA(HLS ARRAY_PARTITION variable=block_offset complete)

  write_data_channels_loop_init:
  for (int cpo=0; cpo<CPO; cpo++) {
	DO_PRAGMA(HLS UNROLL)
	block_offset[cpo] = offset_i[cpo] / WRITE_BLOCK_SIZE;
    #ifdef DEBUG_WRITE_DATA
	printf("WRITE_DATA: cpo %d -> offset %d\n", cpo, block_offset[cpo]);
    #endif
  }

  write_data_channels_loop_blocks:
  for (int p = 0; p < num_blocks; p++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE*H_REFERENCE/WRITE_BLOCK_SIZE)
	write_data_channels_loop_cpo:
	for (int cpo=0; cpo<CPO; cpo++) {
      #pragma HLS pipeline II=1
      bx[cpo] = in[cpo].read();
      if (enable_write[cpo]) {
    	ap_uint<512> data_out;
    	for (int x = 0; x < WRITE_BLOCK_SIZE; x++) {
    		DO_PRAGMA(HLS UNROLL)
    		int first = x * DATA_TYPE_WIDTH;
    		int last = first + DATA_TYPE_WIDTH - 1;
    		data_type datum = bx[cpo].pixel[x];
    		data_out.range(last, first) = *(ap_uint<DATA_TYPE_WIDTH>*)(&datum);
    	}
        ptr[block_offset[cpo]+p] = data_out;
        #ifdef DEBUG_WRITE_DATA
        printf("WRITE_DATA: writing block cpo %d\n", cpo);
        #endif
      }
    }
  }
}
