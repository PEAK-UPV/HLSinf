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

#ifndef _HLS_KERNEL_COUNTERS_H_
#define _HLS_KERNEL_COUNTERS_H_

// Global variables
extern scoped_aligned_ptr<unsigned long> hls_kernel_counters_ul; // array for returning unsigned long values from kernel
extern cl_mem                            hls_kernel_counters_ul_buffer;

// MACROS



// public functions

void hls_kernel_counters_read_and_print(void);


#endif

//-----------------------------------------------------------------------------
// end of file: hls_kernel_counters.h
//-----------------------------------------------------------------------------

