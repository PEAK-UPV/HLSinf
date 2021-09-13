//#include "conv2D.h"
#include "lib_conv2D_commons.h"

#ifdef __INTELFPGA_COMPILER__
  #include "HLS/ac_fixed.h"
  #include "HLS/ac_int.h"
#else
  #include "ref/ac_fixed.h"
  #include "ref/ac_int.h"
#endif


#include "HLS/hls.h"
#include "HLS/math.h"
#include "HLS/ocl_types.h"
//#ifndef HLS_SYNTHESIS
//  #include <iostream>
//#endif

#include "HLS/stdio.h"

#ifndef LIB_HEADERS_PRIVATE_H
#define LIB_HEADERS_PRIVATE_H

#define MAX_O_ITERS 4

#ifdef HLS_DEBUG

#define HLS_O_ITER_MONITOR (int)1
//     ihc::stream<pixel_in_t>   dbg_loop_stream_data_in;
//extern ihc::stream<pixel_in_t>   dbg_loop_stream_data_input_buffer;
// direct convolution streams
//extern ihc::stream<pixel_in_t>    dbg_loop_stream_data_dc_pad_out;
//extern ihc::stream<frame_t>       dbg_loop_stream_data_dc_cvt_out;
//extern ihc::stream<pixel_out_t>   dbg_loop_stream_data_dc_mul_out;
////extern ihc::stream<pixel_out_t>    dbg_loop_stream_data_dc_add_out[MAX_O_ITERS]; directconv_out
//extern ihc::stream<pixel_out_t>   dbg_loop_stream_data_directconv_out;

/*
extern "C" {
extern unsigned long dbg_loop_stream_data_dc_pad_out_counter;
extern unsigned long dbg_loop_stream_data_dc_cvt_out_counter;
extern unsigned long dbg_loop_stream_data_dc_mul_out_counter;
extern unsigned long dbg_loop_stream_data_dc_add_out_counter;



extern unsigned long int dbg_elements_per_iter_data_in[MAX_O_ITERS];
extern unsigned long int dbg_elements_per_iter_kernel[MAX_O_ITERS];
extern unsigned long int dbg_elements_per_iter_bias[MAX_O_ITERS];
extern unsigned long int dbg_elements_per_iter_data_input_buffer[MAX_O_ITERS];
extern unsigned long int dbg_elements_per_iter_data_dc_pad_out[MAX_O_ITERS];
extern unsigned long int dbg_elements_per_iter_data_dc_cvt_out[MAX_O_ITERS];
extern unsigned long int dbg_elements_per_iter_data_dc_mul_out[MAX_O_ITERS];
extern unsigned long int dbg_elements_per_iter_data_dc_add_out[MAX_O_ITERS];
extern unsigned long int dbg_elements_per_iter_data_dc_direcconv_out[MAX_O_ITERS];
}

*/
#endif



//extern ihc::stream<pixel_in_t>   out_read_data;
//extern ihc::stream<pixel_in_t>   out_read_data_1;



struct frame_pool_t {
	pixel_out_t pixel[KW_POOLING * KH_POOLING];
};


#endif


