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
  parameter LOG_MAX_READS_PER_ITER = 16,                         // number of bits for max reads per iter
  localparam ZERO_INFO             = GROUP_SIZE,      // number of bits for repetition detectoor
  localparam INPUT_WIDTH           = 2 * DATA_WIDTH + ZERO_INFO,  // number of bits for input (activation + weight + rep. info)
  localparam OUTPUT_WIDTH          = 2 * DATA_WIDTH + ZERO_INFO   // number of bits for output ( result (2*data width) +  rep. info)

)(
  input clk,
  input rst,

  input                                     configure,               // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS - 1 : 0]             num_iters,               // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER - 1 : 0]    num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration

  input [INPUT_WIDTH - 1 : 0]               data_in,                 // ACTIVATION & WEIGHT interface:: activ ations data
  input                                     valid_in,                // ACTIVATION & WEIGHT interface:: activ ation valid in
  output                                    avail_out,               // ACTIVATION & WEIGHT interface:: avail 

  output [OUTPUT_WIDTH - 1 : 0]             data_out,                // OUT interface: data
  output                                    valid_out,               // OUT interface: valid
  input                                     avail_in                 // OUT interface: avail
);

// wires
wire [INPUT_WIDTH - 1: 0]          data_write_w;                      // data to write to FIFO
wire                               write_w;                           // write signal to FIFO
wire                               full_w;                            // full signal from FIFO
wire                               almost_full_w;                     // almost_full signal from FIFO
wire [INPUT_WIDTH- 1: 0]           data_read_w;                       // data read from FIFO
wire                               next_read_w;                       // next_read signal to FIFO
wire                               empty_w;                           // empty signal from FIFO
wire                               perform_operation_w;               // whether we perform a "read" operation in this cycle

wire [ DATA_WIDTH - 1: 0]          act_data_in;                       // Activation value    
wire [ DATA_WIDTH - 1: 0]          weight_data_in;                    // Weight value
wire [ ZERO_INFO - 1: 0]           zero_info;                          // Repetition information
wire [2 * DATA_WIDTH - 1 : 0]      result;                            // Result
// registers
reg [LOG_MAX_ITERS-1:0]          num_iters_r;               // FIFO
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_copy_r; // copy of number of reads per iteration
reg                              module_enabled_r;          // module enabled

genvar i;

// combinational logic
assign perform_operation_w = /*module_enabled_r &*/ (~empty_w) & avail_in;

//Extract input from FIFO
assign act_data_in = data_read_w[DATA_WIDTH - 1 : 0];
assign weight_data_in = data_read_w[ 2 * DATA_WIDTH - 1 : DATA_WIDTH];
assign zero_info = data_read_w[ INPUT_WIDTH - 1 : 2 * DATA_WIDTH];

//Output
assign result                                       = act_data_in * weight_data_in;
assign data_out[2 * DATA_WIDTH - 1 : 0 ]            =  result;
assign data_out[OUTPUT_WIDTH - 1 : 2 * DATA_WIDTH ] =  zero_info;
assign valid_out                                    = perform_operation_w;

//Get input 

// FIFO write and read
assign data_write_w = data_in;
assign write_w      = valid_in;
assign avail_out    = ~almost_full_w & ~full_w;
assign next_read_w  = perform_operation_w;

// modules

// input fifo
FIFO #(
  .NUM_SLOTS     ( 4                       ),
  .LOG_NUM_SLOTS ( 2                       ),
  .DATA_WIDTH    ( INPUT_WIDTH             )
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

`ifdef DEBUG_MUL
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (perform_operation_w) $display("MUL: cycle %d, output data %x", tics, data_out[2 * DATA_WIDTH - 1 : 0 ]);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on

endmodule

