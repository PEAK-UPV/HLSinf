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
void read_bias(int offset_bias, pixel_out_t *b_ptr, hls::stream<pixel_out_t> &out) {

  #ifdef DEBUG_READ_BIAS
  printf("READ_BIAS: start\n");
  #endif

  pixel_out_t bias;
  #pragma HLS ARRAY_PARTITION variable=bias complete dim=0

  bias = b_ptr[offset_bias];
  out << bias;

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
void read_kernel(int I_ITER, int offset_kernel, data_type *k_ptr, hls::stream<kernel_t> &k_out){

  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL: start\n");
  #endif

  kernel_t k;
  int cnt = 0;
  #pragma HLS array_partition variable=k complete dim=0

  for (int i=0; i<I_ITER; i++) {
	  DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)
	  DO_PRAGMA(HLS LOOP_FLATTEN OFF)

	  for (int cpo=0; cpo < CPO; cpo++) {
		  for (int cpi=0; cpi < CPI; cpi++) {
			  for (int p=0; p<9; p++) {
				  #pragma HLS pipeline II=1
				  k.pixel[cpo][cpi][p] = k_ptr[offset_kernel+cnt];
				  cnt = cnt + 1;
			  }
		  }
	  }
	  k_out << k;
	  #ifdef DEBUG_READ_KERNEL
	  for (int cpo=0; cpo<CPO; cpo++) {
		  for (int cpi=0; cpi<CPI; cpi++) {
		      printf("READ_KERNEL: Kernel read for cpi=%d cpo=%d : ", cpi, cpo);
		      for (int p=0;p<9; p++) printf(" %6.4f ", float(k.pixel[cpo][cpi][p]));
		      printf("\n");
		  }
	  }
	  #endif
  }

  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL: end\n");
  #endif
}

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
void dws_read_kernel(int I_ITER, int offset_dw_kernel, int offset_pw_kernel, data_type *k_dw_ptr, data_type *k_pw_ptr, hls::stream<kernel_dw_t> &k_dw_out, hls::stream<kernel_pw_t> &k_pw_out){

  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL: start\n");
  #endif

  // we read all the kernels and send them through the stream
  kernel_dw_t kernel_dw;
  #pragma HLS ARRAY_PARTITION variable=kernel_dw complete dim=0
  kernel_pw_t kernel_pw;
  #pragma HLS ARRAY_PARTITION variable=kernel_pw complete dim=0

  int cnt_dw = 0;
  int cnt_pw = 0;

	dws_read_kernel_loop_i_iter:
  for (int i_iter=0; i_iter<I_ITER; i_iter++) {
	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)


	// deep-wise
    dws_read_kernel_loop_dw_cpi:
    for (int cpi=0; cpi<CPI; cpi++) {
      dws_read_kernel_loop_dw_p:
	  for (int p=0; p<9; p++) {
		#pragma HLS pipeline II=1
		kernel_dw.pixel[cpi][p] = k_dw_ptr[offset_dw_kernel+cnt_dw];
	    cnt_dw = cnt_dw + 1;
	  }
	}
	k_dw_out << kernel_dw;

	#ifdef DEBUG_READ_KERNEL
    printf("READ_KERNEL: deep-wise kernel read\n");
    for (int cpi=0; cpi<CPI; cpi++) {
      printf("channel ci %d: ", cpi);
      for (int p=0; p<9; p++) printf(" %f ", float(kernel_dw.pixel[cpi][p]));
      printf("\n");
    }
    #endif

    // point-wise
    dws_read_kernel_loop_pw_cpo:
    for (int cpo=0; cpo<CPO; cpo++) {
      dws_read_kernel_loop_pw_cpi:
      for (int cpi=0; cpi<CPI; cpi++) {
		#pragma HLS pipeline II=1
        kernel_pw.pixel[cpo][cpi] = k_pw_ptr[offset_pw_kernel+cnt_pw];
        cnt_pw = cnt_pw + 1;
      }
    }
    k_pw_out << kernel_pw;
    #ifdef DEBUG_READ_KERNEL
    printf("READ_KERNEL: point-wise kernel read\n");
    for (int cpo=0; cpo<CPO; cpo++) {
      printf("channel co %d: ", cpo);
      for (int cpi=0; cpi<CPI; cpi++) printf("ci: %d %f ", cpi, float(kernel_pw.pixel[cpo][cpi]));
	  printf("\n");
    }
    #endif
  }
  #ifdef DEBUG_READ_KERNEL
  printf("READ_KERNEL: end\n");
  #endif
}




// ---------------------------------------------------------------------------------------
// read_data_channels. Reads all data channels and send it through the output streams
//
// Arguments:
//   H, W                : Data channel height and width
//   rows                : Number of rows of the frame to read
//   num_extra_rows      : Number of extra rows to read
//   I_ITER              : Number of input iterations (I / CPI)
//   ptr                 : pointer to input data
//   offset              : offsets within input data for each channel
//   out                 : output streams for each channel
//   enable_read_channel : enables for each channel. If not set the module produces just zeros and does not read memory
//
void read_data_channels(int H, int W, int rows, int I_ITER, ap_uint<512> *ptr, int offset, int num_extra_rows, int channel_blocks, hls::stream<read_block_t> out[CPI], int I) {

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: starts\n");
  #endif

  int channel_size = H * W;							// channel size
  read_block_t bx[CPI];								// buffer for block read from memory
  int offset_[CPI];									// offset for the channel
  int first_block_[CPI];							// first block address

    read_data_channels_loop_I_ITER:
    for (int i_iter = 0; i_iter < I_ITER; i_iter++) {
      DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)
      DO_PRAGMA(HLS LOOP_FLATTEN off)

      // each channel has its first block
      read_data_channels_loop_CPI_init:
      for(int cpi = 0; cpi<CPI; cpi++){
        #pragma HLS pipeline
        offset_[cpi] = offset + (channel_size * CPI * i_iter) + (channel_size * cpi);
        first_block_[cpi] = offset_[cpi] / READ_BLOCK_SIZE;
        #ifdef DEBUG_READ_DATA
        printf("READ_DATA: cpi %d -> offset %d first_block %d\n", cpi, offset_[cpi], first_block_[cpi]);
        #endif
      }

      read_data_channels_loop_blocks:
      for (int block = 0; block < channel_blocks; block=block+READ_BURST_SIZE) {
    	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=(W_REFERENCE*H_REFERENCE/READ_BLOCK_SIZE) / READ_BURST_SIZE)

    	read_data_channels_loop_CPI:
        for(int i = 0; i < CPI; i++){
          DO_PRAGMA(HLS pipeline II=READ_BURST_SIZE)

          read_block_t data_read[READ_BURST_SIZE];
       	  int current_input_channel = (i_iter * CPI) + i;
       	  int addr = first_block_[i];
       	  int enable = current_input_channel < I;
       	  read_data_channels_loop_burst:
       	  for (int b = 0; b < READ_BURST_SIZE; b++) {
    		data_read[b] = ptr[addr + b];
            if (enable && ((block + b) < channel_blocks)) {
        	  out[i] << data_read[b];
              first_block_[i] = first_block_[i] + 1;
            }
          }
        }
      }
    } //i_iter

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: ends\n");
  #endif

}
