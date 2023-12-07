/*
 * initializers.h
 *
 */

#include <hls_stream.h>
#include <hls_math.h>
#include "CL/cl_ext_xilinx.h"
#include "xcl2.hpp"

// defines
#define MAX_INITIALIZERS    10000

// structs
struct st_initializer {
  int valid;
  char *name;
  char type[20]; // can be "weight", "bias", "gamma", "beta", "running_mean", "running_var"
  int num_dimensions;
  int *dimensions;
  float *data;
  // info for running
  cl_mem_ext_ptr_t buffer_ddr;
  cl::Buffer *buffer; 
};

// global variables
extern int                   num_initializers;
extern struct st_initializer aInitializer[MAX_INITIALIZERS];

// function prototypes
int fn_get_initializer_by_name(char *name);
int is_initializer(char *name);
int is_weight_initializer(char *name);
int is_bias_initializer(char *name);
int is_gamma_initializer(char *name);
int is_beta_initializer(char *name);
int is_running_mean_initializer(char *name);
int is_running_var_initializer(char *name);
void fn_read_initializers_data();
void fn_write_initializers_data();
char *get_weight_initializer_name_from_node(int n);
int get_weight_initializer_entry_from_node(int n);
char *get_bias_initializer_name_from_node(int n);
char *get_gamma_initializer_name_from_node(int n);
char *get_beta_initializer_name_from_node(int n);
char *get_running_mean_initializer_name_from_node(int n);
char *get_running_var_initializer_name_from_node(int n);
int  get_gamma_initializer_id_from_node(int n);
int  get_beta_initializer_id_from_node(int n);
int  get_running_mean_initializer_id_from_node(int n);
int  get_running_var_initializer_id_from_node(int n);
void fn_pad_weight_initializer_1x1_to_3x3(char *name);
void fn_add_new_initializer(char *name, char *type, int num_items, float *d);
void fn_create_zero_initializers();
void remove_initializer(int i);
