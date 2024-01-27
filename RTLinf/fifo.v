// Module FIFO
//
// This module implements a FIFO with one write port and one read port. The output of the FIFO
// is combinational (the data at the head of the FIFO is always exposed). A control signal is
// used to discard the head of the FIFO (next_read). An almost_full signal is outputed to indicate
// only one slot is available.
//

`include "RTLinf.vh"

module FIFO #(
    parameter NUM_SLOTS = 4,              // number of slots (depth)
    parameter LOG_NUM_SLOTS = 2,          // number of bits for number of slots
    parameter DATA_WIDTH = 8              // data width
)(
  input clk,                              // clock input
  input rst,                              // reset input

  input [DATA_WIDTH-1:0] data_write,      // WRITE interface:: data
  input                  write,           // WRITE interface:: write signal
  output                 full,            // WRITE interface:: fifo is full
  output                 almost_full,     // WRITE interface:: fifo is almost full

  output [DATA_WIDTH-1:0] data_read,      // READ interface:: data
  input                   next_read,      // READ interface:: next read
  output                  empty           // READ interface:: fifo is empty
);

// wires

// registers
reg [DATA_WIDTH-1:0]    fifo[NUM_SLOTS-1:0];
reg [LOG_NUM_SLOTS-1:0] read_ptr;
reg [LOG_NUM_SLOTS-1:0] write_ptr;
reg [LOG_NUM_SLOTS:0]   counter;

// cobinational logic
assign full        = counter == NUM_SLOTS;
assign almost_full = counter == NUM_SLOTS-1;
assign empty       = counter == 0;
assign data_read   = fifo[read_ptr];

// sequential logic for write logic
always @ (posedge clk) begin
  if (~rst) begin
    write_ptr <= 0;
  end else begin
    if (write) begin
      fifo[write_ptr] <= data_write;
      write_ptr <= write_ptr + 1;
    end
  end
end

// sequential logic for counter
always @ (posedge clk) begin
  if (~rst) begin
    counter <= 0;
  end else begin
    if (write) begin
      if (~next_read) counter <= counter + 1;
    end else begin
      if (next_read & (|counter)) counter <= counter - 1;
    end
  end 
end

// sequential logic for read
always @ (posedge clk) begin
  if (~rst) begin
    read_ptr <= 0;
  end else begin
    if (next_read) read_ptr <= read_ptr + 1;
  end
end

`define DEBUG

`ifdef DEBUG
reg [63:0] tics;

always @ (posedge clk) begin
  if (~rst) begin
    tics <= 0;
  end else begin
    if (write) $display("%d, FIFO (write): data_write %x", tics, data_write);
    if (next_read) $display("%d, FIFO (next_read): counter %d", tics, counter);
    tics <= tics + 1;
  end
end
`endif

endmodule
