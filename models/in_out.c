/*
 * in_out.c
 *
 * This file provides support
 * for the management of inputs and outputs
 * of the model.
 *
 */

#include<stdlib.h>
#include<string.h>

#include "in_out.h"
#include "utils.h"

// global variables
int               num_inputs;              // number of inputs to the model
int               num_outputs;             // number of outputs from the model
struct st_input   aInput[MAX_INPUTS];      // vector of model inputs
struct st_output  aOutput[MAX_OUTPUTS];    // vector of model outputs

/*
 * is_model_input()
 *
 * returns whether the name passed as
 * an argument is an input to the model.
 */
int is_model_input(char *name) {
  // we search all model inputs, if there is a coincidence by name, then the name is an input
  for (int i=0; i<num_inputs; i++) if (aInput[i].valid) if (!strcmp(aInput[i].name, name)) return true;
  return false;
}

/*
 * get_mode_input_id()
 *
 * returns the input model id for a name.
 * if not found then -1 is returned
 *
 */
int get_model_input_id(char *name) {
  // we search all model inputs, if there is a coincidence by name, then we return the entry id
  for (int i=0; i<num_inputs; i++) if (aInput[i].valid) if (!strcmp(aInput[i].name, name)) return i;
  return -1;
}

/*
 * get_model_output_id()
 *
 * returns the output model id for a name.
 * If not found then -1 is returned
 *
 */
int get_model_output_id(char *name) {
  // we search all model outputs, if there is a coincidence by name, then we return the entry id
  for (int o=0; o<num_outputs; o++) if (aOutput[o].valid) if (!strcmp(aOutput[o].name, name)) return o;
  return -1;
}

/*
 * fn_change_output_model_name()
 *
 * This function changes the name of an
 * output model
 *
 * As arguments we have the output id (entry)
 * and the new name.
 */
void fn_change_output_model_name(int o, char *name) {
  // double check entry is valid
  if (o==-1) return;
  if (!aOutput[o].valid) return;
  // let's allocate the new output name
  char *p = (char *)malloc(sizeof(char) * (strlen(name)+1));
  // let's copy the new name
  strcpy(p, name);
  // free and assign old/new name
  free(aOutput[o].name);
  aOutput[o].name = p;
}
