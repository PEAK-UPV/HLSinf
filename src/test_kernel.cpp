/*
 * kernel-related functions for test
 */

#include "test_conv2D.h"

void run_kernel() {
#ifdef OPENCL_TEST
  // set kernel arguments
  cl_int err;
  int arg = 0;
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, *buffer_i));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, H));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, W));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, rows));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, I));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, O));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, i_iter));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, o_iter));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, enable_relu));
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, *buffer_k[0]));
#endif
#ifdef DWS_CONV
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, *buffer_k_dw[0]));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, *buffer_k_pw[0]));
#endif
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, *buffer_bias[0]));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, *buffer_o[0]));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, global_offset));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, enable_upper_padding));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, enable_lower_padding));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, enable_maxpooling));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, enable_avgpooling));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, enable_clipping));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, enable_shift));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, min_clip));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, max_clip));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, dir_shift));
  OCL_CHECK(err, err = kernel_conv2d[0].setArg(arg++, pos_shift));

  OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel_conv2d[0], 0, 1, 1, NULL, &kernel_events[0]));
  set_callback(kernel_events[0], "ooo_queue");
  OCL_CHECK(err, err = kernel_events[0].wait());
#else
  k_conv2D((ap_uint<512> *)data_in, H, W, rows, I, O, i_iter, o_iter, enable_relu,
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
		  kernel,
#endif
#ifdef DWS_CONV
		  dw_kernel, pw_kernel,
#endif
		  (pixel_out_t *)bias, (ap_uint<512> *)out, global_offset,
		   enable_upper_padding, enable_lower_padding, enable_maxpooling, enable_avgpooling,
		   enable_clipping, enable_shift, min_clip, max_clip, dir_shift, pos_shift);
#endif
}

