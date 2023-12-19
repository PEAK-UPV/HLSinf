/*
 * fpga.h
 *
 */

#ifdef RUNTIME_SUPPORT

// function prototypes
void fn_init_fpga();
void allocate_buffers();
void copy_to_fpga(cl::Buffer *buf);
void copy_from_fpga(cl::Buffer *buf);
void deallocate_buffers();
void fn_run_node_on_fpga(int n);

#endif

