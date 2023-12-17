/*
 * stats.c
 *
 * This file provides statistics about node buffers and initializer buffers
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "nodes.h"
#include "initializers.h"
#include "in_out.h"


void fn_buffer_stats(float *data, float *min, float *max, float *avg, size_t num_items) {
  float _sum = 0.f;
  float _min = 99999999.f;
  float _max = -9999999.f;

  for (int x=0; x<num_items; x++) {
    if (data[x] < _min) _min = data[x];
    if (data[x] > _max) _max = data[x];
    _sum += data[x];
  }
  *min = _min;
  *max = _max;
  *avg = _sum / (float)num_items;
}

void fn_node_buffer_stats(int n, float *min, float *max, float *avg, size_t *num_items) {
  if (n==-1) return;
  if (!aNode[n].valid) return;
  int _num_items = aNode[n].O * aNode[n].HO * aNode[n].WO;
  fn_buffer_stats(aNode[n].data, min, max, avg, _num_items);
  *num_items = _num_items;
}

void fn_initializer_buffer_stats(int i, float *min, float *max, float *avg, size_t *num_items) {
  if (i==-1) return;
  if (!aInitializer[i].valid) return;
  size_t _num_items = 1;
  for (int d=0; d<aInitializer[i].num_dimensions; d++) _num_items = _num_items * aInitializer[i].dimensions[d];
  fn_buffer_stats(aInitializer[i].data, min, max, avg, _num_items);
  *num_items = _num_items;
}

void fn_input_buffer_stats(int i, float *min, float *max, float *avg, size_t *num_items) {
  if (i==-1) return;
  if (!aInput[i].valid) return;
  size_t _num_items = 1;

  for (int d=0; d<aInput[i].num_dimensions; d++) _num_items = _num_items * aInput[i].dimensions[d];

  fn_buffer_stats(aInput[i].data, min, max, avg, _num_items);
  *num_items = _num_items;
}

/*
 *
 * fn_start_timer()
 *
 * This function starts a timer (basically, annotates the current time) 
 */
void fn_start_timer() {}

/*
 *
 * fn_stop_timer()
 * 
 * This function stops a timer (basically, annotates the current time and computes
 * the difference with the start timer counter
 * 
 */
void fn_stop_timer() {}

/*
 *
 * fn_get_timer()
 * 
 * This function returns the timer obtained
 * 
 */
unsined long long fn_get_timer() { return 0; }

