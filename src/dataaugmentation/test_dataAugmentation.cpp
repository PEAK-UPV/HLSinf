#include <stdio.h>
#include <stdlib.h>
#include <ap_int.h>
#include <random>
#include <sys/time.h>

#include "da.h"

//#define DEBUG_CPU

// PLEASE, change Image size in da.h corresponding defines
int W = WSIM;                // Width of the data
int H = HSIM;                // Height of the data
int I = ISIM;                // Number of input channels
pixel_t *data_in;            // Input data buffer (format I x W x H)
pixel_t *out_dev;            // Output data buffer (format O x W x H)
pixel_t *out_cpu;            // Output data buffer for cpu (format O x W x H)
int deterministic_input_values;

// Allocate_buffers. Allocates in CPU memory all the needed buffers
void allocate_buffers() {

  // First we allocate buffers in CPU

  // input data buffer
  size_t size_data_in_bytes = I * W * H;
  posix_memalign((void **)&data_in, 4096, size_data_in_bytes);

  // output buffer for fpga
  size_t size_output_in_bytes;
  size_output_in_bytes = (I * W * H) * BATCH_SIZE;
  posix_memalign((void **)&out_dev, 4096, size_output_in_bytes);

  // output buffer for cpu
  posix_memalign((void **)&out_cpu, 4096, size_output_in_bytes);
}

// deallocate_buffers. Deallocates all CPU buffers
void deallocate_buffers() {
  if (out_dev)
    free(out_dev);

  if (out_cpu)
	  free(out_cpu);

  if (data_in)
	  free(data_in);

  out_dev = NULL;
  out_cpu = NULL;
  data_in = NULL;
}

int input_data_address(int i, int h, int w) {
    int addr = (i * H * W) + (h * W) + w;
	return addr;
}

int output_data_address(int n, int i, int h, int w) {
    int addr = (n * I * H * W) + (i * H * W) + (h * W) + w;
	return addr;
}

void init_data() {
  std::random_device rd;
  std::mt19937 gen(rd());

  // random number generators
  std::uniform_int_distribution<int> dist(-10, 10);

  int val = 0;

  // input data
  int addr;
  for (int i=0; i<I; i++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
    	addr = input_data_address(i, h, w);
    	if (i<I) {
          data_in[addr] = deterministic_input_values?i:dist(gen);
    	} else {
    	  data_in[addr] = 0;
    	}
        addr++;
      }
    }
  }
}

void run_kernel() {
	int num_iter_per_batch = BATCH_SIZE / CU;
    k_dataAugmentation((mem_read_block_t*)data_in, (mem_write_block_t *)out_dev, I, H, W, BATCH_SIZE, num_iter_per_batch);
}

void print_input() {

  int Hmax = H;
  int Wmax = W;
  if (H > 5) Hmax = 5;
  if (W > 5) Wmax = 5;

  printf("Input:\n");
  for (int i=0; i<I; i++) {
    printf("channel %d:\n", i);
	for (int h=0; h<Hmax; h++) {
	  for (int w=0; w<Wmax; w++) {
		int addr = input_data_address(i, h, w);
	    printf("%4.2f ", float(data_in[addr]));
        addr++;
	  }
	  if (W != Wmax) printf(" ...");
	  printf("\n");
	}
	if (H != Hmax) printf("...\n");
  }
}


void print_output() {
	printf("Output:\n");
	for (int n=0; n < BATCH_SIZE; n++) {
		for (int o=0; o<I; o++) {
			printf("channel %d:\n", o);
			for (int h=0; h<H; h++) {
				for (int w=0; w<W; w++) {
					int addr_o = output_data_address(n, o, h, w);
				    printf("%6.4f (%6.4f) ", float(out_dev[addr_o]), float(out_cpu[addr_o]));
				}
				printf("\n");
			}
		}
	}
}

void cpu_kernel() {
	int val = 0;
	for (int n=0; n < BATCH_SIZE/CU; n++) {
		for (int cu = 0; cu < CU; cu++) {
			for (int o=0; o<I; o++) {
				for (int h=0; h<H; h++) {
					for (int w=0; w<W; w++) {
						int addr_o = output_data_address(n * CU + cu, o, h, w);
						int addr_i = input_data_address(o, h, w);
						out_cpu[addr_o] = data_in[addr_i] + val;
					}
				}
			}
			val++;
		}
	}
}

void print_timings(unsigned long long time, unsigned long long time_per_iteration, unsigned long long expected_time, float efficiency) {

  printf("| Time %8lld usec  |  Time per iteration %8lld usec  |  Expected time %8lld usec  |   Efficiency %6.4f   |\n", time, time_per_iteration, expected_time, efficiency);
  printf("====================================================================================================================\n");
}

void print_check(int result, float max_difference, int num_differences) {
    if (result) printf("| FAIL                    |            max diff %20.18f           |        num differences %d          |\n", max_difference, num_differences);
    else        printf("| SUCCESS                                                                                                          |\n");
    printf("====================================================================================================================\n");
}

int check_result(float *max_difference, int *num_elements_differ) {
	*max_difference = -1;
	*num_elements_differ = -1;
	for (int i = 0; i < BATCH_SIZE; i++) {
		int offset = i * (H*W*I);
		printf("checking element %d/%d of batch offset=0x%08x...", i, BATCH_SIZE, offset);
		if (memcmp(out_dev+offset, out_cpu+offset, H*W*I) != 0) {
			printf("wrong\n");
			return 1;
		}
		printf("ok\n");
	}

	return 0;
}

void compute(int *enable, int *cpu, int *retval) {

    if (*enable) {

    	allocate_buffers();
    	init_data();


	     #ifdef DEBUG_CPU
	     print_input();
	     #endif

	     // timing stats
	     unsigned long long prof_time;
	     struct timeval prof_t1;
	     gettimeofday(&prof_t1, NULL);

	     run_kernel();

	     // timing
         //struct timeval prof_t2;
	     //gettimeofday(&prof_t2, NULL);
	     //prof_time = ((prof_t2.tv_sec - prof_t1.tv_sec) * 1000000) + (prof_t2.tv_usec - prof_t1.tv_usec);
	     //unsigned long long time = prof_time;
	     //int num_iter = BATCH_SIZE / CU;
	     //unsigned long long time_per_iteration = prof_time / num_iter;
	     //unsigned long long expected_time_one_iteration = I * W * H * 333;
         //unsigned long long expected_time = (expected_time_one_iteration * num_iter) / 100000;
         //float efficiency = ((float)expected_time / (float)time);
         //print_timings(time, time_per_iteration, expected_time, efficiency);


	     if (*cpu) {
	       cpu_kernel();

	       int num_differences;
	       float max_difference;
	       *retval = check_result(&max_difference, &num_differences);

	       print_check(*retval, max_difference, num_differences);

	     } else {
	    	 printf("\n");
	     }

         #ifdef DEBUG_CPU
	     print_output();
	     #endif

	     deallocate_buffers();

	 }
}

int main(int argc, char **argv) {

  int retval = 0;
  int global_retval = 0;
  int cpu;
  int enable;


  if (argc == 1) {
	printf("Co-simulation test...\n");
	enable = 1;
	cpu = 1;
	deterministic_input_values = 1;
	compute(&enable, &cpu, &retval);
    global_retval = retval;
  }

 if(global_retval == 0){
   printf("    *** *** *** *** \n");
   printf("    Results are good \n");
   printf("    *** *** *** *** \n");
 } else {
   printf("    *** *** *** *** \n");
   printf("    Mismatch: retval=%d \n", retval);
   printf("    *** *** *** *** \n");
 }

 // Return 0 if outputs are correct
 return global_retval;
}
