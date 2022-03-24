/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#ifndef _CONV2D_H_
#define _CONV2D_H_

#define AP_INT_MAX_W 8192 // Must be defined before includes
#define AP_UINT_MAX_W 8192 // Must be defined before includes

#include <stdio.h>
#include <ap_fixed.h>
#include <ap_int.h>

#ifndef OPENCL_TEST
#include <hls_stream.h>
#include <hls_math.h>
#endif

// -----------------------------------------------------------------------------------------------------------
// Configuration selection.
// Select only one configuration. Each configuration defines the target device, the type of convolution, the
// arithmetic precision format, and the size of the kernel in input channels and output channels.
// Each configuration is optimized for the specific targeted board
// -----------------------------------------------------------------------------------------------------------

#define HLSINF_1_15
//#define HLSINF_1_0  // U200, 4x4,  FP32:             DIRECT_CONV, RELU, STM, CLIPPING,        POOLING, BN, ADD, UPSIZE
//#define HLSINF_1_1  // U200, 8x8,  MIXED PRECISSION: DIRECT_CONV, RELU,      CLIPPING, SHIFT, POOLING, BN, ADD, UPSIZE
//#define HLSINF_1_2  // U200, 16x8, MIXED PRECISSION: DIRECT_CONV, RELU,      CLIPPING, SHIFT, POOLING, BN, ADD, UPSIZE
//#define HLSINF_1_3  // U200, 8x4,  FP32:             DIRECT_CONV, RELU, STM, CLIPPING,        POOLING, BN, ADD, UPSIZE

//#define HLSINF_TEST // U200, 4x8,  FP32:             DIRECT_CONV, RELU, STM, CLIPPING,        POOLING, BN, ADD, UPSIZE
//#define HLSINF_TEST2 // U200, 6x6,  FP32:             DIRECT_CONV, RELU, STM, CLIPPING,        POOLING, BN, ADD, UPSIZE
//#define HLSINF_TEST3 // U280, 4x4,  FP32:             DIRECT_CONV, RELU, STM, CLIPPING,        POOLING, BN, ADD, UPSIZE


// -----------------------------------------------------------------------------------------------------------
// defines for debug (DEBUG_ALL activates all debug defines)
// -----------------------------------------------------------------------------------------------------------
//#define DEBUG_ALL
//#define DEBUG_VERBOSE
//#define DEBUG_READ_BIAS
//#define DEBUG_READ_KERNEL
//#define DEBUG_READ_DATA
//#define DEBUG_READ_BATCH_NORM
//#define DEBUG_SERIALIZE
//#define DEBUG_JOIN
//#define DEBUG_BUFFER
//#define DEBUG_PADDING
//#define DEBUG_CVT
//#define DEBUG_MUL
//#define DEBUG_ADD
//#define DEBUG_SPLIT
//#define DEBUG_BLOCK
//#define DEBUG_WRITE_DATA
//#define DEBUG_RELU
//#define DEBUG_STM
//#define DEBUG_POOL
//#define DEBUG_BATCH_NORM
//#define DEBUG_ADD_DATA
//#define DEBUG_WEIGHT_BUFFER
//#define DEBUG_OUTPUT_BUFFER
//#define DEBUG_CPU

// -----------------------------------------------------------------------------------------------------------
// Automatic defines (do not change; add new ones if needed)
// -----------------------------------------------------------------------------------------------------------

// Configuration 1.0: U200, 4x4, FP32: DIRECT_CONV, RELU, STM, CLIPPING, POOLING, BATCH_NORM, ADD, UPSIZE
#ifdef HLSINF_1_0
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_BATCH_NORM
#define USE_STM
#define USE_ADD
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          4
#define CPO                          4
#define LOG2_CPO                     2
#define WMAX                      1024 
#define HMAX                       256 
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE        16384 // 32 rows x 32 cols x (512/CPI) pixels_in
#define WEIGHT_BUFFER_SIZE        4096
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE   -9999999
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    float
#define conv_cvt_t               float
#define conv_mul_t               float
#define relu_t                   float
#define stm_t                    float
#define pool_cvt_t               float
#define pool_t                   float
#define bn_t                     float
#define add_t                    float
#define w_t                      float
#define b_t                      float
#define conv_t                   float
#define dout_t                   float
#define read_bias_t              float
#define read_data_t              float
#define read_filter_t            float
#define write_data_t             float
#endif

// Configuration 1.1: U200, 8x8, MIXED PRECISSION: DIRECT_CONV, RELU, CLIPPING, SHIFT, POOLING, BN, ADD, UPSIZE
#ifdef HLSINF_1_1
#define ALVEO_U200
//#define DSP_OPTIMIZATION
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define USE_BATCH_NORM
#define USE_ADD
#define USE_UPSIZE
//#define USE_STM
#define CPI                               8
#define CPO                               8
#define LOG2_CPO                          3
#define WMAX                           1024 
#define HMAX                            256
#define READ_BURST_SIZE                  16
#define STREAMS_DEPTH                    16
#define DATA_BUFFER_SIZE              32768 // 32 rows x 32 cols x (512/CPI) pixels_in
#define WEIGHT_BUFFER_SIZE             5000
#define EPSILON_VALUE               0.00001
#define MIN_DATA_TYPE_VALUE               0
#define READ_BLOCK_SIZE                  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE                 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_uint<8>
#define conv_cvt_t               ap_uint<8>
#define conv_mul_t               ap_int<32>
#define relu_t                   ap_uint<8>
#define stm_t                    ap_uint<8>
#define pool_cvt_t               ap_uint<8>
#define pool_t                   ap_uint<8>
#define bn_t                     ap_uint<8>
#define add_t                    ap_uint<8>
#define w_t                       ap_int<8>
#define b_t                      ap_int<32>
#define conv_t                   ap_int<32>
#define dout_t                   ap_uint<8>
#endif

// HLSINF_1_2: U200, 16x16, MIXED PRECISSION: DIRECT_CONV, RELU, CLIPPING, SHIFT, POOLING, UPSIZE
#ifdef HLSINF_1_2
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
//#define USE_BATCH_NORM
//#define USE_ADD
//#define USE_STM
#define USE_UPSIZE
#define CPI                              16
#define CPO                              16
#define WMAX                            512
#define HMAX                            256
#define READ_BURST_SIZE                  16
#define STREAMS_DEPTH                    16
#define DATA_BUFFER_SIZE               8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE               0.00001
#define MIN_DATA_TYPE_VALUE               0
#define READ_BLOCK_SIZE                  64   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE                 64   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_uint<8>
#define conv_cvt_t               ap_uint<8>
#define conv_mul_t               ap_int<32>
#define relu_t                   ap_uint<8>
#define stm_t                    ap_uint<8>
#define pool_cvt_t               ap_uint<8>
#define pool_t                   ap_uint<8>
#define bn_t                     ap_uint<8>
#define add_t                    ap_uint<8>
#define w_t                       ap_int<8>
#define b_t                      ap_int<32>
#define conv_t                   ap_int<32>
#define dout_t                   ap_uint<8>
#endif

// Configuration 1.3: U200, 6x6, FP32: DIRECT_CONV, RELU, POOLING, UPSIZE
#ifdef HLSINF_1_3
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_POOLING
//#define USE_CLIPPING
//#define USE_SHIFT
//#define USE_BATCH_NORM
//#define USE_STM
//#define USE_ADD
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          6
#define CPO                          6
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               64
#define DATA_BUFFER_SIZE         16384 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE   -9999999
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    float
#define conv_cvt_t               float
#define conv_mul_t               float
#define relu_t                   float
#define stm_t                    float
#define pool_cvt_t               float
#define pool_t                   float
#define bn_t                     float
#define add_t                    float
#define w_t                      float
#define b_t                      float
#define conv_t                   float
#define dout_t                   float
#endif

// Configuration 1.4: U200, 8x8, FP32: DIRECT_CONV, RELU, POOLING, UPSIZE
#ifdef HLSINF_1_4
#define PRUEBA_MUL_2
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_POOLING
//#define USE_CLIPPING
//#define USE_SHIFT
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_ADD
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          8
#define CPO                          8
#define LOG2_CPO                     2
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE   -9999999
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    float
#define conv_cvt_t               float
#define conv_mul_t               float
#define relu_t                   float
#define stm_t                    float
#define pool_cvt_t               float
#define pool_t                   float
#define bn_t                     float
#define add_t                    float
#define w_t                      float
#define b_t                      float
#define conv_t                   float
#define dout_t                   float
#endif

// Configuration 1.5: U200, 4x4, APF<32,16>+APF<16,8>: DIRECT_CONV, RELU, POOLING, ADD
#ifdef HLSINF_1_5
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          4
#define CPO                          4
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE     -65536
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.6: U200, 4x4, APF<32,16>+APF<8,4>: DIRECT_CONV, RELU, POOLING, ADD
#ifdef HLSINF_1_6
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          4
#define CPO                          4
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE     -65536
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<32,16,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.7: U200, 4x4, APF<16,8>: DIRECT_CONV, RELU, POOLING, ADD
#ifdef HLSINF_1_7
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          4
#define CPO                          4
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE       -256
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.8: U200, 4x4, APF<8,4>: DIRECT_CONV, RELU, POOLING, ADD
#ifdef HLSINF_1_8
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          4
#define CPO                          4
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE        -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.9: U200, 4x4, APF<8,4> weight multiplication, bias <16,8>: DIRECT_CONV, RELU, CLIPPING, POOLING, ADD
#ifdef HLSINF_1_9
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          4
#define CPO                          4
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE        -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.10: U200, 8x8, APF<8,4> weight multiplication, bias <16,8>: DIRECT_CONV, RELU, CLIPPING, POOLING, ADD
#ifdef HLSINF_1_10
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          8
#define CPO                          8
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE        -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.11: U200, 16x16, APF<8,4> weight multiplication, bias <16,8>: DIRECT_CONV, RELU, CLIPPING, POOLING, ADD
#ifdef HLSINF_1_11
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                         16
#define CPO                         16
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE        -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.12: U200, 16x16, APF<8,4> weight multiplication, bias <16,8>: DIRECT_CONV, RELU, CLIPPING, POOLING, ADD, *DSP optimization*
#ifdef HLSINF_1_12
#define DSP_OPTIMIZATION
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                         16
#define CPO                         16
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE        -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.13: U200, 16x16, APF<8,4> weight multiplication, bias <16,8>: DIRECT_CONV, RELU, CLIPPING, POOLING, ADD, *solo sumas*
#ifdef HLSINF_1_13
#define PRUEBA_MUL
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                         16
#define CPO                         16
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE        -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.14: U200, 16x16, APF<8,4> weight multiplication, bias <16,8>: DIRECT_CONV, RELU, CLIPPING, POOLING, ADD, PRUEBA_MUL2
#ifdef HLSINF_1_14
#define PRUEBA_MUL_2
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                         16
#define CPO                         16
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE        -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#endif

// Configuration 1.15: U200, 4x4, FP32: DIRECT_CONV, RELU, POOLING, ADD, PRUEBA_MUL2, weight buffer, data buffer
#ifdef HLSINF_1_15
#define PRUEBA_MUL_2
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          4
#define CPO                          4
#define WMAX                      1024  // add-module buffer is of size WMAX x HMAX rows of CPO x sizeof(float) word bits = 131072 x 128. Mapped on URAMs of 4096 x 72 bits = 64 URAMs (32 x 2 URAMs)
#define HMAX                       128
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               2
#define DATA_BUFFER_SIZE          8192  // input buffers are of size 8192 x CPI x sizeof(float) = 8192 x 128. Mapped on URAms of 4096 x 72 bits = 4 URAMs (2 x 2 URAMs)
#define WEIGHT_BUFFER_SIZE        4096
#define EPSILON_VALUE            0.001
#define MIN_DATA_TYPE_VALUE  -99999999
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    float  // ap_intv8v replace this, the design fille be int9
#define conv_cvt_t               float
#define conv_mul_t               float
#define relu_t                   float
#define stm_t                    float
#define pool_cvt_t               float
#define pool_t                   float
#define bn_t                     float
#define add_t                    float
#define w_t                      float
#define b_t                      float
#define conv_t                   float
#define dout_t                   float
#define w_pw_t                   float  // change these float to 8 bit, the floating weights input will be converted to 8 bit aotomaticlly ,, do not change reas_xxx
#define read_bias_t              float
#define read_data_t              float
#define read_filter_t            float
#define write_data_t             float
#endif

// Configuration 1.16: U200, 16x16, APF<8,4> weight multiplication, bias <16,8>: DIRECT_CONV, RELU, CLIPPING, POOLING, ADD, PRUEBA_MUL2, weight buffer, data buffers, precission conversion
#ifdef HLSINF_1_16
#define PRUEBA_MUL_2
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
#define USE_SHIFT
#define USE_POOLING
#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define USE_UPSIZE
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                         16
#define CPO                         16
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          4096
#define WEIGHT_BUFFER_SIZE        4096
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE        -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<16,8,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define read_bias_t              float
#define read_data_t              float
#define read_filter_t            float
#define write_data_t             float
#endif


// Configuration TEST: U200, 4x4, FP32: DWS_CONV, RELU, POOLING, UPSIZE
#ifdef HLSINF_TEST2
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
//#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          4
#define CPO                          4
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE       -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<8,4,AP_RND_ZERO,AP_SAT>
#endif

// Configuration TEST: U200, 4x4, FP32: DWS_CONV, RELU, POOLING, UPSIZE
#ifdef HLSINF_TEST3
#define ALVEO_U200
#define DIRECT_CONV
#define USE_RELU
//#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
//#define USE_ADD
//#define USE_BATCH_NORM
//#define USE_STM
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          4
#define CPO                          4
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE       -16
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define conv_cvt_t               ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define conv_mul_t               ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define relu_t                   ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define stm_t                    ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define pool_cvt_t               ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define pool_t                   ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define bn_t                     ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define add_t                    ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define w_t                      ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define b_t                      ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define conv_t                   ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define dout_t                   ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#define w_pw_t                   ap_fixed<16,4,AP_RND_ZERO,AP_SAT>
#endif

// Configuration TEST4: U280, 8x4, FP32: DIRECT_CONV, RELU, STM, CLIPPING, POOLING, BATCH_NORM, ADD, UPSIZE
#ifdef HLSINF_TEST4
#define ALVEO_U280
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_BATCH_NORM
#define USE_STM
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          8
#define CPO                          4
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE   -9999999
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    float
#define conv_cvt_t               float
#define conv_mul_t               float
#define relu_t                   float
#define stm_t                    float
#define pool_cvt_t               float
#define pool_t                   float
#define bn_t                     float
#define add_t                    float
#define w_t                      float
#define b_t                      float
#define conv_t                   float
#define dout_t                   float
#endif

// Configuration TEST5: U280, 6x6, FP32: DIRECT_CONV, RELU, STM, CLIPPING, POOLING, BATCH_NORM, ADD, UPSIZE
#ifdef HLSINF_TEST5
#define ALVEO_U280
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_BATCH_NORM
#define USE_STM
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          6
#define CPO                          6
//#define LOG2_CPO                     3
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE   -9999999
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    float
#define conv_cvt_t               float
#define conv_mul_t               float
#define relu_t                   float
#define stm_t                    float
#define pool_cvt_t               float
#define pool_t                   float
#define bn_t                     float
#define add_t                    float
#define w_t                      float
#define b_t                      float
#define conv_t                   float
#define dout_t                   float
#endif

// Configuration TEST6: U280, 8x8, FP32: DIRECT_CONV, RELU, STM, CLIPPING, POOLING, BATCH_NORM, ADD, UPSIZE
#ifdef HLSINF_TEST6
#define ALVEO_U280
#define DIRECT_CONV
#define USE_RELU
#define USE_CLIPPING
//#define USE_SHIFT
#define USE_POOLING
#define USE_BATCH_NORM
#define USE_STM
#define FLOAT_DATA_TYPE               // we use float numbers as input data
#define CPI                          8
#define CPO                          8
//#define LOG2_CPO                     3
#define WMAX                      1024
#define HMAX                       256
#define READ_BURST_SIZE             16
#define STREAMS_DEPTH               16
#define DATA_BUFFER_SIZE          8192 // 32 rows x 32 cols x (512/CPI) pixels_in
#define EPSILON_VALUE          0.00001
#define MIN_DATA_TYPE_VALUE   -9999999
#define READ_BLOCK_SIZE             16   // Read block size. READ_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define WRITE_BLOCK_SIZE            16   // Write block size. WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH must be 512 for max perf.
#define din_t                    float
#define conv_cvt_t               float
#define conv_mul_t               float
#define relu_t                   float
#define stm_t                    float
#define pool_cvt_t               float
#define pool_t                   float
#define bn_t                     float
#define add_t                    float
#define w_t                      float
#define b_t                      float
#define conv_t                   float
#define dout_t                   float
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
#define W_SIM         64 //WMAX
#define H_SIM         64 //HMAX
#define I_SIM         CPI //I_REFERENCE
#define O_SIM         CPO //O_REFERENCE
#define PT_SIM        1   
#define PB_SIM        1
#define PL_SIM        1
#define PR_SIM        1
#define SH_SIM        1   // SH
#define SW_SIM        1   // SW
#define INSTANCES_SIM 2   //2

// -----------------------------------------------------------------------------------------------------------
// Direction defines (for shift operations)
// -----------------------------------------------------------------------------------------------------------
#define LEFT_DIRECTION  0	// direction used in ReLU (shift) module
#define RIGHT_DIRECTION 1   // direction used in ReLU (shift) module

// -----------------------------------------------------------------------------------------------------------
// depth of pointers for co-simulation support
// -----------------------------------------------------------------------------------------------------------
#define DATA_IN_PORT_DEPTH   W_SIM * H_SIM * I_SIM / CPI
#define DATA_OUT_PORT_DEPTH  W_SIM * H_SIM * O_SIM / CPO
#define KERNEL_PORT_DEPTH    3 * 3 * I_SIM * O_SIM
#define DW_KERNEL_PORT_DEPTH I_SIM * 9
#define PW_KERNEL_PORT_DEPTH O_SIM * (I_SIM / CPI)
#define BIAS_PORT_DEPTH      O_SIM / CPO
#define BATCH_MORM_VAL_DEPTH O_SIM * 4 / CPI

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

// -----------------------------------------------------------------------------------------------------------
// Defines for debug
// -----------------------------------------------------------------------------------------------------------
#ifdef DEBUG_ALL
#define DEBUG_READ_BIAS
#define DEBUG_READ_KERNEL
#define DEBUG_READ_DATA
#define DEBUG_READ_BATCH_NORM
#define DEBUG_SERIALIZE
#define DEBUG_JOIN
#define DEBUG_BUFFER
#define DEBUG_PADDING
#define DEBUG_CVT
#define DEBUG_MUL
#define DEBUG_ADD
#define DEBUG_SPLIT
#define DEBUG_BLOCK
#define DEBUG_WRITE_DATA
#define DEBUG_RELU
#define DEBUG_STM
#define DEBUG_POOL
#define DEBUG_BATCH_NORM
#define DEBUG_ADD_DATA
#define DEBUG_UPSIZE
#define DEBUG_WEIGHT_BUFFER
#define DEBUG_OUTPUT_BUFFER
#define DEBUG_CPU
#endif

// ----------------------------------------------------------------------------------------------------------
// Data types
struct din_st      {din_t      pixel[CPI];};
struct conv_cvt_st {din_st     pixel[9];}; // 3x3 filter size  kernal w and h  data input  of mul
struct conv_mul_st {conv_mul_t pixel[CPO];}; // mul output  of mul
struct conv_st     {conv_t     pixel[CPO];};
struct relu_st     {relu_t     pixel[CPO];};
struct stm_st      {stm_t      pixel[CPO];};
struct pool_cvt_st {pool_cvt_t pixel[4];};
struct pool_st     {pool_t     pixel[CPO];};
struct bn_st       {bn_t       pixel[CPO];};
struct add_st      {add_t      pixel[CPO];};
struct dout_st     {dout_t     pixel[CPO];};
struct w2_st       {w_t        pixel[CPI][9];};
struct w_st        {w_t        pixel[CPO][CPI][9];};  //input for weights   // pixel is a name
struct w_in_st     {w_t        pixel[9];};
struct b_st        {b_t        pixel[CPO];};
struct bnp_st      {bn_t       values[CPO*4];};
#ifdef DWS_CONV
struct w_dw_st     {w_t        pixel[CPI][9];};
struct w_pw_st     {w_pw_t     pixel[CPI];};
#endif
struct read_bias_st {read_bias_t     pixel[CPO];};
struct read_data_st {read_data_t     pixel[CPI];};
struct read_filter_st {read_filter_t pixel[CPO][CPI][9];};
struct write_data_st  {write_data_t  pixel[CPO];};

// -----------------------------------------------------------------------------------------------------------
// Read and write block struct
#define read_block_t  read_data_st
#define write_block_t write_data_st

// What follows is the function prototypes

// Function prototypes protected with the no-definition of OPENCL_TEST to avoid warnings when compiling for OpenCL
#ifndef OPENCL_TEST

// -----------------------------------------------------------------------------------------------------------
// function prototypes
extern "C" void k_conv2D(read_block_t *ptr_data, 
                         #ifdef USE_ADD 
                         write_block_t *ptr_data_add, 
                         #endif
                         int H, int W, int HO, int WO, int rows, int PT, int PB, int PL, int PR, int SH, int SW, 
                         int I, int O, int I_ITER, int o_iter_first, int o_iter_last, 
                         int enable_relu, int enable_stm, float relu_factor, 
                         #ifdef USE_BATCH_NORM 
                         int enable_batch_norm,
                         #endif
                         #ifdef DIRECT_CONV
                         read_filter_t *ptr_kernel,
                         #endif
                         #ifdef DWS_CONV
						 w_t *ptr_dw_kernel, w_pw_t *ptr_pw_kernel,
                         #endif
                         read_bias_st *ptr_bias,
                         #ifdef USE_BATCH_NORM
                         bnp_st *b_ptr, 
                         #endif
                         write_block_t *ptr_out, 
                         int read_offset, int write_offset, int enable_maxpooling, int enable_avgpooling,
						 int enable_clipping, int enable_shift, 
                         #ifdef USE_ADD
                         int enable_add, 
                         #endif
                         int min_clip, int max_clip, 
                         int dir_shift, int pos_shift, int enable_upsize,
                         int write_to_weight_buffer, int read_from_weight_buffer, int first_row_weight_buffer,
 						 int read_from_mem, int read_from_b0, int read_from_b1, int write_to_mem, int write_to_b0, int write_to_b1);

void read_bias                    (int offset_bias, read_bias_st *b_ptr, hls::stream<b_st> &out);
void read_batch_norm              (int offset_batchnorm, bnp_st *b_ptr, hls::stream<bnp_st> &out);
void read_kernel                  (int enable, int I_ITER, int offset_kernel, read_filter_t *k_ptr, hls::stream<w_t> &k_out);
void weight_buffer                (int I_ITER, int write_to_buff, int read_from_buff, int offset_buff, hls::stream<w_t> &in, hls::stream<w2_st> &out);
void prepare_weight_filters(int I_ITER, hls::stream<w2_st> &in, hls::stream<w_st> &out);
void read_data_channels_gihwcpi   (int num_pixels, int offset, int I_ITER, int cpi_group_offset, read_block_t *ptr, hls::stream<din_st> &out, int enable);

void read_input_add_gihwcpi       (int num_pixels, int offset, write_block_t *ptr, hls::stream<dout_st> &out, int enable);
void write_data_channels_gihwcpi  (int num_pixels, int offset, write_block_t *ptr, hls::stream<dout_st> &in, int write_to_obuf, hls::stream<dout_st> &out);
void buffer0                      (int num_accesses, int read, int read_from_input, int write, int first_write_address, hls::stream<din_st> &input, hls::stream<dout_st> &in, hls::stream<din_st> &out);
void buffer1                      (int num_accesses, int read, int write, int first_write_address, hls::stream<dout_st> &in, hls::stream<din_st> &out);
void mux                          (int num_accesses, int sel, hls::stream<din_st> &in0, hls::stream<din_st> &in1, hls::stream<din_st> &out);
void demux                        (int enable, int num_accesses, int sel, hls::stream<dout_st> &in, hls::stream<dout_st> &out0, hls::stream<dout_st> &out1);

void relu                         (int enable_relu, int enable_clipping, int enable_shift, float relu_factor, int min_clip, int max_clip, int direction_shift, int pos_shift, int num_pixels, hls::stream<conv_st> &in, hls::stream<relu_st> &out);
void stm                          (int enable_stm, int num_pixels, hls::stream<relu_st> &in, hls::stream<stm_st> &out);
void pooling                      (int H, int W, int enable_maxpooling, int enable_avgpooling, hls::stream<stm_st> &input, hls::stream<pool_st> &output);
void batch_norm                   (int enable_batch_norm, int num_pixels, hls::stream<pool_st> &in, hls::stream<bnp_st> &bn_values, hls::stream<dout_st> &out);
template <class in1_st, class in2_st, class out_st> void add_data(int enable_add, int num_pixels, hls::stream<in1_st> &in_r, hls::stream<in2_st> &in_stm, hls::stream<out_st> &out);
void padding                      (int H, int W, int PT, int PB, int PL, int PR, int I_ITER, hls::stream<din_st> &in, hls::stream<din_st> &out);
void add                          (int num_pixels, int I_ITER, hls::stream<conv_mul_st> &in, hls::stream<b_st> &b_in, hls::stream<conv_st> &out);
#ifdef DIRECT_CONV
void direct_conv                  (int H, int W, int PT, int PB, int PL, int PR, int SH, int SW, int num_output_conv_pixels, int I_ITER, hls::stream<din_st> &in, hls::stream<w_st> &k_in, hls::stream<b_st> &b_in, hls::stream<conv_st> &out);
void mul                          (int num_data_frames, int I_ITER, hls::stream<conv_cvt_st> &in, hls::stream<w_st> &k_in, hls::stream<conv_mul_st> &out);
#endif
#ifdef DWS_CONV
void dws_conv(int H, int W, int PT, int PB, int PL, int PR, int SH, int SW, int num_output_conv_pixels, int I_ITER, hls::stream<din_st> &in, hls::stream<w_dw_st> &k_dw_in, hls::stream<w_pw_st> &k_pw_in, hls::stream<b_st> &b_in, hls::stream<conv_st> &out);
void dws_mul(int num_data_frames, int I_ITER, hls::stream<conv_cvt_st> &in, hls::stream<w_dw_st> &k_dw_in, hls::stream<w_pw_st> &k_pw_in, hls::stream<conv_mul_st> &out);
#endif
void cvt                          (int H, int W, int SH, int SW, int I_ITER, hls::stream<din_st> &in, hls::stream<conv_cvt_st> &out);

// -----------------------------------------------------------------------------------------------------------
// What follows are macros used in the code
// To allow using defines inside Xilinx pragmas
#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

#endif

#endif
