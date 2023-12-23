/*
 * stats.c
 *
 * This file provides statistics about node buffers and initializer buffers
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

// global variables
struct timeval start_time;
struct timeval stop_time;

void fn_buffer_stats(float *b, int num_items, char *text) {
  float sum = 0.f;
  float min = 99999999.f;
  float max = -9999999.f;

  for (int x=0; x<num_items; x++) {
    if (b[x] < min) min = b[x];
    if (b[x] > max) max = b[x];
    sum += b[x];
  }
  float avg = sum / (float)num_items;
  printf("%s min: %6.4f max: %6.4f avg: %6.4f, num_items: %6d\n", text, min, max, avg, num_items);
}

/*
 *
 * fn_start_timer()
 *
 * This function starts a timer (basically, annotates the current time) 
 */
void fn_start_timer() {gettimeofday(&start_time, NULL);}

/*
 *
 * fn_stop_timer()
 * 
 * This function stops a timer (basically, annotates the current time and computes
 * the difference with the start timer counter
 * 
 */
void fn_stop_timer() {gettimeofday(&stop_time, NULL);}

/*
 *
 * fn_get_timer()
 * 
 * This function returns the timer obtained
 * 
 */
unsigned long long fn_get_timer() { return (((stop_time.tv_sec - start_time.tv_sec) * 1000000) + (stop_time.tv_usec - start_time.tv_usec));}

