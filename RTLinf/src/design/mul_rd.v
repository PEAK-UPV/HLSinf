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

module MUL_RD #(
  parameter DATA_WIDTH             = 8,                          // input data width (output is 2x input width)
  parameter GROUP_SIZE             = 4,
  localparam REP_INFO              = GROUP_SIZE*GROUP_SIZE,      // number of bits for repetition detectoor
  localparam INPUT_WIDTH           = 2 * DATA_WIDTH + REP_INFO,  // number of bits for input (activation + weight + rep. info)
  localparam OUTPUT_WIDTH          = 2 * DATA_WIDTH + REP_INFO   // number of bits for output ( result (2*data width) +  rep. info)

)(
  input clk,
  input rst,

  input [INPUT_WIDTH - 1 : 0]               data_in,                 // ACTIVATION & WEIGHT interface:: activ ations data
  input                                     valid_in,                // ACTIVATION & WEIGHT interface:: activ ation valid in
  output                                    avail_out,               // ACTIVATION & WEIGHT interface:: avail 

  output [OUTPUT_WIDTH - 1 : 0]             data_out,                // OUT interface: data
  output                                    valid_out,               // OUT interface: valid
  input                                     avail_in                 // OUT interface: avail
);

// wires
wire                               perform_operation_w;               // whether we perform a "read" operation in this cycle
wire [DATA_WIDTH-1:0] act_w;
wire [DATA_WIDTH-1:0] weight_w;

// registers
reg [INPUT_WIDTH-1:0]             data_r;                    // activations
reg                               act_valid_r;               // activation valid flag

wire [2 * DATA_WIDTH - 1 : 0]      result;                            // Result

// combinational logic
assign perform_operation_w                          = act_valid_r;
assign act_w                                        = data_r[DATA_WIDTH-1:0];
assign weight_w                                     = data_r[2 * DATA_WIDTH-1 : DATA_WIDTH];
assign result                                       = act_w * weight_w;
assign data_out[2 * DATA_WIDTH - 1 : 0 ]            = result;
assign data_out[OUTPUT_WIDTH - 1 : 2 * DATA_WIDTH ] = data_r[OUTPUT_WIDTH - 1 : 2 * DATA_WIDTH];
assign valid_out                                    = perform_operation_w;
assign avail_out = 1'b1; // always ready

// data register
always @ (posedge clk) begin
  if (~rst) begin
    data_r <= 0;
    act_valid_r <= 1'b0;
  end else begin
    if (valid_in) begin
      data_r <= data_in;
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
      if (perform_operation_w) $display("MUL: cycle %d, act %x weight %x output data %x", tics, act_w, weight_w, data_out[2 * DATA_WIDTH - 1 : 0 ]);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on

endmodule

