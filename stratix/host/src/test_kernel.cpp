/*
 * kernel-related functions for test
 */

#include "test_conv2D.h"

#include <unistd.h>

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

#ifdef HLS_DEBUG
  printf("HLS DEBUG run_kernel using %d kernels  (o_iter=%d  (max_)NUM_KERNELS=%d) \n", num_kernels, o_iter, NUM_KERNELS);
#endif

   
  for (int k=0; k<num_kernels; k++) {

    int o_iter_first = o_iter_per_kernel * k;
    int o_iter_last  = o_iter_first + o_iter_per_kernel - 1;

    char str[80];
    sprintf(str, "launching kernel %d (output iterations %d to %d)", k, o_iter_first, o_iter_last);
    print_message(str);

   
#ifdef OPENCL_TEST
    // set kernel arguments
    #ifdef HLS_DEBUG
    printf("setting kernel Arguments\n");
    #endif
    cl_int err;
    int arg = 0;
    // kernel_conv2d[k] not exists now, replaced by "kernel"
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(buffer_i), (void*)&buffer_i));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(H),         &H));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(W),         &W));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(rows),      &rows));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(I_input),   &I_input));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(O_output),  &O_output));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(i_iter),    &i_iter));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(o_iter_first), &o_iter_first));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(o_iter_last),  &o_iter_last));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(enable_relu),  &enable_relu));
    #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(buffer_k),     (void*)&buffer_k));
    #endif
    #ifdef DWS_CONV
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(buffer_k_dw),   (void*)&buffer_k_dw));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(buffer_k_pw),   (void*)&buffer_k_pw));
    #endif
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(buffer_bias),   (void*)&buffer_bias));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(buffer_o),      (void*)&buffer_o));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(global_offset), &global_offset));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(enable_upper_padding), &enable_upper_padding));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(enable_lower_padding), &enable_lower_padding));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(enable_maxpooling),    &enable_maxpooling));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(enable_avgpooling),    &enable_avgpooling));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(enable_clipping),      &enable_clipping));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(enable_shift),         &enable_shift));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(min_clip),  &min_clip));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(max_clip),  &max_clip));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(dir_shift), &dir_shift));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(pos_shift), &pos_shift));


    #ifdef HLS_DEBUG
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(hls_dbg_streams_mask),         &hls_dbg_streams_mask));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(hls_dbg_ul_buffer),     (void*)&hls_dbg_ul_buffer));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(hls_dbg_dt_buffer),     (void*)&hls_dbg_dt_buffer));

    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(dbg_loop_data_in_buffer_i),           (void*)&dbg_loop_data_in_buffer_i));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(dbg_loop_data_input_buffer_buffer),   (void*)&dbg_loop_data_input_buffer_buffer));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(dbg_loop_data_dc_pad_out_buffer),     (void*)&dbg_loop_data_dc_pad_out_buffer));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(dbg_loop_data_dc_cvt_out_buffer),     (void*)&dbg_loop_data_dc_cvt_out_buffer));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(dbg_loop_data_dc_cvt_sbs_control_out_buffer), (void*)&dbg_loop_data_dc_cvt_sbs_control_out_buffer));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(dbg_loop_data_dc_cvt_sbs_frame_out_buffer),   (void*)&dbg_loop_data_dc_cvt_sbs_frame_out_buffer));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(dbg_loop_data_dc_mul_out_buffer),     (void*)&dbg_loop_data_dc_mul_out_buffer));
    CHECK(clSetKernelArg(kernel_conv2D[k], arg++, sizeof(dbg_loop_data_directconv_out_buffer), (void*)&dbg_loop_data_directconv_out_buffer));
    #endif


//    const double krnl_start_time = getCurrentTimestamp();
//
//    //
//    #ifdef HLS_DEBUG
//    printf("JM10 OPENCL test mode enabled, enqueueing kernel execution\n");
//    #endif
//    //OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel_conv2d[k], 0, 1, 1, NULL, &kernel_events[k]));
//    //set_callback(kernel_events[k], "ooo_queue");
//    const size_t ws = 1, ls = 1;
//    err = clEnqueueNDRangeKernel(q, kernel_conv2D[k], 1, NULL, &ws, &ls, 0, NULL, &kernel_events[k]);
//    CHECK(err);
//   
//    #ifdef HLS_DEBUG
//    printf("JM10 OPENCL test mode enabled, waiting for kernel execution completion\n");
//    #endif
//    clWaitForEvents(1, &kernel_events);
//    #ifdef HLS_DEBUG
//    printf("JM10 OPENCL test mode enabled, kernel execution completed\n");
//    #endif
//    const double krnl_end_time = getCurrentTimestamp(); 
//  
//    //krnl_elapsed_time = krnl_end_time - krnl_start_time;
//    printf(KGRN "JM10 OPENCL test mode enabled kernel  completed in : %0.3f ms\n" KRST, (krnl_end_time - krnl_start_time) * 1e3);


#else
    k_conv2D((read_block_t *)data_in, H, W, rows, I, O, i_iter, o_iter_first, o_iter_last, enable_relu,
    #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
		kernel,
    #endif
    #ifdef DWS_CONV
		(data_type *)dw_kernel, (read_kernel_pw_t *)pw_kernel,
    #endif
		(pixel_out_t *)bias, (write_block_t *)out, global_offset,
		 enable_upper_padding, enable_lower_padding, enable_maxpooling, enable_avgpooling,
		 enable_clipping, enable_shift, min_clip, max_clip, dir_shift, pos_shift, my_val, &my_ret[k]);
#endif
  }

  #ifdef OPENCL_TEST
  //#ifdef HLS_DEBUG
  //printf("JM10 OPENCL test mode enabled, waiting for kernels execution (finalization notification)\n");
  //#endif
  //for (int k=0; k<num_kernels; k++) {
	//cl_int err;
  //  OCL_CHECK(err, err = kernel_events[k].wait());
  //}
  //cl_int err = clWaitForEvents(num_kernels, kernel_events);
  //CHECK(err);

  // already done
//    #ifdef HLS_DEBUG
//    printf("JM10 OPENCL test mode enabled, waiting for kernel execution completion\n");
//    #endif
//    clWaitForEvents(1, &kernel_events);
//    #ifdef HLS_DEBUG
//    printf("JM10 OPENCL test mode enabled, kernel execution completed\n");
//    #endif

  {
    cl_int t_err;
    const double krnl_start_time = getCurrentTimestamp();
    for (int k=0; k<num_kernels; k++) {
      #ifdef HLS_DEBUG
      printf("OPENCL test mode enabled, enqueueing kernel %d execution\n", k);
      #endif
      //OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel_conv2d[k], 0, 1, 1, NULL, &kernel_events[k]));
      //set_callback(kernel_events[k], "ooo_queue");
      const size_t ws = 1, ls = 1;
      t_err = clEnqueueNDRangeKernel(q, kernel_conv2D[k], 1, NULL, &ws, &ls, 0, NULL, &kernel_events[k]);
      CHECK(t_err);
    }
    #ifdef HLS_DEBUG
    printf("OPENCL test mode enabled, waiting for execution completion of %d kernel(s)\n", num_kernels);
    #endif
    


    //clWaitForEvents(num_kernels, kernel_events);
    int kernel_finished = 0;
    int it = 0;
    do {
      cl_int f_ret;
      cl_int k_ev_status;
      
      f_ret = clGetEventInfo(kernel_events[0], CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(k_ev_status), &k_ev_status, NULL);

      if (f_ret != CL_SUCCESS) printf(KRED " Error reading kernel event info\n" KNRM);

      printf ("JM0 it #%d getEventInfo for kernel event: kernel event status is %s (%d)\n",
          it,
          (k_ev_status == CL_QUEUED) ? "QUEUED" :
          (k_ev_status == CL_SUBMITTED) ? "SUBMITTED" :
          (k_ev_status == CL_RUNNING) ? "RUNNING" :
          (k_ev_status == CL_COMPLETE) ? "COMPLETED" :
          "UNKNOWN",
          k_ev_status);
 

      if (k_ev_status == CL_COMPLETE)
        kernel_finished = 1;
      else
        usleep(500000);

      it++;

    } while(!kernel_finished);

    clWaitForEvents(1, &kernel_events[0]);
    clFinish(q);


    for (int j = 0; j < num_kernels; j++) { 
    cl_int f_ret;
      cl_ulong time_start;
      cl_ulong time_end;
      cl_ulong diff;
      // OpenCL kernel time
      f_ret = clGetEventProfilingInfo(kernel_events[j], CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
      if (f_ret != CL_SUCCESS) printf(KRED " Error reading kernel event info PROFILING_COMMAND_START \n" KNRM);
      f_ret = clGetEventProfilingInfo(kernel_events[j], CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
      if (f_ret != CL_SUCCESS) printf(KRED " Error reading kernel event info PROFILING_COMMAND_END \n" KNRM);
      diff = time_end - time_start;
      printf("TIME KERNEL #%d = %lu ns  (%lf ms)\n",j,  diff, ((double)diff/(double)1000000.0));
    }



    #ifdef HLS_DEBUG
    printf("JM10 OPENCL test mode enabled, kernel execution completed\n");
    #endif
    const double krnl_end_time = getCurrentTimestamp(); 
    printf("OPENCL execution completed %d kernel(s) in %0.3f ms -> Total %0.3f ms\n", num_kernels, (krnl_end_time - krnl_start_time) * 1e3, ((krnl_end_time - krnl_start_time) * (float)1e3)*((float)num_kernels));
  }
  #endif
}

//-----------------------------------------------------------------------------
// end of file: test_kernel.cpp
//-----------------------------------------------------------------------------

