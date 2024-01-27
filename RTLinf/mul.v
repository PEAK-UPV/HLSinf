// MUL module
//
// This module perfoms a set of multiplications
//

module MUL #(
  parameter NUM_INPUTS = 8, // number of inputs (outpus == inputs)
  parameter DATA_WIDTH = 8, // input data width (output is 2x input width)
  parameter LOG_MAX_ITERS = 16,                                  // number of bits for max iters register
  parameter LOG_MAX_READS_PER_ITER = 16                          // number of bits for max reads per iter
)(
  input clk,
  input rst,

  input                                   configure,               // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]               num_iters,               // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration

  input [NUM_INPUTS * DATA_WIDTH - 1 : 0] act_data_in,             // ACTIVATION interface:: activations data
  input                                   act_valid_in,            // ACTIVATION interface:: activation valid in
  output                                  act_avail_out,           // ACTIVATION interface:: avail

  input [DATA_WIDTH-1 : 0]                weight_data_in,          // WEIGHT interface:: weight data
  input                                   weight_valid_in,         // WEIGHT interface:: weight valid in
  output                                  weight_avail_out,        // WEIGHT interface:: avail

  output [2*NUM_INPUTS*DATA_WIDTH-1:0]    data_out,                // OUT interface: data
  output                                  valid_out,               // OUT interface: valid
  input                                   avail_in                 // OUT interface: avail
);

// wires

// registers


// combinational logic

// modules

// sequential logic

endmodule

