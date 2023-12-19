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
 * The node parent is pased as an arument with its name
 *
 * The list is made of the ids of the nodes
 *
 */
int fn_get_child_nodes(char *name, int *list) {
  // we sweep all nodes looking for nodes with an input to the node name
  int num_childs = 0;
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      for (int i=0; i<aNode[n].num_inputs; i++) {
        if (!strcmp(aNode[n].inputs[i], name)) {
          list[num_childs] = n;
          num_childs++;
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
 * This function returns the id of the node with no chids and with
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
  // we look for the inputs of the now that are not initializers
  int num_parents = 0;
  for (int i = 0; i<aNode[n].num_inputs; i++) {
    if (!is_initializer(aNode[n].inputs[i]) && !is_model_input(aNode[n].inputs[i])) {
      list[num_parents] = fn_get_node_by_name(aNode[n].inputs[i]);
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
 * execution graph (nodes with lower rows will run first
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
	      int np = fn_get_node_by_name(aNode[n].inputs[i]);
	      if (aNode[np].row == -1) found = false;
	    }
	  }
	  if (found) return n;
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
  // we first initialize the col field to every node
  for (int n=0; n<num_nodes; n++) if (aNode[n].valid) {aNode[n].col = -1; aNode[n].row = -1;}

  // we allocate the first node, which is the one with a model input and no other inputs to nodes
  for (int i=0; i<num_inputs; i++) {
    if (aInput[i].valid) {
      int n = fn_get_first_node(aInput[i].name);
      if (n == -1) {printf("Error, first node not detected\n"); exit(1);}
      aNode[n].row = 0;
      aNode[n].col = 3 * (i + 1);
      //printf("first node %d allocated addt row %d col %d\n", n, aNode[n].row, aNode[n].col);
    }
  }

  // now we look for a node with its parents already allocated
  int end;
  do {
    int n = fn_get_unallocated_node_with_parents_allocated();

    //printf("encontrado!! n=%d\n", n);
    end = false;
    if (n == -1) end = true;
    if (!end) {
      // we now get the number of parents
      int plist[100];
      int np = fn_get_parent_nodes(aNode[n].name, plist);

      if (np == 1) {
	// for a single parent we look for the number of childs
	int p = plist[0];
	int clist[100];
	int nc = fn_get_child_nodes(aNode[p].name, clist);
	if (nc == 1) {
	  // one parent and one child, so we allocate this node
  	  aNode[n].row = aNode[p].row + 1;
	  aNode[n].col = aNode[p].col;
	  //printf("allocated node %d with row %d and col %d (one parent one child)\n", n, aNode[n].row, aNode[n].col);
	} else if (nc == 2) {
	  // we have a sibling, we allocate first this node
	  int s = (clist[0] == n) ? clist[1] : clist[0];
	  aNode[n].row = aNode[p].row + 1;
	  aNode[n].col = aNode[p].col + 1;
	  //printf("allocated node %d with row %d and col %d (one parent and one sibling\n", n, aNode[n].row, aNode[n].col);
	  // for the sibling we need to check whether all its parents are allocated
	  if (fn_check_all_parents_allocated(s)) {
  	    aNode[s].row = aNode[p].row + 1;
	    aNode[s].col = aNode[p].col - 1;
	    //printf("allocated sibling node %d with row %d and col %d\n", s, aNode[s].row, aNode[s].col);
	  }
	} else {
          printf("more than three childs not supported\n");
	  exit(1);
	}
      } else if (np == 2) {
	// we allocate the node in the middle col
	int p1 = plist[0];
	int p2 = plist[1];
	aNode[n].row = max(aNode[p1].row, aNode[p2].row) + 1;
	aNode[n].col = (aNode[p1].col + aNode[p2].col) / 2;
	//printf("allocated node %d with row %d and col %d (two parents)\n", n, aNode[n].row, aNode[n].col);
      }
    }
  } while (!end);
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

  // we first compute the data geometries of the outputs of the nodes
  fn_allocate_nodes();
  // maximum row
  int max_row = -1;
  for (int n=0; n<num_nodes; n++) if (aNode[n].valid) max_row = max(max_row, aNode[n].row);

  // now we sweep all nodes in row order
  for (int r=0; r<=max_row; r++) {
    for (int n=0; n<num_nodes; n++) {
      if (aNode[n].valid && (aNode[n].row == r)) {
        // the output geometry depends on the input geometry and the node type and its parameters
        if (has_no_parents(n)) {
          int i = fn_get_model_input_from_node(n);
          if (i==-1) {printf("Error, input not detected\n"); exit(1);}
          aNode[n].I  = aInput[i].dimensions[0];
          aNode[n].HI = aInput[i].dimensions[1];
          aNode[n].WI = aInput[i].dimensions[2];
	  if (verbose) printf("  (noparent) node: %3d node_name: %-50s ---> I: %4d HI: %4d WI: %4d\n", n, aNode[n].name, aNode[n].I, aNode[n].HI, aNode[n].WI);
        } else {
          int list[100];
          int np = fn_get_parent_nodes(aNode[n].name, list);
          if (np == 0) {printf("Error, no parents found\n"); exit(1);}
          int p = list[0];
          aNode[n].I  = aNode[p].O;
          aNode[n].HI = aNode[p].HO;
          aNode[n].WI = aNode[p].WO;
	  if (verbose) printf("  (parent)   node: %3d node_name: %-50s ---> I: %4d HI: %4d WI: %4d\n", n, aNode[n].name, aNode[n].I, aNode[n].HI, aNode[n].WI);
        }
        if (is_conv(n)) {
          int i = get_weight_initializer_entry_from_node(n);
          aNode[n].O  = aInitializer[i].dimensions[0];
          aNode[n].HO = ((aNode[n].HI - aNode[n].kh + aNode[n].pt + aNode[n].pb)/aNode[n].sh) + 1;
          aNode[n].WO = ((aNode[n].WI - aNode[n].kw + aNode[n].pr + aNode[n].pl)/aNode[n].sw) + 1;
	  if (verbose) printf("  (conv)     node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
        } else if (is_maxpool(n)) {
          aNode[n].O  = aNode[n].I;
          aNode[n].HO = ((aNode[n].HI - aNode[n].kh + aNode[n].pt + aNode[n].pb)/aNode[n].sh) + 1;
          aNode[n].WO = ((aNode[n].WI - aNode[n].kw + aNode[n].pl + aNode[n].pr)/aNode[n].sw) + 1;
          if (verbose) printf("  (mpool)    node: %3d node_name: %-50s ---> O: %4d HO: %4d WO: %4d\n", n, aNode[n].name, aNode[n].O, aNode[n].HO, aNode[n].WO);
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
 * fn_relink_node_inputs()
 *
 * Relinks all inputs by changing an old name by a new name.
 * Basically, the old_name is replaced by the new_name on every
 * input with the old_name.
 *
 */
void fn_relink_node_inputs(char *old_name, char *new_name) {
  if (old_name==NULL) return;

  // we search all nodes and all of their inputs, changing the name of the input if found
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      for (int i=0; i<aNode[n].num_inputs; i++) {
        if (!strcmp(aNode[n].inputs[i], old_name)) {
          char *p = (char*)malloc(sizeof(char) * (strlen(new_name)+1));
          strcpy(p, new_name);
          free(aNode[n].inputs[i]);
          aNode[n].inputs[i] = p;
        }
      }
    }
  }
}

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
	          fn_relink_node_inputs(aNode[n].name, aNode[nn].name);
	          // we add a link between nn and n
	          add_input_to_node(nn, aNode[n].name);
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
	            fn_change_input_in_node(child, aNode[n].name, aNode[nn].name);
	            // now we add a link between nn and n
	            add_input_to_node(nn, aNode[n].name);
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

