/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

// ---------------------------------------------------------------------------------------------------
// cvt: reads an input stream with an image of format (H, W, CPI) and writes an output stream
// in a 2D format based on (KW, KH) and (SH, SW). (PW=1, PH=1) padding is assumed.
// The function outputs data in the format (KH, KW, CPI).
//
// Arguments:
//   H      : Height of input channel
//   W      : Width of input channel
//   I_ITER : Number of input iterations (I / CPI)
//   SH     : Vertical stride
//   SW     : Horizontal stride
//   in     : input stream (format pixel_in_t)
//   out    : output stream (format frame_t)
//
void cvt(int H, int W, int I_ITER, int SH, int SW, hls::stream<din_st> &in, hls::stream<conv_cvt_st> &out) {

  #ifdef DEBUG_CVT
  printf("cvt: start\n");
  printf("  H: %d, W: %d, I_ITER: %d, SH: %d, SW: %d\n", H, W, I_ITER, SH, SW);
  #endif

  int HH=H;
  int WW=W;
  // buffers (keep three rows)
  din_st buffer0[WMAX+2];
  din_st buffer1[WMAX+2];
  din_st buffer2[WMAX+2];
  conv_cvt_st frame;
  int pin_row;
  int pin_col;
  int stride_row;
  int stride_col;
  int row0_buffer_write;
  int row1_buffer_write;
  int row2_buffer_write;
  int num_pixels = HH * WW;
  DO_PRAGMA(HLS ARRAY_PARTITION variable=frame complete dim=0)
  DO_PRAGMA(HLS AGGREGATE variable=buffer0)
  DO_PRAGMA(HLS AGGREGATE variable=buffer1)
  DO_PRAGMA(HLS AGGREGATE variable=buffer2)
  #ifdef ALVEO_U200
  DO_PRAGMA(HLS bind_storage variable=buffer0 type=ram_s2p impl=bram)
  DO_PRAGMA(HLS bind_storage variable=buffer1 type=ram_s2p impl=bram)
  DO_PRAGMA(HLS bind_storage variable=buffer2 type=ram_s2p impl=bram)
  #endif
  #ifdef ALVEO_U280
  DO_PRAGMA(HLS bind_storage variable=buffer0 type=ram_s2p impl=bram)
  DO_PRAGMA(HLS bind_storage variable=buffer1 type=ram_s2p impl=bram)
  DO_PRAGMA(HLS bind_storage variable=buffer2 type=ram_s2p impl=bram)
  #endif

  // manually flattened loop (for the purposes of getting the expected pipelined design)
  int p = 0;
  int num_iters = I_ITER * num_pixels;
  cvt_loop:
  for(int i_iter = 0; i_iter < num_iters; i_iter++){
    DO_PRAGMA(HLS loop_tripcount  min=1 max=(I_REFERENCE/CPI) * (H_REFERENCE+2)*(W_REFERENCE+2))
    DO_PRAGMA(HLS PIPELINE II=1)

    if (p==0) {
	  pin_row = 0;
	  pin_col = 0;
	  row0_buffer_write = 1;
	  row1_buffer_write = 0;
	  row2_buffer_write = 0;
	  stride_row = 2;
	  stride_col = 2;
    }

    int pin_col0 = (pin_col==0);
    int pin_col1 = (pin_col==1);

    // get the pixel
    din_st pixel;
    DO_PRAGMA(HLS ARRAY_PARTITION variable=pixel complete dim=0)
    pixel = in.read();
#ifdef DEBUG_CVT
    printf("CVT: read data (i %d pin_row %d pin_col %d stride_row %d stride_col %d):", i_iter, pin_row, pin_col, stride_row, stride_col);
    for (int x=0; x<CPI; x++) printf(" %f", float(pixel.pixel[x]));
    printf("\n");
#endif

    // row buffer write (in which buffer row we write the pixel)
    // first row buffer
    int row0 = (pin_row <= 2) | ((pin_row % 3) == 2);
    int row1 = !row0 & ((pin_row % 3) == 0);

    // we write the pixel into the buffer
    if (row0_buffer_write) buffer0[pin_col] = pixel;
    if (row1_buffer_write) buffer1[pin_col] = pixel;
    if (row2_buffer_write) buffer2[pin_col] = pixel;

    // build the frame
    din_st p0, p1, p2, p3, p4, p5, p6, p7, p8;

    int shift_frame = (pin_row>1) & (pin_col > 2);
    int send_frame = (pin_row>1) & (pin_col > 1) & (stride_row == SH) & (stride_col == SW);
    //printf("pin_row %d pin_col %d stride_row %d stride_col %d send_frame %d\n", pin_row, pin_col, stride_row, stride_col, send_frame);
    din_st pixel_b0, pixel_b1, pixel_b2;
    pixel_b0 = buffer0[pin_col];
    pixel_b1 = buffer1[pin_col];
    pixel_b2 = buffer2[pin_col];
    pin_col++;
    if (stride_col == 1) stride_col = SW; else stride_col = stride_col - 1;
    int pin_col_curr = pin_col;
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
      frame.pixel[0] = p0; frame.pixel[1] = p1; frame.pixel[2] = p2;
      frame.pixel[3] = p3; frame.pixel[4] = p4; frame.pixel[5] = p5;
      frame.pixel[6] = p6; frame.pixel[7] = p7; frame.pixel[8] = p8;
      out << frame;
      #ifdef DEBUG_CVT
      #ifdef DEBUG_VERBOSE
      printf("cvt: frame sent:\n");
      for (int cpi=0; cpi<CPI; cpi++) {
        printf("  cpi %d:\n", cpi);
        printf("    %6.4f %6.4f %6.4f\n", float(frame.pixel[0].pixel[cpi]), float(frame.pixel[1].pixel[cpi]), float(frame.pixel[2].pixel[cpi]));
        printf("    %6.4f %6.4f %6.4f\n", float(frame.pixel[3].pixel[cpi]), float(frame.pixel[4].pixel[cpi]), float(frame.pixel[5].pixel[cpi]));
        printf("    %6.4f %6.4f %6.4f\n", float(frame.pixel[6].pixel[cpi]), float(frame.pixel[7].pixel[cpi]), float(frame.pixel[8].pixel[cpi]));
      }
      #endif
      #endif
    }
    if (pin_col_curr == WW) {
   	  pin_col = 0;
          pin_row++;
	  stride_col = 2;
	  if (stride_row == 1) stride_row = SH; else stride_row = stride_row - 1;
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

  #ifdef DEBUG_CVT
  printf("cvt: end\n");
  #endif
}
