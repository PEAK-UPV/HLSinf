XCLBIN implementations can be launched with SLURM. In this directory there are
several run directories which enable to launch an implementation process on each
directory. If you need more directories then simply create the directory and set
the proper links to files and directories (take as an example run1 folder).

In each run directory you will find an script file (launch.sh). This file runs
the implementation for a given configuration and target device. Change the file 
accordingly to your need.

Once the file is ready you need to launch the job on SLURM, for instance:

sbatch -N 1 -q compute --exclusive run1/launch.sh

you can track the job with

squeue

Once the implementation is done you will find all the associated files in the
run folder.

IMPORTANT NOTE: Pay attention to the configuration of the HLSinf accelerator.
The launch scripts will set the configuration. Therefore, you need to guarantee that
in the conv2D.h file no configuration is set (all defines for configurations must be
disabled).
