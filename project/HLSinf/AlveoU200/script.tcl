############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project HLSinf
set_top k_conv2D
add_files ../src/write.cpp
add_files ../src/winograd_convolution.cpp
add_files ../src/serialization.cpp
add_files ../src/relu.cpp
add_files ../src/read.cpp
add_files ../src/pooling.cpp
add_files ../src/padding.cpp
add_files ../src/mul.cpp
add_files ../src/lib_hlsm.cpp
add_files ../src/k_conv2D.cpp
add_files ../src/join_split.cpp
add_files ../src/dws_convolution.cpp
add_files ../src/direct_convolution.cpp
add_files ../src/cvt.cpp
add_files ../src/conv2D.h
add_files ../src/batch_normalization.cpp
add_files ../src/add.cpp
add_files -tb ../src/test_print.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_kernel.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_file.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_cpu.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_conv2D.h -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_conv2D.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_check.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_buffers.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_arguments.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/data_test.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "AlveoU200" -flow_target vitis
set_part {xcu200-fsgd2104-2-e}
create_clock -period 3.33 -name default
config_interface -default_slave_interface s_axilite -m_axi_alignment_byte_size 64 -m_axi_latency 64 -m_axi_max_widen_bitwidth 512 -m_axi_offset slave
config_rtl -register_reset_num 3
source "./HLSinf/AlveoU200/directives.tcl"
csim_design -setup
csynth_design
cosim_design
export_design -format ip_catalog
