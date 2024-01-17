/*
 * cpu.c
 *
 * This fils provides support for node execution on CPU
 *
 */

#ifdef RUNTIME_SUPPORT

#include<math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "nodes.h"
#include "initializers.h"
#include "in_out.h"
#include "stats.h"
#include "fpga.h"
#include "utils.h"

/*
 * fn_softmax()
 *
 * performs the softmax activation function
 *
 */
void fn_softmax(float *in, float *out, int n) {

    int i;
    int j;
    float _max;
    float sum;

    _max = in[0];
    // Get the max value
    for (i = 0; i < n; i++) _max = max(in[i], _max);
    // Get the sum
    sum = 0;
    for (i = 0; i < n; i++) {
     out[i] = exp(in[i] - _max);
     sum = sum + out[i];
    }

    // Get the probabilities
    for (i = 0; i < n; i++) out[i] = out[i] / sum;
}

/* 
 * fn_conv2D()
 *
 * performs 2D durect convolution
 *
 */
void fn_conv2d(float *in, float *w, float *b, float *out, int HI, int WI, int I, int HO, int WO, int O, int KH, int KW, int SH, int SW, int PT, int PB, int PL, int PR, int use_bias) {
  // verbose information
  if (verbose && verbose_level >= 3) {
    printf("    buffer_i: %p buffer_w: %p buffer_b: %p buffer_out: %p\n", in, w, b, out);
    printf("    IxHIxWI: %3dx%3dx%3d OxHOxWO: %3dx%3dx%3d KWxKW: %3dx%3d SHxSW: %3dx%3d padings: %1d-%1d-%1d-%1d\n", 
		I, HI, WI, O, HO, WO, KH, KW, SH, SW, PT, PB, PL, PR);
  }
  // let's initialize output buffer
  int size_o = O * WO * HO;
  memset(out, 0, size_o*sizeof(float));

  // 6-loop 2d-convolution implementation
  #pragma omp parallel for 
  for (int o=0; o<O; o++) {
    for (int i=0; i<I; i++) {
      for (int ho=0; ho<HO; ho++) {
	for (int wo=0; wo<WO; wo++) {
          int addr_o = (o * HO * WO) + (ho * WO) + wo;
	  for (int kh = 0; kh < KH; kh++) {
	    for (int kw = 0; kw < KW; kw++) {
	      int hi = (ho * SH) - PT + kh;
	      int wi = (wo * SW) - PL + kw;
	      int padding = (hi<0) | (wi<0) | (wi>=WI) | (hi>=HI);
	      int addr_w = (o * I * KH * KW) + (i * KH * KW) + (kh * KW) + kw;
	      int addr_i = (i * HI * WI) + (hi * WI) + wi;
	      if (!padding) out[addr_o] += in[addr_i] * w[addr_w];
            }
	  }
	}
      }
    }
  }

  // bias addition
  if (use_bias) {
    for (int o=0; o<O; o++) {
      for (int ho=0; ho<HO; ho++) {
        for (int wo=0; wo<WO; wo++) {
          int addr_o = (o * HO * WO) + (ho * WO) + wo;
          out[addr_o] += b[o];
        }
      }
    }
  }
}

/*
 * fn_global_average_pool()
 *
 * performs global average pool per dimension
 *
 */
void fn_global_average_pool(float *in, float *out, int HI, int WI, int I, int HO, int WO, int O) {
  // let's check some arguments
  if ((HO!=1) || (WO!=1) || (I!=O)) {printf("Error, in global average pool\n"); exit(1);}
  // let's perform global average pool per input (output) channel
  for (int i=0;i<I; i++) {
    float sum = 0.f;
    for (int h=0; h<HI; h++) {
      for (int w=0; w<WI; w++) {
	int addr_in = (i * HI * WI) + (h * WI) + w;
        sum += in[addr_in];
      }
    }
    int addr_out = (i * HO * WO);
    out[addr_out] = sum / (float) (HI * WI);
  }
}

/*
 * fn_flatten()
 *
 * Flattens an input buffer
 *
 */
void fn_flatten(float *in, float *out, int HI, int WI, int I, int HO, int WO, int O) {
  //
  #pragma omp parallel for
  for (int i=0; i<I; i++) {
    for (int hi=0; hi<HI; hi++) {
      for (int wi=0; wi<WI; wi++) {
        int addr_in = (i * HI * WI) + (hi * WI) + wi;
	int addr_out = addr_in;
	out[addr_out] = in[addr_in];
      }
    }
  }
}

/*
 * fn_identity()
 *
 * Performs identity function (just copy an input buffer onto an output buffer)
 *
 */
void fn_identity(float *in, float *out, int HI, int WI, int I, int HO, int WO, int O) {
  size_t n = I * HI * WI;
  for (size_t x=0; x<n; x++) out[x] = in[x];
}

/*
 * fn_pad()
 *
 * Pads the input buffer
 *
 */
void fn_pad(float *in, float *out, int HI, int WI, int I) {   // TODO: implement pad correctly
  size_t n = I * HI * WI;
  for (size_t x=0; x<n; x++) out[x] = in[x];
}

/*
 *
 * fn_sigmoid()
 *
 * Performs element-wise sigmoid function
 *
 */
void fn_sigmoid(float *in, float *out, int HI, int WI, int I) {
  size_t n = I * HI * WI;
  for (size_t x=0; x<n; x++) out[x] = 1 / (1 + expf(-in[x]));
}

/*
 * fn_mul()
 *
 * Performs element-wise multiplication of two input buffers
 *
 */
void fn_mul(float *in0, float *in1, float *out, int HI, int WI, int I) {
  size_t n = I * HI * WI;
  for (size_t x=0; x<n; x++) out[x] = in0[x] * in1[x];
}

/*
 * fn_concat()
 *
 * Concatenates (copies) an input buffer to an output buffer given an offset
 *
 */
void fn_concat(float *in, float *out, size_t size, size_t offset) {
  for (size_t x=0; x<size; x++) out[x + offset] = in[x];
}

/*
 * fn_gemm()
 *
 * Performs GEMM operation: out = in * w + b
 *
 */
void fn_gemm(float *in, float *w, float *b, float *out, int HI, int WI, int I, int HO, int WO, int O) {
  // n and m dimensions
  int n = I * HI * WI;
  int m = O * HO * WO;

  // operation
  for (int o=0; o<m; o++) { // for every output
    float r = 0.f;
    for (int i=0; i<n; i++) { // dot product
      int addr_in = i;
      int addr_w = (o * n) + i;
      r += (in[addr_in] * w[addr_w]);
    }
    int addr_out = o;
    int addr_b = o;
    out[addr_out] = r + b[addr_b];
  }
}

/*
 * fn_matmul()
 *
 * Performs matmul operation: out = in * w
 *
 */
void fn_matmul(float *in, float *w, float *out, int HI, int WI, int I, int HO, int WO, int O) {
  // n and m dimensions
  int n = I * HI * WI;
  int m = O * HO * WO;

  // operation
  for (int o=0; o<m; o++) { // for every output
    float r = 0.f;
    for (int i=0; i<n; i++) { // dot product
      int addr_in = i;
      int addr_w = (o * n) + i;
      r += (in[addr_in] * w[addr_w]);
    }
    int addr_out = o;
    out[addr_out] = r;
  }
}

/*
 * fn_transpose()
 *
 * Transposes the input assuming permutation 2 0 1
 *
 */
void fn_transpose(float *in, float *out, int HI, int WI, int I, int HO, int WO, int O) {
  for (int i=0; i<I; i++) {
    for (int hi=0; hi<HI; hi++) {
      for (int wi=0; wi<WI; wi++) {
	int addr_in = (i * HI * WI) + (hi * WI) + wi;
	float data = in[addr_in];
	int o = wi;
	int ho = hi;
	int wo = i;
	int addr_out = (o * HO * WO) + (ho * WO) + wo;
	out[addr_out] = data;
      }
    }
  }
}

/*
 * fn_bn()
 *
 * Performs batch normalization to each channel
 *
 * out = (in - running_mean) / sqrt(running_var + epsilon) * gamma + beta
 *
 */
void fn_bn(float *in, float *gamma, float *beta, float *running_mean, float *running_var, float *out, int I, int HI, int WI, float epsilon) {
  for (int i=0; i<I; i++) {
    for (int hi = 0; hi<HI; hi++) {
      for (int wi = 0; wi<WI; wi++) {
	int addr = (i * HI * WI) + (hi * WI) + wi;
        float data = in[addr];
	data = (data - running_mean[i]) / sqrt(running_var[i] + epsilon);
	data = (data * gamma[i]) + beta[i];
	out[addr] = data;
      }
    }
  }
}

/*
 * fn_add()
 *
 * Adds two buffers: out = in0 + in1
 *
 */
void fn_add(float *in0, float *in1, float *out, int I, int HI, int WI) {
  for (int i=0; i<I; i++) {
    for (int hi = 0; hi<HI; hi++) {
      for (int wi = 0; wi<WI; wi++) {
        int addr = (i * HI * WI) + (hi * WI) + wi;
        float data = in0[addr] + in1[addr];
        data = (data>=0) ? data : 0.f;
        out[addr] = data;
      }
    }
  }
}

/*
 * fn_relu()
 *
 * Performs relu activation to each input element: out = max(in, 0)
 *
 */
void fn_relu(float *in, float *out, int I, int HI, int WI) {
  for (int i=0; i<I; i++) {
    for (int hi = 0; hi<HI; hi++) {
      for (int wi = 0; wi<WI; wi++) {
        int addr = (i * HI * WI) + (hi * WI) + wi;
        float data = in[addr];
	data = (data>=0) ? data : 0.f;
        out[addr] = data;
      }
    }
  }
}

/*
 * fn_maxpool()
 *
 * Performs the maxpool operation to each channel
 *
 */
void fn_maxpool(float *in, float *out, int I, int HI, int WI, int O, int HO, int WO, int KH, int KW, int SH, int SW, int PT, int PB, int PL, int PR) {
  // verbosity information
  if (verbose && verbose_level >= 3) {
    printf("    buffer_i: %p buffer_out: %p\n", in, out);
    printf("    IxHIxWI: %3dx%3dx%3d OxHOxWO: %3dx%3dx%3d KWxKW: %3dx%3d SHxSW: %3dx%3d padings: %1d-%1d-%1d-%1d\n", 
		I, HI, WI, O, HO, WO, KH, KW, SH, SW, PT, PB, PL, PR);
  }

  // operation
  for (int o=0; o<O; o++) {
    int i = o;
    for (int ho=0; ho<HO; ho++) {
      for (int wo=0; wo<WO; wo++) {
        int addr_o = (o * HO * WO) + (ho * WO) + wo;
        float data_max = -999999999999;  // TODO
        for (int kh = 0; kh < KH; kh++) {
          for (int kw = 0; kw < KW; kw++) {
            int hi = (ho * SH) - PT + kh;
            int wi = (wo * SW) - PL + kw;
            int padding = (hi<0) | (wi<0) | (wi>=WI) | (hi>=HI);
            int addr_i = (i * HI * WI) + (hi * WI) + wi;
            float data = padding ? 0.f : in[addr_i];
            if (data > data_max) data_max = data;
          }
        }
        out[addr_o] = data_max;
      }
    }
  }
}

/*
 * fn_avgpool()
 *
 * Performs the avgpool operation to each channel
 *
 */
void fn_avgpool(float *in, float *out, int I, int HI, int WI, int O, int HO, int WO, int KH, int KW, int SH, int SW, int PT, int PB, int PL, int PR) {
  // verbosity information
  if (verbose && verbose_level >= 3) {
    printf("    buffer_i: %p buffer_out: %p\n", in, out);
    printf("    IxHIxWI: %3dx%3dx%3d OxHOxWO: %3dx%3dx%3d KWxKW: %3dx%3d SHxSW: %3dx%3d padings: %1d-%1d-%1d-%1d\n", 
		I, HI, WI, O, HO, WO, KH, KW, SH, SW, PT, PB, PL, PR);
  }

  // operation
  for (int o=0; o<O; o++) {
    int i = o;
    for (int ho=0; ho<HO; ho++) {
      for (int wo=0; wo<WO; wo++) {
        int addr_o = (o * HO * WO) + (ho * WO) + wo;
	float data_sum = 0.f;
        for (int kh = 0; kh < KH; kh++) {
          for (int kw = 0; kw < KW; kw++) {
            int hi = (ho * SH) - PT + kh;
            int wi = (wo * SW) - PL + kw;
            int padding = (hi<0) | (wi<0) | (wi>=WI) | (hi>=HI);
            int addr_i = (i * HI * WI) + (hi * WI) + wi;
            float data = padding ? 0.f : in[addr_i];
            data_sum += data;
          }
        }
        out[addr_o] = data_sum / (float)(KH * KW);;
      }
    }
  }
}

/*
 * fn_h2d()
 *
 * Performns host_to_device copy of buffer: out = in
 *
 */
void fn_h2d(float *in, float *out, int I, int HI, int WI, int O, int HO, int WO) {

  // output is zeroed first, it may happen that output
  // buffer is larger (more channels than input because of paddings)
  memset(out, 0, O * HO * WO * sizeof(float));

  // copy out = in
  for (int i=0; i<I; i++) {
    for (int hi = 0; hi<HI; hi++) {
      for (int wi = 0; wi<WI; wi++) {
        int addr_in = (i * HI * WI) + (hi * WI) + wi;
	int gi = i / CPI;
	int cpi = i % CPI;
        int addr_out = (gi * HI * WI * CPI) + (hi * WI * CPI) + (wi * CPI) + cpi;
        out[addr_out] = in[addr_in];
      }
    }
  }
}

/*
 * fn_d2h()
 *
 * Performns device_to_host copy of buffer: out = in
 *
 */
void fn_d2h(float *in, float *out, int I, int HI, int WI) {

  // copy operation
  for (int i=0; i<I; i++) {
    for (int hi = 0; hi<HI; hi++) {
      for (int wi = 0; wi<WI; wi++) {
	int gi = i / CPI;
	int cpi = i % CPI;
	int addr_in = (gi * HI * WI * CPI) + (hi * WI * CPI) + (wi * CPI) + cpi;
        int addr_out = (i * HI * WI) + (hi * WI) + wi;
        out[addr_out] = in[addr_in];
      }
    }
  }
}

/*
 * fn_get_buffer_from_name()
 *
 * This function returns the buffer for the associated name.
 * The name can be an input model, an output of a node or an initializer
 *
 */
float *fn_get_buffer_from_name(char *name) {
  int i;
 
  // input buffer is model input?
  i = get_model_input_id(name);
  if (i != -1) return aInput[i].data;

  // input buffer is initializer?
  i = fn_get_initializer_by_name(name);
  if (i != -1) return aInitializer[i].data;

  // input buffer is node buffer?
  i = fn_get_node_by_output_name(name);
  if (i != -1) return aNode[i].data;

  // not found
  return NULL;
}

/*
 * fn_get_num_items_from_name()
 *
 * This function returns the size of a buffer for the associated name.
 * The name can be an input model, an output of a node or an initializer
 *
 */

int fn_get_num_items_from_name(char *name) {

  int num_items = 1;
  int i;

  // input buffer is model input?
  i = get_model_input_id(name);
  if (i != -1) {
    for (int d=0; d<aInput[i].num_dimensions; d++) if (aInput[i].dimensions[d] != 0) num_items = num_items * aInput[i].dimensions[d];
    return num_items;
  }

  // input buffer is initializer?
  i = fn_get_initializer_by_name(name);
  if (i != -1) {
    for (int d=0; d<aInitializer[i].num_dimensions; d++) num_items = num_items * aInitializer[i].dimensions[d];
    return num_items;
  }

  // input buffer is node buffer?
  i = fn_get_node_by_output_name(name);
  if (i != -1) {
    num_items = aNode[i].O * aNode[i].HO * aNode[i].WO;
    return num_items;
  }

  // not found, zero size
  return 0;
}

/*
 * fn_run_node_on_cpu()
 *
 * Runs the node passed as argument on the cpu
 *
 */
void fn_run_node_on_cpu(int n) {

  // let's check validity of the node
  if (n==-1) return;
  if (!aNode[n].valid) return;

  // verbosity information
  if (verbose && verbose_level >= 1) printf("    running %s (cpu)\n", aNode[n].name);

  // output buffer
  float *out = aNode[n].data;
  int num_items_out = fn_get_num_items_from_name(aNode[n].outputs[0]);

  if (!strcmp(aNode[n].type, "Conv")) {
    // input0
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char *)"      input: ");
    // weight
    float *w = fn_get_buffer_from_name(aNode[n].inputs[1]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[1], (char *)"      weight: ");
    // bias (optional)
    int use_bias = false;
    float *b = NULL;
    int num_items_b = 0;
    if (aNode[n].num_inputs == 3) {
      use_bias = true;
      b = fn_get_buffer_from_name(aNode[n].inputs[2]);
      if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[2], (char *)"      bias: ");
    }
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    int HO = aNode[n].HO;
    int WO = aNode[n].WO;
    int O  = aNode[n].O;
    int KH = aNode[n].kh;
    int KW = aNode[n].kw;
    int SH = aNode[n].sh;
    int SW = aNode[n].sw;
    int PT = aNode[n].pt;
    int PB = aNode[n].pb;
    int PL = aNode[n].pl;
    int PR = aNode[n].pr;
    //
    fn_conv2d(in, w, b, out, HI, WI, I, HO, WO, O, KH, KW, SH, SW, PT, PB, PL, PR, use_bias);

  } else if (!strcmp(aNode[n].type, "Add")) {
    // input 0
    float *in0 = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // input 1
    float *in1 = fn_get_buffer_from_name(aNode[n].inputs[1]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[1], (char*)"      input: "); 
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    //
    fn_add(in0, in1, out, I, HI, WI);

  } else if (!strcmp(aNode[n].type, "BatchNormalization")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level >=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"     input: ");
    // gamma
    float *gamma = fn_get_buffer_from_name(aNode[n].inputs[1]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[1], (char*)"      gamma: ");
    // beta
    float *beta = fn_get_buffer_from_name(aNode[n].inputs[2]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[2], (char*)"      beta: ");
    // running mean
    float *running_mean = fn_get_buffer_from_name(aNode[n].inputs[3]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[3], (char*)"      running_mean: ");
    // running var
    float *running_var = fn_get_buffer_from_name(aNode[n].inputs[4]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[4], (char*)"      running_var: ");
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    float epsilon = aNode[n].epsilon;  // TODO: momentum?
    //
    fn_bn(in, gamma, beta, running_mean, running_var, out, I, HI, WI, epsilon);

  } else if (!strcmp(aNode[n].type, "Relu")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    fn_relu(in, out, I, HI, WI);

  } else if (!strcmp(aNode[n].type, "Transpose")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    //
    fn_transpose(in, out, aNode[n].HI, aNode[n].WI, aNode[n].I, aNode[n].HO, aNode[n].WO, aNode[n].O);

  } else if (!strcmp(aNode[n].type, "MaxPool")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    int HO = aNode[n].HO;
    int WO = aNode[n].WO;
    int O  = aNode[n].O;
    int KH = aNode[n].kh;
    int KW = aNode[n].kw;
    int SH = aNode[n].sh;
    int SW = aNode[n].sw;
    int PT = aNode[n].pt;
    int PB = aNode[n].pb;
    int PL = aNode[n].pl;
    int PR = aNode[n].pr;
    fn_maxpool(in, out, I, HI, WI, O, HO, WO, KH, KW, SH, SW, PT, PB, PL, PR);

  } else if (!strcmp(aNode[n].type, "AveragePool")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    int HO = aNode[n].HO;
    int WO = aNode[n].WO;
    int O  = aNode[n].O;
    int KH = aNode[n].kh;
    int KW = aNode[n].kw;
    int SH = aNode[n].sh;
    int SW = aNode[n].sw;
    int PT = aNode[n].pt;
    int PB = aNode[n].pb;
    int PL = aNode[n].pl;
    int PR = aNode[n].pr;
    fn_avgpool(in, out, I, HI, WI, O, HO, WO, KH, KW, SH, SW, PT, PB, PL, PR);

  } else if (!strcmp(aNode[n].type, "h2d")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    int HO = aNode[n].HO;
    int WO = aNode[n].WO;
    int O  = aNode[n].O;
    if (verbose && verbose_level >= 3) printf("      IxHIxWI: %4dx%4dx%4d -> OxHOxWO: %4dx%4dx%4d\n", I, HI, WI, O, HO, WO);
    fn_h2d(in, out, I, HI, WI, O, HO, WO);
    copy_to_fpga(aNode[n].buffer);

  } else if (!strcmp(aNode[n].type, "d2h")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    int n_parent = fn_get_node_by_output_name(aNode[n].inputs[0]);
    copy_from_fpga(aNode[n_parent].buffer);
    fn_d2h(in, out, I, HI, WI);

  } else if (!strcmp(aNode[n].type, "GlobalAveragePool")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    int HO = aNode[n].HO;
    int WO = aNode[n].WO;
    int O  = aNode[n].O;
    fn_global_average_pool(in, out, HI, WI, I, HO, WO, O);

  } else if (!strcmp(aNode[n].type, "Flatten")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char *)"      input: ");
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    int HO = aNode[n].HO;
    int WO = aNode[n].WO;
    int O  = aNode[n].O;
    fn_flatten(in, out, HI, WI, I, HO, WO, O);

  } else if (!strcmp(aNode[n].type, "Identity")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    fn_identity(in, out, aNode[n].HI, aNode[n].WI, aNode[n].I, aNode[n].HO, aNode[n].WO, aNode[n].O);

  } else if (!strcmp(aNode[n].type, "Gemm")) {  // TODO: add C tensor (optional)
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // weight 
    float *w = fn_get_buffer_from_name(aNode[n].inputs[1]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[1], (char*)"      weights: ");
    // bias
    float *b = fn_get_buffer_from_name(aNode[n].inputs[2]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[2], (char*)"      bias: ");
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I = aNode[n].I;
    int HO = aNode[n].HO;
    int WO = aNode[n].WO;
    int O = aNode[n].O;
    fn_gemm(in, w, b, out, HI, WI, I, HO, WO, O);

  } else if (!strcmp(aNode[n].type, "MatMul")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // weights
    float *w = fn_get_buffer_from_name(aNode[n].inputs[1]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[1], (char*)"      weights: ");
    fn_matmul(in, w, out, aNode[n].HI, aNode[n].WI, aNode[n].I, aNode[n].HO, aNode[n].WO, aNode[n].O);

  } else if (!strcmp(aNode[n].type, "Concat")) {
    size_t offset = 0;
    for (int i=0; i<aNode[n].num_inputs; i++) {
      float *in = fn_get_buffer_from_name(aNode[n].inputs[i]);
      if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[i], (char*)"      input: ");
      int x = fn_get_node_by_output_name(aNode[n].inputs[i]);
      size_t size = aNode[x].HO * aNode[x].WO * aNode[x].O;
      fn_concat(in, out, size, offset);
      offset += size;
    }
  } else if (!strcmp(aNode[n].type, "Pad")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    fn_pad(in, out, aNode[n].HI, aNode[n].WI, aNode[n].I);    
  } else if (!strcmp(aNode[n].type, "Sigmoid")) {
    // input 0 
    float *in = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    fn_sigmoid(in, out, aNode[n].HI, aNode[n].WI, aNode[n].I);
  } else if (!strcmp(aNode[n].type, "Mul")) {
    // input 0 
    float *in0 = fn_get_buffer_from_name(aNode[n].inputs[0]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[0], (char*)"      input: ");
    // input 1 
    float *in1 = fn_get_buffer_from_name(aNode[n].inputs[1]);
    if (verbose && verbose_level>=3) fn_buffer_stats_by_name(aNode[n].inputs[1], (char*)"      input: ");    
    fn_mul(in0, in1, out, aNode[n].HI, aNode[n].WI, aNode[n].I);
  } else {
    printf("Error, layer not supported yet\n");
    exit(1);
  }

  // verbosity information
  if (verbose && verbose_level >= 2) fn_buffer_stats_by_name(aNode[n].outputs[0], (char*)"      out  : ");
}

#endif
