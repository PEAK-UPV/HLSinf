// --------------------------------------------------------------------------------------------------------------
// HLSinf kernels
// Version: 1.0
// copyright (c) 2021, Universidad Politècnica de València (UPV), GAP research group
// Date: May 2021
// Authors: GAP Research Group (UPV)
//     José Flich Cardo
//     Jorge García Martínez
//     Izan Catalán Gallarch
//     Carles Hernández Luz
//     Jose Maria Martinez
//
// contact: jflich@disca.upv.es
// All rights reserved
//
// 
//
// WARNING: 
// Libraries that target OpenCL* and are written in HLS cannot
// use streams or pipes as an interface between OpenCL code
// and the library written in HLS.
// However, the library in HLS can use streams or pipes if both
// endpoints are within the library (for example, a stream that
// connects two task functions).

#include "HLS/hls.h"
#include "HLS/math.h"
#include "HLS/ocl_types.h"
//#ifndef HLS_SYNTHESIS
//  #include <iostream>
//#endif

#include "HLS/stdio.h"


//ihc::stream<float> s0, s1;
//ihc::stream<float> s0;
//ihc::stream<float> s1;

#include "lib_conv2D_commons.h"

//#define DIRECT_CONV


extern "C" {
  
  // **********************************************************************************************
  // library main component 
  // ********************************************************************************************** 
//  void lib_relu_ts(OCL_ADDRSP_GLOBAL float* A, OCL_ADDRSP_GLOBAL float* B, const long int size) {
//  }

  /*
  void lib_conv2D(
      #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
      int sss,
      #endif
      int a, 
      int *b
      ) {
    *b = 2 * a;

    #ifdef DIRECT_CONV
    *b = *b + sss;
    #endif
    #ifdef WINOGRAD_CONV
    *b = *b + 300;
    #endif
    #ifdef DWS_CONV
    *b = *b + 500;
    #endif
  }
  */

void lib_conv2D( 
      OCL_ADDRSP_GLOBAL struct read_block_t *ptr_data,
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
      OCL_ADDRSP_GLOBAL data_type *ptr_kernel,
      #endif
      #ifdef DWS_CONV
      OCL_ADDRSP_GLOBAL data_type *ptr_dw_kernel, 
      OCL_ADDRSP_GLOBAL read_kernel_pw_t *ptr_pw_kernel,
      #endif
      OCL_ADDRSP_GLOBAL struct pixel_out_t *ptr_bias,
      OCL_ADDRSP_GLOBAL struct write_block_t *ptr_out,
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
      OCL_ADDRSP_GLOBAL int *my_ret){

        // this currently is a dummy lib....

        *my_ret = *my_ret + my_val;
      }




}
