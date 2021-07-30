// --------------------------------------------------------------------------------------------------------------
// HLSinf kernels
// Version: 1.0
// copyright (c) 2021, Universidad Politècnica de València (UPV), GAP research group
// Date: July 2021
// Authors: GAP Research Group (UPV)
//     José Flich Cardo
//     Jorge García Martínez
//     Izan Catalán Gallarch
//     Carles Hernández Luz
//
// contact: jflich@disca.upv.es
// All rights reserved
// --------------------------------------------------------------------------------------------------------------

#ifndef _HLS_DEBUG_H_
#define _HLS_DEBUG_H_

//-----------------------------------------------------------------------------
// included files
//-----------------------------------------------------------------------------
// -- none

//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
// for debuging purposes: number of i iterations for buffers size
#define NUM_OF_I_ITERS 1
#define NUM_OF_O_ITERS 2  // must match o_iters in lib_conv2D.cpp function call

#define NUM_PIXELS_IN_FRAME 9



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
extern unsigned long int my_val[NUM_KERNELS];
//extern unsigned long my_ret[NUM_KERNELS];
//extern unsigned long my_ret_2[NUM_KERNELS];
//extern unsigned long my_ret_3[NUM_KERNELS];
//extern unsigned long my_ret_4[NUM_KERNELS];
//extern float         my_flt_bias[NUM_KERNELS];
//extern float         my_flt_krnl[NUM_KERNELS];
//extern float         my_flt_din[NUM_KERNELS];
//extern float         my_flt_dout[NUM_KERNELS];
extern unsigned long *hls_dbg_ul; // array for returning unsigned long values from kernel
extern data_type     *hls_dbg_dt; // array for returning data_type values from kernel
extern cl::Buffer    *hls_dbg_ul_buffer[MAX_CONVS];
extern cl::Buffer    *hls_dbg_dt_buffer[MAX_CONVS];


extern data_type dbg_cpu_data_directconv_sum;

extern data_type  *dbg_loop_data_in;                         // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_in_buffer_i[MAX_CONVS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_input_buffer;
extern cl::Buffer *dbg_loop_data_input_buffer_buffer[MAX_CONVS];

// references to variables of buffers related to direct convolution stage 
extern data_type  *dbg_loop_data_dc_pad_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_dc_pad_out_buffer[MAX_CONVS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_dc_cvt_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_dc_cvt_out_buffer[MAX_CONVS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_dc_mul_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_dc_mul_out_buffer[MAX_CONVS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_directconv_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_directconv_out_buffer[MAX_CONVS];   // loop from fpga for input buffer

extern data_type  *dbg_loop_data_relu_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_relu_out_buffer[MAX_CONVS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_pooling_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_pooling_out_buffer[MAX_CONVS];   // loop from fpga for input buffer

extern int I_useful;
extern int O_useful;

extern data_type *dbg_cpu_data_directconv_out;

//-----------------------------------------------------------------------------
// public functions prototypes
//-----------------------------------------------------------------------------
void hls_debug(void);

#endif
//-----------------------------------------------------------------------------
// end of file: hls_debug.h
//-----------------------------------------------------------------------------

