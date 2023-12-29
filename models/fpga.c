/*
 * fpga.c
 *
 * This file includes all the support functions related to the FPGA device
 */

#ifdef RUNTIME_SUPPORT
#include <hls_stream.h>
#include <hls_math.h>
#include "CL/cl_ext_xilinx.h"
#include "xcl2.hpp"


#include "runner.h"
#include "nodes.h"
#include "initializers.h"
#include "in_out.h"
#include "main.h"
#include "stats.h"
#include "fpga.h"

using std::vector;

// OpenCL event callback function printing message
void event_cb(cl_event event1, cl_int cmd_status, void *data) {
  cl_int err;
  cl_command_type command;
  cl::Event event(event1, true);
  OCL_CHECK(err, err = event.getInfo(CL_EVENT_COMMAND_TYPE, &command));
  cl_int status;
  OCL_CHECK(err, err = event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS, &status));
}

// Sets the callback for a particular event
void set_callback(cl::Event event, const char *queue_name) {
  cl_int err;
  OCL_CHECK(err, err = event.setCallback(CL_COMPLETE, event_cb, (void *)queue_name));
}

// global variables
cl::Context       context;
cl::CommandQueue  q;
cl::Program       program;
std::string       binaryFile;
cl::Kernel        kernel_conv2d[MAX_KERNELS];
vector<cl::Event> kernel_events(MAX_KERNELS);
vector<cl::Event> write_events(3);
vector<cl::Event> read_events(3);

float             *add_data_buffer;
float             *bn_data_buffer;
cl::Buffer        *add_buffer;
cl::Buffer        *bn_buffer;
cl_mem_ext_ptr_t  add_buffer_ddr;
cl_mem_ext_ptr_t  bn_buffer_ddr;

cl::Buffer *fn_get_clbuffer_from_name(char *name) {
  int i;
  // input buffer is model input?
  i = get_model_input_id(name);
  if (i != -1) return aInput[i].buffer;
  // input buffer is initializer?
  i = fn_get_initializer_by_name(name);
  if (i != -1) return aInitializer[i].buffer;
  // input buffer is node buffer?
  i = fn_get_node_by_output_name(name);
  if (i != -1) return aNode[i].buffer;
  return NULL;
}

/*
 * fn_init_fpga()
 *
 * initalizes the FPGA by loading the xclbin file
 *
 */
void fn_init_fpga() {
  cl_int err;

  if (verbose && verbose_level >= 2) printf("creating FPGA context...\n");

  binaryFile = xclbin_file_name;

 auto devices = xcl::get_xil_devices();
  auto device = devices[0];
  OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
  OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));
  std::string device_name = device.getInfo<CL_DEVICE_NAME>();
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  devices.resize(1);

  OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
  std::cout << "Device " << device_name.c_str() << ": program successful!" << std::endl;

  // kernels
  for (int k=0; k<num_kernels; k++) {
    char dummy[50];
    sprintf(dummy, "k_conv2D:{k_conv2D_%d}", k+1);
    OCL_CHECK(err, kernel_conv2d[k] = cl::Kernel(program, dummy, &err));
    std::cout << "Kernel successfully created" << std::endl;
  }

  if (verbose && verbose_level >= 2) printf("  completed\n");
}

void allocate_buffers() {

  cl_int err;

  if (verbose && verbose_level >= 2) printf("allocating buffers...\n");

  // every node has an output buffer
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      size_t size = aNode[n].O * aNode[n].HO * aNode[n].WO * sizeof(float);
      if (verbose && verbose_level >= 2) printf("  allocating output buffer for node %d (size %ld), name: %-50s\n", n, size, aNode[n].name);
      posix_memalign((void **)&aNode[n].data, 4096, size);
      aNode[n].buffer_ddr.flags = 0 | XCL_MEM_TOPOLOGY;
      aNode[n].buffer_ddr.obj   = aNode[n].data;
      aNode[n].buffer_ddr.param = 0;
      OCL_CHECK(err, aNode[n].buffer = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, size, &aNode[n].buffer_ddr, &err));
    }
  }

  // add temporal buffer
  size_t size=1000;
  posix_memalign((void**)&add_data_buffer, 4096, size);
  add_buffer_ddr.flags = 0 | XCL_MEM_TOPOLOGY;
  add_buffer_ddr.obj   = add_data_buffer;
  add_buffer_ddr.param = 0;
  OCL_CHECK(err, add_buffer = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size, &add_buffer_ddr, &err));

  // bn temporal buffer
  posix_memalign((void**)&bn_data_buffer, 4096, size);
  bn_buffer_ddr.flags = 0 | XCL_MEM_TOPOLOGY;
  bn_buffer_ddr.obj   = bn_data_buffer;
  bn_buffer_ddr.param = 0;
  OCL_CHECK(err, bn_buffer = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size, &bn_buffer_ddr, &err));  


  // every initializer has a buffer
  for (int i=0; i<num_initializers; i++) {
    if (aInitializer[i].valid) {
      size_t size = 1;
      for (int d=0; d<aInitializer[i].num_dimensions; d++) size = size * aInitializer[i].dimensions[d];
      size = size * sizeof(float);
      if (verbose && verbose_level >= 2) printf("  allocating buffer for initializer %d (size %ld), name: %-50s\n", i, size, aInitializer[i].name);
      //
      aInitializer[i].buffer_ddr.flags = 0 | XCL_MEM_TOPOLOGY;
      aInitializer[i].buffer_ddr.obj   = aInitializer[i].data;
      aInitializer[i].buffer_ddr.param = 0; 
      OCL_CHECK(err, aInitializer[i].buffer = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size, &aInitializer[i].buffer_ddr, &err));
    }
  }

  // finally we need a buffer for every input
  for (int i=0; i<num_inputs; i++) {
    if (aInput[i].valid) {
      size_t size = 1;
      if (aInput[i].num_dimensions == 4) {
	printf("WARNING: Input model with four dimensions, asuming three dimensions (1, 2, 3). Dimension 0 assumed to be batch size\n");
	for (int d=1; d<aInput[i].num_dimensions; d++) size = size * aInput[i].dimensions[d];
      } else if (aInput[i].num_dimensions == 3) {
        for (int d=0; d<aInput[i].num_dimensions; d++) size = size * aInput[i].dimensions[d];
      } else {
	printf("Number of dimensions for input model not supported (%d)\n", aInput[i].num_dimensions);
	exit(1);
      }
      int num_items = size;   // TODO: remove
      size = size * sizeof(float);
      if (verbose && verbose_level >= 2) printf("  allocating buffer for input model %d (size %ld), name %-50s\n", i, size, aInput[i].name);
      posix_memalign((void **)&aInput[i].data, 4096, size);
      aInput[i].buffer_ddr.flags = 0 | XCL_MEM_TOPOLOGY;
      aInput[i].buffer_ddr.obj   = aInput[i].data;
      aInput[i].buffer_ddr.param = 0;       
      OCL_CHECK(err, aInput[i].buffer = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size, &aInput[i].buffer_ddr, &err));
      for (int x=0; x<num_items; x++) aInput[i].data[x] = 1.f; // TODO: remove
    }
  }

  if (verbose && verbose_level >= 2) printf("  completed\n");
}

void copy_to_fpga(cl::Buffer *buf) {
  cl_int err;
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buf}, 0, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
}

void copy_from_fpga(cl::Buffer *buf) {
  cl_int err;
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*buf}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
}

void deallocate_buffers() {
  if (verbose && verbose_level >= 2) printf("deallocating buffers...\n");

  // we deallocate buffers of nodes, initializers, and inputs
  for (int n=0; n<num_nodes; n++) if (aNode[n].valid) free(aNode[n].buffer);
  for (int i=0; i<num_initializers; i++) if (aInitializer[i].valid) free(aInitializer[i].buffer);
  for (int i=0; i<num_inputs; i++) if (aInput[i].valid) free(aInput[i].buffer);

  if (verbose && verbose_level >= 2) printf("  completed\n");
}

/*
 * run_node_on_fpga()
 *
 * This function launches the kernel for a given node
 *
 * The node is passed as argument
 * The kernel identifier is passed as argument aswell
 */

void fn_run_node_on_fpga(int n, int k, int first_O, int last_O, int first_HI, int last_HI) {

  //
  int         I, HI, WI, O, HO, WO;
  int         num_read_rows;
  int         pt, pb, pl, pr;
  int         kh, kw;
  int         sh, sw;
  int         i_iter, first_o_iter, last_o_iter;
  int         read_offset, write_offset;
  int         relu_enable;
  float       relu_factor;
  int         stm_enable;
  int         bn_enable;
  int         bn_relu_enable;
  float       bn_relu_factor;
  int         maxpool_enable, avgpool_enable;
  int         clip_enable, shift_enable;
  int         add_enable, add_relu_enable;
  int         clip_min, clip_max;
  int         shift_dir, shift_pos;
  int         upsize_factor;
  int         write_weight_buffer, read_weight_buffer;
  int         first_row_weight_buffer;
  int         read_from_mem, write_to_mem;
  int         read_from_buffer0, write_to_buffer0;
  int         read_from_buffer1, write_to_buffer1;

  int         i_weight, i_bias, i_bn, n_parent, n_add;

  if (n==-1) return;
  if (!aNode[n].valid) return;
  if (strcmp(aNode[n].type, "HLSinf")) return;

  i_weight = -1;
  i_bias = -1;
  i_bn = -1;
  n_parent = -1;
  n_add = -1;

  if (verbose && verbose_level >= 1) printf("    running %s (keyword %s, O: %d - %d, HI %d - %d) (fpga)\n", aNode[n].name, aNode[n].keyword, first_O, last_O, first_HI, last_HI);

  // input clbuffer
  cl::Buffer *buffer_i = fn_get_clbuffer_from_name(aNode[n].inputs[0]);
  if (verbose && verbose_level >= 2) fn_buffer_stats_by_name(aNode[n].inputs[0], (char *)"      input  : ");

  // weight and bias clbuffers
  cl::Buffer *buffer_w = fn_get_clbuffer_from_name(aNode[n].inputs[1]);
  cl::Buffer *buffer_b = fn_get_clbuffer_from_name(aNode[n].inputs[2]);
  if (verbose && verbose_level >= 2) fn_buffer_stats_by_name(aNode[n].inputs[1], (char *)"      weights: ");
  if (verbose && verbose_level >= 2) fn_buffer_stats_by_name(aNode[n].inputs[2], (char *)"      bias   : ");

  // output data clbuffer
  cl::Buffer *buffer_o = aNode[n].buffer;
  // bn buffer
  cl::Buffer *buffer_bn = bn_buffer;
  int i=-1;
  if (!strcmp(aNode[n].keyword, "cbr")) i = 3;
  if (!strcmp(aNode[n].keyword, "cb")) i = 3;
  if (!strcmp(aNode[n].keyword, "cbar")) i = 4;
  if (i!=-1) {
    buffer_bn = fn_get_clbuffer_from_name(aNode[n].inputs[i]);
    if (verbose && verbose_level >= 2) fn_buffer_stats_by_name(aNode[n].inputs[i], (char *)"      bn     : ");
  }

  // add buffer
  cl::Buffer *buffer_add = add_buffer;
  if (!strcmp(aNode[n].keyword, "cbar")) {
    buffer_add = fn_get_clbuffer_from_name(aNode[n].inputs[3]);
    if (verbose && verbose_level >= 2) fn_buffer_stats_by_name(aNode[n].inputs[3], (char *)"      add    : ");
  }

  // default arguments
  I                       = aNode[n].I;
  HI                      = aNode[n].HI;
  WI                      = aNode[n].WI;
  O                       = aNode[n].O;
  HO                      = aNode[n].HO;
  WO                      = aNode[n].WO;
  num_read_rows           = HI;
  pt                      = aNode[n].hlsinf_pt_conv;
  pb                      = aNode[n].hlsinf_pb_conv;
  pl                      = aNode[n].hlsinf_pl_conv;
  pr                      = aNode[n].hlsinf_pr_conv;
  sh                      = aNode[n].hlsinf_sh_conv;
  sw                      = aNode[n].hlsinf_sw_conv;
  //
  i_iter                  = I / CPI;
  //
  first_o_iter            = (first_O / CPO);
  last_o_iter             = ((last_O+1) / CPO) - 1;
  read_offset             = 0;
  write_offset            = 0;
  //
  relu_enable             = false;
  relu_factor             = 0.f;
  stm_enable              = false;
  bn_enable               = false;
  bn_relu_enable          = false;
  bn_relu_factor          = 0.f;
  maxpool_enable          = false;
  avgpool_enable          = false;
  clip_enable             = false;
  shift_enable            = false;
  add_enable              = false;
  add_relu_enable         = false;
  clip_min                = 0;
  clip_max                = 0;
  shift_dir               = 0;
  shift_pos               = 0;
  upsize_factor           = 1;
  write_weight_buffer     = false;
  read_weight_buffer      = false;
  first_row_weight_buffer = false;
  read_from_mem           = true;
  read_from_buffer0       = false;
  read_from_buffer1       = false;
  write_to_mem            = true;
  write_to_buffer0        = false;
  write_to_buffer1        = false;

  // seting up arguments (depending on the keyword)
  if (!strcmp(aNode[n].keyword, "cbr"))  {bn_enable = true; bn_relu_enable = true;}
  if (!strcmp(aNode[n].keyword, "cbar")) {bn_enable = true; bn_relu_enable = false; add_enable = true; add_relu_enable = true;}
  if (!strcmp(aNode[n].keyword, "cb"))   {bn_enable = true;}
  if (!strcmp(aNode[n].keyword, "cr"))   {relu_enable = true;}

  if (verbose && verbose_level >= 2) {
    printf("      IxHIxWI: %4dx%4dx%4d OxHOxWO: %4dx%4dx%4d num_read_rows: %3d pads: %1d%1d%1d%1d SWxSW: %1dx%1d\n", I, HI, WI, O, HO, WO, num_read_rows, pt, pb, pl, pr, sh, sw);
    printf("      i_iter: %3d first_o_iter: %3d last_o_iter: %3d\n", i_iter, first_o_iter, last_o_iter);
    printf("      relu_enable: %1d stm_enable: %1d, relu_factor: %4.2f bn_enable: %1d\n", relu_enable, stm_enable, relu_factor, bn_enable);
    printf("      bn_relu_enable: %1d bn_relu_factor: %4.2f read_offset: %3d write_offset: %3d\n", bn_relu_enable, bn_relu_factor, read_offset, write_offset);
    printf("      maxpool_enable: %1d avgpool_enable: %1d clip_enable: %1d shift_enable: %1d add_enable: %1d\n", maxpool_enable, avgpool_enable, clip_enable, shift_enable, add_enable);
    printf("      add_relu_enable: %1d clip_min: %3d clip_max: %3d shift_dir: %3d shift_pos: %3d upsize_factor: %1d\n", add_relu_enable, clip_min, clip_max, shift_dir, shift_pos, upsize_factor);
    printf("      write_weight_buffer: %1d read_weight_buffer: %1d first_row_weight_buffer: %3d\n", write_weight_buffer, read_weight_buffer, first_row_weight_buffer);
    printf("      read_from_mem: %1d read_from_buffer0: %1d read_from_buffer1: %1d\n", read_from_mem, read_from_buffer0, read_from_buffer1);
    printf("      write_to_mem: %1d write_to_buffer0: %1d write_to_buffer1: %1d\n", write_to_mem, write_to_buffer0, write_to_buffer1);
  }


  cl_int err;
  int arg = 0;
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_i));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_add));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, HI));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, WI));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, HO));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, WO));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, num_read_rows));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, pt));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, pb));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, pl));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, pr));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, sh));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, sw));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, I));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, O));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, i_iter));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, first_o_iter));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, last_o_iter));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, relu_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, stm_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, relu_factor));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, bn_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, bn_relu_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, bn_relu_factor));  
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_w));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_b));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_bn));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, *buffer_o));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_offset));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_offset));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, maxpool_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, avgpool_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, clip_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, shift_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, add_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, add_relu_enable));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, clip_min));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, clip_max));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, shift_dir));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, shift_pos));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, upsize_factor));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_weight_buffer));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_weight_buffer));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, first_row_weight_buffer));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_from_mem));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_from_buffer0));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, read_from_buffer1));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_to_mem));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_to_buffer0));
  OCL_CHECK(err, err = kernel_conv2d[k].setArg(arg++, write_to_buffer1));
  //
  //
  OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel_conv2d[k], 0, 1, 1, NULL, &kernel_events[k]));
  set_callback(kernel_events[k], "ooo_queue");
  OCL_CHECK(err, err = kernel_events[k].wait());

  if (verbose && verbose_level >= 2) {
    copy_from_fpga(aNode[n].buffer);
    fn_buffer_stats_by_name(aNode[n].outputs[0], (char*)"      out    : ");
  }
}

#endif


