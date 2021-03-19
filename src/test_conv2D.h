#include "conv2D.h"

#include <cstdio>      /* printf, scanf, NULL */
#include <cstdlib>     /* malloc, free, rand */

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
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

// Global variables
extern int CONVS;                       // Number of convolutional layers
extern int KERNELS;                     // Number of FPGA kernels to use
extern int F;                           // Number of frames of the data
extern int W;                           // Width of the data
extern int H;                           // Height of the data
extern int I;                           // Number of input channels
extern int O;                           // Number of output channels
extern int HO;						     // Output width
extern int WO;							 // Output height
extern int I_kernel;					 // Number of input channels for the kernel (filter) - padding
extern int O_kernel;  					 // Number of output channels for the kernel (filter) - padding
extern int rows;						 // number of rows to compute by the kernel
extern int enable_upper_padding;		 // enables the upper row of padding
extern int enable_lower_padding;		 // enables the lower row of padding
extern int enable_relu;				     // enables applying the relu activation functions
extern int enable_shift;				 // enables applying shift to the output
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
extern data_type *out;                   // Output data buffer (format O x W x H)
extern data_type *kernel;                // Conv kernel buffers (format GO x GI x CPO x CPI x KW x KH) - for DirectConv and WinogradConv
                                         // DWS conv kernel buffers (format I x KW x KH + I x O) [DW + PW]
extern data_type *bias;                  // Conv bias buffers (format O)
extern data_type *out_conv_cpu;          // Output data buffer for cpu (format O x W x H)
extern data_type *out_relu_cpu;          // Output data buffer for cpu (format O x W x H)
extern data_type *out_pool_cpu;		     // Output data fuffer for pool for cpu (format O x W/2 x H/2)
extern char *input_data_file;            // file with input parameters
extern FILE *fp;

#ifdef OPENCL_TEST
// OpenCL variables
extern cl::Context context;                          // Context
extern cl::CommandQueue q;                           // Command queue
extern cl::Program program;                          // Program
extern std::string binaryFile;                       // Binary file
extern cl::Kernel kernel_conv2d[MAX_KERNELS];        // FPGA kernels
extern vector<cl::Event> kernel_events; // Kernel events (completion)
extern vector<cl::Event> read_events;             // Read events
extern vector<cl::Event> write_events;            // Write events
extern cl::Buffer *buffer_i;                         // input buffer
extern cl::Buffer *buffer_o[MAX_CONVS];              // output buffers
extern cl::Buffer *buffer_k[MAX_CONVS];              // Conv kernel buffers
extern cl::Buffer *buffer_bias[MAX_CONVS];           // Conv bias buffers
// DDR assignment
extern cl_mem_ext_ptr_t data_in_ddr;                 // input data buffer
extern cl_mem_ext_ptr_t out_ddr[MAX_CONVS];          // output data buffers
extern cl_mem_ext_ptr_t kernel_ddr[MAX_CONVS];       // Conv kernel buffers
extern cl_mem_ext_ptr_t bias_ddr[MAX_CONVS];         // Conv bias buffers
#endif

// function prototypes
void allocate_buffers();
void deallocate_buffers();
int filter_address_direct_conv(int i, int o, int kh, int kw);
void cpu_conv2D();
void print_bias();
void print_input();
void print_output();
void print_kernel();
int check_result(data_type *max_difference, int *num_elements_differ);
void init_data();
int open_test_file();
int read_test_file(int *enable, int *cpu);
int close_test_file();
void run_kernel();
void run_kernel_opencl();
void parse_arguments(int argc, char **argv);
void print_configuration();
void print_timings(unsigned long long time, unsigned long long time_per_iteration, unsigned long long expected_time, float efficiency);
void print_check(int result, float max_difference, int num_differences);
void print_message(const char *str);

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
