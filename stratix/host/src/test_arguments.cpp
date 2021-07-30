#include "test_conv2D.h"

void parse_arguments(int argc, char **argv) {

#ifdef OPENCL_TEST
  if ((argc != 3) && (argc != 4)) {
    printf(KRED "%s <input_file> <xclbin file> <emulation>\n" KNRM, argv[0]);
    printf("    input_file:            Input data file\n");
    printf("    xcl_bin:               kernel image binary file\n");
    printf("    emulation:             optional, enable enmulation mode, if not set application runs for FPGA HW\n");
    exit(1);
  }

  if (argc == 4) {
    if (strcmp(argv[3], "emulation") == 0) {
      use_emulator = 1;
      printf (" option: set emulation mode detected\n");
    }
  }
  input_data_file = argv[1];
  binaryFile = argv[2];
  printf(KGRN);
  printf(" input_file:  %s\n", argv[1]);
  printf(" xcl_bin:     %s\n", argv[2]);
  printf(" mode:        %s\n", use_emulator?"emulation" : "fpga hw");
  printf("\n" KNRM);
#else
  if (argc != 2) {
	printf("%s <input_file>\n", argv[0]);
    exit(1);
  }
  input_data_file = argv[1];
#endif
}


/*
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void parse_arguments(int argc, char **argv) {
  printf(KGRN "Check options\n" KRST);

  Options options(argc, argv);

  // Optional argument to specify whether the emulator should be used.
  if(options.has("emulator")) {
    use_emulator = options.get<bool>("emulator");
    printf("set option parameter emulator\n");
  }

  if(options.has("input")) {
    strncpy(input_data_file, options.get<string>("input").c_str(), sizeof(input_data_file));
     printf("set option input_data_file to \"%s\"\n", input_data_file);
    if( access( input_data_file, F_OK ) == 0 ) {
      // file exists
    } else {
    // file doesn't exist
      printf(KRED "Failed to access input_data_file \"%s\"\n" KNRM, input_data_file);
   }
  }

  if(options.has("bin")) {
    strncpy(binary_file, options.get<string>("bin").c_str(), sizeof(binary_file));
    printf("set option binary_file to \"%s\"\n", binary_file);
    if( access( binary_file, F_OK ) == 0 ) {
      // file exists
    } else {
    // file doesn't exist
      printf(KRED "Failed to access kernel binary file \"%s\"\n" KNRM, binary_file);
   }
  }

  if(options.has("h") || options.has("help")) {
    printf("%s <-emulator> -input=<input_data_file> -bin=<kernel_binary_file> -h\n", argv[0] );
    printf("    emulator: optional. Set hw-emu execution mode\n");
    printf("    input:              Input data file\n");
    printf("    bin:                kernel image binary file\n");
    printf("    h:                  print this menu and exit\n");
    abort("");
  }

  printf("\n");
}
*/


