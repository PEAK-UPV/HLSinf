/*
 * nodes.h
 *
 */

#include <hls_stream.h>
#include <hls_math.h>
#include "CL/cl_ext_xilinx.h"
#include "xcl2.hpp"

// defines
#define MAX_NODES         10000

// structs
struct st_node {
 int valid;
 char *name;
 int num_inputs;
 char **inputs;
 int num_outputs;
 char **outputs;
 char *type;
 // parameters for convs (dilations, kernel, padding, stride, group) and maxpools (kernel, padding, stride)
 int dh, dw, kh, kw, pt, pb, pl, pr, sh, sw, g;
 // parameters for batch normalization
 float epsilon, momentum;
 // parameters for HLSinf
 int has_conv;
 int has_relu;
 int has_maxpool;
 int has_bn;
 int has_add;
 int hlsinf_kh_conv;
 int hlsinf_kw_conv;
 int hlsinf_sh_conv;
 int hlsinf_sw_conv;
 int hlsinf_pt_conv;
 int hlsinf_pb_conv;
 int hlsinf_pr_conv;
 int hlsinf_pl_conv;
 int hlsinf_kh_maxpool;
 int hlsinf_kw_maxpool;
 int hlsinf_sh_maxpool;
 int hlsinf_sw_maxpool;
 // supported in HLSinf
 int supported;
 // input and output data geometry
 int I, O;
 int HI, HO;
 int WI, W, WO;
 // parameters for printing the model
 int row;
 int col;
 // parameters for running
 int run_order;
 cl_mem_ext_ptr_t buffer_ddr;
 cl::Buffer *buffer;
};

// global variables
extern int            num_nodes;
extern struct st_node aNode[MAX_NODES];

// function prototypes
int fn_get_child_nodes(char *name, int *list);
int fn_get_node_by_name(char *name);
int is_node_by_name(char *name);
int is_hlsinf(int n);
int is_conv(int n);
int is_relu(int n);
int is_maxpool(int n);
int is_bn(int n);
int is_add(int n);
int has_no_parents(int n);
int fn_get_first_node(char *name);
int fn_get_last_node(char *name);
int fn_get_model_input_from_node(int n);
int fn_get_parent_nodes(char *name, int *list);
int fn_get_unallocated_node_with_parents_allocated();
int fn_check_all_parents_allocated(int n);
void fn_allocate_nodes();
void fn_compute_data_geometries();
void add_input_to_node(int n, char *name);
void remove_input_from_node(int n, char *name);
int fn_get_node_entry(char *name);
char *get_node_name(int n);
void fn_remove_node(int n);
char *get_data_input_name_from_node(int n, char *exclude_name);
void fn_relink_node_inputs(char *old_name, char *new_name);
void fn_add_host_device_nodes();
int fn_add_node(char *name, char *type);
void fn_change_input_in_node(int n, char *old_name, char *new_name);
