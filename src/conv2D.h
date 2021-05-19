#ifndef _CONV2D_H_
#define _CONV2D_H_

#define AP_INT_MAX_W 4096 // Must be defined before includes

#include <stdio.h>
#include <ap_fixed.h>
#include <ap_int.h>
#include <hls_stream.h>

// -----------------------------------------------------------------------------------------------------------
// Configuration selection.
// Select only one configuration. Each configuration defines the target device, the type of convolution, the
// arithmetic precision format, and the size of the kernel in input channels and output channels.
// Each configuration is optimized for the specific targeted board
// -----------------------------------------------------------------------------------------------------------

// Configurations for Alveo U200 boards

//#define CONF_ALVEO_U200_4x4_DIRECT_FP32                 // Direct convolution 4x4 kernel with FP32
//#define CONF_ALVEO_U200_8x8_DIRECT_API8            	// Direct convolution 8x8 kernel with API8
//#define CONF_ALVEO_U200_16x16_WINOGRAD_API8        	// Winograd convolution 16x16 kernel with API8
//#define CONF_ALVEO_U200_32x32_DWS_API8               	// DeepWise Separable 32x32 kernel with API8
//#define CONF_ALVEO_U200_32x64_DWS_API8             	// DeepWise Separable 32x64 kernel with API8
//#define CONF_ALVEO_U200_64x64_DWS_API8                // DeepWise Separable 64x64 kernel with API8
//#define CONF_ALVEO_U200_4x4_DWS_API8             		// DeepWise Separable 4x4 kernel with API8

// Configurations for Alveo U280 boards
//#define CONF_ALVEO_U280_4x4_DIRECT_FP32                 // Direct convolution 4x4 kernel with FP32
//#define CONF_ALVEO_U280_8x8_DIRECT_FP32                 // Direct convolution 8x8 kernel with FP32
//#define CONF_ALVEO_U280_16x16_DWS_API8                  // DeepWise Separable 16x16 kernel with API8

// -----------------------------------------------------------------------------------------------------------
// Input data format:
//   - IxHxW      : The channel is stored in memory in a single block
//   - GIxHxWxCPI : Channels are interleaved in memory, GI groups of CPI channels are assumed
//
// The GIxHxWxCPI format allows the kernel to read/write data from/to memory efficiently and with low resources
// overhead. This format requires padding (CPI at the input and CPO at the output)
// -----------------------------------------------------------------------------------------------------------
//#define IHW_DATA_FORMAT
#define GIHWCPI_DATA_FORMAT


// -----------------------------------------------------------------------------------------------------------
// defines for debug (DEBUG_ALL activates all debug defines)
// -----------------------------------------------------------------------------------------------------------
//#define DEBUG_ALL

//#define DEBUG_VERBOSE

//#define DEBUG_READ_BIAS
//#define DEBUG_READ_KERNEL
//#define DEBUG_READ_DATA
//#define DEBUG_SERIALIZE
//#define DEBUG_JOIN
//#define DEBUG_INPUT_BUFFER
//#define DEBUG_PADDING
//#define DEBUG_CVT
//#define DEBUG_MUL
//#define DEBUG_ADD
//#define DEBUG_SPLIT
//#define DEBUG_BLOCK
//#define DEBUG_WRITE_DATA
//#define DEBUG_RELU
//#define DEBUG_POOL
//#define DEBUG_CPU

// -----------------------------------------------------------------------------------------------------------
// Automatic defines (do not change; add new ones if needed)
// -----------------------------------------------------------------------------------------------------------
#ifdef CONF_ALVEO_U200_4x4_DIRECT_FP32
#define ALVEO_U200
#define DIRECT_CONV
#define FP32_DATA_TYPE
#define USE_RELU
#define USE_POOLING
#define CPI                          4
#define CPO                          4
#define LOG2_CPO                     2
#define WMAX                       256
#define HMAX                       256
#define READ_BURST_SIZE              4
#define STREAMS_DEPTH                4
#define INPUT_BUFFER_SIZE        65536 //524288
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_ALVEO_U200_8x8_DIRECT_API8
#define ALVEO_U200
#define DIRECT_CONV
#define API8_DATA_TYPE
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define CPI                8
#define CPO                8
#define LOG2_CPO           3
#define WMAX             256
#define HMAX             256
#define READ_BURST_SIZE    2
#define STREAMS_DEPTH      2
#define INPUT_BUFFER_SIZE  65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#endif

#ifdef CONF_ALVEO_U200_16x16_WINOGRAD_API8
#define ALVEO_U200
#define WINOGRAD_CONV
#define API8_DATA_TYPE
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define CPI               16
#define CPO               16
#define LOG2_CPO           4
#define WMAX             256
#define HMAX             256
#define READ_BURST_SIZE    4
#define STREAMS_DEPTH      4
#define INPUT_BUFFER_SIZE  32768 // 32 rows x 32 cols x (512/CPI) pixels_in
#endif

#ifdef CONF_ALVEO_U200_32x32_DWS_API8
#define ALVEO_U200
#define DWS_CONV
#define API8_DATA_TYPE
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define CPI                         32
#define CPO                         32
#define LOG2_CPO                     5
#define WMAX                       256
#define HMAX                       256
#define READ_BURST_SIZE              8
#define STREAMS_DEPTH                8
#define INPUT_BUFFER_SIZE        65536
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_ALVEO_U200_32x64_DWS_API8
#define ALVEO_U200
#define DWS_CONV
#define API8_DATA_TYPE
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define CPI               32
#define CPO               64
#define LOG2_CPO           6
#define WMAX             256
#define HMAX             256
#define READ_BURST_SIZE    8
#define STREAMS_DEPTH      8
#define INPUT_BUFFER_SIZE  16384 // 32 rows x 32 cols x (512/CPI) pixels_in
#endif

#ifdef CONF_ALVEO_U200_64x64_DWS_API8
#define ALVEO_U200
#define DWS_CONV
#define API8_DATA_TYPE
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define CPI                         64
#define CPO                         64
#define LOG2_CPO                     6
#define WMAX                       256
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define INPUT_BUFFER_SIZE        32768 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_ALVEO_U200_4x4_DWS_API8
#define ALVEO_U200
#define DWS_CONV
#define API8_DATA_TYPE
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define CPI                          4
#define CPO                          4
#define LOG2_CPO                     2
#define WMAX                       256
#define HMAX                       256
#define READ_BURST_SIZE              8
#define STREAMS_DEPTH                8
#define INPUT_BUFFER_SIZE      8388608    // 256x256x(512/CPI) pixels
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_ALVEO_U280_4x4_DIRECT_FP32
#define ALVEO_U280
#define DIRECT_CONV
#define FP32_DATA_TYPE
#define USE_RELU
#define USE_POOLING
#define CPI                          4
#define CPO                          4
#define LOG2_CPO                     2
#define WMAX                       256
#define HMAX                       256
#define READ_BURST_SIZE              4
#define STREAMS_DEPTH                4
#define INPUT_BUFFER_SIZE        65536
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4
#define PW_KERNEL_STREAM_DEPTH       4
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_ALVEO_U200_8x8_DIRECT_FP32
#define ALVEO_U280
#define DIRECT_CONV
#define FP32_DATA_TYPE
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define CPI                8
#define CPO                8
#define LOG2_CPO           3
#define WMAX             256
#define HMAX             256
#define READ_BURST_SIZE    2
#define STREAMS_DEPTH      2
#define INPUT_BUFFER_SIZE  65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#endif

#ifdef CONF_ALVEO_U200_16x16_DWS_API8
#define ALVEO_U200
#define DWS_CONV
#define API8_DATA_TYPE
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define CPI                         16
#define CPO                         16
#define LOG2_CPO                     6
#define WMAX                       256
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define INPUT_BUFFER_SIZE        32768 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif


// ***********************************************************************************************************
// ***********************************************************************************************************
// ***********************************************************************************************************
// What follows is the definition of data types (do not change!!!!!!)
// ***********************************************************************************************************
// ***********************************************************************************************************
// ***********************************************************************************************************

// -----------------------------------------------------------------------------------------------------------
// Defines for latency estimation in C synthesis
// -----------------------------------------------------------------------------------------------------------
#define I_REFERENCE     CPI  // I for delay estimation (must be equal or higher than CPI)
#define O_REFERENCE     CPO  // O for delay estimation (must be equal or higher than CPO)
#define W_REFERENCE    WMAX  // W for delay estimation
#define H_REFERENCE    HMAX  // H for delay estimation

// -----------------------------------------------------------------------------------------------------------
// defines for C simulation and C/RTL co-simulation
// -----------------------------------------------------------------------------------------------------------
#define W_SIM         256 //WMAX
#define H_SIM         256 //HMAX
#define I_SIM         64  //I_REFERENCE
#define O_SIM         64  //O_REFERENCE
#define INSTANCES_SIM 1   //2

// -----------------------------------------------------------------------------------------------------------
// Direction defines (for shift operations)
// -----------------------------------------------------------------------------------------------------------
#define LEFT_DIRECTION  0	// direction used in ReLU (shift) module
#define RIGHT_DIRECTION 1   // direction used in ReLU (shift) module

// -----------------------------------------------------------------------------------------------------------
// data type, read and write block parameters
// -----------------------------------------------------------------------------------------------------------
#ifdef FP32_DATA_TYPE
#define data_type float
#define DATA_TYPE_WIDTH  32	  // data type width in bits
#define READ_BLOCK_SIZE  16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE -99999
#define EPSILON_VALUE 0.00001
#endif

#ifdef APF8_DATA_TYPE
#define data_type ap_fixed<8,4,AP_TRN,AP_WRAP>
#define DATA_TYPE_WIDTH   8	  // data type width in bits
#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -99999
#define EPSILON_VALUE 0.0001
#endif

#ifdef API8_DATA_TYPE
#define data_type ap_int<8>
#define DATA_TYPE_WIDTH   8	  // data type width in bits
#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -127
#define EPSILON_VALUE 0
#endif

#ifdef API16_DATA_TYPE
#define data_type ap_int<16>
#define DATA_TYPE_WIDTH  16	  // data type width in bits
#define READ_BLOCK_SIZE  32   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 32   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -127
#define EPSILON_VALUE 0
#endif

// -----------------------------------------------------------------------------------------------------------
// depth of pointers for co-simulation support
// -----------------------------------------------------------------------------------------------------------
#define DATA_IN_PORT_DEPTH   W_SIM * H_SIM * I_SIM * (DATA_TYPE_WIDTH / 8) / 64
#define DATA_OUT_PORT_DEPTH  W_SIM * H_SIM * O_SIM * (DATA_TYPE_WIDTH / 8) / 64
#define KERNEL_PORT_DEPTH    3 * 3 * I_SIM * O_SIM
#define DW_KERNEL_PORT_DEPTH I_SIM * 9
#define PW_KERNEL_PORT_DEPTH O_SIM * (I_SIM / CPI)
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

#if (!defined(API8_DATA_TYPE) && !defined(API16_DATA_TYPE)) && (defined(USE_SHIFT) || defined(USE_CLIPPING))
#error "USE_SHIFT and USE_CLIPPING can be used only with API8 or API16 data types"
#endif

// -----------------------------------------------------------------------------------------------------------
// Defines for debug
// -----------------------------------------------------------------------------------------------------------
#ifdef DEBUG_ALL
#define DEBUG_READ_BIAS
#define DEBUG_READ_KERNEL
#define DEBUG_READ_DATA
#define DEBUG_SERIALIZE
#define DEBUG_JOIN
#define DEBUG_INPUT_BUFFER
#define DEBUG_PADDING
#define DEBUG_CVT
#define DEBUG_MUL
#define DEBUG_ADD
#define DEBUG_SPLIT
#define DEBUG_BLOCK
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
#define read_kernel_pw_t ap_int<CPI*DATA_TYPE_WIDTH>

// -----------------------------------------------------------------------------------------------------------
// Read block struct
#ifdef IHW_DATA_FORMAT
#define read_block_t ap_int<512>
#endif
#ifdef GIHWCPI_DATA_FORMAT
#define read_block_t pixel_in_t
#endif

// -----------------------------------------------------------------------------------------------------------
// Write block struct
#ifdef IHW_DATA_FORMAT
#define write_block_t ap_int<512>
#endif
#ifdef GIHWCPI_DATA_FORMAT
#define write_block_t pixel_out_t
#endif


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
						 int enable_clipping, int enable_shift, int min_clip, int max_clip, int dir_shift, int pos_shift);


// read and write functions
void read_bias(int offset_bias, pixel_out_t *b_ptr, hls::stream<pixel_out_t> &out);
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
void direct_conv(int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding, hls::stream<pixel_in_t> &in, hls::stream<kernel_t> &k_in, hls::stream<pixel_out_t> &b_in, hls::stream<pixel_out_t> &out);
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
void input_buffer(int num_pixels, int write_to_buff, int read_from_buff, hls::stream<pixel_in_t> &in, hls::stream<pixel_in_t> &out);

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
