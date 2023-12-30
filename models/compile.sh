g++ -c fig.c           -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT 
g++ -c in_out.c        -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT
g++ -c initializers.c  -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT
g++ -c main.c          -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT
g++ -c nodes.c         -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT
g++ -c parsers.c       -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT
g++ -c utils.c         -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT
g++ -c runner.c        -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -fopenmp -DRUNTIME_SUPPORT
g++ -c fpga.c          -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT
g++ -c model.c         -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT
g++ -c cpu.c           -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -fopenmp -DRUNTIME_SUPPORT
g++ -c stats.c         -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 -DRUNTIME_SUPPORT
g++ -c ../opencl/common/includes/xcl2/xcl2.cpp -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ stats.o cpu.o xcl2.o runner.o fpga.o fig.o in_out.o nodes.o initializers.o main.o model.o parsers.o utils.o -o hlsinf_runner -lOpenCL -fopenmp -lpthread -lrt -lstdc++ -L/opt/xilinx/xrt/lib
