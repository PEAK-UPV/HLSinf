#include "lib_hdrs_priv.h"

// ---------------------------------------------------------------------------------------
// padding. Adds padding to the input and forwards it through the output
//
// Arguments:
//   H                 : Height of input channel
//   W                 : Width of input channel
//   I_ITER            : Number of input iterations (I / CPI)
//   in                : input stream
//   out               : output stream
//
unsigned long padding(int o_iter, int H, int W, int I_ITER, int enable_upper_padding, int enable_lower_padding) {

  //#ifdef DEBUG_PADDING
  //printf("PADDING: start\n");
  //#endif
  unsigned long cnt = 0;

  int num_iters;
  int h;
  int w;
  pixel_in_t data;
  pixel_in_t zero;
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=data complete)
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=zero complete)

  padding_cpi_loop:
  #pragma unroll
  for (int cpi=0; cpi<CPI; cpi++){
    //DO_PRAGMA(HLS loop_tripcount  min=1 max=CPI)
    //#pragma HLS UNROLL
    zero.pixel[cpi] = 0.f;
  }

  num_iters = I_ITER * (H + 2) * (W + 2);
  h = 0;
  w = 0;
  padding_loop:
  for (int i = 0; i < num_iters; i++) {
    //DO_PRAGMA(HLS loop_tripcount min=1 max=(I_REFERENCE/CPI) * W_REFERENCE * H_REFERENCE)
    //#pragma HLS pipeline II=1
    int enable1 = enable_upper_padding & (h==0);
    int enable2 = enable_lower_padding & (h == H+1);
    int enable3 = (w == 0);
    int enable4 = (w == W+1);
    
    if (enable1 | enable2 | enable3 | enable4) 
      data = zero; 
    else
      data = out_read_data_from_input_buffer.read();
    
    str_pad_cvt.write(data);

    #ifdef HLS_DEBUG 
    if(o_iter == HLS_O_ITER_MONITOR)
    {
      dbg_loop_stream_data_dc_pad_out.write(data);
    }
    //dbg_loop_stream_data_dc_pad_out_counter = dbg_loop_stream_data_dc_pad_out_counter + 1;
    //dbg_elements_per_iter_data_dc_pad_out[o_iter] = dbg_elements_per_iter_data_dc_pad_out[o_iter] + 1;
    cnt = cnt + 1;
    #endif
    
    w = w+1;
    if (w == W+2) {
      w = 0;
      h = h + 1;
      if (h == H+2) {
        h = 0;
      }
    }
  }

  //#ifdef DEBUG_PADDING
  //printf("PADDING: end\n");
  //#endif
  return cnt;
}
