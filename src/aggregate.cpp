/*
* HLSinf accelerator
* Version: 2.0
* copyright (c) 2023, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: July 2023
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#include "conv2D.h"

// ---------------------------------------------------------------------------------------
// aggregate. Aggregate pixels into an IMAX structure and forward it to the nest module
//
// Arguments:
//   H 				   : Height of the input channel
//   W				   : Width of the input channel
//   I_ITER            : Number of input iterations (I / CPI)
//   in                : input stream
//   out               : output stream
//

void aggregate(int H, int W, int I_ITER, hls::stream<din_st> &in, hls::stream<cvt_in_st> &out){
  #ifdef DEBUG_AGGREGATE
  std::cout << "AGGREGATE: start" << std::endl;
  #endif

  cvt_in_st data_out;
  #pragma HLS ARRAY_PARTITION variable=data_out type=complete

  uint num_iters = H * W;

  // TODO: Optimize
  for (uint p = 0; p < num_iters; p++) {
	  //#pragma HLS LOOP_FLATTEN
	  for (uint i = 0; i < I_ITER; i++) {
		//#pragma HLS PIPELINE II=1
		data_out.pixel[i] = in.read();
		#ifdef DEBUG_AGGREGATE
		std::cout << "AGGREGATE: p " << p;
		for (uint cpi = 0; cpi < CPI; cpi++) {
			std::cout << " " << data_out.pixel[i].pixel[cpi];
		}
		std::cout << std::endl;
		#endif
	  }

	  out << data_out;
  }

  #ifdef DEBUG_AGGREGATE
  std::cout << "AGGREGATE: end" << std::endl;
  #endif
}
