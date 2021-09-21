#ifndef _LIB_HDRS_PRIV_H_
#define _LIB_HDRS_PRIV_H_

#include "lib_conv2D_commons.h"

#ifdef __INTELFPGA_COMPILER__
  #include "HLS/ac_fixed.h"
  #include "HLS/ac_int.h"
#else
  #include "ref/ac_fixed.h"
  #include "ref/ac_int.h"
#endif

#include "HLS/hls.h"
#include "HLS/math.h"
#include "HLS/ocl_types.h"
//#ifndef HLS_SYNTHESIS
//  #include <iostream>
//#endif

#include "HLS/stdio.h"

#ifndef LIB_HEADERS_PRIVATE_H
#define LIB_HEADERS_PRIVATE_H

#define MAX_O_ITERS ((HMAX/CPO)+1)

struct frame_pool_t {
	pixel_out_t pixel[KW_POOLING * KH_POOLING];
};

#endif

#endif
