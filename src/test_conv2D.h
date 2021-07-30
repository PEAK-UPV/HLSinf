#include "conv2D.h"

#include <cstdio>      /* printf, scanf, NULL */
#include <cstdlib>     /* malloc, free, rand */

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <sys/time.h>

#ifdef OPENCL_TEST
#include "CL/cl_ext_xilinx.h"
#include "xcl2.hpp"
#endif

using std::vector;

// constants
#define MAX_CONVS        8  // Maximum number of convolutional layers
#define MAX_KERNELS      4  // Maximum number of kernels implemented
#define MAX_WORK_ITEMS 512  // Maximum number of work items to process
#define NUM_KERNELS      1

// Global variables
extern int CONVS;                        // Number of convolutional layers
extern int KERNELS;                      // Number of FPGA kernels to use
extern int PT;                           // Top padding
extern int PB;                           // Bottom padding
extern int PL;                           // Left padding
extern int PR;                           // Right padding
extern int SH;                           // Vertical stride
extern int SW;                           // Horizontal stride
extern int F;                            // Number of frames of the data
extern int W;                            // Width of the data
extern int H;                            // Height of the data
extern int I;                            // Number of input channels
extern int O;                            // Number of output channels
extern int HO;						     // Output width of the conv
extern int WO;							 // Output height of the conv
extern int HO_final;                     // HO at the output of the kernel
extern int WO_final;                     // WO at the output of the kernel
extern int I_kernel;					 // Number of input channels for the kernel (filter) - padding
extern int O_kernel;  					 // Number of output channels for the kernel (filter) - padding
extern int I_input;                      // Number of input channels for the input data - padding (needed in GIHWCPI data format)
extern int O_output;                     // Number of output channels for the output data - padding (needed in GIHWCPI data format)
extern int rows;						 // number of rows to compute by the kernel
extern int enable_relu;				     // enables applying the relu activation functions
extern int enable_stm;  			 	 // enables applying the STM functions
extern data_type relu_factor;
extern int enable_shift;				 // enables applying shift to the output
extern int enable_add; 				     // enables add module
extern int dir_shift;			         // shift direction (left or right)
extern int pos_shift;					 // positions to shift
extern int enable_clipping;			     // enables applying clipping to the output
extern int enable_maxpooling;			 // enables the maxpooling layer
extern int enable_avgpooling;			 // enables the avgpooling layer
extern int min_clip;				 	 // minimum clip value
extern int max_clip;				 	 // maximum clip value
extern int i_iter;						 // number of input iterations
extern int o_iter;						 // number of output iterations
extern int global_offset;				 // global offset for the output data for the kernel
extern int GI;							 // number of groups for input channels
extern int GO;							 // number of groups for output channels
extern data_type *data_in;               // Input data buffer (format I x W x H)
extern data_type *data_in_add;           // Input data buffer for add module(format I x W x H)
extern data_type *out;                   // Output data buffer (format O x W x H)
extern data_type *kernel;                // Conv kernel buffers (format GO x GI x CPO x CPI x KW x KH) - for DirectConv and WinogradConv
extern data_type *dw_kernel;             // DW kernel (format I x KH x KW) - for DWS
extern data_type *pw_kernel;             // PW kernel (format GO x GI x CPO x CPI) - for DWS
extern data_type *bias;                  // Conv bias buffers (format O)
extern data_type *out_conv_cpu;          // Output data buffer for cpu (format O x W x H)
extern data_type *out_relu_cpu;          // Output data buffer for cpu (format O x W x H)
extern data_type *out_stm_cpu;		     // Output data buffer for STM for cpu (format O x O x W x H)
extern data_type *out_add_cpu;		     // Output data buffer for ADD for cpu (format O x O x W x H)
extern data_type *out_pool_cpu;		     // Output data fuffer for pool for cpu (format O x W/2 x H/2)
extern data_type *cpu_out;
extern char *input_data_file;            // file with input parameters
extern int deterministic_input_values;   // whether input data is randomly generated or not (deterministic needed in co-simulation)

extern FILE *fp;

#ifdef OPENCL_TEST
// OpenCL variables
extern cl::Context context;                          // Context
extern cl::CommandQueue q;                           // Command queue
extern cl::Program program;                          // Program
extern std::string binaryFile;                       // Binary file
extern cl::Kernel kernel_conv2d[MAX_KERNELS];        // FPGA kernels
extern vector<cl::Event> kernel_events;              // Kernel events (completion)
extern vector<cl::Event> read_events;                // Read events
extern vector<cl::Event> write_events;               // Write events
extern cl::Buffer *buffer_i;                         // input buffer
extern cl::Buffer *buffer_i_add;                     // input buffer for add module
extern cl::Buffer *buffer_o[MAX_CONVS];              // output buffers
extern cl::Buffer *buffer_k[MAX_CONVS];              // Conv kernel buffers
extern cl::Buffer *buffer_k_dw[MAX_CONVS];           // Conv kernel buffers (deepwise)
extern cl::Buffer *buffer_k_pw[MAX_CONVS];           // Conv kernel buffers (pointwise)
extern cl::Buffer *buffer_bias[MAX_CONVS];           // Conv bias buffers
// DDR assignment
extern cl_mem_ext_ptr_t data_in_ddr;                 // input data buffer
extern cl_mem_ext_ptr_t data_in_add_ddr;             // input data add buffer
extern cl_mem_ext_ptr_t out_ddr[MAX_CONVS];          // output data buffers
extern cl_mem_ext_ptr_t kernel_ddr[MAX_CONVS];       // Conv kernel buffers
extern cl_mem_ext_ptr_t kernel_pw_ddr[MAX_CONVS];    // DeepWise conv kernel buffers
extern cl_mem_ext_ptr_t kernel_dw_ddr[MAX_CONVS];    // PointWise conv kernel buffers
extern cl_mem_ext_ptr_t bias_ddr[MAX_CONVS];         // Conv bias buffers
#endif

// function prototypes
void allocate_buffers();
void deallocate_buffers();
int filter_address_direct_conv(int i, int o, int kh, int kw);
void cpu_conv2D();
void print_bias();
void print_input();
void print_input_add();
void print_output();
void print_kernel();
int check_result(data_type *max_difference, int *num_elements_differ);
void init_data();
int open_test_file();
int read_test_file(int *enable, int *cpu);
int close_test_file();
void run_kernel(int rows_p, int PT_p, int PB_p, int PL_p, int PR_p, int read_offset_p, int write_offset_p);
void compute();
void parse_arguments(int argc, char **argv);
void print_configuration();
void print_timings(unsigned long long time, unsigned long long time_per_iteration, unsigned long long expected_time, float efficiency);
void print_check(int result, float max_difference, int num_differences);
void print_message(const char *str);
int input_data_address(int i, int h, int w);
int output_data_address(int o, int h, int w, int height, int width);
int output_data_address_div(int o, int h, int w);

#ifdef OPENCL_TEST
void parse_arguments(int argc, char **argv);
void allocate_buffers_opencl();
void deallocate_buffers_opencl();
void fn_init_fpga();
void copy_to_fpga();
void copy_from_fpga();
void set_callback(cl::Event event, const char *queue_name);
void event_cb(cl_event event1, cl_int cmd_status, void *data);
#endif
