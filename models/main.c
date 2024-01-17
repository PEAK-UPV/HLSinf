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
#include "dataset.h"

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
#define ARG_MC_DATA          26
#define ARG_MC_WEIGHTS       27
#define ARG_HLSINF_1_0       28
#define ARG_NO_WARNINGS      29
#define ARG_DATA_DIRECTORY   30
#define ARG_LABELS_FILE      31
#define ARG_MEAN_NORMALIZE0  32
#define ARG_MEAN_NORMALIZE1  33
#define ARG_MEAN_NORMALIZE2  34
#define ARG_STD_NORMALIZE0   35
#define ARG_STD_NORMALIZE1   36
#define ARG_STD_NORMALIZE2   37
#define ARG_RESIZE_INPUT     38
#define ARG_CROP_INPUT       39
#define ARG_APPLY_SOFTMAX    40
#define ARG_COMPUTE_ACCURACY 41
#define ARG_KERNEL_CLOCK     42
#define ARG_CONF_FILE        43

// global variables
char   input_file_name[200];   // input file name including the model to convert/run
char   output_file_name[200];  // output file name to write the produced model
char   xclbin_file_name[200];  // xclbin file (for running)
char   data_directory[200];    // data directory (for running)
char   labels_file[200];       // labels file
char   conf_file[200];         // configuration file
int    load_conf;              // whether to load the configuration file
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
int    memory_configuration_weights;   // memory configuration index (which memory configuration use in FPGAs) for data
int    memory_configuration_data;      // memory configuration index for weights (filters, bias, bn, ...)
int    ocp_enabled;            // whether output channel parallelism to be applied
int    ocp_threshold;          // output channel parallelism threshold
int    irp_enabled;            // whether input row parallelism to be applied
int    irp_threshold;          // input row parallelism to be applied
int    np_enabled;             // whether node parallelism to be applied
int    enable_omp;             // whether OpenMP to be applied to CPU nodes
int    no_warnings;            // do not show warning messages
float  mean_normalize_dim0;    // average normalize value for dimension 0
float  mean_normalize_dim1;    // average normalize value for dimension 1     
float  mean_normalize_dim2;    // average normalize value for dimension 2
float  std_normalize_dim0;     // average normalize value for dimension 0
float  std_normalize_dim1;     // average normalize value for dimension 1     
float  std_normalize_dim2;     // average normalize value for dimension 2
int    resize_input;           // resize input to value
int    crop_input;             // crop input
int    apply_softmax;          // applies softmax at the output of model
int    compute_accuracy;       // computes top1 and top5 accuracy
float  kernel_clock;           // kernel clock (in MHz)



/*
 * fn_process_conf_line()
 *
 * processes a configuration file line
 *
 */
void fn_process_conf_line(char *line) {
  if (line[0] == '#') return;

  if (fn_compare_str(line, (char *)"r=", 2)) {run = true; strcpy(input_file_name, &line[2]);}
  if (fn_compare_str(line, (char *)"c=", 2)) {convert = true; strcpy(input_file_name, &line[2]);}
  if (fn_compare_str(line, (char *)"o=", 2)) {strcpy(output_file_name, &line[2]);}
  if (fn_compare_str(line, (char *)"cpi=", 4)) {CPI = atoi(&line[4]);}
  if (fn_compare_str(line, (char *)"cpo=", 4)) {CPO = atoi(&line[4]);}
  if (fn_compare_str(line, (char *)"xclbin=", 7)) {strcpy(xclbin_file_name, &line[7]); xclbin_defined = true;}
  if (fn_compare_str(line, (char *)"k=", 2)) {num_kernels=atoi(&line[2]);}
  if (fn_compare_str(line, (char *)"ocp=", 4)) {ocp_enabled = true; ocp_threshold=atoi(&line[4]);}
  if (fn_compare_str(line, (char *)"mcd=", 4)) {memory_configuration_data=atoi(&line[4]);}
  if (fn_compare_str(line, (char *)"mcw=", 4)) {memory_configuration_weights = atoi(&line[4]);}
  if (fn_compare_str(line, (char *)"lf=", 3)) {strcpy(labels_file, &line[3]);}
  if (fn_compare_str(line, (char *)"dd=", 3)) {strcpy(data_directory, &line[3]);}
  if (fn_compare_str(line, (char *)"softmax=yes",11)) {apply_softmax = true;}
  if (fn_compare_str(line, (char *)"accuracy=yes",12)) {compute_accuracy = true;}
  if (fn_compare_str(line, (char *)"v=",2)) {verbose=true; verbose_level=atoi(&line[2]);}
  if (fn_compare_str(line, (char *)"f=yes",5)) {generate_figs = true;}
  if (fn_compare_str(line, (char *)"k_cbar=yes",10)) {cbar_keyword = true;}
  if (fn_compare_str(line, (char *)"k_cbr=yes",9)) {cbr_keyword = true;}
  if (fn_compare_str(line, (char *)"k_crm=yes",9)) {crm_keyword = true;}
  if (fn_compare_str(line, (char *)"k_cb=yes",8)) {cb_keyword = true;}
  if (fn_compare_str(line, (char *)"k_cr=yes",8)) {cr_keyword = true;}
  if (fn_compare_str(line, (char *)"k_c=",3)) {c_keyword = true;}
  if (fn_compare_str(line, (char *)"mean0=",6)) {mean_normalize_dim0 = atof(&line[6]);}
  if (fn_compare_str(line, (char *)"mean1=",6)) {mean_normalize_dim1 = atof(&line[6]);}
  if (fn_compare_str(line, (char *)"mean2=",6)) {mean_normalize_dim2 = atof(&line[6]);}
  if (fn_compare_str(line, (char *)"std0=",5)) {std_normalize_dim0 = atof(&line[5]);}
  if (fn_compare_str(line, (char *)"std1=",5)) {std_normalize_dim1 = atof(&line[5]);}
  if (fn_compare_str(line, (char *)"std2=",5)) {std_normalize_dim2 = atof(&line[5]);}
  if (fn_compare_str(line, (char *)"resize=",7)) {resize_input = atoi(&line[7]);}
  if (fn_compare_str(line, (char *)"crop=yes",8)) {crop_input = true;}
  if (fn_compare_str(line, (char *)"kernel_clock=",13)) {kernel_clock = atof(&line[13]);}
  if (fn_compare_str(line, (char *)"a1x1=yes",8)) {adapt_1x1_to_3x3 = true;}
  if (fn_compare_str(line, (char *)"a2x2=yes",8)) {adapt_2x2_to_3x3 = true;}
  if (fn_compare_str(line, (char *)"aDense=yes",10)) {adapt_dense = true;}
  if (fn_compare_str(line, (char *)"ri=yes",6)) {remove_identity = true;}
  if (fn_compare_str(line, (char *)"t=yes",5)) {timings = true;}
  if (fn_compare_str(line, (char *)"omp=yes",7)) {enable_omp = true;}
  if (fn_compare_str(line, (char *)"nw=yes",6)) {no_warnings = true;}
}

/*
 * fn_print_conf()
 *
 * prints selected configuration
 *
 */
void fn_print_conf() {
  printf("Configuration:\n");
  if (run) {
    printf(" - run %s model\n", input_file_name);
    printf(" - CPI %d, CPO %d\n", CPI, CPO);
    printf(" - xclbin file: %s\n", xclbin_defined?xclbin_file_name:(char*)"not defined");
    printf(" - number of kernels: %d\n", num_kernels);
    if (ocp_enabled) printf(" - output channel parallelism enabled: threshold %d\n", ocp_threshold);
    printf(" - memory configuration: data %d, weights %d\n", memory_configuration_data, memory_configuration_weights);
    printf(" - labels file: %s\n", labels_file);
    printf(" - data directory: %s\n", data_directory);
    printf(" - softmax: %s\n", apply_softmax?(char *)"yes":(char *)"no");
    printf(" - accuracy: %s\n", compute_accuracy?(char *)"yes":(char *)"no");
    printf(" - mean: %6.4f %6.4f %6.4f\n", mean_normalize_dim0, mean_normalize_dim1, mean_normalize_dim2);
    printf(" - std : %6.4f %6.4f %6.4f\n", std_normalize_dim0, std_normalize_dim1, std_normalize_dim2);
    printf(" - resize: %s\n", resize_input?(char *)"yes":(char *)"no");
    printf(" - crop: %s\n", crop_input?(char *)"yes":(char *)"no");
    printf(" - kernel clock: %6.4f\n", kernel_clock);
    printf(" - Enable OMP: %s\n", enable_omp?(char *)"yes":(char *)"no");
    printf(" - Show timing statistics: %s\n", timings?(char *)"yes":(char *)"no");
  }
  if (convert) {
    printf(" - convert %s model into %s file\n", input_file_name, output_file_name);
    printf(" - CPI %d, CPO %d\n", CPI, CPO);
    printf(" - Generate figs: %s\n", generate_figs?(char *)"yes":(char *)"no");
    printf(" - CBAR optimization: %s\n", cbar_keyword?(char *)"yes":(char *)"no");
    printf(" - CBR optimization : %s\n", cbr_keyword?(char *)"yes":(char *)"no");
    printf(" - CRM optimization : %s\n", crm_keyword?(char *)"yes":(char *)"no");
    printf(" - CB optimization  : %s\n", cb_keyword?(char *)"yes":(char *)"no");
    printf(" - CR optimization  : %s\n", cr_keyword?(char *)"yes":(char *)"no");
    printf(" - C optimization   : %s\n", c_keyword?(char *)"yes":(char *)"no");
    printf(" - Adapt 1x1 kernels: %s\n", adapt_1x1_to_3x3?(char *)"yes":(char *)"no");
    printf(" - Adapt 2x2 kernels: %s\n", adapt_2x2_to_3x3?(char *)"yes":(char *)"no");
    printf(" - Adapt Dense nodes: %s\n", adapt_dense?(char *)"yes":(char *)"no");    
    printf(" - Remove identity nodes: %s\n", remove_identity?(char *)"yes":(char *)"no");
  }
 
  if (verbose) printf(" - verbosity level: %d\n", verbose_level);
  if (no_warnings) printf(" - do not show warnings\n");
}

/*
 *
 * fn_load_conf()
 *
 * loads the configuration from a file passed as argument
 *
 */
void fn_load_conf(char *filename) {
  FILE *fd;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  if ((fd = fopen(filename, "r"))==NULL) {printf("Error, conf file not found\n"); exit(1);}

  while ((read = getline(&line, &len, fd)) != -1) {
    // remove intro
    if (line[strlen(line)-1] == 10) line[strlen(line)-1] = '\0';
    fn_process_conf_line(line);
  }
  fclose(fd);
  if (line) free(line);
  printf("input file: %s\n", input_file_name);
}


/*
 *
 * print_help()
 *
 * prints the help for the program
 * with all its options
 *
 */
void print_help(char *program_name) {
  printf("usage: %s [-c|-r filename] [-o filename] [-dd dirname] [-xclbin filename] [-k_cbar] [-k_cbr] [-k_crm] [-k_cb] [-k_cr] [-k_c] [-a1x1] [-a2x2] [-v value] [-f] [-help] [-cpi value] [-cpo value] [-t] [-omp] ...\n", program_name);

  printf("\n");
  printf("  [-c filename]        : convert/parse input file\n");
  printf("  [-o filename]        : Output file where to write the parsed model\n");
  printf("  [-conf filename]     : configuration file\n");
  #ifdef RUNTIME_SUPPORT
  printf("  [-r filename]        : run input file\n");  
  printf("  [-xclbin filename]   : XCLBIN file to use in running mode\n");
  printf("  [-dd dirname]        : data directory\n");
  printf("  [-lf filename]       : labels file name\n");
  printf("  [-mean0 float]       : average value input data normalization for dimension 0\n");
  printf("  [-mean1 float]       : average value input data normalization for dimension 1\n");
  printf("  [-mean2 float]       : average value input data normalization for dimension 2\n");
  printf("  [-std0 float]        : std dev. input data normalization for dimension 0\n");
  printf("  [-std1 float]        : std dev. input data normalization for dimension 1\n");
  printf("  [-std2 float]        : std dev. input data normalization for dimension 2\n");
  printf("  [-resize value]      : resize input to given size\n");
  printf("  [-crop]              : crop input\n");
  printf("  [-softmax]           : applies softmat at the output of the model\n");
  printf("  [-accuracy]          : computes accuracy\n");
  printf("  [-kernel_clock value]: kernel clock (in MHz)\n");
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
  printf("  [-mcd value]         : Memory configuration for data\n");
  printf("  [-mcw value]         : Memory configuration for weights\n");
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
    {"dd", required_argument,      NULL, ARG_DATA_DIRECTORY},
    {"lf", required_argument,      NULL, ARG_LABELS_FILE},
    {"conf", required_argument,    NULL, ARG_CONF_FILE},
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
    {"mcd", required_argument,  NULL, ARG_MC_DATA},
    {"mcw", required_argument, NULL, ARG_MC_WEIGHTS},
    {"ocp", required_argument, NULL, ARG_OCP},
    {"irp", required_argument, NULL, ARG_IRP},
    {"np", no_argument,        NULL, ARG_NP},
    {"omp", no_argument,       NULL, ARG_OMP},
    {"hlsinf_1_0", no_argument,NULL, ARG_HLSINF_1_0},
    {"mean0", required_argument,NULL, ARG_MEAN_NORMALIZE0},
    {"mean1", required_argument,NULL, ARG_MEAN_NORMALIZE1},
    {"mean2", required_argument,NULL, ARG_MEAN_NORMALIZE2},
    {"std0", required_argument,NULL, ARG_STD_NORMALIZE0},
    {"std1", required_argument,NULL, ARG_STD_NORMALIZE1},
    {"std2", required_argument,NULL, ARG_STD_NORMALIZE2},
    {"resize", required_argument,NULL, ARG_RESIZE_INPUT},
    {"crop", no_argument, NULL, ARG_CROP_INPUT},
    {"softmax", no_argument, NULL, ARG_APPLY_SOFTMAX},
    {"accuracy", no_argument, NULL, ARG_COMPUTE_ACCURACY},
    {"kernel_clock", required_argument, NULL, ARG_KERNEL_CLOCK},
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
  strcpy(data_directory, "./");
  strcpy(labels_file, "");
  strcpy(conf_file, "");
  load_conf        = false;
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
  memory_configuration_data = 0;
  memory_configuration_weights = 0;
  no_warnings          = false;
  mean_normalize_dim0   = 0.f;
  mean_normalize_dim1   = 0.f;
  mean_normalize_dim2   = 0.f;
  std_normalize_dim0   = 1.f;
  std_normalize_dim1   = 1.f;
  std_normalize_dim2   = 1.f;
  resize_input         = 0;
  crop_input           = false;
  apply_softmax        = false;
  compute_accuracy     = false;
  kernel_clock         = 300;

  while ((opt = getopt_long_only(argc, argv, "", long_options, NULL)) != -1) {
    switch(opt) {
      case ARG_HELP              : print_help(argv[0]); break;
      case ARG_RUN               : strcpy(input_file_name, optarg); run = true; break;
      case ARG_DATA_DIRECTORY    : strcpy(data_directory, optarg); break;
      case ARG_LABELS_FILE       : strcpy(labels_file, optarg); break;
      case ARG_CONF_FILE         : strcpy(conf_file, optarg); load_conf = true; break;
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
      case ARG_MC_DATA           : memory_configuration_data = atoi(optarg); break;
      case ARG_MC_WEIGHTS        : memory_configuration_weights = atoi(optarg); break;
      case ARG_OCP               : ocp_enabled = true; ocp_threshold = atoi(optarg); break;
      case ARG_IRP               : irp_enabled = true; irp_threshold = atoi(optarg); break;
      case ARG_NP                : np_enabled = true; break;
      case ARG_OMP               : enable_omp = true; break;
      case ARG_NO_WARNINGS       : no_warnings = true; break;
      case ARG_MEAN_NORMALIZE0   : mean_normalize_dim0 = atof(optarg); break;
      case ARG_MEAN_NORMALIZE1   : mean_normalize_dim1 = atof(optarg); break;
      case ARG_MEAN_NORMALIZE2   : mean_normalize_dim2 = atof(optarg); break;
      case ARG_STD_NORMALIZE0    : std_normalize_dim0 = atof(optarg); break;
      case ARG_STD_NORMALIZE1    : std_normalize_dim1 = atof(optarg); break;
      case ARG_STD_NORMALIZE2    : std_normalize_dim2 = atof(optarg); break;
      case ARG_RESIZE_INPUT      : resize_input = atoi(optarg); break;
      case ARG_CROP_INPUT        : crop_input = true; break;
      case ARG_APPLY_SOFTMAX     : apply_softmax = true; break;
      case ARG_COMPUTE_ACCURACY  : compute_accuracy = true; break;
      case ARG_KERNEL_CLOCK      : kernel_clock = atof(optarg); break;
      case ARG_ALL               : cbar_keyword = true; cbr_keyword = true; crm_keyword = true; cb_keyword = true;
				   cr_keyword = true;   c_keyword = true;
				   adapt_1x1_to_3x3 = true; adapt_2x2_to_3x3 = true;
				   adapt_dense = true; remove_identity = true;
				   break;
      case ARG_HLSINF_1_0        : c_keyword = true; cb_keyword = true; cr_keyword = true; cb_keyword = true;
				   cbr_keyword = true; crm_keyword = true; cbar_keyword = true;
				   adapt_1x1_to_3x3 = true; adapt_2x2_to_3x3 = true;
				   adapt_dense = true; remove_identity = true;
				   memory_configuration_weights = 32; 
				   memory_configuration_data = 32; 
				   num_kernels = 2; kernel_clock = 240; 
				   memory_configuration_data = 32; memory_configuration_weights = 32;
				   break;
      default: exit(1); break;
    }
  }

  if (load_conf) fn_load_conf(conf_file);
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

  // now we show the configuration
  fn_print_conf();

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

    if (verbose && verbose_level >= 1) printf("loading dataset...\n");
    load_dataset();

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
