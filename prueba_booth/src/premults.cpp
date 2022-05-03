
#include "top.h"

//first_swap function:
template<int num_bits> int first_swap(ap_uint<num_bits> *input, ap_uint<num_bits> *output) {
#pragma hls inline off
	if(input[0] == 0) {
		output[0] = input[1];
		output[1] = input[0];
		return 1;
	} else {
		output[0] = input[0];
		output[1] = input[1];
		return 0;
	}
}

template<int num_bits> void last_swap(int swap, ap_uint<2*num_bits> *input, ap_uint<2*num_bits> *output) {
#pragma hls inline off
	if(swap) {
		output[0] = input[1];
		output[1] = input[0];
	} else {
		output[0] = input[0];
		output[1] = input[1];
	}
}

// read filters
template<int o, int data_bits> void read_w(ap_uint<data_bits*o> *ptr, hls::stream<ap_uint<data_bits*o>> &out) {
#pragma hls inline off
#ifdef DEBUG
	printf("READ_W: begin\n");
#endif
  for (int p=0; p<KH * KW; p++) {
	ap_uint<data_bits*o> px = ptr[p];
    out << px;
#ifdef DEBUG
    printf("  read: ");
    for (int x=0; x<o; x++) {
    	int first = x * data_bits;
    	int last = first + data_bits - 1;
    	ap_uint<data_bits> pd = px.range(last, first);
    	printf("%d ", int(pd));
    }
    printf("\n");
#endif
  }
#ifdef DEBUG
	printf("READ_W: end\n");
#endif
}

//read_w<I * O, DATA_SIZE>(w, st_read_w);
// read x (data)
template<int items_per_read, int data_bits> void read_x(ap_uint<items_per_read * data_bits> *ptr, hls::stream<ap_uint<items_per_read * data_bits>> &out) {
#pragma hls inline off
#ifdef DEBUG
	printf("READ_X: begin\n");
#endif
  for (int x=0; x<KH*KW; x++) {
	for (int p=0; p<H*W / items_per_read; p++) {
      #pragma hls pipeline ii=1
	  ap_uint<items_per_read * data_bits> px = ptr[p];
	  out << px;
#ifdef DEBUG
	  printf("  read: ");
	  for (int d=0; d<items_per_read; d++) {
		  int first = d * data_bits;
		  int last = first + data_bits - 1;
		  ap_uint<data_bits> pd = px.range(last, first);
		  printf("%d ", int(pd));
	  }
	  printf("\n");
#endif
	}
  }
#ifdef DEBUG
	printf("READ_X: end\n");
#endif
}

//split<O, I * O, DATA_SIZE, KH * KW>(st_read_w, st_w1);
// split, splits the input stream in a set of output streams
template <int num_outs, int items_per_read, int data_bits, int num_its> void split(hls::stream<ap_uint<items_per_read * data_bits>> &in, hls::stream<ap_uint<items_per_read * data_bits / num_outs>> out[num_outs]) {
#pragma hls inline off
  for (int i=0; i<num_its; i++) {
    #pragma hls pipeline ii=1
    ap_uint<items_per_read * data_bits> px_in = in.read();
    for (int o=0; o<num_outs; o++) {
      #pragma hls unroll
      int first = o*(items_per_read/num_outs)*data_bits;
	  int last = ((o+1)*(items_per_read/num_outs)*data_bits)-1;
	  ap_uint<items_per_read*data_bits/num_outs> px_out;
	  px_out = px_in.range(last, first);
 	  out[o] << px_out;
	}
  }
}

//  broadcast<I, O, READ_GROUP_SIZE, DATA_SIZE, KH * KH * H * W / READ_GROUP_SIZE>(st_read_x, st_x1);
// broadcast
template <int num_ins, int num_outs, int items_per_read, int data_bits, int num_its> void broadcast(hls::stream<ap_uint<data_bits*items_per_read>> in[num_ins], hls::stream<ap_uint<data_bits*items_per_read*num_ins>> out[num_outs]) {
#pragma hls inline off
  for (int n=0; n<num_its; n++) {
    #pragma hls pipeline ii=1
	ap_uint<data_bits*items_per_read*num_ins> px_out;
	bradc_read_loop:
	for (int i=0; i<num_ins; i++) {
      #pragma hls unroll
	  int first = i * (data_bits*items_per_read);
	  int last = first + (data_bits*items_per_read) - 1;
      ap_uint<data_bits*items_per_read> px_in = in[i].read();
	  px_out.range(last, first) = px_in;
	}
	bradc_write_loop:
    for (int o=0; o<num_outs; o++) {
      #pragma hls unroll
 	  out[o] << px_out;
	}
  }
}

template <int num_ins, int items_per_read, int data_bits, int num_its> void join(hls::stream<ap_uint<data_bits*items_per_read>> in[num_ins], hls::stream<ap_uint<data_bits*items_per_read*num_ins>> &out) {
#pragma hls inline off
  for (int i=0; i<num_its; i++) {
    #pragma hls pipeline ii=1
	ap_uint<num_ins*data_bits*items_per_read> px_in_joined;
	for (int p=0; p<num_ins; p++) {
      #pragma hls unroll
      int first = p * data_bits * items_per_read;
      int last = first + (data_bits * items_per_read) - 1;
      ap_uint<items_per_read * data_bits> px_in = in[p].read();
      px_in_joined.range(last, first) = px_in;
	}
    out << px_in_joined;
  }
}

template<int items_per_read, int num_bits, int num_its> void serialize(hls::stream<ap_uint<items_per_read*num_bits>> &in, hls::stream<ap_uint<512>> &out) {
#pragma hls inline off

  if (items_per_read*num_bits >= 512) {
    for (int i=0; i<num_its; i++) {
      #pragma hls pipeline ii=2
      ap_uint<items_per_read*num_bits> px_in = in.read();
      for (int i=0; i<items_per_read*num_bits; i=i+512) {
        out << px_in.range(i+511, i);
	  }
    }
  } else {
    ap_uint<512> px_out;
    int first_bit = 0;
    for (int i=0; i<num_its; i++) {
      #pragma hls pipeline ii=1
      ap_uint<items_per_read*num_bits> px_in = in.read();
      int first = first_bit;
      int last = first + (items_per_read * num_bits) - 1;
      px_out.range(last, first) = px_in;
      first_bit += (items_per_read * num_bits);
      if (first_bit == 512) {
    	  first_bit = 0;
    	  out << px_out;
      }
    }
  }
}
#ifdef ZeroTest
//for o
//    mul<I,                         GROUP_SIZE,       DATA_SIZE,       KH*KW,      H*W/READ_GROUP_SIZE>(st_w1[o], st_x1[o], st_mul[o]);
template<int num_input_channels, int items_per_read, int num_bits, int num_its1, int num_its2> void mul(hls::stream<ap_uint<num_bits*num_input_channels>> &in_w, hls::stream<ap_uint<num_bits*items_per_read*num_input_channels>> &in_x, hls::stream<ap_uint<items_per_read*2*num_bits>> &out) {
#pragma hls inline off

	int niterations = 0;
  num_its1_loop:
  for (int p=0; p<num_its1; p++) { //num_its1 = KW*KH
	ap_uint<num_bits> w[num_input_channels];

	//one read of I elements of W vector each cycle
	ap_uint<num_bits*num_input_channels> px_w = in_w.read();
	num_input_channels_loop:
	for (int i=0; i<num_input_channels; i++) {
      #pragma hls unroll
	  int first = i*num_bits;
	  int last = first + num_bits - 1;
	  w[i] = px_w.range(last, first); //Split each I element in different arrays
	}

	ap_uint<num_bits*2*items_per_read> px_out;
	ap_uint<num_bits*items_per_read*num_input_channels> px_in;
	ap_uint<num_bits> x[2], xs[2];
	ap_uint<2*num_bits> ys[2], yf[2];
	ap_uint<num_bits> op1;
	ap_uint<2*num_bits> y;
	#pragma HLS BIND_OP variable=y op=mul impl=dsp

	int free_slots = 2;
	int end = 0;
	int num_reads = 0;
	int swap = 0;
	int a_first_out = 0;
	int a_last_out  = (2*num_bits)-1;
	int b_first_out = (2*num_bits);
	int b_last_out  = a_last_out + (2*num_bits);

	int a_first_in  = 0;
	int a_last_in   = num_bits - 1;
	int b_first_in  = a_first_in  + num_bits ;
	int b_last_in   = a_last_in + num_bits;

	//for performance
	mult_loop:
	while(!end) {
		#pragma HLS pipeline ii=1
		#pragma HLS loop_tripcount min=num_its2/2 max=num_its2
		if (free_slots == 2) {
			px_in  = in_x.read();
			x[0] = px_in.range(a_last_in, a_first_in);
			x[1] = px_in.range(b_last_in, b_first_in);
			swap = first_swap<num_bits>(x, xs);
			free_slots = 0;
			num_reads++;
		}

		if (free_slots == 0)op1 = xs[0];
		else op1=xs[1];

		y = op1 * w[0];
		niterations ++;
		if(free_slots == 0) ys[0] = y;
		else ys[1] = y;

		if(free_slots == 0 && xs[1] == 0) {
			free_slots = 2;
			ys[1] = 0;
		}
		else {
			free_slots ++;
		}

		if (free_slots == 2) {
			last_swap<num_bits>(swap, ys, yf);
			px_out.range(a_last_out, a_first_out) = yf[0];
			px_out.range(b_last_out, b_first_out) = yf[1];
		    out << px_out;
		}
		end = ((num_reads == num_its2) && (free_slots==2));
	}
  }
	//std::cout << "N. of reads:      "<< num_its2*num_its1<<"\n";
	//std::cout << "N. of iterations: "<< niterations << "\n";
}

#else
//for o
//    mul<I, READ_GROUP_SIZE, DATA_SIZE, KH*KW, H*W/READ_GROUP_SIZE>(st_w1[o], st_x1[o], st_mul[o]);
template<int num_input_channels, int items_per_read, int num_bits, int num_its1, int num_its2> void mul(hls::stream<ap_uint<num_bits*num_input_channels>> &in_w, hls::stream<ap_uint<num_bits*items_per_read*num_input_channels>> &in_x, hls::stream<ap_uint<items_per_read*2*num_bits>> &out) {
#pragma hls inline off
  num_its1_loop:
  for (int p=0; p<num_its1; p++) { //num_its1 = KW*KH
	ap_uint<num_bits> w[num_input_channels];

	//one read of I elements of W vector each cycle
	ap_uint<num_bits*num_input_channels> px_w = in_w.read();
	num_input_channels_loop:
	for (int i=0; i<num_input_channels; i++) {
      #pragma hls unroll
	  int first = i*num_bits;
	  int last = first + num_bits - 1;
	  w[i] = px_w.range(last, first); //Split each I element in different arrays
	}
	num_its2_loop:
	for (int x=0; x < num_its2; x++) { //num_its = H*W/READ_GROUP_SIZE
      #pragma HLS pipeline ii=1
	  //one read of READ_GROUP_SIZE*I elements of X vector each cycle
	  ap_uint<num_bits*items_per_read*num_input_channels> px = in_x.read();
	  ap_uint<num_bits> pxs[items_per_read];
	  ap_uint<num_bits*2*items_per_read> px_out;

	  // paired muls in single DSP
	  //cada ciclo realiza READ_GROUP_SIZE * I * 2 mult
	  //se crea 2 elementos de salida que se sumaran en el sig modulo
	 // for (int z=0; z<items_per_read; z=z+2) { //items_per_read = READ_GROUP_SIZE
     //   #pragma hls unroll
	  int z = 0;
		ap_uint<num_bits*2> res1 = 0;
		ap_uint<num_bits*2> res2 = 0;
  	    for (int l=0; l<num_input_channels; l++) {
	      #pragma hls unroll
		  int first = (z * num_bits) + l * (items_per_read*num_bits);
		  int last = first + num_bits - 1;
		  pxs[z] = px.range(last, first);
		  pxs[z+1] = px.range(last + num_bits , first + num_bits);
		  //printf("in_a.range(%d, %d)\n", last, first); //in_a.range(7, 0)
		 // printf("in_b.range(%d, %d)\n", last + num_bits , first + num_bits); //in_b.range(15, 8)
		  ap_uint<num_bits> a = pxs[z];
		  ap_uint<num_bits> b = pxs[z+1];
		  ap_uint<num_bits> c = w[l];
		  ap_uint<2*num_bits> res1_t;
		  ap_uint<2*num_bits> res2_t;

#ifdef optDSP
		  // code for r1 = a * w and r2 = b * w using one DSP (mul) with ((a << 18) + b) * w
		  ap_uint<27> op1a = (ap_uint<27>)a << 18; //to
		  ap_uint<27> op1b = (ap_uint<27>)b;
		  ap_uint<18> op2 = c;
		  ap_uint<45> res = (op1a + op1b) * op2;
		  res1_t = res.range(15, 0);
		  res2_t = res.range(33, 18);
#else
#ifdef old
			ap_uint<num_bits*2> res_p[2];
#pragma HLS BIND_OP variable=res_p op=mul impl=dsp
		  int end = 2;
		  int init = 0;
		  if (a == 0) {
			  res_p[0]=0;
			  init++;
		  }
		  if (b == 0) {
			  res_p[1]=0;
			  end--;
		  }
			ap_uint<num_bits*2> y;

		  for(int j = init; j < end; j++) {
			#pragma HLS pipeline ii=1
			  res_p[j] = pxs[z+j]*c;
		  }
		  res1_t = res_p[1];
		  res2_t = res_p[0];
#else
		  #pragma HLS BIND_OP variable=res1_t op=mul impl=dsp
		  #pragma HLS BIND_OP variable=res2_t op=mul impl=dsp
		  res1_t = b * c;
		  res2_t = a * c;
#endif
#endif


		  //res1 += res1_t;
		  //res2 += res2_t;
		  res1 = res1_t;
		  res2 = res2_t;
		  //printf("res a %d res b %d\n", (int) res2, (int) res1);
	    //}
		int first_out = z * 2 * num_bits;
		int last_out = first_out  + (2*num_bits)-1;

		//printf("out_a.range(%d, %d)\n", last_out, first_out);//out_a.range(15, 0)
		//printf("out_b.range(%d, %d)\n", last_out + (2*num_bits), first_out + (2*num_bits));//out_b.range(31, 16)
		px_out.range(last_out, first_out) = res2;
		px_out.range(last_out + (2*num_bits), first_out + (2*num_bits)) = res1;
	  }
      out << px_out;
	}
  }
}
#endif

//for o
//       add<READ_GROUP_SIZE, 2*DATA_SIZE , KH*KW,       H*W/READ_GROUP_SIZE>(st_mul[o], st_add[o]);
template<int items_per_read, int data_bits, int num_its1, int num_its2>void add(hls::stream<ap_uint<items_per_read*data_bits>> &in, hls::stream<ap_uint<items_per_read*data_bits>> &out) {
#pragma hls inline off
  //printf("add starts\n");


  ap_uint<items_per_read*data_bits> buff[H*W/items_per_read];
  #pragma hls aggregate variable=buff
  #pragma hls bind_storage variable=buff type=ram_t2p impl=uram

  for (int x=0; x<H*W/items_per_read; x++) buff[x] = 0;

  for (int p=0; p<num_its1; p++) {
    for (int x=0; x<num_its2; x++) {
      #pragma hls pipeline ii=1
      ap_uint<items_per_read*data_bits> px_in;
      ap_uint<items_per_read*data_bits> buff_in = buff[x];
      #pragma hls array_partition variable=px_in complete
      px_in = in.read();
      for (int z=0; z<items_per_read; z++) {
        #pragma hls unroll
    	int first = z * data_bits;
    	int last = first + data_bits-1;
    	ap_uint<data_bits> v = px_in.range(last, first);
    	ap_uint<data_bits> v_buff = buff_in.range(last, first);
    	v += v_buff;
    	buff_in.range(last, first) = v;
      }
      buff[x] = buff_in;
    }
  }

  for (int x=0; x<num_its2; x++) {
    #pragma hls pipeline ii=1
    out << buff[x];
  }
}

//  merge<O, 		  READ_GROUP_SIZE,    2*DATA_SIZE,     H*W/READ_GROUP_SIZE>(st_add, st_merge);
template<int num_ins, int items_per_read, int num_bits, int num_its>void merge(hls::stream<ap_uint<items_per_read*num_bits>> in[num_ins], hls::stream<ap_uint<items_per_read*num_bits>> &out) {
#pragma hls pipeline off
  for (int i=0; i<num_ins; i++) {
    for (int it=0; it<num_its; it++) {
      #pragma hls pipeline ii=1
	  ap_uint<items_per_read*num_bits> px;
	  px = in[i].read();
	  out << px;
	}
  }
}

template <int num_its> void write(ap_uint<512> *ptr, hls::stream<ap_uint<512>> &in) {
#pragma hls inline off
  //printf("write starts\n");
  int addr = 0;
  for (int x=0; x<num_its; x++) {
    #pragma hls pipeline ii=1
    ap_uint<512> px_in = in.read();
    ptr[addr] = px_in;
    addr++;
  }
  //printf("write ends\n");
}

void top(ap_uint<READ_GROUP_SIZE*DATA_SIZE> *x0, ap_uint<READ_GROUP_SIZE*DATA_SIZE> *x1, ap_uint<READ_GROUP_SIZE*DATA_SIZE> *x2, ap_uint<READ_GROUP_SIZE*DATA_SIZE> *x3, ap_uint<I*O*DATA_SIZE> *w, ap_uint<512> *y) {
  #pragma hls interface m_axi port=w offset=slave bundle=gmem
  #pragma hls interface m_axi port=y offset=slave bundle=gmem1
  #pragma hls interface m_axi port=x0 offset=slave bundle=gmem2
  #pragma hls interface m_axi port=x1 offset=slave bundle=gmem3
  #pragma hls interface m_axi port=x2 offset=slave bundle=gmem4
  #pragma hls interface m_axi port=x3 offset=slave bundle=gmem5

  #pragma hls dataflow
  hls::stream<ap_uint<2*DATA_SIZE*READ_GROUP_SIZE>> st_mul[O];
  hls::stream<ap_uint<READ_GROUP_SIZE*2*DATA_SIZE>> st_add[O];

  hls::stream<ap_uint<512>>  st_recombine_o;

  // filters read and split
  hls::stream<ap_uint<I*O*DATA_SIZE>>  st_read_w;
  //hls::stream<ap_uint<DATA_SIZE>>    st_w[O];
  hls::stream<ap_uint<DATA_SIZE*I>>  st_w1[O];
  read_w<I * O, DATA_SIZE>(w, st_read_w);
  split<O, I * O, DATA_SIZE, KH * KW>(st_read_w, st_w1);

  //data read and broadcast
  hls::stream<ap_uint<DATA_SIZE*READ_GROUP_SIZE>> st_read_x[I];
  hls::stream<ap_uint<DATA_SIZE*READ_GROUP_SIZE*I>> st_x1[O];
  if (I>=1) read_x<READ_GROUP_SIZE, DATA_SIZE>(x0, st_read_x[0]);
  if (I>=2) read_x<READ_GROUP_SIZE, DATA_SIZE>(x1, st_read_x[1]);
  if (I>=3) read_x<READ_GROUP_SIZE, DATA_SIZE>(x2, st_read_x[2]);
  if (I>=4) read_x<READ_GROUP_SIZE, DATA_SIZE>(x3, st_read_x[3]);

  broadcast<I, O, READ_GROUP_SIZE, DATA_SIZE, KH * KH * H * W / READ_GROUP_SIZE>(st_read_x, st_x1);

  for (int o=0; o<O; o++) {
    #pragma hls unroll
    mul<I, READ_GROUP_SIZE, DATA_SIZE, KH*KW, H*W/READ_GROUP_SIZE>(st_w1[o], st_x1[o], st_mul[o]);
    add<READ_GROUP_SIZE, 2*DATA_SIZE, KH*KW, H*W/READ_GROUP_SIZE>(st_mul[o], st_add[o]);
  }
  hls::stream<ap_uint<READ_GROUP_SIZE*2*DATA_SIZE>> st_merge;
  merge<O, READ_GROUP_SIZE, 2*DATA_SIZE, H*W/READ_GROUP_SIZE>(st_add, st_merge);
  hls::stream<ap_uint<512>> st_serialize;
  serialize<READ_GROUP_SIZE, 2*DATA_SIZE, O*H*W/READ_GROUP_SIZE>(st_merge, st_serialize);
  write<O*H*W*2*DATA_SIZE/512>(y, st_serialize);
}
