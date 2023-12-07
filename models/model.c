/*
 *
 * model.c
 *
 * This file provides support for reading, writing and printing the model
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "nodes.h"
#include "initializers.h"
#include "utils.h"
#include "main.h"
#include "in_out.h"
#include "parsers.h"

/*
 *
 * fn_print_model()
 *
 * printf in stdout the model
 *
 */
void fn_print_model() {

  if (verbose==0) return;

  // we order the nodes
  fn_allocate_nodes();

	
  printf("model:\n");
  printf("  nodes:\n");
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      printf("  - id:%3d col:%3d row:%3d type:%-20s name:%-40s\n", n, aNode[n].col, aNode[n].row, aNode[n].type, aNode[n].name);
      if (!strcmp(aNode[n].type, "Conv")) printf("    conv_params: dh,dw: %d,%d kh,kw: %d,%d pt,pb,pl,pr: %d,%d,%d,%d sh,sw: %d,%d g: %d", aNode[n].dh, aNode[n].dw, aNode[n].kh, aNode[n].kw, aNode[n].pt, aNode[n].pb, aNode[n].pl, aNode[n].pr, aNode[n].sh, aNode[n].sw, aNode[n].g);
      if (!strcmp(aNode[n].type, "BatchNormalization")) printf("    bn_params: epsilon: %18.12f momentum: %18.12f\n", aNode[n].epsilon, aNode[n].momentum);
      if (!strcmp(aNode[n].type, "MaxPool")) printf("    pool_params: kh,kw: %d,%d pt,pb,pl,pr: %d,%d,%d,%d sh,sw: %d,%d\n", aNode[n].kh, aNode[n].kw, aNode[n].pt, aNode[n].pb, aNode[n].pl, aNode[n].pr, aNode[n].sh, aNode[n].sw);
      if (!strcmp(aNode[n].type, "HLSinf")) {
	printf("    hlsinf_params: has_conv: %s has_relu: %s has_bn: %s has_maxpool: %s has_add: %s", aNode[n].has_conv?"yes":"no", aNode[n].has_relu?"yes":"no", aNode[n].has_bn?"yes":"no", 
			                                                                         aNode[n].has_maxpool?"yes":"no", aNode[n].has_add?"yes":"no");
	if (aNode[n].has_conv) printf(" conv kh,kw: %d,%d sh,sw: %d,%d pt,pb,pl,pr: %d,%d,%d,%d", aNode[n].hlsinf_kh_conv, aNode[n].hlsinf_kw_conv, aNode[n].hlsinf_sh_conv, aNode[n].hlsinf_sw_conv,
			                                                                                       aNode[n].hlsinf_pt_conv, aNode[n].hlsinf_pb_conv, aNode[n].hlsinf_pl_conv, aNode[n].hlsinf_pr_conv);
	if (aNode[n].has_maxpool) printf(" maxpool kh,kw: %d,%d sh,sw: %d,%d", aNode[n].hlsinf_kh_maxpool, aNode[n].hlsinf_kw_maxpool, aNode[n].hlsinf_sh_maxpool, aNode[n].hlsinf_sw_maxpool);
	printf("\n");
      }
      printf("    data_params: input IxHxW: %dx%dx%d output OxHxW: %dx%dx%d", aNode[n].I, aNode[n].HI, aNode[n].WI, aNode[n].O, aNode[n].HO, aNode[n].WO);
      printf("\n");
      printf("    inputs:");
      for (int i=0; i<aNode[n].num_inputs; i++) printf(" %s", aNode[n].inputs[i]);
      printf("\n");
      printf("    outputs:");
      for (int o=0; o<aNode[n].num_outputs; o++) printf(" %s", aNode[n].outputs[o]);
      printf("\n");
    }
  }

  printf("Initializers:\n");
  for (int i=0; i<num_initializers; i++) {
    if (aInitializer[i].valid) {
      printf(" id:%3d name: %-50s", i, aInitializer[i].name);
      printf(" dims:");
      for (int d=0; d<aInitializer[i].num_dimensions; d++) printf(" %4d", aInitializer[i].dimensions[d]);
      printf("\n");
    }
  }

  printf("Inputs:\n");
  for (int i=0; i<num_inputs; i++) {
    if (aInput[i].valid) {
      printf("  - name: %s\n", aInput[i].name);
      printf("    dims:");
      for (int d=0; d<aInput[i].num_dimensions; d++) printf(" %4d", aInput[i].dimensions[d]);
      printf("\n");
      printf("    data type: %s\n", aInput[i].data_type);
    }
  }

  printf("Outputs:\n");
  for (int o=0; o<num_outputs; o++) {
    if (aOutput[o].valid) {
      printf("  - name: %s\n", aOutput[o].name);
    }
  }
}

/*
 *
 * fn_process_input_line()
 *
 * processes a text line which has been read from the model text file
 * This function obtains all the arguments from the line and builds the model
 *
 */
void fn_process_input_line(char *line, size_t len) {
  char item[200];
  char name[200];
  int offset = 0;

  if (verbose) printf("  line: %s", line);  // we do not add \n since the line already has one

  // first we get the item name
  offset = fn_get_item_line(line, len, offset, name);

  // now we get the line type: node, initializer, inputs, outputs
  offset = fn_get_item_line(line, len, offset, item);


  if (!strcmp(item, "node")) {
    aNode[num_nodes].valid = true;
    aNode[num_nodes].name = malloc(sizeof(char) * (strlen(name)+1));
    strcpy(aNode[num_nodes].name, name);
    // node type
    offset = fn_get_item_line(line, len, offset, item);
    aNode[num_nodes].type = malloc(sizeof(char) * (strlen(item)+1));
    strcpy(aNode[num_nodes].type, item);
    // number of inputs
    offset = fn_get_item_line(line, len, offset, item);
    aNode[num_nodes].num_inputs = atoi(item);
    // parents
    aNode[num_nodes].inputs = malloc(sizeof(char*) * aNode[num_nodes].num_inputs);
    for (int i = 0; i<aNode[num_nodes].num_inputs; i++) {
      offset = fn_get_item_line(line, len, offset, item);
      aNode[num_nodes].inputs[i] = malloc(sizeof(char) * (strlen(item) + 1));
      strcpy(aNode[num_nodes].inputs[i], item);
    }
    // number of outputs
    offset = fn_get_item_line(line, len, offset, item);
    aNode[num_nodes].num_outputs  = atoi(item);
    // childs
    aNode[num_nodes].outputs = malloc(sizeof(char*) * aNode[num_nodes].num_outputs);
    for (int o = 0; o<aNode[num_nodes].num_outputs; o++) {
      offset = fn_get_item_line(line, len, offset, item);
      aNode[num_nodes].outputs[o] = malloc(sizeof(char) * (strlen(item) + 1));
      strcpy(aNode[num_nodes].outputs[o], item);
    }
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
    //
    num_nodes++;
  }

  if (!strcmp(item, "initializer")) {
    aInitializer[num_initializers].valid = true;
    aInitializer[num_initializers].name = malloc(sizeof(char) * (strlen(name)+1));
    strcpy(aInitializer[num_initializers].name, name);
    // type
    if (strstr(name, "weight")!=NULL) strcpy(aInitializer[num_initializers].type, "weight");
    else if (strstr(name, "bias")!=NULL) strcpy(aInitializer[num_initializers].type, "bias");
    else if (strstr(name, "gamma")!=NULL) strcpy(aInitializer[num_initializers].type, "gamma");
    else if (strstr(name, "beta")!=NULL) strcpy(aInitializer[num_initializers].type, "beta");
    else if (strstr(name, "running_mean")!=NULL) strcpy(aInitializer[num_initializers].type, "running_mean");
    else if (strstr(name, "running_var")!=NULL) strcpy(aInitializer[num_initializers].type, "running_var");
    else {printf("unrecognized initializer type: %s\n", name); exit(1);}
    // number of dimensions
    offset = fn_get_item_line(line, len, offset, item);
    aInitializer[num_initializers].num_dimensions = atoi(item);
    // dimensions
    aInitializer[num_initializers].dimensions = malloc(sizeof(int) * aInitializer[num_initializers].num_dimensions);
    for (int d=0;d<aInitializer[num_initializers].num_dimensions; d++) {
      offset = fn_get_item_line(line, len, offset, item);
      aInitializer[num_initializers].dimensions[d] = atoi(item);
    }
    num_initializers++;
  }

  if (!strcmp(item, "inputs")) {
    aInput[num_inputs].valid = true;
    aInput[num_inputs].name = malloc(sizeof(char) * (strlen(name)+1));
    strcpy(aInput[num_inputs].name, name);
    // number of dimensions
    offset = fn_get_item_line(line, len, offset, item);
    aInput[num_inputs].num_dimensions = atoi(item);
    // dimensions
    aInput[num_inputs].dimensions = malloc(sizeof(int) * aInput[num_inputs].num_dimensions);
    for (int d=0; d<aInput[num_inputs].num_dimensions; d++) {
      offset = fn_get_item_line(line, len, offset, item);
      aInput[num_inputs].dimensions[d] = atoi(item);
    }
    // data type
    offset = fn_get_item_line(line, len, offset, item);
    aInput[num_inputs].data_type = malloc(sizeof(char) * (strlen(item) + 1));
    strcpy(aInput[num_inputs].data_type, item);
    //
    num_inputs++;
  }

  if (!strcmp(item, "outputs")) {
    aOutput[num_outputs].valid = true;
    aOutput[num_outputs].name = malloc(sizeof(char) * (strlen(name)+1));
    strcpy(aOutput[num_outputs].name, name);
    //
    num_outputs++;
  }
}

/*
 * fn_read_input_model
 *
 * reads the input model and builds the model graph
 *
 */
void fn_read_input_model() {
  FILE *fd;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  if (verbose) printf("reading input model (%s)...\n", input_file_name);

  if ((fd = fopen(input_file_name, "r"))==NULL) {
    printf("Error, could not open input file\n");
    exit(1);
  }

  // now we read lines until the end of the text file
  num_nodes = 0;
  num_initializers = 0;
  num_inputs = 0;
  num_outputs = 0;
  while ((read = getline(&line, &len, fd)) != -1) fn_process_input_line(line, len);

  fclose(fd);
  if (line) free(line);

  if (verbose) printf("  completed (nodes: %d, initializers: %d, inputs: %d, outputs: %d)\n", num_nodes, num_initializers, num_inputs, num_outputs);
}

/*
 * fn_generate_output_model
 *
 * generates the output model by processing the 
 * input model. It takes into account the HLSinf version and subversion
 */
void fn_generate_output_model() {

  if (verbose) printf("generating output model...\n");

  // depending on the HLSinf version and subversion we call the specific generate function
  switch(hlsinf_version) {
    case 1:  switch(hlsinf_subversion) {
	       case 0:  fn_generate_output_model_hlsinf_1_0(); break;
	       default: printf("Error, HLSinf subversion not supported\n"); exit(1); break;
	     }
	     break;
    default: printf("Error, HLSinf version not supported\n"); exit(1); break;
  }

  if (verbose) printf("  completed\n");
}

/*
 * fn_write_output_model
 *
 * writes the output model to disk
 *
 */
void fn_write_output_model() {

 if (verbose) printf("writing output model...\n");

 FILE *fd = fopen("model.hlsinf", "w");
 if (fd == NULL) {printf("Error, could not open the output model for writting\n"); exit(1);}

 // we order the nodes
 fn_allocate_nodes();

 // we compute the max row
 int max_row = -1;
 for (int n=0; n<num_nodes; n++) if (aNode[n].valid) max_row = max(max_row, aNode[n].row);

 // now we list the nodes by row order, nodes in the same row can be run in parallel
 for (int r=0; r<=max_row; r++) {
   for (int n=0; n<num_nodes; n++) {
     if (aNode[n].valid && (aNode[n].row == r)) {
       fprintf(fd, "node,%d,%s,%s", r, aNode[n].type, aNode[n].name);
       // inputs
       fprintf(fd, ",%d", aNode[n].num_inputs);
       for (int i=0; i<aNode[n].num_inputs; i++) fprintf(fd,",%s", aNode[n].inputs[i]);
       // outputs
       fprintf(fd,",%d", aNode[n].num_outputs);
       for (int o=0; o<aNode[n].num_outputs; o++) fprintf(fd,",%s", aNode[n].outputs[o]);
       // data params
       fprintf(fd,",%d,%d,%d,%d,%d,%d", aNode[n].I, aNode[n].HI, aNode[n].WI, aNode[n].O, aNode[n].HO, aNode[n].WO);
       // params
       if (is_hlsinf(n) && (aNode[n].has_conv)) fprintf(fd,",conv,%d,%d,%d,%d,%d,%d,%d,%d", aNode[n].hlsinf_kh_conv, aNode[n].hlsinf_kw_conv, aNode[n].hlsinf_sh_conv, aNode[n].hlsinf_sw_conv, aNode[n].hlsinf_pt_conv, aNode[n].hlsinf_pb_conv, aNode[n].hlsinf_pl_conv, aNode[n].hlsinf_pr_conv);
       if (is_hlsinf(n) && (aNode[n].has_relu)) fprintf(fd,",relu");
       if (is_hlsinf(n) && (aNode[n].has_bn)) fprintf(fd,",bn");
       if (is_hlsinf(n) && (aNode[n].has_maxpool)) fprintf(fd,",maxpool,%d,%d,%d,%d", aNode[n].hlsinf_kh_maxpool, aNode[n].hlsinf_kw_maxpool, aNode[n].hlsinf_sh_maxpool, aNode[n].hlsinf_sw_maxpool);
       if (is_hlsinf(n) && (aNode[n].has_add)) fprintf(fd,",add");
       if (is_conv(n)) fprintf(fd,",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", aNode[n].dh,aNode[n].dw,aNode[n].kh,aNode[n].kw,aNode[n].sh,aNode[n].sw,aNode[n].pt,aNode[n].pb,aNode[n].pl,aNode[n].pr,aNode[n].sh,aNode[n].sw,aNode[n].g);
       if (is_maxpool(n)) fprintf(fd,",%d,%d,%d,%d,%d,%d,%d,%d", aNode[n].kh, aNode[n].kw, aNode[n].sh, aNode[n].sw, aNode[n].pt, aNode[n].pb, aNode[n].pl, aNode[n].pr);
       fprintf(fd, "\n");
     }
   }
 }

 // we write also inputs and outputs of the model
 for (int i=0; i<num_inputs; i++) {
   if (aInput[i].valid) {
     fprintf(fd, "input,%s,%d", aInput[i].name, aInput[i].num_dimensions);
     for (int d=0; d<aInput[i].num_dimensions; d++) fprintf(fd,",%d", aInput[i].dimensions[d]);
     fprintf(fd, "\n");
   }
 }
 for (int o=0; o<num_outputs; o++) if (aOutput[o].valid) fprintf(fd, "output,%s\n", aOutput[o].name);

 // we write also initializers
 for (int i=0; i<num_initializers; i++) {
   if (aInitializer[i].valid) {
     fprintf(fd, "initializer,%s,%d", aInitializer[i].name, aInitializer[i].num_dimensions);
     for (int d=0; d<aInitializer[i].num_dimensions; d++) fprintf(fd, ",%d", aInitializer[i].dimensions[d]);
     fprintf(fd, "\n");
   }
 }
 fclose(fd);

 // now we write the initializers
 fn_write_initializers_data();
}
