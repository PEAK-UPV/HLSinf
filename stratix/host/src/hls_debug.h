#ifndef _HLS_DEBUG_H_
#define _HLS_DEBUG_H_


// for debuging purposes: number of i iterations for buffers size
#define NUM_OF_I_ITERS 1
#define NUM_OF_O_ITERS 2  // must match o_iters in lib_conv2D.cpp function call

#define NUM_PIXELS_IN_FRAME 9



extern unsigned long int my_val[NUM_KERNELS];
extern unsigned long my_ret[NUM_KERNELS];
extern unsigned long my_ret_2[NUM_KERNELS];
extern unsigned long my_ret_3[NUM_KERNELS];
extern unsigned long my_ret_4[NUM_KERNELS];
extern float         my_flt_bias[NUM_KERNELS];
extern float         my_flt_krnl[NUM_KERNELS];
extern float         my_flt_din[NUM_KERNELS];
extern float         my_flt_dout[NUM_KERNELS];


extern size_t jm10_buffer_size_in_bytes;
extern vector<cl::Event> my_loop_read_events;
extern data_type dbg_cpu_data_directconv_sum;

extern data_type  *dbg_loop_data_in;                         // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_in_buffer_i[MAX_KERNELS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_input_buffer;
extern cl::Buffer *dbg_loop_data_input_buffer_buffer[MAX_KERNELS];

// references to variables of buffers related to direct convolution stage 
extern data_type  *dbg_loop_data_dc_pad_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_dc_pad_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_dc_cvt_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_dc_cvt_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_dc_mul_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_dc_mul_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_directconv_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_directconv_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer

extern data_type  *dbg_loop_data_relu_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_relu_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer
extern data_type  *dbg_loop_data_pooling_out;    // loop from fpga for Input data buffer (format I x W x H)
extern cl::Buffer *dbg_loop_data_pooling_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer




extern int I_useful;
extern int O_useful;

extern data_type *dbg_cpu_data_directconv_out;


// function prototypes
void hls_debug(void);

#endif



