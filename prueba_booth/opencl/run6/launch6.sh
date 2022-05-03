#!/bin/bash
cd $HOME
cd git/HLSinf
source scripts/setenv_2020.2.sh
cd ./prueba_booth/opencl/run6
make cleanall
make all TARGET=sw_emu CFG_FILE=cfg_u280.ini DEVICE=xilinx_u280_xdma_201920_3 
