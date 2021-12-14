/*
* HLSinf accelerator
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

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

int read_test_file(int *enable, int *from_files, int *cpu) {
 int n = fscanf(fp, "ENABLE %d FROM_FILES %d CPU %d DET %d %dx%dx%dx%d PT %d PB %d PL %d PR %d SH %d SW %d RELU %d RELU_FACTOR %f STM %d MAXPOOL %d AVGPOOL %d BN %d ADD %d SHIFT %d DIRECTION_SHIFT %d POS_SHIFT %d CLIP %d MINCLIP %d MAXCLIP %d UPSIZE %d\n",
            enable, from_files, cpu, &deterministic_input_values, &H, &W, &I, &O, &PT, &PB, &PL, &PR,
	    &SH, &SW, &enable_relu, &relu_factor, &enable_stm, &enable_maxpooling, &enable_avgpooling, &enable_batch_norm, &enable_add, &enable_shift, &dir_shift, &pos_shift,
	    &enable_clipping, &min_clip, &max_clip, &enable_upsize);

 if (n != 28) return 1;

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
 if (enable_maxpooling | enable_avgpooling) HO_final = HO / 2; else HO_final = HO;
 if (enable_maxpooling | enable_avgpooling) WO_final = WO / 2; else WO_final = WO;

 I_input = ((I + (CPI - 1)) / CPI) * CPI;
 O_output = ((O + (CPO - 1)) / CPO) * CPO;
 
 return 0;
}

int close_test_file() {
  fclose(fp);
  return 0;
}
