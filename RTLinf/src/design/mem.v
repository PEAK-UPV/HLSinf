// Module MEM
//
// This module implements a Memory with one write port and one read port. This memory
// can be implemented as a BRAM.
//

module MEM #(
    parameter DATA_WIDTH = 8,               // data width
    parameter NUM_ADDRESSES = 65536,        // number of addresses
    parameter LOG_MAX_ADDRESS = 16,         // number of bits for addresses
    parameter ID = 0                        // identifier (useful for debug and initialization for simulation)
)(
  input clk,                                // clock input
  input rst,                                // needed for debug

  input [DATA_WIDTH-1:0]        data_write, // WRITE interface:: data
  input [LOG_MAX_ADDRESS-1:0]   addr_write, // WRITE interface:: address
  input                         write,      // WRITE interface:: write signal

  input [LOG_MAX_ADDRESS-1:0]   addr_read,  // READ interface:: address
  input                         read,       // READ interface:: read signal
  output reg [DATA_WIDTH-1:0]   data_read,  // READ interface:: data
  output reg                    valid_out   // READ interface:: valid
);

// wires

// registers
reg [DATA_WIDTH-1:0]    mem[NUM_ADDRESSES-1:0];

integer i;
begin initial
if (DATA_WIDTH==8) begin
  for (i=0; i<NUM_ADDRESSES; i=i+1) mem[i] <= i+1; 
end
end

begin initial
if (DATA_WIDTH==16) begin
  for (i=0; i<NUM_ADDRESSES; i=i+1) mem[i] <= (((i*2)+2) << 8) + ((i*2)+1);
end
end

begin initial
if (DATA_WIDTH==32) begin
  for (i=0; i<NUM_ADDRESSES; i=i+1) mem[i] <= (((i*4)+4) << 24) + (((i*4)+3) << 16) + (((i*4)+2) << 8) + ((i*4)+1);
end
end

begin initial
if (DATA_WIDTH==64) begin
  for (i=0; i<NUM_ADDRESSES; i=i+1) mem[i] <= (((i*8)+8) << 56) + (((i*8)+7) << 48) + (((i*8)+6) << 40) + (((i*8)+5) << 32) +
                                               (((i*8)+4) << 24) + (((i*8)+3) << 16) + (((i*8)+2) << 8) + ((i*8)+1);
end
end

begin initial
if (DATA_WIDTH==72) begin
  for (i=0; i<NUM_ADDRESSES; i=i+1) mem[i] <= (((i*8)+9) << 64) + (((i*8)+8) << 56) + (((i*8)+7) << 48) + (((i*8)+6) << 40) + (((i*8)+5) << 32) +
                                               (((i*8)+4) << 24) + (((i*8)+3) << 16) + (((i*8)+2) << 8) + ((i*8)+1);
end
end

// sequential logic for write logic
always @ (posedge clk) if (write) mem[addr_write] <= data_write;

// sequential logic for read
always @ (posedge clk) if (read) begin data_read <= mem[addr_read]; valid_out <= 1; end else begin valid_out <= 0; end

// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented
// the module has a tics counter register to keep up with current cycle
//
// in this module whenever a read (valid_out) event occurs the associated information is shown as debug
//

// synthesis translate_off

//`define DEBUG_MEM

`ifdef DEBUG_MEM
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (valid_out) $display("MEM (read): cycle %d data_out %x", tics, data_read);
      if (write) $display("MEM (write): cycle %d address %x data %x", tics, addr_write, data_write);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on

endmodule

// Module MEMU
//
// This module implements a Memory with one write port and one read port. The
// read port is unregistered and there is no need to have a read signal. This memory
// can be implemented as a BRAM.
//

module MEMU #(
    parameter DATA_WIDTH = 8,               // data width
    parameter NUM_ADDRESSES = 65536,        // number of addresses
    parameter LOG_MAX_ADDRESS = 16          // number of bits for addresses
)(
  input clk,                                // clock input
  input rst,                                // needed for debug

  input [DATA_WIDTH-1:0]        data_write, // WRITE interface:: data
  input [LOG_MAX_ADDRESS-1:0]   addr_write, // WRITE interface:: address
  input                         write,      // WRITE interface:: write signal

  input [LOG_MAX_ADDRESS-1:0]   addr_read,  // READ interface:: address
  output [DATA_WIDTH-1:0]       data_read   // READ interface:: data
);

// wires

// registers
reg [DATA_WIDTH-1:0]    mem[NUM_ADDRESSES-1:0];

integer i;
begin initial
for (i=0; i<NUM_ADDRESSES; i=i+1) mem[i] <= i;
end

// combinational logic
assign data_read = mem[addr_read];


// sequential logic for write logic
always @ (posedge clk) if (write) mem[addr_write] <= data_write;

// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented
// the module has a tics counter register to keep up with current cycle
//
// for the moment no debug information is shown
//

//`define DEBUG

`ifdef DEBUG
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      tics <= tics + 1;
    end
  end
`endif


endmodule
