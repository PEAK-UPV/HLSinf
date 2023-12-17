/*
 * initializers.c
 *
 * This files provides support for initializers obtained from a onnx file
 * exported to a txt file
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "initializers.h"
#include "utils.h"
#include "nodes.h"
#include "main.h"

// global variables
int    num_initializers;
struct st_initializer aInitializer[MAX_INITIALIZERS];

/*
 * fn_get_initializer_by_name()
 *
 * Returns the initializer id that corresponds to a name
 * passed as an argument
 *
 * If not found it returns -1
 *
 */
int fn_get_initializer_by_name(char *name) {
  if (name == NULL) return -1;
  for (int i=0; i<num_initializers; i++) if (aInitializer[i].valid) if (!strcmp(aInitializer[i].name, name)) return i;
  return -1;
}

/*
 * is_initializer()
 *
 * returns whether the name passed as an argument is an initializer
 *
 */
int is_initializer(char *name) {
  for (int i=0; i<num_initializers; i++) {
    if (aInitializer[i].valid) {
      if (!strcmp(aInitializer[i].name, name)) return true;
    }
  }
  return false;
}

/*
 * is_weight_initializer()
 *
 * returns whether a name is a weight initializer
 *
 */
int is_weight_initializer(char *name) {
  if (!is_initializer(name)) return false;
  int i = fn_get_initializer_by_name(name);
  if (i==-1) return false;
  if (!strcmp(aInitializer[i].type, "weight")) return true;
  return false;  
}

/*
 * is_bias_initializer()
 *
 * returns whether a name is a bias initializer
 *
 */
int is_bias_initializer(char *name) {
  if (!is_initializer(name)) return false;
  int i = fn_get_initializer_by_name(name);
  if (i==-1) return false;
  if (!strcmp(aInitializer[i].type, "bias")) return true;
  return false;
}

/*
 * is_gamma_initializer()
 *
 * returns whether a name is a gamma initializer
 *
 */
int is_gamma_initializer(char *name) {
  if (!is_initializer(name)) return false;
  int i = fn_get_initializer_by_name(name);
  if (i==-1) return false;
  if (!strcmp(aInitializer[i].type, "gamma")) return true;
  return false;
}

/*
 * is_beta_initializer()
 *
 * returns whether a name is a beta initializer
 *
 */
int is_beta_initializer(char *name) {
  if (!is_initializer(name)) return false;
  int i = fn_get_initializer_by_name(name);
  if (i==-1) return false;
  if (!strcmp(aInitializer[i].type, "beta")) return true;
  return false; 
}

/*
 * is_running_mean_initializer()
 *
 * returns whether a name is a running_mean initializer
 *
 */
int is_running_mean_initializer(char *name) {
  if (!is_initializer(name)) return false;
  int i = fn_get_initializer_by_name(name);
  if (i==-1) return false;
  if (!strcmp(aInitializer[i].type, "running_mean")) return true;
  return false; 
}

/*
 * is_running_var_initializer()
 *
 * returns whether a name is a running_var initializer
 *
 */
int is_running_var_initializer(char *name) {
  if (!is_initializer(name)) return false;
  int i = fn_get_initializer_by_name(name);
  if (i==-1) return false;
  if (!strcmp(aInitializer[i].type, "running_var")) return true;
  return false; 
}

/*
 * is_bn_initializer()
 *
 * returns whether a name is a bn initializer
 *
 */
int is_bn_initializer(char *name) {
  if (!is_initializer(name)) return false;
  int i = fn_get_initializer_by_name(name);
  if (i==-1) return false;
  if (!strcmp(aInitializer[i].type, "bn")) return true;
  return false;
}

/*
 * fn_read_initializers_data()
 *
 * This function reads the data from initializer files.
 * This function asumes a file.data file exists with its
 * name (file) equal to the initializer's name.
 *
 * Data is loaded on dynamically allocated buffers
 */
void fn_read_initializers_data() {
  FILE *fd;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;  

  if (verbose) printf("reading initializers data...\n");
  
  // we read every initializer
  for (int i=0; i<num_initializers; i++) {
    if (aInitializer[i].valid) {
      char filename[100];
      sprintf(filename, "%s.data", aInitializer[i].name);
      if ((fd = fopen(filename, "r")) == NULL) {printf("Error, could not open initializer file %s\n", aInitializer[i].name); exit(1);}
      // first row is the number of items
      read = getline(&line, &len, fd);
      int num_items = atoi(line);
      if (verbose) printf("  initializer: %-50s num_items: %12d file: %-50s\n", aInitializer[i].name, num_items, filename);
      // now we read the number of dimensions
      read = getline(&line, &len, fd);
      aInitializer[i].num_dimensions = atoi(line);
      aInitializer[i].dimensions = (int*)malloc(sizeof(int) * aInitializer[i].num_dimensions);
      // now we read each dimension
      for (int d=0; d<aInitializer[i].num_dimensions; d++) {
	read = getline(&line, &len, fd);
	aInitializer[i].dimensions[d] = atoi(line);
      }
      // now we read the data
      posix_memalign((void**)&aInitializer[i].data, 4096, sizeof(float) * num_items);
      //aInitializer[i].data = (float*)malloc(sizeof(float) * num_items);
      for (int d = 0; d < num_items; d++) {
        read = getline(&line, &len, fd);
	if (read == -1) {printf("error, while reading initializer %s\n", aInitializer[i].name); exit(1);}
        aInitializer[i].data[d] = atof(line);
      }
      fclose(fd);
    }
  }

  if (verbose) printf("  completed\n");
}

/*
 * fn_create_zero_initializers()
 *
 * This function checks whether all initializers exist. If for a
 * layer an initializer does not exist (e.g. bias for a conv layer) then
 * it is created and initialized to zeros
 */
void fn_create_zero_initializers() {

  if (verbose) printf("Creating zero initializers...\n");

  // we search all the nodes
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      if (is_conv(n)) {
	if (get_bias_initializer_name_from_node(n)==NULL) {
          // we create a bias initializer
	  char name[300];
	  sprintf(name, "bias_%s", aNode[n].name);
	  int num_items = aNode[n].O;
	  fn_add_new_initializer(name, (char*)"bias", num_items, NULL);
	  add_input_to_node(n, name);
	  if (verbose) printf("  Bias initializer %s (layer %s)\n", name, aNode[n].name);
	}
	if (get_weight_initializer_name_from_node(n)==NULL) {
	  printf("Error, weights for node %s not found\n", aNode[n].name);
	  exit(1);
	}
      }
      if (is_bn(n)) {
	if (get_gamma_initializer_name_from_node(n)==NULL) {
	  printf("Error, gamma initializer for node %s not found\n", aNode[n].name);
	  exit(1);
	}
        if (get_beta_initializer_name_from_node(n)==NULL) {
          printf("Error, beta initializer for node %s not found\n", aNode[n].name);
          exit(1);
        }
        if (get_running_var_initializer_name_from_node(n)==NULL) {
          printf("Error, running_var initializer for node %s not found\n", aNode[n].name);
          exit(1);
        }
        if (get_running_mean_initializer_name_from_node(n)==NULL) {
          printf("Error, running_mean initializer for node %s not found\n", aNode[n].name);
          exit(1);
        }
      }
    }
  }
  if (verbose) printf("  completed\n");
}

/*
 * fn_add_new_initializer()
 *
 * Adds an initializer and all its data is set to zeros
 * The name of the initializer, its type, and its number of items
 * is passed as an argument. 
 *
 * Also, a pointer to float data is passed
 * as argument, if the pointer is NULL then data is created and zeroed,
 * otherwhise the pointer is used and linked to data
 *
 * One dimension is asumed
 */
void fn_add_new_initializer(char *name, char *type, int num_items, float *d) {

  aInitializer[num_initializers].valid = true;
  aInitializer[num_initializers].name = (char*)malloc(sizeof(char) * (strlen(name)+1));
  strcpy(aInitializer[num_initializers].name, name);
  strcpy(aInitializer[num_initializers].type, type);
  aInitializer[num_initializers].num_dimensions = 1;
  aInitializer[num_initializers].dimensions = (int*)malloc(sizeof(int) * 1);
  aInitializer[num_initializers].dimensions[0] = num_items;
  if (d==NULL) {
    aInitializer[num_initializers].data = (float*)malloc(sizeof(float) * num_items);
    for (int x=0; x<num_items; x++) aInitializer[num_initializers].data[x] = 0.f;
  } else {
    aInitializer[num_initializers].data = d;
  }
  num_initializers++;
}

/*
 * fn_write_initializers_data()
 *
 * This function writes the data for each initializer in its own file
 * This function asumes a file.data file will be produced with its
 * name (file) equal to the initializer's name
 *
 */
void fn_write_initializers_data() {
  FILE *fd;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  if (verbose) printf("writing initializers data...\n");

  // we write every initializer
  for (int i=0; i<num_initializers; i++) {
    if (aInitializer[i].valid) {
      char filename[100];
      sprintf(filename, "%s.data", aInitializer[i].name);
      if ((fd = fopen(filename, "w")) == NULL) {printf("Error, could not open initializer file %s\n", aInitializer[i].name); exit(1);}
      if (verbose) printf("  file: %s\n", filename);
      // num items
      int num_items = 1;
      for (int d=0; d<aInitializer[i].num_dimensions; d++) num_items = num_items * aInitializer[i].dimensions[d];
      fprintf(fd, "%d\n", num_items);
      // dimensions
      fprintf(fd, "%d\n", aInitializer[i].num_dimensions);
      for (int d=0; d<aInitializer[i].num_dimensions; d++) fprintf(fd, "%d\n", aInitializer[i].dimensions[d]);
      // data
      for (int d = 0; d < num_items; d++) fprintf(fd, "%f\n", aInitializer[i].data[d]);
      fclose(fd);
    }

  }
}
	


/*
 * get_weight_initializer_name_from_node()
 *
 * Returns the weight initializer name from
 * a node passed as an argument as id
 *
 * If not found it returns NULL
 */
char *get_weight_initializer_name_from_node(int n) {
  if (n == -1) return NULL;
  if (!aNode[n].valid) return NULL;

  // we sweep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_weight_initializer(aNode[n].inputs[i])) return aNode[n].inputs[i];
  }
  return NULL;
}

/*
 * get_weight_initializer_entry_from_node()
 *
 * Returns the weight initializer id from
 * a node passed as an argument as id
 *
 * If not found it returns -1
 */
int get_weight_initializer_entry_from_node(int n) {
  if (n == -1) return -1;
  if (!aNode[n].valid) return -1;

  // we seep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_weight_initializer(aNode[n].inputs[i])) return fn_get_initializer_by_name(aNode[n].inputs[i]);
  }
  return -1;
}

/*
 * get_bias_initializer_name_from_node()
 *
 * Returns the bias initializer name from
 * a node passed as an argument as id
 *
 * If not found it returns NULL
 */
char *get_bias_initializer_name_from_node(int n) {
  if (n == -1) return NULL;
  if (!aNode[n].valid) return NULL;

  // we sweep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_bias_initializer(aNode[n].inputs[i])) return aNode[n].inputs[i];
  }
  return NULL;
}

/*
 * get_bias_initializer_entry_from_node()
 *
 * Returns the bias initializer id from
 * a node passed as an argument as id
 *
 * If not found it returns -1
 */
int get_bias_initializer_entry_from_node(int n) {
  if (n == -1) return -1;
  if (!aNode[n].valid) return -1;

  // we seep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_bias_initializer(aNode[n].inputs[i])) return fn_get_initializer_by_name(aNode[n].inputs[i]);
  }
  return -1;
}


/*
 * get_gamma_initializer_name_from_node()
 *
 * Returns the gamma initializer name from
 * a node passed as an argument as id
 *
 * If not found it returns NULL
 */
char *get_gamma_initializer_name_from_node(int n) {
  if (n == -1) return NULL;
  if (!aNode[n].valid) return NULL;

  // we sweep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_gamma_initializer(aNode[n].inputs[i])) return aNode[n].inputs[i];
  }
  return NULL;
}

/*
 * get_gamma_initializer_id_from_node()
 *
 * Returns the gamma initializer id from
 * a node passed as an argument as id
 *
 * If not found it returns -1
 *
 */
int get_gamma_initializer_id_from_node(int n) {
  int i = fn_get_initializer_by_name(get_gamma_initializer_name_from_node(n));
  return i;
}

/*
 * get_beta_initializer_name_from_node()
 *
 * Returns the beta initializer name from
 * a node passed as an argument as id
 *
 * If not found it returns NULL
 */
char *get_beta_initializer_name_from_node(int n) {
  if (n == -1) return NULL;
  if (!aNode[n].valid) return NULL;

  // we sweep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_beta_initializer(aNode[n].inputs[i])) return aNode[n].inputs[i];
  }
  return NULL;
}

/*
 * get_beta_initializer_id_from_node()
 *
 * Returns the beta initializer id from
 * a node passed as an argument as id
 *
 * If not found it returns -1
 *
 */
int get_beta_initializer_id_from_node(int n) {
  int i = fn_get_initializer_by_name(get_beta_initializer_name_from_node(n));
  return i;
}

/*
 * get_running_mean_initializer_name_from_node()
 *
 * Returns the running_mean initializer name from
 * a node passed as an argument as id
 *
 * If not found it returns NULL
 */
char *get_running_mean_initializer_name_from_node(int n) {
  if (n == -1) return NULL;
  if (!aNode[n].valid) return NULL;

  // we sweep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_running_mean_initializer(aNode[n].inputs[i])) return aNode[n].inputs[i];
  }
  return NULL;
}

/*
 * get_running_mean_initializer_id_from_node()
 *
 * Returns the running_mean initializer id from
 * a node passed as an argument as id
 *
 * If not found it returns -1
 *
 */
int get_running_mean_initializer_id_from_node(int n) {
  int i = fn_get_initializer_by_name(get_running_mean_initializer_name_from_node(n));
  return i;
}
/*
 * get_running_var_initializer_name_from_node()
 *
 * Returns the running_var initializer name from
 * a node passed as an argument as id
 *
 * If not found it returns NULL
 */
char *get_running_var_initializer_name_from_node(int n) {
  if (n == -1) return NULL;
  if (!aNode[n].valid) return NULL;

  // we sweep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_running_var_initializer(aNode[n].inputs[i])) return aNode[n].inputs[i];
  }
  return NULL;
}

/*
 * get_running_var_initializer_id_from_node()
 *
 * Returns the running_var initializer id from
 * a node passed as an argument as id
 *
 * If not found it returns -1
 *
 */
int get_running_var_initializer_id_from_node(int n) {
  int i = fn_get_initializer_by_name(get_running_var_initializer_name_from_node(n));
  return i;
}

/*
 * get_bn_initializer_name_from_node()
 *
 * Returns the bn initializer name from
 * a node passed as an argument as id
 *
 * If not found it returns NULL
 */
char *get_bn_initializer_name_from_node(int n) {
  if (n == -1) return NULL;
  if (!aNode[n].valid) return NULL;

  // we sweep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_bn_initializer(aNode[n].inputs[i])) return aNode[n].inputs[i];
  }
  return NULL;
}

/*
 * get_bn_initializer_id_from_node()
 *
 * Returns the bn initializer id from a node
 * passed as an argument as id
 *
 * If not found it returns -1
 *
 */
int get_bn_initializer_id_from_node(int n) {
  int i = fn_get_initializer_by_name(get_bn_initializer_name_from_node(n));
  return i;
} 

/*
 *
 * fn_pad_weight_initializer_1x1_to_3x3()
 *
 * This function adapts by padding a OxIx1x1 weight filter to a 
 * OxIx3x3 filter
 */
void fn_pad_weight_initializer_1x1_to_3x3(char *name) {

  int i = fn_get_initializer_by_name(name);
  if (i==-1) {printf("Error, could not get initializer id\n"); exit(1);}

  int num_dims = aInitializer[i].num_dimensions;
  int num_items = 1;
  for (int d=0; d<num_dims; d++) num_items = num_items * aInitializer[i].dimensions[d];
  int O = aInitializer[i].dimensions[0];
  int I = aInitializer[i].dimensions[1];
  int KH = aInitializer[i].dimensions[2];
  int KW = aInitializer[i].dimensions[3];
  int KH_new = 3;
  int KW_new = 3;

  if (verbose) printf("      pading 1x1 weight initializer to 3x3: name: %s OxIxKHxKW: %3dx%3dx%3dx%3d\n", name, O, I, KH, KW);

  size_t new_size = KH_new * KW_new * O * I;
  float *p = (float*)malloc(sizeof(float) * new_size);
  size_t cnt = 0;
  for (size_t x=0; x<num_items; x++) {
    p[cnt] = aInitializer[i].data[x];
    cnt++;
    for (int xx=0; xx<8; xx++) p[cnt+xx] = 0.f;
    cnt+=8;
  }
  free(aInitializer[i].data);
  aInitializer[i].data = p;
  // new dimensions
  aInitializer[i].dimensions[2] = KH_new;
  aInitializer[i].dimensions[3] = KW_new;
}

/*
 * remove_initializer()
 *
 * This function removes an initializer
 *
 * The id is passed as argument
 *
 */
void remove_initializer(int i) {
  if (i==-1) return;
  if (!aInitializer[i].valid) return;
  free(aInitializer[i].name);
  free(aInitializer[i].dimensions);
  if (aInitializer[i].data != NULL) free(aInitializer[i].data);
  aInitializer[i].valid = false;
};
