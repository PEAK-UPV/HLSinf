

/*
 * Tests. File-related functions
 */

#include "test_conv2D.h"

//-----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int open_test_file() {
 fp=fopen(input_data_file,"r");
 if (fp==NULL) {
   printf("error, input data file not found\n");
   return 1;
 }
 return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int read_test_file(int *enable,int* from_file, int *cpu) {
  int n = fscanf(fp, "ENABLE %d FROM_FILES %d CPU %d DET %d %dx%dx%dx%d EUP %d ELP %d RELU %d MAXPOOL %d AVGPOOL %d BN %d ADD %d SHIFT %d DIRECTION_SHIFT %d POS_SHIFT %d CLIP %d MINCLIP %d MAXCLIP %d\n",
      enable, from_file, cpu, &deterministic_input_values, &H, &W, &I, &O, &enable_upper_padding, &enable_lower_padding, &enable_relu, &enable_maxpooling, &enable_avgpooling, &enable_batch_norm, &enable_add,
      &enable_shift, &dir_shift, &pos_shift, &enable_clipping, &min_clip, &max_clip);

  if (n != 21) {
    printf(KYEL "unexpected number of parameters: %d\n" KNRM, n);
    return 1;
  }

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

  #ifdef IHW_DATA_FORMAT
  I_input = I;
  O_output = O;
  #endif
  #ifdef GIHWCPI_DATA_FORMAT
  I_input = ((I + (CPI - 1)) / CPI) * CPI;
  O_output = ((O + (CPO - 1)) / CPO) * CPO;
  //printf(KCYN "  JM10 test_file.cpp  O_output = ((O + (CPO - 1)) / CPO) * CPO =   ((%d + (%d - 1)) / %d) * %d = %d\n\n" KNRM, O,CPO, CPO,CPO, O_output);
  #endif

  return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int close_test_file() {
  fclose(fp);
  return 0;
}


//*********************************************************************************************************************
// end of file test_file.cpp
//*********************************************************************************************************************

