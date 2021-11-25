#include "conv2D.h"

#include <hls_stream.h>

void input_buffer(int read_pixels_total, int write_to_buff, int read_from_buff, hls::stream<din_st> &in, hls::stream<din_st> &out) {

  #ifdef DEBUG_INPUT_BUFFER
  printf("INPUT_BUFFER: starts (%d pixels; write_to_buff %d; read_from_buff %d)\n", read_pixels_total, write_to_buff, read_from_buff);
  #endif

  din_st px_input;
  din_st px_buff;
  DO_PRAGMA(HLS AGGREGATE variable=px_input)
  DO_PRAGMA(HLS AGGREGATE variable=px_buff)

  din_st buffer[INPUT_BUFFER_SIZE];
  DO_PRAGMA(HLS aggregate variable=buffer)

  #ifdef ALVEO_U200
  DO_PRAGMA(HLS bind_storage variable=buffer type=ram_t2p impl=uram)
  #endif
  #ifdef ALVEO_U280
  DO_PRAGMA(HLS bind_storage variable=buffer type=ram_t2p impl=uram)
  #endif

  input_buffer_loop_pixels:
  for (int p=0; p<read_pixels_total; p++) {
	DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE * W_REFERENCE * H_REFERENCE / CPI)
    DO_PRAGMA(HLS pipeline)

	if (!read_from_buff) px_input = in.read();
	if (read_from_buff)  px_buff = buffer[p];
	if (write_to_buff)   buffer[p] = px_input;

	if (read_from_buff)  out << px_buff;
	else out << px_input;
  }
  #ifdef DEBUG_INPUT_BUFFER
  printf("INPUT_BUFFER: ends\n");
  #endif
}

