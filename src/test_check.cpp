/*
 * check-related test functions
 */

#include "test_conv2D.h"

// check_result function. Checks output produced by the CPU and by the FPGA
int check_result(data_type *max_difference, int *num_elements_differ) {
  *num_elements_differ = 0;
  *max_difference = data_type(0);
  float epsilon = EPSILON_VALUE;

  for (int cout = 0; cout < O_output; cout++) {
    for (int h = 0; h < HO_final; h++) {
      for (int w = 0; w < WO_final; w++) {
	int addr_out = output_data_address(cout, h, w, HO_final, WO_final);
        data_type diff = data_type(fabs(float(cpu_out[addr_out]) - float(out[addr_out])));
        if (float(diff) > float(epsilon)) {
          (*num_elements_differ)++;
          if (*max_difference < diff) *max_difference = diff;
        }
      }
    }
  }
  return (*num_elements_differ != 0);
}
