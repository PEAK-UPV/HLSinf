#ifndef _CONV2D_H_
#define _CONV2D_H_

#include <stdio.h>
#include <ap_fixed.h>
#include <ap_int.h>
#include <hls_stream.h>

// -----------------------------------------------------------------------------------------------------------
// Convolution type
// Select only one type
// -----------------------------------------------------------------------------------------------------------
//#define DIRECT_CONV		// Direct convolution
//#define WINOGRAD_CONV		// Winograd convolution
#define DWS_CONV			// DeepWise Separable convolution

// -----------------------------------------------------------------------------------------------------------
// data type. Defines the basic data type of the kernel
// Select only one data type (fp32, apf8, api8)
// -----------------------------------------------------------------------------------------------------------
//#define FP32_DATA_TYPE
//#define APF8_DATA_TYPE
#define API8_DATA_TYPE

// -----------------------------------------------------------------------------------------------------------
// Defines for the kernel
// -----------------------------------------------------------------------------------------------------------
#define WMAX            256   // Maximum image width
#define HMAX            256   // Maximum image height
#define CPI               4   // Basic kernel number of input channels
#define CPO               4   // Basic kernel number of output channels
#define LOG2_CPO          2   // number of bits for CPO (if you change CPO please change LOG2_CPO accordingly)

// -----------------------------------------------------------------------------------------------------------
// Defines for the added modules to the conv layer (clipping and shift must be used only for API8 data type)
// -----------------------------------------------------------------------------------------------------------
#define USE_RELU		   // Enables the use of the ReLU activation
#define USE_CLIPPING       // Enables the use of the clipping function (implemented in the ReLU module)
#define USE_SHIFT          // Enables the use of the shift function (implemented in the ReLU module)
#define USE_POOLING		   // Enables the use of the Pooling function (maxpooling or avgpooling)

// -----------------------------------------------------------------------------------------------------------
// Defines for latency estimation
// Change those values and run C Synthesis in order to obtain the delay of the kernel
// -----------------------------------------------------------------------------------------------------------
#define I_REFERENCE     CPI  // I for delay estimation (must be equal or higher than CPI)
#define O_REFERENCE     CPO  // O for delay estimation (must be equal or higher than CPO)
#define W_REFERENCE    WMAX  // W for delay estimation
#define H_REFERENCE    HMAX  // H for delay estimation

// -----------------------------------------------------------------------------------------------------------
// defines for debug (DEBUG_ALL activates all debug defines)
// -----------------------------------------------------------------------------------------------------------
//#define DEBUG_ALL

//#define DEBUG_READ_BIAS
//#define DEBUG_READ_KERNEL
//#define DEBUG_READ_DATA
//#define DEBUG_SERIALIZE
//#define DEBUG_CVT
//#define DEBUG_MUL
//#define DEBUG_ADD
//#define DEBUG_SPLIT
//#define DEBUG_WRITE_DATA
//#define DEBUG_RELU
//#define DEBUG_POOL
//#define DEBUG_CPU

// -----------------------------------------------------------------------------------------------------------
// defines for C simulation and C/RTL co-simulation
// -----------------------------------------------------------------------------------------------------------
#define W_SIM WMAX
#define H_SIM HMAX
#define I_SIM I_REFERENCE
#define O_SIM O_REFERENCE
#define INSTANCES_SIM 2

// ***********************************************************************************************************
// ***********************************************************************************************************
// ***********************************************************************************************************
// What follows is the definition of data types (do not change!!!!!!)
// ***********************************************************************************************************
// ***********************************************************************************************************
// ***********************************************************************************************************

#define LEFT_DIRECTION 0	// direction used in ReLU (shift) module
#define RIGHT_DIRECTION 1   // direction used in ReLU (shift) module

// -----------------------------------------------------------------------------------------------------------
// data type, read and write block parameters
// -----------------------------------------------------------------------------------------------------------
#ifdef FP32_DATA_TYPE
#define data_type float
#define DATA_TYPE_WIDTH  32	  // data type width in bits (32 for float)
#define READ_BLOCK_SIZE  16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE -99999
#define EPSILON_VALUE 0.00001
#endif

#ifdef APF8_DATA_TYPE
#define data_type ap_fixed<8,4,AP_TRN,AP_WRAP>
#define DATA_TYPE_WIDTH   8	  // data type width in bits (32 for float)
#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -99999
#define EPSILON_VALUE 0.0001
#endif

#ifdef API8_DATA_TYPE
#define data_type ap_int<8>
#define DATA_TYPE_WIDTH   8	  // data type width in bits (32 for float)
#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -127
#define EPSILON_VALUE 0
#endif

// -----------------------------------------------------------------------------------------------------------
// depth of pointers for co-simulation support
// -----------------------------------------------------------------------------------------------------------
#define DATA_IN_PORT_DEPTH   W_SIM * H_SIM * I_SIM * (DATA_TYPE_WIDTH / 8) / 64
#define DATA_OUT_PORT_DEPTH  W_SIM * H_SIM * O_SIM * (DATA_TYPE_WIDTH / 8) / 64
#define KERNEL_PORT_DEPTH    3 * 3 * I_SIM * O_SIM
#define DW_KERNEL_PORT_DEPTH 3 * 3 * I_SIM
#define PW_KERNEL_PORT_DEPTH I_SIM * O_SIM
#define BIAS_PORT_DEPTH      O_SIM / CPO

// -----------------------------------------------------------------------------------------------------------
// Defines for the CONV layer
// -----------------------------------------------------------------------------------------------------------
#define KW             3   // Convolutional kernel width
#define KH             3   // Convolutional kernel height

// -----------------------------------------------------------------------------------------------------------
// Defines for the POOLING layer (USE_MAXPOOLING or USE_AVGPOOLING must be defined)
// -----------------------------------------------------------------------------------------------------------
#define KW_POOLING	   2   // Maxpooling kernel width
#define KH_POOLING     2   // Maxpooling kernel height
#define SW_POOLING     2   // MAxpooling horizontal stride
#define SH_POOLING     2   // MAxpooling vertical stride

#if !defined(API8_DATA_TYPE) && (defined(USE_SHIFT) || defined(USE_CLIPPING))
#error "USE_SHIFT and USE_CLIPPING can be used only with API8 data type"
#endif

// -----------------------------------------------------------------------------------------------------------
// Defines for debug
// -----------------------------------------------------------------------------------------------------------
#ifdef DEBUG_ALL
#define DEBUG_READ_BIAS
#define DEBUG_READ_KERNEL
#define DEBUG_READ_DATA
#define DEBUG_SERIALIZE
#define DEBUG_CVT
#define DEBUG_MUL
#define DEBUG_ADD
#define DEBUG_SPLIT
#define DEBUG_WRITE_DATA
#define DEBUG_RELU
#define DEBUG_POOL
#define DEBUG_CPU
#endif

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
  int block_offset;
};

// What follows is the function prototypes

// Function prototypes protected with the no-definition of OPENCL_TEST to avoid warnings when compiling for OpenCL
#ifndef OPENCL_TEST

// -----------------------------------------------------------------------------------------------------------
// function prototypes
extern "C" void k_conv2D(ap_uint<512> *ptr_data, int H, int W, int rows, int I, int O, int I_ITER, int O_ITER, int enable_relu,
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
                         data_type *ptr_kernel,
#endif
#ifdef DWS_CONV
						 data_type *ptr_dw_kernel, data_type *ptr_pw_kernel,
#endif
						 pixel_out_t *ptr_bias, ap_uint<512> *ptr_out, int global_offset, int enable_upper_padding,
						 int enable_lower_padding, int enable_maxpooling, int enable_avgpooling,
						 int enable_clipping, int enable_shift, int min_clip, int max_clip, int dir_shift, int pos_shift);

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
void write_data_channels(int num_pixels, ap_uint<512> *ptr, hls::stream<write_block_t> in[CPO], int *enable_write);

// direct conv functions
void direct_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);

// winograd functions
void winograd_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);

// dws functions
void dws_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_dw_t> &k_dw_in, hls::stream<kernel_pw_t> &k_pw_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);
void dws_read_kernel(int I_ITER, int offset_dw_kernel, int offset_pw_kernel, data_type *k_dw_ptr, data_type *k_pw_ptr, hls::stream<kernel_dw_t> &k_dw_out, hls::stream<kernel_pw_t> &k_pw_out);
void dws_mul(int H, int W, int I_ITER, hls::stream<frame_t> &in, hls::stream<kernel_dw_t> &k_dw_in, hls::stream<kernel_pw_t> &k_pw_in, hls::stream<pixel_out_t> &out);

// data reorganization
void join(int H, int W, int I_ITER, int num_extra_rows, hls::stream<data_type> in[CPI], hls::stream<pixel_in_t> &out);
void split(int H, int W, int *offset_channel, int *block_offset_channel, hls::stream<pixel_out_t> &in, hls::stream<write_block_t> out[CPO]);

// activation functions
void relu(int enable_relu, int enable_clipping, int enable_shift, int min_clip, int max_clip, int direction_shift, int pos_shift,
		  int H, int W, hls::stream<pixel_out_t> &in, hls::stream<pixel_out_t> &out);

// pooling function
void pooling(int H, int W, int enable_maxpooling, int enable_avgpooling, hls::stream<pixel_out_t> &input, hls::stream<pixel_out_t> &output);

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

#endif
