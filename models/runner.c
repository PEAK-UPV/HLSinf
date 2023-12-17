/*
 *
 * runner.c
 *
 * This file runs a model provided by its argument on a FPGA device using the xclbin file passed as argument
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "utils.h"
#include "in_out.h"
#include "nodes.h"
#include "initializers.h"
#include "main.h"
#include "fpga.h"
#include "cpu.h"

// global variables
int max_execution_order;


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

  if (verbose) {
    char sz_line[200];
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
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pt = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pb = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pl = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pr = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sw = atoi(item);
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
    // type
    offset = fn_get_item_line(line, len, offset, item);
    strcpy(aInitializer[num_initializers].type, item);
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

  if (verbose) printf("reading run graph (%s)...\n", input_file_name);

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

  if (verbose) printf("  completed (nodes: %d, inputs: %d, outputs: %d, initializers: %d)\n", num_nodes, num_inputs, num_outputs, num_initializers);
}

/*
 * run_graph()
 *
 * This function runs the graph on the FPGA
 *
 */
void run_graph() {
  
  if (verbose) printf("running graph\n");

  // we read the node graph in execution order
  for (int order=0; order < max_execution_order; order++) {
    for (int n=0; n<num_nodes; n++) {
      if ((aNode[n].valid) && (aNode[n].run_order == order)) {
        if (verbose) printf("  running: node: %3d order: %3d name: %-50s\n", n, order, aNode[n].name);
	if (!strcmp(aNode[n].type, "HLSinf")) fn_run_node_on_fpga(n);
	else fn_run_node_on_cpu(n);
      }
    }
  }

  if (verbose) printf("  completed\n");
}

/*
 * copy_initializers_to_fpga()
 *
 * copies all initializers to the fpga buffers
 *
 */
void copy_initializers_to_fpga() {

  if (verbose) printf("copying initializers to FPGA...\n");
  for (int i=0; i<num_initializers; i++) {
    if (aInitializer[i].valid) {
      if (verbose) printf("  copying initializer %3d name %s\n", i, aInitializer[i].name);
      copy_to_fpga(aInitializer[i].buffer);
    }
  }
  if (verbose) printf("  completed\n");
}
