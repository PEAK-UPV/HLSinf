/*
 * data-related test functions
 */

#include "test_conv2D.h"

void init_data() {
  std::random_device rd;
  std::mt19937 gen(rd());

  // random number generators
  #if defined(FP32_DATA_TYPE) || defined(APF8_DATA_TYPE) || defined(APF16_DATA_TYPE)
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
  #endif
  #ifdef API8_DATA_TYPE
  std::uniform_int_distribution<int> dist(-10, 10);
  #endif
  #ifdef API16_DATA_TYPE
  std::uniform_int_distribution<int> dist(-10, 10);
  #endif

  // input data
  int addr;
  for (int i=0; i<I_input; i++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
    	addr = input_data_address(i, h, w);
    	if (i<I) {
          data_in[addr] = deterministic_input_values?i+1:dist(gen);
    	} else {
    	  data_in[addr] = 0;
    	}
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
          if ((i<I) && (o<O)) kernel[addr_k] = deterministic_input_values?(i % 20)-10:dist(gen);
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
    	  if (i < I) dw_kernel[addr_k] = deterministic_input_values?(kernel_id%20)-10:dist(gen); else dw_kernel[addr_k] = 0;
      }
    }
    kernel_id++;
  }
  kernel_id = 1;
  for (int i=0; i<I_kernel; i++) {
	  for (int o=0; o<O_kernel; o++) {
		  int gi = i / CPI;
		  int ki = i % CPI;
		  int go = o / CPO;
		  int ko = o % CPO;
		  int addr_k = (go * GI * CPO * CPI) + (gi * CPO * CPI) + (ko * CPI) + ki;
		  if ((i < I) && (o < O)) pw_kernel[addr_k] = deterministic_input_values?(kernel_id%20)-10:dist(gen); else pw_kernel[addr_k] = 0;
	  }
	  kernel_id++;
  }
#endif

  for (int cout=0; cout<O; cout++) bias[cout] = deterministic_input_values?(cout%20)-10:dist(gen);
}
