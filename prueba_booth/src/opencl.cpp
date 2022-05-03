#include "tb.h"

void parse_arguments(int argc, char **argv) {
#ifdef OPENCL_TEST
  if (argc != 2 || argc != 3) {
    printf("%s <probability> <xclbin file>\n", argv[0]);
    exit(1);
  } else {
	  if(argc == 3) {
		  prob = atoi(argv[1]);
		  binaryFile = argv[2];
	  } else {
		  binaryFile = argv[1];
	  }
  }
#else
  if (argc > 2) prob =  atoi(argv[1]);
#endif
}


#ifdef OPENCL_TEST
void set_callback(cl::Event event, const char *queue_name) {
  cl_int err;
  OCL_CHECK(err, err = event.setCallback(CL_COMPLETE, event_cb, (void *)queue_name));
}

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

// Sets the callback for a particular event


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
    char dummy[50];
    sprintf(dummy, "top:{top_%d}", 1);
  //  printf("dummy %s\n", dummy);
    OCL_CHECK(err, kernel_conv2d = cl::Kernel(program, dummy, &err));
    std::cout << "Kernel sucessfully created" << std::endl ;
}

void allocate_buffers() {
  // Now we allocate those buffers in the FPGA (for OpenCL)
  cl_int err;
  data_in_ddr0.flags  =  0 | XCL_MEM_TOPOLOGY;
  data_in_ddr0.obj = x0;
  data_in_ddr0.param = 0;

  data_in_ddr1.flags  =  0 | XCL_MEM_TOPOLOGY;
  data_in_ddr1.obj = x1;
  data_in_ddr1.param = 0;

  data_in_ddr2.flags  =  0 | XCL_MEM_TOPOLOGY;
  data_in_ddr2.obj = x2;
  data_in_ddr2.param = 0;

  data_in_ddr3.flags  =  0 | XCL_MEM_TOPOLOGY;
  data_in_ddr3.obj = x3;
  data_in_ddr3.param = 0;

  out_ddr.flags  = 0 | XCL_MEM_TOPOLOGY;
  out_ddr.obj = y;
  out_ddr.param = 0;

  kernel_ddr.flags  = 0 | XCL_MEM_TOPOLOGY;
  kernel_ddr.obj = w;
  kernel_ddr.param = 0;

  OCL_CHECK(err, buffer_i0      = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_x_in_bytes*I, &data_in_ddr0, &err));
  OCL_CHECK(err, buffer_i1      = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_x_in_bytes*I, &data_in_ddr1, &err));
  OCL_CHECK(err, buffer_i2      = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_x_in_bytes*I, &data_in_ddr2, &err));
  OCL_CHECK(err, buffer_i3      = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_x_in_bytes*I, &data_in_ddr3, &err));
  OCL_CHECK(err, buffer_o    = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_WRITE_ONLY  | CL_MEM_USE_HOST_PTR , size_y_in_bytes, &out_ddr, &err));
  OCL_CHECK(err, buffer_k    = new cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR , size_w_in_bytes, &kernel_ddr, &err));
}

void copy_to_fpga() {
  cl_int err;
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_i0}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());

  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_i1}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());

  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_i2}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());

  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_i3}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());

  OCL_CHECK(err, err = q.enqueueMigrateMemObjects( {*buffer_k}, 0 /*0 means from host*/, NULL, &write_events[0]));
  set_callback(write_events[0], "ooo_queue");
  OCL_CHECK(err, err = write_events[0].wait());
}

void copy_from_fpga() {
  cl_int err;
  // std::cout << "Getting Results (Device to Host)..." << std::endl;
  std::vector<cl::Event> eventList;
  eventList.push_back(kernel_events);
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({*buffer_o}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &read_events[0]));
  set_callback(read_events[0], "ooo_queue");
  OCL_CHECK(err, err = read_events[0].wait());
  OCL_CHECK(err, err = q.flush());
  OCL_CHECK(err, err = q.finish());
}


void run_kernel() {
	//void top(ap_uint<READ_GROUP_SIZE*DATA_SIZE> *x0, ap_uint<READ_GROUP_SIZE*DATA_SIZE> *x1, ap_uint<READ_GROUP_SIZE*DATA_SIZE> *x2, ap_uint<READ_GROUP_SIZE*DATA_SIZE> *x3, ap_uint<I*O*DATA_SIZE> *w, ap_uint<512> *y) {
	//  top(x0, x1, x2, x3, (ap_uint<8*O*I> *)w, (ap_uint<512> *)y);
	cl_int err;
	int arg = 0;
	OCL_CHECK(err, err = kernel_conv2d.setArg(arg++, *buffer_i0));
	OCL_CHECK(err, err = kernel_conv2d.setArg(arg++, *buffer_i1));
	OCL_CHECK(err, err = kernel_conv2d.setArg(arg++, *buffer_i2));
	OCL_CHECK(err, err = kernel_conv2d.setArg(arg++, *buffer_i3));
	OCL_CHECK(err, err = kernel_conv2d.setArg(arg++, *buffer_i0));
	OCL_CHECK(err, err = kernel_conv2d.setArg(arg++, *buffer_k));
	OCL_CHECK(err, err = kernel_conv2d.setArg(arg++, *buffer_o));
    OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel_conv2d, 0, 1, 1, NULL, &kernel_events));
    set_callback(kernel_events, "ooo_queue");
    OCL_CHECK(err, err = kernel_events.wait());
}
#endif

