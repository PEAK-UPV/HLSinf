/*
 * Print-related test functions
 *
 */

#include "test_conv2D.h"

void print_bias() {
  printf("Bias: ");
  for (int o=0; o<O_output; o++) printf("%6.4f ", float(bias[o]));
  printf("\n");
}

#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
void print_kernel() {
  printf("Kernels: ");
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
  printf("Kernels (dw): ");
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
  if (H > 8) Hmax = 8;
  if (W > 8) Wmax = 8;

  printf("Input:\n");
  for (int i=0; i<I_input; i++) {
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
  if ((enable_maxpooling) || (enable_avgpooling)) {

	printf("Output:\n");
	for (int o=0; o<O_output; o++) {
		printf("channel %d:\n", o);
		for (int h=0; h<HO/2; h++) {
			for (int w=0; w<WO/2; w++) {
				int addr_o = output_data_address_div(o, h, w);
				printf("%6.4f (%6.4f) ", float(out[addr_o]), float(out_pool_cpu[addr_o]));
			}
			printf("\n");
		}
	}

  } else {

	printf("Output:\n");
	for (int o=0; o<O_output; o++) {
		printf("channel %d:\n", o);
		for (int h=0; h<HO; h++) {
			for (int w=0; w<WO; w++) {
				int addr_o = output_data_address(o, h, w);
				if (enable_relu) printf("%6.4f (%6.4f) ", float(out[addr_o]), float(out_relu_cpu[addr_o]));
				else printf("%6.4f (%6.4f) ", float(out[addr_o]), float(out_conv_cpu[addr_o]));
			}
			printf("\n");
		}
	}

  }

}

void print_configuration() {
  printf("\n");
  printf("====================================================================================================================\n");
  printf("| In: %3d x %3d x %3d | Out: %3d x %3d x %3d | Kernel: %3d x %3d  | Pad (TBLR): %1d x %1d x %1d x %1d | Stride: %3d x %3d  |\n", H, W, I, HO, WO, O, KH, KW, PT, PB, PL, PR, SH, SW);
  printf("|------------------------------------------------------------------------------------------------------------------|\n");
  printf("| ReLU: %s   |   MaxPooling: %s   |   AvgPooling: %s    |  Clipping: %s (%2d:%2d)    |  Shift: %s (%s,%2d)    |\n", enable_relu?"Yes":"No ",
		    enable_maxpooling?"Yes":"No ", enable_avgpooling?"Yes":"No ", enable_clipping?"Yes":"No ", min_clip, max_clip, enable_shift?"Yes":"No ", dir_shift==LEFT_DIRECTION?"LEFT ":"RIGHT", pos_shift);
  printf("|------------------------------------------------------------------------------------------------------------------|\n");
  printf("| MUL_ADD: %6.4f %6.4f | ADD_MUL: %6.4f %6.4f                                                                    |\n", mul_value, add_value, mul_value2, add_value2);
  printf("| MULTI_THRESHOLD: count %3d min %6.4f max %6.4f stride %6.4f scale %6.4f bias %6.4f|\n", multi_threshold_count, multi_threshold_min, multi_threshold_max, multi_threshold_stride, multi_threshold_scale, multi_threshold_bias);

  printf("====================================================================================================================\n");
}

void print_timings(unsigned long long time, unsigned long long time_per_iteration, unsigned long long expected_time, float efficiency) {

  printf("| Time %8lld usec  |  Time per iteration %8lld usec  |  Expected time %8lld usec  |   Efficiency %6.4f   |\n", time, time_per_iteration, expected_time, efficiency);
  printf("====================================================================================================================\n");
}

void print_check(int result, float max_difference, int num_differences) {
    if (result) printf("| FAIL                    |            max diff %20.18f           |        num differences %d          |\n", max_difference, num_differences);
    else        printf("| SUCCESS (< %12.8f)                                                                                         |\n", EPSILON_VALUE);
    printf("====================================================================================================================\n");
}

void print_message(const char *str) {
    printf("| %-112s |\n", str);
    printf("====================================================================================================================\n");
}
