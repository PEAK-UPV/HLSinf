/*
 * utils.c
 *
 * support generic functions
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * max function
 *
 * returns the maximum integer value between two 
 * arguments.
 *
 */
int max(int a, int b) {
  if (a>=b) return a;
  return b;
}

/*
 * min function
 *
 * returns the minimum integer value between two
 * arguments.
 *
 */
int min(int a, int b) {
  if (a<=b) return a;
  return b;
}

/*
 * fn_compare_str()
 *
 * compares two strings a given number of characters
 *
 */
int fn_compare_str(char *str1, char *str2, int n) {
  if (strlen(str1) < n) return false;
  if (strlen(str2) < n) return false;
  for (int x=0; x<n; x++) if (str1[x] != str2[x]) return false;
  return true;
}

/*
 *
 * fn_get_item_line()
 *
 * takes the first comma-separated item from an offset,
 * returns the item as a string and returns the next offset.
 *
 * left spaces and right spaces are removed
 */
size_t fn_get_item_line(char *line, size_t len, size_t offset, char *item) {
 size_t i = 0;
 size_t i_item = 0;
 while ((line[i+offset] != ',') && (line[i+offset] != 10) && (line[i+offset] != '\0')) {
   if (line[i+offset] != ' ') {
     item[i_item] = line[i+offset];
     i_item++;
   }
   i++;
 }
 item[i_item] = '\0';
 return i+offset+1;
}

/*
 *
 * fn_get_item_line_space()
 *
 * takes the first space-separated item from an offset,
 * returns the item as a string and returns the next offset.
 *
 */
size_t fn_get_item_line_space(char *line, size_t len, size_t offset, char *item) {
 size_t i = 0;
 size_t i_item = 0;
 while ((line[i+offset] != ' ') && (line[i+offset] != 10) && (line[i+offset] != '\0')) {
  item[i_item] = line[i+offset];
  i_item++;
  i++;
 }
 item[i_item] = '\0';
 return i+offset+1;
}


/*
 * fn_print_inference_table_header()
 *
 * prints the table header for inference
 *
 */
void fn_print_inference_table_header() {
  // let's print table header
  printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  printf("| image                                                                                                               | load time | inf. time | predict | prob.  | top1_acc | top5_acc |\n");
  printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

/*
 * fn_print_inference_table_info()
 *
 * prints one entry of the inference table
 *
 */
void fn_print_inference_table_info(char *filename, unsigned long long load_time, unsigned long long inference_time, int predict, float prob, float top1_acc, float top5_acc) {
  printf("| %-115s | %9lld | %9lld | %7d | %6.4f | %8.4f | %8.4f |\n", filename, load_time, inference_time, predict, prob, top1_acc, top5_acc);
}

/*
 * fn_print_inference_table_bottom()
 *
 * prints the table bottom for inference
 *
 */
void fn_print_inference_table_bottom() {
  printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}
