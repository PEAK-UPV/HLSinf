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

    for (int cout=0; cout < O_output; cout++) {
      for (int h=0; h<H/2; h++) {
        for (int w=0; w<W/2; w++) {
          // data_out pixel position
          int addr_o = output_data_address_div(cout, h, w);
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

	for (int cout=0; cout < O_output; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          // data_out pixel position
          int addr_o = output_data_address(cout, h, w);
          data_type diff;
          if (enable_relu) diff = data_type(fabs(float(out_relu_cpu[addr_o]) - float(out[addr_o])));
          else diff = fabs(float(out_conv_cpu[addr_o]) - float(out[addr_o]));
#ifdef DEBUG_CHECK
          printf("addr %3d  epsilon %2.2f   diff %2.2f   cpu %2.2f   mem %2.2f\n", addr_o, epsilon, diff, out_conv_cpu[addr_o], out[addr_o]);
#endif
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


