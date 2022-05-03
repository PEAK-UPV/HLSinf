#include <systemc>
#include <vector>
#include <iostream>
#include "hls_stream.h"
#include "ap_int.h"
#include "ap_fixed.h"
using namespace std;
using namespace sc_dt;
class AESL_RUNTIME_BC {
  public:
    AESL_RUNTIME_BC(const char* name) {
      file_token.open( name);
      if (!file_token.good()) {
        cout << "Failed to open tv file " << name << endl;
        exit (1);
      }
      file_token >> mName;//[[[runtime]]]
    }
    ~AESL_RUNTIME_BC() {
      file_token.close();
    }
    int read_size () {
      int size = 0;
      file_token >> mName;//[[transaction]]
      file_token >> mName;//transaction number
      file_token >> mName;//pop_size
      size = atoi(mName.c_str());
      file_token >> mName;//[[/transaction]]
      return size;
    }
  public:
    fstream file_token;
    string mName;
};
struct __cosim_s40__ { char data[64]; };
extern "C" void top(short*, short*, short*, short*, char*, __cosim_s40__*, int, int, int, int, int, int);
extern "C" void apatb_top_hw(volatile void * __xlx_apatb_param_x0, volatile void * __xlx_apatb_param_x1, volatile void * __xlx_apatb_param_x2, volatile void * __xlx_apatb_param_x3, volatile void * __xlx_apatb_param_w, volatile void * __xlx_apatb_param_y) {
  // Collect __xlx_x0__tmp_vec
  vector<sc_bv<16> >__xlx_x0__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_x0__tmp_vec.push_back(((short*)__xlx_apatb_param_x0)[j]);
  }
  int __xlx_size_param_x0 = 1;
  int __xlx_offset_param_x0 = 0;
  int __xlx_offset_byte_param_x0 = 0*2;
  short* __xlx_x0__input_buffer= new short[__xlx_x0__tmp_vec.size()];
  for (int i = 0; i < __xlx_x0__tmp_vec.size(); ++i) {
    __xlx_x0__input_buffer[i] = __xlx_x0__tmp_vec[i].range(15, 0).to_uint64();
  }
  // Collect __xlx_x1__tmp_vec
  vector<sc_bv<16> >__xlx_x1__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_x1__tmp_vec.push_back(((short*)__xlx_apatb_param_x1)[j]);
  }
  int __xlx_size_param_x1 = 1;
  int __xlx_offset_param_x1 = 0;
  int __xlx_offset_byte_param_x1 = 0*2;
  short* __xlx_x1__input_buffer= new short[__xlx_x1__tmp_vec.size()];
  for (int i = 0; i < __xlx_x1__tmp_vec.size(); ++i) {
    __xlx_x1__input_buffer[i] = __xlx_x1__tmp_vec[i].range(15, 0).to_uint64();
  }
  // Collect __xlx_x2__tmp_vec
  vector<sc_bv<16> >__xlx_x2__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_x2__tmp_vec.push_back(((short*)__xlx_apatb_param_x2)[j]);
  }
  int __xlx_size_param_x2 = 1;
  int __xlx_offset_param_x2 = 0;
  int __xlx_offset_byte_param_x2 = 0*2;
  short* __xlx_x2__input_buffer= new short[__xlx_x2__tmp_vec.size()];
  for (int i = 0; i < __xlx_x2__tmp_vec.size(); ++i) {
    __xlx_x2__input_buffer[i] = __xlx_x2__tmp_vec[i].range(15, 0).to_uint64();
  }
  // Collect __xlx_x3__tmp_vec
  vector<sc_bv<16> >__xlx_x3__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_x3__tmp_vec.push_back(((short*)__xlx_apatb_param_x3)[j]);
  }
  int __xlx_size_param_x3 = 1;
  int __xlx_offset_param_x3 = 0;
  int __xlx_offset_byte_param_x3 = 0*2;
  short* __xlx_x3__input_buffer= new short[__xlx_x3__tmp_vec.size()];
  for (int i = 0; i < __xlx_x3__tmp_vec.size(); ++i) {
    __xlx_x3__input_buffer[i] = __xlx_x3__tmp_vec[i].range(15, 0).to_uint64();
  }
  // Collect __xlx_w__tmp_vec
  vector<sc_bv<8> >__xlx_w__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    __xlx_w__tmp_vec.push_back(((char*)__xlx_apatb_param_w)[j]);
  }
  int __xlx_size_param_w = 1;
  int __xlx_offset_param_w = 0;
  int __xlx_offset_byte_param_w = 0*1;
  char* __xlx_w__input_buffer= new char[__xlx_w__tmp_vec.size()];
  for (int i = 0; i < __xlx_w__tmp_vec.size(); ++i) {
    __xlx_w__input_buffer[i] = __xlx_w__tmp_vec[i].range(7, 0).to_uint64();
  }
  // Collect __xlx_y__tmp_vec
  vector<sc_bv<512> >__xlx_y__tmp_vec;
  for (int j = 0, e = 1; j != e; ++j) {
    sc_bv<512> _xlx_tmp_sc;
    _xlx_tmp_sc.range(63, 0) = ((long long*)__xlx_apatb_param_y)[j*8+0];
    _xlx_tmp_sc.range(127, 64) = ((long long*)__xlx_apatb_param_y)[j*8+1];
    _xlx_tmp_sc.range(191, 128) = ((long long*)__xlx_apatb_param_y)[j*8+2];
    _xlx_tmp_sc.range(255, 192) = ((long long*)__xlx_apatb_param_y)[j*8+3];
    _xlx_tmp_sc.range(319, 256) = ((long long*)__xlx_apatb_param_y)[j*8+4];
    _xlx_tmp_sc.range(383, 320) = ((long long*)__xlx_apatb_param_y)[j*8+5];
    _xlx_tmp_sc.range(447, 384) = ((long long*)__xlx_apatb_param_y)[j*8+6];
    _xlx_tmp_sc.range(511, 448) = ((long long*)__xlx_apatb_param_y)[j*8+7];
    __xlx_y__tmp_vec.push_back(_xlx_tmp_sc);
  }
  int __xlx_size_param_y = 1;
  int __xlx_offset_param_y = 0;
  int __xlx_offset_byte_param_y = 0*64;
  __cosim_s40__* __xlx_y__input_buffer= new __cosim_s40__[__xlx_y__tmp_vec.size()];
  for (int i = 0; i < __xlx_y__tmp_vec.size(); ++i) {
    ((long long*)__xlx_y__input_buffer)[i*8+0] = __xlx_y__tmp_vec[i].range(63, 0).to_uint64();
    ((long long*)__xlx_y__input_buffer)[i*8+1] = __xlx_y__tmp_vec[i].range(127, 64).to_uint64();
    ((long long*)__xlx_y__input_buffer)[i*8+2] = __xlx_y__tmp_vec[i].range(191, 128).to_uint64();
    ((long long*)__xlx_y__input_buffer)[i*8+3] = __xlx_y__tmp_vec[i].range(255, 192).to_uint64();
    ((long long*)__xlx_y__input_buffer)[i*8+4] = __xlx_y__tmp_vec[i].range(319, 256).to_uint64();
    ((long long*)__xlx_y__input_buffer)[i*8+5] = __xlx_y__tmp_vec[i].range(383, 320).to_uint64();
    ((long long*)__xlx_y__input_buffer)[i*8+6] = __xlx_y__tmp_vec[i].range(447, 384).to_uint64();
    ((long long*)__xlx_y__input_buffer)[i*8+7] = __xlx_y__tmp_vec[i].range(511, 448).to_uint64();
  }
  // DUT call
  top(__xlx_x0__input_buffer, __xlx_x1__input_buffer, __xlx_x2__input_buffer, __xlx_x3__input_buffer, __xlx_w__input_buffer, __xlx_y__input_buffer, __xlx_offset_byte_param_x0, __xlx_offset_byte_param_x1, __xlx_offset_byte_param_x2, __xlx_offset_byte_param_x3, __xlx_offset_byte_param_w, __xlx_offset_byte_param_y);
// print __xlx_apatb_param_x0
  sc_bv<16>*__xlx_x0_output_buffer = new sc_bv<16>[__xlx_size_param_x0];
  for (int i = 0; i < __xlx_size_param_x0; ++i) {
    __xlx_x0_output_buffer[i] = __xlx_x0__input_buffer[i+__xlx_offset_param_x0];
  }
  for (int i = 0; i < __xlx_size_param_x0; ++i) {
    ((short*)__xlx_apatb_param_x0)[i] = __xlx_x0_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_x1
  sc_bv<16>*__xlx_x1_output_buffer = new sc_bv<16>[__xlx_size_param_x1];
  for (int i = 0; i < __xlx_size_param_x1; ++i) {
    __xlx_x1_output_buffer[i] = __xlx_x1__input_buffer[i+__xlx_offset_param_x1];
  }
  for (int i = 0; i < __xlx_size_param_x1; ++i) {
    ((short*)__xlx_apatb_param_x1)[i] = __xlx_x1_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_x2
  sc_bv<16>*__xlx_x2_output_buffer = new sc_bv<16>[__xlx_size_param_x2];
  for (int i = 0; i < __xlx_size_param_x2; ++i) {
    __xlx_x2_output_buffer[i] = __xlx_x2__input_buffer[i+__xlx_offset_param_x2];
  }
  for (int i = 0; i < __xlx_size_param_x2; ++i) {
    ((short*)__xlx_apatb_param_x2)[i] = __xlx_x2_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_x3
  sc_bv<16>*__xlx_x3_output_buffer = new sc_bv<16>[__xlx_size_param_x3];
  for (int i = 0; i < __xlx_size_param_x3; ++i) {
    __xlx_x3_output_buffer[i] = __xlx_x3__input_buffer[i+__xlx_offset_param_x3];
  }
  for (int i = 0; i < __xlx_size_param_x3; ++i) {
    ((short*)__xlx_apatb_param_x3)[i] = __xlx_x3_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_w
  sc_bv<8>*__xlx_w_output_buffer = new sc_bv<8>[__xlx_size_param_w];
  for (int i = 0; i < __xlx_size_param_w; ++i) {
    __xlx_w_output_buffer[i] = __xlx_w__input_buffer[i+__xlx_offset_param_w];
  }
  for (int i = 0; i < __xlx_size_param_w; ++i) {
    ((char*)__xlx_apatb_param_w)[i] = __xlx_w_output_buffer[i].to_uint64();
  }
// print __xlx_apatb_param_y
  sc_bv<512>*__xlx_y_output_buffer = new sc_bv<512>[__xlx_size_param_y];
  for (int i = 0; i < __xlx_size_param_y; ++i) {
    char* start = (char*)(&(__xlx_y__input_buffer[__xlx_offset_param_y]));
    __xlx_y_output_buffer[i].range(63, 0) = ((long long*)start)[i*8+0];
    __xlx_y_output_buffer[i].range(127, 64) = ((long long*)start)[i*8+1];
    __xlx_y_output_buffer[i].range(191, 128) = ((long long*)start)[i*8+2];
    __xlx_y_output_buffer[i].range(255, 192) = ((long long*)start)[i*8+3];
    __xlx_y_output_buffer[i].range(319, 256) = ((long long*)start)[i*8+4];
    __xlx_y_output_buffer[i].range(383, 320) = ((long long*)start)[i*8+5];
    __xlx_y_output_buffer[i].range(447, 384) = ((long long*)start)[i*8+6];
    __xlx_y_output_buffer[i].range(511, 448) = ((long long*)start)[i*8+7];
  }
  for (int i = 0; i < __xlx_size_param_y; ++i) {
    ((long long*)__xlx_apatb_param_y)[i*8+0] = __xlx_y_output_buffer[i].range(63, 0).to_uint64();
    ((long long*)__xlx_apatb_param_y)[i*8+1] = __xlx_y_output_buffer[i].range(127, 64).to_uint64();
    ((long long*)__xlx_apatb_param_y)[i*8+2] = __xlx_y_output_buffer[i].range(191, 128).to_uint64();
    ((long long*)__xlx_apatb_param_y)[i*8+3] = __xlx_y_output_buffer[i].range(255, 192).to_uint64();
    ((long long*)__xlx_apatb_param_y)[i*8+4] = __xlx_y_output_buffer[i].range(319, 256).to_uint64();
    ((long long*)__xlx_apatb_param_y)[i*8+5] = __xlx_y_output_buffer[i].range(383, 320).to_uint64();
    ((long long*)__xlx_apatb_param_y)[i*8+6] = __xlx_y_output_buffer[i].range(447, 384).to_uint64();
    ((long long*)__xlx_apatb_param_y)[i*8+7] = __xlx_y_output_buffer[i].range(511, 448).to_uint64();
  }
}
