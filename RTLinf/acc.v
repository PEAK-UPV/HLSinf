// ACC module
//
// This module accumulates input data on a BRAM. The module runs for a given number of iterations. On every iteration
// the module performs a given number of "read" operations. On each operation, the input data is read and the 
// data is accumulated and stored in a memory. The memory has as many entries as number of "read" operations performed
// in each iteration. On the first iteration the data is stored in the memory (no accumulation). On the last iteration
// the accumulated data is forwarded through the output port.
//

module ACC #(
    parameter NUM_INPUTS             = 8,                            // number of inputs (outpus == inputs)
    parameter DATA_WIDTH             = 8,                            // input data width (output width = input width)
    parameter GROUP_SIZE             = 4,                            // group size
    parameter LOG_MAX_ITERS          = 16,                           // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16                            // number of bits for max reads per iter
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
// wires (mem)
wire [GROUP_SIZE * DATA_WIDTH-1 : 0]  mem_data_write_w;                  // data to write to memory
wire [LOG_MAX_READS_PER_ITER-1 : 0]   mem_addr_write_w;                  // write address to memory
wire                                  mem_write_w;                       // write signal
wire [LOG_MAX_READS_PER_ITER-1 : 0]   mem_addr_read_w;                   // read address to memory
wire [GROUP_SIZE * DATA_WIDTH-1 : 0]  mem_data_read_w;                   // data read from memory

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
// to upstream module
assign avail_out           = ~almost_full_w & ~full_w;                   // avail out to upstream module
// module and iterations
assign perform_operation_w = module_enabled_r & (~empty_w) & avail_in;   // perform operation when enabled, with input data and output available
assign first_iteration_w   = num_iters_r == num_iters_copy_r;            // is this first iteration?
assign last_iteration_w    = num_iters_r == 1;                           // is this last iteration?
// to downstream module
assign data_out            = data_added_w;                               // output data
assign valid_out           = perform_operation_w & last_iteration_w;     // valid out to downstream module
// memory
assign mem_data_write_w    = data_added_w;             // data to write is the output of the adders
assign mem_addr_write_w    = num_reads_per_iter_w;     // address to write is the current iteration (no problem if we have decreassing addresses)
assign mem_write_w         = perform_operation_w;      // write whenever we perform the operation
assign mem_addr_read_w     = num_reads_per_iter_w;     // address to read is the current iteration (no problem if we have decreassing addresses)
// adders (one per item in the group size)
generate
  for (i=0; i<GROUP_SIZE; i++) begin
    assign data_added_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] = 
                first_iteration_w ? 0 : data_read_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] + 
                mem_data_read_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH];
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
MEMU #(
  .DATA_WIDTH      ( GROUP_SIZE * DATA_WIDTH  ),
  .NUM_ADDRESSES   ( MAX_READS_PER_ITER       ),
  .LOG_MAX_ADDRESS ( LOG_MAX_READS_PER_ITER   )
) mem (
  .clk             ( clk                      ),
  .rst             ( rst                      ),
  .data_write      ( mem_data_write_w         ),
  .addr_write      ( mem_addr_write_w         ),
  .write           ( mem_write_w              ),
  .addr_read       ( mem_addr_read_w          ),
  .data_read       ( mem_data_read_w          )
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
      if (perform_operation_w) $display("ACC: cycle %d", tics);
      tics <= tics + 1;
    end
  end
`endif
  
endmodule
  
  
