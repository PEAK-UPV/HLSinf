#ifndef __DA_H__

#include <ap_int.h>

// Image size for CSIM/CO-SIM
#define ISIM                        4
#define HSIM                        64
#define WSIM                        32
#define BATCH_SIZE                  8


// Max values for internal buffer image size
// The modification of these values can impact the number of
// compute units that could be implemented
#define HMAX                        256
#define WMAX                        256
#define IMAX                        4

// Number of compute units to implement and Processing Elements per CU
// Max CU must be 4 due to resource occupation for xcku115-flvb2104-2-e
#define CU                          4      // Compute units.
#define PU                          16      // Processing units per CU

// Memory bus width (in bytes)
#define PIXELS_PER_MEM_READ_BLOCK   16   // It must be multiple of PU
#define PIXELS_PER_MEM_WRITE_BLOCK  16   // It must be multiple of PU
#define PIXEL_WIDTH                 32  // Number of bits required to code a pixel

// Memory bus width (in bits)
#define MEM_READ_BLOCK_WIDTH        PIXELS_PER_MEM_READ_BLOCK*PIXEL_WIDTH
#define MEM_WRITE_BLOCK_WIDTH       PIXELS_PER_MEM_WRITE_BLOCK*PIXEL_WIDTH

typedef ap_int<MEM_READ_BLOCK_WIDTH>  mem_read_block_t;
typedef ap_int<MEM_WRITE_BLOCK_WIDTH> mem_write_block_t;
//typedef ap_int<8> pixel_t;
typedef float pixel_t;



extern "C"
void k_dataAugmentation(mem_read_block_t* data_in, mem_write_block_t* data_out, int I, int H, int W, int batch_size, int num_iter_per_batch);


#endif  // ifndef __DA_H__
