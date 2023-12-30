/*
 *
 * main.c
 *
 * This file includes the main entry point of the parser.
 * The file includes the arguments parsing and the function's call
 * to perform the desired operation (either convert a model or to run a model).
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

// arguments defines
#define ARG_HELP              0
#define ARG_OUTPUT            1
#define ARG_RUN               2
#define ARG_VERBOSE           3
#define ARG_CONVERT           4
#define ARG_GENERATE_FIGS     5
#define ARG_CBAR_KEYWORD      6
#define ARG_CBR_KEYWORD       7
#define ARG_CRM_KEYWORD       8
#define ARG_CB_KEYWORD        9
#define ARG_CR_KEYWORD       10
#define ARG_C_KEYWORD        11
#define ARG_ADAPT_1x1_TO_3x3 12
#define ARG_ADAPT_2x2_TO_3x3 13
#define ARG_CPI              14
#define ARG_CPO              15
#define ARG_XCLBIN           16
#define ARG_TIMINGS          17
#define ARG_ADAPT_DENSE      18
#define ARG_REMOVE_IDENTITY  19
#define ARG_NUM_KERNELS      20
#define ARG_OCP              21
#define ARG_IRP              22
#define ARG_NP               23
#define ARG_ALL              24
#define ARG_OMP              25
#define ARG_MC               26
#define ARG_HLSINF_1_0       27
#define ARG_NO_WARNINGS      28

// global variables
char   input_file_name[200];   // input file name including the model to convert/run
char   output_file_name[200];  // output file name to write the produced model
char   xclbin_file_name[200];  // xclbin file (for running)
int    xclbin_defined;         // whether xclbin has been defined
int    verbose;                // if set then we print debug information
int    verbose_level;          // verbosity level (currently 1, 2, 3)
int    convert;                // whether we have to convert an input model to an output file
int    generate_figs;          // whether fig files will be generated for input model and converted one
int    timings;                // whether to collect and show timing statistics
int    run;                    // whether we have to run an input model
int    cbar_keyword;           // CBAR optimization to be applied
int    cbr_keyword;            // CBR optimization to be applied
int    crm_keyword;            // CRM optimization to be applied
int    cb_keyword;             // CB optimization to be applied
int    cr_keyword;             // CR optimization to be applied
int    c_keyword;              // C optimization to be applied
int    adapt_1x1_to_3x3;       // whether 1x1 conv filters to be adapted/padded to 3x3 conv filters
int    adapt_2x2_to_3x3;       // whether 2x2 conv filters to be adapted/padded to 3x3 conv filters
int    adapt_dense;            // whether dense nodes to be adapted/padded to 2D conv nodes
int    remove_identity;        // whether identity nodes to be removed from the input model
int    CPI;                    // Channels per input to be assumed (HLSinf input channel parallelism)
int    CPO;                    // Channels per output to be assumed (HLSinf output channel parallelism)
int    num_kernels;            // number of available HLSinf kernels in the FPGA system
int    memory_configuration;   // memory configuration index (which memory configuration use in FPGAs)
int    ocp_enabled;            // whether output channel parallelism to be applied
int    ocp_threshold;          // output channel parallelism threshold
int    irp_enabled;            // whether input row parallelism to be applied
int    irp_threshold;          // input row parallelism to be applied
int    np_enabled;             // whether node parallelism to be applied
int    enable_omp;             // whether OpenMP to be applied to CPU nodes
int    no_warnings;            // do not show warning messages

/*
 *
 * print_help()
 *
 * prints the help for the program
 * with all its options
 *
 */
void print_help(char *program_name) {
  printf("usage: %s [-c|-r filename] [-o filename] [-xclbin filename] [-k_cbar] [-k_cbr] [-k_crm] [-k_cb] [-k_cr] [-k_c] [-a1x1] [-a2x2] [-v value] [-f] [-help] [-cpi value] [-cpo value] [-t] [-omp]\n", program_name);

  printf("\n");
  printf("  [-c filename]        : convert/parse input file\n");
  printf("  [-o filename]        : Output file where to write the parsed model\n");
  #ifdef RUNTIME_SUPPORT
  printf("  [-r filename]        : run input file\n");  
  printf("  [-xclbin filename]   : XCLBIN file to use in running mode\n");
  #endif
  printf("\n");
  printf("Parameters/optimizations for model conversion:\n");
  printf("  [-cpi value]         : CPI (channels per input) value assumed (by default set to 4)\n");
  printf("  [-cpo value]         : CPO (channels per output) value assumed (by default set to 4)\n");
  printf("  [-k_cbar]            : Merge Conv + BatchNorm + Add + ReLU nodes into a single HLSinf node\n");
  printf("  [-k_cbr]             : Merge Conv + BatchNorm + ReLU nodes into a single HLSinf node\n");
  printf("  [-k_crm]             : Merge Conv + Relu + MaxPool\n");
  printf("  [-k_cb]              : Merge Conv + BatchNorm nodes into a single HLSinf node\n");
  printf("  [-k_cr]              : Merge Conv + ReLU nodes into a single HLSinf node\n");
  printf("  [-k_c]               : Convert Conv into HLSinf node\n");
  printf("  [-a1x1]              : Adapt 1x1 conv filters into 3x3 conv filters\n");
  printf("  [-a2x2]              : Adapt 2x2 conv filters into 3x3 conv filters\n");
  printf("  [-aDense]            : Adapt a nxw dense layer into a oxixhxw conv layer\n");
  printf("  [-ri]                : Remove identity nodes\n");
  printf("  [-all]               : Use all optimizations available\n");
  printf("\n");
  printf("Debug:\n");
  printf("  [-v value]           : Verbose mode activated. Available levels:\n");
  printf("    1                  : just processes being run\n");
  printf("    2                  : output information for every node\n");
  printf("    3                  : all\n");
  printf("\n");
  #ifdef RUNTIME_SUPPORT
  printf("Parameters for model run:\n");
  printf("  [-k value]           : Number of HLSinf kernels availablei (1 by default)\n");
  printf("  [-mc value]          : Memory configuration. Configurations:\n");
  printf("        0              : DDR0 in Alveo U200\n");
  printf("       32              : DDR0 in Alveo U280\n");
  printf("  [-ocp value]         : Use output channel parallelism with indicated threshold\n");
  printf("  [-irp value]         : Use input row parallelism with indicated threshold\n");
  printf("  [-np]                : Use node parallelism\n");
  printf("  [-t]                 : Provide timings (for every run node)\n");
  printf("  [-omp]               : Enable OpenMP (for nodes running on CPU)\n");  
  printf("\n");
  #endif
  printf("Specific HLSinf configurations (conversion and running):\n");
  printf("  [-hlsinf_1.0]        : Alveo U280, 4x4, float32, 2 kernels\n");
  printf("\n");
  printf("Others:\n");
  printf("  [-f]                 : Generate fig files with input model and generated model\n");
  printf("  [-nw]                : Do not show warnings\n");
  printf("  [-help]              : Shows this text\n");
  printf("\n");
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
    {"o", required_argument, NULL, ARG_OUTPUT},
    #ifdef RUNTIME_SUPPORT
    {"xclbin", required_argument, NULL, ARG_XCLBIN},
    {"r", required_argument,       NULL, ARG_RUN},
    #endif
    {"v", required_argument,       NULL, ARG_VERBOSE},
    {"c", required_argument,       NULL, ARG_CONVERT},
    {"f", no_argument,       NULL, ARG_GENERATE_FIGS},
    {"k_cbar", no_argument,    NULL, ARG_CBAR_KEYWORD},
    {"k_cbr", no_argument,     NULL, ARG_CBR_KEYWORD},
    {"k_crm", no_argument,     NULL, ARG_CRM_KEYWORD},
    {"k_cb", no_argument,      NULL, ARG_CB_KEYWORD},
    {"k_cr", no_argument,      NULL, ARG_CR_KEYWORD},
    {"k_c", no_argument,       NULL, ARG_C_KEYWORD},
    {"a1x1", no_argument,      NULL, ARG_ADAPT_1x1_TO_3x3},
    {"a2x2", no_argument,      NULL, ARG_ADAPT_2x2_TO_3x3},
    {"aDense", no_argument,    NULL, ARG_ADAPT_DENSE},
    {"ri", no_argument,        NULL, ARG_REMOVE_IDENTITY},
    {"all", no_argument,       NULL, ARG_ALL},
    {"cpi", required_argument, NULL, ARG_CPI},
    {"cpo", required_argument, NULL, ARG_CPO},
    {"nw", no_argument,        NULL, ARG_NO_WARNINGS},
    #ifdef RUNTIME_SUPPORT
    {"t", no_argument, NULL, ARG_TIMINGS},
    {"k", required_argument,   NULL, ARG_NUM_KERNELS},
    {"mc", required_argument,  NULL, ARG_MC},
    {"ocp", required_argument, NULL, ARG_OCP},
    {"irp", required_argument, NULL, ARG_IRP},
    {"np", no_argument,        NULL, ARG_NP},
    {"omp", no_argument,       NULL, ARG_OMP},
    {"hlsinf_1_0", no_argument,NULL, ARG_HLSINF_1_0},
    #endif
    {0,0,0,0}
  };

  int opt;

  run              = false;
  verbose          = false;
  convert          = false;
  generate_figs    = false;
  timings          = false;
  strcpy(output_file_name, "model.out");
  cbar_keyword     = false;
  cbr_keyword      = false;
  crm_keyword      = false;
  cb_keyword       = false;
  c_keyword        = false;
  adapt_1x1_to_3x3 = false;
  adapt_2x2_to_3x3 = false;
  adapt_dense      = false;
  remove_identity  = false;
  CPI              = 4;
  CPO              = 4;
  num_kernels      = 1;
  ocp_enabled      = false;
  ocp_threshold    = 0;
  irp_enabled      = false;
  irp_threshold    = 0;
  np_enabled       = false;
  verbose_level    = 0;
  xclbin_defined   = false;
  enable_omp       = false;
  memory_configuration = 0;
  no_warnings          = false;


  while ((opt = getopt_long_only(argc, argv, "", long_options, NULL)) != -1) {
    switch(opt) {
      case ARG_HELP              : print_help(argv[0]); break;
      case ARG_RUN               : strcpy(input_file_name, optarg); run = true; break;
      case ARG_CONVERT           : strcpy(input_file_name, optarg); convert = true; break;
      case ARG_OUTPUT            : strcpy(output_file_name, optarg); break;
      case ARG_XCLBIN            : strcpy(xclbin_file_name, optarg); xclbin_defined = true; break;
      case ARG_VERBOSE           : verbose = true; verbose_level = atoi(optarg); break;		    
      case ARG_GENERATE_FIGS     : generate_figs = true; break;
      case ARG_CBAR_KEYWORD      : cbar_keyword = true; break;
      case ARG_CBR_KEYWORD       : cbr_keyword = true; break;
      case ARG_CRM_KEYWORD       : crm_keyword = true; break;
      case ARG_CB_KEYWORD        : cb_keyword = true; break;
      case ARG_CR_KEYWORD        : cr_keyword = true; break;
      case ARG_C_KEYWORD         : c_keyword = true; break;
      case ARG_ADAPT_1x1_TO_3x3  : adapt_1x1_to_3x3 = true; break;
      case ARG_ADAPT_2x2_TO_3x3  : adapt_2x2_to_3x3 = true; break;
      case ARG_ADAPT_DENSE       : adapt_dense = true; break;
      case ARG_REMOVE_IDENTITY   : remove_identity = true; break;
      case ARG_CPI               : CPI = atoi(optarg); break;
      case ARG_CPO               : CPO = atoi(optarg); break;
      case ARG_TIMINGS           : timings = true; break;
      case ARG_NUM_KERNELS       : num_kernels = atoi(optarg); break;
      case ARG_MC                : memory_configuration = atoi(optarg); break;
      case ARG_OCP               : ocp_enabled = true; ocp_threshold = atoi(optarg); break;
      case ARG_IRP               : irp_enabled = true; irp_threshold = atoi(optarg); break;
      case ARG_NP                : np_enabled = true; break;
      case ARG_OMP               : enable_omp = true; break;
      case ARG_NO_WARNINGS       : no_warnings = true; break;
      case ARG_ALL               : cbar_keyword = true; cbr_keyword = true; crm_keyword = true; cb_keyword = true;
				   cr_keyword = true;   c_keyword = true;
				   adapt_1x1_to_3x3 = true; adapt_2x2_to_3x3 = true;
				   adapt_dense = true; remove_identity = true;
				   break;
      case ARG_HLSINF_1_0        : c_keyword = true; cb_keyword = true; cr_keyword = true; cb_keyword = true;
				   cbr_keyword = true; crm_keyword = true; cbar_keyword = true;
				   adapt_1x1_to_3x3 = true; adapt_2x2_to_3x3 = true;
				   adapt_dense = true; remove_identity = true;
				   memory_configuration = 32; num_kernels = 2;
				   break;
      default: exit(1); break;
    }
  }
}

/*
 * main()
 *
 * This is the main entry point, calls the argument's parsing function
 * and based on the arguments runs all needed functions to either convert
 * an input model or to run an input model
 *
 */
int main(int argc, char *argv[]) {

  // we first parse the arguments
  fn_parse_arguments(argc, argv);

  // conversion procedure
  if (convert) {
    if (verbose && verbose_level >= 1) printf("reading input model (file %s)...\n", input_file_name); 
    fn_read_input_model();
    //
    if (verbose && verbose_level >= 1) printf("computing data geometries (output of nodes)...\n");
    fn_compute_data_geometries();

    if (generate_figs) fn_draw_model((char*)"original_model.fig");

    if (verbose && verbose_level >= 1) printf("generating output model...\n");
    fn_generate_output_model();

    if (generate_figs) fn_draw_model((char*)"final_model.fig");

    if (verbose && verbose_level >= 1) printf("writing output model...\n");
    fn_write_output_model();
  }

  // run procedure
  if (run) {
    #ifdef RUNTIME_SUPPORT
    if (verbose && verbose_level >= 1) printf("reading input model (file %s)...\n", input_file_name);
    fn_read_run_graph();
  
    if (verbose && verbose_level >= 1) printf("initializing the FPGA...\n");
    fn_init_fpga();

    if (verbose && verbose_level >= 1) printf("allocating buffers...\n");
    allocate_buffers();

    if (verbose && verbose_level >= 1) printf("copying initializers into buffers...\n");
    copy_initializers_to_fpga();

    if (verbose && verbose_level >= 1) printf("running the graph...\n");
    run_graph();

    // we show the output
    fn_show_output();

    if (verbose && verbose_level >= 1) printf("deallocating buffers...\n");
    deallocate_buffers();
    #else
    printf("binary not compiled for runtime support\n");
    exit(1);
    #endif
  }
  return 0; // success (anyway)
}
