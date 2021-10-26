# ==============================================================
# Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
# Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
# ==============================================================
CSIM_DESIGN = 1

__SIM_FPO__ = 1

__SIM_MATHHLS__ = 1

__SIM_FFT__ = 1

__SIM_FIR__ = 1

__SIM_DDS__ = 1

ObjDir = obj

HLS_SOURCES = ../../../../../src/test_print.cpp ../../../../../src/test_kernel.cpp ../../../../../src/test_file.cpp ../../../../../src/test_cpu.cpp ../../../../../src/test_conv2D.cpp ../../../../../src/test_check.cpp ../../../../../src/test_buffers.cpp ../../../../../src/test_arguments.cpp ../../../../../src/data_test.cpp ../../../../../src/write.cpp ../../../../../src/winograd_convolution.cpp ../../../../../src/serialization.cpp ../../../../../src/relu.cpp ../../../../../src/read.cpp ../../../../../src/pooling.cpp ../../../../../src/padding.cpp ../../../../../src/mul.cpp ../../../../../src/lib_hlsm.cpp ../../../../../src/k_conv2D.cpp ../../../../../src/join_split.cpp ../../../../../src/dws_convolution.cpp ../../../../../src/direct_convolution.cpp ../../../../../src/cvt.cpp ../../../../../src/batch_normalization.cpp ../../../../../src/add.cpp

override TARGET := csim.exe

AUTOPILOT_ROOT := /opt/Xilinx/Vitis_HLS/2020.2
AUTOPILOT_MACH := lnx64
ifdef AP_GCC_M32
  AUTOPILOT_MACH := Linux_x86
  IFLAG += -m32
endif
IFLAG += -fPIC
ifndef AP_GCC_PATH
  AP_GCC_PATH := /opt/Xilinx/Vitis_HLS/2020.2/tps/lnx64/gcc-6.2.0/bin
endif
AUTOPILOT_TOOL := ${AUTOPILOT_ROOT}/${AUTOPILOT_MACH}/tools
AP_CLANG_PATH := ${AUTOPILOT_TOOL}/clang-3.9/bin
AUTOPILOT_TECH := ${AUTOPILOT_ROOT}/common/technology


IFLAG += -I "${AUTOPILOT_TOOL}/systemc/include"
IFLAG += -I "${AUTOPILOT_ROOT}/include"
IFLAG += -I "${AUTOPILOT_ROOT}/include/ap_sysc"
IFLAG += -I "${AUTOPILOT_TECH}/generic/SystemC"
IFLAG += -I "${AUTOPILOT_TECH}/generic/SystemC/AESL_FP_comp"
IFLAG += -I "${AUTOPILOT_TECH}/generic/SystemC/AESL_comp"
IFLAG += -I "${AUTOPILOT_TOOL}/auto_cc/include"
IFLAG += -I "/usr/include/x86_64-linux-gnu"
IFLAG += -D__VITIS_HLS__

IFLAG += -D__SIM_FPO__

IFLAG += -D__SIM_FFT__

IFLAG += -D__SIM_FIR__

IFLAG += -D__SIM_DDS__

IFLAG += -D__DSP48E2__
IFLAG += -Wno-unknown-pragmas 
IFLAG += -g
DFLAG += -D__xilinx_ip_top= -DAESL_TB
CCFLAG += -Werror=return-type
TOOLCHAIN += 



include ./Makefile.rules

all: $(TARGET)



$(ObjDir)/test_print.o: ../../../../../src/test_print.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/test_print.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD -Wno-unknown-pragmas -Wno-unknown-pragmas  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/test_print.d

$(ObjDir)/test_kernel.o: ../../../../../src/test_kernel.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/test_kernel.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD -Wno-unknown-pragmas -Wno-unknown-pragmas  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/test_kernel.d

$(ObjDir)/test_file.o: ../../../../../src/test_file.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/test_file.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD -Wno-unknown-pragmas -Wno-unknown-pragmas  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/test_file.d

$(ObjDir)/test_cpu.o: ../../../../../src/test_cpu.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/test_cpu.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD -Wno-unknown-pragmas -Wno-unknown-pragmas  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/test_cpu.d

$(ObjDir)/test_conv2D.o: ../../../../../src/test_conv2D.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/test_conv2D.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD -Wno-unknown-pragmas -Wno-unknown-pragmas  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/test_conv2D.d

$(ObjDir)/test_check.o: ../../../../../src/test_check.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/test_check.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD -Wno-unknown-pragmas -Wno-unknown-pragmas  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/test_check.d

$(ObjDir)/test_buffers.o: ../../../../../src/test_buffers.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/test_buffers.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD -Wno-unknown-pragmas -Wno-unknown-pragmas  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/test_buffers.d

$(ObjDir)/test_arguments.o: ../../../../../src/test_arguments.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/test_arguments.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD -Wno-unknown-pragmas -Wno-unknown-pragmas  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/test_arguments.d

$(ObjDir)/data_test.o: ../../../../../src/data_test.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/data_test.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD -Wno-unknown-pragmas -Wno-unknown-pragmas  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/data_test.d

$(ObjDir)/write.o: ../../../../../src/write.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/write.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/write.d

$(ObjDir)/winograd_convolution.o: ../../../../../src/winograd_convolution.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/winograd_convolution.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/winograd_convolution.d

$(ObjDir)/serialization.o: ../../../../../src/serialization.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/serialization.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/serialization.d

$(ObjDir)/relu.o: ../../../../../src/relu.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/relu.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/relu.d

$(ObjDir)/read.o: ../../../../../src/read.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/read.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/read.d

$(ObjDir)/pooling.o: ../../../../../src/pooling.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/pooling.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/pooling.d

$(ObjDir)/padding.o: ../../../../../src/padding.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/padding.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/padding.d

$(ObjDir)/mul.o: ../../../../../src/mul.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/mul.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/mul.d

$(ObjDir)/lib_hlsm.o: ../../../../../src/lib_hlsm.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/lib_hlsm.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/lib_hlsm.d

$(ObjDir)/k_conv2D.o: ../../../../../src/k_conv2D.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/k_conv2D.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/k_conv2D.d

$(ObjDir)/join_split.o: ../../../../../src/join_split.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/join_split.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/join_split.d

$(ObjDir)/dws_convolution.o: ../../../../../src/dws_convolution.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/dws_convolution.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/dws_convolution.d

$(ObjDir)/direct_convolution.o: ../../../../../src/direct_convolution.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/direct_convolution.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/direct_convolution.d

$(ObjDir)/cvt.o: ../../../../../src/cvt.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/cvt.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/cvt.d

$(ObjDir)/batch_normalization.o: ../../../../../src/batch_normalization.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/batch_normalization.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/batch_normalization.d

$(ObjDir)/add.o: ../../../../../src/add.cpp $(ObjDir)/.dir
	$(Echo) "   Compiling ../../../../../src/add.cpp in $(BuildMode) mode" $(AVE_DIR_DLOG)
	$(Verb)  $(CC) ${CCFLAG} -c -MMD  $(IFLAG) $(DFLAG) $< -o $@ ; \

-include $(ObjDir)/add.d
