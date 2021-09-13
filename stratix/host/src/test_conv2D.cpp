// --------------------------------------------------------------------------------------------------------------
// HLSinf kernels
// Version: 1.0
// copyright (c) 2020, Universidad Politècnica de València (UPV), GAP research group
// Date: March 2021
// Authors: GAP Research Group (UPV)
//     José Flich Cardo
//     Jorge García Martínez
//     Izan Catalán Gallarch
//     Carles Hernández Luz
//
// contact: jflich@disca.upv.es
// All rights reserved

//
// test_conv2D. Test for the conv2D kernel
//
//  Data formats:
//
//  - weights (kernel)   : GO x GI x CPO x CPI x KH x KW
//  - bias               : O
//  - data_in            : I x H x W
//  - data_out           : O x H x W
//
//  GO = ceil(O / CPO), GI = ceil(I / CPI)
//
// The kernel must have at least CPI channels and CPO channels, filled with zeroes if needed

// This test works both for C simulation and for Opencel
// In OpenCL the OPENCL_TEST define should be defined, if not
// C simulation is assumed instead

#include "test_conv2D.h"


//using std::vector;

// Global variables
int CONVS;                       // Number of convolutional layers
int KERNELS;                     // Number of FPGA kernels to use
int F;                           // Number of frames of the data
int W = W_SIM;                   // Width of the data
int H = H_SIM;                   // Height of the data
int I = I_SIM;                   // Number of input channels
int O = O_SIM;                   // Number of output channels
int HO = H_SIM;       			 // Output height
int WO = W_SIM;    				 // Output width
int I_kernel = I_SIM;  			 // Number of input channels for the kernel (filter) - padding
int O_kernel = O_SIM;			 // Number of output channels for the kernel (filter) - padding
int I_input = I_SIM;             // Number of input channels for the input data - padding (needed in GIHWCPI data format)
int O_output = O_SIM;            // Number of output channels for the output data - padding (needed in GIHWCPI data format)
int rows = H_SIM;				 // number of rows to compute by the kernel
int enable_upper_padding = 1;	 // enables the upper row of padding
int enable_lower_padding = 1;	 // enables the lower row of padding
int enable_relu = 1;			 // enables applying the relu activation functions
int enable_shift = 0;			 // enables applying shift to the output
int dir_shift = 0;     			 // shift direction (left or right)
int pos_shift = 0;				 // positions to shift
int enable_clipping = 0;		 // enables applying clipping to the output
int enable_maxpooling = 0;		 // enables the maxpooling layer
int enable_avgpooling = 0;		 // enables the avgpooling layer
int min_clip = 0;   			 // minimum clip value
int max_clip = 0;				 // maximum clip value
int i_iter = I_SIM/CPI; 		 // number of input iterations
int o_iter = O_SIM/CPO;			 // number of output iterations
int global_offset = 0;			 // global offset for the output data for the kernel
int GI = I_SIM/CPI;				 // number of groups for input channels
int GO = O_SIM/CPO;				 // number of groups for output channels
char *input_data_file;           // input data file with configurations to test
int deterministic_input_values;  // whether input data is randomly generated or not (deterministic needed in co-simulation)




// buffers
scoped_aligned_ptr<data_type> data_in;               // Input data buffer (format I x W x H)
scoped_aligned_ptr<data_type> out;                   // Output data buffer (format O x W x H)
scoped_aligned_ptr<data_type> kernel;                // Conv kernel buffers (format GO x GI x CPO x CPI x KH x KW) - for DirectConv and WinogradConv
scoped_aligned_ptr<data_type> dw_kernel;             // DW kernel (format I x KH x KW) - for DWS
scoped_aligned_ptr<data_type> pw_kernel;             // PW kernel (format GO x GI x CPO x CPI) - for DWS
scoped_aligned_ptr<data_type> bias;                  // Conv bias buffers (format O)
scoped_aligned_ptr<data_type> out_conv_cpu;          // Output data buffer for cpu (format O x W x H)
scoped_aligned_ptr<data_type> out_relu_cpu;          // Output data buffer for cpu (format O x W x H)
scoped_aligned_ptr<data_type> out_pool_cpu;		  // Output data fuffer for pool for cpu (format O x W/2 x H/2)

FILE *fp;

int use_emulator = 0;


#ifdef OPENCL_TEST

// OpenCL variables
//cl::Context context;                          // Context
//cl::CommandQueue q;                           // Command queue
//cl::Program program;                          // Program
//std::string binaryFile;                       // Binary file
//cl::Kernel kernel_conv2d[MAX_KERNELS];        // FPGA kernels
//vector<cl::Event> kernel_events(MAX_KERNELS); // Kernel events (completion)
//vector<cl::Event> read_events(1);             // Read events
//vector<cl::Event> write_events(3);            // Write events
//cl::Buffer *buffer_i;                         // input buffer
//cl::Buffer *buffer_o[MAX_CONVS];              // output buffers
//cl::Buffer *buffer_k[MAX_CONVS];              // Conv kernel buffers
//cl::Buffer *buffer_bias[MAX_CONVS];           // Conv bias buffers
//cl::Buffer *buffer_k_dw[MAX_CONVS];           // Conv kernel buffers (deepwise)
//cl::Buffer *buffer_k_pw[MAX_CONVS];           // Conv kernel buffers (pointwise)

cl_platform_id   platform = NULL;
cl_device_id     device   = NULL;
cl_context       context  = NULL;
cl_command_queue q        = NULL;
cl_program       program  = NULL;
char   *binaryFile;                       // Binary file
//static cl_kernel        kernel[MAX_KERNELS];
//static cl_event         kernel_events[MAX_KERNELS]; // Kernel events (completion)
cl_kernel        kernel_conv2D = NULL;
cl_event         kernel_events; // Kernel events (completion)

//cl_event         read_events[24];                // Read events
//cl_event         write_events[5];            // Write events


cl_mem buffer_i = NULL;                         // input buffer
cl_mem buffer_o = NULL;//[MAX_CONVS];              // output buffers
cl_mem buffer_k = NULL;//[MAX_CONVS];              // Conv kernel buffers
cl_mem buffer_bias = NULL;//[MAX_CONVS];           // Conv bias buffers
cl_mem buffer_k_dw = NULL;//[MAX_CONVS];           // Conv kernel buffers (deepwise)
cl_mem buffer_k_pw = NULL;//[MAX_CONVS];           // Conv kernel buffers (pointwise)


#endif




// Functions

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void abort (const char * msg){
  printf(KRED "%s" KNRM " \n", msg);
  printf("quit now\n\n");
  exit(1);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void compute(int *enable, int *cpu, int *retval) {

  if (*enable) {
    print_configuration();

    #ifndef USE_POOLING
    if (enable_maxpooling | enable_avgpooling) {
      print_message("Pooling not supported (disabled)");
      enable_maxpooling = 0; enable_avgpooling = 0;
    }
    #endif

    if (enable_maxpooling && enable_avgpooling) {
      print_message("MaxPooling and AvgPooling cannot be active at the same time (skipped)");
      *enable = 0;
    }

    #ifndef API8_DATA_TYPE
    if (enable_clipping || enable_shift) {
      print_message("Clipping/shift only for API8 (disabled)");
      enable_clipping = 0; enable_shift = 0;
    }
    #endif

    // Check, output must be at least as large as one write block

    #ifdef IHW_DATA_FORMAT
    if (HO * WO * O < WRITE_BLOCK_SIZE) {
      print_message("Output too small (skipped)");
      *enable = 0;
    }

    if (((H * W * (DATA_TYPE_WIDTH / 8)) % READ_BLOCK_SIZE) != 0) {
      print_message("Input channel size must be multiple of read block size (skipped)");
      *enable = 0;
    }

    if (((HO * WO * (DATA_TYPE_WIDTH / 8)) % WRITE_BLOCK_SIZE) != 0) {
      print_message("Output channel size must be multiple of write block size (skipped)");
      *enable = 0;
    }
    #endif

    //*enable = 0;
    //printf(KYEL " under devel, not allocating buffers, initializing data nor running kernel yet \n" KRST);

    if (*enable) {
      printf("allocate buffers\n");
      allocate_buffers();

      printf("initialize data\n");
      // generates input data in host depending on configuration "macros" and parameters at "input_data" file
      init_data();
      // at this point, generated data is stored in "data_in"


      #ifdef OPENCL_TEST
      // set events to move values in "data_in" to fpga memory
      copy_to_fpga();
      #endif

      #ifdef DEBUG_CPU
      print_input();
      print_bias();
      print_kernel();
      #endif

      // timing stats
      unsigned long long prof_time;
      struct timeval prof_t1;
      gettimeofday(&prof_t1, NULL);

      
      run_kernel();
       

      // timing
      struct timeval prof_t2;
      gettimeofday(&prof_t2, NULL);
      prof_time = ((prof_t2.tv_sec - prof_t1.tv_sec) * 1000000) + (prof_t2.tv_usec - prof_t1.tv_usec);
      unsigned long long time = prof_time;
      unsigned long long time_per_iteration = prof_time / i_iter / o_iter;
      unsigned long long expected_time_one_iteration = W * H * 333;
      unsigned long long expected_time = (expected_time_one_iteration * i_iter * o_iter) / 100000;
      float efficiency = ((float)expected_time / (float)time);

      print_timings(time, time_per_iteration, expected_time, efficiency);

      #ifdef OPENCL_TEST
      // OpenCL kernel time
      cl_ulong time_start, time_end;
      printf(KRED "WARNING under devel events profiling info disabled\n\n" KNRM);
      //kernel_events[0].getProfilingInfo(CL_PROFILING_COMMAND_START, &time_start);
      //kernel_events[0].getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);
      double diff = time_end-time_start;
      std::cout<< "TIME KERNEL = " << (diff/1000000)<<" ms \n"<<std::endl;
      #endif

      if (*cpu) {
        cpu_conv2D();
        #ifdef OPENCL_TEST
        copy_from_fpga();
        #endif
        int num_differences;
        data_type max_difference;
        *retval = check_result(&max_difference, &num_differences);

        print_check(*retval, float(max_difference), num_differences);

      } else {
        printf("\n");
      }


#ifdef HLS_DEBUG
      printf("\n");
      hls_debug();
      printf("\n\n");
#endif


      #ifdef DEBUG_CPU
      print_output();
      #endif

      printf("deallocate buffers\n");
      deallocate_buffers();

    }
  }
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int main(int argc, char **argv) {

  int retval = 0;
  int global_retval = 0;
  int cpu;
  int enable;


  if (argc == 1) {
    printf("Co-simulation test...\n");
    enable = 1;
    cpu = 1;
    deterministic_input_values = 1;
    for (int i=0; i<INSTANCES_SIM; i++)
    {
      compute(&enable, &cpu, &retval);
    }
    global_retval = retval;
  } else {

    printf("File-based test...\n");
    deterministic_input_values = 0;


    //#ifdef HLS_DEBUG
    printf("\n\n");
    //printf(KRED "HLS DEBUG ENABLED\n" KNRM);
    printf(KRED "  forcing data_in deterministic values\n" KNRM);
    printf("\n\n");
    deterministic_input_values = 1;
    //#endif


    parse_arguments(argc, argv);

    #ifdef OPENCL_TEST

    // reubicar este bucle
    printf(KRED "JM10 work in progress, reubicar este bucle\n" KNRM);
    //for (int i = 0; i < MAX_KERNELS; i++) {
    //kernel[i] = NULL;
    //}


    enable = fn_init_fpga();
    if (!enable) {
      printf(KRED "Error initializing fpga device\n" KNRM);
    } 
    else {
      printf(KGRN "OpenCL device succesfully initialized for test\n" KRST);
    }
    #endif


  
    printf("open test input data configuration file\n");
    if (open_test_file() == 1) {
      return 1;
    }

    printf("Process test intput data file\n");
    int file_line = 0;
    while (!read_test_file(&enable, &cpu)) {
    //while (!read_test_file(&enable, &cpu) && (file_line < 1)) {
      printf("Process test intput data file line #%2d\n", file_line);

      // Launh kernel wiht configuration read from file (one line contains the configuration of a "computation")
      compute(&enable, &cpu, &retval);
      
      printf("Test check results returned\n");
      if(retval == 0){
        printf(KGRN "  OK: RESULTS match for input file line #%d\n" KNRM, file_line + 1);
      } else {
        printf(KRED "  ERROR: RESULTS mismatch, retval=%d  for input file line #%d \n" KNRM, retval, file_line + 1);
      }

      if (enable) {
        global_retval = global_retval + retval;
      }
      
      file_line++;
    }

    close_test_file();
  }

    printf(KGRN "Finished reading input data file\n");
 
  #ifdef OPENCL_TEST
  fn_release_fpga();
  #endif

  printf("\n\n");
  printf(KCYN "End of test\n" KNRM);
 if(global_retval == 0){
   printf(" \n");
   printf(KGRN "  Results are good for all entries in input file \n" KNRM);
 } else {
   printf("\n");
   printf(KRED "  ERROR: Results mismatch detected, retval=%d \n" KNRM, retval);
 }
   printf(" \n");

 // Return 0 if outputs are correct
 return global_retval;
}




