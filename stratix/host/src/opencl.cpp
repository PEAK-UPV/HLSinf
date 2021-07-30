/*
 * Open-CL related functions
 */

#include "test_conv2D.h"



void cleanup() {
printf("\n\n");
printf(KRED "CLEANUP function to be implemented" KRST);
printf("\n");

}

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



std::vector<cl::Device> get_devices(const std::string &vendor_name) {
  size_t i;
  cl_int err;
  std::vector<cl::Platform> platforms;
  OCL_CHECK(err, err = cl::Platform::get(&platforms));
  cl::Platform platform;
  for (i = 0; i < platforms.size(); i++) {
    platform = platforms[i];
    OCL_CHECK(err, std::string platformName =
                       platform.getInfo<CL_PLATFORM_NAME>(&err));
    if (!(platformName.compare(vendor_name))) {
      std::cout << "Found Platform" << std::endl;
      std::cout << "Platform Name: " << platformName.c_str() << std::endl;
      break;
    }
  }
  if (i == platforms.size()) {
    std::cout << "Error: Failed to find "<< vendor_name.c_str() << " platform" << std::endl;
    std::cout << "Found the following platforms : " << std::endl;
    for (size_t j = 0; j < platforms.size(); j++) {
        platform = platforms[j];
        OCL_CHECK(err, std::string platformName =
                           platform.getInfo<CL_PLATFORM_NAME>(&err));
        std::cout << "Platform Name: " << platformName.c_str() << std::endl;        
    }
    exit(EXIT_FAILURE);
  }
  // Getting ACCELERATOR Devices and selecting 1st such device
  std::vector<cl::Device> devices;
  //OCL_CHECK(err, err = platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices));
  OCL_CHECK(err, err = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices));

  printf("Number of devices found: %ld\n\n", devices.size());
  //for (i = 0; i < devices.size(); i++ ) {
  //  printf("  #%ld  %s\n", i, devices[i].getInfo)
  //}

  return devices;
}

std::vector<cl::Device> get_altera_devices() {
  if (use_emulator) {
    return get_devices("Intel(R) FPGA Emulation Platform for OpenCL(TM)");
  } 
  else {
    return get_devices("Intel(R) FPGA SDK for OpenCL(TM)"); 
  }
}


std::vector<unsigned char>
read_binary_file(const std::string &xclbin_file_name) {
  std::cout << "INFO: Reading " << xclbin_file_name << std::endl;
  FILE *fp;
  if ((fp = fopen(xclbin_file_name.c_str(), "r")) == NULL) {
    printf("ERROR: %s xclbin not available please build\n",
           xclbin_file_name.c_str());
    exit(EXIT_FAILURE);
  }
  // Loading XCL Bin into char buffer
  std::cout << "Loading: '" << xclbin_file_name.c_str() << "'\n";
  std::ifstream bin_file(xclbin_file_name.c_str(), std::ifstream::binary);
  bin_file.seekg(0, bin_file.end);
  auto nb = bin_file.tellg();
  bin_file.seekg(0, bin_file.beg);
  std::vector<unsigned char> buf;
  buf.resize(nb);
  bin_file.read(reinterpret_cast<char *>(buf.data()), nb);
  return buf;
}


const char *getErrorString(cl_int error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}

#ifdef OPENCL_TEST
int fn_init_fpga() {
  cl_int err;

  printf(KGRN "Initializing OpenCL\n" KRST);
  //  Set the current working directory to be the same as the directory
  //  containing the running executable.
    if (!setCwdToExeDir()) {
      return false;
    }
  auto devices = get_altera_devices();
  auto device = devices[0];
  
  std::string device_name = device.getInfo<CL_DEVICE_NAME>();

  printf("Create context...\n");
  OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));


  printf("Create command queue...\n");
  OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE/* | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/, &err));

  printf("Reading kernel binary file\n");
  auto fileBuf = read_binary_file(binaryFile);

  printf("Creating binary file\n");
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  printf("  bins size : %ld\n", bins.size());
  devices.resize(1);

  printf("Creating program\n");
  OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
  //cl::Program program(context, devices, bins, &bins_status, &err);

  printf("Building program\n");
  program.build(NULL, NULL, NULL);

  
  // kernels
  printf("Creating kernels\n");
/*  for (int kernel=0; kernel<NUM_KERNELS; kernel++) {
    char dummy[50];
    //sprintf(dummy, "k_conv2D:{k_conv2D_%d}", kernel+1);
    strcpy (dummy, "k_relu");
  //  printf("dummy %s\n", dummy);
    OCL_CHECK(err, kernel_conv2d[kernel] = cl::Kernel(program, dummy, &err));
    std::cout << "Kernel sucessfully created" << std::endl ;
  }
*/


  //OCL_CHECK(err, kernel_conv2d[0] = cl::Kernel(program, "k_relu", &err));
  printf("Macro NUM_KERNELS set to: %d\n", NUM_KERNELS); 
  const char*  k_name = "k_conv2D";
  for (int krnl=0; krnl<NUM_KERNELS; krnl++) {
    // Create the kernel with the program that was just built.
    char c_k_name[50];
    //sprintf(c_k_name, "kernel:{k_%d_%s}", krnl+1, k_name);
    sprintf(c_k_name, "%s", k_name);
    printf("Creating %s  index %d\n", c_k_name, krnl);   
    OCL_CHECK(err, kernel_conv2d[krnl] = cl::Kernel(program, c_k_name, &err));
    std::cout << "Kernel sucessfully created" << std::endl ;
  
  }

  printf("OpenCL for device \"%s\" initialized\n", device_name.c_str() );
  return true;

}
#endif
