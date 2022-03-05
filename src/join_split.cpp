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

void input_buffer(int read_pixels_total, int write_to_buff, int read_from_buff, int copy_from_obuf, hls::stream<din_st> &in, hls::stream<din_st> &in_obuf, hls::stream<din_st> &out) {

  din_st px_input;
  din_st px_buff;
  DO_PRAGMA(HLS AGGREGATE variable=px_input)
  DO_PRAGMA(HLS AGGREGATE variable=px_buff)

  static din_st buffer[INPUT_BUFFER_SIZE];
  DO_PRAGMA(HLS aggregate variable=buffer)
  #ifdef ALVEO_U200
  DO_PRAGMA(HLS bind_storage variable=buffer type=ram_t2p impl=uram)
  #endif
  #ifdef ALVEO_U280
  DO_PRAGMA(HLS bind_storage variable=buffer type=ram_t2p impl=uram)
  #endif

  #ifdef DEBUG_INPUT_BUFFER
  printf("INPUT_BUFFER: starts (%d pixels; write_to_buff %d; read_from_buff %d. Input buffer size %d)\n", read_pixels_total, write_to_buff, read_from_buff, INPUT_BUFFER_SIZE);
  printf("INPUT_BUFFER: copy from obuf %d\n", copy_from_obuf);
  printf("INPUT_BUFFER: sizeof %d\n", sizeof(buffer));
  #endif

  input_buffer_loop_pixels:
  for (int p=0; p<read_pixels_total; p++) {
	DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE * W_REFERENCE * H_REFERENCE / CPI)
    DO_PRAGMA(HLS pipeline)

	if (!read_from_buff) if (!copy_from_obuf) px_input = in.read(); else px_input = in_obuf.read();
	if (read_from_buff)  px_buff = buffer[p];

	if (write_to_buff)  {
    buffer[p] = px_input;

    #ifdef DEBUG_INPUT_BUFFER
    printf("BUFFER: write to buff p=%d ", p);
    for (int x=0; x<CPI; x++) printf("%f ", float(px_input.pixel[x]));
    printf(" FROM BUFF: ");
    for (int x=0; x<CPI; x++) printf("%f ", float(buffer[p].pixel[x]));
    printf("\n");
    #endif
  }


	if (read_from_buff)  {
    out << px_buff;

    #ifdef DEBUG_INPUT_BUFFER
    printf("BUFFER: read from buff p=%d ", p);
    for (int x=0; x<CPI; x++) printf("%f ", float(px_buff.pixel[x]));
    printf("\n");
    #endif
  }
	else out << px_input;
  }
  #ifdef DEBUG_INPUT_BUFFER
  printf("INPUT_BUFFER: ends\n");
  #endif
}

void output_buffer(int num_writes, int num_reads, int write, int read, hls::stream<dout_st> &in, hls::stream<din_st> &out) {

  din_st px_input;
  din_st px_buff;
  DO_PRAGMA(HLS AGGREGATE variable=px_input)
  DO_PRAGMA(HLS AGGREGATE variable=px_buff)

  static din_st buffer[INPUT_BUFFER_SIZE];
  DO_PRAGMA(HLS aggregate variable=buffer)
  #ifdef ALVEO_U200
  DO_PRAGMA(HLS bind_storage variable=buffer type=ram_t2p impl=uram)
  #endif
  #ifdef ALVEO_U280
  DO_PRAGMA(HLS bind_storage variable=buffer type=ram_t2p impl=uram)
  #endif

  #ifdef DEBUG_OUTPUT_BUFFER
  printf("OUTPUT_BUFFER: starts (%d num_writes; %d num_reads, write %d, read %d\n", num_writes, num_reads, write, read);
  #endif

  int remaining_reads  = read  ? num_reads  : 0;
  int remaining_writes = write ? num_writes : 0;
  int read_ptr  = 0;
  int write_ptr = 0;
  int successful_write = 0;
  int successful_read = 0;
  dout_st pxx;
  din_st px2;

  while (remaining_reads || remaining_writes) {
    #pragma HLS pipeline II=1
    #pragma HLS dependence variable=buffer inter false

    din_st px = buffer[read_ptr];

    if (remaining_reads) successful_write = !out.full(); else successful_write = 0;
    if (remaining_writes) successful_read = !in.empty(); else successful_read = 0;
    //
    if (successful_read) pxx = in.read();
    if (successful_read) { px2 = *(din_st *)&pxx; buffer[write_ptr] = px2; }
    if (successful_read) {
      remaining_writes--; write_ptr++;
      #ifdef DEBUG_OUTPUT_BUFFER
      printf("write performed (remaining %d)\n", remaining_writes);
      #endif
    }
    //
    if (successful_write) {
      out.write(px);
    }

    if (successful_write) {
      #ifdef DEBUG_OUTPUT_BUFFER
      printf("read performed (remaining %d)\n", remaining_reads);
      #endif
      remaining_reads--; read_ptr++;
    }
  }
}

void weight_buffer(int I_ITER, int write_to_buff, int read_from_buff, int offset_buff, hls::stream<w_st> &in, hls::stream<w_st> &out) {

  w_st px_input;
  w_st px_buff;
  DO_PRAGMA(HLS AGGREGATE variable=px_input)
  DO_PRAGMA(HLS AGGREGATE variable=px_buff)

  static w_st buffer[WEIGHT_BUFFER_SIZE];
  DO_PRAGMA(HLS aggregate variable=buffer)
  #ifdef ALVEO_U200
  DO_PRAGMA(HLS bind_storage variable=buffer type=ram_t2p impl=uram)
  #endif
  #ifdef ALVEO_U280
  DO_PRAGMA(HLS bind_storage variable=buffer type=ram_t2p impl=uram)
  #endif

  #ifdef DEBUG_WEIGHT_BUFFER
  printf("WEIGHT_BUFFER: starts (%d iters; write_to_buff %d; read_from_buff %d. weight buffer size %d)\n", I_ITER, write_to_buff, read_from_buff, WEIGHT_BUFFER_SIZE);
  printf("WEIGHT_BUFFER: sizeof %d\n", sizeof(buffer));
  #endif

  weight_buffer_loop_pixels:
  for (int p=0; p<I_ITER; p++) {
	  DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE / CPI)
    DO_PRAGMA(HLS pipeline)
    #pragma HLS dependence variable=buffer inter false

    int index = p+offset_buff;

	  if (!read_from_buff) px_input = in.read();
	  px_buff = buffer[index];

  	if (read_from_buff) out << px_buff;	else out << px_input;

 	  if (write_to_buff) buffer[index] = px_input;

  }
  #ifdef DEBUG_WEIGHT_BUFFER
  printf("WEIGHT_BUFFER: ends\n");
  #endif
}
