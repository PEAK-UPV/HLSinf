#include<ap_int.h>
#include<hls_stream.h>
#include <random>

#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

#define NUM_VECTORS       16    // number of vectors to multiply (pipelined)
#define OUTPUT_CHANNELS   16    // Number of output channels to compute (all channels are processed in parallel)
#define VECTOR_SIZE       16    // vector size for x and w inputs (used for vector dot product)
#define NUM_READ_PIXELS   16

#define BBITS              3    // booth bits
#define ACC_BBITS          6    // booth bits capacity (number of booth bits)

#define OPT_BOOTH       // booth conversion in gates (reducing luts)


//#define DEBUG

#define USE_BOOTH
//#define USE_MULTIPLIERS

struct xw_st {
	ap_int<8> pixel[OUTPUT_CHANNELS];
};

#define xw_read_st ap_uint<NUM_READ_PIXELS*8*OUTPUT_CHANNELS>

struct xw_vector_st {
	xw_st pixel[VECTOR_SIZE];
};

struct y_st {
	ap_int<32> pixel[OUTPUT_CHANNELS];
};


#ifdef USE_BOOTH
struct xw_booth_st {
	ap_int<BBITS * 4> pixel[OUTPUT_CHANNELS];
};

struct xw_vector_booth_st {
    xw_booth_st pixel[VECTOR_SIZE];
};

struct y_booth_st {
	ap_int<ACC_BBITS * 7> pixel[OUTPUT_CHANNELS];
};

#endif

void top(xw_read_st *x, xw_read_st *w, ap_int<32> *y);
