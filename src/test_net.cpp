// --------------------------------------------------------------------------------------------------------------
// HLSinf kernels
// Version: 1.0
// copyright (c) 2020, Universidad Politècnica de València (UPV), GAP research group
// Date: March 2021
// Authors: GAP Research Group (UPV)
//     José Flich Cardo
//     Jorge García Martínez
//     Izan Catalán Gallarch
//     Carles Hernández Luz
//
// contact: jflich@disca.upv.es
// All rights reserved

//
// test_conv2D. Test for the conv2D kernel
//
//  Data formats:
//
//  - weights (kernel)   : GO x GI x CPO x CPI x KH x KW
//  - bias               : O
//  - data_in            : I x H x W
//  - data_out           : O x H x W
//
//  GO = ceil(O / CPO), GI = ceil(I / CPI)
//
// The kernel must have at least CPI channels and CPO channels, filled with zeroes if needed

// This test works both for C simulation and for Opencel
// In OpenCL the OPENCL_TEST define should be defined, if not
// C simulation is assumed instead

#include <stdlib.h>
#include <string.h>
#include "net.h"

int main(int argc, char **argv) {

 int H = 32;
 int W = 32;
 int C = 3;

 // The data in memory is aligned to 16 bytes, so we add space for a fourth channel!
 float *src;
 posix_memalign((void **)&src, 4096, H * W * (C+1) * sizeof(float));
 memset(src, 0, sizeof(float) * H * W * (C+1));
 for (int h=0; h<H; h++) {
   for (int w=0; w<W; w++) {
     for (int c=0; c<3; c++) {
       int addr = (h * W * 4) + (w * 4) + c;
       src[addr] = random() % 256;
     }
   }
 }
 float *dst;
 posix_memalign((void **)&dst, 4096, H * W * (C+1) * sizeof(float));
 memset(dst, 0, sizeof(float) * H * W * C);

 k_net(src, dst);

 free(src);
 free(dst);
 // Return 0 if outputs are correct
 return 0;
}
