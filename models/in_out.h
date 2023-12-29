/*
 * in_out.h
 *
 */

#ifndef _IN_OUT_H
#define _IN_OUT_H

#ifdef RUNTIME_SUPPORT
#include <hls_stream.h>
#include <hls_math.h>
#include "CL/cl_ext_xilinx.h"
#include "xcl2.hpp"
#endif

// defines
#define MAX_INPUTS    100     // maximum number of inputs to the model
#define MAX_OUTPUTS   100     // maximum number of outputs from the model

// st_input struct. It contains all useful information for a model input
struct st_input {
  int    valid;                   // valid entry
  char  *name;                    // input's name
  int    num_dimensions;          // number of dimensions
  int   *dimensions;              // list of dimensions
  char  *data_type;               // data type (float32, int8, ...)
  float *data;                    // pointer to data
  #ifdef RUNTIME_SUPPORT
  cl_mem_ext_ptr_t  buffer_ddr;   // xilinx buffer
  cl::Buffer       *buffer;       // opencl buffer
  #endif
};

// st_output struct. It contains all useful information for a model output
struct st_output {
  int   valid;                    // valid entry
  char *name;                     // output's name
};

// global variables
extern int               num_inputs;            // number of inputs to the model
extern int               num_outputs;           // number of outputs from the model
extern struct st_input   aInput[MAX_INPUTS];    // vector of model inputs
extern struct st_output  aOutput[MAX_OUTPUTS];  // vector of model outputs

// function prototypes
int is_model_input(char *name);
int get_model_input_id(char *name);
void fn_change_output_model_name(int o, char *name);
int get_model_output_id(char *name);

#endif
