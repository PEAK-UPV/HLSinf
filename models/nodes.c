/*
 *
 * nodes.c
 *
 * This file provides support for nodes management from an onnx-exported text file
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "nodes.h"
#include "initializers.h"
#include "in_out.h"
#include "utils.h"
#include "main.h"

// global variables
int    num_nodes;
struct st_node aNode[MAX_NODES];

/*
 * fn_get_child_nodes()
 *
 * returns the number of childs of a node and a list of the childs
 *
 * The node parent is pased as an argument with its name
 *
 * The list is made of the ids of the nodes
 *
 */
int fn_get_child_nodes(char *name, int *list) {
  // we first get the entry of the node
  int node = fn_get_node_by_name(name);
  if (node == -1) {printf("Error, node not found in fn_get_child_nodes()\n"); exit(1);}
  // we sweep all nodes looking for nodes with an input to the node
  int num_childs = 0;
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      for (int i=0; i<aNode[n].num_inputs; i++) {
	for (int o=0; o<aNode[node].num_outputs; o++) {
          if (!strcmp(aNode[n].inputs[i], aNode[node].outputs[o])) {
            list[num_childs] = n;
            num_childs++;
	  }
        }
      }
    }
  }
  return num_childs;
}

/*
 *
 * fn_get_node_by_name()
 *
 * This function returns the node id from its name
 *
 * If not found it returns -1
 */
int fn_get_node_by_name(char *name) {
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      if (!strcmp(aNode[n].name, name)) return n;
    }
  }
  return -1;
}

/*
 * fn_get_node_by_output_name()
 *
 * returns the node id based on an output name
 *
 */
int fn_get_node_by_output_name(char *name) {
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      for (int o=0; o<aNode[n].num_outputs; o++) {
        if (!strcmp(aNode[n].outputs[o], name)) return n;
      }
    }
  }
  return -1;
}

/*
 *
 * is_node_by_name()
 *
 * returns whether the name passed as an argument is a node or not
 *
 */
int is_node_by_name(char *name) {
  for (int n=0; n<num_nodes; n++) if (aNode[n].valid && (!strcmp(aNode[n].name, name))) return true;
  return false;
}

/*
 *
 * is_hlsinf()
 *
 * returns whether the node id is an hlsinf node or not
 */
int is_hlsinf(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "HLSinf")) return true;
  return false;
}

/*
 *
 * is_h2d()
 *
 * returns whether the node id is a h2d node or not
 */
int is_h2d(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "h2d")) return true;
  return false;
}

/*
 *
 * is_d2h()
 *
 * returns whether the node id is a d2h node or not
 */
int is_d2h(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "d2h")) return true;
  return false;
}

/*
 * is_conv()
 *
 * Returns whether a node passed as its id is a conv node
 *
 */
int is_conv(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "Conv")) return true;
  return false;
}

/*
 * is_transpose()
 *
 * Returns whether a node passed as its id is a transpose node
 *
 */
int is_transpose(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "Transpose")) return true;
  return false;
}

/*
 * is_global_average_pool()
 *
 * Returns whether a node passed as its id is a global average pool node
 *
 */
int is_global_average_pool(int n) {
  if (n == -1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "GlobalAveragePool")) return true;
  return false;
}

/*
 * is_gemm()
 *
 * Returns whether a node passed as its id is a gemm node
 *
 */
int is_gemm(int n) {
  if (n == -1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "Gemm")) return true;
  return false;
}

/*
 * is_matmul()
 *
 * Returns whether the node is a matmul node
 *
 */
int is_matmul(int n) {
  if (n == -1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "MatMul")) return true;
  return false;
}

/*
 * is_concat()
 *
 * Returns whether the node is a concat
 *
 */
int is_concat(int n) {
  if (n == -1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "Concat")) return true;
  return false;
}

/*
 * is_flatten()
 *
 * Returns whether a node passed as its id is a flatten node
 *
 */
int is_flatten(int n) {
  if (n == -1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "Flatten")) return true;
  return false;
}

/*
 * is_relu()
 *
 * Returns whether a node passed as its id is a relu node
 *
 */
int is_relu(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "Relu")) return true;
  return false;
}

/*
 * is_maxpool()
 *
 * Returns whether a node passed as its id is a maxpool node
 *
 */
int is_maxpool(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "MaxPool")) return true;
  return false;
}

/*
 * is_avgpool()
 *
 * Returns whether a node passed as its id is a avgpool node
 *
 */
int is_avgpool(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "AveragePool")) return true;
  return false;
}

/*
 * is_bn()
 *
 * Returns whether a node passed as its id is a bn node
 *
 */
int is_bn(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "BatchNormalization")) return true;
  return false;
}

/*
 * is_add()
 *
 * Returns whether a node passed as its id is an add node
 *
 */
int is_add(int n) {
  if (n==-1) return false;
  if (!aNode[n].valid) return false;
  if (!strcmp(aNode[n].type, "Add")) return true;
  return false;
}

/*
 * add_input_to_node()
 *
 * Adds an input (name) to the list of inputs of the node (passed as an id)
 *
 */
void add_input_to_node(int n, char *name){
  if (n==-1) return;
  if (name==NULL) return;
  if (!aNode[n].valid) return;

  int num_inputs = aNode[n].num_inputs + 1;
  char **p = (char**)malloc(sizeof(char*) * num_inputs);
  for (int i=0; i<num_inputs;i++) {
    if (i!=num_inputs-1) {
      p[i] = (char*)malloc(sizeof(char) * (strlen(aNode[n].inputs[i])+1));
      strcpy(p[i], aNode[n].inputs[i]);
    } else {
      p[i] = (char*)malloc(sizeof(char) * (strlen(name)+1));
      strcpy(p[i], name);
    }
  }
  aNode[n].num_inputs = num_inputs;
  if (num_inputs != 1) {
    for (int i=0; i<num_inputs-1; i++) free(aNode[n].inputs[i]);
    if (aNode[n].inputs!=NULL) free(aNode[n].inputs);
  }

  aNode[n].inputs = p;
}

/*
 * remove_input_from_node()
 *
 * This function removes an input from a node
 *
 * The node id is passed as argument and the input name to remove
 *
 */
void remove_input_from_node(int n, char *name) {
  if (n==-1) return;
  if (name==NULL) return;
  if (!aNode[n].valid) return;
  if (aNode[n].num_inputs == 0) return;

  int ni = aNode[n].num_inputs;
  char **p;
  if (ni > 1) p = (char**)malloc(sizeof(char*) * ni-1); else p = NULL;
  int ci = 0;
  for (int i=0; i<ni; i++) {
    if (strcmp(aNode[n].inputs[i], name)) {
      p[ci] = (char*)malloc(sizeof(char) * (strlen(aNode[n].inputs[i])+1));
      strcpy(p[ci], aNode[n].inputs[i]);
      ci++;
    }
    free(aNode[n].inputs[i]);
  }
  free(aNode[n].inputs);
  aNode[n].inputs = p;
  aNode[n].num_inputs = aNode[n].num_inputs-1;
}


/*
 *
 * has_no_parents()
 *
 * returns whether the node passed as an argument (its id)
 * has no parents
 *
 */
int has_no_parents(int n) {
  if (n == -1) return false;
  if (!aNode[n].valid) return false;
  int list[100];
  int np = fn_get_parent_nodes(aNode[n].name, list);
  if (np == 0) return true;
  return false;
}

/*
 *
 * fn_get_first_node()
 *
 * returns the id of the node with no parents and with
 * an input equal as the name passed as argument. The
 * name is expected to be an input model name
 *
 * if not found returns -1
 */
int fn_get_first_node(char *name) {
  // we sweep all nodes looking for a node with no parents
  // and with name as an input
  for (int n=0; n<num_nodes; n++) {
    if (has_no_parents(n)) {
      for (int i=0; i<aNode[n].num_inputs; i++) {
        if (!strcmp(aNode[n].inputs[i], name)) return n;
      }
    }
  }
  return -1;
}

/*
 * fn_get_last_node()
 *
 * This function returns the id of the node with no childs and with
 * the output listed as an output model and passed as an argument
 *
 * if not found returns -1
 *
 */
int fn_get_last_node(char *name) {
  int n = fn_get_node_by_name(name);
  if (n==-1) return -1;
  int list[100];
  int nc = fn_get_child_nodes(name, list);
  if (nc != 0) return -1;
  return n;
}

/*
 *
 * fn_get_model_input_from_node()
 *
 * returns a model input from a node (from its id)
 *
 * It returns the initializer id
 */
int fn_get_model_input_from_node(int n) {
  if (n == -1) return -1;
  if (!aNode[n].valid) return -1;
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if ((!is_initializer(aNode[n].inputs[i])) && (!is_node_by_name(aNode[n].inputs[i]))) return get_model_input_id(aNode[n].inputs[i]);
  }
  return -1;
}

/*
 * fn_get_parent_by_input()
 *
 * This function returns the parent of a node input
 * As argument the function receives the specific input
 * name and the function searches a node with an output
 * with the same name
 *
 */
int fn_get_parent_by_input(char *name) {
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      for (int o=0; o<aNode[n].num_outputs; o++) {
	if (!strcmp(aNode[n].outputs[o], name)) return n;
      }
    }
  }
  return -1;
}

/*
 * fn_get_parent_nodes()
 *
 * returns the number of parents of a node and a list of its parents
 *
 * The list is made of the parent ids. The node is passed as an
 * argument with its name
 *
 */
int fn_get_parent_nodes(char *name, int *list) {
  int n = fn_get_node_by_name(name);
  if (n==-1) return 0;
  // we look for the inputs of the node that are not initializers
  int num_parents = 0;
  for (int i = 0; i<aNode[n].num_inputs; i++) {
    if (!is_initializer(aNode[n].inputs[i]) && !is_model_input(aNode[n].inputs[i])) {
      list[num_parents] = fn_get_parent_by_input(aNode[n].inputs[i]);
      num_parents++;
    }
  }
  return num_parents;
}

/*
 * fn_get_unallocated_node_with_partns_allocated()
 *
 * returns a node id which is not allocated (row == -1)
 * but all its parents are allocated (row != -1)
 *
 * Allocated means that the node is already mapped on the
 * execution graph (nodes with lower rows will run first)
 *
 * If not found it returns -1
 */
int fn_get_unallocated_node_with_parents_allocated() {
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      if (aNode[n].row == -1) {
        int plist[100];
        int np = fn_get_parent_nodes(aNode[n].name, plist);
        if (np != 0) {
  	  int found = true;
 	  for (int i=0; i<aNode[n].num_inputs; i++) {
	    if (!is_initializer(aNode[n].inputs[i])) {
	      int np = fn_get_parent_by_input(aNode[n].inputs[i]);
	      if (aNode[np].row == -1) found = false;
	    }
	  }
	  if (found) return n;
        } else {
	  // node with no parents, this is a good candidate!
	  return n;
	}
      }
    }
  }
  return -1;
}

/*
 * fn_check_all_parents_allocated()
 *
 * It checks whether all the parents of a node id have already
 * been allocated (row != -1)
 *
 */
int fn_check_all_parents_allocated(int n) {
  if (n==-1) return false;
  int list[100];
  int np = fn_get_parent_nodes(aNode[n].name, list);
  for (int p=0; p<np; p++) if (aNode[list[p]].row == -1) return false;
  return true;
}

/*
 * fn_allocate_nodes()
 *
 * Allocates all nodes in the execution graph.
 * For each node it specifies its row and columns.
 * A row indicates roughly the execution order, and a column
 * indicates the parallelism of execution
 */
void fn_allocate_nodes() {

  if (verbose) printf("    allocating nodes...\n");

  // we first initialize the col field to every node
  for (int n=0; n<num_nodes; n++) if (aNode[n].valid) {aNode[n].col = -1; aNode[n].row = -1;}

  // now we look for a node with its parents already allocated
  int end;
  do {
    int n = fn_get_unallocated_node_with_parents_allocated();

    //printf("encontrado!! n=%d\n", n);
    end = false;
    if (n == -1) end = true;
    if (!end) {
      if (verbose) printf("      allocating node: %d (%s)\n", n, aNode[n].name);
      // we now get the number of parents
      int plist[100];
      int np = fn_get_parent_nodes(aNode[n].name, plist);

      if (np == 0) {
	// a node with no parents is an initial node with running order 0
	aNode[n].row = 0;
	aNode[n].col = 0;
      } else {
	// we allocate the node with row order as the maximum row order of its parents plus one
	int row_max = -1;
	for (int p=0; p<np; p++) if (aNode[plist[p]].row > row_max) row_max = aNode[plist[p]].row;
	aNode[n].row = row_max + 1;
      }
    }
  } while (!end);

  if (verbose) printf("      completed\n");
}

void fn_get_dimensions_from_name(char *name, int *num_dimensions, int *dim0, int *dim1, int *dim2, int *dim3) {
  int i;
  // name is model input?
  i = get_model_input_id(name);
  if (i != -1) {
    *num_dimensions = aInput[i].num_dimensions;
    *dim0 = aInput[i].num_dimensions >= 1 ? aInput[i].dimensions[0] : 0;
    *dim1 = aInput[i].num_dimensions >= 2 ? aInput[i].dimensions[1] : 0;
    *dim2 = aInput[i].num_dimensions >= 3 ? aInput[i].dimensions[2] : 0;
    *dim3 = aInput[i].num_dimensions >= 4 ? aInput[i].dimensions[3] : 0;
    return;
  }
  // name is initializer?
  i = fn_get_initializer_by_name(name);
  if (i != -1) {
    *num_dimensions = aInitializer[i].num_dimensions;
    *dim0 = aInitializer[i].num_dimensions >= 1 ? aInitializer[i].dimensions[0] : 0;
    *dim1 = aInitializer[i].num_dimensions >= 2 ? aInitializer[i].dimensions[1] : 0;
    *dim2 = aInitializer[i].num_dimensions >= 3 ? aInitializer[i].dimensions[2] : 0;
    *dim3 = aInitializer[i].num_dimensions >= 4 ? aInitializer[i].dimensions[3] : 0;
    return;
  }
  // name is node?
  i = fn_get_node_by_output_name(name);
  if (i != -1) {
    *num_dimensions = 3;
    *dim0 = aNode[i].O;
    *dim1 = aNode[i].HO;
    *dim2 = aNode[i].WO;
    *dim3 = 0;
    return;
  }
  *num_dimensions = 0;
  *dim0 = 0;
  *dim1 = 0;
  *dim2 = 0;
  *dim3 = 0;
}

/*
 * fn_compute_data_geometries()
 *
 * Computes the input and output data geometries
 * for every node in the model
 *
 */
void fn_compute_data_geometries() {

  if (verbose) printf("computing data geometries...\n");

  // we allocate the nodes in the running order
  fn_allocate_nodes();
  // we get the maximum row
  int max_row = -1;
  for (int n=0; n<num_nodes; n++) if (aNode[n].valid) max_row = max(max_row, aNode[n].row);

  // now we sweep all nodes in row order
  for (int r=0; r<=max_row; r++) {
    for (int n=0; n<num_nodes; n++) {
      if (aNode[n].valid && (aNode[n].row == r)) {
        int num_dimensions;
	int dim0, dim1, dim2, dim3;
	fn_get_dimensions_from_name(aNode[n].inputs[0], &num_dimensions, &dim0, &dim1, &dim2, &dim3);   // input 0 is the data input
        if (num_dimensions == 4) {
          printf("WARNING: initializer has four dimensions, only three dimensions (1, 2, 3) will be considered (assuming first dimension is batch size)\n");
          aNode[n].I = dim1;
          aNode[n].HI = dim2 == 0 ? 1 : dim2;
          aNode[n].WI = dim3 == 0 ? 1 : dim3;
        } else {
          aNode[n].I  = dim0;
          aNode[n].HI = dim1 == 0 ? 1 : dim1;
          aNode[n].WI = dim2 == 0 ? 1 : dim2;
	}
	if (verbose) printf("  (parent)   node: %3d node_name: %-50s ---> I: %4d HI: %4d WI: %4d\n", n, aNode[n].name, aNode[n].I, aNode[n].HI, aNode[n].WI);

        if (is_conv(n)) {
	  int i = fn_get_initializer_by_name(aNode[n].inputs[1]);	// weight is input 1 in onnx
          aNode[n].O  = aInitializer[i].dimensions[0];
          aNode[n].HO = ((aNode[n].HI - aNode[n].kh + aNode[n].pt + aNode[n].pb)/aNode[n].sh) + 1;
          aNode[n].WO = ((aNode[n].WI - aNode[n].kw + aNode[n].pr + aNode[n].pl)/aNode[n].sw) + 1;
	  if (verbose) printf("  (conv)     node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
        } else if (is_maxpool(n)) {
          aNode[n].O  = aNode[n].I;
          aNode[n].HO = ((aNode[n].HI - aNode[n].kh + aNode[n].pt + aNode[n].pb)/aNode[n].sh) + 1;
          aNode[n].WO = ((aNode[n].WI - aNode[n].kw + aNode[n].pl + aNode[n].pr)/aNode[n].sw) + 1;
          if (verbose) printf("  (maxpool)  node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
        } else if (is_avgpool(n)) {
          aNode[n].O  = aNode[n].I;
          aNode[n].HO = ((aNode[n].HI - aNode[n].kh + aNode[n].pt + aNode[n].pb)/aNode[n].sh) + 1;
          aNode[n].WO = ((aNode[n].WI - aNode[n].kw + aNode[n].pl + aNode[n].pr)/aNode[n].sw) + 1;
          if (verbose) printf("  (avgpool)  node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
        } else if (is_transpose(n)) {
	  aNode[n].O = aNode[n].WI;
	  aNode[n].HO = aNode[n].HI;
	  aNode[n].WO = aNode[n].I;
	  if (verbose) printf("  (trans)    node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
	} else if (is_global_average_pool(n)) {
	  aNode[n].O  = aNode[n].I;
	  aNode[n].HO = 1;
	  aNode[n].WO = 1;
	  if (verbose) printf("  (gapool)   node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
	} else if (is_flatten(n)) {
	  aNode[n].O = aNode[n].I * aNode[n].HI * aNode[n].WI;
	  aNode[n].HO = 1;
	  aNode[n].WO = 1;
	  if (verbose) printf("  (flatten)  node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
	} else if (is_gemm(n)) {
	  int i_w = fn_get_initializer_by_name(aNode[n].inputs[1]); // weight is input 1 in onnx
	  if (i_w == -1) {printf("Error, could not get weight initializer from node\n"); exit(1);}
	  aNode[n].O = aInitializer[i_w].dimensions[0];   // in ONNX the output dimension is the first one
	  aNode[n].HO = 1;
	  aNode[n].WO = 1;
	  if (verbose) printf("  (gemm)     node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
	} else if (is_matmul(n)) {
	  int i_w = fn_get_initializer_by_name(aNode[n].inputs[1]); // weight is input 1 in onnx
	  if (i_w == -1) {printf("Error, could not get weight initializer from node\n"); exit(1);}
	  aNode[n].O = aInitializer[i_w].dimensions[1];   // in ONNX the output dimension is the second one
	  aNode[n].HO = 1;
	  aNode[n].WO = 1;
          if (verbose) printf("  (matmul)   node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
	} else if (is_concat(n)) {  // Todo, implement axis in concat
	  // dimension 0 will be added
	  int dim0_count = 0;
	  for (int x=0; x<aNode[n].num_inputs; x++) {
	    int i = fn_get_node_by_output_name(aNode[n].inputs[x]);
	    dim0_count += aNode[i].O;
	  }
	  aNode[n].O = dim0_count;
	  aNode[n].HO = aNode[n].HI;
	  aNode[n].WO = aNode[n].WI;
          if (verbose) printf("  (concat)   node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
        } else {
          aNode[n].O  = aNode[n].I; aNode[n].HO = aNode[n].HI; aNode[n].WO = aNode[n].WI;
          if (verbose) printf("  (other)    node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
	}
      }
    }
  }

  if (verbose) printf("  completed\n");
}

/*
 * get_node_entry()
 *
 * Obtains the id of a node by its name passed as an argument
 *
 */
int fn_get_node_entry(char *name) {
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      if (!strcmp(aNode[n].name, name)) return n;
    }
  }
  printf("Error, node %s not found\n", name);
  exit(1);
}

/*
 * get_node_name()
 *
 * Returns the name of a node by its id
 * If not found NULL is returned
 *
 */
char *get_node_name(int n) {
  if (n == -1) return NULL;
  if (!aNode[n].valid) return NULL;
  return (aNode[n].name);
}

/*
 * fn_remove_node()
 *
 * Removes a node passed as an id as argument
 *
 */
void fn_remove_node(int n) {
  if (n==-1) return;
  if (!aNode[n].valid) return;
  for (int i=0; i<aNode[n].num_inputs; i++) free(aNode[n].inputs[i]);
  free(aNode[n].inputs);
  for (int o=0; o<aNode[n].num_outputs; o++) free(aNode[n].outputs[o]);
  free(aNode[n].outputs);
  aNode[n].valid = 0;
}

/*
 * get_data_input_name_from_node()
 *
 * It returns the name of the input of a node. The
 * node is passed as an argument as an id, an exclude name
 * is passed as second argument
 *
 */
char *get_data_input_name_from_node(int n, char *exclude_name) {
  if (n == -1) return NULL;
  if (!aNode[n].valid) return NULL;

  // we sweep all its inputs and check they are not initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if ((!is_initializer(aNode[n].inputs[i])) && ((exclude_name==NULL) || strcmp(aNode[n].inputs[i], exclude_name))) {
      //printf("data_input_name: %s\n", aNode[n].inputs[i]);
      return aNode[n].inputs[i];
    }
  }
  return NULL;
}

/*
 * fn_relink()
 *
 * This function relinks nodes. Specifically
 * n1 and n2 nodes are passed as arguments
 * nodes pointing to n1 will now point to n2
 *
 */
void fn_relink_nodes(int n1, int n2) {
  if (n1 == -1) return;
  if (n2 == -1) return;
  if (!aNode[n1].valid) return;
  if (!aNode[n2].valid) return;

  // we search all outputs of n1
  for (int o=0; o<aNode[n1].num_outputs; o++) {
    for (int n=0; n<num_nodes; n++) {
      if (aNode[n].valid) {
	if (n != n1) {
          for (int i=0; i<aNode[n].num_inputs; i++) {
            if (!strcmp(aNode[n1].outputs[o], aNode[n].inputs[i])) {
              char *p = (char*)malloc(sizeof(char) * (strlen(aNode[n2].outputs[0])+1));
              strcpy(p, aNode[n2].outputs[0]);
              free(aNode[n].inputs[i]);
              aNode[n].inputs[i] = p;
            }
	  }
	}
      }
    }
  }
}

/*
 * fn_relink_node_inputs()
 *
 * Relinks all inputs by changing an old name by a new name.
 * Basically, the old_name is replaced by the new_name on every
 * input with the old_name.
 *
 */
//void fn_relink_node_inputs(char *old_name, char *new_name) {
//  if (old_name==NULL) return;
//
//  // we search all nodes and all of their inputs, changing the name of the input if found
//  for (int n=0; n<num_nodes; n++) {
//    if (aNode[n].valid) {
//      for (int i=0; i<aNode[n].num_inputs; i++) {
//        if (!strcmp(aNode[n].inputs[i], old_name)) {
//          char *p = (char*)malloc(sizeof(char) * (strlen(new_name)+1));
//          strcpy(p, new_name);
//          free(aNode[n].inputs[i]);
//          aNode[n].inputs[i] = p;
//        }
//      }
//    }
//  }
//}

/*
 * fn_add_node()
 *
 * Adds a node with a name and type specified in the arguments
 * Initially it has no inputs and its output is the same as its name
 *
 * It returns the id of the new node
 */
int fn_add_node(char *name, char *type) {
  int n = num_nodes;
  aNode[n].valid = true;
  aNode[n].name = (char*)malloc(sizeof(char) * (strlen(name)+1));
  strcpy(aNode[n].name, name);
  aNode[n].num_inputs = 0;
  aNode[n].type = (char*)malloc(sizeof(char) * (strlen(type)+1));
  strcpy(aNode[n].type, type);
  aNode[n].num_inputs = 0;
  aNode[n].inputs = NULL;
  aNode[n].num_outputs = 1;
  aNode[n].outputs = (char**)malloc(sizeof(char*) * 1);
  aNode[n].outputs[0] = (char*)malloc(sizeof(char) * (strlen(name)+1));
  strcpy(aNode[n].outputs[0], name);
  num_nodes=num_nodes+1;
  return n;
}

/*
 * fn_change_input_in_node()
 *
 * This function changes one input in a given node id.
 *
 * As arguments we receive the node id, the old input name
 * and the new one
 *
 */
void fn_change_input_in_node(int n, char *old_name, char *new_name) {
  if (n==-1) return;
  if (!aNode[n].valid) return;

  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (!strcmp(aNode[n].inputs[i], old_name)) {
      char *p = (char*)malloc(sizeof(char) * (strlen(new_name)+1));
      strcpy(p, new_name);
      free(aNode[n].inputs[i]);
      aNode[n].inputs[i] = p;
    }
  }
}


/*
 * fn_add_host_device_nodes()
 *
 * This function adds extra nodes for the host-to-device and device-to-host copy of buffers.
 * a h2d node will be added between a non-HLSinf parent node and a HLSinf child node. Similarly,
 * a d2h node will be added between a HLSinf parent node and a non-HLSinf child node.
 *
 */
void fn_add_host_device_nodes() {

  if (verbose) printf("adding h2d and d2h nodes...\n");

  // if the first node is an hlsinf node then we need to add a h2d node
  for (int i=0; i<num_inputs; i++) {
    if (aInput[i].valid) {
      int n = fn_get_first_node(aInput[i].name);
      if (n != -1) {
        if (is_hlsinf(n)) {
          // a first hlsinf node, we add a h2d node
	  char name[100];
	  sprintf(name, "h2d_%0d", num_nodes);
	  int nn = fn_add_node(name, (char*)"h2d");
	  // we add the input to the nn node
	  add_input_to_node(nn, aInput[i].name);
	  // we change the input data to the n node
	  fn_change_input_in_node(n, aInput[i].name, name);
	  // we set the geometry of n to nn
	  aNode[nn].I  = aNode[n].I;
	  aNode[nn].HI = aNode[n].HI;
	  aNode[nn].WI = aNode[n].WI;
	  aNode[nn].O  = aNode[nn].I;
	  aNode[nn].HO = aNode[nn].HO;
	  aNode[nn].WO = aNode[nn].WO;
        }
      }
    }
  }

  // if the last node is an hlsinf node then we need to add a d2h node
  for (int o=0; o<num_outputs; o++) {
    if (aOutput[o].valid) {
      int n = fn_get_last_node(aOutput[o].name);
      if (n != -1) {
        if (is_hlsinf(n)) {
          // a last hlsinf node, we add a d2h node
	  char name[100];
	  sprintf(name, "d2h_%0d", num_nodes);
	  int nn = fn_add_node(name, (char*)"d2h");
	  // we add the link from nn to n
	  add_input_to_node(nn, aNode[n].name);
	  // we change the output
	  fn_change_output_model_name(o, aNode[nn].name);
	  // we set the geometry of n to nn
          aNode[nn].I  = aNode[n].O;
          aNode[nn].HI = aNode[n].HO;
          aNode[nn].WI = aNode[n].WO;
          aNode[nn].O  = aNode[nn].I;
          aNode[nn].HO = aNode[nn].HO;
          aNode[nn].WO = aNode[nn].WO;
        }
      }
    }
  }

  // now we try to find intermediate needs for h2d and d2h nodes
  for (int n=0; n < num_nodes; n++) {
    if (aNode[n].valid) {
      if (is_hlsinf(n)) {
	// this is an hlsinf node, if one of its childs is a non-HLSinf layer and is not a d2h node then
	// we add a d2h node between both
	int list[100];
	int nc = fn_get_child_nodes(aNode[n].name, list);
	if (nc != 0) {
	  int child = list[0];
	  if (!is_d2h(child) && !is_hlsinf(child)) {
	    if (verbose) printf("  adding d2h between nodes %d (%s) and %d (%s)\n", n, aNode[n].name, child, aNode[child].name);
            // we need to add a d2c node in between
	    char name[100];
	    sprintf(name, "d2h_%0d", num_nodes);
	    int nn = fn_add_node(name, (char*)"d2h");
	    // we link all nodes pointing to n to point now to nn
	    fn_relink_nodes(n, nn);
	    //fn_relink_node_inputs(aNode[n].name, aNode[nn].name);
	    // we add a link between nn and n
	    add_input_to_node(nn, aNode[n].outputs[0]);
	    // we set the geometry of nn from n
	    aNode[nn].I  = aNode[n].O;
	    aNode[nn].HI = aNode[n].HO;
	    aNode[nn].WI = aNode[n].WO;
	    aNode[nn].O  = aNode[nn].I;
	    aNode[nn].HO = aNode[nn].HI;
	    aNode[nn].WO = aNode[nn].WI;
	  }
        } 
      }
    
      if (!is_hlsinf(n) && !is_h2d(n) && !is_d2h(n)) {
        // this is a host-based node, if one of its childs is a HLSinf layer then we add a h2d node between both
        // We annotate the first h2d we put so in the case more HLSinf childs are found they will share the same h2d node
        int h2d_already_placed = false;
        int h2d_node_placed = -1; 
        int list[100];
        int nc = fn_get_child_nodes(aNode[n].name, list);
	for (int c = 0; c<nc; c++) {
          int child = list[c];
          if (is_hlsinf(child)) {
            // if we already put a h2d node then we reuse it
            if (h2d_already_placed) {
              // the child will simply point to the h2d node
              fn_change_input_in_node(child, aNode[n].name, aNode[h2d_node_placed].name);
            } else {
              if (verbose) printf("  adding h2d between nodes %d (%s) and %d (%s)\n", n, aNode[n].name, child, aNode[child].name);
              // we need to add a d2c node in between
              char name[100];
              sprintf(name, "h2d_%0d", num_nodes);
              int nn = fn_add_node(name, (char*)"h2d");
              // the child node now points to nn
	      fn_change_input_in_node(child, aNode[n].outputs[0], aNode[nn].outputs[0]);
	      // now we add a link between nn and n
	      add_input_to_node(nn, aNode[n].outputs[0]);
              // we set the geometry of nn from n
              aNode[nn].I  = aNode[n].O;
              aNode[nn].HI = aNode[n].HO;
              aNode[nn].WI = aNode[n].WO;
              aNode[nn].O  = aNode[nn].I;
              aNode[nn].HO = aNode[nn].HI;
              aNode[nn].WO = aNode[nn].WI;
              //
              h2d_already_placed = true;
              h2d_node_placed = nn;
            }
          }
        }
      }
    }
  }
  //
  if (verbose) printf("  completed\n");
}

