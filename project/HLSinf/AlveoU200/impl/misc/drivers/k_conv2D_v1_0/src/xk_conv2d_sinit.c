// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xk_conv2d.h"

extern XK_conv2d_Config XK_conv2d_ConfigTable[];

XK_conv2d_Config *XK_conv2d_LookupConfig(u16 DeviceId) {
	XK_conv2d_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XK_CONV2D_NUM_INSTANCES; Index++) {
		if (XK_conv2d_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XK_conv2d_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XK_conv2d_Initialize(XK_conv2d *InstancePtr, u16 DeviceId) {
	XK_conv2d_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XK_conv2d_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XK_conv2d_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

