#include "top.h"


int main() {

  std::random_device rd;
  std::mt19937 gen(0); //rd());

  std::uniform_int_distribution<int> dist(-16, 16);

  // x and w integer vectors
  ap_int<8> x_input[NUM_VECTORS][OUTPUT_CHANNELS][VECTOR_SIZE];
  ap_int<8> w_input[NUM_VECTORS][OUTPUT_CHANNELS][VECTOR_SIZE];


  for (int v=0; v<NUM_VECTORS; v++) {
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  for (int i=0; i<VECTOR_SIZE; i++) {
	    x_input[v][c][i] = dist(gen);
	  }
    }
  }

  for (int v=0; v<NUM_VECTORS; v++) {
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  for (int i=0; i<VECTOR_SIZE; i++) {
	    w_input[v][c][i] = dist(gen);
	  }
    }
  }

  #ifdef DEBUG
  printf("input vectors x:\n");
  for (int v=0; v<NUM_VECTORS; v++) {
	printf("  vector %d: ", v);
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  printf("  channel %d: ", c);
	  for (int i=0; i<VECTOR_SIZE; i++) {
		  printf("%d ", int(x_input[v][c][i]));
	  }
	  printf("\n");
    }
  }

  printf("input vectors w:\n");
  for (int v=0; v<NUM_VECTORS; v++) {
	printf("  vector %d: ", v);
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  printf("  channel %d: ", c);
	  for (int i=0; i<VECTOR_SIZE; i++) {
		  printf("%d ", int(w_input[v][c][i]));
	  }
	  printf("\n");
    }
  }
  #endif

  ap_int<8> *x;
  ap_int<8> *w;
  ap_int<32> *y;

  int size_x_in_bytes = sizeof(ap_int<8>) * OUTPUT_CHANNELS * VECTOR_SIZE * NUM_VECTORS;
  int size_w_in_bytes = sizeof(ap_int<8>) * OUTPUT_CHANNELS * VECTOR_SIZE * NUM_VECTORS;
  int size_y_in_bytes = sizeof(ap_int<32>) * OUTPUT_CHANNELS * NUM_VECTORS;
  posix_memalign((void **)&x, 4096, size_x_in_bytes);
  posix_memalign((void **)&w, 4096, size_w_in_bytes);
  posix_memalign((void **)&y, 4096, size_y_in_bytes);

  for (int v=0; v<NUM_VECTORS; v++) {
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  for (int i=0; i<VECTOR_SIZE; i++) {
	    x[(v*OUTPUT_CHANNELS*VECTOR_SIZE)+(i*OUTPUT_CHANNELS)+c] = x_input[v][c][i];
		w[(v*OUTPUT_CHANNELS*VECTOR_SIZE)+(i*OUTPUT_CHANNELS)+c] = w_input[v][c][i];
	  }
    }
  }

#ifdef pepito



  ap_int<BBITS> binary_to_booth(ap_uint<3> bits) {
  	ap_int<BBITS> result;
  	switch (bits) {
  	case 0: result = 0; break;
  	case 1: result = 1; break;
  	case 2: result = 1; break;
  	case 3: result = 2; break;
  	case 4: result = -2; break;
  	case 5: result = -1; break;
  	case 6: result = -1; break;
  	case 7: result = 0; break;
  	}
  	return result;
  }




  // x and w vectors
  ap_uint<BBITS*4> *x;
  ap_uint<BBITS*4> *w;
  ap_int<32> *y;

  int size_x_in_bytes = sizeof(ap_uint<BBITS*4>) * OUTPUT_CHANNELS * VECTOR_SIZE;
  int size_w_in_bytes = sizeof(ap_uint<BBITS*4>) * OUTPUT_CHANNELS * VECTOR_SIZE;
  int size_y_in_bytes = sizeof(ap_int<32>) * OUTPUT_CHANNELS;
  posix_memalign((void **)&x, 4096, size_x_in_bytes);
  posix_memalign((void **)&w, 4096, size_w_in_bytes);
  posix_memalign((void **)&y, 4096, size_y_in_bytes);

  // conversion to booth
  for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  for (int i=0; i<VECTOR_SIZE; i++) {
		  ap_int<BBITS> x_0, x_2, x_4, x_6, x_8, w_0, w_2, w_4, w_6;
		  ap_int<8> x_value = x_input[c][i];
		  ap_uint<3> x_bits_0 = x_value.range(1, 0) << 1;
		  ap_uint<3> x_bits_2 = x_value.range(3, 1);
		  ap_uint<3> x_bits_4 = x_value.range(5, 3);
		  ap_uint<3> x_bits_6 = x_value.range(7, 5);
		  ap_int<BBITS> x_booth_0 = binary_to_booth(x_bits_0);
		  ap_int<BBITS> x_booth_2 = binary_to_booth(x_bits_2);
		  ap_int<BBITS> x_booth_4 = binary_to_booth(x_bits_4);
		  ap_int<BBITS> x_booth_6 = binary_to_booth(x_bits_6);
		  ap_int<8> w_value = w_input[c][i];
		  ap_uint<3> w_bits_0 = w_value.range(1, 0) << 1;
		  ap_uint<3> w_bits_2 = w_value.range(3, 1);
		  ap_uint<3> w_bits_4 = w_value.range(5, 3);
		  ap_uint<3> w_bits_6 = w_value.range(7, 5);
		  ap_int<BBITS> w_booth_0 = binary_to_booth(w_bits_0);
		  ap_int<BBITS> w_booth_2 = binary_to_booth(w_bits_2);
		  ap_int<BBITS> w_booth_4 = binary_to_booth(w_bits_4);
		  ap_int<BBITS> w_booth_6 = binary_to_booth(w_bits_6);
		  ap_uint<BBITS*4> x_booth, w_booth;
		  x_booth.range((BBITS*1)-1, BBITS*0) = x_booth_0;
		  x_booth.range((BBITS*2)-1, BBITS*1) = x_booth_2;
		  x_booth.range((BBITS*3)-1, BBITS*2) = x_booth_4;
		  x_booth.range((BBITS*4)-1, BBITS*3) = x_booth_6;
		  w_booth.range((BBITS*1)-1, BBITS*0) = w_booth_0;
		  w_booth.range((BBITS*2)-1, BBITS*1) = w_booth_2;
		  w_booth.range((BBITS*3)-1, BBITS*2) = w_booth_4;
		  w_booth.range((BBITS*4)-1, BBITS*3) = w_booth_6;
		  x[(c*VECTOR_SIZE)+i] = x_booth;
		  w[(c*VECTOR_SIZE)+i] = w_booth;
	  }
  }

  #ifdef DEBUG
  printf("conversion to booth:\n");
  for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  printf("  channel %d:\n", c);
	  for (int i=0; i<VECTOR_SIZE; i++) {
		  ap_uint<BBITS*4> x_value = x[(c*VECTOR_SIZE)+i];
		  ap_int<BBITS> x_0 = x_value.range((BBITS*1)-1, BBITS*0);
		  ap_int<BBITS> x_2 = x_value.range((BBITS*2)-1, BBITS*1);
		  ap_int<BBITS> x_4 = x_value.range((BBITS*3)-1, BBITS*2);
		  ap_int<BBITS> x_6 = x_value.range((BBITS*4)-1, BBITS*3);
		  ap_int<BBITS*4> w_value = w[(c*VECTOR_SIZE)+i];
		  ap_int<BBITS> w_0 = w_value.range((BBITS*1)-1, BBITS*0);
		  ap_int<BBITS> w_2 = w_value.range((BBITS*2)-1, BBITS*1);
		  ap_int<BBITS> w_4 = w_value.range((BBITS*3)-1, BBITS*2);
		  ap_int<BBITS> w_6 = w_value.range((BBITS*4)-1, BBITS*3);
		  printf("    x: %4d -> %3d %3d %3d %3d\n", int(x_input[c][i]), int(x_6), int(x_4), int(x_2), int(x_0));
		  printf("    w: %4d -> %3d %3d %3d %3d\n", int(w_input[c][i]), int(w_6), int(w_4), int(w_2), int(w_0));
	  }
  }
  #endif
#endif

  top((xw_read_st *)x, (xw_read_st *)w, y);

  // cpu vector multiplication
  ap_int<32> y_cpu[NUM_VECTORS][OUTPUT_CHANNELS];

  for (int v=0; v<NUM_VECTORS; v++) {
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  y_cpu[v][c] = 0;
	  for (int i=0; i<VECTOR_SIZE; i++) {
	    ap_int<32> res = x_input[v][c][i] * w_input[v][c][i];
		y_cpu[v][c] = y_cpu[v][c] + res;
	  }
    }
  }

  int failed = 0;
  for (int v=0; v<NUM_VECTORS; v++) {
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  if (y_cpu[v][c] != y[v*OUTPUT_CHANNELS+c]) failed = 1;
    }
  }

  printf("results:\n");
  for (int v=0; v<NUM_VECTORS; v++) {
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
      printf("  vector %d, channel %3d -> %12d (cpu %12d) - %s\n", v, c, int(y[v*OUTPUT_CHANNELS+c]), int(y_cpu[v][c]), y[v*OUTPUT_CHANNELS+c] == y_cpu[v][c] ? "CORRECT":"WRONG");
    }
  }

  printf("%d VECTORS, %d CHANNELS, vector size %d -> %s\n", NUM_VECTORS, OUTPUT_CHANNELS, VECTOR_SIZE, failed?"FAILED!":"SUCCESS");

  free(x);
  free(w);
  free(y);

  return failed;

}
