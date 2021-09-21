/*
 * Open-CL related functions
 */

#include "test_conv2D.h"



void cleanup() {
printf("\n\n");
printf(KRED "CLEANUP function to be implemented" KRST);
printf("\n");

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

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
unsigned char* load_file(const char *filename, size_t *size_ret) {
  FILE *fp = fopen(filename, "rb");
  if(!fp) {
    printf("Failed to open the input file: %s.\n", filename);
    return NULL;
  }
  long size;
  unsigned char *buffer;

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  buffer = (unsigned char*)malloc(size);
  assert(buffer && "Malloc failed");
  size_t fread_sz = fread(buffer, size, 1, fp);
  if (fread_sz == 0) {
    printf("Failed to read from the AOCX file (fread).\n");
    fclose(fp);
    free(const_cast<unsigned char*>(buffer));
    return NULL;
  }
  fclose(fp);
  *size_ret = size;
  return buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#ifdef OPENCL_TEST
//int fn_init_fpga() {
//  cl_int err;
//
//  printf(KGRN "Initializing OpenCL\n" KRST);
//  //  Set the current working directory to be the same as the directory
//  //  containing the running executable.
//    if (!setCwdToExeDir()) {
//      return false;
//    }
//  auto devices = get_altera_devices();
//  auto device = devices[0];
//  
//  std::string device_name = device.getInfo<CL_DEVICE_NAME>();
//
//  printf("Create context...\n");
//  OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
//
//
//  printf("Create command queue...\n");
//  OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE/* | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/, &err));
//
//  printf("Reading kernel binary file\n");
//  auto fileBuf = read_binary_file(binaryFile);
//
//  printf("Creating binary file\n");
//  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
//  printf("  bins size : %ld\n", bins.size());
//  devices.resize(1);
//
//  printf("Creating program\n");
//  OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
//  //cl::Program program(context, devices, bins, &bins_status, &err);
//
//  printf("Building program\n");
//  program.build(NULL, NULL, NULL);
//
//  
//  // kernels
//  printf("Creating kernels\n");
//  //OCL_CHECK(err, kernel_conv2d[0] = cl::Kernel(program, "k_relu", &err));
//  printf("Macro NUM_KERNELS set to: %d\n", NUM_KERNELS); 
//  const char*  k_name = "k_conv2D";
//  for (int krnl=0; krnl<NUM_KERNELS; krnl++) {
//    // Create the kernel with the program that was just built.
//    char c_k_name[50];
//    //sprintf(c_k_name, "kernel:{k_%d_%s}", krnl+1, k_name);
//    sprintf(c_k_name, "%s", k_name);
//    printf("Creating %s  index %d\n", c_k_name, krnl);   
//    OCL_CHECK(err, kernel_conv2d[krnl] = cl::Kernel(program, c_k_name, &err));
//    std::cout << "Kernel sucessfully created" << std::endl ; 
//  }
//
//  printf("OpenCL for device \"%s\" initialized\n", device_name.c_str() );
//  return true;
//
//}

int fn_init_fpga(){
    printf(KGRN "Initializing OpenCL environment for Altera \n" KRST);
  cl_int status = 0;
  cl_int bin_status = 0;
  const unsigned char *bin_file;
  size_t bin_file_len = 0;
  char * device_name = NULL;

  // Set the current working directory to be the same as the directory
  // containing the running executable.
  if (!setCwdToExeDir()) {
    return false;
  }

  // initialize to NULL all kernel pointers and their associate events pointers
  for (int i = 0; i < MAX_KERNELS; i++) {
    kernel_conv2D[i] = NULL;
    kernel_events[i] = NULL; 
  }

  // Find platform 
  // This code assumes there is just ONE Intel/Altera platform, or at least the Intel platform is the first one on the list
  // get the OpenCL platform ID
  printf("Searching platform...\n");
  if (use_emulator) {
    printf("      ... intel emulation platform\n");
    platform = findPlatform("Intel(R) FPGA Emulation Platform for OpenCL(TM)");

  } else {
    printf("      ... intel fpga physical platform\n");
    platform = findPlatform("Intel(R) FPGA SDK for OpenCL(TM)");
  }
  if(platform == NULL) {
    printf("ERROR: Unable to find Intel(R) FPGA OpenCL platform.\n");
    cleanup();
    return false;
  }

  printf("Get device id for platform id...\n");
  // Get Device ID from platform
  CHECK(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, 0));
  /*
  // get device name for "print info" purposes
  //std::string device_name = device->getInfo<CL_DEVICE_NAME>();
  size_t valueSize;
  clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &valueSize);
  device_name = (char*) malloc(valueSize);
  clGetDeviceInfo(device, CL_DEVICE_NAME, valueSize, device_name, NULL);
  printf("Device: %s\n", device_name);
   */
  printf("Create context...\n");
  // Create the context.
  context = clCreateContext(0, 1, &device, &oclContextCallback, 0, &status);
  CHECK(status);

  printf("Create command queue...\n");
  // Create the command queue
  //cq = clCreateCommandQueue(context, device, 0, &status);
  q = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &status);
  CHECK(status);

  printf("Loading kernel binary file\n");
  //bin_file = load_file(binary_file_path, &bin_file_len);
  bin_file = load_file(binaryFile, &bin_file_len);

  // Create the program.
  printf("Creating program with binary file\n");
  program = clCreateProgramWithBinary(context, 1, &device, &bin_file_len, &bin_file, &bin_status, &status);
  printf("create program status  %d\n", status);
  fflush(stdout);
  CHECK(status);

  // Build the program that was just created.
  printf("Building kernel program\n");
  CHECK(clBuildProgram(program, 1, &device, "", NULL, NULL));

  for (int krnl=0; krnl<NUM_KERNELS; krnl++) {
    // Create the kernel with the program that was just built.
    char c_k_name[50];
    //sprintf(c_k_name, "kernel:{k_%d_%s}", krnl+1, k_name);
    //sprintf(c_k_name, "%s", k_name);
    // c_k_name: A kernel is a function declared in a program. 
    //   A kernel object encapsulates the specific __kernel function declared in a program and the argument values
    //   to be used when executing this __kernel function
    // c_k_name value is the same for all the conv2D kernel functions in hls library
    sprintf(c_k_name, "%s", "k_conv2D");
    printf(KGRN "Creating kernel #%d:  %s\n" KNRM, krnl, c_k_name);   

    // formar correctamente la asignacion del kernel al array.....
    //kernel[krnl] = clCreateKernel(program, c_k_name, &status);
    kernel_conv2D[krnl] = clCreateKernel(program, c_k_name, &status);
    CHECK(status);
  }
  //··········

  // release mem pointers
  if (device_name != NULL) {
    free (device_name);
  }

  printf("OpenCL initialized\n");
  return true;

}

//-----------------------------------------------------------------------------
// release variables initialized in fpga_init() function
//-----------------------------------------------------------------------------
void fn_release_fpga( void ){
    printf(KGRN "Releasing OpenCL environment for Altera \n" KRST);

  if (program) {
    clReleaseProgram(program);
    program = NULL;
  }
  if (q) {
    clReleaseCommandQueue(q);
    q = NULL;
  }
  if (context) {
    clReleaseContext(context);
    context = NULL;
  }
}

#endif

//-----------------------------------------------------------------------------
// end of file: opencl.cpp
//-----------------------------------------------------------------------------

