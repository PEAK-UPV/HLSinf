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

// cpu_conv2d. Performs the convolutions on the cpu
void cpu_conv2D() {

  int size_out = O * W * H;
  for (int i=0; i<size_out; i++) out_conv_cpu[i] = 0.f;

  for (int c=0; c<I; c++) {
    for (int cout=0; cout<O; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          for (int kh=0; kh<KH; kh++) {
            for (int kw=0; kw<KW; kw++) {

              int data_h = (h-1)+kh;
              int data_w = (w-1)+kw;

              int padding = (data_h == -1) | (data_w == -1) | (data_w == W) | (data_h == H);

              // kernel position
			  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
              int addr_k = filter_address_direct_conv(c, cout, kh, kw);
              #endif

			  #ifdef DWS_CONV
              // addr_dw_k and addr_pw_k for dws convs
              int addr_dw_k = (c * KH * KW) + (kh * KW) + kw;
              int addr_pw_k = filter_address_dws_conv(c, cout);
              #endif

              // data_in pixel position
              int addr_p = (c * W * H) + (data_h * W) + data_w;
              // data_out pixel position
              int addr_o = (cout * W * H) + (h * W) + w;

              // operation
              data_type din = padding? data_type(0) : data_in[addr_p];
			  #if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
              if (!padding) out_conv_cpu[addr_o] += din * kernel[addr_k];
			  #endif
              #ifdef DWS_CONV
              if (!padding) out_conv_cpu[addr_o] += din * dw_kernel[addr_dw_k] * pw_kernel[addr_pw_k];
              #endif
            }
          }
        }
      }
    }
  }

  // let's add bias
  for (int cout=0; cout<O; cout++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
        // data_out pixel position
        int addr_o = (cout * W * H) + (h * W) + w;
        // bias operation
        out_conv_cpu[addr_o] += bias[cout];
      }
    }
  }

  // apply shift
  #ifdef API8_DATA_TYPE
  if (enable_shift) {
    for (int cout=0; cout<O; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          int addr_o = (cout * W * H) + (h * W) + w;
          if (dir_shift == LEFT_DIRECTION) out_conv_cpu[addr_o] = out_conv_cpu[addr_o] << pos_shift;
          else out_conv_cpu[addr_o] = out_conv_cpu[addr_o] >> pos_shift;
        }
      }
    }
  }
  #endif

  // apply clipping
  #ifdef API8_DATA_TYPE
  if (enable_clipping){
    for (int cout=0; cout<O; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          int addr_o = (cout * W * H) + (h * W) + w;
          if (out_conv_cpu[addr_o] < min_clip) out_conv_cpu[addr_o] = min_clip;
          else if (out_conv_cpu[addr_o] > max_clip) out_conv_cpu[addr_o] = max_clip;
        }
      }
    }
  }
  #endif

  // apply relu
  if (enable_relu){
    for (int cout=0; cout<O; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          int addr_o = (cout * W * H) + (h * W) + w;
          if (float(out_conv_cpu[addr_o]) < float(0)) out_relu_cpu[addr_o] = 0.f; else out_relu_cpu[addr_o] = out_conv_cpu[addr_o];
        }
      }
    }
  }

  // apply maxpooling or avgpooling
  if (enable_maxpooling) {
    for (int o=0; o<O; o++) {
      for (int h=0; h<H/2; h++) {
    	for (int w=0; w<W/2; w++) {
    	  int addr_out = (o * (W/2) * (H/2)) + (h * (W/2)) + w;
    	  data_type max_v = -9999999;
    	  for (int kh=0; kh<2; kh++) {
    		for (int kw=0; kw<2; kw++) {
    		  int h_in = (h * 2) + kh;
    		  int w_in = (w * 2) + kw;
              int addr_in = (o * W * H) + (h_in * W) + w_in;
              if (enable_relu) {
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
    for (int o=0; o<O; o++) {
      for (int h=0; h<H/2; h++) {
    	for (int w=0; w<W/2; w++) {
    	  int addr_out = (o * (W/2) * (H/2)) + (h * (W/2)) + w;
    	  data_type sum_v = 0;
    	  for (int kh=0; kh<2; kh++) {
    		for (int kw=0; kw<2; kw++) {
    		  int h_in = (h * 2) + kh;
    		  int w_in = (w * 2) + kw;
              int addr_in = (o * W * H) + (h_in * W) + w_in;
              if (enable_relu) {
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
}
