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

`define FSM_IDLE  0
`define FSM_READ  1
`define FSM_WRITE 2

module READ #(
    parameter DATA_WIDTH = 8,                                      // data width
    parameter LOG_MAX_ITERS = 16,                                  // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16,                         // number of bits for max reads per iter
    parameter LOG_MAX_ADDRESS = 16                                 // number of bits for addresses
)(
  input clk,                                                       // clock input
  input rst,                                                       // reset input

  input configure,                                                 // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]          num_iters,                    // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter,           // CONFIGURE interface:: number of reads per iteration
  input [LOG_MAX_ADDRESS-1:0]        base_address,                 // CONFIGURE interface:: address for reads

  input valid_in,                                                  // IN interface:: input valid signal
  input [DATA_WIDTH-1:0] data_in,                                  // IN interface:: input valid data

  output [LOG_MAX_ADDRESS-1:0] address_out,                        // OUT1 interface:: address
  output request,                                                  // OUT1 interface:: request signal 

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

// registers
reg [LOG_MAX_ITERS-1:0]          num_iters_r;               // number of iterations (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_copy_r; // copy of number of reads per iteration
reg [LOG_MAX_ADDRESS-1:0]        base_address_r;            // base address to access block ram (up counter)
reg [LOG_MAX_ADDRESS-1:0]        base_address_copy_r;       // copy of base address to access block ram 
reg                              module_enabled_r;          // module enabled
reg [1:0]                        read_fsm_state;            // FSM state for read from BRAM
reg [1:0]                        read_fsm_next_state;       // FSM next state for read from BRAM
reg [1:0]                        write_fsm_state;           // FSM state for write to downstream
reg [1:0]                        write_fsm_next_state;      // FSM state for write to downstream

// combinational logic
assign address_out  = base_address_r;                                      // address to the block ream
assign request      = module_enabled_r & (read_fsm_state == `FSM_READ);    // read request to the block ram
assign data_write_w = data_in;                                             // data to FIFO
assign write_w      = valid_in;                                            // write signal to FIFO
assign next_read_w  = avail_in & ~empty_w & (write_fsm_state == `FSM_WRITE);          // next_read signal to FIFO
assign valid_out    = avail_in & ~empty_w & (write_fsm_state == `FSM_WRITE);          // valid signal to downstream module
assign data_out     = data_read_w;                                         // data to downstream module

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
// then we disable the module. Notice the write FSM state will be still working draining the FIFO
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
      if (num_reads_per_iter_r == 1) begin
        if (num_iters_r == 1) module_enabled_r <= 0;
        else begin
          num_iters_r <= num_iters_r - 1;
          num_reads_per_iter_r <= num_reads_per_iter_copy_r;
          base_address_r <= base_address_copy_r;
        end
      end else begin
        if (read_fsm_state == `FSM_READ) begin
          num_reads_per_iter_r <= num_reads_per_iter_r - 1;
          base_address_r <= base_address_r + 1;
        end
      end
    end
  end 
end

// write to the fifo: FSM with two states (IDLE, READ)
// IDLE when FIFO is almost full or module is not enabled, READ otherwise
//
//  --------  enabled & !almost_full  --------
//  |      | ------------------------>|      |
//  | IDLE |                          | READ |
//  |      | <------------------------|      |
//  --------  almost_full | !enabled  --------
//
// in read state a read request is sent to the BRAM
// the write logic to the FIFO depends on the availability of data
// at the input interface
//
always @ (posedge clk) begin
  if (~rst) begin
    read_fsm_state <= `FSM_IDLE;
    read_fsm_next_state <= `FSM_IDLE;
  end else begin
    read_fsm_state <= read_fsm_next_state;
    if (read_fsm_state == `FSM_IDLE) begin
      if (module_enabled_r & ~almost_full_w) read_fsm_next_state <= `FSM_READ;
    end else if (read_fsm_state == `FSM_READ) begin
      if (almost_full_w | ~module_enabled_r) read_fsm_next_state <= `FSM_IDLE;
    end    
  end
end

// read from the fifo: FSM with two states (IDLE, WRITE)
// IDLE when downstream module is not available or module is not enabled and FIFO is empty, READ otherwise
//
//  --------         avail_downstream  & !fifo_empty          ---------
//  |      | ------------------------------------------------>|       |
//  | IDLE |                                                  | WRITE |
//  |      | <------------------------------------------------|       |
//  --------            !avail_downstream | fifo_empty        ---------
//
// in write state data read from the fifo is assigned to the OUT2 interface
// and next_read signal is triggered
always @ (posedge clk) begin
  if (~rst) begin
    write_fsm_state      <= `FSM_IDLE;
    write_fsm_next_state <= `FSM_IDLE;
  end else begin
    write_fsm_state <= write_fsm_next_state;
    if (write_fsm_state == `FSM_IDLE) begin
      if (avail_in & ~empty_w) write_fsm_next_state <= `FSM_WRITE;
    end else if (write_fsm_state == `FSM_WRITE) begin
      if (~avail_in | empty_w) write_fsm_next_state <= `FSM_IDLE;
    end    
  end
end

// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented
// the module has a tics counter register to keep up with current cycle
//
// in this module whenever a read or write event is performed the associated information is shown as debug
//

`define DEBUG

`ifdef DEBUG
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (configure) $display("READ (configure): cycle %d", tics);
      if (request) $display("READ (read to bram): cycle %d, address_out %x, num_iters %d num_reads %d module_enabled %d", tics, address_out, num_iters_r, num_reads_per_iter_r, module_enabled_r);
      if (valid_out) $display("WRITE (forward): cycle %d, data_out %x (full %d almost_full %d empty %d)", tics, data_out, full_w, almost_full_w, empty_w); 
      tics <= tics + 1;
    end
  end
`endif

endmodule
