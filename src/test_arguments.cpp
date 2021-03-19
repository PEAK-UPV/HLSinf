#include "test_conv2D.h"

void parse_arguments(int argc, char **argv) {

#ifdef OPENCL_TEST
  if (argc != 3) {
	printf("%s <input_file> <xclbin file>\n", argv[0]);
    exit(1);
  }
  input_data_file = argv[1];
  binaryFile = argv[2];
#else
  if (argc != 2) {
	printf("%s <input_file>\n", argv[0]);
    exit(1);
  }
  input_data_file = argv[1];
#endif
}
