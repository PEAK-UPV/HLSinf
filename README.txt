This document describes the methods that can be used to implement the HLSinf accelerator
------------------------------------------------------------------------------------------

The project has been configured with proper scripts in order to not use the Xilinx Vitis HLS GUI.
However, the GUI can be used if needed.

How to run C-Synthesis from shell:
-----------------------------------

$ cd $HOME/git/HLSinf
$ source scripts/setenv_2020.2.sh
$ cd project
$ vitis_hls -f ../scripts/alveo_u200_csynth.tcl CONF_ALVEO_U200_4x4_DIRECT_FP32

The tcl script is launched with the proper configuration (CONF_...). See k_conv2d.h file for
available configurations of the HLS accelerator.

Note: A new solution directory will be created with the solution implemented and reported
Note: no configuration must be enabled in conv2d.h file
Note: The solution can be opened withthe Vitis HLS GUI
Note: A tcl script is used for each FPGA type


How to run C-Synthesis in a batch queuing system:
-------------------------------------------------

$ cd $HOME/git/HLSinf
$ sbatch -N 1 scripts/launch_alveo_u200_csynth.sh

The sh script can be edited to indicate the target FPGA and the configuration to synthesize. You
can create new scripts for each configuration you may need to launch. Each solution is 
reported in a different directory. The solutions can be opened with the VITIS HLS GUI

