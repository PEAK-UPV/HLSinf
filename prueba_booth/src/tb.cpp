#include "tb.h"

//Global variables;
int prob = 5;
size_t size_x_in_bytes = 0;
size_t size_w_in_bytes = 0;
size_t size_y_in_bytes = 0;

ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x0;
ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x1;
ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x2;
ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x3;
ap_uint<8> *w;
ap_uint<16> *y;

#ifdef OPENCL_TEST
// OpenCL variables
cl::Context context;                          // Context
cl::CommandQueue q;                           // Command queue
cl::Program program;                          // Program
std::string binaryFile;                       // Binary file
cl::Event kernel_events; // Kernel events (completion)
cl::Kernel kernel_conv2d;        // FPGA kernels
vector<cl::Event> read_events(1);             // Read events
vector<cl::Event> write_events(3);            // Write events
cl::Buffer *buffer_i0;                         // input buffer
cl::Buffer *buffer_i1;                         // input buffer
cl::Buffer *buffer_i2;                         // input buffer
cl::Buffer *buffer_i3;                         // input buffer
cl::Buffer *buffer_o;              			  // output buffers
cl::Buffer *buffer_k;                         // Conv kernel buffers
// DDR assignment
cl_mem_ext_ptr_t data_in_ddr0;                 // input data buffer
cl_mem_ext_ptr_t data_in_ddr1;                 // input data buffer
cl_mem_ext_ptr_t data_in_ddr2;                 // input data buffer
cl_mem_ext_ptr_t data_in_ddr3;                 // input data buffer
cl_mem_ext_ptr_t out_ddr;          			  // output data buffers
cl_mem_ext_ptr_t kernel_ddr;                  // Conv kernel buffers
#endif

void zerogen(ap_uint<8> *ptr, int prob, int n, std::mt19937 gen, std::uniform_int_distribution<int> dist) {
	int zero[n] = {};
	int nzero = n * prob / 100;
	while(nzero > 0) {
		int pos = rand() % n;
		if(zero[pos] == 0) {
			zero[pos] = 1;
			nzero--;
		}
	}
	for(int i = 0; i < n; i++) {
		if(zero[i] == 1) ptr[i] =  0;
		else ptr[i] = dist(gen);
	}
}

int main(int argc, char* argv[]) {

  std::random_device rd;
  std::mt19937 gen(0); //rd());

  std::uniform_int_distribution<int> dist(0, 255);

#ifdef OPENCL_TEST
  fn_init_fpga();
#endif

  ap_uint<8> *x[I];
  parse_arguments(argc, argv);
  printf("Prob of 0: %d%\n", prob);
  // x and w integer vectors
#ifdef OPENCL_TEST
  allocate_buffers();
#else
  size_x_in_bytes = sizeof(ap_uint<8>) * H * W;
  size_w_in_bytes = sizeof(ap_uint<8>) * I * O * KH * KW;
  size_y_in_bytes = sizeof(ap_uint<16>) * H * W * O;

  for (int i=0; i<I; i++) posix_memalign((void **)&x[i], 4096, size_x_in_bytes);
  posix_memalign((void **)&w, 4096, size_w_in_bytes);
  posix_memalign((void **)&y, 4096, size_y_in_bytes);
#endif
  for (int i=0; i<I; i++) {
	  ap_uint<8> *ptr = x[i];
	  zerogen(ptr, prob, H*W, gen, dist);
  }

  for (int i=0; i<I; i++) for (int o=0; o<O; o++) for (int k=0; k<KH*KW; k++) w[i+(o*I)+(k*O*I)] = dist(gen);
  for (int i=0; i<H*W*O; i++) y[i] = 0;

#ifdef OPENCL_TEST
    copy_to_fpga();
#endif

  #ifdef DEBUG
  printf("x:\n");
  for (int i=0; i<I; i++) {
	  printf("  channel %d:\n", i);
	  ap_uint<8> *ptr = x[i];
	  for (int h=0; h<H; h++) {for (int w=0; w<W; w++) printf("%d ", int(ptr[h*W + w])); printf("\n");}
  }
  printf("w:\n");
  for (int o=0; o<O; o++) {
	  for (int i=0;i<I; i++) {
	    printf(" - O %d I %d:\n        ", o, i);
	    for (int k=0; k<KH*KW; k++) printf("%d ", int(w[i+(o*I)+(k*O*I)])); printf("\n");
	  }
  }
  #endif

  x0 = (ap_uint<DATA_SIZE*READ_GROUP_SIZE> *)x[0];
  x1 = (I>=2) ? (ap_uint<DATA_SIZE*READ_GROUP_SIZE> *)x[1] : nullptr;
  x2 = (I>=3) ? (ap_uint<DATA_SIZE*READ_GROUP_SIZE> *)x[2] : nullptr;
  x3 = (I>=4) ? (ap_uint<DATA_SIZE*READ_GROUP_SIZE> *)x[3] : nullptr;

#ifdef OPENCL_TEST
  run_kernel();
#else
  top(x0, x1, x2, x3, (ap_uint<8*O*I> *)w, (ap_uint<512> *)y);
#endif

#ifdef OPENCL_TEST
  // OpenCL kernel time
  cl_ulong time_start, time_end;
  kernel_events.getProfilingInfo(CL_PROFILING_COMMAND_START, &time_start);
  kernel_events.getProfilingInfo(CL_PROFILING_COMMAND_END, &time_end);
  double diff = time_end-time_start;
  std::cout<< "TIME KERNEL = " << (diff/1000000)<<" ms \n"<<std::endl;

  copy_from_fpga();
#endif


  // cpu vector multiplication
  ap_uint<16> *y_cpu;
  posix_memalign((void **)&y_cpu, 4096, size_y_in_bytes);

  for (int z=0; z<O*H*W; z++) y_cpu[z] = 0;

  for (int i=0; i<I; i++) {
	  ap_uint<8> *ptr = x[i];
	  for (int o=0; o<O; o++) {
		  for (int k=0; k<KH*KW; k++) {
			  int addr_w = i + (o*I) + (k*O*I);
			  for (int z=0; z<H*W; z++) y_cpu[(o*H*W)+z] += ptr[z] * w[addr_w];
		  }
	  }
  }

  int failed = 0;
  for (int o=0; o<O; o++) for (int z=0; z<H*W; z++) if (y_cpu[(o*H*W)+z] != y[(o * H * W) + z]) failed = 1;

  if (failed) {
    printf("results:\n");
    for (int o=0; o<O; o++) for (int z=0; z<H*W; z++) printf("  %12d (cpu %12d) - %s\n", int(y[(o*H*W)+z]), int(y_cpu[(o*H*W)+z]), y[(o*H*W)+z] == y_cpu[(o*H*W)+z] ? "CORRECT":"WRONG");
  }

  printf("H %d W %d I %d O %d, KHxKW %dx%d -> %s\n", H, W, I, O, KH, KW, failed?"FAILED!":"SUCCESS");

  for (int i=0; i<I; i++) free(x[i]);
  free(w);
  free(y);
  free(y_cpu);

  return failed;

}

