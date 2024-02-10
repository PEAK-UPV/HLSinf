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
  
    input                                   configure,               // CONFIGURE interface:: configure signal
    input [LOG_MAX_ITERS-1:0]               num_iters,               // CONFIGURE interface:: number of iterations for reads
    input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration
  
    input [GROUP_SIZE * DATA_WIDTH - 1 : 0] data_in,                 // IN interface:: data
    input                                   valid_in,                // IN interface:: valid in
    output                                  avail_out,               // IN interface:: avail
  
    output [GROUP_SIZE*DATA_WIDTH-1:0]      data_out,                // OUT interface: data
    output                                  valid_out,               // OUT interface: valid
    input                                   avail_in                 // OUT interface: avail
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

// combinational logic
assign data_write_w        = data_in;
assign write_w             = valid_in;
assign avail_out           = ~almost_full_w & ~full_w;
assign next_read_w         = perform_operation_w;
assign perform_operation_w = module_enabled_r & (~empty_w) & avail_in;
assign data_out            = data_read_w;  // for the moment no alignment
assign valid_out           = perform_operation_w;


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
      if (perform_operation_w) begin
        if (num_reads_per_iter_r == 1) begin
          if (num_iters_r == 1) module_enabled_r <= 0;
          else begin
            num_iters_r <= num_iters_r - 1;
            num_reads_per_iter_r <= num_reads_per_iter_copy_r;
          end
        end else begin
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
  
  
