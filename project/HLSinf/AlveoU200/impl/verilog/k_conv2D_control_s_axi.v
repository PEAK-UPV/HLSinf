// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1ns/1ps
module k_conv2D_control_s_axi
#(parameter
    C_S_AXI_ADDR_WIDTH = 8,
    C_S_AXI_DATA_WIDTH = 32
)(
    input  wire                          ACLK,
    input  wire                          ARESET,
    input  wire                          ACLK_EN,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] AWADDR,
    input  wire                          AWVALID,
    output wire                          AWREADY,
    input  wire [C_S_AXI_DATA_WIDTH-1:0] WDATA,
    input  wire [C_S_AXI_DATA_WIDTH/8-1:0] WSTRB,
    input  wire                          WVALID,
    output wire                          WREADY,
    output wire [1:0]                    BRESP,
    output wire                          BVALID,
    input  wire                          BREADY,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] ARADDR,
    input  wire                          ARVALID,
    output wire                          ARREADY,
    output wire [C_S_AXI_DATA_WIDTH-1:0] RDATA,
    output wire [1:0]                    RRESP,
    output wire                          RVALID,
    input  wire                          RREADY,
    output wire                          interrupt,
    output wire [63:0]                   ptr_data,
    output wire [31:0]                   H,
    output wire [31:0]                   W,
    output wire [31:0]                   rows,
    output wire [31:0]                   I,
    output wire [31:0]                   O,
    output wire [31:0]                   I_ITER,
    output wire [31:0]                   o_iter_first,
    output wire [31:0]                   o_iter_last,
    output wire [31:0]                   enable_relu,
    output wire [63:0]                   ptr_kernel,
    output wire [63:0]                   ptr_bias,
    output wire [63:0]                   ptr_out,
    output wire [31:0]                   global_offset,
    output wire [31:0]                   enable_upper_padding,
    output wire [31:0]                   enable_lower_padding,
    output wire [31:0]                   enable_maxpooling,
    output wire [31:0]                   enable_avgpooling,
    output wire [31:0]                   enable_clipping,
    output wire [31:0]                   enable_shift,
    output wire [31:0]                   min_clip,
    output wire [31:0]                   max_clip,
    output wire [31:0]                   dir_shift,
    output wire [31:0]                   pos_shift,
    output wire                          ap_start,
    input  wire                          ap_done,
    input  wire                          ap_ready,
    output wire                          ap_continue,
    input  wire                          ap_idle
);
//------------------------Address Info-------------------
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 4  - ap_continue (Read/Write/SC)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - enable ap_done interrupt (Read/Write)
//        bit 1  - enable ap_ready interrupt (Read/Write)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - ap_done (COR/TOW)
//        bit 1  - ap_ready (COR/TOW)
//        others - reserved
// 0x10 : Data signal of ptr_data
//        bit 31~0 - ptr_data[31:0] (Read/Write)
// 0x14 : Data signal of ptr_data
//        bit 31~0 - ptr_data[63:32] (Read/Write)
// 0x18 : reserved
// 0x1c : Data signal of H
//        bit 31~0 - H[31:0] (Read/Write)
// 0x20 : reserved
// 0x24 : Data signal of W
//        bit 31~0 - W[31:0] (Read/Write)
// 0x28 : reserved
// 0x2c : Data signal of rows
//        bit 31~0 - rows[31:0] (Read/Write)
// 0x30 : reserved
// 0x34 : Data signal of I
//        bit 31~0 - I[31:0] (Read/Write)
// 0x38 : reserved
// 0x3c : Data signal of O
//        bit 31~0 - O[31:0] (Read/Write)
// 0x40 : reserved
// 0x44 : Data signal of I_ITER
//        bit 31~0 - I_ITER[31:0] (Read/Write)
// 0x48 : reserved
// 0x4c : Data signal of o_iter_first
//        bit 31~0 - o_iter_first[31:0] (Read/Write)
// 0x50 : reserved
// 0x54 : Data signal of o_iter_last
//        bit 31~0 - o_iter_last[31:0] (Read/Write)
// 0x58 : reserved
// 0x5c : Data signal of enable_relu
//        bit 31~0 - enable_relu[31:0] (Read/Write)
// 0x60 : reserved
// 0x64 : Data signal of ptr_kernel
//        bit 31~0 - ptr_kernel[31:0] (Read/Write)
// 0x68 : Data signal of ptr_kernel
//        bit 31~0 - ptr_kernel[63:32] (Read/Write)
// 0x6c : reserved
// 0x70 : Data signal of ptr_bias
//        bit 31~0 - ptr_bias[31:0] (Read/Write)
// 0x74 : Data signal of ptr_bias
//        bit 31~0 - ptr_bias[63:32] (Read/Write)
// 0x78 : reserved
// 0x7c : Data signal of ptr_out
//        bit 31~0 - ptr_out[31:0] (Read/Write)
// 0x80 : Data signal of ptr_out
//        bit 31~0 - ptr_out[63:32] (Read/Write)
// 0x84 : reserved
// 0x88 : Data signal of global_offset
//        bit 31~0 - global_offset[31:0] (Read/Write)
// 0x8c : reserved
// 0x90 : Data signal of enable_upper_padding
//        bit 31~0 - enable_upper_padding[31:0] (Read/Write)
// 0x94 : reserved
// 0x98 : Data signal of enable_lower_padding
//        bit 31~0 - enable_lower_padding[31:0] (Read/Write)
// 0x9c : reserved
// 0xa0 : Data signal of enable_maxpooling
//        bit 31~0 - enable_maxpooling[31:0] (Read/Write)
// 0xa4 : reserved
// 0xa8 : Data signal of enable_avgpooling
//        bit 31~0 - enable_avgpooling[31:0] (Read/Write)
// 0xac : reserved
// 0xb0 : Data signal of enable_clipping
//        bit 31~0 - enable_clipping[31:0] (Read/Write)
// 0xb4 : reserved
// 0xb8 : Data signal of enable_shift
//        bit 31~0 - enable_shift[31:0] (Read/Write)
// 0xbc : reserved
// 0xc0 : Data signal of min_clip
//        bit 31~0 - min_clip[31:0] (Read/Write)
// 0xc4 : reserved
// 0xc8 : Data signal of max_clip
//        bit 31~0 - max_clip[31:0] (Read/Write)
// 0xcc : reserved
// 0xd0 : Data signal of dir_shift
//        bit 31~0 - dir_shift[31:0] (Read/Write)
// 0xd4 : reserved
// 0xd8 : Data signal of pos_shift
//        bit 31~0 - pos_shift[31:0] (Read/Write)
// 0xdc : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

//------------------------Parameter----------------------
localparam
    ADDR_AP_CTRL                     = 8'h00,
    ADDR_GIE                         = 8'h04,
    ADDR_IER                         = 8'h08,
    ADDR_ISR                         = 8'h0c,
    ADDR_PTR_DATA_DATA_0             = 8'h10,
    ADDR_PTR_DATA_DATA_1             = 8'h14,
    ADDR_PTR_DATA_CTRL               = 8'h18,
    ADDR_H_DATA_0                    = 8'h1c,
    ADDR_H_CTRL                      = 8'h20,
    ADDR_W_DATA_0                    = 8'h24,
    ADDR_W_CTRL                      = 8'h28,
    ADDR_ROWS_DATA_0                 = 8'h2c,
    ADDR_ROWS_CTRL                   = 8'h30,
    ADDR_I_DATA_0                    = 8'h34,
    ADDR_I_CTRL                      = 8'h38,
    ADDR_O_DATA_0                    = 8'h3c,
    ADDR_O_CTRL                      = 8'h40,
    ADDR_I_ITER_DATA_0               = 8'h44,
    ADDR_I_ITER_CTRL                 = 8'h48,
    ADDR_O_ITER_FIRST_DATA_0         = 8'h4c,
    ADDR_O_ITER_FIRST_CTRL           = 8'h50,
    ADDR_O_ITER_LAST_DATA_0          = 8'h54,
    ADDR_O_ITER_LAST_CTRL            = 8'h58,
    ADDR_ENABLE_RELU_DATA_0          = 8'h5c,
    ADDR_ENABLE_RELU_CTRL            = 8'h60,
    ADDR_PTR_KERNEL_DATA_0           = 8'h64,
    ADDR_PTR_KERNEL_DATA_1           = 8'h68,
    ADDR_PTR_KERNEL_CTRL             = 8'h6c,
    ADDR_PTR_BIAS_DATA_0             = 8'h70,
    ADDR_PTR_BIAS_DATA_1             = 8'h74,
    ADDR_PTR_BIAS_CTRL               = 8'h78,
    ADDR_PTR_OUT_DATA_0              = 8'h7c,
    ADDR_PTR_OUT_DATA_1              = 8'h80,
    ADDR_PTR_OUT_CTRL                = 8'h84,
    ADDR_GLOBAL_OFFSET_DATA_0        = 8'h88,
    ADDR_GLOBAL_OFFSET_CTRL          = 8'h8c,
    ADDR_ENABLE_UPPER_PADDING_DATA_0 = 8'h90,
    ADDR_ENABLE_UPPER_PADDING_CTRL   = 8'h94,
    ADDR_ENABLE_LOWER_PADDING_DATA_0 = 8'h98,
    ADDR_ENABLE_LOWER_PADDING_CTRL   = 8'h9c,
    ADDR_ENABLE_MAXPOOLING_DATA_0    = 8'ha0,
    ADDR_ENABLE_MAXPOOLING_CTRL      = 8'ha4,
    ADDR_ENABLE_AVGPOOLING_DATA_0    = 8'ha8,
    ADDR_ENABLE_AVGPOOLING_CTRL      = 8'hac,
    ADDR_ENABLE_CLIPPING_DATA_0      = 8'hb0,
    ADDR_ENABLE_CLIPPING_CTRL        = 8'hb4,
    ADDR_ENABLE_SHIFT_DATA_0         = 8'hb8,
    ADDR_ENABLE_SHIFT_CTRL           = 8'hbc,
    ADDR_MIN_CLIP_DATA_0             = 8'hc0,
    ADDR_MIN_CLIP_CTRL               = 8'hc4,
    ADDR_MAX_CLIP_DATA_0             = 8'hc8,
    ADDR_MAX_CLIP_CTRL               = 8'hcc,
    ADDR_DIR_SHIFT_DATA_0            = 8'hd0,
    ADDR_DIR_SHIFT_CTRL              = 8'hd4,
    ADDR_POS_SHIFT_DATA_0            = 8'hd8,
    ADDR_POS_SHIFT_CTRL              = 8'hdc,
    WRIDLE                           = 2'd0,
    WRDATA                           = 2'd1,
    WRRESP                           = 2'd2,
    WRRESET                          = 2'd3,
    RDIDLE                           = 2'd0,
    RDDATA                           = 2'd1,
    RDRESET                          = 2'd2,
    ADDR_BITS                = 8;

//------------------------Local signal-------------------
    reg  [1:0]                    wstate = WRRESET;
    reg  [1:0]                    wnext;
    reg  [ADDR_BITS-1:0]          waddr;
    wire [C_S_AXI_DATA_WIDTH-1:0] wmask;
    wire                          aw_hs;
    wire                          w_hs;
    reg  [1:0]                    rstate = RDRESET;
    reg  [1:0]                    rnext;
    reg  [C_S_AXI_DATA_WIDTH-1:0] rdata;
    wire                          ar_hs;
    wire [ADDR_BITS-1:0]          raddr;
    // internal registers
    reg                           int_ap_idle;
    reg                           int_ap_continue;
    reg                           int_ap_ready;
    wire                          int_ap_done;
    reg                           int_ap_start = 1'b0;
    reg                           int_auto_restart = 1'b0;
    reg                           int_gie = 1'b0;
    reg  [1:0]                    int_ier = 2'b0;
    reg  [1:0]                    int_isr = 2'b0;
    reg  [63:0]                   int_ptr_data = 'b0;
    reg  [31:0]                   int_H = 'b0;
    reg  [31:0]                   int_W = 'b0;
    reg  [31:0]                   int_rows = 'b0;
    reg  [31:0]                   int_I = 'b0;
    reg  [31:0]                   int_O = 'b0;
    reg  [31:0]                   int_I_ITER = 'b0;
    reg  [31:0]                   int_o_iter_first = 'b0;
    reg  [31:0]                   int_o_iter_last = 'b0;
    reg  [31:0]                   int_enable_relu = 'b0;
    reg  [63:0]                   int_ptr_kernel = 'b0;
    reg  [63:0]                   int_ptr_bias = 'b0;
    reg  [63:0]                   int_ptr_out = 'b0;
    reg  [31:0]                   int_global_offset = 'b0;
    reg  [31:0]                   int_enable_upper_padding = 'b0;
    reg  [31:0]                   int_enable_lower_padding = 'b0;
    reg  [31:0]                   int_enable_maxpooling = 'b0;
    reg  [31:0]                   int_enable_avgpooling = 'b0;
    reg  [31:0]                   int_enable_clipping = 'b0;
    reg  [31:0]                   int_enable_shift = 'b0;
    reg  [31:0]                   int_min_clip = 'b0;
    reg  [31:0]                   int_max_clip = 'b0;
    reg  [31:0]                   int_dir_shift = 'b0;
    reg  [31:0]                   int_pos_shift = 'b0;

//------------------------Instantiation------------------


//------------------------AXI write fsm------------------
assign AWREADY = (wstate == WRIDLE);
assign WREADY  = (wstate == WRDATA);
assign BRESP   = 2'b00;  // OKAY
assign BVALID  = (wstate == WRRESP);
assign wmask   = { {8{WSTRB[3]}}, {8{WSTRB[2]}}, {8{WSTRB[1]}}, {8{WSTRB[0]}} };
assign aw_hs   = AWVALID & AWREADY;
assign w_hs    = WVALID & WREADY;

// wstate
always @(posedge ACLK) begin
    if (ARESET)
        wstate <= WRRESET;
    else if (ACLK_EN)
        wstate <= wnext;
end

// wnext
always @(*) begin
    case (wstate)
        WRIDLE:
            if (AWVALID)
                wnext = WRDATA;
            else
                wnext = WRIDLE;
        WRDATA:
            if (WVALID)
                wnext = WRRESP;
            else
                wnext = WRDATA;
        WRRESP:
            if (BREADY)
                wnext = WRIDLE;
            else
                wnext = WRRESP;
        default:
            wnext = WRIDLE;
    endcase
end

// waddr
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (aw_hs)
            waddr <= AWADDR[ADDR_BITS-1:0];
    end
end

//------------------------AXI read fsm-------------------
assign ARREADY = (rstate == RDIDLE);
assign RDATA   = rdata;
assign RRESP   = 2'b00;  // OKAY
assign RVALID  = (rstate == RDDATA);
assign ar_hs   = ARVALID & ARREADY;
assign raddr   = ARADDR[ADDR_BITS-1:0];

// rstate
always @(posedge ACLK) begin
    if (ARESET)
        rstate <= RDRESET;
    else if (ACLK_EN)
        rstate <= rnext;
end

// rnext
always @(*) begin
    case (rstate)
        RDIDLE:
            if (ARVALID)
                rnext = RDDATA;
            else
                rnext = RDIDLE;
        RDDATA:
            if (RREADY & RVALID)
                rnext = RDIDLE;
            else
                rnext = RDDATA;
        default:
            rnext = RDIDLE;
    endcase
end

// rdata
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (ar_hs) begin
            rdata <= 'b0;
            case (raddr)
                ADDR_AP_CTRL: begin
                    rdata[0] <= int_ap_start;
                    rdata[1] <= int_ap_done;
                    rdata[2] <= int_ap_idle;
                    rdata[3] <= int_ap_ready;
                    rdata[4] <= int_ap_continue;
                    rdata[7] <= int_auto_restart;
                end
                ADDR_GIE: begin
                    rdata <= int_gie;
                end
                ADDR_IER: begin
                    rdata <= int_ier;
                end
                ADDR_ISR: begin
                    rdata <= int_isr;
                end
                ADDR_PTR_DATA_DATA_0: begin
                    rdata <= int_ptr_data[31:0];
                end
                ADDR_PTR_DATA_DATA_1: begin
                    rdata <= int_ptr_data[63:32];
                end
                ADDR_H_DATA_0: begin
                    rdata <= int_H[31:0];
                end
                ADDR_W_DATA_0: begin
                    rdata <= int_W[31:0];
                end
                ADDR_ROWS_DATA_0: begin
                    rdata <= int_rows[31:0];
                end
                ADDR_I_DATA_0: begin
                    rdata <= int_I[31:0];
                end
                ADDR_O_DATA_0: begin
                    rdata <= int_O[31:0];
                end
                ADDR_I_ITER_DATA_0: begin
                    rdata <= int_I_ITER[31:0];
                end
                ADDR_O_ITER_FIRST_DATA_0: begin
                    rdata <= int_o_iter_first[31:0];
                end
                ADDR_O_ITER_LAST_DATA_0: begin
                    rdata <= int_o_iter_last[31:0];
                end
                ADDR_ENABLE_RELU_DATA_0: begin
                    rdata <= int_enable_relu[31:0];
                end
                ADDR_PTR_KERNEL_DATA_0: begin
                    rdata <= int_ptr_kernel[31:0];
                end
                ADDR_PTR_KERNEL_DATA_1: begin
                    rdata <= int_ptr_kernel[63:32];
                end
                ADDR_PTR_BIAS_DATA_0: begin
                    rdata <= int_ptr_bias[31:0];
                end
                ADDR_PTR_BIAS_DATA_1: begin
                    rdata <= int_ptr_bias[63:32];
                end
                ADDR_PTR_OUT_DATA_0: begin
                    rdata <= int_ptr_out[31:0];
                end
                ADDR_PTR_OUT_DATA_1: begin
                    rdata <= int_ptr_out[63:32];
                end
                ADDR_GLOBAL_OFFSET_DATA_0: begin
                    rdata <= int_global_offset[31:0];
                end
                ADDR_ENABLE_UPPER_PADDING_DATA_0: begin
                    rdata <= int_enable_upper_padding[31:0];
                end
                ADDR_ENABLE_LOWER_PADDING_DATA_0: begin
                    rdata <= int_enable_lower_padding[31:0];
                end
                ADDR_ENABLE_MAXPOOLING_DATA_0: begin
                    rdata <= int_enable_maxpooling[31:0];
                end
                ADDR_ENABLE_AVGPOOLING_DATA_0: begin
                    rdata <= int_enable_avgpooling[31:0];
                end
                ADDR_ENABLE_CLIPPING_DATA_0: begin
                    rdata <= int_enable_clipping[31:0];
                end
                ADDR_ENABLE_SHIFT_DATA_0: begin
                    rdata <= int_enable_shift[31:0];
                end
                ADDR_MIN_CLIP_DATA_0: begin
                    rdata <= int_min_clip[31:0];
                end
                ADDR_MAX_CLIP_DATA_0: begin
                    rdata <= int_max_clip[31:0];
                end
                ADDR_DIR_SHIFT_DATA_0: begin
                    rdata <= int_dir_shift[31:0];
                end
                ADDR_POS_SHIFT_DATA_0: begin
                    rdata <= int_pos_shift[31:0];
                end
            endcase
        end
    end
end


//------------------------Register logic-----------------
assign interrupt            = int_gie & (|int_isr);
assign ap_start             = int_ap_start;
assign int_ap_done          = ap_done;
assign ap_continue          = int_ap_continue;
assign ptr_data             = int_ptr_data;
assign H                    = int_H;
assign W                    = int_W;
assign rows                 = int_rows;
assign I                    = int_I;
assign O                    = int_O;
assign I_ITER               = int_I_ITER;
assign o_iter_first         = int_o_iter_first;
assign o_iter_last          = int_o_iter_last;
assign enable_relu          = int_enable_relu;
assign ptr_kernel           = int_ptr_kernel;
assign ptr_bias             = int_ptr_bias;
assign ptr_out              = int_ptr_out;
assign global_offset        = int_global_offset;
assign enable_upper_padding = int_enable_upper_padding;
assign enable_lower_padding = int_enable_lower_padding;
assign enable_maxpooling    = int_enable_maxpooling;
assign enable_avgpooling    = int_enable_avgpooling;
assign enable_clipping      = int_enable_clipping;
assign enable_shift         = int_enable_shift;
assign min_clip             = int_min_clip;
assign max_clip             = int_max_clip;
assign dir_shift            = int_dir_shift;
assign pos_shift            = int_pos_shift;
// int_ap_start
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_start <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_AP_CTRL && WSTRB[0] && WDATA[0])
            int_ap_start <= 1'b1;
        else if (ap_ready)
            int_ap_start <= int_auto_restart; // clear on handshake/auto restart
    end
end

// int_ap_idle
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_idle <= 1'b0;
    else if (ACLK_EN) begin
            int_ap_idle <= ap_idle;
    end
end

// int_ap_ready
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_ready <= 1'b0;
    else if (ACLK_EN) begin
            int_ap_ready <= ap_ready;
    end
end

// int_ap_continue
always @(posedge ACLK) begin
    if (ARESET)
        int_ap_continue <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_AP_CTRL && WSTRB[0] && WDATA[4])
            int_ap_continue <= 1'b1;
        else if (ap_done & ~int_ap_continue & int_auto_restart)
            int_ap_continue <= 1'b1; // auto restart
        else
            int_ap_continue <= 1'b0; // self clear
    end
end

// int_auto_restart
always @(posedge ACLK) begin
    if (ARESET)
        int_auto_restart <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_AP_CTRL && WSTRB[0])
            int_auto_restart <=  WDATA[7];
    end
end

// int_gie
always @(posedge ACLK) begin
    if (ARESET)
        int_gie <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_GIE && WSTRB[0])
            int_gie <= WDATA[0];
    end
end

// int_ier
always @(posedge ACLK) begin
    if (ARESET)
        int_ier <= 1'b0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_IER && WSTRB[0])
            int_ier <= WDATA[1:0];
    end
end

// int_isr[0]
always @(posedge ACLK) begin
    if (ARESET)
        int_isr[0] <= 1'b0;
    else if (ACLK_EN) begin
        if (int_ier[0] & ap_done)
            int_isr[0] <= 1'b1;
        else if (w_hs && waddr == ADDR_ISR && WSTRB[0])
            int_isr[0] <= int_isr[0] ^ WDATA[0]; // toggle on write
    end
end

// int_isr[1]
always @(posedge ACLK) begin
    if (ARESET)
        int_isr[1] <= 1'b0;
    else if (ACLK_EN) begin
        if (int_ier[1] & ap_ready)
            int_isr[1] <= 1'b1;
        else if (w_hs && waddr == ADDR_ISR && WSTRB[0])
            int_isr[1] <= int_isr[1] ^ WDATA[1]; // toggle on write
    end
end

// int_ptr_data[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_ptr_data[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_PTR_DATA_DATA_0)
            int_ptr_data[31:0] <= (WDATA[31:0] & wmask) | (int_ptr_data[31:0] & ~wmask);
    end
end

// int_ptr_data[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_ptr_data[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_PTR_DATA_DATA_1)
            int_ptr_data[63:32] <= (WDATA[31:0] & wmask) | (int_ptr_data[63:32] & ~wmask);
    end
end

// int_H[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_H[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_H_DATA_0)
            int_H[31:0] <= (WDATA[31:0] & wmask) | (int_H[31:0] & ~wmask);
    end
end

// int_W[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_W[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_W_DATA_0)
            int_W[31:0] <= (WDATA[31:0] & wmask) | (int_W[31:0] & ~wmask);
    end
end

// int_rows[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_rows[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ROWS_DATA_0)
            int_rows[31:0] <= (WDATA[31:0] & wmask) | (int_rows[31:0] & ~wmask);
    end
end

// int_I[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_I[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_I_DATA_0)
            int_I[31:0] <= (WDATA[31:0] & wmask) | (int_I[31:0] & ~wmask);
    end
end

// int_O[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_O[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_O_DATA_0)
            int_O[31:0] <= (WDATA[31:0] & wmask) | (int_O[31:0] & ~wmask);
    end
end

// int_I_ITER[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_I_ITER[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_I_ITER_DATA_0)
            int_I_ITER[31:0] <= (WDATA[31:0] & wmask) | (int_I_ITER[31:0] & ~wmask);
    end
end

// int_o_iter_first[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_o_iter_first[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_O_ITER_FIRST_DATA_0)
            int_o_iter_first[31:0] <= (WDATA[31:0] & wmask) | (int_o_iter_first[31:0] & ~wmask);
    end
end

// int_o_iter_last[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_o_iter_last[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_O_ITER_LAST_DATA_0)
            int_o_iter_last[31:0] <= (WDATA[31:0] & wmask) | (int_o_iter_last[31:0] & ~wmask);
    end
end

// int_enable_relu[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_enable_relu[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ENABLE_RELU_DATA_0)
            int_enable_relu[31:0] <= (WDATA[31:0] & wmask) | (int_enable_relu[31:0] & ~wmask);
    end
end

// int_ptr_kernel[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_ptr_kernel[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_PTR_KERNEL_DATA_0)
            int_ptr_kernel[31:0] <= (WDATA[31:0] & wmask) | (int_ptr_kernel[31:0] & ~wmask);
    end
end

// int_ptr_kernel[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_ptr_kernel[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_PTR_KERNEL_DATA_1)
            int_ptr_kernel[63:32] <= (WDATA[31:0] & wmask) | (int_ptr_kernel[63:32] & ~wmask);
    end
end

// int_ptr_bias[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_ptr_bias[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_PTR_BIAS_DATA_0)
            int_ptr_bias[31:0] <= (WDATA[31:0] & wmask) | (int_ptr_bias[31:0] & ~wmask);
    end
end

// int_ptr_bias[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_ptr_bias[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_PTR_BIAS_DATA_1)
            int_ptr_bias[63:32] <= (WDATA[31:0] & wmask) | (int_ptr_bias[63:32] & ~wmask);
    end
end

// int_ptr_out[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_ptr_out[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_PTR_OUT_DATA_0)
            int_ptr_out[31:0] <= (WDATA[31:0] & wmask) | (int_ptr_out[31:0] & ~wmask);
    end
end

// int_ptr_out[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_ptr_out[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_PTR_OUT_DATA_1)
            int_ptr_out[63:32] <= (WDATA[31:0] & wmask) | (int_ptr_out[63:32] & ~wmask);
    end
end

// int_global_offset[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_global_offset[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_GLOBAL_OFFSET_DATA_0)
            int_global_offset[31:0] <= (WDATA[31:0] & wmask) | (int_global_offset[31:0] & ~wmask);
    end
end

// int_enable_upper_padding[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_enable_upper_padding[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ENABLE_UPPER_PADDING_DATA_0)
            int_enable_upper_padding[31:0] <= (WDATA[31:0] & wmask) | (int_enable_upper_padding[31:0] & ~wmask);
    end
end

// int_enable_lower_padding[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_enable_lower_padding[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ENABLE_LOWER_PADDING_DATA_0)
            int_enable_lower_padding[31:0] <= (WDATA[31:0] & wmask) | (int_enable_lower_padding[31:0] & ~wmask);
    end
end

// int_enable_maxpooling[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_enable_maxpooling[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ENABLE_MAXPOOLING_DATA_0)
            int_enable_maxpooling[31:0] <= (WDATA[31:0] & wmask) | (int_enable_maxpooling[31:0] & ~wmask);
    end
end

// int_enable_avgpooling[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_enable_avgpooling[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ENABLE_AVGPOOLING_DATA_0)
            int_enable_avgpooling[31:0] <= (WDATA[31:0] & wmask) | (int_enable_avgpooling[31:0] & ~wmask);
    end
end

// int_enable_clipping[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_enable_clipping[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ENABLE_CLIPPING_DATA_0)
            int_enable_clipping[31:0] <= (WDATA[31:0] & wmask) | (int_enable_clipping[31:0] & ~wmask);
    end
end

// int_enable_shift[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_enable_shift[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ENABLE_SHIFT_DATA_0)
            int_enable_shift[31:0] <= (WDATA[31:0] & wmask) | (int_enable_shift[31:0] & ~wmask);
    end
end

// int_min_clip[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_min_clip[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_MIN_CLIP_DATA_0)
            int_min_clip[31:0] <= (WDATA[31:0] & wmask) | (int_min_clip[31:0] & ~wmask);
    end
end

// int_max_clip[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_max_clip[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_MAX_CLIP_DATA_0)
            int_max_clip[31:0] <= (WDATA[31:0] & wmask) | (int_max_clip[31:0] & ~wmask);
    end
end

// int_dir_shift[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_dir_shift[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_DIR_SHIFT_DATA_0)
            int_dir_shift[31:0] <= (WDATA[31:0] & wmask) | (int_dir_shift[31:0] & ~wmask);
    end
end

// int_pos_shift[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_pos_shift[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_POS_SHIFT_DATA_0)
            int_pos_shift[31:0] <= (WDATA[31:0] & wmask) | (int_pos_shift[31:0] & ~wmask);
    end
end


//------------------------Memory logic-------------------

endmodule
