############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_interface -mode s_axilite -bundle control "k_conv2D" ptr_data
set_directive_interface -mode s_axilite -bundle control "k_conv2D" H
set_directive_interface -mode s_axilite -bundle control "k_conv2D" W
set_directive_interface -mode s_axilite -bundle control "k_conv2D" rows
set_directive_interface -mode s_axilite -bundle control "k_conv2D" I
set_directive_interface -mode s_axilite -bundle control "k_conv2D" O
set_directive_interface -mode s_axilite -bundle control "k_conv2D" I_ITER
set_directive_interface -mode s_axilite -bundle control "k_conv2D" enable_relu
set_directive_interface -mode s_axilite -bundle control "k_conv2D" ptr_dw_kernel
set_directive_interface -mode s_axilite -bundle control "k_conv2D" ptr_pw_kernel
set_directive_interface -mode s_axilite -bundle control "k_conv2D" ptr_bias
set_directive_interface -mode s_axilite -bundle control "k_conv2D" ptr_out
set_directive_interface -mode s_axilite -bundle control "k_conv2D" global_offset
set_directive_interface -mode s_axilite -bundle control "k_conv2D" enable_upper_padding
set_directive_interface -mode s_axilite -bundle control "k_conv2D" enable_lower_padding
set_directive_interface -mode s_axilite -bundle control "k_conv2D" enable_maxpooling
set_directive_interface -mode s_axilite -bundle control "k_conv2D" enable_avgpooling
set_directive_interface -mode s_axilite -bundle control "k_conv2D" enable_clipping
set_directive_interface -mode s_axilite -bundle control "k_conv2D" enable_shift
set_directive_interface -mode s_axilite -bundle control "k_conv2D" min_clip
set_directive_interface -mode s_axilite -bundle control "k_conv2D" max_clip
set_directive_interface -mode s_axilite -bundle control "k_conv2D" dir_shift
set_directive_interface -mode s_axilite -bundle control "k_conv2D" pos_shift
set_directive_interface -mode s_axilite -bundle control "k_conv2D"
set_directive_array_partition -type complete -dim 1 "block_generate" bx.Base
set_directive_array_partition -type complete -dim 1 "serialize_and_filter/serialize_and_filter_label1" bx.Base
set_directive_interface -mode s_axilite -bundle control "k_conv2D" o_iter_first
set_directive_interface -mode s_axilite -bundle control "k_conv2D" o_iter_last
set_directive_pipeline "add/add_load_data_it_loop"
set_directive_resource -core RAM_S2P_BRAM "pool_cvt" buffer0
set_directive_resource -core RAM_S2P_BRAM "pool_cvt" buffer1
set_directive_resource -core RAM_T2P_BRAM "add" buff_o_channels
set_directive_resource -core RAM_T2P_BRAM "input_buffer" buffer
set_directive_dependence -variable pin_col -type inter -direction RAW -dependent true "pool_cvt/cvt_pooling_iterations"
set_directive_dependence -variable pin_row -type inter -direction RAW -dependent true "pool_cvt/cvt_pooling_iterations"
set_directive_stable "k_conv2D" O_ITER
set_directive_interface -mode s_axilite -bundle control "k_conv2D" O_ITER
set_directive_dependence -variable pin_col -type inter -direction RAW -dependent true "cvt/cvt_loop"
set_directive_dependence -variable pin_row -type inter -direction RAW -dependent true "cvt/cvt_loop"
set_directive_dependence -variable row0_buffer_write -type inter -direction RAW -dependent true "cvt/cvt_loop"
set_directive_dependence -variable row1_buffer_write -type inter -direction RAW -dependent true "cvt/cvt_loop"
set_directive_dependence -variable row2_buffer_write -type inter -direction RAW -dependent true "cvt/cvt_loop"
set_directive_dependence -variable w -type inter -direction RAW -dependent true "padding/padding_loop"
set_directive_dependence -variable h -type inter -direction RAW -dependent true "padding/padding_loop"
set_directive_dependence -variable iter_load_kernel -type inter -direction RAW -dependent true "mul/mul_loop_1"
set_directive_dependence -variable kernel_dw.pixel -type inter -dependent false "dws_read_dw_kernel"
set_directive_dependence -variable addr -type inter -direction RAW -dependent true "dws_read_dw_kernel/dw_read_kernel_loop_k"
set_directive_data_pack "k_conv2D" ptr_data
set_directive_data_pack "k_conv2D" ptr_out
