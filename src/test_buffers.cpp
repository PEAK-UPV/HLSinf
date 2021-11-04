/*
 * Buffer-related functions for test
 */

#include "test_conv2D.h"

// Allocate_buffers. Allocates in CPU memory all the needed buffers
void allocate_buffers() {

  // First we allocate buffers in CPU

  // input data buffer
  size_t size_data_in_bytes = I_input * W * H * sizeof(data_type);
  posix_memalign((void **)&data_in, 4096, size_data_in_bytes);

  // input data add buffer
  if (enable_add) {
	  posix_memalign((void **)&data_in_add, 4096, HO_final * WO_final * O_output * sizeof(data_type));
	  posix_memalign((void **)&out_add_cpu, 4096, HO_final * WO_final * O_output * sizeof(data_type));
  }

  // weights buffer (kernel), depending on the type of convolution
  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  size_t size_kernel_in_bytes = I_kernel * O_kernel * KW * KH * sizeof(data_type);
  posix_memalign((void **)&kernel, 4096, size_kernel_in_bytes);
  #endif
  #ifdef DWS_CONV
  size_t size_kernel_dw_in_bytes = (I_kernel * KW * KH) * sizeof(data_type);
  size_t size_kernel_pw_in_bytes = (I_kernel * O_kernel) * sizeof(data_type);
  posix_memalign((void **)&dw_kernel, 4096, size_kernel_dw_in_bytes);
  posix_memalign((void **)&pw_kernel, 4096, size_kernel_pw_in_bytes);
  #endif

  // bias buffer
  size_t size_bias_in_bytes = O_output * sizeof(data_type);
  posix_memalign((void **)&bias, 4096, size_bias_in_bytes);

  // batch norm values buffer
  size_t size_bnvalues_in_bytes = (O_output * 4) * sizeof(data_type);
  posix_memalign((void **)&batch_norm_values, 4096, size_bnvalues_in_bytes);
  if (enable_batch_norm) {
    posix_memalign((void **)&out_batch_norm_cpu, 4096, HO_final * WO_final * O_output * sizeof(data_type));
  }

  // output buffer for fpga
  size_t size_output_in_bytes;
  size_output_in_bytes = O_output * WO_final * HO_final * sizeof(data_type);
  posix_memalign((void **)&out, 4096, size_output_in_bytes);

  // output buffer for cpu
  posix_memalign((void **)&out_conv_cpu, 4096, O_output * WO * HO * sizeof(data_type));

  // output for relu function
  if (enable_relu) {
    posix_memalign((void **)&out_relu_cpu, 4096, O_output * WO * HO * sizeof(data_type));
  }

  // output for STM functions
    if (enable_stm) {
      posix_memalign((void **)&out_stm_cpu, 4096, O_output * WO * HO * sizeof(data_type));
   }

  // output for pool function
  if ((enable_maxpooling) || (enable_avgpooling)) {
	  posix_memalign((void **)&out_pool_cpu, 4096, O_output * (WO/2) * (HO/2) * sizeof(data_type));
  }

  // final output for cpu
  posix_memalign((void **)&cpu_out, 4096, O_output * WO_final * HO_final * sizeof(data_type));

#ifdef OPENCL_TEST
  // Now we allocate those buffers in the FPGA (for OpenCL)
  cl_int err;
  data_in_ddr.flags  =  0 | XCL_MEM_TOPOLOGY;
  data_in_ddr.obj = data_in;
  data_in_ddr.param = 0;

  data_in_add_ddr.flags  =  0 | XCL_MEM_TOPOLOGY;
  data_in_add_ddr.obj = data_in_add;
  data_in_add_ddr.param = 0;

  batch_norm_val_ddr[0].flags  = 0 | XCL_MEM_TOPOLOGY;
  batch_norm_val_ddr[0].obj = batch_norm_values;
  batch_norm_val_ddr[0].param = 0;

  out_ddr[0].flags  = 0 | XCL_MEM_TOPOLOGY;
  out_ddr[0].obj = out;
  out_ddr[0].param = 0;
  //out_ddr[0].banks = XCL_MEM_DDR_BANK1;
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  kernel_ddr[0].flags  = 0 | XCL_MEM_TOPOLOGY;
  kernel_ddr[0].obj = kernel;
  kernel_ddr[0].param = 0;
#endif
#ifdef DWS_CONV
  kernel_dw_ddr[0].flags  = 0 | XCL_MEM_TOPOLOGY;
  kernel_dw_ddr[0].obj = dw_kernel;
  kernel_dw_ddr[0].param = 0;
  kernel_pw_ddr[0].flags  = 0 | XCL_MEM_TOPOLOGY;
  kernel_pw_ddr[0].obj = pw_kernel;
  kernel_pw_ddr[0].param = 0;
#endif

  bias_ddr[0].flags  = 0 | XCL_MEM_TOPOLOGY;
  bias_ddr[0].obj = bias;
  bias_ddr[0].param = 0;
  OCL_CHECK(err, buffer_i    = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_data_in_bytes, &data_in_ddr, &err));

#if defined(USE_BATCH_NORM)
  OCL_CHECK(err, buffer_batch_norm_val[0]    = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR , size_output_in_bytes, &batch_norm_val_ddr[0], &err));
#endif
  if (enable_add){
	  OCL_CHECK(err, buffer_i_add = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_output_in_bytes, &data_in_add_ddr, &err));
  }
  else { //create a dummy buffer
	  buffer_i_add = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , sizeof(data_type), &data_in_ddr, &err);
  }
  OCL_CHECK(err, buffer_o[0]    = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR , size_output_in_bytes, &out_ddr[0], &err));
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  OCL_CHECK(err, buffer_k[0]    = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_kernel_in_bytes, &kernel_ddr[0], &err));
#endif
#ifdef DWS_CONV
  OCL_CHECK(err, buffer_k_dw[0]    = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_kernel_dw_in_bytes, &kernel_dw_ddr[0], &err));
  OCL_CHECK(err, buffer_k_pw[0]    = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_kernel_pw_in_bytes, &kernel_pw_ddr[0], &err));
#endif
  OCL_CHECK(err, buffer_bias[0] = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_bias_in_bytes, &bias_ddr[0], &err));
#endif
}

// deallocate_buffers. Deallocates all CPU buffers
void deallocate_buffers() {

  free(data_in);
  if (enable_add) free(data_in_add);
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  free(kernel);
#endif
#ifdef DWS_CONV
  free(dw_kernel);
  free(pw_kernel);
#endif
  free(bias);
  free(out);
  free(out_conv_cpu);
  if (enable_relu) free(out_relu_cpu);
  if (enable_stm) free(out_stm_cpu);
  if ((enable_maxpooling) || (enable_avgpooling))	free(out_pool_cpu);
  free(batch_norm_values);
  if (enable_batch_norm) { free(out_batch_norm_cpu); }
  if (enable_add) free(out_add_cpu);
  free(cpu_out);
}

#ifdef OPENCL_TEST
// copy buffers to FPGA
void copy_to_fpga() {
  cl_int err;
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_i}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());


  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_i_add}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());

#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_k[0]}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
#endif
#ifdef DWS_CONV
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_k_dw[0]}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_k_pw[0]}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
#endif

  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_bias[0]}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());

#if defined(USE_BATCH_NORM)
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_batch_norm_val[0]}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
#endif
}
#endif

#ifdef OPENCL_TEST
void copy_from_fpga() {
  cl_int err;
  // std::cout << "Getting Results (Device to Host)..." << std::endl;
  std::vector<cl::Event> eventList;
  eventList.push_back(kernel_events[0]);
  // This operation only needs to wait for the kernel call.
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*buffer_o[0]}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
  // Wait for all of the OpenCL operations to complete
  OCL_CHECK(err, err = q.flush());
  OCL_CHECK(err, err = q.finish());
}
#endif

