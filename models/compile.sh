g++ -c fig.c           -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2 
g++ -c in_out.c        -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ -c initializers.c  -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ -c main.c          -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ -c nodes.c         -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ -c parsers.c       -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ -c utils.c         -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ -c runner.c        -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ -c fpga.c          -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ -c model.c         -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ -c ../opencl/common/includes/xcl2/xcl2.cpp -I/opt/xilinx/xrt/include -I/opt/Xilinx/Vivado/2020.2/include -g -std=c++11 -I./../opencl/common/includes/xcl2
g++ xcl2.o runner.o fpga.o fig.o in_out.o nodes.o initializers.o main.o model.o parsers.o utils.o -o txt2hlsinf -lOpenCL -lpthread -lrt -lstdc++ -L/opt/xilinx/xrt/lib
