/*
 * fig.c
 *
 * This files provides support for the generation of fig files
 * representing the model topology
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "fig.h"
#include "nodes.h"
#include "main.h"

// defines for fig figures
#define FIG_X_SCALE    130
#define FIG_Y_SCALE    350
#define FIG_COL_WIDTH   20
#define FIG_ROW_HEIGHT   2
#define FIG_FONT_HEIGHT  1
#define FIG_FONT_WIDTH   1
#define FIG_X_TOP_LEFT   2
#define FIG_Y_TOP_LEFT   2
#define FIG_ROW_SPACE    2

// global variables
FILE *fd;                // file descriptor


/* 
 * fn_prepare_fig_file()
 *
 * This function opens the file and writes the fig header text
 *
 */
void fn_prepare_fig_file(char *filename) {
  // let's open the file
  fd = fopen(filename, "w");
  if (fd == NULL) {printf("Error, could not open fig file for write\n"); exit(1);}
  // add header text
  fprintf(fd, "#FIG 3.2  Produced by xfig version 3.2.8a\n");
  fprintf(fd, "Landscape\n");
  fprintf(fd, "Center\n");
  fprintf(fd, "Metric\n");
  fprintf(fd, "A4\n");
  fprintf(fd, "100.00\n");
  fprintf(fd, "Single\n");
  fprintf(fd, "-2\n");
  fprintf(fd, "1200 2\n");
}

/*
 * fn_draw_node()
 *
 * This function writes a node into the fig file.
 * A node is represented in the graph with its row and col.
 *
 */
void fn_draw_node(char *name, int row, int col) {

  // text
  int x_text = (FIG_X_TOP_LEFT * FIG_X_SCALE) + (col * FIG_COL_WIDTH * FIG_X_SCALE);
  int y_text = (FIG_Y_TOP_LEFT * FIG_Y_SCALE) + (row * FIG_ROW_HEIGHT * FIG_Y_SCALE);
  fprintf(fd, "4 1 0 50 -1 0 12 0.000 4 150 405 %d %d %s\\001\n", x_text, y_text, name);

  // polyline
  //
  // -------x----->
  // (x0,y0)---------------(x1,y1)
  // |                           |
  // |                           |
  // |                           |
  // (x3,y3)---------------(x2,y2)
  //
  int x0 = (FIG_X_TOP_LEFT * FIG_X_SCALE) + (col * FIG_COL_WIDTH * FIG_X_SCALE) - ((strlen(name)/2) * FIG_FONT_WIDTH * FIG_X_SCALE);
  int y0 = (FIG_Y_TOP_LEFT * FIG_Y_SCALE) + (row * FIG_ROW_HEIGHT * FIG_Y_SCALE) - (0.75 * FIG_FONT_HEIGHT * FIG_Y_SCALE);
  int x1 = x0 + (strlen(name) * FIG_FONT_WIDTH * FIG_X_SCALE);
  int y1 = y0;
  int x2 = x1;
  int y2 = y1 + (FIG_FONT_HEIGHT * FIG_Y_SCALE);
  int x3 = x0;
  int y3 = y2;
  int x4 = x0;
  int y4 = y0;
  fprintf(fd, "2 2 0 1 0 7 59 -1 -1 0.000 0 0 -1 0 0 5\n");
  fprintf(fd, "          %d %d %d %d %d %d %d %d %d %d\n", x0, y0, x1, y1, x2, y2, x3, y3, x4, y4);
}

/*
 * fn_draw_arrow()
 *
 * This function writes an arrow between two nodes
 *
 * A node is represented by its row and column 
 *
 */
void fn_draw_arrow(int row0, int col0, int row1, int col1) {

  // x and y positions
  int x0 = (FIG_X_TOP_LEFT * FIG_X_SCALE) + (col0 * FIG_COL_WIDTH * FIG_X_SCALE);
  int y0 = (FIG_Y_TOP_LEFT * FIG_Y_SCALE) + (row0 * FIG_ROW_HEIGHT * FIG_Y_SCALE) + (0.25 * FIG_FONT_HEIGHT * FIG_Y_SCALE);
  int x1 = (FIG_X_TOP_LEFT * FIG_X_SCALE) + (col1 * FIG_COL_WIDTH * FIG_X_SCALE);
  int y1 = (FIG_Y_TOP_LEFT * FIG_Y_SCALE) + (row1 * FIG_ROW_HEIGHT * FIG_Y_SCALE) - (0.75 * FIG_FONT_HEIGHT * FIG_Y_SCALE);
  // line
  fprintf(fd, "2 1 0 1 0 7 50 -1 -1 0.000 0 0 -1 0 0 2\n");
  fprintf(fd, "          %d %d %d %d\n", x0, y0, x1, y1);
}

/* 
 * fn_end_fig_file()
 *
 * This function ends (closes) the fig file
 *
 */
void fn_end_fig_file() {fclose(fd);}

/*
 * fn_draw_mode()
 *
 * This function draws the model into the
 * file passed as an argument
 *
 */
void fn_draw_model(char *fig_filename) {

  // verbose/debug information
  if (verbose) printf("generating fig file for model...\n");

  // let's first sort the nodes in order to know the row order
  fn_allocate_nodes();

  // let's open and prepare the file
  fn_prepare_fig_file(fig_filename);
  
  // we draw the nodes
  for (int n = 0; n<num_nodes; n++) if (aNode[n].valid) fn_draw_node(aNode[n].name, aNode[n].row, aNode[n].col);

  // we draw the links
  for (int n = 0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      int list[100];
      int nc = fn_get_child_nodes(aNode[n].name, list);
      for (int c = 0; c < nc; c++) {
        int cc = list[c];
        fn_draw_arrow(aNode[n].row, aNode[n].col, aNode[cc].row, aNode[cc].col);
      }
    }
  }

  // we end up the file (close the file)
  fn_end_fig_file();

  // verbose/debug information
  if (verbose) printf("  completed\n");
}
