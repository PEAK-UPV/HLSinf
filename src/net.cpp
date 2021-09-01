#define AP_INT_MAX_W 32768
#include "ap_int.h"

#include "net.h"

#include <hls_stream.h>

#include "weights.h"

#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

//#define DEBUG
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

template<int N, int M, int DATA_WIDTH, int PE>
void matmul_bipolar(hls::stream<ap_uint<1 * N>> &in, ap_uint<1 * N> weights[M], hls::stream<ap_uint<DATA_WIDTH * M>> &out) {

#ifdef DEBUG
  printf("matmul_bipolar %d x %dx%d\n", N, N, M);
#endif

  ap_uint<1 * N> p_in;
  ap_uint<DATA_WIDTH * M> p_out;
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
      int first = m * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&v);
      m = m + 1;
      r = 0;
    }
  }
  out << p_out;
}

template<int C, int H, int W, int DATA_WIDTH>
void read(ap_uint<DATA_WIDTH * C> *ptr, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
#ifdef DEBUG
  printf("read %dx%dx%d\n", C, H, W);
#endif
  for (int i = 0; i < H * W; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<DATA_WIDTH * C> px = ptr[i];
    out << px;
  } 
}

template<int C, int H, int W, int DATA_WIDTH, int PE>
void div(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float F, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
#ifdef DEBUG 
  printf("div %dx%dx%d\n", C, H, W);
#endif 
  int block_channels = C / PE;
  int num_iterations = H * W * block_channels;
  int block = 0;

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    if (block == 0) px = in.read();

    for (int pe=0; pe<PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      int first = (block + pe) * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;
#ifdef DEBUG_DIV
      printf(" DIV (%f): i = %d, channel = %d, f_in = %f\n", F, i, c, f_in);
#endif
      f_in = f_in / F;
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_in);
    }
    block = block + PE;
    if (block == C) {
      block = 0;
      out << p_out;
    }
  }
}

template<int C, int H, int W, int DATA_WIDTH, int PE>
void mul(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float F, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
#ifdef DEBUG 
  printf("mul %dx%dx%d\n", C, H, W);
#endif 

  int block_channels = C / PE;
  int num_iterations = H * W * block_channels;
  int block = 0;

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    if (block == 0) px = in.read();

    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      int first = (block + pe) * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;
#ifdef DEBUG_MUL
      printf(" MUL (%f): i = %d, channel = %d, f_in = %f\n", F, i, c, f_in);
#endif
      f_in = f_in * F;
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_in);
    }
    block = block + PE;
    if (block == C) {
      block = 0;
      out << p_out;
    }
  }
}

template<int C, int H, int W, int DATA_WIDTH, int PE>
void sub(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float F, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
#ifdef DEBUG 
  printf("sub %dx%dx%d\n", C, H, W);
#endif 

  int block_channels = C / PE;
  int num_iterations = H * W * block_channels;
  int block = 0;

  for (int i=0; i< num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;
 
    if (block == 0) px = in.read();

    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      int first = (block + pe) * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;
#ifdef DEBUG_SUB
      printf(" SUB (%f): i = %d, channel = %d, f_in = %f -> %f\n", F, i, c, f_in, f_in - F);
#endif
      f_in = f_in - F;
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_in);
    }
    block = block + PE;
    if (block == C) {
      block = 0;
      out << p_out;
    }
  }
}

template<int C, int H, int W, int DATA_WIDTH, int PE>
void quantize(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float min, float max, float stride, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
#ifdef DEBUG 
  printf("quantize %dx%dx%d\n", C, H, W);
#endif 

  int block_channels = C / PE;
  int num_iterations = H * W * block_channels;
  int block = 0;
  
  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    if (block == 0) px = in.read();

    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)
      int first = (block + pe) * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;
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
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_out);
    }
    block = block + PE;
    if (block == C) {
      block = 0;
      out << p_out;
    }
  }
}

template<int C, int H, int W, int DATA_WIDTH, int PE>
void bipolar(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<1 * C>> &out) {
#ifdef DEBUG 
  printf("bipolar %dx%dx%d\n", C, H, W);
#endif 

  int block_channels = C / PE;
  int num_iterations = H * W * block_channels;
  int block = 0;

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<DATA_WIDTH * C> px;
    ap_uint<1 * C> p_out;

    if (block == 0) px = in.read();

    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      int c_pe = block + pe;
      int first = c_pe * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;
      ap_uint<1> v_out;
      if (f_in < 0) v_out = 0;
      else v_out = 1;
#ifdef DEBUG_BIPOLAR
      printf("BIPOLAR: in %f out %d\n", f_in, (int)v_out);
#endif
      p_out.range(c_pe, c_pe) = v_out;
    }

    block = block + PE;
    if (block == C) {
      block = 0;
      out << p_out;
    }
  }
}


template<int C, int H, int W, int DATA_WIDTH, int PE>
void add(hls::stream<ap_uint<DATA_WIDTH * C>> &in, float f, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
#ifdef DEBUG 
  printf("add %dx%dx%d\n", C, H, W);
#endif 

  int block_channels = C / PE;
  int num_iterations = H * W * block_channels;
  int block = 0;

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    if (block == 0) px = in.read();

    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      int c_pe = block + pe;
      int first = c_pe * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;
      //
      float f_out;
      f_out = f_in + f;
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_out);
    }

    block = block + PE;
    if (block == C) {
      block = 0;
      out << p_out;
    }
  }
}


template<int C, int H, int W, int DATA_WIDTH>
void write(hls::stream<ap_uint<DATA_WIDTH * C>> &in, ap_uint<DATA_WIDTH * C> *ptr) {
#ifdef DEBUG 
  printf("write %dx%dx%d\n", C, H, W);
#endif 
  for (int i = 0; i < H * W; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    ap_uint<DATA_WIDTH * C> px = in.read();
    ptr[i] = px;
  }
}

template<int C, int H, int W,
	 int PT, int PB, int PL, int PR,
	 int DATA_WIDTH>
void padding(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
#ifdef DEBUG 
  printf("padding %dx%dx%d\n", C, H, W);
#endif 
  ap_uint<DATA_WIDTH * C> all_zeros;
  all_zeros = 0;

  for (int h = -PT; h < H + PB; h++) {
    for (int w = -PL; w < W + PR; w++) {
      ap_uint<DATA_WIDTH * C> p_in;
      if ((h >= 0) && (h < H) && (w >= 0) && (w < W)) p_in = in.read(); else p_in = all_zeros;
      //
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
      out << p_in;
    }
  }
}

template<int C, int H, int W,
	 int KH, int KW,
	 int SH, int SW,
	 int DATA_WIDTH>
void cvt(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<DATA_WIDTH * KH * KW * C>> &out) {
#ifdef DEBUG 
  printf("cvt %dx%dx%d\n", C, H, W);
#endif 
  // row buffer
  ap_uint<DATA_WIDTH * C> buff[KH][W];

  ap_uint<DATA_WIDTH * C> p_in;

  ap_uint<DATA_WIDTH * C> frame[KH][KW];

  int row_to_write = 0;
  int col_to_write = 0;
  int stride_row = KH;
  int stride_col = KW;
  int row = 0;
  int col = 0;

  for (int p = 0; p < H * W; p++) {

#ifdef DEBUG_CVT
    printf("CVT: p = %d, row_to_write %d, col_to_write %d, stride_row %d stride_col %d\n", p, row_to_write, col_to_write, stride_row, stride_col);
#endif
    // read input
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

template<int C, int KH, int KW, int DATA_WIDTH>
float mul_bipolar(ap_uint<DATA_WIDTH * C * KH * KW> data, ap_uint<C * KH * KW * 1> filters) {

  float r = 0.f;
  for (int i=0; i<C*KH*KW; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    int bit = filters.range(i, i);
    int first = i * DATA_WIDTH;
    int last  = first + DATA_WIDTH - 1;
    ap_uint<DATA_WIDTH> x;
    *(ap_uint<DATA_WIDTH>*)(&x) = data.range(last, first);
    float f_in;
    *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)x;
    if (bit) r = r + f_in; else r = r - f_in;
#ifdef DEBUG_MUL_BIPOLAR
    printf("MUL_BIPOLAR: i = %d f_in %f bit filter = %d -> r = %f\n", i, f_in, bit, r);
#endif
  }
  return r;
}

template<int C, int H, int W,
         int COUT,
         int KH, int KW,
         int DATA_WIDTH_IN,
	 int DATA_WIDTH_OUT,
	 int II,
	 int PE>
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

template<int C, int H, int W,
         int COUT,
         int KH, int KW,
         int PT, int PB, int PL, int PR,
         int SH, int SW,
	 int DATA_WIDTH_IN,
         int DATA_WIDTH_OUT,
	 int II,
	 int PE>
void conv_bipolar(hls::stream<ap_uint<DATA_WIDTH_IN * C>> &in, const ap_uint<C * KH * KW> filters[COUT], hls::stream<ap_uint<DATA_WIDTH_OUT * COUT>> &out) {
#ifdef DEBUG 
  printf("conv_bipolar %dx%dx%dx%d\n", COUT, C, H, W);
#endif 
  DO_PRAGMA(HLS DATAFLOW)

  hls::stream<ap_uint<DATA_WIDTH_IN * C>> padding_out;
  padding<C, H, W, PT, PB, PL, PR, DATA_WIDTH_IN>(in, padding_out);

  hls::stream<ap_uint<DATA_WIDTH_IN * C * KH * KW>> cvt_out;
  cvt<C, H+PT+PB, W+PL+PR, KH, KW, SH, SW, DATA_WIDTH_IN>(padding_out, cvt_out);

  conv_mul_bipolar<C, (H + PT + PB - KH + SH) / SH, (W + PL + PR - KW + SW) / SW, COUT, KH, KW, DATA_WIDTH_IN, DATA_WIDTH_OUT, II, PE>(cvt_out, filters, out);

}

template<int C, int H, int W, int KH, int KW, int DATA_WIDTH, int PE>
void pool_maxpool(hls::stream<ap_uint<DATA_WIDTH * C * KH * KW>> &in, hls::stream<ap_uint<DATA_WIDTH * C >> &out) {
#ifdef DEBUG 
  printf("pool_maxpool %dx%dx%d\n", C, H, W);
#endif 
  ap_uint<DATA_WIDTH * C * KH * KW> p_in;
  ap_uint<DATA_WIDTH * C> p_out;

  int num_channel_blocks = C / PE;
  int num_iterations = H * W * num_channel_blocks;
  int block = 0;

  for (int i = 0; i < num_iterations; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    if (block == 0) p_in = in.read();

    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

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
      int first_out = c_pe * DATA_WIDTH;
      int last_out = first_out + DATA_WIDTH - 1;
      p_out.range(last_out, first_out) = *(ap_uint<DATA_WIDTH>*)(&f_max);
    }
    block = block + PE;
    if (block == C) {
      block = 0;
      out << p_out;
    }
  }
}

template<int C, int H, int W, int KH, int KW, int SH, int SW, int DATA_WIDTH, int PE>
void maxpool(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<DATA_WIDTH * C>> &out) {

#ifdef DEBUG
  printf("maxpool %dx%dx%d\n", C, H, W);
#endif
  DO_PRAGMA(HLS DATAFLOW)

  hls::stream<ap_uint<DATA_WIDTH * C * KH * KW>> cvt_out;
  cvt<C, H, W, KH, KW, SH, SW, DATA_WIDTH>(in, cvt_out);
  pool_maxpool<C, (H - KH + SH) / SH, (W - KW + SW) / SW, KH, KW, DATA_WIDTH, PE>(cvt_out, out);
}


template<int C, int H, int W, int DATA_WIDTH, int PE>
void batch_normalization(hls::stream<ap_uint<DATA_WIDTH * C>> &in, const float scale[C], const float B[C], const float mean[C], const float var[C], hls::stream<ap_uint<DATA_WIDTH * C>> &out) {
#ifdef DEBUG 
  printf("batch_normalization %dx%dx%d\n", C, H, W);
#endif 

  int num_items = H * W;
  int block = 0;

  for (int i = 0; i < num_items; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)

    ap_uint<DATA_WIDTH * C> px;
    ap_uint<DATA_WIDTH * C> p_out;

    if (block == 0) px = in.read();

    for (int pe = 0; pe < PE; pe++) {
      DO_PRAGMA(HLS UNROLL)

      int c_pe = block+pe;
      int first = c_pe * DATA_WIDTH;
      int last = first + DATA_WIDTH - 1;
      ap_uint<DATA_WIDTH> aux = px.range(last, first);
      float f_in;
      *(ap_uint<DATA_WIDTH>*)(&f_in) = (float)aux;
      //
      float f_out;
      f_out = (((f_in - mean[c_pe]) / var[c_pe]) * scale[c_pe]) + B[c_pe];
#ifdef DEBUG_BATCH_NORMALIZATION
      printf("BATCH_NORMALIZATION: i %d f_in %f f_out %f\n", i, f_in, f_out);
#endif
      p_out.range(last, first) = *(ap_uint<DATA_WIDTH>*)(&f_out);
    }

    block = block + PE;

    if (block == PE) {
      block = 0;
      out << p_out;
    }
  }
}

template<int C, int H, int W, int COUT, int DATA_WIDTH>
void reduction(hls::stream<ap_uint<DATA_WIDTH * C>> &in, hls::stream<ap_uint<DATA_WIDTH * COUT>> &out) {
#ifdef DEBUG 
  printf("reduction %dx%dx%dx%d\n", COUT, C, H, W);
#endif 
  ap_uint<DATA_WIDTH * C> p_in;
  ap_uint<DATA_WIDTH * COUT> p_out;
  for (int i = 0; i < H*W; i++) {
    DO_PRAGMA(HLS PIPELINE II=1)
    p_in = in.read();
    int first = 0;
    int last = COUT * DATA_WIDTH - 1;
    p_out = p_in.range(last, first);
    out << p_out;
  }
}

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
  hls::stream<ap_uint<DATA_WIDTH * 128 /*256*/>> st21;
  conv_bipolar<128, 5, 5, 128 /*256*/, 3, 3, 0, 0, 0, 0, 1, 1, 1, DATA_WIDTH, 1, 4>(st20, bfilter4, st21);

  // batch normalization
  hls::stream<ap_uint<DATA_WIDTH * 128 /*256*/>> st22;
  batch_normalization<128 /*256*/, 3, 3, DATA_WIDTH, 1>(st21, scale4, B4, mean4, var4, st22);

  // bipolar
  hls::stream<ap_uint<1 * 128 /*256*/>> st23;
  bipolar<128 /*256*/, 3, 3, DATA_WIDTH, 1>(st22, st23);

  // conv (256x256x3x3)
  hls::stream<ap_uint<DATA_WIDTH * 128 /*256*/>> st24;
  conv_bipolar<128 /*256*/, 3, 3, 128 /*256*/, 3, 3, 0, 0, 0, 0, 1, 1, 1, DATA_WIDTH, 1, 1>(st23, bfilter5, st24);

  // batch normalization   
  hls::stream<ap_uint<DATA_WIDTH * 128 /*256*/>> st25;
  batch_normalization<128 /*256*/, 1, 1, DATA_WIDTH, 1>(st24, scale5, B5, mean5, var5, st25);

  // bipolar
  hls::stream<ap_uint<1 * 128 /*256*/>>st26;
  bipolar<128 /*256*/, 1, 1, DATA_WIDTH, 1>(st25, st26);

  // reshape 256x1x1 -> 256
  //

  // matmul (256x512) 
  hls::stream<ap_uint<DATA_WIDTH * 128 /*512*/>> st27;
  matmul_bipolar<128 /*256*/, 128 /*512*/, DATA_WIDTH, 1>(st26, weights0, st27);

  // unsqueeze 512 -> 512x1
  //

  // batch normalization
  hls::stream<ap_uint<DATA_WIDTH * 128 /*512*/>> st28;
  batch_normalization<128 /*512*/, 1, 1, DATA_WIDTH, 1>(st27, scale6, B6, mean6, var6, st28);

  // squeeze 512x1 -> 512
  //

  // bipolar
  hls::stream<ap_uint<1 * 128 /*512*/>> st29;
  bipolar<128 /*512*/, 1, 1, DATA_WIDTH, 1>(st28, st29);

  // matmul (512x512)
  hls::stream<ap_uint<DATA_WIDTH * 128 /*512*/>> st30;
  matmul_bipolar<128 /*512*/, 128 /*512*/, DATA_WIDTH, 1>(st29, weights1, st30);

  // unsqueeze 512 -> 512x1
  //

  // batch normalization
  hls::stream<ap_uint<DATA_WIDTH * 128 /*512*/>> st31;
  batch_normalization<128 /*512*/, 1, 1, DATA_WIDTH, 1>(st30, scale7, B7, mean7, var7, st31);

  // squeeze 512x1 -> 512
  //

  // bipolar
  hls::stream<ap_uint<1 * 128 /*512*/>> st32;
  bipolar<128 /*512*/, 1, 1, DATA_WIDTH, 1>(st31, st32);

  // matmul (512x10)
  hls::stream<ap_uint<DATA_WIDTH * 10>> st33;
  matmul_bipolar<128 /*512*/, 10, DATA_WIDTH, 1>(st32, weights2, st33);

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

  // reduction
  //hls::stream<ap_uint<DATA_WIDTH * 1>> st17;
  //reduction<128, 12, 12, 1, DATA_WIDTH>(st15, st17);

  // 
  // Write (10x1x1)
  write<10, 1, 1, DATA_WIDTH>(st37, (ap_uint<DATA_WIDTH * 10> *)ptr_out);


}
