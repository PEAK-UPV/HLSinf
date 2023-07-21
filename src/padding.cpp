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
// padding. Adds padding to the input and forwards it through the output
//
// Arguments:
//   H                 : Height of input channel
//   W                 : Width of input channel
//   PT				   : Top padding
//   PB				   : Bottom padding
//   PL			       : Left padding
//   PR				   : Right padding
//   I_ITER            : Number of input iterations (I / CPI)
//   in                : input stream
//   out               : output stream
//

void padding(int H, int W, int PT, int PB, int PL, int PR, int I_ITER, hls::stream<din_st> &in, hls::stream<din_st> &out){
  #ifdef DEBUG_PADDING
  std::cout << "PADDING: start" << std::endl;
  #endif

  //uint num_iters;
  //uint h;
  //uint w;
  // Variable to keep track of all the groups of CPI in the input
  uint i_iter;
  din_st data;
  // In C/C++, variables defined with the static qualifier and those defined in the global scope
  // are initialized to zero, by default.
  // https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Initialization-Behavior
  static din_st zero;

  uint HH = H + PT + PB;
  uint WW = W + PL + PR;

  #pragma HLS ARRAY_PARTITION variable=data type=complete
  #pragma HLS ARRAY_PARTITION variable=zero type=complete

  uint val_e2 = H+PT;
  uint val_e4 = W+PL;
  // num_iters = I_ITER * (H + PT + PB) * (W + PL + PR);
  // h = 0;
  // w = 0;
  padding_loop:
  for (uint h = 0; h < HH; h++) {
    //#pragma HLS LOOP_TRIPCOUNT min=1 max=((I_REFERENCE/CPI)*W_REFERENCE*H_REFERENCE)
    //#pragma HLS PIPELINE II=1
	for (uint w = 0; w < WW; w++) {
		for (uint i = 0; i < I_ITER; i++) {
			int enable1 = (h < PT);
			int enable2 = (h >= val_e2);
			int enable3 = (w < PL);
			int enable4 = (w >= val_e4);
			if (enable1 | enable2 | enable3 | enable4) {
				data = zero;
			} else {
				data = in.read();
			}
			out << data;
			#ifdef DEBUG_PADDING
			std::cout << "PADDING: send data (i " << i << ", h " << h << ", w " << w << ", |enableX " << (enable1|enable2|enable3|enable4) << std::endl;
			for (int cpi = 0; cpi < CPI; cpi++) {
				std::cout << data.pixel[cpi] << " ";
			}
			std::cout << std::endl;
			#endif
		}
	}
  }

  #ifdef DEBUG_PADDING
  std::cout << "PADDING: end" << std::endl;
  #endif
}

