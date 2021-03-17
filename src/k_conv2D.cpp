#include "conv2D.h"

#include <hls_stream.h>

void set_write_enables(int enable_write[CPO], int o_channel, int O) {
  set_write_enables_loop:
  for (int o = 0; o <CPO; o++) {
    DO_PRAGMA(HLS loop_tripcount min=1 max=CPO)
    #pragma HLS UNROLL
    enable_write[o] = (o_channel + o) < O;
  }
}

void set_reading_channel_offsets(int offset_read_data_channel_i[CPI], int offset_read_data_channel, int channel_offset) {
 set_reading_channel_offsets_loop:
 for(int i=0; i<CPI; i++){
   DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
   #pragma HLS UNROLL
   offset_read_data_channel_i[i] = (offset_read_data_channel + i * channel_offset) % READ_BLOCK_SIZE;
 }
}

void set_writing_channel_offsets(int offset_write_data_channel_i[CPO], int block_offset_write_data_channel_i[CPO], int global_offset, int channel_offset, int o_channel) {
  set_writing_channel_offsets_loop:
  for(int i=0; i<CPO; i++){
    DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
    #pragma HLS UNROLL
    offset_write_data_channel_i[i] = global_offset + (channel_offset * i) + (o_channel * channel_offset);
    block_offset_write_data_channel_i[i] = offset_write_data_channel_i[i] % WRITE_BLOCK_SIZE;
  }
}

void set_channel_write_blocks(int num_channel_write_blocks[CPO], int addr[CPO], int H, int W) {
  set_channel_write_blocks_loop:
  for(int i=0; i<CPO; i++) {
    #pragma HLS UNROLL
	num_channel_write_blocks[i] = ((H * W) + (addr[i] % WRITE_BLOCK_SIZE) + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
  }
}


extern "C" {

void k_conv2D(ap_uint<512> *ptr_data, int H, int W, int rows, int I, int O, int I_ITER, int O_ITER, int enable_relu,
              data_type *ptr_kernel, pixel_out_t *ptr_bias, ap_uint<512> *ptr_out, int global_offset, int enable_upper_padding, int enable_lower_padding, int enable_maxpooling, int enable_avgpooling) {
	#pragma HLS INTERFACE m_axi port=ptr_data   depth=1024 offset=slave bundle=gmem
	#pragma HLS INTERFACE m_axi port=ptr_kernel depth=1024 offset=slave bundle=gmem1
	#pragma HLS INTERFACE m_axi port=ptr_bias   depth=1024 offset=slave bundle=gmem2
	#pragma HLS INTERFACE m_axi port=ptr_out    depth=1024 offset=slave bundle=gmem3

  #ifdef DEBUG_VERBOSE
  printf("kernel starts...\n");
  #endif

  o_iter_loop:
  for (int o_iter = 0; o_iter<O_ITER; o_iter++) {
	DO_PRAGMA(HLS loop_tripcount min=1 max=O_REFERENCE/CPO)
	#pragma HLS dataflow

	int o_channel = o_iter << LOG2_CPO;  // current output channel (first one in this iteration)

    // input and output streams
    static hls::stream<pixel_in_t>   out_read_data;
    static hls::stream<pixel_in_t>   out_read_data_2;
    DO_PRAGMA(HLS STREAM variable=out_read_data depth=32)
    DO_PRAGMA(HLS STREAM variable=out_read_data_2 depth=32)

	// DIRECT CONV, WINOGRAD, DWS
    #ifdef DIRECT_CONV
    static hls::stream<kernel_t>     out_read_kernel;
    DO_PRAGMA(HLS STREAM variable=out_read_kernel depth=CPO)
    #endif
    #ifdef WINOGRAD_CONV
    static hls::stream<kernel_t>     out_read_kernel;
    DO_PRAGMA(HLS STREAM variable=out_read_kernel depth=CPO)
    #endif
    #ifdef DWS_CONV
    static hls::stream<kernel_dw_t>     str_dw_kernel;
    static hls::stream<kernel_pw_t>     str_pw_kernel;
    #endif

    static hls::stream<pixel_out_t>  out_read_bias;
    static hls::stream<pixel_out_t>  out_conv;
    DO_PRAGMA(HLS STREAM variable=out_read_bias depth=CPO)
    DO_PRAGMA(HLS STREAM variable=out_conv depth=32)

	// RELU, CLIPPING, and SHIFT support
    #if defined(USE_RELU) || defined(USE_CLIPPING) || defined(USE_SHIFT)
    static hls::stream<pixel_out_t>  out_relu;
    DO_PRAGMA(HLS STREAM variable=out_relu depth=32)
    #endif

    // MAXPOOLING, AVGPOOLING
    #ifdef USE_POOLING
    static hls::stream<pixel_out_t>  out_pooling;
    DO_PRAGMA(HLS STREAM variable=out_pooling depth=32)
    #endif

    static hls::stream<read_block_t> stream_data_ch_0[CPI];
    static hls::stream<data_type>    stream_data_ch_1[CPI];
    static hls::stream<write_block_t> out_write_channel[CPO];
    DO_PRAGMA(HLS STREAM variable=stream_data_ch_0  depth=32)
    DO_PRAGMA(HLS STREAM variable=stream_data_ch_1  depth=32)
    DO_PRAGMA(HLS STREAM variable=out_write_channel depth=32)

    // variables
    int enable_write[CPO];
    int offset_read_data_channel_i[CPI];
    int offset_write_data_channel_i[CPO];
    int block_offset_write_data_channel_i[CPO];
    int num_channel_write_blocks[CPO];
    int corrected_offset         = (enable_upper_padding==0)? W : 0;
    int num_extra_rows           = (enable_lower_padding == 0) + (enable_upper_padding == 0);
    int offset_read_data_channel = global_offset - corrected_offset;
    int channel_size             = H * W;
    int read_pixels              = W * (rows + num_extra_rows);
    int enable_pooling           = enable_maxpooling | enable_avgpooling;

    int read_channel_offset      = (W * H);

    #ifdef USE_POOLING
    int write_pixels             = enable_pooling ? (rows * W / 4) : (rows * W);
    int write_channel_blocks     = (write_pixels + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
    int write_rows               = enable_pooling ? rows/2 : rows;
    int write_cols               = enable_pooling ? W/2 : W;
    int write_channel_offset     = enable_pooling ? (W * H) / 4 : (W * H);
    #else
    int write_pixels             = rows * W;
    int write_channel_blocks     = (write_pixels + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
    int write_rows               = rows;
    int write_cols               = W;
    int write_channel_offset     = (W * H);
    #endif

    int read_channel_blocks      = (read_pixels + READ_BLOCK_SIZE - 1) / READ_BLOCK_SIZE;
    int offset_bias              = o_iter;
    int offset_kernel            = o_iter * ((I + CPI - 1) / CPI) * CPI * CPO * 9;
    #pragma HLS array_partition variable=enable_write dim=0 complete
    DO_PRAGMA(HLS ARRAY_PARTITION variable=offset_read_data_channel_i dim=0 complete)
    DO_PRAGMA(HLS ARRAY_PARTITION variable=offset_write_data_channel_i dim=0 complete)
    DO_PRAGMA(HLS ARRAY_PARTITION variable=block_offset_write_data_channel_i dim=0 complete)
	DO_PRAGMA(HLS ARRAY_PARTITION variable=num_channel_write_blocks dim=0 complete)

    // we compute the enable_write signals
    set_write_enables(enable_write, o_channel, O);

    // channel offsets for reading
    set_reading_channel_offsets(offset_read_data_channel_i, offset_read_data_channel, read_channel_offset);

    // channel offsets for writing
    set_writing_channel_offsets(offset_write_data_channel_i, block_offset_write_data_channel_i, global_offset, write_channel_offset, o_channel);

    // channel write blocks
    set_channel_write_blocks(num_channel_write_blocks, offset_write_data_channel_i, H, W);

    read_bias(offset_bias, ptr_bias, out_read_bias);

    // read kernel (different version based on the type of convolution)
    #ifdef DIRECT_CONV
    read_kernel(I_ITER, offset_kernel, ptr_kernel, out_read_kernel);
    #endif
    #ifdef WINOGRAD_CONV
    read_kernel(I_ITER, offset_kernel, ptr_kernel, out_read_kernel);
    #endif
    #ifdef DWS_CONV
    dws_read_kernel(I_ITER, offset_kernel, ptr_kernel, str_dw_kernel, str_pw_kernel);
    #endif

    read_data_channels(H, W, rows, I_ITER, ptr_data, offset_read_data_channel, num_extra_rows, read_channel_blocks, stream_data_ch_0, I);
    ch_serialize_and_filter<CPI>(I_ITER, read_pixels, read_channel_blocks, channel_size, offset_read_data_channel_i, stream_data_ch_0, stream_data_ch_1, I);
    join(rows, W, I_ITER, num_extra_rows, stream_data_ch_1,  out_read_data);

    // convolution: Direct, Winograd, DWS
    #ifdef DIRECT_CONV
    direct_conv(rows, W, I_ITER, enable_upper_padding, enable_lower_padding, out_read_data, out_read_kernel, out_read_bias, out_conv);
    #endif
    #ifdef WINOGRAD_CONV
    winograd_conv(rows, W, I_ITER, enable_upper_padding, enable_lower_padding, out_read_data, out_read_kernel, out_read_bias, out_conv);
    #endif
    #ifdef DWS_CONV
    dws_conv(rows, W, I_ITER, enable_upper_padding, enable_lower_padding, out_read_data, str_dw_kernel, str_pw_kernel, out_read_bias, out_conv);
    #endif

    // Relu, Clipping, shift
    #if defined(USE_RELU) || defined(USE_CLIPPING) || defined(USE_SHIFT)
    int enable_clipping = 0;
    int enable_shift = 0;
    int min_clip = 0;
    int max_clip = 0;
    int direction_shift = 0;
    int pos_shift = 0;
    relu(enable_relu, enable_clipping, enable_shift, min_clip, max_clip, direction_shift, pos_shift, rows, W, out_conv, out_relu);

      // Pooling: avgpooling or maxpooling
      #ifdef USE_POOLING
      pooling(H, W, enable_maxpooling, enable_avgpooling, out_relu, out_pooling);
      split(write_rows, write_cols, block_offset_write_data_channel_i, write_channel_blocks, out_pooling, out_write_channel);
      #else
      split(write_rows, write_cols, block_offset_write_data_channel_i, write_channel_blocks, out_relu, out_write_channel);
      #endif

    #else
      // Pooling: avgpooling or maxpooling
      #ifdef USE_POOLING
      pooling(H, W, enable_maxpooling, enable_avgpooling, out_conv, out_pooling);
      split(write_rows, write_cols, block_offset_write_data_channel_i, write_channel_blocks, out_pooling, out_write_channel);
      #else
      split(write_rows, write_cols, block_offset_write_data_channel_i, write_channel_blocks, out_conv, out_write_channel);
      #endif
    #endif

    write_data_channels(write_pixels, ptr_out, offset_write_data_channel_i, out_write_channel, enable_write);

 }

 #ifdef DEBUG_VERBOSE
 printf("kernel finishes\n");
 #endif

}

} // extern "C"
