/*
 * utils.c
 *
 * support functions
 *
 */

#include <stdio.h>
#include <stdlib.h>

int max(int a, int b) {
  if (a>=b) return a;
  return b;
}

/*
 *
 * fn_get_item_line()
 *
 * takes the first comma-separated item from an offset
 * returns the item as a string and returns the next offset
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


