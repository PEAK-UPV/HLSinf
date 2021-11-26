/*
 * kernel-related functions for test
 */

#include "test_conv2D.h"



void run_aoc_kernels() {
  cl_int status;
  
  // Legacy num_kernels interpretation
  uint num_kernels;
  uint o_iter_per_kernel;

  num_kernels = 1;
	o_iter_per_kernel = o_iter;
   
  uint k = 0; // (int k=0; k<num_kernels; k++)
  uint o_iter_first = o_iter_per_kernel * k;
  uint o_iter_last  = o_iter_first + o_iter_per_kernel - 1;
  // --------------------------------

  cl_uint k_H    = (cl_uint)H;
  cl_uint k_W    = (cl_uint)W;
  cl_uint k_rows = (cl_uint)rows;
  cl_uint k_I    = (cl_uint)I_input;
  cl_uint k_O    = (cl_uint)O_output;
  cl_uint k_i_iter        = (cl_uint)i_iter;
  cl_uint k_o_iter_first  = (cl_uint)o_iter_first;
  cl_uint k_o_iter_last   = (cl_uint)o_iter_last;
  cl_uint k_enable_relu   = (cl_uint)enable_relu;
  cl_uint k_global_offset = (cl_uint)global_offset;
  cl_uint k_enable_upper_padding = (cl_uint)enable_upper_padding;
  cl_uint k_enable_lower_padding = (cl_uint)enable_lower_padding;
  cl_uint k_enable_maxpooling    = (cl_uint)enable_maxpooling;
  cl_uint k_enable_avgpooling    = (cl_uint)enable_avgpooling;
  cl_uint k_enable_clipping = (cl_uint)enable_clipping;
  cl_uint k_enable_shift    = (cl_uint)enable_shift;
  cl_uint k_min_clip  = (cl_uint)min_clip;
  cl_uint k_max_clip  = (cl_uint)max_clip;
  cl_uint k_dir_shift = (cl_uint)dir_shift;
  cl_uint k_pos_shift = (cl_uint)pos_shift;

  cl_uint k_M = k_H * k_W * k_I;
  cl_uint k_N = k_H * k_W * k_O;


  cl_uint k_enable_pooling = (k_enable_maxpooling != 0) || (k_enable_avgpooling != 0);
  //----------------------------------------------------------

  //cl_uint k_offset_factor = ((I_input + CPI - 1) / CPI) * CPI * CPO * 9;
  //cl_uint k_offset_factor = ((I_input + CPI - 1) / CPI) * CPI * CPO;  // since we are reading kernel_t data type on the other side, we do not multiply *9 (kH*kW)
  cl_uint k_offset_factor = ((I_input + CPI - 1) / CPI) ;

  cl_uint k_O_ITER        = o_iter_last - o_iter_first + 1;

  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels for 2D convolution\n");
  printf("k_conv2D H %d   W %d   rows %d   I %d   O %d   I_ITER %d   o_iter_first %d   o_iter_last %d   enable_relu %d   global_offset %d"  \
         "   upper_padding %d   lower_padding %d, maxpooling %d   avgpooling %d   enable clip %d   enable shift %d \n",
              k_H, k_W, k_rows, k_I, k_O, k_i_iter, k_o_iter_first, k_o_iter_last, k_enable_relu, k_global_offset,
              k_enable_upper_padding, k_enable_lower_padding, k_enable_maxpooling, k_enable_avgpooling, k_enable_clipping, k_enable_shift 
              );
  #endif

  //size_t window_size 

  // buffers already allocated and transferred

  cl_uint arg_ind = 0;
  //--------------------------------
  // let's set the kernels arguments
  // DATA IN
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_DATA_IN_READER");
  #endif
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_mem), (void*)&buffer_i);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_M);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_H);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_rows);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_global_offset);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_enable_lower_padding);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_enable_upper_padding);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind, sizeof(cl_uint), (void*)&i_iter);
  checkError(status, "Failed to set data read arg %u\n", arg_ind);
  arg_ind++;

  // KERNEL_IN
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_KERNEL_IN_READER");
  #endif
  arg_ind = 0;
  //kernel void kernel_in(global kernel_t * kernel, uint offset_factor, uint I_ITER, uint o_iter_first, uint O_ITER){...}
  status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind, sizeof(cl_mem), (void*)& buffer_k);
  checkError(status, "Failed to set kernel_read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_offset_factor);
  checkError(status, "Failed to set kernel_read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_i_iter);
  checkError(status, "Failed to set kernel_read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_o_iter_first);
  checkError(status, "Failed to set kernel_read arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set kernel_read arg %u\n", arg_ind);
  arg_ind++;

  // BIAS_IN
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_BIAS_IN_READER");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_BIAS_IN_READER], arg_ind, sizeof(cl_mem), (void*)&buffer_bias);
  checkError(status, "Failed to set bias in kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_BIAS_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_o_iter_first);
  checkError(status, "Failed to set bias in kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_BIAS_IN_READER], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set bias in kernel arg %u\n", arg_ind);
  arg_ind++;

  // WRITE
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_WRITER");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_WRITER], arg_ind, sizeof(cl_mem), (void*)&buffer_o);
  checkError(status, "Failed to set kernel_write arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_WRITER], arg_ind, sizeof(cl_uint), (void*)&k_H);
  checkError(status, "Failed to set kernel_write arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_WRITER], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set kernel_write arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_WRITER], arg_ind, sizeof(cl_uint), (void*)&k_rows);
  checkError(status, "Failed to set kernel_write arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_WRITER], arg_ind, sizeof(cl_uint), (void*)&k_o_iter_first);
  checkError(status, "Failed to set kernel_write arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_WRITER], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set kernel_write arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_WRITER], arg_ind, sizeof(cl_uint), (void*)&k_enable_pooling);
  checkError(status, "Failed to set kernel_write arg %u\n", arg_ind);
  arg_ind++;


  // IB - (INPUT BUFFER)
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_INPUT_BUFFER");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind, sizeof(cl_uint), (void*)&k_H);
  checkError(status, "Failed to set input buffer kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set input buffer kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind, sizeof(cl_uint), (void*)&k_rows);
  checkError(status, "Failed to set input buffer kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind, sizeof(cl_uint), (void*)&k_i_iter);
  checkError(status, "Failed to set input buffer kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set input buffer kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind, sizeof(cl_uint), (void*)&k_enable_lower_padding);
  checkError(status, "Failed to set input buffer kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind, sizeof(cl_uint), (void*)&k_enable_upper_padding);
  checkError(status, "Failed to set input buffer kernel arg %u\n", arg_ind);
  arg_ind++;

  // PAD - PADDING
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_PADDING");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_PADDING], arg_ind, sizeof(cl_uint), (void*)&k_H);
  checkError(status, "Failed to set padding kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_PADDING], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set padding kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_PADDING], arg_ind, sizeof(cl_uint), (void*)&k_i_iter);
  checkError(status, "Failed to set padding kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_PADDING], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set padding kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_PADDING], arg_ind, sizeof(cl_uint), (void*)&k_enable_lower_padding);
  checkError(status, "Failed to set padding kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_PADDING], arg_ind, sizeof(cl_uint), (void*)&k_enable_upper_padding);
  checkError(status, "Failed to set padding kernel arg %u\n", arg_ind);
  arg_ind++;

  // CVT
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_CVT");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_CVT], arg_ind, sizeof(cl_uint), (void*)&k_rows);
  checkError(status, "Failed to set cvt kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_CVT], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set cvt kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_CVT], arg_ind, sizeof(cl_uint), (void*)&k_i_iter);
  checkError(status, "Failed to set cvt kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_CVT], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set cvt kernel arg %u\n", arg_ind);
  arg_ind++;

  // MUL - MULTIPLIER
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_MULTIPLIER");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind, sizeof(cl_uint), (void*)&k_rows);
  checkError(status, "Failed to set mult kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set mult kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind, sizeof(cl_uint), (void*)&k_i_iter);
  checkError(status, "Failed to set mult kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set mult kernel arg %u\n", arg_ind);
  arg_ind++;

  // ADD - ADDER
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_ADDER");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_ADDER], arg_ind, sizeof(cl_uint), (void*)&k_rows);
  checkError(status, "Failed to set add kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_ADDER], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set add kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_ADDER], arg_ind, sizeof(cl_uint), (void*)&k_i_iter);
  checkError(status, "Failed to set add kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_ADDER], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set add kernel arg %u\n", arg_ind);
  arg_ind++;
 
  // RELU
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_RELU");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_RELU], arg_ind, sizeof(cl_uint), (void*)&k_H);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_RELU], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_RELU], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_RELU], arg_ind, sizeof(cl_uint), (void*)&k_enable_relu);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;

  // POOL CVT
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_POOL_CVT");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind, sizeof(cl_uint), (void*)&k_H);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind, sizeof(cl_uint), (void*)&k_enable_maxpooling);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind, sizeof(cl_uint), (void*)&k_enable_avgpooling);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;

  // POOL POOLING
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_POOL_POOLING");
  #endif
  arg_ind = 0;
  status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind, sizeof(cl_uint), (void*)&k_H);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind, sizeof(cl_uint), (void*)&k_W);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind, sizeof(cl_uint), (void*)&k_enable_maxpooling);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind, sizeof(cl_uint), (void*)&k_enable_avgpooling);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;
  status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind, sizeof(cl_uint), (void*)&k_O_ITER);
  checkError(status, "Failed to set relu kernel arg %u\n", arg_ind);
  arg_ind++;

//

 cl_uint num_pixels_in  = H * W;
 cl_uint num_pixels_out = rows * W;
// cl_uint num_kernels_in = 1; seguro que es uno ?
// cl_uint num_bias_in    = ; a este que valor le toca ?


  //size_t sample_data_in_size   = num_pixels_in;
  //size_t sample_data_in_size   = num_pixels_in * k_O_ITER;
  size_t sample_data_out_size  = num_pixels_out;
//  size_t sample_kernel_in_size = num_kernels_in;
//  size_t sample_bias_in_size   = num_bias_in;

  // clEnqueueNDRangeKernel(q, kernel_conv2D[k], 1, NULL, &ws, &ls, 0, NULL, &kernel_events[k]);
  // cl_int clEnqueueNDRangeKernel(
  //        cl_command_queue command_queue,
  //        cl_kernel kernel,
  //        cl_uint work_dim,
  //        const size_t* global_work_offset,
  //        const size_t* global_work_size,
  //        const size_t* local_work_size,
  //        cl_uint num_events_in_wait_list,
  //        const cl_event* event_wait_list,
  //        cl_event* event
  //       );
  // clEnqueueNDRangeKernel( 
  //   command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event);
  //clEnqueueNDRangeKernel(
  //
  //   q,                        kernel_conv2D[k]         , 1, NULL, &ws                 , &ls , 0, NULL, &kernel_events[k]);
  //   queues[K_DATA_IN_READER], kernels[K_DATA_IN_READER], 1, NULL, &sample_data_in_size, NULL, 0, NULL, NULL             );         
  //--------------------------------
  size_t ws = 1;
  size_t ls = 1;
  // Let's trigger kernels execution
  //double time = getCurrentTimestamp();

  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: Start kernels\n");
  #endif
  // read data_in
  //status = clEnqueueNDRangeKernel(queues[K_DATA_IN_READER], kernels[K_DATA_IN_READER], 1, NULL, &sample_data_in_size, NULL, 0, NULL, NULL);
  #ifdef DEBUG_HOST_KERNELS 
  printf("                 %s\n", kernel_names[K_DATA_IN_READER]);
  #endif
  status = clEnqueueNDRangeKernel(queues[K_DATA_IN_READER], kernels[K_DATA_IN_READER], 1, NULL, &ws, &ls, 0, NULL, &kernel_events[K_DATA_IN_READER]);//NULL);
  checkError(status, "Failed to launch data_in kernel");
  // read kernel_in
  //status = clEnqueueNDRangeKernel(queues[K_KERNEL_IN_READER], kernels[K_KERNEL_IN_READER], 1, NULL, &sample_kernel_in_size, NULL, 0, NULL, NULL);
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_KERNEL_IN_READER]);
  #endif
  status = clEnqueueNDRangeKernel(queues[K_KERNEL_IN_READER], kernels[K_KERNEL_IN_READER], 1, NULL, &ws, &ls, 0, NULL, &kernel_events[K_KERNEL_IN_READER]);// NULL);
  checkError(status, "Failed to launch kernel_in kernel");
  // read bias_in
  //status = clEnqueueNDRangeKernel(queues[K_BIAS_IN_READER], kernels[K_KERNEL_IN_READER], 1, NULL, &sample_bias_in_size, NULL, 0, NULL, NULL);
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_BIAS_IN_READER]);
  #endif
  status = clEnqueueNDRangeKernel(queues[K_BIAS_IN_READER], kernels[K_BIAS_IN_READER], 1, NULL, &ws, &ls, 0, NULL, &kernel_events[K_BIAS_IN_READER]);// NULL);
  checkError(status, "Failed to launch bias_in kernel");

  // ib - input buffer
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_INPUT_BUFFER]);
  #endif
  clEnqueueTask(queues[K_INPUT_BUFFER], kernels[K_INPUT_BUFFER], 0, NULL, &kernel_events[K_INPUT_BUFFER]);// NULL);
  checkError(status, "Failed to launch input_buffer task");

  // padding
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_PADDING]);
  #endif
  status = clEnqueueTask(queues[K_PADDING], kernels[K_PADDING], 0, NULL, &kernel_events[K_PADDING]);// NULL);
  checkError(status, "Failed to launch padding task");

  // cvt
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_CVT]);
  #endif
  status = clEnqueueTask(queues[K_CVT], kernels[K_CVT], 0, NULL, &kernel_events[K_CVT]);// NULL);
  checkError(status, "Failed to launch cvt task");

  // mul
  #ifdef DEBUG_HOSkernel_eventsT_KERNELS
  printf("                 %s\n", kernel_names[K_MULTIPLIER]);
  #endif
  status = clEnqueueTask(queues[K_MULTIPLIER], kernels[K_MULTIPLIER], 0, NULL, &kernel_events[K_MULTIPLIER]);// NULL);
  checkError(status, "Failed to launch mul task");

  //add
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_ADDER]);
  #endif
  status = clEnqueueTask(queues[K_ADDER], kernels[K_ADDER], 0, NULL, &kernel_events[K_ADDER]);// NULL);
  checkError(status, "Failed to launch add task");

  // relu
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_RELU]);
  #endif
  status = clEnqueueTask(queues[K_RELU], kernels[K_RELU], 0, NULL, &kernel_events[K_RELU]);// NULL);
  checkError(status, "Failed to launch relu task");

  // pool_cvt
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_POOL_CVT]);
  #endif
  status = clEnqueueTask(queues[K_POOL_CVT], kernels[K_POOL_CVT], 0, NULL, &kernel_events[K_POOL_CVT]);// NULL);
  checkError(status, "Failed to launch pool_cvt task");

  // pool_pooling 
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_POOL_POOLING]);
  #endif
  status = clEnqueueTask(queues[K_POOL_POOLING], kernels[K_POOL_POOLING], 0, NULL, &kernel_events[K_POOL_POOLING]);// NULL);
  checkError(status, "Failed to launch pool_pooling task");

  // write
  //status = clEnqueueNDRangeKernel(queues[K_WRITER], kernels[K_WRITER], 1, NULL,  &sample_data_out_size, NULL, 0, NULL, NULL);
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_WRITER]);
  #endif
  status = clEnqueueNDRangeKernel(queues[K_WRITER], kernels[K_WRITER], 1, NULL,  &ws, &ls, 0, NULL, &kernel_events[K_WRITER]);// NULL);
  checkError(status, "Failed to launch kernel_write");

  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: all kernels launched\n");
  #endif
  printf("run_aoc_kernels: waiting for kernels completion\n");

  // Wait for command queue to complete pending events
  for(int i=0; i<K_NUM_KERNELS; ++i) {
    status = clFinish(queues[i]);
    checkError(status, "Failed to finish (%d: %s)", i, kernel_names[i]);
  }

  // getCurrentTimestamp is a helper function (common) that returns time in seconds
  // Record execution time
  //time = getCurrentTimestamp() - time;

  //kernels_execution_time = time * 1E6; // value in us
 
  //int num_pixels = H*W;
  //double gpixels_per_sec = ((double)(num_pixels / time) * 1E-9);
  //printf("\tThroughput = %.4f Gpx / sec\n", gpixels_per_sec);

  //printf("\tProcessing time = %.4f ms\n", (float)(time * 1E3));

  // OpenCL kernel time
  //printf("JM10 kernel profiler timings summary\n");
  cl_int f_ret;
  cl_ulong ts_first;
  cl_ulong te_last;
  cl_ulong ts_writer;
  cl_ulong te_writer;
  cl_ulong diff_writer;
  cl_ulong diff_kernels;
  f_ret = clGetEventProfilingInfo(kernel_events[0], CL_PROFILING_COMMAND_START, sizeof(ts_first), &ts_first, NULL);
  if (f_ret != CL_SUCCESS) printf(KRED " Error reading kernel event info PROFILING_COMMAND_START first kernel \n" KNRM);
  f_ret = clGetEventProfilingInfo(kernel_events[K_NUM_KERNELS-1], CL_PROFILING_COMMAND_END, sizeof(te_last), &te_last, NULL);
  if (f_ret != CL_SUCCESS) printf(KRED " Error reading kernel event info PROFILING_COMMAND_END last kernel\n" KNRM);
  
  f_ret = clGetEventProfilingInfo(kernel_events[K_WRITER], CL_PROFILING_COMMAND_START, sizeof(ts_writer), &ts_writer, NULL);
  if (f_ret != CL_SUCCESS) printf(KRED " Error reading kernel event info PROFILING_COMMAND_START kernel writer\n" KNRM);
  f_ret = clGetEventProfilingInfo(kernel_events[K_WRITER], CL_PROFILING_COMMAND_END, sizeof(te_writer), &te_writer, NULL);
  if (f_ret != CL_SUCCESS) printf(KRED " Error reading kernel event info PROFILING_COMMAND_END kernel writer\n" KNRM);
  
  // profiling info is returned in ns
  diff_writer  = te_writer - ts_writer;
  diff_kernels = te_last   - ts_first;

  #ifdef DEBUG_HOST_KERNELS
  #ifdef DEBUG_VERBOSE
  printf("PROFILE EVENT - TIME      WRITER KERNEL = %lu ns  (%lf ms)\n",  diff_writer,((double)diff_writer/(double)1000000.0));
  printf("PROFILE EVENT - TIME LAST -FIRST KERNEL = %lu ns  (%lf ms)\n",  diff_kernels,((double)diff_kernels/(double)1000000.0));
  #endif
  #endif


  // update kernels execution time
  kernels_execution_time = diff_kernels / 1000.0; // value from ns to us
  #ifdef DEBUG_HOST_KERNELS
  //printf("test_kernels: update kernels execution time with event profiling time, %lf ns to %lu ns \n",  kernels_execution_time, diff_kernels);
    printf("\tProcessing time = %.4f ms\n", (float)(kernels_execution_time/1000.0));
  #endif


  //--------------------------------
  //

}
