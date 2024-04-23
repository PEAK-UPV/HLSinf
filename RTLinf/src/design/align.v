// ALIGN module
//
// This module alings the input and sends it downstream
//
// The module has one input port which receives a set of items (GROUP_SIZE items). Input data is stored in a FIFO.
//
// The module, when configured, will run for a given number of iterations and within each
// iteration will perform a given number of "read" operations.
// 
// A "read" operation will be run when data is available at the input and output is available. In a "read" operation
// the module will align the input. Currently a transparent move between input and output is performed.
// 
//

`include "RTLinf.vh"

module ALIGN #(
    parameter GROUP_SIZE             = 4,                            // number of inputs (group size)
    parameter DATA_WIDTH             = 8,                            // input data width (output width is input width)
    parameter LOG_MAX_ITERS          = 16,                           // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16                            // number of bits for max reads per iter
  )(
    input                                   clk,                     // clock signal
    input                                   rst,                     // reset signal
  
    input [GROUP_SIZE * DATA_WIDTH - 1 : 0] data_in,                 // IN interface:: data
    input                                   valid_in,                // IN interface:: valid in
    output                                  avail_out,               // IN interface:: avail
  
    output [GROUP_SIZE*DATA_WIDTH-1:0]      data_out,                // OUT interface: data
    output                                  valid_out,               // OUT interface: valid
    input                                   avail_in                 // OUT interface: avail
  );

// regs
reg [GROUP_SIZE * DATA_WIDTH - 1 : 0] data_r;
reg                                   data_in_r;
  
// wires
wire                             perform_operation_w;       // whether we perform a "read" operation in this cycle


// combinational logic
assign perform_operation_w = avail_in & data_in_r;
assign data_out            = data_r;  // for the moment no alignment
assign valid_out           = perform_operation_w;
assign avail_out           = 1'b1;


// sequential logic
always @ (posedge clk) begin
  if (~rst) begin
    data_in_r <= 1'b0;
  end else begin
    if (valid_in) begin
      data_r <= data_in;
      data_in_r <= 1'b1;
    end else begin
      data_in_r <= 1'b0;
    end
  end
end

// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented
// the module has a tics counter register to keep up with current cycle
//
// in this module whenever a "read" cycle is performed the associated information is shown as debug
//

// synthesis translate_off

`ifdef DEBUG_ALIGN
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (perform_operation_w) $display("ALIGN: cycle %d, output data %x", tics, data_out);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on
  
endmodule
  
  
