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


// global variables
int           verbose;
char         *run_graph_file_name;
char         *xclbin_file_name;

/*
 *
 * fn_process_input_line()
 *
 * processes a text line with the configuration to run
 * This function obtains all the arguments from the line and builds the execution model
 *
 */
void fn_process_input_line(char *line, size_t len) {
  char item[200];
  int offset = 0;

  if (verbose) printf("  line: %s", line);  // we do not add \n since the line already has one

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

    // if we have a conv then we read the conv parameters (dh, dw, kh, kw, pt, pb, pl, pr, sh, sw, g)
    if (!strcmp(aNode[num_nodes].type, "Conv")) {
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].dh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].dw = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].kh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].kw = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pt = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pb = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pl = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].pr = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sh = atoi(item);
      offset = fn_get_item_line(line, len, offset, item); aNode[num_nodes].sw = atoi(item);
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

  if (verbose) printf("reading run graph (%s)...\n", run_graph_file_name);

  if ((fd = fopen(run_graph_file_name, "r"))==NULL) {
    printf("Error, could not open input file\n");
    exit(1);
  }

  // now we read lines until the end of the text file
  num_nodes = 0;
  num_inputs = 0;
  num_outputs = 0;
  num_initializers = 0;
  while ((read = getline(&line, &len, fd)) != -1) fn_process_input_line(line, len);

  fclose(fd);
  if (line) free(line);

  if (verbose) printf("  completed (nodes: %d, inputs: %d, outputs: %d, initializers: %d)\n", num_nodes, num_inputs, num_outputs, num_initializers);
}

/*
 * fn_parse_arguments()
 *
 * parses the arguments
 *
 */
void fn_parse_arguments(int argc, char *argv[]) {

  if (argc!=4) {
    printf("usage: <%s> <run_graph> <xclbin> <verbose>\n", argv[0]);
    exit(1);
  }
  run_graph_file_name = (char*)malloc(sizeof(char) * (strlen(argv[1])+1));
  strcpy(run_graph_file_name, argv[1]);
  xclbin_file_name = (char*)malloc(sizeof(char) * (strlen(argv[2])+1));
  strcpy(xclbin_file_name, argv[2]);
  verbose = atoi(argv[3]);
}
