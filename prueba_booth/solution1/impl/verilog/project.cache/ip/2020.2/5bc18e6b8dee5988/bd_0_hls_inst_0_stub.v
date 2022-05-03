// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
// Date        : Thu Apr 28 16:01:26 2022
// Host        : cmts1.cmts running 64-bit Ubuntu 18.04.5 LTS
// Command     : write_verilog -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ bd_0_hls_inst_0_stub.v
// Design      : bd_0_hls_inst_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xcu200-fsgd2104-2-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "top,Vivado 2020.2" *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix(s_axi_control_AWADDR, 
  s_axi_control_AWVALID, s_axi_control_AWREADY, s_axi_control_WDATA, s_axi_control_WSTRB, 
  s_axi_control_WVALID, s_axi_control_WREADY, s_axi_control_BRESP, s_axi_control_BVALID, 
  s_axi_control_BREADY, s_axi_control_ARADDR, s_axi_control_ARVALID, 
  s_axi_control_ARREADY, s_axi_control_RDATA, s_axi_control_RRESP, s_axi_control_RVALID, 
  s_axi_control_RREADY, ap_clk, ap_rst_n, interrupt, m_axi_gmem2_AWADDR, m_axi_gmem2_AWLEN, 
  m_axi_gmem2_AWSIZE, m_axi_gmem2_AWBURST, m_axi_gmem2_AWLOCK, m_axi_gmem2_AWREGION, 
  m_axi_gmem2_AWCACHE, m_axi_gmem2_AWPROT, m_axi_gmem2_AWQOS, m_axi_gmem2_AWVALID, 
  m_axi_gmem2_AWREADY, m_axi_gmem2_WDATA, m_axi_gmem2_WSTRB, m_axi_gmem2_WLAST, 
  m_axi_gmem2_WVALID, m_axi_gmem2_WREADY, m_axi_gmem2_BRESP, m_axi_gmem2_BVALID, 
  m_axi_gmem2_BREADY, m_axi_gmem2_ARADDR, m_axi_gmem2_ARLEN, m_axi_gmem2_ARSIZE, 
  m_axi_gmem2_ARBURST, m_axi_gmem2_ARLOCK, m_axi_gmem2_ARREGION, m_axi_gmem2_ARCACHE, 
  m_axi_gmem2_ARPROT, m_axi_gmem2_ARQOS, m_axi_gmem2_ARVALID, m_axi_gmem2_ARREADY, 
  m_axi_gmem2_RDATA, m_axi_gmem2_RRESP, m_axi_gmem2_RLAST, m_axi_gmem2_RVALID, 
  m_axi_gmem2_RREADY, m_axi_gmem3_AWADDR, m_axi_gmem3_AWLEN, m_axi_gmem3_AWSIZE, 
  m_axi_gmem3_AWBURST, m_axi_gmem3_AWLOCK, m_axi_gmem3_AWREGION, m_axi_gmem3_AWCACHE, 
  m_axi_gmem3_AWPROT, m_axi_gmem3_AWQOS, m_axi_gmem3_AWVALID, m_axi_gmem3_AWREADY, 
  m_axi_gmem3_WDATA, m_axi_gmem3_WSTRB, m_axi_gmem3_WLAST, m_axi_gmem3_WVALID, 
  m_axi_gmem3_WREADY, m_axi_gmem3_BRESP, m_axi_gmem3_BVALID, m_axi_gmem3_BREADY, 
  m_axi_gmem3_ARADDR, m_axi_gmem3_ARLEN, m_axi_gmem3_ARSIZE, m_axi_gmem3_ARBURST, 
  m_axi_gmem3_ARLOCK, m_axi_gmem3_ARREGION, m_axi_gmem3_ARCACHE, m_axi_gmem3_ARPROT, 
  m_axi_gmem3_ARQOS, m_axi_gmem3_ARVALID, m_axi_gmem3_ARREADY, m_axi_gmem3_RDATA, 
  m_axi_gmem3_RRESP, m_axi_gmem3_RLAST, m_axi_gmem3_RVALID, m_axi_gmem3_RREADY, 
  m_axi_gmem4_AWADDR, m_axi_gmem4_AWLEN, m_axi_gmem4_AWSIZE, m_axi_gmem4_AWBURST, 
  m_axi_gmem4_AWLOCK, m_axi_gmem4_AWREGION, m_axi_gmem4_AWCACHE, m_axi_gmem4_AWPROT, 
  m_axi_gmem4_AWQOS, m_axi_gmem4_AWVALID, m_axi_gmem4_AWREADY, m_axi_gmem4_WDATA, 
  m_axi_gmem4_WSTRB, m_axi_gmem4_WLAST, m_axi_gmem4_WVALID, m_axi_gmem4_WREADY, 
  m_axi_gmem4_BRESP, m_axi_gmem4_BVALID, m_axi_gmem4_BREADY, m_axi_gmem4_ARADDR, 
  m_axi_gmem4_ARLEN, m_axi_gmem4_ARSIZE, m_axi_gmem4_ARBURST, m_axi_gmem4_ARLOCK, 
  m_axi_gmem4_ARREGION, m_axi_gmem4_ARCACHE, m_axi_gmem4_ARPROT, m_axi_gmem4_ARQOS, 
  m_axi_gmem4_ARVALID, m_axi_gmem4_ARREADY, m_axi_gmem4_RDATA, m_axi_gmem4_RRESP, 
  m_axi_gmem4_RLAST, m_axi_gmem4_RVALID, m_axi_gmem4_RREADY, m_axi_gmem5_AWADDR, 
  m_axi_gmem5_AWLEN, m_axi_gmem5_AWSIZE, m_axi_gmem5_AWBURST, m_axi_gmem5_AWLOCK, 
  m_axi_gmem5_AWREGION, m_axi_gmem5_AWCACHE, m_axi_gmem5_AWPROT, m_axi_gmem5_AWQOS, 
  m_axi_gmem5_AWVALID, m_axi_gmem5_AWREADY, m_axi_gmem5_WDATA, m_axi_gmem5_WSTRB, 
  m_axi_gmem5_WLAST, m_axi_gmem5_WVALID, m_axi_gmem5_WREADY, m_axi_gmem5_BRESP, 
  m_axi_gmem5_BVALID, m_axi_gmem5_BREADY, m_axi_gmem5_ARADDR, m_axi_gmem5_ARLEN, 
  m_axi_gmem5_ARSIZE, m_axi_gmem5_ARBURST, m_axi_gmem5_ARLOCK, m_axi_gmem5_ARREGION, 
  m_axi_gmem5_ARCACHE, m_axi_gmem5_ARPROT, m_axi_gmem5_ARQOS, m_axi_gmem5_ARVALID, 
  m_axi_gmem5_ARREADY, m_axi_gmem5_RDATA, m_axi_gmem5_RRESP, m_axi_gmem5_RLAST, 
  m_axi_gmem5_RVALID, m_axi_gmem5_RREADY, m_axi_gmem_AWADDR, m_axi_gmem_AWLEN, 
  m_axi_gmem_AWSIZE, m_axi_gmem_AWBURST, m_axi_gmem_AWLOCK, m_axi_gmem_AWREGION, 
  m_axi_gmem_AWCACHE, m_axi_gmem_AWPROT, m_axi_gmem_AWQOS, m_axi_gmem_AWVALID, 
  m_axi_gmem_AWREADY, m_axi_gmem_WDATA, m_axi_gmem_WSTRB, m_axi_gmem_WLAST, 
  m_axi_gmem_WVALID, m_axi_gmem_WREADY, m_axi_gmem_BRESP, m_axi_gmem_BVALID, 
  m_axi_gmem_BREADY, m_axi_gmem_ARADDR, m_axi_gmem_ARLEN, m_axi_gmem_ARSIZE, 
  m_axi_gmem_ARBURST, m_axi_gmem_ARLOCK, m_axi_gmem_ARREGION, m_axi_gmem_ARCACHE, 
  m_axi_gmem_ARPROT, m_axi_gmem_ARQOS, m_axi_gmem_ARVALID, m_axi_gmem_ARREADY, 
  m_axi_gmem_RDATA, m_axi_gmem_RRESP, m_axi_gmem_RLAST, m_axi_gmem_RVALID, 
  m_axi_gmem_RREADY, m_axi_gmem1_AWADDR, m_axi_gmem1_AWLEN, m_axi_gmem1_AWSIZE, 
  m_axi_gmem1_AWBURST, m_axi_gmem1_AWLOCK, m_axi_gmem1_AWREGION, m_axi_gmem1_AWCACHE, 
  m_axi_gmem1_AWPROT, m_axi_gmem1_AWQOS, m_axi_gmem1_AWVALID, m_axi_gmem1_AWREADY, 
  m_axi_gmem1_WDATA, m_axi_gmem1_WSTRB, m_axi_gmem1_WLAST, m_axi_gmem1_WVALID, 
  m_axi_gmem1_WREADY, m_axi_gmem1_BRESP, m_axi_gmem1_BVALID, m_axi_gmem1_BREADY, 
  m_axi_gmem1_ARADDR, m_axi_gmem1_ARLEN, m_axi_gmem1_ARSIZE, m_axi_gmem1_ARBURST, 
  m_axi_gmem1_ARLOCK, m_axi_gmem1_ARREGION, m_axi_gmem1_ARCACHE, m_axi_gmem1_ARPROT, 
  m_axi_gmem1_ARQOS, m_axi_gmem1_ARVALID, m_axi_gmem1_ARREADY, m_axi_gmem1_RDATA, 
  m_axi_gmem1_RRESP, m_axi_gmem1_RLAST, m_axi_gmem1_RVALID, m_axi_gmem1_RREADY)
/* synthesis syn_black_box black_box_pad_pin="s_axi_control_AWADDR[6:0],s_axi_control_AWVALID,s_axi_control_AWREADY,s_axi_control_WDATA[31:0],s_axi_control_WSTRB[3:0],s_axi_control_WVALID,s_axi_control_WREADY,s_axi_control_BRESP[1:0],s_axi_control_BVALID,s_axi_control_BREADY,s_axi_control_ARADDR[6:0],s_axi_control_ARVALID,s_axi_control_ARREADY,s_axi_control_RDATA[31:0],s_axi_control_RRESP[1:0],s_axi_control_RVALID,s_axi_control_RREADY,ap_clk,ap_rst_n,interrupt,m_axi_gmem2_AWADDR[63:0],m_axi_gmem2_AWLEN[7:0],m_axi_gmem2_AWSIZE[2:0],m_axi_gmem2_AWBURST[1:0],m_axi_gmem2_AWLOCK[1:0],m_axi_gmem2_AWREGION[3:0],m_axi_gmem2_AWCACHE[3:0],m_axi_gmem2_AWPROT[2:0],m_axi_gmem2_AWQOS[3:0],m_axi_gmem2_AWVALID,m_axi_gmem2_AWREADY,m_axi_gmem2_WDATA[31:0],m_axi_gmem2_WSTRB[3:0],m_axi_gmem2_WLAST,m_axi_gmem2_WVALID,m_axi_gmem2_WREADY,m_axi_gmem2_BRESP[1:0],m_axi_gmem2_BVALID,m_axi_gmem2_BREADY,m_axi_gmem2_ARADDR[63:0],m_axi_gmem2_ARLEN[7:0],m_axi_gmem2_ARSIZE[2:0],m_axi_gmem2_ARBURST[1:0],m_axi_gmem2_ARLOCK[1:0],m_axi_gmem2_ARREGION[3:0],m_axi_gmem2_ARCACHE[3:0],m_axi_gmem2_ARPROT[2:0],m_axi_gmem2_ARQOS[3:0],m_axi_gmem2_ARVALID,m_axi_gmem2_ARREADY,m_axi_gmem2_RDATA[31:0],m_axi_gmem2_RRESP[1:0],m_axi_gmem2_RLAST,m_axi_gmem2_RVALID,m_axi_gmem2_RREADY,m_axi_gmem3_AWADDR[63:0],m_axi_gmem3_AWLEN[7:0],m_axi_gmem3_AWSIZE[2:0],m_axi_gmem3_AWBURST[1:0],m_axi_gmem3_AWLOCK[1:0],m_axi_gmem3_AWREGION[3:0],m_axi_gmem3_AWCACHE[3:0],m_axi_gmem3_AWPROT[2:0],m_axi_gmem3_AWQOS[3:0],m_axi_gmem3_AWVALID,m_axi_gmem3_AWREADY,m_axi_gmem3_WDATA[31:0],m_axi_gmem3_WSTRB[3:0],m_axi_gmem3_WLAST,m_axi_gmem3_WVALID,m_axi_gmem3_WREADY,m_axi_gmem3_BRESP[1:0],m_axi_gmem3_BVALID,m_axi_gmem3_BREADY,m_axi_gmem3_ARADDR[63:0],m_axi_gmem3_ARLEN[7:0],m_axi_gmem3_ARSIZE[2:0],m_axi_gmem3_ARBURST[1:0],m_axi_gmem3_ARLOCK[1:0],m_axi_gmem3_ARREGION[3:0],m_axi_gmem3_ARCACHE[3:0],m_axi_gmem3_ARPROT[2:0],m_axi_gmem3_ARQOS[3:0],m_axi_gmem3_ARVALID,m_axi_gmem3_ARREADY,m_axi_gmem3_RDATA[31:0],m_axi_gmem3_RRESP[1:0],m_axi_gmem3_RLAST,m_axi_gmem3_RVALID,m_axi_gmem3_RREADY,m_axi_gmem4_AWADDR[63:0],m_axi_gmem4_AWLEN[7:0],m_axi_gmem4_AWSIZE[2:0],m_axi_gmem4_AWBURST[1:0],m_axi_gmem4_AWLOCK[1:0],m_axi_gmem4_AWREGION[3:0],m_axi_gmem4_AWCACHE[3:0],m_axi_gmem4_AWPROT[2:0],m_axi_gmem4_AWQOS[3:0],m_axi_gmem4_AWVALID,m_axi_gmem4_AWREADY,m_axi_gmem4_WDATA[31:0],m_axi_gmem4_WSTRB[3:0],m_axi_gmem4_WLAST,m_axi_gmem4_WVALID,m_axi_gmem4_WREADY,m_axi_gmem4_BRESP[1:0],m_axi_gmem4_BVALID,m_axi_gmem4_BREADY,m_axi_gmem4_ARADDR[63:0],m_axi_gmem4_ARLEN[7:0],m_axi_gmem4_ARSIZE[2:0],m_axi_gmem4_ARBURST[1:0],m_axi_gmem4_ARLOCK[1:0],m_axi_gmem4_ARREGION[3:0],m_axi_gmem4_ARCACHE[3:0],m_axi_gmem4_ARPROT[2:0],m_axi_gmem4_ARQOS[3:0],m_axi_gmem4_ARVALID,m_axi_gmem4_ARREADY,m_axi_gmem4_RDATA[31:0],m_axi_gmem4_RRESP[1:0],m_axi_gmem4_RLAST,m_axi_gmem4_RVALID,m_axi_gmem4_RREADY,m_axi_gmem5_AWADDR[63:0],m_axi_gmem5_AWLEN[7:0],m_axi_gmem5_AWSIZE[2:0],m_axi_gmem5_AWBURST[1:0],m_axi_gmem5_AWLOCK[1:0],m_axi_gmem5_AWREGION[3:0],m_axi_gmem5_AWCACHE[3:0],m_axi_gmem5_AWPROT[2:0],m_axi_gmem5_AWQOS[3:0],m_axi_gmem5_AWVALID,m_axi_gmem5_AWREADY,m_axi_gmem5_WDATA[31:0],m_axi_gmem5_WSTRB[3:0],m_axi_gmem5_WLAST,m_axi_gmem5_WVALID,m_axi_gmem5_WREADY,m_axi_gmem5_BRESP[1:0],m_axi_gmem5_BVALID,m_axi_gmem5_BREADY,m_axi_gmem5_ARADDR[63:0],m_axi_gmem5_ARLEN[7:0],m_axi_gmem5_ARSIZE[2:0],m_axi_gmem5_ARBURST[1:0],m_axi_gmem5_ARLOCK[1:0],m_axi_gmem5_ARREGION[3:0],m_axi_gmem5_ARCACHE[3:0],m_axi_gmem5_ARPROT[2:0],m_axi_gmem5_ARQOS[3:0],m_axi_gmem5_ARVALID,m_axi_gmem5_ARREADY,m_axi_gmem5_RDATA[31:0],m_axi_gmem5_RRESP[1:0],m_axi_gmem5_RLAST,m_axi_gmem5_RVALID,m_axi_gmem5_RREADY,m_axi_gmem_AWADDR[63:0],m_axi_gmem_AWLEN[7:0],m_axi_gmem_AWSIZE[2:0],m_axi_gmem_AWBURST[1:0],m_axi_gmem_AWLOCK[1:0],m_axi_gmem_AWREGION[3:0],m_axi_gmem_AWCACHE[3:0],m_axi_gmem_AWPROT[2:0],m_axi_gmem_AWQOS[3:0],m_axi_gmem_AWVALID,m_axi_gmem_AWREADY,m_axi_gmem_WDATA[31:0],m_axi_gmem_WSTRB[3:0],m_axi_gmem_WLAST,m_axi_gmem_WVALID,m_axi_gmem_WREADY,m_axi_gmem_BRESP[1:0],m_axi_gmem_BVALID,m_axi_gmem_BREADY,m_axi_gmem_ARADDR[63:0],m_axi_gmem_ARLEN[7:0],m_axi_gmem_ARSIZE[2:0],m_axi_gmem_ARBURST[1:0],m_axi_gmem_ARLOCK[1:0],m_axi_gmem_ARREGION[3:0],m_axi_gmem_ARCACHE[3:0],m_axi_gmem_ARPROT[2:0],m_axi_gmem_ARQOS[3:0],m_axi_gmem_ARVALID,m_axi_gmem_ARREADY,m_axi_gmem_RDATA[31:0],m_axi_gmem_RRESP[1:0],m_axi_gmem_RLAST,m_axi_gmem_RVALID,m_axi_gmem_RREADY,m_axi_gmem1_AWADDR[63:0],m_axi_gmem1_AWLEN[7:0],m_axi_gmem1_AWSIZE[2:0],m_axi_gmem1_AWBURST[1:0],m_axi_gmem1_AWLOCK[1:0],m_axi_gmem1_AWREGION[3:0],m_axi_gmem1_AWCACHE[3:0],m_axi_gmem1_AWPROT[2:0],m_axi_gmem1_AWQOS[3:0],m_axi_gmem1_AWVALID,m_axi_gmem1_AWREADY,m_axi_gmem1_WDATA[511:0],m_axi_gmem1_WSTRB[63:0],m_axi_gmem1_WLAST,m_axi_gmem1_WVALID,m_axi_gmem1_WREADY,m_axi_gmem1_BRESP[1:0],m_axi_gmem1_BVALID,m_axi_gmem1_BREADY,m_axi_gmem1_ARADDR[63:0],m_axi_gmem1_ARLEN[7:0],m_axi_gmem1_ARSIZE[2:0],m_axi_gmem1_ARBURST[1:0],m_axi_gmem1_ARLOCK[1:0],m_axi_gmem1_ARREGION[3:0],m_axi_gmem1_ARCACHE[3:0],m_axi_gmem1_ARPROT[2:0],m_axi_gmem1_ARQOS[3:0],m_axi_gmem1_ARVALID,m_axi_gmem1_ARREADY,m_axi_gmem1_RDATA[511:0],m_axi_gmem1_RRESP[1:0],m_axi_gmem1_RLAST,m_axi_gmem1_RVALID,m_axi_gmem1_RREADY" */;
  input [6:0]s_axi_control_AWADDR;
  input s_axi_control_AWVALID;
  output s_axi_control_AWREADY;
  input [31:0]s_axi_control_WDATA;
  input [3:0]s_axi_control_WSTRB;
  input s_axi_control_WVALID;
  output s_axi_control_WREADY;
  output [1:0]s_axi_control_BRESP;
  output s_axi_control_BVALID;
  input s_axi_control_BREADY;
  input [6:0]s_axi_control_ARADDR;
  input s_axi_control_ARVALID;
  output s_axi_control_ARREADY;
  output [31:0]s_axi_control_RDATA;
  output [1:0]s_axi_control_RRESP;
  output s_axi_control_RVALID;
  input s_axi_control_RREADY;
  input ap_clk;
  input ap_rst_n;
  output interrupt;
  output [63:0]m_axi_gmem2_AWADDR;
  output [7:0]m_axi_gmem2_AWLEN;
  output [2:0]m_axi_gmem2_AWSIZE;
  output [1:0]m_axi_gmem2_AWBURST;
  output [1:0]m_axi_gmem2_AWLOCK;
  output [3:0]m_axi_gmem2_AWREGION;
  output [3:0]m_axi_gmem2_AWCACHE;
  output [2:0]m_axi_gmem2_AWPROT;
  output [3:0]m_axi_gmem2_AWQOS;
  output m_axi_gmem2_AWVALID;
  input m_axi_gmem2_AWREADY;
  output [31:0]m_axi_gmem2_WDATA;
  output [3:0]m_axi_gmem2_WSTRB;
  output m_axi_gmem2_WLAST;
  output m_axi_gmem2_WVALID;
  input m_axi_gmem2_WREADY;
  input [1:0]m_axi_gmem2_BRESP;
  input m_axi_gmem2_BVALID;
  output m_axi_gmem2_BREADY;
  output [63:0]m_axi_gmem2_ARADDR;
  output [7:0]m_axi_gmem2_ARLEN;
  output [2:0]m_axi_gmem2_ARSIZE;
  output [1:0]m_axi_gmem2_ARBURST;
  output [1:0]m_axi_gmem2_ARLOCK;
  output [3:0]m_axi_gmem2_ARREGION;
  output [3:0]m_axi_gmem2_ARCACHE;
  output [2:0]m_axi_gmem2_ARPROT;
  output [3:0]m_axi_gmem2_ARQOS;
  output m_axi_gmem2_ARVALID;
  input m_axi_gmem2_ARREADY;
  input [31:0]m_axi_gmem2_RDATA;
  input [1:0]m_axi_gmem2_RRESP;
  input m_axi_gmem2_RLAST;
  input m_axi_gmem2_RVALID;
  output m_axi_gmem2_RREADY;
  output [63:0]m_axi_gmem3_AWADDR;
  output [7:0]m_axi_gmem3_AWLEN;
  output [2:0]m_axi_gmem3_AWSIZE;
  output [1:0]m_axi_gmem3_AWBURST;
  output [1:0]m_axi_gmem3_AWLOCK;
  output [3:0]m_axi_gmem3_AWREGION;
  output [3:0]m_axi_gmem3_AWCACHE;
  output [2:0]m_axi_gmem3_AWPROT;
  output [3:0]m_axi_gmem3_AWQOS;
  output m_axi_gmem3_AWVALID;
  input m_axi_gmem3_AWREADY;
  output [31:0]m_axi_gmem3_WDATA;
  output [3:0]m_axi_gmem3_WSTRB;
  output m_axi_gmem3_WLAST;
  output m_axi_gmem3_WVALID;
  input m_axi_gmem3_WREADY;
  input [1:0]m_axi_gmem3_BRESP;
  input m_axi_gmem3_BVALID;
  output m_axi_gmem3_BREADY;
  output [63:0]m_axi_gmem3_ARADDR;
  output [7:0]m_axi_gmem3_ARLEN;
  output [2:0]m_axi_gmem3_ARSIZE;
  output [1:0]m_axi_gmem3_ARBURST;
  output [1:0]m_axi_gmem3_ARLOCK;
  output [3:0]m_axi_gmem3_ARREGION;
  output [3:0]m_axi_gmem3_ARCACHE;
  output [2:0]m_axi_gmem3_ARPROT;
  output [3:0]m_axi_gmem3_ARQOS;
  output m_axi_gmem3_ARVALID;
  input m_axi_gmem3_ARREADY;
  input [31:0]m_axi_gmem3_RDATA;
  input [1:0]m_axi_gmem3_RRESP;
  input m_axi_gmem3_RLAST;
  input m_axi_gmem3_RVALID;
  output m_axi_gmem3_RREADY;
  output [63:0]m_axi_gmem4_AWADDR;
  output [7:0]m_axi_gmem4_AWLEN;
  output [2:0]m_axi_gmem4_AWSIZE;
  output [1:0]m_axi_gmem4_AWBURST;
  output [1:0]m_axi_gmem4_AWLOCK;
  output [3:0]m_axi_gmem4_AWREGION;
  output [3:0]m_axi_gmem4_AWCACHE;
  output [2:0]m_axi_gmem4_AWPROT;
  output [3:0]m_axi_gmem4_AWQOS;
  output m_axi_gmem4_AWVALID;
  input m_axi_gmem4_AWREADY;
  output [31:0]m_axi_gmem4_WDATA;
  output [3:0]m_axi_gmem4_WSTRB;
  output m_axi_gmem4_WLAST;
  output m_axi_gmem4_WVALID;
  input m_axi_gmem4_WREADY;
  input [1:0]m_axi_gmem4_BRESP;
  input m_axi_gmem4_BVALID;
  output m_axi_gmem4_BREADY;
  output [63:0]m_axi_gmem4_ARADDR;
  output [7:0]m_axi_gmem4_ARLEN;
  output [2:0]m_axi_gmem4_ARSIZE;
  output [1:0]m_axi_gmem4_ARBURST;
  output [1:0]m_axi_gmem4_ARLOCK;
  output [3:0]m_axi_gmem4_ARREGION;
  output [3:0]m_axi_gmem4_ARCACHE;
  output [2:0]m_axi_gmem4_ARPROT;
  output [3:0]m_axi_gmem4_ARQOS;
  output m_axi_gmem4_ARVALID;
  input m_axi_gmem4_ARREADY;
  input [31:0]m_axi_gmem4_RDATA;
  input [1:0]m_axi_gmem4_RRESP;
  input m_axi_gmem4_RLAST;
  input m_axi_gmem4_RVALID;
  output m_axi_gmem4_RREADY;
  output [63:0]m_axi_gmem5_AWADDR;
  output [7:0]m_axi_gmem5_AWLEN;
  output [2:0]m_axi_gmem5_AWSIZE;
  output [1:0]m_axi_gmem5_AWBURST;
  output [1:0]m_axi_gmem5_AWLOCK;
  output [3:0]m_axi_gmem5_AWREGION;
  output [3:0]m_axi_gmem5_AWCACHE;
  output [2:0]m_axi_gmem5_AWPROT;
  output [3:0]m_axi_gmem5_AWQOS;
  output m_axi_gmem5_AWVALID;
  input m_axi_gmem5_AWREADY;
  output [31:0]m_axi_gmem5_WDATA;
  output [3:0]m_axi_gmem5_WSTRB;
  output m_axi_gmem5_WLAST;
  output m_axi_gmem5_WVALID;
  input m_axi_gmem5_WREADY;
  input [1:0]m_axi_gmem5_BRESP;
  input m_axi_gmem5_BVALID;
  output m_axi_gmem5_BREADY;
  output [63:0]m_axi_gmem5_ARADDR;
  output [7:0]m_axi_gmem5_ARLEN;
  output [2:0]m_axi_gmem5_ARSIZE;
  output [1:0]m_axi_gmem5_ARBURST;
  output [1:0]m_axi_gmem5_ARLOCK;
  output [3:0]m_axi_gmem5_ARREGION;
  output [3:0]m_axi_gmem5_ARCACHE;
  output [2:0]m_axi_gmem5_ARPROT;
  output [3:0]m_axi_gmem5_ARQOS;
  output m_axi_gmem5_ARVALID;
  input m_axi_gmem5_ARREADY;
  input [31:0]m_axi_gmem5_RDATA;
  input [1:0]m_axi_gmem5_RRESP;
  input m_axi_gmem5_RLAST;
  input m_axi_gmem5_RVALID;
  output m_axi_gmem5_RREADY;
  output [63:0]m_axi_gmem_AWADDR;
  output [7:0]m_axi_gmem_AWLEN;
  output [2:0]m_axi_gmem_AWSIZE;
  output [1:0]m_axi_gmem_AWBURST;
  output [1:0]m_axi_gmem_AWLOCK;
  output [3:0]m_axi_gmem_AWREGION;
  output [3:0]m_axi_gmem_AWCACHE;
  output [2:0]m_axi_gmem_AWPROT;
  output [3:0]m_axi_gmem_AWQOS;
  output m_axi_gmem_AWVALID;
  input m_axi_gmem_AWREADY;
  output [31:0]m_axi_gmem_WDATA;
  output [3:0]m_axi_gmem_WSTRB;
  output m_axi_gmem_WLAST;
  output m_axi_gmem_WVALID;
  input m_axi_gmem_WREADY;
  input [1:0]m_axi_gmem_BRESP;
  input m_axi_gmem_BVALID;
  output m_axi_gmem_BREADY;
  output [63:0]m_axi_gmem_ARADDR;
  output [7:0]m_axi_gmem_ARLEN;
  output [2:0]m_axi_gmem_ARSIZE;
  output [1:0]m_axi_gmem_ARBURST;
  output [1:0]m_axi_gmem_ARLOCK;
  output [3:0]m_axi_gmem_ARREGION;
  output [3:0]m_axi_gmem_ARCACHE;
  output [2:0]m_axi_gmem_ARPROT;
  output [3:0]m_axi_gmem_ARQOS;
  output m_axi_gmem_ARVALID;
  input m_axi_gmem_ARREADY;
  input [31:0]m_axi_gmem_RDATA;
  input [1:0]m_axi_gmem_RRESP;
  input m_axi_gmem_RLAST;
  input m_axi_gmem_RVALID;
  output m_axi_gmem_RREADY;
  output [63:0]m_axi_gmem1_AWADDR;
  output [7:0]m_axi_gmem1_AWLEN;
  output [2:0]m_axi_gmem1_AWSIZE;
  output [1:0]m_axi_gmem1_AWBURST;
  output [1:0]m_axi_gmem1_AWLOCK;
  output [3:0]m_axi_gmem1_AWREGION;
  output [3:0]m_axi_gmem1_AWCACHE;
  output [2:0]m_axi_gmem1_AWPROT;
  output [3:0]m_axi_gmem1_AWQOS;
  output m_axi_gmem1_AWVALID;
  input m_axi_gmem1_AWREADY;
  output [511:0]m_axi_gmem1_WDATA;
  output [63:0]m_axi_gmem1_WSTRB;
  output m_axi_gmem1_WLAST;
  output m_axi_gmem1_WVALID;
  input m_axi_gmem1_WREADY;
  input [1:0]m_axi_gmem1_BRESP;
  input m_axi_gmem1_BVALID;
  output m_axi_gmem1_BREADY;
  output [63:0]m_axi_gmem1_ARADDR;
  output [7:0]m_axi_gmem1_ARLEN;
  output [2:0]m_axi_gmem1_ARSIZE;
  output [1:0]m_axi_gmem1_ARBURST;
  output [1:0]m_axi_gmem1_ARLOCK;
  output [3:0]m_axi_gmem1_ARREGION;
  output [3:0]m_axi_gmem1_ARCACHE;
  output [2:0]m_axi_gmem1_ARPROT;
  output [3:0]m_axi_gmem1_ARQOS;
  output m_axi_gmem1_ARVALID;
  input m_axi_gmem1_ARREADY;
  input [511:0]m_axi_gmem1_RDATA;
  input [1:0]m_axi_gmem1_RRESP;
  input m_axi_gmem1_RLAST;
  input m_axi_gmem1_RVALID;
  output m_axi_gmem1_RREADY;
endmodule
