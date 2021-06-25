/*
 * Buffer-related functions for test
 */

#include "test_conv2D.h"

// Allocate_buffers. Allocates in CPU memory all the needed buffers
void allocate_buffers() {

  // First we allocate buffers in CPU


  // input data buffer
  size_t size_data_in_bytes = I_input * W * H * sizeof(data_type);
  if (posix_memalign((void **)&data_in, 4096, size_data_in_bytes) != 0) {
    printf("FATAL posix_memalign failed to allocate space for pointer: data_in\n");
    CHECK(1);
  }
  #ifdef HLS_DEBUG 
  printf("test buffers data_in_buffer size is %lu bytes\n", size_data_in_bytes);
  #endif

  // weights buffer (kernel), depending on the type of convolution
  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  size_t size_kernel_in_bytes = I_kernel * O_kernel * KW * KH * sizeof(data_type);
  if (posix_memalign((void **)&kernel, 4096, size_kernel_in_bytes) != 0 ) {
    printf("FATAL posix_memalign failed to allocate space for pointer: kernel\n");
    CHECK(1);
  }
  #endif
  #ifdef DWS_CONV
  size_t size_kernel_dw_in_bytes = (I_kernel * KW * KH) * sizeof(data_type);
  size_t size_kernel_pw_in_bytes = (I_kernel * O_kernel) * sizeof(data_type);
  if( posix_memalign((void **)&dw_kernel, 4096, size_kernel_dw_in_bytes)!= 0 ) {
    printf("FATAL posix_memalign failed to allocate space for pointer: dw_kernel\n");
    CHECK(1);
  }
  if( posix_memalign((void **)&pw_kernel, 4096, size_kernel_pw_in_bytes) != 0 ) {
    printf("FATAL posix_memalign failed to allocate space for pointer: pw_kernel\n");
    CHECK(1);
  }
  #endif

  // bias buffer
  size_t size_bias_in_bytes = O_output * sizeof(data_type);
  if( posix_memalign((void **)&bias, 4096, size_bias_in_bytes)!= 0 ) {
    printf("FATAL posix_memalign failed to allocate space for pointer: bias\n");
    CHECK(1);
  }

  // output buffer for fpga
  size_t size_output_in_bytes;
  if ((enable_maxpooling) || (enable_avgpooling)) {
	size_output_in_bytes = O_output * (W/2) * (H/2) * sizeof(data_type);
	//posix_memalign((void **)&out, 4096, size_output_in_bytes);
  } else {
	size_output_in_bytes = O_output * W * H * sizeof(data_type);
  }
  if ( posix_memalign((void **)&out, 4096, size_output_in_bytes)!= 0 ) {
    printf("FATAL posix_memalign failed to allocate space for pointer: out\n");
    CHECK(1);
  }

  // output buffer for cpu
  if (posix_memalign((void **)&out_conv_cpu, 4096, O_output * W * H * sizeof(data_type))!= 0 ) {
    printf("FATAL posix_memalign failed to allocate space for pointer: out_conv_cpu\n");
    CHECK(1);
  }

  #ifdef HLS_DEBUG
  //
  if (posix_memalign((void **)&dbg_cpu_data_directconv_out, 4096, O_output * W * H * sizeof(data_type))!= 0 ) {
    printf("FATAL posix_memalign failed to allocate space for pointer: dbg_cpu_data_directconv_out\n");
    CHECK(1);
  }
  #endif




  // output for relu function
  if (enable_relu) {
    if (posix_memalign((void **)&out_relu_cpu, 4096, O_output * W * H * sizeof(data_type))!= 0 ) {
    printf("FATAL posix_memalign failed to allocate space for pointer: out_relu_cpu\n");
    CHECK(1);
  }
  }

  // output for pool function
  if ((enable_maxpooling) || (enable_avgpooling)) {
	  if (posix_memalign((void **)&out_pool_cpu, 4096, O_output * (W/2) * (H/2) * sizeof(data_type))!= 0 ) {
    printf("FATAL posix_memalign failed to allocate space for pointer: out_pool_cpu\n");
    CHECK(1);
    }
  }

#ifdef HLS_DEBUG
  // data_in 
    size_t size_dbg_loop_data_in = I_input * W * H * MAX_KERNELS;
    size_t size_dbg_loop_data_in_bytes = size_dbg_loop_data_in * sizeof(data_type);
    if (posix_memalign((void **)&dbg_loop_data_in, 4096, size_dbg_loop_data_in_bytes) != 0) {
      printf("FATAL posix_memalign failed to allocate space for pointer: dbg_loop_data_in[MAX_KERNELS]\n");
      CHECK(1);
    }
    for(int ti=0; ti <  size_dbg_loop_data_in; ti++) {
      dbg_loop_data_in[ti] = 0;
    }
  

  // values after input_buffer stage of kernel
  // this data has the same format as data_in, it is data after input buffers, to deal with congestion of input
    size_t size_dbg_loop_data_input_buffer = I_input * W * H * MAX_KERNELS;
    size_t size_dbg_loop_data_input_buffer_bytes = size_dbg_loop_data_input_buffer * sizeof(data_type);
    if (posix_memalign((void **)&dbg_loop_data_input_buffer, 4096, size_dbg_loop_data_input_buffer_bytes) != 0) {
      printf("FATAL posix_memalign failed to allocate space for pointer: dbg_loop_data_input_buffer[MAX_KERNELS]\n");
      CHECK(1);
    }
    for(int ti=0; ti <  size_dbg_loop_data_input_buffer; ti++) {
      dbg_loop_data_in[ti] = 0;
    }
  
  // values for internal operations of direct convolution stage: padding, cvt, mul, add
  // padding
  size_t size_dbg_loop_data_dc_pad_out =  I_input * NUM_OF_I_ITERS * (H + 2) * (W + 2) * MAX_KERNELS;
  size_t size_dbg_loop_data_dc_pad_out_bytes = size_dbg_loop_data_dc_pad_out * sizeof(data_type);
  if (posix_memalign((void **)&dbg_loop_data_dc_pad_out, 4096, size_dbg_loop_data_dc_pad_out_bytes) != 0) {
    printf("FATAL posix_memalign failed to allocate space for pointer: dbg_loop_data_dc_pad_out[MAX_KERNELS]\n");
    CHECK(1);
  }
  
  // cvt
  // frame_t consists of 9 pixel_in_t
  // Despite that the main loop in cvt.cpp runs for NUM_OF_I_ITERS * (H + 2) * (W + 2), it writes less data to output buffer
  size_t size_dbg_loop_data_dc_cvt_out = CPI * NUM_PIXELS_IN_FRAME * NUM_OF_I_ITERS * (H) * (W) * MAX_KERNELS;
  size_t size_dbg_loop_data_dc_cvt_out_bytes = size_dbg_loop_data_dc_cvt_out * sizeof(data_type);
  if (posix_memalign((void **)&dbg_loop_data_dc_cvt_out, 4096, size_dbg_loop_data_dc_cvt_out_bytes) != 0) {
    printf("FATAL posix_memalign failed to allocate space for pointer: dbg_loop_data_dc_cvt_out[MAX_KERNELS]\n");
    CHECK(1);
  }
   
  // mul
  size_t size_dbg_loop_data_dc_mul_out = O_output * NUM_OF_I_ITERS * W * H * MAX_KERNELS;
  size_t size_dbg_loop_data_dc_mul_out_bytes = size_dbg_loop_data_dc_mul_out * sizeof(data_type);
  if (posix_memalign((void **)&dbg_loop_data_dc_mul_out, 4096, size_dbg_loop_data_dc_mul_out_bytes) != 0) {
    printf("FATAL posix_memalign failed to allocate space for pointer: my_loop_data_dc_mul_out[MAX_KERNELS]\n");
    CHECK(1);
  }
  
  // add stage output, it is the same as
  // copy of values of directconv (full) stage output 
  size_t size_dbg_loop_data_out =  O_output * W * H * MAX_KERNELS;
  size_t size_dbg_loop_data_out_bytes = size_dbg_loop_data_out * sizeof(data_type);
  if (posix_memalign((void **)&dbg_loop_data_directconv_out, 4096, size_dbg_loop_data_out_bytes) != 0) {
    printf("FATAL posix_memalign failed to allocate space for pointer: my_loop_data_out[MAX_KERNELS]\n");
    CHECK(1);
  }
  for(int ti=0; ti <  size_dbg_loop_data_out; ti++) {
   dbg_loop_data_directconv_out[ti] = 0;
  }
#endif

#ifdef OPENCL_TEST
  // Now we allocate those buffers in the FPGA (for OpenCL)
  cl_int err;
//  data_in_ddr.flags  =  0 /*| XCL_MEM_TOPOLOGY*/;
//  data_in_ddr.obj = data_in;
//  data_in_ddr.param = 0;
//  out_ddr[0].flags  = 0 /*| XCL_MEM_TOPOLOGY*/;
//  out_ddr[0].obj = out;
//  out_ddr[0].param = 0;
//  //out_ddr[0].banks = XCL_MEM_DDR_BANK1;
  //data_in_ddr. = data_in;
  //out_ddr[0].  = out;
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
//  kernel_ddr[0].flags  = 0 /*| XCL_MEM_TOPOLOGY*/;
//  kernel_ddr[0].obj = kernel;
//  kernel_ddr[0].param = 0;
  //kernel_ddr[0]. = ;
#endif
#ifdef DWS_CONV
//  kernel_dw_ddr[0].flags  = 0 /*| XCL_MEM_TOPOLOGY*/;
//  kernel_dw_ddr[0].obj = dw_kernel;
//  kernel_dw_ddr[0].param = 0;
//  kernel_pw_ddr[0].flags  = 0 /*| XCL_MEM_TOPOLOGY*/;
//  kernel_pw_ddr[0].obj = pw_kernel;
//  kernel_pw_ddr[0].param = 0;
  //kernel_dw_ddr[0]. = dw_kernel;
  //kernel_pw_ddr[0]. = pw_kernel
#endif

//  bias_ddr[0].flags  = 0 /*| XCL_MEM_TOPOLOGY*/;
//  bias_ddr[0].obj = bias;
//  bias_ddr[0].param = 0;
    //bias_ddr[0]. = bias;

  
  OCL_CHECK(err, buffer_i       = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_data_in_bytes, data_in, &err));
  OCL_CHECK(err, buffer_o[0]    = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR , size_output_in_bytes, &out[0], &err));

#ifdef HLS_DEBUG
  printf("JM10  data_in -> buffer_i -> size_in_bytes = %lu\n", size_data_in_bytes);
  printf("JM10  out     -> buffer_o -> size_in_bytes = %lu\n", size_output_in_bytes);
  jm10_buffer_size_in_bytes = size_data_in_bytes;
#endif
  
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  OCL_CHECK(err, buffer_k[0]    = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_kernel_in_bytes, &kernel[0], &err));
#endif
#ifdef DWS_CONV
  OCL_CHECK(err, buffer_k_dw[0]    = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_kernel_dw_in_bytes, &dw_kernel[0], &err));
  OCL_CHECK(err, buffer_k_pw[0]    = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_kernel_pw_in_bytes, &pw_kernel[0], &err));
#endif
  OCL_CHECK(err, buffer_bias[0] = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_bias_in_bytes, &bias[0], &err));
#endif


#ifdef HLS_DEBUG
  OCL_CHECK(err, dbg_loop_data_in_buffer_i[0]           = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR , size_dbg_loop_data_in_bytes,            &dbg_loop_data_in[0],  &err));
  OCL_CHECK(err, dbg_loop_data_input_buffer_buffer[0]   = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR , size_dbg_loop_data_input_buffer_bytes,  &dbg_loop_data_input_buffer[0],  &err));

  OCL_CHECK(err, dbg_loop_data_dc_pad_out_buffer[0] = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR , size_dbg_loop_data_dc_pad_out_bytes,        &dbg_loop_data_dc_pad_out[0], &err));
  OCL_CHECK(err, dbg_loop_data_dc_cvt_out_buffer[0] = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR , size_dbg_loop_data_dc_cvt_out_bytes,        &dbg_loop_data_dc_cvt_out[0], &err));
  OCL_CHECK(err, dbg_loop_data_dc_mul_out_buffer[0] = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR , size_dbg_loop_data_dc_mul_out_bytes,        &dbg_loop_data_dc_mul_out[0], &err));
  OCL_CHECK(err, dbg_loop_data_directconv_out_buffer[0] = new cl::Buffer(context, /*CL_MEM_EXT_PTR_XILINX |*/ CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR , size_dbg_loop_data_out_bytes,           &dbg_loop_data_directconv_out[0], &err));
#endif


}

// deallocate_buffers. Deallocates all CPU buffers
void deallocate_buffers() {
  free(data_in);
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
  if ((enable_maxpooling) || (enable_avgpooling)) {
	free(out_pool_cpu);
  }

#ifdef HLS_DEBUG
  free (dbg_loop_data_in);
  free (dbg_loop_data_input_buffer);
  free (dbg_loop_data_dc_pad_out);
  free (dbg_loop_data_dc_cvt_out);
  free (dbg_loop_data_dc_mul_out);
  free (dbg_loop_data_directconv_out);
#endif
}


#ifdef OPENCL_TEST
// copy buffers to FPGA
void copy_to_fpga() {
  cl_int err;
  printf("Write data from buffers into fpga memory\n");
  #ifdef HLS_DEBUG
  printf("enqueue buffer-din migrate operation\n");
  #endif
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_i}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
  #ifdef HLS_DEBUG
  printf("....data moved\n");
  #endif

#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  #ifdef HLS_DEBUG
  printf("enqueue buffer-kernel migrate operation\n");
  #endif

  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_k[0]}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
  #ifdef HLS_DEBUG
  printf("....data moved\n");
  #endif
#endif
#ifdef DWS_CONV
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_k_dw[0]}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_k_pw[0]}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
#endif

  #ifdef HLS_DEBUG
  printf("enqueue buffer-bias migrate operation\n");
  #endif
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_bias[0]}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
  #ifdef HLS_DEBUG
  printf("....data moved\n");
  #endif

}
#endif

#ifdef OPENCL_TEST
void copy_from_fpga() {
  cl_int err;
  // std::cout << "Getting Results (Device to Host)..." << std::endl;
  std::vector<cl::Event> eventList;
  eventList.push_back(kernel_events[0]);

  printf("Read data from fpga memory and write into hots memory buffers\n");

  // This operation only needs to wait for the kernel call.
  #ifdef HLS_DEBUG
  printf("HLS debug enqueue read buffer_o migrate operation\n");
  #endif
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*buffer_o[0]}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
  printf("....data moved\n");


  #ifdef HLS_DEBUG
  // This operation only needs to wait for the kernel call.
  printf("HLS debug enqueue read dbg_loop_data_in_buffer migrate operation\n");
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*dbg_loop_data_in_buffer_i[0]}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
  printf("....data moved\n");

  // This operation only needs to wait for the kernel call.
  printf("HLS debug enqueue read dbg_loop_data_input_buffer migrate operation\n");
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*dbg_loop_data_input_buffer_buffer[0]}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
  printf("....data moved\n");

  printf("HLS debug enqueue read dbg_loop_data_dc_pad_out_buffer migrate operation\n");
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*dbg_loop_data_dc_pad_out_buffer[0]}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
  printf("....data moved\n");

  printf("HLS debug enqueue read dbg_loop_data_dc_cvt_out_buffer migrate operation\n");
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*dbg_loop_data_dc_cvt_out_buffer[0]}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
  printf("....data moved\n");

  printf("HLS debug enqueue read dbg_loop_data_dc_mul_out_buffer migrate operation\n");
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*dbg_loop_data_dc_mul_out_buffer[0]}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
  printf("....data moved\n");

  printf("HLS debug enqueue read dbg_loop_data_directconv_out_buffer migrate operation\n");
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*dbg_loop_data_directconv_out_buffer[0]}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
  printf("....data moved\n");
  #endif

  
  // Wait for all of the OpenCL operations to complete
  OCL_CHECK(err, err = q.flush());
  OCL_CHECK(err, err = q.finish());
}
#endif



