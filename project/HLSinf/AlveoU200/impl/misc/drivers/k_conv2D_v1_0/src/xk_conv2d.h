// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XK_CONV2D_H
#define XK_CONV2D_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xk_conv2d_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_BaseAddress;
} XK_conv2d_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XK_conv2d;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XK_conv2d_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XK_conv2d_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XK_conv2d_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XK_conv2d_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XK_conv2d_Initialize(XK_conv2d *InstancePtr, u16 DeviceId);
XK_conv2d_Config* XK_conv2d_LookupConfig(u16 DeviceId);
int XK_conv2d_CfgInitialize(XK_conv2d *InstancePtr, XK_conv2d_Config *ConfigPtr);
#else
int XK_conv2d_Initialize(XK_conv2d *InstancePtr, const char* InstanceName);
int XK_conv2d_Release(XK_conv2d *InstancePtr);
#endif

void XK_conv2d_Start(XK_conv2d *InstancePtr);
u32 XK_conv2d_IsDone(XK_conv2d *InstancePtr);
u32 XK_conv2d_IsIdle(XK_conv2d *InstancePtr);
u32 XK_conv2d_IsReady(XK_conv2d *InstancePtr);
void XK_conv2d_Continue(XK_conv2d *InstancePtr);
void XK_conv2d_EnableAutoRestart(XK_conv2d *InstancePtr);
void XK_conv2d_DisableAutoRestart(XK_conv2d *InstancePtr);

void XK_conv2d_Set_ptr_data(XK_conv2d *InstancePtr, u64 Data);
u64 XK_conv2d_Get_ptr_data(XK_conv2d *InstancePtr);
void XK_conv2d_Set_H(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_H(XK_conv2d *InstancePtr);
void XK_conv2d_Set_W(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_W(XK_conv2d *InstancePtr);
void XK_conv2d_Set_rows(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_rows(XK_conv2d *InstancePtr);
void XK_conv2d_Set_I(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_I(XK_conv2d *InstancePtr);
void XK_conv2d_Set_O(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_O(XK_conv2d *InstancePtr);
void XK_conv2d_Set_I_ITER(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_I_ITER(XK_conv2d *InstancePtr);
void XK_conv2d_Set_o_iter_first(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_o_iter_first(XK_conv2d *InstancePtr);
void XK_conv2d_Set_o_iter_last(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_o_iter_last(XK_conv2d *InstancePtr);
void XK_conv2d_Set_enable_relu(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_enable_relu(XK_conv2d *InstancePtr);
void XK_conv2d_Set_ptr_kernel(XK_conv2d *InstancePtr, u64 Data);
u64 XK_conv2d_Get_ptr_kernel(XK_conv2d *InstancePtr);
void XK_conv2d_Set_ptr_bias(XK_conv2d *InstancePtr, u64 Data);
u64 XK_conv2d_Get_ptr_bias(XK_conv2d *InstancePtr);
void XK_conv2d_Set_ptr_out(XK_conv2d *InstancePtr, u64 Data);
u64 XK_conv2d_Get_ptr_out(XK_conv2d *InstancePtr);
void XK_conv2d_Set_global_offset(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_global_offset(XK_conv2d *InstancePtr);
void XK_conv2d_Set_enable_upper_padding(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_enable_upper_padding(XK_conv2d *InstancePtr);
void XK_conv2d_Set_enable_lower_padding(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_enable_lower_padding(XK_conv2d *InstancePtr);
void XK_conv2d_Set_enable_maxpooling(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_enable_maxpooling(XK_conv2d *InstancePtr);
void XK_conv2d_Set_enable_avgpooling(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_enable_avgpooling(XK_conv2d *InstancePtr);
void XK_conv2d_Set_enable_clipping(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_enable_clipping(XK_conv2d *InstancePtr);
void XK_conv2d_Set_enable_shift(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_enable_shift(XK_conv2d *InstancePtr);
void XK_conv2d_Set_min_clip(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_min_clip(XK_conv2d *InstancePtr);
void XK_conv2d_Set_max_clip(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_max_clip(XK_conv2d *InstancePtr);
void XK_conv2d_Set_dir_shift(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_dir_shift(XK_conv2d *InstancePtr);
void XK_conv2d_Set_pos_shift(XK_conv2d *InstancePtr, u32 Data);
u32 XK_conv2d_Get_pos_shift(XK_conv2d *InstancePtr);

void XK_conv2d_InterruptGlobalEnable(XK_conv2d *InstancePtr);
void XK_conv2d_InterruptGlobalDisable(XK_conv2d *InstancePtr);
void XK_conv2d_InterruptEnable(XK_conv2d *InstancePtr, u32 Mask);
void XK_conv2d_InterruptDisable(XK_conv2d *InstancePtr, u32 Mask);
void XK_conv2d_InterruptClear(XK_conv2d *InstancePtr, u32 Mask);
u32 XK_conv2d_InterruptGetEnabled(XK_conv2d *InstancePtr);
u32 XK_conv2d_InterruptGetStatus(XK_conv2d *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
