/*
 *
 * main.c
 *
 * This file includes the main entry point of the parser
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "nodes.h"
#include "initializers.h"
#include "model.h"
#include "fig.h"
#include "txt2hlsinf.h"
#include "utils.h"
#include "runner.h"
#include "fpga.h"

// global variables
char   input_file_name[200];
int    hlsinf_version;
int    hlsinf_subversion;
int    verbose;          // if set then we print debug information
int    command;

/*
 * fn_parse_arguments()
 *
 * parses the arguments
 *
 */
void fn_parse_arguments(int argc, char *argv[]) {

  if (argc!=6) {
    printf("usage: <%s> <input_text_file> <HLSinf_version> <HLSinf_subversion> <command> <verbose>\n", argv[0]);
    exit(1);
  }
  strcpy(input_file_name, argv[1]);
  hlsinf_version = atoi(argv[2]);
  hlsinf_subversion = atoi(argv[3]);
  command = atoi(argv[4]);
  verbose = atoi(argv[5]);
}

int main(int argc, char *argv[]) {

  fn_parse_arguments(argc, argv);

  if (command==0) {
    fn_read_input_model();
    fn_read_initializers_data();
    fn_compute_data_geometries();
    fn_create_zero_initializers();
    fn_print_model();
    fn_draw_model((char*)"original_model.fig");
    fn_generate_output_model();
    fn_add_host_device_nodes();
    fn_print_model();
    fn_draw_model((char*)"generated_model.fig");
    fn_write_output_model();
  } else {
    fn_read_run_graph();
    //fn_init_fpga();
    //allocate_buffers();
    run_graph();
    //deallocate_buffers();
  }
}
