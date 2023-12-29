/*
 * stats.h
 *
 */

#ifndef _STATS_H
#define _STATS_H

// function prototypes
void fn_buffer_stats(float *b, int num_items, char *text);
void fn_buffer_stats_by_name(char *name, char *text);
void fn_start_timer(int x);
void fn_stop_timer(int x);
unsigned long long fn_get_timer(int x);

#endif
