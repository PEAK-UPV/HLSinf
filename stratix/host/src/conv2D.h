#ifndef _CONV2D_H_
#define _CONV2D_H_

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY

#define OPENCL_TEST


#ifdef OPENCL_TEST
  #ifdef __INTELFPGA_COMPILER__
    #include "HLS/ac_fixed.h"
    #include "HLS/ac_int.h"
  #else
    #include "ref/ac_fixed.h"
    #include "ref/ac_int.h"
  #endif

#endif


#include <stdio.h>
//#include <ap_fixed.h>    XILINX
//#include <ap_int.h>      XILINX
//#include <hls_stream.h>  XILINX
#ifdef OPENCL_TEST
  #include <linux/limits.h>
  #include <unistd.h>
  #include <assert.h>
  #include <math.h>
  #include <vector>

  #include "CL/opencl.h"
  #include <CL/cl2.hpp>
  #include <CL/cl.hpp>
  #include "CL/cl_ext.h"
  #include "AOCLUtils/aocl_utils.h"


  using namespace aocl_utils;
  using namespace std;
  using std::vector;

#endif
  
#include "conv2D_commons.h"

// What follows is the function prototypes

// Function prototypes protected with the no-definition of OPENCL_TEST to avoid warnings when compiling for OpenCL
#ifndef OPENCL_TEST

// -----------------------------------------------------------------------------------------------------------
// function prototypes
extern "C" void k_conv2D(read_block_t *ptr_data, int H, int W, int rows, int I, int O, int I_ITER, int o_iter_first, int o_iter_last, int enable_relu,
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
                         data_type *ptr_kernel,
#endif
#ifdef DWS_CONV
						 data_type *ptr_dw_kernel, read_kernel_pw_t *ptr_pw_kernel,
#endif
						 pixel_out_t *ptr_bias, write_block_t *ptr_out, int global_offset, int enable_upper_padding,
						 int enable_lower_padding, int enable_maxpooling, int enable_avgpooling,
						 int enable_clipping, int enable_shift, int min_clip, int max_clip, int dir_shift, int pos_shift, 
             unsigned long int my_val, unsigned long *my_ret, unsigned long *my_ret_2, unsigned long *my_ret_3, unsigned long *my_ret_4
             );


// read and write functions
void read_bias(int offset_bias, struct pixel_out_t *b_ptr, hls::stream<struct pixel_out_t> &out);
void read_kernel(int I_ITER, int offset_kernel, data_type *k_ptr, hls::stream<kernel_t> &k_out);

#ifdef IHW_DATA_FORMAT
void read_data_channels(int H, int W, int rows, int I_ITER, read_block_t *ptr, int offset, int num_extra_rows, int channel_blocks, hls::stream<read_block_t> out[CPI], int I, int enable);
void write_data_channels(int num_pixels, int channel_offset, write_block_t *ptr, hls::stream<write_block_t> in[CPO], int *enable_write);
#endif

#ifdef GIHWCPI_DATA_FORMAT
void read_data_channels_gihwcpi(int num_pixels, int offset, read_block_t *ptr, hls::stream<pixel_in_t> &out, int enable);
void write_data_channels_gihwcpi(int num_pixels, int offset, write_block_t *ptr, hls::stream<pixel_out_t> &in);
#endif

// data reorganization
#ifdef IHW_DATA_FORMAT
void serialize_and_filter(int I_ITER, int num_pixels, int channel_blocks, hls::stream<read_block_t> &in, hls::stream<data_type> &out, int first_channel, int I, int enable);
template <int LEVELS> void ch_serialize_and_filter(int I_ITER, int num_pixels, int channel_blocks, hls::stream<read_block_t> stream_data_ch_0[LEVELS], hls::stream<data_type> stream_data_ch_1[LEVELS], int I, int enable){
#pragma HLS inline
ch_serialize_and_filter:
  for (int i = 0; i < LEVELS; i++) {
    #pragma HLS UNROLL
    serialize_and_filter(I_ITER, num_pixels, channel_blocks, stream_data_ch_0[i], stream_data_ch_1[i], i, I, enable);
  }
}
void join(int H, int W, int I_ITER, int num_extra_rows, int enable, hls::stream<data_type> in[CPI], hls::stream<pixel_in_t> &out);
void split(int H, int W, hls::stream<pixel_out_t> &in, hls::stream<data_type> out[CPO]);
void block_generate(int H, int W, hls::stream<data_type> &in, hls::stream<write_block_t> &out);
template <int LEVELS> void ch_block_generate(int H, int W, hls::stream<data_type> in[LEVELS], hls::stream<write_block_t> out[LEVELS]){
#pragma HLS inline
ch_block_generate:
  for (int i = 0; i < LEVELS; i++) {
    #pragma HLS UNROLL
    block_generate(H, W, in[i], out[i]);
  }
}
#endif

// convolution modules
#ifdef DIRECT_CONV
void direct_conv( int o_iter, int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);
#endif

#ifdef WINOGRAD_CONV
void winograd_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);
#endif

#ifdef DWS_CONV
void dws_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_dw_t> &k_dw_in, hls::stream<kernel_pw_t> &k_pw_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);
void dws_read_dw_kernel(int I_ITER, int o_iter, data_type *k_dw_ptr, hls::stream<kernel_dw_t> &k_dw_out);
void dws_read_pw_kernel(int I_ITER, int O, int o_iter, read_kernel_pw_t *k_pw_ptr, hls::stream<kernel_pw_t> &k_pw_out);
void dws_mul(int H, int W, int I_ITER, hls::stream<frame_t> &in, hls::stream<kernel_dw_t> &k_dw_in, hls::stream<kernel_pw_t> &k_pw_in, hls::stream<pixel_out_t> &out);
#endif

// buffer
void input_buffer( int o_iter, int num_pixels, int write_to_buff, int read_from_buff, hls::stream<pixel_in_t> &in, hls::stream<pixel_in_t> &out);

// activation functions
void relu(int enable_relu, int enable_clipping, int enable_shift, int min_clip, int max_clip, int direction_shift, int pos_shift,
		  int H, int W, hls::stream<pixel_out_t> &in, hls::stream<pixel_out_t> &out);

// pooling function
void pooling(int H, int W, int enable_maxpooling, int enable_avgpooling, hls::stream<pixel_out_t> &input, hls::stream<pixel_out_t> &output);

// padding functions
void padding(int o_iter,int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<pixel_in_t> &out);

// other functions
void add(int o_iter, int H, int W, int I_ITER, hls::stream<pixel_out_t> &in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);
void mul(int o_iter, int H, int W, int I_ITER, hls::stream<frame_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &out);
void cvt(int o_iter, int H, int W, int I_ITER, hls::stream<pixel_in_t> &in, hls::stream<frame_t> &out);

// What follows are macros used in the code

// -----------------------------------------------------------------------------------------------------------
// To allow using defines inside Xilinx pragmas
#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

#endif

#endif


