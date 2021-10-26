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

#define XK_CONV2D_CONTROL_ADDR_AP_CTRL                   0x00
#define XK_CONV2D_CONTROL_ADDR_GIE                       0x04
#define XK_CONV2D_CONTROL_ADDR_IER                       0x08
#define XK_CONV2D_CONTROL_ADDR_ISR                       0x0c
#define XK_CONV2D_CONTROL_ADDR_PTR_DATA_DATA             0x10
#define XK_CONV2D_CONTROL_BITS_PTR_DATA_DATA             64
#define XK_CONV2D_CONTROL_ADDR_H_DATA                    0x1c
#define XK_CONV2D_CONTROL_BITS_H_DATA                    32
#define XK_CONV2D_CONTROL_ADDR_W_DATA                    0x24
#define XK_CONV2D_CONTROL_BITS_W_DATA                    32
#define XK_CONV2D_CONTROL_ADDR_ROWS_DATA                 0x2c
#define XK_CONV2D_CONTROL_BITS_ROWS_DATA                 32
#define XK_CONV2D_CONTROL_ADDR_I_DATA                    0x34
#define XK_CONV2D_CONTROL_BITS_I_DATA                    32
#define XK_CONV2D_CONTROL_ADDR_O_DATA                    0x3c
#define XK_CONV2D_CONTROL_BITS_O_DATA                    32
#define XK_CONV2D_CONTROL_ADDR_I_ITER_DATA               0x44
#define XK_CONV2D_CONTROL_BITS_I_ITER_DATA               32
#define XK_CONV2D_CONTROL_ADDR_O_ITER_FIRST_DATA         0x4c
#define XK_CONV2D_CONTROL_BITS_O_ITER_FIRST_DATA         32
#define XK_CONV2D_CONTROL_ADDR_O_ITER_LAST_DATA          0x54
#define XK_CONV2D_CONTROL_BITS_O_ITER_LAST_DATA          32
#define XK_CONV2D_CONTROL_ADDR_ENABLE_RELU_DATA          0x5c
#define XK_CONV2D_CONTROL_BITS_ENABLE_RELU_DATA          32
#define XK_CONV2D_CONTROL_ADDR_PTR_KERNEL_DATA           0x64
#define XK_CONV2D_CONTROL_BITS_PTR_KERNEL_DATA           64
#define XK_CONV2D_CONTROL_ADDR_PTR_BIAS_DATA             0x70
#define XK_CONV2D_CONTROL_BITS_PTR_BIAS_DATA             64
#define XK_CONV2D_CONTROL_ADDR_PTR_OUT_DATA              0x7c
#define XK_CONV2D_CONTROL_BITS_PTR_OUT_DATA              64
#define XK_CONV2D_CONTROL_ADDR_GLOBAL_OFFSET_DATA        0x88
#define XK_CONV2D_CONTROL_BITS_GLOBAL_OFFSET_DATA        32
#define XK_CONV2D_CONTROL_ADDR_ENABLE_UPPER_PADDING_DATA 0x90
#define XK_CONV2D_CONTROL_BITS_ENABLE_UPPER_PADDING_DATA 32
#define XK_CONV2D_CONTROL_ADDR_ENABLE_LOWER_PADDING_DATA 0x98
#define XK_CONV2D_CONTROL_BITS_ENABLE_LOWER_PADDING_DATA 32
#define XK_CONV2D_CONTROL_ADDR_ENABLE_MAXPOOLING_DATA    0xa0
#define XK_CONV2D_CONTROL_BITS_ENABLE_MAXPOOLING_DATA    32
#define XK_CONV2D_CONTROL_ADDR_ENABLE_AVGPOOLING_DATA    0xa8
#define XK_CONV2D_CONTROL_BITS_ENABLE_AVGPOOLING_DATA    32
#define XK_CONV2D_CONTROL_ADDR_ENABLE_CLIPPING_DATA      0xb0
#define XK_CONV2D_CONTROL_BITS_ENABLE_CLIPPING_DATA      32
#define XK_CONV2D_CONTROL_ADDR_ENABLE_SHIFT_DATA         0xb8
#define XK_CONV2D_CONTROL_BITS_ENABLE_SHIFT_DATA         32
#define XK_CONV2D_CONTROL_ADDR_MIN_CLIP_DATA             0xc0
#define XK_CONV2D_CONTROL_BITS_MIN_CLIP_DATA             32
#define XK_CONV2D_CONTROL_ADDR_MAX_CLIP_DATA             0xc8
#define XK_CONV2D_CONTROL_BITS_MAX_CLIP_DATA             32
#define XK_CONV2D_CONTROL_ADDR_DIR_SHIFT_DATA            0xd0
#define XK_CONV2D_CONTROL_BITS_DIR_SHIFT_DATA            32
#define XK_CONV2D_CONTROL_ADDR_POS_SHIFT_DATA            0xd8
#define XK_CONV2D_CONTROL_BITS_POS_SHIFT_DATA            32

