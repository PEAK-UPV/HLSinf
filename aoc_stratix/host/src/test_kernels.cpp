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






  ///////////////////////////////////////////////////////////////////
  // modificacion de la convolucion para anyadir nuevos kernels, pero resulta que ha habido redefinicion de tipos, .....
  //
  // estoy muy intranquilo con los cambios ya que ha habido cambios en la convolucion de hls de forma que al copiar
  //  los nuevos cambios  podrían colisionar con mis nombres variables y tipos, ya que ahora utiliza nombres tipos/variables que yo habia creado y antes no existia, y ahora puede que si
  //  con  lo que tendre que ir con cuidado y buscar bien 

//  int PT, int PB, int PL, int PR, int SH, int SW,
//
//  int HO_conv                  = (rows + PT + PB - KH + SH) / SH; // HO = ceil(H + padding - (KH-1) / SH)
//  int WO_conv                  = (W + PL + PR - KW + SW) / SW; // WO = ceil(H + padding - (KW-1) / SW)
//  int read_pixels_add  = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv; 
//
//  cl_uint k_PT = (cl_uint)PT;
//  cl_uint k_PB = (cl_uint)PB;
//  cl_uint k_PL = (cl_uint)PL;
//  cl_uint k_PR = (cl_uint)PR;
//  cl_uint k_SH = (cl_uint)SH;
//  cl_uint k_SW = (cl_uint)SW;
//  cl_uint k_HO_conv = (cl_uint)HO_conv;
//  cl_uint k_WO_conv = (cl_uint)WO_conv;
//  cl_uint k_read_pixels_add = (cl_uint)read_pixels_add ;



  // batch normalization
  //cl_uint k_num_pixels_bn = (cl_uint)read_pixels_add;
  cl_uint k_enable_batch_norm = (cl_uint)enable_batch_norm;
  // -- batch normalization end

  // add data
  //cl_uint k_num_pixels_add = (cl_uint)read_pixels_add;
  cl_uint k_enable_add = (cl_uint)enable_add;
  // -- add data end
  //----------------------------------------------------------




  // fin de cambios 
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





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
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_mem), (void*)&buffer_i));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_M));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_rows));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_global_offset));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_lower_padding));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_upper_padding));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&i_iter));

  // KERNEL_IN
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_KERNEL_IN_READER");
  #endif
  arg_ind = 0;
  //kernel void kernel_in(global kernel_t * kernel, uint offset_factor, uint I_ITER, uint o_iter_first, uint O_ITER){...}
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_mem), (void*)& buffer_k));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_offset_factor));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));

  // BIAS_IN
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_BIAS_IN_READER");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BIAS_IN_READER], arg_ind++, sizeof(cl_mem), (void*)&buffer_bias));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BIAS_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BIAS_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));

  // BATCH_NORM_IN
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_BATCH_NORM_READER");
  #endif
  //kernel void batch_norm_in(global bnp_st_t *restrict b_ptr, uint o_iter_first, uint O_ITER, uint enable_bn)
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM_READER], arg_ind++, sizeof(cl_mem), (void*)&buffer_batch_norm_val));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM_READER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM_READER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_batch_norm));

  // ADD_DATA_IN
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_ADD_DATA_READER");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_mem), (void*)&buffer_i_add));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_rows));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_pooling));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_add));

  // WRITE
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_WRITER");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_mem), (void*)&buffer_o));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_rows));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_pooling));


  // IB - (INPUT BUFFER)
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_INPUT_BUFFER");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_rows));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_lower_padding));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_upper_padding));

  // PAD - PADDING
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_PADDING");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_enable_lower_padding));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_enable_upper_padding));

  // CVT
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_CVT");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_rows));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));

  // MUL - MULTIPLIER
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_MULTIPLIER");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind++, sizeof(cl_uint), (void*)&k_rows));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));

  // ADD - ADDER
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_ADDER");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADDER], arg_ind++, sizeof(cl_uint), (void*)&k_rows));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADDER], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADDER], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADDER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
 
  // RELU
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_RELU");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_RELU], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_RELU], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_RELU], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_RELU], arg_ind++, sizeof(cl_uint), (void*)&k_enable_relu));

  // POOL CVT
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_POOL_CVT");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_enable_maxpooling));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_enable_avgpooling));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));

  // POOL POOLING
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_POOL_POOLING");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_enable_maxpooling));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_enable_avgpooling));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));

  // BATCH_NORM_IN
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_BATCH_NORM");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM], arg_ind++, sizeof(cl_uint), (void*)&k_enable_batch_norm));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM], arg_ind++, sizeof(cl_uint), (void*)&k_enable_maxpooling));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM], arg_ind++, sizeof(cl_uint), (void*)&k_enable_avgpooling));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));

  // ADD_DATA_IN
  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels: set kernel %s arguments\n", "K_ADD_DATA");
  #endif
  arg_ind = 0;
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA], arg_ind++, sizeof(cl_uint), (void*)&k_enable_add));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA], arg_ind++, sizeof(cl_uint), (void*)&k_enable_maxpooling));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA], arg_ind++, sizeof(cl_uint), (void*)&k_enable_avgpooling));
  OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));


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

  // batch_norm_in
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_BATCH_NORM_READER]);
  #endif
  status = clEnqueueNDRangeKernel(queues[K_BATCH_NORM_READER], kernels[K_BATCH_NORM_READER], 1, NULL, &ws, &ls, 0, NULL, &kernel_events[K_BATCH_NORM_READER]);// NULL);
  checkError(status, "Failed to launch batch_norm_in kernel");

  // add_data_in
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_ADD_DATA_READER]);
  #endif
  status = clEnqueueNDRangeKernel(queues[K_ADD_DATA_READER], kernels[K_ADD_DATA_READER], 1, NULL, &ws, &ls, 0, NULL, &kernel_events[K_ADD_DATA_READER]);// NULL);
  checkError(status, "Failed to launch add_data_in kernel");

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
  #ifdef DEBUG_HOST_KERNELS
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


  // pool_cvt
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_BATCH_NORM]);
  #endif
  status = clEnqueueTask(queues[K_BATCH_NORM], kernels[K_BATCH_NORM], 0, NULL, &kernel_events[K_BATCH_NORM]);// NULL);
  checkError(status, "Failed to launch batch normalization task");

  // pool_pooling 
  #ifdef DEBUG_HOST_KERNELS
  printf("                 %s\n", kernel_names[K_ADD_DATA]);
  #endif
  status = clEnqueueTask(queues[K_ADD_DATA], kernels[K_ADD_DATA], 0, NULL, &kernel_events[K_ADD_DATA]);// NULL);
  checkError(status, "Failed to launch add_data task");

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
