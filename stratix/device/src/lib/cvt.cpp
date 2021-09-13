#include "lib_hdrs_priv.h"

// ---------------------------------------------------------------------------------------------------
// cvt: reads an input stream with an image of format (H, W, CPI) and writes an output stream
// in a 2D format based on (KW, KH). (SW=1, SH=1) stride is assumed and (PW=1, PH=1) padding is assumed.
// The function outputs data in the format (KH, KW, CPI).
//
// Arguments:
//   H      : Height of input channel
//   W      : Width of input channel
//   I_ITER : Number of input iterations (I / CPI)
//   in     : input stream (format pixel_in_t)
//   out    : output stream (format frame_t)
//

// changes log:
//  hls::stream     ihc::stream
// no prinf support
unsigned long cvt(int o_iter, int H, int W, int I_ITER) {

  //#ifdef DEBUG_CVT
  //printf("cvt: start\n");
  //#endif
  unsigned long cnt = 0;

  int HH=H+2;
  int WW=W+2;
  // buffers (keep three rows)
  pixel_in_t buffer0[WMAX+2];
  pixel_in_t buffer1[WMAX+2];
  pixel_in_t buffer2[WMAX+2];
  //frame_t frame;

  int pin_row = 0;
  int pin_col = 0;
  int row0_buffer_write = 0;
  int row1_buffer_write = 0;
  int row2_buffer_write = 0;
  int num_pixels = HH * WW;
  //DO_PRAGMA(HLS ARRAY_PARTITION variable=frame complete dim=0)
  //DO_PRAGMA(HLS AGGREGATE variable=buffer0)
  //DO_PRAGMA(HLS AGGREGATE variable=buffer1)
  //DO_PRAGMA(HLS AGGREGATE variable=buffer2)

  // manually flattened loop (for the purposes of getting the expected pipelined design)
  int p = 0;
  int num_iters = I_ITER * num_pixels;

  // build the frame
  pixel_in_t p0, p1, p2, p3, p4, p5, p6, p7, p8;

  
  cvt_loop:
  for(int i_iter = 0; i_iter < num_iters; i_iter++){

    //DO_PRAGMA(HLS loop_tripcount  min=1 max=(I_REFERENCE/CPI) * (H_REFERENCE+2)*(W_REFERENCE+2))
    //DO_PRAGMA(HLS PIPELINE II=1)

    if (p==0) {
	  pin_row = 0;
	  pin_col = 0;
	  row0_buffer_write = 1;
	  row1_buffer_write = 0;
	  row2_buffer_write = 0;
    }

    int pin_col0 = (pin_col==0);
    int pin_col1 = (pin_col==1);

    // get the pixel
    pixel_in_t pixel;
    //DO_PRAGMA(HLS ARRAY_PARTITION variable=pixel complete dim=0)
    pixel = str_pad_cvt.read();


    // row buffer write (in which buffer row we write the pixel)
    // first row buffer
    int row0 = (pin_row <= 2) | ((pin_row % 3) == 2);
    int row1 = !row0 & ((pin_row % 3) == 0);

    // we write the pixel into the buffer
    if (row0_buffer_write) buffer0[pin_col] = pixel;
    if (row1_buffer_write) buffer1[pin_col] = pixel;
    if (row2_buffer_write) buffer2[pin_col] = pixel;



    int shift_frame = (pin_row>1) & (pin_col > 2);
    int send_frame = (pin_row>1) & (pin_col > 1);
    pixel_in_t pixel_b0, pixel_b1, pixel_b2;
    pixel_b0 = buffer0[pin_col];
    pixel_b1 = buffer1[pin_col];
    pixel_b2 = buffer2[pin_col];

    // p0, p1, p2
    if (shift_frame) {p0 = p1;} else if (pin_col0) {if (row0) p0 = pixel_b0; else if (row1) p0 = pixel_b1; else p0 = pixel_b2;}
    if (shift_frame) {p1 = p2;} else if (pin_col1) {if (row0) p1 = pixel_b0; else if (row1) p1 = pixel_b1; else p1 = pixel_b2;}
    if (row0) p2 = pixel_b0; else if (row1) p2 = pixel_b1; else p2 = pixel_b2;
    // p3, p4, p5
    if (shift_frame) {p3 = p4;} else if (pin_col0) {if (row0) p3 = pixel_b1; else if (row1) p3 = pixel_b2; else p3 = pixel_b0;}
    if (shift_frame) {p4 = p5;} else if (pin_col1) {if (row0) p4 = pixel_b1; else if (row1) p4 = pixel_b2; else p4 = pixel_b0;}
    if (row0) p5 = pixel_b1; else if (row1) p5 = pixel_b2; else p5 = pixel_b0;
    // p6, p7, p8
    if (shift_frame) {p6 = p7;} else if (pin_col0) {if (row0) p6 = pixel_b2; else if (row1) p6 = pixel_b0; else p6 = pixel_b1;}
    if (shift_frame) {p7 = p8;} else if (pin_col1) {if (row0) p7 = pixel_b2; else if (row1) p7 = pixel_b0; else p7 = pixel_b1;}
    if (row0) p8 = pixel_b2; else if (row1) p8 = pixel_b0; else p8 = pixel_b1;
    if (send_frame) {
      frame_t frame;
      frame.pixel[0] = p0; frame.pixel[1] = p1; frame.pixel[2] = p2;
      frame.pixel[3] = p3; frame.pixel[4] = p4; frame.pixel[5] = p5;
      frame.pixel[6] = p6; frame.pixel[7] = p7; frame.pixel[8] = p8;
      str_cvt_mul.write(frame);

      #ifdef HLS_DEBUG
      if(o_iter == HLS_O_ITER_MONITOR)
      {
        dbg_loop_stream_data_dc_cvt_out.write(frame);
      }
      cnt = cnt + 1;
      //dbg_loop_stream_data_dc_cvt_out_counter = dbg_loop_stream_data_dc_cvt_out_counter + 1;
      //dbg_elements_per_iter_data_dc_cvt_out[o_iter] = dbg_elements_per_iter_data_dc_cvt_out[o_iter] + 1;
      #endif

      //#ifdef DEBUG_CVT
      //#ifdef DEBUG_VERBOSE
      //printf("cvt: frame sent:\n");
      //for (int cpi=0; cpi<CPI; cpi++) {
      //  printf("  cpi %d:\n", cpi);
      //  printf("    %6.4f %6.4f %6.4f\n", float(frame.pixel[0].pixel[cpi]), float(frame.pixel[1].pixel[cpi]), float(frame.pixel[2].pixel[cpi]));
      //  printf("    %6.4f %6.4f %6.4f\n", float(frame.pixel[3].pixel[cpi]), float(frame.pixel[4].pixel[cpi]), float(frame.pixel[5].pixel[cpi]));
      //  printf("    %6.4f %6.4f %6.4f\n", float(frame.pixel[6].pixel[cpi]), float(frame.pixel[7].pixel[cpi]), float(frame.pixel[8].pixel[cpi]));
      //}
      //#endif
      //#endif
    }

    #ifdef HLS_DEBUG
    {
      if(o_iter == HLS_O_ITER_MONITOR) {
        hls_cvt_sbs_control_t build_ctrl;
        build_ctrl.iter    = i_iter;
        build_ctrl.pin_row = pin_row;
        build_ctrl.pin_col = pin_col;
        build_ctrl.row0_buffer_write = row0_buffer_write;
        build_ctrl.row1_buffer_write = row1_buffer_write;
        build_ctrl.row2_buffer_write = row2_buffer_write;
        build_ctrl.p        = p;
        build_ctrl.pin_col0 = pin_col0;
        build_ctrl.pin_col1 = pin_col1;
        build_ctrl.row0     = row0;
        build_ctrl.row1     = row1;
        build_ctrl.shift_frame = shift_frame;
        build_ctrl.send_frame  = send_frame;
        dbg_loop_stream_data_dc_cvt_sbs_control_out.write(build_ctrl);

        frame_t  build_frame;
        build_frame.pixel[0] = p0; build_frame.pixel[1] = p1; build_frame.pixel[2] = p2;
        build_frame.pixel[3] = p3; build_frame.pixel[4] = p4; build_frame.pixel[5] = p5;
        build_frame.pixel[6] = p6; build_frame.pixel[7] = p7; build_frame.pixel[8] = p8;
        dbg_loop_stream_data_dc_cvt_sbs_frame_out.write(build_frame);
      }
    }
    #endif


    pin_col++;
    int pin_col_curr = pin_col;
    if (pin_col_curr == WW) {
   	  pin_col = 0;
      pin_row++;
      // row buffer write rotation
      if (row0_buffer_write) {
      	row0_buffer_write = 0;
      	row1_buffer_write = 1;
      } else if (row1_buffer_write) {
      	row1_buffer_write = 0;
      	row2_buffer_write = 1;
      } else {
      	row2_buffer_write = 0;
      	row0_buffer_write = 1;
      }
    }
    p = p + 1;
    if (p == num_pixels) p = 0;
   
  } //i_iter

  //#ifdef DEBUG_CVT
  //printf("cvt: end\n");
  //#endif

  
  return cnt;
}