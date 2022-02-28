#ifndef _LIB_CONV2D_COMMONS_H_
#define _LIB_CONV2D_COMMONS_H_


#define RANDOM_GEN_SEED_FIXED 1

#define DEBUG_VERBOSE
// host side debug 
#define DEBUG_LOG_INIT
#define PRINT_LOG_BUFFERS
#define DEBUG_DATA_DIN
#define DEBUG_DATA_KERNEL
#define DEBUG_DATA_DOUT
#define DEBUG_DATA_BIAS
//
// Kernel side
#define DEBUG_READ_BIAS
#define DEBUG_READ_KERNEL
#define DEBUG_READ_DATA
#define DEBUG_HOST_KERNELS
//#define DEBUG_SERIALIZE
//#define DEBUG_JOIN
#define DEBUG_INPUT_BUFFER
#define DEBUG_PADDING
#define DEBUG_CVT
#define DEBUG_MUL
#define DEBUG_ADD
//#define DEBUG_SPLIT
//#define DEBUG_BLOCK
//#define DEBUG_RELU
#define DEBUG_POOL
//#define DEBUG_ADD_DATA
#define DEBUG_BATCH_NORM
#define DEBUG_WRITE_DATA

// host side (again)
#define DEBUG_CPU
//#define DEBUG_CPU_MUL
#define DEBUG_CHECK






#define FPGA_STRATIX_10MX

// #define EMULATION_CONFIG_ENABLED

#ifdef EMULATION_CONFIG_ENABLED
  #define CONF_S10MX_4x4_DIRECT_FP32_EMULATION
  //#define CONF_S10MX_8x8_DIRECT_FP32_EMULATION
  //#define CONF_S10MX_8x8_DIRECT_API8_EMULATION
  //#define CONF_S10MX_8x8_DIRECT_API16_EMULATION
  //#define CONF_S10MX_8x8_DIRECT_API32_EMULATION
#else
  // debo llevarlo a un include compartido entre el k_conv2D.cl y el lib_conv2D.cpp
  #define CONF_S10MX_4x4_DIRECT_FP32    
  //#define CONF_S10MX_8x8_DIRECT_FP32    
  //#define CONF_S10MX_16x16_DIRECT_FP32
  //#define CONF_S10MX_8x8_DIRECT_API8
  //#define CONF_S10MX_8x8_DIRECT_API16
  //#define CONF_S10MX_8x8_DIRECT_API32
#endif
#define GIHWCPI_DATA_FORMAT


// constants
//#define MAX_CONVS        8  // Maximum number of convolutional layers
//#define MAX_KERNELS      4  // Maximum number of kernels implemented
#define MAX_CONVS        1  // Maximum number of convolutional layers
#define MAX_KERNELS      1  // Maximum number of kernels implemented


#define MAX_WORK_ITEMS 512  // Maximum number of work items to process
//#define NUM_KERNELS      2   //JM10
#define NUM_KERNELS      1   //JM10


#ifdef CONF_S10MX_4x4_DIRECT_FP32_EMULATION
#define DIRECT_CONV
#define FP32_DATA_TYPE
#define USE_RELU
#define USE_POOLING
#define CPI                          4
#define CPO                          4
#define LOG2_CPO                     2
#define WMAX                      32 // 32  for emulation 256 for impl
#define HMAX                      32 // 32  for emulation 256 for impl
#define READ_BURST_SIZE              4
#define STREAMS_DEPTH               16 // 16  for emulation  64 for impl   //4 original from xilinx
#define INPUT_BUFFER_SIZE        65536 //524288
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_S10MX_8x8_DIRECT_FP32_EMULATION
#define DIRECT_CONV
#define FP32_DATA_TYPE
#define USE_RELU
#define USE_POOLING
#define CPI                          8
#define CPO                          8
#define LOG2_CPO                     3
#define WMAX                        32
#define HMAX                        32
#define READ_BURST_SIZE              4 // 1 ?? JM10 encara que crec que no afecta, 
#define STREAMS_DEPTH                4 //4
#define INPUT_BUFFER_SIZE        65536 //524288
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif


// API 8  CPI 8 CPO 8
#ifdef CONF_S10MX_8x8_DIRECT_API8
#define DIRECT_CONV
#define API8_S10MX_DATA_TYPE
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define CPI                8
#define CPO                8
#define LOG2_CPO           3
#define WMAX             256
#define HMAX             256
#define READ_BURST_SIZE    2
#define STREAMS_DEPTH      2
#define INPUT_BUFFER_SIZE  65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_S10MX_8x8_DIRECT_API8_EMULATION
#define DIRECT_CONV
#define API8_S10MX_DATA_TYPE
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define CPI                8
#define CPO                8
#define LOG2_CPO           3
#define WMAX             32
#define HMAX             32
#define READ_BURST_SIZE    2
#define STREAMS_DEPTH      16
#define INPUT_BUFFER_SIZE  65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

// API 16  CPI 8 CPO 8
#ifdef CONF_S10MX_8x8_DIRECT_API16
#define DIRECT_CONV
#define API16_S10MX_DATA_TYPE
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define CPI                          8
#define CPO                          8
#define LOG2_CPO                     3
#define WMAX                       256
#define HMAX                       256
#define READ_BURST_SIZE              2
#define STREAMS_DEPTH                2
#define INPUT_BUFFER_SIZE        65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_S10MX_8x8_DIRECT_API16_EMULATION
#define DIRECT_CONV
#define API16_S10MX_DATA_TYPE
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define CPI                          8
#define CPO                          8
#define LOG2_CPO                     3
#define WMAX                        32
#define HMAX                        32
#define READ_BURST_SIZE              2
#define STREAMS_DEPTH               16
#define INPUT_BUFFER_SIZE        65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

// API 16  CPI 16 CPO 16
#ifdef CONF_S10MX_16x16_DIRECT_API16
#define DIRECT_CONV
#define API16_S10MX_DATA_TYPE
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define CPI                         16
#define CPO                         16
#define LOG2_CPO                     4
#define WMAX                       128
#define HMAX                       128
#define READ_BURST_SIZE              2
#define STREAMS_DEPTH                2
#define INPUT_BUFFER_SIZE        65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_S10MX_16x16_DIRECT_API16_EMULATION
#define DIRECT_CONV
#define API16_S10MX_DATA_TYPE
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define CPI                         16
#define CPO                         16
#define LOG2_CPO                     4
#define WMAX                        32
#define HMAX                        32
#define READ_BURST_SIZE              2
#define STREAMS_DEPTH               16
#define INPUT_BUFFER_SIZE        65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif


// API 16  CPI 8 CPO 8
#ifdef CONF_S10MX_8x8_DIRECT_API32
#define DIRECT_CONV
#define API32_S10MX_DATA_TYPE
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define CPI                          8
#define CPO                          8
#define LOG2_CPO                     3
#define WMAX                       512
#define HMAX                       256
#define READ_BURST_SIZE              2
#define STREAMS_DEPTH                2
#define INPUT_BUFFER_SIZE        65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_S10MX_8x8_DIRECT_API32_EMULATION
#define DIRECT_CONV
#define API32_S10MX_DATA_TYPE
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define CPI                          8
#define CPO                          8
#define LOG2_CPO                     3
#define WMAX                        32
#define HMAX                        32
#define READ_BURST_SIZE              2
#define STREAMS_DEPTH               16
#define INPUT_BUFFER_SIZE        65536 // 32 rows x 32 cols x (512/CPI) pixels_in
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

// -------------------------------------------


#ifdef CONF_S10MX_4x4_DIRECT_FP32
#define DIRECT_CONV
#define FP32_DATA_TYPE
#define USE_RELU
#define USE_POOLING
#define CPI                          4
#define CPO                          4
#define LOG2_CPO                     2
#define WMAX                       256 // 32  for emulation 256 for impl
#define HMAX                       256 // 32  for emulation 256 for impl
#define READ_BURST_SIZE              4
#define STREAMS_DEPTH               16 // 16  for emulation  64 for impl   //4 original from xilinx
#define INPUT_BUFFER_SIZE        65536 //524288
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_S10MX_8x8_DIRECT_FP32
#define DIRECT_CONV
#define FP32_DATA_TYPE
#define USE_RELU
#define USE_POOLING
#define CPI                          8
#define CPO                          8
#define LOG2_CPO                     3
#define WMAX                       128
#define HMAX                       128
#define READ_BURST_SIZE              4 // 1 ?? JM10 encara que crec que no afecta, 
#define STREAMS_DEPTH               16 //4
#define INPUT_BUFFER_SIZE        65536 //524288
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_S10MX_16x16_DIRECT_FP32
#define DIRECT_CONV
#define FP32_DATA_TYPE
#define USE_RELU
#define USE_POOLING
#define CPI                         16
#define CPO                         16
#define LOG2_CPO                     4
#define WMAX                       128
#define HMAX                       128
#define READ_BURST_SIZE              4 // 1 ?? JM10 encara que crec que no afecta, 
#define STREAMS_DEPTH               32 //4
#define INPUT_BUFFER_SIZE        65536 //524288
#define MAX_KERNELS_DW         512/CPI
#define DW_KERNEL_STREAM_DEPTH       4      // 512 DW kernels
#define PW_KERNEL_STREAM_DEPTH       4      // 512 * 512 PW kernels
#define DWS_STREAM_DEPTH            64
#endif

#ifdef CONF_S10MX_8x8_DIRECT_API8
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

#ifdef CONF_S10MX_16x16_WINOGRAD_API8
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

#ifdef CONF_S10MX_32x32_DWS_API8
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

#ifdef CONF_S10MX_32x64_DWS_API8
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

#ifdef CONF_S10MX_64x64_DWS_API8
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

#ifdef CONF_S10MX_4x4_DWS_API8
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
#ifdef FP16_DATA_TYPE
#define data_type short float
#define DATA_TYPE_WIDTH  32	  // data type width in bits
#define READ_BLOCK_SIZE  16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE -99999
#define EPSILON_VALUE 0.00001
#endif

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


#ifdef API8_S10MX_DATA_TYPE
#define data_type int8_t
#define DATA_TYPE_WIDTH   8	  // data type width in bits
#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -127
#define EPSILON_VALUE 0
#endif

#ifdef API16_S10MX_DATA_TYPE
#define data_type int16_t
#define DATA_TYPE_WIDTH  16	  // data type width in bits
#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -127 // 32767
#define EPSILON_VALUE 0
#endif

#ifdef API32_S10MX_DATA_TYPE
#define data_type int32_t
#define DATA_TYPE_WIDTH  16	  // data type width in bits
#define READ_BLOCK_SIZE  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define MIN_DATA_TYPE_VALUE  -127 // 2147483647
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
}__attribute__((packed));
typedef struct pixel_in_t2_st pixel_in_t2;

// -----------------------------------------------------------------------------------------------------------
// frames struct (KWxKH)
struct frame_t_st {
  pixel_in_t pixel[9];
}__attribute__((packed));
typedef struct frame_t_st frame_t;


// -----------------------------------------------------------------------------------------------------------
// frames struct (4x4) winograd
struct frame_d_st {
  pixel_in_t pixel[16];
}__attribute__((packed));
typedef struct frame_d_st frame_d;

// -----------------------------------------------------------------------------------------------------------
// frames struct (4x4) winograd
struct frame_d_2_st {
  pixel_in_t2 pixel[16];
}__attribute__((packed));
typedef struct frame_d_2_st frame_d_2;

// -----------------------------------------------------------------------------------------------------------
// kernel struct
struct kernel_t_st {
  data_type pixel[CPO][CPI][9];
}__attribute__((packed));
typedef struct kernel_t_st kernel_t;

// -----------------------------------------------------------------------------------------------------------
//kernel read struct
struct kernel_in_t_st {
  data_type pixel[9];
}__attribute__((packed));
typedef struct kernel_in_t_st kernel_in_t;

// -----------------------------------------------------------------------------------------------------------
// kernel struct (deepwise)
struct kernel_dw_t_st {
  data_type pixel[CPI][KH*KW];
}__attribute__((packed));
typedef struct kernel_dw_t_st kernel_dw_t;

// -----------------------------------------------------------------------------------------------------------
// kernel struct (pointwise)
struct kernel_pw_t_st {
  data_type pixel[CPO][CPI];
}__attribute__((packed));
typedef struct kernel_pw_t_st kernel_pw_t;

#define read_kernel_pw_t ac_int<CPI*DATA_TYPE_WIDTH>
// typedef ac_int<CPI*DATA_TYPE_WIDTH> read_kernel_pw_t;


// -----------------------------------------------------------------------------------------------------------
// batch normalization 

typedef data_type add_data_t;
typedef data_type       bn_t;  // JM10 this define (and others) will later depend on the architecture

struct add_data_t_st {
  add_data_t      pixel[CPO];
}__attribute__((packed));
typedef struct add_data_t_st add_data_st_t;

struct bn_t_st {
  bn_t    pixel[CPO];
}__attribute__((packed));
typedef struct bn_t_st bn_st_t;


struct bnp_t_st {
  bn_t    values[CPO*4];
}__attribute__((packed));
typedef struct bnp_t_st bnp_st_t;



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




