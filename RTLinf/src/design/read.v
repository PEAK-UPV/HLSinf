// Module READ
//
// This module reads data from a blockram and forwards it. A FIFO is implemented to store the 
// data read from the block ram. Two FSM machines are created, one (read_fsm) to read from the block
// ram and write to the FIFO and one (write_fsm) to write the data to the output interface (to the 
// downstream module) and read from the FIFO. 
//
// The module is configured with num_iterations and num_reads_per_iteration. The module runs
// each iteration the cycles needed to perform num_reads_per_iteration reads. The write_fsm machine
// completes all writes even if the module is disabled (the fifo still has some pending data to send)
//

`include "RTLinf.vh"

`define FSM_IDLE  0
`define FSM_READ  1
`define FSM_WRITE 2

module READ #(
    parameter DATA_WIDTH             = 8,                          // data width
    parameter LOG_MAX_ITERS          = 16,                         // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16,                         // number of bits for max reads per iter
    parameter LOG_MAX_ADDRESS        = 16,                         // number of bits for addresses
    parameter TYPE                   = "unspecified"               // module type
)(
  input clk,                                                       // clock input
  input rst,                                                       // reset input

  input configure,                                                 // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]          num_iters,                    // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter,           // CONFIGURE interface:: number of reads per iteration
  input [LOG_MAX_ADDRESS-1:0]        base_address,                 // CONFIGURE interface:: address for reads

  input valid_in,                                                  // IN interface:: input valid signal
  input [DATA_WIDTH-1:0] data_in,                                  // IN interface:: input valid data

  output reg [LOG_MAX_ADDRESS-1:0] address_out,                    // OUT1 interface:: address
  output reg                       request,                        // OUT1 interface:: request signal 

  input  avail_in,                                                 // OUT2 interface:: downstream module available
  output valid_out,                                                // OUT2 interface:: output valid signal
  output [DATA_WIDTH-1:0] data_out                                 // OUT2 interface:: output data
);

// wires
wire [DATA_WIDTH - 1: 0] data_write_w;                      // data to write to FIFO
wire                     write_w;                           // write signal to FIFO
wire                     full_w;                            // full signal from FIFO
wire                     almost_full_w;                     // almost_full signal from FIFO
wire [DATA_WIDTH - 1: 0] data_read_w;                       // data read from FIFO
wire                     next_read_w;                       // next_read signal to FIFO
wire                     empty_w;                           // empty signal from FIFO
//
wire                     perform_operation_w;               // control signal to perform a forward operation
//
wire [LOG_MAX_ADDRESS-1:0] address_out_w;
wire                       request_w;

// registers
reg [LOG_MAX_ITERS-1:0]          num_iters_r;               // number of iterations (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_copy_r; // copy of number of reads per iteration
reg [LOG_MAX_ADDRESS-1:0]        base_address_r;            // base address to access block ram (up counter)
reg [LOG_MAX_ADDRESS-1:0]        base_address_copy_r;       // copy of base address to access block ram 
reg                              module_enabled_r;          // module enabled

// combinational logic
assign address_out_w= base_address_r;                                  // address to the block ream
assign request_w    = ~almost_full_w & ~ full_w & module_enabled_r;    // read request to the block ram
assign data_write_w = data_in;                                         // data to FIFO
assign write_w      = valid_in;                                        // write signal to FIFO
assign next_read_w  = perform_operation_w;                             // next_read signal to FIFO
assign valid_out    = perform_operation_w;                             // valid signal to downstream module
assign data_out     = data_read_w;                                     // data to downstream module
//
assign perform_operation_w = ~empty_w & avail_in;                      // control signal to indicate when to perform a forward operation

// modules

// input fifo
FIFO #(
  .NUM_SLOTS     ( 4               ),
  .LOG_NUM_SLOTS ( 2               ),
  .DATA_WIDTH    ( DATA_WIDTH      )
) fifo_in (
  .clk           ( clk             ),
  .rst           ( rst             ),
  .data_write    ( data_write_w    ),
  .write         ( write_w         ),
  .full          ( full_w          ),
  .almost_full   ( almost_full_w   ),
  .data_read     ( data_read_w     ),
  .next_read     ( next_read_w     ),
  .empty         ( empty_w         )
);

// sequential logic

// configuration and iterations (+ base address)
// whenever we perform a read from the block ram we decrease the number of reads per iteration
// We also increment the base address for the next read.
// When the reads per iteration reaches zero we decrement number of iterations and restore
// both the base address and the reads per iteration. If number of iterations reaches zero
// then we disable the module. Notice forward operations will be still working draining the FIFO
//
always @ (posedge clk) begin
  if (~rst) begin
    num_iters_r          <= 0;
    num_reads_per_iter_r <= 0;
    base_address_r       <= 0;
    module_enabled_r     <= 1'b0;
  end else begin
    if (configure) begin
      num_iters_r          <= num_iters;
      num_reads_per_iter_r <= num_reads_per_iter;
      num_reads_per_iter_copy_r <= num_reads_per_iter;
      base_address_r       <= base_address;
      base_address_copy_r  <= base_address;
      module_enabled_r     <= 1'b1;
    end else begin
      request <= request_w;
      address_out <= address_out_w;
      if (request_w) begin   // when we trigger a read to bram update the counters for iteration control
        if (num_reads_per_iter_r == 1) begin
          if (num_iters_r == 1) begin
            module_enabled_r <= 0; 
          end else begin
            num_iters_r <= num_iters_r - 1;
            num_reads_per_iter_r <= num_reads_per_iter_copy_r;
            base_address_r <= base_address_copy_r;
          end
        end else begin
          num_reads_per_iter_r <= num_reads_per_iter_r - 1;
          base_address_r <= base_address_r + 1;
        end
      end
    end
  end 
end

// synthesis translate_off

// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented
// the module has a tics counter register to keep up with current cycle
//
// in this module whenever a read or write event is performed the associated information is shown as debug
//

`ifdef DEBUG_READ
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (configure) $display("READ (configure): cycle %d base_address %x num_iters %d num_reads_per_iter %d", tics, base_address, num_iters, num_reads_per_iter);
      if (request) $display("READ (type %s, read from bram): cycle %d, address_out %x, num_iters %d num_reads %d module_enabled %d", TYPE, tics, address_out, num_iters_r, num_reads_per_iter_r, module_enabled_r);
      if (valid_out) $display("READ (type %s, forward): cycle %d, data_out %x (full %d almost_full %d empty %d, enabled %d)", TYPE, tics, data_out, full_w, almost_full_w, empty_w, module_enabled_r); 
      if (valid_in) $display("READ (type %s, valid_in) data %x", TYPE, data_in);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on

endmodule
