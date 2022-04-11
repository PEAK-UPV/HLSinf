// --------------------------------------------------------------------------------------------------------------
//
//
// --------------------------------------------------------------------------------------------------------------


#include "conv2D.h"

#include <cstdio>      /* printf, scanf, NULL */
#include <cstdlib>     /* malloc, free, rand */

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <random>
//#include <vector>
#include <sys/time.h>
#include <stdint.h>

// color codes for nice printf
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KRST  "\033[0m"

//#define CHECK(X) assert(CL_SUCCESS == (X))

const char *getErrorString(cl_int error); //function in opencl.cpp
// OCL_CHECK doesn't work if call has templatized function call
#define OCL_CHECK(error, call)                                                          \
  call;                                                                                 \
  if (error != CL_SUCCESS) {                                                            \
    printf("%s:%d Error calling " #call ", error code is: %d   corresponds to:  %s\n",  \
           __FILE__, __LINE__, error, getErrorString(error));                           \
    exit(EXIT_FAILURE);                                                                 \
  }

#define CHECK(X) OCL_CHECK(X, "not-specified")

// The set of simultaneous kernels
enum KERNELS {
  K_DATA_IN_READER,
  K_KERNEL_IN_READER,
  K_BIAS_IN_READER,
  K_BATCH_NORM_READER,
  K_ADD_DATA_READER,
  //K_INPUT_BUFFER,
  K_PADDING,
  K_CVT,
  K_MULTIPLIER,
  K_ADDER,
  K_RELU,
  K_POOL_CVT,
  K_POOL_POOLING,
  K_BATCH_NORM,
  K_ADD_DATA,
  K_WRITER,
  K_NUM_KERNELS
};
static const char* kernel_names[K_NUM_KERNELS] =
{
  "data_in",
  "kernel_in",
  "bias_in",
  "batch_norm_in",
  "add_data_in",
  //"input_buffer",
  "padding",
  "cvt",
  "mul",
  "add",
  "relu",
  "pool_cvt",
  "pool_pooling",
  "batch_norm",
  "add_data",
  "data_out"
};


//JM10_TO_DELETE
//extern int enable_upper_padding;   // enables the upper row of padding
//extern int enable_lower_padding;   // enables the lower row of padding

// Global variables
extern int CONVS;     // Number of convolutional layers
extern int KERNELS;   // Number of FPGA kernels to use
extern int F;         // Number of frames of the data
extern int W;         // Width of the data
extern int H;         // Height of the data
extern int I;         // Number of input channels
extern int O;         // Number of output channels
extern int HO;        // Output width
extern int WO;        // Output height

extern int HO_final;   // HO at the output of the kernel
extern int WO_final;   // WO at the output of the kernel

extern int enable_batch_norm;
extern int enable_add;
//


extern int I_kernel;    // Number of input channels for the kernel (filter) - padding
extern int O_kernel;    // Number of output channels for the kernel (filter) - padding
extern int I_input;     // Number of input channels for the input data - padding (needed in GIHWCPI data format)
extern int O_output;    // Number of output channels for the output data - padding (needed in GIHWCPI data format)
extern int rows;        // number of rows to compute by the kernel

extern int PT;          // enables the upper row of padding
extern int PB;          // enables the lower row of padding
extern int PL;          // enables the left  column of padding
extern int PR;          // enables the right column of padding

extern int SH;          // vertical stride
extern int SW;          // horizontal stride

extern int enable_relu;				     // enables applying the relu activation functions
extern int enable_shift;           // enables applying shift to the output
extern int dir_shift;              // shift direction (left or right)
extern int pos_shift;              // positions to shift
extern int enable_clipping;			   // enables applying clipping to the output
extern int enable_maxpooling;			 // enables the maxpooling layer
extern int enable_avgpooling;			 // enables the avgpooling layer
extern int min_clip;				 // minimum clip value
extern int max_clip;				 // maximum clip value
extern int i_iter;					 // number of input iterations
extern int o_iter;					 // number of output iterations
//extern int global_offset;    // global offset for the output data for the kernel
extern int GI;							 // number of groups for input channels
extern int GO;							 // number of groups for output channels

//extern data_type *data_in;   // Input data buffer (format I x W x H)
//extern data_type *out;       // Output data buffer (format O x W x H)
//extern data_type *kernel;    // Conv kernel buffers (format GO x GI x CPO x CPI x KW x KH) - for DirectConv and WinogradConv
//extern data_type *dw_kernel;       // DW kernel (format I x KH x KW) - for DWS
//extern data_type *pw_kernel;       // PW kernel (format GO x GI x CPO x CPI) - for DWS
//extern data_type *bias;            // Conv bias buffers (format O)
//extern data_type *out_conv_cpu;    // Output data buffer for cpu (format O x W x H)
//extern data_type *out_relu_cpu;    // Output data buffer for cpu (format O x W x H)
//extern data_type *out_pool_cpu;		 // Output data fuffer for pool for cpu (format O x W/2 x H/2)
extern scoped_aligned_ptr<data_type> data_in;   // Input data buffer (format I x W x H)
extern scoped_aligned_ptr<data_type> out;       // Output data buffer (format O x W x H)
extern scoped_aligned_ptr<data_type> kernel;    // Conv kernel buffers (format GO x GI x CPO x CPI x KW x KH) - for DirectConv and WinogradConv
extern scoped_aligned_ptr<data_type> dw_kernel;     // DW kernel (format I x KH x KW) - for DWS
extern scoped_aligned_ptr<data_type> pw_kernel;     // PW kernel (format GO x GI x CPO x CPI) - for DWS
extern scoped_aligned_ptr<data_type> bias;          // Conv bias buffers (format O)

extern scoped_aligned_ptr<bn_t> batch_norm_values;  // Batch normalization values
extern scoped_aligned_ptr<add_data_t> data_in_add;  // Input data buffer for add module(format I x W x H)

extern scoped_aligned_ptr<data_type> out_conv_cpu;  // Output data buffer for cpu (format O x W x H)
extern scoped_aligned_ptr<data_type> out_relu_cpu;  // Output data buffer for cpu (format O x W x H)
extern scoped_aligned_ptr<data_type> out_pool_cpu;	// Output data fuffer for pool for cpu (format O x W/2 x H/2)

extern scoped_aligned_ptr<data_type> out_batch_norm_cpu;  // Output data buffer for cpu (format O x W x H
extern scoped_aligned_ptr<data_type> out_add_cpu;         // Output data buffer for ADD for cpu (format O x W x H)
extern scoped_aligned_ptr<data_type> out_cpu;             // Output data buffer for cpu. final output

extern scoped_aligned_ptr<data_type> out_cpu_from_file;        // Output data buffer for EDDL final output

extern char *input_data_file;            // file with input parameters

extern int deterministic_input_values;   // whether input data is randomly generated or not (deterministic needed in co-simulation)

extern FILE *fp;


//extern double kernels_start_time;
//extern double kernels_end_time;
extern double kernels_execution_time; // kernels execution time in miliseconds
extern double epsilon_dataset_tuned;  //

#ifdef OPENCL_TEST

extern int use_emulator;
// OpenCL variables
extern cl_platform_id   platform;
extern cl_device_id     device;
extern cl_context       context;
//extern cl_command_queue q;
extern cl_program       program;
extern char            *binaryFile;                       // kernel Binary file

extern cl_mem buffer_i;                         // input buffer
extern cl_mem buffer_o;//[MAX_CONVS];              // output buffers
extern cl_mem buffer_k;//[MAX_CONVS];              // Conv kernel buffers
extern cl_mem buffer_bias;//[MAX_CONVS];           // Conv bias buffers
extern cl_mem buffer_batch_norm_val;
extern cl_mem buffer_i_add;
extern cl_mem buffer_k_dw;//[MAX_CONVS];           // Conv kernel buffers (deepwise)
extern cl_mem buffer_k_pw;//[MAX_CONVS];           // Conv kernel buffers (pointwise)

extern cl_command_queue queues[K_NUM_KERNELS];
extern cl_kernel kernels[K_NUM_KERNELS];
extern cl_event  kernel_events[K_NUM_KERNELS];


#endif

// function prototypes
void allocate_buffers();
void deallocate_buffers();
int filter_address_direct_conv(int i, int o, int kh, int kw);

void cpu_conv2D();

void print_bias();
void print_input();
void print_output(int only_cpu);
void print_kernel();

void print_input_buffer_stats(data_type *p, int size);
void print_weight_buffer_stats(data_type *p, int size);
void print_bias_buffer_stats(data_type *p, int size);
void print_data_in_buffer_stats(data_type *p, int size);
void print_batchnorm_buffer_stats(data_type *p, int size);
void print_output_buffer_stats(data_type *p, int size);

int check_result(data_type *max_difference, int *num_elements_differ, int from_file);
void init_data(int from_file);
int open_test_file();
int read_test_file(int *enable, int *from_file, int *cpu);
int close_test_file();
//void run_kernel();
//void run_aoc_kernels();
void compute();  // determine multi-fame config and launch kernel
void run_kernel_opencl();
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
int  fn_init_fpga();
void fn_release_fpga();
void copy_to_fpga();
void copy_from_fpga();
//void set_callback(cl::Event event, const char *queue_name);
//void event_cb(cl_event event1, cl_int cmd_status, void *data);
#endif




