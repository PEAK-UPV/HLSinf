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
  parameter LOG_MAX_ITERS          = 16,                         // number of bits for max iters register
  parameter LOG_MAX_READS_PER_ITER = 16,                         // number of bits for max reads per iter
  parameter REP_INFO               = GROUP_SIZE + 1,    // number of bits for repetition detectoor
  localparam INPUT_WIDTH           = DATA_WIDTH + REP_INFO,  // number of bits for input (activation + weight + rep. info)
  localparam OUTPUT_WIDTH          = 2 * DATA_WIDTH + REP_INFO   // number of bits for output ( result (2*data width) +  rep. info)

)(
  input clk,
  input rst,
  
  input                               configure,           // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]           num_iters,           // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0]  num_reads_per_iter,  // CONFIGURE interface:: number of reads per iteration

  input [INPUT_WIDTH - 1 : 0]         act_data_in,         // ACTIVATION & WEIGHT interface:: activ ations data
  input                               act_valid_in,        // ACTIVATION & WEIGHT interface:: activ ation valid in
  output                              act_avail_out,       // ACTIVATION & WEIGHT interface:: avail 

  input [DATA_WIDTH - 1 : 0]          weight_data_in,      // WEIGHT interface:: weight data
  input                               weight_valid_in,     // WEIGHT interface:: weight valid in
  output                              weight_avail_out,    // WEIGHT interface:: avail

  output [OUTPUT_WIDTH - 1 : 0]       data_out,            // OUT interface: data
  output                              valid_out,           // OUT interface: valid
  input                               avail_in             // OUT interface: avail
);

//FIFOs
wire [DATA_WIDTH -1 : 0]    weight_data_write_w;        // WEIGHT FIFO :: data to write to FIFO
wire                        weight_write_w;             // WEIGHT FIFO :: write signal to FIFO
wire                        weight_full_w;              // WEIGHT FIFO :: full signal from FIFO
wire                        weight_almost_full_w;       // WEIGHT FIFO :: almost_full signal from FIFO
wire [DATA_WIDTH -1 : 0]    weight_data_read_fifo;      // WEIGHT FIFO :: data read from FIFO
wire                        weight_next_read_w;         // WEIGHT FIFO :: next_read signal to FIFO
wire                        weight_empty_w;             // WEIGHT FIFO :: empty signal from FIFO

// wires
wire                            perform_operation_w;   // whether we perform a "read" operation in this cycle
wire [DATA_WIDTH-1:0]           act_w;
wire [2 * DATA_WIDTH - 1 : 0]   result;                // Result
wire                            is_last;               // Indicates if is last element from group
wire                            next_read_w;


// registers
reg [INPUT_WIDTH-1:0]             data_r;                    // activations
reg                               act_valid_r;               // activation valid flag
reg [LOG_MAX_ITERS-1:0]          num_iters_r;               // 
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_copy_r; // copy of number of reads per iteration
reg                              module_enabled_r;          // module enabled

// WEIGHT FIFO :: write and read
// next read to the fifo
assign next_read_w = perform_operation_w & is_last;
assign weight_data_write_w  = weight_data_in;
assign weight_write_w       = weight_valid_in;
assign weight_avail_out     = ~weight_almost_full_w && ~weight_full_w;
assign weight_next_read_w   = perform_operation_w  & (num_reads_per_iter_r == 1) & next_read_w;

// combinational logic
assign perform_operation_w                          = act_valid_r;
assign act_w                                        = data_r[DATA_WIDTH-1:0];
assign is_last                                      = data_r[INPUT_WIDTH-1];
assign result                                       = act_w * weight_data_read_fifo;
assign data_out[2 * DATA_WIDTH - 1 : 0 ]            = result;
assign data_out[OUTPUT_WIDTH - 1 : 2 * DATA_WIDTH ] = data_r[DATA_WIDTH + REP_INFO - 1 : DATA_WIDTH];
assign valid_out                                    = perform_operation_w;
assign act_avail_out = 1'b1; // always ready

// data register
always @ (posedge clk) begin
  if (~rst) begin
    data_r <= 0;
    act_valid_r <= 1'b0;
  end else begin
    if (act_valid_in) begin
      data_r <= act_data_in;
      act_valid_r <= 1'b1;
    end else begin
      act_valid_r <= 1'b0;
    end
  end
end


// sequential logic

// configuration and iterations
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
      if (next_read_w) begin //perform_operation_w) begin
        if (num_reads_per_iter_r == 1) begin
          if (num_iters_r == 1) module_enabled_r <= 0;
          else begin
            num_iters_r <= num_iters_r - 1;
            num_reads_per_iter_r <= num_reads_per_iter_copy_r;
          end
        end else begin
          ///if(left_elements <= 1) num_reads_per_iter_r <= num_reads_per_iter_r - 1;
          if (next_read_w) num_reads_per_iter_r <= num_reads_per_iter_r - 1;
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
      if (perform_operation_w) $display("MUL: cycle %d, act %x weight %x output data %x", tics, act_w, weight_w, data_out[2 * DATA_WIDTH - 1 : 0 ]);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on


// modules

// weights fifo
FIFO #(
  .NUM_SLOTS     ( 4          ),
  .LOG_NUM_SLOTS ( 2          ),
  .DATA_WIDTH    ( DATA_WIDTH )
) fifo_in_weight (
  .clk           ( clk                    ),
  .rst           ( rst                    ),
  .data_write    ( weight_data_write_w    ),
  .write         ( weight_write_w         ),
  .full          ( weight_full_w          ),
  .almost_full   ( weight_almost_full_w   ),
  .data_read     ( weight_data_read_fifo  ),
  .next_read     ( weight_next_read_w     ),
  .empty         ( weight_empty_w         )
);

endmodule

