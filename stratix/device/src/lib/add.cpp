#include "lib_hdrs_priv.h"



// -------------------------------------------------------------------------------
// add: This function performs the addition of all subpixels for the same channel.
// It adds also the corresponding bias.
//
// Arguments:
//   H     : Height of input channel
//   W     : Width of input channel
//   I_ITER: Number of input iterations (I / CPI)
//   in    : input streams data
//   b_in  : input stream bias
//   out   : output stream
//
unsigned long add(int o_iter, int H, int W, int I_ITER) {

  //#ifdef DEBUG_ADD
  //printf("add: start\n");
  //#endif

  unsigned long cnt = 0;

  pixel_out_t bias;
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=bias dim=0 complete)

  // number of iterations by CPI || CPO channels
  int num_iterations = W * H;

  // Buffer for all data and CPO channels
   pixel_out_t buff_o_channels[WMAX*HMAX];
  //DO_PRAGMA(HLS AGGREGATE variable=buffer_o_channels)
  //#ifdef ALVEO_U200
  //DO_PRAGMA(HLS bind_storage variable=buffer_o_channels type=ram_t2p impl=uram)
  //#endif


  // We receive bias in packs of CPO
  bias = out_read_bias.read();

  //#ifdef DEBUG_ADD
  //#ifdef DEBUG_VERBOSE
  //for (int b=0; b<CPO; b++) {
  //  printf("Bias[%d] = %6.4f \n", b, float(bias.pixel[b]));
  //}
  //printf("add: bias received\n");
  //for(int cpo = 0; cpo<CPO; cpo++){
  //  printf("Channel cpo = %d: ", cpo);
  //  for(int it = 0; it<num_iterations; it++){
  //    printf("%6.2f ", float(buff_o_channels[it].pixel[cpo]));
  //  }
  //  printf("\n");
  //}
  //#endif
  //#endif

/*
  //// JM10 debe inicializarse a zero en cada i_iter ??
  pixel_out_t pout_zero;
  for (int i = 0; i < CPO; i++) {
     pout_zero.pixel[i] = (data_type)0;  
  }
  // JM10 debe inicializarse a zero  ??
  for(int it = 0; it<num_iterations; it++){ 
    buff_o_channels[it] = pout_zero;
  } 
*/
  // All input data have effect into output add
  add_i_iter_loop:
  for (int i_iter = 0; i_iter < I_ITER; i_iter++){
    //DO_PRAGMA(HLS loop_tripcount  min=1 max=I_REFERENCE/CPI)
    //pixel_out_t data_out;

    //#pragma HLS loop_flatten off
    add_load_data_it_loop:
    for(int it = 0; it<num_iterations; it++){
      //DO_PRAGMA(HLS loop_tripcount  min=1 max=W_REFERENCE*H_REFERENCE)
      pixel_out_t px;
      px = str_mul_add.read();
      pixel_out_t data_in;
      pixel_out_t data_out;

      if (i_iter == 0) data_in = bias; else data_in = buff_o_channels[it];

      add_load_data_cpo_loop:
      #pragma unroll
      for (int cpo=0; cpo<CPO; cpo++) {
        //DO_PRAGMA(HLS loop_tripcount  min=1 max=CPO)
        //#pragma HLS unroll
        data_out.pixel[cpo] = data_in.pixel[cpo] + px.pixel[cpo];
      }
      buff_o_channels[it] = data_out;

      if(i_iter ==(I_ITER-1)){
        out_conv.write(data_out);

        #ifdef HLS_DEBUG
        if(o_iter == HLS_O_ITER_MONITOR)
        {
          dbg_loop_stream_data_directconv_out.write(data_out);
        }
        cnt = cnt + 1;
        //dbg_loop_stream_data_dc_add_out_counter = dbg_loop_stream_data_dc_add_out_counter + 1;
        //dbg_elements_per_iter_data_dc_add_out[o_iter] = dbg_elements_per_iter_data_dc_add_out[o_iter] + 1;
        #endif
      }
    }
  } //i_iter

  //#ifdef DEBUG_ADD
  //#ifdef DEBUG_VERBOSE
  //for (int cpo=0; cpo<CPO; cpo++) {
  //  printf("CH %d: ", cpo);
  //  for (int it=0; it<num_iterations; it++) {
  //    printf("%6.2f ", float(buff_o_channels[it].pixel[cpo]));
  //  }
  //  printf("\n");
  //}
  //#endif
  //#endif

  //#ifdef DEBUG_ADD
  //printf("add: end\n");
  //#endif
  
  return cnt;
}


