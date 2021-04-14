/*
 * Open-CL related functions
 */

#include "test_conv2D.h"

#ifdef OPENCL_TEST
// OpenCL event callback function printing message
void event_cb(cl_event event1, cl_int cmd_status, void *data) {
  cl_int err;
  cl_command_type command;
  cl::Event event(event1, true);
  OCL_CHECK(err, err = event.getInfo(CL_EVENT_COMMAND_TYPE, &command));
  cl_int status;
  OCL_CHECK(err, err = event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS, &status));
  /*const char *command_str;
  const char *status_str;
  switch (command) {
    case CL_COMMAND_READ_BUFFER:          command_str = "buffer read"; break;
    case CL_COMMAND_WRITE_BUFFER:         command_str = "buffer write"; break;
    case CL_COMMAND_NDRANGE_KERNEL:       command_str = "kernel"; break;
    case CL_COMMAND_MAP_BUFFER:           command_str = "kernel"; break;
    case CL_COMMAND_COPY_BUFFER:          command_str = "kernel"; break;
    case CL_COMMAND_MIGRATE_MEM_OBJECTS:  command_str = "buffer migrate"; break;
    default:                              command_str = "unknown";
  }
  switch (status) {
    case CL_QUEUED:       status_str = "Queued"; break;
    case CL_SUBMITTED:    status_str = "Submitted"; break;
    case CL_RUNNING:      status_str = "Executing"; break;
    case CL_COMPLETE:     status_str = "Completed"; break;
  }
  printf("[%s]: %s %s\n", reinterpret_cast<char *>(data), status_str, command_str);
  fflush(stdout);*/
}
#endif

#ifdef OPENCL_TEST
// Sets the callback for a particular event
void set_callback(cl::Event event, const char *queue_name) {
  cl_int err;
  OCL_CHECK(err, err = event.setCallback(CL_COMPLETE, event_cb, (void *)queue_name));
}
#endif


#ifdef OPENCL_TEST
void fn_init_fpga() {
  cl_int err;

  std::cout << "Creating Context..." << std::endl;
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
  for (int kernel=0; kernel<NUM_KERNELS; kernel++) {
    char dummy[50];
    sprintf(dummy, "k_conv2D:{k_conv2D_%d}", kernel+1);
  //  printf("dummy %s\n", dummy);
    OCL_CHECK(err, kernel_conv2d[kernel] = cl::Kernel(program, dummy, &err));
    std::cout << "Kernel sucessfully created" << std::endl ;
  }
  //OCL_CHECK(err, kernel_conv2d[0] = cl::Kernel(program, "k_conv2D", &err));
}
#endif
