/*
 * data-related test functions
 */

#include "test_conv2D.h"

void init_data() {
  std::random_device rd;
  std::mt19937 gen(rd());

  // random number generators
  #if defined(FLOAT_DATA_TYPE)
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
  #else
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

  //input add data
  if(enable_add) {
	  addr = 0;
	  for (int o=0; o<O_output; o++) {
		  for (int h=0; h<HO_final; h++) {
			  for (int w=0; w<WO_final; w++) {
				  addr = output_data_address(o, h, w, HO_final, WO_final);
				  if (o<O) {
					  data_in_add[addr] = deterministic_input_values?o:dist(gen);
				  } else {
					  data_in_add[addr] = 0;
				  }
				  addr++;
			  }
		  }
	  }
  }

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
          if ((i<I) && (o<O)) kernel[addr_k] = deterministic_input_values?(i % 20):dist(gen);
          else kernel[addr_k] = 0;
	    }
	  }
      kernel_id++;
    }
  }

  #ifdef USE_BATCH_NORM
  // Generating values for batch normalization layer
  for (int cout=0; cout<O_output; cout++) {
	  if (cout < O) {
      batch_norm_values[cout*4+0] = 0;
      batch_norm_values[cout*4+1] = 1;
      batch_norm_values[cout*4+2] = 0;
      batch_norm_values[cout*4+3] = 1;
		  //batch_norm_values[cout] = deterministic_input_values?(cout%20)-10:dist(gen);
	  } else {
		  batch_norm_values[cout] = 0;
	  }
  }
  #endif

  for (int cout=0; cout<O; cout++) bias[cout] = deterministic_input_values?(cout%20)-10:dist(gen);
}
