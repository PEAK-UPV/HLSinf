/*
 * cpu.h
 *
 *
 */

#ifndef _CPU_H
#define _CPU_H

// function prototypes
void fn_run_node_on_cpu(int n);
float *fn_get_buffer_from_name(char *name);
int fn_get_num_items_from_name(char *name);
void fn_softmax(float *in, float *out, int n);

#endif
