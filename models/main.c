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
#include<getopt.h>

#include "nodes.h"
#include "initializers.h"
#include "model.h"
#include "fig.h"
#include "txt2hlsinf.h"
#include "utils.h"
#include "runner.h"
#include "fpga.h"
#include "parsers.h"

#define ARG_HELP              0
#define ARG_INPUT             1
#define ARG_OUTPUT            2
#define ARG_RUN               3
#define ARG_VERBOSE           4
#define ARG_CONVERT           5
#define ARG_GENERATE_FIGS     6
#define ARG_CBAR_KEYWORD      7
#define ARG_CBR_KEYWORD       8
#define ARG_CB_KEYWORD        9
#define ARG_C_KEYWORD        10
#define ARG_ADAPT_1x1_TO_3x3 11
#define ARG_CPI              12
#define ARG_CPO              13
#define ARG_XCLBIN           14

// global variables
char   input_file_name[200];
char   output_file_name[200];
char   xclbin_file_name[200];
int    verbose;          // if set then we print debug information
int    command;
int    convert;
int    generate_figs;
int    run;
int    cbar_keyword;
int    cbr_keyword;
int    cb_keyword;
int    c_keyword;
int    adapt_1x1_to_3x3;
int    CPI;
int    CPO;

/*
 *
 * print_help()
 *
 * prints the help for the program
 * with all its options
 *
 */
void print_help(char *program_name) {
  printf("usage: %s [-i filename] [-o filename] [-xclbin filename] [-k_cbar] [-k_cbr] [-k_cb] [-k_c] [-a1x1] [-c] [-r] [-v] [-f] [-help] [-cpi value] [-cpo value]\n", program_name);

  printf("[-i filename]        : Input file with the model to parse/run\n");
  printf("[-o filename]        : Output file where to write the parsed model\n");
  printf("[-xclbin filename]   : XCLBIN file to use in running mode\n");
  printf("--\n");
  printf("[-cpi value]         : CPI (channels per input) value assumed (by default set to 4)\n");
  printf("[-cpo value]         : CPO (channels per output) value assumed (by default set to 4)\n");
  printf("[-k_cbar]            : Merge Conv + BatchNorm + Add + ReLU nodes into a single HLSinf node\n");
  printf("[-k_cbr]             : Merge Conv + BatchNorm + ReLU nodes into a single HLSinf node\n");
  printf("[-k_cb]              : Merge Conv + BatchNorm nodes into a single HLSinf node\n");
  printf("[-k_c]               : Convert Conv into HLSinf node\n");
  printf("--\n");
  printf("[-a1x1]              : Adapt 1x1 conv filters into 3x3 conv filters\n");
  printf("--\n");
  printf("[-c]                 : Compile input model and generate output model\n");
  printf("[-r]                 : Run input model\n");
  printf("[-v]                 : Verbose mode activated\n");
  printf("--\n");
  printf("[-f]                 : Generate fig files with input model and generated model\n");
  printf("--\n");
  printf("[-help]              : Shows this text\n");
}


/*
 * fn_parse_arguments()
 *
 * parses the arguments
 *
 */
void fn_parse_arguments(int argc, char *argv[]) {

  struct option long_options[] = {
    {"help", no_argument, NULL, ARG_HELP},
    {"i", required_argument, NULL, ARG_INPUT},
    {"o", required_argument, NULL, ARG_OUTPUT},
    {"xclbin", required_argument, NULL, ARG_XCLBIN},
    {"r", no_argument,       NULL, ARG_RUN},
    {"v", no_argument,       NULL, ARG_VERBOSE},
    {"c", no_argument,       NULL, ARG_CONVERT},
    {"f", no_argument,       NULL, ARG_GENERATE_FIGS},
    {"k_cbar", no_argument,    NULL, ARG_CBAR_KEYWORD},
    {"k_cbr", no_argument,     NULL, ARG_CBR_KEYWORD},
    {"k_cb", no_argument,      NULL, ARG_CB_KEYWORD},
    {"k_c", no_argument,       NULL, ARG_C_KEYWORD},
    {"a1x1", no_argument,      NULL, ARG_ADAPT_1x1_TO_3x3},
    {"cpi", required_argument, NULL, ARG_CPI},
    {"cpo", required_argument, NULL, ARG_CPO},
    {0,0,0,0}
  };

  int opt;

  run = false;
  verbose = false;
  convert = false;
  generate_figs = false;
  strcpy(output_file_name, "model.out");
  cbar_keyword = false;
  cbr_keyword = false;
  cb_keyword = false;
  c_keyword = false;
  adapt_1x1_to_3x3 = false;
  CPI = 4;
  CPO = 4;

  while ((opt = getopt_long_only(argc, argv, "", long_options, NULL)) != -1) {
    switch(opt) {
      case ARG_HELP              : print_help(argv[0]); break;
      case ARG_INPUT             : strcpy(input_file_name, optarg); break;
      case ARG_OUTPUT            : strcpy(output_file_name, optarg); break;
      case ARG_XCLBIN            : strcpy(xclbin_file_name, optarg); break;				   
      case ARG_RUN               : run = true; break;				  
      case ARG_VERBOSE           : verbose = true; break;		    
      case ARG_CONVERT           : convert = true; break;
      case ARG_GENERATE_FIGS     : generate_figs = true; break;
      case ARG_CBAR_KEYWORD      : cbar_keyword = true; break;
      case ARG_CBR_KEYWORD       : cbr_keyword = true; break;
      case ARG_CB_KEYWORD        : cb_keyword = true; break;
      case ARG_C_KEYWORD         : c_keyword = true; break;
      case ARG_ADAPT_1x1_TO_3x3  : adapt_1x1_to_3x3 = true; break;
      case ARG_CPI               : CPI = atoi(optarg); break;
      case ARG_CPO               : CPO = atoi(optarg); break;
      default: exit(1); break;
    }
  }
}

int main(int argc, char *argv[]) {

  fn_parse_arguments(argc, argv);

  if (convert) {
    printf("reading input model (file %s)...\n", input_file_name); 
    fn_read_input_model();
    //
    printf("computing data geometries (output of nodes)...\n");
    fn_compute_data_geometries();

    printf("generating zero initializers...\n");
    fn_create_zero_initializers();

    if (generate_figs) fn_draw_model((char*)"original_model.fig");

    printf("generating output model...\n");
    fn_generate_output_model();

    printf("adding host_device nodes...\n");
    fn_add_host_device_nodes();

    if (generate_figs) fn_draw_model((char*)"final_model.fig");

    printf("writing output model...\n");
    fn_write_output_model();
  }

  if (run) {
    printf("reading input model (file %s)...\n", input_file_name);
    fn_read_run_graph();

    printf("initializing the FPGA...\n");
    fn_init_fpga();

    printf("allocating buffers...\n");
    allocate_buffers();

    printf("copying initializers into buffers...\n");
    copy_initializers_to_fpga();

    printf("running the graph...\n");
    run_graph();

    printf("deallocating buffers...\n");
    deallocate_buffers();
  }
}
