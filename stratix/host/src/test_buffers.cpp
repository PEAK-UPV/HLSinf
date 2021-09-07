/*
 * Buffer-related functions for test
 */

#include "test_conv2D.h"

//-----------------------------------------------------------------------------
// Allocate_buffers. Allocates in CPU memory all the needed buffers
//-----------------------------------------------------------------------------
void allocate_buffers() {

  // First we allocate buffers in CPU


  // NOTE that scoped_aligned_ptr vector reset function sets the number of of elements in array of type "template"
  printf("1\n");
  // input data buffer
  size_t size_data_num_values = I_input * W * H;
  size_t size_data_in_bytes   = size_data_num_values * sizeof(data_type);
  data_in.reset(size_data_num_values);

  printf("2\n");
  // weights buffer (kernel), depending on the type of convolution
  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  size_t size_kernel_num_values = I_kernel * O_kernel * KW * KH;
  size_t size_kernel_in_bytes   = size_kernel_num_values * sizeof(data_type);
  kernel.reset(size_kernel_num_values);
  #endif
  printf("3\n");
  #ifdef DWS_CONV
  size_t size_kernel_dw_num_values = (I_kernel * KW * KH);
  size_t size_kernel_dw_in_bytes   = size_kernel_dw_num_values * sizeof(data_type);
  size_t size_kernel_pw_num_values = (I_kernel * O_kernel);
  size_t size_kernel_pw_in_bytes   = (I_kernel * O_kernel) * sizeof(data_type);
  dw_kernel.reset(size_kernel_dw_num_values);
  pw_kernel.reset(size_kernel_pw_num_values);
  #endif

  printf("4\n");
  // bias buffer
  size_t size_bias_num_values = O_output;
  size_t size_bias_in_bytes   = size_bias_num_values * sizeof(data_type);
  bias.reset(size_bias_num_values);

  printf("5\n");
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
  printf("6\n");
  out.reset(size_output_num_values);

  printf("7\n");
  out_conv_cpu.reset(O_output * W * H);


   printf("8\n");
  // output for relu function
  if (enable_relu) {
    out_relu_cpu.reset(O_output * W * H );
  }

  printf("9\n");
  // output for pool function
  if ((enable_maxpooling) || (enable_avgpooling)) {
    out_pool_cpu.reset(O_output * (W/2) * (H/2));

  }

#ifdef HLS_DEBUG
  printf("10\n");
  // simple type values arrays
  size_t size_hls_dbg_ul_num_values = NUM_HLS_DBG_VALUE_ARRAY_ENTRIES * MAX_CONVS;
  size_t size_hls_dbg_ul_in_bytes   = size_hls_dbg_ul_num_values * sizeof(unsigned long); 
  hls_dbg_ul.reset(size_hls_dbg_ul_num_values);
  for(int ti=0; ti < size_hls_dbg_ul_num_values; ti++) {
    hls_dbg_ul[ti] = (unsigned long)0;
  }

  printf("11\n");
  size_t size_hls_dbg_dt_num_values = NUM_HLS_DBG_VALUE_ARRAY_ENTRIES * MAX_CONVS;
  size_t size_hls_dbg_dt_in_bytes   = size_hls_dbg_dt_num_values * sizeof(data_type);
  hls_dbg_dt.reset(size_hls_dbg_dt_num_values);
  for(int ti=0; ti < size_hls_dbg_dt_num_values; ti++) {
    hls_dbg_dt[ti] = (data_type)0;
  }

  // data_in 
  printf("12\n");
  size_t size_dbg_loop_data_in = I_input * W * H * MAX_CONVS;
  size_t size_dbg_loop_data_in_bytes = size_dbg_loop_data_in * sizeof(data_type);
  dbg_loop_data_in.reset(size_dbg_loop_data_in);
  for(int ti=0; ti <  size_dbg_loop_data_in; ti++) {
    dbg_loop_data_in[ti] = (data_type)0;
  }

  // values after input_buffer stage of kernel
  // this data has the same format as data_in, it is data after input buffers, to deal with congestion of input
  printf("13\n");
  size_t size_dbg_loop_data_input_buffer = I_input * W * H * MAX_CONVS;
  size_t size_dbg_loop_data_input_buffer_bytes = size_dbg_loop_data_input_buffer * sizeof(data_type);
  dbg_loop_data_input_buffer.reset(size_dbg_loop_data_input_buffer);
  for(int ti=0; ti <  size_dbg_loop_data_input_buffer; ti++) {
    dbg_loop_data_input_buffer[ti] = (data_type)0;
  }
  
  // values for internal operations of direct convolution stage: padding, cvt, mul, add
  // padding
  printf("14\n");
  size_t size_dbg_loop_data_dc_pad_out =  I_input * NUM_OF_I_ITERS * (H + 2) * (W + 2) * MAX_CONVS;
  size_t size_dbg_loop_data_dc_pad_out_bytes = size_dbg_loop_data_dc_pad_out * sizeof(data_type);
  dbg_loop_data_dc_pad_out.reset(size_dbg_loop_data_dc_pad_out);
  for(int ti=0; ti < size_dbg_loop_data_dc_pad_out; ti++) {
    dbg_loop_data_dc_pad_out[ti] = (data_type)0;
  }
  
  // cvt
  // frame_t consists of 9 pixel_in_t
  // Despite that the main loop in cvt.cpp runs for NUM_OF_I_ITERS * (H + 2) * (W + 2), it writes less data to output buffer
  printf("15\n");
  size_t size_dbg_loop_data_dc_cvt_out = CPI * NUM_PIXELS_IN_FRAME * NUM_OF_I_ITERS * (H) * (W) * MAX_CONVS;
  size_t size_dbg_loop_data_dc_cvt_out_bytes = size_dbg_loop_data_dc_cvt_out * sizeof(data_type);
  dbg_loop_data_dc_cvt_out.reset(size_dbg_loop_data_dc_cvt_out_bytes);
  for ( int ti = 0; ti < size_dbg_loop_data_dc_cvt_out; ti++) {
    dbg_loop_data_dc_cvt_out[ti] = (data_type)0;
  }

  printf("15-a\n");
  // int num_extra_rowscols already calculated for padding vector
  // the number of elements in this vector matches the the number of elements of padding stream * xxx (number of
  size_t size_dbg_loop_data_dc_cvt_sbs_control_out = CPI * NUM_OF_I_ITERS * (H + 2) * (W + 2) * MAX_CONVS;
  size_t size_dbg_loop_data_dc_cvt_sbs_control_out_bytes = size_dbg_loop_data_dc_cvt_out * sizeof(hls_cvt_sbs_control_t); 
  dbg_loop_data_dc_cvt_sbs_control_out.reset(size_dbg_loop_data_dc_cvt_sbs_control_out_bytes);
  for(int ti = 0; ti < size_dbg_loop_data_dc_cvt_sbs_control_out; ti++){
    dbg_loop_data_dc_cvt_sbs_control_out[ti].iter = (unsigned long) 0;//                  0
    dbg_loop_data_dc_cvt_sbs_control_out[ti].pin_row = (unsigned long) 0;//               1
    dbg_loop_data_dc_cvt_sbs_control_out[ti].pin_col = (unsigned long) 0;//               2
    dbg_loop_data_dc_cvt_sbs_control_out[ti].row0_buffer_write = (unsigned long) 0;//     3
    dbg_loop_data_dc_cvt_sbs_control_out[ti].row1_buffer_write = (unsigned long) 0;//     4
    dbg_loop_data_dc_cvt_sbs_control_out[ti].row2_buffer_write = (unsigned long) 0;//     5
    dbg_loop_data_dc_cvt_sbs_control_out[ti].p = (unsigned long) 0;//                     6
    dbg_loop_data_dc_cvt_sbs_control_out[ti].pin_col0 = (unsigned long) 0;//              7
    dbg_loop_data_dc_cvt_sbs_control_out[ti].pin_col = (unsigned long) 01;//              8
    dbg_loop_data_dc_cvt_sbs_control_out[ti].row0 = (unsigned long) 0;//                  9
    dbg_loop_data_dc_cvt_sbs_control_out[ti].row1 = (unsigned long) 0;//                 10
    dbg_loop_data_dc_cvt_sbs_control_out[ti].shift_frame = (unsigned long) 0;//          11
    dbg_loop_data_dc_cvt_sbs_control_out[ti].send_frame = (unsigned long) 0;//           12
  }

  printf("15-b\n");
  // int num_extra_rowscols already calculated for padding vector
  // the number of elements in this vector matches the the number of elements of padding stream 
  size_t size_dbg_loop_data_dc_cvt_sbs_frame_out = CPI *  NUM_OF_I_ITERS * (H + 2) * (W + 2) * MAX_CONVS;
  size_t size_dbg_loop_data_dc_cvt_sbs_frame_out_bytes = size_dbg_loop_data_dc_cvt_sbs_frame_out * sizeof(frame_t); 
  dbg_loop_data_dc_cvt_sbs_frame_out.reset(size_dbg_loop_data_dc_cvt_sbs_frame_out_bytes);
  //for (int ti = 0; ti < size_dbg_loop_data_dc_cvt_sbs_frame_out; ti++) {
  //  dbg_loop_data_dc_cvt_sbs_frame_out[ti] = (data_type)0;
  //}

  // mul
  printf("16\n");
  size_t size_dbg_loop_data_dc_mul_out = O_output * NUM_OF_I_ITERS * W * H * MAX_CONVS;
  size_t size_dbg_loop_data_dc_mul_out_bytes = size_dbg_loop_data_dc_mul_out * sizeof(data_type);
  dbg_loop_data_dc_mul_out.reset(size_dbg_loop_data_dc_mul_out);
  for(int ti = 0; ti < size_dbg_loop_data_dc_mul_out; ti++) {
    dbg_loop_data_dc_mul_out[ti] = (data_type)0;
  }
  
  // add stage output, it is the same as
  // copy of values of directconv (full) stage output 
  
  printf("17\n");
  size_t size_dbg_loop_data_out =  O_output * W * H * MAX_CONVS;
  size_t size_dbg_loop_data_out_bytes = size_dbg_loop_data_out * sizeof(data_type);
  dbg_loop_data_directconv_out.reset(size_dbg_loop_data_out);
  for (int ti = 0; ti < size_dbg_loop_data_out; ti++){
    dbg_loop_data_directconv_out[ti] = (data_type)0;
  }
  
  //
  printf("18\n");
  dbg_cpu_data_directconv_out.reset(O_output * W * H * sizeof(data_type));
  for(int ti=0; ti <  O_output * W * H * sizeof(data_type); ti++) {
   dbg_cpu_data_directconv_out[ti] = (data_type)0;
  }

#endif


  // CREATE memroy buffers and link them with the host memory pointer
#ifdef OPENCL_TEST
  cl_int err;
  
  buffer_i = clCreateBuffer(context, CL_MEM_READ_ONLY, size_data_in_bytes, NULL, &err); // not using host pointer
  CHECK(err);
  buffer_o = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_output_in_bytes, NULL, &err);
  CHECK(err);
  
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  buffer_k = clCreateBuffer(context, CL_MEM_READ_ONLY, size_kernel_in_bytes, NULL, &err);
  CHECK(err);
#endif
#ifdef DWS_CONV
  buffer_k_dw = clCreateBuffer(context, CL_MEM_READ_ONLY, size_kernel_dw_in_bytes, NULL, &err);
  CHECK(err);
  buffer_k_pw[0] = clCreateBuffer(context, CL_MEM_READ_ONLY, size_kernel_pw_in_bytes, NULL, &err);
  CHECK(err);
#endif
  buffer_bias=clCreateBuffer(context, CL_MEM_READ_ONLY, size_bias_in_bytes, NULL, &err);
  CHECK(err);
#endif


#ifdef HLS_DEBUG
  printf("JM10  data_in -> buffer_i -> size_in_bytes = %lu\n", size_data_in_bytes);
  printf("JM10  out     -> buffer_o -> size_in_bytes = %lu\n", size_output_in_bytes);

  printf("  hls_dbg_ul_buffer\n");
  hls_dbg_ul_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_hls_dbg_ul_in_bytes, NULL, &err);
  CHECK(err);
  printf("  hls_dbg_dt_buffer\n");
  hls_dbg_dt_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_hls_dbg_dt_in_bytes, NULL, &err);
  CHECK(err);
  printf("  dbg_loop_data_in_buffer_i\n");
  dbg_loop_data_in_buffer_i = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_dbg_loop_data_in_bytes, NULL, &err);
  CHECK(err);
  printf("  dbg_loop_data_input_buffer_buffer\n");
  dbg_loop_data_input_buffer_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_dbg_loop_data_input_buffer_bytes, NULL, &err);
  CHECK(err);
  printf("  dbg_loop_data_dc_pad_out_buffer\n");
  dbg_loop_data_dc_pad_out_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_dbg_loop_data_dc_pad_out_bytes, NULL, &err);
  CHECK(err);
  printf("  dbg_loop_data_dc_cvt_out_buffer\n");
  dbg_loop_data_dc_cvt_out_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_dbg_loop_data_dc_cvt_out_bytes, NULL, &err);
  CHECK(err);
  printf("  dbg_loop_data_dc_cvt_sbs_control_out_buffer\n");
  dbg_loop_data_dc_cvt_sbs_control_out_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_dbg_loop_data_dc_cvt_sbs_control_out_bytes, NULL, &err);
  CHECK(err);
  printf("  dbg_loop_data_dc_cvt_sbs_frame_out_buffer\n");
  dbg_loop_data_dc_cvt_sbs_frame_out_buffer   = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_dbg_loop_data_dc_cvt_sbs_frame_out_bytes, NULL, &err);
  CHECK(err);
  printf("  dbg_loop_data_dc_mul_out_buffer\n");
  dbg_loop_data_dc_mul_out_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_dbg_loop_data_dc_mul_out_bytes, NULL, &err);
  CHECK(err);
  printf("  dbg_loop_data_directconv_out_buffer\n");
  dbg_loop_data_directconv_out_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_dbg_loop_data_out_bytes, NULL, &err);
  CHECK(err);
#endif


}

//-----------------------------------------------------------------------------
// deallocate_buffers. Deallocates all CPU buffers
//-----------------------------------------------------------------------------
void deallocate_buffers() {

  printf(KGRN "Release host memory resources\n" KNRM);
  if(buffer_i) {
    clReleaseMemObject(buffer_i);
    buffer_i = NULL;
  }
  if (buffer_o) {
    clReleaseMemObject(buffer_o);
    buffer_o = NULL;
  }
  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  if (buffer_k) {
    clReleaseMemObject(buffer_k);
    buffer_k = NULL;
  }
  #endif
  #ifdef DWS_CONV
  if (buffer_k_dw) {
    clReleaseMemObject(buffer_k_dw);
    buffer_k_dw = NULL;
  }

  if (buffer_k_pw[0]) {
    clReleaseMemObject(buffer_k_pw);
    buffer_k_pw = NULL;
  }
  #endif

  if(buffer_bias) {
    clReleaseMemObject(buffer_bias);
    buffer_bias = NULL;
  }


#ifdef HLS_DEBUG
  if(hls_dbg_ul_buffer) {
    clReleaseMemObject(hls_dbg_ul_buffer);
    hls_dbg_ul_buffer = NULL;
  }
  if(hls_dbg_dt_buffer) {
    clReleaseMemObject(hls_dbg_dt_buffer);
    hls_dbg_dt_buffer = NULL;
  }
  if(dbg_loop_data_in_buffer_i) {
    clReleaseMemObject(dbg_loop_data_in_buffer_i);
    dbg_loop_data_in_buffer_i = NULL;
  }
  if(dbg_loop_data_input_buffer_buffer) {
    clReleaseMemObject(dbg_loop_data_input_buffer_buffer);
    dbg_loop_data_input_buffer_buffer = NULL;
  }
  if(dbg_loop_data_dc_pad_out_buffer) {
    clReleaseMemObject(dbg_loop_data_dc_pad_out_buffer);
    dbg_loop_data_dc_pad_out_buffer = NULL;
  }
  if(dbg_loop_data_dc_cvt_out_buffer) {
    clReleaseMemObject(dbg_loop_data_dc_cvt_out_buffer);
    dbg_loop_data_dc_cvt_out_buffer = NULL;
  }
  if (dbg_loop_data_dc_cvt_sbs_control_out_buffer) {
    clReleaseMemObject(dbg_loop_data_dc_cvt_sbs_control_out_buffer);
    dbg_loop_data_dc_cvt_sbs_control_out_buffer = NULL;
  }
  if(dbg_loop_data_dc_cvt_sbs_frame_out_buffer) {
    clReleaseMemObject(dbg_loop_data_dc_cvt_sbs_frame_out_buffer);
    dbg_loop_data_dc_cvt_sbs_frame_out_buffer = NULL;
  }
  if(dbg_loop_data_dc_mul_out_buffer) {
    clReleaseMemObject(dbg_loop_data_dc_mul_out_buffer);
    dbg_loop_data_dc_mul_out_buffer = NULL;
  }
  if(dbg_loop_data_directconv_out_buffer) {
    clReleaseMemObject(dbg_loop_data_directconv_out_buffer);
    dbg_loop_data_directconv_out_buffer = NULL;
  }
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

  printf("Write data from buffers into fpga memory\n");
  #ifdef HLS_DEBUG
  printf("enqueue buffer-din migrate operation\n");
  #endif
  size_t size_data_in_bytes = I_input * W * H * sizeof(data_type);
  err = clEnqueueWriteBuffer(q, buffer_i, CL_FALSE, 0, size_data_in_bytes, data_in, 0, NULL, &write_events[0]);
  num_write_events_before_kernel_execution++;
  CHECK(err);
  #ifdef HLS_DEBUG
  printf("....data moved\n");
  #endif

  #ifdef HLS_DEBUG
  printf("enqueue buffer-bias migrate operation\n");
  #endif
  size_t size_bias_in_bytes = O_output * sizeof(data_type);
  err = clEnqueueWriteBuffer(q, buffer_bias, CL_FALSE, 0, size_bias_in_bytes, bias, 0, NULL, &write_events[1]);
  num_write_events_before_kernel_execution++;
  CHECK(err);
  #ifdef HLS_DEBUG
  printf("....data moved\n");
  #endif


#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  #ifdef HLS_DEBUG
  printf("enqueue buffer-kernel migrate operation\n");
  #endif
  size_t size_kernel_in_bytes = I_kernel * O_kernel * KW * KH * sizeof(data_type);
  err = clEnqueueWriteBuffer(q, buffer_k, CL_FALSE, 0, size_kernel_in_bytes, kernel, 0, NULL, &write_events[2]);
  num_write_events_before_kernel_execution++;
  CHECK(err);
  #ifdef HLS_DEBUG
  printf("....datamoved\n");
  #endif
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

  #ifdef HLS_DEBUG
  printf("copy to fpga: triggered write operation for %d buffers\n", num_write_events_before_kernel_execution);
  #endif
  err = clWaitForEvents(num_write_events_before_kernel_execution, write_events);
  CHECK(err);
  #ifdef HLS_DEBUG
  printf("write buffers to fpga completed\n" );
  #endif


  //finished, releasing vents
  printf(KRED "JM10 TO-DO: release write events\n" KNRM);
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

  printf (KGRN "Enqueue read-data operation from device memory via DMA\n" KRST);

  // This operation only needs to wait for the kernel call.
  #ifdef HLS_DEBUG
  printf("HLS debug enqueue read buffer_o migrate operation\n");
  #endif
  size_t size_output_in_bytes;
  if ((enable_maxpooling) || (enable_avgpooling)) {
    size_output_in_bytes = O_output * (W/2) * (H/2) * sizeof(data_type);
	//posix_memalign((void **)&out, 4096, size_output_in_bytes);
  } else {
    size_output_in_bytes = O_output * W * H * sizeof(data_type);
  }

  num_events_to_wait = 0;
  err = clEnqueueReadBuffer(q, buffer_o, CL_FALSE, 0, size_output_in_bytes, out, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  #ifdef HLS_DEBUG
  printf("HLS debug enqueue read hls_dbg_ul_buffer migrate operation\n");
  size_t size_hls_dbg_ul_num_values = NUM_HLS_DBG_VALUE_ARRAY_ENTRIES * MAX_CONVS;
  size_t size_hls_dbg_ul_in_bytes   = size_hls_dbg_ul_num_values * sizeof(unsigned long);  
  err = clEnqueueReadBuffer(q, hls_dbg_ul_buffer, CL_FALSE, 0, size_hls_dbg_ul_in_bytes, hls_dbg_ul, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  printf("HLS debug enqueue read hls_dbg_dt_buffer migrate operation\n");
  size_t size_hls_dbg_dt_num_values = NUM_HLS_DBG_VALUE_ARRAY_ENTRIES * MAX_CONVS;
  size_t size_hls_dbg_dt_in_bytes   = size_hls_dbg_dt_num_values * sizeof(data_type);
  err = clEnqueueReadBuffer(q, hls_dbg_dt_buffer, CL_FALSE, 0, size_hls_dbg_dt_in_bytes, hls_dbg_dt, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  printf("HLS debug enqueue read dbg_loop_data_in_buffer migrate operation\n");
  size_t size_dbg_loop_data_in = I_input * W * H * MAX_CONVS;
  size_t size_dbg_loop_data_in_bytes = size_dbg_loop_data_in * sizeof(data_type);
  err = clEnqueueReadBuffer(q, dbg_loop_data_in_buffer_i, CL_FALSE, 0, size_dbg_loop_data_in_bytes, dbg_loop_data_in, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  printf("HLS debug enqueue read dbg_loop_data_input_buffer migrate operation\n");
  size_t size_dbg_loop_data_input_buffer = I_input * W * H * MAX_CONVS;
  size_t size_dbg_loop_data_input_buffer_bytes = size_dbg_loop_data_input_buffer * sizeof(data_type);
  err = clEnqueueReadBuffer(q, dbg_loop_data_input_buffer_buffer, CL_FALSE, 0, size_dbg_loop_data_input_buffer_bytes, dbg_loop_data_input_buffer, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  printf("HLS debug enqueue read dbg_loop_data_dc_pad_out_buffer migrate operation\n");
  size_t size_dbg_loop_data_dc_pad_out =  I_input * NUM_OF_I_ITERS * (H + 2) * (W + 2) * MAX_CONVS;
  size_t size_dbg_loop_data_dc_pad_out_bytes = size_dbg_loop_data_dc_pad_out * sizeof(data_type);
  err = clEnqueueReadBuffer(q, dbg_loop_data_dc_pad_out_buffer, CL_FALSE, 0, size_dbg_loop_data_dc_pad_out_bytes, dbg_loop_data_dc_pad_out, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  printf("HLS debug enqueue read dbg_loop_data_dc_cvt_out_buffer migrate operation\n");
  size_t size_dbg_loop_data_dc_cvt_out = CPI * NUM_PIXELS_IN_FRAME * NUM_OF_I_ITERS * (H) * (W) * MAX_CONVS;
  size_t size_dbg_loop_data_dc_cvt_out_bytes = size_dbg_loop_data_dc_cvt_out * sizeof(data_type);
  err = clEnqueueReadBuffer(q, dbg_loop_data_dc_cvt_out_buffer, CL_FALSE, 0, size_dbg_loop_data_dc_cvt_out_bytes, dbg_loop_data_dc_cvt_out, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  printf("HLS debug enqueue read dbg_loop_data_dc_cvt_sbs_control_out migrate operation\n"); fflush(stdout);
  size_t size_dbg_loop_data_dc_cvt_sbs_control_out = CPI * NUM_OF_I_ITERS * (H + 2) * (W + 2) * MAX_CONVS;
  size_t size_dbg_loop_data_dc_cvt_sbs_control_out_bytes = size_dbg_loop_data_dc_cvt_out * sizeof(hls_cvt_sbs_control_t); 
  err = clEnqueueReadBuffer(q, dbg_loop_data_dc_cvt_sbs_control_out_buffer, CL_FALSE, 0, size_dbg_loop_data_dc_cvt_sbs_control_out_bytes, dbg_loop_data_dc_cvt_sbs_control_out, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  printf("HLS debug enqueue read dbg_loop_data_dc_cvt_sbs_frame_out migrate operation\n"); fflush(stdout);
  size_t size_dbg_loop_data_dc_cvt_sbs_frame_out = CPI *  NUM_OF_I_ITERS * (H + 2) * (W + 2) * MAX_CONVS; 
  size_t size_dbg_loop_data_dc_cvt_sbs_frame_out_bytes = size_dbg_loop_data_dc_cvt_sbs_frame_out * sizeof(frame_t);
  err = clEnqueueReadBuffer(q, dbg_loop_data_dc_cvt_sbs_frame_out_buffer, CL_FALSE, 0, size_dbg_loop_data_dc_cvt_sbs_frame_out_bytes, dbg_loop_data_dc_cvt_sbs_frame_out, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  printf("HLS debug enqueue read dbg_loop_data_dc_mul_out_buffer migrate operation\n");
  size_t size_dbg_loop_data_dc_mul_out = O_output * NUM_OF_I_ITERS * W * H * MAX_CONVS;
  size_t size_dbg_loop_data_dc_mul_out_bytes = size_dbg_loop_data_dc_mul_out * sizeof(data_type);
  err = clEnqueueReadBuffer(q, dbg_loop_data_dc_mul_out_buffer, CL_FALSE, 0, size_dbg_loop_data_dc_mul_out_bytes, dbg_loop_data_dc_mul_out, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

  printf("HLS debug enqueue read dbg_loop_data_directconv_out_buffer migrate operation\n");
  size_t size_dbg_loop_data_out =  O_output * W * H * MAX_CONVS;
  size_t size_dbg_loop_data_out_bytes = size_dbg_loop_data_out * sizeof(data_type);
  err = clEnqueueReadBuffer(q, dbg_loop_data_directconv_out_buffer, CL_FALSE, 0, size_dbg_loop_data_out_bytes, dbg_loop_data_directconv_out, 0, NULL, &read_events[num_events_to_wait]);
  num_events_to_wait++;
  CHECK(err);

#endif
 
  // Wait for all of the OpenCL operations to complete
  #ifdef HLS_DEBUG
  printf("copy from fpga: triggered read operation for %d buffers\n", num_events_to_wait);
  #endif
  err = clWaitForEvents(num_events_to_wait, read_events);
  CHECK(err);
  #ifdef HLS_DEBUG
  printf("read buffers from fpga completed\n" );
  #endif

}
#endif

//-----------------------------------------------------------------------------
// end of file: test_buffers.cpp
//-----------------------------------------------------------------------------
