/*
 * kernel-related functions for test
 */

#include "test_conv2D.h"

// H HO HO_final W WO WO_final 

// ----------------------------------------------------------------------------
// run kernel
//  padding configuration for multi-frame support :
//    one tensor split in multiple frames
// 
//   rows_fr  number of rows to read in current frame
//   PT_fr    Padding Top    enable sinal for current frame
//   PB_fr    Padding Bottom enable sinal for current frame
//   PL_fr    Padding Left   enable sinal for current frame
//   PR_fr    Padding Right  enable sinal for current frame
//   read_offset_fr_fr  ..... for current frame
//   write_offset_fr    ..... for current frame
// ----------------------------------------------------------------------------
void run_aoc_kernels(int first_frame_H, int last_frame_H, int rows_p, int PT_p, int PB_p, int PL_p, int PR_p, int read_offset_p, int write_offset_p) {
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
  cl_uint k_rows = (cl_uint)rows_p;
  cl_uint k_I    = (cl_uint)I_input;
  cl_uint k_O    = (cl_uint)O_output;
  cl_uint k_i_iter        = (cl_uint)i_iter;
  cl_uint k_o_iter_first  = (cl_uint)o_iter_first;
  cl_uint k_o_iter_last   = (cl_uint)o_iter_last;
  cl_uint k_enable_relu   = (cl_uint)enable_relu;
  //cl_uint k_global_offset = (cl_uint)global_offset;

  // rename paddings
  //cl_uint k_enable_upper_padding = (cl_uint)enable_upper_padding;
  //cl_uint k_enable_lower_padding = (cl_uint)enable_lower_padding;
  // add new paddings
  // add new paddings
  cl_uint k_PT = (cl_uint)PT_p;
  cl_uint k_PB = (cl_uint)PB_p;
  cl_uint k_PL = (cl_uint)PL_p;
  cl_uint k_PR = (cl_uint)PR_p;




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




  //int write_to_weight_buffer = 0;
  //int read_from_weight_buffer = 0;
  //int first_row_weight_buffer = 0;
  //int read_from_mem = 1;
  //int read_from_b0 = 0;
  //int read_from_b1 = 0;
  //int write_to_mem = 1;
  //int write_to_b0 = 0;
  //int write_to_b1 = 0;




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
  cl_uint k_SH = (cl_uint)SH;
  cl_uint k_SW = (cl_uint)SW;
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

  cl_uint k_O_ITER        = o_iter_last - o_iter_first + 1;

  #ifdef DEBUG_HOST_KERNELS
  printf("run_aoc_kernels for 2D convolution\n");
  printf("k_conv2D H %2u   rows %u   W %u   rows %u   I %u   O %u   O_ITER %u   I_ITER %u   o_iter_first %u   o_iter_last %u  "  \
         "   PT %u   PB %u   PL %u   PR %u   enable_relu %u   maxpooling %u   avgpooling %u   enable_clip %u   enable_shift %u " \
         "  enable_bn %u   enable_add %u\n",
              k_H, k_rows, k_W, k_rows, k_I, k_O, o_iter_per_kernel,  k_i_iter, k_o_iter_first, k_o_iter_last,
              k_PT, k_PB, k_PL, k_PR,  k_enable_relu, k_enable_maxpooling, k_enable_avgpooling, k_enable_clipping, k_enable_shift,
              enable_batch_norm, enable_add
              );
  #endif

  //size_t window_size 

  // buffers already allocated and transferred

  cl_uint arg_ind = 0;
  //--------------------------------
  // DATA IN
  // xil_hls -> for o_iter -> read_data_channels_gihwcpi(read_pixels, offset_read_data_channel, I_ITER, offset_data_in_group_cpi, ptr_data, out_read_data, enable_read);
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_DATA_IN_READER");
    #endif

    // prepare kernel arguments 
    int read_pixels = W * rows_p;  // W (global) rows_p (entrada de esta llamada
    int offset_read_data_channel = read_offset_p;
    //I_ITER = i_iter
    int offset_data_in_group_cpi = H * W;
  
    // set ocl kernel arguments
    //(global pixel_in_t* restrict data_in, uint num_pixels_in, uint I_ITER_in, uint O_ITER_in, uint offset_in) {
    cl_uint k_read_pixels = (cl_uint) read_pixels;
    cl_uint k_i_iter = (cl_uint) i_iter;
    //cl_uint k_O_ITER = (cl_uint) O_ITER;
    cl_uint k_offset_read_data_channel = offset_read_data_channel;
    cl_uint k_offset_data_in_group_cpi = (cl_uint) offset_data_in_group_cpi;

    arg_ind = 0;  
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_mem), (void*)&buffer_i));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_read_pixels));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_offset_read_data_channel));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_DATA_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_offset_data_in_group_cpi));
  }

  //--------------------------------
  // KERNEL_IN
  // xil_hls -> for o_iter -> read_kernel(enable_read_kernel, I_ITER, offset_kernel, ptr_kernel, out_read_kernel);
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_KERNEL_IN_READER");
    #endif

    // prepare kernel arguments based on hls for xilinx
    //cl_uint k_offset_factor = ((I_input + CPI - 1) / CPI) * CPI * CPO * 9;
    //cl_uint k_offset_factor = ((I_input + CPI - 1) / CPI) * CPI * CPO;  // since we are reading kernel_t data type on the other side, we do not multiply *9 (kH*kW)
    cl_uint k_offset_factor = ((I_input + CPI - 1) / CPI) ;

    // set ocl kernel arguments
    arg_ind = 0;
    //kernel void kernel_in(global kernel_t * kernel, uint offset_factor, uint I_ITER, uint o_iter_first, uint O_ITER){...}
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_mem), (void*)&buffer_k));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_offset_factor));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_KERNEL_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  }

  //--------------------------------
  // BIAS_IN
  // xil_hls -> for o_iter -> read_bias(offset_bias, ptr_bias, out_read_bias);
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_BIAS_IN_READER");
    #endif

    // prepare kernel arguments based on hls for xilinx
    // no extra agruments to calculate

    // set ocl kernel arguments
    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BIAS_IN_READER], arg_ind++, sizeof(cl_mem), (void*)&buffer_bias));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BIAS_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BIAS_IN_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  }

  //--------------------------------
  // BATCH_NORM_IN
  // xil_hls -> for o_iter -> read_batch_norm(offset_bias, b_ptr, out_read_batch_norm);
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_BATCH_NORM_READER");
    #endif

    // prepare kernel arguments based on hls for xilinx
    // read_batch_norm(offset_bias, b_ptr, out_read_batch_norm);
    // offset_bias = o_iter + o_iter_first; so this value has to be calculated inside the kernel loop
     
    // set ocl kernel arguments
    //kernel void batch_norm_in(global bnp_st_t *restrict b_ptr, uint o_iter_first, uint O_ITER, uint enable_bn)
    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM_READER], arg_ind++, sizeof(cl_mem), (void*)&buffer_batch_norm_val));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM_READER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM_READER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_batch_norm));
  }


    uint enable_pooling           = enable_maxpooling | enable_avgpooling;
    //uint HO_conv                  = (rows_p + PT + PB - KH + SH) / SH;  // rows,PT,PB,SH are kernel input params, KH is a macro
    //uint WO_conv                  = (W + PL + PR - KW + SW) / SW;     // W, PL,PR,SW are kernel input parameters, KW is a macro


    uint HO_conv                  = (rows_p + k_PT + k_PB - KH + SH) / SH;  // rows,PT,PB,SH are kernel input params, KH is a macro
    uint WO_conv                  = (W + k_PL + k_PR - KW + SW) / SW;     // W, PL,PR,SW are kernel input parameters, KW is a macro

    //cl_uint k_enable_pooling   = (cl_uint)enable_pooling;
    cl_uint k_HO_conv          = (cl_uint)HO_conv;
    cl_uint k_WO_conv          = (cl_uint)WO_conv;


  //--------------------------------
  // ADD_DATA_IN
  // xil_hls -> for o_iter -> read_input_add_gihwcpi(read_pixels_add, o_iter_read_add_offset, ptr_data_add, out_read_data_add, enable_add);
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_ADD_DATA_READER");
    #endif

    // prepare kernel arguments based on hls for xilinx
    // read_input_add_gihwcpi(read_pixels_add, o_iter_read_add_offset, ptr_data_add, out_read_data_add, enable_add);
    // read_pixels_add
    //uint enable_pooling           = enable_maxpooling | enable_avgpooling;
    //uint HO_conv                  = (rows + PT + PB - KH + SH) / SH;  // rows,PT,PB,SH are kernel input params, KH is a macro
    //uint WO_conv                  = (W + PL + PR - KW + SW) / SW;     // W, PL,PR,SW are kernel input parameters, KW is a macro
    uint read_pixels_add          = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv;
    
    // o_iter_read_add_offset
    //int o_iter_read_add_offset     = write_offset + (offset_read_add_group_cpo * (o_iter + o_iter_first));  // write_offset, o_iter_first are kernel input paramenters.  o_iter is a kernel internal loop variable
    uint offset_read_add_group_cpo = HO * WO;
    cl_uint k_offset_read_add_group_cpo = (cl_uint)offset_read_add_group_cpo;

    cl_uint k_read_pixels_add = (cl_uint)read_pixels_add;
    cl_uint k_write_offset    = (cl_uint)write_offset_p;

    //k_o_iter_first   already set, it is common to several kernels

    // set ocl kernel arguments
    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_mem), (void*)&buffer_i_add));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_read_pixels_add));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_write_offset));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_offset_read_add_group_cpo));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA_READER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_add));
  }

  //--------------------------------
  // WRITE
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_WRITER");
    #endif

    //int write_pixels_tmp         = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv;
    //int write_pixels             = (enable_upsize)? write_pixels_tmp * 4 : write_pixels_tmp;
    uint write_pixels = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv;

    cl_uint k_write_pixels = (cl_uint)write_pixels;

//    if ((first_frame_H != 0) || (last_frame_H != 0)) {
//      // let's remove one row, so we remove (num_columns -> width) frames
//      printf (KCYN "JM10 debugging force updated num data frames for write kernel from %u  to %u\n" KNRM, 
//          k_write_pixels, k_write_pixels - WO_conv);
//       k_write_pixels = k_write_pixels - WO_conv;
//    }

cl_uint k_write_offset    = (cl_uint)write_offset_p;
 
    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_mem), (void*)&buffer_o));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_write_pixels));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_o_iter_first));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_WRITER], arg_ind++, sizeof(cl_uint), (void*)&k_write_offset));
  }


  //--------------------------------
  // IB - (INPUT BUFFER)
  // xil_hls -> for o_iter -> void input_buffer 
  //
  // THIS KERNEL IS CURRENTLY UNUSED 
  //    since it does not improve performance in opencl, but
  //    has a negative impact in performance(reduces kernel frequency) due to resources usage
  //--------------------------------
  //{
  //  #ifdef DEBUG_HOST_KERNELS
  //    printf("run_aoc_kernels: set kernel %s arguments\n", "K_INPUT_BUFFER");
  //  #endif
  //  arg_ind = 0;
  //  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_H));
  //  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_W));
  //  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_rows));
  //  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
  //  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  //  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_lower_padding));
  //  OCL_CHECK(status, status = clSetKernelArg(kernels[K_INPUT_BUFFER], arg_ind++, sizeof(cl_uint), (void*)&k_enable_upper_padding));
  //}

  //--------------------------------
  // PAD - PADDING
  // xil_hls -> for o_iter -> void padding(int H, int W, int PT, int PB, int PL, int PR, int I_ITER, hls::stream<din_st> &in, hls::stream<din_st> &out) {
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_PADDING");
    #endif
    arg_ind = 0;
    uint padd_H = rows_p;
    uint padd_W = W; // framing capability not implemented for W dimension

    cl_uint k_padd_H = (cl_uint)padd_H;
    cl_uint k_padd_W = (cl_uint)padd_W;

    OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_padd_H));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_padd_W));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_PT));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_PB));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_PL));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_PR));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_PADDING], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  }

  //--------------------------------
  // CVT
  // xil_hls -> for o_iter -> void cvt(int H, int W, int I_ITER, int SH, int SW, hls::stream<din_st> &in, hls::stream<conv_cvt_st> &out)
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_CVT");
    #endif
    cl_uint k_HH = k_rows + k_PT + k_PB;
    cl_uint k_WW = k_W + k_PL + k_PR;

    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_HH));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_WW));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_SH));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_SW));
  }

  //--------------------------------
  // MUL - MULTIPLIER
  // xil_hls -> for o_iter -> void mul(int num_data_frames, int I_ITER, hls::stream<conv_cvt_st> &in, hls::stream<w_st> &k_in, hls::stream<conv_mul_st> &out)
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_MULTIPLIER");
    #endif
    // number of kmul data frames equals the number of pixels of the direct_conv stage (pad+cvt+mul)
    // from hls num_data_frames = num_output_conv_pixels =  HO_conv * WO_conv;
    uint mul_num_data_frames    = HO_conv * WO_conv;

    // in case of multi-frame, the kernel does not have to create the zero-padding,
    // uses instead the real values
    // currently set rows_p are good for that purpose but NOt for mult nor add stages, where these kernels
    // would expect more frames/pixels, 
    //  let's set the rows to the value to not expect the real value (fake-zero-padding)
//    if ((first_frame_H != 0) || (last_frame_H != 0)) {
//      // let's remove one row, so we remove (num_columns -> width) frames
//      printf (KCYN "JM10 debugging force updated num data frames for mul kernel from %u  to %u\n" KNRM, 
//          mul_num_data_frames, mul_num_data_frames - WO_conv);
//       mul_num_data_frames = mul_num_data_frames - WO_conv;
//    }

    cl_uint k_mul_num_data_frames = (cl_uint)mul_num_data_frames;

    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind++, sizeof(cl_uint), (void*)&k_mul_num_data_frames));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_MULTIPLIER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  }

  //--------------------------------
  // ADD - ADDER
  // xil_hls -> for o_iter -> void 
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_ADDER");
    #endif
    uint add_num_data_frames    = HO_conv * WO_conv;

    // in case of multi-frame, the kernel does not have to create the zero-padding,
    // uses instead the real values
    // currently set rows_p are good for that purpose but NOt for mult nor add stages, where these kernels
    // would expect more frames/pixels, 
    //  let's set the rows to the value to not expect the real value (fake-zero-padding)
//    if ((first_frame_H != 0) || (last_frame_H != 0)) {
//      // let's remove one row, so we remove (num_columns -> width) frames
//      printf (KCYN "JM10 debugging force updated num data frames for add kernel from %u  to %u\n" KNRM, 
//          add_num_data_frames, add_num_data_frames - WO_conv);
//       add_num_data_frames = add_num_data_frames - WO_conv;
//    }

    cl_uint k_add_num_data_frames = (cl_uint)add_num_data_frames;
    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADDER], arg_ind++, sizeof(cl_uint), (void*)&k_add_num_data_frames));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADDER], arg_ind++, sizeof(cl_uint), (void*)&k_i_iter));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADDER], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  }

  //--------------------------------
  // RELU
  // xil_hls -> for o_iter -> void 
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_RELU");
    #endif
    
    uint relu_num_pixels = HO_conv * WO_conv;

    // in case of multi-frame, the kernel does not have to create the zero-padding,
    // uses instead the real values
    // currently set rows_p are good for that purpose but NOt for mult nor add stages, where these kernels
    // would expect more frames/pixels, 
    //  let's set the rows to the value to not expect the real value (fake-zero-padding)
//    if ((first_frame_H != 0) || (last_frame_H != 0)) {
//      // let's remove one row, so we remove (num_columns -> width) frames
//      printf (KCYN "JM10 debugging force updated num data frames for relu kernel from %u  to %u\n" KNRM, 
//          relu_num_pixels, relu_num_pixels - WO_conv);
//       relu_num_pixels = relu_num_pixels - WO_conv;
//    }
    cl_uint k_relu_num_pixels = relu_num_pixels;

    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_RELU], arg_ind++, sizeof(cl_uint), (void*)&k_relu_num_pixels));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_RELU], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_RELU], arg_ind++, sizeof(cl_uint), (void*)&k_enable_relu));
  }

  //--------------------------------
  // POOL CVT
  // xil_hls -> for o_iter -> void 
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_POOL_CVT");
    #endif

    cl_uint k_HI_pooling  = HO_conv; // input rows for pooling
    cl_uint k_WI_pooling  = WO_conv; // input cols for pooling

//    if ((first_frame_H != 0) || (last_frame_H != 0)) {
//      // let's remove one row, so we remove (num_columns -> width) frames
//      printf (KCYN "JM10 debugging force updated k_HI_pooling for pool kernels from %u  to %u\n" KNRM, 
//          k_HI_pooling, k_HI_pooling - 1);
//       k_HI_pooling = k_HI_pooling - 1;
//    }

    
    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_HI_pooling));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_WI_pooling));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_enable_maxpooling));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_enable_avgpooling));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_CVT], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  
    // POOL POOLING
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_POOL_POOLING");
    #endif
    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_HI_pooling));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_WI_pooling));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_enable_maxpooling));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_enable_avgpooling));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_POOL_POOLING], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  }

  //--------------------------------
  // BATCH_NORM
  // xil_hls -> for o_iter -> void batch_norm(enable_batch_norm, num_bn_pixels, out_pooling, out_read_batch_norm, out_batch_norm);
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_BATCH_NORM");
    #endif
    //uint enable_pooling = enable_maxpooling || enable_avgpooling;
    uint bn_num_pixels = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv; // pixels to read for add module (before upsize)
    cl_uint k_bn_num_pixels = (cl_uint)bn_num_pixels;

//    if ((first_frame_H != 0) || (last_frame_H != 0)) {
//      // let's remove one row, so we remove (num_columns -> width) frames
//      printf (KCYN "JM10 debugging force updated num_pixles for batch_norm kernel from %u  to %u\n" KNRM, 
//           k_bn_num_pixels, k_bn_num_pixels - WO_conv);
//       k_bn_num_pixels = k_bn_num_pixels - WO_conv;
//    }


    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM], arg_ind++, sizeof(cl_uint), (void*)&k_bn_num_pixels));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM], arg_ind++, sizeof(cl_uint), (void*)&k_enable_batch_norm));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_BATCH_NORM], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  }
  
  //--------------------------------
  // ADD_DATA
  // xil_hls -> for o_iter -> void add_data<dout_st, dout_st, dout_st>(enable_add, num_add_pixels, out_read_data_add, out_batch_norm, out_add)
  //--------------------------------
  {
    #ifdef DEBUG_HOST_KERNELS
    printf("run_aoc_kernels: set kernel %s arguments\n", "K_ADD_DATA");
    #endif
    // number of pixels for add_data kernel is the same as for batch_norm kernel
    //uint enable_pooling = enable_maxpooling || enable_avgpooling;
    uint add_num_pixels = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv; // pixels to read for add module (before upsize)
    cl_uint k_add_num_pixels = (cl_uint)add_num_pixels;

//    if ((first_frame_H != 0) || (last_frame_H != 0)) {
//      // let's remove one row, so we remove (num_columns -> width) frames
//      printf (KCYN "JM10 debugging force updated num_pixles for add_data kernel from %u  to %u\n" KNRM, 
//           k_add_num_pixels, k_add_num_pixels - WO_conv);
//       k_add_num_pixels = k_add_num_pixels - WO_conv;
//    }


    arg_ind = 0;
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA], arg_ind++, sizeof(cl_uint), (void*)&k_add_num_pixels));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA], arg_ind++, sizeof(cl_uint), (void*)&k_enable_add));
    OCL_CHECK(status, status = clSetKernelArg(kernels[K_ADD_DATA], arg_ind++, sizeof(cl_uint), (void*)&k_O_ITER));
  }

//

 //cl_uint num_pixels_in  = H * W;
 //cl_uint num_pixels_out = rows * W;
// cl_uint num_kernels_in = 1; seguro que es uno ?
// cl_uint num_bias_in    = ; a este que valor le toca ?


  //size_t sample_data_in_size   = num_pixels_in;
  //size_t sample_data_in_size   = num_pixels_in * k_O_ITER;
  //size_t sample_data_out_size  = num_pixels_out;
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

  //// ib - input buffer
  //#ifdef DEBUG_HOST_KERNELS
  //printf("                 %s\n", kernel_names[K_INPUT_BUFFER]);
  //#endif
  //clEnqueueTask(queues[K_INPUT_BUFFER], kernels[K_INPUT_BUFFER], 0, NULL, &kernel_events[K_INPUT_BUFFER]);// NULL);
  //checkError(status, "Failed to launch input_buffer task");

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
  printf("\n")
  printf("run_aoc_kernels: waiting for kernels completion\n");
  #endif

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


// ----------------------------------------------------------------------------
// determine frames configuration and trigger kernel execution
//  
// ----------------------------------------------------------------------------
void compute() {

  // Let's check if the input geometry must beWW decomposed in multiple frames
  if (HO > HMAX) {
    //
    int num_frames = ceil( (float) HO / (float)HMAX);

    #ifdef DEBUG_HOST_KERNELS
    printf("HOST compute: Launching multiframes mode (%d frames)...\n", num_frames);
    #endif
    
    // JM10, luego mover esto a un bucle independiente para cada kernel, ya que no debo esperar a que acaben todos 
    // para lanzar la siguiente iteración
    for (int fr=0; fr < num_frames; fr++) {

      #ifdef DEBUG_HOST_KERNELS
      printf("HOST compute: frame %d -> %d out of %d\n", fr, fr+1, num_frames);
      #endif

      int first_frame_H = (fr == 0); // first frame, processing first chunk of rows
      int last_frame_H  = (fr == num_frames - 1);  // last frame,  processing last  chunk of rows

      // first output row for this frame
      int row_o = fr * HMAX;

      // rows to be produced in this frame
      int output_rows_frame = HMAX;
      if ((fr == num_frames-1) && ((HMAX * num_frames) != HO)) output_rows_frame = HO % HMAX;

      // padding
      int PT_frame = (fr==0) ? PT : 0;
      int PB_frame = (fr == num_frames - 1) ? PB : 0;
      int PL_frame = PL;
      int PR_frame = PR;

      // first input row to read for this frame
      // row_i = (row_o * SH) - PT
      // We correct if negative (because of padding)
      //
      int row_i = (row_o * SH) - PT;
      if (row_i < 0) row_i = 0;

      // rows to read for this frame
      int rows_to_read = KH + ((output_rows_frame - 1) * SH) - PT_frame - PB_frame;
      #ifdef DEBUG_HOST_KERNELS
      printf("HOST compute rows_to_read = KH + ((output_rows_frame - 1) * SH) - PT_frame - PB_frame = %d + ((%d - 1) * %d) - %d - %d = %d\n",
          KH ,output_rows_frame, SH, PT_frame, PB_frame, rows_to_read
          );
      #endif

      // read and write offsets
      int read_offset_frame = row_i * W;
      int write_offset_frame = (fr * HMAX * WO);
      
      #ifdef DEBUG_HOST_KERNELS
      printf("HOST compute: Frame %d   HxW = %3dx%3d, rows %d   Padding = PT %1d - PB %1d - PL %1d - PR %1d, off_rd %d, off_wr %d, rows_to_read %d\n",
          fr, H, W, rows_to_read, PT_frame, PB_frame, PL_frame, PR_frame, read_offset_frame, write_offset_frame, rows_to_read);
      #endif
      run_aoc_kernels(first_frame_H, last_frame_H, rows_to_read, PT_frame, PB_frame, PL_frame, PR_frame, read_offset_frame, write_offset_frame);
    }
  } else {
    #ifdef DEBUG_HOST_KERNELS
    printf("HOST compute: Launching single-frame mode (1 frame)...\n");
    #endif
    run_aoc_kernels(0 ,0 ,rows, PT, PB, PL, PR, 0, 0);
  }
}

