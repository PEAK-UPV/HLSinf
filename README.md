# HLSinf
High-Level Synthesis inference accelerator for FPGAs

This project targets inference processes on FPGA systems. For this, HLS is used as the main programming language (C++ with HLS pragmas).

It is highly recommended to use vitis_hls (Vitis 2020.1) to work with the project. In order to launch the project you need to first set the environment variables and then launch vitis_hls and open the project (we asume you cloned the repo in HLSinf directory):

cd HLSinf
  
source scripts/setenv.sh

vitis_hls

(the project is located at <HLSinf/project/HLSinf)

--------------------------------------------------------------
First try: Configure the accelerator
--------------------------------------------------------------

Within vitis_hls edit the conv2D.h file and select the data type of the accelerator (float, ap_fixed, ap_int) and the accelerator width (CPI and CPO). Recommendation is to test first float and 4x4 configuration (data_type set to float and CPI CPO set both to 4). You can also select whether using ReLU activation function and a pooling layer (maxpool and avgpool supported). Enable the use of ReLU only.

Once done, launch C Simulation process (Project|Run C Simulation) and select "Build Only". The simulator will be built. You can find the binary for the simulator at HLSinf/project/HLSinf/AlveoU200/csim/build folder. Run ./csim.exe there. Several tests are launched, indicating for each one whether it succeeded or failed. Some tests may be skipped as some specific support (e.g. pooling) has not been activated.
  
--------------------------------------------------------------
Second try: Launch C synthesis
--------------------------------------------------------------
  
Launch C synthesis (Solution|Run C Synthesis|Active Solution). Once completed you will see a summary of the synthesis process. Notice that estimated execution time of the accelerator is indicated. For an specific input geometry of the data you can change the defines I_REFERENCE, O_REFERENCE, W_REFERENCE, and H_REFERENCE, located in the conv2D.h file. These defines set the number of input and output channels as well as the image width and height. Those defines are used only for C synthesis in order to compute the execution time of the accelerator in cycles. As a reference, for an input image of 256x256 pixels with 4 input channels and 4 output channels, the expected execution time is around 64K cycles (assuming CPI = 4 and CPO = 4; the accelerator handles CPI pixels per cycle).

--------------------------------------------------------------
Third try: Compile and launch test with OpenCL (sw_emu)
--------------------------------------------------------------

You can also compile and build an xclbin binary and use it with OpenCL. To do this, go to the opencl folder and make the project for sw_emu as follows:

make all TARGET=sw_emu

Once completed the xclbin has been generated in the build_dir.sw_emu.xilinx_u200_xdma_201830_2

In order to run the OpenCL test type:

export XCL_EMULATION_MODE=sw_emu
./test_conv2D build_dir.sw_emu.xilinx_u200_xdma_201830_2/kernel_conv2D.xclbin 1 1 1 64 64 4 4

The test will perform a convolution operation for images of 64x64 with 4 input channels and 4 output channels. The test will determine success or failure.

--------------------------------------------------------------
Fourth try: Compile and launch test with OpenCL (hw_emu)
--------------------------------------------------------------

The previous step can be done for hardware emulation. Just type:

make all TARGET=hw_emu

and run the test but using the new generated xclbin file (and with the XCL_EMULATION_MODE properly set):

export XCL_EMULATION_MODE=hw_emu
./test_conv2D build_dir.hw_emu.xilinx_u200_xdma_201830_2/kernel_conv2D.xclbin 1 1 1 64 64 4 4

This process will take much more time but at the end you will get the estimated execution time of your kernel.

--------------------------------------------------------------
Final try: Compile and launch test with OpenCL (hw)
--------------------------------------------------------------

As a final try you can generate the xclbin binary for hw, that is running it on real hardware. To do this:

make all TARGET=hw

and run the test but using the new generated xclbin file (and with the XCL_EMULATION_MODE unset):

unset XCL_EMULATION_MODE
./test_conv2D build_dir.hw.xilinx_u200_xdma_201830_2/kernel_conv2D.xclbin 1 1 1 64 64 4 4

good luck!!!

------------------------------------------------------------------------------------------
Current Versions supported
------------------------------------------------------------------------------------------

For a list of achieved configurations and performance see solutions/achieved_solutions.txt
