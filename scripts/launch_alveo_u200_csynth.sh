#!/bin/bash
cd $HOME
cd git/HLSinf
source scripts/setenv_2020.2.sh
cd project
vitis_hls -f ../scripts/alveo_u200_csynth.tcl CONF_ALVEO_U200_4x4_DIRECT_FP32
