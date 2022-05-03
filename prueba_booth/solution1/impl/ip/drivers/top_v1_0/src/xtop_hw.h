// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
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
// 0x10 : Data signal of x0
//        bit 31~0 - x0[31:0] (Read/Write)
// 0x14 : Data signal of x0
//        bit 31~0 - x0[63:32] (Read/Write)
// 0x18 : reserved
// 0x1c : Data signal of x1
//        bit 31~0 - x1[31:0] (Read/Write)
// 0x20 : Data signal of x1
//        bit 31~0 - x1[63:32] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of x2
//        bit 31~0 - x2[31:0] (Read/Write)
// 0x2c : Data signal of x2
//        bit 31~0 - x2[63:32] (Read/Write)
// 0x30 : reserved
// 0x34 : Data signal of x3
//        bit 31~0 - x3[31:0] (Read/Write)
// 0x38 : Data signal of x3
//        bit 31~0 - x3[63:32] (Read/Write)
// 0x3c : reserved
// 0x40 : Data signal of w
//        bit 31~0 - w[31:0] (Read/Write)
// 0x44 : Data signal of w
//        bit 31~0 - w[63:32] (Read/Write)
// 0x48 : reserved
// 0x4c : Data signal of y
//        bit 31~0 - y[31:0] (Read/Write)
// 0x50 : Data signal of y
//        bit 31~0 - y[63:32] (Read/Write)
// 0x54 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XTOP_CONTROL_ADDR_AP_CTRL 0x00
#define XTOP_CONTROL_ADDR_GIE     0x04
#define XTOP_CONTROL_ADDR_IER     0x08
#define XTOP_CONTROL_ADDR_ISR     0x0c
#define XTOP_CONTROL_ADDR_X0_DATA 0x10
#define XTOP_CONTROL_BITS_X0_DATA 64
#define XTOP_CONTROL_ADDR_X1_DATA 0x1c
#define XTOP_CONTROL_BITS_X1_DATA 64
#define XTOP_CONTROL_ADDR_X2_DATA 0x28
#define XTOP_CONTROL_BITS_X2_DATA 64
#define XTOP_CONTROL_ADDR_X3_DATA 0x34
#define XTOP_CONTROL_BITS_X3_DATA 64
#define XTOP_CONTROL_ADDR_W_DATA  0x40
#define XTOP_CONTROL_BITS_W_DATA  64
#define XTOP_CONTROL_ADDR_Y_DATA  0x4c
#define XTOP_CONTROL_BITS_Y_DATA  64

