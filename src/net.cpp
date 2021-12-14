#include "ap_int.h"

#include "net.h"

#include <hls_stream.h>

#include "weights.h"

#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

//#define DEBUG
//#efine DEBUG_READ
//#define DEBUG_DIV
//#define DEBUG_MUL
//#define DEBUG_SUB
//#define DEBUG_BATCH_NORMALIZATION
//#define DEBUG_QUANTIZE
//#define DEBUG_CONV_MUL
//#define DEBUG_MUL_BIPOLAR
//#define DEBUG_PADDING
//#define DEBUG_CVT
//#define DEBUG_BIPOLAR
//#define DEBUG_MAXPOOL
//#define DEBUG_MATMUL

// ------------------------------------------------------------------------------------------
// broadcast module
//
// Parameters:
//    ITEMS: items 
//    WIDTH: width of each item
// 
// Arguments:
//    in: input stream
//    out1: output stream
//    out2: output stream
//
// This module broadcast the input stream to two output streams
//
template<int ITEMS, int WIDTH> void broadcast(hls::stream<ap_uint<WIDTH>> &in, hls::stream<ap_uint<WIDTH>> &out1, hls::stream<ap_uint<WIDTH>> &out2) {
  #ifdef DEBUG
  printf("broadcast ITEMS %d WIDTH %d\n", ITEMS, WIDTH);
  #endif

  for (int i=0; i < ITEMS; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<WIDTH> px_in;
    px_in = in.read();
    out1 << px_in;
    out2 << px_in;
  }
}

// ------------------------------------------------------------------------------------------
// broadcast_1x4 module
//
// Parameters:
//    ITEMS: items 
//    WIDTH: width of each item
// 
// Arguments:
//    in: input stream
//    out1: output stream
//    out2: output stream
//    out3: output stream
//    out4: output stream
//
// This module broadcast the input stream to four output streams
//
template<int ITEMS, int WIDTH> void broadcast_1x4(hls::stream<ap_uint<WIDTH>> &in, hls::stream<ap_uint<WIDTH>> &out1, hls::stream<ap_uint<WIDTH>> &out2, hls::stream<ap_uint<WIDTH>> &out3, hls::stream<ap_uint<WIDTH>> &out4) {
  #ifdef DEBUG
  printf("broadcast_1x4 ITEMS %d WIDTH %d\n", ITEMS, WIDTH);
  #endif

  for (int i=0; i < ITEMS; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<WIDTH> px_in;
    px_in = in.read();
    out1 << px_in;
    out2 << px_in;
    out3 << px_in;
    out4 << px_in;
  }
}

// ------------------------------------------------------------------------------------------
// reduce module
//
// Parameters:
//    C: Number of channels
//    H: Height of data channel
//    W: Width of data channel
//    DATA_WIDTH: Data item width in bits
//    PE: Number of processing elelemtns
// 
// Arguments:
//    in1: input stream
//    in2: input stream
//    out: output stream
//
// This module reduces two input streams into a single one
//
template<int C, int H, int W, int DATA_WIDTH, int PE> void reduce(hls::stream<ap_uint<DATA_WIDTH * C>> &in1, hls::stream<ap_uint<DATA_WIDTH * C>> &in2, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
  #ifdef DEBUG
  printf("reduce %dx%dx%d\n", C, H, W);
  #endif

  int block_channels = C / PE;                     // Number of block channels to serializa per input data item
  int num_iterations = H * W * block_channels;     // Number of iterations to perform
  int block = 0;                                   // Current block channel being performed

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px1;
    ap_uint<DATA_WIDTH * C> px2;
    ap_uint<DATA_WIDTH * C> p_out;

    // Read from input stream
    if (block == 0) {px1 = in1.read(); px2 = in2.read();}

    // We perform PE operations in parallel
    //for (int pe = 0; pe < PE; pe++) {
    //  DO_PRAGMA(HLS UNROLL)

      // Get the correct bits from the input data
      int c_pe = block /*+ pe*/;
      int first = c_pe * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux1 = px1.range(last, first);
      int f_in1;
      *(ap_uint<DATA_WIDTH>*)(&f_in1) = (int)aux1;
      ap_uint<DATA_WIDTH> aux2 = px2.range(last, first);
      int f_in2;
      *(ap_uint<DATA_WIDTH>*)(&f_in2) = (int)aux2;

      // Perform the add operation and write it to the output
      int f_out;
      f_out = f_in1 + f_in2;
      p_out.range(DATA_WIDTH-1, 0) = (ap_uint<DATA_WIDTH>)f_out;
      p_out = p_out << DATA_WIDTH;
      //p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_out);
    //}

    // Next block channel
    block = block + 1; //PE;
    // Check end of block channel
    if (block == C) {
      block = 0;
      // Output block channel
      out << p_out;
    }
  }
}

// --------------------------------------------------------------------------------------------------------------------------
// matmul_bipolar
//
// Parameters:
//    N: Number of matrix rows
//    M: Number of matrix cols
//    DATA_WIDTH: Data item width in bits for the output
//    PE: Number of processing elements in parallel
//
// Arguments:
//    in: input stream (stream objects of 1 * N)
//    weights[M]: matrix of weights (N x M) (matrix objects of 1)
//    out: output stream (stream objects of DATA_WIDTH * M)
//
// This module performs the multiplications of a vector V by a Matrix W. The vector is of size N and the matrix
// is of size N rows and M columns. The output is a vector of M elements. All vector and matrix elements (at the input)
//  are considered bipolar of one bit (0 or 1). The output elements are integers of size DATA_WIDTH.
//
// TODO: Check correctness and add parallelization (PE)
template<int N, int M, int DATA_WIDTH, int PE> void matmul_bipolar(hls::stream<ap_uint<1 * N>> &in, ap_uint<1 * N> weights[M], hls::stream<ap_uint<DATA_WIDTH * M>> &out) {

  #ifdef DEBUG
  printf("matmul_bipolar %d x %dx%d\n", N, N, M);
  #endif

  ap_uint<1 * N> p_in;                    // Input item
  ap_uint<DATA_WIDTH * M> p_out;          // Output item
  int r = 0;                           

  p_in = in.read();

  int num_iterations = N * M;
  int n = 0;
  int m = 0;

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<1> x = p_in.range(n, n);
    ap_uint<1> y = weights[m].range(n,n);
    
    if (x ^ y) r = r + 1; else r = r - 1;

    n = n + 1;
    if (n == N) {
      n = 0;
      float v = (float) r;
      //int first = m * DATA_WIDTH;
      //int last = first + DATA_WIDTH - 1;
      p_out.range(DATA_WIDTH-1, 0) = (ap_uint<DATA_WIDTH>)v;
      p_out = p_out << DATA_WIDTH;
      //p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&v);
      m = m + 1;
      r = 0;
    }
  }
  out << p_out;
}

// ----------------------------------------------------------------------------------------------------------------------------
// matmul_bipolar_2x4
//
// Parameters
//    N: Number of matrix rows
//    M: Number of matrix cols
//    DATA_WIDTH: Data item width in bits for the output
//    PE: Number of processing elements in parallel
//
// Arguments:
//    in1: input stream 1 (stream objects of 1 * N / 2)
//    in2: input stream 2 (stream objects of 1 * N / 2)
//    weights1a[M/4]: matrix of weights (N/2 x M/4) (matrix objects of 1)
//    weights1b[M/4]: matrix of weights (N/2 x M/4) (matrix objects of 1)
//    weights1c[M/4]: matrix of weights (N/2 x M/4) (matrix objects of 1)
//    weights1d[M/4]: matrix of weights (N/2 x M/4) (matrix objects of 1)
//    weights2a[M/4]: matrix of weights (N/2 x M/4) (matrix objects of 1)
//    weights2b[M/4]: matrix of weights (N/2 x M/4) (matrix objects of 1)
//    weights2c[M/4]: matrix of weights (N/2 x M/4) (matrix objects of 1)
//    weights2d[M/4]: matrix of weights (N/2 x M/4) (matrix objects of 1)
//    out1: output stream (stream objects of DATA_WIDTH * M / 4)
//    out2: output stream (stream objects of DATA_WIDTH * M / 4)
//    out3: output stream (stream objects of DATA_WIDTH * M / 4)
//    out4: output stream (stream objects of DATA_WIDTH * M / 4)
//
// This module performs the multiplications of a vector V by a Matrix W. The vector is of size N and the matrix
// is of size N rows and M columns. The output is a vector of M elements. All vector and matrix elements (at the input)
//  are considered bipolar of one bit (0 or 1). The output elements are integers of size DATA_WIDTH.
// This module assumes two input streams and four output streams (to avoid stream width limit issues)
//
template<int N, int M, int DATA_WIDTH, int PE> void matmul_bipolar_2x4(hls::stream<ap_uint<1 * N/2>> &in1, hls::stream<ap_uint<1 * N/2>> &in2, 
                                                                       ap_uint<1 * N / 2> weights1a[M / 4], ap_uint<1 * N / 2> weights1b[M / 4], ap_uint<1 * N / 2> weights1c[M / 4], ap_uint<1 * N / 2> weights1d[M / 4],
                                                                       ap_uint<1 * N / 2> weights2a[M / 4], ap_uint<1 * N / 2> weights2b[M / 4], ap_uint<1 * N / 2> weights2c[M / 4], ap_uint<1 * N / 2> weights2d[M / 4],
                                                                       hls::stream<ap_uint<DATA_WIDTH * M / 4>> &out1, hls::stream<ap_uint<DATA_WIDTH * M / 4>> &out2, hls::stream<ap_uint<DATA_WIDTH * M / 4>> &out3, hls::stream<ap_uint<DATA_WIDTH * M / 4>> &out4) {
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_1a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_1b;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_1c;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_1d;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_2a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_2b;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_2c;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_2d;

 hls::stream<ap_uint<1 * N / 2>> in1a;
 hls::stream<ap_uint<1 * N / 2>> in1b;
 hls::stream<ap_uint<1 * N / 2>> in1c;
 hls::stream<ap_uint<1 * N / 2>> in1d;
 hls::stream<ap_uint<1 * N / 2>> in2a;
 hls::stream<ap_uint<1 * N / 2>> in2b;
 hls::stream<ap_uint<1 * N / 2>> in2c;
 hls::stream<ap_uint<1 * N / 2>> in2d;

 broadcast_1x4<1, N / 2>(in1, in1a, in1b, in1c, in1d);
 broadcast_1x4<1, N / 2>(in2, in2a, in2b, in2c, in2d);

 DO_PRAGMA(HLS DATAFLOW)

 matmul_bipolar<N / 2, M / 4, DATA_WIDTH, PE>(in1a, weights1a, mul_out_1a);
 matmul_bipolar<N / 2, M / 4, DATA_WIDTH, PE>(in1b, weights1b, mul_out_1b);
 matmul_bipolar<N / 2, M / 4, DATA_WIDTH, PE>(in1c, weights1c, mul_out_1c);
 matmul_bipolar<N / 2, M / 4, DATA_WIDTH, PE>(in1d, weights1d, mul_out_1d);
 matmul_bipolar<N / 2, M / 4, DATA_WIDTH, PE>(in2a, weights2a, mul_out_2a);
 matmul_bipolar<N / 2, M / 4, DATA_WIDTH, PE>(in2b, weights2b, mul_out_2b);
 matmul_bipolar<N / 2, M / 4, DATA_WIDTH, PE>(in2c, weights2c, mul_out_2c);
 matmul_bipolar<N / 2, M / 4, DATA_WIDTH, PE>(in2d, weights2d, mul_out_2d);

 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_1a, mul_out_2a, out1);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_1b, mul_out_2b, out2);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_1c, mul_out_2c, out3);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_1d, mul_out_2d, out4);

}

// ----------------------------------------------------------------------------------------------------------------------------
// matmul_bipolar_4x4
//
// Parameters
//    N: Number of matrix rows
//    M: Number of matrix cols
//    DATA_WIDTH: Data item width in bits for the output
//    PE: Number of processing elements in parallel
//
// Arguments:
//    in1: input stream 1 (stream objects of 1 * N / 4)
//    in2: input stream 2 (stream objects of 1 * N / 4)
//    in3: input stream 3 (stream objects of 1 * N / 4)
//    in4: input stream 4 (stream objects of 1 * N / 4)
//    weights1a[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights1b[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights1c[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights1d[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights2a[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights2b[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights2c[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights2d[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights3a[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights3b[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights3c[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights3d[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights4a[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights4b[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights4c[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    weights4d[M/4]: matrix of weights (N/4 x M/4) (matrix objects of 1)
//    out1: output stream (stream objects of DATA_WIDTH * M / 4)
//    out2: output stream (stream objects of DATA_WIDTH * M / 4)
//    out3: output stream (stream objects of DATA_WIDTH * M / 4)
//    out4: output stream (stream objects of DATA_WIDTH * M / 4)
//
// This module performs the multiplications of a vector V by a Matrix W. The vector is of size N and the matrix
// is of size N rows and M columns. The output is a vector of M elements. All vector and matrix elements (at the input)
//  are considered bipolar of one bit (0 or 1). The output elements are integers of size DATA_WIDTH.
// This module assumes four input streams and four output streams (to avoid stream width limit issues)
//
template<int N, int M, int DATA_WIDTH, int PE> void matmul_bipolar_4x4(hls::stream<ap_uint<1 * N/4>> &in1, hls::stream<ap_uint<1 * N/4>> &in2, hls::stream<ap_uint<1 * N/4>> &in3, hls::stream<ap_uint<1 * N/4>> &in4, 
                                                                       ap_uint<1 * N / 4> weights1a[M / 4], ap_uint<1 * N / 4> weights1b[M / 4], ap_uint<1 * N / 4> weights1c[M / 4], ap_uint<1 * N / 4> weights1d[M / 4],
                                                                       ap_uint<1 * N / 4> weights2a[M / 4], ap_uint<1 * N / 4> weights2b[M / 4], ap_uint<1 * N / 4> weights2c[M / 4], ap_uint<1 * N / 4> weights2d[M / 4],
                                                                       ap_uint<1 * N / 4> weights3a[M / 4], ap_uint<1 * N / 4> weights3b[M / 4], ap_uint<1 * N / 4> weights3c[M / 4], ap_uint<1 * N / 4> weights3d[M / 4],
                                                                       ap_uint<1 * N / 4> weights4a[M / 4], ap_uint<1 * N / 4> weights4b[M / 4], ap_uint<1 * N / 4> weights4c[M / 4], ap_uint<1 * N / 4> weights4d[M / 4],
                                                                       hls::stream<ap_uint<DATA_WIDTH * M / 4>> &out1, hls::stream<ap_uint<DATA_WIDTH * M / 4>> &out2, hls::stream<ap_uint<DATA_WIDTH * M / 4>> &out3, hls::stream<ap_uint<DATA_WIDTH * M / 4>> &out4) {
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_1a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_1b;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_1c;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_1d;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_2a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_2b;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_2c;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_2d;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_3a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_3b;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_3c;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_3d;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_4a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_4b;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_4c;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> mul_out_4d;

 hls::stream<ap_uint<1 * N / 4>> in1a;
 hls::stream<ap_uint<1 * N / 4>> in1b;
 hls::stream<ap_uint<1 * N / 4>> in1c;
 hls::stream<ap_uint<1 * N / 4>> in1d;
 hls::stream<ap_uint<1 * N / 4>> in2a;
 hls::stream<ap_uint<1 * N / 4>> in2b;
 hls::stream<ap_uint<1 * N / 4>> in2c;
 hls::stream<ap_uint<1 * N / 4>> in2d;
 hls::stream<ap_uint<1 * N / 4>> in3a;
 hls::stream<ap_uint<1 * N / 4>> in3b;
 hls::stream<ap_uint<1 * N / 4>> in3c;
 hls::stream<ap_uint<1 * N / 4>> in3d;
 hls::stream<ap_uint<1 * N / 4>> in4a;
 hls::stream<ap_uint<1 * N / 4>> in4b;
 hls::stream<ap_uint<1 * N / 4>> in4c;
 hls::stream<ap_uint<1 * N / 4>> in4d;

 broadcast_1x4<1, N / 4>(in1, in1a, in1b, in1c, in1d);
 broadcast_1x4<1, N / 4>(in2, in2a, in2b, in2c, in2d);
 broadcast_1x4<1, N / 4>(in3, in3a, in3b, in3c, in3d);
 broadcast_1x4<1, N / 4>(in4, in4a, in4b, in4c, in4d);


 DO_PRAGMA(HLS DATAFLOW)

 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in1a, weights1a, mul_out_1a);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in1b, weights1b, mul_out_1b);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in1c, weights1c, mul_out_1c);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in1d, weights1d, mul_out_1d);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in2a, weights2a, mul_out_2a);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in2b, weights2b, mul_out_2b);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in2c, weights2c, mul_out_2c);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in2d, weights2d, mul_out_2d);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in3a, weights3a, mul_out_3a);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in3b, weights3b, mul_out_3b);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in3c, weights3c, mul_out_3c);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in3d, weights3d, mul_out_3d);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in4a, weights4a, mul_out_4a);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in4b, weights4b, mul_out_4b);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in4c, weights4c, mul_out_4c);
 matmul_bipolar<N / 4, M / 4, DATA_WIDTH, PE> (in4d, weights4d, mul_out_4d);

 hls::stream<ap_uint<DATA_WIDTH * M / 4>> out1_a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> out2_a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> out3_a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> out4_a;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> out1_b;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> out2_b;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> out3_b;
 hls::stream<ap_uint<DATA_WIDTH * M / 4>> out4_b;
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_1a, mul_out_2a, out1_a);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_1b, mul_out_2b, out2_a);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_1c, mul_out_2c, out3_a);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_1d, mul_out_2d, out4_a);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_3a, mul_out_4a, out1_b);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_3b, mul_out_4b, out2_b);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_3c, mul_out_4c, out3_b);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(mul_out_3d, mul_out_4d, out4_b);

 reduce<M/4, 1, 1, DATA_WIDTH, PE>(out1_a, out1_b, out1);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(out2_a, out2_b, out2);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(out3_a, out3_b, out3);
 reduce<M/4, 1, 1, DATA_WIDTH, PE>(out4_a, out4_b, out4);

}

// ----------------------------------------------------------------------------------------------------------------------------
// matmul_bipolar_4x1
//
// Parameters
//    N: Number of matrix rows
//    M: Number of matrix cols
//    DATA_WIDTH: Data item width in bits for the output
//    PE: Number of processing elements in parallel
//
// Arguments:
//    in1: input stream 1 (stream objects of 1 * N / 4)
//    in2: input stream 2 (stream objects of 1 * N / 4)
//    in3: input stream 3 (stream objects of 1 * N / 4)
//    in4: input stream 4 (stream objects of 1 * N / 4)
//    weights1a[M/4]: matrix of weights (N/4 x M) (matrix objects of 1)
//    weights2a[M/4]: matrix of weights (N/4 x M) (matrix objects of 1)
//    weights3a[M/4]: matrix of weights (N/4 x M) (matrix objects of 1)
//    weights4a[M/4]: matrix of weights (N/4 x M) (matrix objects of 1)
//    out: output stream (stream objects of DATA_WIDTH * M)
//
// This module performs the multiplications of a vector V by a Matrix W. The vector is of size N and the matrix
// is of size N rows and M columns. The output is a vector of M elements. All vector and matrix elements (at the input)
//  are considered bipolar of one bit (0 or 1). The output elements are integers of size DATA_WIDTH.
// This module assumes four input streams and four output streams (to avoid stream width limit issues)
//
template<int N, int M, int DATA_WIDTH, int PE> void matmul_bipolar_4x1(hls::stream<ap_uint<1 * N/4>> &in1, hls::stream<ap_uint<1 * N/4>> &in2, hls::stream<ap_uint<1 * N/4>> &in3, hls::stream<ap_uint<1 * N/4>> &in4, 
                                                                       ap_uint<1 * N / 4> weights1a[M], ap_uint<1 * N / 4> weights2a[M], ap_uint<1 * N / 4> weights3a[M], ap_uint<1 * N / 4> weights4a[M],
                                                                       hls::stream<ap_uint<DATA_WIDTH * M>> &out) {
 hls::stream<ap_uint<DATA_WIDTH * M>> mul_out_1a;
 hls::stream<ap_uint<DATA_WIDTH * M>> mul_out_2a;
 hls::stream<ap_uint<DATA_WIDTH * M>> mul_out_3a;
 hls::stream<ap_uint<DATA_WIDTH * M>> mul_out_4a;

 DO_PRAGMA(HLS DATAFLOW)

 matmul_bipolar<N / 4, M, DATA_WIDTH, PE> (in1, weights1a, mul_out_1a);
 matmul_bipolar<N / 4, M, DATA_WIDTH, PE> (in2, weights2a, mul_out_2a);
 matmul_bipolar<N / 4, M, DATA_WIDTH, PE> (in3, weights3a, mul_out_3a);
 matmul_bipolar<N / 4, M, DATA_WIDTH, PE> (in4, weights4a, mul_out_4a);

 hls::stream<ap_uint<DATA_WIDTH * M>> out_a;
 hls::stream<ap_uint<DATA_WIDTH * M>> out_b;
 reduce<M, 1, 1, DATA_WIDTH, PE>(mul_out_1a, mul_out_2a, out_a);
 reduce<M, 1, 1, DATA_WIDTH, PE>(mul_out_3a, mul_out_4a, out_b);
 reduce<M, 1, 1, DATA_WIDTH, PE>(out_a, out_b, out);
}

// ------------------------------------------------------------------------------------------
// read module
//
// Parameters:
//    C: Number of channels
//    H: Height
//    W: Width
//    DATA_WIDTH: Data item width in bits
//
// Arguments:
//    ptr: Pointer to data in memory (memory objects of size DATA_WIDTH * C)
//    out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module reads data from main memory and writes it into an output stream
//    The data read is in format HxWxC
//
template<int C, int H, int W, int DATA_WIDTH> void read(ap_uint<DATA_WIDTH * C> *ptr, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {

  #ifdef DEBUG
  printf("read %dx%dx%d\n", C, H, W);
  #endif

  for (int i = 0; i < H * W; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<DATA_WIDTH * C> px = ptr[i];

    #ifdef DEBUG_READ
    printf("iteration %d: ", i);
    for (int p=0; p<C; p++) {
      int first = p * DATA_WIDTH;
      int last = first + DATA_WIDTH-1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f;
      *(ap_uint<DATA_WIDTH>*)(&f) = (float)aux;
      printf("%f ", f);
    }
    printf("\n");
    #endif

    out << px;
  } 

  #ifdef DEBUG
  printf("end read\n");
  #endif
}

// -------------------------------------------------------------------------------------------------------
// div module
//
// Parameters:
//   C: Number of data channels
//   H: Height of data channel
//   W: Width of data channel
//   DATA_WIDTH: Data item width in bits
//   PE: Number of processing elements to use
//
// Arguments:
//   in: Input stream (stream objects of size DATA_WIDTH * C)
//   F:  Float value (divisor)
//   out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module performs a division of every input data. Input channels are independent and
// can be operated in parallel
//
template<int C, int H, int W, int DATA_WIDTH, int PE> void div(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float F, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {

  #ifdef DEBUG 
  printf("div %dx%dx%d\n", C, H, W);
  #endif 

  int block_channels = C / PE;                   // Number of block channels to serialize per input data item
  int num_iterations = H * W * block_channels;   // Number of iterations to perform
  int block = 0;                                 // Current block channel being performed

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    // Read from input stream
    if (block == 0) px = in.read();

    // We perform PE operations in parallel
    for (int pe=0; pe<PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      // Get the correct bits from the input data
      int first = (block + pe) * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;

      #ifdef DEBUG_DIV
      printf(" DIV (%f): i = %d, channel = %d, f_in = %f\n", F, i, block + pe, f_in);
      #endif

      // Perform the division operation and add it to the output
      f_in = f_in / F;
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_in);
    }

    // Next block channel
    block = block + PE;

    // Check end of block channel
    if (block == C) {
      block = 0;
      // Output block channel
      out << p_out;
    }
  }
}

// -------------------------------------------------------------------------------------------------------
// mul module
//
// Parameters:
//   C: Number of data channels
//   H: Height of data channel
//   W: Width of data channel
//   DATA_WIDTH: Data item width in bits
//   PE: Number of processing elements to use
//
// Arguments:
//   in: Input stream (stream objects of size DATA_WIDTH * C)
//   F:  Float value (multiplier)
//   out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module performs a multiplication of every input data. Input channels are independent and
// can be operated in parallel
//
template<int C, int H, int W, int DATA_WIDTH, int PE> void mul(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float F, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {

  #ifdef DEBUG 
  printf("mul %dx%dx%d\n", C, H, W);
  #endif 

  int block_channels = C / PE;                  // Number of block channels to serialize per input data item
  int num_iterations = H * W * block_channels;  // Number of iterations to perform
  int block = 0;                                // Current block channel being performed

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    // read from input stream
    if (block == 0) px = in.read();

    // We perform PE operations in parallel
    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      // Get the correct bits from the input data
      int first = (block + pe) * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;

      #ifdef DEBUG_MUL
      printf(" MUL (%f): i = %d, channel = %d, f_in = %f\n", F, i, block + pe, f_in);
      #endif

      // Perform the multiplication operation and add it to the output
      f_in = f_in * F;
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_in);
    }

    // Next block channel
    block = block + PE;

    // Check end of block channel
    if (block == C) {
      block = 0;
      // Output block channel
      out << p_out;
    }
  }
}

// -------------------------------------------------------------------------------------------------------
// sub module
//
// Parameters:
//   C: Number of data channels
//   H: Height of data channel
//   W: Width of data channel
//   DATA_WIDTH: Data item width in bits
//   PE: Number of processing elements to use
//
// Arguments:
//   in: Input stream (stream objects of size DATA_WIDTH * C)
//   F:  Float value (substraend)
//   out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module performs a substraction of every input data. Input channels are independent and
// can be operated in parallel
//
template<int C, int H, int W, int DATA_WIDTH, int PE> void sub(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float F, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {

  #ifdef DEBUG 
  printf("sub %dx%dx%d\n", C, H, W);
  #endif 

  int block_channels = C / PE;                  // Number of block channels to serialize per input data item
  int num_iterations = H * W * block_channels;  // Number of iterations to perform
  int block = 0;                                // Current block channel being performed

  for (int i=0; i< num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;
 
    // Read from input stream
    if (block == 0) px = in.read();

    // We perform PE operations in parallel
    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      // Get the correct bits from the input data
      int first = (block + pe) * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;

      #ifdef DEBUG_SUB
      printf(" SUB (%f): i = %d, channel = %d, f_in = %f -> %f\n", F, i, block + pe, f_in, f_in - F);
      #endif

      // We perform the substraction operation and add it to the output
      f_in = f_in - F;
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_in);
    }

    // Next block channel
    block = block + PE;

    // Check end of block channel
    if (block == C) {
      block = 0;
      // Output block channel
      out << p_out;
    }
  }
}

// -------------------------------------------------------------------------------------------------------
// quantize module
//
// Parameters:
//   C: Number of data channels
//   H: Height of data channel
//   W: Width of data channel
//   DATA_WIDTH: Data item width in bits
//   PE: Number of processing elements to use
//
// Arguments:
//   in: Input stream (stream objects of size DATA_WIDTH * C)
//   min: minimum value for quantization
//   max: maximum value for quantization
//   stride: stride for quantiation
//   out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module quantizes the input based on min, max, and stride. If the input data is lower than min then 
// the output is set to min value. If the input data is higher then max then the output is set to max value.
// If the input is between min and max values, then the output is quantized. Quantization is performed as follows:
//    count  = (input - min) / stride
//    output = count * stride
// Input channels are independent and can be operated in parallel
//
template<int C, int H, int W, int DATA_WIDTH, int PE> void quantize(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float min, float max, float stride, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {

  #ifdef DEBUG 
  printf("quantize %dx%dx%d\n", C, H, W);
  #endif 

  int block_channels = C / PE;                   // Number of block channels to serialize per input data item
  int num_iterations = H * W * block_channels;   // Number of iterations to perform
  int block = 0;                                 // Current block channel being performed
  
  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    // Read from input stream
    if (block == 0) px = in.read();

    // We perform PE operations in parallel
    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)
	     
      // Get the correct bits from the input data 
      int first = (block + pe) * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;

      // We perform the quantization operation
      float f_out;
      if (f_in < min) f_out = min;
      else if (f_in > max) f_out = max;
      else {
	int count = (int) ((f_in - min) / stride);
	f_out = min + ((float)count * stride);
      }

      #ifdef DEBUG_QUANTIZE
      printf("QUANTIZE (min %f, max %f, stride %f) : in %f out %f\n", min, max, stride, f_in, f_out);
      #endif

      // We write the quantized data to the output
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_out);
    }

    // Next block channel
    block = block + PE;
    // Check end of block channel
    if (block == C) {
      block = 0;
      // Output block channel
      out << p_out;
    }
  }
}

// -------------------------------------------------------------------------------------------------------
// bipolar module
//
// Parameters:
//   C: Number of data channels
//   H: Height of data channel
//   W: Width of data channel
//   DATA_WIDTH: Data item width in bits
//   PE: Number of processing elements to use
//
// Arguments:
//   in: Input stream (stream objects of size DATA_WIDTH * C)
//   out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module performs the bipolar transformation to the input. The input data is assumed to be float numbers
// Negative numbers are coded at the output as a zero value and positive numbers as a one value
// Input channels are independent and can be operated in parallel
//
template<int C, int H, int W, int DATA_WIDTH, int PE> void bipolar(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<1 * C>> &out) {

  #ifdef DEBUG 
  printf("bipolar %dx%dx%d\n", C, H, W);
  #endif 

  int block_channels = C / PE;                    // Number of block channels to serialize per input data item
  int num_iterations = H * W * block_channels;    // Number of iterations to perform
  int block = 0;                                  // Current block channel being performed

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px;
    ap_uint<1 * C> p_out;

    // Read from input stream
    if (block == 0) px = in.read();

    // We perform PE operations in parallel
    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      // Get the correct bits from the input data
      int c_pe = block + pe;
      int first = c_pe * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;

      // Perform the bipolar operation
      ap_uint<1> v_out;
      if (f_in < 0) v_out = 0;
      else v_out = 1;

      #ifdef DEBUG_BIPOLAR
      printf("BIPOLAR: in %f out %d\n", f_in, (int)v_out);
      #endif

      // We wite the bipolar data to the output
      p_out.range(c_pe, c_pe) = v_out;
    }

    // Next block channel
    block = block + PE;
    // Check end of block channel
    if (block == C) {
      block = 0;
      // Output block channel
      out << p_out;
    }
  }
}

// -------------------------------------------------------------------------------------------------------
// add module
//
// Parameters:
//   C: Number of data channels
//   H: Height of data channel
//   W: Width of data channel
//   DATA_WIDTH: Data item width in bits
//   PE: Number of processing elements to use
//
// Arguments:
//   in: Input stream (stream objects of size DATA_WIDTH * C)
//   f:  Float value (adder)
//   out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module performs an addition to every input data. Input channels are independent and
// can be operated in parallel
//
template<int C, int H, int W, int DATA_WIDTH, int PE> void add(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float f, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {

  #ifdef DEBUG 
  printf("add %dx%dx%d\n", C, H, W);
  #endif 

  int block_channels = C / PE;                     // Number of block channels to serializa per input data item
  int num_iterations = H * W * block_channels;     // Number of iterations to perform
  int block = 0;                                   // Current block channel being performed

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    // Read from input stream
    if (block == 0) px = in.read();

    // We perform PE operations in parallel
    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      // Get the correct bits from the input data
      int c_pe = block + pe;
      int first = c_pe * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;

      // Perform the add operation and write it to the output
      float f_out;
      f_out = f_in + f;
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_out);
    }

    // Next block channel
    block = block + PE;
    // Check end of block channel
    if (block == C) {
      block = 0;
      // Output block channel
      out << p_out;
    }
  }
}

// ------------------------------------------------------------------------------------------
// write module
//
// Parameters:
//    C: Number of channels
//    H: Height
//    W: Width
//    DATA_WIDTH: Data item width in bits
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH * C)
//    ptr: Pointer to data in memory (memory objects of size DATA_WIDTH * C)
//
// This module writes data items to main memory from its input stream
//    The data written is in format HxWxC
//
template<int C, int H, int W, int DATA_WIDTH> void write(hls::stream<ap_uint<DATA_WIDTH * C>> &in, ap_uint<DATA_WIDTH * C> *ptr) {

  #ifdef DEBUG 
  printf("write %dx%dx%d\n", C, H, W);
  #endif 

  for (int i = 0; i < H * W; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px = in.read();
    ptr[i] = px;
  }
}

// ------------------------------------------------------------------------------------------
// padding module
//
// Parameters:
//    C: Number of channels
//    H: Height
//    W: Width
//    PT, PB, PL, PR: Padding at top, bottom, left, right
//    DATA_WIDTH: Data item width in bits
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH * C)
//    out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module adds padding to the input data read from the in stream and sends new data
// to the output stream (out)
//
template<int C, int H, int W, int PT, int PB, int PL, int PR, int DATA_WIDTH> void padding(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {

  #ifdef DEBUG 
  printf("padding %dx%dx%d\n", C, H, W);
  #endif 
  
  // all_zeros variable (to insert padding)
  ap_uint<DATA_WIDTH * C> all_zeros;
  all_zeros = 0;

  // Two loops to sweep all the input data and decide when to use padding
  // and one extra loop for the serialization factor
  for (int h = -PT; h < H + PB; h++) {
    for (int w = -PL; w < W + PR; w++) {
      ap_uint<DATA_WIDTH * C> p_in;
      // If we are in the padding region then we select the padded variable if not then we read from input stream
      if ((h >= 0) && (h < H) && (w >= 0) && (w < W)) p_in = in.read(); else p_in = all_zeros;

      #ifdef DEBUG_PADDING
      printf("PADDING: h=%d w=%d: ", h, w);
      for (int c=0; c<C; c++) {
        int first = c * DATA_WIDTH;
        int last = first + DATA_WIDTH - 1;
        ap_uint<DATA_WIDTH> x = p_in.range(last, first);
        float f;
        *(ap_uint<DATA_WIDTH>*)(&f) = (float)x;
        printf("%f ", f);
      }
      printf("\n"); 
      #endif      
      // We output the data
      out << p_in;
    }
  }
}

// ------------------------------------------------------------------------------------------
// cvt module
//
// Parameters:
//    C: Number of channels
//    H: Height of data channel
//    W: Width of data channel
//    KH, KW: Height and Width of the filter
//    SH, SW: Stride (horizontal and vertical)
//    DATA_WIDTH: Data item width in bits
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH * C)
//    out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module converts an input image on a set of frames of KHxKW size assuming SHxSW stride
//
template<int C, int H, int W, int KH, int KW, int SH, int SW, int DATA_WIDTH> void cvt(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<DATA_WIDTH * KH * KW * C>> &out) {

  #ifdef DEBUG 
  printf("cvt %dx%dx%d\n", C, H, W);
  #endif

  ap_uint<DATA_WIDTH * C> buff[KH][W];    // row buffer
  ap_uint<DATA_WIDTH * C> p_in;           // input item
  ap_uint<DATA_WIDTH * C> frame[KH][KW];  // output frame

  int row_to_write = 0;                   // control variable (row to write on the row buffer)
  int col_to_write = 0;                   // control variable (column to write on the row buffer)
  int stride_row = KH;                    // control variable (to check when stride in rows has been exhausted)
  int stride_col = KW;                    // control variable (to check when stride in cols has been exhausted)
  int row = 0;                            // control variable (current row)
  int col = 0;                            // control variable (current col)

  // Loop for all the input items
  for (int p = 0; p < H * W; p++) {

      #ifdef DEBUG_CVT
      printf("CVT: p = %d, row_to_write %d, col_to_write %d, stride_row %d stride_col %d\n", p, row_to_write, col_to_write, stride_row, stride_col);
      #endif

      // read input item
      p_in = in.read();

      #ifdef DEBUG_CVT
      printf("CVT: item received\n");
      for (int c = 0; c < C; c++) {
        int first = c * DATA_WIDTH;
        int last = first + DATA_WIDTH - 1;
        ap_uint<DATA_WIDTH> x = p_in.range(last, first);
        float f;
        *(ap_uint<DATA_WIDTH>*)(&f) = (float)x;
        printf(" %f", f);
      }
      printf("\n");
      #endif

      // write in row buffer
      buff[row_to_write][col_to_write] = p_in;

      // frame copy from the buffer
      if ((row >= KH-1) && (col >= KW-1)) {
        for (int kh = 0; kh < KH; kh++) {
  	  for (int kw = 0; kw < KW; kw++) {
	    int row_buffer = (row-KH+1+kh) % KH;
	    int col_buffer = (col-KW+1+kw);
            #ifdef DEBUG_CVT
	    printf("copy from buffer: kh %d kw %d -> row %d col %d\n", kh, kw, row_buffer, col_buffer);
            #endif
	    frame[kh][kw] = buff[row_buffer][col_buffer];
	  }
        }
      }

      // Check if we have a frame to send
      int send_frame = (row >= KH-1) && (col >= KW-1) && (stride_row == 1) && (stride_col == 1);

      if (send_frame) {
        //
        #ifdef DEBUG_CVT
        printf("FRAME: ");
        for (int kh=0; kh < KH; kh++) {
	  for (int kw=0; kw < KW; kw++) {
	    ap_uint<DATA_WIDTH * C> item;
  	    item = frame[kh][kw];
  	    for (int c = 0; c < C; c++) {
              int first = c * DATA_WIDTH;
	      int last = first + DATA_WIDTH - 1;
	      ap_uint<DATA_WIDTH> x = item.range(last, first);
	      float f;
	      *(ap_uint<DATA_WIDTH>*)(&f) = (float)x;
	      printf(" %f", f);
	    }
	    printf("\n");
  	  }
        }
        #endif
        //
        ap_uint<DATA_WIDTH * C * KH * KW> frame_out;
        for (int kh = 0; kh < KH; kh++) {
          for (int kw = 0; kw < KW; kw++) {
            ap_uint<DATA_WIDTH * C> item;
  	    item = frame[kh][kw];
	    int first = ((kh * KW) + kw) * C * DATA_WIDTH;
	    int last = first + (C * DATA_WIDTH) - 1;
  	    frame_out.range(last, first) = *(ap_uint<DATA_WIDTH * C>*)(&item);
	  }
        }
        out << frame_out;
      }

    // stride variables update
    if (stride_col == 1) stride_col = SW; else stride_col = stride_col - 1;

    col++;

    // next column and row (with modulo checking)
    col_to_write = col_to_write + 1;
    if (col_to_write == W) {
      col = 0;
      row++;
      col_to_write = 0;
      stride_col = KW;
      if (stride_row == 1) stride_row = SH; else stride_row = stride_row - 1;
      row_to_write = row_to_write + 1;
      if (row_to_write == KH) row_to_write = 0;  
    }
  }
}

// ------------------------------------------------------------------------------------------
// mul_bipolar module
//
// Parameters:
//    C: Number of channels
//    KH, KW: Height and Width of the filter
//    DATA_WIDTH: Data item width in bits
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH * C)
//    out: Output stream (stream objects of size DATA_WIDTH * C)
//
// Returns:
//    float value as the bipolar multiplication
//
// This module performs a multiplication of bipolar filters of size C x KH x KW with input data
// of size C x KH x KW. Because the filters are bipolar the multiplication is implemented as a
// series of accumulations of additions and substractions of the input data
//
// TODO: Remove this function if useless (currently is not being used)
//
template<int C, int KH, int KW, int DATA_WIDTH> float mul_bipolar(ap_uint<DATA_WIDTH * C * KH * KW> data, ap_uint<C * KH * KW * 1> filters) {

  float r = 0.f;    // output of multiplication (accumulator)

  for (int i=0; i<C*KH*KW; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    // Get filter bit and input data bits
    int bit = filters.range(i, i);
    int first = i * DATA_WIDTH;
    int last  = first + DATA_WIDTH - 1;
    ap_uint<DATA_WIDTH> x;
    *(ap_uint<DATA_WIDTH>*)(&x) = data.range(last, first);
    float f_in;
    *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)x;

    // Perform the multiplication (addition or substraction)
    if (bit) r = r + f_in; else r = r - f_in;

    #ifdef DEBUG_MUL_BIPOLAR
    printf("MUL_BIPOLAR: i = %d f_in %f bit filter = %d -> r = %f\n", i, f_in, bit, r);
    #endif
  }
  // Return the result
  return r;
}

// ------------------------------------------------------------------------------------------
// conv_mul_bipolar  module
//
// Parameters:
//    C: Number of channels
//    H: Height of data channel
//    W: Width of data channel
//    COUT: Number of output channels (feature maps)
//    KH, KW: Height and Width of the filter
//    DATA_WIDTH_IN: Data item width in bits (input)
//    DATA_WIDTH_OUT: Data item width in bits (output)
//    II: Initial interval
//    PE: Number of processing elelemtns
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH_IN * C * KH * KW)
//    filters: Vector of filters (COUT elements, each of size C * KH * KW * 1)
//    out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module performs the convolution of an input frame with a filter set. Filters are bipolar
//
template<int C, int H, int W, int COUT, int KH, int KW, int DATA_WIDTH_IN, int DATA_WIDTH_OUT, int II, int PE>
void conv_mul_bipolar(hls::stream<ap_uint<DATA_WIDTH_IN * C * KH * KW>> &in, const ap_uint<C * KH * KW * 1> filters[COUT], hls::stream<ap_uint<DATA_WIDTH_OUT * COUT>> &out) {

  #ifdef DEBUG 
  printf("conv_mul_bipolar %dx%dx%dx%d\n", COUT, C, H, W);
  #endif

  ap_uint<DATA_WIDTH_IN * C * KH * KW> p_in;
  ap_uint<DATA_WIDTH_OUT * COUT> p_out;

  int num_iterations = H * W * KH * KW * C * COUT / PE;
  int num_iterations_per_cout = KH * KW * C;
  int cout_iteration = 0;
  int cout = 0;
  int bit_pos = 0;
  int r[PE];

  for (int pe=0; pe<PE; pe++) {
    DO_PRAGMA(HLS UNROLL)
    r[pe] = 0;
  }

  for (int i = 0; i< num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=II)

    #ifdef DEBUG_CONV_MUL
    printf("conv_mul_bipolar: cout %d c %d kh %d kw %d\n", cout, c, kh, kw);
    #endif

    if ((cout_iteration == 0) && (cout == 0)) {
      p_in = in.read();

      #ifdef DEBUG_CONV_MUL
      printf("MUL: frame received:\n");
      for (int p=0; p < KH * KW * C; p++) {
        ap_uint<DATA_WIDTH_IN> item;
        int first = p * DATA_WIDTH_IN;
        int last = first + DATA_WIDTH_IN - 1;
        ap_uint<DATA_WIDTH_IN> x = p_in.range(last, first);
        float f;
        *(ap_uint<DATA_WIDTH_IN>*)(&f) = (float)x;
        printf(" %f", f);
      }
      printf("\n");
      #endif
    }

    for (int pe=0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)
      int first = DATA_WIDTH_IN * cout_iteration;
      int last = first + DATA_WIDTH_IN - 1;
      ap_uint<DATA_WIDTH_IN> v = p_in.range(last, first);
      float f;
      *(ap_uint<DATA_WIDTH_IN>*)(&f) = (float)v;
      int bit = filters[cout].range(bit_pos, bit_pos);

      if (DATA_WIDTH_IN == 1) {
        if (bit ^ v) r[pe] = r[pe] - 1; else r[pe] = r[pe] + 1;
      } else {
        if (bit) r[pe] = r[pe] + v; else r[pe] = r[pe] - v;
      }
    }

    cout_iteration = cout_iteration + PE;
    bit_pos = bit_pos + 1;
    if (cout_iteration == num_iterations_per_cout) {
      cout_iteration = 0;
      cout = cout + 1;
      if (cout == COUT) {
        cout = 0;
        bit_pos = 0;
        int first_out = 0;
        int last_out = COUT * DATA_WIDTH_OUT - 1;
        int r_total = 0;
        for (int pe = 0; pe < PE; pe++) {
          DO_PRAGMA(HLS UNROLL)
          r_total = r_total + r[pe];
        }
        float f = (float)r_total;
        p_out.range(last_out, first_out) = *(ap_uint<DATA_WIDTH_OUT * COUT>*)(&f);
        out << p_out;
        for (int pe = 0; pe < PE; pe++) {
          DO_PRAGMA(HLS UNROLL)
          r[pe] = 0;
        }
      }
    }
  }
}


// ------------------------------------------------------------------------------------------
//  module conv_bipolar
//
// Parameters:
//    C: Number of channels
//    H: Height of data channel
//    W: Width of data channel
//    COUT: Number of output channels (feature maps)
//    KH, KW: Height and Width of the filter
//    PT, PB, PL, PR: Paddint on top, bottom, left, and right
//    SH, SW: Stride (horizontal and vertical)
//    DATA_WIDTH_IN: Data item width in bits (input)
//    DATA_WIDTH_OUT: Data item width in bits (output)
//    II: Initial interval
//    PE: Number of processing elements
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH_IN * C)
//    filters: Vector of filters (COUT elements each of size C * KH * KW)
//    out: Output stream (stream objects of size DATA_WIDTH_OUT * COUT)
//
// This module performs the convolution, assuming filters are bipolar. The module simply builds
// a dataflow with the following models:
//   - padding
//   - cvt
//   - conv_mul_bipolar
//
template<int C, int H, int W, int COUT, int KH, int KW, int PT, int PB, int PL, int PR, int SH, int SW, int DATA_WIDTH_IN, int DATA_WIDTH_OUT, int II, int PE>
void conv_bipolar(hls::stream<ap_uint<DATA_WIDTH_IN * C>> &in, const ap_uint<C * KH * KW> filters[COUT], hls::stream<ap_uint<DATA_WIDTH_OUT * COUT>> &out) {

  #ifdef DEBUG 
  printf("conv_bipolar %dx%dx%dx%d (PT %d PB %d PL %d PR %d SH %d SW %d KH %d KW %d)\n", COUT, C, H, W, PT, PB, PL, PR, SH, SW, KH, KW);
  #endif 
  
  DO_PRAGMA(HLS DATAFLOW)

  hls::stream<ap_uint<DATA_WIDTH_IN * C>> padding_out;
  padding<C, H, W, PT, PB, PL, PR, DATA_WIDTH_IN>(in, padding_out);

  hls::stream<ap_uint<DATA_WIDTH_IN * C * KH * KW>> cvt_out;
  cvt<C, H+PT+PB, W+PL+PR, KH, KW, SH, SW, DATA_WIDTH_IN>(padding_out, cvt_out);

  conv_mul_bipolar<C, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, COUT, KH, KW, DATA_WIDTH_IN, DATA_WIDTH_OUT, II, PE>(cvt_out, filters, out);

}

// ------------------------------------------------------------------------------------------
//  module conv_bipolar_1x2
//
// Parameters:
//    C: Number of channels
//    H: Height of data channel
//    W: Width of data channel
//    COUT: Number of output channels (feature maps)
//    KH, KW: Height and Width of the filter
//    PT, PB, PL, PR: Paddint on top, bottom, left, and right
//    SH, SW: Stride (horizontal and vertical)
//    DATA_WIDTH_IN: Data item width in bits (input)
//    DATA_WIDTH_OUT: Data item width in bits (output)
//    II: Initial interval
//    PE: Number of processing elements
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH_IN * C)
//    filters1: Vector of filters (First COUT/2 elements each of size C * KH * KW)
//    filters2: Vector of filters (Last COUT/2 elements each of size C * KH * KW)
//    out1: First output stream (stream objects of size DATA_WIDTH_OUT * COUT / 2)
//    out2: Last output stream (stream objects of size DATA_WIDTH_OUT * COUT / 2)
//
// This module performs the convolution, assuming filters are bipolar. The module simply builds
// a dataflow with the following models:
//   - padding
//   - cvt
//   - broadcast
//   - conv_mul_bipolar
//   - conv_mul_bipolar
//
// In this module the output is split in two streams (to avoid maximum stream limit set to 4096)
//
template<int C, int H, int W, int COUT, int KH, int KW, int PT, int PB, int PL, int PR, int SH, int SW, int DATA_WIDTH_IN, int DATA_WIDTH_OUT, int II, int PE>
void conv_bipolar_1x2(hls::stream<ap_uint<DATA_WIDTH_IN * C>> &in, const ap_uint<C * KH * KW> filters1[COUT/2], const ap_uint<C * KH * KW> filters2[COUT/2], hls::stream<ap_uint<DATA_WIDTH_OUT * COUT / 2>> &out1, hls::stream<ap_uint<DATA_WIDTH_OUT * COUT / 2>> &out2) {

  #ifdef DEBUG 
  printf("conv_bipolar_1x2 %dx%dx%dx%d (PT %d PB %d PL %d PR %d SH %d SW %d KH %d KW %d)\n", COUT, C, H, W, PT, PB, PL, PR, SH, SW, KH, KW);
  #endif 
  
  DO_PRAGMA(HLS DATAFLOW)

  hls::stream<ap_uint<DATA_WIDTH_IN * C>> padding_out;
  padding<C, H, W, PT, PB, PL, PR, DATA_WIDTH_IN>(in, padding_out);

  hls::stream<ap_uint<DATA_WIDTH_IN * C * KH * KW>> cvt_out;
  cvt<C, H+PT+PB, W+PL+PR, KH, KW, SH, SW, DATA_WIDTH_IN>(padding_out, cvt_out);

  hls::stream<ap_uint<DATA_WIDTH_IN * C * KH * KW>> bcast_out1;
  hls::stream<ap_uint<DATA_WIDTH_IN * C * KH * KW>> bcast_out2;
  broadcast<((H + PT + PB - KH + SH) / SH) *  ((W + PL + PR - KW + SW) / SW), C * KW * KH * DATA_WIDTH_IN>(cvt_out, bcast_out1, bcast_out2);

  conv_mul_bipolar<C, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, COUT / 2, KH, KW, DATA_WIDTH_IN, DATA_WIDTH_OUT, II, PE>(bcast_out1, filters1, out1);
  conv_mul_bipolar<C, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, COUT / 2, KH, KW, DATA_WIDTH_IN, DATA_WIDTH_OUT, II, PE>(bcast_out2, filters2, out2);

}

// ------------------------------------------------------------------------------------------
//  module conv_bipolar_2x2
//
// Parameters:
//    C: Number of channels
//    H: Height of data channel
//    W: Width of data channel
//    COUT: Number of output channels (feature maps)
//    KH, KW: Height and Width of the filter
//    PT, PB, PL, PR: Paddint on top, bottom, left, and right
//    SH, SW: Stride (horizontal and vertical)
//    DATA_WIDTH_IN: Data item width in bits (input)
//    DATA_WIDTH_OUT: Data item width in bits (output)
//    II: Initial interval
//    PE: Number of processing elements
//
// Arguments:
//    in1: First input stream (stream objects of size DATA_WIDTH_IN * C / 2)
//    in2: Last input stream (stream objects of size DATA_WIDTH_IN * C / 2)
//    filters1: Vector of filters (First COUT/2 elements each of size C/2 * KH * KW)
//    filters2: Vector of filters (First COUT/2 elements each of size C/2 * KH * KW)
//    filters3: Vector of filters (Last COUT/2 elements each of size C/2 * KH * KW)
//    filters4: Vector of filters (Last COUT/2 elements each of size C/2 * KH * KW)
//    out1: First output stream (stream objects of size DATA_WIDTH_OUT * COUT / 2)
//    out2: Last output stream (stream objects of size DATA_WIDTH_OUT * COUT / 2)
//
// This module performs the convolution, assuming filters are bipolar. The module simply builds
// a dataflow with the following models:
//   - padding
//   - cvt
//   - padding
//   - cvt
//   - broadcast
//   - conv_mul_bipolar
//   - conv_mul_bipolar
//   - reduce
//   - reduce
//
// In this module the inptu is provided as two separate streams and the output is split in two streams (to avoid maximum stream limit set to 4096)
//
template<int C, int H, int W, int COUT, int KH, int KW, int PT, int PB, int PL, int PR, int SH, int SW, int DATA_WIDTH_IN, int DATA_WIDTH_OUT, int II, int PE>
void conv_bipolar_2x2(hls::stream<ap_uint<DATA_WIDTH_IN * C / 2>> &in1, hls::stream<ap_uint<DATA_WIDTH_IN * C / 2>> &in2,   const ap_uint<C * KH * KW / 2> filters_aa[COUT/2], const ap_uint<C * KH * KW / 2> filters_ab[COUT/2], const ap_uint<C * KH * KW / 2> filters_ba[COUT/2], const ap_uint<C * KH * KW / 2> filters_bb[COUT/2], hls::stream<ap_uint<DATA_WIDTH_OUT * COUT / 2>> &out1, hls::stream<ap_uint<DATA_WIDTH_OUT * COUT / 2>> &out2) {

  #ifdef DEBUG 
  printf("conv_bipolar_2x2 %dx%dx%dx%d (PT %d PB %d PL %d PR %d SH %d SW %d KH %d KW %d)\n", COUT, C, H, W, PT, PB, PL, PR, SH, SW, KH, KW);
  #endif 
  
  DO_PRAGMA(HLS DATAFLOW)

  hls::stream<ap_uint<DATA_WIDTH_IN * C / 2>> padding_out1;
  hls::stream<ap_uint<DATA_WIDTH_IN * C / 2>> padding_out2;
  padding<C / 2, H, W, PT, PB, PL, PR, DATA_WIDTH_IN>(in1, padding_out1);
  padding<C / 2, H, W, PT, PB, PL, PR, DATA_WIDTH_IN>(in2, padding_out2);

  hls::stream<ap_uint<DATA_WIDTH_IN * C * KH * KW / 2>> cvt_out1;
  hls::stream<ap_uint<DATA_WIDTH_IN * C * KH * KW / 2>> cvt_out2;
  cvt<C / 2, H+PT+PB, W+PL+PR, KH, KW, SH, SW, DATA_WIDTH_IN>(padding_out1, cvt_out1);
  cvt<C / 2, H+PT+PB, W+PL+PR, KH, KW, SH, SW, DATA_WIDTH_IN>(padding_out2, cvt_out2);

  hls::stream<ap_uint<DATA_WIDTH_IN * C / 2 * KH * KW>> bcast_out1a;
  hls::stream<ap_uint<DATA_WIDTH_IN * C / 2 * KH * KW>> bcast_out1b;
  hls::stream<ap_uint<DATA_WIDTH_IN * C / 2 * KH * KW>> bcast_out2a;
  hls::stream<ap_uint<DATA_WIDTH_IN * C / 2 * KH * KW>> bcast_out2b;
  broadcast<((H + PT + PB - KH + SH) / SH) *  ((W + PL + PR - KW + SW) / SW), DATA_WIDTH_IN * KW * KH * C / 2>(cvt_out1, bcast_out1a, bcast_out1b);
  broadcast<((H + PT + PB - KH + SH) / SH) *  ((W + PL + PR - KW + SW) / SW), DATA_WIDTH_IN * KW * KH * C / 2>(cvt_out2, bcast_out2a, bcast_out2b);

  hls::stream<ap_uint<DATA_WIDTH_OUT * COUT / 2>> out1a;
  hls::stream<ap_uint<DATA_WIDTH_OUT * COUT / 2>> out1b;
  hls::stream<ap_uint<DATA_WIDTH_OUT * COUT / 2>> out2a;
  hls::stream<ap_uint<DATA_WIDTH_OUT * COUT / 2>> out2b;
  conv_mul_bipolar<C / 2, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, COUT / 2, KH, KW, DATA_WIDTH_IN, DATA_WIDTH_OUT, II, PE>(bcast_out1a, filters_aa, out1a);
  conv_mul_bipolar<C / 2, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, COUT / 2, KH, KW, DATA_WIDTH_IN, DATA_WIDTH_OUT, II, PE>(bcast_out1b, filters_ab, out1b);
  conv_mul_bipolar<C / 2, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, COUT / 2, KH, KW, DATA_WIDTH_IN, DATA_WIDTH_OUT, II, PE>(bcast_out2a, filters_ba, out2a);
  conv_mul_bipolar<C / 2, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, COUT / 2, KH, KW, DATA_WIDTH_IN, DATA_WIDTH_OUT, II, PE>(bcast_out2b, filters_bb, out2b);

  reduce<C / 2, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, DATA_WIDTH_OUT, PE>(out1a, out1b, out1);
  reduce<C / 2, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, DATA_WIDTH_OUT, PE>(out2a, out2b, out2);

}


// ------------------------------------------------------------------------------------------
// pool_maxpool module
//
// Parameters:
//    C: Number of channels
//    H: Height of data channel
//    W: Width of data channel
//    KH, KW: Height and Width of the filter
//    DATA_WIDTH: Data item width in bits
//    PE: Number of processing elements
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH * C * KH * KW)
//    out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module performs the maxpooling operation on the input frames, and produces an output item
//
template<int C, int H, int W, int KH, int KW, int DATA_WIDTH, int PE> void pool_maxpool(hls::stream<ap_uint<DATA_WIDTH * C * KH * KW>> &in, hls::stream<ap_uint<DATA_WIDTH * C >> &out) {

  #ifdef DEBUG 
  printf("pool_maxpool %dx%dx%d\n", C, H, W);
  #endif 

  ap_uint<DATA_WIDTH * C * KH * KW> p_in;
  ap_uint<DATA_WIDTH * C> p_out;

  int num_channel_blocks = C / PE;                      // Number of block channels per input item
  int num_iterations = H * W * num_channel_blocks;      // Number of total iterations
  int block = 0;                                        // Current block channel

  // We sweep all iterations
  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    // We potentialy read from the input
    if (block == 0) p_in = in.read();

    // We perform PE operations in parallel
    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      // Basic operation: maxpooling
      int c_pe = block + pe;
      float f_max = -99999999.f;
      for (int kh = 0; kh < KH; kh++) {
	for (int kw = 0; kw < KW; kw++) {
          int first = (c_pe * KH * KW * DATA_WIDTH) + (kh * KW * DATA_WIDTH) + (kw * DATA_WIDTH);
          int last = first + DATA_WIDTH - 1;
          ap_uint<DATA_WIDTH> x = p_in.range(last, first);
          float f;
          *(ap_uint<DATA_WIDTH>*)(&f) = (float)x;
	  if (f > f_max) f_max = f;
	}
      }
      // We write the result on the output
      int first_out = c_pe * DATA_WIDTH;
      int last_out = first_out + DATA_WIDTH - 1;
      p_out.range(last_out, first_out) = *(ap_uint<DATA_WIDTH>*)(&f_max);
    }
    // Next block channel
    block = block + PE;
    // We check end of channel
    if (block == C) {
      block = 0;
      // We write to the output stream
      out << p_out;
    }
  }
}

// ------------------------------------------------------------------------------------------
//  module maxpool
//
// Parameters:
//    C: Number of channels
//    H: Height of data channel
//    W: Width of data channel
//    KH, KW: Height and Width of the filter
//    SH, SW: Stride (horizontal and vertical)
//    DATA_WIDTH: Data item width in bits
//    PE: Number of processing elements
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH * C)
//    out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module performs the maxpooling operation. The module simply builds
// a dataflow with the following models:
//   - cvt
//   - pool_maxpool
//
template<int C, int H, int W, int KH, int KW, int SH, int SW, int DATA_WIDTH, int PE> void maxpool(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {

  #ifdef DEBUG
  printf("maxpool %dx%dx%d\n", C, H, W);
  #endif

  DO_PRAGMA(HLS DATAFLOW)

  hls::stream<ap_uint<DATA_WIDTH * C * KH * KW>> cvt_out;
  cvt<C, H, W, KH, KW, SH, SW, DATA_WIDTH>(in, cvt_out);
  pool_maxpool<C, (H - KH + SH) / SH, (W - KW + SW) / SW, KH, KW, DATA_WIDTH, PE>(cvt_out, out);

}

// -------------------------------------------------------------------------------------------------------
// batch_normalization module
//
// Parameters:
//   C: Number of data channels
//   H: Height of data channel
//   W: Width of data channel
//   DATA_WIDTH: Data item width in bits
//   PE: Number of processing elements to use
//
// Arguments:
//   in: Input stream (stream objects of size DATA_WIDTH * C)
//   scale: Scale vector of C floats
//   B: B vector of C floats
//   mean: mean vector of C floats
//   var: var vector of C floats
//   out: Output stream (stream objects of size DATA_WIDTH * C)
//
// This module performs the batch normalization operation of the input data. Input channels are independent and
// can be operated in parallel
//
template<int C, int H, int W, int DATA_WIDTH, int PE> 
void batch_normalization(hls::stream<ap_uint<DATA_WIDTH * C>> &in, const float scale[C], const float B[C], const float mean[C], const float var[C], hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
  #ifdef DEBUG 
  printf("batch_normalization %dx%dx%d\n", C, H, W);
  #endif 

  int num_channel_blocks = C / PE;                      // Number of block channels per input item
  int num_iterations = H * W * num_channel_blocks;      // Number of total iterations
  int block = 0;                                        // Current block channel

  // We sweep all iterations
  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    // We potentially read from the input
    if (block == 0) px = in.read();

    // We perform PE operations in parallel
    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      // We get the input data
      int c_pe = block+pe;
      int first = c_pe * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;

      // We perform the basic block operation: Normalization
      float f_out;
      f_out = (((f_in - mean[c_pe]) / var[c_pe]) * scale[c_pe]) + B[c_pe];

      #ifdef DEBUG_BATCH_NORMALIZATION
      printf("BATCH_NORMALIZATION: i %d f_in %f f_out %f\n", i, f_in, f_out);
      #endif

      // We write to the output
      //p_out.range(last, first) = (ap_uint<DATA_WIDTH>)f_out;
      //p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_out);
      p_out.range(DATA_WIDTH-1, 0) = (ap_uint<DATA_WIDTH>)f_out;
      p_out = p_out << DATA_WIDTH;
    }

    // Next block channel
    block = block + PE;
    // We check end of channel
    if (block == C) {
      block = 0;
      // We write the output
      out << p_out;
    }
  }
}

// ------------------------------------------------------------------------------------------
//  module reduction
//
// Parameters:
//    C: Number of channels
//    H: Height of data channel
//    W: Width of data channel
//    COUT: Number of output channels
//    DATA_WIDTH: Data item width in bits
//
// Arguments:
//    in: Input stream (stream objects of size DATA_WIDTH * C)
//    out: Output stream (stream objects of size DATA_WIDTH * COUT)
//
// This module performs a reduction of channels. Is used to accomodate a particular output.
// The C input channels are read, then the COUT first channels are forwarded to the output
// and the remaining input channels are removed.
//
// TODO: Remove this module when not longer used
//
template<int C, int H, int W, int COUT, int DATA_WIDTH> void reduction(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<DATA_WIDTH * COUT>> &out) {
#ifdef DEBUG 
  printf("reduction %dx%dx%dx%d\n", COUT, C, H, W);
#endif 
  ap_uint<DATA_WIDTH * C> p_in;
  ap_uint<DATA_WIDTH * COUT> p_out;
  for (int i = 0; i < H * W; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    p_in = in.read();
    int first = 0;
    int last = (COUT * DATA_WIDTH) - 1;
    p_out = p_in.range(last, first);
    out << p_out;
  }
}

// ------------------------------------------------------------------------------------------
// module k_nel 
//
// Arguments:
//    ptr_in: memory pointer to input data
//    ptr_out: memory pointer to output data
//
// This module implements a complete model by building a dataflow model
//
void k_net(float *ptr_in, float *ptr_out) { 

  DO_PRAGMA(HLS INTERFACE m_axi port=ptr_in        offset=slave bundle=gmem)
  DO_PRAGMA(HLS INTERFACE m_axi port=ptr_out       offset=slave bundle=gmem3)

  DO_PRAGMA(HLS DATAFLOW)

  const int DATA_WIDTH = 32;

  // Read 3x32x32 (float)
  hls::stream<ap_uint<DATA_WIDTH * 3>> st0;
  read<3, 32, 32, DATA_WIDTH>((ap_uint<DATA_WIDTH * 3> *)ptr_in, st0);

  // Div 3x32x32 (float)
  hls::stream<ap_uint<DATA_WIDTH * 3>> st1;
  div<3, 32, 32, DATA_WIDTH, 3>(st0, 255.f, st1);

  // Mul 3x32x32 (float)
  hls::stream<ap_uint<DATA_WIDTH * 3>> st2;
  mul<3, 32, 32, DATA_WIDTH, 3>(st1, 2.f, st2);

  // Sub 3x32x32 (float)
  hls::stream<ap_uint<DATA_WIDTH * 3>> st3;
  sub<3, 32, 32, DATA_WIDTH, 3>(st2, 1.f, st3);

  // quantization (3x32x32)
  hls::stream<ap_uint<DATA_WIDTH * 3>> st4;
  quantize<3, 32, 32, DATA_WIDTH, 3>(st3, -0.99609375, 0.98828125, 0.0078125, st4);

  // conv (64x3x3x3)
  // Target = 100us: II=4, PE=8
  hls::stream<ap_uint<DATA_WIDTH * 64>> st7;
  conv_bipolar<3, 32, 32, 64, 3, 3, 0, 0, 0, 0, 1, 1, DATA_WIDTH, DATA_WIDTH, 1, 9>(st4, bfilter0, st7);

  // batch normalization (64x30x30)
  hls::stream<ap_uint<DATA_WIDTH * 64>> st8;
  batch_normalization<64, 30, 30, DATA_WIDTH, 1>(st7, scale0, B0, mean0, var0, st8);

  // bipolar
  hls::stream<ap_uint<1 * 64>> st9;
  bipolar<64, 30, 30, DATA_WIDTH, 1>(st8, st9);

  // conv (64x64x3x3)
  // Target = 100us: II=4, PE=8
  hls::stream<ap_uint<DATA_WIDTH * 64>> st10;
  conv_bipolar<64, 30, 30, 64, 3, 3, 0, 0, 0, 0, 1, 1, 1, DATA_WIDTH, 1, 64>(st9, bfilter1, st10);

  // batch normalization (64x28x28)
  hls::stream<ap_uint<DATA_WIDTH * 64>> st11;
  batch_normalization<64, 28, 28, DATA_WIDTH, 1>(st10, scale1, B1, mean1, var1, st11);

  // bipolar
  hls::stream<ap_uint<1 * 64>> st12;
  bipolar<64, 28, 28, DATA_WIDTH, 1>(st11, st12);

  // maxpool
  hls::stream<ap_uint<1 * 64>> st13;
  maxpool<64, 28, 28, 2, 2, 2, 2, 1, 1>(st12, st13);

  // conv (128x64x3x3)
  // Target = 100us: II=4, PE=16
  hls::stream<ap_uint<DATA_WIDTH * 128>> st14;
  conv_bipolar<64, 14, 14, 128, 3, 3, 0, 0, 0, 0, 1, 1, 1, DATA_WIDTH, 1, 32>(st13, bfilter2, st14);

  // batch normalization
  hls::stream<ap_uint<DATA_WIDTH * 128>> st15;
  batch_normalization<128, 12, 12, DATA_WIDTH, 1>(st14, scale2, B2, mean2, var2, st15);

  // bipolar
  hls::stream<ap_uint<1 * 128>> st16;
  bipolar<128, 12, 12, DATA_WIDTH, 1>(st15, st16);

  // conv (128x128x3x3)
  // Target = 100us: II=4, PE=16
  hls::stream<ap_uint<DATA_WIDTH * 128>> st17;
  conv_bipolar<128, 12, 12, 128, 3, 3, 0, 0, 0, 0, 1, 1, 1, DATA_WIDTH, 1, 64>(st16, bfilter3, st17);

  // batch normalization
  hls::stream<ap_uint<DATA_WIDTH * 128>> st18;
  batch_normalization<128, 10, 10, DATA_WIDTH, 1>(st17, scale3, B3, mean3, var3, st18);

  // bipolar
  hls::stream<ap_uint<1 * 128>> st19;
  bipolar<128, 10, 10, DATA_WIDTH, 1>(st18, st19);

  // maxpool
  hls::stream<ap_uint<1 * 128>> st20;
  maxpool<128, 10, 10, 2, 2, 2, 2, 1, 1>(st19, st20);

  // conv (256x128x3x3)
  hls::stream<ap_uint<DATA_WIDTH * 128>> st21a;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st21b;
  conv_bipolar_1x2<128, 5, 5, 256, 3, 3, 0, 0, 0, 0, 1, 1, 1, DATA_WIDTH, 1, 4>(st20, bfilter4a, bfilter4b, st21a, st21b);

  // batch normalization
  hls::stream<ap_uint<DATA_WIDTH * 128>> st22a;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st22b;
  batch_normalization<128, 3, 3, DATA_WIDTH, 1>(st21a, scale4a, B4a, mean4a, var4a, st22a);
  batch_normalization<128, 3, 3, DATA_WIDTH, 1>(st21b, scale4b, B4b, mean4b, var4b, st22b);

  // bipolar
  hls::stream<ap_uint<1 * 128>> st23a;
  hls::stream<ap_uint<1 * 128>> st23b;
  bipolar<128, 3, 3, DATA_WIDTH, 1>(st22a, st23a);
  bipolar<128, 3, 3, DATA_WIDTH, 1>(st22b, st23b);

  // conv (256x256x3x3)
  hls::stream<ap_uint<DATA_WIDTH * 128>> st24a;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st24b;
  conv_bipolar_2x2<256, 3, 3, 256, 3, 3, 0, 0, 0, 0, 1, 1, 1, DATA_WIDTH, 1, 1>(st23a, st23b, bfilter5aa, bfilter5ab, bfilter5ba, bfilter5bb, st24a, st24b);

  // batch normalization
  hls::stream<ap_uint<DATA_WIDTH * 128>> st25a;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st25b;
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st24a, scale5a, B5a, mean5a, var5a, st25a);
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st24b, scale5b, B5b, mean5b, var5b, st25b);

  // bipolar
  hls::stream<ap_uint<1 * 128>>st26a;
  hls::stream<ap_uint<1 * 128>>st26b;
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st25a, st26a);
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st25b, st26b);

  // reshape 256x1x1 -> 256
  //

  // matmul (256x512)
  hls::stream<ap_uint<DATA_WIDTH * 128>> st27a;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st27b;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st27c;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st27d;
  matmul_bipolar_2x4<256, 512, DATA_WIDTH, 1>(st26a, st26b, weights0_1a, weights0_1b, weights0_1c, weights0_1d, weights0_2a, weights0_2b, weights0_2c, weights0_2d, st27a, st27b, st27c, st27d);

  // unsqueeze 512 -> 512x1
  //

  // batch normalization
  hls::stream<ap_uint<DATA_WIDTH * 128>> st28a;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st28b;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st28c;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st28d;
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st27a, scale6a, B6a, mean6a, var6a, st28a);
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st27b, scale6b, B6b, mean6b, var6b, st28b);
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st27c, scale6c, B6c, mean6c, var6c, st28c);
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st27d, scale6d, B6d, mean6d, var6d, st28d);

  // squeeze 512x1 -> 512
  //

  // bipolar
  hls::stream<ap_uint<1 * 128>> st29a;
  hls::stream<ap_uint<1 * 128>> st29b;
  hls::stream<ap_uint<1 * 128>> st29c;
  hls::stream<ap_uint<1 * 128>> st29d;
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st28a, st29a);
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st28b, st29b);
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st28c, st29c);
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st28d, st29d);

  // matmul (512x512)
  hls::stream<ap_uint<DATA_WIDTH * 128>> st30a;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st30b;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st30c;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st30d;
  matmul_bipolar_4x4<512, 512, DATA_WIDTH, 1>(st29a, st29b, st29c, st29d, weights1_1a, weights1_1b, weights1_1c, weights1_1d, weights1_2a, weights1_2b, weights1_2c, weights1_2d, weights1_3a, weights1_3b, weights1_3c, weights1_3d, weights1_4a, weights1_4b, weights1_4c, weights1_4d, st30a, st30b, st30c, st30d);

  // unsqueeze 512 -> 512x1
  //

  // batch normalization
  hls::stream<ap_uint<DATA_WIDTH * 128>> st31a;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st31b;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st31c;
  hls::stream<ap_uint<DATA_WIDTH * 128>> st31d;
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st30a, scale7a, B7a, mean7a, var7a, st31a);
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st30b, scale7b, B7b, mean7b, var7b, st31b);
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st30c, scale7c, B7c, mean7c, var7c, st31c);
  batch_normalization<128, 1, 1, DATA_WIDTH, 1>(st30d, scale7d, B7d, mean7d, var7d, st31d);

  // squeeze 512x1 -> 512
  //

  // bipolar
  hls::stream<ap_uint<1 * 128>> st32a;
  hls::stream<ap_uint<1 * 128>> st32b;
  hls::stream<ap_uint<1 * 128>> st32c;
  hls::stream<ap_uint<1 * 128>> st32d;
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st31a, st32a);
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st31b, st32b);
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st31c, st32c);
  bipolar<128, 1, 1, DATA_WIDTH, 1>(st31d, st32d);

  // matmul (512x10)
  hls::stream<ap_uint<DATA_WIDTH * 10>> st33;
  matmul_bipolar_4x1<512, 10, DATA_WIDTH, 1>(st32a, st32b, st32c, st32d, weights2_1a, weights2_2a, weights2_3a, weights2_4a, st33);

  // sub 10x1
  hls::stream<ap_uint<DATA_WIDTH * 10>> st34;
  sub<10, 1, 1, DATA_WIDTH, 1>(st33, 16.55174446105957, st34);

  // div 10x1
  hls::stream<ap_uint<DATA_WIDTH * 10>> st35;
  div<10, 1, 1, DATA_WIDTH, 1>(st34, 122.42723846435547, st35);

  // mul 10x1
  hls::stream<ap_uint<DATA_WIDTH * 10>> st36;
  mul<10, 1, 1, DATA_WIDTH, 1>(st35, 0.5850340723991394, st36);

  // add 10x1
  hls::stream<ap_uint<DATA_WIDTH * 10>> st37;
  add<10, 1, 1, DATA_WIDTH, 1>(st36, -0.9069851636886597, st37);

  // topK
  //

  // 
  // Write (10x3x3)
  write<10, 1, 1, DATA_WIDTH>(st37, (ap_uint<DATA_WIDTH * 10> *)ptr_out);


}
