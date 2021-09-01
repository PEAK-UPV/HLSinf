############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project HLSinf
set_top k_net
add_files src/net.cpp -cflags "-D [lindex $argv 2]"
add_files -tb src/test_net.cpp -cflags "-D [lindex $argv 2]"
open_solution "[lindex $argv 2]" -flow_target vitis
set_part {xcu200-fsgd2104-2-e}
create_clock -period 3.33 -name default
config_interface -default_slave_interface s_axilite -m_axi_alignment_byte_size 64 -m_axi_latency 64 -m_axi_max_widen_bitwidth 512 -m_axi_offset slave
config_rtl -register_reset_num 3
csim_design -O -setup
exit
