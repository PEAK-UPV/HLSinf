/*
 * initializers.c
 *
 * This files provides support for initializers obtained from a onnx file
 * exported to a txt file.
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
int    num_initializers;                               // number of initializers
struct st_initializer aInitializer[MAX_INITIALIZERS];  // vector of initializers

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
  // double check null name
  if (name == NULL) return -1;
  // we sweep all initializers, if there is a coincidence by name then we return the entry
  for (int i=0; i<num_initializers; i++) if (aInitializer[i].valid) if (!strcmp(aInitializer[i].name, name)) return i;
  // otherwise we return -1 (not found)
  return -1;
}

/*
 * is_initializer()
 *
 * returns whether the name passed as an argument is an initializer
 *
 */
int is_initializer(char *name) {
  // we sweep all initializers, if there is a coincidence by name then we return true
  for (int i=0; i<num_initializers; i++) {
    if (aInitializer[i].valid) {
      if (!strcmp(aInitializer[i].name, name)) return true;
    }
  }
  // otherwise we return false
  return false;
}

/*
 * fn_add_new_initializer()
 *
 * Adds an initializer. The name of the initializer, 
 * its type, and its number of items
 * is passed as an argument.
 *
 * Also, a pointer to float data is passed
 * as argument, if the pointer is NULL then data is created and zeroed,
 * otherwhise the pointer is used and linked to data.
 *
 * One dimension is asumed.
 */
void fn_add_new_initializer(char *name, int num_items, float *d) {

  // we initialize some fields (valid and name)
  aInitializer[num_initializers].valid = true;
  aInitializer[num_initializers].name = (char*)malloc(sizeof(char) * (strlen(name)+1));
  strcpy(aInitializer[num_initializers].name, name);
  // just one dimension assumed
  aInitializer[num_initializers].num_dimensions = 1;
  // dimensions
  aInitializer[num_initializers].dimensions = (int*)malloc(sizeof(int) * 1);
  aInitializer[num_initializers].dimensions[0] = num_items;
  // let's zero the values or just assign the values passed as argument
  if (d==NULL) {
    aInitializer[num_initializers].data = (float*)malloc(sizeof(float) * num_items);
    for (int x=0; x<num_items; x++) aInitializer[num_initializers].data[x] = 0.f;
  } else {
    aInitializer[num_initializers].data = d;
  }
  // ready for next initializer
  num_initializers++;
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

  if (verbose && verbose_level >= 3) printf("      pading 1x1 weight initializer to 3x3: name: %s OxIxKHxKW: %3dx%3dx%3dx%3d\n", name, O, I, KH, KW);

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
 *
 * fn_pad_weight_initializer_2x2_to_3x3()
 *
 * This function adapts by padding a OxIx2x2 weight filter to a
 * OxIx3x3 filter
 */
void fn_pad_weight_initializer_2x2_to_3x3(char *name) {

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

  if (verbose && verbose_level >= 3) printf("      pading 2x2 weight initializer to 3x3: name: %s OxIxKHxKW: %3dx%3dx%3dx%3d\n", name, O, I, KH, KW);

  size_t new_size = KH_new * KW_new * O * I;
  float *p = (float*)malloc(sizeof(float) * new_size);
  size_t cnt = 0;
  for (size_t x=0; x<num_items; x=x+4) {
    p[cnt]   = aInitializer[i].data[x];
    p[cnt+1] = aInitializer[i].data[x+1];
    p[cnt+2] = 0.f;
    p[cnt+3] = aInitializer[i].data[x+2];
    p[cnt+4] = aInitializer[i].data[x+3];
    p[cnt+5] = 0.f;
    p[cnt+6] = 0.f;
    p[cnt+7] = 0.f;
    p[cnt+8] = 0.f;
    cnt+=9;
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

/*
 * fn_pad_weights()
 *
 * resizes the initializer with new I and O
 *
 */
void fn_pad_weights(int i, int new_I, int new_O) {
  if (i==-1) return;
  if (!aInitializer[i].valid) return;

  int O = aInitializer[i].dimensions[0];
  int I = aInitializer[i].dimensions[1];
  int KH = aInitializer[i].dimensions[2];
  int KW = aInitializer[i].dimensions[3];

  size_t new_num_items = new_I * new_O * KH * KW;
  float *p = (float *)malloc(sizeof(float) * new_num_items);
  memset(p, 0, sizeof(float) * new_num_items);
  for (int ii=0; ii<I; ii++) {
    for (int oo=0; oo<O; oo++) {
      for (int kh = 0; kh<KH; kh++) {
	for (int kw = 0; kw<KW; kw++) {
          int addr_in = (oo * I * KH * KW) + (ii * KH * KW) + (kh * KW) + kw;
	  int addr_out = (oo * new_I * KH * KW) + (ii * KH * KW) + (kh * KW) + kw;
	  p[addr_out] = aInitializer[i].data[addr_in];
	}
      }
    }
  }
  aInitializer[i].dimensions[0] = new_O;
  aInitializer[i].dimensions[1] = new_I;
  free(aInitializer[i].data);
  aInitializer[i].data = p;
}

/*
 * fn_pad_bias()
 *
 * resizes the initializer with new O
 *
 */
void fn_pad_bias(int i, int new_O) {
  if (i==-1) return;
  if (!aInitializer[i].valid) return;

  int O = aInitializer[i].dimensions[0];

  size_t new_num_items = new_O;
  float *p = (float *)malloc(sizeof(float) * new_num_items);
  memset(p, 0, sizeof(float) * new_num_items);
  for (int o=0; o<O; o++) p[o] = aInitializer[i].data[o];
  aInitializer[i].dimensions[0] = new_O;
  free(aInitializer[i].data);
  aInitializer[i].data = p;
}

/*
 * fn_pad_bn_vector()
 *
 * resizes the initializer with new O
 *
 */
void fn_pad_bn_vector(int i, int new_O) {
  if (i==-1) return;
  if (!aInitializer[i].valid) return;

  int O = aInitializer[i].dimensions[0];

  size_t new_num_items = new_O;
  float *p = (float *)malloc(sizeof(float) * new_num_items);
  memset(p, 0, sizeof(float) * new_num_items);
  for (int o=0; o<O; o++) p[o] = aInitializer[i].data[o];
  aInitializer[i].dimensions[0] = new_O;
  free(aInitializer[i].data);
  aInitializer[i].data = p;
}

