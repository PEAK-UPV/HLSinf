#!/bin/bash
cd $HOME
cd git/HLSinf
source scripts/setenv_2020.2.sh
cd project
vitis_hls -f ../scripts/alveo_u200_csynth.tcl HLSINF_1_0
