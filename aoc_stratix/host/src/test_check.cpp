/*
 * check-related test functions
 */

#include "test_conv2D.h"

// check_result function. Checks output produced by the CPU and by the FPGA


int check_result(data_type *max_difference, int *num_elements_differ) {
  *num_elements_differ = 0;
  *max_difference = data_type(0);
  float epsilon = EPSILON_VALUE;


  // let's hand-tune the EPSILON depending on the size of the operations due to the accumulated error for big operations on floats
  long tmp_mat = (H * W ) / 256;
  long tmp_chan = ( I_input * O_output) / 256;
  long tmp_mat_2 = tmp_mat > 2.0 ? tmp_mat : 2.0;
  long tmp_chan_2 = tmp_chan > 2.0 ? tmp_chan : 2.0;
  long tmp = tmp_mat_2 * tmp_chan_2;
  long tmp_2 = tmp > 64.0 ? 64.0 : tmp;
  float epsilon_factor = tmp_2;

  epsilon = EPSILON_VALUE * epsilon_factor;
  #ifdef DEBUG_CHECK
  printf("Manual Tuning EPSILON %f for float operations by factor: %f -> current epsilon value set to:%f \n", EPSILON_VALUE, epsilon_factor, epsilon);
  #endif
  epsilon_dataset_tuned = epsilon;

  double my_epsilon = std::numeric_limits<float>::epsilon();
  double ratio = (double)epsilon / (double)my_epsilon;
  double dsp_epsilon = 0.000000119209;
  double dsp_ratio =  (double)epsilon / (double)dsp_epsilon;

  if ((enable_maxpooling) || (enable_avgpooling)) {

    for (int cout=0; cout < O_output; cout++) {
      for (int h=0; h<H/2; h++) {
        for (int w=0; w<W/2; w++) {
          // data_out pixel position
          int addr_o = output_data_address_div(cout, h, w);
          data_type diff = data_type(fabs(float(out_cpu[addr_o]) - float(out[addr_o])));
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
          diff = data_type(fabs(float(out_cpu[addr_o]) - float(out[addr_o])));
          #ifdef DEBUG_CHECK
          #ifdef DEBUG_VERBOSE
          if(diff > epsilon) printf("o %2d  h %2d  w %2d  addr %3d  epsilon %2.2f   diff %2.2f   cpu %2.2f   mem %2.2f\n", cout, h, w, addr_o, epsilon, diff, out_conv_cpu[addr_o], out[addr_o]);
          #endif
          #endif
          if (float(diff) > float(epsilon)) {
            (*num_elements_differ)++;
            if (*max_difference < diff) *max_difference = diff;
          }
        }
      }
    }

    #ifdef DEBUG_CHECK
    printf(KCYN "data out(cpu) matrix\n" KNRM);
  	for (int cout=0; cout < O_output; cout++) {
      printf("Channel %d\n", cout);
      printf("W   ");
      for (int w=0; w<W; w++) printf("    %22d", w);
      printf("\n");

      for (int h=0; h<H; h++) {
        printf ("H %2d ", h);
        for (int w=0; w<W; w++) {
          // data_out pixel position
          int addr_o = output_data_address(cout, h, w);
          data_type diff;
          if (enable_relu) diff = data_type(fabs(float(out_relu_cpu[addr_o]) - float(out[addr_o])));
          else diff = fabs(float(out_cpu[addr_o]) - float(out[addr_o]));
          if(diff > epsilon) {
            printf(KRED "  %10.2f (%10.2f) " KNRM, out[addr_o], out_conv_cpu[addr_o]);
          }
          else {
            printf("  %10.2f (%10.2f) ", out[addr_o], out_conv_cpu[addr_o]);
          }
        }
        printf("\n");
      }
    }

    #endif
    return (*num_elements_differ != 0);

  }
}



/*
// this is a jm10 custom implementation for the  aoc migration process early stages
int check_result(data_type *max_difference, int *num_elements_differ) {
  *num_elements_differ = 0;
  *max_difference = data_type(0);
  float epsilon = EPSILON_VALUE;

  printf(KYEL "WARNING\n" KNRM);
  printf(" @check_result, custom implementation for the  aoc migration process early stages\n");

  printf("\n");
  printf(KCYN "DATA in - data matrix sent from HOST to FPGA\n" KNRM);

  for (int i=0; i<I_input; i++) {
    printf ("I channel  #%d\n", i);
    printf("W  ");
    for (int w=0; w<W; w++) printf(" %5d ", w);
    printf("\n");

    for (int h=0; h<H; h++) {
      printf ("H %2d ", h);
      for (int w=0; w<W; w++) {       
        int addr = input_data_address(i, h, w);
        printf("  %2.2f ", data_in[addr]);
      }
      printf("\n");
    }
  }
  printf("\n");
  printf(KCYN "DATA out - data matrix sent from FPGA to HOST\n" KNRM);

  for (int i=0; i<O_output; i++) {
    printf ("O channel  #%d\n", i);
    printf("W  ");
    for (int w=0; w<W; w++) printf(" %5d ", w);
    printf("\n");

    for (int h=0; h<H; h++) {
      printf ("H %2d ", h);
      for (int w=0; w<W; w++) {       
        int addr = input_data_address(i, h, w);
        printf("  %2.2f ", data_in[addr]);
      }
      printf("\n");
    }
  }



	for (int cout=0; cout < O_output; cout++) {
      for (int h=0; h<H; h++) {
        for (int w=0; w<W; w++) {
          // data_out pixel position
          int addr_o = output_data_address(cout, h, w);
          data_type diff;
          float expected_value = (data_in[addr_o] * AOC_VALUE_MUL) + AOC_VALUE_ADD; 
          diff = fabs(float(expected_value ) - float(out[addr_o]));
          #ifdef DEBUG_CHECK
          printf("o %2d  h %2d  w %2d  addr %3d  epsilon %2.2f   diff %2.2f   cpu %2.2f   mem %2.2f\n", cout, h, w, addr_o, epsilon, diff, expected_value, out[addr_o]);
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
*/
