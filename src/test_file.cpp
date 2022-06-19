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
 int n = fscanf(fp, "ENABLE %d FROM_FILES %d CPU %d DET %d %dx%dx%dx%d PT %d PB %d PL %d PR %d SH %d SW %d RELU %d RELU_FACTOR %f STM %d MAXPOOL %d AVGPOOL %d BN %d BN_RELU %d BN_RELU_FACTOR %f ADD %d ADD_RELU %d SHIFT %d DIRECTION_SHIFT %d POS_SHIFT %d CLIP %d MINCLIP %d MAXCLIP %d UPSIZE %d UPSIZE_FACTOR %d\n",
            enable, from_files, cpu, &deterministic_input_values, &H, &W, &I, &O, &PT, &PB, &PL, &PR,
	    &SH, &SW, &enable_relu, &relu_factor, &enable_stm, &enable_maxpooling, &enable_avgpooling, &enable_batch_norm, &enable_batch_norm_relu, &batch_norm_relu_factor, &enable_add, &apply_add_relu, &enable_shift, &dir_shift, &pos_shift,
	    &enable_clipping, &min_clip, &max_clip, &enable_upsize, &upsize_factor);

 if (n != 32) return 1;

 // derived arguments
 if (!enable_upsize) upsize_factor = 1;
 rows = H;
 I_kernel = ((I + (CPI - 1)) / CPI) * CPI;
 O_kernel = ((O + (CPO - 1)) / CPO) * CPO;
 i_iter = (I + (CPI - 1)) / CPI;
 o_iter = (O + (CPO - 1)) / CPO;
 global_offset = 0;
 GI = I_kernel / CPI;
 GO = O_kernel / CPO;
 HO_conv = (H + PT + PB - KH + SH) / SH;  // HO = ceil((H + padding - (KH-1)) / SH)
 WO_conv = (W + PL + PR - KW + SW) / SW;  // WO = ceil((W + padding - (KW-1)) / SW)
 HO_relu = HO_conv;
 WO_relu = WO_conv;
 HO_stm = HO_conv;
 WO_stm = WO_conv;
 if (enable_maxpooling | enable_avgpooling) {
   HO_pool = HO_conv / 2;
   WO_pool = WO_conv / 2;
 } else {
   HO_pool = HO_conv;
   WO_pool = WO_conv;
 }
 HO_add = HO_pool;
 WO_add = WO_pool;
 HO_bn = HO_pool;
 WO_bn = WO_pool;

 HI_upsize = HO_pool;
 WI_upsize = WO_pool;
 HO_upsize = HO_pool * upsize_factor;
 WO_upsize = WO_pool * upsize_factor;

 HO_final = HO_upsize;
 WO_final = WO_upsize;

 I_input = ((I + (CPI - 1)) / CPI) * CPI;
 O_output = ((O + (CPO - 1)) / CPO) * CPO;
 
 return 0;
}

int close_test_file() {
  fclose(fp);
  return 0;
}
