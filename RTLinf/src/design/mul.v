// MUL module
//
// This module perfoms a set of multiplications.
// The module has one input port which receives a set of items (GROUP_SIZE items). Input data is stored in a FIFO.
// Anothre input will receive a weight which will be stored in a register.
//
// The module, when configured, will run for a given number of iterations and within each
// iteration will perform a given number of "read" operations.
// 
// A "read" operation will be run when data is available at the input and output is available. In a "read" operation
// the module will multiply the input data (each item) by the current weight.
// 

`include "RTLinf.vh"

module MUL #(
  parameter GROUP_SIZE             = 4,                          // group size
  parameter DATA_WIDTH             = 8,                          // input data width (output is 2x input width)
  parameter LOG_MAX_ITERS          = 16,                         // number of bits for max iters register
  parameter LOG_MAX_READS_PER_ITER = 16                          // number of bits for max reads per iter
)(
  input clk,
  input rst,

  input [GROUP_SIZE * DATA_WIDTH - 1 : 0]  act_data_in,             // ACTIVATION interface:: activations data
  input                                    act_valid_in,            // ACTIVATION interface:: activation valid in
  output                                   act_avail_out,           // ACTIVATION interface:: avail

  input [DATA_WIDTH-1 : 0]                 weight_data_in,          // WEIGHT interface:: weight data
  input                                    weight_valid_in,         // WEIGHT interface:: weight valid in
  output                                   weight_avail_out,        // WEIGHT interface:: avail

  output [GROUP_SIZE*2*DATA_WIDTH-1:0]     data_out,                // OUT interface: data
  output                                   valid_out,               // OUT interface: valid
  input                                    avail_in                 // OUT interface: avail
);

// wires
wire                             perform_operation_w;       // whether we perform an operation in this cycle

// registers
reg [DATA_WIDTH-1:0]             weight_r;                  // weight
reg [GROUP_SIZE * DATA_WIDTH-1:0]act_r;                     // activations
reg                              act_valid_r;               // activation valid flag

genvar i;

// combinational logic
assign weight_avail_out = 1'b1;                                         // always ready
assign perform_operation_w = act_valid_r; 
generate
  for (i=0; i<GROUP_SIZE; i=i+1) begin
    assign data_out[((i+1)*2*DATA_WIDTH)-1:i*2*DATA_WIDTH] = act_r[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] * weight_r;
  end
endgenerate
assign valid_out = perform_operation_w;

assign act_avail_out = 1'b1;

// sequential logic

// weight register
always @ (posedge clk) begin
  if (~rst) begin
    weight_r <= 0;
  end else begin
    if (weight_valid_in) weight_r <= weight_data_in;
  end
end

// activation register
always @ (posedge clk) begin
  if (~rst) begin
    act_r <= 0;
    act_valid_r <= 1'b0;
  end else begin
    if (act_valid_in) begin
      act_r <= act_data_in;
      act_valid_r <= 1'b1;
    end else begin
      act_valid_r <= 1'b0;
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

`ifdef DEBUG_MUL
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (perform_operation_w) $display("MUL: cycle %d, output data %x", tics, data_out);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on

endmodule

