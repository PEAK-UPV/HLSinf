//Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
//Date        : Thu Apr 28 15:59:10 2022
//Host        : cmts1.cmts running 64-bit Ubuntu 18.04.5 LTS
//Command     : generate_target bd_0.bd
//Design      : bd_0
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CORE_GENERATION_INFO = "bd_0,IP_Integrator,{x_ipVendor=xilinx.com,x_ipLibrary=BlockDiagram,x_ipName=bd_0,x_ipVersion=1.00.a,x_ipLanguage=VERILOG,numBlks=1,numReposBlks=1,numNonXlnxBlks=0,numHierBlks=0,maxHierDepth=0,numSysgenBlks=0,numHlsBlks=1,numHdlrefBlks=0,numPkgbdBlks=0,bdsource=USER,synth_mode=OOC_per_IP}" *) (* HW_HANDOFF = "bd_0.hwdef" *) 
module bd_0
   (ap_clk,
    ap_rst_n,
    interrupt,
    m_axi_gmem1_araddr,
    m_axi_gmem1_arburst,
    m_axi_gmem1_arcache,
    m_axi_gmem1_arlen,
    m_axi_gmem1_arlock,
    m_axi_gmem1_arprot,
    m_axi_gmem1_arqos,
    m_axi_gmem1_arready,
    m_axi_gmem1_arregion,
    m_axi_gmem1_arsize,
    m_axi_gmem1_arvalid,
    m_axi_gmem1_awaddr,
    m_axi_gmem1_awburst,
    m_axi_gmem1_awcache,
    m_axi_gmem1_awlen,
    m_axi_gmem1_awlock,
    m_axi_gmem1_awprot,
    m_axi_gmem1_awqos,
    m_axi_gmem1_awready,
    m_axi_gmem1_awregion,
    m_axi_gmem1_awsize,
    m_axi_gmem1_awvalid,
    m_axi_gmem1_bready,
    m_axi_gmem1_bresp,
    m_axi_gmem1_bvalid,
    m_axi_gmem1_rdata,
    m_axi_gmem1_rlast,
    m_axi_gmem1_rready,
    m_axi_gmem1_rresp,
    m_axi_gmem1_rvalid,
    m_axi_gmem1_wdata,
    m_axi_gmem1_wlast,
    m_axi_gmem1_wready,
    m_axi_gmem1_wstrb,
    m_axi_gmem1_wvalid,
    m_axi_gmem2_araddr,
    m_axi_gmem2_arburst,
    m_axi_gmem2_arcache,
    m_axi_gmem2_arlen,
    m_axi_gmem2_arlock,
    m_axi_gmem2_arprot,
    m_axi_gmem2_arqos,
    m_axi_gmem2_arready,
    m_axi_gmem2_arregion,
    m_axi_gmem2_arsize,
    m_axi_gmem2_arvalid,
    m_axi_gmem2_awaddr,
    m_axi_gmem2_awburst,
    m_axi_gmem2_awcache,
    m_axi_gmem2_awlen,
    m_axi_gmem2_awlock,
    m_axi_gmem2_awprot,
    m_axi_gmem2_awqos,
    m_axi_gmem2_awready,
    m_axi_gmem2_awregion,
    m_axi_gmem2_awsize,
    m_axi_gmem2_awvalid,
    m_axi_gmem2_bready,
    m_axi_gmem2_bresp,
    m_axi_gmem2_bvalid,
    m_axi_gmem2_rdata,
    m_axi_gmem2_rlast,
    m_axi_gmem2_rready,
    m_axi_gmem2_rresp,
    m_axi_gmem2_rvalid,
    m_axi_gmem2_wdata,
    m_axi_gmem2_wlast,
    m_axi_gmem2_wready,
    m_axi_gmem2_wstrb,
    m_axi_gmem2_wvalid,
    m_axi_gmem3_araddr,
    m_axi_gmem3_arburst,
    m_axi_gmem3_arcache,
    m_axi_gmem3_arlen,
    m_axi_gmem3_arlock,
    m_axi_gmem3_arprot,
    m_axi_gmem3_arqos,
    m_axi_gmem3_arready,
    m_axi_gmem3_arregion,
    m_axi_gmem3_arsize,
    m_axi_gmem3_arvalid,
    m_axi_gmem3_awaddr,
    m_axi_gmem3_awburst,
    m_axi_gmem3_awcache,
    m_axi_gmem3_awlen,
    m_axi_gmem3_awlock,
    m_axi_gmem3_awprot,
    m_axi_gmem3_awqos,
    m_axi_gmem3_awready,
    m_axi_gmem3_awregion,
    m_axi_gmem3_awsize,
    m_axi_gmem3_awvalid,
    m_axi_gmem3_bready,
    m_axi_gmem3_bresp,
    m_axi_gmem3_bvalid,
    m_axi_gmem3_rdata,
    m_axi_gmem3_rlast,
    m_axi_gmem3_rready,
    m_axi_gmem3_rresp,
    m_axi_gmem3_rvalid,
    m_axi_gmem3_wdata,
    m_axi_gmem3_wlast,
    m_axi_gmem3_wready,
    m_axi_gmem3_wstrb,
    m_axi_gmem3_wvalid,
    m_axi_gmem4_araddr,
    m_axi_gmem4_arburst,
    m_axi_gmem4_arcache,
    m_axi_gmem4_arlen,
    m_axi_gmem4_arlock,
    m_axi_gmem4_arprot,
    m_axi_gmem4_arqos,
    m_axi_gmem4_arready,
    m_axi_gmem4_arregion,
    m_axi_gmem4_arsize,
    m_axi_gmem4_arvalid,
    m_axi_gmem4_awaddr,
    m_axi_gmem4_awburst,
    m_axi_gmem4_awcache,
    m_axi_gmem4_awlen,
    m_axi_gmem4_awlock,
    m_axi_gmem4_awprot,
    m_axi_gmem4_awqos,
    m_axi_gmem4_awready,
    m_axi_gmem4_awregion,
    m_axi_gmem4_awsize,
    m_axi_gmem4_awvalid,
    m_axi_gmem4_bready,
    m_axi_gmem4_bresp,
    m_axi_gmem4_bvalid,
    m_axi_gmem4_rdata,
    m_axi_gmem4_rlast,
    m_axi_gmem4_rready,
    m_axi_gmem4_rresp,
    m_axi_gmem4_rvalid,
    m_axi_gmem4_wdata,
    m_axi_gmem4_wlast,
    m_axi_gmem4_wready,
    m_axi_gmem4_wstrb,
    m_axi_gmem4_wvalid,
    m_axi_gmem5_araddr,
    m_axi_gmem5_arburst,
    m_axi_gmem5_arcache,
    m_axi_gmem5_arlen,
    m_axi_gmem5_arlock,
    m_axi_gmem5_arprot,
    m_axi_gmem5_arqos,
    m_axi_gmem5_arready,
    m_axi_gmem5_arregion,
    m_axi_gmem5_arsize,
    m_axi_gmem5_arvalid,
    m_axi_gmem5_awaddr,
    m_axi_gmem5_awburst,
    m_axi_gmem5_awcache,
    m_axi_gmem5_awlen,
    m_axi_gmem5_awlock,
    m_axi_gmem5_awprot,
    m_axi_gmem5_awqos,
    m_axi_gmem5_awready,
    m_axi_gmem5_awregion,
    m_axi_gmem5_awsize,
    m_axi_gmem5_awvalid,
    m_axi_gmem5_bready,
    m_axi_gmem5_bresp,
    m_axi_gmem5_bvalid,
    m_axi_gmem5_rdata,
    m_axi_gmem5_rlast,
    m_axi_gmem5_rready,
    m_axi_gmem5_rresp,
    m_axi_gmem5_rvalid,
    m_axi_gmem5_wdata,
    m_axi_gmem5_wlast,
    m_axi_gmem5_wready,
    m_axi_gmem5_wstrb,
    m_axi_gmem5_wvalid,
    m_axi_gmem_araddr,
    m_axi_gmem_arburst,
    m_axi_gmem_arcache,
    m_axi_gmem_arlen,
    m_axi_gmem_arlock,
    m_axi_gmem_arprot,
    m_axi_gmem_arqos,
    m_axi_gmem_arready,
    m_axi_gmem_arregion,
    m_axi_gmem_arsize,
    m_axi_gmem_arvalid,
    m_axi_gmem_awaddr,
    m_axi_gmem_awburst,
    m_axi_gmem_awcache,
    m_axi_gmem_awlen,
    m_axi_gmem_awlock,
    m_axi_gmem_awprot,
    m_axi_gmem_awqos,
    m_axi_gmem_awready,
    m_axi_gmem_awregion,
    m_axi_gmem_awsize,
    m_axi_gmem_awvalid,
    m_axi_gmem_bready,
    m_axi_gmem_bresp,
    m_axi_gmem_bvalid,
    m_axi_gmem_rdata,
    m_axi_gmem_rlast,
    m_axi_gmem_rready,
    m_axi_gmem_rresp,
    m_axi_gmem_rvalid,
    m_axi_gmem_wdata,
    m_axi_gmem_wlast,
    m_axi_gmem_wready,
    m_axi_gmem_wstrb,
    m_axi_gmem_wvalid,
    s_axi_control_araddr,
    s_axi_control_arready,
    s_axi_control_arvalid,
    s_axi_control_awaddr,
    s_axi_control_awready,
    s_axi_control_awvalid,
    s_axi_control_bready,
    s_axi_control_bresp,
    s_axi_control_bvalid,
    s_axi_control_rdata,
    s_axi_control_rready,
    s_axi_control_rresp,
    s_axi_control_rvalid,
    s_axi_control_wdata,
    s_axi_control_wready,
    s_axi_control_wstrb,
    s_axi_control_wvalid);
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 CLK.AP_CLK CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME CLK.AP_CLK, ASSOCIATED_BUSIF m_axi_gmem:m_axi_gmem1:m_axi_gmem2:m_axi_gmem3:m_axi_gmem4:m_axi_gmem5:s_axi_control, ASSOCIATED_RESET ap_rst_n, CLK_DOMAIN bd_0_ap_clk_0, FREQ_HZ 300000000.0, FREQ_TOLERANCE_HZ 0, INSERT_VIP 0, PHASE 0.000" *) input ap_clk;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 RST.AP_RST_N RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME RST.AP_RST_N, INSERT_VIP 0, POLARITY ACTIVE_LOW" *) input ap_rst_n;
  (* X_INTERFACE_INFO = "xilinx.com:signal:interrupt:1.0 INTR.INTERRUPT INTERRUPT" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME INTR.INTERRUPT, PortWidth 1, SENSITIVITY LEVEL_HIGH" *) output interrupt;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_gmem1, ADDR_WIDTH 64, ARUSER_WIDTH 0, AWUSER_WIDTH 0, BUSER_WIDTH 0, CLK_DOMAIN bd_0_ap_clk_0, DATA_WIDTH 512, FREQ_HZ 300000000.0, HAS_BRESP 1, HAS_BURST 0, HAS_CACHE 1, HAS_LOCK 1, HAS_PROT 1, HAS_QOS 1, HAS_REGION 1, HAS_RRESP 1, HAS_WSTRB 1, ID_WIDTH 0, INSERT_VIP 0, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 16, NUM_READ_THREADS 1, NUM_WRITE_OUTSTANDING 16, NUM_WRITE_THREADS 1, PHASE 0.000, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, RUSER_BITS_PER_BYTE 0, RUSER_WIDTH 0, SUPPORTS_NARROW_BURST 0, WUSER_BITS_PER_BYTE 0, WUSER_WIDTH 0" *) output [63:0]m_axi_gmem1_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [1:0]m_axi_gmem1_arburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [3:0]m_axi_gmem1_arcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [7:0]m_axi_gmem1_arlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [1:0]m_axi_gmem1_arlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [2:0]m_axi_gmem1_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [3:0]m_axi_gmem1_arqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) input m_axi_gmem1_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [3:0]m_axi_gmem1_arregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [2:0]m_axi_gmem1_arsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output m_axi_gmem1_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [63:0]m_axi_gmem1_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [1:0]m_axi_gmem1_awburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [3:0]m_axi_gmem1_awcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [7:0]m_axi_gmem1_awlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [1:0]m_axi_gmem1_awlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [2:0]m_axi_gmem1_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [3:0]m_axi_gmem1_awqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) input m_axi_gmem1_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [3:0]m_axi_gmem1_awregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [2:0]m_axi_gmem1_awsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output m_axi_gmem1_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output m_axi_gmem1_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) input [1:0]m_axi_gmem1_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) input m_axi_gmem1_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) input [511:0]m_axi_gmem1_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) input m_axi_gmem1_rlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output m_axi_gmem1_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) input [1:0]m_axi_gmem1_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) input m_axi_gmem1_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [511:0]m_axi_gmem1_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output m_axi_gmem1_wlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) input m_axi_gmem1_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output [63:0]m_axi_gmem1_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem1 " *) output m_axi_gmem1_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_gmem2, ADDR_WIDTH 64, ARUSER_WIDTH 0, AWUSER_WIDTH 0, BUSER_WIDTH 0, CLK_DOMAIN bd_0_ap_clk_0, DATA_WIDTH 32, FREQ_HZ 300000000.0, HAS_BRESP 1, HAS_BURST 0, HAS_CACHE 1, HAS_LOCK 1, HAS_PROT 1, HAS_QOS 1, HAS_REGION 1, HAS_RRESP 1, HAS_WSTRB 1, ID_WIDTH 0, INSERT_VIP 0, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 16, NUM_READ_THREADS 1, NUM_WRITE_OUTSTANDING 16, NUM_WRITE_THREADS 1, PHASE 0.000, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, RUSER_BITS_PER_BYTE 0, RUSER_WIDTH 0, SUPPORTS_NARROW_BURST 0, WUSER_BITS_PER_BYTE 0, WUSER_WIDTH 0" *) output [63:0]m_axi_gmem2_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [1:0]m_axi_gmem2_arburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [3:0]m_axi_gmem2_arcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [7:0]m_axi_gmem2_arlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [1:0]m_axi_gmem2_arlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [2:0]m_axi_gmem2_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [3:0]m_axi_gmem2_arqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) input m_axi_gmem2_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [3:0]m_axi_gmem2_arregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [2:0]m_axi_gmem2_arsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output m_axi_gmem2_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [63:0]m_axi_gmem2_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [1:0]m_axi_gmem2_awburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [3:0]m_axi_gmem2_awcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [7:0]m_axi_gmem2_awlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [1:0]m_axi_gmem2_awlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [2:0]m_axi_gmem2_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [3:0]m_axi_gmem2_awqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) input m_axi_gmem2_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [3:0]m_axi_gmem2_awregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [2:0]m_axi_gmem2_awsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output m_axi_gmem2_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output m_axi_gmem2_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) input [1:0]m_axi_gmem2_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) input m_axi_gmem2_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) input [31:0]m_axi_gmem2_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) input m_axi_gmem2_rlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output m_axi_gmem2_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) input [1:0]m_axi_gmem2_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) input m_axi_gmem2_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [31:0]m_axi_gmem2_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output m_axi_gmem2_wlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) input m_axi_gmem2_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output [3:0]m_axi_gmem2_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem2 " *) output m_axi_gmem2_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_gmem3, ADDR_WIDTH 64, ARUSER_WIDTH 0, AWUSER_WIDTH 0, BUSER_WIDTH 0, CLK_DOMAIN bd_0_ap_clk_0, DATA_WIDTH 32, FREQ_HZ 300000000.0, HAS_BRESP 1, HAS_BURST 0, HAS_CACHE 1, HAS_LOCK 1, HAS_PROT 1, HAS_QOS 1, HAS_REGION 1, HAS_RRESP 1, HAS_WSTRB 1, ID_WIDTH 0, INSERT_VIP 0, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 16, NUM_READ_THREADS 1, NUM_WRITE_OUTSTANDING 16, NUM_WRITE_THREADS 1, PHASE 0.000, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, RUSER_BITS_PER_BYTE 0, RUSER_WIDTH 0, SUPPORTS_NARROW_BURST 0, WUSER_BITS_PER_BYTE 0, WUSER_WIDTH 0" *) output [63:0]m_axi_gmem3_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [1:0]m_axi_gmem3_arburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [3:0]m_axi_gmem3_arcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [7:0]m_axi_gmem3_arlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [1:0]m_axi_gmem3_arlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [2:0]m_axi_gmem3_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [3:0]m_axi_gmem3_arqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) input m_axi_gmem3_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [3:0]m_axi_gmem3_arregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [2:0]m_axi_gmem3_arsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output m_axi_gmem3_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [63:0]m_axi_gmem3_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [1:0]m_axi_gmem3_awburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [3:0]m_axi_gmem3_awcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [7:0]m_axi_gmem3_awlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [1:0]m_axi_gmem3_awlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [2:0]m_axi_gmem3_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [3:0]m_axi_gmem3_awqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) input m_axi_gmem3_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [3:0]m_axi_gmem3_awregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [2:0]m_axi_gmem3_awsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output m_axi_gmem3_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output m_axi_gmem3_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) input [1:0]m_axi_gmem3_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) input m_axi_gmem3_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) input [31:0]m_axi_gmem3_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) input m_axi_gmem3_rlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output m_axi_gmem3_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) input [1:0]m_axi_gmem3_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) input m_axi_gmem3_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [31:0]m_axi_gmem3_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output m_axi_gmem3_wlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) input m_axi_gmem3_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output [3:0]m_axi_gmem3_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem3 " *) output m_axi_gmem3_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_gmem4, ADDR_WIDTH 64, ARUSER_WIDTH 0, AWUSER_WIDTH 0, BUSER_WIDTH 0, CLK_DOMAIN bd_0_ap_clk_0, DATA_WIDTH 32, FREQ_HZ 300000000.0, HAS_BRESP 1, HAS_BURST 0, HAS_CACHE 1, HAS_LOCK 1, HAS_PROT 1, HAS_QOS 1, HAS_REGION 1, HAS_RRESP 1, HAS_WSTRB 1, ID_WIDTH 0, INSERT_VIP 0, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 16, NUM_READ_THREADS 1, NUM_WRITE_OUTSTANDING 16, NUM_WRITE_THREADS 1, PHASE 0.000, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, RUSER_BITS_PER_BYTE 0, RUSER_WIDTH 0, SUPPORTS_NARROW_BURST 0, WUSER_BITS_PER_BYTE 0, WUSER_WIDTH 0" *) output [63:0]m_axi_gmem4_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [1:0]m_axi_gmem4_arburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [3:0]m_axi_gmem4_arcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [7:0]m_axi_gmem4_arlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [1:0]m_axi_gmem4_arlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [2:0]m_axi_gmem4_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [3:0]m_axi_gmem4_arqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) input m_axi_gmem4_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [3:0]m_axi_gmem4_arregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [2:0]m_axi_gmem4_arsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output m_axi_gmem4_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [63:0]m_axi_gmem4_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [1:0]m_axi_gmem4_awburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [3:0]m_axi_gmem4_awcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [7:0]m_axi_gmem4_awlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [1:0]m_axi_gmem4_awlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [2:0]m_axi_gmem4_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [3:0]m_axi_gmem4_awqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) input m_axi_gmem4_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [3:0]m_axi_gmem4_awregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [2:0]m_axi_gmem4_awsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output m_axi_gmem4_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output m_axi_gmem4_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) input [1:0]m_axi_gmem4_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) input m_axi_gmem4_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) input [31:0]m_axi_gmem4_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) input m_axi_gmem4_rlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output m_axi_gmem4_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) input [1:0]m_axi_gmem4_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) input m_axi_gmem4_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [31:0]m_axi_gmem4_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output m_axi_gmem4_wlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) input m_axi_gmem4_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output [3:0]m_axi_gmem4_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem4 " *) output m_axi_gmem4_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_gmem5, ADDR_WIDTH 64, ARUSER_WIDTH 0, AWUSER_WIDTH 0, BUSER_WIDTH 0, CLK_DOMAIN bd_0_ap_clk_0, DATA_WIDTH 32, FREQ_HZ 300000000.0, HAS_BRESP 1, HAS_BURST 0, HAS_CACHE 1, HAS_LOCK 1, HAS_PROT 1, HAS_QOS 1, HAS_REGION 1, HAS_RRESP 1, HAS_WSTRB 1, ID_WIDTH 0, INSERT_VIP 0, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 16, NUM_READ_THREADS 1, NUM_WRITE_OUTSTANDING 16, NUM_WRITE_THREADS 1, PHASE 0.000, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, RUSER_BITS_PER_BYTE 0, RUSER_WIDTH 0, SUPPORTS_NARROW_BURST 0, WUSER_BITS_PER_BYTE 0, WUSER_WIDTH 0" *) output [63:0]m_axi_gmem5_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [1:0]m_axi_gmem5_arburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [3:0]m_axi_gmem5_arcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [7:0]m_axi_gmem5_arlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [1:0]m_axi_gmem5_arlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [2:0]m_axi_gmem5_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [3:0]m_axi_gmem5_arqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) input m_axi_gmem5_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [3:0]m_axi_gmem5_arregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [2:0]m_axi_gmem5_arsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output m_axi_gmem5_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [63:0]m_axi_gmem5_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [1:0]m_axi_gmem5_awburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [3:0]m_axi_gmem5_awcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [7:0]m_axi_gmem5_awlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [1:0]m_axi_gmem5_awlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [2:0]m_axi_gmem5_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [3:0]m_axi_gmem5_awqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) input m_axi_gmem5_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [3:0]m_axi_gmem5_awregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [2:0]m_axi_gmem5_awsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output m_axi_gmem5_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output m_axi_gmem5_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) input [1:0]m_axi_gmem5_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) input m_axi_gmem5_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) input [31:0]m_axi_gmem5_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) input m_axi_gmem5_rlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output m_axi_gmem5_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) input [1:0]m_axi_gmem5_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) input m_axi_gmem5_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [31:0]m_axi_gmem5_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output m_axi_gmem5_wlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) input m_axi_gmem5_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output [3:0]m_axi_gmem5_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem5 " *) output m_axi_gmem5_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axi_gmem, ADDR_WIDTH 64, ARUSER_WIDTH 0, AWUSER_WIDTH 0, BUSER_WIDTH 0, CLK_DOMAIN bd_0_ap_clk_0, DATA_WIDTH 32, FREQ_HZ 300000000.0, HAS_BRESP 1, HAS_BURST 0, HAS_CACHE 1, HAS_LOCK 1, HAS_PROT 1, HAS_QOS 1, HAS_REGION 1, HAS_RRESP 1, HAS_WSTRB 1, ID_WIDTH 0, INSERT_VIP 0, MAX_BURST_LENGTH 256, NUM_READ_OUTSTANDING 16, NUM_READ_THREADS 1, NUM_WRITE_OUTSTANDING 16, NUM_WRITE_THREADS 1, PHASE 0.000, PROTOCOL AXI4, READ_WRITE_MODE READ_WRITE, RUSER_BITS_PER_BYTE 0, RUSER_WIDTH 0, SUPPORTS_NARROW_BURST 0, WUSER_BITS_PER_BYTE 0, WUSER_WIDTH 0" *) output [63:0]m_axi_gmem_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [1:0]m_axi_gmem_arburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [3:0]m_axi_gmem_arcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [7:0]m_axi_gmem_arlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [1:0]m_axi_gmem_arlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [2:0]m_axi_gmem_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [3:0]m_axi_gmem_arqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) input m_axi_gmem_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [3:0]m_axi_gmem_arregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [2:0]m_axi_gmem_arsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output m_axi_gmem_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [63:0]m_axi_gmem_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [1:0]m_axi_gmem_awburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [3:0]m_axi_gmem_awcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [7:0]m_axi_gmem_awlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [1:0]m_axi_gmem_awlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [2:0]m_axi_gmem_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [3:0]m_axi_gmem_awqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) input m_axi_gmem_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [3:0]m_axi_gmem_awregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [2:0]m_axi_gmem_awsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output m_axi_gmem_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output m_axi_gmem_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) input [1:0]m_axi_gmem_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) input m_axi_gmem_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) input [31:0]m_axi_gmem_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) input m_axi_gmem_rlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output m_axi_gmem_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) input [1:0]m_axi_gmem_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) input m_axi_gmem_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [31:0]m_axi_gmem_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output m_axi_gmem_wlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) input m_axi_gmem_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output [3:0]m_axi_gmem_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m_axi_gmem " *) output m_axi_gmem_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME s_axi_control, ADDR_WIDTH 32, ARUSER_WIDTH 0, AWUSER_WIDTH 0, BUSER_WIDTH 0, CLK_DOMAIN bd_0_ap_clk_0, DATA_WIDTH 32, FREQ_HZ 300000000.0, HAS_BRESP 1, HAS_BURST 0, HAS_CACHE 0, HAS_LOCK 0, HAS_PROT 0, HAS_QOS 0, HAS_REGION 0, HAS_RRESP 1, HAS_WSTRB 1, ID_WIDTH 0, INSERT_VIP 0, MAX_BURST_LENGTH 1, NUM_READ_OUTSTANDING 1, NUM_READ_THREADS 1, NUM_WRITE_OUTSTANDING 1, NUM_WRITE_THREADS 1, PHASE 0.000, PROTOCOL AXI4LITE, READ_WRITE_MODE READ_WRITE, RUSER_BITS_PER_BYTE 0, RUSER_WIDTH 0, SUPPORTS_NARROW_BURST 0, WUSER_BITS_PER_BYTE 0, WUSER_WIDTH 0" *) input [6:0]s_axi_control_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) output s_axi_control_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) input s_axi_control_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) input [6:0]s_axi_control_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) output s_axi_control_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) input s_axi_control_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) input s_axi_control_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) output [1:0]s_axi_control_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) output s_axi_control_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) output [31:0]s_axi_control_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) input s_axi_control_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) output [1:0]s_axi_control_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) output s_axi_control_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) input [31:0]s_axi_control_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) output s_axi_control_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) input [3:0]s_axi_control_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi_control " *) input s_axi_control_wvalid;

  wire ap_clk_0_1;
  wire ap_rst_n_0_1;
  wire hls_inst_interrupt;
  wire [63:0]hls_inst_m_axi_gmem1_ARADDR;
  wire [1:0]hls_inst_m_axi_gmem1_ARBURST;
  wire [3:0]hls_inst_m_axi_gmem1_ARCACHE;
  wire [7:0]hls_inst_m_axi_gmem1_ARLEN;
  wire [1:0]hls_inst_m_axi_gmem1_ARLOCK;
  wire [2:0]hls_inst_m_axi_gmem1_ARPROT;
  wire [3:0]hls_inst_m_axi_gmem1_ARQOS;
  wire hls_inst_m_axi_gmem1_ARREADY;
  wire [3:0]hls_inst_m_axi_gmem1_ARREGION;
  wire [2:0]hls_inst_m_axi_gmem1_ARSIZE;
  wire hls_inst_m_axi_gmem1_ARVALID;
  wire [63:0]hls_inst_m_axi_gmem1_AWADDR;
  wire [1:0]hls_inst_m_axi_gmem1_AWBURST;
  wire [3:0]hls_inst_m_axi_gmem1_AWCACHE;
  wire [7:0]hls_inst_m_axi_gmem1_AWLEN;
  wire [1:0]hls_inst_m_axi_gmem1_AWLOCK;
  wire [2:0]hls_inst_m_axi_gmem1_AWPROT;
  wire [3:0]hls_inst_m_axi_gmem1_AWQOS;
  wire hls_inst_m_axi_gmem1_AWREADY;
  wire [3:0]hls_inst_m_axi_gmem1_AWREGION;
  wire [2:0]hls_inst_m_axi_gmem1_AWSIZE;
  wire hls_inst_m_axi_gmem1_AWVALID;
  wire hls_inst_m_axi_gmem1_BREADY;
  wire [1:0]hls_inst_m_axi_gmem1_BRESP;
  wire hls_inst_m_axi_gmem1_BVALID;
  wire [511:0]hls_inst_m_axi_gmem1_RDATA;
  wire hls_inst_m_axi_gmem1_RLAST;
  wire hls_inst_m_axi_gmem1_RREADY;
  wire [1:0]hls_inst_m_axi_gmem1_RRESP;
  wire hls_inst_m_axi_gmem1_RVALID;
  wire [511:0]hls_inst_m_axi_gmem1_WDATA;
  wire hls_inst_m_axi_gmem1_WLAST;
  wire hls_inst_m_axi_gmem1_WREADY;
  wire [63:0]hls_inst_m_axi_gmem1_WSTRB;
  wire hls_inst_m_axi_gmem1_WVALID;
  wire [63:0]hls_inst_m_axi_gmem2_ARADDR;
  wire [1:0]hls_inst_m_axi_gmem2_ARBURST;
  wire [3:0]hls_inst_m_axi_gmem2_ARCACHE;
  wire [7:0]hls_inst_m_axi_gmem2_ARLEN;
  wire [1:0]hls_inst_m_axi_gmem2_ARLOCK;
  wire [2:0]hls_inst_m_axi_gmem2_ARPROT;
  wire [3:0]hls_inst_m_axi_gmem2_ARQOS;
  wire hls_inst_m_axi_gmem2_ARREADY;
  wire [3:0]hls_inst_m_axi_gmem2_ARREGION;
  wire [2:0]hls_inst_m_axi_gmem2_ARSIZE;
  wire hls_inst_m_axi_gmem2_ARVALID;
  wire [63:0]hls_inst_m_axi_gmem2_AWADDR;
  wire [1:0]hls_inst_m_axi_gmem2_AWBURST;
  wire [3:0]hls_inst_m_axi_gmem2_AWCACHE;
  wire [7:0]hls_inst_m_axi_gmem2_AWLEN;
  wire [1:0]hls_inst_m_axi_gmem2_AWLOCK;
  wire [2:0]hls_inst_m_axi_gmem2_AWPROT;
  wire [3:0]hls_inst_m_axi_gmem2_AWQOS;
  wire hls_inst_m_axi_gmem2_AWREADY;
  wire [3:0]hls_inst_m_axi_gmem2_AWREGION;
  wire [2:0]hls_inst_m_axi_gmem2_AWSIZE;
  wire hls_inst_m_axi_gmem2_AWVALID;
  wire hls_inst_m_axi_gmem2_BREADY;
  wire [1:0]hls_inst_m_axi_gmem2_BRESP;
  wire hls_inst_m_axi_gmem2_BVALID;
  wire [31:0]hls_inst_m_axi_gmem2_RDATA;
  wire hls_inst_m_axi_gmem2_RLAST;
  wire hls_inst_m_axi_gmem2_RREADY;
  wire [1:0]hls_inst_m_axi_gmem2_RRESP;
  wire hls_inst_m_axi_gmem2_RVALID;
  wire [31:0]hls_inst_m_axi_gmem2_WDATA;
  wire hls_inst_m_axi_gmem2_WLAST;
  wire hls_inst_m_axi_gmem2_WREADY;
  wire [3:0]hls_inst_m_axi_gmem2_WSTRB;
  wire hls_inst_m_axi_gmem2_WVALID;
  wire [63:0]hls_inst_m_axi_gmem3_ARADDR;
  wire [1:0]hls_inst_m_axi_gmem3_ARBURST;
  wire [3:0]hls_inst_m_axi_gmem3_ARCACHE;
  wire [7:0]hls_inst_m_axi_gmem3_ARLEN;
  wire [1:0]hls_inst_m_axi_gmem3_ARLOCK;
  wire [2:0]hls_inst_m_axi_gmem3_ARPROT;
  wire [3:0]hls_inst_m_axi_gmem3_ARQOS;
  wire hls_inst_m_axi_gmem3_ARREADY;
  wire [3:0]hls_inst_m_axi_gmem3_ARREGION;
  wire [2:0]hls_inst_m_axi_gmem3_ARSIZE;
  wire hls_inst_m_axi_gmem3_ARVALID;
  wire [63:0]hls_inst_m_axi_gmem3_AWADDR;
  wire [1:0]hls_inst_m_axi_gmem3_AWBURST;
  wire [3:0]hls_inst_m_axi_gmem3_AWCACHE;
  wire [7:0]hls_inst_m_axi_gmem3_AWLEN;
  wire [1:0]hls_inst_m_axi_gmem3_AWLOCK;
  wire [2:0]hls_inst_m_axi_gmem3_AWPROT;
  wire [3:0]hls_inst_m_axi_gmem3_AWQOS;
  wire hls_inst_m_axi_gmem3_AWREADY;
  wire [3:0]hls_inst_m_axi_gmem3_AWREGION;
  wire [2:0]hls_inst_m_axi_gmem3_AWSIZE;
  wire hls_inst_m_axi_gmem3_AWVALID;
  wire hls_inst_m_axi_gmem3_BREADY;
  wire [1:0]hls_inst_m_axi_gmem3_BRESP;
  wire hls_inst_m_axi_gmem3_BVALID;
  wire [31:0]hls_inst_m_axi_gmem3_RDATA;
  wire hls_inst_m_axi_gmem3_RLAST;
  wire hls_inst_m_axi_gmem3_RREADY;
  wire [1:0]hls_inst_m_axi_gmem3_RRESP;
  wire hls_inst_m_axi_gmem3_RVALID;
  wire [31:0]hls_inst_m_axi_gmem3_WDATA;
  wire hls_inst_m_axi_gmem3_WLAST;
  wire hls_inst_m_axi_gmem3_WREADY;
  wire [3:0]hls_inst_m_axi_gmem3_WSTRB;
  wire hls_inst_m_axi_gmem3_WVALID;
  wire [63:0]hls_inst_m_axi_gmem4_ARADDR;
  wire [1:0]hls_inst_m_axi_gmem4_ARBURST;
  wire [3:0]hls_inst_m_axi_gmem4_ARCACHE;
  wire [7:0]hls_inst_m_axi_gmem4_ARLEN;
  wire [1:0]hls_inst_m_axi_gmem4_ARLOCK;
  wire [2:0]hls_inst_m_axi_gmem4_ARPROT;
  wire [3:0]hls_inst_m_axi_gmem4_ARQOS;
  wire hls_inst_m_axi_gmem4_ARREADY;
  wire [3:0]hls_inst_m_axi_gmem4_ARREGION;
  wire [2:0]hls_inst_m_axi_gmem4_ARSIZE;
  wire hls_inst_m_axi_gmem4_ARVALID;
  wire [63:0]hls_inst_m_axi_gmem4_AWADDR;
  wire [1:0]hls_inst_m_axi_gmem4_AWBURST;
  wire [3:0]hls_inst_m_axi_gmem4_AWCACHE;
  wire [7:0]hls_inst_m_axi_gmem4_AWLEN;
  wire [1:0]hls_inst_m_axi_gmem4_AWLOCK;
  wire [2:0]hls_inst_m_axi_gmem4_AWPROT;
  wire [3:0]hls_inst_m_axi_gmem4_AWQOS;
  wire hls_inst_m_axi_gmem4_AWREADY;
  wire [3:0]hls_inst_m_axi_gmem4_AWREGION;
  wire [2:0]hls_inst_m_axi_gmem4_AWSIZE;
  wire hls_inst_m_axi_gmem4_AWVALID;
  wire hls_inst_m_axi_gmem4_BREADY;
  wire [1:0]hls_inst_m_axi_gmem4_BRESP;
  wire hls_inst_m_axi_gmem4_BVALID;
  wire [31:0]hls_inst_m_axi_gmem4_RDATA;
  wire hls_inst_m_axi_gmem4_RLAST;
  wire hls_inst_m_axi_gmem4_RREADY;
  wire [1:0]hls_inst_m_axi_gmem4_RRESP;
  wire hls_inst_m_axi_gmem4_RVALID;
  wire [31:0]hls_inst_m_axi_gmem4_WDATA;
  wire hls_inst_m_axi_gmem4_WLAST;
  wire hls_inst_m_axi_gmem4_WREADY;
  wire [3:0]hls_inst_m_axi_gmem4_WSTRB;
  wire hls_inst_m_axi_gmem4_WVALID;
  wire [63:0]hls_inst_m_axi_gmem5_ARADDR;
  wire [1:0]hls_inst_m_axi_gmem5_ARBURST;
  wire [3:0]hls_inst_m_axi_gmem5_ARCACHE;
  wire [7:0]hls_inst_m_axi_gmem5_ARLEN;
  wire [1:0]hls_inst_m_axi_gmem5_ARLOCK;
  wire [2:0]hls_inst_m_axi_gmem5_ARPROT;
  wire [3:0]hls_inst_m_axi_gmem5_ARQOS;
  wire hls_inst_m_axi_gmem5_ARREADY;
  wire [3:0]hls_inst_m_axi_gmem5_ARREGION;
  wire [2:0]hls_inst_m_axi_gmem5_ARSIZE;
  wire hls_inst_m_axi_gmem5_ARVALID;
  wire [63:0]hls_inst_m_axi_gmem5_AWADDR;
  wire [1:0]hls_inst_m_axi_gmem5_AWBURST;
  wire [3:0]hls_inst_m_axi_gmem5_AWCACHE;
  wire [7:0]hls_inst_m_axi_gmem5_AWLEN;
  wire [1:0]hls_inst_m_axi_gmem5_AWLOCK;
  wire [2:0]hls_inst_m_axi_gmem5_AWPROT;
  wire [3:0]hls_inst_m_axi_gmem5_AWQOS;
  wire hls_inst_m_axi_gmem5_AWREADY;
  wire [3:0]hls_inst_m_axi_gmem5_AWREGION;
  wire [2:0]hls_inst_m_axi_gmem5_AWSIZE;
  wire hls_inst_m_axi_gmem5_AWVALID;
  wire hls_inst_m_axi_gmem5_BREADY;
  wire [1:0]hls_inst_m_axi_gmem5_BRESP;
  wire hls_inst_m_axi_gmem5_BVALID;
  wire [31:0]hls_inst_m_axi_gmem5_RDATA;
  wire hls_inst_m_axi_gmem5_RLAST;
  wire hls_inst_m_axi_gmem5_RREADY;
  wire [1:0]hls_inst_m_axi_gmem5_RRESP;
  wire hls_inst_m_axi_gmem5_RVALID;
  wire [31:0]hls_inst_m_axi_gmem5_WDATA;
  wire hls_inst_m_axi_gmem5_WLAST;
  wire hls_inst_m_axi_gmem5_WREADY;
  wire [3:0]hls_inst_m_axi_gmem5_WSTRB;
  wire hls_inst_m_axi_gmem5_WVALID;
  wire [63:0]hls_inst_m_axi_gmem_ARADDR;
  wire [1:0]hls_inst_m_axi_gmem_ARBURST;
  wire [3:0]hls_inst_m_axi_gmem_ARCACHE;
  wire [7:0]hls_inst_m_axi_gmem_ARLEN;
  wire [1:0]hls_inst_m_axi_gmem_ARLOCK;
  wire [2:0]hls_inst_m_axi_gmem_ARPROT;
  wire [3:0]hls_inst_m_axi_gmem_ARQOS;
  wire hls_inst_m_axi_gmem_ARREADY;
  wire [3:0]hls_inst_m_axi_gmem_ARREGION;
  wire [2:0]hls_inst_m_axi_gmem_ARSIZE;
  wire hls_inst_m_axi_gmem_ARVALID;
  wire [63:0]hls_inst_m_axi_gmem_AWADDR;
  wire [1:0]hls_inst_m_axi_gmem_AWBURST;
  wire [3:0]hls_inst_m_axi_gmem_AWCACHE;
  wire [7:0]hls_inst_m_axi_gmem_AWLEN;
  wire [1:0]hls_inst_m_axi_gmem_AWLOCK;
  wire [2:0]hls_inst_m_axi_gmem_AWPROT;
  wire [3:0]hls_inst_m_axi_gmem_AWQOS;
  wire hls_inst_m_axi_gmem_AWREADY;
  wire [3:0]hls_inst_m_axi_gmem_AWREGION;
  wire [2:0]hls_inst_m_axi_gmem_AWSIZE;
  wire hls_inst_m_axi_gmem_AWVALID;
  wire hls_inst_m_axi_gmem_BREADY;
  wire [1:0]hls_inst_m_axi_gmem_BRESP;
  wire hls_inst_m_axi_gmem_BVALID;
  wire [31:0]hls_inst_m_axi_gmem_RDATA;
  wire hls_inst_m_axi_gmem_RLAST;
  wire hls_inst_m_axi_gmem_RREADY;
  wire [1:0]hls_inst_m_axi_gmem_RRESP;
  wire hls_inst_m_axi_gmem_RVALID;
  wire [31:0]hls_inst_m_axi_gmem_WDATA;
  wire hls_inst_m_axi_gmem_WLAST;
  wire hls_inst_m_axi_gmem_WREADY;
  wire [3:0]hls_inst_m_axi_gmem_WSTRB;
  wire hls_inst_m_axi_gmem_WVALID;
  wire [6:0]s_axi_control_0_1_ARADDR;
  wire s_axi_control_0_1_ARREADY;
  wire s_axi_control_0_1_ARVALID;
  wire [6:0]s_axi_control_0_1_AWADDR;
  wire s_axi_control_0_1_AWREADY;
  wire s_axi_control_0_1_AWVALID;
  wire s_axi_control_0_1_BREADY;
  wire [1:0]s_axi_control_0_1_BRESP;
  wire s_axi_control_0_1_BVALID;
  wire [31:0]s_axi_control_0_1_RDATA;
  wire s_axi_control_0_1_RREADY;
  wire [1:0]s_axi_control_0_1_RRESP;
  wire s_axi_control_0_1_RVALID;
  wire [31:0]s_axi_control_0_1_WDATA;
  wire s_axi_control_0_1_WREADY;
  wire [3:0]s_axi_control_0_1_WSTRB;
  wire s_axi_control_0_1_WVALID;

  assign ap_clk_0_1 = ap_clk;
  assign ap_rst_n_0_1 = ap_rst_n;
  assign hls_inst_m_axi_gmem1_ARREADY = m_axi_gmem1_arready;
  assign hls_inst_m_axi_gmem1_AWREADY = m_axi_gmem1_awready;
  assign hls_inst_m_axi_gmem1_BRESP = m_axi_gmem1_bresp[1:0];
  assign hls_inst_m_axi_gmem1_BVALID = m_axi_gmem1_bvalid;
  assign hls_inst_m_axi_gmem1_RDATA = m_axi_gmem1_rdata[511:0];
  assign hls_inst_m_axi_gmem1_RLAST = m_axi_gmem1_rlast;
  assign hls_inst_m_axi_gmem1_RRESP = m_axi_gmem1_rresp[1:0];
  assign hls_inst_m_axi_gmem1_RVALID = m_axi_gmem1_rvalid;
  assign hls_inst_m_axi_gmem1_WREADY = m_axi_gmem1_wready;
  assign hls_inst_m_axi_gmem2_ARREADY = m_axi_gmem2_arready;
  assign hls_inst_m_axi_gmem2_AWREADY = m_axi_gmem2_awready;
  assign hls_inst_m_axi_gmem2_BRESP = m_axi_gmem2_bresp[1:0];
  assign hls_inst_m_axi_gmem2_BVALID = m_axi_gmem2_bvalid;
  assign hls_inst_m_axi_gmem2_RDATA = m_axi_gmem2_rdata[31:0];
  assign hls_inst_m_axi_gmem2_RLAST = m_axi_gmem2_rlast;
  assign hls_inst_m_axi_gmem2_RRESP = m_axi_gmem2_rresp[1:0];
  assign hls_inst_m_axi_gmem2_RVALID = m_axi_gmem2_rvalid;
  assign hls_inst_m_axi_gmem2_WREADY = m_axi_gmem2_wready;
  assign hls_inst_m_axi_gmem3_ARREADY = m_axi_gmem3_arready;
  assign hls_inst_m_axi_gmem3_AWREADY = m_axi_gmem3_awready;
  assign hls_inst_m_axi_gmem3_BRESP = m_axi_gmem3_bresp[1:0];
  assign hls_inst_m_axi_gmem3_BVALID = m_axi_gmem3_bvalid;
  assign hls_inst_m_axi_gmem3_RDATA = m_axi_gmem3_rdata[31:0];
  assign hls_inst_m_axi_gmem3_RLAST = m_axi_gmem3_rlast;
  assign hls_inst_m_axi_gmem3_RRESP = m_axi_gmem3_rresp[1:0];
  assign hls_inst_m_axi_gmem3_RVALID = m_axi_gmem3_rvalid;
  assign hls_inst_m_axi_gmem3_WREADY = m_axi_gmem3_wready;
  assign hls_inst_m_axi_gmem4_ARREADY = m_axi_gmem4_arready;
  assign hls_inst_m_axi_gmem4_AWREADY = m_axi_gmem4_awready;
  assign hls_inst_m_axi_gmem4_BRESP = m_axi_gmem4_bresp[1:0];
  assign hls_inst_m_axi_gmem4_BVALID = m_axi_gmem4_bvalid;
  assign hls_inst_m_axi_gmem4_RDATA = m_axi_gmem4_rdata[31:0];
  assign hls_inst_m_axi_gmem4_RLAST = m_axi_gmem4_rlast;
  assign hls_inst_m_axi_gmem4_RRESP = m_axi_gmem4_rresp[1:0];
  assign hls_inst_m_axi_gmem4_RVALID = m_axi_gmem4_rvalid;
  assign hls_inst_m_axi_gmem4_WREADY = m_axi_gmem4_wready;
  assign hls_inst_m_axi_gmem5_ARREADY = m_axi_gmem5_arready;
  assign hls_inst_m_axi_gmem5_AWREADY = m_axi_gmem5_awready;
  assign hls_inst_m_axi_gmem5_BRESP = m_axi_gmem5_bresp[1:0];
  assign hls_inst_m_axi_gmem5_BVALID = m_axi_gmem5_bvalid;
  assign hls_inst_m_axi_gmem5_RDATA = m_axi_gmem5_rdata[31:0];
  assign hls_inst_m_axi_gmem5_RLAST = m_axi_gmem5_rlast;
  assign hls_inst_m_axi_gmem5_RRESP = m_axi_gmem5_rresp[1:0];
  assign hls_inst_m_axi_gmem5_RVALID = m_axi_gmem5_rvalid;
  assign hls_inst_m_axi_gmem5_WREADY = m_axi_gmem5_wready;
  assign hls_inst_m_axi_gmem_ARREADY = m_axi_gmem_arready;
  assign hls_inst_m_axi_gmem_AWREADY = m_axi_gmem_awready;
  assign hls_inst_m_axi_gmem_BRESP = m_axi_gmem_bresp[1:0];
  assign hls_inst_m_axi_gmem_BVALID = m_axi_gmem_bvalid;
  assign hls_inst_m_axi_gmem_RDATA = m_axi_gmem_rdata[31:0];
  assign hls_inst_m_axi_gmem_RLAST = m_axi_gmem_rlast;
  assign hls_inst_m_axi_gmem_RRESP = m_axi_gmem_rresp[1:0];
  assign hls_inst_m_axi_gmem_RVALID = m_axi_gmem_rvalid;
  assign hls_inst_m_axi_gmem_WREADY = m_axi_gmem_wready;
  assign interrupt = hls_inst_interrupt;
  assign m_axi_gmem1_araddr[63:0] = hls_inst_m_axi_gmem1_ARADDR;
  assign m_axi_gmem1_arburst[1:0] = hls_inst_m_axi_gmem1_ARBURST;
  assign m_axi_gmem1_arcache[3:0] = hls_inst_m_axi_gmem1_ARCACHE;
  assign m_axi_gmem1_arlen[7:0] = hls_inst_m_axi_gmem1_ARLEN;
  assign m_axi_gmem1_arlock[1:0] = hls_inst_m_axi_gmem1_ARLOCK;
  assign m_axi_gmem1_arprot[2:0] = hls_inst_m_axi_gmem1_ARPROT;
  assign m_axi_gmem1_arqos[3:0] = hls_inst_m_axi_gmem1_ARQOS;
  assign m_axi_gmem1_arregion[3:0] = hls_inst_m_axi_gmem1_ARREGION;
  assign m_axi_gmem1_arsize[2:0] = hls_inst_m_axi_gmem1_ARSIZE;
  assign m_axi_gmem1_arvalid = hls_inst_m_axi_gmem1_ARVALID;
  assign m_axi_gmem1_awaddr[63:0] = hls_inst_m_axi_gmem1_AWADDR;
  assign m_axi_gmem1_awburst[1:0] = hls_inst_m_axi_gmem1_AWBURST;
  assign m_axi_gmem1_awcache[3:0] = hls_inst_m_axi_gmem1_AWCACHE;
  assign m_axi_gmem1_awlen[7:0] = hls_inst_m_axi_gmem1_AWLEN;
  assign m_axi_gmem1_awlock[1:0] = hls_inst_m_axi_gmem1_AWLOCK;
  assign m_axi_gmem1_awprot[2:0] = hls_inst_m_axi_gmem1_AWPROT;
  assign m_axi_gmem1_awqos[3:0] = hls_inst_m_axi_gmem1_AWQOS;
  assign m_axi_gmem1_awregion[3:0] = hls_inst_m_axi_gmem1_AWREGION;
  assign m_axi_gmem1_awsize[2:0] = hls_inst_m_axi_gmem1_AWSIZE;
  assign m_axi_gmem1_awvalid = hls_inst_m_axi_gmem1_AWVALID;
  assign m_axi_gmem1_bready = hls_inst_m_axi_gmem1_BREADY;
  assign m_axi_gmem1_rready = hls_inst_m_axi_gmem1_RREADY;
  assign m_axi_gmem1_wdata[511:0] = hls_inst_m_axi_gmem1_WDATA;
  assign m_axi_gmem1_wlast = hls_inst_m_axi_gmem1_WLAST;
  assign m_axi_gmem1_wstrb[63:0] = hls_inst_m_axi_gmem1_WSTRB;
  assign m_axi_gmem1_wvalid = hls_inst_m_axi_gmem1_WVALID;
  assign m_axi_gmem2_araddr[63:0] = hls_inst_m_axi_gmem2_ARADDR;
  assign m_axi_gmem2_arburst[1:0] = hls_inst_m_axi_gmem2_ARBURST;
  assign m_axi_gmem2_arcache[3:0] = hls_inst_m_axi_gmem2_ARCACHE;
  assign m_axi_gmem2_arlen[7:0] = hls_inst_m_axi_gmem2_ARLEN;
  assign m_axi_gmem2_arlock[1:0] = hls_inst_m_axi_gmem2_ARLOCK;
  assign m_axi_gmem2_arprot[2:0] = hls_inst_m_axi_gmem2_ARPROT;
  assign m_axi_gmem2_arqos[3:0] = hls_inst_m_axi_gmem2_ARQOS;
  assign m_axi_gmem2_arregion[3:0] = hls_inst_m_axi_gmem2_ARREGION;
  assign m_axi_gmem2_arsize[2:0] = hls_inst_m_axi_gmem2_ARSIZE;
  assign m_axi_gmem2_arvalid = hls_inst_m_axi_gmem2_ARVALID;
  assign m_axi_gmem2_awaddr[63:0] = hls_inst_m_axi_gmem2_AWADDR;
  assign m_axi_gmem2_awburst[1:0] = hls_inst_m_axi_gmem2_AWBURST;
  assign m_axi_gmem2_awcache[3:0] = hls_inst_m_axi_gmem2_AWCACHE;
  assign m_axi_gmem2_awlen[7:0] = hls_inst_m_axi_gmem2_AWLEN;
  assign m_axi_gmem2_awlock[1:0] = hls_inst_m_axi_gmem2_AWLOCK;
  assign m_axi_gmem2_awprot[2:0] = hls_inst_m_axi_gmem2_AWPROT;
  assign m_axi_gmem2_awqos[3:0] = hls_inst_m_axi_gmem2_AWQOS;
  assign m_axi_gmem2_awregion[3:0] = hls_inst_m_axi_gmem2_AWREGION;
  assign m_axi_gmem2_awsize[2:0] = hls_inst_m_axi_gmem2_AWSIZE;
  assign m_axi_gmem2_awvalid = hls_inst_m_axi_gmem2_AWVALID;
  assign m_axi_gmem2_bready = hls_inst_m_axi_gmem2_BREADY;
  assign m_axi_gmem2_rready = hls_inst_m_axi_gmem2_RREADY;
  assign m_axi_gmem2_wdata[31:0] = hls_inst_m_axi_gmem2_WDATA;
  assign m_axi_gmem2_wlast = hls_inst_m_axi_gmem2_WLAST;
  assign m_axi_gmem2_wstrb[3:0] = hls_inst_m_axi_gmem2_WSTRB;
  assign m_axi_gmem2_wvalid = hls_inst_m_axi_gmem2_WVALID;
  assign m_axi_gmem3_araddr[63:0] = hls_inst_m_axi_gmem3_ARADDR;
  assign m_axi_gmem3_arburst[1:0] = hls_inst_m_axi_gmem3_ARBURST;
  assign m_axi_gmem3_arcache[3:0] = hls_inst_m_axi_gmem3_ARCACHE;
  assign m_axi_gmem3_arlen[7:0] = hls_inst_m_axi_gmem3_ARLEN;
  assign m_axi_gmem3_arlock[1:0] = hls_inst_m_axi_gmem3_ARLOCK;
  assign m_axi_gmem3_arprot[2:0] = hls_inst_m_axi_gmem3_ARPROT;
  assign m_axi_gmem3_arqos[3:0] = hls_inst_m_axi_gmem3_ARQOS;
  assign m_axi_gmem3_arregion[3:0] = hls_inst_m_axi_gmem3_ARREGION;
  assign m_axi_gmem3_arsize[2:0] = hls_inst_m_axi_gmem3_ARSIZE;
  assign m_axi_gmem3_arvalid = hls_inst_m_axi_gmem3_ARVALID;
  assign m_axi_gmem3_awaddr[63:0] = hls_inst_m_axi_gmem3_AWADDR;
  assign m_axi_gmem3_awburst[1:0] = hls_inst_m_axi_gmem3_AWBURST;
  assign m_axi_gmem3_awcache[3:0] = hls_inst_m_axi_gmem3_AWCACHE;
  assign m_axi_gmem3_awlen[7:0] = hls_inst_m_axi_gmem3_AWLEN;
  assign m_axi_gmem3_awlock[1:0] = hls_inst_m_axi_gmem3_AWLOCK;
  assign m_axi_gmem3_awprot[2:0] = hls_inst_m_axi_gmem3_AWPROT;
  assign m_axi_gmem3_awqos[3:0] = hls_inst_m_axi_gmem3_AWQOS;
  assign m_axi_gmem3_awregion[3:0] = hls_inst_m_axi_gmem3_AWREGION;
  assign m_axi_gmem3_awsize[2:0] = hls_inst_m_axi_gmem3_AWSIZE;
  assign m_axi_gmem3_awvalid = hls_inst_m_axi_gmem3_AWVALID;
  assign m_axi_gmem3_bready = hls_inst_m_axi_gmem3_BREADY;
  assign m_axi_gmem3_rready = hls_inst_m_axi_gmem3_RREADY;
  assign m_axi_gmem3_wdata[31:0] = hls_inst_m_axi_gmem3_WDATA;
  assign m_axi_gmem3_wlast = hls_inst_m_axi_gmem3_WLAST;
  assign m_axi_gmem3_wstrb[3:0] = hls_inst_m_axi_gmem3_WSTRB;
  assign m_axi_gmem3_wvalid = hls_inst_m_axi_gmem3_WVALID;
  assign m_axi_gmem4_araddr[63:0] = hls_inst_m_axi_gmem4_ARADDR;
  assign m_axi_gmem4_arburst[1:0] = hls_inst_m_axi_gmem4_ARBURST;
  assign m_axi_gmem4_arcache[3:0] = hls_inst_m_axi_gmem4_ARCACHE;
  assign m_axi_gmem4_arlen[7:0] = hls_inst_m_axi_gmem4_ARLEN;
  assign m_axi_gmem4_arlock[1:0] = hls_inst_m_axi_gmem4_ARLOCK;
  assign m_axi_gmem4_arprot[2:0] = hls_inst_m_axi_gmem4_ARPROT;
  assign m_axi_gmem4_arqos[3:0] = hls_inst_m_axi_gmem4_ARQOS;
  assign m_axi_gmem4_arregion[3:0] = hls_inst_m_axi_gmem4_ARREGION;
  assign m_axi_gmem4_arsize[2:0] = hls_inst_m_axi_gmem4_ARSIZE;
  assign m_axi_gmem4_arvalid = hls_inst_m_axi_gmem4_ARVALID;
  assign m_axi_gmem4_awaddr[63:0] = hls_inst_m_axi_gmem4_AWADDR;
  assign m_axi_gmem4_awburst[1:0] = hls_inst_m_axi_gmem4_AWBURST;
  assign m_axi_gmem4_awcache[3:0] = hls_inst_m_axi_gmem4_AWCACHE;
  assign m_axi_gmem4_awlen[7:0] = hls_inst_m_axi_gmem4_AWLEN;
  assign m_axi_gmem4_awlock[1:0] = hls_inst_m_axi_gmem4_AWLOCK;
  assign m_axi_gmem4_awprot[2:0] = hls_inst_m_axi_gmem4_AWPROT;
  assign m_axi_gmem4_awqos[3:0] = hls_inst_m_axi_gmem4_AWQOS;
  assign m_axi_gmem4_awregion[3:0] = hls_inst_m_axi_gmem4_AWREGION;
  assign m_axi_gmem4_awsize[2:0] = hls_inst_m_axi_gmem4_AWSIZE;
  assign m_axi_gmem4_awvalid = hls_inst_m_axi_gmem4_AWVALID;
  assign m_axi_gmem4_bready = hls_inst_m_axi_gmem4_BREADY;
  assign m_axi_gmem4_rready = hls_inst_m_axi_gmem4_RREADY;
  assign m_axi_gmem4_wdata[31:0] = hls_inst_m_axi_gmem4_WDATA;
  assign m_axi_gmem4_wlast = hls_inst_m_axi_gmem4_WLAST;
  assign m_axi_gmem4_wstrb[3:0] = hls_inst_m_axi_gmem4_WSTRB;
  assign m_axi_gmem4_wvalid = hls_inst_m_axi_gmem4_WVALID;
  assign m_axi_gmem5_araddr[63:0] = hls_inst_m_axi_gmem5_ARADDR;
  assign m_axi_gmem5_arburst[1:0] = hls_inst_m_axi_gmem5_ARBURST;
  assign m_axi_gmem5_arcache[3:0] = hls_inst_m_axi_gmem5_ARCACHE;
  assign m_axi_gmem5_arlen[7:0] = hls_inst_m_axi_gmem5_ARLEN;
  assign m_axi_gmem5_arlock[1:0] = hls_inst_m_axi_gmem5_ARLOCK;
  assign m_axi_gmem5_arprot[2:0] = hls_inst_m_axi_gmem5_ARPROT;
  assign m_axi_gmem5_arqos[3:0] = hls_inst_m_axi_gmem5_ARQOS;
  assign m_axi_gmem5_arregion[3:0] = hls_inst_m_axi_gmem5_ARREGION;
  assign m_axi_gmem5_arsize[2:0] = hls_inst_m_axi_gmem5_ARSIZE;
  assign m_axi_gmem5_arvalid = hls_inst_m_axi_gmem5_ARVALID;
  assign m_axi_gmem5_awaddr[63:0] = hls_inst_m_axi_gmem5_AWADDR;
  assign m_axi_gmem5_awburst[1:0] = hls_inst_m_axi_gmem5_AWBURST;
  assign m_axi_gmem5_awcache[3:0] = hls_inst_m_axi_gmem5_AWCACHE;
  assign m_axi_gmem5_awlen[7:0] = hls_inst_m_axi_gmem5_AWLEN;
  assign m_axi_gmem5_awlock[1:0] = hls_inst_m_axi_gmem5_AWLOCK;
  assign m_axi_gmem5_awprot[2:0] = hls_inst_m_axi_gmem5_AWPROT;
  assign m_axi_gmem5_awqos[3:0] = hls_inst_m_axi_gmem5_AWQOS;
  assign m_axi_gmem5_awregion[3:0] = hls_inst_m_axi_gmem5_AWREGION;
  assign m_axi_gmem5_awsize[2:0] = hls_inst_m_axi_gmem5_AWSIZE;
  assign m_axi_gmem5_awvalid = hls_inst_m_axi_gmem5_AWVALID;
  assign m_axi_gmem5_bready = hls_inst_m_axi_gmem5_BREADY;
  assign m_axi_gmem5_rready = hls_inst_m_axi_gmem5_RREADY;
  assign m_axi_gmem5_wdata[31:0] = hls_inst_m_axi_gmem5_WDATA;
  assign m_axi_gmem5_wlast = hls_inst_m_axi_gmem5_WLAST;
  assign m_axi_gmem5_wstrb[3:0] = hls_inst_m_axi_gmem5_WSTRB;
  assign m_axi_gmem5_wvalid = hls_inst_m_axi_gmem5_WVALID;
  assign m_axi_gmem_araddr[63:0] = hls_inst_m_axi_gmem_ARADDR;
  assign m_axi_gmem_arburst[1:0] = hls_inst_m_axi_gmem_ARBURST;
  assign m_axi_gmem_arcache[3:0] = hls_inst_m_axi_gmem_ARCACHE;
  assign m_axi_gmem_arlen[7:0] = hls_inst_m_axi_gmem_ARLEN;
  assign m_axi_gmem_arlock[1:0] = hls_inst_m_axi_gmem_ARLOCK;
  assign m_axi_gmem_arprot[2:0] = hls_inst_m_axi_gmem_ARPROT;
  assign m_axi_gmem_arqos[3:0] = hls_inst_m_axi_gmem_ARQOS;
  assign m_axi_gmem_arregion[3:0] = hls_inst_m_axi_gmem_ARREGION;
  assign m_axi_gmem_arsize[2:0] = hls_inst_m_axi_gmem_ARSIZE;
  assign m_axi_gmem_arvalid = hls_inst_m_axi_gmem_ARVALID;
  assign m_axi_gmem_awaddr[63:0] = hls_inst_m_axi_gmem_AWADDR;
  assign m_axi_gmem_awburst[1:0] = hls_inst_m_axi_gmem_AWBURST;
  assign m_axi_gmem_awcache[3:0] = hls_inst_m_axi_gmem_AWCACHE;
  assign m_axi_gmem_awlen[7:0] = hls_inst_m_axi_gmem_AWLEN;
  assign m_axi_gmem_awlock[1:0] = hls_inst_m_axi_gmem_AWLOCK;
  assign m_axi_gmem_awprot[2:0] = hls_inst_m_axi_gmem_AWPROT;
  assign m_axi_gmem_awqos[3:0] = hls_inst_m_axi_gmem_AWQOS;
  assign m_axi_gmem_awregion[3:0] = hls_inst_m_axi_gmem_AWREGION;
  assign m_axi_gmem_awsize[2:0] = hls_inst_m_axi_gmem_AWSIZE;
  assign m_axi_gmem_awvalid = hls_inst_m_axi_gmem_AWVALID;
  assign m_axi_gmem_bready = hls_inst_m_axi_gmem_BREADY;
  assign m_axi_gmem_rready = hls_inst_m_axi_gmem_RREADY;
  assign m_axi_gmem_wdata[31:0] = hls_inst_m_axi_gmem_WDATA;
  assign m_axi_gmem_wlast = hls_inst_m_axi_gmem_WLAST;
  assign m_axi_gmem_wstrb[3:0] = hls_inst_m_axi_gmem_WSTRB;
  assign m_axi_gmem_wvalid = hls_inst_m_axi_gmem_WVALID;
  assign s_axi_control_0_1_ARADDR = s_axi_control_araddr[6:0];
  assign s_axi_control_0_1_ARVALID = s_axi_control_arvalid;
  assign s_axi_control_0_1_AWADDR = s_axi_control_awaddr[6:0];
  assign s_axi_control_0_1_AWVALID = s_axi_control_awvalid;
  assign s_axi_control_0_1_BREADY = s_axi_control_bready;
  assign s_axi_control_0_1_RREADY = s_axi_control_rready;
  assign s_axi_control_0_1_WDATA = s_axi_control_wdata[31:0];
  assign s_axi_control_0_1_WSTRB = s_axi_control_wstrb[3:0];
  assign s_axi_control_0_1_WVALID = s_axi_control_wvalid;
  assign s_axi_control_arready = s_axi_control_0_1_ARREADY;
  assign s_axi_control_awready = s_axi_control_0_1_AWREADY;
  assign s_axi_control_bresp[1:0] = s_axi_control_0_1_BRESP;
  assign s_axi_control_bvalid = s_axi_control_0_1_BVALID;
  assign s_axi_control_rdata[31:0] = s_axi_control_0_1_RDATA;
  assign s_axi_control_rresp[1:0] = s_axi_control_0_1_RRESP;
  assign s_axi_control_rvalid = s_axi_control_0_1_RVALID;
  assign s_axi_control_wready = s_axi_control_0_1_WREADY;
  bd_0_hls_inst_0 hls_inst
       (.ap_clk(ap_clk_0_1),
        .ap_rst_n(ap_rst_n_0_1),
        .interrupt(hls_inst_interrupt),
        .m_axi_gmem1_ARADDR(hls_inst_m_axi_gmem1_ARADDR),
        .m_axi_gmem1_ARBURST(hls_inst_m_axi_gmem1_ARBURST),
        .m_axi_gmem1_ARCACHE(hls_inst_m_axi_gmem1_ARCACHE),
        .m_axi_gmem1_ARLEN(hls_inst_m_axi_gmem1_ARLEN),
        .m_axi_gmem1_ARLOCK(hls_inst_m_axi_gmem1_ARLOCK),
        .m_axi_gmem1_ARPROT(hls_inst_m_axi_gmem1_ARPROT),
        .m_axi_gmem1_ARQOS(hls_inst_m_axi_gmem1_ARQOS),
        .m_axi_gmem1_ARREADY(hls_inst_m_axi_gmem1_ARREADY),
        .m_axi_gmem1_ARREGION(hls_inst_m_axi_gmem1_ARREGION),
        .m_axi_gmem1_ARSIZE(hls_inst_m_axi_gmem1_ARSIZE),
        .m_axi_gmem1_ARVALID(hls_inst_m_axi_gmem1_ARVALID),
        .m_axi_gmem1_AWADDR(hls_inst_m_axi_gmem1_AWADDR),
        .m_axi_gmem1_AWBURST(hls_inst_m_axi_gmem1_AWBURST),
        .m_axi_gmem1_AWCACHE(hls_inst_m_axi_gmem1_AWCACHE),
        .m_axi_gmem1_AWLEN(hls_inst_m_axi_gmem1_AWLEN),
        .m_axi_gmem1_AWLOCK(hls_inst_m_axi_gmem1_AWLOCK),
        .m_axi_gmem1_AWPROT(hls_inst_m_axi_gmem1_AWPROT),
        .m_axi_gmem1_AWQOS(hls_inst_m_axi_gmem1_AWQOS),
        .m_axi_gmem1_AWREADY(hls_inst_m_axi_gmem1_AWREADY),
        .m_axi_gmem1_AWREGION(hls_inst_m_axi_gmem1_AWREGION),
        .m_axi_gmem1_AWSIZE(hls_inst_m_axi_gmem1_AWSIZE),
        .m_axi_gmem1_AWVALID(hls_inst_m_axi_gmem1_AWVALID),
        .m_axi_gmem1_BREADY(hls_inst_m_axi_gmem1_BREADY),
        .m_axi_gmem1_BRESP(hls_inst_m_axi_gmem1_BRESP),
        .m_axi_gmem1_BVALID(hls_inst_m_axi_gmem1_BVALID),
        .m_axi_gmem1_RDATA(hls_inst_m_axi_gmem1_RDATA),
        .m_axi_gmem1_RLAST(hls_inst_m_axi_gmem1_RLAST),
        .m_axi_gmem1_RREADY(hls_inst_m_axi_gmem1_RREADY),
        .m_axi_gmem1_RRESP(hls_inst_m_axi_gmem1_RRESP),
        .m_axi_gmem1_RVALID(hls_inst_m_axi_gmem1_RVALID),
        .m_axi_gmem1_WDATA(hls_inst_m_axi_gmem1_WDATA),
        .m_axi_gmem1_WLAST(hls_inst_m_axi_gmem1_WLAST),
        .m_axi_gmem1_WREADY(hls_inst_m_axi_gmem1_WREADY),
        .m_axi_gmem1_WSTRB(hls_inst_m_axi_gmem1_WSTRB),
        .m_axi_gmem1_WVALID(hls_inst_m_axi_gmem1_WVALID),
        .m_axi_gmem2_ARADDR(hls_inst_m_axi_gmem2_ARADDR),
        .m_axi_gmem2_ARBURST(hls_inst_m_axi_gmem2_ARBURST),
        .m_axi_gmem2_ARCACHE(hls_inst_m_axi_gmem2_ARCACHE),
        .m_axi_gmem2_ARLEN(hls_inst_m_axi_gmem2_ARLEN),
        .m_axi_gmem2_ARLOCK(hls_inst_m_axi_gmem2_ARLOCK),
        .m_axi_gmem2_ARPROT(hls_inst_m_axi_gmem2_ARPROT),
        .m_axi_gmem2_ARQOS(hls_inst_m_axi_gmem2_ARQOS),
        .m_axi_gmem2_ARREADY(hls_inst_m_axi_gmem2_ARREADY),
        .m_axi_gmem2_ARREGION(hls_inst_m_axi_gmem2_ARREGION),
        .m_axi_gmem2_ARSIZE(hls_inst_m_axi_gmem2_ARSIZE),
        .m_axi_gmem2_ARVALID(hls_inst_m_axi_gmem2_ARVALID),
        .m_axi_gmem2_AWADDR(hls_inst_m_axi_gmem2_AWADDR),
        .m_axi_gmem2_AWBURST(hls_inst_m_axi_gmem2_AWBURST),
        .m_axi_gmem2_AWCACHE(hls_inst_m_axi_gmem2_AWCACHE),
        .m_axi_gmem2_AWLEN(hls_inst_m_axi_gmem2_AWLEN),
        .m_axi_gmem2_AWLOCK(hls_inst_m_axi_gmem2_AWLOCK),
        .m_axi_gmem2_AWPROT(hls_inst_m_axi_gmem2_AWPROT),
        .m_axi_gmem2_AWQOS(hls_inst_m_axi_gmem2_AWQOS),
        .m_axi_gmem2_AWREADY(hls_inst_m_axi_gmem2_AWREADY),
        .m_axi_gmem2_AWREGION(hls_inst_m_axi_gmem2_AWREGION),
        .m_axi_gmem2_AWSIZE(hls_inst_m_axi_gmem2_AWSIZE),
        .m_axi_gmem2_AWVALID(hls_inst_m_axi_gmem2_AWVALID),
        .m_axi_gmem2_BREADY(hls_inst_m_axi_gmem2_BREADY),
        .m_axi_gmem2_BRESP(hls_inst_m_axi_gmem2_BRESP),
        .m_axi_gmem2_BVALID(hls_inst_m_axi_gmem2_BVALID),
        .m_axi_gmem2_RDATA(hls_inst_m_axi_gmem2_RDATA),
        .m_axi_gmem2_RLAST(hls_inst_m_axi_gmem2_RLAST),
        .m_axi_gmem2_RREADY(hls_inst_m_axi_gmem2_RREADY),
        .m_axi_gmem2_RRESP(hls_inst_m_axi_gmem2_RRESP),
        .m_axi_gmem2_RVALID(hls_inst_m_axi_gmem2_RVALID),
        .m_axi_gmem2_WDATA(hls_inst_m_axi_gmem2_WDATA),
        .m_axi_gmem2_WLAST(hls_inst_m_axi_gmem2_WLAST),
        .m_axi_gmem2_WREADY(hls_inst_m_axi_gmem2_WREADY),
        .m_axi_gmem2_WSTRB(hls_inst_m_axi_gmem2_WSTRB),
        .m_axi_gmem2_WVALID(hls_inst_m_axi_gmem2_WVALID),
        .m_axi_gmem3_ARADDR(hls_inst_m_axi_gmem3_ARADDR),
        .m_axi_gmem3_ARBURST(hls_inst_m_axi_gmem3_ARBURST),
        .m_axi_gmem3_ARCACHE(hls_inst_m_axi_gmem3_ARCACHE),
        .m_axi_gmem3_ARLEN(hls_inst_m_axi_gmem3_ARLEN),
        .m_axi_gmem3_ARLOCK(hls_inst_m_axi_gmem3_ARLOCK),
        .m_axi_gmem3_ARPROT(hls_inst_m_axi_gmem3_ARPROT),
        .m_axi_gmem3_ARQOS(hls_inst_m_axi_gmem3_ARQOS),
        .m_axi_gmem3_ARREADY(hls_inst_m_axi_gmem3_ARREADY),
        .m_axi_gmem3_ARREGION(hls_inst_m_axi_gmem3_ARREGION),
        .m_axi_gmem3_ARSIZE(hls_inst_m_axi_gmem3_ARSIZE),
        .m_axi_gmem3_ARVALID(hls_inst_m_axi_gmem3_ARVALID),
        .m_axi_gmem3_AWADDR(hls_inst_m_axi_gmem3_AWADDR),
        .m_axi_gmem3_AWBURST(hls_inst_m_axi_gmem3_AWBURST),
        .m_axi_gmem3_AWCACHE(hls_inst_m_axi_gmem3_AWCACHE),
        .m_axi_gmem3_AWLEN(hls_inst_m_axi_gmem3_AWLEN),
        .m_axi_gmem3_AWLOCK(hls_inst_m_axi_gmem3_AWLOCK),
        .m_axi_gmem3_AWPROT(hls_inst_m_axi_gmem3_AWPROT),
        .m_axi_gmem3_AWQOS(hls_inst_m_axi_gmem3_AWQOS),
        .m_axi_gmem3_AWREADY(hls_inst_m_axi_gmem3_AWREADY),
        .m_axi_gmem3_AWREGION(hls_inst_m_axi_gmem3_AWREGION),
        .m_axi_gmem3_AWSIZE(hls_inst_m_axi_gmem3_AWSIZE),
        .m_axi_gmem3_AWVALID(hls_inst_m_axi_gmem3_AWVALID),
        .m_axi_gmem3_BREADY(hls_inst_m_axi_gmem3_BREADY),
        .m_axi_gmem3_BRESP(hls_inst_m_axi_gmem3_BRESP),
        .m_axi_gmem3_BVALID(hls_inst_m_axi_gmem3_BVALID),
        .m_axi_gmem3_RDATA(hls_inst_m_axi_gmem3_RDATA),
        .m_axi_gmem3_RLAST(hls_inst_m_axi_gmem3_RLAST),
        .m_axi_gmem3_RREADY(hls_inst_m_axi_gmem3_RREADY),
        .m_axi_gmem3_RRESP(hls_inst_m_axi_gmem3_RRESP),
        .m_axi_gmem3_RVALID(hls_inst_m_axi_gmem3_RVALID),
        .m_axi_gmem3_WDATA(hls_inst_m_axi_gmem3_WDATA),
        .m_axi_gmem3_WLAST(hls_inst_m_axi_gmem3_WLAST),
        .m_axi_gmem3_WREADY(hls_inst_m_axi_gmem3_WREADY),
        .m_axi_gmem3_WSTRB(hls_inst_m_axi_gmem3_WSTRB),
        .m_axi_gmem3_WVALID(hls_inst_m_axi_gmem3_WVALID),
        .m_axi_gmem4_ARADDR(hls_inst_m_axi_gmem4_ARADDR),
        .m_axi_gmem4_ARBURST(hls_inst_m_axi_gmem4_ARBURST),
        .m_axi_gmem4_ARCACHE(hls_inst_m_axi_gmem4_ARCACHE),
        .m_axi_gmem4_ARLEN(hls_inst_m_axi_gmem4_ARLEN),
        .m_axi_gmem4_ARLOCK(hls_inst_m_axi_gmem4_ARLOCK),
        .m_axi_gmem4_ARPROT(hls_inst_m_axi_gmem4_ARPROT),
        .m_axi_gmem4_ARQOS(hls_inst_m_axi_gmem4_ARQOS),
        .m_axi_gmem4_ARREADY(hls_inst_m_axi_gmem4_ARREADY),
        .m_axi_gmem4_ARREGION(hls_inst_m_axi_gmem4_ARREGION),
        .m_axi_gmem4_ARSIZE(hls_inst_m_axi_gmem4_ARSIZE),
        .m_axi_gmem4_ARVALID(hls_inst_m_axi_gmem4_ARVALID),
        .m_axi_gmem4_AWADDR(hls_inst_m_axi_gmem4_AWADDR),
        .m_axi_gmem4_AWBURST(hls_inst_m_axi_gmem4_AWBURST),
        .m_axi_gmem4_AWCACHE(hls_inst_m_axi_gmem4_AWCACHE),
        .m_axi_gmem4_AWLEN(hls_inst_m_axi_gmem4_AWLEN),
        .m_axi_gmem4_AWLOCK(hls_inst_m_axi_gmem4_AWLOCK),
        .m_axi_gmem4_AWPROT(hls_inst_m_axi_gmem4_AWPROT),
        .m_axi_gmem4_AWQOS(hls_inst_m_axi_gmem4_AWQOS),
        .m_axi_gmem4_AWREADY(hls_inst_m_axi_gmem4_AWREADY),
        .m_axi_gmem4_AWREGION(hls_inst_m_axi_gmem4_AWREGION),
        .m_axi_gmem4_AWSIZE(hls_inst_m_axi_gmem4_AWSIZE),
        .m_axi_gmem4_AWVALID(hls_inst_m_axi_gmem4_AWVALID),
        .m_axi_gmem4_BREADY(hls_inst_m_axi_gmem4_BREADY),
        .m_axi_gmem4_BRESP(hls_inst_m_axi_gmem4_BRESP),
        .m_axi_gmem4_BVALID(hls_inst_m_axi_gmem4_BVALID),
        .m_axi_gmem4_RDATA(hls_inst_m_axi_gmem4_RDATA),
        .m_axi_gmem4_RLAST(hls_inst_m_axi_gmem4_RLAST),
        .m_axi_gmem4_RREADY(hls_inst_m_axi_gmem4_RREADY),
        .m_axi_gmem4_RRESP(hls_inst_m_axi_gmem4_RRESP),
        .m_axi_gmem4_RVALID(hls_inst_m_axi_gmem4_RVALID),
        .m_axi_gmem4_WDATA(hls_inst_m_axi_gmem4_WDATA),
        .m_axi_gmem4_WLAST(hls_inst_m_axi_gmem4_WLAST),
        .m_axi_gmem4_WREADY(hls_inst_m_axi_gmem4_WREADY),
        .m_axi_gmem4_WSTRB(hls_inst_m_axi_gmem4_WSTRB),
        .m_axi_gmem4_WVALID(hls_inst_m_axi_gmem4_WVALID),
        .m_axi_gmem5_ARADDR(hls_inst_m_axi_gmem5_ARADDR),
        .m_axi_gmem5_ARBURST(hls_inst_m_axi_gmem5_ARBURST),
        .m_axi_gmem5_ARCACHE(hls_inst_m_axi_gmem5_ARCACHE),
        .m_axi_gmem5_ARLEN(hls_inst_m_axi_gmem5_ARLEN),
        .m_axi_gmem5_ARLOCK(hls_inst_m_axi_gmem5_ARLOCK),
        .m_axi_gmem5_ARPROT(hls_inst_m_axi_gmem5_ARPROT),
        .m_axi_gmem5_ARQOS(hls_inst_m_axi_gmem5_ARQOS),
        .m_axi_gmem5_ARREADY(hls_inst_m_axi_gmem5_ARREADY),
        .m_axi_gmem5_ARREGION(hls_inst_m_axi_gmem5_ARREGION),
        .m_axi_gmem5_ARSIZE(hls_inst_m_axi_gmem5_ARSIZE),
        .m_axi_gmem5_ARVALID(hls_inst_m_axi_gmem5_ARVALID),
        .m_axi_gmem5_AWADDR(hls_inst_m_axi_gmem5_AWADDR),
        .m_axi_gmem5_AWBURST(hls_inst_m_axi_gmem5_AWBURST),
        .m_axi_gmem5_AWCACHE(hls_inst_m_axi_gmem5_AWCACHE),
        .m_axi_gmem5_AWLEN(hls_inst_m_axi_gmem5_AWLEN),
        .m_axi_gmem5_AWLOCK(hls_inst_m_axi_gmem5_AWLOCK),
        .m_axi_gmem5_AWPROT(hls_inst_m_axi_gmem5_AWPROT),
        .m_axi_gmem5_AWQOS(hls_inst_m_axi_gmem5_AWQOS),
        .m_axi_gmem5_AWREADY(hls_inst_m_axi_gmem5_AWREADY),
        .m_axi_gmem5_AWREGION(hls_inst_m_axi_gmem5_AWREGION),
        .m_axi_gmem5_AWSIZE(hls_inst_m_axi_gmem5_AWSIZE),
        .m_axi_gmem5_AWVALID(hls_inst_m_axi_gmem5_AWVALID),
        .m_axi_gmem5_BREADY(hls_inst_m_axi_gmem5_BREADY),
        .m_axi_gmem5_BRESP(hls_inst_m_axi_gmem5_BRESP),
        .m_axi_gmem5_BVALID(hls_inst_m_axi_gmem5_BVALID),
        .m_axi_gmem5_RDATA(hls_inst_m_axi_gmem5_RDATA),
        .m_axi_gmem5_RLAST(hls_inst_m_axi_gmem5_RLAST),
        .m_axi_gmem5_RREADY(hls_inst_m_axi_gmem5_RREADY),
        .m_axi_gmem5_RRESP(hls_inst_m_axi_gmem5_RRESP),
        .m_axi_gmem5_RVALID(hls_inst_m_axi_gmem5_RVALID),
        .m_axi_gmem5_WDATA(hls_inst_m_axi_gmem5_WDATA),
        .m_axi_gmem5_WLAST(hls_inst_m_axi_gmem5_WLAST),
        .m_axi_gmem5_WREADY(hls_inst_m_axi_gmem5_WREADY),
        .m_axi_gmem5_WSTRB(hls_inst_m_axi_gmem5_WSTRB),
        .m_axi_gmem5_WVALID(hls_inst_m_axi_gmem5_WVALID),
        .m_axi_gmem_ARADDR(hls_inst_m_axi_gmem_ARADDR),
        .m_axi_gmem_ARBURST(hls_inst_m_axi_gmem_ARBURST),
        .m_axi_gmem_ARCACHE(hls_inst_m_axi_gmem_ARCACHE),
        .m_axi_gmem_ARLEN(hls_inst_m_axi_gmem_ARLEN),
        .m_axi_gmem_ARLOCK(hls_inst_m_axi_gmem_ARLOCK),
        .m_axi_gmem_ARPROT(hls_inst_m_axi_gmem_ARPROT),
        .m_axi_gmem_ARQOS(hls_inst_m_axi_gmem_ARQOS),
        .m_axi_gmem_ARREADY(hls_inst_m_axi_gmem_ARREADY),
        .m_axi_gmem_ARREGION(hls_inst_m_axi_gmem_ARREGION),
        .m_axi_gmem_ARSIZE(hls_inst_m_axi_gmem_ARSIZE),
        .m_axi_gmem_ARVALID(hls_inst_m_axi_gmem_ARVALID),
        .m_axi_gmem_AWADDR(hls_inst_m_axi_gmem_AWADDR),
        .m_axi_gmem_AWBURST(hls_inst_m_axi_gmem_AWBURST),
        .m_axi_gmem_AWCACHE(hls_inst_m_axi_gmem_AWCACHE),
        .m_axi_gmem_AWLEN(hls_inst_m_axi_gmem_AWLEN),
        .m_axi_gmem_AWLOCK(hls_inst_m_axi_gmem_AWLOCK),
        .m_axi_gmem_AWPROT(hls_inst_m_axi_gmem_AWPROT),
        .m_axi_gmem_AWQOS(hls_inst_m_axi_gmem_AWQOS),
        .m_axi_gmem_AWREADY(hls_inst_m_axi_gmem_AWREADY),
        .m_axi_gmem_AWREGION(hls_inst_m_axi_gmem_AWREGION),
        .m_axi_gmem_AWSIZE(hls_inst_m_axi_gmem_AWSIZE),
        .m_axi_gmem_AWVALID(hls_inst_m_axi_gmem_AWVALID),
        .m_axi_gmem_BREADY(hls_inst_m_axi_gmem_BREADY),
        .m_axi_gmem_BRESP(hls_inst_m_axi_gmem_BRESP),
        .m_axi_gmem_BVALID(hls_inst_m_axi_gmem_BVALID),
        .m_axi_gmem_RDATA(hls_inst_m_axi_gmem_RDATA),
        .m_axi_gmem_RLAST(hls_inst_m_axi_gmem_RLAST),
        .m_axi_gmem_RREADY(hls_inst_m_axi_gmem_RREADY),
        .m_axi_gmem_RRESP(hls_inst_m_axi_gmem_RRESP),
        .m_axi_gmem_RVALID(hls_inst_m_axi_gmem_RVALID),
        .m_axi_gmem_WDATA(hls_inst_m_axi_gmem_WDATA),
        .m_axi_gmem_WLAST(hls_inst_m_axi_gmem_WLAST),
        .m_axi_gmem_WREADY(hls_inst_m_axi_gmem_WREADY),
        .m_axi_gmem_WSTRB(hls_inst_m_axi_gmem_WSTRB),
        .m_axi_gmem_WVALID(hls_inst_m_axi_gmem_WVALID),
        .s_axi_control_ARADDR(s_axi_control_0_1_ARADDR),
        .s_axi_control_ARREADY(s_axi_control_0_1_ARREADY),
        .s_axi_control_ARVALID(s_axi_control_0_1_ARVALID),
        .s_axi_control_AWADDR(s_axi_control_0_1_AWADDR),
        .s_axi_control_AWREADY(s_axi_control_0_1_AWREADY),
        .s_axi_control_AWVALID(s_axi_control_0_1_AWVALID),
        .s_axi_control_BREADY(s_axi_control_0_1_BREADY),
        .s_axi_control_BRESP(s_axi_control_0_1_BRESP),
        .s_axi_control_BVALID(s_axi_control_0_1_BVALID),
        .s_axi_control_RDATA(s_axi_control_0_1_RDATA),
        .s_axi_control_RREADY(s_axi_control_0_1_RREADY),
        .s_axi_control_RRESP(s_axi_control_0_1_RRESP),
        .s_axi_control_RVALID(s_axi_control_0_1_RVALID),
        .s_axi_control_WDATA(s_axi_control_0_1_WDATA),
        .s_axi_control_WREADY(s_axi_control_0_1_WREADY),
        .s_axi_control_WSTRB(s_axi_control_0_1_WSTRB),
        .s_axi_control_WVALID(s_axi_control_0_1_WVALID));
endmodule
