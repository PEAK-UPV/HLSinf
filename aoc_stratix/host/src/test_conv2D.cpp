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

//*********************************************************************************************************************
// 
//  Included Files
// 
//*********************************************************************************************************************

#include "test_conv2D.h"


//*********************************************************************************************************************
// 
//  Macro definitions
// 
//*********************************************************************************************************************

#ifdef FPGA_STRATIX_10MX
  //#define KERNEL_FREQUENCY_MHZ 43  BSP 20.4 WITH TIMING VIOLATIONS
  //#define KERNEL_FREQUENCY_MHZ ((float)316.67)
  #define KERNEL_FREQUENCY_MHZ ((float)358.33)
#else
  // let's set the alveo u200 as the default device
//iffdef FPGA_ALVEO_U200
  #define KERNEL_FREQUENCY_MHZ 333
#endif

//*********************************************************************************************************************
// 
//  Public Global variables
// 
//*********************************************************************************************************************
//#include "ihc_apint.h"
//using std::vector;

//bfloat16 aaaaa;
//bfloat16
//ap_int<4,11,5>

int16_t unknown_type; 
//half    unknown2;
//typedef ac_fixed<__HLS_AC_W_SHORT, __HLS_AC_I_SHORT, true, AC_RND, AC_SAT>

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
scoped_aligned_ptr<data_type> data_in ;               // Input data buffer (format I x W x H)
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

// kernels execution time
//double kernels_start_time;
//double kernels_end_time;
double kernels_execution_time = 0;

#ifdef OPENCL_TEST

// OpenCL variables
cl_platform_id   platform = NULL;
cl_device_id     device   = NULL;
cl_context       context  = NULL;
//cl_command_queue q        = NULL;
cl_program       program  = NULL;
char   *binaryFile;                       // Binary file

cl_command_queue queues[K_NUM_KERNELS];
cl_kernel        kernels[K_NUM_KERNELS];
cl_event         kernel_events[K_NUM_KERNELS];

cl_mem buffer_i = NULL;                         // input buffer
cl_mem buffer_o = NULL;//[MAX_CONVS];              // output buffers
cl_mem buffer_k = NULL;//[MAX_CONVS];              // Conv kernel buffers
cl_mem buffer_bias = NULL;//[MAX_CONVS];           // Conv bias buffers
cl_mem buffer_k_dw = NULL;//[MAX_CONVS];           // Conv kernel buffers (deepwise)
cl_mem buffer_k_pw = NULL;//[MAX_CONVS];           // Conv kernel buffers (pointwise)


#endif


//*********************************************************************************************************************
// 
//  Function definitions
// 
//*********************************************************************************************************************


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
      allocate_buffers();

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

      // run kernel
      //run_kernel();
      run_aoc_kernels();
       
      // compute execution timing
      unsigned long long time;
      unsigned long long time_per_iteration ;
      unsigned long long expected_time_one_iteration;
      unsigned long long expected_time;
      float efficiency;

//#ifdef FPGA_STRATIX_10MX
//      cl_int f_ret;
//      cl_ulong time_start;
//      cl_ulong time_end;
//      cl_ulong diff;
//      cl_ulong expected_cycles_per_iteration;
//      cl_ulong expected_iterations;
//
//      // OpenCL kernel time
//      f_ret = clGetEventProfilingInfo(kernel_events[0], CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
//      if (f_ret != CL_SUCCESS) printf(KRED " Error reading kernel event info PROFILING_COMMAND_START \n" KNRM);
//      f_ret = clGetEventProfilingInfo(kernel_events[0], CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
//      if (f_ret != CL_SUCCESS) printf(KRED " Error reading kernel event info PROFILING_COMMAND_END \n" KNRM);
//      diff = time_end - time_start;
//      
//      expected_cycles_per_iteration =  W * H ;
//      expected_iterations = I_input/CPI *O_output/CPO;
//
//      //expected_time_one_iteration = expected_cycles_per_iteration / KERNEL_FREQUENCY_MHZ / 1000; //time in us
//      float etoi = (float)expected_cycles_per_iteration / (float)39.0; // time in us 
//      float et = (float)(etoi * (float)expected_iterations) ;  //time in us
//
//      expected_time = et;
//
//      // set time value from ns to us
//      time = (unsigned long long) (diff / ((unsigned long long)1000)); // time value provided by profiling tools is given in ns
//      time_per_iteration = time / expected_iterations;
//
//#else
//      struct timeval prof_t2;
//      gettimeofday(&prof_t2, NULL);
//      prof_time = ((prof_t2.tv_sec - prof_t1.tv_sec) * 1000000) + (prof_t2.tv_usec - prof_t1.tv_usec);
//      time = prof_time;
//      #ifdef OPENCL_TEST
//         // OpenCL kernel time
//         cl_ulong time_start, time_end;
//         kernel_events[0].getProfilingInfo(CL_PROFILING_COMMAND_START, &time_start);
//         kernel_events[0].getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);
//         double diff = time_end-time_start;
//         std::cout<< "TIME KERNEL = " << (diff/1000000)<<" ms \n"<<std::endl;
//      #endif
//      time_per_iteration = prof_time / i_iter / o_iter;
//      expected_time_one_iteration = W * H * KERNEL_FREQUENCY_MHZ;
//      expected_time = (expected_time_one_iteration * i_iter * o_iter) / 100000;
//
//#endif
 
      
      cl_ulong expected_cycles_per_iteration;
      cl_ulong expected_iterations;

     
      expected_cycles_per_iteration =  W * H ;
      expected_iterations = I_input/CPI *O_output/CPO;

      //expected_time_one_iteration = expected_cycles_per_iteration / KERNEL_FREQUENCY_MHZ / 1000; //time in us
      float etoi = (float)expected_cycles_per_iteration / (float)KERNEL_FREQUENCY_MHZ; // time in us 
      float et = (float)(etoi * (float)expected_iterations) ;  //time in us

      expected_time = et;

      // set time value from ns to us
      //time = (unsigned long long) (diff / ((unsigned long long)1000)); // time value provided by profiling tools is given in ns
      time = (unsigned long long)kernels_execution_time;
      time_per_iteration = time / expected_iterations;
      #ifdef DEBUG_VERBOSE
      printf("Board Kernel frequency = %f MHz\n", KERNEL_FREQUENCY_MHZ);
      printf("PROFILE EVENT - TIME KERNEL = %llu us  (%lf ms)   Expected cicles %lu  expected time %llu (%f) ns\n", 
          time,
          ((double)time/(double)1000.0), 
          (expected_cycles_per_iteration * expected_iterations),
          expected_time,
          et);
      #endif

      efficiency = ((float)expected_time / (float)time);
      print_timings(time, time_per_iteration, expected_time, efficiency);

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

      #ifdef DEBUG_CPU
      print_output();
      #endif

      //deallocate_buffers();

    }
  }
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int main(int argc, char **argv) {

  int retval = 0;
  int global_retval = 0;
  int cpu = 0;
  int enable = 0 ;
  int total_processed_lines= 0 ;
  int total_errors = 0;
  int total_success = 0;
  vector <string> log_err;

#ifdef EMULATION_CONFIG_ENABLED
  printf("\n");
  printf(     "---------------------------------------------------------------------\n");
  printf(KYEL "    EMULATION CONFIGURATION MODE, REDUCED WMAX HMAX STREAMS_DEPTH    \n" KNRM);
  printf(     "---------------------------------------------------------------------\n");
  printf("\n");
#endif

  printf ("JM10 - Size of data_type in this host is %lu bytes\n\n", sizeof(data_type));

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

    //----------------jm10 under devel
    printf("\n\n");
    printf(KRED "  forcing data_in deterministic values\n" KNRM);
    printf("\n\n");
//    deterministic_input_values = 1;
    //----------------jm10 under devel

    parse_arguments(argc, argv);

    #ifdef OPENCL_TEST

    enable = fn_init_fpga();
    if (!enable) {
      printf(KRED "Error initializing fpga device\n" KNRM);
    } 
    else {
      printf(KGRN "OpenCL device succesfully initialized for test\n" KRST);
    }
    #endif

    #ifdef DEBUG_LOG_INIT
    printf("open test input data configuration file\n");
    #endif
    if (open_test_file() == 1) {
      printf(KRED "Error opening input data file\n" KNRM);
      return 1;
    }

    #ifdef DEBUG_LOG_INIT
    printf("Process test intput data file \n");
    #endif
    int file_line = 0;
    while (!read_test_file(&enable, &cpu)) {
    //while (!read_test_file(&enable, &cpu) && (file_line < 1)) {
      printf("\n-------------------------\n");
      printf("Process test intput data file line #%2d\n", file_line + 1);

      // Launh kernel wiht configuration read from file (one line contains the configuration of a "computation")
      compute(&enable, &cpu, &retval);
      #ifdef DEBUG_VERBOSE
      printf("Test check results returned\n");
      #endif
      if(retval == 0){
        #ifdef DEBUG_VERBOSE
        printf(KGRN "  OK: RESULTS match for input file line #%d \n" KNRM, file_line + 1);
        #endif
      } else {
        std::ostringstream string_err;
        string_err << "input file line #" << file_line + 1 << endl; 
        log_err.push_back(string_err.str());
        #ifdef DEBUG_VERBOSE
        printf(KRED "  ERROR: RESULTS mismatch, retval= %d %s \n" KNRM, retval, (string_err.str()).c_str());
        #endif
      }

      if (enable) {
        total_processed_lines = total_processed_lines + 1;
        global_retval = global_retval + retval;
        if (retval == 0)
          total_success = total_success + 1;
        else
          total_errors = total_errors + 1;
      }
      
      file_line++;
    }

    close_test_file();
  }

  printf("Finished reading input data file\n");
 
  #ifdef OPENCL_TEST
  fn_release_fpga();
  #endif

  printf("\n\n");
#ifdef EMULATION_CONFIG_ENABLED
  printf(KYEL "EMULATION CONFIGURATION MODE, REDUCED WMAX HMAX STREAMS_DEPTH    \n" KNRM);
#endif
  printf(KCYN "End of test\n" KNRM);
  printf("Results summary\n");
  printf("  Total proccessed lines in input file: %d\n", total_processed_lines);
  printf("        matches: %d\n", total_success);
  printf("        errors:  %d\n", total_errors);
  if(global_retval == 0){
    printf(" \n");
    printf(KGRN "  Results are good\n" KNRM);
  } else {
    printf("\n");
    printf(KRED "  Output errors detected for\n" KNRM);
    for (size_t i = 0; i < log_err.size(); i++) {
      printf("      %s", log_err[i].c_str());
    }
  }
  printf(" \n");

 // Return 0 if outputs are correct
 return global_retval;
}

//*********************************************************************************************************************
// end of file test_conv2D.c
//*********************************************************************************************************************
