/*
 *
 * runner.c
 *
 * This file runs a model provided by its argument on a FPGA device using the xclbin file passed as argument
 *
 */

#ifdef RUNTIME_SUPPORT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include "utils.h"
#include "in_out.h"
#include "nodes.h"
#include "initializers.h"
#include "main.h"
#include "fpga.h"
#include "cpu.h"
#include "stats.h"
#include "runner.h"

#define MAX_EOG_ENTRIES     1000
#define MAX_EXECUTION_ORDER 1000

// global variables
int max_execution_order;
int eog_entries[MAX_EXECUTION_ORDER];
int current_eog_entry[MAX_EXECUTION_ORDER];
struct st_eog eog[MAX_EOG_ENTRIES][MAX_EXECUTION_ORDER];
int num_runs[MAX_EXECUTION_ORDER];
unsigned long long accumulated_runtime[MAX_EXECUTION_ORDER];
pthread_mutex_t lock; 

/*
 *
 * fn_process_input_line2()
 *
 * processes a text line with the configuration to run
 * This function obtains all the arguments from the line and builds the execution model
 *
 */
void fn_process_input_line2(char *line, size_t len) {
  char item[200];
  int offset = 0;

  if (verbose && verbose_level >= 3) {
    char sz_line[201];
    if (strlen(line) < 200) printf("  line: %s", line);  // the line has already a \n
    else {
      strncpy(sz_line, line, 200);
      sz_line[200] = '\0';
      printf("  line: %s ...\n", sz_line);
    }
  }

  // we first get the line type: node, input, output, initializer
  offset = fn_get_item_line(line, len, offset, item);
  int is_node = !strcmp(item, "node");
  int is_input = !strcmp(item, "input");
  int is_output = !strcmp(item, "output");
  int is_initializer = !strcmp(item, "initializer");

  if (is_node) {
    // we get the run order
    offset = fn_get_item_line(line, len, offset, item);
    aNode[num_nodes].run_order = atoi(item);
    max_execution_order = max(max_execution_order, aNode[num_nodes].run_order);

    // now we have the node type to run
    offset = fn_get_item_line(line, len, offset, item);
    aNode[num_nodes].type = (char*)malloc(sizeof(char) * (strlen(item)+1));
    strcpy(aNode[num_nodes].type, item);

    // now we have the name of the node to run
    offset = fn_get_item_line(line, len, offset, item);
    aNode[num_nodes].name = (char*)malloc(sizeof(char) * (strlen(item)+1));
    strcpy(aNode[num_nodes].name, item);

    // number of inputs
    offset = fn_get_item_line(line, len, offset, item);
    aNode[num_nodes].num_inputs = atoi(item);
    // inputs
    aNode[num_nodes].inputs = (char**)malloc(sizeof(char*) * aNode[num_nodes].num_inputs);
    for (int i = 0; i<aNode[num_nodes].num_inputs; i++) {
      offset = fn_get_item_line(line, len, offset, item);
      aNode[num_nodes].inputs[i] = (char*)malloc(sizeof(char) * (strlen(item) + 1));
      strcpy(aNode[num_nodes].inputs[i], item);
    }
    // number of outputs
    offset = fn_get_item_line(line, len, offset, item);
    aNode[num_nodes].num_outputs  = atoi(item);
    // outputs
    aNode[num_nodes].outputs = (char**)malloc(sizeof(char*) * aNode[num_nodes].num_outputs);
    for (int o = 0; o<aNode[num_nodes].num_outputs; o++) {
      offset = fn_get_item_line(line, len, offset, item);
      aNode[num_nodes].outputs[o] = (char*)malloc(sizeof(char) * (strlen(item) + 1));
      strcpy(aNode[num_nodes].outputs[o], item);
    }

    // data geometry
    offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].I  = atoi(item);
    offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].HI = atoi(item);
    offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].WI = atoi(item);
    offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].O  = atoi(item);
    offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].HO = atoi(item);
    offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].WO = atoi(item);

    // keyword (if is an hlsinf node)
    if (!strcmp(aNode[num_nodes].type, "HLSinf")) {
      offset = fn_get_item_line(line, len, offset, item);
      aNode[num_nodes].keyword = (char*)malloc(sizeof(char) * (strlen(item)+1));
      strcpy(aNode[num_nodes].keyword, item);
    }
    // input add
    if (!strcmp(aNode[num_nodes].type, "HLSinf")) {
      offset = fn_get_item_line(line, len, offset, item);
      aNode[num_nodes].input_add = (char*)malloc(sizeof(char) * (strlen(item)+1));
      strcpy(aNode[num_nodes].input_add, item);
    }

    // if we have a conv then we read the conv parameters (dh, dw, kh, kw, pt, pb, pl, pr, sh, sw, g)
    if (!strcmp(aNode[num_nodes].type, "Conv")) {
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].dh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].dw = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].kh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].kw = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sw = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pt = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pb = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pl = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pr = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].g  = atoi(item);
    }
    // if we have a batchnormalization then we read the parameters epsilon and momentum
    if (!strcmp(aNode[num_nodes].type, "BatchNormalization")) {
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].epsilon = atof(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].momentum = atof(item);
    }
    // if we have a maxpool then we read the parameters kh, kw, pt, pb, pl, pr, sh, sw)
    if (!strcmp(aNode[num_nodes].type, "MaxPool")) {
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].kh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].kw = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sw = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pt = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pb = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pl = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pr = atoi(item);
    }
    // if we have an avgpool then we read the parameters kh, kw, pt, pb, pl, pr, sh, sw)
    if (!strcmp(aNode[num_nodes].type, "AveragePool")) {
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].kh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].kw = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sw = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pt = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pb = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pl = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pr = atoi(item);
    }
    // if we have a HLSinf then we read all the parameters: kh, kw, pt, pb, pl, pr, sh, sw
    if (!strcmp(aNode[num_nodes].type, "HLSinf")) {
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].hlsinf_kh_conv = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].hlsinf_kw_conv = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].hlsinf_sh_conv = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].hlsinf_sw_conv = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].hlsinf_pt_conv = atoi(item);	    
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].hlsinf_pb_conv = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].hlsinf_pl_conv = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].hlsinf_pr_conv = atoi(item);
    }
    aNode[num_nodes].valid = true;
    num_nodes++;
  }

  if (is_input) {
    // name
    offset = fn_get_item_line(line, len, offset, item);
    aInput[num_inputs].name = (char*)malloc(sizeof(char) * (strlen(item)+1));
    strcpy(aInput[num_inputs].name, item);
    // number of dimensions
    offset = fn_get_item_line(line, len, offset, item);
    aInput[num_inputs].num_dimensions = atoi(item);
    aInput[num_inputs].dimensions = (int*)malloc(sizeof(int) * aInput[num_inputs].num_dimensions);
    for (int d=0; d<aInput[num_inputs].num_dimensions; d++) {
      offset = fn_get_item_line(line, len, offset, item); 
      aInput[num_inputs].dimensions[d] = atoi(item);
    }
    aInput[num_inputs].valid = true;
    num_inputs++;
  }

  if (is_output) {
    // name
    offset = fn_get_item_line(line, len, offset, item);
    aOutput[num_outputs].name = (char*)malloc(sizeof(char) * (strlen(item)+1));
    strcpy(aOutput[num_outputs].name, item);
    aOutput[num_outputs].valid = true;
    num_outputs++;
  }

  if (is_initializer) {
    // name
    offset = fn_get_item_line(line, len, offset, item);
    aInitializer[num_initializers].name = (char*)malloc(sizeof(char) * (strlen(item)+1));
    strcpy(aInitializer[num_initializers].name, item);
    // dimensions
    offset = fn_get_item_line(line, len, offset, item);
    aInitializer[num_initializers].num_dimensions = atoi(item);
    aInitializer[num_initializers].dimensions = (int *)malloc(sizeof(int) * aInitializer[num_initializers].num_dimensions);
    for (int d=0; d<aInitializer[num_initializers].num_dimensions; d++) {
      offset = fn_get_item_line(line, len, offset, item);
      aInitializer[num_initializers].dimensions[d] = atoi(item);
    }
    // data type
    offset = fn_get_item_line(line, len, offset, item);
    strcpy(aInitializer[num_initializers].data_type, item);
    //
    // now the data
    int num_items = 1;
    for (int d=0; d<aInitializer[num_initializers].num_dimensions; d++) num_items *= aInitializer[num_initializers].dimensions[d];
    posix_memalign((void **)&aInitializer[num_initializers].data, 4096, num_items * sizeof(float));    
    for (int x=0; x<num_items; x++) {
      offset = fn_get_item_line(line, len, offset, item);
      aInitializer[num_initializers].data[x] = atof(item);
    }
    aInitializer[num_initializers].valid = true;
    num_initializers++;
  }
}

/*
 * fn_read_run_graph
 *
 * reads the run graph
 *
 */
void fn_read_run_graph() {
  FILE *fd;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  if (verbose && verbose_level >= 3) printf("reading run graph (%s)...\n", input_file_name);

  if ((fd = fopen(input_file_name, "r"))==NULL) {
    printf("Error, could not open input file\n");
    exit(1);
  }

  // now we read lines until the end of the text file
  num_nodes = 0;
  num_inputs = 0;
  num_outputs = 0;
  num_initializers = 0;
  max_execution_order = -1;
  while ((read = getline(&line, &len, fd)) != -1) fn_process_input_line2(line, len);

  fclose(fd);
  if (line) free(line);

  if (verbose && verbose_level >= 3) printf("  completed (nodes: %d, inputs: %d, outputs: %d, initializers: %d)\n", num_nodes, num_inputs, num_outputs, num_initializers);
}

/*
 * build_execution_order_graph()
 *
 * This function builds an execution order graph taking into
 * account the number of kernels, and the nodes available
 * in a given execution order
 * The function takes into account the modes of parallelism available
 * (ocp, irp, np)
 */
void build_execution_order_graph(int order) {

  //if (verbose) printf("    building execution order graph for execution order %d\n", order);

  // we create a list of hlsinf nodes and a list of cpu nodes
  int num_hlsinf_nodes = 0;
  int num_cpu_nodes = 0;
  int hlsinf_nodes[500];
  int cpu_nodes[500];
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid && (aNode[n].run_order == order)) {
      if (is_hlsinf(n)) {hlsinf_nodes[num_hlsinf_nodes] = n; num_hlsinf_nodes++;}
      else {cpu_nodes[num_cpu_nodes] = n; num_cpu_nodes++;}
    }
  }

  eog_entries[order] = 0;

  // every cpu node is added as a single entry in the execution graph
  for (int n=0; n<num_cpu_nodes; n++) {
    eog[eog_entries[order]][order].node = cpu_nodes[n];
    eog[eog_entries[order]][order].cpu  = true;
    eog_entries[order]++;
  }

  // hlsinf nodes
  if (ocp_enabled) {
    // we build groups of ocp_threshold, each one in a different task
    for (int n=0; n<num_hlsinf_nodes; n++) {
      int O  = aNode[hlsinf_nodes[n]].O;
      int HI = aNode[hlsinf_nodes[n]].HI;
      for (int o=0; o<O; o+= ocp_threshold) {
	eog[eog_entries[order]][order].node = hlsinf_nodes[n];
	eog[eog_entries[order]][order].cpu  = false;
	eog[eog_entries[order]][order].first_O = o;
	eog[eog_entries[order]][order].last_O = min(O-1, o+ocp_threshold-1);
	eog[eog_entries[order]][order].first_HI = 0;
	eog[eog_entries[order]][order].last_HI = HI-1;
	eog_entries[order]++;
      }
    }
  } else if (irp_enabled) {
    // we build groups of irp_threshold, each one in a different task
    for (int n=0; n<num_hlsinf_nodes; n++) {
      int O  = aNode[hlsinf_nodes[n]].O;
      int HI = aNode[hlsinf_nodes[n]].HI;
      for (int hi=0; hi<HI; hi+= irp_threshold) {
	eog[eog_entries[order]][order].node = hlsinf_nodes[n];
	eog[eog_entries[order]][order].cpu  = false;
	eog[eog_entries[order]][order].first_O = 0;
	eog[eog_entries[order]][order].last_O  = O-1;
	eog[eog_entries[order]][order].first_HI = hi;
	eog[eog_entries[order]][order].last_HI = min(HI-1, hi+irp_threshold-1);
	eog_entries[order]++;
      }
    }
  } else {
    // no parallelism at all, one task per node
    for (int n=0; n<num_hlsinf_nodes; n++) {
      int O  = aNode[hlsinf_nodes[n]].O;
      int HI = aNode[hlsinf_nodes[n]].HI;
      eog[eog_entries[order]][order].node = hlsinf_nodes[n];
      eog[eog_entries[order]][order].cpu  = false;
      eog[eog_entries[order]][order].first_O = 0;
      eog[eog_entries[order]][order].last_O = O-1;
      eog[eog_entries[order]][order].first_HI = 0;
      eog[eog_entries[order]][order].last_HI = HI-1;
      eog_entries[order]++;
    }
  }
}

/*
 * get_eog_entry
 *
 * This function provides the next execution order graph entry
 * in a mutex section, guaranteeing every thread will not 
 * share any eog task
 *
 */
int get_eog_entry(int order) {
  int eog_entry;
  pthread_mutex_lock(&lock); 
  if (current_eog_entry[order] >= eog_entries[order]) eog_entry = -1; else eog_entry = current_eog_entry[order];
  current_eog_entry[order]++;
  pthread_mutex_unlock(&lock);
  return eog_entry;
}

/*
 * run_execution_order()
 *
 * runs all nodes that belong to the same execution order
 * node parallelism is applied if enabled
 * First, an execution graph is built and then run in parallel
 *
 */
void run_execution_order(int order) {

  if (timings) fn_start_timer(200);

  if (verbose && verbose_level >= 3) printf("    running execution order %d\n", order);
  build_execution_order_graph(order);
  if (verbose && verbose_level >= 3) printf("      %d tasks found\n", eog_entries[order]);

  // cpu tasks
  if (enable_omp) {
    #pragma omp parallel for
    for (int e=0; e<eog_entries[order]; e++) {
      if (eog[e][order].cpu == true) {
        if (timings) fn_start_timer(e);
        fn_run_node_on_cpu(eog[e][order].node);
        if (timings) {fn_stop_timer(e); eog[e][order].accumulated_runtime += fn_get_timer(e); eog[e][order].num_runs++;}
      }
    }
  } else {
    for (int e=0; e<eog_entries[order]; e++) {
      if (eog[e][order].cpu == true) {
        if (timings) fn_start_timer(e);
        fn_run_node_on_cpu(eog[e][order].node);
        if (timings) {fn_stop_timer(e); eog[e][order].accumulated_runtime += fn_get_timer(e); eog[e][order].num_runs++;}
      }
    }
  }

  // fpga tasks
  current_eog_entry[order] = 0;
  #pragma omp parallel for
  for (int k=0; k<num_kernels; k++) {
    int e;
    do {
      e = get_eog_entry(order);
      if (e!=-1) if (eog[e][order].cpu == false) {
	if (timings) fn_start_timer(k);
        fn_run_node_on_fpga(eog[e][order].node, k, eog[e][order].first_O, eog[e][order].last_O, eog[e][order].first_HI, eog[e][order].last_HI);
	if (timings) {fn_stop_timer(k); eog[e][order].accumulated_runtime += fn_get_timer(k); eog[e][order].num_runs++;}
      }
    } while (e!=-1);
  }

  if (timings) {fn_stop_timer(200); num_runs[order]++; accumulated_runtime[order] += fn_get_timer(200);}
}


/*
 * run_graph()
 *
 * This function runs the graph on the FPGA
 *
 */
void run_graph() {
  
  // every group of nodes (in the same run order)  will run and we will collect stats for every run order. We will
  // acount for the number of runs and the accumulated time for the run order
  // we first reset the run statistics
  if (timings) {
    for (int e=0; e<MAX_EOG_ENTRIES; e++) for (int o=0; o<MAX_EXECUTION_ORDER; o++) {eog[e][o].num_runs = 0; eog[e][o].accumulated_runtime = 0;}
    for (int o=0; o<MAX_EXECUTION_ORDER; o++) {num_runs[o] = 0; accumulated_runtime[o] = 0;}
  }

  // now we read the node graph in execution order
  for (int order=0; order <= max_execution_order; order++) run_execution_order(order);
    
  if (timings) {
    // now we print statistics for every run order and task
    printf("| Graph timing statistics                                                                                                                  |\n");
    printf("|------------------------------------------------------------------------------------------|-----------------|------------|-----------------|-----------------|\n");
    printf("| Execution order / task                                                                   | conf.           |   #runs    | accum. runtime  |  avg. runtime   |\n");
    printf("|------------------------------------------------------------------------------------------|-----------------|------------|-----------------|-----------------|\n");
    for (int order=0; order <= max_execution_order; order++) {
      int nr = num_runs[order];
      unsigned long long crt = accumulated_runtime[order];
      unsigned long long art = crt / nr;
      printf("| execution order: %4d                                                                    |                 | %10d | %15lld | % 15lld |\n", order, nr, crt, art);
      for (int e=0; e<eog_entries[order]; e++) {
	int n = eog[e][order].node;
	nr = eog[e][order].num_runs;
	crt = eog[e][order].accumulated_runtime;
	art = crt / nr;
	char conf[40];
	if (is_hlsinf(n)) sprintf(conf, "O: %4d-%4d", eog[e][order].first_O, eog[e][order].last_O); else sprintf(conf, "cpu");
	printf("| %-88s | %-15s | %10d | %15lld | %15lld |\n", aNode[n].name, conf, nr, crt, art);
      }
      printf("|------------------------------------------------------------------------------------------|-----------------|------------|-----------------|-----------------|\n");
    }
  }
}

/*
 * copy_initializers_to_fpga()
 *
 * copies all initializers to the fpga buffers
 *
 */
void copy_initializers_to_fpga() {

  if (!xclbin_defined) {printf("WARNING: initializers not copied to FPGA since no XCLBIN has been specified\n"); return;}

  if (verbose && verbose_level >= 3) printf("copying initializers to FPGA...\n");
  for (int i=0; i<num_initializers; i++) {
    if (aInitializer[i].valid) {
      if (verbose && verbose_level >= 3) printf("  copying initializer %3d name %s\n", i, aInitializer[i].name);
      copy_to_fpga(aInitializer[i].buffer);
    }
  }
  if (verbose && verbose_level >= 3) printf("  completed\n");
}

#endif
