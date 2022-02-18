/*
 * Buffer-related functions for test
 */

#include "test_conv2D.h"

//-----------------------------------------------------------------------------
// Allocate_buffers. Allocates in CPU memory all the needed buffers
//-----------------------------------------------------------------------------
void allocate_buffers() {

  // First we allocate buffers in CPU

  #ifdef PRINT_LOG_BUFFERS
  printf("allocate buffers\n");
  #endif

  // NOTE that scoped_aligned_ptr vector reset function sets the number of of elements in array of type "template"
  //printf("1\n");
  // input data buffer
  size_t size_data_num_values = I_input * W * H;
  size_t size_data_in_bytes   = size_data_num_values * sizeof(data_type);
  //data_in = (data_type *)alignedMalloc(size_data_in_bytes);
  //if (data_in == NULL) checkError(999, "Error allocating memory for bias, null pointer");
  data_in.reset(size_data_num_values);

  //printf("2\n");
  // weights buffer (kernel), depending on the type of convolution
  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  size_t size_kernel_num_values = I_kernel * O_kernel * KW * KH;
  size_t size_kernel_in_bytes   = size_kernel_num_values * sizeof(data_type);
  //kernel = (data_type *)alignedMalloc(size_kernel_in_bytes);
  //if (kernel == NULL) checkError(999, "Error allocating memory for bias, null pointer");
  kernel.reset(size_kernel_num_values);
  #endif
  //printf("3\n");
  #ifdef DWS_CONV
  size_t size_kernel_dw_num_values = (I_kernel * KW * KH);
  size_t size_kernel_dw_in_bytes   = size_kernel_dw_num_values * sizeof(data_type);
  size_t size_kernel_pw_num_values = (I_kernel * O_kernel);
  size_t size_kernel_pw_in_bytes   = (I_kernel * O_kernel) * sizeof(data_type);
  dw_kernel.reset(size_kernel_dw_num_values);
  pw_kernel.reset(size_kernel_pw_num_values);
  #endif

  //printf("4\n");
  // bias buffer
  size_t size_bias_num_values = O_output;
  size_t size_bias_in_bytes   = size_bias_num_values * sizeof(data_type);
  bias.reset(size_bias_num_values);
  //bias = (data_type *)alignedMalloc(size_bias_in_bytes);
  //if (bias == NULL) checkError(999, "Error allocating memory for bias, null pointer");
  for(size_t i = 0; i < size_bias_num_values; i++) bias[i] = (data_type)0;

  //printf("5\n");
  // output buffer for fpga
  size_t size_output_num_values;
  size_t size_output_in_bytes;
  if ((enable_maxpooling) || (enable_avgpooling)) {
    size_output_num_values = O_output * (W/2) * (H/2);
    size_output_in_bytes   = size_output_num_values * sizeof(data_type);
  } else {
    size_output_num_values = O_output * W * H;
    size_output_in_bytes   = size_output_num_values * sizeof(data_type);
  }
  //printf("6\n");

  out.reset(size_output_num_values);
  //out = (data_type *)alignedMalloc(size_output_in_bytes);
  //if (kernel == NULL) checkError(999, "Error allocating memory for output, null pointer");

  for(size_t i = 0; i < size_output_num_values; i++) out[i] = (data_type)0;

  //printf("7\n");
  out_conv_cpu.reset(O_output * W * H);
  for(size_t i = 0; i < (O_output * W * H); i++) out_conv_cpu[i] = (data_type)0;


  // printf("8\n");
  // output for relu function
  if (enable_relu) {
    out_relu_cpu.reset(O_output * W * H );
    for(size_t i = 0; i < (O_output * W * H); i++) out_relu_cpu[i] = (data_type)0;
  }

  //printf("9\n");
  // output for pool function
  if ((enable_maxpooling) || (enable_avgpooling)) {
    out_pool_cpu.reset(O_output * (W/2) * (H/2));

  }

  // CREATE memroy buffers and link them with the host memory pointer
#ifdef OPENCL_TEST
  cl_int err;
  
  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for data_in: %lu values - %lu bytes \n",size_data_num_values, size_data_in_bytes);
  #endif
  //buffer_i = clCreateBuffer(context, CL_MEM_READ_WRITE, size_data_in_bytes, NULL, &err); // not using host pointer
  OCL_CHECK(err, buffer_i = clCreateBuffer(context, CL_MEM_READ_ONLY, size_data_in_bytes, NULL, &err)); // not using host pointer

  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for data_out: %lu values - %lu bytes \n", size_output_num_values, size_output_in_bytes);
  #endif
  //buffer_o = clCreateBuffer(context, CL_MEM_WRITE_ONLY| CL_CHANNEL_2_INTELFPGA, size_output_in_bytes, NULL, &err);
  OCL_CHECK(err, buffer_o = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_output_in_bytes, NULL, &err));
  
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for kernel: %lu values - %lu bytes \n",size_kernel_num_values, size_kernel_in_bytes);
  #endif
  buffer_k = clCreateBuffer(context, CL_MEM_READ_ONLY, size_kernel_in_bytes, NULL, &err);
  CHECK(err);
#endif
#ifdef DWS_CONV
  buffer_k_dw = clCreateBuffer(context, CL_MEM_READ_ONLY, size_kernel_dw_in_bytes, NULL, &err);
  CHECK(err);
  buffer_k_pw[0] = clCreateBuffer(context, CL_MEM_READ_ONLY, size_kernel_pw_in_bytes, NULL, &err);
  CHECK(err);
#endif
  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for bias: %lu values - %lu bytes \n", size_bias_num_values, size_bias_in_bytes);
  #endif

  buffer_bias=clCreateBuffer(context, CL_MEM_READ_ONLY, size_bias_in_bytes, NULL, &err);
  CHECK(err);
#endif
}

//-----------------------------------------------------------------------------
// deallocate_buffers. Deallocates all CPU buffers
//-----------------------------------------------------------------------------
void deallocate_buffers() {
  #ifdef PRINT_LOG_BUFFERS
  printf("Release host memory resources\n");
  #endif

  #ifdef PRINT_LOG_BUFFERS
  printf("    buffer_i\n");fflush(stdout);
  #endif
  if(buffer_i != NULL) {
    clReleaseMemObject(buffer_i);
    buffer_i = NULL;
  }
  #ifdef PRINT_LOG_BUFFERS
  printf("    buffer_o\n");fflush(stdout);
  #endif
  if (buffer_o != NULL) {
    clReleaseMemObject(buffer_o);
    buffer_o = NULL;
  }
  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  #ifdef PRINT_LOG_BUFFERS
  printf("    buffer_k\n");fflush(stdout);
  #endif
  if (buffer_k != NULL) {
    clReleaseMemObject(buffer_k);
    buffer_k = NULL;
  }
  #endif
  #ifdef DWS_CONV
  if (buffer_k_dw != NULL) {
    clReleaseMemObject(buffer_k_dw);
    buffer_k_dw = NULL;
  }

  if (buffer_k_pw[0] != NULL) {
    clReleaseMemObject(buffer_k_pw);
    buffer_k_pw = NULL;
  }
  #endif

  #ifdef PRINT_LOG_BUFFERS
  printf("    buffer_bias\n");fflush(stdout);
  #endif
  if(buffer_bias != NULL) {
    clReleaseMemObject(buffer_bias);
    buffer_bias = NULL;
  }

#ifdef DEBUG_VERBOSE
printf("    function ends\n");fflush(stdout);
#endif

}

#ifdef OPENCL_TEST
//-----------------------------------------------------------------------------
// copy buffers to FPGA
//-----------------------------------------------------------------------------
void copy_to_fpga() {
  cl_event  write_events[4];            // Write events
  cl_int    err;
  int       num_write_events_before_kernel_execution = 0;
  
  #ifdef PRINT_LOG_BUFFERS
  printf("Write data from buffers into fpga memory\n");
  #endif
  #ifdef PRINT_LOG_BUFFERS
  printf("enqueue buffer-din migrate operation\n");
  #endif
  size_t size_data_in_bytes = I_input * W * H * sizeof(data_type);
  err = clEnqueueWriteBuffer(queues[K_DATA_IN_READER], buffer_i, CL_FALSE, 0, size_data_in_bytes, data_in, 0, NULL, &write_events[0]);
  num_write_events_before_kernel_execution++;
  CHECK(err);
  //err = clEnqueueWriteBuffer(queues[K_DATA_IN_READER], buffer_i, CL_TRUE, 0, size_data_in_bytes, data_in, 0, NULL, NULL);
  //CHECK(err);
  //#ifdef PRINT_LOG_BUFFERS
  //printf("....data moved\n");
  //#endif

  #ifdef PRINT_LOG_BUFFERS
  printf("enqueue buffer-bias migrate operation\n");
  #endif
  size_t size_bias_in_bytes = O_output * sizeof(data_type);
  err = clEnqueueWriteBuffer(queues[K_BIAS_IN_READER], buffer_bias, CL_FALSE, 0, size_bias_in_bytes, bias, 0, NULL, &write_events[1]);
  num_write_events_before_kernel_execution++;
  CHECK(err);
  //err = clEnqueueWriteBuffer(queues[K_BIAS_IN_READER], buffer_bias, CL_TRUE, 0, size_bias_in_bytes, bias, 0, NULL, NULL);
  //CHECK(err);
  //#ifdef PRINT_LOG_BUFFERS
  //printf("....data moved\n");
  //#endif


  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  #ifdef PRINT_LOG_BUFFERS
  printf("enqueue buffer-kernel migrate operation\n");
  #endif
  size_t size_kernel_in_bytes = I_kernel * O_kernel * KW * KH * sizeof(data_type);
  err = clEnqueueWriteBuffer(queues[K_KERNEL_IN_READER], buffer_k, CL_FALSE, 0, size_kernel_in_bytes, kernel, 0, NULL, &write_events[2]);
  num_write_events_before_kernel_execution++;
  CHECK(err);
  //err = clEnqueueWriteBuffer(queues[K_KERNEL_IN_READER], buffer_k, CL_TRUE, 0, size_kernel_in_bytes, kernel, 0, NULL, NULL);
  //CHECK(err);
  //#ifdef PRINT_LOG_BUFFERS
  //printf("....data moved\n");
  //#endif
  #endif
  #ifdef DWS_CONV
  size_t size_kernel_dw_in_bytes = (I_kernel * KW * KH) * sizeof(data_type);
  size_t size_kernel_pw_in_bytes = (I_kernel * O_kernel) * sizeof(data_type);
  err = clEnqueueWriteBuffer(q, buffer_k_dw, CL_FALSE, 0, size_kernel_dw_in_bytes, dw_kernel, 0, NULL, &write_events[2]);
  num_write_events_before_kernel_execution++;
  CHECK(err);
  err = clEnqueueWriteBuffer(q, buffer_k_pw, CL_FALSE, 0, size_kernel_pw_in_bytes, pw_kernel, 0, NULL, &write_events[3]);
  num_write_events_before_kernel_execution++;
  CHECK(err);
  #endif

  #ifdef PRINT_LOG_BUFFERS
  printf("copy to fpga: triggered write operation for %d buffers\n", num_write_events_before_kernel_execution);
  #endif
  err = clWaitForEvents(num_write_events_before_kernel_execution, write_events);
  CHECK(err);
  #ifdef PRINT_LOG_BUFFERS
  printf("write buffers to fpga completed\n" );
  #endif


  //finished, releasing vents
  //printf(KRED "JM10 TO-DO: release write events\n" KNRM);
  //for (int i = 0, i < 4; i++) {
  //  clReleaseEvent[i]
  //}

}
#endif

#ifdef OPENCL_TEST
void copy_from_fpga() {
  cl_event   read_events[24]; 
  cl_int     err;
  int        num_events_to_wait = 0;

  #ifdef PRINT_LOG_BUFFERS
  printf ("Enqueue read-data operation from device memory via DMA\n");
  #endif

  // This operation only needs to wait for the kernel call.
  #ifdef PRINT_LOG_BUFFERS
  printf("  enqueue read buffer_o migrate operation\n");
  #endif
  size_t size_output_in_bytes;
  if ((enable_maxpooling) || (enable_avgpooling)) {
    size_output_in_bytes = O_output * (W/2) * (H/2) * sizeof(data_type);
	//posix_memalign((void **)&out, 4096, size_output_in_bytes);
  } else {
    size_output_in_bytes = O_output * W * H * sizeof(data_type);
  }

  #ifdef PRINT_LOG_BUFFERS
  printf("   buffer_o migrate operation for %lu values - %lu bytes\n",size_output_in_bytes/sizeof(data_type) ,size_output_in_bytes);
  #endif

  num_events_to_wait = 0;
  err = clEnqueueReadBuffer(queues[K_WRITER], buffer_o, CL_FALSE, 0, size_output_in_bytes, out, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);
  //err = clEnqueueReadBuffer(queues[K_WRITER], buffer_o, CL_TRUE, 0, size_output_in_bytes, out, 0, NULL, NULL);
  //CHECK(err);

  // Wait for all of the OpenCL operations to complete
  #ifdef PRINT_LOG_BUFFERS
  printf("copy from fpga: triggered read operation for %d buffers\n", num_events_to_wait);
  #endif
  err = clWaitForEvents(num_events_to_wait, read_events);
  CHECK(err);
  #ifdef PRINT_LOG_BUFFERS
  printf("read buffers from fpga completed\n" );
  #endif

}
#endif

//-----------------------------------------------------------------------------
// end of file: test_buffers.cpp
//-----------------------------------------------------------------------------
