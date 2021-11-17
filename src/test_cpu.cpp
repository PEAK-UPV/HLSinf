/*
 * CPU-related test functions
 */

#include "test_conv2D.h"

int filter_address_direct_conv(int i, int o, int kh, int kw) {
    int gi = i / CPI;
    int ki = i % CPI;
    int go = o / CPO;
    int ko = o % CPO;
    // addr_k for direct convs or winograd convs
    int addr_k = (go * GI * CPO * CPI * KH * KW) +
                 (gi * CPO * CPI * KH * KW) +
                 (ko * CPI * KH * KW) +
                 (ki * KH * KW) +
                 (kh * KW) +
                 kw;
    return addr_k;
}

int filter_address_dws_conv(int i, int o) {
    int gi = i / CPI;
    int ki = i % CPI;
    int go = o / CPO;
    int ko = o % CPO;
    // addr_k for direct convs or winograd convs
    int addr_k = (go * GI * CPO * CPI) +
                 (gi * CPO * CPI) +
                 (ko * CPI) +
                  ki;
    return addr_k;
}

int input_data_address(int i, int h, int w) {
    #ifdef GIHWCPI_DATA_FORMAT
	int gi = i / CPI;
	int ii = i % CPI;
	int addr = (gi * H * W * CPI) + (h * W * CPI) + (w * CPI) + ii;
    #endif
    #ifdef IHW_DATA_FORMAT
    int addr = (i * H * W) + (h * W) + w;
    #endif
	return addr;
}

int output_data_address(int o, int h, int w, int height, int width) {
    #ifdef GIHWCPI_DATA_FORMAT
	int go = o / CPO;
	int oo = o % CPO;
	int addr = (go * height * width * CPO) + (h * width * CPO) + (w * CPO) + oo;
    #endif
    #ifdef IHW_DATA_FORMAT
    int addr = (o * height * width) + (h * width) + w;
    #endif
	return addr;
}

int output_data_address_div(int o, int h, int w) {
    #ifdef GIHWCPI_DATA_FORMAT
	int go = o / CPO;
	int oo = o % CPO;
	int addr = (go * (HO/2) * (WO/2) * CPO) + (h * (WO/2) * CPO) + (w * CPO) + oo;
    #endif
    #ifdef IHW_DATA_FORMAT
    int addr = (o * (HO/2) * (WO/2)) + (h * (WO/2)) + w;
    #endif
	return addr;
}

// cpu_conv2d. Performs the convolutions on the cpu
void cpu_conv2D() {

  int size_out = O_output * WO * HO;
  for (int i=0; i<size_out; i++) out_conv_cpu[i] = 0.f;

#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  for (int c=0; c<I_input; c++) {
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO; h++) {
        for (int w=0; w<WO; w++) {
          // data_out pixel position
          int addr_o = output_data_address(cout, h, w, HO, WO);

          for (int kh=0; kh<KH; kh++) {
            for (int kw=0; kw<KW; kw++) {

              int data_h = (h*SH)-PT+kh;
              int data_w = (w*SW)-PL+kw;

              int padding = (data_h < 0) | (data_w < 0) | (data_w >= W) | (data_h >= H);

              // kernel position
              int addr_k = filter_address_direct_conv(c, cout, kh, kw);

              // data_in pixel position
              int addr_p = input_data_address(c, data_h, data_w);

              // operation
              data_type din = padding? data_type(0) : data_in[addr_p];
              if (!padding) out_conv_cpu[addr_o] += din * kernel[addr_k];
            }
          }
	  //printf("cpu_conv: c %d h %d w %d out_conv: %f\n", cout, h, w, out_conv_cpu[addr_o]);
        }
      }
    }
  }
#endif

#ifdef DWS_CONV
  data_type *pixel_dw = (data_type *)malloc(sizeof(data_type) * I_input);
  for (int h=0; h<H; h++) {
    for (int w=0; w<W; w++) {
      for (int i=0; i<I_input; i++) pixel_dw[i] = 0;
      for (int kh=0; kh<KH; kh++) {
        for (int kw=0; kw<KW; kw++) {

          int data_h = (h-1)+kh;
          int data_w = (w-1)+kw;

          int padding = (data_h == -1) | (data_w == -1) | (data_w == W) | (data_h == H);

          // DW operation
          for (int c=0; c<I_input; c++) {

             // kernel position
            int addr_dw_k = (c * KH * KW) + (kh * KW) + kw;

            // data_in pixel position
            int addr_p = input_data_address(c, data_h, data_w);

            data_type din = padding? data_type(0) : data_in[addr_p];
            if (!padding) pixel_dw[c] += din * dw_kernel[addr_dw_k];
          }
        }
      }

      // point wise reduce operation
      for (int o=0; o<O_output; o++) {
        int addr_o = output_data_address(o, h, w, HO, WO);
        out_conv_cpu[addr_o] = 0;
    	for (int i=0; i<I_input; i++) {
          int addr_pw_k = filter_address_dws_conv(i, o);
          out_conv_cpu[addr_o] += pixel_dw[i] * pw_kernel[addr_pw_k];
        }
      }
    }
  }
  free(pixel_dw);
#endif

  // let's add bias
  for (int cout=0; cout<O_output; cout++) {
    for (int h=0; h<HO; h++) {
      for (int w=0; w<WO; w++) {
        // data_out pixel position
        int addr_o = output_data_address(cout, h, w, HO, WO);
        // bias operation
        out_conv_cpu[addr_o] += bias[cout];
	//printf("cpu_bias: c %d h %d w %d out_conv %f\n", cout, h, w, out_conv_cpu[addr_o]);
      }
    }
  }

  // apply shift
  #if defined(API8_DATA_TYPE) || defined (API16_DATA_TYPE)
  if (enable_shift) {
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO; h++) {
        for (int w=0; w<WO; w++) {
          int addr_o = output_data_address(cout, h, w, HO, WO);
          if (dir_shift == LEFT_DIRECTION) out_conv_cpu[addr_o] = out_conv_cpu[addr_o] << pos_shift;
          else out_conv_cpu[addr_o] = out_conv_cpu[addr_o] >> pos_shift;
        }
      }
    }
  }
  #endif

  // apply clipping
  #if defined(API8_DATA_TYPE) || defined (API16_DATA_TYPE)
  if (enable_clipping){
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO; h++) {
        for (int w=0; w<WO; w++) {
          int addr_o = output_data_address(cout, h, w, HO, WO);
          if (out_conv_cpu[addr_o] < min_clip) out_conv_cpu[addr_o] = min_clip;
          else if (out_conv_cpu[addr_o] > max_clip) out_conv_cpu[addr_o] = max_clip;
        }
      }
    }
  }
  #endif

  // apply relu
  if (enable_relu){
    // multiply by scalar
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO; h++) {
        for (int w=0; w<WO; w++) {
          int addr_o = output_data_address(cout, h, w, HO, WO);
          if (out_conv_cpu[addr_o]) out_scalar_mult_cpu[addr_o] = out_conv_cpu[addr_o] * scalar_mult_value;
        }
      }
    }
    // relu
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO; h++) {
        for (int w=0; w<WO; w++) {
          int addr_o = output_data_address(cout, h, w, HO, WO);
          if (float(out_scalar_mult_cpu[addr_o]) < float(0)) out_relu_cpu[addr_o] = out_scalar_mult_cpu[addr_o] * relu_factor; else out_relu_cpu[addr_o] = out_scalar_mult_cpu[addr_o];
          //printf("cpu_relu: c %d h %d w %d cout_conv %f out_relu %f\n", cout, h, w, out_conv_cpu[addr_o], out_relu_cpu[addr_o]);
        }
      }
    }
  }
  #ifdef USE_STM
  // apply stm
  if (enable_stm){
	  for (int cout=0; cout<O_output; cout++) {
		  for (int h=0; h<HO; h++) {
			  for (int w=0; w<WO; w++) {
				  int addr_o = output_data_address(cout, h, w, HO, WO);
          if(enable_relu)
          	out_stm_cpu[addr_o] = (tanh(log(exp(out_conv_cpu[addr_o]) + 1))) * out_relu_cpu[addr_o];
          else
				    out_stm_cpu[addr_o] = (tanh(log(exp(out_conv_cpu[addr_o]) + 1))) * out_conv_cpu[addr_o];
          //printf("cpu_stm: (addr_o %d) c %d h %d w %d out_relu %f out_stm %f \n", addr_o, cout, h, w, out_relu_cpu[addr_o], out_stm_cpu[addr_o]);
			  }
		  }
	  }
  }
  #endif

  // apply maxpooling or avgpooling
  if (enable_maxpooling) {
    for (int o=0; o<O_output; o++) {
      for (int h=0; h<HO; h=h+2) {
    	for (int w=0; w<WO; w=w+2) {
          int addr_out = output_data_address_div(o, h/2, w/2);
    	  data_type max_v = -9999999;
    	  for (int kh=0; kh<2; kh++) {
    		for (int kw=0; kw<2; kw++) {
    		  int h_in = h + kh;
    		  int w_in = w + kw;
              int addr_in = output_data_address(o, h_in, w_in, HO, WO);
	      //printf("cpu: o %d h_in %d w_in %d value %f\n", o, h_in, w_in, out_conv_cpu[addr_in]);
              if(enable_stm) {
                if (out_stm_cpu[addr_in] > max_v) max_v = out_stm_cpu[addr_in];
              } else if (enable_relu) {
                if (out_relu_cpu[addr_in] > max_v) max_v = out_relu_cpu[addr_in];
 	      } else {
                if (out_conv_cpu[addr_in] > max_v) max_v = out_conv_cpu[addr_in];
              }
    	  }
	  }
    	  out_pool_cpu[addr_out] = max_v;
    	}
      }
    }
  }

  if (enable_avgpooling) {
    for (int o=0; o<O_output; o++) {
      for (int h=0; h<HO; h=h+2) {
    	for (int w=0; w<WO; w=w+2) {
          int addr_out = output_data_address_div(o, h/2, w/2);
    	  data_type sum_v = 0;
    	  for (int kh=0; kh<2; kh++) {
    		for (int kw=0; kw<2; kw++) {
    		  int h_in = h + kh;
    		  int w_in = w + kw;
              int addr_in = output_data_address(o, h_in, w_in, HO, WO);
	      //printf("cpu_avgp: o %d h_in %d w_in %d value %f\n", o, h_in, w_in, out_conv_cpu[addr_in]);
              if (enable_stm) {
            	sum_v += out_stm_cpu[addr_in];
              } else if (enable_relu) {
	        sum_v += out_relu_cpu[addr_in];
	      } else {
                sum_v += out_conv_cpu[addr_in];
              }
    	  }
	  }
    	  out_pool_cpu[addr_out] = sum_v / 4;
    	}
      }
    }
  }

  // apply batch normalization
  if (enable_batch_norm) {
	  for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO_final; h++) {
        for (int w=0; w<WO_final; w++) {
  	      int addr_o = output_data_address(cout, h, w, HO_final, WO_final);
				  int index = cout/CPO * CPO + (addr_o % CPO); // mal ([0,3])
				  data_type std, xn;
				  std = sqrtf(batch_norm_values[(index*4)+3] + 1e-5);
          if(enable_maxpooling || enable_avgpooling) {
            xn = (out_pool_cpu[addr_o] - batch_norm_values[(index*4)+2]) / std;
          } else if(enable_stm) {
            xn = (out_stm_cpu[addr_o] - batch_norm_values[(index*4)+2]) / std;
          } else if (enable_relu) {
				    xn = (out_relu_cpu[addr_o] - batch_norm_values[(index*4)+2]) / std;
          } else {
				    xn = (out_conv_cpu[addr_o] - batch_norm_values[(index*4)+2]) / std;
          }
				  out_batch_norm_cpu[addr_o] = batch_norm_values[(index*4)+1] * xn + batch_norm_values[index*4];
          //printf("cpu: o %d h_in %d w_in %d out %f (%f,%f,%f) index %d\n", cout, h, w, out_batch_norm_cpu[addr_o],batch_norm_values[(index*4)+3],batch_norm_values[(index*4)+2],batch_norm_values[(index*4)+1], index);
			  }
		  }
	  }
  }

  if (enable_add) {
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO_final; h++) {
        for (int w=0; w<WO_final; w++) {
      	  int addr_o = output_data_address(cout, h, w, HO_final, WO_final);
          if(enable_batch_norm) 
            out_add_cpu[addr_o] = data_in_add[addr_o] + out_batch_norm_cpu[addr_o];
  	      else if (enable_avgpooling || enable_maxpooling) 
            out_add_cpu[addr_o] = data_in_add[addr_o] + out_pool_cpu[addr_o];
  	      else if (enable_stm)
	          out_add_cpu[addr_o] = data_in_add[addr_o] + out_stm_cpu[addr_o];
	        else if (enable_relu)
  	        out_add_cpu[addr_o] = data_in_add[addr_o] + out_relu_cpu[addr_o];
  	      else
  	        out_add_cpu[addr_o] = data_in_add[addr_o] +  out_conv_cpu[addr_o];
	      }
      }
    }
  }

  // We now copy the output to the final output for the cpu
  for (int cout = 0; cout < O_output; cout++) {
    for (int h = 0; h < HO_final; h++) {
      for (int w = 0; w < WO_final; w++) {
        int addr_out = output_data_address(cout, h, w, HO_final, WO_final);
	      if (enable_add) cpu_out[addr_out] = out_add_cpu[addr_out];
        else if (enable_batch_norm) cpu_out[addr_out] = out_batch_norm_cpu[addr_out];
	      else if (enable_avgpooling | enable_maxpooling) cpu_out[addr_out] = out_pool_cpu[addr_out];
	      else if (enable_stm) cpu_out[addr_out] = out_stm_cpu[addr_out];
	      else if (enable_relu) cpu_out[addr_out] = out_relu_cpu[addr_out];
	      else cpu_out[addr_out] = out_conv_cpu[addr_out];
      }
    }
  }
}
