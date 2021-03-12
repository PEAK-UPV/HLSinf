#ifndef _CONV2D_H_
#define _CONV2D_H_

#include <stdio.h>
#include <ap_fixed.h>
#include <ap_int.h>
#include <hls_stream.h>

// -----------------------------------------------------------------------------------------------------------
// Convolution type (direct, winograd, deepwise separable)
// Select only one type of convolution
// -----------------------------------------------------------------------------------------------------------
//#define DIRECT_CONV
#define WINOGRAD_CONV
//#define DWS_CONV

// -----------------------------------------------------------------------------------------------------------
// data type. Defines the basic data type of the kernel
// Select only one data type
// -----------------------------------------------------------------------------------------------------------
// FP32 (uncomment the next four lines for FP32 support)
#define data_type float
#define DATA_TYPE_WIDTH  32	  // data type width in bits (32 for float)
#define READ_BLOCK_SIZE  16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.

// APFIXED<8> (uncomment the next four lines for APFIXED<8> support)
//#define data_type ap_fixed<8,4,AP_TRN,AP_WRAP>
//#define DATA_TYPE_WIDTH   8	  // data type width in bits (32 for float)
//#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
//#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.

// APINT<8> (uncomment the next four lines for APINT<8> support)
//#define data_type ap_int<8>
//#define DATA_TYPE_WIDTH   8	  // data type width in bits (32 for float)
//#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
//#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.

// -----------------------------------------------------------------------------------------------------------
// Defines for the kernel
// -----------------------------------------------------------------------------------------------------------
#define WMAX            256   // Maximum image width
#define HMAX            256   // Maximum image height
#define CPI               4   // Basic kernel number of input channels
#define CPO               4   // Basic kernel number of output channels
#define LOG2_CPO		  2   // number of bits for CPO (if you change CPO please change LOG2_CPO accordingly)
#define KW                3   // Convolutional kernel width
#define KH                3   // Convolutional kernel height
#define KWmpool 		  2   // Convolutional kernel width maxpool
#define KHmpool 		  2	  // Convolutional kernel height maxpool
#define SWmpool 		  2
#define SHmpool 		  2

// -----------------------------------------------------------------------------------------------------------
// Defines for the added modules to the conv layer
// -----------------------------------------------------------------------------------------------------------
#define USE_RELU		      // Enables the use of the ReLU activation after the conv layer
//#define USE_MAXPOOLING		  // Enables the use of the Maxpooling function after the relu layer

// -----------------------------------------------------------------------------------------------------------
// Defines for the maxpooling layer (USE_MAXPOOLING must be defined)
// -----------------------------------------------------------------------------------------------------------
#define KW_MAXPOOLING	  2	  // Maxpooling kernel width
#define KH_MAXPOOLING     2   // Maxpooling kernel height
#define SW_MAXPOOLING     2   // MAxpooling horizontal stride
#define SH_MAXPOOLING     2   // MAxpooling vertical stride

// -----------------------------------------------------------------------------------------------------------
// Defines for latency estimation
// Change those values and run C Synthesis in order to obtain the delay of the kernel
// -----------------------------------------------------------------------------------------------------------
#define I_REFERENCE       4  // I for delay estimation (must be equal or higher than CPI)
#define O_REFERENCE       4  // O for delay estimation (must be equal or higher than CPO)
#define W_REFERENCE     256  // W for delay estimation
#define H_REFERENCE     256  // H for delay estimatipn

// -----------------------------------------------------------------------------------------------------------
// defines for debug
// -----------------------------------------------------------------------------------------------------------
//#define DEBUG_READ_BIAS
//#define DEBUG_READ_KERNEL
//#define DEBUG_READ_DATA
//#define DEBUG_SERIALIZE
//#define DEBUG_MUL
//#define DEBUG_SPLIT
//#define DEBUG_WRITE_DATA
//#define DEBUG_CPU

// What follows is the definition of data types (do not change)

// -----------------------------------------------------------------------------------------------------------
// Data type for input data to the conv module
struct pixel_in_t {
	data_type pixel[CPI];
};

// -----------------------------------------------------------------------------------------------------------
// Data type for output data from the conv module
struct pixel_out_t {
  data_type pixel[CPO];
};
// -----------------------------------------------------------------------------------------------------------
// Data type for output data from the conv_winograd module
struct pixel_in_t2 {           // pixel in
  data_type pixel[CPI/2];
};

// -----------------------------------------------------------------------------------------------------------
// frames struct (KWxKH)
struct frame_t {
  pixel_in_t pixel[9];
};

// -----------------------------------------------------------------------------------------------------------
// frames struct (4x4) winograd
struct frame_d {
  pixel_in_t pixel[16];
};

// -----------------------------------------------------------------------------------------------------------
// frames struct (4x4) winograd
struct frame_d_2 {
  pixel_in_t2 pixel[16];
};
// -----------------------------------------------------------------------------------------------------------
// frames struct maxpool
struct frame_m {
  pixel_in_t pixel[KWmpool * KHmpool];
};
// -----------------------------------------------------------------------------------------------------------
// kernel struct
struct kernel_t {
  data_type pixel[CPO][CPI][9];
};

// -----------------------------------------------------------------------------------------------------------
//kernel read struct
struct kernel_in_t {
  data_type pixel[9];
};

// -----------------------------------------------------------------------------------------------------------
// kernel struct (deepwise)
struct kernel_dw_t {
  data_type pixel[CPI][KH*KW];
};

// -----------------------------------------------------------------------------------------------------------
// kernel struct (pointwise)
struct kernel_pw_t {
  data_type pixel[CPO][CPI];
};

// -----------------------------------------------------------------------------------------------------------
// Read block struct
typedef struct {
  data_type pixel[READ_BLOCK_SIZE];
} read_block_t;

// -----------------------------------------------------------------------------------------------------------
// Write block struct
struct write_block_t {
  data_type pixel[WRITE_BLOCK_SIZE];
};

// What follows is the function prototypes

// -----------------------------------------------------------------------------------------------------------
// function prototypes
extern "C" void k_conv2D(ap_uint<512> *ptr_data, int H, int W, int rows, int I, int O, int I_ITER, int O_ITER, int enable_relu, data_type *ptr_kernel, pixel_out_t *ptr_bias, ap_uint<512> *ptr_out, int global_offset, int enable_upper_padding, int enable_lower_padding);

void serialize_and_filter(int I_ITER, int num_pixels, int channel_blocks, int channel_size, int offset, hls::stream<read_block_t> &in, hls::stream<data_type> &out, int first_channel, int I);
template <int LEVELS> void ch_serialize_and_filter(int I_ITER, int num_pixels, int channel_blocks, int channel_size, int *offset_read_data_channel_i, hls::stream<read_block_t> stream_data_ch_0[LEVELS], hls::stream<data_type> stream_data_ch_1[LEVELS], int I){
#pragma HLS inline
ch_serialize_and_filter:
  for (int i = 0; i < LEVELS; i++) {
    #pragma HLS UNROLL
    serialize_and_filter(I_ITER, num_pixels, channel_blocks, channel_size, offset_read_data_channel_i[i], stream_data_ch_0[i], stream_data_ch_1[i], i, I);
  }
}

// read functions
void read_bias(int offset_bias, pixel_out_t *b_ptr, hls::stream<pixel_out_t> &out);
void read_kernel(int I_ITER, int offset_kernel, data_type *k_ptr, hls::stream<kernel_t> &k_out);
void read_data_channels(int H, int W, int rows, int I_ITER, ap_uint<512> *ptr, int offset, int num_extra_rows, int channel_blocks, hls::stream<read_block_t> out[CPI], int I);

// write functions
void write_data_channels(int num_pixels, ap_uint<512> *ptr, int *offset_i, hls::stream<write_block_t> in[CPO], int *enable_write);

// direct conv functions
void direct_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);

// winograd functions
void winograd_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);

// dws functions
void dws_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_dw_t> &k_dw_in, hls::stream<kernel_pw_t> &k_pw_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);
void dws_read_kernel(int I_ITER, int offset_kernel, data_type *k_ptr, hls::stream<kernel_dw_t> &k_dw_out, hls::stream<kernel_pw_t> &k_pw_out);
void dws_mul(int H, int W, int I_ITER, hls::stream<frame_t> &in, hls::stream<kernel_dw_t> &k_dw_in, hls::stream<kernel_pw_t> &k_pw_in, hls::stream<pixel_out_t> &out);

// data reorganization
void join(int H, int W, int I_ITER, int num_extra_rows, hls::stream<data_type> in[CPI], hls::stream<pixel_in_t> &out);
void split(int H, int W, int *addr_channel, int num_blocks_channel, hls::stream<pixel_out_t> &in, hls::stream<write_block_t> out[CPO]);

// activation functions
void relu(int enable_relu, int H, int W, hls::stream<pixel_out_t> &in, hls::stream<pixel_out_t> &out);

// padding functions
void padding(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<pixel_in_t> &out);

// other functions
void add(int H, int W, int I_ITER, hls::stream<pixel_out_t> &in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);
void mul(int H, int W, int I_ITER, hls::stream<frame_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &out);
void cvt(int H, int W, int I_ITER, hls::stream<pixel_in_t> &in, hls::stream<frame_t> &out);

// What follows are macros used in the code

// -----------------------------------------------------------------------------------------------------------
// To allow using defines inside Xilinx pragmas
#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

#endif
