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
void fn_add_new_initializer(char *name, int num_items, float *d) {

  aInitializer[num_initializers].valid = true;
  aInitializer[num_initializers].name = (char*)malloc(sizeof(char) * (strlen(name)+1));
  strcpy(aInitializer[num_initializers].name, name);
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
 * get_initializer_entry_from_node()
 *
 * Returns the initializer id from a node passed
 * as an argument as id
 *
 * If not found it returns -1
 */
int get_initializer_entry_from_node(int n) {
  if (n == -1) return -1;
  if (!aNode[n].valid) return -1;

  // we sweep all its inputs and check whether they are initializers
  for (int i=0; i<aNode[n].num_inputs; i++) {
    if (is_initializer(aNode[n].inputs[i])) return fn_get_initializer_by_name(aNode[n].inputs[i]);
  }
  return -1;
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
