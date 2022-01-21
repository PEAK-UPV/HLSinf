#include <stdio.h>
#include <string.h>
#include <hls_stream.h>

#include "da.h"

// Enable debug per module
//#define DEBUG_READ_DATA
//#define DEBUG_BROADCAST
//#define DEBUG_CU0
//#define DEBUG_CU1
//#define DEBUG_CU2
//#define DEBUG_CU3
//#define DEBUG_WRITE_DATA
//#define DEBUG_VERBOSE

// To allow using defines inside Xilinx pragmas
#define PRAGMA_SUB(x)               _Pragma (#x)
#define DO_PRAGMA(x)                PRAGMA_SUB(x)

// For loop_tripcount
#define I_REFERENCE                 IMAX
#define H_REFERENCE                 HMAX
#define W_REFERENCE                 WMAX
#define BS_REFERENCE                64
#define PIXELS_TOTAL                I_REFERENCE*H_REFERENCE*W_REFERENCE

struct proc_unit_t {
	pixel_t pixel[PU];
};


void read_image(int enable, int num_mem_reads, mem_read_block_t* data_in, hls::stream<mem_read_block_t>& mem_data_block) {
	#ifdef DEBUG_READ_DATA
	printf("READ_IMAGE: starts (ihw format)\n");
	#endif

	mem_read_block_t blk;

	if (!enable) return;

	mem_read_data_blocks_loop:
	for (int i = 0; i < num_mem_reads; i++) {
	  DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=PIXELS_TOTAL/PIXELS_PER_MEM_READ_BLOCK)
	  DO_PRAGMA(HLS pipeline)

      blk = data_in[i];
	  mem_data_block << blk;

	  #ifdef DEBUG_READ_DATA
	  #ifdef DEBUG_VERBOSE
	  printf("data read: ");
	  for (int x=0; x<PIXELS_PER_MEM_READ_BLOCK; x++) {
		  ap_int<PIXEL_WIDTH> val = blk.range((x+1)*PIXEL_WIDTH-1, x*PIXEL_WIDTH);
		  pixel_t val1 = *(pixel_t*)(&val);
		  printf("%f ", val1);
	  }
	  printf("\n");
	  #endif
	  #endif
	}

	#ifdef DEBUG_READ_DATA
	printf("READ_IMAGE: ends (ihw format)\n");
	#endif
}

void broadcast_pixels_to_cu(int num_mem_reads, hls::stream<mem_read_block_t>& in, hls::stream<mem_read_block_t> out[CU]) {

    #ifdef DEBUG_BROADCAST
    printf("DEBUG_BROADCAST: starts\n");
    #endif

	mem_read_block_t pixels;

	broadcast_read_pixels_loop:
	for (int i = 0; i < num_mem_reads; i++) {
		DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=PIXELS_TOTAL/PIXELS_PER_MEM_READ_BLOCK)
		DO_PRAGMA(HLS pipeline)

		pixels = in.read();

		broadcast_send_to_cu_lopp:
		for (int cu = 0; cu < CU; cu++) {
			DO_PRAGMA(HLS loop_tripcount  min=CU max=CU)
			#pragma HLS unroll

			out[cu] << pixels;
		}
	}

    #ifdef DEBUG_BROADCAST
    printf("DEBUG_BROADCAST: ends\n");
    #endif
}


void cu0(int num_mem_reads, int num_pixels_per_pu, int I, int H, int W, const pixel_t val, hls::stream<mem_read_block_t>& in, hls::stream<mem_write_block_t>& out) {
    #ifdef DEBUG_CU0
    printf("DEBUG_CU0: starts\n");
    #endif

    mem_read_block_t mem_data_block;
    mem_write_block_t mem_block_out;
    proc_unit_t      puv;
    DO_PRAGMA(HLS array_partition variable=puv.pixel dim=1 complete)
    proc_unit_t      puv_out;
    DO_PRAGMA(HLS array_partition variable=puv_out.pixel dim=1 complete)

    cu_read_pixels_loop:
	for (int i = 0; i < num_mem_reads; i++) {
		DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=PIXELS_TOTAL/PIXELS_PER_MEM_READ_BLOCK)
		DO_PRAGMA(HLS pipeline)

		mem_data_block = in.read();

		cu_mem_block_to_pe_loop:
		for (int p = 0; p < PIXELS_PER_MEM_READ_BLOCK; p++) {
			DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=PIXELS_PER_MEM_READ_BLOCK)
			DO_PRAGMA(HLS unroll)

			int begin = p * PIXEL_WIDTH;
			int last  = (p + 1) * PIXEL_WIDTH - 1;
			ap_int<PIXEL_WIDTH> val = mem_data_block.range(last, begin);
			puv.pixel[p] = *(pixel_t*)(&val);
		}

		cu_process_element_loop:
		for (int pu = 0; pu < PU; pu++) {
			DO_PRAGMA(HLS LOOP_TRIPCOUNT min=PU max=PU)
			DO_PRAGMA(HLS unroll)

			puv_out.pixel[pu] = puv.pixel[pu] + val;
		}

		cu_pe_to_mem_block_loop:
		for (int p = 0; p < PIXELS_PER_MEM_WRITE_BLOCK; p++) {
			DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=PIXELS_PER_MEM_WRITE_BLOCK)
			DO_PRAGMA(HLS unroll)

			int begin = p * PIXEL_WIDTH;
			int last  = (p + 1) * PIXEL_WIDTH - 1;
			mem_block_out(last, begin) = *(reinterpret_cast<ap_int<PIXEL_WIDTH>*>(&puv_out.pixel[p]));
		}

		out << mem_block_out;
	}

    #ifdef DEBUG_CU0
    printf("DEBUG_CU0: ends\n");
    #endif
}

void compute_units(int num_mem_reads, int num_pixels_per_pu, pixel_t val, int I, int H, int W, hls::stream<mem_read_block_t> input_channel[CU], hls::stream<mem_write_block_t> output_channel[CU]) {
	cu_generate_loop:
	pixel_t val1 = val;
	for (int i = 0; i < CU; i++) {
		DO_PRAGMA(HLS loop_tripcount min=CU max=CU)
		#pragma HLS unroll

		cu0(num_mem_reads, num_pixels_per_pu, I, H, W, val1, input_channel[i], output_channel[i]);
		val1++;
	}
}

void output_buffer(int num_mem_writes_per_image, hls::stream<mem_write_block_t> in[CU], hls::stream<mem_write_block_t>& out) {

	mem_write_block_t blk[CU];
	mem_write_block_t wblk;
	DO_PRAGMA(HLS array_partition variable=blk dim=1 complete)
	static mem_write_block_t buffer[CU][IMAX*HMAX*WMAX/PIXELS_PER_MEM_WRITE_BLOCK];
	DO_PRAGMA(HLS array_partition variable=buffer dim=1 complete)

	ob_external_loop:
	for (int i = 0; i < num_mem_writes_per_image; i++) {
		DO_PRAGMA(HLS loop_tripcount min=1 max=PIXELS_TOTAL/PIXELS_PER_MEM_WRITE_BLOCK)
		DO_PRAGMA(HLS pipeline)

		ob_write_buffer_loop:
		for (int cu = 0; cu < CU; cu++) {
			DO_PRAGMA(HLS loop_tripcount min=CU max=CU)
			#pragma HLS unroll

			blk[cu] = in[cu].read();
			buffer[cu][i] = blk[cu];
		}
		out << blk[0];
	}

	ob_send_images_loop:
	for (int cu = 1; cu < CU; cu++) {
		DO_PRAGMA(HLS loop_tripcount min=CU-1 max=CU-1)

		ob_read_pixels_of_image_loop:
		for (int i = 0; i < num_mem_writes_per_image; i++) {
			DO_PRAGMA(HLS loop_tripcount min=1 max=PIXELS_TOTAL/PIXELS_PER_MEM_WRITE_BLOCK)
			DO_PRAGMA(HLS dependence variable=i inter WAR distance=1 true)
			DO_PRAGMA(HLS loop_flatten off)
			DO_PRAGMA(HLS pipeline)

			wblk = buffer[cu][i];
			out << wblk;
		}
	}
}

void write_image(int num_mem_writes, int offset, hls::stream<mem_write_block_t>& in, mem_write_block_t* out) {
	#ifdef DEBUG_READ_DATA
	printf("WRITE_IMAGE: starts (ihw format)\n");
	#endif

	mem_write_block_t blk;

	mem_write_data_block_loop:
	for (int i = 0; i < num_mem_writes; i++) {
	  DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=CU*PIXELS_TOTAL/PIXELS_PER_MEM_WRITE_BLOCK)
	  DO_PRAGMA(HLS pipeline)

	  blk = in.read();
	  out[i+offset] = blk;

	  #ifdef DEBUG_WRITE_DATA
	  #ifdef DEBUG_VERBOSE
	  printf("data write: ");
	  for (int x=0; x<PIXELS_PER_MEM_WRITE_BLOCK; x++) {
		  ap_int<PIXEL_WIDTH> val = blk.range((x+1)*PIXEL_WIDTH-1, x*PIXEL_WIDTH);
		  pixel_t val1 = *(pixel_t*)(&val);
		  printf("%f ", val1);
	  }
	  printf("\n");
	  #endif
	  #endif
	}

	#ifdef DEBUG_WRITE_DATA
	printf("WRITE_IMAGE: ends (ihw format)\n");
	#endif
}

extern "C" {

void k_dataAugmentation(mem_read_block_t* data_in, mem_write_block_t* data_out, int I, int H, int W, int batch_size, int num_iter_per_batch) {
	DO_PRAGMA(HLS INTERFACE m_axi port=data_in      depth=ISIM*HSIM*WSIM/PIXELS_PER_MEM_READ_BLOCK               num_read_outstanding=16  max_read_burst_length=64 num_write_outstanding=1   max_write_burst_length=2    offset=slave bundle=gmem)
	DO_PRAGMA(HLS INTERFACE m_axi port=data_out     depth=BATCH_SIZE*ISIM*HSIM*WSIM/PIXELS_PER_MEM_WRITE_BLOCK   num_read_outstanding=1   max_read_burst_length=2  num_write_outstanding=16  max_write_burst_length=64   offset=slave bundle=gmem1)


	static hls::stream<mem_read_block_t>   mem_read_block;
	static hls::stream<mem_write_block_t>  mem_write_block;
	static hls::stream<mem_read_block_t>   cu_input_channels[CU];
	DO_PRAGMA(HLS array_partition variable=cu_input_channels dim=1 complete)
	static hls::stream<mem_write_block_t>  cu_output_channels[CU];
	DO_PRAGMA(HLS array_partition variable=cu_output_channels dim=1 complete)


	data_augmentation_iter_per_batch_loop:
	for (int i = 0; i < num_iter_per_batch; i++) {
		DO_PRAGMA(HLS loop_tripcount min=1 max=BS_REFERENCE/CU)
        #pragma HLS dataflow

		const int num_pixels = I * H * W;
		const int num_mem_reads = (num_pixels + (PIXELS_PER_MEM_READ_BLOCK - 1)) / PIXELS_PER_MEM_READ_BLOCK;
		const int num_mem_writes_per_image = (num_pixels + (PIXELS_PER_MEM_WRITE_BLOCK - 1)) / PIXELS_PER_MEM_WRITE_BLOCK;
		const int num_pixels_per_pu = (num_pixels + (PU-1)) / PU;
		const int num_mem_writes = CU * num_mem_writes_per_image;
		int offset = i * CU * num_pixels / PIXELS_PER_MEM_WRITE_BLOCK;
        float val = float(i * CU);

		int enable = 1;
		read_image(enable, num_mem_reads, data_in, mem_read_block);
		broadcast_pixels_to_cu(num_mem_reads, mem_read_block, cu_input_channels);
		compute_units(num_mem_reads, num_pixels_per_pu, val, I, H, W, cu_input_channels, cu_output_channels);
		output_buffer(num_mem_writes_per_image, cu_output_channels, mem_write_block);
		write_image(num_mem_writes, offset, mem_write_block, data_out);
	}
}

} // extern C
