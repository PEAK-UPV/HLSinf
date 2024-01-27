// Module DISTRIBUTE_IN
//
// This module distributes input data to outputs based on specific configuration
//

module DISTRIBUTE_IN #(
    parameter NUM_DATA_INPUTS  = 8,  // number of data inputs
    parameter DATA_WIDTH       = 8,  // input and output data width
    parameter NUM_DATA_OUTPUTS = 8,  // number of data outputs
    parameter LOG_MAX_ITERS = 16,          // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16  // number of bits for max reads per iter
) (
  input clk,
  input rst,

  input configure,                                                 // CONFIGURE interface:: configure signal
  input conf_mode,                                                 // CONFIGURE interface:: configuration mode
  input [LOG_MAX_ITERS-1:0]          num_iters,                    // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter,           // CONFIGURE interface:: number of reads per iteration  

  input [NUM_DATA_INPUTS*DATA_WIDTH-1:0] act_data_in,    // ACTIVATION interface:: data
  input  [NUM_DATA_INPUTS-1:0]           act_valid_in,   // ACTIVATION interface:: valid
  output [NUM_DATA_INPUTS-1:0]           act_avail_out,  // ACTIVATION interface:: avail

  input [NUM_DATA_OUTPUTS*DATA_WIDTH-1:0] weights_data_in,    // WEIGHTS interface:: data
  input [NUM_DATA_OUTPUTS-1:0]            weights_valid_in,   // WEIGHTS interface:: valid
  output [NUM_DATA_OUTPUTS-1:0]           weights_avail_out,  // WEIGHTS interface:: avail

  output [NUM_DATA_OUTPUTS*DATA_WIDTH-1:0] data_out,  // OUT interface:: data
  output [NUM_DATA_OUTPUTS-1:0] valid_out,            // OUT interface:: valid
  input  [NUM_DATA_OUTPUTS-1:0] avail_in              // OUT interface:: avail
);

// wires

// registers

// combinational logic

// modules

// sequential logic

endmodule

// Module DISTRIBUTE_OUT
//
// This module distributes input data to outputs based on specific configuration

module DISTRIBUTE_OUT #(
    parameter NUM_DATA_INPUTS  = 8,        // number of data inputs
    parameter DATA_WIDTH       = 8,        // input and output data width
    parameter NUM_DATA_OUTPUTS = 8,        // number of data outputs
    parameter LOG_MAX_ITERS = 16,          // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16  // number of bits for max reads per iter
) (
  input clk,
  input rst,

  input configure,                                                 // CONFIGURE interface:: configure signal
  input conf_mode,                                                 // CONFIGURE interface:: configuration mode
  input [LOG_MAX_ITERS-1:0]          num_iters,                    // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter,           // CONFIGURE interface:: number of reads per iteration  

  input [NUM_DATA_INPUTS*DATA_WIDTH-1:0] data_in,    // IN interface:: data
  input [NUM_DATA_INPUTS-1:0]            valid_in,   // IN interface:: valid
  output [NUM_DATA_INPUTS-1:0]           avail_out,  // IN interface:: avail

  output [NUM_DATA_OUTPUTS*DATA_WIDTH-1:0] data_out,  // OUT interface:: data
  output [NUM_DATA_OUTPUTS-1:0] valid_out,            // OUT interface:: valid
  input  [NUM_DATA_OUTPUTS-1:0] avail_in              // OUT interface:: avail
);

// wires

// registers

// combinational logic

// modules

// sequential logic

endmodule