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


void buffer1(int num_accesses, int read, int write, int first_write_address, hls::stream<dout_st> &in, hls::stream<din_st> &out) {

  static din_st buff1[DATA_BUFFER_SIZE];
  DO_PRAGMA(HLS aggregate    variable=buff1)
  #ifdef ALVEO_U200
  DO_PRAGMA(HLS bind_storage variable=buff1 type=ram_s2p impl=uram)
  #endif
  #ifdef ALVEO_U280
  DO_PRAGMA(HLS bind_storage variable=buff1 type=ram_s2p impl=uram)
  #endif

#ifdef DEBUG_BUFFER
  printf("BUFFER1: [START]\n");
  printf("BUFFER1: num_accesses: %d, read %d\n", num_accesses, read);
#endif

  if (read) {
	  int read_ptr = 0;
	  for (int I=0; I<num_accesses; I++) {
        #pragma HLS PIPELINE II=1
		din_st PX = buff1[read_ptr];
		out << PX;
		read_ptr++;
	  }
  }

  if (write) {
	  int write_ptr = first_write_address;
	  for (int I=0; I<num_accesses; I++) {
        #pragma HLS PIPELINE II=1
		dout_st PX = in.read();
		din_st PX2 = *(din_st *)&PX;
		buff1[write_ptr] = PX2;
		write_ptr++;
	  }
  }

#ifdef DEBUG_BUFFER
  printf("BUFFER1: [END]\n");
#endif

}

void mux(int num_accesses, int SEL, hls::stream<din_st> &in0, hls::stream<din_st> &in1, hls::stream<din_st> &out) {
	for (int I = 0; I < num_accesses; I++) {
        #pragma HLS PIPELINE II=1
		din_st PX;
		if (SEL == 0) PX = in0.read(); else PX = in1.read();
		out << PX;
	}
}

void demux(int ENABLE, int num_accesses, int SEL, hls::stream<dout_st> &IN, hls::stream<dout_st> &out0, hls::stream<dout_st> &out1) {

	if (!ENABLE) return;

	for (int I = 0; I < num_accesses; I++) {
        #pragma HLS PIPELINE II=1
		dout_st PX = IN.read();
		if (SEL == 0) out0 << PX; else out1 << PX;
	}
}

void buffer0(int num_accesses, int read, int read_from_input, int write, int first_write_address, hls::stream<din_st> &input, hls::stream<dout_st> &in, hls::stream<din_st> &out) {

  // buffer
  static din_st buff0[DATA_BUFFER_SIZE];
  DO_PRAGMA(HLS aggregate    variable=buff0)
  #ifdef ALVEO_U200
  DO_PRAGMA(HLS bind_storage variable=buff0 type=ram_s2p impl=uram)
  #endif
  #ifdef ALVEO_U280
  DO_PRAGMA(HLS bind_storage variable=buff0 type=ram_s2p impl=uram)
  #endif

#ifdef DEBUG_BUFFER
  printf("BUFFER0: [start]\n");
  printf("BUFFER0: num_accesses: %d, write %d, read %d, read_from_input %d\n", num_accesses, write, read, read_from_input);
#endif

  if (read_from_input) {
	  int write_ptr = 0;
	  for (int i=0; i<num_accesses; i++) {
        #pragma HLS pipeline II=1
		din_st px = input.read();
		out << px;
		if (write) {buff0[write_ptr] = px; write_ptr++;}
	  }
  } else {
    if (write) {
	  int write_ptr = first_write_address;
	  for (int i=0; i<num_accesses; i++) {
        #pragma HLS pipeline II=1
        dout_st px2 = in.read();
		din_st px = *(din_st *)&px2;
		buff0[write_ptr] = px;
		write_ptr++;
	  }
    }
  }

  if (read) {
	  int read_ptr = 0;
	  for (int i=0; i<num_accesses; i++) {
        #pragma HLS pipeline II=1
		din_st px = buff0[read_ptr];
		out << px;
		read_ptr++;
	  }
  }

#ifdef DEBUG_BUFFER
  printf("BUFFER0: [end]\n");
#endif

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
  //printf("WEIGHT_BUFFER: sizeof %l\n", sizeof(buffer));
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
