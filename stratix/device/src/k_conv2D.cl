/********************************
**
**
**
********************************/


/*
#define AP_INT_MAX_W 4096 // Must be defined before includes
#define CONF_ALVEO_U200_4x4_DIRECT_FP32    
#define GIHWCPI_DATA_FORMAT

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
  struct pixel_in_t pixel[9];
};

// -----------------------------------------------------------------------------------------------------------
// frames struct (4x4) winograd
struct frame_d {
  struct pixel_in_t pixel[16];
};

// -----------------------------------------------------------------------------------------------------------
// frames struct (4x4) winograd
struct frame_d_2 {
  struct pixel_in_t2 pixel[16];
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
#define read_block_t struct pixel_in_t
#endif

// -----------------------------------------------------------------------------------------------------------
// Write block struct
#ifdef IHW_DATA_FORMAT
#define write_block_t ap_int<512>
#endif
#ifdef GIHWCPI_DATA_FORMAT
#define write_block_t struct pixel_out_t
#endif

*/

#include "lib_conv2D_commons.h"
#include "lib_conv2D.hcl"

#define DEBUG_VERBOSE

__kernel void k_conv2D( 
      global struct read_block_t *ptr_data,
      int H, 
      int W, 
      int rows,
      int I,
      int O,
      int I_ITER,
      int o_iter_first,
      int o_iter_last,
      int enable_relu,
      #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
      global data_type *ptr_kernel,
      #endif
      #ifdef DWS_CONV
      global data_type *ptr_dw_kernel, 
      global read_kernel_pw_t *ptr_pw_kernel,
      #endif
      global struct pixel_out_t *ptr_bias,
      global struct write_block_t *ptr_out,
      int global_offset,
      int enable_upper_padding,
      int enable_lower_padding,
      int enable_maxpooling,
      int enable_avgpooling,
      int enable_clipping,
      int enable_shift,
      int min_clip,
      int max_clip,
      int dir_shift,
      int pos_shift,
      int my_val,
      global int *my_ret) {


    printf( "Like Adele would say:\n");
    printf( "  Hello from the other side\n");
/*
    int a = 3; 
    int b = 2;
    
    lib_conv2D(
        #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
        100,
        #endif
        a,
        &b);
    printf(" a=%d  b=%d\n\n", a, b);

    *my_ret = *my_ret + my_val;
*/

    lib_conv2D( 
      ptr_data,
      H, 
      W, 
      rows,
      I,
      O,
      I_ITER,
      o_iter_first,
      o_iter_last,
      enable_relu,
      #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
      ptr_kernel,
      #endif
      #ifdef DWS_CONV
      ptr_dw_kernel, 
      ptr_pw_kernel,
      #endif
      ptr_bias,
      ptr_out,
      global_offset,
      enable_upper_padding,
      enable_lower_padding,
      enable_maxpooling,
      enable_avgpooling,
      enable_clipping,
      enable_shift,
      min_clip,
      max_clip,
      dir_shift,
      pos_shift,
      my_val,
      my_ret);


    #ifdef DIRECT_CONV
    printf(" DIRECT_CONV defined\n");
    #endif
    #ifdef WINOGRAD_CONV
    printf(" WINOGRAD_CONV defined\n");
    #endif
    #ifdef DWS_CONV
    printf(" DWS_CONV defined\n");
    #endif



    #ifdef DEBUG_VERBOSE
    printf("kernel finishes\n");
    #endif

  }



