// Module WRITE
//
// This module writes data to a blockram. A FIFO is implemented to store the 
// data read from the upstream module. Two FSM machines are created, one (read_fsm) to read from the upstream
// module and write to the FIFO and one (write_fsm) to write the data to the output interface (to the 
// block ram) and read from the FIFO. 
//
// The module is configured with num_iterations and num_reads_per_iteration. The module runs
// each iteration the cycles needed to perform num_reads_per_iteration reads. The write_fsm machine
// completes all writes even if the module is disabled (the fifo still has some pending data to send)
//

`define FSM_IDLE  0
`define FSM_READ  1
`define FSM_WRITE 2

module WRITE #(
    parameter NUM_INPUTS = 8,                                      // number of inputs (number of outputs = number of inputs)
    parameter INPUT_DATA_WIDTH = 8,                                // input data width
    parameter OUTPUT_DATA_WIDTH = 8,                               // output data width
    parameter LOG_MAX_ITERS = 16,                                  // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16,                         // number of bits for max reads per iter
    parameter LOG_MAX_ADDRESS = 16                                 // number of bits for addresses
)(
  input clk,                                                       // clock input
  input rst,                                                       // reset input

  input configure,                                                 // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]          num_iters,                    // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter,           // CONFIGURE interface:: number of reads per iteration
  input [LOG_MAX_ADDRESS-1:0]        base_address,                 // CONFIGURE interface:: address for writes
  input [OUTPUT_DATA_WIDTH-1:0]      min_clip,                     // CONFIGURE interface:: clip minimum value
  input [OUTPUT_DATA_WIDTH-1:0]      max_clip,                     // CONFIGURE interface:: clip maximum value

  input [INPUT_DATA_WIDTH-1:0] data_in,                            // IN interface:: input valid data
  input valid_in,                                                  // IN interface:: input valid signal
  output avail_out,                                                // IN interface:: avail signal

  output [OUTPUT_DATA_WIDTH-1:0] data_out,                         // OUT interface:: output data
  output address_out,                                              // OUT interface:: address
  output valid_out                                                 // OUT interface:: valid
);

// wires

// registers

// combinational logic

// modules

// sequential logic


endmodule
