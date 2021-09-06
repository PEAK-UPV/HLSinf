// --------------------------------------------------------------------------------------------------------------
//
//
// --------------------------------------------------------------------------------------------------------------


#ifndef _CONV2D_H_
#define _CONV2D_H_

//#define CL_HPP_TARGET_OPENCL_VERSION 200
//#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY

#define OPENCL_TEST

#ifdef OPENCL_TEST
  #ifdef __INTELFPGA_COMPILER__
    #include "HLS/ac_fixed.h"
    #include "HLS/ac_int.h"
  #else
    #include "ref/ac_fixed.h"
    #include "ref/ac_int.h"
  #endif
#endif

#include <stdio.h>
#ifdef OPENCL_TEST
  #include <linux/limits.h>
  #include <unistd.h>
  #include <assert.h>
  #include <math.h>
  #include <vector>

  #include "CL/opencl.h"
  //#include <CL/cl2.hpp>  // related to opencl in c++ 
  //#include <CL/cl.hpp>   // related to opencl in c++
  //#include "CL/cl_ext.h" // related to opencl in c++
  #include "AOCLUtils/aocl_utils.h"

  using namespace aocl_utils;
  using namespace std;
  //using std::vector;
#endif
  
#include "conv2D_commons.h"


#endif




