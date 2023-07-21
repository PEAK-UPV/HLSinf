############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
############################################################
open_project HLSinf
set_top k_conv2D
add_files ../src/add.cpp -cflags "-DHLSINF_2_1"
add_files ../src/add_data.cpp -cflags "-DHLSINF_2_1"
add_files ../src/aggregate.cpp -cflags "-DHLSINF_2_1"
add_files ../src/batch_normalization.cpp -cflags "-DHLSINF_2_1"
add_files ../src/conv2D.h -cflags "-DHLSINF_2_1"
add_files ../src/cvt.cpp -cflags "-DHLSINF_2_1"
add_files ../src/direct_convolution.cpp -cflags "-DHLSINF_2_1"
add_files ../src/dws_convolution.cpp -cflags "-DHLSINF_2_1"
add_files ../src/filters_buffer.cpp -cflags "-DHLSINF_2_1"
add_files ../src/k_conv2D.cpp -cflags "-DHLSINF_2_1"
add_files ../src/mul.cpp -cflags "-DHLSINF_2_1"
add_files ../src/padding.cpp -cflags "-DHLSINF_2_1"
add_files ../src/pooling.cpp -cflags "-DHLSINF_2_1"
add_files ../src/read.cpp -cflags "-DHLSINF_2_1"
add_files ../src/relu.cpp -cflags "-DHLSINF_2_1"
add_files ../src/serialization.cpp -cflags "-DHLSINF_2_1"
add_files ../src/stm.cpp -cflags "-DHLSINF_2_1"
add_files ../src/winograd_convolution.cpp -cflags "-DHLSINF_2_1"
add_files ../src/write.cpp -cflags "-DHLSINF_2_1"
add_files -tb ../src/data_test.cpp -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_arguments.cpp -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_buffers.cpp -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_check.cpp -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_conv2D.cpp -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_conv2D.h -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_cpu.cpp -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_file.cpp -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_kernel.cpp -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
add_files -tb ../src/test_print.cpp -cflags "-DHLSINF_2_1 -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "AlveoU200" -flow_target vitis
set_part {xcu200-fsgd2104-2-e}
create_clock -period 3.33 -name default
config_interface -default_slave_interface s_axilite -m_axi_alignment_byte_size 64 -m_axi_latency 64 -m_axi_max_widen_bitwidth 512 -m_axi_offset slave
config_rtl -register_reset_num 3
source "./HLSinf/AlveoU200/directives.tcl"
csim_design -argv {/nfs/drodagu/HLSinf/src/input.data} -clean -O
csynth_design
cosim_design -wave_debug -trace_level all
export_design -format ip_catalog
