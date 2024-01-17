/*
 *
 * main.c
 *
 */

// global variables (see main.c for a description of each)
extern char   input_file_name[200];
extern char   output_file_name[200];
extern char   xclbin_file_name[200];
extern char   data_directory[200];
extern char   labels_file[200];
extern char   conf_file[200];
extern int    xclbin_defined;
extern int    generate_fig;
extern int    timings;
extern int    verbose;
extern int    verbose_level;
extern int    cbar_keyword;
extern int    cbr_keyword;
extern int    crm_keyword;
extern int    cb_keyword;
extern int    cr_keyword;
extern int    c_keyword;
extern int    adapt_1x1_to_3x3;
extern int    adapt_2x2_to_3x3;
extern int    adapt_dense;
extern int    remove_identity;
extern int    CPI;
extern int    CPO;
extern int    num_kernels;
extern int    memory_configuration_weights;
extern int    memory_configuration_data;
extern int    ocp_enabled;
extern int    ocp_threshold;
extern int    irp_enabled;
extern int    irp_threshold;
extern int    np_enabled;
extern int    enable_omp;
extern int    no_warnings;
extern float  mean_normalize_dim0;
extern float  mean_normalize_dim1;
extern float  mean_normalize_dim2;
extern float  std_normalize_dim0;
extern float  std_normalize_dim1;
extern float  std_normalize_dim2;
extern int    resize_input;
extern int    crop_input;
extern int    apply_softmax;
extern int    compute_accuracy;
extern float  kernel_clock;
