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
void dws_read_dw_kernel(int I_ITER, int o_iter, data_type *k_dw_ptr, hls::stream<kernel_dw_t> &k_dw_out){

  #ifdef DEBUG_READ_KERNEL
  printf("READ_DW_KERNEL: starts\n");
  #endif

  // Kernels buffer
  static kernel_dw_t dw[MAX_KERNELS_DW];
  DO_PRAGMA(HLS AGGREGATE variable=dw)
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=dw dim=1 complete)

  kernel_dw_t kernel_dw;
  DO_PRAGMA(HLS AGGREGATE variable=kernel_dw)
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=kernel_dw dim=0 complete)

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

void dws_read_pw_kernel(int I_ITER, int O, int o_iter, read_kernel_pw_t *k_pw_ptr, hls::stream<kernel_pw_t> &k_pw_out){

  #ifdef DEBUG_READ_KERNEL
  printf("READ_PW_KERNEL: starts\n");
  #endif

  read_kernel_pw_t read_kernel_pw;
  kernel_pw_t kernel_pw;
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
        data_type data = (data_type)read_kernel_pw.range(last, first);
        kernel_pw.pixel[cpo][cpi] = data;
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


#ifdef IHW_DATA_FORMAT
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
void read_data_channels(int H, int W, int rows, int I_ITER, read_block_t *ptr, int offset, int num_extra_rows, int channel_blocks, hls::stream<read_block_t> out[CPI], int I, int enable) {

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: starts\n");
  #endif

  int channel_size = H * W;							// channel size
  read_block_t bx[CPI];								// buffer for block read from memory
  int offset_[CPI];									// offset for the channel
  int first_block_[CPI];							// first block address
  int first_current_input_channel;    				// first current input channel being accessed

  if (!enable) return;

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

      first_current_input_channel = i_iter * CPI;

      read_data_channels_loop_blocks:
      for (int block = 0; block < channel_blocks; block=block+READ_BURST_SIZE) {
    	DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=(W_REFERENCE*H_REFERENCE/READ_BLOCK_SIZE) / READ_BURST_SIZE)

    	read_data_channels_loop_CPI:
        for(int i = 0; i < CPI; i++){
          DO_PRAGMA(HLS pipeline II=READ_BURST_SIZE)

          read_block_t data_read[READ_BURST_SIZE];
       	  int addr = first_block_[i];
       	  int channel = first_current_input_channel + i;
       	  int enable = channel < I;
       	  if (enable) {
       	    read_data_channels_loop_burst:
       	    for (int b = 0; b < READ_BURST_SIZE; b++) {
    		  data_read[b] = ptr[addr + b];
              if ((block + b) < channel_blocks) {
        	    out[i] << data_read[b];
                first_block_[i] = first_block_[i] + 1;
              }
       	    }
          }
        }
      }
    } //i_iter

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: ends\n");
  #endif

}
#endif

#ifdef GIHWCPI_DATA_FORMAT
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

void read_data_channels_gihwcpi(int num_pixels, int offset, read_block_t *ptr, hls::stream<pixel_in_t> &out, int enable) {

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: starts (gihwcpi format)\n");
  #endif

  if (!enable) return;

  read_data_channels_loop_pixels:
  for (int i = 0; i < num_pixels; i++) {
    DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max = I_REFERENCE * H_REFERENCE * W_REFERENCE / CPI)
    DO_PRAGMA(HLS pipeline)

    pixel_in_t px;
	px = ptr[offset+i];
    out << px;
    #ifdef DEBUG_READ_DATA
    printf("data read: ");
    for (int x=0; x<CPI; x++) printf("%f ", float(px.pixel[x]));
    printf("\n");
    #endif
  }

  #ifdef DEBUG_READ_DATA
  printf("READ_DATA: ends (gihwcpi format)\n");
  #endif

}
#endif
