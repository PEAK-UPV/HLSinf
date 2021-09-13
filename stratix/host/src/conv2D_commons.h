#ifndef _LIB_CONV2D_COMMONS_H_
#define _LIB_CONV2D_COMMONS_H_


#define HLS_DEBUG



//#define DEBUG_READ_BIAS
//#define DEBUG_READ_KERNEL
//#define DEBUG_READ_DATA
////#define DEBUG_SERIALIZE
////#define DEBUG_JOIN
////#define DEBUG_INPUT_BUFFER
////#define DEBUG_PADDING
////#define DEBUG_CVT
////#define DEBUG_MUL
////#define DEBUG_ADD
////#define DEBUG_SPLIT
////#define DEBUG_BLOCK
//#define DEBUG_WRITE_DATA
////#define DEBUG_RELU
////#define DEBUG_POOL
//#define DEBUG_CPU
////#define DEBUG_CHECK

// HLS DEBUG MACROS
#define HLS_DBG_H_IND          0
#define HLS_DBG_W_IND          1
#define HLS_DBG_ROWS_IND       2
#define HLS_DBG_I_INPUT_IND    3
#define HLS_DBG_O_OUTPUT_IND   4
#define HLS_DBG_I_ITER_IND           5
#define HLS_DBG_O_ITER_FIRST_IND     6
#define HLS_DBG_O_ITER_LAST_IND      7
#define HLS_DBG_ENABLE_RELU_IND      8
#define HLS_DBG_GLOBAL_OFFSET_IND    9
#define HLS_DBG_ENABLE_UPPER_PADDING_IND   10
#define HLS_DBG_ENABLE_LOWER_PADDING_IND   11
#define HLS_DBG_ENABLE_MAX_POOLING_IND     12
#define HLS_DBG_ENABLE_AVG_POOLING_IND     13
#define HLS_DBG_ENABLE_CLIPPING_IND        14
#define HLS_DBG_ENABLE_SHIFT_IND   15
#define HLS_DBG_MIN_CLIP_IND       16
#define HLS_DBG_MAX_CLIP_IND       17
#define HLS_DBG_DIR_SHIFT_IND      18
#define HLS_DBG_POS_SHIFT_IND      19

#define HLS_DBG_VALUES_read_from_bias_IND                 20
#define HLS_DBG_VALUES_read_from_kernel_IND               21
#define HLS_DBG_VALUES_read_from_data_in_IND              22

#define HLS_DBG_VALUES_write_to_bias_stream_IND   23 
#define HLS_DBG_VALUES_read_from_bias_stream_IND  24

#define HLS_DBG_VALUES_write_to_kernel_stream_IND  25
#define HLS_DBG_VALUES_read_from_kernel_stream_IND 26

#define HLS_DBG_VALUES_write_to_out_read_data_stream_IND  27
#define HLS_DBG_VALUES_read_from_out_read_data_stream_IND 28

#define HLS_DBG_VALUES_write_to_out_read_data_from_input_buffer_stream_IND  29
#define HLS_DBG_VALUES_read_from_out_read_data_from_input_buffer_stream_IND 30

#define HLS_DBG_VALUES_write_to_pad_cvt_stream_IND  31
#define HLS_DBG_VALUES_read_from_pad_cvt_stream_IND 32

#define HLS_DBG_VALUES_write_to_cvt_mul_stream_IND  33
#define HLS_DBG_VALUES_read_from_cvt_mul_stream_IND 34

#define HLS_DBG_VALUES_write_to_mul_add_stream_IND  35
#define HLS_DBG_VALUES_read_from_mul_add_stream_IND 36

#define HLS_DBG_VALUES_write_to_out_conv_stream_IND  37
#define HLS_DBG_VALUES_read_from_out_conv_stream_IND 38

#define HLS_DBG_VALUES_write_to_out_relu_stream_IND  39
#define HLS_DBG_VALUES_read_from_out_relu_stream_IND 40

#define HLS_DBG_VALUES_write_to_stream_pool_stream_IND  41 
#define HLS_DBG_VALUES_read_from_stream_pool_stream_IND 42

#define HLS_DBG_VALUES_write_to_out_pooling_stream_IND  43
#define HLS_DBG_VALUES_read_from_out_polling_stream_IND 44

#define HLS_DBG_VALUES_write_to_data_out_IND  45

#define HLS_DBG_VALUES_write_to_out_conv_sbs_stream_IND  46

//
#define HLS_DBG_DT_bias_sum_IND              0
#define HLS_DBG_DT_kernel_sum_IND            1
#define HLS_DBG_DT_din_sum_IND               2
#define HLS_DBG_DT_dout_sum_IND              3


// max index in both arrays (plus one)
#define NUM_HLS_DBG_VALUE_ARRAY_ENTRIES  47


#define NUM_HLS_DBG_SBS_CONTROL_iter_IND                  0
#define NUM_HLS_DBG_SBS_CONTROL_pin_row_IND               1
#define NUM_HLS_DBG_SBS_CONTROL_pin_col_IND               2
#define NUM_HLS_DBG_SBS_CONTROL_row0_buffer_write_IND     3
#define NUM_HLS_DBG_SBS_CONTROL_row1_buffer_write_IND     4
#define NUM_HLS_DBG_SBS_CONTROL_row2_buffer_write_IND     5
#define NUM_HLS_DBG_SBS_CONTROL_p_IND                     6
#define NUM_HLS_DBG_SBS_CONTROL_pin_col0_IND              7
#define NUM_HLS_DBG_SBS_CONTROL_pin_col1_IND              8
#define NUM_HLS_DBG_SBS_CONTROL_row0_IND                  9
#define NUM_HLS_DBG_SBS_CONTROL_row1_IND                 10
#define NUM_HLS_DBG_SBS_CONTROL_shift_frame_IND          11
#define NUM_HLS_DBG_SBS_CONTROL_send_frame_IND           12

#define NUM_HLS_DBG_SBS_CONTROL_ARRAY_ENTRIES            13


//----end of HLS debug macros


// debo llevarlo a un include compartido entre el k_conv2D.cl y el lib_conv2D.cpp
#define AP_INT_MAX_W 4096 // Must be defined before includes
#define CONF_ALVEO_U200_4x4_DIRECT_FP32    
#define GIHWCPI_DATA_FORMAT


// constants
//#define MAX_CONVS        8  // Maximum number of convolutional layers
//#define MAX_KERNELS      4  // Maximum number of kernels implemented
#define MAX_CONVS        1  // Maximum number of convolutional layers
#define MAX_KERNELS      1  // Maximum number of kernels implemented


#define MAX_WORK_ITEMS 512  // Maximum number of work items to process
//#define NUM_KERNELS      2   //JM10
#define NUM_KERNELS      1   //JM10



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
#define data_type ac_fixed<8,4,AP_TRN,AP_WRAP>
#define DATA_TYPE_WIDTH   8	  // data type width in bits
#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -99999
#define EPSILON_VALUE 0.0001
#endif

#ifdef API8_DATA_TYPE
#define data_type ac_int<8>
#define DATA_TYPE_WIDTH   8	  // data type width in bits
#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -127
#define EPSILON_VALUE 0
#endif

#ifdef API16_DATA_TYPE
#define data_type ac_int<16>
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
struct pixel_in_t_st {
	data_type pixel[CPI];
}__attribute__((packed));
typedef struct pixel_in_t_st pixel_in_t;

// -----------------------------------------------------------------------------------------------------------
// Data type for output data from the conv module
struct pixel_out_t_st {
  data_type pixel[CPO];
}__attribute__((packed));
typedef struct pixel_out_t_st pixel_out_t;

// -----------------------------------------------------------------------------------------------------------
// Data type for output data from the conv_winograd module
struct pixel_in_t2_st {           // pixel in
  data_type pixel[CPI/2];
};
typedef struct pixel_in_t2_st pixel_in_t2;

// -----------------------------------------------------------------------------------------------------------
// frames struct (KWxKH)
struct frame_t_st {
  pixel_in_t pixel[9];
};
typedef struct frame_t_st frame_t;


// -----------------------------------------------------------------------------------------------------------
// frames struct (4x4) winograd
struct frame_d_st {
  pixel_in_t pixel[16];
};
typedef struct frame_d_st frame_d;

// -----------------------------------------------------------------------------------------------------------
// frames struct (4x4) winograd
struct frame_d_2_st {
  pixel_in_t2 pixel[16];
};
typedef struct frame_d_2_st frame_d_2;

// -----------------------------------------------------------------------------------------------------------
// kernel struct
struct kernel_t_st {
  data_type pixel[CPO][CPI][9];
};
typedef struct kernel_t_st kernel_t;

// -----------------------------------------------------------------------------------------------------------
//kernel read struct
struct kernel_in_t_st {
  data_type pixel[9];
};
typedef struct kernel_in_t_st kernel_in_t;

// -----------------------------------------------------------------------------------------------------------
// kernel struct (deepwise)
struct kernel_dw_t_st {
  data_type pixel[CPI][KH*KW];
};
typedef struct kernel_dw_t_st kernel_dw_t;

// -----------------------------------------------------------------------------------------------------------
// kernel struct (pointwise)
struct kernel_pw_t_st {
  data_type pixel[CPO][CPI];
};
typedef struct kernel_pw_t_st kernel_pw_t;

//#ifdef HLS_DEBUG
// kernel struct (pointwise)
struct hls_cvt_sbs_control_t_st {
  unsigned long iter;//                  0
  unsigned long pin_row;//               1
  unsigned long pin_col;//               2
  unsigned long row0_buffer_write;//     3
  unsigned long row1_buffer_write;//     4
  unsigned long row2_buffer_write;//     5
  unsigned long p;//                     6
  unsigned long pin_col0;//              7
  unsigned long pin_col1;//              8
  unsigned long row0;//                  9
  unsigned long row1;//                 10
  unsigned long shift_frame;//          11
  unsigned long send_frame;//           12

}__attribute__((packed));
typedef struct hls_cvt_sbs_control_t_st hls_cvt_sbs_control_t;
//#endif

#define read_kernel_pw_t ac_int<CPI*DATA_TYPE_WIDTH>
// typedef ac_int<CPI*DATA_TYPE_WIDTH> read_kernel_pw_t;


// -----------------------------------------------------------------------------------------------------------
// Read block struct
#ifdef IHW_DATA_FORMAT
  #define read_block_t ac_int<512>
  // typedef ac_int<512> read_block_t;
#endif
#ifdef GIHWCPI_DATA_FORMAT
  #define read_block_t pixel_in_t
  // typedef pixel_in_t read_block_t;
#endif

// -----------------------------------------------------------------------------------------------------------
// Write block struct
#ifdef IHW_DATA_FORMAT
  #define write_block_t ac_int<512>
  // typedef ac_int<512> write_block_t;
#endif
#ifdef GIHWCPI_DATA_FORMAT
  #define write_block_t pixel_out_t
  //typedef pixel_out_t write_block_t;
#endif


#endif




