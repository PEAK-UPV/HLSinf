
  // bfilter0: 64x3x3x3 = 1728 bits = 216 bytes = 54 ints
  ap_uint<27> bfilter0[64];

  float scale0[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float B0[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float mean0[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float var0[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

  // bfilter1: 64x64x3x3 = 36864 bits = 4608 bytes = 1152 ints
  ap_uint<576> bfilter1[64];

  float scale1[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float B1[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float mean1[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float var1[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

  // bfilter2: 128x64x3x3 = 73728 bits = 9126 bytes = 2304 ints
  ap_uint<576> bfilter2[128];

  // bfilter3: 128x128x3x3
  ap_uint<1152> bfilter3[128];

  // bfilter4: 256x128x3x3
  ap_uint<1152> bfilter4a[128];
  ap_uint<1152> bfilter4b[128];

  // bfilter5: 256x256x3x3
  ap_uint<1152> bfilter5aa[128];
  ap_uint<1152> bfilter5ab[128];
  ap_uint<1152> bfilter5ba[128];
  ap_uint<1152> bfilter5bb[128];
  
  float scale2[128];
  float B2[128];
  float mean2[128];
  float var2[128];

  float scale3[128]; 
  float B3[128]; 
  float mean3[128]; 
  float var3[128]; 
  
  float scale4a[128];
  float B4a[128];
  float mean4a[128];
  float var4a[128];
  
  float scale4b[128];
  float B4b[128];
  float mean4b[128];
  float var4b[128];
  
  float scale5a[128];
  float B5a[128];
  float mean5a[128];
  float var5a[128];

  float scale5b[128];
  float B5b[128];
  float mean5b[128];
  float var5b[128];

  float scale6a[128];
  float B6a[128];
  float mean6a[128];
  float var6a[128];

  float scale6b[128];
  float B6b[128];
  float mean6b[128];
  float var6b[128];

  float scale6c[128];
  float B6c[128];
  float mean6c[128];
  float var6c[128];

  float scale6d[128];
  float B6d[128];
  float mean6d[128];
  float var6d[128];

  float scale7a[128];
  float B7a[128];
  float mean7a[128];
  float var7a[128];

  float scale7b[128];
  float B7b[128];
  float mean7b[128];
  float var7b[128];

  float scale7c[128];
  float B7c[128];
  float mean7c[128];
  float var7c[128];

  float scale7d[128];
  float B7d[128];
  float mean7d[128];
  float var7d[128];

  ap_uint<128> weights0_1a[128];
  ap_uint<128> weights0_1b[128];
  ap_uint<128> weights0_1c[128];
  ap_uint<128> weights0_1d[128];
  ap_uint<128> weights0_2a[128];
  ap_uint<128> weights0_2b[128];
  ap_uint<128> weights0_2c[128];
  ap_uint<128> weights0_2d[128];
  
  ap_uint<128> weights1_1a[128];
  ap_uint<128> weights1_1b[128];
  ap_uint<128> weights1_1c[128];
  ap_uint<128> weights1_1d[128];
  ap_uint<128> weights1_2a[128];
  ap_uint<128> weights1_2b[128];
  ap_uint<128> weights1_2c[128];
  ap_uint<128> weights1_2d[128];
  ap_uint<128> weights1_3a[128];
  ap_uint<128> weights1_3b[128];
  ap_uint<128> weights1_3c[128];
  ap_uint<128> weights1_3d[128];
  ap_uint<128> weights1_4a[128];
  ap_uint<128> weights1_4b[128];
  ap_uint<128> weights1_4c[128];
  ap_uint<128> weights1_4d[128];

  ap_uint<128> weights2_1a[10];
  ap_uint<128> weights2_2a[10];
  ap_uint<128> weights2_3a[10];
  ap_uint<128> weights2_4a[10];
