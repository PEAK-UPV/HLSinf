// Module MEM
//
// This module implements a Memory with one write port and one read port. This memory
// can be implemented as a BRAM.
//

`include "RTLinf.vh"

// Module MEMORY_BANK.
//
// This module instantiates a set of memories. The module
// implements a set of read ports and a set of write ports.
// Each memory can be configured to use a specific read port and write port
//

module MEMORY_BANK #(
  parameter DATA_WIDTH             = 32,   // data width
  parameter NUM_ADDRESSES          = 1024, // number of addresses
  parameter LOG_MAX_ADDRESS        = 10,   // number of bit addresses
  parameter NUM_MEMORIES           = 1,    // number of memories
  parameter LOG_NUM_MEMORIES       = 1,    // number of bits to encode a memory id
  parameter NUM_READ_PORTS         = 2,    // number of read ports
  parameter LOG_NUM_READ_PORTS     = 1,    // number of bits to encode a read port id
  parameter NUM_WRITE_PORTS        = 2,    // number of write ports
  parameter LOG_NUM_WRITE_PORTS    = 1     // number of bits to encode a write port id
) (
  input                                                clk,
  input                                                rst,
  //
  input                                                cmd_assign,
  input                                                cmd_unassign,
  input [LOG_NUM_READ_PORTS-1:0]                       read_port,
  input [LOG_NUM_WRITE_PORTS-1:0]                      write_port,
  input [LOG_NUM_MEMORIES-1:0]                         memory,
  //
  //
  input [(NUM_WRITE_PORTS * DATA_WIDTH) - 1 : 0]       write_data_in,
  input [(NUM_WRITE_PORTS * LOG_MAX_ADDRESS) - 1 : 0]  write_addr_in,
  input [NUM_WRITE_PORTS-1 : 0]                        write_in,
  //
  output [(NUM_READ_PORTS * DATA_WIDTH) - 1: 0]        read_data_out,
  output [NUM_READ_PORTS-1:0]                          read_valid_out,
  input  [(NUM_READ_PORTS * LOG_MAX_ADDRESS) - 1: 0]   read_addr_in,
  input  [NUM_READ_PORTS-1 : 0]                        read_in            
);

genvar i;

// wires to connect to memories
wire [DATA_WIDTH-1:0]      data_write_w[NUM_MEMORIES-1:0];
wire [LOG_MAX_ADDRESS-1:0] addr_write_w[NUM_MEMORIES-1:0];
wire [NUM_MEMORIES-1:0]    write_w;
wire [LOG_MAX_ADDRESS-1:0] addr_read_w[NUM_MEMORIES-1:0];
wire [NUM_MEMORIES-1:0]    read_w;
wire [DATA_WIDTH-1:0]      data_read_w[NUM_MEMORIES-1:0];
wire [NUM_MEMORIES-1:0]    valid_read_w;

// registers to assign ports to mems
reg [LOG_NUM_READ_PORTS-1:0]  sel_read_port_r[NUM_MEMORIES-1:0];                  // assigned read port to each memory
reg [NUM_READ_PORTS-1:0]      is_assigned_read_port_to_memory_r;                  // whether a read port has an assigned memory
reg [NUM_MEMORIES-1:0]        is_memory_assigned_to_read_port_r;                  // whether a memory is assigned to a read port
reg [LOG_NUM_MEMORIES-1:0]    assigned_memory_to_read_port_r[NUM_READ_PORTS-1:0]; // assigned memory to each read port
//
reg [LOG_NUM_WRITE_PORTS-1:0] sel_write_port_r[NUM_MEMORIES-1:0];                   // assigned write port to each memory
reg [NUM_MEMORIES-1:0]        is_memory_assigned_to_write_port_r;                   // whether a memory is assigned to a write por

// read interface to memories (mux & demux)
generate
  for (i=0; i<NUM_MEMORIES; i=i+1) begin
    assign read_w[i]         = is_memory_assigned_to_read_port_r[i] & read_in[sel_read_port_r[i]];
    assign addr_read_w[i]    = read_addr_in[((sel_read_port_r[i]+1)*LOG_MAX_ADDRESS)-1 -: LOG_MAX_ADDRESS];
  end
  for (i=0; i<NUM_READ_PORTS; i=i+1) begin
    assign read_valid_out[i] = is_assigned_read_port_to_memory_r[i] ? valid_read_w[assigned_memory_to_read_port_r[i]] : 0;
    assign read_data_out[((i+1)*DATA_WIDTH)-1 -: DATA_WIDTH] = is_assigned_read_port_to_memory_r[i] ? data_read_w[assigned_memory_to_read_port_r[i]] : 0;
  end
endgenerate

// write interface to memories (mux & demux)
generate
  for (i=0; i<NUM_MEMORIES; i=i+1) begin
    assign write_w[i]       = is_memory_assigned_to_write_port_r[i] & write_in[sel_write_port_r[i]];
    assign addr_write_w[i]  = write_addr_in[((sel_write_port_r[i]+1)*LOG_MAX_ADDRESS)-1 -: LOG_MAX_ADDRESS];
    assign data_write_w[i]  = write_data_in[((sel_write_port_r[i]+1)*DATA_WIDTH)-1 -: DATA_WIDTH];
  end
endgenerate

// modules
generate
  for (i = 0; i<NUM_MEMORIES; i=i+1) begin
   // input weight memory
   MEM #(
     .DATA_WIDTH      ( DATA_WIDTH      ),
     .NUM_ADDRESSES   ( NUM_ADDRESSES   ),
     .LOG_MAX_ADDRESS ( LOG_MAX_ADDRESS )
   ) mem_m (
     .clk             ( clk                       ),
     .rst             ( rst                       ),
     .data_write      ( data_write_w[i]           ),
     .addr_write      ( addr_write_w[i]           ),
     .write           ( write_w[i]                ),
     .addr_read       ( addr_read_w[i]            ),
     .read            ( read_w[i]                 ),
     .data_read       ( data_read_w[i]            ),
     .valid_out       ( valid_read_w[i]           )
    );
  end
endgenerate

integer m;
integer p;

// sequential
always @ (posedge clk) begin
  if (~rst) begin
    for (m=0; m<NUM_MEMORIES; m=m+1) begin
      sel_read_port_r[m] <= 0;
      sel_write_port_r[m] <= 0;
      is_memory_assigned_to_read_port_r[m] <= 1'b0;
      is_memory_assigned_to_write_port_r[m] <= 1'b0;
    end
    for (p=0; p<NUM_READ_PORTS; p=p+1) begin
      is_assigned_read_port_to_memory_r[p] = 0;
      assigned_memory_to_read_port_r[p] = 0;
    end
  end else begin
    if (cmd_assign) begin
      sel_read_port_r[memory] <= read_port;
      sel_write_port_r[memory]<= write_port;
      is_assigned_read_port_to_memory_r[read_port] <= 1'b1;
      is_memory_assigned_to_read_port_r[memory] <= 1'b1;
      is_memory_assigned_to_write_port_r[memory] <= 1'b1;
      assigned_memory_to_read_port_r[read_port] <= memory;
    end
    else if (cmd_unassign) begin
      is_assigned_read_port_to_memory_r[read_port] <= 1'b0;
      is_memory_assigned_to_read_port_r[memory] <= 1'b0;
      is_memory_assigned_to_write_port_r[memory] <= 1'b0;
      assigned_memory_to_read_port_r[read_port] <= memory;
    end
  end
end

// debug support
// synthesis translate_off
`ifdef DEBUG_MEMORY_BANK
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (cmd_assign) $display("MEMORY_BANK: cycle %d assigning memory %d to read port %d and write port %d", tics, memory, read_port, write_port);
      if (cmd_unassign) $display("MEMORY_BANK: cycle %d unassigning memory %d from read port %d and write port %d", tics, memory, read_port, write_port);
      tics <= tics + 1;
    end
  end
`endif
// synthesis translate_on


endmodule

// Module MEMORY
// Provides a memory for a given data width and number of addresses. Separate read and write ports
// are provided.
//

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

// debug support
// synthesis translate_off
`ifdef DEBUG_MEMORY
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (write) $display("MEMORY: cycle %d write operation address %x data %x", tics, addr_write, data_write);
      if (read) $display("MEMORY: cycle %d read operation address %x", tics, addr_read);
      if (valid1) $display("MEMORY: cycle %d read data -> %x", tics, data_read);
      tics <= tics + 1;
    end
  end
`endif
// synthesis translate_on


endmodule

// -------------------------------------------------------------------------------------------
// 4Kx72 memory implemented as 8 512x72 memories 
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
  wire [7:0]  write_bram;
  wire [7:0]  read_bram;
  
  genvar i;
  
  generate
    for (i=0; i<8; i=i+1) begin
      assign write_bram[i] = write & (addr_write[11:9] == i);
      assign read_bram[i] = read & (addr_read[11:9] == i);
    end
  endgenerate
  
  assign data_read = data_read_bram[addr_read[11:9]];
  
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
        .WRITE_MODE          ( "READ_FIRST"           )  // Specify "READ_FIRST" for same clock or synchronous clocks, Specify "WRITE_FIRST for asynchronous clocks on ports
     ) BRAM_SDP_MACRO_inst_0 (
        .DO            ( data_read_bram[i]  ), // Output read data port, width defined by READ_WIDTH parameter
        .DI            ( data_write         ), // Input write data port, width defined by WRITE_WIDTH parameter
        .RDADDR        ( addr_read[8:0]     ), // Input read address, width defined by read port depth
        .RDCLK         ( clk                ), // 1-bit input read clock
        .RDEN          ( read_bram[i]       ), // 1-bit input read port enable
        .REGCE         ( 0                  ), // 1-bit input read output register enable
        .RST           ( ~rst               ), // 1-bit input reset
        .WE            ( 9'b111111111       ), // Input write enable, width defined by write port depth
        .WRADDR        ( addr_write[8:0]    ), // Input write address, width defined by write port depth
        .WRCLK         ( clk                ), // 1-bit input write clock
        .WREN          ( write_bram[i]      )  // 1-bit input write port enable
     );
   end
  endgenerate
      
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
  
  assign data_read = data_read_bram[ addr_read[11:9] ];
  
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

  wire [31:0] data_read_bram[3:0];
  wire [3:0]  write_bram;
  wire [3:0]  read_bram;
  wire [31:0] data_read_final;
  wire [31:0] data_write_final;
  
  genvar i;
  
  generate
    for (i=0; i<4; i=i+1) begin
      assign write_bram[i] = write & (addr_write[11:10] == i);
      assign read_bram[i] = read & (addr_read[11:10] == i);
    end
  endgenerate
  
  assign data_read_final = data_read_bram[addr_read[11:10]];
  
  assign data_read = data_read_final;
  assign data_write_final = data_write;
  
  //assign data_read = {data_read_final[34:27],data_read_final[25:18],data_read_final[16:9],data_read_final[7:0]};
  //assign data_write_final = {1'b0,data_write[34:27],1'b0,data_write[25:18],1'b0,data_write[16:9],1'b0,data_write[7:0]};
  
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
        .WRITE_WIDTH   ( 32        ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
        .READ_WIDTH    ( 32        ), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
        .DO_REG        ( 0         ), // Optional output register (0 or 1)
        .INIT_FILE     ( "NONE"    ),
        .SIM_COLLISION_CHECK ( "ALL"                           ), // Collision check enable "ALL", "WARNING_ONLY", "GENERATE_X_ONLY" or "NONE" 
        .SRVAL               ( 72'h000000000000000000          ), // Set/Reset value for port output
        .INIT                ( 72'h000000000000000000          ), // Initial values on output port
        .WRITE_MODE          ( "READ_FIRST"                    ) // Specify "READ_FIRST" for same clock or synchronous clocks, Specify "WRITE_FIRST" for asynchronous clocks on ports

       ) BRAM_SDP_MACRO_inst_0 (
        .DO            ( data_read_bram[i]  ), // Output read data port, width defined by READ_WIDTH parameter
        .DI            ( data_write_final   ), // Input write data port, width defined by WRITE_WIDTH parameter
        .RDADDR        ( addr_read[9:0]     ), // Input read address, width defined by read port depth
        .RDCLK         ( clk                ), // 1-bit input read clock
        .RDEN          ( read_bram[i]       ), // 1-bit input read port enable
        .REGCE         ( 0                  ), // 1-bit input read output register enable
        .RST           ( ~rst               ), // 1-bit input reset
        .WE            ( 4'b1111            ), // Input write enable, width defined by write port depth
        .WRADDR        ( addr_write[9:0]    ), // Input write address, width defined by write port depth
        .WRCLK         ( clk                ), // 1-bit input write clock
        .WREN          ( write_bram[i]      )  // 1-bit input write port enable
       );
     end
   endgenerate
   
 endmodule