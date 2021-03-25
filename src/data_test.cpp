/*
 * data-related test functions
 */

#include "test_conv2D.h"

void init_data() {
  std::random_device rd;
  std::mt19937 gen(rd());

#if defined(FP32_DATA_TYPE) || defined(APF8_DATA_TYPE)
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
#endif
#ifdef API8_DATA_TYPE
  std::uniform_int_distribution<int> dist(-127.0, 127.0);
#endif

  int addr = 0;
  for (int i=0; i<I; i++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
        data_in[addr] = deterministic_input_values?i:dist(gen);
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
          if ((i<I) && (o<O)) kernel[addr_k] = deterministic_input_values?i:dist(gen);
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
    	  if (i < I) dw_kernel[addr_k] = deterministic_input_values?kernel_id:dist(gen); else dw_kernel[addr_k] = 0;
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
		  if ((i < I) && (o < O)) pw_kernel[addr_k] = deterministic_input_values?kernel_id:dist(gen); else pw_kernel[addr_k] = 0;
	  }
	  kernel_id++;
  }
#endif

  for (int cout=0; cout<O; cout++) bias[cout] = deterministic_input_values?cout:dist(gen);
}
