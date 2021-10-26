// ==============================================================
// RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Version: 2020.2
// Copyright (C) Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

module k_conv2D_pooling (
        H,
        W,
        enable_maxpooling,
        enable_avgpooling,
        out_batch_norm_dout,
        out_batch_norm_empty_n,
        out_batch_norm_read,
        out_pooling_din,
        out_pooling_full_n,
        out_pooling_write,
        ap_clk,
        ap_rst,
        enable_avgpooling_ap_vld,
        enable_maxpooling_ap_vld,
        W_ap_vld,
        H_ap_vld,
        ap_start,
        ap_done,
        ap_ready,
        ap_idle,
        ap_continue
);


input  [31:0] H;
input  [31:0] W;
input  [31:0] enable_maxpooling;
input  [31:0] enable_avgpooling;
input  [127:0] out_batch_norm_dout;
input   out_batch_norm_empty_n;
output   out_batch_norm_read;
output  [127:0] out_pooling_din;
input   out_pooling_full_n;
output   out_pooling_write;
input   ap_clk;
input   ap_rst;
input   enable_avgpooling_ap_vld;
input   enable_maxpooling_ap_vld;
input   W_ap_vld;
input   H_ap_vld;
input   ap_start;
output   ap_done;
output   ap_ready;
output   ap_idle;
input   ap_continue;

wire    pooling_Block_split7_proc_U0_ap_start;
wire    pooling_Block_split7_proc_U0_ap_done;
wire    pooling_Block_split7_proc_U0_ap_continue;
wire    pooling_Block_split7_proc_U0_ap_idle;
wire    pooling_Block_split7_proc_U0_ap_ready;
wire    pooling_Block_split7_proc_U0_start_out;
wire    pooling_Block_split7_proc_U0_start_write;
wire   [31:0] pooling_Block_split7_proc_U0_cond1_out_out_din;
wire    pooling_Block_split7_proc_U0_cond1_out_out_write;
wire   [31:0] pooling_Block_split7_proc_U0_HO_out_out_din;
wire    pooling_Block_split7_proc_U0_HO_out_out_write;
wire   [31:0] pooling_Block_split7_proc_U0_ap_return;
wire    ap_channel_done_enable_pooling_loc_i_channel;
wire    enable_pooling_loc_i_channel_full_n;
wire    pool_cvt_U0_ap_start;
wire    pool_cvt_U0_ap_done;
wire    pool_cvt_U0_ap_continue;
wire    pool_cvt_U0_ap_idle;
wire    pool_cvt_U0_ap_ready;
wire    pool_cvt_U0_out_batch_norm_read;
wire   [511:0] pool_cvt_U0_stream_pool_din;
wire    pool_cvt_U0_stream_pool_write;
wire    pool_pooling_U0_ap_start;
wire    pool_pooling_U0_ap_done;
wire    pool_pooling_U0_ap_continue;
wire    pool_pooling_U0_ap_idle;
wire    pool_pooling_U0_ap_ready;
wire    pool_pooling_U0_HO_loc_i_read;
wire    pool_pooling_U0_cond1_loc_i_read;
wire    pool_pooling_U0_stream_pool_read;
wire   [127:0] pool_pooling_U0_out_pooling_din;
wire    pool_pooling_U0_out_pooling_write;
wire    ap_sync_continue;
wire    cond1_loc_i_c_full_n;
wire   [31:0] cond1_loc_i_c_dout;
wire    cond1_loc_i_c_empty_n;
wire    HO_loc_i_c_full_n;
wire   [31:0] HO_loc_i_c_dout;
wire    HO_loc_i_c_empty_n;
wire   [31:0] enable_pooling_loc_i_channel_dout;
wire    enable_pooling_loc_i_channel_empty_n;
wire    stream_pool_full_n;
wire   [511:0] stream_pool_dout;
wire    stream_pool_empty_n;
wire    ap_sync_done;
wire    ap_sync_ready;
wire   [0:0] start_for_pool_pooling_U0_din;
wire    start_for_pool_pooling_U0_full_n;
wire   [0:0] start_for_pool_pooling_U0_dout;
wire    start_for_pool_pooling_U0_empty_n;
wire    pool_cvt_U0_start_full_n;
wire    pool_cvt_U0_start_write;
wire    pool_pooling_U0_start_full_n;
wire    pool_pooling_U0_start_write;

k_conv2D_pooling_Block_split7_proc pooling_Block_split7_proc_U0(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst),
    .ap_start(pooling_Block_split7_proc_U0_ap_start),
    .start_full_n(start_for_pool_pooling_U0_full_n),
    .ap_done(pooling_Block_split7_proc_U0_ap_done),
    .ap_continue(pooling_Block_split7_proc_U0_ap_continue),
    .ap_idle(pooling_Block_split7_proc_U0_ap_idle),
    .ap_ready(pooling_Block_split7_proc_U0_ap_ready),
    .start_out(pooling_Block_split7_proc_U0_start_out),
    .start_write(pooling_Block_split7_proc_U0_start_write),
    .enable_avgpooling(enable_avgpooling),
    .enable_maxpooling(enable_maxpooling),
    .W(W),
    .H(H),
    .cond1_out_out_din(pooling_Block_split7_proc_U0_cond1_out_out_din),
    .cond1_out_out_full_n(cond1_loc_i_c_full_n),
    .cond1_out_out_write(pooling_Block_split7_proc_U0_cond1_out_out_write),
    .HO_out_out_din(pooling_Block_split7_proc_U0_HO_out_out_din),
    .HO_out_out_full_n(HO_loc_i_c_full_n),
    .HO_out_out_write(pooling_Block_split7_proc_U0_HO_out_out_write),
    .ap_return(pooling_Block_split7_proc_U0_ap_return)
);

k_conv2D_pool_cvt pool_cvt_U0(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst),
    .ap_start(pool_cvt_U0_ap_start),
    .ap_done(pool_cvt_U0_ap_done),
    .ap_continue(pool_cvt_U0_ap_continue),
    .ap_idle(pool_cvt_U0_ap_idle),
    .ap_ready(pool_cvt_U0_ap_ready),
    .H(H),
    .W(W),
    .p_read(enable_pooling_loc_i_channel_dout),
    .out_batch_norm_dout(out_batch_norm_dout),
    .out_batch_norm_empty_n(out_batch_norm_empty_n),
    .out_batch_norm_read(pool_cvt_U0_out_batch_norm_read),
    .stream_pool_din(pool_cvt_U0_stream_pool_din),
    .stream_pool_full_n(stream_pool_full_n),
    .stream_pool_write(pool_cvt_U0_stream_pool_write)
);

k_conv2D_pool_pooling pool_pooling_U0(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst),
    .ap_start(pool_pooling_U0_ap_start),
    .ap_done(pool_pooling_U0_ap_done),
    .ap_continue(pool_pooling_U0_ap_continue),
    .ap_idle(pool_pooling_U0_ap_idle),
    .ap_ready(pool_pooling_U0_ap_ready),
    .HO_loc_i_dout(HO_loc_i_c_dout),
    .HO_loc_i_empty_n(HO_loc_i_c_empty_n),
    .HO_loc_i_read(pool_pooling_U0_HO_loc_i_read),
    .cond1_loc_i_dout(cond1_loc_i_c_dout),
    .cond1_loc_i_empty_n(cond1_loc_i_c_empty_n),
    .cond1_loc_i_read(pool_pooling_U0_cond1_loc_i_read),
    .enable_maxpooling(enable_maxpooling),
    .enable_avgpooling(enable_avgpooling),
    .stream_pool_dout(stream_pool_dout),
    .stream_pool_empty_n(stream_pool_empty_n),
    .stream_pool_read(pool_pooling_U0_stream_pool_read),
    .out_pooling_din(pool_pooling_U0_out_pooling_din),
    .out_pooling_full_n(out_pooling_full_n),
    .out_pooling_write(pool_pooling_U0_out_pooling_write)
);

k_conv2D_fifo_w32_d3_S cond1_loc_i_c_U(
    .clk(ap_clk),
    .reset(ap_rst),
    .if_read_ce(1'b1),
    .if_write_ce(1'b1),
    .if_din(pooling_Block_split7_proc_U0_cond1_out_out_din),
    .if_full_n(cond1_loc_i_c_full_n),
    .if_write(pooling_Block_split7_proc_U0_cond1_out_out_write),
    .if_dout(cond1_loc_i_c_dout),
    .if_empty_n(cond1_loc_i_c_empty_n),
    .if_read(pool_pooling_U0_cond1_loc_i_read)
);

k_conv2D_fifo_w32_d3_S HO_loc_i_c_U(
    .clk(ap_clk),
    .reset(ap_rst),
    .if_read_ce(1'b1),
    .if_write_ce(1'b1),
    .if_din(pooling_Block_split7_proc_U0_HO_out_out_din),
    .if_full_n(HO_loc_i_c_full_n),
    .if_write(pooling_Block_split7_proc_U0_HO_out_out_write),
    .if_dout(HO_loc_i_c_dout),
    .if_empty_n(HO_loc_i_c_empty_n),
    .if_read(pool_pooling_U0_HO_loc_i_read)
);

k_conv2D_fifo_w32_d2_S_x enable_pooling_loc_i_channel_U(
    .clk(ap_clk),
    .reset(ap_rst),
    .if_read_ce(1'b1),
    .if_write_ce(1'b1),
    .if_din(pooling_Block_split7_proc_U0_ap_return),
    .if_full_n(enable_pooling_loc_i_channel_full_n),
    .if_write(pooling_Block_split7_proc_U0_ap_done),
    .if_dout(enable_pooling_loc_i_channel_dout),
    .if_empty_n(enable_pooling_loc_i_channel_empty_n),
    .if_read(pool_cvt_U0_ap_ready)
);

k_conv2D_fifo_w512_d2_S stream_pool_U(
    .clk(ap_clk),
    .reset(ap_rst),
    .if_read_ce(1'b1),
    .if_write_ce(1'b1),
    .if_din(pool_cvt_U0_stream_pool_din),
    .if_full_n(stream_pool_full_n),
    .if_write(pool_cvt_U0_stream_pool_write),
    .if_dout(stream_pool_dout),
    .if_empty_n(stream_pool_empty_n),
    .if_read(pool_pooling_U0_stream_pool_read)
);

k_conv2D_start_for_pool_pooling_U0 start_for_pool_pooling_U0_U(
    .clk(ap_clk),
    .reset(ap_rst),
    .if_read_ce(1'b1),
    .if_write_ce(1'b1),
    .if_din(start_for_pool_pooling_U0_din),
    .if_full_n(start_for_pool_pooling_U0_full_n),
    .if_write(pooling_Block_split7_proc_U0_start_write),
    .if_dout(start_for_pool_pooling_U0_dout),
    .if_empty_n(start_for_pool_pooling_U0_empty_n),
    .if_read(pool_pooling_U0_ap_ready)
);

assign ap_channel_done_enable_pooling_loc_i_channel = pooling_Block_split7_proc_U0_ap_done;

assign ap_done = pool_pooling_U0_ap_done;

assign ap_idle = (pooling_Block_split7_proc_U0_ap_idle & pool_pooling_U0_ap_idle & pool_cvt_U0_ap_idle & (enable_pooling_loc_i_channel_empty_n ^ 1'b1));

assign ap_ready = pooling_Block_split7_proc_U0_ap_ready;

assign ap_sync_continue = ap_continue;

assign ap_sync_done = pool_pooling_U0_ap_done;

assign ap_sync_ready = pooling_Block_split7_proc_U0_ap_ready;

assign out_batch_norm_read = pool_cvt_U0_out_batch_norm_read;

assign out_pooling_din = pool_pooling_U0_out_pooling_din;

assign out_pooling_write = pool_pooling_U0_out_pooling_write;

assign pool_cvt_U0_ap_continue = 1'b1;

assign pool_cvt_U0_ap_start = enable_pooling_loc_i_channel_empty_n;

assign pool_cvt_U0_start_full_n = 1'b1;

assign pool_cvt_U0_start_write = 1'b0;

assign pool_pooling_U0_ap_continue = ap_continue;

assign pool_pooling_U0_ap_start = start_for_pool_pooling_U0_empty_n;

assign pool_pooling_U0_start_full_n = 1'b1;

assign pool_pooling_U0_start_write = 1'b0;

assign pooling_Block_split7_proc_U0_ap_continue = enable_pooling_loc_i_channel_full_n;

assign pooling_Block_split7_proc_U0_ap_start = ap_start;

assign start_for_pool_pooling_U0_din = 1'b1;

endmodule //k_conv2D_pooling
