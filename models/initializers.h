/*
 * initializers.h
 *
 */

#ifndef _INITIALIZERS_H
#define _INITIALIZERS_H

#ifdef RUNTIME_SUPPORT
#include <hls_stream.h>
#include <hls_math.h>
#include "CL/cl_ext_xilinx.h"
#include "xcl2.hpp"
#endif

// defines
#define MAX_INITIALIZERS    10000      // maximum number of initializers (should be enough)

// st_initializer struct. Contains all related information to an initializer
struct st_initializer {
  int valid;                    // whether the entry is valid
  char *name;                   // initializer's name
  int num_dimensions;           // number of dimensions
  int *dimensions;              // list of dimensions
  char data_type[20];           // data type ("float32", "int8", ..,)
  float *data;                  // pointer to data
  #ifdef RUNTIME_SUPPORT
  cl_mem_ext_ptr_t buffer_ddr;  // xilinx buffer
  cl::Buffer *buffer;           // fpga opencl buffer
  #endif
};

// global variables
extern int                   num_initializers;                  // number of initializers
extern struct st_initializer aInitializer[MAX_INITIALIZERS];    // vector of initializers

// function prototypes
int fn_get_initializer_by_name(char *name);
int is_initializer(char *name);
void fn_pad_weight_initializer_1x1_to_3x3(char *name);
void fn_pad_weight_initializer_2x2_to_3x3(char *name);
void fn_add_new_initializer(char *name, int num_items, float *d);
void remove_initializer(int i);
int get_initializer_entry_from_node(int n);
void fn_pad_weights(int i, int new_I, int new_O);
void fn_pad_bias(int i, int new_O);
void fn_pad_bn_vector(int i, int new_O);

#endif
