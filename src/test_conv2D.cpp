// --------------------------------------------------------------------------------------------------------------
// FPGA kernels for EDDL Library - European Distributed Deep Learning Library.
// Version: 0.6
// copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
// Date: November 2020
// Authors: GAP Research Group (UPV)
//     José Flich Cardo
//     Jorge García Martínez
//     Izan Catalán Gallarch
//     Carles Hernández Luz
//
// contact: jflich@disca.upv.es
// All rights reserved


//
// test_conv2D_8x8. Test for the conv2D kernel
//
// Constants:
//
//  - CPI
//  - CPO
//  - KW = 3
//  - KH = 3
//  - PW = 1
//  - PH = 1
//  - SW = 1
//  - SH = 1
//
//  Arguments:
//
//  - W
//  - H
//  - I
//  - O
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

#include "conv2D.h"

#include <cstdio>      /* printf, scanf, NULL */
#include <cstdlib>     /* malloc, free, rand */

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include <sys/time.h>

using std::vector;

// Global variables
int W;                           // Width of the data
int H;                           // Height of the data
int I;                           // Number of input channels
int O;                           // Number of output channels
int I_kernel;					 // Number of input channels for the kernel (filter) - padding
int O_kernel;  					 // Number of output channels for the kernel (filter) - padding
int rows;						 // number of rows to compute by the kernel
int enable_upper_padding;		 // enables the upper row of padding
int enable_lower_padding;		 // enables the lower row of padding
int enable_relu;				 // enables applying the relu activation functions
int i_iter;						 // number of input iterations
int o_iter;						 // number of output iterations
int global_offset;				 // global offset for the output data for the kernel
int GI;							 // number of groups for input channels
int GO;							 // number of groups for output channels

// buffers
data_type *data_in;               // Input data buffer (format I x W x H)
data_type *out;                   // Output data buffer (format O x W x H)
data_type *kernel;                // Conv kernel buffers (format GO x GI x CPO x CPI x KW x KH) - for DirectConv and WinogradConv
                                  // DWS conv kernel buffers (format I x KW x KH + I x O) [DW + PW]
data_type *bias;                  // Conv bias buffers (format O)
data_type *out_cpu;               // Output data buffer for cpu (format O x W x H)

// -------------------------------------------------------------------------------------------------
// Functions
//

// Allocate_buffers. Allocates in CPU memory all the needed buffers
void allocate_buffers() {
  posix_memalign((void **)&data_in, 4096, I * W * H * sizeof(data_type));
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  posix_memalign((void **)&kernel, 4096, I_kernel * O_kernel * KW * KH * sizeof(data_type));
#endif
#ifdef DWS_CONV
  posix_memalign((void **)&kernel, 4096, ((I_kernel * KW * KH) + (I_kernel * O_kernel)) * sizeof(data_type));
#endif
  posix_memalign((void **)&bias, 4096, O * sizeof(data_type));
  posix_memalign((void **)&out, 4096, O * W * H * sizeof(data_type));
  posix_memalign((void **)&out_cpu, 4096, O * W * H * sizeof(data_type));
}

// deallocate_buffers. Deallocates all CPU buffers
void deallocate_buffers() {
  free(data_in);
  free(kernel);
  free(bias);
  free(out);
  free(out_cpu);
}

// init_arguments. Initializes the arguments
void init_arguments() {
  W = 4;
  H = 4;
  I = 4;
  O = 4;
  I_kernel = 4;
  O_kernel = 4;
  i_iter = (I + CPI - 1) / CPI;
  o_iter = (O + CPO - 1) / CPO;
  enable_upper_padding = 1;
  enable_lower_padding = 1;
  rows = H;
  enable_relu = 0;
  global_offset = 0;
  GI = I_kernel / CPI;
  GO = O_kernel / CPO;
}

// cpu_conv2d. Performs the convolutions on the cpu
void cpu_conv2D() {

  int size_out = O * W * H;
  for (int i=0; i<size_out; i++) out_cpu[i] = 0.f;

  for (int c=0; c<I; c++) {
    for (int cout=0; cout<O; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          for (int kh=0; kh<KH; kh++) {
            for (int kw=0; kw<KW; kw++) {
              int data_h = (h-1)+kh;
              int data_w = (w-1)+kw;
              int padding = (data_h == -1) | (data_w == -1) | (data_w == W) | (data_h == H);
              // kernel position
              int gi = c / CPI;
              int ki = c % CPI;
              int go = cout / CPO;
              int ko = cout % CPO;
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
              // addr_k for direct convs or winograd convs
              int addr_k = (go * GI * CPO * CPI * KH * KW) +
                           (gi * CPO * CPI * KH * KW) +
                           (ko * CPI * KH * KW) +
                           (ki * KH * KW) +
                           (kh * KW) +
                           kw;
#endif
#ifdef DWS_CONV
              // addr_dw_k and addr_pw_k for dws convs
              int addr_dw_k = (c * KH * KW) + (kh * KW) + kw;
              int addr_pw_k = (I_kernel * KH * KW) + (c * O_kernel) + cout;
#endif
              // data_in pixel position
              int addr_p = (c * W * H) + (data_h * W) + data_w;
              // data_out pixel position
              int addr_o = (cout * W * H) + (h * W) + w;
              // operation
              data_type din = padding? data_type(0) : data_in[addr_p];
#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
              if (!padding) out_cpu[addr_o] += din * kernel[addr_k];
#endif
#ifdef DWS_CONV
              if (!padding) out_cpu[addr_o] += din * kernel[addr_dw_k] * kernel[addr_pw_k];
              if (!padding && (h==0) && (w==0)) printf("kh %d kw %d din %f dw %f pw %f accum %f\n", kh, kw, din, kernel[addr_dw_k], kernel[addr_pw_k], out_cpu[addr_o]);
#endif
            }
          }
        }
      }
    }
  }

  // let's add bias
  for (int cout=0; cout<O; cout++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
        // data_out pixel position
        int addr_o = (cout * W * H) + (h * W) + w;
        // bias operation
        out_cpu[addr_o] += bias[cout];
      }
    }
  }

  // apply relu
  if (enable_relu){
    for (int cout=0; cout<O; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          int addr_o = (h * W * O) + (w * O) + cout;
          if (out_cpu[addr_o] < 0.f) out_cpu[addr_o] = 0.f;
        }
      }
    }
  }
}

// check_result function. Checks output produced by the CPU and by the FPGA
int check_result() {

  for (int cout=0; cout < O; cout++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
        // data_out pixel position
        int addr_o = (cout * W * H) + (h * W) + w;
        if (fabs(float(out_cpu[addr_o]) - float(out[addr_o])) > 0.001) {
          return 1;
        }
      }
    }
  }
  return 0;
}

void print_bias() {
  printf("Bias: ");
  for (int o=0; o<O; o++) printf("%6.4f ", bias[o]);
  printf("\n");
}

#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
void print_kernel() {
  printf("Kernels: ");
  for (int c=0; c<I; c++) {
    for (int cout=0; cout<O; cout++) {
      printf("i=%d o=%d:\n", c, cout);
      for (int kh=0; kh<KH; kh++) {
        for (int kw=0; kw<KW; kw++) {
          // kernel position
          int gi = c / CPI;
          int ki = c % CPI;
          int go = cout / CPO;
          int ko = cout % CPO;
          int addr_k = (go * GI * CPO * CPI * KH * KW) +
                       (gi * CPO * CPI * KH * KW) +
                       (ko * CPI * KH * KW) +
                       (ki * KH * KW) +
                       (kh * KW) +
                       kw;
          printf(" %6.4f ", kernel[addr_k]);
        }
        printf("\n");
      }
    }
  }
}
#endif

#ifdef DWS_CONV
void print_kernel() {
  printf("Kernels (dw): ");
  for (int c=0; c<I; c++) {
    printf("i=%d:\n", c);
    for (int kh=0; kh<KH; kh++) {
      for (int kw=0; kw<KW; kw++) {
        // kernel position
        int addr_k = (c * KW * KH) + (kh * KW) + kw;
        printf(" %6.4f ", kernel[addr_k]);
      }
      printf("\n");
    }
  }
  printf("Kernels (pw): ");
  for (int c=0; c<I; c++) {
	for (int o=0; o<O; o++) {
	  int addr_k = (I_kernel * KW * KH) + (c * O) + o;
      printf("i=%d o=%d: %6.4f\n", c, o, kernel[addr_k]);
    }
  }
}
#endif


void print_input() {
  printf("Input:\n");
  int addr = 0;
  for (int i=0; i<I; i++) {
    printf("channel %d:\n", i);
	for (int h=0; h<H; h++) {
	  for (int w=0; w<W; w++) {
	    printf("%6.4f ", data_in[addr]);
        addr++;
	  }
	  printf("\n");
	}
  }
}


void print_output() {
	printf("Output:\n");
	for (int o=0; o<O; o++) {
		printf("channel %d:\n", o);
		for (int h=0; h<H; h++) {
			for (int w=0; w<W; w++) {
				int addr_o = (o * W * H) + (h * W) + w;
				printf("%6.4f (%6.4f) ", out[addr_o], out_cpu[addr_o]);
			}
			printf("\n");
		}
	}
}

void print_configuration() {
  printf("Test:\n");
  printf("  Input shape            : [%d ch x %d rows x %d cols]\n", I, H, W);
  printf("  Ouput shape            : [%d ch x %d rows x %d cols]\n", O, H, W);
  printf("  Kernel size            : %d x %d\n", KW, KH);
  printf("  Stride                 : 1 x 1\n");
  printf("  Padding                : 1 x 1\n");
  printf("  Apply RELU             : %s\n", enable_relu?"Yes":"No");
}

void init_data() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

  int addr = 0;
  for (int i=0; i<I; i++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
        data_in[addr] = i+1; //dist(gen);
        addr++;
      }
    }
  }

#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  int kernel_id = 1;
  for (int i=0; i<I_kernel; i++) {
    for (int o=0; o<O_kernel; o++) {
      for (int kh=0; kh<KH; kh++) {
        for (int kw=0; kw<KW; kw++) {
          int gi = i / CPI;
          int ki = i % CPI;
          int go = o / CPO;
          int ko = o % CPO;
          int addr_k = (go * GI * CPO * CPI * KH * KW) +
                       (gi * CPO * CPI * KH * KW) +
                       (ko * CPI * KH * KW) +
                       (ki * KH * KW) +
                       (kh * KW) +
                       kw;
          if ((i<I) && (o<O)) kernel[addr_k] = kernel_id; //  dist(gen);
          else kernel[addr_k] = 0;
	    }
	  }
      kernel_id++;
    }
  }
#endif

#ifdef DWS_CONV
  int kernel_id = 1;
  for (int i=0; i<I_kernel; i++) {
    for (int kh=0; kh<KH; kh++) {
      for (int kw=0; kw<KW; kw++) {
    	  int addr_k = (i * KW * KH) + (kh * KW) + kw;
    	  if (i < I) kernel[addr_k] = dist(gen); else kernel[addr_k] = 0;
      }
    }
    kernel_id++;
  }
  kernel_id = 1;
  for (int i=0; i<I_kernel; i++) {
	  for (int o=0; o<O_kernel; o++) {
		  int addr_k = (I_kernel * KW * KH) + (i * O_kernel) + o;
		  if ((i < I) && (o < O)) kernel[addr_k] = dist(gen); else kernel[addr_k] = 0;
	  }
	  kernel_id++;
  }
#endif

  for (int cout=0; cout<O; cout++) bias[cout] = cout; //dist(gen);
}

int main() {

 int retval = 0;

 init_arguments();
 print_configuration();
 allocate_buffers();
 init_data();

#ifdef DEBUG_CPU
 print_input();
 print_bias();
 print_kernel();
#endif

 k_conv2D((ap_uint<512> *)data_in, H, W, rows, I, O, i_iter, o_iter, enable_relu, kernel, (pixel_out_t *)bias, (ap_uint<512> *)out, global_offset, enable_upper_padding, enable_lower_padding);

 cpu_conv2D();
 retval = check_result();

#ifdef DEBUG_CPU
 print_output();
#endif

 deallocate_buffers();

 if(retval == 0){
   printf("    *** *** *** *** \n");
   printf("    Results are good \n");
   printf("    *** *** *** *** \n");
 } else {
   printf("    *** *** *** *** \n");
   printf("    Mismatch: retval=%d \n", retval);
   printf("    *** *** *** *** \n");
 }

 // Return 0 if outputs are correct
 return retval;
}
