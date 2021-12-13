/*
 * check-related test functions
 */

#include "test_conv2D.h"

// check_result function. Checks output produced by the CPU and by the FPGA
int check_result(dout_t *max_difference, int *num_elements_differ) {
  *num_elements_differ = 0;
  *max_difference = dout_t(0);
  float epsilon = EPSILON_VALUE;

  int rows = enable_upsize ? HO_final * 2 : HO_final;
  int cols = enable_upsize ? WO_final * 2 : WO_final;


  for (int cout = 0; cout < O_output; cout++) {
    for (int h = 0; h < rows; h++) {
      for (int w = 0; w < cols; w++) {
	      int addr_out = output_data_address(cout, h, w, rows, cols);
        dout_t diff = dout_t(fabs(float(cpu_out[addr_out]) - float(out[addr_out])));
        if (float(diff) > float(epsilon)) {
          (*num_elements_differ)++;
//	  printf("difference at cout %d h %d w %d %6.4f cpu %6.4f fpga\n", cout, h, w, float(cpu_out[addr_out]), float(out[addr_out]));
          if (*max_difference < diff) *max_difference = diff;
        }
      }
    }
  }
  return (*num_elements_differ != 0);
}
