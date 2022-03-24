/*
 * check-related test functions
 */

#include "test_conv2D.h"

// check_result function. Checks output produced by the CPU and by the FPGA


int check_result(data_type *max_difference, int *num_elements_differ, int from_file) {
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


  printf("Check results %s\n", from_file?" three way host - fpga - eddl library output": "host vs fpga");

  if ((enable_maxpooling) || (enable_avgpooling)) {

    for (int cout=0; cout < O_output; cout++) {
      for (int h=0; h<H/2; h++) {
        for (int w=0; w<W/2; w++) {
          // data_out pixel position
          int addr_o = output_data_address_div(cout, h, w);
          data_type diff  = (data_type)(fabs(float(out_cpu[addr_o]) - float(out[addr_o])));
          data_type diff2 = from_file?(data_type)(fabs(float(out_cpu_from_file[addr_o]) - float(out[addr_o]))): diff;
          if ((float(diff) > float(epsilon)) || (float(diff2) > float(epsilon))){
            #ifdef DEBUG_CHECK
            #ifdef DEBUG_VERBOSE
            printf("o %2d  h %2d  w %2d  addr %3d  epsilon %4.3f   diff %4.3f   diff2 %4.3f   cpu %4.3f   fpga %4.3f   file %4.3f\n", cout, h, w, addr_o, epsilon, diff, diff2, out_cpu[addr_o], out[addr_o],out_cpu_from_file[addr_o]);
            #endif
            #endif
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
          data_type diff2 = from_file?(data_type)(fabs(float(out_cpu_from_file[addr_o]) - float(out[addr_o]))): diff;
          if ((float(diff) > float(epsilon)) || (float(diff2) > float(epsilon))){
            #ifdef DEBUG_CHECK
            #ifdef DEBUG_VERBOSE
            printf("o %2d  h %2d  w %2d  addr %3d  epsilon %4.3f   diff %4.3f   diff2 %4.3f   cpu %4.3f   fpga %4.3f   file %4.3f\n", cout, h, w, addr_o, epsilon, diff, diff2, out_cpu[addr_o], out[addr_o],out_cpu_from_file[addr_o]);
            #endif
            #endif
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
          int       addr_o = output_data_address(cout, h, w);
          data_type diff   = fabs(float(out_cpu[addr_o]) - float(out[addr_o]));

          if(diff > epsilon) {
            printf(KRED "  %10.3f (%10.3f) " KNRM, out[addr_o], out_cpu[addr_o]);
          }
          else {
            printf("  %10.3f (%10.3f) ", out[addr_o], out_cpu[addr_o]);
          }
        }
        printf("\n");
      }
    }

    #endif
    return (*num_elements_differ != 0);

  }
}

