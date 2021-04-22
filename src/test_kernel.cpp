/*
 * kernel-related functions for test
 */

#include "test_conv2D.h"

void run_kernel() {
  
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
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, H));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, W));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, rows));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, I_input));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, O_output));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, i_iter));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, o_iter_first));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, o_iter_last));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_relu));
    OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, enable_stm));
    #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_k[0]));
    #endif
    #ifdef DWS_CONV
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_k_dw[0]));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_k_pw[0]));
    #endif
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_bias[0]));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_o[0]));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, global_offset));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_upper_padding));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_lower_padding));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_maxpooling));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_avgpooling));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_clipping));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, enable_shift));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, min_clip));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, max_clip));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, dir_shift));
    OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, pos_shift));
    //
    OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel_conv2d[k], 0, 1, 1, NULL, &kernel_events[k]));
    set_callback(kernel_events[k], "ooo_queue");
    #else

    k_conv2D((ap_uint<512> *)data_in, H, W, rows, I, O, i_iter, o_iter, enable_relu, enable_stm,
    k_conv2D((read_block_t *)data_in, H, W, rows, I, O, i_iter, o_iter_first, o_iter_last, enable_relu, enable_stm,
          #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
>>>>>>> origin/develop
		  kernel,
          #endif
          #ifdef DWS_CONV
		  (data_type *)dw_kernel, (read_kernel_pw_t *)pw_kernel,
          #endif
		  (pixel_out_t *)bias, (write_block_t *)out, global_offset,
		   enable_upper_padding, enable_lower_padding, enable_maxpooling, enable_avgpooling,
		   enable_clipping, enable_shift, min_clip, max_clip, dir_shift, pos_shift);
    #endif
  }

  #ifdef OPENCL_TEST
  for (int k=0; k<num_kernels; k++) {
	cl_int err;
    OCL_CHECK(err, err = kernel_events[k].wait());
  }
  #endif

}

