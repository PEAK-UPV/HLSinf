/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

/*
 * Print-related test functions
 *
 */

#include "test_conv2D.h"

void print_input_buffer_stats(din_t *p, int size) {
  float min =  999999;
  float max = -999999;
  double sum = 0.f;

  for (int x = 0; x < size; x++) {
    float v = float(p[x]);
    if (min > v) min = v;
    if (max < v) max = v;
    sum += v;
  }
  float avg = sum / (float)size;
  printf("Min %8.4f Max %8.4f Avg %8.4f\n", min, max, avg);
}

void print_weight_buffer_stats(w_t *p, int size) {
   float min =  999999;
  float max = -999999;
  double sum = 0.f;

  for (int x = 0; x < size; x++) {
    float v = float(p[x]);
    if (min > v) min = v;
    if (max < v) max = v;
    sum += v;
  }
  float avg = sum / (float)size;
  printf("Min %8.4f Max %8.4f Avg %8.4f\n", min, max, avg);
}

void print_bias_buffer_stats(b_t *p, int size) {
  float min =  999999;
  float max = -999999;
  double sum = 0.f;

  for (int x = 0; x < size; x++) {
    float v = float(p[x]);
    if (min > v) min = v;
    if (max < v) max = v;
    sum += v;
  }
  float avg = sum / (float)size;
  printf("Min %8.4f Max %8.4f Avg %8.4f\n", min, max, avg);
}

void print_data_in_buffer_stats(dout_t *p, int size) {
  float min =  999999;
  float max = -999999;
  double sum = 0.f;

  for (int x = 0; x < size; x++) {
    float v = float(p[x]);
    if (min > v) min = v;
    if (max < v) max = v;
    sum += v;
  }
  float avg = sum / (float)size;
  printf("Min %8.4f Max %8.4f Avg %8.4f\n", min, max, avg);
}

void print_batchnorm_buffer_stats(dout_t *p, int size) {
   float min =  999999;
  float max = -999999;
  double sum = 0.f;

  for (int x = 0; x < size; x++) {
    float v = float(p[x]);
    if (min > v) min = v;
    if (max < v) max = v;
    sum += v;
  }
  float avg = sum / (float)size;
  printf("Min %8.4f Max %8.4f Avg %8.4f\n", min, max, avg);
}

void print_output_buffer_stats(dout_t *p, int size) {
  float min =  999999;
  float max = -999999;
  double sum = 0.f;

  for (int x = 0; x < size; x++) {
    float v = float(p[x]);
    if (min > v) min = v;
    if (max < v) max = v;
    sum += v;
  }
  float avg = sum / (float)size;
  printf("Min %8.4f Max %8.4f Avg %8.4f\n", min, max, avg);
}

void print_bias() {
  printf("Bias: ");
  for (int o=0; o<O_output; o++) printf("%6.4f ", float(bias[o]));
  printf("\n");
}

#ifdef USE_BATCH_NORM
void print_batch_norm() {
	printf("Batch Normalization Values: ");
	for (int o=0; o<O_output*4; o++) printf("%6.4f ", float(batch_norm_values[o]));
	printf("\n");
}
#endif

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

void print_input_add() {

  int Hmax = HO_final;
  int Wmax = WO_final;
  if (H > 5) Hmax = 5;
  if (W > 5) Wmax = 5;

  printf("Input ADD:\n");
  for (int i=0; i<I_input; i++) {
    printf("channel %d:\n", i);
	for (int h=0; h<Hmax; h++) {
	  for (int w=0; w<Wmax; w++) {
		int addr = input_data_address(i, h, w);
	    printf("%4.2f ", float(data_in_add[addr]));
        addr++;
	  }
	  if (W != Wmax) printf(" ...");
	  printf("\n");
	}
	if (H != Hmax) printf("...\n");
  }
}


void print_output(int only_cpu) {
  printf("Output:\n");
  for (int o=0; o<O_output; o++) {
    printf("channel %d:\n", o);
    int rows = enable_upsize ? HO_final * 2 : HO_final;
    int cols = enable_upsize ? WO_final * 2 : WO_final;
    if (rows > 5) rows = 5;
    if (cols > 5) cols = 5;
    for (int h=0; h<rows; h++) {
      for (int w=0; w<cols; w++) {
        int addr_o = output_data_address(o, h, w, rows, cols);
        if (only_cpu) printf("%6.4f ", float(cpu_out[addr_o])); else printf("%6.4f (%6.4f) ", float(out[addr_o]), float(cpu_out[addr_o]));
      }
      printf("\n");
    }
  }
}

void print_configuration() {
  printf("\n");
  printf("====================================================================================================================\n");
  printf("| In: %3d x %3d x %3d | Out: %3d x %3d x %3d | Kernel: %3d x %3d  | Pad (TBLR): %1d x %1d x %1d x %1d | Stride: %3d x %3d  |\n", H, W, I, HO, WO, O, KH, KW, PT, PB, PL, PR, SH, SW);
  printf("|------------------------------------------------------------------------------------------------------------------|\n");
  printf("| ReLU: %s | STM: %s | MaxP: %s | AvgP: %s | BN: %s |Add: %s | Clip: %s (%2d:%2d) | Shift: %s (%s,%2d) | Upsize %s|\n", enable_relu?"Yes":"No ", enable_stm?"Yes":"No ",
  		    enable_maxpooling?"Y":"N", enable_avgpooling?"Y":"N", enable_batch_norm?"Y":"N", enable_add?"Y":"N", enable_clipping?"Y":"N", min_clip, max_clip, enable_shift?"Y":"N", dir_shift==LEFT_DIRECTION?"LEFT ":"RIGHT", pos_shift, enable_upsize?"Y":"N");
  printf("====================================================================================================================\n");
}

void print_timings(unsigned long long time, unsigned long long time_per_iteration, unsigned long long expected_time, float efficiency) {

  printf("| Time %8lld usec  |  Time per iteration %8lld usec  |  Expected time %8lld usec  |   Efficiency %6.4f   |\n", time, time_per_iteration, expected_time, efficiency);
  printf("====================================================================================================================\n");
}

void print_check(int result, float max_difference, int num_differences) {
    if (result) printf("| FAIL                    |            max diff %20.18f           |        num differences %d          |\n", max_difference, num_differences);
    else        printf("| SUCCESS                                                                                                          |\n");
    printf("====================================================================================================================\n");
}

void print_message(const char *str) {
    printf("| %-112s |\n", str);
    printf("====================================================================================================================\n");
}
