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


extern int I_useful;
extern int O_useful;




extern scoped_aligned_ptr<unsigned long> hls_dbg_ul; // array for returning unsigned long values from kernel
extern scoped_aligned_ptr<data_type>     hls_dbg_dt; // array for returning data_type values from kernel
extern cl_mem                            hls_dbg_ul_buffer;
extern cl_mem                            hls_dbg_dt_buffer;


extern data_type dbg_cpu_data_directconv_sum;

extern scoped_aligned_ptr<data_type> dbg_loop_data_in;                         // loop from fpga for Input data buffer (format I x W x H)
extern cl_mem                        dbg_loop_data_in_buffer_i;   // loop from fpga for input buffer
extern scoped_aligned_ptr<data_type> dbg_loop_data_input_buffer;
extern cl_mem                        dbg_loop_data_input_buffer_buffer;

// references to variables of buffers related to direct convolution stage 
extern scoped_aligned_ptr<data_type>  dbg_loop_data_dc_pad_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl_mem                         dbg_loop_data_dc_pad_out_buffer;   // loop from fpga for input buffer
extern scoped_aligned_ptr<data_type>  dbg_loop_data_dc_cvt_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl_mem                         dbg_loop_data_dc_cvt_out_buffer;   // loop from fpga for input buffer
extern scoped_aligned_ptr<hls_cvt_sbs_control_t>  dbg_loop_data_dc_cvt_sbs_control_out;     // loop from fpga for step by step frame conformation, control signals
extern cl_mem                                 dbg_loop_data_dc_cvt_sbs_control_out_buffer;  // from fpga for step by step frame conformation, control signals
extern scoped_aligned_ptr<frame_t>  dbg_loop_data_dc_cvt_sbs_frame_out;           // from fpga for step by step frame conformation, frame value,
extern cl_mem                         dbg_loop_data_dc_cvt_sbs_frame_out_buffer;    // loop from fpga for step by step frame conformation, frame value
extern scoped_aligned_ptr<data_type>  dbg_loop_data_dc_mul_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl_mem                         dbg_loop_data_dc_mul_out_buffer;   // loop from fpga for input buffer
extern scoped_aligned_ptr<data_type>  dbg_loop_data_directconv_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl_mem                         dbg_loop_data_directconv_out_buffer;   // loop from fpga for input buffer

extern scoped_aligned_ptr<data_type>  dbg_loop_data_relu_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl_mem                         dbg_loop_data_relu_out_buffer;   // loop from fpga for input buffer
extern scoped_aligned_ptr<data_type>  dbg_loop_data_pooling_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl_mem                         dbg_loop_data_pooling_out_buffer;   // loop from fpga for input buffer

extern scoped_aligned_ptr<data_type>  dbg_cpu_data_directconv_out;







//-----------------------------------------------------------------------------
// public functions prototypes
//-----------------------------------------------------------------------------
void hls_debug(void);

#endif
//-----------------------------------------------------------------------------
// end of file: hls_debug.h
//-----------------------------------------------------------------------------

