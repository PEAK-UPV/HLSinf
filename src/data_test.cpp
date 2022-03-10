/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

/*
 * data-related test functions
 */

#include "test_conv2D.h"

void read_from_file(char *file, int size, int data_size, void *buf) {
  FILE *fd = fopen(file, "r");
  if (fd == NULL) {printf("Error, file not found\n"); exit(1);}
  fread(buf, data_size, size, fd); 
  fclose(fd);
}

void init_data(int from_file) {
  std::random_device rd;
  std::mt19937 gen(rd());

  if (from_file) {
    read_from_file("input.bin", I_input * H * W, sizeof(read_data_t), (void *)data_in);
    if (enable_add) read_from_file("add.bin", O_output * HO_final * WO_final, sizeof(write_data_t), (void *)data_in_add);
    read_from_file("weights.bin", I_kernel * O_kernel * KH * KW, sizeof(read_filter_t), (void *)kernel);
    read_from_file("bias.bin", O, sizeof(read_bias_t), bias);
    read_from_file("output.bin", O_output * HO_final * WO_final, sizeof(write_data_t), (void *)cpu_out);
    if (enable_batch_norm) read_from_file("batchnorm.bin", O_output * 4, sizeof(dout_t), (void *)batch_norm_values);

    // statistics for each buffer
    printf("input (file)  : "); print_input_buffer_stats(data_in, I_input * H * W);
    printf("filter (file) : "); print_weight_buffer_stats(kernel, I_kernel * O_kernel * KH * KW);
    printf("bias (file)   : "); print_bias_buffer_stats(bias, O);
    if (enable_add) {printf("add    : "); print_data_in_buffer_stats(data_in_add, O_output * HO_final * WO_final);}
    if (enable_batch_norm) {printf("bn_v (file)   : "); print_batchnorm_buffer_stats(batch_norm_values, O_output * 4);}
    printf("output (file) : "); print_output_buffer_stats(cpu_out, O_output * HO_final * WO_final);
    return;
  }

  // random number generators
  #if defined(FLOAT_DATA_TYPE)
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
  std::uniform_real_distribution<float> dist_filters(-2.0f, 2.0f);
  std::uniform_real_distribution<float> dist_bias(-1000.0f, 1000.0f);
  #else
  std::uniform_int_distribution<int> dist(0, 255);
  std::uniform_int_distribution<int> dist_filters(-125, 127);
  std::uniform_int_distribution<int> dist_bias(-32700, 32700);
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
          if ((i<I) && (o<O)) kernel[addr_k] = deterministic_input_values?(i % 20):dist_filters(gen);
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
		  batch_norm_values[cout*4+0] = 0;
		  batch_norm_values[cout*4+1] = 0;
		  batch_norm_values[cout*4+2] = 0;
		  batch_norm_values[cout*4+3] = 0;
	  }
  }
  #endif

  for (int cout=0; cout<O_output; cout++) {
    if (cout < O) bias[cout] = deterministic_input_values?(cout%20)-10:dist_bias(gen);
    else bias[cout] = 0;
  }
}
