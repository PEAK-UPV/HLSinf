/*
 * fpga.c
 *
 * This file includes all the support functions related to the FPGA device
 */

#include <hls_stream.h>
#include <hls_math.h>
#include "CL/cl_ext_xilinx.h"
#include "xcl2.hpp"

#include "runner.h"
#include "nodes.h"
#include "initializers.h"
#include "in_out.h"

// defines
#define MAX_KERNELS 20
#define NUM_KERNELS 1

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
vector<cl::Event> kernel_evens[MAX_KERNELS];

/*
 * fn_init_fpga()
 *
 * initalizes the FPGA by loading the xclbin file
 *
 */
void fn_init_fpga() {
  cl_int err;

  if (verbose) printf("creating FPGA context...\n");

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
  for (int k=0; k<NUM_KERNELS; k++) {
    char dummy[50];
    sprintf(dummy, "k_conv2D:{k_conv2D_%d}", k+1);
    OCL_CHECK(err, kernel_conv2d[k] = cl::Kernel(program, dummy, &err));
    std::cout << "Kernel successfully created" << std::endl;
  }

  if (verbose) printf("  completed\n");
}

void allocate_buffers() {

  cl_int err;

  // every node has an output buffer
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      aNode[n].buffer_ddr.flags = 0 | XCL_MEM_TOPOLOGY;
      aNode[n].buffer_ddr.obj   = aNode[n].buffer;
      aNode[n].buffer_ddr.param = 0;
      int size = aNode[n].O * aNode[n].HO * aNode[n].WO * sizeof(float);
      posix_memalign((void **)&aNode[n].buffer, 4096, size);
      OCL_CHECK(err, aNode[n].buffer = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size, &aNode[n].buffer_ddr, &err));
    }
  }

  // every initializer has a buffer
  for (int i=0; i<num_initializers; i++) {
    if (aInitializer[i].valid) {
      aInitializer[i].buffer_ddr.flags = 0 | XCL_MEM_TOPOLOGY;
      aInitializer[i].buffer_ddr.obj   = aInitializer[i].buffer;
      aInitializer[i].buffer_ddr.param = 0; 
      int size = 1;
      for (int d=0; d<aInitializer[i].num_dimensions; d++) size = size * aInitializer[i].dimensions[d];
      size = size * sizeof(float);
      posix_memalign((void **)&aInitializer[i].buffer, 4096, size);
      OCL_CHECK(err, aInitializer[i].buffer = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size, &aInitializer[i].buffer_ddr, &err));
    }
  }

  // finally we need an buffer for every input
  for (int i=0; i<num_inputs; i++) {
    if (aInput[i].valid) {
      aInput[i].buffer_ddr.flags = 0 | XCL_MEM_TOPOLOGY;
      aInput[i].buffer_ddr.obj   = aInput[i].buffer;
      aInput[i].buffer_ddr.param = 0;       
      int size = 1;
      for (int d=0; d<aInput[i].num_dimensions; d++) size = size * aInput[i].dimensions[d];
      size = size * sizeof(float);
      posix_memalign((void **)&aInput[i].buffer, 4096, size);
      OCL_CHECK(err, aInput[i].buffer = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size, &aInput[i].buffer_ddr, &err));
    }
  }
}

void dealocate_buffers() {
  // we deallocate buffers of nodes, initializers, and inputs
  for (int n=0; n<num_nodes; n++) if (aNode[n].valid) free(aNode[n].buffer);
  for (int i=0; i<num_initializers; i++) if (aInitializer[i].valid) free(aInitializer[i].buffer);
  for (int i=0; i<num_inputs; i++) if (aInput[i].valid) free(aInput[i].buffer);
}




