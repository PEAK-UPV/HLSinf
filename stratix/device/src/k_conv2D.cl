/********************************
**
**
**
********************************/

#include "lib_conv2D_commons.h"
#include "lib_conv2D.hcl"

#define DEBUG_VERBOSE

__kernel void k_conv2D( 
      global read_block_t * restrict ptr_data,
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
      global data_type * restrict ptr_kernel,
      #endif
      #ifdef DWS_CONV
      global data_type * restrict ptr_dw_kernel, 
      global read_kernel_pw_t * restrict ptr_pw_kernel,
      #endif
      global pixel_out_t * restrict ptr_bias,
      global write_block_t * restrict ptr_out,
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
      int pos_shift
#ifdef HLS_DEBUG
      ,
      unsigned long         my_val,
      global unsigned long  * restrict dbg_ptr_ul,
      global data_type      * restrict dbg_ptr_dt,
      global read_block_t   * restrict dbg_loop_ptr_data_in,
      global read_block_t   * restrict dbg_loop_ptr_data_input_buffer,
      global read_block_t   * restrict dbg_loop_ptr_data_dc_pad_out,
      global frame_t        * restrict dbg_loop_ptr_data_dc_cvt_out,
      global hls_cvt_sbs_control_t  * restrict dbg_loop_ptr_data_dc_cvt_sbs_control_out,
      global frame_t                * restrict dbg_loop_ptr_data_dc_cvt_sbs_frame_out,
      global write_block_t  * restrict dbg_loop_ptr_data_dc_mul_out,
      global write_block_t  * restrict dbg_loop_ptr_data_directconv_out
#endif
      ) {

    //printf("OCL kernel entry point received value=%lu  ret=%lu\n\n", my_val, *my_ret);
    #ifdef HLS_DEBUG
     printf("\n\n");
     printf("OpenCL k_conv2D H %d   W %d   rows %d   I %d   O %d   I_ITER %d   o_iter_first %d   o_iter_last %d   enable_relu %d   global_offset %d\n",
              H, W, rows, I, O, I_ITER, o_iter_first, o_iter_last, enable_relu, global_offset);
     printf("                upper_padding %d   lower_padding %d, maxpooling %d   avgpooling %d   enable clip %d   enable shift %d \n",
              enable_upper_padding, enable_lower_padding, enable_maxpooling, enable_avgpooling, enable_clipping, enable_shift );
     printf("\n");
    #endif

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
      pos_shift
      #ifdef HLS_DEBUG
      ,
      my_val,
      dbg_ptr_ul,
      dbg_ptr_dt,
      dbg_loop_ptr_data_in,
      dbg_loop_ptr_data_input_buffer,
      dbg_loop_ptr_data_dc_pad_out,
      dbg_loop_ptr_data_dc_cvt_out,
      dbg_loop_ptr_data_dc_cvt_sbs_control_out,
      dbg_loop_ptr_data_dc_cvt_sbs_frame_out,
      dbg_loop_ptr_data_dc_mul_out,
      dbg_loop_ptr_data_directconv_out
      #endif
      );


    #ifdef DEBUG_VERBOSE
    printf("kernel finishes\n");
    #endif

  }





