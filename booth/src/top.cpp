#include "top.h"

void read_vector(xw_read_st *ptr, hls::stream<xw_vector_st> &out) {

  ap_uint<VECTOR_SIZE * OUTPUT_CHANNELS * 8> tmp;
  xw_read_st px_in;
  xw_vector_st px_out;
  #pragma hls aggregate variable=px_in

  int addr = 0;
  read_num_vectors_loop:
  for (int n=0; n<NUM_VECTORS; n++) {
	read_vector_blocks_loop:
    for (int r=0; r<VECTOR_SIZE / NUM_READ_PIXELS; r++) {
      #pragma HLS pipeline II=1
	  px_in = ptr[addr];
	  addr++;
	  int first = r * NUM_READ_PIXELS * OUTPUT_CHANNELS * 8;
	  int last = first + (NUM_READ_PIXELS * OUTPUT_CHANNELS * 8) - 1;
	  tmp.range(last, first) = px_in;
    }
	px_out = *(xw_vector_st *)(&tmp);


    #ifdef DEBUG
	printf("read_vector: vector %d, data read:\n", n);
	for (int c=0; c<OUTPUT_CHANNELS; c++) {
	  printf("  channel %d:", c);
	  for (int x=0; x<VECTOR_SIZE; x++) {
		  printf("%d ", int(px_out.pixel[x].pixel[c]));
	  }
	  printf("\n");
	}
    #endif


	out << px_out;
  }
}

void agrupate_vector(hls::stream<xw_vector_st> &in, hls::stream<xw_vector_st> &out) {

	xw_vector_st px_in;
	xw_vector_st px_out[NUM_VECTORS];

	for (int v=0; v<NUM_VECTORS; v++) {
      #pragma hls pipeline ii=1
	  px_out[v] = in.read();
	}

	for (int v=0; v<NUM_VECTORS; v++) out << px_out[v];
}

void write_vector(ap_int<32> *ptr, hls::stream<y_st> &in) {

  int addr = 0;
  int v = 0;
  int c = 0;
  y_st px;
  #pragma hls array_partition variable=px complete

  for (int v=0; v<NUM_VECTORS * OUTPUT_CHANNELS; v++) {
    #pragma hls pipeline ii=1
    if (c == 0) px = in.read();
    ptr[addr] = px.pixel[c];
    c = (c + 1) % OUTPUT_CHANNELS;
	addr++;
  }
}

#ifdef USE_MULTIPLIERS
void mul_vector(hls::stream<xw_vector_st> &x_in, hls::stream<xw_vector_st> &w_in, hls::stream<y_st> &out) {


  for (int v=0; v<NUM_VECTORS; v++) {
    xw_vector_st x = x_in.read();
    xw_vector_st w = w_in.read();

    y_st sum;
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
      #pragma hls unroll
	  sum.pixel[c] = 0;
    }

    for (int c=0; c<OUTPUT_CHANNELS; c++) {
      #pragma hls unroll

  	  for (int n=0; n<VECTOR_SIZE; n++) {
        #pragma hls unroll
	    sum.pixel[c] += x.pixel[n].pixel[c] * w.pixel[n].pixel[c];
	  }
    }

    out << sum;
  }

}
#endif

#ifdef USE_BOOTH

ap_int<3> booth(ap_uint<3> bits) {
#pragma HLS inline off
  ap_int<3> result;

#ifdef OPT_BOOTH
  ap_uint<1> bit_0 = bits.range(0, 0);
  ap_uint<1> bit_1 = bits.range(1, 1);
  ap_uint<1> bit_2 = bits.range(2, 2);

  ap_uint<1> result_0 = bit_0 ^ bit_1;
  ap_uint<1> result_1 = (bit_2 & ~bit_0) | (bit_2 & ~bit_1) | (~bit_2 & bit_1 & bit_0);
  ap_uint<1> result_2 = bit_2 & ~(bit_1 & bit_0);

  result.range(0, 0) = result_0;
  result.range(1, 1) = result_1;
  result.range(2, 2) = result_2;

  #ifdef∫ DEBUG
  printf("booth(%d): bits: %d %d %d, result_bits: %d %d %d, result_final %d\n", int(bits), int(bit_2), int(bit_1), int(bit_0), int(result_2), int(result_1), int(result_0), int(result));
  #endif

#else

  switch (bits) {
  	case 0: result = 0; break;
  	case 1: result = 1; break;
  	case 2: result = 1; break;
  	case 3: result = 2; break;
  	case 4: result = -2; break;
  	case 5: result = -1; break;
  	case 6: result = -1; break;
  	case 7: result = 0; break;
  }

#endif

  return result;
}

ap_int<12> bin_to_booth_2(ap_int<8> vin) {
#pragma HLS inline off
  ap_int<12> vout;
  ap_uint<3> b0 = vin.range(1, 0) << 1;
  ap_uint<3> b2 = vin.range(3, 1);
  ap_uint<3> b4 = vin.range(5, 3);
  ap_uint<3> b6 = vin.range(7, 5);

  vout.range(2, 0) = booth(b0);
  vout.range(5, 3) = booth(b2);
  vout.range(8, 6) = booth(b4);
  vout.range(11, 9) = booth(b6);
  return vout;
}

xw_booth_st bin_to_booth(xw_st vin) {
#pragma HLS inline off
  xw_booth_st vout;
  for (int c=0; c<OUTPUT_CHANNELS; c++) {
    #pragma hls unroll
	vout.pixel[c] = bin_to_booth_2(vin.pixel[c]);
    #ifdef DEBUG
	ap_int<3> b0 = vout.pixel[c].range(2, 0);
	ap_int<3> b2 = vout.pixel[c].range(5, 3);
	ap_int<3> b4 = vout.pixel[c].range(8, 6);
	ap_int<3> b6 = vout.pixel[c].range(11, 9);
    printf("bin_to_booth: %d -> %d %d %d %d\n", int(vin.pixel[c]), int(b6), int(b4), int(b2), int (b0));
    #endif
  }
  return vout;
}

void generate_booth(hls::stream<xw_vector_st> &in, hls::stream<xw_vector_booth_st> &out) {
#pragma HLS inline off

  xw_vector_st px_in;
  xw_vector_booth_st px_out;

  for (int n=0; n<NUM_VECTORS; n++) {
    #pragma hls pipeline ii=1
	px_in = in.read();
	for (int x=0; x<VECTOR_SIZE; x++) {
      #pragma HLS unroll
	  px_out.pixel[x] = bin_to_booth(px_in.pixel[x]);
	}
	out << px_out;
  }
}

ap_int<ACC_BBITS> mul_booth_digit(ap_int<BBITS> a, ap_int<BBITS> b) {
#pragma HLS inline off

	ap_int<ACC_BBITS> result;
	result = a * b;
	return result;
}

ap_int<ACC_BBITS> add_booth_digit(ap_int<ACC_BBITS> a, ap_int<ACC_BBITS> b) {
#pragma HLS inline off

	ap_int<ACC_BBITS> result = a + b;
	return result;
}


ap_int<ACC_BBITS*7> mul_scalar_booth(ap_int<12> a, ap_int<12> b) {

#pragma HLS inline off

	ap_int<BBITS> a_0 = a.range((1*BBITS)-1, 0*BBITS);
	ap_int<BBITS> a_2 = a.range((2*BBITS)-1, 1*BBITS);
	ap_int<BBITS> a_4 = a.range((3*BBITS)-1, 2*BBITS);
	ap_int<BBITS> a_6 = a.range((4*BBITS)-1, 3*BBITS);
	ap_int<BBITS> b_0 = b.range((1*BBITS)-1, 0*BBITS);
	ap_int<BBITS> b_2 = b.range((2*BBITS)-1, 1*BBITS);
	ap_int<BBITS> b_4 = b.range((3*BBITS)-1, 2*BBITS);
	ap_int<BBITS> b_6 = b.range((4*BBITS)-1, 3*BBITS);


	// booth digit 0
	ap_int<ACC_BBITS> booth_0  = mul_booth_digit(a_0, b_0);

	// booth digit 2
	ap_int<ACC_BBITS> booth_2_a = mul_booth_digit(a_0, b_2);
	ap_int<ACC_BBITS> booth_2_b = mul_booth_digit(a_2, b_0);
	ap_int<ACC_BBITS> booth_2   = add_booth_digit(booth_2_a, booth_2_b);

	// booth digit 4
	ap_int<ACC_BBITS> booth_4_a = mul_booth_digit(a_0, b_4);
	ap_int<ACC_BBITS> booth_4_b = mul_booth_digit(a_2, b_2);
	ap_int<ACC_BBITS> booth_4_c = mul_booth_digit(a_4, b_0);
	ap_int<ACC_BBITS> booth_4_d = add_booth_digit(booth_4_a, booth_4_b);
	ap_int<ACC_BBITS> booth_4   = add_booth_digit(booth_4_c, booth_4_d);

	// booth digit 6
	ap_int<ACC_BBITS> booth_6_a = mul_booth_digit(a_0, b_6);
	ap_int<ACC_BBITS> booth_6_b = mul_booth_digit(a_2, b_4);
	ap_int<ACC_BBITS> booth_6_c = mul_booth_digit(a_4, b_2);
	ap_int<ACC_BBITS> booth_6_d = mul_booth_digit(a_6, b_0);
	ap_int<ACC_BBITS> booth_6_e = add_booth_digit(booth_6_a, booth_6_b);
	ap_int<ACC_BBITS> booth_6_f = add_booth_digit(booth_6_c, booth_6_d);
	ap_int<ACC_BBITS> booth_6   = add_booth_digit(booth_6_e, booth_6_f);

	// booth digit 8
	ap_int<ACC_BBITS> booth_8_a = mul_booth_digit(a_2, b_6);
	ap_int<ACC_BBITS> booth_8_b = mul_booth_digit(a_4, b_4);
	ap_int<ACC_BBITS> booth_8_c = mul_booth_digit(a_6, b_2);
	ap_int<ACC_BBITS> booth_8_d = add_booth_digit(booth_8_a, booth_8_b);
	ap_int<ACC_BBITS> booth_8   = add_booth_digit(booth_8_c, booth_8_d);

	// booth digit 10
	ap_int<ACC_BBITS> booth_10_a = mul_booth_digit(a_4, b_6);
	ap_int<ACC_BBITS> booth_10_b = mul_booth_digit(a_6, b_4);
	ap_int<ACC_BBITS> booth_10   = add_booth_digit(booth_10_a, booth_10_b);

	// booth digit 12
	ap_int<ACC_BBITS> booth_12   = mul_booth_digit(a_6, b_6);

	ap_uint<ACC_BBITS*7> result;
	result.range((1*ACC_BBITS)-1, (0*ACC_BBITS)) = booth_0;
	result.range((2*ACC_BBITS)-1, (1*ACC_BBITS)) = booth_2;
	result.range((3*ACC_BBITS)-1, (2*ACC_BBITS)) = booth_4;
	result.range((4*ACC_BBITS)-1, (3*ACC_BBITS)) = booth_6;
	result.range((5*ACC_BBITS)-1, (4*ACC_BBITS)) = booth_8;
	result.range((6*ACC_BBITS)-1, (5*ACC_BBITS)) = booth_10;
	result.range((7*ACC_BBITS)-1, (6*ACC_BBITS)) = booth_12;

    #ifdef DEBUG
	int op1 = (int(a_6) << 6) + (int(a_4) << 4) + (int(a_2) << 2) + int(a_0);
	int op2 = (int(b_6) << 6) + (int(b_4) << 4) + (int(b_2) << 2) + int(b_0);
	int r   = (int(booth_12) << 12) + (int(booth_10) << 10) + (int(booth_8) << 8) + (int(booth_6) << 6) + (int(booth_4) << 4) + (int(booth_2) << 2) + int(a_0);

    printf("mul_booth: %3d %3d %3d %3d x %3d %3d %3d %3d = %3d %3d %3d %3d %3d %3d %3d (%3d x %3d = %3d)\n",
    		    int(a_6), int(a_4), int(a_2), int(a_0), int(b_6), int(b_4), int(b_2), int(b_0),
    		    int(booth_12), int(booth_10), int(booth_8), int(booth_6), int(booth_4), int(booth_2), int(booth_0),
				op1, op2, r);
    #endif

	return result;
}

ap_int<ACC_BBITS*7> sum_scalar_booth(ap_int<ACC_BBITS*7> a, ap_int<ACC_BBITS*7> b) {
#pragma HLS inline off

	ap_int<ACC_BBITS> a_0   = a.range((ACC_BBITS*1)-1, ACC_BBITS*0);
	ap_int<ACC_BBITS> a_2   = a.range((ACC_BBITS*2)-1, ACC_BBITS*1);
	ap_int<ACC_BBITS> a_4   = a.range((ACC_BBITS*3)-1, ACC_BBITS*2);
	ap_int<ACC_BBITS> a_6   = a.range((ACC_BBITS*4)-1, ACC_BBITS*3);
	ap_int<ACC_BBITS> a_8   = a.range((ACC_BBITS*5)-1, ACC_BBITS*4);
	ap_int<ACC_BBITS> a_10  = a.range((ACC_BBITS*6)-1, ACC_BBITS*5);
	ap_int<ACC_BBITS> a_12  = a.range((ACC_BBITS*7)-1, ACC_BBITS*6);
	ap_int<ACC_BBITS> b_0   = b.range((ACC_BBITS*1)-1, ACC_BBITS*0);
	ap_int<ACC_BBITS> b_2   = b.range((ACC_BBITS*2)-1, ACC_BBITS*1);
	ap_int<ACC_BBITS> b_4   = b.range((ACC_BBITS*3)-1, ACC_BBITS*2);
	ap_int<ACC_BBITS> b_6   = b.range((ACC_BBITS*4)-1, ACC_BBITS*3);
	ap_int<ACC_BBITS> b_8   = b.range((ACC_BBITS*5)-1, ACC_BBITS*4);
	ap_int<ACC_BBITS> b_10  = b.range((ACC_BBITS*6)-1, ACC_BBITS*5);
	ap_int<ACC_BBITS> b_12  = b.range((ACC_BBITS*7)-1, ACC_BBITS*6);

	ap_int<ACC_BBITS> res_0  = a_0 + b_0;
	ap_int<ACC_BBITS> res_2  = a_2 + b_2;
	ap_int<ACC_BBITS> res_4  = a_4 + b_4;
	ap_int<ACC_BBITS> res_6  = a_6 + b_6;
	ap_int<ACC_BBITS> res_8  = a_8 + b_8;
	ap_int<ACC_BBITS> res_10  = a_10 + b_10;
	ap_int<ACC_BBITS> res_12  = a_12 + b_12;

#ifdef DEBUG
	printf("sum_booth: %3d %3d %3d %3d %3d %3d %3d + %3d %3d %3d %3d %3d %3d %3d = %3d %3d %3d %3d %3d %3d %3d\n",
			    int(a_12), int(a_10), int(a_8), int(a_6), int(a_4), int(a_2), int(a_0), int(b_12), int(b_10), int(b_8), int(b_6), int(b_4), int(b_2), int(b_0),
	            int(res_12), int(res_10), int(res_8), int(res_6), int(res_4), int(res_2), int(res_0));
#endif

	ap_uint<ACC_BBITS*7> result;
	result.range((1*ACC_BBITS)-1, (0*ACC_BBITS)) = res_0;
	result.range((2*ACC_BBITS)-1, (1*ACC_BBITS)) = res_2;
	result.range((3*ACC_BBITS)-1, (2*ACC_BBITS)) = res_4;
	result.range((4*ACC_BBITS)-1, (3*ACC_BBITS)) = res_6;
	result.range((5*ACC_BBITS)-1, (4*ACC_BBITS)) = res_8;
	result.range((6*ACC_BBITS)-1, (5*ACC_BBITS)) = res_10;
	result.range((7*ACC_BBITS)-1, (6*ACC_BBITS)) = res_12;

	return result;
}

void mul_vector_booth(hls::stream<xw_vector_booth_st> &x_in, hls::stream<xw_vector_booth_st> &w_in, hls::stream<y_booth_st> &out) {


  for (int v=0; v<NUM_VECTORS; v++) {
    xw_vector_booth_st x = x_in.read();
    xw_vector_booth_st w = w_in.read();

    y_booth_st sum;
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
      #pragma hls unroll
	  sum.pixel[c] = 0;
    }

    for (int c=0; c<OUTPUT_CHANNELS; c++) {
      #pragma hls unroll

  	  for (int n=0; n<VECTOR_SIZE; n++) {
        #pragma hls unroll
  		ap_int<ACC_BBITS*7> temp;
  		temp = mul_scalar_booth(x.pixel[n].pixel[c], w.pixel[n].pixel[c]);
  		sum.pixel[c] = sum_scalar_booth(sum.pixel[c], temp);
	  }
    }

    out << sum;
  }

}

ap_int<32> booth_to_bin(ap_int<ACC_BBITS*7> a) {
#pragma HLS inline off
	ap_int<ACC_BBITS> a_0   = a.range((ACC_BBITS*1)-1, ACC_BBITS*0);
	ap_int<ACC_BBITS> a_2   = a.range((ACC_BBITS*2)-1, ACC_BBITS*1);
	ap_int<ACC_BBITS> a_4   = a.range((ACC_BBITS*3)-1, ACC_BBITS*2);
	ap_int<ACC_BBITS> a_6   = a.range((ACC_BBITS*4)-1, ACC_BBITS*3);
	ap_int<ACC_BBITS> a_8   = a.range((ACC_BBITS*5)-1, ACC_BBITS*4);
	ap_int<ACC_BBITS> a_10  = a.range((ACC_BBITS*6)-1, ACC_BBITS*5);
	ap_int<ACC_BBITS> a_12  = a.range((ACC_BBITS*7)-1, ACC_BBITS*6);

	ap_int<32> a_0_ext = a_0;
	ap_int<32> a_2_ext = a_2;
	ap_int<32> a_4_ext = a_4;
	ap_int<32> a_6_ext = a_6;
	ap_int<32> a_8_ext = a_8;
	ap_int<32> a_10_ext = a_10;
	ap_int<32> a_12_ext = a_12;

	ap_int<32> result = (a_12_ext << 12) + (a_10_ext << 10) + (a_8_ext << 8) + (a_6_ext << 6) + (a_4_ext << 4) + (a_2_ext << 2) + a_0_ext;

#ifdef DEBUG
	printf("booth_bin: %3d %3d %3d %3d %3d %3d %3d -> %10d\n", int(a_12), int(a_10), int(a_8), int(a_6), int(a_4), int(a_2), int(a_0), int(result));
#endif

	return result;
}

void generate_bin(hls::stream<y_booth_st> &in, hls::stream<y_st> &out) {
  y_booth_st px_in;
  y_st px_out;

  for (int v=0; v<NUM_VECTORS; v++) {
    #pragma hls pipeline ii=1
    px_in = in.read();
    for (int c=0; c<OUTPUT_CHANNELS; c++) {
      #pragma hls unroll
      px_out.pixel[c] = booth_to_bin(px_in.pixel[c]);
    }
    out << px_out;
  }
}

#endif

#ifdef pepito



ap_uint<ACC_BBITS*7> mul_booth(ap_uint<BBITS*4> a, ap_uint<BBITS*4> b) {
#pragma HLS inline off

	ap_int<BBITS> a_0 = a.range((1*BBITS)-1, 0*BBITS);
	ap_int<BBITS> a_2 = a.range((2*BBITS)-1, 1*BBITS);
	ap_int<BBITS> a_4 = a.range((3*BBITS)-1, 2*BBITS);
	ap_int<BBITS> a_6 = a.range((4*BBITS)-1, 3*BBITS);
	ap_int<BBITS> b_0 = b.range((1*BBITS)-1, 0*BBITS);
	ap_int<BBITS> b_2 = b.range((2*BBITS)-1, 1*BBITS);
	ap_int<BBITS> b_4 = b.range((3*BBITS)-1, 2*BBITS);
	ap_int<BBITS> b_6 = b.range((4*BBITS)-1, 3*BBITS);


	// booth digit 0
	ap_int<ACC_BBITS> booth_0  = mul_booth_digit(a_0, b_0);

	// booth digit 2
	ap_int<ACC_BBITS> booth_2_a = mul_booth_digit(a_0, b_2);
	ap_int<ACC_BBITS> booth_2_b = mul_booth_digit(a_2, b_0);
	ap_int<ACC_BBITS> booth_2   = add_booth_digit(booth_2_a, booth_2_b);

	// booth digit 4
	ap_int<ACC_BBITS> booth_4_a = mul_booth_digit(a_0, b_4);
	ap_int<ACC_BBITS> booth_4_b = mul_booth_digit(a_2, b_2);
	ap_int<ACC_BBITS> booth_4_c = mul_booth_digit(a_4, b_0);
	ap_int<ACC_BBITS> booth_4_d = add_booth_digit(booth_4_a, booth_4_b);
	ap_int<ACC_BBITS> booth_4   = add_booth_digit(booth_4_c, booth_4_d);

	// booth digit 6
	ap_int<ACC_BBITS> booth_6_a = mul_booth_digit(a_0, b_6);
	ap_int<ACC_BBITS> booth_6_b = mul_booth_digit(a_2, b_4);
	ap_int<ACC_BBITS> booth_6_c = mul_booth_digit(a_4, b_2);
	ap_int<ACC_BBITS> booth_6_d = mul_booth_digit(a_6, b_0);
	ap_int<ACC_BBITS> booth_6_e = add_booth_digit(booth_6_a, booth_6_b);
	ap_int<ACC_BBITS> booth_6_f = add_booth_digit(booth_6_c, booth_6_d);
	ap_int<ACC_BBITS> booth_6   = add_booth_digit(booth_6_e, booth_6_f);

	// booth digit 8
	ap_int<ACC_BBITS> booth_8_a = mul_booth_digit(a_2, b_6);
	ap_int<ACC_BBITS> booth_8_b = mul_booth_digit(a_4, b_4);
	ap_int<ACC_BBITS> booth_8_c = mul_booth_digit(a_6, b_2);
	ap_int<ACC_BBITS> booth_8_d = add_booth_digit(booth_8_a, booth_8_b);
	ap_int<ACC_BBITS> booth_8   = add_booth_digit(booth_8_c, booth_8_d);

	// booth digit 10
	ap_int<ACC_BBITS> booth_10_a = mul_booth_digit(a_4, b_6);
	ap_int<ACC_BBITS> booth_10_b = mul_booth_digit(a_6, b_4);
	ap_int<ACC_BBITS> booth_10   = add_booth_digit(booth_10_a, booth_10_b);

	// booth digit 12
	ap_int<ACC_BBITS> booth_12   = mul_booth_digit(a_6, b_6);

	ap_uint<ACC_BBITS*7> result;
	result.range((1*ACC_BBITS)-1, (0*ACC_BBITS)) = booth_0;
	result.range((2*ACC_BBITS)-1, (1*ACC_BBITS)) = booth_2;
	result.range((3*ACC_BBITS)-1, (2*ACC_BBITS)) = booth_4;
	result.range((4*ACC_BBITS)-1, (3*ACC_BBITS)) = booth_6;
	result.range((5*ACC_BBITS)-1, (4*ACC_BBITS)) = booth_8;
	result.range((6*ACC_BBITS)-1, (5*ACC_BBITS)) = booth_10;
	result.range((7*ACC_BBITS)-1, (6*ACC_BBITS)) = booth_12;

    #ifdef DEBUG
	int op1 = (int(a_6) << 6) + (int(a_4) << 4) + (int(a_2) << 2) + int(a_0);
	int op2 = (int(b_6) << 6) + (int(b_4) << 4) + (int(b_2) << 2) + int(b_0);
	int r   = (int(booth_12) << 12) + (int(booth_10) << 10) + (int(booth_8) << 8) + (int(booth_6) << 6) + (int(booth_4) << 4) + (int(booth_2) << 2) + int(a_0);

    printf("mul_booth: %3d %3d %3d %3d x %3d %3d %3d %3d = %3d %3d %3d %3d %3d %3d %3d (%3d x %3d = %3d)\n",
    		    int(a_6), int(a_4), int(a_2), int(a_0), int(b_6), int(b_4), int(b_2), int(b_0),
    		    int(booth_12), int(booth_10), int(booth_8), int(booth_6), int(booth_4), int(booth_2), int(booth_0),
				op1, op2, r);
    #endif

	return result;
}

ap_uint<ACC_BBITS*7> sum_booth(ap_uint<ACC_BBITS*7> a, ap_uint<ACC_BBITS*7> b) {
#pragma HLS inline off

	ap_int<ACC_BBITS> a_0   = a.range((ACC_BBITS*1)-1, ACC_BBITS*0);
	ap_int<ACC_BBITS> a_2   = a.range((ACC_BBITS*2)-1, ACC_BBITS*1);
	ap_int<ACC_BBITS> a_4   = a.range((ACC_BBITS*3)-1, ACC_BBITS*2);
	ap_int<ACC_BBITS> a_6   = a.range((ACC_BBITS*4)-1, ACC_BBITS*3);
	ap_int<ACC_BBITS> a_8   = a.range((ACC_BBITS*5)-1, ACC_BBITS*4);
	ap_int<ACC_BBITS> a_10  = a.range((ACC_BBITS*6)-1, ACC_BBITS*5);
	ap_int<ACC_BBITS> a_12  = a.range((ACC_BBITS*7)-1, ACC_BBITS*6);
	ap_int<ACC_BBITS> b_0   = b.range((ACC_BBITS*1)-1, ACC_BBITS*0);
	ap_int<ACC_BBITS> b_2   = b.range((ACC_BBITS*2)-1, ACC_BBITS*1);
	ap_int<ACC_BBITS> b_4   = b.range((ACC_BBITS*3)-1, ACC_BBITS*2);
	ap_int<ACC_BBITS> b_6   = b.range((ACC_BBITS*4)-1, ACC_BBITS*3);
	ap_int<ACC_BBITS> b_8   = b.range((ACC_BBITS*5)-1, ACC_BBITS*4);
	ap_int<ACC_BBITS> b_10  = b.range((ACC_BBITS*6)-1, ACC_BBITS*5);
	ap_int<ACC_BBITS> b_12  = b.range((ACC_BBITS*7)-1, ACC_BBITS*6);

	ap_int<ACC_BBITS> res_0  = a_0 + b_0;
	ap_int<ACC_BBITS> res_2  = a_2 + b_2;
	ap_int<ACC_BBITS> res_4  = a_4 + b_4;
	ap_int<ACC_BBITS> res_6  = a_6 + b_6;
	ap_int<ACC_BBITS> res_8  = a_8 + b_8;
	ap_int<ACC_BBITS> res_10  = a_10 + b_10;
	ap_int<ACC_BBITS> res_12  = a_12 + b_12;

#ifdef DEBUG
	printf("sum_booth: %3d %3d %3d %3d %3d %3d %3d + %3d %3d %3d %3d %3d %3d %3d = %3d %3d %3d %3d %3d %3d %3d\n",
			    int(a_12), int(a_10), int(a_8), int(a_6), int(a_4), int(a_2), int(a_0), int(b_12), int(b_10), int(b_8), int(b_6), int(b_4), int(b_2), int(b_0),
	            int(res_12), int(res_10), int(res_8), int(res_6), int(res_4), int(res_2), int(res_0));
#endif

	ap_uint<ACC_BBITS*7> result;
	result.range((1*ACC_BBITS)-1, (0*ACC_BBITS)) = res_0;
	result.range((2*ACC_BBITS)-1, (1*ACC_BBITS)) = res_2;
	result.range((3*ACC_BBITS)-1, (2*ACC_BBITS)) = res_4;
	result.range((4*ACC_BBITS)-1, (3*ACC_BBITS)) = res_6;
	result.range((5*ACC_BBITS)-1, (4*ACC_BBITS)) = res_8;
	result.range((6*ACC_BBITS)-1, (5*ACC_BBITS)) = res_10;
	result.range((7*ACC_BBITS)-1, (6*ACC_BBITS)) = res_12;

	return result;
}


//           +1  -1  -2  0
//         x +2   0  +1 +1
//        ----------------
//  	     +1  -1  -2  0
//		  +1 -1  -2   0
//      0  0  0   0
//  +2 -2 -4  0
//------------------------
//  +2 -2 -3  0  -3  -2  0

//    +2 -2 -3  0  -3  -2  0
// +  +1 -1  0 +4  -2  -1  0
// --------------------------
//    +3 -3 -3 +4  -5  -3  0

ap_uint<ACC_BBITS*7> mul_vect(ap_uint<BBITS*4> x[VECTOR_SIZE], ap_uint<BBITS*4> w[VECTOR_SIZE]) {
  #pragma HLS inline off
  #pragma HLS array_partition variable=x complete
  #pragma HLS array_partition variable=w complete

  ap_uint<ACC_BBITS*7> res[VECTOR_SIZE];
  #pragma HLS array_partition variable=res complete

  ap_uint<ACC_BBITS*7> sum = 0;
  #pragma HLS aggregate variable=sum
  multiply_reduce_loop:
  for (int i=0; i<VECTOR_SIZE; i++) {
    #pragma HLS unroll
    res[i] = mul_booth(x[i], w[i]);
  }

  for (int i=0; i<VECTOR_SIZE; i=i+1) {
    #pragma hls unroll
    sum = sum_booth(res[i], sum);
    #ifdef DEBUG
    ap_int<ACC_BBITS> sum_0 = sum.range((ACC_BBITS*1)-1, ACC_BBITS*0);
    ap_int<ACC_BBITS> sum_2 = sum.range((ACC_BBITS*2)-1, ACC_BBITS*1);
    ap_int<ACC_BBITS> sum_4 = sum.range((ACC_BBITS*3)-1, ACC_BBITS*2);
    ap_int<ACC_BBITS> sum_6 = sum.range((ACC_BBITS*4)-1, ACC_BBITS*3);
    ap_int<ACC_BBITS> sum_8 = sum.range((ACC_BBITS*5)-1, ACC_BBITS*4);
    ap_int<ACC_BBITS> sum_10 = sum.range((ACC_BBITS*6)-1, ACC_BBITS*5);
    ap_int<ACC_BBITS> sum_12 = sum.range((ACC_BBITS*7)-1, ACC_BBITS*6);
    printf("acc_booth: %3d %3d %3d %3d %3d %3d %3d\n", int(sum_12), int(sum_10), int(sum_8), int(sum_6), int(sum_4), int(sum_2), int(sum_0));
    #endif
  }

  return sum;
}

ap_int<32> booth_to_bin(ap_int<ACC_BBITS*7> a) {
#pragma HLS inline off
	ap_int<ACC_BBITS> a_0   = a.range((ACC_BBITS*1)-1, ACC_BBITS*0);
	ap_int<ACC_BBITS> a_2   = a.range((ACC_BBITS*2)-1, ACC_BBITS*1);
	ap_int<ACC_BBITS> a_4   = a.range((ACC_BBITS*3)-1, ACC_BBITS*2);
	ap_int<ACC_BBITS> a_6   = a.range((ACC_BBITS*4)-1, ACC_BBITS*3);
	ap_int<ACC_BBITS> a_8   = a.range((ACC_BBITS*5)-1, ACC_BBITS*4);
	ap_int<ACC_BBITS> a_10  = a.range((ACC_BBITS*6)-1, ACC_BBITS*5);
	ap_int<ACC_BBITS> a_12  = a.range((ACC_BBITS*7)-1, ACC_BBITS*6);

	ap_int<32> a_0_ext = a_0;
	ap_int<32> a_2_ext = a_2;
	ap_int<32> a_4_ext = a_4;
	ap_int<32> a_6_ext = a_6;
	ap_int<32> a_8_ext = a_8;
	ap_int<32> a_10_ext = a_10;
	ap_int<32> a_12_ext = a_12;

	ap_int<32> result = (a_12_ext << 12) + (a_10_ext << 10) + (a_8_ext << 8) + (a_6_ext << 6) + (a_4_ext << 4) + (a_2_ext << 2) + a_0_ext;

#ifdef DEBUG
	printf("booth_bin: %3d %3d %3d %3d %3d %3d %3d -> %10d\n", int(a_12), int(a_10), int(a_8), int(a_6), int(a_4), int(a_2), int(a_0), int(result));
#endif

	return result;
}
#endif

void top(xw_read_st *x, xw_read_st *w, ap_int<32> *y) {

DO_PRAGMA(HLS interface m_axi port=x offset=slave bundle=gmem depth=OUTPUT_CHANNELS*VECTOR_SIZE)
DO_PRAGMA(HLS interface m_axi port=w offset=slave bundle=gmem1 depth=OUTPUT_CHANNELS*VECTOR_SIZE)
DO_PRAGMA(HLS interface m_axi port=y offset=slave bundle=gmem2 depth=OUTPUT_CHANNELS)

  hls::stream<xw_vector_st> str0_x;
  hls::stream<xw_vector_st> str0_w;
  hls::stream<xw_vector_st> str1_x;
  hls::stream<xw_vector_st> str1_w;
  hls::stream<y_st> str2_y;
  #ifdef USE_BOOTH
  hls::stream<xw_vector_booth_st> str1_x_booth;
  hls::stream<xw_vector_booth_st> str1_w_booth;
  hls::stream<y_booth_st> str2_y_booth;
  #endif

#pragma hls dataflow

  read_vector(x, str0_x);
  read_vector(w, str0_w);
  agrupate_vector(str0_x, str1_x);
  agrupate_vector(str0_w, str1_w);
#ifdef USE_BOOTH
  generate_booth(str1_x, str1_x_booth);
  generate_booth(str1_w, str1_w_booth);
  mul_vector_booth(str1_x_booth, str1_w_booth, str2_y_booth);
  generate_bin(str2_y_booth, str2_y);
#endif
#ifdef USE_MULTIPLIERS
  mul_vector(str1_x, str1_w, str2_y);
#endif
  write_vector(y, str2_y);
}

#ifdef pepito
void top(ap_uint<BBITS*4> *x, ap_uint<BBITS*4> *w, ap_int<32> *y) {

DO_PRAGMA(HLS interface m_axi port=x offset=slave bundle=gmem depth=OUTPUT_CHANNELS*VECTOR_SIZE)
DO_PRAGMA(HLS interface m_axi port=w offset=slave bundle=gmem1 depth=OUTPUT_CHANNELS*VECTOR_SIZE)
DO_PRAGMA(HLS interface m_axi port=y offset=slave bundle=gmem2 depth=OUTPUT_CHANNELS)

  ap_uint<BBITS*4> xx[OUTPUT_CHANNELS][VECTOR_SIZE];
  ap_uint<BBITS*4> ww[OUTPUT_CHANNELS][VECTOR_SIZE];
  #pragma HLS array_partition variable=xx complete
  #pragma hls array_partition variable=ww complete

  int addr = 0;
  load_x_loop:
  for (int c=0; c<OUTPUT_CHANNELS; c++) {
    for (int i=0; i<VECTOR_SIZE; i++) {
      #pragma HLS pipeline II=1
	  xx[c][i] = x[addr];
	  addr++;
    }
  }

  addr = 0;
  load_w_loop:
  for (int c=0; c<OUTPUT_CHANNELS; c++) {
    for (int i=0; i<VECTOR_SIZE; i++) {
      #pragma HLS pipeline II=1
      ww[c][i] = w[addr];
      addr++;
    }
  }

  ap_uint<ACC_BBITS*7> result_booth[OUTPUT_CHANNELS];
  #pragma HLS array_partition variable=result_booth complete

  mult_vect_loop:
  for (int c=0; c<OUTPUT_CHANNELS; c++) {
    #pragma HLS unroll
    result_booth[c] = mul_vect(xx[c], ww[c]);
  }

  ap_int<32> result[OUTPUT_CHANNELS];
  #pragma HLS array_partition variable=result complete

  booth_to_bin_loop:
  for (int c=0; c<OUTPUT_CHANNELS; c++) {
    #pragma hls unroll
	result[c] = booth_to_bin(result_booth[c]);
  }

  store_y_loop:
  for (int c=0; c<OUTPUT_CHANNELS; c++) {
    #pragma hls pipeline ii=1
	y[c] = result[c];
  }

}
#endif
