

/*
 * Tests. File-related functions
 */

#include "test_conv2D.h"

int open_test_file() {
 fp=fopen(input_data_file,"r");
 if (fp==NULL) {
   printf("error, input data file not found\n");
   return 1;
 }
 return 0;
}

int read_test_file(int *enable, int *cpu) {
 int n = fscanf(fp, "ENABLE %d CPU %d DET %d %dx%dx%dx%d PT %d PB %d PL %d PR %d SH %d SW %d RELU %d RELU_FACTOR %f STM %d MAXPOOL %d AVGPOOL %d ADD %d SHIFT %d DIRECTION_SHIFT %d POS_SHIFT %d CLIP %d MINCLIP %d MAXCLIP %d\n",
            enable, cpu, &deterministic_input_values, &H, &W, &I, &O, &PT, &PB, &PL, &PR,
	    &SH, &SW, &enable_relu, &relu_factor, &enable_stm, &enable_maxpooling, &enable_avgpooling, &enable_add, &enable_shift, &dir_shift, &pos_shift,
	    &enable_clipping, &min_clip, &max_clip);

 if (n != 25) return 1;

 // derived arguments
 rows = H;
 I_kernel = ((I + (CPI - 1)) / CPI) * CPI;
 O_kernel = ((O + (CPO - 1)) / CPO) * CPO;
 i_iter = (I + (CPI - 1)) / CPI;
 o_iter = (O + (CPO - 1)) / CPO;
 global_offset = 0;
 GI = I_kernel / CPI;
 GO = O_kernel / CPO;
 HO = (H + PT + PB - KH + SH) / SH;  // HO = ceil((H + padding - (KH-1)) / SH)
 WO = (W + PL + PR - KW + SW) / SW;  // WO = ceil((W + padding - (KW-1)) / SW)

 #ifdef IHW_DATA_FORMAT
 I_input = I;
 O_output = O;
 #endif
 #ifdef GIHWCPI_DATA_FORMAT
 I_input = ((I + (CPI - 1)) / CPI) * CPI;
 O_output = ((O + (CPO - 1)) / CPO) * CPO;
 #endif

 return 0;
}

int close_test_file() {
  fclose(fp);
  return 0;
}
