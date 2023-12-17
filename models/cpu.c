/*
 * cpu.c
 *
 * This fils provides support for node execution on CPU
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "nodes.h"
#include "initializers.h"
#include "in_out.h"
#include "stats.h"
#include "fpga.h"

void fn_conv2d(float *in, float *w, float *b, float *out, int HI, int WI, int I, int HO, int WO, int O, int KH, int KW, int SH, int SW, int PT, int PB, int PL, int PR) {
  //
  if (verbose) {
    printf("    buffer_i: %p buffer_w: %p buffer_b: %p buffer_out: %p\n", in, w, b, out);
    printf("    IxHIxWI: %3dx%3dx%3d OxHOxWO: %3dx%3dx%3d KWxKW: %3dx%3d SHxSW: %3dx%3d padings: %1d-%1d-%1d-%1d\n", I, HI, WI, O, HO, WO, KH, KW, SH, SW, PT, PB, PL, PR);
  }
  int size_o = O * WO * HO;
  for (int x = 0; x<size_o; x++) out[x] = 0.f;

  for (int i=0; i<I; i++) {
    for (int o=0; o<O; o++) {
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
	  // bias
	  out[addr_o] += b[o];
	}
      }
    }
  }
}

void fn_bn(float *in, float *gamma, float *beta, float *running_mean, float *running_var, float *out, int I, int HI, int WI, float epsilon) {
  for (int i=0; i<I; i++) {
    for (int hi = 0; hi<HI; hi++) {
      for (int wi = 0; wi<WI; wi++) {
	int addr = (i * HI * WI) + (hi * WI) + wi;
        float data = in[addr];
	data = (data - running_mean[i]) / sqrt(running_var[i] + epsilon);
	data = (data * gamma[i]) - beta[i];
	out[addr] = data;
      }
    }
  }
}

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

void fn_maxpool(float *in, float *out, int I, int HI, int WI, int O, int HO, int WO, int KH, int KW, int SH, int SW, int PT, int PB, int PL, int PR) {
  //
  if (verbose) {
    printf("    buffer_i: %p buffer_out: %p\n", in, out);
    printf("    IxHIxWI: %3dx%3dx%3d OxHOxWO: %3dx%3dx%3d KWxKW: %3dx%3d SHxSW: %3dx%3d padings: %1d-%1d-%1d-%1d\n", I, HI, WI, O, HO, WO, KH, KW, SH, SW, PT, PB, PL, PR);
  }

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

void fn_h2d(float *in, float *out, int I, int HI, int WI) {
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

void fn_d2h(float *in, float *out, int I, int HI, int WI) {
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


void fn_run_node_on_cpu(int n) {

  if (n==-1) return;
  if (!aNode[n].valid) return;

  if (verbose) printf("    running on CPU\n");

  // input data buffer is the output buffer of its parent or the input model buffer
  float *in;
  float *in0, *in1;
  int n_parent;              // parent for the case only one parent
  int n_parent0, n_parent1;  // parents for the case two parents
  if (has_no_parents(n)) {
    int i_input = fn_get_model_input_from_node(n);
    if (i_input==-1) {printf("Error, model input not found\n"); exit(1);}
    in = aInput[i_input].data;
    if (verbose) {
      float min, max, avg;
      size_t num_items;
      fn_input_buffer_stats(i_input, &min, &max, &avg, &num_items);
      printf("    buffer_i: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);
    }
  } else {
    int list[100];
    int np = fn_get_parent_nodes(aNode[n].name, list);
    if (np == 0) {printf("Error, no parents\n"); exit(1);}
    else if (np == 1) {    
      n_parent = list[0];
      in = aNode[n_parent].data;
      if (verbose) {
        float min, max, avg;
        size_t num_items;
        fn_node_buffer_stats(n_parent, &min, &max, &avg, &num_items);
        printf("    buffer_i: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);
      }
    } else if (np == 2) {
      n_parent0 = list[0];
      n_parent1 = list[1];
      in0 = aNode[n_parent0].data;
      in1 = aNode[n_parent1].data;
      if (verbose) {
	float min, max, avg;
	size_t num_items;
	fn_node_buffer_stats(n_parent0, &min, &max, &avg, &num_items);
	printf("    buffer_i: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);
	fn_node_buffer_stats(n_parent1, &min, &max, &avg, &num_items);
        printf("    buffer_i: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);
      }
    } else {
      printf("non-supported number of parents\n"); 
      exit(1);
    }
  }
  // output buffer
  float *out = aNode[n].data;

  if (!strcmp(aNode[n].type, "Conv")) {
    // weight and bias
    int i_weight = get_weight_initializer_entry_from_node(n);
    if (i_weight == -1) {printf("Error, weight initializer not found\n"); exit(1);}
    float *w = aInitializer[i_weight].data;
    int i_bias = get_bias_initializer_entry_from_node(n);
    if (i_bias == -1) {printf("Error, bias initializer not found\n"); exit(1);}
    float *b = aInitializer[i_bias].data;
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
    fn_conv2d(in, w, b, out, HI, WI, I, HO, WO, O, KH, KW, SH, SW, PT, PB, PL, PR);
    //
    if (verbose) {
      float min, max, avg;
      size_t num_items;
      fn_initializer_buffer_stats(i_weight, &min, &max, &avg, &num_items);
      printf("    buffer_w: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);
      fn_initializer_buffer_stats(i_bias, &min, &max, &avg, &num_items);
      printf("    buffer_b: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);
    }
  }

  if (!strcmp(aNode[n].type, "Add")) {
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    //
    fn_add(in0, in1, out, I, HI, WI);
  }

  if (!strcmp(aNode[n].type, "BatchNormalization")) {
    // gamma initializer
    int i_gamma = get_gamma_initializer_id_from_node(n);
    if (i_gamma == -1) {printf("Error, gamma initializer not found\n"); exit(1);}
    float *gamma = aInitializer[i_gamma].data;
    // beta initializer
    int i_beta = get_beta_initializer_id_from_node(n);
    if (i_beta == -1) {printf("Error, beta initializer not found\n"); exit(1);}
    float *beta = aInitializer[i_beta].data;
    // running_mean initializer
    int i_running_mean = get_running_mean_initializer_id_from_node(n);
    if (i_running_mean == -1) {printf("Error, running_mean initializer not found\n"); exit(1);}
    float *running_mean = aInitializer[i_running_mean].data;
    // running_var initializer
    int i_running_var = get_running_var_initializer_id_from_node(n);
    if (i_running_var == -1) {printf("Error, running_var initializer not found\n"); exit(1);}
    float *running_var = aInitializer[i_running_var].data;
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    float epsilon = aNode[n].epsilon;  // TODO: momentum?
    //
    fn_bn(in, gamma, beta, running_mean, running_var, out, I, HI, WI, epsilon);
    //
    if (verbose) {
      float min, max, avg;
      size_t num_items;
      fn_initializer_buffer_stats(i_gamma, &min, &max, &avg, &num_items);
      printf("    buffer_gamma: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);
      fn_initializer_buffer_stats(i_beta, &min, &max, &avg, &num_items);
      printf("    buffer_beta: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);
      fn_initializer_buffer_stats(i_running_mean, &min, &max, &avg, &num_items);
      printf("    buffer_running_mean: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);
      fn_initializer_buffer_stats(i_running_var, &min, &max, &avg, &num_items);
      printf("    buffer_running_var: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);    }
  }

  if (!strcmp(aNode[n].type, "Relu")) {
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    fn_relu(in, out, I, HI, WI);
  }

  if (!strcmp(aNode[n].type, "MaxPool")) {
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
  }

  if (!strcmp(aNode[n].type, "h2d")) {
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    fn_h2d(in, out, I, HI, WI);
    copy_to_fpga(aNode[n].buffer);
  }

  if (!strcmp(aNode[n].type, "d2h")) {
    // parameters
    int HI = aNode[n].HI;
    int WI = aNode[n].WI;
    int I  = aNode[n].I;
    copy_from_fpga(aNode[n_parent].buffer);
    fn_d2h(in, out, I, HI, WI);
  }

  if (verbose) {
    float min, max, avg;
    size_t num_items;
    fn_node_buffer_stats(n, &min, &max, &avg, &num_items);
    printf("    buffer_o: min: %6.4f max: %6.4f avg: %6.4f, items: %6ld\n", min, max, avg, num_items);

  }
}
