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


//-----------------------------------------------------------------------------
// included files
//-----------------------------------------------------------------------------
#include "test_conv2D.h"

#include "hls_kernel_counters.h"

//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
scoped_aligned_ptr<unsigned long> hls_kernel_counters_ul; // array for returning unsigned long values from kernel
cl_mem                            hls_kernel_counters_ul_buffer;

//-----------------------------------------------------------------------------
// Local variables
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

void hls_kernel_counters_read_and_print(void) {
#ifdef  HLS_KERNEL_COUNTERS_ENABLE

      // primero debo forzar la lectura de memoria
      cl_event dbg_rd_ev;
      cl_int err;
      printf("HLS debug test_kernel read hls_dbg_ul_buffer migrate operation\n");

      size_t size_hls_kernel_counters_ul_num_values = HLS_DBG_KRNL_COUNTERS_ARRAY_NUM_ENTRIES * MAX_CONVS * MAX_KERNELS;
      size_t size_hls_kernel_counters_ul_in_bytes   = size_hls_kernel_counters_ul_num_values * sizeof(unsigned long);  
      err = clEnqueueReadBuffer(q, hls_kernel_counters_ul_buffer, CL_FALSE, 0, size_hls_kernel_counters_ul_in_bytes, hls_kernel_counters_ul, 0, NULL, &dbg_rd_ev);
      CHECK(err);
      err = clWaitForEvents(1, &dbg_rd_ev);
      CHECK(err);

      // y ahora motrar los datos
      // el total de ind debe ser write_pixels -1, ya que el bucle es (...; i<write_pixels; i++)
      printf ("   Prorgess reported from kernel o_iter: %lu  ena_pool %lu  ena_poolcvt %lu  ena_poolpool %lu  o_iter_state: %lu   bias_entries: %lu   kernel_entries: %lu   c_out_pixels_read %lu (before loop)  iter_loop_ind %lu  ind/write_pixels = %lu / %lu \n",
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_index_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_pooling_enable_value_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_poolingcvt_enable_value_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_poolingpooling_enable_value_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_state_index_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_bias_elements_in_iter_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_kernel_elements_in_iter_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_2_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_3_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_4_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_1_IND]
          );
      printf ("    eb %lu  wib %lu  rib %lu  er %lu index - read_pixels for read_pixels_total  %lu - %lu   IB: %lu  PAD: %lu  CVT: %lu  MUL: %lu  ADD: %lu  RELU: %lu  POOLcvt: %lu (%lu)  POOLpooling: %lu (%lu)\n",
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_enable_buffer],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_write_to_input_buffer],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_read_from_input_buffer],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_enable_read],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_6_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_5_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_IB_out_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_PAD_out_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_CVT_out_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_MUL_out_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_ADD_out_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_RELU_out_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_POOLCVT_out_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_POOLCVT_it_limit_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_POOLPOOLING_out_IND],
          hls_kernel_counters_ul[HLS_DBG_KRNL_COUNTERS_kernel_o_iter_value_POOLPOOLING_it_limit_IND]
          );
#endif

}
//-----------------------------------------------------------------------------
// end of file: hls_kernel_counters.cpp
//-----------------------------------------------------------------------------

