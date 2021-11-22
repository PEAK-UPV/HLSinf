############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project DataAugmentation
set_top k_dataAugmentation
add_files ../../src/dataaugmentation/da.h
add_files ../../src/dataaugmentation/k_dataAgumentation.cpp
add_files -tb ../../src/dataaugmentation/test_dataAugmentation.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "MANGO"
set_part {xcku115-flvb2104-2-e} -tool vivado
create_clock -period 3.33 -name default
config_rtl -encoding onehot -kernel_profile=0 -module_auto_prefix=0 -mult_keep_attribute=0 -reset control -reset_async=0 -reset_level high -verbose=0
config_interface   -m_axi_addr64 -m_axi_offset off -register_io off -trim_dangling_port=0
config_export -description {DataAugmentation HLS IP core} -display_name k_data_augmentation_cu4_pe64 -format ip_catalog -library gap-acc -rtl verilog -taxonomy /datamanip -use_netlist none -vendor upv-gap -version 1.0 -vivado_impl_strategy default -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0 -vivado_synth_design_args {-directive sdx_optimization_effort_high} -vivado_synth_strategy default
set_clock_uncertainty 12.5%
source "./DataAugmentation/MANGO/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog -description "DataAugmentation HLS IP core" -vendor "upv-gap" -library "gap-acc" -display_name "k_data_augmentation_cu4_pe64" -taxonomy "/datamanip"
