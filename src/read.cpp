/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

// ---------------------------------------------------------------------------------------
// read_bias. Reading bias from memory and sending to conv module
//
// Arguments:
//   b_ptr               : pointer to bias
//   offset_bias         : offset to bias
//   b_out               : output stream
//
// All the bias are read and sent through the out stream
//
void read_bias(int offset_bias, read_bias_st *b_ptr, hls::stream<b_st> &out) {

  #ifdef DEBUG_READ_BIAS
  printf("READ_BIAS: start\n");
  #endif

  read_bias_st bias;
  #pragma HLS ARRAY_PARTITION variable=bias complete dim=0

  bias = b_ptr[offset_bias];
  b_st px;
  for (int i=0; i<CPI; i++) {
    #pragma HLS unroll
	px.pixel[i] = b_t(bias.pixel[i]);
  }

  out << px;

  #ifdef DEBUG_READ_BIAS
  printf("READ_BIAS: offset_bias = %d\n", offset_bias);
  printf("READ_BIAS: bias = ");
  for (int c=0; c<CPO; c++) printf(" %f ", float(bias.pixel[c]));
  printf("\n");
  #endif

  #ifdef DEBUG_READ_BIAS
  printf("READ_BIAS: end\n");
  #endif
}


// ---------------------------------------------------------------------------------------
// read_batch_norm. Reading batch normalization values from memory and sending to batch
// 					normalization module
//
// Arguments:
//   b_ptr               : pointer to batch normalization values
//   offset_batchnorm    : offset to batch normalization values
//   b_out               : output stream
//
// All the bias are read and sent through the out stream
//
void read_batch_norm(int offset_batchnorm, bnp_st *b_ptr, hls::stream<bnp_st> &out) {
  #ifdef DEBUG_READ_BATCH_NORM
  printf("DEBUG_READ_BATCH_NORM: start\n");
  #endif

  bnp_st batch_norm;
  #pragma HLS ARRAY_PARTITION variable=batch_norm complete dim=0

  batch_norm = b_ptr[offset_batchnorm];
  out << batch_norm;

  #ifdef DEBUG_READ_BATCH_NORM
  printf("DEBUG_READ_BATCH_NORM: value = ");
  for (int c=0; c<CPO*4; c++) {
	  printf(" %f ", float(batch_norm.values[c]));
  }
  printf("\n");
  #endif

  #ifdef DEBUG_READ_BATCH_NORM
  printf("DEBUG_READ_BATCH_NORM: end\n");
  #endif
}

#ifdef DIRECT_CONV
// ---------------------------------------------------------------------------------------
// read_kernel. Reads kernels and sends them through the stream
//
// Arguments:
//   I_ITER              : Number of input iterations (I / CPI)
//   k_ptr               : pointer to kernels
//   offset_kernel       : offset to kernels
//   k_out               : output stream
//
// kernels are stored in memory with the format GO x GI x CPO x CPI x KH x KW
// This storage formats lets the module to read memory sequentially and send all the
// kernels in the same order they are read through the output stream.
// kernels are sent in frame structures (3x3 grid)
//
void read_kernel(int enable, int I_ITER, int offset_kernel, read_filter_t *k_ptr, hls::stream<w_t> &k_out){

  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL: start\n");
  #endif

  if (!enable) return;

  read_filter_t k;
  w_t kk;
  int cnt = 0;
  #pragma HLS array_partition variable=k complete dim=0

  for (int i=0; i<I_ITER; i++) {
    DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)
    DO_PRAGMA(HLS LOOP_FLATTEN OFF)
    for (int cpo=0; cpo < CPO; cpo++) {
      for (int cpi=0; cpi < CPI; cpi++) {
        for (int p=0; p<9; p++) {
          #pragma HLS pipeline II=1
          k = k_ptr[offset_kernel+cnt];
	  kk = w_t(k);
	  k_out << kk;
	  cnt = cnt + 1;
	}
        #ifdef DEBUG_READ_KERNEL
	printf("READ_KERNEL: cpi %d cpo %d", cpi, cpo);
	for (int p=0; p<9; p++) printf(" %-6.4f", float(k_ptr[offset_kernel+cnt-9+p]));
	printf("\n");
        #endif
      }
    }
  }

  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL: end\n");
  #endif
}
#endif

#ifdef DWS_CONV
// ---------------------------------------------------------------------------------------
// dws_read_kernel. Reads kernels and sends them through the stream. This kernel is for
// DWS convolution
//
// Arguments:
//   I_ITER              : Number of input iterations (I / CPI)
//   offset_kernel       : offset to kernels
//   k_ptr               : pointer to kernels
//   k_dw_out            : deepwise output stream
//   k_pw_out			 : pointwise output stream
//
// kernels are stored in memory with the format GO x GI x CPO x CPI x KH x KW
// This storage formats lets the module to read memory sequentially and send all the
// kernels in the same order they are read through the output stream.
// kernels are sent in frame structures (3x3 grid)
//
void dws_read_dw_kernel(int I_ITER, int o_iter, w_t *k_dw_ptr, hls::stream<w_dw_st> &k_dw_out){

  #ifdef DEBUG_READ_KERNEL
  printf("READ_DW_KERNEL: starts\n");
  #endif

  // Kernels buffer
  #define MAX_KERNELS_DW         512/CPI
  static w_dw_st dw[MAX_KERNELS_DW];
  DO_PRAGMA(HLS AGGREGATE variable=dw)

  w_dw_st kernel_dw;
  DO_PRAGMA(HLS AGGREGATE variable=kernel_dw)

  if (o_iter == 0) {
  	int addr = 0;
    dw_read_kernel_loop_i:
    for (int i=0; i<I_ITER; i++) {
      DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE)
	    dw_read_kernel_loop_cpi:
	    for (int cpi=0; cpi<CPI; cpi++) {
		    dw_read_kernel_loop_k:
		    for (int k=0; k<9; k++) {
	        DO_PRAGMA(HLS pipeline)
    		  kernel_dw.pixel[cpi][k] = k_dw_ptr[addr+k];
    		}
		    addr = addr+9;
	    }
	    dw[i] = kernel_dw;
	    k_dw_out << kernel_dw;
    }
  } else {
    // Now we send through the stream the kernels
    dws_loop_dw:
    for (int i=0; i<I_ITER; i++) {
	  DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)
	  DO_PRAGMA(HLS PIPELINE)
	  kernel_dw = dw[i];
	  k_dw_out << kernel_dw;
      #ifdef DEBUG_READ_KERNEL
      #ifdef DEBUG_VERBOSE
	  printf("READ_DW_KERNEL: dw sent:\n");
	  for (int cpi=0; cpi<CPI; cpi++) {
		  for (int k=0; k<9; k++) {
			  printf("%4.2f ", float(dw[i].pixel[cpi][k]));
		  }
		  printf("\n");
	  }
      #endif
      #endif
    }
  }

  #ifdef DEBUG_READ_KERNEL
  printf("READ_DW_KERNEL: ends\n");
  #endif

}

void dws_read_pw_kernel(int I_ITER, int O, int o_iter, w_pw_t *k_pw_ptr, hls::stream<w_pw_st> &k_pw_out){

  #ifdef DEBUG_READ_KERNEL
  printf("READ_PW_KERNEL: starts\n");
  #endif

  w_pw_st read_kernel_pw;
  w_pw_st kernel_pw;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel_pw complete dim=0)

  int addr = o_iter * CPO * I_ITER;

  // point-wise
  pw_read_kernel_loop_i_iter:
  for (int i_iter=0; i_iter<I_ITER; i_iter++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)

    pw_read_kernel_loop_o:
    for (int cpo=0; cpo<CPO; cpo++) {
      DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=O_REFERENCE)
      DO_PRAGMA(HLS PIPELINE)

      read_kernel_pw = k_pw_ptr[addr+cpo];

      pw_read_kernel_loop_cpi:
      for (int cpi=0; cpi<CPI; cpi++) {
        DO_PRAGMA(HLS UNROLL)

        int first = cpi * DATA_TYPE_WIDTH;
        int last = first + DATA_TYPE_WIDTH - 1;
	ap_int<DATA_TYPE_WIDTH> aux = read_kernel_pw.range(last, first);
        *(ap_uint<DATA_TYPE_WIDTH>*)(&kernel_pw.pixel[cpo][cpi]) = (data_type)aux;
      }
    }
    addr += CPO;

    k_pw_out << kernel_pw;

    #ifdef DEBUG_READ_KERNEL
    #ifdef DEBUG_VERBOSE
    printf("READ_PW_KERNEL (cpo/cpi table):\n");
    for (int cpo=0; cpo<CPO; cpo++) {
  	  for (int cpi=0; cpi<CPI; cpi++) {
  		  printf(" %4.2f", float(kernel_pw.pixel[cpo][cpi]));
  	  }
  	  printf("\n");
    }
    #endif
    #endif
  }

  #ifdef DEBUG_READ_KERNEL
  printf("READ_PW_KERNEL: ends\n");
  #endif
}
#endif

// ---------------------------------------------------------------------------------------
// read_data_channels_gihwcpi. Reads all input data assuming GIxHxWxCPI input data format
// Read pixels are sent out through the output stream
//
// Arguments:
//   num_pixels          : Number of pixels to read in total (each pixel is CPI wide)
//   offset              : offsets within input data to read (offset aligned to CPI wide pixels)
//   ptr                 : pointer to input data
//   out                 : output stream
//   enable              : enable for the read operation
//

void read_data_channels_gihwcpi(int num_pixels, int offset, int I_ITER, int cpi_group_offset, read_block_t *ptr, hls::stream<din_st> &out, int enable) {

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: starts (gihwcpi format)\n");
  printf("  num_pixels : %d\n", num_pixels);
  printf("  offset     : %d\n", offset);
  printf("  cpi_group_offset : %d\n", cpi_group_offset);
  printf("  ptr        : %p\n", ptr);
  #endif

  if (!enable) return;

  read_data_channels_loop_i_iter:
  for (int iter = 0; iter < I_ITER; iter++) {
    DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE / CPI)
    int offset_global = offset + (cpi_group_offset * iter);
#ifdef DEBUG_READ_DATA
#ifdef DEBUG_VERBOSE
    printf("offset global for iteration %d = %d\n", iter, offset_global);
#endif
#endif

    #ifdef DEBUG_READ_DATA
    int debug_iter = 0;
    printf("data read (iteration %d):\n", iter);
    #endif

    read_data_channels_loop_pixels:
    for (int i = 0; i < num_pixels; i++) {
      DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max = H_REFERENCE * W_REFERENCE)
      DO_PRAGMA(HLS pipeline)

      read_data_st px;
      din_st px_out;
      px = ptr[offset_global+i];
      for (int cpi=0; cpi<CPI; cpi++) {
        #pragma HLS unroll
    	px_out.pixel[cpi] = din_t(px.pixel[cpi]);
      }
      out << px_out;
      #ifdef DEBUG_READ_DATA
      for (int x=0; x<CPI; x++) printf("%6.4f ", float(px.pixel[x]));
      debug_iter++;
      if (debug_iter == 8) {debug_iter = 0; printf("\n");} else printf(",");
      #endif
    }

    #ifdef DEBUG_READ_DATA
    printf("\n");
    #endif
  }



  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: ends (gihwcpi format)\n");
  #endif

}

// ---------------------------------------------------------------------------------------
// read_input_add_gihwcpi. Reads all input data assuming GIxHxWxCPO input data format
// Read pixels are sent out through the output stream
//
// Arguments:
//   num_pixels          : Number of pixels to read in total (each pixel is CPI wide)
//   offset              : offsets within input data to read (offset aligned to CPI wide pixels)
//   ptr                 : pointer to input data
//   out                 : output stream
//   enable              : enable for the read operation
//

void read_input_add_gihwcpi(int num_pixels, int offset, write_block_t *ptr, hls::stream<dout_st> &out, int enable) {

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: starts (gihwcpi format)\n");
  printf("  num_pixels : %d\n", num_pixels);
  printf("  offset : %d\n", offset);
  #endif

  if (!enable) return;

  read_data_channels_loop_pixels:
  for (int i = 0; i < num_pixels; i++) {
    DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max = I_REFERENCE * H_REFERENCE * W_REFERENCE / CPI)
    DO_PRAGMA(HLS pipeline)

    write_data_st px;
    dout_st px_out;
  	px = ptr[offset+i];
  	for (int cpo=0; cpo<CPO; cpo++) {
      #pragma HLS unroll
  	  px_out.pixel[cpo] = dout_t(px.pixel[cpo]);
  	}
    out << px_out;
    #ifdef DEBUG_READ_DATA
    #ifdef DEBUG_VERBOSE
    printf("data read : %d : ", i);
    for (int x=0; x<CPO; x++) printf("%f ", float(px.pixel[x]));
    printf("\n");
    #endif
    #endif
  }

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: ends (gihwcpi format)\n");
  #endif

}
