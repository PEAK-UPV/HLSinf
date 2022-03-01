#!/bin/bash
cd $HOME
cd git/HLSinf
source scripts/setenv_2020.2.sh
cd opencl/run1
make cleanall
make all TARGET=sw_emu CFG_FILE=cfg_u200.ini DEVICE=xilinx_u200_xdma_201830_2 CONF=HLSINF_1_0
