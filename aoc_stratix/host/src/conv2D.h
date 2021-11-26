// --------------------------------------------------------------------------------------------------------------
//
//
// --------------------------------------------------------------------------------------------------------------


#ifndef _CONV2D_H_
#define _CONV2D_H_

//#define CL_HPP_TARGET_OPENCL_VERSION 200
//#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY
#include <stdio.h>

#define OPENCL_TEST

//#ifdef OPENCL_TEST
//  #ifdef __INTELFPGA_COMPILER__
//    #include "HLS/ac_fixed.h"
//    #include "HLS/ac_int.h"
//  #else
//    #include "ref/ac_fixed.h"
//    #include "ref/ac_int.h"
//  #endif
//#endif

#ifdef OPENCL_TEST
#define _USE_MATH_DEFINES
#include <cstring>
#include <CL/opencl.h>
#include <CL/cl_ext_intelfpga.h>
#include "AOCLUtils/aocl_utils.h"
using namespace aocl_utils;
#endif

using namespace std;


#ifdef OPENCL_TEST
  #include <linux/limits.h>
  #include <unistd.h>
  #include <assert.h>
  #include <math.h>
  #include <vector>

// to prevent warning message  
//host/src/opencl.cpp:355:120: warning: ‘_cl_command_queue* clCreateCommandQueue(cl_context, cl_device_id, cl_command_queue_properties, cl_int*)’ is deprecated [-Wdeprecated-declarations]
  #define CL_USE_DEPRECATED_OPENCL_1_2_APIS
  #include <CL/cl.h>

  //#include <CL/cl2.hpp>  // related to opencl in c++ 
  //#include <CL/cl.hpp>   // related to opencl in c++
  //#include "CL/cl_ext.h" // related to opencl in c++

  //using std::vector;
#endif
  
#include "conv2D_commons.h"


#endif




