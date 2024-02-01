// Module MEM
//
// This module implements a Memory with one write port and one read port. This memory
// can be implemented as a BRAM.
//

`include "RTLinf.vh"

module MEM #(
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
  input                         read,       // READ interface:: read signal
  output reg [DATA_WIDTH-1:0]   data_read,  // READ interface:: data
  output reg                    valid_out   // READ interface:: valid
);

// wires

// registers
reg [DATA_WIDTH-1:0]    mem[NUM_ADDRESSES-1:0];

integer i;
begin initial
for (i=0; i<NUM_ADDRESSES; i=i+1) mem[i] <= i;
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

//`define DEBUG

`ifdef DEBUG
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (valid_out) $display("MEM: cycle %d data_out %x", tics, data_read);
      tics <= tics + 1;
    end
  end
`endif


endmodule
