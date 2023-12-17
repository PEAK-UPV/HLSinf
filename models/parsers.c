/*
 * parsers.c
 *
 * This file includes parser for the HLSinf accelerator
 *
 * The configuration of this accelerator is as follows:
 *
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "nodes.h"
#include "initializers.h"
#include "utils.h"
#include "main.h"
#include "model.h"
#include "in_out.h"

//int step=0;

int fn_detect_pattern(int n, char *type0, char *type1, char *type2, char *type3, char *type4, int *n0, int *n1, int *n2, int *n3, int *n4) {

  // printf("detect: %d -> %s %s %s %s %s : current type %s\n", n, type0, type1, type2, type3, type4, aNode[n].type);

  if (!aNode[n].valid) return false;
  if (type0 == NULL) return true;
  if (strcmp(aNode[n].type, type0)) return false;
  if (!aNode[n].supported) return false;

  // we get the child nodes list
  int list[100];
  int num_childs;
  num_childs = fn_get_child_nodes(aNode[n].name, list);
  if (num_childs == 0) {
    if (type1 == NULL) {*n0 = n; return true;}
  } else {
    for (int c=0; c<num_childs; c++) {
      if (fn_detect_pattern(list[c], type1, type2, type3, type4, NULL, n1, n2, n3, n4, NULL)) {
	*n0 = n;
	return true;
      }
    }
  }
  return false;
}

void fn_merge_nodes(int n0, int n1, int n2, int n3, int n4, char *keyword) {

  int conv_layer = is_conv(n0) ? n0 : is_conv(n1) ? n1 : is_conv(n2) ? n2 : is_conv(n3) ? n3 : is_conv(n4) ? n4 : -1;
  int relu_layer = is_relu(n0) ? n0 : is_relu(n1) ? n1 : is_relu(n2) ? n2 : is_relu(n3) ? n3 : is_relu(n4) ? n4 : -1;
  int maxpool_layer = is_maxpool(n0) ? n0 : is_maxpool(n1) ? n1 : is_maxpool(n2) ? n2 : is_maxpool(n3) ? n3 : is_maxpool(n4) ? n4 : -1;
  int bn_layer = is_bn(n0) ? n0 : is_bn(n1) ? n1 : is_bn(n2) ? n2 : is_bn(n3) ? n3 : is_bn(n4) ? n4 : -1;
  int add_layer = is_add(n0) ? n0 : is_add(n1) ? n1 : is_add(n2) ? n2 : is_add(n3) ? n3 : is_add(n4) ? n4 : -1;
  int merging_conv = conv_layer != -1;
  int merging_relu = relu_layer != -1;
  int merging_bn = bn_layer != -1;
  int merging_maxpool = maxpool_layer != -1;
  int merging_add = add_layer != -1;
  int last_merged_node = (n1==-1)?n0:(n2==-1)?n1:(n3==-1)?n2:(n4==-1)?n3:n4;

  // new node
  int n = num_nodes;
  num_nodes++;

  aNode[n].valid = true;
  aNode[n].name = (char*)malloc(sizeof(char) * (30));
  sprintf(aNode[n].name, "HLSinf_%0d", n);
  // keyword
  aNode[n].keyword = (char*)malloc(sizeof(char) * (strlen(keyword)+1));
  strcpy(aNode[n].keyword, keyword);
  // type
  aNode[n].type = (char*)malloc(sizeof(char) * (strlen("HLSinf")+1));
  strcpy(aNode[n].type, "HLSinf");
  // outputs
  aNode[n].num_outputs = 1;
  aNode[n].outputs = (char**)malloc(sizeof(char*) * 1);
  aNode[n].outputs[0] = (char*)malloc(sizeof(char) * (strlen(aNode[n].name)+1));
  strcpy(aNode[n].outputs[0], aNode[n].name);

  // inputs
  // if a merged layer is a conv then its weights and bias need to be added
  if (merging_conv) add_input_to_node(n, get_weight_initializer_name_from_node(conv_layer));
  if (merging_conv) add_input_to_node(n, get_bias_initializer_name_from_node(conv_layer));

  // the data input from first node to merge needs to be added
  add_input_to_node(n, get_data_input_name_from_node(n0, NULL));

  // if a batch normalization layer is merged its initializers need to be added
  if (merging_bn) {
    add_input_to_node(n, get_gamma_initializer_name_from_node(bn_layer)); 
    add_input_to_node(n, get_beta_initializer_name_from_node(bn_layer)); 
    add_input_to_node(n, get_running_mean_initializer_name_from_node(bn_layer)); 
    add_input_to_node(n, get_running_var_initializer_name_from_node(bn_layer));
  }
  // if an add layer is merged, its data input not included in the merge set must be added
  if (is_add(n1)) add_input_to_node(n, get_data_input_name_from_node(n1, get_node_name(n0)));
  if (is_add(n2)) add_input_to_node(n, get_data_input_name_from_node(n2, get_node_name(n1)));
  if (is_add(n3)) add_input_to_node(n, get_data_input_name_from_node(n3, get_node_name(n2)));
  if (is_add(n4)) add_input_to_node(n, get_data_input_name_from_node(n4, get_node_name(n3))); 

  // now we need to fix inputs of nodes pointing to the merged nodes, now they need to be linked to the new node
  fn_relink_node_inputs(get_node_name(n0), get_node_name(n));
  fn_relink_node_inputs(get_node_name(n1), get_node_name(n));
  fn_relink_node_inputs(get_node_name(n2), get_node_name(n));
  fn_relink_node_inputs(get_node_name(n3), get_node_name(n));
  fn_relink_node_inputs(get_node_name(n4), get_node_name(n));

  // parameters for HLSinf layer
  aNode[n].has_conv = merging_conv;
  aNode[n].has_relu = merging_relu;
  aNode[n].has_bn   = merging_bn;
  aNode[n].has_add  = merging_add;
  aNode[n].has_maxpool = merging_maxpool;

  // input add
  char *input_add = NULL;
  if (is_add(n1)) input_add = get_data_input_name_from_node(n1, get_node_name(n0));
  if (is_add(n2)) input_add = get_data_input_name_from_node(n2, get_node_name(n1));
  if (is_add(n3)) input_add = get_data_input_name_from_node(n3, get_node_name(n2));
  if (is_add(n4)) input_add = get_data_input_name_from_node(n4, get_node_name(n3));
  if (input_add != NULL) {
    aNode[n].input_add = (char*)malloc(sizeof(char) * (strlen(input_add)+1));
    strcpy(aNode[n].input_add, input_add);
  } else aNode[n].input_add = NULL;

  aNode[n].I = aNode[n0].I;
  aNode[n].HI = aNode[n0].HI;
  aNode[n].WI = aNode[n0].WI;

  aNode[n].O = aNode[last_merged_node].O;
  aNode[n].HO = aNode[last_merged_node].HO;
  aNode[n].WO = aNode[last_merged_node].WO;

  if (aNode[n].has_conv) {
    aNode[n].hlsinf_kh_conv = aNode[conv_layer].kh;
    aNode[n].hlsinf_kw_conv = aNode[conv_layer].kw;
    aNode[n].hlsinf_sh_conv = aNode[conv_layer].sh;
    aNode[n].hlsinf_sw_conv = aNode[conv_layer].sw;
    aNode[n].hlsinf_pt_conv = aNode[conv_layer].pt;
    aNode[n].hlsinf_pb_conv = aNode[conv_layer].pb;
    aNode[n].hlsinf_pr_conv = aNode[conv_layer].pr;
    aNode[n].hlsinf_pl_conv = aNode[conv_layer].pl;
  }
  if (aNode[n].has_maxpool) {
    aNode[n].hlsinf_kh_maxpool = aNode[maxpool_layer].kh;
    aNode[n].hlsinf_kw_maxpool = aNode[maxpool_layer].kw;
    aNode[n].hlsinf_sh_maxpool = aNode[maxpool_layer].sh;
    aNode[n].hlsinf_sw_maxpool = aNode[maxpool_layer].sw;
  }

  // we change the output model name if needed
  int o = get_model_output_id(aNode[last_merged_node].name);
  if (o!=-1) fn_change_output_model_name(o, aNode[n].name);

  // finally, we remove the merged nodes from the list
  fn_remove_node(n0);
  fn_remove_node(n1);
  fn_remove_node(n2);
  fn_remove_node(n3);
  fn_remove_node(n4);

  //char filename[100];
  //sprintf(filename, "file%0d.fig", step);
  //step++;
  //fn_set_columns();
  //fn_draw_model(filename);
}

void fn_check_supported() {
  int list[100];

  if (verbose) printf("  checking supported nodes...\n");

  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      aNode[n].supported = false;
      // RELU is fully supported
      if (!strcmp(aNode[n].type, "Relu")) aNode[n].supported = true;
      // Conv layers are supported for filter size of 3x3 and with no dilations and single-groups (g==1)
      if ((!strcmp(aNode[n].type, "Conv")) && (aNode[n].kh==3) && (aNode[n].kw==3) && (aNode[n].dh==1) && (aNode[n].dw==1) && (aNode[n].g==1)) aNode[n].supported = true;
      // Batch normalization layer is fully supported
      if (!strcmp(aNode[n].type, "BatchNormalization")) aNode[n].supported = true;
      // MaxPool layer is supported for kernel size 2x2, with stride of 2x2 and with no padding
      if ((!strcmp(aNode[n].type, "MaxPooling")) && (aNode[n].kh==2) && (aNode[n].kw==2) && (aNode[n].sh==2) && (aNode[n].sw==2) && 
	  (aNode[n].pt == 0) && (aNode[n].pb == 0) && (aNode[n].pl==0) && (aNode[n].pr==0)) aNode[n].supported = true;
      // Add layer is supported with two parents
      if ((!strcmp(aNode[n].type, "Add")) && (fn_get_parent_nodes(aNode[n].name, list)==2)) aNode[n].supported = true;

      if (verbose) printf("    node %d (%s) supported: %s\n", n, aNode[n].name, aNode[n].supported?"yes":"no");
    }
  }
  if (verbose) printf("  completed\n");
}

/*
 * fn_adapt_initializers()
 *
 * This functions adapts initializers to the HLSinf storage format
 *
 * For weights: O x I x KH x KW is converted into GO x GI x CPO x CPI x KH x KW
 *
 */
void fn_adapt_initializers() {

  if (verbose) printf("  adapting initializers...\n");

  // we search for HLSinf nodes
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      if (is_hlsinf(n)) {
        if (aNode[n].has_conv) {	      
	  if (verbose) printf("    node %d (hlsinf), adapting weights...\n", n);
          // we adapt weights, first we get the parameters I, O, kh, kw
	  int ii = get_weight_initializer_entry_from_node(n);
	  if (ii==-1) {printf("Error, weight initializer not found\n"); exit(1);}

	  if (verbose) printf("    weight initializer: %s (%d)\n", aInitializer[ii].name, ii);

	  int I  = aNode[n].I;
	  int KH = aNode[n].hlsinf_kh_conv;
	  int KW = aNode[n].hlsinf_kw_conv;
	  int O  = aNode[n].O;

	  if (verbose) printf("    I %d kh %d kw %d O %d\n", I, KH, KW, O);

	  int num_items = 1;
	  for (int d=0; d<aInitializer[ii].num_dimensions; d++) num_items = num_items * aInitializer[ii].dimensions[d];
	  float* p = (float*)malloc(num_items * sizeof(float));

	  if (verbose) printf("    number of items: %d\n", num_items);

	  int GI = I / CPI;
	  int GO = O / CPO;
	  for (int o=0; o<O; o++) {
  	    for (int i=0; i<I; i++) {
	      for (int kh=0; kh<KH; kh++) {
	        for (int kw=0; kw<KW; kw++) {
		  size_t addr_in = (o * I * KH * KW) + (i * KH * KW) + (kh * KW) + kw;
   	          float item = aInitializer[ii].data[addr_in];
		  int gi = i / CPI;
		  int cpi = i % CPI;
		  int go = o / CPO;
		  int cpo = o % CPO;
		  size_t addr_out = (go * GI * CPO * CPI * KH * KW) + (gi * CPO * CPI * KH * KW) + (cpo * CPI * KH * KW) + (cpi * KH * KW) + (kh * KW) + kw;
		  p[addr_out] = item;
		}
	      }
	    }
	  }
	  if (verbose) printf("    new memory allocated and initialized\n");

	  free(aInitializer[ii].data);
	  aInitializer[ii].data = p;
        }
	
	if (aNode[n].has_bn) {
	  // we merge gamma, beta, runing_mean, runing_var into a single initializer
	  if (verbose) printf("    node %d (hlsinf), merging bn initializers...\n", n);

	  int i0 = get_gamma_initializer_id_from_node(n);
	  int i1 = get_beta_initializer_id_from_node(n);
	  int i2 = get_running_mean_initializer_id_from_node(n);
	  int i3 = get_running_var_initializer_id_from_node(n);
	  // checks: initializers must exist and must have a single dimension
	  if ((i0 == -1) || (i1 == -1) || (i2 == -1) || (i3 == -1)) {printf("Error, initializers not found for bn node\n"); exit(1);}
	  if ((aInitializer[i0].num_dimensions != 1) || (aInitializer[i1].num_dimensions != 1) || (aInitializer[i2].num_dimensions != 1) || (aInitializer[i3].num_dimensions != 1)) {
	    printf("Error, bn node initializers must have a single dimension\n"); exit(1);
	  }

	  if (verbose) printf("    initializers found: %d %d %d %d\n", i0, i1, i2, i3);

	  int num_items = aInitializer[i0].dimensions[0];

          if (verbose) printf("    number of items: %d\n", num_items);
	  float *p = (float*)malloc(sizeof(float) * num_items * 4);
	  for (int d=0; d<num_items; d++) {
	    p[(d*4)    ] = aInitializer[i0].data[d];
	    p[(d*4) + 1] = aInitializer[i1].data[d];
	    p[(d*4) + 2] = aInitializer[i2].data[d];
	    p[(d*4) + 3] = aInitializer[i3].data[d];
	  }

	  // we remove the four inputs to initializers from the node
	  remove_input_from_node(n, aInitializer[i0].name);
	  remove_input_from_node(n, aInitializer[i1].name);
	  remove_input_from_node(n, aInitializer[i2].name);
	  remove_input_from_node(n, aInitializer[i3].name);
	  // we remove the initializers
	  remove_initializer(i0);
	  remove_initializer(i1);
	  remove_initializer(i2);
	  remove_initializer(i3);
	  // now we add a new initializer
	  char name[100];
	  sprintf(name, "bn_hlsinf_initializer_%0d", num_initializers);
	  fn_add_new_initializer(name, (char*)"bn", num_items*4, p);
	  // now we add an input to the node to the initializer
	  add_input_to_node(n, name);
        }
      }
    }
  }

  if (verbose) printf("    completed\n");
}

/*
 * fn_adapt_conv1x1_to_conv3x3
 *
 * This function adapts all conv1x1 nodes into conv3x3
 * Eligible nodes must have dilations equal to one and group set to one
 * When adapted, the new convolution will have two extra rows at the bottom
 * and two extra columns at the right with padding
 */
void fn_adapt_conv1x1_to_conv3x3() {
  if (verbose) printf("  adapting conv1x1 -> conv3x3\n");
  // we search all the nodes looking for possible conv 1x1 nodes
  for (int n=0; n<num_nodes; n++) {
    if (aNode[n].valid) {
      if (is_conv(n)) {
        if ((aNode[n].kh==1) && (aNode[n].kw==1) && (aNode[n].dh==1) && (aNode[n].dw==1) && (aNode[n].g==1)) {
          if (verbose) printf("    found node %d (%s)\n", n, aNode[n].name);
	  // we change the filter geometry first
	  aNode[n].kh = 3;
	  aNode[n].kw = 3;
	  // we add two extra rows and columns to padding (bottom and right)
	  aNode[n].pb = aNode[n].pb + 2;
	  aNode[n].pr = aNode[n].pr + 2;

	  // now the weight initializer is adapted accordingly
	  fn_pad_weight_initializer_1x1_to_3x3(get_weight_initializer_name_from_node(n));
	}
      }
    }
  }
  if (verbose) printf("  completed\n");
}

/*
 * fn_find_and_merge()
 *
 * Finds and merges a pattern of nodes passed as arguments (five names)
 *
 * As an additional argument the name of the new node out of the merge is passed
 *
 */
void fn_find_and_merge(char *name0, char *name1, char *name2, char *name3, char *name4, char *keyword) {
  int n0, n1, n2, n3, n4;
  if (verbose) printf("  merging %s-%s-%s-%s-%s pattern\n", name0, name1, name2, name3, name4);
  for (int n=0; n<num_nodes; n++) {
    n0 = -1; n1 = -1; n2 = -1; n3 = -1; n4 = -1;
    if (fn_detect_pattern(n, name0, name1, name2, name3, name4, &n0, &n1, &n2, &n3, &n4)) {
      if (verbose) printf("    merging nodes %d %d %d %d %d\n", n0, n1, n2, n3, n4);
      fn_merge_nodes(n0, n1, n2, n3, n4, keyword);
    }
  }
  if (verbose) printf("    completed\n");
}

void fn_generate_output_model() {

  if (verbose) printf("generating output model...\n");

  // we adapt convs1x1 to 3x3
  if (adapt_1x1_to_3x3) fn_adapt_conv1x1_to_conv3x3();

  // first we check whether the layers can be supported in HLSinf 1.0
  fn_check_supported();

  if (cbar_keyword)  fn_find_and_merge((char*)"Conv", (char*)"BatchNormalization", (char*)"Add",  (char*)"Relu", NULL, (char*)"cbar");	
  if (cbr_keyword)   fn_find_and_merge((char*)"Conv", (char*)"BatchNormalization", (char*)"Relu", NULL,          NULL, (char*)"cbr");
  if (cb_keyword)    fn_find_and_merge((char*)"Conv", (char*)"BatchNormalization", NULL,          NULL,          NULL, (char*)"cb");
  if (c_keyword)     fn_find_and_merge((char*)"Conv", NULL, NULL, NULL, NULL, (char*)"c");

  // now we adapt the initializers accordingly
  fn_adapt_initializers();

  if (verbose) printf("  completed\n");
}
