#!/bin/bash 
cd $HOME
cd git/HLSinf
source scripts/setenv_2020.2.sh
cd opencl/run3
make cleanall
make all TARGET=hw CFG_FILE=cfg_u280.ini DEVICE=xilinx_u280_xdma_201920_3 CONF=CONF_ALVEO_U280_16x16_DWS_API8