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
  printf("allocate host side vectors and cl_memory buffers\n");
  #endif

  // NOTE that scoped_aligned_ptr vector reset function sets the number of of elements in array of type "template"
  // input data buffer
  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  data_in vector reset\n");
  #endif
  #endif
  size_t size_data_num_values = I_input * W * H;
  size_t size_data_in_bytes   = size_data_num_values * sizeof(data_type);
  //data_in = (data_type *)alignedMalloc(size_data_in_bytes);
  //if (data_in == NULL) checkError(999, "Error allocating memory for bias, null pointer");
  data_in.reset(size_data_num_values);


  // weights buffer (kernel), depending on the type of convolution
  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  kernel vector reset\n");
  #endif
  #endif
  size_t size_kernel_num_values = I_kernel * O_kernel * KW * KH;
  size_t size_kernel_in_bytes   = size_kernel_num_values * sizeof(data_type);
  //kernel = (data_type *)alignedMalloc(size_kernel_in_bytes);
  //if (kernel == NULL) checkError(999, "Error allocating memory for bias, null pointer");
  kernel.reset(size_kernel_num_values);
  #endif
  //printf("3\n");
  #ifdef DWS_CONV
  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  dw pw kernels vectors reset\n");
  #endif
  #endif
  size_t size_kernel_dw_num_values = (I_kernel * KW * KH);
  size_t size_kernel_dw_in_bytes   = size_kernel_dw_num_values * sizeof(data_type);
  size_t size_kernel_pw_num_values = (I_kernel * O_kernel);
  size_t size_kernel_pw_in_bytes   = (I_kernel * O_kernel) * sizeof(data_type);
  dw_kernel.reset(size_kernel_dw_num_values);
  pw_kernel.reset(size_kernel_pw_num_values);
  #endif

  //printf("4\n");
  // bias buffer
  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  bias vector reset\n");
  #endif
  #endif
  size_t size_bias_num_values = O_output;
  size_t size_bias_in_bytes   = size_bias_num_values * sizeof(data_type);
  bias.reset(size_bias_num_values);
  //bias = (data_type *)alignedMalloc(size_bias_in_bytes);
  //if (bias == NULL) checkError(999, "Error allocating memory for bias, null pointer");
  for(size_t i = 0; i < size_bias_num_values; i++) bias[i] = (data_type)0;
  
  //printf("5\n");
  // output buffer for fpga
  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  output vector reset\n");
  #endif
  #endif
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

  // batch_normalization buffer
  // batch_norm buffer size is 
  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  batch_norm vector  reset\n");
  #endif
  #endif

  size_t size_bn_num_values = (O_output * 4);
  size_t size_bn_in_bytes = size_bn_num_values * sizeof(bn_t);
  batch_norm_values.reset(size_bn_num_values);
  for(size_t i = 0; i < size_bn_num_values; i++) batch_norm_values[i] = (bn_t)0;

  // i_add buffer
  // i_add buffer size equals out(put) buffer size
  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  add_data vector reset\n");
  #endif
  #endif
  size_t size_i_add_num_values;
  size_t size_i_add_in_bytes;
//  if ((enable_maxpooling) || (enable_avgpooling)) {
//    size_i_add_num_values = O_output * (W/2) * (H/2);
//    size_i_add_in_bytes   = size_i_add_num_values * sizeof(data_type);
//  } else {
//    size_i_add_num_values = O_output * W * H;
//    size_i_add_in_bytes   = size_i_add_num_values * sizeof(data_type);
//  }

  size_i_add_num_values = O_output * W * H;
  size_i_add_in_bytes   = size_i_add_num_values * sizeof(data_type);

  data_in_add.reset(size_i_add_num_values);
  for(size_t i = 0; i < size_i_add_num_values; i++) data_in_add[i] = (data_type)0;




  #ifdef PRINT_LOG_BUFFERS  
  printf("  host side vectors for output data validation\n");
  #endif

  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  out_conv_cpu vector reset\n");
  #endif
  #endif
  out_conv_cpu.reset(O_output * W * H);
  for(size_t i = 0; i < (O_output * W * H); i++) out_conv_cpu[i] = (data_type)0;
  //out_conv_cpu.reset(size_output_num_values);
  //for(size_t i = 0; i < size_output_num_values; i++) out_conv_cpu[i] = (data_type)0;

  // output for relu function
  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  out_relu_cpu vector reset\n");
  #endif
  #endif
  int size_relu_num_values = O_output * W * H;
  if (enable_relu) {
    out_relu_cpu.reset(size_relu_num_values );
    for(size_t i = 0; i < (size_relu_num_values); i++) out_relu_cpu[i] = (data_type)0;
  }

  // output for pool function
  //if ((enable_maxpooling) || (enable_avgpooling)) {
  //  out_pool_cpu.reset(O_output * (W/2) * (H/2));
  //}
  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  out_pool_cpu vector reset\n");
  #endif
  #endif
  out_pool_cpu.reset(O_output * W * H);



  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  out_batch_norm_cpu vector reset\n");
  #endif
  #endif
  //if (enable_batch_norm) {
    out_batch_norm_cpu.reset(O_output * W * H);
    for(size_t i = 0; i < O_output * W * H; i++) out_batch_norm_cpu[i] = (data_type)0;
  //}

  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  out_add_cpu vector reset\n");
  #endif
  #endif
  //if ((enable_add)) {
    out_add_cpu.reset(O_output * W * H);
    for(size_t i = 0; i < O_output * W * H; i++) out_add_cpu[i] = (data_type)0;
  //}

  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  out_cpu vector reset\n");
  #endif
  #endif
  out_cpu.reset(O_output * W * H);
  for(size_t i = 0; i < O_output * W * H; i++) out_cpu[i] = (data_type)0;

  #ifdef PRINT_LOG_BUFFERS  
  #ifdef DEBUG_VERBOSE
  printf("  out_cpu_from_file vector reset\n");
  #endif
  #endif
  out_cpu_from_file.reset(O_output * W * H);
  for(size_t i = 0; i < O_output * W * H; i++) out_cpu_from_file[i] = (data_type)0;



  // CREATE memroy buffers and link them with the host memory pointer
#ifdef OPENCL_TEST
  cl_int err;
  
  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for data_in: %lu values - %lu bytes \n",size_data_num_values, size_data_in_bytes);
  #endif
  buffer_i = clCreateBuffer(context, CL_MEM_READ_WRITE, size_data_in_bytes, NULL, &err); // not using host pointer
  checkError(err, "buffer_i");
  //OCL_CHECK(err, buffer_i = clCreateBuffer(context, CL_MEM_READ_ONLY, size_data_in_bytes, NULL, &err)); // not using host pointer

  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for data_out: %lu values - %lu bytes \n", size_output_num_values, size_output_in_bytes);
  #endif
  //buffer_o = clCreateBuffer(context, CL_MEM_WRITE_ONLY| CL_CHANNEL_2_INTELFPGA, size_output_in_bytes, NULL, &err);
  OCL_CHECK(err, buffer_o = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_output_in_bytes, NULL, &err));
  
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for kernel: %lu values - %lu bytes \n",size_kernel_num_values, size_kernel_in_bytes);
  #endif
  OCL_CHECK(err, buffer_k = clCreateBuffer(context, CL_MEM_READ_ONLY, size_kernel_in_bytes, NULL, &err));
#endif
#ifdef DWS_CONV
  OCL_CHECK(err, buffer_k_dw = clCreateBuffer(context, CL_MEM_READ_ONLY, size_kernel_dw_in_bytes, NULL, &err));
  OCL_CHECK(err, buffer_k_pw[0] = clCreateBuffer(context, CL_MEM_READ_ONLY, size_kernel_pw_in_bytes, NULL, &err));
#endif
  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for bias: %lu values - %lu bytes \n", size_bias_num_values, size_bias_in_bytes);
  #endif
  OCL_CHECK(err, buffer_bias=clCreateBuffer(context, CL_MEM_READ_ONLY, size_bias_in_bytes, NULL, &err));

  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for batch normalization: %lu values - %lu bytes \n", size_bn_num_values, size_bn_in_bytes);
  #endif
  OCL_CHECK(err, buffer_batch_norm_val = clCreateBuffer(context, CL_MEM_READ_ONLY, size_bn_in_bytes, NULL, &err));

  #ifdef PRINT_LOG_BUFFERS 
  printf(" create buffer for add_data: %lu values - %lu bytes \n", size_i_add_num_values, size_i_add_in_bytes);
  #endif
  OCL_CHECK(err, buffer_i_add = clCreateBuffer(context, CL_MEM_READ_ONLY, size_i_add_in_bytes, NULL, &err));


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

 #ifdef PRINT_LOG_BUFFERS
  printf("    buffer_batch_norm_val\n");fflush(stdout);
  #endif
  if(buffer_batch_norm_val != NULL) {
    clReleaseMemObject(buffer_batch_norm_val);
    buffer_batch_norm_val = NULL;
  }
 #ifdef PRINT_LOG_BUFFERS
  printf("    buffer_i_add\n");fflush(stdout);
  #endif
  if(buffer_i_add != NULL) {
    clReleaseMemObject(buffer_i_add);
    buffer_i_add = NULL;
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
  cl_event  write_events[6];            // Write events
  cl_int    err;
  int       num_write_events_before_kernel_execution = 0;
  
  #ifdef PRINT_LOG_BUFFERS
  printf("Write data from buffers into fpga memory\n");
  #endif
  #ifdef PRINT_LOG_BUFFERS
  printf("enqueue buffer-din migrate operation\n");
  #endif
  size_t size_data_in_bytes = I_input * W * H * sizeof(data_type);
  OCL_CHECK(err, err = clEnqueueWriteBuffer(queues[K_DATA_IN_READER], buffer_i, CL_FALSE, 0, size_data_in_bytes, data_in, 0, NULL, &write_events[0]));
  num_write_events_before_kernel_execution++;
  
  //err = clEnqueueWriteBuffer(queues[K_DATA_IN_READER], buffer_i, CL_TRUE, 0, size_data_in_bytes, data_in, 0, NULL, NULL);
  //CHECK(err);
  //#ifdef PRINT_LOG_BUFFERS
  //printf("....data moved\n");
  //#endif

  #ifdef PRINT_LOG_BUFFERS
  printf("enqueue buffer-bias migrate operation\n");
  #endif
  size_t size_bias_in_bytes = O_output * sizeof(data_type);
  OCL_CHECK(err, err = clEnqueueWriteBuffer(queues[K_BIAS_IN_READER], buffer_bias, CL_FALSE, 0, size_bias_in_bytes, bias, 0, NULL, &write_events[1]));
  num_write_events_before_kernel_execution++;
  //err = clEnqueueWriteBuffer(queues[K_BIAS_IN_READER], buffer_bias, CL_TRUE, 0, size_bias_in_bytes, bias, 0, NULL, NULL);
  //CHECK(err);
  //#ifdef PRINT_LOG_BUFFERS
  //printf("....data moved\n");
  //#endif

  //batch normalization 
  #ifdef PRINT_LOG_BUFFERS 
  printf("enqueue buffer for batch normalization migrate operation\n");
  #endif
  size_t size_bn_num_values = (O_output * 4);
  size_t size_bn_in_bytes = size_bn_num_values * sizeof(bn_t);
  OCL_CHECK(err, err = clEnqueueWriteBuffer(queues[K_BATCH_NORM_READER], buffer_batch_norm_val, CL_FALSE, 0, size_bn_in_bytes, batch_norm_values, 0, NULL, &write_events[2]));
  num_write_events_before_kernel_execution++;

  // in_add
  #ifdef PRINT_LOG_BUFFERS 
  printf("enqueue buffer for add-data migrate operation\n");
  #endif
  size_t size_i_add_num_values;
  size_t size_i_add_in_bytes;
  if ((enable_maxpooling) || (enable_avgpooling)) {
    size_i_add_num_values = O_output * (W/2) * (H/2);
    size_i_add_in_bytes   = size_i_add_num_values * sizeof(data_type);
  } else {
    size_i_add_num_values = O_output * W * H;
    size_i_add_in_bytes   = size_i_add_num_values * sizeof(data_type);
  }
  OCL_CHECK(err, err = clEnqueueWriteBuffer(queues[K_ADD_DATA_READER], buffer_i_add, CL_FALSE, 0, size_i_add_in_bytes, data_in_add, 0, NULL, &write_events[3]));
  num_write_events_before_kernel_execution++;



  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  #ifdef PRINT_LOG_BUFFERS
  printf("enqueue buffer-kernel migrate operation\n");
  #endif
  size_t size_kernel_in_bytes = I_kernel * O_kernel * KW * KH * sizeof(data_type);
  OCL_CHECK(err, err = clEnqueueWriteBuffer(queues[K_KERNEL_IN_READER], buffer_k, CL_FALSE, 0, size_kernel_in_bytes, kernel, 0, NULL, &write_events[4]));
  num_write_events_before_kernel_execution++;

  //err = clEnqueueWriteBuffer(queues[K_KERNEL_IN_READER], buffer_k, CL_TRUE, 0, size_kernel_in_bytes, kernel, 0, NULL, NULL);
  //CHECK(err);
  //#ifdef PRINT_LOG_BUFFERS
  //printf("....data moved\n");
  //#endif
  #endif
  #ifdef DWS_CONV
  size_t size_kernel_dw_in_bytes = (I_kernel * KW * KH) * sizeof(data_type);
  size_t size_kernel_pw_in_bytes = (I_kernel * O_kernel) * sizeof(data_type);
  OCL_CHECK(err, err = clEnqueueWriteBuffer(q, buffer_k_dw, CL_FALSE, 0, size_kernel_dw_in_bytes, dw_kernel, 0, NULL, &write_events[4]));
  num_write_events_before_kernel_execution++;
  OCL_CHECK(err, err = clEnqueueWriteBuffer(q, buffer_k_pw, CL_FALSE, 0, size_kernel_pw_in_bytes, pw_kernel, 0, NULL, &write_events[5]));
  num_write_events_before_kernel_execution++;
  #endif

  #ifdef PRINT_LOG_BUFFERS
  printf("copy to fpga: triggered write operation for %d buffers\n", num_write_events_before_kernel_execution);
  #endif
  OCL_CHECK(err, err = clWaitForEvents(num_write_events_before_kernel_execution, write_events));

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
