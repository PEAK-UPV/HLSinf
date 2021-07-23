#!/bin/bash
cd $HOME
cd git/HLSinf
source scripts/setenv_2020.2.sh
cd opencl/run2
make cleanall
make all TARGET=hw CFG_FILE=cfg_u200.ini DEVICE=xilinx_u200_xdma_201830_2 CONF=CONF_ALVEO_U200_4x4_DIRECT_FP32
