

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
 int n = fscanf(fp, "ENABLE %d CPU %d %dx%dx%dx%d EUP %d ELP %d RELU %d MAXPOOL %d AVGPOOL %d SHIFT %d DIRECTION_SHIFT %d POS_SHIFT %d CLIP %d MINCLIP %d MAXCLIP %d\n",
            enable, cpu, &H, &W, &I, &O, &enable_upper_padding, &enable_lower_padding, &enable_relu, &enable_maxpooling, &enable_avgpooling, &enable_shift, &dir_shift, &pos_shift,
	    &enable_clipping, &min_clip, &max_clip);

 if (n != 17) return 1;

 // derived arguments
 rows = H;
 I_kernel = ((I + (CPI - 1)) / CPI) * CPI;
 O_kernel = ((O + (CPO - 1)) / CPO) * CPO;
 i_iter = (I + (CPI - 1)) / CPI;
 o_iter = (O + (CPO - 1)) / CPO;
 global_offset = 0;
 GI = I_kernel / CPI;
 GO = O_kernel / CPO;
 if (enable_maxpooling || enable_avgpooling) {HO = H / 2; WO = W / 2;} else {HO = H; WO = W;}

 return 0;
}

int close_test_file() {
  fclose(fp);
  return 0;
}