/*
 * data-related test functions
 */

#include "test_conv2D.h"


void init_data() {

  #ifdef DEBUG_DATA_DIN
  printf("initialize data\n");
  #endif

  #if defined (RANDOM_GEN_SEED_FIXED)
  printf(KCYN "INIT_DATA: set fix random seed for deterministic random values for debugging\n" KNRM);
  std::mt19937 gen(0);
  #else
  std::random_device rd;
  std::mt19937 gen(rd());
  #endif


  float my_flt_din  = 0;
  float my_flt_bias = 0;
  float my_flt_krnl = 0;

 // // random number generators
 // #if defined(FP32_DATA_TYPE) || defined(APF8_DATA_TYPE)
 // std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
 // #endif
 // #ifdef API8_DATA_TYPE
 // std::uniform_int_distribution<int> dist(-10, 10);
 // #endif
 // #ifdef API16_DATA_TYPE
 // std::uniform_int_distribution<int> dist(-10, 10);
 // #endif

 // #ifdef API8_S10MX_DATA_TYPE
 // std::uniform_int_distribution<int> dist(-10, 10);
 // #endif
 // #ifdef API16_S10MX_DATA_TYPE
 // std::uniform_int_distribution<int> dist(-10, 10);
 // #endif
 // #ifdef API32_S10MX_DATA_TYPE
 // std::uniform_int_distribution<int> dist(-10, 10);
 // #endif


  // random number generators
  #if defined(FP32_DATA_TYPE) || defined(APF8_DATA_TYPE)
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
  std::uniform_real_distribution<float> dist_filters(-2.0f, 2.0f);
  std::uniform_real_distribution<float> dist_bias(-1000.0f, 1000.0f);
  #else
  std::uniform_int_distribution<int> dist(0, 255);
  std::uniform_int_distribution<int> dist_filters(-125, 127);
  std::uniform_int_distribution<int> dist_bias(-32700, 32700);
  #endif


  // ----------------- 
  //    DATA IN
  // ----------------

  // input data
  #ifdef DEBUG_DATA_DIN
  printf(KCYN "initializing data  deterministic_values -> %s\n" KNRM,  deterministic_input_values?"yes":" no");
  printf("data in matrix for %d input channels channels\n", I);
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

      #ifdef DEBUG_DATA_DIN
      #ifdef DEBUG_VERBOSE
      if (i < I )printf("  i = %2d   h = %2d   w = %2d   addr = %2d   data_in[%2d] = %2.2f \n", i, h, w, addr, addr, (float)value);
      #endif       
      #endif
      addr++;
      }
    }
  }
  #ifdef DEBUG_DATA_DIN
  printf(KCYN "data in matrix\n" KNRM);
  for (int i=0; i<I_input; i++) {
    printf("input channel i = %d\n", i);
    printf("W    ");
    for (int w=0; w<W; w++) printf("  %6d ", w);
    printf("\n");

    for (int h=0; h<H; h++) {
      printf ("H %2d ", h);
      for (int w=0; w<W; w++) {       
        addr = input_data_address(i, h, w);
        printf("  %6.2f ", data_in[addr]);
      }
      printf("\n");
    }
  }

  #endif
  
  // ----------------- 
  //    KERNEL
  // ----------------
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

          // for direct conv kernel
          my_flt_krnl = my_flt_krnl + kernel[addr_k];
	    }
	  }
      kernel_id++;
    }
  }

  #ifdef DEBUG_DATA_KERNEL
  // for this case number of kernels is 1, and input and output channels are one, so
  printf("I_kernel = %d   O_kernel = %d   KH = %d   KW = %d\n", I_kernel, O_kernel, KH, KW);
  printf(KCYN "kernel matrix\n" KNRM);
  
  for (int i=0; i<I_kernel; i++) {
    for (int o=0; o<O_kernel; o++) {
      printf("k_i = %2d   k_o = %2d\n", i, o);
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
          printf("  %6.2f ", kernel[addr_k]);
        }
        printf("\n");
      }
    }
  }
  #endif // DEBUG

  #endif // direct_conv winograd

  // ----------------- 
  //    KERNEL dw-pw
  // ----------------
  #ifdef DWS_CONV
  int kernel_id = 1;
  for (int i=0; i<I_kernel; i++) {
    for (int kh=0; kh<KH; kh++) {
      for (int kw=0; kw<KW; kw++) {
    	  int addr_k = (i * KW * KH) + (kh * KW) + kw;
    	  if (i < I) dw_kernel[addr_k] = deterministic_input_values ? (kernel_id%20)-10 : dist_filters(gen);
        else       dw_kernel[addr_k] = 0;
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
		  if ((i < I) && (o < O)) pw_kernel[addr_k] = deterministic_input_values?(kernel_id%20)-10:dist_filters(gen); else pw_kernel[addr_k] = 0;
	  }
	  kernel_id++;
  }
  #endif

  // ----------------- 
  //    BIAS
  // ----------------
  for (int cout=0; cout<O; cout++) 
    bias[cout] = deterministic_input_values?(cout%20)-10:dist(gen);

  #ifdef DEBUG_DATA_BIAS
  // for bias
  printf(KCYN "bias matrix ( %d output channels )\n" KNRM, O);
  for (int cout=0; cout<O; cout++){
    printf("  bias [%2d] = %6.2f\n", cout, bias[cout]);
  }
  printf("\n");
  #endif

  // ----------------- 
  //    ADD DATA IN
  // -----------------
  // IF enable_add ???
  //input add data
  if(enable_add) {
	  addr = 0;
    uint enable_pooling = enable_maxpooling || enable_avgpooling;
    uint WO_final = enable_pooling ? ((W - KW_POOLING)/SW_POOLING + 1) : W;
    uint HO_final = enable_pooling ? ((H - KH_POOLING)/SH_POOLING + 1) : H;



//  #ifdef DEBUG_ADD_DATA
  printf("enable_add ->>>  add_data: O_OUTPUT %d  enable_pooling %s  H = %d   W = %d   HO = %d   WO = %d\n", O_output, enable_pooling?"yes":" no", H, W, HO_final, WO_final);
//  #endif


    #ifdef DEBUG_READ_ADD_DATA
    printf(KCYN "initializing add_data deterministic_values -> %s\n" KNRM,  deterministic_input_values?"yes":" no");
    printf("add data in matrix for %d output channels channels\n", O);
    #endif
    

	  for (int o=0; o<O_output; o++) {
		  for (int h=0; h<HO_final; h++) {
			  for (int w=0; w<WO_final; w++) {
				  //addr = output_data_address(o, h, w, HO_final, WO_final);
          addr = output_data_address(o, h, w);
				  if (o<O) {
					  data_in_add[addr] = deterministic_input_values?o:dist(gen);
				  } else {
					  data_in_add[addr] = 0;
				  }
          #ifdef DEBUG_READ_ADD_DATA
          #ifdef DEBUG_VERBOSE
          if (o < O)printf("  o = %2d   h = %2d   w = %2d   addr = %2d   add_data_in[%2d] = %2.2f \n", o, h, w, addr, addr, (float)data_in_add[addr]);
          #endif
          #endif
				  addr++;
			  }
		  }
	  }
    printf("enable_add finalizes\n\n");
  }
  else {
    #ifdef DEBUG_READ_ADD_DATA
    printf(KCYN "add_data kernel dissabled\n" KNRM);
    #endif
  }
  


  // ----------------- 
  //    BATCH NORM
  // -----------------
  //#ifdef USE_BATCH_NORM
  // Generating values for batch normalization layer
  for (int cout=0; cout<O_output; cout++) {
	  if (cout < O) {
      batch_norm_values[cout*4+0] = 0;
      batch_norm_values[cout*4+1] = 1;
      batch_norm_values[cout*4+2] = 0;
      batch_norm_values[cout*4+3] = 1;
		  //batch_norm_values[cout] = deterministic_input_values?(cout%20)-10:dist(gen);
	  } else {
		  batch_norm_values[cout*4+0] = 0;
		  batch_norm_values[cout*4+1] = 0;
		  batch_norm_values[cout*4+2] = 0;
		  batch_norm_values[cout*4+3] = 0;
	  }
  }
  //#endif
  printf("batch_norm data initialization finalizes\n\n");



  #ifdef DEBUG_BATCH_NORM
  printf(KCYN "batch_norm_values ( %d output channels )\n" KNRM, O_output);
  for (int cout=0; cout<O; cout++){
    printf("  bn_value [%2d][0:3] = %6.2f  %6.2f  %6.2f  %6.2f \n", 
        cout,
        batch_norm_values[cout*4+0], batch_norm_values[cout*4+1],
        batch_norm_values[cout*4+2], batch_norm_values[cout*4+3]
        );
  }
  printf("\n");
  #endif


  printf("data initialization finalizes\n\n");


}
