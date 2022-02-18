/*
 * Print-related test functions
 *
 */

#include "test_conv2D.h"

void print_bias() {
  printf("Bias: \n");
  for (int o=0; o<O_output; o++) printf("%6.4f ", float(bias[o]));
  printf("\n");
}

#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
void print_kernel() {
  printf("Kernels: \n");
  for (int c=0; c<I_input; c++) {
    for (int cout=0; cout<O_output; cout++) {
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
          printf(" %6.4f ", float(kernel[addr_k]));
        }
        printf("\n");
      }
    }
  }
}
#endif

#ifdef DWS_CONV
void print_kernel() {
  printf("Kernels (dw): \n");
  for (int c=0; c<I_input; c++) {
    printf("i=%d: ", c);
    for (int kh=0; kh<KH; kh++) {
      for (int kw=0; kw<KW; kw++) {
        // kernel position
        int addr_k = (c * KW * KH) + (kh * KW) + kw;
        printf(" %6.4f ", float(dw_kernel[addr_k]));
      }
    }
    printf("\n");
  }
  printf("Kernels (pw) (i/o table):\n");
  for (int c=0; c<I_input; c++) {
	for (int o=0; o<O_output; o++) {
	  int gi = c / CPI;
	  int ki = c % CPI;
	  int go = o / CPO;
	  int ko = o % CPO;
	  int addr_k = (go * GI * CPO * CPI) + (gi * CPO * CPI) + (ko * CPI) + ki;
      printf(" %6.4f", float(pw_kernel[addr_k]));
    }
    printf("\n");
  }
}
#endif


void print_input() {
  int Hmax = H;
  int Wmax = W;
  if (H > 5) Hmax = 5;
  if (W > 5) Wmax = 5;

  printf("Input:\n");
  //for (int i=0; i<I_input; i++) {
  for (int i=0; i<I; i++) {
    printf("channel %d:\n", i);
	for (int h=0; h<Hmax; h++) {
	  for (int w=0; w<Wmax; w++) {
		int addr = input_data_address(i, h, w);
	    printf("%4.2f ", float(data_in[addr]));
        addr++;
	  }
	  if (W != Wmax) printf(" ...");
	  printf("\n");
	}
	if (H != Hmax) printf("...\n");
  }
}


void print_output() {
  float epsilon = EPSILON_VALUE;

  if ((enable_maxpooling) || (enable_avgpooling)) {
    printf("Output:\n");
    //for (int o=0; o<O_output; o++) {
    for (int o=0; o<O; o++) {
      printf("channel %d:\n", o);
      for (int h=0; h<H/2; h++) {
        for (int w=0; w<W/2; w++) {
          int addr_o = output_data_address_div(o, h, w);
          data_type diff = float(out[addr_o]) - float(out_pool_cpu[addr_o]);
          if (float(diff) > float(epsilon)) {
            printf( KRED );
          }
          printf("%6.4f (%6.4f) ", float(out[addr_o]), float(out_pool_cpu[addr_o]));
          printf( KNRM );
        }
        printf("\n");
      }
    }

  } else {
    printf("Output:\n");
    //for (int o=0; o<O_output; o++) {
    for (int o=0; o<O; o++) {
      printf("channel %d:\n", o);
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          int addr_o = output_data_address(o, h, w);
          if (enable_relu) {
            data_type diff = data_type(fabs(float(out[addr_o]) - float(out_relu_cpu[addr_o])));
            if (float(diff) > float(epsilon)) {
              printf( KRED );
            }
            printf("%6.4f (%6.4f) ", float(out[addr_o]), float(out_relu_cpu[addr_o]));
            printf( KNRM );
          } else { 
            data_type diff = data_type(fabs(float(out[addr_o]) - float(out_conv_cpu[addr_o])));
            if  (float(diff) > float(epsilon)) {
              printf( KRED );
            }
            printf("%6.4f (%6.4f) ", float(out[addr_o]), float(out_conv_cpu[addr_o]));
            printf( KNRM );
          }
        }
        printf("\n");
      }
    }

  }

}

void print_configuration() {
  printf("\n");
  printf("====================================================================================================================\n");
  printf("| Input: %3d x %3d x %3d x %3d      |  Kernel: %3d x %3d   |    Padding: %3d x %3d     |     Stride: %3d x %3d     |\n", H, W, I, O, KH, KW, 1, 1, 1, 1);
  printf("|------------------------------------------------------------------------------------------------------------------|\n");
  printf("| CPI: %2d     |  CPO: %2d     | log2_CPO: %2d     |  WMAX: %2d     | HMAX: %2d                                      |\n", CPI, CPO, LOG2_CPO, WMAX, HMAX);
  printf("|------------------------------------------------------------------------------------------------------------------|\n");
  printf("| ReLU: %s   |   MaxPooling: %s   |   AvgPooling: %s    |  Clipping: %s (%2d:%2d)    |  Shift: %s (%s,%2d)    |\n", enable_relu?"Yes":"No ",
		    enable_maxpooling?"Yes":"No ", enable_avgpooling?"Yes":"No ", enable_clipping?"Yes":"No ", min_clip, max_clip, enable_shift?"Yes":"No ", dir_shift==LEFT_DIRECTION?"LEFT ":"RIGHT", pos_shift);
  printf("====================================================================================================================\n");
}

void print_timings(unsigned long long time, unsigned long long time_per_iteration, unsigned long long expected_time, float efficiency) {

  printf("| Time %8lld usec  |  Time per iteration %8lld usec  |  Expected time %8lld usec  |   Efficiency %6.4f   |\n", time, time_per_iteration, expected_time, efficiency);
  printf("====================================================================================================================\n");
}

void print_check(int result, float max_difference, int num_differences) {
    if (result) {
      float epsilon = EPSILON_VALUE;
      int hhh, www;
      if ((enable_maxpooling) || (enable_avgpooling)) {
        hhh = H/2;
        www = W/2;
      } else{
        hhh = H;
        www = W;
      }
      int total_calculations = hhh * www * O_output;
      float factor = (float)num_differences / (float)total_calculations * 100;
      float factor_epsilon = max_difference / epsilon;
      printf("| " KRED "FAIL" KNRM "                |    max diff %20.18f   | num differences %d of %d    %8.6f%% samples  --  %6.4f x epsilon    |\n", max_difference, num_differences, total_calculations, factor, factor_epsilon);
      printf("=======================================================================================================================================================\n");
    }else{
      printf("| " KGRN "SUCCESS" KNRM "                                                                                                          |\n");
      printf("====================================================================================================================\n");
    }
}

void print_message(const char *str) {
    printf("| %-112s |\n", str);
    printf("====================================================================================================================\n");
}
