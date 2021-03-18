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
int W;                           // Width of the data
int H;                           // Height of the data
int I;                           // Number of input channels
int O;                           // Number of output channels
int HO;							 // Output height
int WO;							 // Output width
int I_kernel;					 // Number of input channels for the kernel (filter) - padding
int O_kernel;  					 // Number of output channels for the kernel (filter) - padding
int rows;						 // number of rows to compute by the kernel
int enable_upper_padding;		 // enables the upper row of padding
int enable_lower_padding;		 // enables the lower row of padding
int enable_relu;				 // enables applying the relu activation functions
int enable_shift;				 // enables applying shift to the output
int dir_shift;      			 // shift direction (left or right)
int pos_shift;					 // positions to shift
int enable_clipping;			 // enables applying clipping to the output
int enable_maxpooling;			 // enables the maxpooling layer
int enable_avgpooling;			 // enables the avgpooling layer
int min_clip;					 // minimum clip value
int max_clip;					 // maximum clip value
int i_iter;						 // number of input iterations
int o_iter;						 // number of output iterations
int global_offset;				 // global offset for the output data for the kernel
int GI;							 // number of groups for input channels
int GO;							 // number of groups for output channels
char *input_data_file;           // input data file with configurations to test

// buffers
data_type *data_in;               // Input data buffer (format I x W x H)
data_type *out;                   // Output data buffer (format O x W x H)
data_type *kernel;                // Conv kernel buffers (format GO x GI x CPO x CPI x KW x KH) - for DirectConv and WinogradConv
                                  // DWS conv kernel buffers (format I x KW x KH + I x O) [DW + PW]
data_type *bias;                  // Conv bias buffers (format O)
data_type *out_conv_cpu;          // Output data buffer for cpu (format O x W x H)
data_type *out_relu_cpu;          // Output data buffer for cpu (format O x W x H)
data_type *out_pool_cpu;		  // Output data fuffer for pool for cpu (format O x W/2 x H/2)

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
cl::Buffer *buffer_o[MAX_CONVS];              // output buffers
cl::Buffer *buffer_k[MAX_CONVS];              // Conv kernel buffers
cl::Buffer *buffer_bias[MAX_CONVS];           // Conv bias buffers
// DDR assignment
cl_mem_ext_ptr_t data_in_ddr;                 // input data buffer
cl_mem_ext_ptr_t out_ddr[MAX_CONVS];          // output data buffers
cl_mem_ext_ptr_t kernel_ddr[MAX_CONVS];       // Conv kernel buffers
cl_mem_ext_ptr_t bias_ddr[MAX_CONVS];         // Conv bias buffers
#endif

int main(int argc, char **argv) {

  int retval = 0;
  int global_retval = 0;

  input_data_file = argv[1];

  #ifdef OPENCL_TEST
  binaryFile = argv[2];
  fn_init_fpga();
  #endif

  if (open_test_file() == 1) return 1;

  int enable;
  while (!read_test_file(&enable)) {

    if (enable) {
	   printf("%3d x %3d x %3d x %3d K=%1dx%1d S=%1dx%1d P=%1dx%1d RELU=%s MAXP=%s AVGP=%s CLIP=%s (%d:%d) SHIFT=%s (%s,%d) ===> ",
	   		 	 	 H, W, I, O, KH, KW, 1, 1, 1, 1, enable_relu?"Yes":"No ", enable_maxpooling?"Yes":"No ", enable_avgpooling?"Yes":"No ", enable_clipping?"Yes":"No ", min_clip, max_clip, enable_shift?"Yes":"No ",
	   		 	 			 dir_shift==LEFT_DIRECTION?"LEFT":"RIGHT", pos_shift);

       #ifndef USE_POOLING
	   if (enable_maxpooling | enable_avgpooling) {
	     printf("Pooling not supported (disabled)");
	     enable_maxpooling = 0; enable_avgpooling = 0;
	   }
	   #endif

	   if (enable_maxpooling && enable_avgpooling) {
	   	 printf("MaxPooling and AvgPooling cannot be active at the same time (skipped)\n");
	   	 enable = 0;
	   }

       #ifndef API8_DATA_TYPE
	   if (enable_clipping || enable_shift) {
		 printf("Clipping/shift only for API8 (disabled)");
		 enable_clipping = 0; enable_shift = 0;
	   }
       #endif

	   // Check, output must be at least as large as one write block

	   if (HO * WO * O < WRITE_BLOCK_SIZE) {
		 printf("Output too small (skipped)\n");
		 enable = 0;
	   }

	   if (enable) {
	     allocate_buffers();
	     init_data();

         #ifdef OPENCL_TEST
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

         #ifdef OPENCL_TEST
	     copy_from_fpga();
         #endif

	     cpu_conv2D();

	     int num_differences;
	     data_type max_difference;
	     retval = check_result(&max_difference, &num_differences);
         if (retval) printf(" Time %8lld usec - FAIL (max diff %20.18f, num differences %d)\n", prof_time, float(max_difference), num_differences);
	     else        printf(" Time %8lld usec - SUCCESS\n", prof_time);

         #ifdef DEBUG_CPU
	     print_output();
	     #endif

	     deallocate_buffers();

	     global_retval = global_retval || retval;
	   }
	 }
   }

   close_test_file();


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
