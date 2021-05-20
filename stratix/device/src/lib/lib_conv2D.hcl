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
// Header file declaring library function call prototype
// to be included from OpenCL kernel file only: k_conv2D.cl

#include "lib_conv2D_commons.h"
/*
void lib_conv2D(
    #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
    int sss,
    #endif
    int a,
    int *b
    );
*/

void lib_conv2D( 
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
      global int *my_ret);

