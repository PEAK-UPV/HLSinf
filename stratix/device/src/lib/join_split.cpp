#include "lib_hdrs_priv.h"



#ifdef IHW_DATA_FORMAT
// ---------------------------------------------------------------------------------------
// join. Joins input streams of pixels and combines them to produce groups of pixels
//
// Arguments:
//   H, W                : Data channel height and width
//   I_ITER              : Number of input iterations (I / CPI)
//   in                  : input streams
//   out                 : output stream
//
// The input streams have width of BLOCK_SIZE elements whereas the output stream
// has width of CPI elements. This module gets the first elements of all input
// streams and produces an output data, then it takes the second elements of all
// input streams and produces a new output data, and so on... For every received
// input data from all streams the join module uses BLOCK_SIZE cycles to produce
// BLOCK_SIZE data items. All data items are sent through the output stream
//
void join(int H, int W, int I_ITER, int num_extra_rows, int enable, ihc::stream<data_type> in[CPI], ihc::stream<pixel_in_t> &out) {

  //#ifdef DEBUG_JOIN
  //printf("JOIN: starts\n");
  //#endif

  if (!enable) return;

  // input buffer
  pixel_in_t data;
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=data complete dim=0)

  int num_pixels = (H + num_extra_rows) * W;                    // pixels to read

  //#ifdef DEBUG_JOIN
  //printf("JOIN: Expected pixels = %d\n", num_pixels);
  //#endif

  for (int i_iter = 0; i_iter < I_ITER; i_iter++) {
	//DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=I_REFERENCE/CPI)

    join_loop:
    for (int r=0; r<num_pixels; r++) {
      //DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
      //#pragma HLS PIPELINE II=1

      #pragma unroll
      for(int i=0; i<CPI; i++){
        //DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
        //#pragma HLS UNROLL
        data.pixel[i] = in[i].read();
      }
      //#ifdef DEBUG_JOIN
      //#ifdef DEBUG_VERBOSE
      //#endif
      //#endif

      out.write(data);

    }
  }

  //#ifdef DEBUG_JOIN
  //printf("JOIN: ends\n");
  //#endif
}
#endif


pixel_in_t buffer_din[INPUT_BUFFER_SIZE];
unsigned long input_buffer(int o_iter, int read_pixels_total, int write_to_buff, int read_from_buff) {

  //#ifdef DEBUG_INPUT_BUFFER
  //printf("INPUT_BUFFER: starts (%d pixels; write_to_buff %d; read_from_buff %d)\n", read_pixels_total, write_to_buff, read_from_buff);
  //#endif
  unsigned long cnt = 0;

  //DO_PRAGMA(HLS aggregate variable=buffer)

  #ifdef ALVEO_U200
  //DO_PRAGMA(HLS bind_storage variable=buffer type=ram_t2p impl=uram)
  #endif

  input_buffer_loop_pixels:
  for (int p=0; p<read_pixels_total; p++) {

    pixel_in_t px_input;
    pixel_in_t px_buff;
    //DO_PRAGMA(HLS AGGREGATE variable=px_input)
    //DO_PRAGMA(HLS AGGREGATE variable=px_buff)

  
    //DO_PRAGMA(HLS loop_tripcount min=1 max=I_REFERENCE * W_REFERENCE * H_REFERENCE / CPI)
    //  DO_PRAGMA(HLS pipeline)
    
    if (read_from_buff) {
      px_buff = buffer_din[p];
    }
    else {
      px_input = out_read_data.read();
      if (write_to_buff) {
        buffer_din[p] = px_input;
      }
    }

    if (read_from_buff) {
      out_read_data_from_input_buffer.write(px_buff);
      #ifdef HLS_DEBUG //#ifdef laguasa 
      if(o_iter == HLS_O_ITER_MONITOR)  
      {
        dbg_loop_stream_data_input_buffer.write(px_buff); 
      }
      cnt = cnt + 1;
      #endif
    } else {
      out_read_data_from_input_buffer.write(px_input);
      #ifdef HLS_DEBUG //#ifdef laguasa
      if(o_iter == HLS_O_ITER_MONITOR) 
      {
        dbg_loop_stream_data_input_buffer.write(px_input);
      }
      cnt = cnt + 1;
      
      #endif
    }
  }
  //#ifdef DEBUG_INPUT_BUFFER
  //printf("INPUT_BUFFER: ends\n");
  //#endif
  
  return cnt;
}

#ifdef IHW_DATA_FORMAT
// ---------------------------------------------------------------------------------------
// split. Splits incoming pixels grouped in pixel_out_t struct into eight output streams
// of size BLOCK_SIZE elements each.
//
// Arguments:
//   H, W                : data channel height and width
//   in                  : input stream
//   out0, ... out7      : output streams
//
// The input stream has CPO pixels per data item whereas each output stream has
// BLOCK_SIZE pixels per data item. Therefore, this module reads during BLOCK_SIZE
// cycles the input stream and assigns each pixel from each read data item into
// every output data item to be sent. After those cycles the out data items are
// sent through the corresponding output stream
//
void split(int H, int W, ihc::stream<pixel_out_t> &in, ihc::stream<data_type> out[CPO]) {

  //#ifdef DEBUG_SPLIT
  //printf("DEBUG_SPLIT: starts\n");
  //#endif

  int num_pixels = H * W;                                       // pixels to receive per channel
  pixel_out_t data;												// received pixels
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=data        complete dim=0)

  // We read input pixels and forward blocks
  split_loop_pixels:
  for (int r=0; r<num_pixels; r++) {
    //DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE * H_REFERENCE)
    //#pragma HLS PIPELINE

	// We read the pixels
    data = in.read();

    split_loop_cpo_1:
    #pragma unroll
    for(int cpo=0; cpo<CPO; cpo++) {
      //DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
      //#pragma HLS UNROLL

  	  data_type datum = data.pixel[cpo];

      out[cpo].write(datum);
    }
  }

  //#ifdef DEBUG_SPLIT
  //printf("DEBUG_SPLIT: starts\n");
  //#endif

}

void block_generate(int H, int W, ihc::stream<data_type> &in, ihc::stream<write_block_t> &out) {

//#ifdef DEBUG_BLOCK
//printf("DEBUG_BLOCK: starts\n");
//#endif

  write_block_t bx;
  int last = (WRITE_BLOCK_SIZE * DATA_TYPE_WIDTH) - 1;
  int first = (WRITE_BLOCK_SIZE-1) * DATA_TYPE_WIDTH;

  int num_pixels = H * W;
  int num_blocks = (num_pixels + WRITE_BLOCK_SIZE - 1) / WRITE_BLOCK_SIZE;
  block_generate_blocks:
  for (int b=0; b < num_blocks; b++) {
	//DO_PRAGMA(HLS loop_tripcount min=1 max=(W_REFERENCE * H_REFERENCE) / WRITE_BLOCK_SIZE)

    block_generate_pixels:
    for (int p=0; p < WRITE_BLOCK_SIZE; p++) {
	  //DO_PRAGMA(HLS PIPELINE)
	  data_type dx;
	  if (num_pixels) dx = in.read();
      //#ifdef DEBUG_BLOCK
      //#ifdef DEBUG_VERBOSE
      //printf("read pixel %f, pending %d\n", float(dx), num_pixels);
      //#endif
      //#endif
      bx.range(last, first) = *(ap_uint<DATA_TYPE_WIDTH>*)(&dx);
      if (p != WRITE_BLOCK_SIZE-1) bx = bx >> DATA_TYPE_WIDTH;
	  if (num_pixels) num_pixels--;
    }
    out.write(bx);
  }

//#ifdef DEBUG_BLOCK
//printf("DEBUG_BLOCK: ends\n");
//#endif

}
#endif


