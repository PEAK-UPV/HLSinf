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

int output_data_address(int o, int h, int w) {
    #ifdef GIHWCPI_DATA_FORMAT
	int go = o / CPO;
	int oo = o % CPO;
	int addr = (go * H * W * CPO) + (h * W * CPO) + (w * CPO) + oo;
    #endif
    #ifdef IHW_DATA_FORMAT
    int addr = (o * H * W) + (h * W) + w;
    #endif
	return addr;
}

int output_data_address_div(int o, int h, int w) {
    #ifdef GIHWCPI_DATA_FORMAT
	int go = o / CPO;
	int oo = o % CPO;
	int addr = (go * (H/2) * (W/2) * CPO) + (h * (W/2) * CPO) + (w * CPO) + oo;
    #endif
    #ifdef IHW_DATA_FORMAT
    int addr = (o * (H/2) * (W/2)) + (h * (W/2)) + w;
    #endif
	return addr;
}

// cpu_conv2d. Performs the convolutions on the cpu
void cpu_conv2D() {

  int size_out = O_output * W * H;
  for (int i=0; i<size_out; i++) out_conv_cpu[i] = 0.f;

#if defined(DIRECT_CONV) || defined(WINOGRAD_CONV)
  for (int c=0; c<I_input; c++) {
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          // data_out pixel position
          int addr_o = output_data_address(cout, h, w);

          for (int kh=0; kh<KH; kh++) {
            for (int kw=0; kw<KW; kw++) {

              int data_h = (h-1)+kh;
              int data_w = (w-1)+kw;

              int padding = (data_h == -1) | (data_w == -1) | (data_w == W) | (data_h == H);

              // kernel position
              int addr_k = filter_address_direct_conv(c, cout, kh, kw);

              // data_in pixel position
              int addr_p = input_data_address(c, data_h, data_w);

              // operation
              data_type din = padding? data_type(0) : data_in[addr_p];


              if (!padding) {
                out_conv_cpu[addr_o] += din * kernel[addr_k];
                #ifdef DEBUG_CPU_MUL
                #ifdef DEBUG_VERBOSE
                printf("CPU_MUL PARTIAL c %2d  cout %2d   h %2d  w %2d   kh %2d   kw %2d    cout out_conv_cpu[%d] =  %8.2f\n",
                    c, cout, h, w, kh, kw, addr_o, out_conv_cpu[addr_o]
                    );
                #endif
                #endif
              }
            }
          }
        }
      }
    }
  }

  #ifdef DEBUG_CPU_MUL
  printf("CPU_MUL output matrix\n");
  for (int cout=0; cout<O_output; cout++) {

    printf("Channel %d\n", cout);
    printf("W    ");
    for (int w=0; w<W; w++) printf("   %8d", w);
    printf("\n");

    for (int h=0; h<H; h++) {
      printf ("H %2d ", h);
      for (int w=0; w<W; w++) {
        // data_out pixel position
        int addr_o = output_data_address(cout, h, w); 

        printf("  %9.2f", out_conv_cpu[addr_o]);
        //printf("CPU_MUL cout %2d   h %2d  w %2d   cout out_conv_cpu[%d] =  %8.2f\n", cout, h, w, addr_o, out_conv_cpu[addr_o]);
      }
      printf("\n");
    }
  }
  #endif
  

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
        int addr_o = output_data_address(o, h, w);
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
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
        // data_out pixel position
        int addr_o = output_data_address(cout, h, w);
        // bias operation
        out_conv_cpu[addr_o] += bias[cout];
      }
    }
  }



// let's store the generated values after direct convolution to compare with output of direct convolution values in kernel 
  #ifdef HLS_DEBUG
  for (int cout=0; cout<O_output; cout++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
        // data_out pixel position
        int addr_o = output_data_address(cout, h, w);
        // bias operation
        dbg_cpu_data_directconv_sum +=out_conv_cpu[addr_o];
        dbg_cpu_data_directconv_out[addr_o] = out_conv_cpu[addr_o];
        #ifdef PRINT_DATA_DIRECTCONV_CPU
        if (O_output < O_useful)printf("dbg_cpu_data_directconv_out[%2d] = %f\n", addr_o, dbg_cpu_data_directconv_out[addr_o]);
        #endif
      }
    }
  }
#endif



  // apply shift
  #ifdef API8_DATA_TYPE
  if (enable_shift) {
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          int addr_o = output_data_address(cout, h, w);
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
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          int addr_o = output_data_address(cout, h, w);
          if (out_conv_cpu[addr_o] < min_clip) out_conv_cpu[addr_o] = min_clip;
          else if (out_conv_cpu[addr_o] > max_clip) out_conv_cpu[addr_o] = max_clip;
        }
      }
    }
  }
  #endif

  // apply relu
  if (enable_relu){
    for (int cout=0; cout<O_output; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          int addr_o = output_data_address(cout, h, w);
          if (float(out_conv_cpu[addr_o]) < float(0)) out_relu_cpu[addr_o] = 0.f; else out_relu_cpu[addr_o] = out_conv_cpu[addr_o];
        }
      }
    }
  }

  // apply maxpooling or avgpooling
  if (enable_maxpooling) {
    for (int o=0; o<O_output; o++) {
      for (int h=0; h<H/2; h++) {
    	for (int w=0; w<W/2; w++) {
          int addr_out = output_data_address_div(o, h, w);
    	  data_type max_v = -(data_type)0xFFFFFFFFF;
    	  for (int kh=0; kh<2; kh++) {
    		for (int kw=0; kw<2; kw++) {
    		  int h_in = (h * 2) + kh;
    		  int w_in = (w * 2) + kw;
              int addr_in = input_data_address(o, h_in, w_in);
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
    for (int o=0; o<O_output; o++) {
      for (int h=0; h<H/2; h++) {
    	for (int w=0; w<W/2; w++) {
          int addr_out = output_data_address_div(o, h, w);
    	  data_type sum_v = 0;
    	  for (int kh=0; kh<2; kh++) {
    		for (int kw=0; kw<2; kw++) {
    		  int h_in = (h * 2) + kh;
    		  int w_in = (w * 2) + kw;
              int addr_in = input_data_address(o, h_in, w_in);
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

