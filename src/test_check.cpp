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
      for (int h=0; h<HO/2; h++) {
        for (int w=0; w<WO/2; w++) {
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
      for (int h=0; h<HO_final; h++) {
        for (int w=0; w<WO_final; w++) {
          // data_out pixel position
          int addr_o = output_data_address(cout, h, w, HO_final, WO_final);
          data_type diff;
          if (enable_relu) diff = data_type(fabs(float(out_relu_cpu[addr_o]) - float(out[addr_o])));
          else if(enable_stm) {
        	   	   if (enable_add) diff = data_type(fabs(float(out_add_cpu[addr_o]) - float(out[addr_o])));
        	   	   else diff = data_type(fabs(float(out_stm_cpu[addr_o]) - float(out[addr_o])));
          	  }
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
