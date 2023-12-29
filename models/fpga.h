/*
 * fpga.h
 *
 */

#ifdef RUNTIME_SUPPORT

#define MAX_KERNELS 8

// function prototypes
void fn_init_fpga();
void allocate_buffers();
void copy_to_fpga(cl::Buffer *buf);
void copy_from_fpga(cl::Buffer *buf);
void deallocate_buffers();
void fn_run_node_on_fpga(int n, int k, int first_O, int last_O, int first_HI, int last_HI);

#endif

