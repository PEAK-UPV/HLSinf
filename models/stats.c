/*
 * stats.c
 *
 * This file provides statistics about buffers (min, max, avg) and 
 * provides support for timing statistics
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "main.h"
#include "nodes.h"
#include "initializers.h"
#include "in_out.h"
#include "cpu.h"

#define MAX_TIMERS 1000

// global variables
struct timeval start_time[MAX_TIMERS];
struct timeval stop_time[MAX_TIMERS];

void fn_buffer_stats(float *b, int num_items, char *text) {
  float sum = 0.f;
  float min = 99999999.f;
  float max = -9999999.f;

  for (int x=0; x<num_items; x++) {
    if (b[x] < min) min = b[x];
    if (b[x] > max) max = b[x];
    sum += b[x];
    //printf("%6.4f ", b[x]);
  }
  //printf("\n");
  float avg = sum / (float)num_items;
  printf("%s min: %6.4f max: %6.4f avg: %6.4f, num_items: %6d\n", text, min, max, avg, num_items);
}

void fn_buffer_stats_by_name(char *name, char *text) {
  float *b = fn_get_buffer_from_name(name);
  int num_items = fn_get_num_items_from_name(name);
  fn_buffer_stats(b, num_items, text);
}

/*
 *
 * fn_start_timer()
 *
 * This function starts a timer (basically, annotates the current time) 
 */
void fn_start_timer(int x) {gettimeofday(&start_time[x], NULL);}

/*
 *
 * fn_stop_timer()
 * 
 * This function stops a timer (basically, annotates the current time and computes
 * the difference with the start timer counter
 * 
 */
void fn_stop_timer(int x) {gettimeofday(&stop_time[x], NULL);}

/*
 *
 * fn_get_timer()
 * 
 * This function returns the timer obtained
 * 
 */
unsigned long long fn_get_timer(int x) { return (((stop_time[x].tv_sec - start_time[x].tv_sec) * 1000000) + (stop_time[x].tv_usec - start_time[x].tv_usec));}

