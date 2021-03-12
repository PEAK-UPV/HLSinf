# HLSinf
High-Level Synthesis inference accelerator for FPGAs

This project targets inference processes on FPGA systems. For this, HLS is used as the main programming language (C++ with HLS pragmas).

It is highly recommended to use vitis_hls (Vitis 2020.1) to work with the project. In order to launch the project you need to first set the environment variables and then launch vitis_hls and open the project:

cd <HLSinf directory>
  
source scripts/setenv.sh

vitis_hls

(the project is located at <HLSinf directory>/project/HLSinf)

--------------------------------------------------------------
First try: Configure the accelerator
--------------------------------------------------------------

Within vitis_hls edit the conv2D.h file and select the data type of the accelerator (float, ap_fixed, ap_int) and the accelerator width (CPI and CPO). Recommendation is to test first float and 4x4 configuration (data_type set to float and CPI CPO set both to 4.

Once done, launch C Simulation process (Project|Run C Simulation) and select "Build Only". The simulator will be built. You can find the binary for the simulator at <HLSinf directory>/project/HLSinf/AlveoU200/csim/build folder. Run ./csim.exe there. The input and output matrices for a convolution process will be shown and the test will tell you whether results are ok or not.
  
--------------------------------------------------------------
Second try: Launch C synthesis
--------------------------------------------------------------
  
Launch C synthesis (Solution|Run C Synthesis|Active Solution). Once completed you will see a summary of the synthesis process. Notice that estimated execution time of the accelerator is indicated. For an specific input geometry of the data you can change the defines I_REFERENCE, O_REFERENCE, W_REFERENCE, and H_REFERENCE, located in the conv2D.h file. These defines set the number of input and output channels as well as the image width and height. Those defines are used only for C synthesis in order to compute the execution time of the accelerator in cycles. As a reference, for an input image of 256x256 pixels with 4 input channels and 4 output channels, the expected execution time is around 64K cycles (assuming CPI = 4 and CPO = 4; the accelerator handles CPI pixels per cycle).
