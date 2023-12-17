/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

/*
 * kernel-related functions for test
 */

#include "test_conv2D.h"

void run_kernel(int rows_p, int PT_p, int PB_p, int PL_p, int PR_p, int read_offset_p, int write_offset_p) {
  int num_kernels;
  int o_iter_per_kernel;

  int write_to_weight_buffer = 0;
  int read_from_weight_buffer = 0;
  int first_row_weight_buffer = 0;
  int read_from_mem = 1;
  int read_from_b0 = 0;
  int read_from_b1 = 0;
  int write_to_mem = 1;
  int write_to_b0 = 0;
  int write_to_b1 = 0;

  // each kernel processes a group of output iterations
  // there must be at least one iteration per kernel, if not then
  // we force to use only one kernel
  if (o_iter < NUM_KERNELS) {
	  num_kernels = 1;
	  o_iter_per_kernel = o_iter;
  } else {
	  num_kernels = NUM_KERNELS;
	  o_iter_per_kernel = o_iter / num_kernels;
  }

  for (int k=0; k<num_kernels; k++) {


    int o_iter_first = o_iter_per_kernel * k;
    int o_iter_last  = o_iter_first + o_iter_per_kernel - 1;
    if (k == num_kernels-1) o_iter_last = o_iter-1;

    char str[50];
    sprintf(str, "launching kernel %d (output iterations %d to %d)", k, o_iter_first, o_iter_last);
    print_message(str);

    #ifdef OPENCL_TEST
	// set kernel arguments
    cl_int err;
    int arg = 0;
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_i));
    #ifdef USE_ADD
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_i_add));
    #endif
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, H));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, W));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, HO_final));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, WO_final));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, rows_p));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, PT_p));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, PB_p));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, PL_p));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, PR_p));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, SH));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, SW));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, I_input));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, O_output));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, i_iter));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, o_iter_first));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, o_iter_last));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_relu));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_stm));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, relu_factor));
    #ifdef USE_BATCH_NORM
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_batch_norm));
    #endif
    #ifdef USE_BATCH_NORM_RELU
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_batch_norm_relu));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, batch_norm_relu_factor));
    #endif    
    #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_k[0]));
    #endif
    #ifdef DWS_CONV
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_k_dw[0]));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_k_pw[0]));
    #endif
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_bias[0]));
    #ifdef USE_BATCH_NORM
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_batch_norm_val[0]));
    #endif
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_o[0]));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_offset_p));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_offset_p));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_maxpooling));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_avgpooling));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_clipping));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_shift));
    #ifdef USE_ADD
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_add));
    #endif
    #ifdef USE_ADD_RELU
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, apply_add_relu));
    #endif
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, min_clip));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, max_clip));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, dir_shift));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, pos_shift));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, upsize_factor));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_to_weight_buffer));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_from_weight_buffer));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, first_row_weight_buffer));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_from_mem));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_from_b0));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_from_b1));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_to_mem));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_to_b0));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_to_b1));
    OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel_conv2d[k], 0, 1, 1, NULL, &kernel_events[k]));
    set_callback(kernel_events[k], "ooo_queue");
    #else
    k_conv2D((read_block_t *)data_in, 
        #ifdef USE_ADD
        (write_block_t *)data_in_add, 
        #endif
		    H, W, HO_final, WO_final, rows_p, PT_p, PB_p, PL_p, PR_p, SH, SW, I_input, O_output, i_iter, 
		    o_iter_first, o_iter_last, 
		    enable_relu, enable_stm, relu_factor, 
        #ifdef USE_BATCH_NORM
        enable_batch_norm,
        #endif
        #ifdef USE_BATCH_NORM_RELU
	enable_batch_norm_relu,
	batch_norm_relu_factor,
        #endif
        #ifdef DIRECT_CONV
		    kernel, 
        #endif
        #ifdef DWS_CONV
        dw_kernel, (w_pw_t *)pw_kernel,
        #endif
        (read_bias_st *)bias,
        #ifdef USE_BATCH_NORM
        (bnp_st *)batch_norm_values, 
        #endif
        (write_block_t *)out, 
		    read_offset_p, write_offset_p,
		    enable_maxpooling, enable_avgpooling,
		    enable_clipping, enable_shift, 
        #ifdef USE_ADD
        enable_add, 
        #endif
        #ifdef USE_ADD_RELU
	apply_add_relu,
        #endif
        min_clip, max_clip, dir_shift, pos_shift, upsize_factor, write_to_weight_buffer, read_from_weight_buffer, first_row_weight_buffer, read_from_mem, read_from_b0, read_from_b1, write_to_mem, write_to_b0, write_to_b1);
    #endif
  }

  #ifdef OPENCL_TEST
  for (int k=0; k<num_kernels; k++) {
	cl_int err;
    OCL_CHECK(err, err = kernel_events[k].wait());
  }
  #endif

}

void compute() {

  char str[200];
  // Let's check if the input geometry must beWW decomposed in multiple frames
  if (HO_conv > HMAX) {
    printf("frame!!!\n"); exit(1);
    //
    int num_frames = ceil( (float) HO_conv / (float) HMAX);
    sprintf(str, "Launching multiframes mode (%d frames)...", num_frames);
    print_message(str);
    // 
    for (int fr=0; fr < num_frames; fr++) {

      printf("frame %d out of %d\n", fr, num_frames);

      // first output row for this frame
      int row_o = fr * HMAX;

      // rows to be produced in this frame
      int output_rows_frame = HMAX;
      if ((fr == num_frames-1) && ((HMAX * num_frames) != HO_conv)) output_rows_frame = HO_conv % HMAX;

      // padding
      int PT_frame = (fr==0) ? PT : 0;
      int PB_frame = (fr == num_frames - 1) ? PB : 0;
      int PL_frame = PL;
      int PR_frame = PR;

      // first input row to read for this frame
      // row_i = (row_o * SH) - PT
      // We correct if negative (because of padding)
      //
      int row_i = (row_o * SH) - PT;
      if (row_i < 0) row_i = 0;

      // rows to read for this frame
      int rows_to_read = KH + ((output_rows_frame - 1) * SH) - PT_frame - PB_frame;

      // read and write offsets
      int read_offset_frame = row_i * W;
      printf("fr %d HMAX %d WO %d\n", fr, HMAX, WO_conv);
      int write_offset_frame = (fr * HMAX * WO_conv);

      sprintf(str, "Frame %d: HxW = %3dx%3d, Pad = %1d-%1d-%1d-%1d, off_rd %d, off_wr %d, rows_to_read %d", fr, H, W, PT_frame, PB_frame, PL_frame, PR_frame, read_offset_frame, write_offset_frame, rows_to_read);
      print_message(str);

      // run kernel
      run_kernel(rows_to_read, PT_frame, PB_frame, PL_frame, PR_frame, read_offset_frame, write_offset_frame);
    }
  } else {
    run_kernel(rows, PT, PB, PL, PR, 0, 0);
  }
}



