############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project HLSinf
set_top k_conv2D
add_files ../../src/write.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/winograd_convolution.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/serialization.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/relu.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/read.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/pooling.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/padding.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/mul.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/k_conv2D.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/join_split.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/dws_convolution.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/direct_convolution.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/cvt.cpp -cflags "-D__VIVADO_HLS__"
add_files ../../src/conv2D.h -cflags "-D__VIVADO_HLS__"
add_files ../../src/add.cpp -cflags "-D__VIVADO_HLS__"
add_files -tb ../../src/data_test.cpp
add_files -tb ../../src/test_arguments.cpp
add_files -tb ../../src/test_buffers.cpp
add_files -tb ../../src/test_check.cpp
add_files -tb ../../src/test_conv2D.cpp
add_files -tb ../../src/test_conv2D.h
add_files -tb ../../src/test_cpu.cpp
add_files -tb ../../src/test_file.cpp
add_files -tb ../../src/test_kernel.cpp
add_files -tb ../../src/test_print.cpp
open_solution "MANGO"
set_part {xcku115-flvb2104-2-e} -tool vivado
create_clock -period 3.33 -name default
config_sdx -target none
config_export -description {2D Convolution HLS IP core} -display_name k_conv2D_dws_gihwcpi_fp_cpi8_cpo8 -format ip_catalog -library gap-acc -rtl verilog -taxonomy /Accelerators -use_netlist none -vendor upv-gap -version 4.0 -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0
config_rtl -encoding onehot -kernel_profile=0 -module_auto_prefix=0 -mult_keep_attribute=0 -reset control -reset_async=0 -reset_level high -verbose=0
config_interface -clock_enable=0 -expose_global=0 -m_axi_addr64 -m_axi_offset off -register_io off -trim_dangling_port=0
set_clock_uncertainty 12.5%
source "./HLSinf/MANGO/directives.tcl"
csim_design -clean
csynth_design
cosim_design -trace_level port
export_design -rtl verilog -format ip_catalog -description "2D Convolution HLS IP core" -vendor "upv-gap" -library "gap-acc" -version "4.0" -display_name "k_conv2D_dws_gihwcpi_fp_cpi8_cpo8" -taxonomy "/Accelerators"
