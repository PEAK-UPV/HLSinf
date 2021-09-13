/*
 * data-related test functions
 */

#include "test_conv2D.h"

void init_data() {
  std::random_device rd;
  std::mt19937 gen(rd());


  float my_flt_din  = 0;
  float my_flt_bias = 0;
  float my_flt_krnl = 0;

  // random number generators
  #if defined(FP32_DATA_TYPE) || defined(APF8_DATA_TYPE)
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
  #endif
  #ifdef API8_DATA_TYPE
  std::uniform_int_distribution<int> dist(-10, 10);
  #endif
  #ifdef API16_DATA_TYPE
  std::uniform_int_distribution<int> dist(-10, 10);
  #endif

  // input data
  #ifdef HLS_DEBUG
  printf(KCYN "initializing data  deterministic_values -> %s   I = %2d\n" KNRM,  deterministic_input_values?"yes":" no", I);
  #endif
  int addr;
  int det_val = 1;
  for (int i=0; i<I_input; i++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
    	addr = input_data_address(i, h, w);
      data_type value;
    	if (i<I) {
          //value = deterministic_input_values?(i):dist(gen);
          //value = deterministic_input_values?(i+1):dist(gen);
          value = deterministic_input_values?(det_val++):dist(gen);
    	} else {
    	  value = 0;
    	}

      data_in[addr] = value;
      my_flt_din = my_flt_din + data_in[addr];

      #ifdef HLS_DEBUG
      if (I_input < I_useful )printf("  i = %2d   h = %2d   w = %2d   addr = %2d   data_in[%2d] = %2.2f \n", i, h, w, addr, addr, (float)value);
      #endif

        
      addr++;
      }
    }
  }

#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  #ifdef HLS_DEBUG
  // for this case number of kernels is 1, and input and output channels are one, so
  printf(KRED "warning: forced to display I=0 and O=0 for kernel, skipping other input and output kernel channels\n" KNRM);
  printf(KCYN "I_kernel = %d   O_kernel = %d   KH = %d   KW = %d\n" KNRM, I_kernel, O_kernel, KH, KW);
  printf(KCYN "kernel matrix\n" KNRM);
  #endif

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

          // for direct conv kernel
          my_flt_krnl = my_flt_krnl + kernel[addr_k];
          #ifdef HLS_DEBUG
          // for this case number of kernels is 1, and input and output channels are one, so
          if ((i == 0) && (o == 0))printf(" %3.3f ", kernel[addr_k]);
          #endif
	    }
        #ifdef HLS_DEBUG
        if ((i == 0) && (o == 0)) printf("\n"); // printf of kernel values, next row
        #endif
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

  for (int cout=0; cout<O; cout++) 
    bias[cout] = deterministic_input_values?(cout%20)-10:dist(gen);

  #ifdef HLS_DEBUG
  // for bias
  printf(KCYN "bias matrix\n" KNRM);

  for (int cout=0; cout<O; cout++){
    my_flt_bias = my_flt_bias + bias[cout];
    printf("  bias [%2d] = %2.2f\n", cout, bias[cout]);
  }

  printf("\n");
  printf(" -- This is JM10 at host side  my_flt_bias = %f   my_flt_krnl = %f   my_flt_din = %f\n\n", my_flt_bias, my_flt_krnl, my_flt_din);

  #endif

}
