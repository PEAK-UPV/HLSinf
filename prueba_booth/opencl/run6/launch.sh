#!/bin/bash

cd $HOME
cd HLSinf
source scripts/setenv_2020.2.sh
cd opencl/run7
./test_conv2D ../../src/input.data build_dir.hw.xilinx_u280_xdma_201920_3/kernel_conv2D.xclbin

