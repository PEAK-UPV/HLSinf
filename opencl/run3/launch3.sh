#!/bin/bash
cd $HOME
cd git/HLSinf
source scripts/setenv_2020.2.sh
cd opencl/run3
make cleanall
#make all TARGET=hw CFG_FILE=cfg_u280.ini DEVICE=xilinx_u280_xdma_201920_3 CONF=HLSINF_TEST4
make all TARGET=hw CFG_FILE=cfg_u200.ini DEVICE=xilinx_u200_xdma_201830_2 CONF=HLSINF_1_8
