// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xk_conv2d.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XK_conv2d_CfgInitialize(XK_conv2d *InstancePtr, XK_conv2d_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XK_conv2d_Start(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_AP_CTRL) & 0x80;
    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XK_conv2d_IsDone(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XK_conv2d_IsIdle(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XK_conv2d_IsReady(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XK_conv2d_Continue(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_AP_CTRL) & 0x80;
    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_AP_CTRL, Data | 0x10);
}

void XK_conv2d_EnableAutoRestart(XK_conv2d *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XK_conv2d_DisableAutoRestart(XK_conv2d *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_AP_CTRL, 0);
}

void XK_conv2d_Set_ptr_data(XK_conv2d *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_DATA_DATA, (u32)(Data));
    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_DATA_DATA + 4, (u32)(Data >> 32));
}

u64 XK_conv2d_Get_ptr_data(XK_conv2d *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_DATA_DATA);
    Data += (u64)XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_DATA_DATA + 4) << 32;
    return Data;
}

void XK_conv2d_Set_H(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_H_DATA, Data);
}

u32 XK_conv2d_Get_H(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_H_DATA);
    return Data;
}

void XK_conv2d_Set_W(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_W_DATA, Data);
}

u32 XK_conv2d_Get_W(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_W_DATA);
    return Data;
}

void XK_conv2d_Set_rows(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ROWS_DATA, Data);
}

u32 XK_conv2d_Get_rows(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ROWS_DATA);
    return Data;
}

void XK_conv2d_Set_I(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_I_DATA, Data);
}

u32 XK_conv2d_Get_I(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_I_DATA);
    return Data;
}

void XK_conv2d_Set_O(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_O_DATA, Data);
}

u32 XK_conv2d_Get_O(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_O_DATA);
    return Data;
}

void XK_conv2d_Set_I_ITER(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_I_ITER_DATA, Data);
}

u32 XK_conv2d_Get_I_ITER(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_I_ITER_DATA);
    return Data;
}

void XK_conv2d_Set_o_iter_first(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_O_ITER_FIRST_DATA, Data);
}

u32 XK_conv2d_Get_o_iter_first(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_O_ITER_FIRST_DATA);
    return Data;
}

void XK_conv2d_Set_o_iter_last(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_O_ITER_LAST_DATA, Data);
}

u32 XK_conv2d_Get_o_iter_last(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_O_ITER_LAST_DATA);
    return Data;
}

void XK_conv2d_Set_enable_relu(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_RELU_DATA, Data);
}

u32 XK_conv2d_Get_enable_relu(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_RELU_DATA);
    return Data;
}

void XK_conv2d_Set_ptr_kernel(XK_conv2d *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_KERNEL_DATA, (u32)(Data));
    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_KERNEL_DATA + 4, (u32)(Data >> 32));
}

u64 XK_conv2d_Get_ptr_kernel(XK_conv2d *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_KERNEL_DATA);
    Data += (u64)XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_KERNEL_DATA + 4) << 32;
    return Data;
}

void XK_conv2d_Set_ptr_bias(XK_conv2d *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_BIAS_DATA, (u32)(Data));
    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_BIAS_DATA + 4, (u32)(Data >> 32));
}

u64 XK_conv2d_Get_ptr_bias(XK_conv2d *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_BIAS_DATA);
    Data += (u64)XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_BIAS_DATA + 4) << 32;
    return Data;
}

void XK_conv2d_Set_ptr_out(XK_conv2d *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_OUT_DATA, (u32)(Data));
    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_OUT_DATA + 4, (u32)(Data >> 32));
}

u64 XK_conv2d_Get_ptr_out(XK_conv2d *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_OUT_DATA);
    Data += (u64)XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_PTR_OUT_DATA + 4) << 32;
    return Data;
}

void XK_conv2d_Set_global_offset(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_GLOBAL_OFFSET_DATA, Data);
}

u32 XK_conv2d_Get_global_offset(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_GLOBAL_OFFSET_DATA);
    return Data;
}

void XK_conv2d_Set_enable_upper_padding(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_UPPER_PADDING_DATA, Data);
}

u32 XK_conv2d_Get_enable_upper_padding(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_UPPER_PADDING_DATA);
    return Data;
}

void XK_conv2d_Set_enable_lower_padding(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_LOWER_PADDING_DATA, Data);
}

u32 XK_conv2d_Get_enable_lower_padding(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_LOWER_PADDING_DATA);
    return Data;
}

void XK_conv2d_Set_enable_maxpooling(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_MAXPOOLING_DATA, Data);
}

u32 XK_conv2d_Get_enable_maxpooling(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_MAXPOOLING_DATA);
    return Data;
}

void XK_conv2d_Set_enable_avgpooling(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_AVGPOOLING_DATA, Data);
}

u32 XK_conv2d_Get_enable_avgpooling(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_AVGPOOLING_DATA);
    return Data;
}

void XK_conv2d_Set_enable_clipping(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_CLIPPING_DATA, Data);
}

u32 XK_conv2d_Get_enable_clipping(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_CLIPPING_DATA);
    return Data;
}

void XK_conv2d_Set_enable_shift(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_SHIFT_DATA, Data);
}

u32 XK_conv2d_Get_enable_shift(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ENABLE_SHIFT_DATA);
    return Data;
}

void XK_conv2d_Set_min_clip(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_MIN_CLIP_DATA, Data);
}

u32 XK_conv2d_Get_min_clip(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_MIN_CLIP_DATA);
    return Data;
}

void XK_conv2d_Set_max_clip(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_MAX_CLIP_DATA, Data);
}

u32 XK_conv2d_Get_max_clip(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_MAX_CLIP_DATA);
    return Data;
}

void XK_conv2d_Set_dir_shift(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_DIR_SHIFT_DATA, Data);
}

u32 XK_conv2d_Get_dir_shift(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_DIR_SHIFT_DATA);
    return Data;
}

void XK_conv2d_Set_pos_shift(XK_conv2d *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_POS_SHIFT_DATA, Data);
}

u32 XK_conv2d_Get_pos_shift(XK_conv2d *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_POS_SHIFT_DATA);
    return Data;
}

void XK_conv2d_InterruptGlobalEnable(XK_conv2d *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_GIE, 1);
}

void XK_conv2d_InterruptGlobalDisable(XK_conv2d *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_GIE, 0);
}

void XK_conv2d_InterruptEnable(XK_conv2d *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_IER);
    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_IER, Register | Mask);
}

void XK_conv2d_InterruptDisable(XK_conv2d *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_IER);
    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_IER, Register & (~Mask));
}

void XK_conv2d_InterruptClear(XK_conv2d *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XK_conv2d_WriteReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ISR, Mask);
}

u32 XK_conv2d_InterruptGetEnabled(XK_conv2d *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_IER);
}

u32 XK_conv2d_InterruptGetStatus(XK_conv2d *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XK_conv2d_ReadReg(InstancePtr->Control_BaseAddress, XK_CONV2D_CONTROL_ADDR_ISR);
}

