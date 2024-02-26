// ACC module
//
// This module accumulates input data on a BRAM. The module runs for a given number of iterations. On every iteration
// the module performs a given number of "read" operations. On each operation, the input data is read and the 
// data is accumulated and stored in a memory. The memory has as many entries as number of "read" operations performed
// in each iteration. On the first iteration the data is stored in the memory (no accumulation). On the last iteration
// the accumulated data is forwarded through the output port.
//

`include "RTLinf.vh"

module ACC #(
    parameter DATA_WIDTH             = 8,                            // input data width (output width = input width)
    parameter GROUP_SIZE             = 4,                            // group size
    parameter LOG_MAX_ITERS          = 16,                           // number of bits for max iters register
    parameter NUM_ADDRESSES          = 4096,                         // number of addresses
    parameter LOG_MAX_ADDRESS        = 12,                           // number of address bits
    parameter LOG_MAX_READS_PER_ITER = 12                            // number of bits for max reads per iter
  )(
    input clk,                                                       // clock signal
    input rst,                                                       // reset signal
  
    input                                   configure,               // CONFIGURE interface:: configure signal
    input [LOG_MAX_ITERS-1:0]               num_iters,               // CONFIGURE interface:: number of iterations for reads
    input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration
  
    input [GROUP_SIZE * DATA_WIDTH - 1 : 0] data_in,                 // IN interface:: data
    input                                   valid_in,                // IN interface:: valid in
    output                                  avail_out,               // IN interface:: avail
  
    output [GROUP_SIZE * DATA_WIDTH-1 : 0]  data_out,                // OUT interface: data
    output                                  valid_out,               // OUT interface: valid
    input                                   avail_in                 // OUT interface: avail
  );
  
// wires (fifo)
wire [GROUP_SIZE * DATA_WIDTH - 1: 0] data_write_w;                      // data to write to FIFO
wire                                  write_w;                           // write signal to FIFO
wire                                  full_w;                            // full signal from FIFO
wire                                  almost_full_w;                     // almost_full signal from FIFO
wire [GROUP_SIZE * DATA_WIDTH - 1: 0] data_read_w;                       // data read from FIFO
wire                                  next_read_w;                       // next_read signal to FIFO
wire                                  empty_w;                           // empty signal from FIFO
// wires (operation and iterations)
wire                                  perform_operation_w;               // whether we perform a "read" operation in this cycle
wire                                  first_iteration_w;                 // whether we are in the first iteration
wire                                  last_iteration_w;                  // whether we are in the last iteration

// pipeline (read -> add -> write stages)
reg                                  read_r;
reg  [LOG_MAX_ADDRESS-1 : 0]         read_addr_r;
wire [GROUP_SIZE * DATA_WIDTH-1 : 0] read_data_w;
reg  [GROUP_SIZE * DATA_WIDTH-1 : 0] read_data_fifo_r;
reg                                  read_first_iteration_r;
reg                                  read_last_iteration_r;
//
reg                                  add_r;
reg [LOG_MAX_ADDRESS-1 : 0]          add_addr_r;
reg [GROUP_SIZE * DATA_WIDTH-1 : 0]  add_data_fifo_r;
reg                                  add_first_iteration_r;
reg                                  add_last_iteration_r;

reg                                  write_r;
reg [LOG_MAX_ADDRESS-1 : 0]          write_addr_r;
reg [GROUP_SIZE * DATA_WIDTH-1 : 0]  write_data_r;
reg                                  write_last_iteration_r;
//

// wires (data added)
wire [GROUP_SIZE * DATA_WIDTH-1 : 0]  data_added_w;                      // contains the added values from mem and from input

// registers
reg [LOG_MAX_ITERS-1:0]          num_iters_r;                            // number of iterations (down counter)
reg [LOG_MAX_ITERS-1:0]          num_iters_copy_r;                       // copy of number of iterations
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;                   // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_copy_r;              // copy of number of reads per iteration
reg                              module_enabled_r;                       // module enabled

genvar i;

// combinational logic 
// to upstream module (via FIFO)
assign data_write_w        = data_in;
assign write_w             = valid_in;
assign avail_out           = ~almost_full_w & ~full_w;                   // avail out to upstream module
assign next_read_w         = perform_operation_w;
// module and iterations
assign perform_operation_w = module_enabled_r & (~empty_w) & avail_in;   // perform operation when enabled, with input data and output available
assign first_iteration_w   = num_iters_r == num_iters_copy_r;            // is this first iteration?
assign last_iteration_w    = num_iters_r == 1;                           // is this last iteration?
// to downstream module
assign data_out            = write_data_r;                               // output data
assign valid_out           = write_r & write_last_iteration_r;           // valid out to downstream module
// memory

// adders (one per item in the group size)
generate
  for (i=0; i<GROUP_SIZE; i=i+1) begin
    assign data_added_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] = add_first_iteration_r ?  add_data_fifo_r[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] : add_data_fifo_r[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] + read_data_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH];
  end
endgenerate

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

// memory (unregistered output)
MEM #(
  .DATA_WIDTH      ( GROUP_SIZE * DATA_WIDTH  ),
  .NUM_ADDRESSES   ( NUM_ADDRESSES            ),
  .LOG_MAX_ADDRESS ( LOG_MAX_ADDRESS          )
) mem (
  .clk             ( clk                      ),
  .rst             ( rst                      ),
  .data_write      ( write_data_r             ),
  .addr_write      ( write_addr_r             ),
  .write           ( write_r                  ),
  .addr_read       ( read_addr_r              ),
  .data_read       ( read_data_w              ),
  .read            ( read_r                   )
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
      num_iters_r               <= num_iters;
      num_iters_copy_r          <= num_iters;
      num_reads_per_iter_r      <= num_reads_per_iter;
      num_reads_per_iter_copy_r <= num_reads_per_iter;
      module_enabled_r          <= 1'b1;
      read_data_fifo_r          <= 0;
    end else begin
      // pipelined operations: READ -> ADD -> WRITE
      read_r                 <= perform_operation_w;      // read cycle
      read_addr_r            <= num_reads_per_iter_r;     // address is the current iteration cycle
      read_data_fifo_r       <= data_read_w;              // we capture the input data for the next stage (add)
      read_first_iteration_r <= first_iteration_w;        // first iteration
      read_last_iteration_r  <= last_iteration_w;         // last iteration
      //
      add_r                  <= read_r;                   // add cycle (one cycle after read cycle)
      add_addr_r             <= read_addr_r;              // we keep the address for the next stage (write)
      add_data_fifo_r        <= read_data_fifo_r;         // we keep the data from the fifo to this stage (add)
      add_first_iteration_r  <= read_first_iteration_r;   // first iteration
      add_last_iteration_r   <= read_last_iteration_r;    // last iteration
      //
      write_r      <= add_r;                              // write cycle (one cycle after add cycle)
      write_addr_r <= add_addr_r;                         // write address comes from previous stage
      write_data_r <= data_added_w;                       // data to write comes from the logic (data_added_w)
      write_last_iteration_r <= add_last_iteration_r;     // last iteration
      
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

//`ifdef DEBUG_ACC
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (write_r) $display("ACC: cycle %d writing %x in address %x", tics, write_data_r, write_addr_r);
      if (valid_out) $display("ACC: cycle %d forwarding %x", tics, data_out);
      tics <= tics + 1;
    end
  end
//`endif

// synthesis translate_on
  
endmodule
  
  
