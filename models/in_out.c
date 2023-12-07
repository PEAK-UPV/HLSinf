/*
 * in_out.c
 *
 * This file provides support
 * for the management of inputs and outputs
 * of the model
 *
 */

#include<stdlib.h>
#include<string.h>

#include "in_out.h"

// global variables
int               num_inputs;
int               num_outputs;
struct st_input   aInput[MAX_INPUTS];
struct st_output  aOutput[MAX_OUTPUTS];

/*
 * is_model_input()
 *
 * returns whether the name passed as
 * an argument is an input to the model
 */
int is_model_input(char *name) {
  return !strcmp(name, "data");    // TODO
}

/*
 * get_model_output_id()
 *
 * returns the output model id for a name
 * If not found then -1 is returned
 *
 */
int get_model_output_id(char *name) {
  for (int o=0; o<num_outputs; o++) if (aOutput[o].valid) if (!strcmp(aOutput[o].name, name)) return o;
  return -1;
}

/*
 * fn_change_output_model_name()
 *
 * This function changes the name of an
 * output model
 *
 * As arguments we have the output id
 * and the new name
 */
void fn_change_output_model_name(int o, char *name) {
  if (o==-1) return;
  if (!aOutput[o].valid) return;
  char *p = (char *)malloc(sizeof(char) * (strlen(name)+1));
  strcpy(p, name);
  free(aOutput[o].name);
  aOutput[o].name = p;
}
