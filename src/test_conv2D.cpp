/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

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
int PT = PT_SIM;                 // Top padding
int PB = PB_SIM;                 // Bottom padding
int PL = PL_SIM;                 // Left padding
int PR = PR_SIM;                 // Right padding
int SH = SH_SIM;                 // Vertical stride
int SW = SW_SIM;                 // Horizontal stride
int F;                           // Number of frames of the data
int W = W_SIM;                   // Width of the data
int H = H_SIM;                   // Height of the data
int I = I_SIM;                   // Number of input channels
int O = O_SIM;                   // Number of output channels
int HO = H_SIM;       			 // Output height
int WO = W_SIM;    				 // Output width
int HO_final = H_SIM;            // HO at the output of the kernel
int WO_final = W_SIM;            // WO at the output of the kernel
int I_kernel = I_SIM;  			 // Number of input channels for the kernel (filter) - padding
int O_kernel = O_SIM;			 // Number of output channels for the kernel (filter) - padding
int I_input = I_SIM;             // Number of input channels for the input data - padding (needed in GHWC data format)
int O_output = O_SIM;            // Number of output channels for the output data - padding (needed in GHWC data format)
int rows = H_SIM;				 // number of rows to compute by the kernel
int enable_relu = 1;			 // enables applying the relu activation functions
float relu_factor = 0;
int enable_shift = 0;			 // enables applying shift to the output
int enable_stm = 1;			 	 // enables applying the STM functions
int enable_batch_norm = 0;		 // enables applying batch normalization
int enable_add = 0; 			 // enables add module
int enable_upsize = 0;           // enables upsize (resize)
int enable_fault_tolerance = 1;  // enables fault tolerance mode
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
int flag_error = 0;				 // Integer to knowing if there is any fault in multiplication opetations

// buffers
din_t *data_in;               // Input data buffer (format I x W x H)
din_t *data_in_add;           // Input data buffer for add module(format I x W x H)
dout_t *out;                   // Output data buffer (format O x W x H)
w_t *kernel;                // Conv kernel buffers (format GO x GI x CPO x CPI x KH x KW) - for DirectConv and WinogradConv
b_t *bias;                  // Conv bias buffers (format O)
bn_t *batch_norm_values;	  // Batch normalization values
conv_t *out_conv_cpu;          // Output data buffer for cpu (format O x W x H)
relu_t *out_relu_cpu;          // Output data buffer for cpu (format O x W x H)
stm_t *out_stm_cpu; 		  // Output data buffer for STM for cpu (format O x W x H)
pool_t *out_pool_cpu;		  // Output data fuffer for pool for cpu (format O x W/2 x H/2)
bn_t *out_batch_norm_cpu;	  // Output data buffer for cpu (format O x W x H)
add_t *out_add_cpu;          // Output data buffer for ADD for cpu (format O x W x H)
dout_t *cpu_out;               // final output
FILE *fp;

#ifdef OPENCL_TEST
// OpenCL variables
cl::Context context;                          // Context
cl::CommandQueue q;                           // Command queue
cl::Program program;                          // Program
std::string binaryFile;                       // Binary file
cl::Kernel kernel_conv2d[MAX_KERNELS];        // FPGA kernels
vector<cl::Event> kernel_events(MAX_KERNELS); // Kernel events (completion)
vector<cl::Event> read_events(1);             // Read events
vector<cl::Event> write_events(3);            // Write events
cl::Buffer *buffer_i;                         // input buffer
cl::Buffer *buffer_i_add;                     // input buffer add module
cl::Buffer *buffer_o[MAX_CONVS];              // output buffers
cl::Buffer *buffer_batch_norm_val[MAX_CONVS]; // Batch norm values buffers
cl::Buffer *buffer_k[MAX_CONVS];              // Conv kernel buffers
cl::Buffer *buffer_bias[MAX_CONVS];           // Conv bias buffers
cl::Buffer *buffer_k_dw[MAX_CONVS];           // Conv kernel buffers (deepwise)
cl::Buffer *buffer_k_pw[MAX_CONVS];           // Conv kernel buffers (pointwise)
cl::Buffer *intToDevice;					  // Error Flag
// DDR assignment
cl_mem_ext_ptr_t data_in_ddr;                 // input data buffer
cl_mem_ext_ptr_t data_in_add_ddr;             // input data add buffer
cl_mem_ext_ptr_t batch_norm_val_ddr[MAX_CONVS];          // Batch norm values buffers
cl_mem_ext_ptr_t out_ddr[MAX_CONVS];          // output data buffers
cl_mem_ext_ptr_t kernel_ddr[MAX_CONVS];       // Conv kernel buffers
cl_mem_ext_ptr_t kernel_pw_ddr[MAX_CONVS];    // DeepWise conv kernel buffers
cl_mem_ext_ptr_t kernel_dw_ddr[MAX_CONVS];    // PointWise conv kernel buffers
cl_mem_ext_ptr_t bias_ddr[MAX_CONVS];         // Conv bias buffers
#endif

void compute(int *enable, int *from_file, int *cpu, int *retval) {

    if (*enable) {
       print_configuration();

       #ifndef USE_POOLING
	   if (enable_maxpooling | enable_avgpooling) {
	     print_message("Pooling not supported (disabled)");
	     enable_maxpooling = 0; enable_avgpooling = 0;
	   }
	   #endif

	   #ifndef USE_STM
	   if (enable_stm) {
	     print_message("STM not supported (disabled)");
	     enable_stm = 0;
	   }
	   #endif

	   #ifndef USE_BATCH_NORM
	   if (enable_batch_norm) {
	     print_message("Batch Norm. not supported (disabled)");
	     enable_batch_norm = 0;
	   }
	   #endif

	   if (enable_maxpooling && enable_avgpooling) {
	   	 print_message("MaxPooling and AvgPooling cannot be active at the same time (skipped)");
	   	 *enable = 0;
	   }

	   if ((enable_maxpooling || enable_avgpooling) && ((HO % 2) || (WO % 2))) {
		 print_message("Pooling not allowed with outut convolution with no even rows and columns (skipped)");
		 *enable = 0;
	   }

       #if defined(FLOAT_DATA_TYPE)
	   if (enable_shift) {
		 print_message("shift only for integers (disabled)");
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

	   if (WO_final > WMAX) {
	     print_message("Width of output data is too large, consider increasing WMAX");
	     *enable = 0;
	   }

	   if (*enable) {
	     allocate_buffers();
	     init_data(*from_file);

         #ifdef OPENCL_TEST
	     copy_to_fpga();
         #endif

	     #ifdef DEBUG_CPU
	     print_input();
    	 if (enable_add) print_input_add();
	     print_bias();
	     print_kernel();
         #ifdef USE_BATCH_NORM
	     print_batch_norm();
         #endif
	     if (*from_file) print_output(1);
	     #endif

	     // timing stats
	     unsigned long long prof_time;
	     struct timeval prof_t1;
	     gettimeofday(&prof_t1, NULL);

	     compute();

		 printf("compute terminated\n");
		 printf("fault tolerance mode: %d -> faults processing the data: %d\n", enable_fault_tolerance, flag_error);

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
         kernel_events[0].getProfilingInfo(CL_PROFILING_COMMAND_START, &time_start);
         kernel_events[0].getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);
         double diff = time_end-time_start;
         std::cout<< "TIME KERNEL = " << (diff/1000000)<<" ms \n"<<std::endl;
         #endif

	     if (*cpu) cpu_conv2D();

	     if (*cpu || *from_file) {
           #ifdef OPENCL_TEST
           copy_from_fpga();
           #endif
           int num_differences;
	       dout_t max_difference;
	       *retval = check_result(&max_difference, &num_differences);

	       print_check(*retval, float(max_difference), num_differences);

	     } else {
	    	printf("\n");
	     }
	     

         #ifdef DEBUG_CPU
	     print_output(0);
	     #endif

	     deallocate_buffers();

	   }
	 }
}

int main(int argc, char **argv) {

  int retval = 0;
  int global_retval = 0;
  int cpu;
  int from_file;
  int enable;


  if (argc == 1) {
	printf("Co-simulation test...\n");
	enable = 1;
	from_file = 0;
	cpu = 1;
	deterministic_input_values = 1;
	for (int i=0; i<INSTANCES_SIM; i++) compute(&enable, &from_file, &cpu, &retval);
    global_retval = retval;
  } else {
	printf("File-based test...\n");
	deterministic_input_values = 0;
    parse_arguments(argc, argv);

    #ifdef OPENCL_TEST
    fn_init_fpga();
    #endif

    if (open_test_file() == 1) return 1;

    while (!read_test_file(&enable, &from_file, &cpu)) {

	  compute(&enable, &from_file, &cpu, &retval);
      if (enable) global_retval = global_retval || retval;
    }

    close_test_file();
  }

 if(global_retval == 0){
   printf("    *** *** *** *** \n");
   printf("    Results are good \n");
   printf("    *** *** *** *** \n");
 } else {
   printf("    *** *** *** *** \n");
   printf("    Mismatch: retval=%d \n", retval);
   printf("    *** *** *** *** \n");
 }

 // Return 0 if outputs are correct
 return global_retval;
}
