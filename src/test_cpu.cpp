/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

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
	int gi = i / CPI;
	int ii = i % CPI;
	int addr = (gi * H * W * CPI) + (h * W * CPI) + (w * CPI) + ii;
	return addr;
}

int output_data_address(int o, int h, int w, int height, int width) {
	int go = o / CPO;
	int oo = o % CPO;
	int addr = (go * height * width * CPO) + (h * width * CPO) + (w * CPO) + oo;
	return addr;
}

// cpu_conv2d. Performs the convolutions on the cpu
void cpu_conv2D() {

  int size_out = O_output * WO_conv * HO_conv;
  for (int i=0; i<size_out; i++) out_conv_cpu[i] = conv_t(0.f);

  for (int c=0; c<I_input; c++) {
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO_conv; h++) {
        for (int w=0; w<WO_conv; w++) {
          // data_out pixel position
          int addr_o = output_data_address(cout, h, w, HO_conv, WO_conv);

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
              din_t din = padding? din_t(0) : din_t(data_in[addr_p]);
              if (!padding) out_conv_cpu[addr_o] += din_t(din) * w_t(kernel[addr_k]);
  //            if ((h==0) && (w==1) && (cout==0)) printf("CONV: data %f kernel %f, pixel out %f\n", float(din), float(kernel[addr_k]), float(out_conv_cpu[addr_o]));
            }
          }
//	  printf("cpu_conv: c %d h %d w %d out_conv: %f\n", cout, h, w, float(out_conv_cpu[addr_o]));
        }
      }
    }
  }

  // let's add bias
  for (int cout=0; cout<O_output; cout++) {
    for (int h=0; h<HO_conv; h++) {
      for (int w=0; w<WO_conv; w++) {
        // data_out pixel position
        int addr_o = output_data_address(cout, h, w, HO_conv, WO_conv);
        // bias operation
        out_conv_cpu[addr_o] += b_t(bias[cout]);
	//printf("cpu_bias: c %d h %d w %d out_conv %f\n", cout, h, w, float(out_conv_cpu[addr_o]));
      }
    }
  }

  // apply shift
  #ifdef USE_SHIFT
  if (enable_shift) {
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO_conv; h++) {
        for (int w=0; w<WO_conv; w++) {
          int addr_o = output_data_address(cout, h, w, HO_conv, WO_conv);
          if (dir_shift == LEFT_DIRECTION) out_conv_cpu[addr_o] = out_conv_cpu[addr_o] << pos_shift;
          else out_conv_cpu[addr_o] = out_conv_cpu[addr_o] >> pos_shift;
          //printf("shift: c %d h %d w %d out_conv %f \n", cout, h, w, float(out_conv_cpu[addr_o]));
        }
      }
    }
  }
  #endif

  // apply clipping
  if (enable_clipping){
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO_conv; h++) {
        for (int w=0; w<WO_conv; w++) {
          int addr_o = output_data_address(cout, h, w, HO_conv, WO_conv);
          if (out_conv_cpu[addr_o] < min_clip) out_conv_cpu[addr_o] = min_clip;
          else if (out_conv_cpu[addr_o] > max_clip) out_conv_cpu[addr_o] = max_clip;
          //printf("clip: c %d h %d w %d out_conv %f \n", cout, h, w, float(out_conv_cpu[addr_o]));
        }
      }
    }
  }


  // apply relu
  if (enable_relu){
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO_relu; h++) {
        for (int w=0; w<WO_relu; w++) {
          int addr_o = output_data_address(cout, h, w, HO_relu, WO_relu);
          if (float(out_conv_cpu[addr_o]) < float(0)) out_relu_cpu[addr_o] = out_conv_cpu[addr_o] * (relu_t)relu_factor; else out_relu_cpu[addr_o] = out_conv_cpu[addr_o];
          //printf("cpu_relu: c %d h %d w %d out_conv %f out_relu %f\n", cout, h, w, float(out_conv_cpu[addr_o]), float(out_relu_cpu[addr_o]));
        }
      }
    }
  }

// apply stm
#ifdef USE_STM
  if (enable_stm){
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO_stm; h++) {
        for (int w=0; w<WO_stm; w++) {
          int addr_o = output_data_address(cout, h, w, HO_stm, WO_stm);
          if(enable_relu)
            out_stm_cpu[addr_o] = (tanh(log(exp((float)out_conv_cpu[addr_o]) + 1))) * out_relu_cpu[addr_o];
          else
            out_stm_cpu[addr_o] = (tanh(log(exp((float)out_conv_cpu[addr_o]) + 1))) * out_conv_cpu[addr_o];
	  }
        }
     }
  }
#endif

  // apply maxpooling or avgpooling
  if (enable_maxpooling) {
    for (int o=0; o<O_output; o++) {
      for (int h=0; h<HO_pool; h=h+1) {
    	for (int w=0; w<WO_pool; w=w+1) {
          int addr_out = output_data_address(o, h, w, HO_pool, WO_pool);
    	  pool_t max_v = -9999999;
    	  for (int kh=0; kh<2; kh++) {
            for (int kw=0; kw<2; kw++) {
    	      int h_in = (h*2) + kh;
    	      int w_in = (w*2) + kw;
              int addr_in = output_data_address(o, h_in, w_in, HO_conv, WO_conv);
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
      for (int h=0; h<HO_pool; h=h+1) {
    	for (int w=0; w<WO_pool; w=w+1) {
          int addr_out = output_data_address(o, h, w, HO_pool, WO_pool);
    	  pool_t sum_v = 0;
    	  for (int kh=0; kh<2; kh++) {
            for (int kw=0; kw<2; kw++) {
    	      int h_in = (h*2) + kh;
    	      int w_in = (w*2) + kw;
              int addr_in = output_data_address(o, h_in, w_in, HO_conv, WO_conv);
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
      for (int h=0; h<HO_bn; h++) {
        for (int w=0; w<WO_bn; w++) {
          int addr_o = output_data_address(cout, h, w, HO_bn, WO_bn);
          int index = cout/CPO * CPO + (addr_o % CPO); // mal ([0,3])
	  bn_t std, xn;
	  std = sqrtf((float)batch_norm_values[(index*4)+3] + 1e-5);
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
        }
      }
    }
  }

  if (enable_add) {
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<HO_add; h++) {
        for (int w=0; w<WO_add; w++) {
      	  int addr_o = output_data_address(cout, h, w, HO_add, WO_add);
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
          if (apply_add_relu && out_add_cpu[addr_o] < 0) out_add_cpu[addr_o] = 0;
        }
      }
    }
  }

  // We now copy the output to the final output for the cpu, taking into account if upsize is enabled
  for (int cout = 0; cout < O_output; cout++) {
    for (int h = 0; h < HO_upsize; h++) {
      for (int w = 0; w < WO_upsize; w++) {
	int row, col;
	if (enable_upsize) row = h / upsize_factor; else row = h;
	if (enable_upsize) col = w / upsize_factor; else col = w;
        int addr_out = output_data_address(cout, row, col, HI_upsize, WI_upsize);
	dout_t v;
        if (enable_add) v = out_add_cpu[addr_out];
        else if (enable_batch_norm) v = out_batch_norm_cpu[addr_out];
        else if (enable_avgpooling | enable_maxpooling) v = out_pool_cpu[addr_out];
        else if (enable_stm) v = out_stm_cpu[addr_out];
        else if (enable_relu) v = out_relu_cpu[addr_out];
        else v = out_conv_cpu[addr_out];

        int addr_upsize_out = output_data_address(cout, h, w, HO_upsize, WO_upsize);
        cpu_out[addr_upsize_out] = v;
      }
    }
  }

}
