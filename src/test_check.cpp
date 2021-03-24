/*
 * check-related test functions
 */

#include "test_conv2D.h"

// check_result function. Checks output produced by the CPU and by the FPGA
int check_result(data_type *max_difference, int *num_elements_differ) {
  *num_elements_differ = 0;
  *max_difference = data_type(0);
  float epsilon = EPSILON_VALUE;

  if ((enable_maxpooling) || (enable_avgpooling)) {

    for (int cout=0; cout < O; cout++) {
      for (int h=0; h<H/2; h++) {
        for (int w=0; w<W/2; w++) {
          // data_out pixel position
          int addr_o = (cout * W/2 * H/2) + (h * W/2) + w;
          data_type diff = data_type(fabs(float(out_pool_cpu[addr_o]) - float(out[addr_o])));
          if (float(diff) > float(epsilon)) {
            (*num_elements_differ)++;
            if (*max_difference < diff) *max_difference = diff;
          }
        }
      }
    }
    return (*num_elements_differ != 0);

  } else {

	for (int cout=0; cout < O; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          // data_out pixel position
          int addr_o = (cout * W * H) + (h * W) + w;
          data_type diff;
          if (enable_relu) diff = data_type(fabs(float(out_relu_cpu[addr_o]) - float(out[addr_o])));
          else diff = fabs(float(out_conv_cpu[addr_o]) - float(out[addr_o]));
          if (float(diff) > float(epsilon)) {
            (*num_elements_differ)++;
            if (*max_difference < diff) *max_difference = diff;
          }
        }
      }
    }
    return (*num_elements_differ != 0);

  }

}
