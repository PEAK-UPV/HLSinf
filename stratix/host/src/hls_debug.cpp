#include "test_conv2D.h"



#include "hls_debug.h"

int padding_data_address(int i, int h, int w) {
  #ifdef GIHWCPI_DATA_FORMAT
	int gi = i / (CPI);
	int ii = i % (CPI);
	int addr = (gi * (H + 2) * (W + 2)* (CPI)) + (h * (W + 2) * (CPI)) + (w * (CPI)) + ii;
  #endif
  #ifdef IHW_DATA_FORMAT
  not sure about this
  int addr = (i * (H + 2) * (W + 2)) + (h * (W +2)) + w;
  #endif
	return addr;
}



unsigned long int my_val[NUM_KERNELS];
unsigned long my_ret[NUM_KERNELS];
unsigned long my_ret_2[NUM_KERNELS];
unsigned long my_ret_3[NUM_KERNELS];
unsigned long my_ret_4[NUM_KERNELS];
float         my_flt_bias[NUM_KERNELS];
float         my_flt_krnl[NUM_KERNELS];
float         my_flt_din[NUM_KERNELS];
float         my_flt_dout[NUM_KERNELS];


size_t jm10_buffer_size_in_bytes; // integration debug purposes

vector<cl::Event> my_loop_read_events(MAX_KERNELS); // (completion)
data_type  dbg_cpu_data_directconv_sum = (data_type)0;


// input data to fpga kernel
data_type  *dbg_loop_data_in;                         // loop from fpga for Input data buffer (format I x W x H)
cl::Buffer *dbg_loop_data_in_buffer_i[MAX_KERNELS];   // loop from fpga for input buffer
// input data to fpga kernel after input_buffer stage
data_type  *dbg_loop_data_input_buffer;                    // loop from fpga for data after input_buffer stage, same format as data_in (format I x W x H)
cl::Buffer *dbg_loop_data_input_buffer_buffer[MAX_KERNELS];   // loop from fpga for input buffer

// output from the padding stage of direct convolution function
data_type  *dbg_loop_data_dc_pad_out;    // loop from fpga for Input data buffer (format I x W x H)
cl::Buffer *dbg_loop_data_dc_pad_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer
// output from the cvt stage of the direct convolution function
data_type  *dbg_loop_data_dc_cvt_out;    // loop from fpga for Input data buffer (format I x W x H)
cl::Buffer *dbg_loop_data_dc_cvt_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer
// output from the mul stage of the direct convolution function
data_type  *dbg_loop_data_dc_mul_out;    // loop from fpga for (format O x W x H)
cl::Buffer *dbg_loop_data_dc_mul_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer

// output data from direct convolution stage of the kernel 
data_type  *dbg_loop_data_directconv_out;                 // loop from fpga for Input data buffer (format I x W x H)
cl::Buffer *dbg_loop_data_directconv_out_buffer[MAX_KERNELS];   // loop from fpga for input buffer

data_type *dbg_cpu_data_directconv_out;

// se puede mejorar contantdo con el formato gwhipi y si i= 1 y o= 1
// Folowing values have to be updated with values of current sim
// int i_useful = I_input;
// int o_useful = O_output;
int I_useful = 4;
int O_useful = 1;

#define PRINT_DIN
#define PRINT_DATA_INPUT_BUFFER
#define PRINT_DATA_DC_PAD_OUT
#define PRINT_DATA_DC_CVT_OUT
//#define PRINT_DATA_DC_MUL_OUT
//#define PRINT_DATA_DIRECTCONV_OUT

void hls_debug(void) {

  printf("\n\n");
  printf(KGRN "NOTICE: for this test 4x4x1x1 just one channel with useful data\n" KNRM);
  
  int num_kernels;
  if (o_iter < NUM_KERNELS) {
	  num_kernels = 1;
  } else {
	  num_kernels = NUM_KERNELS;
  }

  for (int k=0; k<num_kernels; k++) {
    printf("k=%d  my_val=%lu  my_ret=%lu  my_ret_2=%lu  my_ret_3=%lu  my_ret_4=%lu  my_flt_bias = %f  my_flt_krnl = %f  my_flt_din = %f  my_flt_dout = %f \n",
        k, my_val[k], my_ret[k], my_ret_2[k], my_ret_3[k], my_ret_4[k], my_flt_bias[k], my_flt_krnl[k], my_flt_din[k], my_flt_dout[k]
        );
    printf("\n");
  }


  // data_in 
  float  dbg_my_loop_data_in_sum = 0.0;
  size_t size_data_in       = I_input * W * H ;
  size_t size_data_in_bytes = size_data_in * sizeof(data_type);

 // input data to fpga calculated here in host
  int addr;

  //  //for (int i=0; i<I_useful; i++) {
  //  //  for (int h=0; h<H; h++) {
  //  //    for (int w=0; w<W; w++) {
  //  //  	  addr = input_data_address(i, h, w);
  //  //      printf("i = %d   h = %d   w = %d    addr = %d\n", i, h ,w, addr);
  //  //      addr++;
  //  //    }
  //  //  }
  //  //}
  //  //
  //  //  printf(KCYN "Data in matrix\n" KNRM);
  //  //  for (int i=0; i<I_useful; i++) {
  //  //    printf("input channel i = %d\n", i);
  //  //    for (int h=0; h<H; h++) {
  //  //      for (int w=0; w<W; w++) {       
  //  //        addr = input_data_address(i, h, w);
  //  //        printf("[%d] = %2.2f\n", addr, data_in[addr]);
  //  //        addr++;
  //  //      }
  //  //    }
  //  //  }
  //  //
  



  printf("\n");
  printf(KCYN "HOST SIDE calculated - data in matrix sent to FPGA\n" KNRM);
 
  #ifdef PRINT_DIN
  for (int i=0; i<I_useful; i++) {
    printf("W  ");
    for (int w=0; w<W; w++) printf(" %5d ", w);
    printf("\n");

    for (int h=0; h<H; h++) {
      printf ("H %2d ", h);
      for (int w=0; w<W; w++) {       
        addr = input_data_address(i, h, w);
        printf("  %2.2f ", data_in[addr]);
      }
      printf("\n");
    }
  }
  #endif 

  printf("\n");
  printf(KCYN "data in matrix looped from FPGA to host\n" KNRM);
  printf("dbg_loop_data_in\n"); 
  #ifdef PRINT_DIN  
  for (int i=0; i<I_useful; i++) {
    printf("W  ");
    for (int w=0; w<W; w++) printf(" %5d ", w);
    printf("\n");

    for (int h=0; h<H; h++) {
      printf ("H %2d ", h);
      for (int w=0; w<W; w++) {       
        addr = input_data_address(i, h, w);
        printf("  %2.2f ",  dbg_loop_data_in[addr]);
      }
      printf("\n");
    }
  }
  #endif
  //for (int i = 0; i < size_data_in_bytes; i++) 
  for (int i = 0; i < size_data_in; i++)
  {
    dbg_my_loop_data_in_sum = dbg_my_loop_data_in_sum + dbg_loop_data_in[i];
    //if((i%CPI)==0)printf("    [%2d] = %3.2f\n", i, dbg_loop_data_in[i]);
  }

  // input buffer 
  printf("\n");
  printf(KCYN "data in matrix after input_buffer stage from FPGA to host\n" KNRM);
  printf("dbg_loop_data_input_buffer\n"); 
  #ifdef PRINT_DATA_INPUT_BUFFER
  for (int i=0; i<I_useful; i++) {
    printf("W  ");
    for (int w=0; w<W; w++) printf(" %5d ", w);
    printf("\n");

    for (int h=0; h<H; h++) {
      printf ("H %2d ", h);
      for (int w=0; w<W; w++) {       
        addr = input_data_address(i, h, w);
        printf("  %2.2f ",  dbg_loop_data_input_buffer[addr]);
      }
      printf("\n");
    }
  }
  #endif
  float  dbg_my_loop_input_buffer_sum = 0.0;
  size_t size_data_input_buffer       = I_input * W * H ;
  size_t size_data_input_buffer_bytes = size_data_input_buffer * sizeof(data_type);
  //for (int i = 0; i < size_data_input_buffer_bytes; i++) 
  for (int i = 0; i < size_data_input_buffer; i++)
  {
    dbg_my_loop_input_buffer_sum = dbg_my_loop_input_buffer_sum + dbg_loop_data_input_buffer[i];
    //if((i%CPI)==0)printf("    [%2d] = %3.2f\n", i, dbg_loop_data_in[i]);
  }

  // ---
  // DIRECT CONVOLUTION stages
  // data after padding stage
  printf("\n");
  printf(KCYN "data matrix after DC padding stage from FPGA to host\n" KNRM);
  printf("dbg_loop_data_dc_pad_out\n"); 
  #ifdef PRINT_DATA_DC_PAD_OUT
  //for (int i=0; i<I_useful; i++) {
  for (int i=0; i<I_input; i++) {
    printf("In channel  %2d\n", i);
    printf("W    ");
    for (int w=0; w<(W+2); w++) printf(" %5d ", w -1);
    printf("\n");
    for (int h=0; h<(H+2); h++) {
      printf ("H %2d ", h-1);
      for (int w=0; w<(W+2); w++) {       
        addr = padding_data_address(i, h, w);
        printf("  %2.2f ",  dbg_loop_data_dc_pad_out[addr]);
      }
      printf("\n");
    }
  }
  #endif

  //  //printf("dbg_loop_data_dc_pad_out COMPLETE ARRAY\n");
  //  //for (int i = 0; i < (I_input * (W+2) * (H+2)); i++)
  //  //{
  //  //  /*if((i%CPI)==0)*/printf("    [%2d] = %3.2f\n", i, dbg_loop_data_dc_pad_out[i]);
  //  //}

  // data after cvt stage, frame composition
  printf("\n");
  printf(KCYN "data matrix after CVT stage from FPGA to host\n" KNRM);
  printf("dbg_loop_data_dc_cvt_out\n");
  
  // buffer created with    I_input * NUM_PIXELS_IN_FRAME * NUM_OF_I_ITERS * (H) * (W) * MAX_KERNELS  * sizeof(data_type);
  size_t size_data_dc_cvt = I_input * NUM_PIXELS_IN_FRAME * NUM_OF_I_ITERS * (H) * (W) * MAX_KERNELS; // size values: pixels * channels_in
  size_t size_data_dc_cvt_bytes  = size_data_dc_cvt + sizeof(data_type);



  printf("num values of data_type in frames: %lu\n", size_data_dc_cvt);
  printf("  only %d kernels used, so %lu first values must be checked\n", NUM_KERNELS, (size_data_dc_cvt/MAX_KERNELS));
  #ifdef PRINT_DATA_DC_CVT_OUT
  for (int i=0; i<NUM_KERNELS; i++) {
    printf("Kernel %d\n", i);
    //printf("  H /  W     ");
    //for (int j = 0; j < NUM_PIXELS_IN_FRAME; j++) {
    //  printf("     %2d  ", j);
    //}
    //printf("\n");

    int index_base_curr_kernel = (CPI * NUM_PIXELS_IN_FRAME * NUM_OF_I_ITERS * (H) * (W)) * i;
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
        int frame_index = (h*W)+w;

        //addr = input_data_address(i, h, w);
        //printf("  %2.2f ",  dbg_loop_data_input_buffer[addr]);
        int index_base = index_base_curr_kernel + ((frame_index ) * NUM_PIXELS_IN_FRAME * CPI) ;
        printf ("FRAME %d  h %2d / w %2d  index_base = %d\n", frame_index, h, w , index_base);
        for (int k = 0; k < CPI; k++) { printf(" cpi %2d\n", k);
        //for (int k = 0; k < 1; k++) {printf(" cpi %2d    " KYEL "WARNING printing only cpi 0 of [%2d,%2d]\n" KNRM, k, k, CPI);

          //for (int l = 0; l < NUM_PIXELS_IN_FRAME; l++) {
          //  int cvt_index = index_base + (l * CPI) + k;
          //  printf("   %4.2f  ", dbg_loop_data_dc_cvt_out[ cvt_index]);
          //}
          //printf("\n");

          printf("  %4.2f  %4.2f  %4.2f\n", dbg_loop_data_dc_cvt_out[index_base + (0 * CPI) + k], dbg_loop_data_dc_cvt_out[index_base + (1 * CPI) + k], dbg_loop_data_dc_cvt_out[index_base + (2 * CPI) + k]);
          printf("  %4.2f  %4.2f  %4.2f\n", dbg_loop_data_dc_cvt_out[index_base + (3 * CPI) + k], dbg_loop_data_dc_cvt_out[index_base + (4 * CPI) + k], dbg_loop_data_dc_cvt_out[index_base + (5 * CPI) + k]);
          printf("  %4.2f  %4.2f  %4.2f\n", dbg_loop_data_dc_cvt_out[index_base + (6 * CPI) + k], dbg_loop_data_dc_cvt_out[index_base + (7 * CPI) + k], dbg_loop_data_dc_cvt_out[index_base + (8 * CPI) + k]);
        }
      }
    }
  }
  #endif


  // direct convolution data out
  printf("\n");
  printf(KCYN "data matrix after add stage from FPGA to host\n" KNRM);
  printf("dbg_loop_data_dc_add_out, equals dbg_loop_data_directconv_out\n");
  float  dbg_my_loop_out_sum = 0.0f;
  size_t size_data_out       = O_output * W * H ;
  size_t size_data_out_bytes = size_data_out * sizeof(data_type);
  //for (int i = 0; i < size_data_out_bytes; i++) 
  for (int i = 0; i < size_data_out; i++)
  {
    dbg_my_loop_out_sum = dbg_my_loop_out_sum + dbg_loop_data_directconv_out[i];
    ///*if((i%4)==0)*/printf("    [%2d] = %3.2f\n", i, dbg_loop_data_directconv_out[i]);
  }
  #ifdef PRINT_DATA_DIRECTCONV_OUT
  for (int o=0; o<O_useful; o++) {
    for (int h=0; h<H; h++) {
      for (int w=0; w<W; w++) {
    	  addr = output_data_address(o, h, w);
        printf(" [%2d] = %3.2f\n", addr, dbg_loop_data_directconv_out[addr]);
        addr++;
      }
    }
  }
  #endif

  // print summary
  printf("\n");
  printf("JM10 debug: size_data_in_bytes = %lu\n", size_data_in_bytes);
  printf("  dbg_my_loop_data_in_sum      = %f\n", dbg_my_loop_data_in_sum);
  printf("  dbg_my_loop_out_sum          = %f\n", dbg_my_loop_out_sum);
  printf("  dbg_cpu_data_directconv_sum  = %f\n", dbg_cpu_data_directconv_sum );
  printf("\n");


}


