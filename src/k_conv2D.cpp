/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"
#include <hls_stream.h>
//#include "add_data.cpp"

int fn_get_read_b0(int read_from_b0, int read_from_mem, int input_fits_in_b0, int o_iter) {
	return read_from_b0 || (read_from_mem && input_fits_in_b0 && (o_iter != 0));
}

int fn_get_write_b0(int write_to_b0, int read_from_mem, int input_fits_in_b0, int o_iter) {
	return write_to_b0 || (read_from_mem && input_fits_in_b0 && (o_iter == 0));
}

#ifdef USE_UPSIZE
template <class in_st, class out_st> void upsize(int enable, int factor, int H, int W, hls::stream<in_st> &in, hls::stream<out_st> &out) {

  out_st buff[WMAX];

  #ifdef DEBUG_UPSIZE
  printf("UPSIZE: begin (factor %d)\n", factor);
  #endif

  if (enable) {
    for (int h = 0; h<H; h++) {
      DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE)
      for (int w = 0; w<W; w++) {
        DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
        in_st px;
        px = in.read();
        for (int cpo=0; cpo<CPO; cpo++) {
          DO_PRAGMA(HLS UNROLL)
          buff[w].pixel[cpo] = px.pixel[cpo];
        }
      }
      for (int r = 0; r < factor; r++) {
	DO_PRAGMA(HLS loop_tripcount min=1 max=4)
        for (int w = 0; w<W; w++) {
          DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
	  for (int rw = 0; rw < factor; rw++) {
	    DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
            out << buff[w];
            #ifdef DEBUG_UPSIZE
//            #ifdef DEBUG_VERBOSE
            printf("UPSIZE: h %d r %d w %d rw %d -> ", h, r, w, rw);
            for (int c=0; c<CPO; c++) printf(" %f ", buff[w].pixel[c]);
            printf("\n");
  //          #endif
            #endif
	  }
        }
      }
    }
  } else {
    for (int h = 0; h<H; h++) {
      DO_PRAGMA(HLS loop_tripcount min=1 max=H_REFERENCE)
      for (int w = 0; w<W; w++) {
        DO_PRAGMA(HLS loop_tripcount min=1 max=W_REFERENCE)
        in_st px;
        out_st px_out;
        px = in.read();
        for (int cpo=0; cpo<CPO; cpo++) {
          DO_PRAGMA(HLS UNROLL)
          px_out.pixel[cpo] = px.pixel[cpo];
        }
	out << px_out;
      }
    }
  }

  #ifdef DEBUG_UPSIZE
  printf("UPSIZE: end\n");
  #endif

}
#endif

extern "C" {
void k_conv2D(read_block_t *ptr_data, 
            #ifdef USE_ADD
            write_block_t *ptr_data_add, 
            #endif
            int H, int W, int HO, int WO, int rows, int PT, int PB, int PL, int PR, int SH, int SW, int I, int O, int I_ITER, int o_iter_first, int o_iter_last, 
            int enable_relu, int enable_stm, float relu_factor, 
            #ifdef USE_BATCH_NORM 
            int enable_batch_norm,
            #endif
            #ifdef USE_BATCH_NORM_RELU
            int enable_batch_norm_relu,
            float batch_norm_relu_factor,
            #endif
            #ifdef DIRECT_CONV
            read_filter_t *ptr_kernel,
            #endif
            #ifdef DWS_CONV
  					w_t *ptr_dw_kernel, w_pw_t *ptr_pw_kernel,
            #endif
            read_bias_st *ptr_bias,
            #ifdef USE_BATCH_NORM
            bnp_st *b_ptr, 
            #endif
            write_block_t *ptr_out, int read_offset, int write_offset, int enable_maxpooling, int enable_avgpooling, int enable_clipping, int enable_shift,
            #ifdef USE_ADD
            int enable_add, 
            #endif
            #ifdef USE_ADD_RELU
	    int apply_add_relu,
            #endif
            int min_clip, int max_clip, int dir_shift, int pos_shift, int upsize_factor, int write_to_weight_buffer, int read_from_weight_buffer, int first_row_weight_buffer,
			int read_from_mem, int read_from_b0, int read_from_b1, int write_to_mem, int write_to_b0, int write_to_b1) {

	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_data         depth=DATA_IN_PORT_DEPTH    offset=slave bundle=gmem)
    #ifdef DIRECT_CONV
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_kernel       depth=KERNEL_PORT_DEPTH     offset=slave bundle=gmem1)
    #endif
    #ifdef DWS_CONV
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_dw_kernel    depth=DW_KERNEL_PORT_DEPTH  offset=slave bundle=gmem1)
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_pw_kernel    depth=PW_KERNEL_PORT_DEPTH  offset=slave bundle=gmem4)
    #endif
    #ifdef USE_ADD
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_data_add     depth=DATA_IN_PORT_DEPTH    offset=slave bundle=gmem5)
    #endif
    #ifdef USE_BATCH_NORM
	DO_PRAGMA(HLS INTERFACE m_axi port=b_ptr            depth=BATCH_MORM_VAL_DEPTH  offset=slave bundle=gmem6)
    #endif
    DO_PRAGMA(HLS INTERFACE m_axi port=ptr_bias         depth=BIAS_PORT_DEPTH       offset=slave bundle=gmem2)
	DO_PRAGMA(HLS INTERFACE m_axi port=ptr_out          depth=DATA_OUT_PORT_DEPTH   offset=slave bundle=gmem3)

	DO_PRAGMA(HLS shared variable=I)
	DO_PRAGMA(HLS shared variable=O)
	DO_PRAGMA(HLS shared variable=I_ITER)
	DO_PRAGMA(HLS shared variable=o_iter_first)
	DO_PRAGMA(HLS shared variable=o_iter_last)
	DO_PRAGMA(HLS shared variable=enable_maxpooling)
	DO_PRAGMA(HLS shared variable=enable_avgpooling)
	DO_PRAGMA(HLS shared variable=rows)
	DO_PRAGMA(HLS shared variable=H)
	DO_PRAGMA(HLS shared variable=W)
	DO_PRAGMA(HLS stable variable=I)
	DO_PRAGMA(HLS stable variable=O)
	DO_PRAGMA(HLS stable variable=I_ITER)
	DO_PRAGMA(HLS stable variable=o_iter_first)
	DO_PRAGMA(HLS stable variable=o_iter_last)
	DO_PRAGMA(HLS stable variable=H)
	DO_PRAGMA(HLS stable variable=W)
	DO_PRAGMA(HLS stable variable=enable_maxpooling)
	DO_PRAGMA(HLS stable variable=enable_avgpooling)
	DO_PRAGMA(HLS stable variable=rows)
	DO_PRAGMA(HLS stable variable=SH)
	DO_PRAGMA(HLS stable variable=SW)
	DO_PRAGMA(HLS stable variable=relu_factor)

  #ifdef DEBUG_VERBOSE
  printf("kernel starts...\n");
  #endif

  #ifndef USE_ADD_RELU
  int apply_add_relu = 0;
  #endif


  int enable_upsize = (upsize_factor != 1);

  #ifndef USE_BATCH_NORM_RELU
  int enable_batch_norm_relu = 0;
  int batch_norm_relu_factor = 0;
  #endif

  // non dataflow variables
  int O_ITER = o_iter_last - o_iter_first + 1;                                                                           // number of output iterations to perform
                                                                                                                         // output convolution geometry:
  int HO_conv                  = (rows + PT + PB - KH + SH) / SH;                                                        // height: HO = ceil(H + padding - (KH-1) / SH)
  int WO_conv                  = (W + PL + PR - KW + SW) / SW;                                                           // width WO = ceil(H + padding - (KW-1) / SW)
  int num_output_conv_pixels   = HO_conv * WO_conv;                                                                      // number of output pixels per channel
  #ifdef USE_POOLING                                                                                                     // VARIABLES when POOLING is ENABLED
  int enable_pooling           = enable_maxpooling | enable_avgpooling;                                                  // pooling enable
  int HI_pooling               = HO_conv;                                                                                // input rows for pooling
  int WI_pooling               = WO_conv;                                                                                // input cols for pooling
  int write_pixels_tmp         = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv;                     // pixels to write per channel (before upsize)
  int read_pixels_add          = enable_pooling ? (HO_conv / 2) * (WO_conv / 2) : HO_conv * WO_conv;                     // pixels to read for add module (before upsize)
  int num_bn_pixels            = read_pixels_add;                                                                        // number of pixels for BN layer
  int num_add_pixels           = read_pixels_add;                                                                        // number of pixels for ADD layer
  int write_pixels             = enable_upsize ? write_pixels_tmp * upsize_factor * upsize_factor : write_pixels_tmp;               // pixels to write per channel (final, after upsize)
  int write_rows               = enable_pooling ? HO_conv / 2 : HO_conv;                                                 // number of rows of pixels for UPSIZE layer

  int write_cols               = enable_pooling ? WO_conv / 2 : WO_conv;                                                 // number of cols of pixels for UPSIZE layer
  #else
  int write_pixels             = HO_conv * WO_conv;
  if (enable_upsize) write_pixels = write_pixels * upsize_factor * upsize_factor;
  int read_pixels_add          = HO_conv * WO_conv;
  int num_bn_pixels            = read_pixels_add;
  int num_add_pixels           = read_pixels_add;
  int write_rows               = HO_conv;
  int write_cols               = WO_conv;
  #endif
  int read_pixels              = W * rows;                                                                               // number of pixels to read in one input iteration
  int read_pixels_total        = read_pixels * I_ITER;                                                                   // number of pixels to read (complete input)
  int offset_data_in_group_cpi = H * W;                                                                                  // input data offset for a CPI group (H * W)
  int offset_read_add_group_cpo = HO * WO;                                                                               // input ADD data offset for a CPI group (HO * WO)
  int offset_data_out_group_cpo = (enable_upsize) ?  HO * WO * upsize_factor * upsize_factor : HO * WO;                  // output data offset for a CPO group
                                                                                                                         // WEIGHTS BUFFER
  int enable_read_kernel        = !read_from_weight_buffer;                                                              // read enable for weight buffer (weights have been previously stored)
                                                                                                                         // DATA BUFFER 0 and DATA BUFFER 1
  int input_fits_in_b0         = (read_pixels_total <= DATA_BUFFER_SIZE);                                                // whether input buffer has enough capacity for the input
  int num_accesses_b0          = (read_from_mem || read_from_b0) ? read_pixels_total : write_pixels;                     // number of accesses (reads or writes) to buffer0
  int num_accesses_b1          = (read_from_b1) ? read_pixels_total : write_pixels;                                      // number of accesses (reads or writes) to buffer1
  int read_b1                  = read_from_b1;                                                                           // read enable for buffer 1
  int write_b1                 = write_to_b1;                                                                            // write to buffer 1 enable signal
  int write_to_obuf            = write_to_b0 || write_to_b1;                                                             // write to buffer signal (for WRITE module)
                                                                                                                         // MUX and DEMUX associated to DATA BUFFER 0 and DATA BUFFER 1
  int mux_accesses             = read_pixels_total;                                                                      // mux number of accesses
  int mux_sel                  = !(read_from_b0 || read_from_mem);                                                       // mux select signal (0 from buffer 0 and 1 from buffer 1)
  int demux_accesses           = write_pixels;                                                                           // demux number of accesses
  int demux_sel                = (write_to_b0) ? 0 : 1;                                                                  // demux select signal (0 to buffer 0 and 1 to buffer 1)
  int demux_enable             = !write_to_mem;                                                                          // demux enable signal
  int offset_read_data_channel = read_offset;                                                                            // offset for reading data channel

  o_iter_loop:
  for (int o_iter = 0; o_iter<O_ITER; o_iter++) {
	DO_PRAGMA(HLS loop_tripcount min=1 max=O_REFERENCE/CPO)
    #pragma HLS dataflow

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// streams
    static hls::stream<din_st>   out_read_data;                                                                          // read data stream from READ module
    static hls::stream<dout_st>  out_read_data_add;                                                                      // read data stream from READ_ADD module
    #ifdef DIRECT_CONV
    static hls::stream<w_t>      out_read_kernel;                                                                        // read filters stream from READ_KERNEL module (direct conv)
    static hls::stream<w2_st>    out_read_kernel_2;                                                                      // read filters stream from WEIGHT_BUFFER module (direct conv)
    static hls::stream<w_st>     out_read_kernel_3;                                                                      // read filters stream from PREPARE_WEIGHT_FILTERS module (direct conv)
    #endif
    #ifdef DWS_CONV
    static hls::stream<w_dw_st>  out_read_kernel_dw;                                                                     // read filters stream for depth wise conv (DWS conv)
    static hls::stream<w_pw_st>  out_read_kernel_pw;                                                                     // read filters stream for point wise conv (DWS conv)
    #endif
    static hls::stream<b_st>     out_read_bias;                                                                          // read bias stream from READ_BIAS module
    static hls::stream<conv_st>  out_conv;                                                                               // data output stream from CONVOLUTION module
    static hls::stream<relu_st>  out_relu;                                                                               // data output stream from RELU module
    static hls::stream<stm_st>   out_stm;                                                                                // data output stream from STM module
    #ifdef USE_POOLING
    static hls::stream<pool_st>  out_pooling;                                                                            // data output stream from POOLING module
    #endif
    static hls::stream<dout_st>  out_add;                                                                                // data output stream from ADD module
    #ifdef USE_UPSIZE
    static hls::stream<dout_st>  to_write;                                                                               // data output stream from UPSIZE module
    #endif
    static hls::stream<dout_st>  out_write;                                                                              // data output stream from WRITE module (to buffers via demux)
    static hls::stream<dout_st>  out_demux_0;                                                                            // data output stream from DEMUX module (output 0 to buffer 0)
    static hls::stream<dout_st>  out_demux_1;                                                                            // data output stream from DEMUX module (output 1 to buffer 1)
	static hls::stream<din_st>   out_buffer0;                                                                            // data output stream from BUFFER 0 module (to mux)
    static hls::stream<din_st>   out_buffer1;                                                                            // data output stream from BUFFER 1 module (to mux)
    static hls::stream<din_st>   out_mux;                                                                                // data output stream from MUX module
    #ifdef USE_BATCH_NORM
    static hls::stream<dout_st>  out_batch_norm;                                                                         // data output stream from BN module
    static hls::stream<bnp_st>   out_read_batch_norm;                                                                    // read data stream from READ_BN module
    #endif

    // stream depths
    DO_PRAGMA(HLS STREAM variable=out_read_data      depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_read_data_add  depth=STREAMS_DEPTH)
    #ifdef DIRECT_CONV
    DO_PRAGMA(HLS STREAM variable=out_read_kernel    depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_read_kernel_2  depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_read_kernel_3  depth=STREAMS_DEPTH)
    #endif
    #ifdef DWS_CONV
    DO_PRAGMA(HLS STREAM variable=out_read_kernel_dw depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_read_kernel_pw depth=STREAMS_DEPTH)
    #endif    
    DO_PRAGMA(HLS STREAM variable=out_read_bias      depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_conv           depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_relu           depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_stm            depth=STREAMS_DEPTH)
    #ifdef USE_POOLING
    DO_PRAGMA(HLS STREAM variable=out_pooling        depth=STREAMS_DEPTH)
    #endif
    DO_PRAGMA(HLS STREAM variable=out_add            depth=STREAMS_DEPTH)
    #ifdef USE_UPSIZE
    DO_PRAGMA(HLS STREAM variable=to_write           depth=STREAMS_DEPTH)
    #endif
    DO_PRAGMA(HLS STREAM variable=out_write          depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_demux_0        depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_demux_1        depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_buffer0        depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_buffer1        depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_mux            depth=STREAMS_DEPTH)
    #ifdef USE_BATCH_NORM
    DO_PRAGMA(HLS STREAM variable=out_batch_norm depth=STREAMS_DEPTH)
    DO_PRAGMA(HLS STREAM variable=out_read_batch_norm depth=STREAMS_DEPTH)
	#endif

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // variables (loop dependent)
	int o_channel                  = (o_iter + o_iter_first) * CPO; //<< LOG2_CPO;  // current output channel (first one in this iteration)
    int o_iter_read_add_offset     = write_offset + (offset_read_add_group_cpo * (o_iter + o_iter_first));
    int o_iter_write_offset        = write_offset + (offset_data_out_group_cpo * (o_iter + o_iter_first));
    int offset_bias                = o_iter + o_iter_first;
    int offset_kernel              = (o_iter + o_iter_first) * ((I + CPI - 1) / CPI) * CPI * CPO * 9;
    int offset_weight_buffer       = first_row_weight_buffer + (o_iter * I_ITER);     // offset weight buffer
    int read_b0                    = fn_get_read_b0(read_from_b0, read_from_mem, input_fits_in_b0, o_iter);
    int write_b0                   = fn_get_write_b0(write_to_b0, read_from_mem, input_fits_in_b0, o_iter);
    int read_from_input            = read_from_mem && !read_b0;
    int enable_read                = read_from_input;
    int first_buffer_write_address = write_pixels * o_iter;

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // modules
    read_bias(offset_bias, ptr_bias, out_read_bias);
    #ifdef DIRECT_CONV
    read_kernel(enable_read_kernel, I_ITER, offset_kernel, ptr_kernel, out_read_kernel);
    weight_buffer(I_ITER, write_to_weight_buffer, read_from_weight_buffer, offset_weight_buffer, out_read_kernel, out_read_kernel_2);
    prepare_weight_filters(I_ITER, out_read_kernel_2, out_read_kernel_3);
    #endif
    #ifdef DWS_CONV
    dws_read_dw_kernel(I_ITER, o_iter, ptr_dw_kernel, out_read_kernel_dw);  // o_iter as argument to load all kernels in the first iteration (o_iter==0)
    dws_read_pw_kernel(I_ITER, O, o_iter + o_iter_first, ptr_pw_kernel, out_read_kernel_pw); // o_iter+o_iter_ifrst sent to let the module compute the offset to read the kernels
    #endif
    read_data_channels_gihwcpi(read_pixels, offset_read_data_channel, I_ITER, offset_data_in_group_cpi, ptr_data, out_read_data, enable_read);
    buffer0(num_accesses_b0, read_b0, read_from_input, write_b0, first_buffer_write_address, out_read_data, out_demux_0, out_buffer0);
    buffer1(num_accesses_b1, read_b1, write_b1, first_buffer_write_address, out_demux_1, out_buffer1);
	mux(mux_accesses, mux_sel, out_buffer0, out_buffer1, out_mux);
    #ifdef USE_ADD
    read_input_add_gihwcpi(read_pixels_add, o_iter_read_add_offset, ptr_data_add, out_read_data_add, enable_add);
    #endif
    #ifdef USE_BATCH_NORM
    read_batch_norm(offset_bias, b_ptr, out_read_batch_norm);
    #endif
    #ifdef DIRECT_CONV
    direct_conv(rows, W, PT, PB, PL, PR, SH, SW, num_output_conv_pixels, I_ITER, out_mux, out_read_kernel_3, out_read_bias, out_conv);
    #endif
    #ifdef DWS_CONV
    dws_conv(rows, W, PT, PB, PL, PR, SH, SW, num_output_conv_pixels, I_ITER, out_mux, out_read_kernel_dw, out_read_kernel_pw, out_read_bias, out_conv);
    #endif
    relu(enable_relu, enable_clipping, enable_shift, relu_factor, min_clip, max_clip, dir_shift, pos_shift, num_output_conv_pixels, out_conv, out_relu);
    stm(enable_stm, num_output_conv_pixels, out_relu, out_stm); 
    pooling(HI_pooling, WI_pooling, enable_maxpooling, enable_avgpooling, out_stm, out_pooling);
    #ifdef USE_BATCH_NORM
      batch_norm(enable_batch_norm, num_bn_pixels, enable_batch_norm_relu, batch_norm_relu_factor, out_pooling, out_read_batch_norm, out_batch_norm);
      #ifdef USE_ADD
        add_data<dout_st, dout_st, dout_st>(enable_add, num_add_pixels, apply_add_relu, out_read_data_add, out_batch_norm, out_add); 
        upsize<dout_st, dout_st>(enable_upsize, upsize_factor, write_rows, write_cols, out_add, to_write);
      #else
        upsize<pool_st, dout_st>(enable_upsize, upsize_factor, write_rows, write_cols, out_pooling, to_write);
      #endif
    #else
      #ifdef USE_ADD
        add_data<dout_st, pool_st, dout_st>(enable_add, num_add_pixels, apply_add_relu, out_read_data_add, out_pooling, out_add); 
        upsize<dout_st, dout_st>(enable_upsize, upsize_factor, write_rows, write_cols, out_add, to_write);
      #else
        upsize<pool_st, dout_st>(enable_upsize, upsize_factor, write_rows, write_cols, out_pooling, to_write);
      #endif
    #endif
    write_data_channels_gihwcpi(write_pixels, o_iter_write_offset, ptr_out, to_write, write_to_obuf, out_write);
    demux(demux_enable, demux_accesses, demux_sel, out_write, out_demux_0, out_demux_1);

 } // end o_iter

 #ifdef DEBUG_VERBOSE
 printf("kernel finishes\n");
 #endif

}

} // extern "C"
