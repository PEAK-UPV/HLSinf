#include <systemc>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <stdint.h>
#include "SysCFileHandler.h"
#include "ap_int.h"
#include "ap_fixed.h"
#include <complex>
#include <stdbool.h>
#include "autopilot_cbe.h"
#include "hls_stream.h"
#include "hls_half.h"
#include "hls_signal_handler.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

// wrapc file define:
#define AUTOTB_TVIN_gmem2 "../tv/cdatafile/c.top.autotvin_gmem2.dat"
#define AUTOTB_TVOUT_gmem2 "../tv/cdatafile/c.top.autotvout_gmem2.dat"
// wrapc file define:
#define AUTOTB_TVIN_gmem3 "../tv/cdatafile/c.top.autotvin_gmem3.dat"
#define AUTOTB_TVOUT_gmem3 "../tv/cdatafile/c.top.autotvout_gmem3.dat"
// wrapc file define:
#define AUTOTB_TVIN_gmem4 "../tv/cdatafile/c.top.autotvin_gmem4.dat"
#define AUTOTB_TVOUT_gmem4 "../tv/cdatafile/c.top.autotvout_gmem4.dat"
// wrapc file define:
#define AUTOTB_TVIN_gmem5 "../tv/cdatafile/c.top.autotvin_gmem5.dat"
#define AUTOTB_TVOUT_gmem5 "../tv/cdatafile/c.top.autotvout_gmem5.dat"
// wrapc file define:
#define AUTOTB_TVIN_gmem "../tv/cdatafile/c.top.autotvin_gmem.dat"
#define AUTOTB_TVOUT_gmem "../tv/cdatafile/c.top.autotvout_gmem.dat"
// wrapc file define:
#define AUTOTB_TVIN_gmem1 "../tv/cdatafile/c.top.autotvin_gmem1.dat"
#define AUTOTB_TVOUT_gmem1 "../tv/cdatafile/c.top.autotvout_gmem1.dat"
// wrapc file define:
#define AUTOTB_TVIN_x0 "../tv/cdatafile/c.top.autotvin_x0.dat"
#define AUTOTB_TVOUT_x0 "../tv/cdatafile/c.top.autotvout_x0.dat"
// wrapc file define:
#define AUTOTB_TVIN_x1 "../tv/cdatafile/c.top.autotvin_x1.dat"
#define AUTOTB_TVOUT_x1 "../tv/cdatafile/c.top.autotvout_x1.dat"
// wrapc file define:
#define AUTOTB_TVIN_x2 "../tv/cdatafile/c.top.autotvin_x2.dat"
#define AUTOTB_TVOUT_x2 "../tv/cdatafile/c.top.autotvout_x2.dat"
// wrapc file define:
#define AUTOTB_TVIN_x3 "../tv/cdatafile/c.top.autotvin_x3.dat"
#define AUTOTB_TVOUT_x3 "../tv/cdatafile/c.top.autotvout_x3.dat"
// wrapc file define:
#define AUTOTB_TVIN_w "../tv/cdatafile/c.top.autotvin_w.dat"
#define AUTOTB_TVOUT_w "../tv/cdatafile/c.top.autotvout_w.dat"
// wrapc file define:
#define AUTOTB_TVIN_y "../tv/cdatafile/c.top.autotvin_y.dat"
#define AUTOTB_TVOUT_y "../tv/cdatafile/c.top.autotvout_y.dat"

#define INTER_TCL "../tv/cdatafile/ref.tcl"

// tvout file define:
#define AUTOTB_TVOUT_PC_gmem2 "../tv/rtldatafile/rtl.top.autotvout_gmem2.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_gmem3 "../tv/rtldatafile/rtl.top.autotvout_gmem3.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_gmem4 "../tv/rtldatafile/rtl.top.autotvout_gmem4.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_gmem5 "../tv/rtldatafile/rtl.top.autotvout_gmem5.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_gmem "../tv/rtldatafile/rtl.top.autotvout_gmem.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_gmem1 "../tv/rtldatafile/rtl.top.autotvout_gmem1.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_x0 "../tv/rtldatafile/rtl.top.autotvout_x0.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_x1 "../tv/rtldatafile/rtl.top.autotvout_x1.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_x2 "../tv/rtldatafile/rtl.top.autotvout_x2.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_x3 "../tv/rtldatafile/rtl.top.autotvout_x3.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_w "../tv/rtldatafile/rtl.top.autotvout_w.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_y "../tv/rtldatafile/rtl.top.autotvout_y.dat"

class INTER_TCL_FILE {
  public:
INTER_TCL_FILE(const char* name) {
  mName = name; 
  gmem2_depth = 0;
  gmem3_depth = 0;
  gmem4_depth = 0;
  gmem5_depth = 0;
  gmem_depth = 0;
  gmem1_depth = 0;
  x0_depth = 0;
  x1_depth = 0;
  x2_depth = 0;
  x3_depth = 0;
  w_depth = 0;
  y_depth = 0;
  trans_num =0;
}
~INTER_TCL_FILE() {
  mFile.open(mName);
  if (!mFile.good()) {
    cout << "Failed to open file ref.tcl" << endl;
    exit (1); 
  }
  string total_list = get_depth_list();
  mFile << "set depth_list {\n";
  mFile << total_list;
  mFile << "}\n";
  mFile << "set trans_num "<<trans_num<<endl;
  mFile.close();
}
string get_depth_list () {
  stringstream total_list;
  total_list << "{gmem2 " << gmem2_depth << "}\n";
  total_list << "{gmem3 " << gmem3_depth << "}\n";
  total_list << "{gmem4 " << gmem4_depth << "}\n";
  total_list << "{gmem5 " << gmem5_depth << "}\n";
  total_list << "{gmem " << gmem_depth << "}\n";
  total_list << "{gmem1 " << gmem1_depth << "}\n";
  total_list << "{x0 " << x0_depth << "}\n";
  total_list << "{x1 " << x1_depth << "}\n";
  total_list << "{x2 " << x2_depth << "}\n";
  total_list << "{x3 " << x3_depth << "}\n";
  total_list << "{w " << w_depth << "}\n";
  total_list << "{y " << y_depth << "}\n";
  return total_list.str();
}
void set_num (int num , int* class_num) {
  (*class_num) = (*class_num) > num ? (*class_num) : num;
}
void set_string(std::string list, std::string* class_list) {
  (*class_list) = list;
}
  public:
    int gmem2_depth;
    int gmem3_depth;
    int gmem4_depth;
    int gmem5_depth;
    int gmem_depth;
    int gmem1_depth;
    int x0_depth;
    int x1_depth;
    int x2_depth;
    int x3_depth;
    int w_depth;
    int y_depth;
    int trans_num;
  private:
    ofstream mFile;
    const char* mName;
};

static void RTLOutputCheckAndReplacement(std::string &AESL_token, std::string PortName) {
  bool no_x = false;
  bool err = false;

  no_x = false;
  // search and replace 'X' with '0' from the 3rd char of token
  while (!no_x) {
    size_t x_found = AESL_token.find('X', 0);
    if (x_found != string::npos) {
      if (!err) { 
        cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'X' on port" 
             << PortName << ", possible cause: There are uninitialized variables in the C design."
             << endl; 
        err = true;
      }
      AESL_token.replace(x_found, 1, "0");
    } else
      no_x = true;
  }
  no_x = false;
  // search and replace 'x' with '0' from the 3rd char of token
  while (!no_x) {
    size_t x_found = AESL_token.find('x', 2);
    if (x_found != string::npos) {
      if (!err) { 
        cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' on port" 
             << PortName << ", possible cause: There are uninitialized variables in the C design."
             << endl; 
        err = true;
      }
      AESL_token.replace(x_found, 1, "0");
    } else
      no_x = true;
  }
}
struct __cosim_s40__ { char data[64]; };
extern "C" void top_hw_stub_wrapper(volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *);

extern "C" void apatb_top_hw(volatile void * __xlx_apatb_param_x0, volatile void * __xlx_apatb_param_x1, volatile void * __xlx_apatb_param_x2, volatile void * __xlx_apatb_param_x3, volatile void * __xlx_apatb_param_w, volatile void * __xlx_apatb_param_y) {
  refine_signal_handler();
  fstream wrapc_switch_file_token;
  wrapc_switch_file_token.open(".hls_cosim_wrapc_switch.log");
  int AESL_i;
  if (wrapc_switch_file_token.good())
  {

    CodeState = ENTER_WRAPC_PC;
    static unsigned AESL_transaction_pc = 0;
    string AESL_token;
    string AESL_num;{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_gmem1);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<512> > gmem1_pc_buffer(1);
          int i = 0;

          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            RTLOutputCheckAndReplacement(AESL_token, "gmem1");
  
            // push token into output port buffer
            if (AESL_token != "") {
              gmem1_pc_buffer[i] = AESL_token.c_str();;
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (i > 0) {{
            int i = 0;
            for (int j = 0, e = 1; j < e; j += 1, ++i) {((long long*)__xlx_apatb_param_y)[j*8+0] = gmem1_pc_buffer[i].range(63,0).to_int64();
((long long*)__xlx_apatb_param_y)[j*8+1] = gmem1_pc_buffer[i].range(127,64).to_int64();
((long long*)__xlx_apatb_param_y)[j*8+2] = gmem1_pc_buffer[i].range(191,128).to_int64();
((long long*)__xlx_apatb_param_y)[j*8+3] = gmem1_pc_buffer[i].range(255,192).to_int64();
((long long*)__xlx_apatb_param_y)[j*8+4] = gmem1_pc_buffer[i].range(319,256).to_int64();
((long long*)__xlx_apatb_param_y)[j*8+5] = gmem1_pc_buffer[i].range(383,320).to_int64();
((long long*)__xlx_apatb_param_y)[j*8+6] = gmem1_pc_buffer[i].range(447,384).to_int64();
((long long*)__xlx_apatb_param_y)[j*8+7] = gmem1_pc_buffer[i].range(511,448).to_int64();
}}}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
    AESL_transaction_pc++;
    return ;
  }
static unsigned AESL_transaction;
static AESL_FILE_HANDLER aesl_fh;
static INTER_TCL_FILE tcl_file(INTER_TCL);
std::vector<char> __xlx_sprintf_buffer(1024);
CodeState = ENTER_WRAPC;
//gmem2
aesl_fh.touch(AUTOTB_TVIN_gmem2);
aesl_fh.touch(AUTOTB_TVOUT_gmem2);
//gmem3
aesl_fh.touch(AUTOTB_TVIN_gmem3);
aesl_fh.touch(AUTOTB_TVOUT_gmem3);
//gmem4
aesl_fh.touch(AUTOTB_TVIN_gmem4);
aesl_fh.touch(AUTOTB_TVOUT_gmem4);
//gmem5
aesl_fh.touch(AUTOTB_TVIN_gmem5);
aesl_fh.touch(AUTOTB_TVOUT_gmem5);
//gmem
aesl_fh.touch(AUTOTB_TVIN_gmem);
aesl_fh.touch(AUTOTB_TVOUT_gmem);
//gmem1
aesl_fh.touch(AUTOTB_TVIN_gmem1);
aesl_fh.touch(AUTOTB_TVOUT_gmem1);
//x0
aesl_fh.touch(AUTOTB_TVIN_x0);
aesl_fh.touch(AUTOTB_TVOUT_x0);
//x1
aesl_fh.touch(AUTOTB_TVIN_x1);
aesl_fh.touch(AUTOTB_TVOUT_x1);
//x2
aesl_fh.touch(AUTOTB_TVIN_x2);
aesl_fh.touch(AUTOTB_TVOUT_x2);
//x3
aesl_fh.touch(AUTOTB_TVIN_x3);
aesl_fh.touch(AUTOTB_TVOUT_x3);
//w
aesl_fh.touch(AUTOTB_TVIN_w);
aesl_fh.touch(AUTOTB_TVOUT_w);
//y
aesl_fh.touch(AUTOTB_TVIN_y);
aesl_fh.touch(AUTOTB_TVOUT_y);
CodeState = DUMP_INPUTS;
unsigned __xlx_offset_byte_param_x0 = 0;
// print gmem2 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_gmem2, __xlx_sprintf_buffer.data());
  {  __xlx_offset_byte_param_x0 = 0*2;
  if (__xlx_apatb_param_x0) {
    for (int j = 0  - 0, e = 1 - 0; j != e; ++j) {
sc_bv<16> __xlx_tmp_lv = ((short*)__xlx_apatb_param_x0)[j];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_gmem2, __xlx_sprintf_buffer.data()); 
      }
  }
}
  tcl_file.set_num(1, &tcl_file.gmem2_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_gmem2, __xlx_sprintf_buffer.data());
}
unsigned __xlx_offset_byte_param_x1 = 0;
// print gmem3 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_gmem3, __xlx_sprintf_buffer.data());
  {  __xlx_offset_byte_param_x1 = 0*2;
  if (__xlx_apatb_param_x1) {
    for (int j = 0  - 0, e = 1 - 0; j != e; ++j) {
sc_bv<16> __xlx_tmp_lv = ((short*)__xlx_apatb_param_x1)[j];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_gmem3, __xlx_sprintf_buffer.data()); 
      }
  }
}
  tcl_file.set_num(1, &tcl_file.gmem3_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_gmem3, __xlx_sprintf_buffer.data());
}
unsigned __xlx_offset_byte_param_x2 = 0;
// print gmem4 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_gmem4, __xlx_sprintf_buffer.data());
  {  __xlx_offset_byte_param_x2 = 0*2;
  if (__xlx_apatb_param_x2) {
    for (int j = 0  - 0, e = 1 - 0; j != e; ++j) {
sc_bv<16> __xlx_tmp_lv = ((short*)__xlx_apatb_param_x2)[j];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_gmem4, __xlx_sprintf_buffer.data()); 
      }
  }
}
  tcl_file.set_num(1, &tcl_file.gmem4_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_gmem4, __xlx_sprintf_buffer.data());
}
unsigned __xlx_offset_byte_param_x3 = 0;
// print gmem5 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_gmem5, __xlx_sprintf_buffer.data());
  {  __xlx_offset_byte_param_x3 = 0*2;
  if (__xlx_apatb_param_x3) {
    for (int j = 0  - 0, e = 1 - 0; j != e; ++j) {
sc_bv<16> __xlx_tmp_lv = ((short*)__xlx_apatb_param_x3)[j];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_gmem5, __xlx_sprintf_buffer.data()); 
      }
  }
}
  tcl_file.set_num(1, &tcl_file.gmem5_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_gmem5, __xlx_sprintf_buffer.data());
}
unsigned __xlx_offset_byte_param_w = 0;
// print gmem Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_gmem, __xlx_sprintf_buffer.data());
  {  __xlx_offset_byte_param_w = 0*1;
  if (__xlx_apatb_param_w) {
    for (int j = 0  - 0, e = 1 - 0; j != e; ++j) {
sc_bv<8> __xlx_tmp_lv = ((char*)__xlx_apatb_param_w)[j];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_gmem, __xlx_sprintf_buffer.data()); 
      }
  }
}
  tcl_file.set_num(1, &tcl_file.gmem_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_gmem, __xlx_sprintf_buffer.data());
}
unsigned __xlx_offset_byte_param_y = 0;
// print gmem1 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_gmem1, __xlx_sprintf_buffer.data());
  {  __xlx_offset_byte_param_y = 0*64;
  if (__xlx_apatb_param_y) {
    for (int j = 0  - 0, e = 1 - 0; j != e; ++j) {
sc_bv<512> __xlx_tmp_lv;
__xlx_tmp_lv.range(63,0) = ((long long*)__xlx_apatb_param_y)[j*8+0];
__xlx_tmp_lv.range(127,64) = ((long long*)__xlx_apatb_param_y)[j*8+1];
__xlx_tmp_lv.range(191,128) = ((long long*)__xlx_apatb_param_y)[j*8+2];
__xlx_tmp_lv.range(255,192) = ((long long*)__xlx_apatb_param_y)[j*8+3];
__xlx_tmp_lv.range(319,256) = ((long long*)__xlx_apatb_param_y)[j*8+4];
__xlx_tmp_lv.range(383,320) = ((long long*)__xlx_apatb_param_y)[j*8+5];
__xlx_tmp_lv.range(447,384) = ((long long*)__xlx_apatb_param_y)[j*8+6];
__xlx_tmp_lv.range(511,448) = ((long long*)__xlx_apatb_param_y)[j*8+7];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_gmem1, __xlx_sprintf_buffer.data()); 
      }
  }
}
  tcl_file.set_num(1, &tcl_file.gmem1_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_gmem1, __xlx_sprintf_buffer.data());
}
// print x0 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_x0, __xlx_sprintf_buffer.data());
  {
    sc_bv<64> __xlx_tmp_lv = __xlx_offset_byte_param_x0;

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_x0, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.x0_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_x0, __xlx_sprintf_buffer.data());
}
// print x1 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_x1, __xlx_sprintf_buffer.data());
  {
    sc_bv<64> __xlx_tmp_lv = __xlx_offset_byte_param_x1;

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_x1, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.x1_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_x1, __xlx_sprintf_buffer.data());
}
// print x2 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_x2, __xlx_sprintf_buffer.data());
  {
    sc_bv<64> __xlx_tmp_lv = __xlx_offset_byte_param_x2;

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_x2, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.x2_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_x2, __xlx_sprintf_buffer.data());
}
// print x3 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_x3, __xlx_sprintf_buffer.data());
  {
    sc_bv<64> __xlx_tmp_lv = __xlx_offset_byte_param_x3;

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_x3, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.x3_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_x3, __xlx_sprintf_buffer.data());
}
// print w Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_w, __xlx_sprintf_buffer.data());
  {
    sc_bv<64> __xlx_tmp_lv = __xlx_offset_byte_param_w;

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_w, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.w_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_w, __xlx_sprintf_buffer.data());
}
// print y Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_y, __xlx_sprintf_buffer.data());
  {
    sc_bv<64> __xlx_tmp_lv = __xlx_offset_byte_param_y;

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_y, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.y_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_y, __xlx_sprintf_buffer.data());
}
CodeState = CALL_C_DUT;
top_hw_stub_wrapper(__xlx_apatb_param_x0, __xlx_apatb_param_x1, __xlx_apatb_param_x2, __xlx_apatb_param_x3, __xlx_apatb_param_w, __xlx_apatb_param_y);
CodeState = DUMP_OUTPUTS;
// print gmem1 Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVOUT_gmem1, __xlx_sprintf_buffer.data());
  {  __xlx_offset_byte_param_y = 0*64;
  if (__xlx_apatb_param_y) {
    for (int j = 0  - 0, e = 1 - 0; j != e; ++j) {
sc_bv<512> __xlx_tmp_lv;
__xlx_tmp_lv.range(63,0) = ((long long*)__xlx_apatb_param_y)[j*8+0];
__xlx_tmp_lv.range(127,64) = ((long long*)__xlx_apatb_param_y)[j*8+1];
__xlx_tmp_lv.range(191,128) = ((long long*)__xlx_apatb_param_y)[j*8+2];
__xlx_tmp_lv.range(255,192) = ((long long*)__xlx_apatb_param_y)[j*8+3];
__xlx_tmp_lv.range(319,256) = ((long long*)__xlx_apatb_param_y)[j*8+4];
__xlx_tmp_lv.range(383,320) = ((long long*)__xlx_apatb_param_y)[j*8+5];
__xlx_tmp_lv.range(447,384) = ((long long*)__xlx_apatb_param_y)[j*8+6];
__xlx_tmp_lv.range(511,448) = ((long long*)__xlx_apatb_param_y)[j*8+7];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVOUT_gmem1, __xlx_sprintf_buffer.data()); 
      }
  }
}
  tcl_file.set_num(1, &tcl_file.gmem1_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVOUT_gmem1, __xlx_sprintf_buffer.data());
}
CodeState = DELETE_CHAR_BUFFERS;
AESL_transaction++;
tcl_file.set_num(AESL_transaction , &tcl_file.trans_num);
}
