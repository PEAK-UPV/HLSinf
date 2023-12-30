/*
 *
 * runner.h
 *
 */

struct st_eog {
  int node;
  int cpu;
  int first_O;
  int last_O;
  int first_HI;
  int last_HI;
  // parameters for statistics
  int num_runs;
  unsigned long long accumulated_runtime;  
};

// global variables
void fn_read_run_graph();
void run_graph();
void copy_initializers_to_fpga();
void fn_show_output();
