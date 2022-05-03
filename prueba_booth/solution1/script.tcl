############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project prueba_booth
set_top top
add_files prueba_booth/src/premults.cpp
add_files prueba_booth/src/top.h
add_files -tb prueba_booth/src/opencl.cpp
add_files -tb prueba_booth/src/tb.cpp -cflags "-Wno-unknown-pragmas"
add_files -tb prueba_booth/src/tb.h
open_solution "solution1" -flow_target vitis
set_part {xcu200-fsgd2104-2-e}
create_clock -period 3.33 -name default
config_rtl -register_reset_num 3
config_interface -m_axi_alignment_byte_size 64 -m_axi_latency 64 -m_axi_max_widen_bitwidth 512 -m_axi_offset slave
config_export -display_name top -format ip_catalog -ipname top -rtl verilog
source "./prueba_booth/solution1/directives.tcl"
csim_design -argv {70}
csynth_design
cosim_design -wave_debug -disable_deadlock_detection -trace_level all
export_design -flow impl -rtl verilog -format xo
