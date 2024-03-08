// Module MEM
//
// This module implements a Memory with one write port and one read port. This memory
// can be implemented as a BRAM.
//

`include "RTLinf.vh"

module MEM #(
    parameter DATA_WIDTH      = 32,         // data width
    parameter NUM_ADDRESSES   = 4096,       // number of addresses
    parameter LOG_MAX_ADDRESS = 12,         // number of bits for addresses
    parameter ID = 0                        // identifier (useful for debug and initialization for simulation)
)(
  input clk,                                // clock input
  input rst,                                // needed for debug

  input [DATA_WIDTH-1:0]        data_write, // WRITE interface:: data
  input [LOG_MAX_ADDRESS-1:0]   addr_write, // WRITE interface:: address
  input                         write,      // WRITE interface:: write signal

  input [LOG_MAX_ADDRESS-1:0]   addr_read,  // READ interface:: address
  input                         read,       // READ interface:: read signal
  output [DATA_WIDTH-1:0]       data_read,  // READ interface:: data
  output                        valid_out   // READ interface:: valid
);

reg valid1;

assign valid_out = valid1;

generate
  if (DATA_WIDTH == 32 && NUM_ADDRESSES == 4096) begin
    MEM_4Kx32 mem_4kx32_m (
      .clk        ( clk        ),
      .rst        ( rst        ),
      .data_write ( data_write ),
      .addr_write ( addr_write ),
      .write      ( write      ),
      .addr_read  ( addr_read  ),
      .data_read  ( data_read  ),
      .read       ( read       )
    );
  end else if (DATA_WIDTH == 72 && NUM_ADDRESSES == 4096) begin
      MEM_4Kx72 mem_4kx72_m (
        .clk        ( clk        ),
        .rst        ( rst        ),
        .data_write ( data_write ),
        .addr_write ( addr_write ),
        .write      ( write      ),
        .addr_read  ( addr_read  ),
        .data_read  ( data_read  ),
        .read       ( read       )
      );
  end else if (DATA_WIDTH == 64 && NUM_ADDRESSES == 4096) begin
      MEM_4Kx64 mem_4kx64_m (
        .clk        ( clk        ),
        .rst        ( rst        ),
        .data_write ( data_write ),
        .addr_write ( addr_write ),
        .write      ( write      ),
        .addr_read  ( addr_read  ),
        .data_read  ( data_read  ),
        .read       ( read       )
      );
    end else begin
      initial $display("memory geometry not supported %d x %d", NUM_ADDRESSES, DATA_WIDTH);
    end
endgenerate

always @ (posedge clk) begin
  if (~rst) begin
    valid1 <= 1'b0;
  end else begin
    valid1 <= read;
  end
end

endmodule

// -------------------------------------------------------------------------------------------
// 4Kx72 memory implemented as 8 512x72 memories plus one 4096x9 memory
// Each memory has a one bit extra for each byte
//
 
module MEM_4Kx72 #(
    localparam DATA_WIDTH      = 72,        // data width
    localparam NUM_ADDRESSES   = 4096,      // number of addresses
    localparam LOG_MAX_ADDRESS = 12         // number of bits for addresses
)(
  input clk,                                // clock input
  input rst,                                // needed for debug

  input [DATA_WIDTH-1:0]        data_write, // WRITE interface:: data
  input [LOG_MAX_ADDRESS-1:0]   addr_write, // WRITE interface:: address
  input                         write,      // WRITE interface:: write signal

  input [LOG_MAX_ADDRESS-1:0]   addr_read,  // READ interface:: address
  output [DATA_WIDTH-1:0]       data_read,  // READ interface:: data
  input                         read
);

  wire [71:0] data_read_bram[7:0];
  wire [8:0] data_read_bram_last;
  wire [7:0]  write_bram;
  wire [7:0]  read_bram;
  wire [80:0] data_read_final;
  
  genvar i;
  
  generate
    for (i=0; i<8; i=i+1) begin
      assign write_bram[i] = write & (addr_write[11:9] == i);
      assign read_bram[i] = read & (addr_read[11:9] == i);
    end
  endgenerate
  
  assign data_read_final = {data_read_bram_last,data_read_bram[addr_write[11:9]]};
  
  assign data_read = {data_read_final[79:72],data_read_final[70:63],data_read_final[61:54],data_read_final[52:45],data_read_final[43:36],data_read_final[34:27],data_read_final[25:18],data_read_final[16:9],data_read_final[7:0]};
  
  generate
    for (i=0; i<8; i=i+1) begin
      BRAM_SDP_MACRO #(
        .BRAM_SIZE     ( "36Kb"    ), // Target BRAM, "18Kb" or "36Kb" 
        .DEVICE        ( "7SERIES" ), // Target device: "7SERIES" 
        .WRITE_WIDTH   ( 72        ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
        .READ_WIDTH    ( 72        ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
        .DO_REG        ( 0         ), // Optional output register (0 or 1)
        .INIT_FILE     ( "NONE"    ),
        .SIM_COLLISION_CHECK ( "ALL"                  ), // Collision check enable "ALL", "WARNING_ONLY", "GENERATE_X_ONLY" or "NONE" 
        .SRVAL               ( 72'h000000000000000000 ), // Set/Reset value for port output
        .INIT                ( 72'h000000000000000000 ), // Initial values on output port
        .WRITE_MODE          ( "READ_FIRST"           ), // Specify "READ_FIRST" for same clock or synchronous clocks, Specify "WRITE_FIRST for asynchronous clocks on ports
        .INIT_00(256'h23222120_1f1e1d1c_1a191817_16151413_11100f0e_0d0c0b0a_08070605_04030201)
     ) BRAM_SDP_MACRO_inst_0 (
        .DO            ( data_read_bram[i]  ), // Output read data port, width defined by READ_WIDTH parameter
        .DI            ( data_write         ), // Input write data port, width defined by WRITE_WIDTH parameter
        .RDADDR        ( addr_read[8:0]     ), // Input read address, width defined by read port depth
        .RDCLK         ( clk                ), // 1-bit input read clock
        .RDEN          ( read               ), // 1-bit input read port enable
        .REGCE         ( 1'b0               ), // 1-bit input read output register enable
        .RST           ( ~rst               ), // 1-bit input reset
        .WE            ( 8'b00000000        ), // Input write enable, width defined by write port depth
        .WRADDR        ( addr_write[8:0]    ), // Input write address, width defined by write port depth
        .WRCLK         ( clk                ), // 1-bit input write clock
        .WREN          ( 0                  )  // 1-bit input write port enable
     );
   end
  endgenerate
  
  BRAM_SDP_MACRO #(
    .BRAM_SIZE     ( "36Kb"    ), // Target BRAM, "18Kb" or "36Kb" 
    .DEVICE        ( "7SERIES" ), // Target device: "7SERIES" 
    .WRITE_WIDTH   ( 9         ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
    .READ_WIDTH    ( 9         ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
    .DO_REG        ( 0         ), // Optional output register (0 or 1)
    .INIT_FILE     ( "NONE"    ),
    .SIM_COLLISION_CHECK ( "ALL"                  ), // Collision check enable "ALL", "WARNING_ONLY", "GENERATE_X_ONLY" or "NONE" 
    .SRVAL               ( 72'h000000000000000000 ), // Set/Reset value for port output
    .INIT                ( 72'h000000000000000000 ), // Initial values on output port
    .WRITE_MODE          ( "READ_FIRST"           ), // Specify "READ_FIRST" for same clock or synchronous clocks, Specify "WRITE_FIRST for asynchronous clocks on ports
    .INIT_00(256'h00000000_00000000_00000000_00000000_00000000_00000000_00000000_241b1209)
  ) BRAM_SDP_MACRO_inst_0 (
    .DO            ( data_read_bram_last  ), // Output read data port, width defined by READ_WIDTH parameter
    .DI            ( data_write         ), // Input write data port, width defined by WRITE_WIDTH parameter
    .RDADDR        ( addr_read[11:0]    ), // Input read address, width defined by read port depth
    .RDCLK         ( clk                ), // 1-bit input read clock
    .RDEN          ( read               ), // 1-bit input read port enable
    .REGCE         ( 1'b0               ), // 1-bit input read output register enable
    .RST           ( ~rst               ), // 1-bit input reset
    .WE            ( 1'b0               ), // Input write enable, width defined by write port depth
    .WRADDR        ( addr_write[11:0]   ), // Input write address, width defined by write port depth
    .WRCLK         ( clk                ), // 1-bit input write clock
    .WREN          ( 1'b0               )  // 1-bit input write port enable
  );
    
 endmodule

// -------------------------------------------------------------------------------------------
// 4Kx64 memory implemented with 8 512x72 memories (each memory has one extra bit per byte)
//
module MEM_4Kx64 #(
    localparam DATA_WIDTH      = 64,        // data width
    localparam NUM_ADDRESSES   = 4096,      // number of addresses
    localparam LOG_MAX_ADDRESS = 12         // number of bits for addresses
)(
  input clk,                                // clock input
  input rst,                                // needed for debug

  input [DATA_WIDTH-1:0]        data_write, // WRITE interface:: data
  input [LOG_MAX_ADDRESS-1:0]   addr_write, // WRITE interface:: address
  input                         write,      // WRITE interface:: write signal

  input [LOG_MAX_ADDRESS-1:0]   addr_read,  // READ interface:: address
  output [DATA_WIDTH-1:0]       data_read,  // READ interface:: data
  input                         read        // READ interface:: read signal
);

  wire [71:0] data_read_bram[7:0];
  wire [7:0]  write_bram;
  wire [7:0]  read_bram;
  
  genvar i;
  
  generate
    for (i=0; i<8; i=i+1) begin
      assign read_bram[i] = read & (addr_read[11:9] == i);
      assign write_bram[i] = write & (addr_write[11:9] == i);
    end
  endgenerate
  
  assign data_read = data_read_bram[ addr_write[11:9] ];
  
  generate
   for (i=0; i<8; i=i+1) begin
       BRAM_SDP_MACRO #(
        .BRAM_SIZE     ( "36Kb"    ), // Target BRAM, "18Kb" or "36Kb" 
        .DEVICE        ( "7SERIES" ), // Target device: "7SERIES" 
        .WRITE_WIDTH   ( 72        ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
        .READ_WIDTH    ( 72        ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
        .DO_REG        ( 0         ), // Optional output register (0 or 1)
        .INIT_FILE     ( "NONE"    ),
        .SIM_COLLISION_CHECK ( "NONE"                  ), // Collision check enable "ALL", "WARNING_ONLY", "GENERATE_X_ONLY" or "NONE" 
        .SRVAL               ( 72'h000000000000000000 ), // Set/Reset value for port output
        .INIT                ( 72'h000000000000000000 ), // Initial values on output port
        .WRITE_MODE          ( "WRITE_FIRST"          )  // Specify "READ_FIRST" for same clock or synchronous clocks, Specify "WRITE_FIRST for asynchronous clocks on ports
       ) BRAM_SDP_MACRO_inst_0 (
        .DO            ( data_read_bram[i]  ), // Output read data port, width defined by READ_WIDTH parameter
        .DI            ( data_write         ), // Input write data port, width defined by WRITE_WIDTH parameter
        .RDADDR        ( addr_read[8:0]     ), // Input read address, width defined by read port depth
        .RDCLK         ( clk                ), // 1-bit input read clock
        .RDEN          ( read_bram[i]       ), // 1-bit input read port enable
        .REGCE         ( 0                  ), // 1-bit input read output register enable
        .RST           ( ~rst               ), // 1-bit input reset
        .WE            ( 8'b11111111        ), // Input write enable, width defined by write port depth
        .WRADDR        ( addr_write[8:0]    ), // Input write address, width defined by write port depth
        .WRCLK         ( clk                ), // 1-bit input write clock
        .WREN          ( write_bram[i]      )  // 1-bit input write port enable
       );
     end
   endgenerate
   
 endmodule
   
// -------------------------------------------------------------------------------------------
// memory 4K addresses with 32 bit datawidth
//
// Implemented as 4 BRAMs SDP (simple dual port)
module MEM_4Kx32 #(
    localparam DATA_WIDTH      = 32,        // data width
    localparam NUM_ADDRESSES   = 4096,      // number of addresses
    localparam LOG_MAX_ADDRESS = 12         // number of bits for addresses
)(
  input clk,                                // clock input
  input rst,                                // needed for debug

  input [DATA_WIDTH-1:0]        data_write, // WRITE interface:: data
  input [LOG_MAX_ADDRESS-1:0]   addr_write, // WRITE interface:: address
  input                         write,      // WRITE interface:: write signal

  input [LOG_MAX_ADDRESS-1:0]   addr_read,  // READ interface:: address
  output [DATA_WIDTH-1:0]       data_read,  // READ interface:: data
  input                         read        // READ interface:: read
);

  wire [35:0] data_read_bram[3:0];
  wire [3:0]  write_bram;
  wire [3:0]  read_bram;
  wire [35:0] data_read_final;
  
  genvar i;
  
  generate
    for (i=0; i<4; i=i+1) begin
      assign write_bram[i] = write & (addr_write[11:10] == i);
      assign read_bram[i] = read & (addr_read[11:10] == i);
    end
  endgenerate
  
  assign data_read_final = data_read_bram[addr_write[11:10]];
  
  assign data_read = {data_read_final[34:27],data_read_final[25:18],data_read_final[16:9],data_read_final[7:0]};
  
   ///////////////////////////////////////////////////////////////////////
   //  READ_WIDTH | BRAM_SIZE | READ Depth  | RDADDR Width |            //
   // WRITE_WIDTH |           | WRITE Depth | WRADDR Width |  WE Width  //
   // ============|===========|=============|==============|============//
   //    37-72    |  "36Kb"   |      512    |     9-bit    |    8-bit   //
   //    19-36    |  "36Kb"   |     1024    |    10-bit    |    4-bit   //
   //    19-36    |  "18Kb"   |      512    |     9-bit    |    4-bit   //
   //    10-18    |  "36Kb"   |     2048    |    11-bit    |    2-bit   //
   //    10-18    |  "18Kb"   |     1024    |    10-bit    |    2-bit   //
   //     5-9     |  "36Kb"   |     4096    |    12-bit    |    1-bit   //
   //     5-9     |  "18Kb"   |     2048    |    11-bit    |    1-bit   //
   //     3-4     |  "36Kb"   |     8192    |    13-bit    |    1-bit   //
   //     3-4     |  "18Kb"   |     4096    |    12-bit    |    1-bit   //
   //       2     |  "36Kb"   |    16384    |    14-bit    |    1-bit   //
   //       2     |  "18Kb"   |     8192    |    13-bit    |    1-bit   //
   //       1     |  "36Kb"   |    32768    |    15-bit    |    1-bit   //
   //       1     |  "18Kb"   |    16384    |    14-bit    |    1-bit   //
   ///////////////////////////////////////////////////////////////////////

   generate
     for (i=0; i<4; i=i+1) begin
       BRAM_SDP_MACRO #(
        .BRAM_SIZE     ( "36Kb"    ), // Target BRAM, "18Kb" or "36Kb" 
        .DEVICE        ( "7SERIES" ), // Target device: "7SERIES" 
        .WRITE_WIDTH   ( 36        ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
        .READ_WIDTH    ( 36        ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
        .DO_REG        ( 0         ), // Optional output register (0 or 1)
        .INIT_FILE     ( "NONE"    ),
        .SIM_COLLISION_CHECK ( "ALL"                  ), // Collision check enable "ALL", "WARNING_ONLY", "GENERATE_X_ONLY" or "NONE" 
        .SRVAL               ( 72'h000000000000000000          ), // Set/Reset value for port output
        .INIT                ( 72'h000000000000000000          ), // Initial values on output port
        .WRITE_MODE          ( "READ_FIRST"          ), // Specify "READ_FIRST" for same clock or synchronous clocks, Specify "WRITE_FIRST for asynchronous clocks on ports
      .INIT_00(256'h201f1e1d_1c1b1a19_18171615_14131211_100f0e0d_0c0b0a09_08070605_04030201),
      .INIT_01(256'h403f3e3d_3c3b3a39_38373635_34333231_302f2e2c_2c2b2a29_28272625_24232221),
      .INIT_02(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_03(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_04(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_05(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_06(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_07(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_08(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_09(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_10(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_11(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_12(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_13(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_14(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_15(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_16(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_17(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_18(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_19(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_20(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_21(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_22(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_23(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_24(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_25(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_26(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_27(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_28(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_29(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_30(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_31(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_32(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_33(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_34(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_35(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_36(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_37(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_38(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_39(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_40(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_41(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_42(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_43(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_44(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_45(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_46(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_47(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_48(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_49(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_4A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_4B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_4C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_4D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_4E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_4F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_50(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_51(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_52(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_53(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_54(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_55(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_56(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_57(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_58(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_59(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_5A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_5B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_5C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_5D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_5E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_5F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_60(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_61(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_62(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_63(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_64(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_65(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_66(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_67(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_68(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_69(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_6A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_6B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_6C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_6D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_6E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_6F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_70(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_71(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_72(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_73(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_74(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_75(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_76(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_77(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_78(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_79(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_7A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_7B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_7C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_7D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_7E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_7F(256'h0000000000000000000000000000000000000000000000000000000000000000)
       ) BRAM_SDP_MACRO_inst_0 (
        .DO            ( data_read_bram[i]  ), // Output read data port, width defined by READ_WIDTH parameter
        .DI            ( data_write         ), // Input write data port, width defined by WRITE_WIDTH parameter
        .RDADDR        ( addr_read[9:0]     ), // Input read address, width defined by read port depth
        .RDCLK         ( clk                ), // 1-bit input read clock
        .RDEN          ( read_bram[i]       ), // 1-bit input read port enable
        .REGCE         ( 1'b0               ), // 1-bit input read output register enable
        .RST           ( ~rst               ), // 1-bit input reset
        .WE            ( 4'b1111            ), // Input write enable, width defined by write port depth
        .WRADDR        ( addr_write[9:0]    ), // Input write address, width defined by write port depth
        .WRCLK         ( clk                ), // 1-bit input write clock
        .WREN          ( write_bram[i]      )  // 1-bit input write port enable
       );
     end
   endgenerate
   
 endmodule