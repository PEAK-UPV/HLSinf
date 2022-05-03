#include <cstdio>      /* printf, scanf, NULL */
#include <cstdlib>     /* malloc, free, rand */

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <sys/time.h>

#include "top.h"
#ifdef OPENCL_TEST
#include "CL/cl_ext_xilinx.h"
#include "xcl2.hpp"
#endif

#define MAX_WORK_ITEMS 512  // Maximum number of work items to process

using std::vector;

extern int prob;
extern size_t size_x_in_bytes;
extern size_t size_w_in_bytes;
extern size_t size_y_in_bytes;

extern ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x0;
extern ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x1;
extern ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x2;
extern ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x3;
extern ap_uint<8> *w;
extern ap_uint<16> *y;
extern ap_uint<8> *x[I];

#ifdef OPENCL_TEST
// OpenCL variables
extern cl::Context context;                          // Context
extern cl::CommandQueue q;                           // Command queue
extern cl::Program program;                          // Program
extern std::string binaryFile;                       // Binary file
extern cl::Kernel kernel_conv2d;        // FPGA kernels
extern cl::Event kernel_events;              // Kernel events (completion)
extern vector<cl::Event> read_events;                // Read events
extern vector<cl::Event> write_events;               // Write events
extern cl::Buffer *buffer_i0;                         // input buffer
extern cl::Buffer *buffer_i1;                         // input buffer
extern cl::Buffer *buffer_i2;                         // input buffer
extern cl::Buffer *buffer_i3;                         // input buffer
extern cl::Buffer *buffer_o;              // output buffers
extern cl::Buffer *buffer_k;						 // Conv kernel buffers
// DDR assignment
extern cl_mem_ext_ptr_t data_in_ddr0;                  // input data buffer
extern cl_mem_ext_ptr_t data_in_ddr1;                  // input data buffer
extern cl_mem_ext_ptr_t data_in_ddr2;                  // input data buffer
extern cl_mem_ext_ptr_t data_in_ddr3;                  // input data buffer
extern cl_mem_ext_ptr_t out_ddr;           // output data buffers
extern cl_mem_ext_ptr_t kernel_ddr;        // Conv kernel buffers
#endif


void parse_arguments(int argc, char **argv);


#ifdef OPENCL_TEST
void parse_arguments(int argc, char **argv);
void fn_init_fpga();
void copy_to_fpga();
void copy_from_fpga();
void allocate_buffers();
void set_callback(cl::Event event, const char *queue_name);
void event_cb(cl_event event1, cl_int cmd_status, void *data);
void run_kernel();
#endif
