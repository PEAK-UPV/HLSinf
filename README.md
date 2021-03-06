# HLSinf
High-Level Synthesis inference accelerator for FPGAs

This project targets inference processes on FPGA systems. For this, HLS is used as the main programming language (C++ with HLS pragmas).

It is highly recommended to use vitis_hls (Vitis 2020.1) to work with the project. In order to launch the project you need to first set the environment variables and then launch `vitis_hls` and open the project (we asume you cloned the repo in HLSinf directory):
```
$ cd HLSinf
$ source scripts/setenv_2020.2.sh
```
You can use either VITIS IDE or the shell scripts. Scripts are provided for all the needed processes (C simulation, C synthesis, implementation).

Several configurations are available for the accelerator. You can try any of them or create new configurations. All configurations are listed and defined in `conv2D.h` file. Each configuration is named HLSINF_x_y where x is the version identifier and y is the subversion identifier. 

How to run C simulation from shell:
-----------------------------------
```
$ cd HLSinf
$ source scripts/setenv_2020.2.sh
$ cd project
$ vitis_hls -f ../scripts/alveo_u200_csim.tcl HLSINF_1_0
$ ./HLSinf/HLSINF_1_0/csim/build/csim.exe ../src/input.data
```

Note: The tcl script is launched with the proper configuration (HLSINF_...). See `conv2d.h` file for available configurations of the HLS accelerator.
Note: A new solution directory will be created with the solution implemented and reported
Note: no configuration must be enabled in conv2d.h file
Note: The solution can be opened with the Vitis HLS GUI
Note: A tcl script is used for each FPGA type

How to run C-Synthesis from shell:
-----------------------------------
```
$ cd HLSinf
$ source scripts/setenv_2020.2.sh
$ cd project
$ vitis_hls -f ../scripts/alveo_u200_csynth.tcl HLSINF_1_0
```
Note: The tcl script is launched with the proper configuration (HLSINF_...). See `conv2d.h` file for available configurations of the HLS accelerator.
Note: A new solution directory will be created with the solution implemented and reported
Note: no configuration must be enabled in conv2d.h file
Note: The solution can be opened with the Vitis HLS GUI
Note: A tcl script is used for each FPGA type

How to run C-Synthesis in a batch queuing system:
-------------------------------------------------
```
$ cd HLSinf
$ sbatch -N 1 scripts/launch_alveo_u200_csynth.sh
```
The script can be edited to indicate the target FPGA and the configuration to synthesize. You can create new scripts for each configuration you may need to launch. Each solution is reported in a different directory. The solutions can be opened with the VITIS HLS GUI.

How to create an xclbin binary
------------------------------

XCLBIN implementations can be launched with SLURM. In the HLSinf/opencl directory there are several run directories which enable to launch an implementation process on each directory. If you need more directories then simply create the directory and set
the proper links to files and directories (take as an example run1 folder).

In each run directory you will find an script file (`launch.sh`). This file runs the implementation for a given configuration and target device. Change the file accordingly to your need.

The possible configurations are: `sw_emu`, `hw_emu`, and `hw`

Once the file is ready you need to launch the job on SLURM, for instance:
```
$ sbatch -N 1 -q compute --exclusive run1/launch.sh
```
you can track the job with
```
$ squeue
```
Once the implementation is done you will find all the associated files in the run folder.

IMPORTANT NOTE: Pay attention to the configuration of the HLSinf accelerator. The launch scripts will set the configuration. Therefore, you need to guarantee that
in the conv2D.h file no configuration is set (all defines for configurations must be disabled).


