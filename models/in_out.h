/*
 * in_out.h
 *
 */

#include <hls_stream.h>
#include <hls_math.h>
#include "CL/cl_ext_xilinx.h"
#include "xcl2.hpp"

// defines
#define MAX_INPUTS    100
#define MAX_OUTPUTS   100

// structs
struct st_input {
  int valid;
  char *name;
  int num_dimensions;
  int *dimensions;
  char *data_type;
  // info for running
  cl_mem_ext_ptr_t buffer_ddr;
  cl::Buffer *buffer;
};

struct st_output {
  int valid;
  char *name;
};

// global variables
extern int               num_inputs;
extern int               num_outputs;
extern struct st_input   aInput[MAX_INPUTS];
extern struct st_output  aOutput[MAX_OUTPUTS];

// function prototypes
int is_model_input(char *name);
void fn_change_output_model_name(int o, char *name);
int get_model_output_id(char *name);
