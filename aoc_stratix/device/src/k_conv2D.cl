// --------------------------------------------------------------------------------------------------------------
// HLSinf kernels
// Version: 1.1
// copyright (c) 2020, Universidad Politècnica de València (UPV), GAP research group
// Date: March 2021
// Authors: GAP Research Group (UPV)
//     José Flich Cardo
//     Jorge García Martínez
//     Izan Catalán Gallarch
//     Carles Hernández Luz
//     Jose Maria Martinez Martinez
//
// contact: jflich@disca.upv.es
// All rights reserved
//
//
// OpenCL version of kconv2D kernels
//
//  Data formats:
//
//  - weights (kernel)   : GO x GI x CPO x CPI x KH x KW
//  - bias               : O
//  - data_in            : I x H x W
//  - data_out           : O x H x W
//
//
// The kernel must have at least CPI channels and CPO channels, filled with zeroes if needed
//
// This file contains multiples kernels for 2D convolution
// This file is designed and tested for Altera Stratix 10 MX board
// This kernel works in two modes
//   FPGA emulation
//   FPGA deployement
// These file has been tested under Quartus Pro v19.3
// --------------------------------------------------------------------------------------------------------------


//*********************************************************************************************************************
// 
//  Included Files
// 
//*********************************************************************************************************************
#include "ihc_apint.h"
#include "conv2D_commons.h"

//*********************************************************************************************************************
// 
//  ....
// 
//*********************************************************************************************************************

#pragma OPENCL EXTENSION cl_intel_channels : enable

//*********************************************************************************************************************
// 
//  Private definitions
// 
//*********************************************************************************************************************
// Following struct is also defined in conv2D_commons.h
struct frame_pool_st {
  pixel_out_t pixel[KW_POOLING * KH_POOLING];
}__attribute__((packed));
typedef struct frame_pool_st frame_pool_t;

// Global variables
// Channel declarations
channel pixel_in_t    CH_DATA_IN          __attribute__((depth(STREAMS_DEPTH)));
channel kernel_t      CH_KERNEL_IN        __attribute__((depth(STREAMS_DEPTH)));
channel pixel_out_t   CH_BIAS_IN          __attribute__((depth(STREAMS_DEPTH)));
channel bnp_st_t      CH_BN_IN            __attribute__((depth(STREAMS_DEPTH)));
//channel add_data_st_t CH_ADD_DATA_IN      __attribute__((depth(STREAMS_DEPTH)));
channel pixel_out_t   CH_ADD_DATA_IN      __attribute__((depth(STREAMS_DEPTH)));

//channel pixel_in_t   CH_IB_TO_PAD        __attribute__((depth(STREAMS_DEPTH)));
channel pixel_in_t   CH_PAD_TO_CVT       __attribute__((depth(STREAMS_DEPTH)));
channel frame_t      CH_CVT_TO_MUL       __attribute__((depth(STREAMS_DEPTH)));
channel pixel_out_t  CH_MUL_TO_ADD       __attribute__((depth(STREAMS_DEPTH)));
channel pixel_out_t  CH_ADD_TO_RELU      __attribute__((depth(STREAMS_DEPTH)));
channel pixel_out_t  CH_RELU_TO_POOLCVT        __attribute__((depth(STREAMS_DEPTH)));
channel frame_pool_t CH_POOLCVT_TO_POOLPOOL    __attribute__((depth(STREAMS_DEPTH)));
channel pixel_out_t  CH_POOLPOOL_TO_BATCHNORM  __attribute__((depth(STREAMS_DEPTH)));
channel pixel_out_t  CH_BATCHNORM_TO_ADDDATA   __attribute__((depth(STREAMS_DEPTH)));

channel pixel_out_t  CH_DATA_OUT          __attribute__((depth(STREAMS_DEPTH)));

//*********************************************************************************************************************
// 
//  Macro definitions
// 
//*********************************************************************************************************************

// ----------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------
//#define CH_IB_IN         CH_DATA_IN
//#define CH_IB_OUT        CH_IB_TO_PAD

//#define CH_PADD_IN       CH_IB_TO_PAD
#define CH_PADD_IN       CH_DATA_IN
#define CH_PADD_OUT      CH_PAD_TO_CVT

#define CH_CVT_IN        CH_PAD_TO_CVT
#define CH_CVT_OUT       CH_CVT_TO_MUL

#define CH_MUL_KERNEL_IN CH_KERNEL_IN
#define CH_MUL_IN        CH_CVT_TO_MUL
#define CH_MUL_OUT       CH_MUL_TO_ADD

#define CH_ADD_BIAS_IN   CH_BIAS_IN
#define CH_ADD_IN        CH_MUL_TO_ADD
#define CH_ADD_OUT       CH_ADD_TO_RELU

#define CH_RELU_IN       CH_ADD_TO_RELU
#define CH_RELU_OUT      CH_RELU_TO_POOLCVT

#define CH_POOL_CVT_IN   CH_RELU_TO_POOLCVT
#define CH_POOL_CVT_OUT  CH_POOLCVT_TO_POOLPOOL

#define CH_POOL_POOL_IN  CH_POOLCVT_TO_POOLPOOL
#define CH_POOL_POOL_OUT CH_POOLPOOL_TO_BATCHNORM

#define CH_BATCHNORM_BN_IN   CH_BN_IN
#define CH_BATCHNORM_IN      CH_POOLPOOL_TO_BATCHNORM
#define CH_BATCHNORM_OUT     CH_BATCHNORM_TO_ADDDATA

#define CH_ADDDATA_ADD_IN   CH_ADD_DATA_IN
#define CH_ADDDATA_IN       CH_BATCHNORM_TO_ADDDATA
#define CH_ADDDATA_OUT      CH_DATA_OUT


// ----------------------------------------------------------------------------
// Debug
// ----------------------------------------------------------------------------

//#define DEBUG_VERBOSE

//*********************************************************************************************************************
// 
//  Included Files
// 
//*********************************************************************************************************************


// ----------------------------------------------------------------------------
// Data IN kernel
//  read data from memory and write it to input stream
// ----------------------------------------------------------------------------
kernel void data_in(global pixel_in_t* restrict data_in, uint num_pixels_in, uint I_ITER_in, uint O_ITER_in, uint offset_in, uint cpi_group_offset_in) {

  uint num_pixels  = num_pixels_in;
  uint I_ITER      = I_ITER_in;
  uint O_ITER      = O_ITER_in;
  uint offset      = offset_in;
  uint cpi_group_offset  = cpi_group_offset_in;

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA_IN: gihwcpi format: start num_pixels(per_iter) %2u  i_iter %2u   o_iter %2u   offset %6u\n", num_pixels, I_ITER, O_ITER, offset);  
  #endif 
   
  for (uint o_iter = 0; o_iter < O_ITER; o_iter++) {    
    for (int i_iter = 0; i_iter < I_ITER; i_iter++) {
      int offset_global = offset + (cpi_group_offset * i_iter);

      #ifdef DEBUG_READ_DATA
      #ifdef DEBUG_VERBOSE
      printf("READ_DATA_IN: o_iter %2u   i_iter %2u   offset %6u\n", o_iter, i_iter , offset);
      #endif
      #endif
  
      for (uint i = 0; i < num_pixels; i++) {
     
        pixel_in_t data = data_in[offset + i];
      
        #ifdef DEBUG_READ_DATA
          printf("  READ_DATA_IN  o_iter %u   i_iter %u   ind %3u ", o_iter, i_iter, i);
          #ifdef DEBUG_VERBOSE
            printf("   data: ");
            for(uint j = 0; j < CPI; j++) {
              printf(" %2.2f", data.pixel[j]);
            }
          #endif
          printf("\n");
        #endif
    
        #ifdef DEBUG_READ_DATA
        #ifdef DEBUG_VERY_VERBOSE
        printf("READ_DATA_IN: waiting to write to CH_DATA_IN\n");  
        #endif
        #endif
        write_channel_intel(CH_DATA_IN, data);
        #ifdef DEBUG_READ_DATA
        #ifdef DEBUG_VERY_VERBOSE
        printf("READ_DATA_IN: wrote data to CH_DATA_IN\n");  
        #endif
        #endif
      }
    }
  }
  #ifdef DEBUG_READ_DATA
  printf("READ_DATA_IN gihwcpi: end\n");
  #endif
}


// ----------------------------------------------------------------------------
// Filter kernel data read
//
// kernels are stored in memory with the format GO x GI x CPO x CPI x KH x KW
// This storage formats lets the module to read memory sequentially and send all the
// kernels in the same order they are read through the output stream.
// kernels are sent in frame structures (3x3 grid)
//
// ----------------------------------------------------------------------------
kernel void kernel_in(global kernel_t * restrict krnl, uint offset_factor, uint I_ITER, uint o_iter_first, uint O_ITER){

  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL_IN: start  O_ITER %u   o_iter_first %u   I_ITER %u\n", O_ITER, o_iter_first, I_ITER);
  #endif

  for (uint o_iter = 0; o_iter<O_ITER; o_iter++) {
    uint iter_offset = (o_iter + o_iter_first);
    uint      offset = iter_offset * offset_factor;
    
    for (uint i_iter  = 0 ; i_iter < I_ITER; i_iter++) {
      kernel_t k = krnl[offset + i_iter ];
      write_channel_intel(CH_KERNEL_IN,k);

      #ifdef DEBUG_READ_KERNEL
        #ifdef DEBUG_VERBOSE
          for (uint cpo=0; cpo<CPO; cpo++) {
            for (uint cpi=0; cpi<CPI; cpi++) {
              printf("READ_KERNEL_IN: o_iter %u  i_iter %u offset %2u   cpo=%d cpi=%d : ", o_iter, i_iter, offset, cpo, cpi);
              for (uint p=0;p<9; p++) printf(" %8.2f ", (float)k.pixel[cpo][cpi][p]);
              printf("\n");
            }
          }
        #endif
      #endif
    }
  }
  
  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL_IN: end\n");
  #endif
}

// ----------------------------------------------------------------------------
// Data IN bias kernel
// ----------------------------------------------------------------------------
kernel void bias_in(global pixel_out_t * restrict bias, uint o_iter_first, uint O_ITER){

  #ifdef DEBUG_READ_BIAS
  printf("READ_BIAS: start  O_ITER %u   o_iter_first %u\n", O_ITER, o_iter_first);
  #endif

  for(uint o_iter = 0; o_iter < O_ITER; o_iter++) {
    uint    iter_offset = o_iter_first + o_iter;
    pixel_out_t    px_b = bias[iter_offset];

    write_channel_intel(CH_BIAS_IN, px_b);
    #ifdef DEBUG_READ_BIAS
    #ifdef DEBUG_VERBOSE
      printf("READ_BIAS: offset_bias = %d\n", iter_offset);
      printf("READ_BIAS: bias = ");
      for (uint c=0; c<CPO; c++) printf(" %f", (float)px_b.pixel[c]);
      printf("\n");
    #endif
    #endif
  }

  #ifdef DEBUG_READ_BIAS
  printf("READ_BIAS: end\n");
  #endif
}

// ----------------------------------------------------------------------------
// Data IN Batch Normalization kernel
// ----------------------------------------------------------------------------
//read_batch_norm(offset_bias, b_ptr, out_read_batch_norm);
kernel void batch_norm_in(global bnp_st_t *restrict b_ptr, uint o_iter_first, uint O_ITER, uint enable_batch_norm) {
  uint enable_bn = enable_batch_norm;
  
  #ifdef DEBUG_READ_BATCH_NORM
  printf("BATCH NORM IN: start enable_bn %u  O_ITER %u   o_iter_first %u\n", enable_bn, O_ITER, o_iter_first);
  #endif

  if (enable_bn) {
    for(uint o_iter = 0; o_iter < O_ITER; o_iter++) {
      uint    iter_offset  = o_iter_first + o_iter;
      bnp_st_t batch_norm  = b_ptr[iter_offset];

      write_channel_intel(CH_BN_IN, batch_norm);

      #ifdef DEBUG_READ_BATCH_NORM
      printf("DEBUG_READ_BATCH_NORM: value = ");
      for (int c=0; c<CPO*4; c++) {
        printf(" %f ", (float)batch_norm.values[c]);
      }
      printf("\n");
      #endif
    }
  }

  #ifdef DEBUG_READ_BATCH_NORM
  printf("BATCH NORM IN: end\n");
  #endif
}

// ----------------------------------------------------------------------------
// Data IN Add kernel
// ----------------------------------------------------------------------------

//kernel void add_data_in(global pixel_out_t* restrict data_in, uint H, uint W, uint rows, uint o_iter_first, uint O_ITER,  uint enable_pooling, uint enable_add_data) 
kernel void add_data_in(global pixel_out_t* restrict data_in, uint num_pixels_in, uint write_offset_in, uint offset_read_add_group_cpo_in, uint o_iter_first_in, uint O_ITER_in, uint enable_add_data_in){
  
  uint enable_add      = enable_add_data_in;
  uint num_pixels      = num_pixels_in;
  uint write_offset    = write_offset_in;
  uint offset_read_add_group_cpo = offset_read_add_group_cpo_in;
  uint o_iter_first    = o_iter_first_in;
  uint O_ITER          = O_ITER_in;

  
  #ifdef DEBUG_READ_ADD_DATA
  printf("ADD_DATA_READER: start (gihwcpi data format) enable_add %u   num_pixels %u   write_offset %u   offset_read_add_group_cpo %u   o_iter_first %u   O_ITER %u\n",
      enable_add, num_pixels, write_offset, offset_read_add_group_cpo, o_iter_first, O_ITER
      );
  #endif


  if (enable_add) {
    for (uint o_iter = 0; o_iter < O_ITER; o_iter++) {
      //uint o_iter_read_add_offset = num_pixels * (o_iter + o_iter_first);
      uint o_iter_read_add_offset =   write_offset + (offset_read_add_group_cpo * (o_iter + o_iter_first));
      #ifdef DEBUG_READ_ADD_DATA
        printf("ADD_DATA_READER: o_iter %u   num_pixels %u  o_iter_write_offset %u\n", o_iter, num_pixels, o_iter_read_add_offset);
      #endif
      for(uint i = 0; i < num_pixels; i++) {
        pixel_out_t px = data_in[o_iter_read_add_offset + i];
        write_channel_intel(CH_ADDDATA_ADD_IN, px);
        #ifdef DEBUG_READ_ADD_DATA
          #ifdef DEBUG_VERBOSE
            printf("ADD_DATA_READER:  o_iter %u   index %u   px: ", o_iter, i);
            for(uint i = 0; i < CPO; i++) {
              printf(" %2.2f", px.pixel[i]);
            }
            printf("\n");
          #endif
        #endif
      }
    }
  }
  #ifdef DEBUG_READ_ADD_DATA
  printf("ADD_DATA_READER: end \n");
  #endif

}


// ----------------------------------------------------------------------------
// Data  OUT kernel
// ----------------------------------------------------------------------------
kernel void data_out(global pixel_out_t * restrict data_out, uint write_pixels_in, uint o_iter_first_in, uint O_ITER_in, uint write_offset_frame_in) {

  uint write_pixels = write_pixels_in;
  uint o_iter_first = o_iter_first_in;
  uint O_ITER       = O_ITER_in;
  uint write_offset_frame = write_offset_frame_in; // offset to data ptr due to splitting matrix in frames

  #ifdef DEBUG_WRITE_DATA
  printf("WRITER: start (gihwcpi data format) num_pixels %u   o_iter_first %u   O_ITER %u  glbl_frames_offset %u\n",
      write_pixels, o_iter_first, O_ITER, write_offset_frame
      );
  #endif

  for (uint o_iter = 0; o_iter < O_ITER; o_iter++) {
    uint offset_tmp1 = (o_iter + o_iter_first);
    uint offset_tmp2 = write_pixels * offset_tmp1;
    uint o_iter_write_offset = write_offset_frame_in + offset_tmp2;
    
    #ifdef DEBUG_WRITE_DATA
      printf("WRITER: o_iter %u   num_pixels %u  o_iter_write_offset %u\n", o_iter, write_pixels, o_iter_write_offset);
    #endif
    for(uint i = 0; i < write_pixels; i++) {
      #ifdef DEBUG_WRITE_DATA
      #ifdef DEBUG_VERY_VERBOSE
      printf("WRITER: waiting to read from CH_DATA_OUT\n");
      #endif
      #endif
      pixel_out_t px = read_channel_intel(CH_DATA_OUT);
      #ifdef DEBUG_WRITE_DATA
      #ifdef DEBUG_VERY_VERBOSE
      printf("WRITER: data read  from CH_DATA_OUT\n");
      #endif
      #endif
      data_out[o_iter_write_offset + i] = px;
      #ifdef DEBUG_WRITE_DATA
        //#ifdef DEBUG_VERBOSE
          printf("WRITE_DATA_OUT  o_iter %u   index %u   px: ", o_iter, i);
          for(uint i = 0; i < CPO; i++) {
            printf(" %2.2f", px.pixel[i]);
          }
          printf("\n");
       // #endif
      #endif
    }
  }
  #ifdef DEBUG_WRITE_DATA
  printf("WRITER: end \n");
  #endif

}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// datapath kernels  -- called as tasks
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Kernel input_buffer disables
//  dummy kernel bypassing input pixles
//  currently disabled to minimize resource usage
// ----------------------------------------------------------------------------
/*
kernel void input_buffer ( uint num_pixels_in, uint I_ITER_in, uint O_ITER_in) {
  #ifdef DEBUG_INPUT_BUFFER
  printf ("INPUT_BUFFER: start\n");
  #endif

  #ifdef DEBUG_INPUT_BUFFER
  printf ("INPUT_BUFFER: ALWAYS reading pixels from memory:  buffer not implemented yet\n");
  #endif

  // under development
  // input data buffering is not currently supported
  // data is read from memory for all iterations

  uint num_pixels = num_pixels_in;
  uint I_ITER     = I_ITER_in;
  uint O_ITER     = O_ITER_in;

  for (uint o_iter = 0; o_iter < O_ITER; o_iter++) {    
    for (int i_iter = 0; i_iter < I_ITER; i_iter++) {
      for (uint i = 0; i < read_pixels_total; i++) {
        pixel_in_t px = read_channel_intel(CH_IB_IN);
        #ifdef DEBUG_INPUT_BUFFER
        #ifdef DEBUG_VERBOSE
        uint i_it = i / read_pixels_i_it;
        uint o_it = i / read_pixels_o_it;
        uint ind = i % read_pixels_i_it;
        uint ch_low = i_it * CPI;
        uint ch_hi  = (i_it + 1) * CPI - 1;
        printf("INPUT_BUFFER: o_it: %2u   i_it: %2u   px %2u [%2u, %2u]: ", o_it, i_it, ind, ch_low, ch_hi);
        for (uint p = 0; p < CPI; p++) printf("  %8.2f", px.pixel[p]);
        printf("\n");
        #endif
        write_channel_intel(CH_IB_OUT, px);
      }
    }
  }
  #ifdef DEBUG_INPUT_BUFFER
  printf ("INPUT_BUFFER: end\n");
  #endif
}
*/

// ----------------------------------------------------------------------------
// padding. Adds padding to the input and forwards it through the output
//
// Arguments:
//   H                 : Height of input channel
//   W                 : Width of input channel
//   i_iter            : Number of input iterations (I / CPI)
//   enable_lower_padding  : 
//   enable_uppder_padding : 
//// --------------------------------------------------------------------------
kernel void padding(uint H_in, uint W_in, uint PT_in, uint PB_in, uint PL_in, uint PR_in, uint I_ITER_in, uint O_ITER_in){

  #ifdef DEBUG_PADDING
  printf("PADDING: start  H %2u   W %2u   PT %2u   PB %2u   PL %2u   PR %2u   I_ITER %2u   O_ITER %2u\n", H_in, W_in, PT_in, PB_in, PL_in, PR_in, I_ITER_in, O_ITER_in);
  #endif


  uint O_ITER = O_ITER_in;
  uint I_ITER = I_ITER_in;
  uint H = H_in;
  uint W = W_in;
  uint PT = PT_in;
  uint PB = PB_in;
  uint PL = PL_in;
  uint PR = PR_in;

  pixel_in_t px_data;
  pixel_in_t px_zero;
  
  uint HH = H + PT + PB;
  uint WW = W + PL + PR;

  padding_cpi_loop:
  #pragma unroll CPI
  for (uint cpi=0; cpi<CPI; cpi++){
    px_zero.pixel[cpi] = 0.f;
  }

  //uint val_e1 = ;
  uint val_e2 = H+PT;
  //uint val_e3 = ;
  uint val_e4 = W+PL;

  #ifdef DEBUG_PADDING
    //#ifdef DEBUG_VERBOSE
      uint num_pixels_per_i_iter = (H + PT + PB) * (W + PL + PR);
      printf("PADDING:  O_ITER %2u   I_ITER %2u   HH %2u   WW %2u   pixels per i_iter %4u \n",
          O_ITER, I_ITER, HH, WW, HH*WW 
          );
    //#endif
  #endif

  #ifdef DEBUG_PADDING
  uint px_index = 0;
  #endif

  for (uint o_iter = 0; o_iter < O_ITER; o_iter++) {
    padding_loop:
    #pragma loop_coalesce
    for (uint i = 0; i < I_ITER; i++) {

      #ifdef DEBUG_PADDING
        //#ifdef DEBUG_VERBOSE
          printf("PADDING: o_iter %u   i_iter %2u of %2u \n", o_iter, i, I_ITER);
        //#endif
      #endif


      for (uint h = 0; h < HH ;h++) {
        for (uint w = 0; w < WW; w++) {



          //uint enable1 = enable_upper_padding & (h==0);
          //uint enable2 = enable_lower_padding & (h == H+1);
          //uint enable3 = (w == 0);
          //uint enable4 = (w == W+1);
          uint enable1 = (h < PT);
          uint enable2 = (h >= val_e2); //(h >= H+PT);
          uint enable3 = (w < PL);
          uint enable4 = (w >= val_e4); //(w >= W+PL);
          


          if (enable1 | enable2 | enable3 | enable4) {
            px_data = px_zero; 
          } else {
            #ifdef DEBUG_PADDING
            #ifdef DEBUG_VERY_VERBOSE
            printf("PADDING:  waiting to read from CH_PADD_OUT\n");
            #endif
            #endif
            px_data = read_channel_intel(CH_PADD_IN);
            #ifdef DEBUG_PADDING
            #ifdef DEBUG_VERY_VERBOSE
            printf("PADDING: data read from CH_PADD_OUT\n");
            #endif
            #endif
          }
    
          #ifdef DEBUG_PADDING
          printf("PADDING: o_it: %2u   i_it: %2u   h: %2u  w: %2u  px_index %2u  padding %u\n", o_iter, i, h, w, px_index, (enable1 | enable2 | enable3 | enable4));        
          //#ifdef DEBUG_VERBOSE
          //printf("PADDING: o_it: %2u   i_it: %2u   h: %2u  w: %2u  px_index %2u  padding %u  px: ", o_iter, i, h, w, px_index, (enable1 | enable2 | enable3 | enable4));
          //for (uint p = 0; p < CPI; p++) printf("  %8.2f",(float)px_data.pixel[p]);
          // printf("\n");
          //#endif
          #endif
      
          write_channel_intel(CH_PADD_OUT, px_data);

          #ifdef DEBUG_PADDING
          px_index++;
          #endif
        }
      }
    }
  }
  #ifdef DEBUG_PADDING
  printf("PADDING: end\n");
  #endif
}

// ----------------------------------------------------------------------------
// cvt: reads an input stream with an image of format (H, W, CPI) and writes an output stream
// in a 2D format based on (KW, KH). (SW=1, SH=1) stride is assumed and (PW=1, PH=1) padding is assumed.
// The function outputs data in the format (KH, KW, CPI).
//
// Arguments:
//   H      : Height of input channel
//   W      : Width of input channel
//   I_ITER : Number of input iterations (I / CPI)
//   in     : input stream (format pixel_in_t)
//   out    : output stream (format frame_t)
// ----------------------------------------------------------------------------
kernel void cvt(uint HH_in, uint WW_in, uint I_ITER_in, uint O_ITER_in, uint SH_in, uint SW_in){

  #ifdef DEBUG_CVT
  printf("CVT: start\n");

  uint dbg_cnt = 0;
  #endif

  uint HH = HH_in;
  uint WW = WW_in;
  uint I_ITER = I_ITER_in;
  uint O_ITER = O_ITER_in;
  uint SH = SH_in;
  uint SW = SW_in;

  // buffers (keep three rows)
  pixel_in_t buffer0[WMAX+2];
  pixel_in_t buffer1[WMAX+2];
  pixel_in_t buffer2[WMAX+2];

  #ifdef DEBUG_CVT
  //#ifdef DEBUG_VERBOSE
  printf("CVT:  HH %2u   WW %2u   I_ITER %2u   O_ITER %2u   SH %2u   SW %2u\n", HH, WW, I_ITER, O_ITER, SH, SW);
  //#endif
  #endif


  // Automatically flattened loop (for the purposes of getting the expected pipelined design) 
  for (uint o_iter = 0; o_iter < O_ITER; o_iter++){
    pixel_in_t p0, p1, p2, p3, p4, p5, p6, p7, p8;

    cvt_loop:
    #pragma loop_coalesce
    for(uint i_it = 0; i_it < I_ITER; i_it++){
      uint row0_buffer_write = 0;
      uint row1_buffer_write = 0;
      uint row2_buffer_write = 0;
	  
      uint stride_row = 2;
	    uint stride_col = 2;
      
      for(uint pin_row = 0; pin_row < HH; pin_row++) {  // rows iteration, 
        if (row0_buffer_write) {
          row0_buffer_write = 0;
          row1_buffer_write = 1;
          row2_buffer_write = 0;
        } else if (row1_buffer_write) {
          row0_buffer_write = 0;
          row1_buffer_write = 0;
          row2_buffer_write = 1;
        } else {
          row0_buffer_write = 1;
          row1_buffer_write = 0;
          row2_buffer_write = 0;
        }

        for(uint pin_col = 0; pin_col < WW; pin_col++) { // column
  
          pixel_in_t pb0, pb1, pb2;
          pixel_in_t pixel_b0, pixel_b1, pixel_b2;
  
          uint pin_col0 = (pin_col==0);
          uint pin_col1 = (pin_col==1);
      
          // get the pixel
          pixel_in_t pixel;
          #ifdef DEBUG_CVT
          #ifdef DEBUG_VERY_VERBOSE
          printf("CVT: waiting read from CH_CVT_IN\n");
          #endif
          #endif
          pixel = read_channel_intel(CH_CVT_IN);
          #ifdef DEBUG_CVT
          #ifdef DEBUG_VERY_VERBOSE
          printf("CVT: data read from CH_CVT_IN\n");
          #endif
          #endif

          #ifdef DEBUG_CVT
          #ifdef DEBUG_VERBOSE
          printf("CVT: read data (o_it %2u   i_it %2u    pin_row %d pin_col %d stride_row %d stride_col %d):",o_iter, i_it, pin_row, pin_col, stride_row, stride_col);
          for (int x=0; x<CPI; x++) printf(" %f", (float)pixel.pixel[x]);
          printf("\n");
          #endif
          #endif
      
          // row buffer write (in which buffer row we write the pixel)
          // first row buffer
          uint row0 = (pin_row <= 2) | ((pin_row % 3) == 2);
          uint row1 = !row0 & ((pin_row % 3) == 0);
      
          // from  aocl-best-practices-guide-19-2.pdf
          //   Ensure that the offline compiler does not assume false dependencies
          //   When the static memory dependence analysis fails to prove that dependency does
          //   not exist, the offline compiler assumes that a dependency exists and modifies the
          //   kernel execution to enforce the dependency. Impact of the dependency
          //   enforcement is lower if the memory system is stall-free.
          //     — Write after read operations with data dependency on a load-store unit can
          //       take just two clock cycles (II=2). Other stall-free scenarios can take up to
          //       seven clock cycles.
          //     — Read after write (control dependency) operation can be fully resolved by the
          //       offline compiler
       
          // we write the pixel into the buffer
          if (row0_buffer_write) buffer0[pin_col] = pixel;
          if (row1_buffer_write) buffer1[pin_col] = pixel;
          if (row2_buffer_write) buffer2[pin_col] = pixel;
           
          pixel_b0 = buffer0[pin_col];
          pixel_b1 = buffer1[pin_col];
          pixel_b2 = buffer2[pin_col];
  
          uint shift_frame = (pin_row>1) & (pin_col > 2);
          uint send_frame_pin    = (pin_row>1) & (pin_col > 1);
          uint send_frame_stride = (stride_row == SH) & (stride_col == SW);
          uint send_frame = send_frame_pin & send_frame_stride;

           if (stride_col == 1) stride_col = SW; else stride_col = stride_col - 1;
         
          // p0, p1, p2
          if (shift_frame) {p0 = p1;} else if (pin_col0) {if (row0) p0 = pixel_b0; else if (row1) p0 = pixel_b1; else p0 = pixel_b2;}
          if (shift_frame) {p1 = p2;} else if (pin_col1) {if (row0) p1 = pixel_b0; else if (row1) p1 = pixel_b1; else p1 = pixel_b2;}
          if (row0) p2 = pixel_b0; else if (row1) p2 = pixel_b1; else p2 = pixel_b2;
          // p3, p4, p5
          if (shift_frame) {p3 = p4;} else if (pin_col0) {if (row0) p3 = pixel_b1; else if (row1) p3 = pixel_b2; else p3 = pixel_b0;}
          if (shift_frame) {p4 = p5;} else if (pin_col1) {if (row0) p4 = pixel_b1; else if (row1) p4 = pixel_b2; else p4 = pixel_b0;}
          if (row0) p5 = pixel_b1; else if (row1) p5 = pixel_b2; else p5 = pixel_b0;
          // p6, p7, p8
          if (shift_frame) {p6 = p7;} else if (pin_col0) {if (row0) p6 = pixel_b2; else if (row1) p6 = pixel_b0; else p6 = pixel_b1;}
          if (shift_frame) {p7 = p8;} else if (pin_col1) {if (row0) p7 = pixel_b2; else if (row1) p7 = pixel_b0; else p7 = pixel_b1;}
          if (row0) p8 = pixel_b2; else if (row1) p8 = pixel_b0; else p8 = pixel_b1;
          if (send_frame) {
            frame_t frame;
            frame.pixel[0] = p0; frame.pixel[1] = p1; frame.pixel[2] = p2;
            frame.pixel[3] = p3; frame.pixel[4] = p4; frame.pixel[5] = p5;
            frame.pixel[6] = p6; frame.pixel[7] = p7; frame.pixel[8] = p8;
            #ifdef DEBUG_CVT
            //#ifdef DEBUG_VERY_VERBOSE
            printf("CVT: waiting to write to CH_CVT_IN\n");
            //#endif
            #endif
            write_channel_intel(CH_CVT_OUT, frame);
            #ifdef DEBUG_CVT
            //#ifdef DEBUG_VERY_VERBOSE
            printf("CVT: wrote data to CH_CVT_IN\n");
            //#endif
            #endif


            #ifdef DEBUG_CVT
            uint num_pixels = HH * WW;
            uint p = (pin_row * WW) + pin_col;
            uint fr_ind = i_it * num_pixels + p;
  
            uint ch_low = i_it * CPI;
            uint ch_hi  = (i_it + 1) * CPI - 1;
            printf("CVT: o_it: %2u   i_it: %2u   FRAME %2u (%2u)   frames sent %2u   [%2u, %2u]    TO-BE-DELETED JM10 DEBUGGING\n"
                , o_iter, i_it, p, fr_ind, dbg_cnt++, ch_low, ch_hi);
            #ifdef DEBUG_VERBOSE
            //uint i_it = i_iter / ((H + 2) * (W + 2));
            //uint px   = i_iter % ((H + 2) * (W + 2));
            uint num_pixels = HH * WW;
            uint p = (pin_row * WW) + pin_col;
            uint fr_ind = i_it * num_pixels + p;
  
            uint ch_low = i_it * CPI;
            uint ch_hi  = (i_it + 1) * CPI - 1;
            printf("CVT: o_it: %2u   i_it: %2u   FRAME %2u (%2u)  [%2u, %2u]: \n", o_iter, i_it, p, fr_ind, ch_low, ch_hi);
            //printf("CVT: frame sent:\n");
            for (uint cpi=0; cpi<CPI; cpi++) {
              printf("  cpi %d (ch %2u):\n", cpi, ch_low + cpi);
              printf("    %6.4f %6.4f %6.4f\n", (float)frame.pixel[0].pixel[cpi], (float)frame.pixel[1].pixel[cpi], (float)frame.pixel[2].pixel[cpi]);
              printf("    %6.4f %6.4f %6.4f\n", (float)frame.pixel[3].pixel[cpi], (float)frame.pixel[4].pixel[cpi], (float)frame.pixel[5].pixel[cpi]);
              printf("    %6.4f %6.4f %6.4f\n", (float)frame.pixel[6].pixel[cpi], (float)frame.pixel[7].pixel[cpi], (float)frame.pixel[8].pixel[cpi]);
            }
            #endif
            #endif
          }
        } // pin_col ++
        stride_col = 2;
    	  if (stride_row == 1) stride_row = SH; else stride_row = stride_row - 1;
      } // pin_row ++
    } //i_iter
  }// o_iter

  #ifdef DEBUG_CVT
  printf("CVT: end\n");
  #endif
}

// ----------------------------------------------------------------------------
// mul: This function performs the application of the kernel filter to the frames
//   
// Arguments:
//   num_data_frames   : 
//   I_ITER            : Number of input iterations (I / CPI)
//   O_ITER            : Number of output iterations 
// ----------------------------------------------------------------------------
kernel void mul (uint num_data_frames_in, uint i_iter_in, uint O_ITER_in) {

  uint i_iter  = i_iter_in;
  uint O_ITER = O_ITER_in;
  uint num_data_frames  = num_data_frames_in;

  #ifdef DEBUG_MUL
  printf("MUL: start   data_frames %4u   i_iter %2u   o_iter %2u\n",num_data_frames, i_iter, O_ITER);
  #endif

  for (uint o_iter = 0; o_iter < O_ITER; o_iter++){

    kernel_t    kernel_frame;  // kernel_t    -> data_type pixel[CPO][CPI][9];
    kernel_in_t k;             // kernel_in_t -> data_type pixel[9];
    frame_t     data_in;       // frame_t     -> pixel_in_t pixel[9];
    data_type   sum[CPO];
    pixel_out_t p_out;
  
    mul_loop_i_iter:
    for (uint i_it = 0; i_it < i_iter; i_it++) {
      kernel_frame = read_channel_intel(CH_KERNEL_IN); // kernel_t -> pixel[CPO][CPI][9];
      

      #ifdef DEBUG_MUL
        #ifdef DEBUG_VERBOSE
          printf("MUL: kernel read  o_iter %2u  i_iter %2u\n", o_iter, i_it);
          for(int i=0; i<CPI; i++){
            for(int o=0; o<CPO; o++){
              printf("kernel  o_iter %2u  i_iter %2u  cpi=%d cpo=%d\n", o_iter, i_it , i, o);
              for (int p=0; p<9; p++){
                printf(" %f ", (float)kernel_frame.pixel[o][i][p]);
                if((p+1)%3==0)printf("\n");
              }
              printf("\n");
            }
          }
          #endif
        #endif 
 
      for (uint j_ext = 0; j_ext < num_data_frames; j_ext++) {

        mul_loop_2:
        #pragma unroll CPO
        for(int i=0; i<CPO; i++){
          sum[i] = (data_type)0;
        }
  
        #ifdef DEBUG_MUL
        #ifdef DEBUG_VERY_VERBOSE
        printf("MUL: waiting read from CH_MUL_IN\n");
        #endif
        #endif      
        data_in = read_channel_intel(CH_MUL_IN); // frame_t from cvt stage-> 9 pixels (pixel_in_t)
        #ifdef DEBUG_MUL
        #ifdef DEBUG_VERY_VERBOSE
        printf("MUL: data read from CH_MUL_IN\n");
        #endif
        #endif


        loop_mul_cpi:
        #pragma unroll CPI
        for (int cpi=0; cpi<CPI; cpi++) {
          loop_mul_j:
          #pragma unroll
          for (int j=0; j<KW*KH; j++) {
            loop_mul_cpo:
            #pragma unroll CPO
            for (int cpo=0; cpo<CPO; cpo++) {
              sum[cpo] += data_in.pixel[j].pixel[cpi] * kernel_frame.pixel[cpo][cpi][j];
              //#ifdef DEBUG_MUL
              //  #ifdef DEBUG_VERBOSE
              //    printf("MUL PARTIAL c(i_it) %2u  channel out %2u   j_ext(0:h*w) %2u   j(0:kh*kw) %2u    sum[%u] =  %8.2f\n",
              //        i_it, cpo, j_ext, j,cpo, sum[cpo]
              //        );
              //  #endif
              //  #endif
            }
          }
        }
  
        #pragma unroll
        for(int i=0; i<CPO; i++){
          p_out.pixel[i] = sum[i];
        }
        #ifdef DEBUG_MUL
          #ifdef DEBUG_VERBOSE
          printf("MUL: o_iter %2u  i_iter %2u  p_out.pixel[%d]", o_iter, i_it, j_ext);
          for(int i = 0;i<CPO;i++) {
            printf("  %8.2f  ", (float)p_out.pixel[i]);
          }
          printf("\n");
          #endif
        #endif

        #ifdef DEBUG_MUL
        #ifdef DEBUG_VERY_VERBOSE
        printf("MUL: waiting to write data to CH_MUL_OUT\n");
        #endif
        #endif
        write_channel_intel(CH_MUL_OUT, p_out);
        #ifdef DEBUG_MUL
        #ifdef DEBUG_VERY_VERBOSE
        printf("MUL: wrote data to CH_MUL_OUT\n");
        #endif
        #endif

      }
    } // i_it
  } // o_iter

  #ifdef DEBUG_MUL
  printf("MUL: end\n");
  #endif
}

// ----------------------------------------------------------------------------
// add: This function performs the addition of all subpixels for the same channel.
// It adds also the corresponding bias.
//
// Arguments:
//   num_data_frames   : 
//   I_ITER            : Number of input iterations (I / CPI)
//   O_ITER            : Number of output iterations 
// ----------------------------------------------------------------------------
kernel void add (uint num_data_frames_in, uint i_iter_in, uint O_ITER_in) {
 
  // Buffer for all data and CPO channels
  pixel_out_t buff_o_channels[(WMAX)*(HMAX)];
  // number of iterations by CPI || CPO channels
  uint num_data_frames = num_data_frames_in;
  uint i_iter  = i_iter_in;
  uint O_ITER  = O_ITER_in;
  
  #ifdef DEBUG_ADD
  printf("ADD: start  data_frames %4u   i_iter %2u   o_iter %2u\n", num_data_frames, i_iter, O_ITER);
  #endif


  add_o_iter_loop:
  for(uint o_iter = 0; o_iter < O_ITER; o_iter++) {
    #ifdef DEBUG_ADD
      printf("ADD: o_iter %u \n", o_iter);
    #endif
    pixel_out_t bias;
  
    // read bias
    #ifdef DEBUG_ADD
    #ifdef DEBUG_VERY_VERBOSE
    printf("ADD: waiting read from CH_BIAS_IN\n");
    #endif
    #endif
    bias = read_channel_intel(CH_BIAS_IN);
    #ifdef DEBUG_ADD
    #ifdef DEBUG_VERY_VERBOSE
    printf("ADD: data read from CH_BIAS_IN\n");
    #endif
    #endif
    #ifdef DEBUG_ADD
    #ifdef DEBUG_VERBOSE
      printf("ADD:  bias ");
      for (uint b=0; b<CPO; b++) {
        printf("%6.4f ",(float)bias.pixel[b]);
      }
      printf("\n");
  
      for(uint cpo = 0; cpo<CPO; cpo++){
        printf("Channel cpo = %u: ", (unsigned)(cpo + (CPO* o_iter)));
        for(uint f = 0; f < num_data_frames; f++){
          printf("%6.2f ", (float)buff_o_channels[f].pixel[cpo]);
        }
        printf("\n");
      }
      #endif
    #endif
   
    // JM10, no entiendo porque está hecho el bucle de esta forma y no en modo pipeline
    // leo y hago la primera iteración
    // entro en bucle
    // salgo del bucle y escribo en el canal
    //.....rehacerlo?  al hacerlo segun he pensado creo queno haría falta el buff_o_channels_it 
  
    // All input data have effect into output add
    add_i_iter_loop:
    for (uint i_it = 0; i_it < i_iter; i_it++){
      pixel_out_t data_out;
      //#pragma HLS loop_flatten off
      add_load_data_it_loop:
      for(uint f = 0; f < num_data_frames; f++){
        pixel_out_t px;
        pixel_out_t data_in;
        pixel_out_t data_out;
       
        #ifdef DEBUG_ADD
        #ifdef DEBUG_VERY_VERBOSE
        printf("ADD: waiting read from CH_ADD_IN\n");
        #endif
        #endif
        px = read_channel_intel(CH_ADD_IN);
        #ifdef DEBUG_ADD
        #ifdef DEBUG_VERY_VERBOSE
        printf("ADD: data read from CH_ADD_IN\n");
        #endif
        #endif
  
        if (i_it == 0) data_in = bias; else data_in = buff_o_channels[f];
  
        // data_in  = bias o calculos anteriores
        // px       = datos del mmul
        #ifdef DEBUG_ADD
          #ifdef DEBUG_VERBOSE
            printf("ADD:  o_iter %2u  i_iter %2u  frame %2u [0, HxW[\n", (unsigned)o_iter, i_it, f );
            printf("ADD:  px (from mul stage) :  ");
            for (uint c = 0; c < CPO; c++) printf ("  %8.2f", (float)px.pixel[c]);
            printf("\n");
          #endif
        #endif

        add_load_data_cpo_loop:
        #pragma unroll CPO
        for (uint cpo=0; cpo<CPO; cpo++) {
          data_out.pixel[cpo] = data_in.pixel[cpo] + px.pixel[cpo];
        }

        buff_o_channels[f] = data_out;

        if(i_it ==(i_iter-1)){
          #ifdef DEBUG_ADD
          #ifdef DEBUG_VERBOSE
          printf("ADD:  writting adder out channel for frame %2u - channels %2u to %2u",f,  (unsigned)(o_iter*CPO), (unsigned)((o_iter*CPO) + CPO - 1 ));
          for(uint c = 0; c < CPO; c++) printf("  %8.2f", data_out.pixel[c]);
          printf ("\n");
          #endif
          #endif
      
          #ifdef DEBUG_ADD
          #ifdef DEBUG_VERY_VERBOSE
          printf("RELU: waiting to write data to CH_ADD_OUT\n");
          #endif
          #endif
          write_channel_intel(CH_ADD_OUT, data_out);
          #ifdef DEBUG_ADD
          #ifdef DEBUG_VERY_VERBOSE
          printf("RELU: wrote data to CH_ADD_OUT\n");
          #endif
          #endif

        }
      }
    } //i_iter
  
    #ifdef DEBUG_ADD
      #ifdef DEBUG_VERBOSE
      for (uint cpo=0; cpo<CPO; cpo++) {
        printf("CH %u: ", cpo + (CPO* o_iter));
        for (uint f = 0; f < num_data_frames; f++) {
          printf("%6.2f ", (float)buff_o_channels[f].pixel[cpo]);
        }
        printf("\n");
      }
      #endif
    #endif

  } // o_iter
  #ifdef DEBUG_ADD
  printf("ADD: end\n");
  #endif

}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
kernel void relu(uint num_pixels_in, uint O_ITER_in, uint enable_relu_in) {
  uint num_pixels = num_pixels_in;
  uint O_ITER = O_ITER_in;
  uint enable_relu = enable_relu_in;

  #ifdef DEBUG_RELU
  printf("RELU: start\n");
  //#ifdef DEBUG_VERBOSE
  printf("RELU:   num_pixels %2u   O_ITER %2u   enable_relu %u\n", num_pixels, O_ITER, enable_relu);
  //#endif
  #endif

  for(uint o_iter = 0; o_iter < O_ITER; o_iter++) {
    #ifdef DEBUG_RELU
      printf("RELU: o_iter %u\n", o_iter);
    #endif
    for (uint i = 0; i < num_pixels; i++) {
      pixel_out_t data_in;
      pixel_out_t data_out;

      #ifdef DEBUG_RELU
      #ifdef DEBUG_VERY_VERBOSE
      printf("RELU: waiting read from CH_RELU_IN\n");
      #endif
      #endif
      data_in = read_channel_intel(CH_RELU_IN);
      #ifdef DEBUG_RELU
      #ifdef DEBUG_VERY_VERBOSE
      printf("RELU: data read from CH_RELU_IN\n");
      #endif
      #endif
      
      loop_relu_cpo:
      #pragma unroll
      for(uint cpo = 0; cpo<CPO; cpo++){
        data_type v_in, v_shift, v_clipping, v_relu;
        v_in = data_in.pixel[cpo];
        // shift
        #ifdef USE_SHIFT
          v_shift = v_in;
          if (enable_shift) {
            if (direction_shift == LEFT_DIRECTION) v_shift = v_in << pos_shift;
            if (direction_shift == RIGHT_DIRECTION) v_shift = v_in >> pos_shift;
          }
        #else
          v_shift = v_in;
        #endif
  
        // clipping
        #ifdef USE_CLIPPING
          v_clipping = v_shift;
          if (enable_clipping) {
            if (v_shift < min_clip) {
              v_clipping = min_clip;
            } else if (v_shift > max_clip) {
              v_clipping = max_clip;
            }
          }
        #else
          v_clipping = v_shift;
        #endif
        
        // relu
        #ifdef USE_RELU
          v_relu = v_clipping;
          if(enable_relu && (v_relu < 0)) v_relu = 0;
        #else
          v_relu = v_clipping;
        #endif
        data_out.pixel[cpo] = v_relu;
  
      }
  
      #ifdef DEBUG_RELU
        #ifdef DEBUG_VERBOSE
          printf("RELU (pixel %d):\n", i);
          for (uint x=0; x<CPI; x++) {
            printf("  cpi %d : in %f out %f\n", x, (float)data_in.pixel[x], (float)data_out.pixel[x]);
          }
        #endif
      #endif
 
      #ifdef DEBUG_RELU
      #ifdef DEBUG_VERY_VERBOSE
      printf("RELU: waiting to write data to CH_RELU_OUT\n");
      #endif
      #endif
      write_channel_intel(CH_RELU_OUT, data_out);
      #ifdef DEBUG_RELU
      #ifdef DEBUG_VERY_VERBOSE
      printf("RELU: wrote data to CH_RELU_OUT\n");
      #endif
      #endif

    }
  }
  #ifdef DEBUG_RELU
  printf("RELU: end\n");
  #endif

}

// ----------------------------------------------------------------------------
// Convert module for the pooling layer
// Only works for fixed size of (KW=2, KH=2) and (SW=2, SH=2).
// 
// The output data format is (KH, KW, CPI)
// 
// Parameters:
//   H            : Height of the input image
//   W           : Width of the input image
//   enable_pooling : Activates pooling
//   in           : Input stream (pixel_out_t)
//  out          : Output stream (frame_pool_t)
//
// In case enable_pooling is not set, the module bypasses the input
//  to the output, sending the pixel on the first position of the output frame.
// ----------------------------------------------------------------------------
kernel void pool_cvt(uint H, uint W, uint enable_max_pooling, uint enable_avg_pooling, uint O_ITER) {
  pixel_out_t  buffer0[WMAX];
  pixel_out_t  buffer1[WMAX];
  uint          row0;
  uint          shift_frame;
  uint          send_frame;
  uint          odd_col = 0;       // whether we are in an odd col (so to be able to send a frame)
  uint          pin_row = 0;
  uint          pin_col = 0;
  uint          row_write;      // either 0 or 1 (we assume 2x2 kernel)
  uint          size_channel = H * W;
  uint          iterations = size_channel;
  // pixel_out_t  pixel; 
  pixel_out_t  p0, p1, p2, p3;
  pixel_out_t  pix_b0, pix_b1;
  pixel_out_t  poz; // pixel_out_zero -> output pixel set to zeros
  
  uint          kpcpo = KW_POOLING * KH_POOLING;
  uint enable_pooling =  enable_max_pooling || enable_avg_pooling;

  #pragma unroll
  for (uint i = 0; i < CPO; i++) {
    poz.pixel[i] = (data_type)0;
  }

  #ifdef DEBUG_POOL
  printf("POOL_CVT: start   H %u   W %u   ena_max_pool %u   ena_avg_pool %u  O_ITER %u\n", 
      H, W, enable_max_pooling, enable_avg_pooling, O_ITER
      );
  #endif

  pool_cvt_o_iter_loop:
  for(uint o_iter = 0; o_iter < O_ITER; o_iter++) {
    #ifdef DEBUG_POOL 
      printf("POOL_CVT: o_iter %u\n", o_iter);
    #endif
    pool_cvt_loop:
    #pragma loop_coalesce
    for(uint pin_row = 0; pin_row < H; pin_row++) {
      for (uint pin_col = 0; pin_col < W; pin_col++) {
        // read pixel from channel
        pixel_out_t pixel = read_channel_intel(CH_POOL_CVT_IN);
        
        #ifdef DEBUG_POOL
          #ifdef DEBUG_VERBOSE
            printf("DEBUG_POOL: pixel read: ");
            for (int x=0; x<CPO; x++) printf(" %f ", (float)pixel.pixel[x]);
            printf("\n");
          #endif
        #endif
    
        // allways perform all operations as done in the pool_pooling kernel
        //    then decide at the final stage whether to send the frame or not yet
        row_write = pin_row % 2;
        // Let's compute shift and send flag variables
        shift_frame = (pin_row > 0) & (pin_col > 1);
        send_frame = (row_write & odd_col) | !(enable_pooling);
        row0 = (row_write == 0);
    
        // Let's write on the buffer and at the same time
        // we set the two pixels pix_b0 and pix_b1
        if (row_write==0) {
          buffer0[pin_col] = pixel;
          pix_b0 = pixel;
          pix_b1 = buffer1[pin_col];
        } else {
          buffer1[pin_col] = pixel;
          pix_b0 = buffer0[pin_col];
          pix_b1 = pixel;
        }
    
        // p0 p1
        if (shift_frame) {p0 = p1;} else if (pin_col == 0) p0 = pix_b0;
        p1 = pix_b0;
        // p2 p3
        if (shift_frame) {p2 = p3;} else if (!pin_col) p2 = pix_b1;
        p3 = pix_b1;
    
        // Control the iteration count 
        odd_col = (odd_col + 1) % 2;
    
        if (send_frame) {
          frame_pool_t kernel_frame;
          if (enable_pooling) {
            kernel_frame.pixel[0] = p0; kernel_frame.pixel[1] = p1;
            kernel_frame.pixel[2] = p2; kernel_frame.pixel[3] = p3;
          } else {
            kernel_frame.pixel[0] = pixel;
            #pragma unroll
            for (uint z = 1; z < kpcpo; z++) {
              kernel_frame.pixel[z] = poz;
            }
          }
          write_channel_intel(CH_POOL_CVT_OUT, kernel_frame);
          #ifdef DEBUG_POOL
            #ifdef DEBUG_VERBOSE
            printf("POOL_CVT: Send Frame  pooling %s:\n", enable_pooling?" enabled":" NOT enabled");
            for (int x=0; x<CPO; x++) printf(" cpo %d: %f %f %f %f\n", x, (float)kernel_frame.pixel[0].pixel[x], (float)kernel_frame.pixel[1].pixel[x], (float)kernel_frame.pixel[2].pixel[x], (float)kernel_frame.pixel[3].pixel[x]);
            #endif
          #endif
        }
      }
    } // iterations
  } // o_iter
  #ifdef DEBUG_POOL
  printf("POOL_CVT: end \n");
  #endif
}

// ----------------------------------------------------------------------------
// Pooling operation of the layer
//
// Parameters:
//   H                  : Height of the output image kernel (k_H)
//  W                 : Width of the output image kernel (k_W)
//  enable_maxpooling : Activates maxpooling operation
//  enable_avgpooling : Activates avgpooling operation
//   in              : Input stream (frame_pool_t) from the cvt module
//   out              : Output stream (pixel_out_t)
//
//   If no enable is active then the module bypasses the first pixel of the incomming frame to the output stream
//
// ----------------------------------------------------------------------------
kernel void pool_pooling (uint H, uint W, uint enable_maxpooling, uint enable_avgpooling, uint O_ITER){

  frame_pool_t kernel_frame;
  pixel_out_t out_pix;

  uint enable_pooling = enable_maxpooling || enable_avgpooling;
  uint WO = enable_pooling ? ((W - KW_POOLING)/SW_POOLING + 1) : W;
  uint HO = enable_pooling ? ((H - KH_POOLING)/SH_POOLING + 1) : H;

  uint size_out = HO * WO;


  uint size_kernel = KH_POOLING * KW_POOLING;
  uint iterations = size_out;

  #ifdef DEBUG_POOL
    printf("POOL_POOLING: start \n");
    #ifdef DEBUG_VERBOSE
      printf("POOL_POOLING: H %u   W %u   ena_max_pooling %u   ena_avg_pooling %u\n", H, W, enable_maxpooling, enable_avgpooling );
      uint index_rd = 0; 
      uint index_wr = 0;
    #endif
  #endif

  pool_pooling_o_iter_loop:
  for(uint o_iter = 0; o_iter < O_ITER; o_iter++) {
    #ifdef DEBUG_POOL 
      #ifdef DEBUG_VERBOSE
      printf("POOL_POOLING: o_iter %u\n", o_iter);
    #endif
    #endif
  
    pool_pooling_loop_iter:
    for (uint i=0; i < iterations; i++) {
      
      // Let's read the input frame
      kernel_frame = read_channel_intel(CH_POOL_POOL_IN);


      #ifdef DEBUG_POOL
        #ifdef DEBUG_VERBOSE
        printf("POOL_POOLING: read  frame %u \n", index_rd++);
        for (uint x=0; x<CPO; x++) {
          printf("  cpo %d: ", x);
          for (uint xx=0; xx<size_kernel; xx++) printf(" %f", (float)kernel_frame.pixel[xx].pixel[x]);
          printf("\n");
        }
        #endif
      #endif
  

      data_type maxpool_value[CPO];
      data_type avgpool_accumulator[CPO];
      data_type avgpool_value[CPO];
      data_type pool_value[CPO];

     // printf ("Jelou world, this is jm10, size of sizekernel is: %u\n\n", size_kernel);
      pooling_loop_kernel:
      #pragma unroll
      for (uint cpo=0; cpo < CPO; cpo++) {
        data_type v_0 = kernel_frame.pixel[0].pixel[cpo];
        data_type v_1 = kernel_frame.pixel[1].pixel[cpo];
        data_type v_2 = kernel_frame.pixel[2].pixel[cpo];
        data_type v_3 = kernel_frame.pixel[3].pixel[cpo];

        data_type v_max_0_a = v_0 > v_2 ? v_0 : v_2;
        data_type v_max_0_b = v_1 > v_3 ? v_1 : v_3;;

        data_type v_max_1_a = v_max_0_a > v_max_0_b ? v_max_0_a : v_max_0_b;
        
        maxpool_value[cpo] = v_max_1_a ;

        data_type v_avg_0_a = v_0 + v_2;
        data_type v_avg_0_b = v_1 + v_3; 
        data_type v_avg_1_a = v_avg_0_a + v_avg_0_b;
        avgpool_accumulator[cpo] = v_avg_1_a;

      }      

      #pragma unroll
      for (uint cpo=0; cpo < CPO; cpo++) {
        avgpool_value[cpo] = avgpool_accumulator[cpo] / (data_type)size_kernel;
      }

      #pragma unroll
      for (uint cpo=0; cpo < CPO; cpo++) {
        pool_value[cpo] = enable_maxpooling ? maxpool_value[cpo] : avgpool_value[cpo];
      }

      #pragma unroll
      for (uint cpo=0; cpo < CPO; cpo++) {
        out_pix.pixel[cpo] = enable_pooling ? pool_value[cpo] : kernel_frame.pixel[0].pixel[cpo];
      }

      write_channel_intel(CH_POOL_POOL_OUT, out_pix);
      #ifdef DEBUG_POOL
        #ifdef DEBUG_VERBOSE
          printf("POOL_POOLING: send pixel  %u: ", index_wr++);
            for (uint x=0; x<CPO; x++) printf(" %f", (float)out_pix.pixel[x]);
            printf("\n");
        #endif
      #endif
    }
  }

  #ifdef DEBUG_POOL
    printf("POOL_POOLING: end\n");
  #endif
}

// -----------------------------------------------------------------------------------------------------------
// Batch normalization layer
// new layer between pool_pooling and output,
//  During training the layer returns    gamma * (batch - mean(batch)     ) / sqrt(var(batch)      + epsilon) + beta
//  During inference the layer returns   gamma * (batch - self.moving_mean) / sqrt(self.moving_var + epsilon) + beta
kernel void batch_norm (uint num_pixels_in, uint enable_batch_norm_in, uint O_ITER_in) {
  
  uint enable_bn  = enable_batch_norm_in;
  uint O_ITER     = O_ITER_in;
  uint num_pixels = num_pixels_in; //num iteratios per o_iter pass

  //#ifdef DEBUG_BATCH_NORM
  //printf("BATCH NORM: start. enable_batch_norm = %u\n", enable_bn);
  //#endif

  #ifdef DEBUG_BATCH_NORM
  //#ifdef DEBUG_VERBOSE
  printf("BATCH_NORM: enable_batch_norm = %u   O_ITER %u  num_pixels %u\n",
          enable_bn, O_ITER, num_pixels);
  //#endif
  #endif

  // ---------------------------------------------

  for (uint o_iter = 0; o_iter < O_ITER; o_iter++){

    bnp_st_t     bn_values_in; // batch normalaziton values, to perform bn stage
    
    #ifdef DEBUG_BATCH_NORM
    #ifdef DEBUG_VERBOSE
    printf("BATCH NORM: dbg o_iter %u\n", o_iter);
    #endif
    #endif
    
    if (enable_bn) {
      bn_values_in = read_channel_intel(CH_BATCHNORM_BN_IN);
    } else {
      bn_zero_init_loop:
      #pragma unroll
      for (uint i=0; i<(CPO*4); i++){
        bn_values_in.values[i] = (data_type)0.f;
      }
    }

    loop_batch_norm_pixels:
    for(uint i = 0; i < num_pixels; i++) {
      pixel_out_t  data_in; // data from previous convolution stage, currently previous stage is pooling
      pixel_out_t  data_out;
      
      data_in = read_channel_intel(CH_BATCHNORM_IN);

      loop_batch_norm_cpo:
      #pragma unroll
      for (uint cpo = 0; cpo < CPO; cpo++) {
        bn_t  v_in;
        bn_t  std;
        bn_t  xn;
        bn_t  v_batchnorm;
        
        v_in = data_in.pixel[cpo];

        // Apply normalization
        // std = sqrt(run_var + eps)
        uint   ind_rv      = (cpo*4)+3;
        bn_t   run_var     = bn_values_in.values[ind_rv];
        bn_t   run_var_eps = run_var + BN_EPS;
        //std                = sqrt(run_var_eps);

        data_type rveps = (data_type)run_var_eps;
        std = sqrt(((float)rveps));
        
        // xn = (prev_a - run_mean) / std
        // xn = (v_in - bn_values_in.values[(cpo*4)+2]) / std;
        uint   ind_rm    = (cpo*4) + 2;
        bn_t   run_mean  = bn_values_in.values[ind_rm];
        bn_t   tmp_1_rm  = v_in - run_mean;
        xn               = tmp_1_rm / std;
        
        // y = gamma * xn + beta
        //v_batchnorm = bn_values_in.values[(cpo*4)+1] * xn + bn_values_in.values[cpo*4];
        uint   ind_y_gamma  = (cpo*4)+1;
        uint   ind_y_beta   =  cpo*4;
        bn_t   gamma        = bn_values_in.values[ind_y_gamma];
        bn_t   tmp_g_1      = gamma * xn;
        bn_t   beta         = bn_values_in.values[ind_y_beta];
        v_batchnorm         = tmp_g_1 + beta;

        data_out.pixel[cpo] = enable_bn ? v_batchnorm : v_in;
      }

      #ifdef DEBUG_BATCH_NORM
      #ifdef DEBUG_VERBOSE
      printf("Batch Norm (pixel %d): \n", i);
      for (int x = 0; x < CPO; x++) {
        printf("   cpi %d : in (from pooling): %6.2f   out (to data_add):  %6.2f\n", x, (float)data_in.pixel[x], (float)data_out.pixel[x]);
      }
      #endif
      #endif
      write_channel_intel(CH_BATCHNORM_OUT, data_out);
    }
  }

  #ifdef DEBUG_BATCH_NORM
  printf("BATCH NORM: end\n");
  #endif

}

// -------------------------------------------------------------------------------
// add_data: this function performs the element-wise addition on the input data
//
// Arguments:
//   in_r   : input stream data from read module
//   in_pool: input stream data from previous module
//   out    : output stream
//
kernel void add_data(uint num_pixels_in, uint enable_add_data_in, uint O_ITER_in) {

  uint enable_add = enable_add_data_in;
  uint O_ITER     = O_ITER_in;
  uint num_pixels = num_pixels_in; //num iteratios per o_iter pass

  #ifdef DEBUG_ADD_DATA
  printf("ADD_DATA: start\n");
  //#ifdef DEBUG_VERBOSE
  printf("ADD_DATA: num_pixels %u   O_ITER %u   enable_add %u\n", num_pixels, O_ITER, enable_add);
  //#endif
  #endif

  pixel_out_t data_in_r;
  pixel_out_t data_in_prev;
  pixel_out_t data_out;
  pixel_out_t px_o_zero;

  // copied from pool pooling, since this kernel/task will read as many "pixels" as the previous stage writes into the channel


  // ----------------------------------------------------------------------------------------------------


  px_o_zero_init_loop:
  #pragma unroll
  for (uint o=0; o<CPO; o++){
    px_o_zero.pixel[o] = (data_type)0.f;
  }

  add_o_iter_loop:
  for(uint o_iter = 0; o_iter < O_ITER; o_iter++) {
    #ifdef DEBUG_ADD
    #ifdef DEBUG_VERBOSE
    printf("ADD_DATA: o_iter %u \n", o_iter);
    #endif
    #endif

    loop_add_data_pixels:
    for (int i=0; i < num_pixels; i++) {
      // Let's read the input data
      
      data_in_prev = read_channel_intel(CH_ADDDATA_IN); // Data from previous stage, pool_pooling

      if(enable_add) {
        data_in_r  = read_channel_intel(CH_ADDDATA_ADD_IN); //data from memory to add
      } else {
        data_in_r  = px_o_zero;
      }
  
      loop_add_data_cpo:
      #pragma unroll
      for(int cpo = 0; cpo<CPO; cpo++){
        data_type   v_in_a;
        bn_t        v_in_b;
        //add_data_t  v_out;
        v_in_a = (data_type)data_in_r.pixel[cpo];
        v_in_b = (data_type)data_in_prev.pixel[cpo];
        //v_out = v_in_a + v_in_b;
        //data_out.pixel[cpo] = v_out;
        data_out.pixel[cpo] = v_in_a + v_in_b;
      }
      
      #ifdef DEBUG_ADD_DATA
      #ifdef DEBUG_VERBOSE
       printf("ADD_DATA (pixel %d):\n", i);
       for (int x=0; x<CPO; x++) {
          printf("  cpi %d : in_a %f in_b %f out %f\n", x, (float)data_in_r.pixel[x],(float)data_in_prev.pixel[x], (float)data_out.pixel[x]);
       }
      #endif
      #endif
      write_channel_intel(CH_ADDDATA_OUT,data_out);
    }
  }
  #ifdef DEBUG_ADD_DATA
  printf("ADD_DATA: end\n");
  #endif
}
//*********************************************************************************************************************
// end of file: k_conv2D.cl
//*********************************************************************************************************************

