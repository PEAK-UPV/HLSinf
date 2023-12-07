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


// global variables
FILE *fd_fig;      // file descriptor

// pendiente, pasar a defines
int x_scale=130;
int y_scale=350;

int col_width = 20;
int row_height = 2;

int font_height = 1;
int font_width = 1;

int x_top_left = 2;
int y_top_left = 2;
int row_space = 2;


/* 
 * fn_prepare_fig_file()
 *
 * This function opens the file and writes the fig header text
 *
 */
void fn_prepare_fig_file(char *filename) {
  fd_fig = fopen(filename, "w");
  if (fd_fig == NULL) {printf("Error, could not open fig file for write\n"); exit(1);}
  fprintf(fd_fig, "#FIG 3.2  Produced by xfig version 3.2.8a\n");
  fprintf(fd_fig, "Landscape\n");
  fprintf(fd_fig, "Center\n");
  fprintf(fd_fig, "Metric\n");
  fprintf(fd_fig, "A4\n");
  fprintf(fd_fig, "100.00\n");
  fprintf(fd_fig, "Single\n");
  fprintf(fd_fig, "-2\n");
  fprintf(fd_fig, "1200 2\n");
}

/*
 * fn_draw_node
 *
 * This function writes a node into the fig file
 *
 * A node is represented in the graph with its row and col
 *
 */
void fn_draw_node(char *name, int row, int col) {

  // text
  int x_text = (x_top_left * x_scale) + (col * col_width * x_scale);
  int y_text = (y_top_left * y_scale) + (row * row_height * y_scale);
  fprintf(fd_fig, "4 1 0 50 -1 0 12 0.000 4 150 405 %d %d %s\\001\n", x_text, y_text, name);

  // polyline
  //
  // -------x----->
  // (x0,y0)---------------(x1,y1)
  // |                           |
  // |                           |
  // |                           |
  // (x3,y3)---------------(x2,y2)
  //
  int x0 = (x_top_left * x_scale) + (col * col_width * x_scale) - ((strlen(name)/2) * font_width * x_scale);;
  int y0 = (y_top_left * y_scale) + (row * row_height * y_scale) - (0.75 * font_height * y_scale);
  int x1 = x0 + (strlen(name) * font_width * x_scale);
  int y1 = y0;
  int x2 = x1;
  int y2 = y1 + (font_height * y_scale);
  int x3 = x0;
  int y3 = y2;
  int x4 = x0;
  int y4 = y0;
  fprintf(fd_fig, "2 2 0 1 0 7 59 -1 -1 0.000 0 0 -1 0 0 5\n");
  fprintf(fd_fig, "          %d %d %d %d %d %d %d %d %d %d\n", x0, y0, x1, y1, x2, y2, x3, y3, x4, y4);
}

/*
 * fn_draw_arrow
 *
 * This function writes an arrow between two nodes
 *
 * A node is represented by its row and column 
 *
 */
void fn_draw_arrow(int row0, int col0, int row1, int col1) {

  int x0 = (x_top_left * x_scale) + (col0 * col_width * x_scale);
  int y0 = (y_top_left * y_scale) + (row0 * row_height * y_scale) + (0.25 * font_height * y_scale);
  //
  int x1 = (x_top_left * x_scale) + (col1 * col_width * x_scale);
  int y1 = (y_top_left * y_scale) + (row1 * row_height * y_scale) - (0.75 * font_height * y_scale);
  fprintf(fd_fig, "2 1 0 1 0 7 50 -1 -1 0.000 0 0 -1 0 0 2\n");
  fprintf(fd_fig, "          %d %d %d %d\n", x0, y0, x1, y1);
}

/* 
 * This function ends (closes) the fig file
 *
 */
void fn_end_fig_file() {fclose(fd_fig);}

/*
 * This function draws the model into the
 * file passed as an argument
 *
 */
void fn_draw_model(char *fig_filename) {

  if (verbose) printf("generating fig file for model...\n");

  fn_allocate_nodes();

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

  fn_end_fig_file();

  if (verbose) printf("  completed\n");
}
