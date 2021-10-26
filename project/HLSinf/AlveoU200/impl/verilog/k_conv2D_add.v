// ==============================================================
// RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Version: 2020.2
// Copyright (C) Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

module k_conv2D_add (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
        ap_continue,
        ap_idle,
        ap_ready,
        H,
        W,
        I_ITER,
        out_read_bias_dout,
        out_read_bias_empty_n,
        out_read_bias_read,
        str_mul_add_dout,
        str_mul_add_empty_n,
        str_mul_add_read,
        out_conv_din,
        out_conv_full_n,
        out_conv_write
);

parameter    ap_ST_fsm_state1 = 5'd1;
parameter    ap_ST_fsm_state2 = 5'd2;
parameter    ap_ST_fsm_state3 = 5'd4;
parameter    ap_ST_fsm_pp0_stage0 = 5'd8;
parameter    ap_ST_fsm_state14 = 5'd16;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
input   ap_continue;
output   ap_idle;
output   ap_ready;
input  [31:0] H;
input  [31:0] W;
input  [31:0] I_ITER;
input  [127:0] out_read_bias_dout;
input   out_read_bias_empty_n;
output   out_read_bias_read;
input  [127:0] str_mul_add_dout;
input   str_mul_add_empty_n;
output   str_mul_add_read;
output  [127:0] out_conv_din;
input   out_conv_full_n;
output   out_conv_write;

reg ap_done;
reg ap_idle;
reg ap_ready;
reg out_read_bias_read;
reg str_mul_add_read;
reg out_conv_write;

reg    ap_done_reg;
(* fsm_encoding = "none" *) reg   [4:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
reg    buff_o_channels_ce0;
reg    buff_o_channels_we0;
wire   [15:0] buff_o_channels_address1;
reg    buff_o_channels_ce1;
wire   [127:0] buff_o_channels_q1;
reg    out_read_bias_blk_n;
wire    ap_CS_fsm_state2;
reg    str_mul_add_blk_n;
wire    ap_CS_fsm_pp0_stage0;
reg    ap_enable_reg_pp0_iter1;
wire    ap_block_pp0_stage0;
reg    out_conv_blk_n;
reg    ap_enable_reg_pp0_iter9;
reg   [0:0] cmp16_i_reg_406;
reg   [31:0] it_reg_149;
wire   [31:0] grp_fu_176_p2;
reg   [127:0] tmp_reg_382;
wire   [31:0] sub_i_fu_182_p2;
wire   [30:0] i_iter_1_fu_188_p2;
reg   [30:0] i_iter_1_reg_392;
wire    ap_CS_fsm_state3;
wire   [0:0] icmp_ln56_fu_198_p2;
wire   [0:0] cmp4_i_fu_203_p2;
reg   [0:0] cmp4_i_reg_401;
wire   [0:0] cmp16_i_fu_209_p2;
wire   [31:0] add_ln61_fu_214_p2;
reg    ap_enable_reg_pp0_iter0;
wire    ap_block_state4_pp0_stage0_iter0;
reg    ap_block_state5_pp0_stage0_iter1;
wire    ap_block_state6_pp0_stage0_iter2;
wire    ap_block_state7_pp0_stage0_iter3;
wire    ap_block_state8_pp0_stage0_iter4;
wire    ap_block_state9_pp0_stage0_iter5;
wire    ap_block_state10_pp0_stage0_iter6;
wire    ap_block_state11_pp0_stage0_iter7;
wire    ap_block_state12_pp0_stage0_iter8;
reg    ap_block_state13_pp0_stage0_iter9;
reg    ap_block_pp0_stage0_11001;
wire   [0:0] icmp_ln61_fu_220_p2;
reg   [15:0] buff_o_channels_addr_reg_419;
reg   [15:0] buff_o_channels_addr_reg_419_pp0_iter1_reg;
reg   [15:0] buff_o_channels_addr_reg_419_pp0_iter2_reg;
reg   [15:0] buff_o_channels_addr_reg_419_pp0_iter3_reg;
reg   [15:0] buff_o_channels_addr_reg_419_pp0_iter4_reg;
reg   [15:0] buff_o_channels_addr_reg_419_pp0_iter5_reg;
reg   [15:0] buff_o_channels_addr_reg_419_pp0_iter6_reg;
reg   [15:0] buff_o_channels_addr_reg_419_pp0_iter7_reg;
reg   [15:0] buff_o_channels_addr_reg_419_pp0_iter8_reg;
wire   [31:0] trunc_ln64_fu_230_p1;
reg   [31:0] trunc_ln64_reg_425;
reg   [31:0] trunc_ln64_1_reg_430;
reg   [31:0] trunc_ln64_2_reg_435;
reg   [31:0] trunc_ln64_3_reg_440;
reg   [31:0] trunc_ln36_2_reg_445;
reg   [31:0] trunc_ln36_3_reg_450;
reg   [31:0] trunc_ln36_4_reg_455;
wire   [31:0] trunc_ln36_fu_300_p1;
reg   [31:0] trunc_ln36_reg_460;
wire   [31:0] grp_fu_160_p2;
reg   [31:0] data_out_reg_505;
wire   [31:0] grp_fu_164_p2;
reg   [31:0] data_out_1_reg_510;
wire   [31:0] grp_fu_168_p2;
reg   [31:0] data_out_2_reg_515;
wire   [31:0] grp_fu_172_p2;
reg   [31:0] data_out_3_reg_520;
reg    ap_block_pp0_stage0_subdone;
reg    ap_condition_pp0_exit_iter0_state4;
reg    ap_enable_reg_pp0_iter2;
reg    ap_enable_reg_pp0_iter3;
reg    ap_enable_reg_pp0_iter4;
reg    ap_enable_reg_pp0_iter5;
reg    ap_enable_reg_pp0_iter6;
reg    ap_enable_reg_pp0_iter7;
reg    ap_enable_reg_pp0_iter8;
reg   [30:0] i_iter_reg_138;
wire    ap_CS_fsm_state14;
wire   [63:0] it_cast_i_fu_225_p1;
reg    ap_block_state1;
wire   [127:0] or_ln_fu_348_p5;
reg    ap_block_pp0_stage0_01001;
wire   [31:0] grp_fu_160_p0;
wire   [31:0] grp_fu_160_p1;
wire   [31:0] grp_fu_164_p0;
wire   [31:0] grp_fu_164_p1;
wire   [31:0] grp_fu_168_p0;
wire   [31:0] grp_fu_168_p1;
wire   [31:0] grp_fu_172_p0;
wire   [31:0] grp_fu_172_p1;
wire   [31:0] zext_ln56_fu_194_p1;
wire   [127:0] data_in_sroa_0_0_in_in_i_fu_264_p3;
wire   [31:0] bitcast_ln76_3_fu_345_p1;
wire   [31:0] bitcast_ln76_2_fu_342_p1;
wire   [31:0] bitcast_ln76_1_fu_339_p1;
wire   [31:0] bitcast_ln76_fu_336_p1;
reg    grp_fu_160_ce;
reg    grp_fu_164_ce;
reg    grp_fu_168_ce;
reg    grp_fu_172_ce;
reg    grp_fu_176_ce;
reg   [4:0] ap_NS_fsm;
reg    ap_idle_pp0;
wire    ap_enable_pp0;
wire    ap_ce_reg;

// power-on initialization
initial begin
#0 ap_done_reg = 1'b0;
#0 ap_CS_fsm = 5'd1;
#0 ap_enable_reg_pp0_iter1 = 1'b0;
#0 ap_enable_reg_pp0_iter9 = 1'b0;
#0 ap_enable_reg_pp0_iter0 = 1'b0;
#0 ap_enable_reg_pp0_iter2 = 1'b0;
#0 ap_enable_reg_pp0_iter3 = 1'b0;
#0 ap_enable_reg_pp0_iter4 = 1'b0;
#0 ap_enable_reg_pp0_iter5 = 1'b0;
#0 ap_enable_reg_pp0_iter6 = 1'b0;
#0 ap_enable_reg_pp0_iter7 = 1'b0;
#0 ap_enable_reg_pp0_iter8 = 1'b0;
end

k_conv2D_add_buff_o_channels #(
    .DataWidth( 128 ),
    .AddressRange( 65536 ),
    .AddressWidth( 16 ))
buff_o_channels_U(
    .clk(ap_clk),
    .reset(ap_rst),
    .address0(buff_o_channels_addr_reg_419_pp0_iter8_reg),
    .ce0(buff_o_channels_ce0),
    .we0(buff_o_channels_we0),
    .d0(or_ln_fu_348_p5),
    .address1(buff_o_channels_address1),
    .ce1(buff_o_channels_ce1),
    .q1(buff_o_channels_q1)
);

k_conv2D_fadd_32ns_32ns_32_7_full_dsp_1 #(
    .ID( 1 ),
    .NUM_STAGE( 7 ),
    .din0_WIDTH( 32 ),
    .din1_WIDTH( 32 ),
    .dout_WIDTH( 32 ))
fadd_32ns_32ns_32_7_full_dsp_1_U400(
    .clk(ap_clk),
    .reset(ap_rst),
    .din0(grp_fu_160_p0),
    .din1(grp_fu_160_p1),
    .ce(grp_fu_160_ce),
    .dout(grp_fu_160_p2)
);

k_conv2D_fadd_32ns_32ns_32_7_full_dsp_1 #(
    .ID( 1 ),
    .NUM_STAGE( 7 ),
    .din0_WIDTH( 32 ),
    .din1_WIDTH( 32 ),
    .dout_WIDTH( 32 ))
fadd_32ns_32ns_32_7_full_dsp_1_U401(
    .clk(ap_clk),
    .reset(ap_rst),
    .din0(grp_fu_164_p0),
    .din1(grp_fu_164_p1),
    .ce(grp_fu_164_ce),
    .dout(grp_fu_164_p2)
);

k_conv2D_fadd_32ns_32ns_32_7_full_dsp_1 #(
    .ID( 1 ),
    .NUM_STAGE( 7 ),
    .din0_WIDTH( 32 ),
    .din1_WIDTH( 32 ),
    .dout_WIDTH( 32 ))
fadd_32ns_32ns_32_7_full_dsp_1_U402(
    .clk(ap_clk),
    .reset(ap_rst),
    .din0(grp_fu_168_p0),
    .din1(grp_fu_168_p1),
    .ce(grp_fu_168_ce),
    .dout(grp_fu_168_p2)
);

k_conv2D_fadd_32ns_32ns_32_7_full_dsp_1 #(
    .ID( 1 ),
    .NUM_STAGE( 7 ),
    .din0_WIDTH( 32 ),
    .din1_WIDTH( 32 ),
    .dout_WIDTH( 32 ))
fadd_32ns_32ns_32_7_full_dsp_1_U403(
    .clk(ap_clk),
    .reset(ap_rst),
    .din0(grp_fu_172_p0),
    .din1(grp_fu_172_p1),
    .ce(grp_fu_172_ce),
    .dout(grp_fu_172_p2)
);

k_conv2D_mul_32s_32s_32_2_1 #(
    .ID( 1 ),
    .NUM_STAGE( 2 ),
    .din0_WIDTH( 32 ),
    .din1_WIDTH( 32 ),
    .dout_WIDTH( 32 ))
mul_32s_32s_32_2_1_U404(
    .clk(ap_clk),
    .reset(ap_rst),
    .din0(W),
    .din1(H),
    .ce(grp_fu_176_ce),
    .dout(grp_fu_176_p2)
);

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_CS_fsm <= ap_ST_fsm_state1;
    end else begin
        ap_CS_fsm <= ap_NS_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_done_reg <= 1'b0;
    end else begin
        if ((ap_continue == 1'b1)) begin
            ap_done_reg <= 1'b0;
        end else if (((1'b1 == ap_CS_fsm_state3) & (icmp_ln56_fu_198_p2 == 1'd0))) begin
            ap_done_reg <= 1'b1;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter0 <= 1'b0;
    end else begin
        if (((1'b0 == ap_block_pp0_stage0_subdone) & (1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_condition_pp0_exit_iter0_state4))) begin
            ap_enable_reg_pp0_iter0 <= 1'b0;
        end else if (((1'b1 == ap_CS_fsm_state3) & (icmp_ln56_fu_198_p2 == 1'd1))) begin
            ap_enable_reg_pp0_iter0 <= 1'b1;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter1 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            if ((1'b1 == ap_condition_pp0_exit_iter0_state4)) begin
                ap_enable_reg_pp0_iter1 <= (1'b1 ^ ap_condition_pp0_exit_iter0_state4);
            end else if ((1'b1 == 1'b1)) begin
                ap_enable_reg_pp0_iter1 <= ap_enable_reg_pp0_iter0;
            end
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter2 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter2 <= ap_enable_reg_pp0_iter1;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter3 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter3 <= ap_enable_reg_pp0_iter2;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter4 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter4 <= ap_enable_reg_pp0_iter3;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter5 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter5 <= ap_enable_reg_pp0_iter4;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter6 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter6 <= ap_enable_reg_pp0_iter5;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter7 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter7 <= ap_enable_reg_pp0_iter6;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter8 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter8 <= ap_enable_reg_pp0_iter7;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter9 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter9 <= ap_enable_reg_pp0_iter8;
        end else if (((1'b1 == ap_CS_fsm_state3) & (icmp_ln56_fu_198_p2 == 1'd1))) begin
            ap_enable_reg_pp0_iter9 <= 1'b0;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_CS_fsm_state14)) begin
        i_iter_reg_138 <= i_iter_1_reg_392;
    end else if (((out_read_bias_empty_n == 1'b1) & (1'b1 == ap_CS_fsm_state2))) begin
        i_iter_reg_138 <= 31'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0) & (icmp_ln61_fu_220_p2 == 1'd0) & (ap_enable_reg_pp0_iter0 == 1'b1))) begin
        it_reg_149 <= add_ln61_fu_214_p2;
    end else if (((1'b1 == ap_CS_fsm_state3) & (icmp_ln56_fu_198_p2 == 1'd1))) begin
        it_reg_149 <= 32'd0;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0) & (icmp_ln61_fu_220_p2 == 1'd0))) begin
        buff_o_channels_addr_reg_419 <= it_cast_i_fu_225_p1;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        buff_o_channels_addr_reg_419_pp0_iter1_reg <= buff_o_channels_addr_reg_419;
        trunc_ln36_2_reg_445 <= {{data_in_sroa_0_0_in_in_i_fu_264_p3[127:96]}};
        trunc_ln36_3_reg_450 <= {{data_in_sroa_0_0_in_in_i_fu_264_p3[95:64]}};
        trunc_ln36_4_reg_455 <= {{data_in_sroa_0_0_in_in_i_fu_264_p3[63:32]}};
        trunc_ln36_reg_460 <= trunc_ln36_fu_300_p1;
        trunc_ln64_1_reg_430 <= {{str_mul_add_dout[63:32]}};
        trunc_ln64_2_reg_435 <= {{str_mul_add_dout[95:64]}};
        trunc_ln64_3_reg_440 <= {{str_mul_add_dout[127:96]}};
        trunc_ln64_reg_425 <= trunc_ln64_fu_230_p1;
    end
end

always @ (posedge ap_clk) begin
    if ((1'b0 == ap_block_pp0_stage0_11001)) begin
        buff_o_channels_addr_reg_419_pp0_iter2_reg <= buff_o_channels_addr_reg_419_pp0_iter1_reg;
        buff_o_channels_addr_reg_419_pp0_iter3_reg <= buff_o_channels_addr_reg_419_pp0_iter2_reg;
        buff_o_channels_addr_reg_419_pp0_iter4_reg <= buff_o_channels_addr_reg_419_pp0_iter3_reg;
        buff_o_channels_addr_reg_419_pp0_iter5_reg <= buff_o_channels_addr_reg_419_pp0_iter4_reg;
        buff_o_channels_addr_reg_419_pp0_iter6_reg <= buff_o_channels_addr_reg_419_pp0_iter5_reg;
        buff_o_channels_addr_reg_419_pp0_iter7_reg <= buff_o_channels_addr_reg_419_pp0_iter6_reg;
        buff_o_channels_addr_reg_419_pp0_iter8_reg <= buff_o_channels_addr_reg_419_pp0_iter7_reg;
        data_out_1_reg_510 <= grp_fu_164_p2;
        data_out_2_reg_515 <= grp_fu_168_p2;
        data_out_3_reg_520 <= grp_fu_172_p2;
        data_out_reg_505 <= grp_fu_160_p2;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_state3) & (icmp_ln56_fu_198_p2 == 1'd1))) begin
        cmp16_i_reg_406 <= cmp16_i_fu_209_p2;
        cmp4_i_reg_401 <= cmp4_i_fu_203_p2;
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_CS_fsm_state3)) begin
        i_iter_1_reg_392 <= i_iter_1_fu_188_p2;
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_CS_fsm_state2)) begin
        tmp_reg_382 <= out_read_bias_dout;
    end
end

always @ (*) begin
    if ((icmp_ln61_fu_220_p2 == 1'd1)) begin
        ap_condition_pp0_exit_iter0_state4 = 1'b1;
    end else begin
        ap_condition_pp0_exit_iter0_state4 = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state3) & (icmp_ln56_fu_198_p2 == 1'd0))) begin
        ap_done = 1'b1;
    end else begin
        ap_done = ap_done_reg;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b0))) begin
        ap_idle = 1'b1;
    end else begin
        ap_idle = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter9 == 1'b0) & (ap_enable_reg_pp0_iter1 == 1'b0) & (ap_enable_reg_pp0_iter8 == 1'b0) & (ap_enable_reg_pp0_iter7 == 1'b0) & (ap_enable_reg_pp0_iter6 == 1'b0) & (ap_enable_reg_pp0_iter5 == 1'b0) & (ap_enable_reg_pp0_iter4 == 1'b0) & (ap_enable_reg_pp0_iter3 == 1'b0) & (ap_enable_reg_pp0_iter2 == 1'b0) & (ap_enable_reg_pp0_iter0 == 1'b0))) begin
        ap_idle_pp0 = 1'b1;
    end else begin
        ap_idle_pp0 = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state3) & (icmp_ln56_fu_198_p2 == 1'd0))) begin
        ap_ready = 1'b1;
    end else begin
        ap_ready = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter9 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        buff_o_channels_ce0 = 1'b1;
    end else begin
        buff_o_channels_ce0 = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_enable_reg_pp0_iter0 == 1'b1))) begin
        buff_o_channels_ce1 = 1'b1;
    end else begin
        buff_o_channels_ce1 = 1'b0;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter9 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        buff_o_channels_we0 = 1'b1;
    end else begin
        buff_o_channels_we0 = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        grp_fu_160_ce = 1'b1;
    end else begin
        grp_fu_160_ce = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        grp_fu_164_ce = 1'b1;
    end else begin
        grp_fu_164_ce = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        grp_fu_168_ce = 1'b1;
    end else begin
        grp_fu_168_ce = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        grp_fu_172_ce = 1'b1;
    end else begin
        grp_fu_172_ce = 1'b0;
    end
end

always @ (*) begin
    if (((~((ap_done_reg == 1'b1) | (ap_start == 1'b0)) & (1'b1 == ap_CS_fsm_state1)) | ((out_read_bias_empty_n == 1'b1) & (1'b1 == ap_CS_fsm_state2)))) begin
        grp_fu_176_ce = 1'b1;
    end else begin
        grp_fu_176_ce = 1'b0;
    end
end

always @ (*) begin
    if (((cmp16_i_reg_406 == 1'd1) & (ap_enable_reg_pp0_iter9 == 1'b1) & (1'b0 == ap_block_pp0_stage0))) begin
        out_conv_blk_n = out_conv_full_n;
    end else begin
        out_conv_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((cmp16_i_reg_406 == 1'd1) & (ap_enable_reg_pp0_iter9 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001))) begin
        out_conv_write = 1'b1;
    end else begin
        out_conv_write = 1'b0;
    end
end

always @ (*) begin
    if ((1'b1 == ap_CS_fsm_state2)) begin
        out_read_bias_blk_n = out_read_bias_empty_n;
    end else begin
        out_read_bias_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((out_read_bias_empty_n == 1'b1) & (1'b1 == ap_CS_fsm_state2))) begin
        out_read_bias_read = 1'b1;
    end else begin
        out_read_bias_read = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        str_mul_add_blk_n = str_mul_add_empty_n;
    end else begin
        str_mul_add_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        str_mul_add_read = 1'b1;
    end else begin
        str_mul_add_read = 1'b0;
    end
end

always @ (*) begin
    case (ap_CS_fsm)
        ap_ST_fsm_state1 : begin
            if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0)) & (1'b1 == ap_CS_fsm_state1))) begin
                ap_NS_fsm = ap_ST_fsm_state2;
            end else begin
                ap_NS_fsm = ap_ST_fsm_state1;
            end
        end
        ap_ST_fsm_state2 : begin
            if (((out_read_bias_empty_n == 1'b1) & (1'b1 == ap_CS_fsm_state2))) begin
                ap_NS_fsm = ap_ST_fsm_state3;
            end else begin
                ap_NS_fsm = ap_ST_fsm_state2;
            end
        end
        ap_ST_fsm_state3 : begin
            if (((1'b1 == ap_CS_fsm_state3) & (icmp_ln56_fu_198_p2 == 1'd0))) begin
                ap_NS_fsm = ap_ST_fsm_state1;
            end else begin
                ap_NS_fsm = ap_ST_fsm_pp0_stage0;
            end
        end
        ap_ST_fsm_pp0_stage0 : begin
            if ((~((1'b0 == ap_block_pp0_stage0_subdone) & (ap_enable_reg_pp0_iter1 == 1'b0) & (icmp_ln61_fu_220_p2 == 1'd1) & (ap_enable_reg_pp0_iter0 == 1'b1)) & ~((ap_enable_reg_pp0_iter9 == 1'b1) & (1'b0 == ap_block_pp0_stage0_subdone) & (ap_enable_reg_pp0_iter8 == 1'b0)))) begin
                ap_NS_fsm = ap_ST_fsm_pp0_stage0;
            end else if ((((ap_enable_reg_pp0_iter9 == 1'b1) & (1'b0 == ap_block_pp0_stage0_subdone) & (ap_enable_reg_pp0_iter8 == 1'b0)) | ((1'b0 == ap_block_pp0_stage0_subdone) & (ap_enable_reg_pp0_iter1 == 1'b0) & (icmp_ln61_fu_220_p2 == 1'd1) & (ap_enable_reg_pp0_iter0 == 1'b1)))) begin
                ap_NS_fsm = ap_ST_fsm_state14;
            end else begin
                ap_NS_fsm = ap_ST_fsm_pp0_stage0;
            end
        end
        ap_ST_fsm_state14 : begin
            ap_NS_fsm = ap_ST_fsm_state3;
        end
        default : begin
            ap_NS_fsm = 'bx;
        end
    endcase
end

assign add_ln61_fu_214_p2 = (it_reg_149 + 32'd1);

assign ap_CS_fsm_pp0_stage0 = ap_CS_fsm[32'd3];

assign ap_CS_fsm_state1 = ap_CS_fsm[32'd0];

assign ap_CS_fsm_state14 = ap_CS_fsm[32'd4];

assign ap_CS_fsm_state2 = ap_CS_fsm[32'd1];

assign ap_CS_fsm_state3 = ap_CS_fsm[32'd2];

assign ap_block_pp0_stage0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_pp0_stage0_01001 = (((cmp16_i_reg_406 == 1'd1) & (ap_enable_reg_pp0_iter9 == 1'b1) & (out_conv_full_n == 1'b0)) | ((ap_enable_reg_pp0_iter1 == 1'b1) & (str_mul_add_empty_n == 1'b0)));
end

always @ (*) begin
    ap_block_pp0_stage0_11001 = (((cmp16_i_reg_406 == 1'd1) & (ap_enable_reg_pp0_iter9 == 1'b1) & (out_conv_full_n == 1'b0)) | ((ap_enable_reg_pp0_iter1 == 1'b1) & (str_mul_add_empty_n == 1'b0)));
end

always @ (*) begin
    ap_block_pp0_stage0_subdone = (((cmp16_i_reg_406 == 1'd1) & (ap_enable_reg_pp0_iter9 == 1'b1) & (out_conv_full_n == 1'b0)) | ((ap_enable_reg_pp0_iter1 == 1'b1) & (str_mul_add_empty_n == 1'b0)));
end

always @ (*) begin
    ap_block_state1 = ((ap_done_reg == 1'b1) | (ap_start == 1'b0));
end

assign ap_block_state10_pp0_stage0_iter6 = ~(1'b1 == 1'b1);

assign ap_block_state11_pp0_stage0_iter7 = ~(1'b1 == 1'b1);

assign ap_block_state12_pp0_stage0_iter8 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_state13_pp0_stage0_iter9 = ((cmp16_i_reg_406 == 1'd1) & (out_conv_full_n == 1'b0));
end

assign ap_block_state4_pp0_stage0_iter0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_state5_pp0_stage0_iter1 = (str_mul_add_empty_n == 1'b0);
end

assign ap_block_state6_pp0_stage0_iter2 = ~(1'b1 == 1'b1);

assign ap_block_state7_pp0_stage0_iter3 = ~(1'b1 == 1'b1);

assign ap_block_state8_pp0_stage0_iter4 = ~(1'b1 == 1'b1);

assign ap_block_state9_pp0_stage0_iter5 = ~(1'b1 == 1'b1);

assign ap_enable_pp0 = (ap_idle_pp0 ^ 1'b1);

assign bitcast_ln76_1_fu_339_p1 = data_out_1_reg_510;

assign bitcast_ln76_2_fu_342_p1 = data_out_2_reg_515;

assign bitcast_ln76_3_fu_345_p1 = data_out_3_reg_520;

assign bitcast_ln76_fu_336_p1 = data_out_reg_505;

assign buff_o_channels_address1 = it_cast_i_fu_225_p1;

assign cmp16_i_fu_209_p2 = ((zext_ln56_fu_194_p1 == sub_i_fu_182_p2) ? 1'b1 : 1'b0);

assign cmp4_i_fu_203_p2 = ((i_iter_reg_138 == 31'd0) ? 1'b1 : 1'b0);

assign data_in_sroa_0_0_in_in_i_fu_264_p3 = ((cmp4_i_reg_401[0:0] == 1'b1) ? tmp_reg_382 : buff_o_channels_q1);

assign grp_fu_160_p0 = trunc_ln36_reg_460;

assign grp_fu_160_p1 = trunc_ln64_reg_425;

assign grp_fu_164_p0 = trunc_ln36_4_reg_455;

assign grp_fu_164_p1 = trunc_ln64_1_reg_430;

assign grp_fu_168_p0 = trunc_ln36_3_reg_450;

assign grp_fu_168_p1 = trunc_ln64_2_reg_435;

assign grp_fu_172_p0 = trunc_ln36_2_reg_445;

assign grp_fu_172_p1 = trunc_ln64_3_reg_440;

assign i_iter_1_fu_188_p2 = (i_iter_reg_138 + 31'd1);

assign icmp_ln56_fu_198_p2 = (($signed(zext_ln56_fu_194_p1) < $signed(I_ITER)) ? 1'b1 : 1'b0);

assign icmp_ln61_fu_220_p2 = ((it_reg_149 == grp_fu_176_p2) ? 1'b1 : 1'b0);

assign it_cast_i_fu_225_p1 = it_reg_149;

assign or_ln_fu_348_p5 = {{{{bitcast_ln76_3_fu_345_p1}, {bitcast_ln76_2_fu_342_p1}}, {bitcast_ln76_1_fu_339_p1}}, {bitcast_ln76_fu_336_p1}};

assign out_conv_din = or_ln_fu_348_p5;

assign sub_i_fu_182_p2 = ($signed(I_ITER) + $signed(32'd4294967295));

assign trunc_ln36_fu_300_p1 = data_in_sroa_0_0_in_in_i_fu_264_p3[31:0];

assign trunc_ln64_fu_230_p1 = str_mul_add_dout[31:0];

assign zext_ln56_fu_194_p1 = i_iter_reg_138;

endmodule //k_conv2D_add
