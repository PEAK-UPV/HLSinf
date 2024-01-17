/*
 * utils.h
 *
 */

#ifndef _UTILS_H
#define _UTILS_H

// defines
#define false 0
#define true  1

// prototype functions
int max(int a, int b);
int min(int a, int b);
int fn_compare_str(char *str1, char *str2, int n);
size_t fn_get_item_line(char *line, size_t len, size_t offset, char *item);
size_t fn_get_item_line_space(char *line, size_t len, size_t offset, char *item);
void fn_print_inference_table_header();
void fn_print_inference_table_info(char *filename, unsigned long long load_time, unsigned long long inference_time, int predict, float prob, float top1_acc, float top5_acc);
void fn_print_inference_table_bottom();
#endif
