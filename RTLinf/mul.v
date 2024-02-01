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

module MUL #(
  parameter GROUP_SIZE = 8,                                      // group size
  parameter DATA_WIDTH = 8,                                      // input data width (output is 2x input width)
  parameter LOG_MAX_ITERS = 16,                                  // number of bits for max iters register
  parameter LOG_MAX_READS_PER_ITER = 16                          // number of bits for max reads per iter
)(
  input clk,
  input rst,

  input                                    configure,               // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]                num_iters,               // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0]       num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration

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
wire [GROUP_SIZE * DATA_WIDTH - 1: 0] data_write_w;                      // data to write to FIFO
wire                                  write_w;                           // write signal to FIFO
wire                                  full_w;                            // full signal from FIFO
wire                                  almost_full_w;                     // almost_full signal from FIFO
wire [GROUP_SIZE * DATA_WIDTH - 1: 0] data_read_w;                       // data read from FIFO
wire                                  next_read_w;                       // next_read signal to FIFO
wire                                  empty_w;                           // empty signal from FIFO
wire                                  perform_operation_w;               // whether we perform a "read" operation in this cycle

// registers
reg [LOG_MAX_ITERS-1:0]          num_iters_r;               // FIFO
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_copy_r; // copy of number of reads per iteration
reg                              module_enabled_r;          // module enabled
reg [DATA_WIDTH-1:0]             weight_r;                  // weight

genvar i;

// combinational logic
assign act_avail_out = ~almost_full_w & ~full_w;
assign weight_avail_out = 1'b1;                                         // always ready
assign perform_operation_w = module_enabled_r & (~empty_w) & avail_in;
generate
  for (i=0; i<GROUP_SIZE; i=i+1) begin
    assign data_out[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] = data_read_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] * weight_r;
  end
endgenerate
assign valid_out = perform_operation_w;

// modules

// input fifo
FIFO #(
  .NUM_SLOTS     ( 4                       ),
  .LOG_NUM_SLOTS ( 2                       ),
  .DATA_WIDTH    ( GROUP_SIZE * DATA_WIDTH )
) fifo_in (
  .clk           ( clk                     ),
  .rst           ( rst                     ),
  .data_write    ( data_write_w            ),
  .write         ( write_w                 ),
  .full          ( full_w                  ),
  .almost_full   ( almost_full_w           ),
  .data_read     ( data_read_w             ),
  .next_read     ( next_read_w             ),
  .empty         ( empty_w                 )
);

// sequential logic

// weight register
always @ (posedge clk) begin
  if (~rst) begin
    weight_r <= 0;
  end else begin
    if (weight_valid_in) weight_r <= weight_data_in;
  end
end

// configuration and iterations
// whenever we perform a "read" operation we decrement the number of reads per iteration
// When the reads per iteration reaches zero we decrement number of iterations and restore
// the reads per iteration. If number of iterations reaches zero
// then we disable the module. 
//
always @ (posedge clk) begin
  if (~rst) begin
    num_iters_r          <= 0;
    num_reads_per_iter_r <= 0;
    module_enabled_r     <= 1'b0;
  end else begin
    if (configure) begin
      num_iters_r          <= num_iters;
      num_reads_per_iter_r <= num_reads_per_iter;
      num_reads_per_iter_copy_r <= num_reads_per_iter;
      module_enabled_r     <= 1'b1;
    end else begin
      if (num_reads_per_iter_r == 1) begin
        if (num_iters_r == 1) module_enabled_r <= 0;
        else begin
          num_iters_r <= num_iters_r - 1;
          num_reads_per_iter_r <= num_reads_per_iter_copy_r;
        end
      end else begin
        if (perform_operation_w) begin
          num_reads_per_iter_r <= num_reads_per_iter_r - 1;
        end
      end
    end
  end 
end

// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented
// the module has a tics counter register to keep up with current cycle
//
// in this module whenever a "read" cycle is performed the associated information is shown as debug
//

`define DEBUG

`ifdef DEBUG
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (perform_operation_w) $display("MUL: cycle %d, output data %x", tics, data_out);
      tics <= tics + 1;
    end
  end
`endif

endmodule

