/*
 * Print-related test functions
 *
 */

#include "test_conv2D.h"

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
  for (int c=0; c<I; c++) {
    printf("i=%d:\n", c);
    for (int kh=0; kh<KH; kh++) {
      for (int kw=0; kw<KW; kw++) {
        // kernel position
        int addr_k = (c * KW * KH) + (kh * KW) + kw;
        printf(" %6.4f ", float(kernel[addr_k]));
      }
      printf("\n");
    }
  }
  printf("Kernels (pw): ");
  for (int c=0; c<I; c++) {
	for (int o=0; o<O; o++) {
	  int addr_k = (I_kernel * KW * KH) + (c * O) + o;
      printf("i=%d o=%d: %6.4f\n", c, o, float(kernel[addr_k]));
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
	    printf("%6.4f ", float(data_in[addr]));
        addr++;
	  }
	  printf("\n");
	}
  }
}


void print_output() {
  if ((enable_maxpooling) || (enable_avgpooling)) {

	printf("Output:\n");
	for (int o=0; o<O; o++) {
		printf("channel %d:\n", o);
		for (int h=0; h<H/2; h++) {
			for (int w=0; w<W/2; w++) {
				int addr_o = (o * (W/2) * (H/2)) + (h * (W/2)) + w;
				printf("%6.4f (%6.4f) ", float(out[addr_o]), float(out_pool_cpu[addr_o]));
			}
			printf("\n");
		}
	}

  } else {

	printf("Output:\n");
	for (int o=0; o<O; o++) {
		printf("channel %d:\n", o);
		for (int h=0; h<H; h++) {
			for (int w=0; w<W; w++) {
				int addr_o = (o * W * H) + (h * W) + w;
				if (enable_relu) printf("%6.4f (%6.4f) ", float(out[addr_o]), float(out_relu_cpu[addr_o]));
				else printf("%6.4f (%6.4f) ", float(out[addr_o]), float(out_conv_cpu[addr_o]));
			}
			printf("\n");
		}
	}

  }

}
