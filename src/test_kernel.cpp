/*
 * kernel-related functions for test
 */

#include "test_conv2D.h"

void run_kernel(int rows_p, int PT_p, int PB_p, int PL_p, int PR_p, int read_offset_p, int write_offset_p) {
  int num_kernels;
  int o_iter_per_kernel;

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

    char str[50];
    sprintf(str, "launching kernel %d (output iterations %d to %d)", k, o_iter_first, o_iter_last);
    print_message(str);

    #ifdef OPENCL_TEST
	// set kernel arguments
    cl_int err;
    int arg = 0;
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_i));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_i_add));
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
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_batch_norm));
    #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_k[0]));
    #endif
    #ifdef DWS_CONV
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_k_dw[0]));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_k_pw[0]));
    #endif
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_bias[0]));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_batch_norm_val[0]));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_o[0]));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_offset_p));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_offset_p));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_maxpooling));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_avgpooling));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_clipping));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_shift));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_add));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, min_clip));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, max_clip));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, dir_shift));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, pos_shift));
    OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel_conv2d[k], 0, 1, 1, NULL, &kernel_events[k]));
    set_callback(kernel_events[k], "ooo_queue");
    #else
    k_conv2D((read_block_t *)data_in, (write_block_t *)data_in_add, 
		    H, W, HO_final, WO_final, rows_p, PT_p, PB_p, PL_p, PR_p, SH, SW, I_input, O_output, i_iter, 
		    o_iter_first, o_iter_last, 
		    enable_relu, enable_stm, relu_factor, enable_batch_norm,
          #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
		  kernel,
          #endif
          #ifdef DWS_CONV
		  (data_type *)dw_kernel, (read_kernel_pw_t *)pw_kernel,
          #endif
		  (pixel_out_t *)bias, (batch_norm_in_t *)batch_norm_values, (write_block_t *)out, 
		   read_offset_p, write_offset_p,
		   enable_maxpooling, enable_avgpooling,
		   enable_clipping, enable_shift, enable_add, min_clip, max_clip, dir_shift, pos_shift);
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
  if (HO > HMAX) {
    //
    int num_frames = ceil( (float) HO / (float) HMAX);
    sprintf(str, "Launching multiframes mode (%d frames)...", num_frames);
    print_message(str);
    // 
    for (int fr=0; fr < num_frames; fr++) {

      // first output row for this frame
      int row_o = fr * HMAX;

      // rows to be produced in this frame
      int output_rows_frame = HMAX;
      if ((fr == num_frames-1) && ((HMAX * num_frames) != HO)) output_rows_frame = HO % HMAX;

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
      int rows_to_read = (output_rows_frame * SH) - PT_frame - PB_frame + (KH - 1);

      // read and write offsets
      int read_offset_frame = row_i * W;
      int write_offset_frame = (fr * HMAX * WO);

      sprintf(str, "Frame %d: HxW = %3dx%3d, Pad = %1d-%1d-%1d-%1d, off_rd %d, off_wr %d, rows_to_read %d", fr, H, W, PT_frame, PB_frame, PL_frame, PR_frame, read_offset_frame, write_offset_frame, rows_to_read);
      print_message(str);

      // run kernel
      run_kernel(rows_to_read, PT_frame, PB_frame, PL_frame, PR_frame, read_offset_frame, write_offset_frame);
    }
  } else {
    run_kernel(rows, PT, PB, PL, PR, 0, 0);
  }
}



