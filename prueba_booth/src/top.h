#include<ap_int.h>
#include<hls_stream.h>
#include <random>

//#define optDSP
#define ZeroTest
//#define old
#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

#define H                16
#define W                16
#define KH               3
#define KW               3
#define I                1
#define O                1
#define READ_GROUP_SIZE  2
#define DATA_SIZE        8

extern "C" {
void top(ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x0, ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x1, ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x2, ap_uint<DATA_SIZE*READ_GROUP_SIZE> *x3, ap_uint<8*O*I> *w, ap_uint<512> *y);
}
