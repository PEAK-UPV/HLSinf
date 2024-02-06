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
    parameter GROUP_SIZE = 4,                                      // group size
    parameter DATA_WIDTH = 8,                                      // data width
    parameter LOG_MAX_READS_PER_ITER = 16,                         // number of bits for max reads per iter
    parameter LOG_MAX_ADDRESS = 16,                                // number of bits for addresses
    localparam OUTPUT_DATA_WIDTH = DATA_WIDTH / 2                  // output data width
)(
  input                              clk,                          // clock input
  input                              rst,                          // reset input

  input                              configure,                    // CONFIGURE interface:: configure signal
  input [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter,           // CONFIGURE interface:: number of reads per iteration
  input [LOG_MAX_ADDRESS-1:0]        base_address,                 // CONFIGURE interface:: address for writes
  input [OUTPUT_DATA_WIDTH-1:0]      min_clip,                     // CONFIGURE interface:: clip minimum value
  input [OUTPUT_DATA_WIDTH-1:0]      max_clip,                     // CONFIGURE interface:: clip maximum value

  input [GROUP_SIZE*DATA_WIDTH-1:0]  data_in,                      // IN interface:: input valid data
  input                              valid_in,                     // IN interface:: input valid signal
  output                             avail_out,                    // IN interface:: avail signal

  output [GROUP_SIZE*OUTPUT_DATA_WIDTH-1:0] data_out,              // OUT interface:: output data
  output [LOG_MAX_ADDRESS-1:0]              address_out,           // OUT interface:: address
  output                                    valid_out              // OUT interface:: valid
);

// wires
wire [GROUP_SIZE*DATA_WIDTH - 1: 0] data_write_w;           // data to write to FIFO
wire                     write_w;                           // write signal to FIFO
wire                     full_w;                            // full signal from FIFO
wire                     almost_full_w;                     // almost_full signal from FIFO
wire [GROUP_SIZE*DATA_WIDTH - 1: 0] data_read_w;                       // data read from FIFO
wire                     next_read_w;                       // next_read signal to FIFO
wire                     empty_w;                           // empty signal from FIFO
// 
wire                     perform_operation_w;               // whether an operation is performed

// registers
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_ADDRESS-1:0]        base_address_r;            // base address to access block ram (up counter)
reg [OUTPUT_DATA_WIDTH-1:0]      min_clip_r;                // min clip value
reg [OUTPUT_DATA_WIDTH-1:0]      max_clip_r;                // max clip value
reg                              module_enabled_r;          // module enabled

// combinational logic
assign avail_out    = ~full_w & ~almost_full_w;
assign perform_operation_w = ~empty_w;                             // perform operation if data available at the input
assign address_out  = base_address_r;                              // address to downstream
assign data_write_w = data_in;                                     // data to FIFO
assign write_w      = valid_in;                                    // write signal to FIFO
assign next_read_w  = perform_operation_w;
assign valid_out    = perform_operation_w;                         // valid signal to downstream module (no avail signal needed as mem is always ready)
//

genvar i;

generate
  for (i=0; i<GROUP_SIZE; i=i+1) begin
    assign data_out[((i+1)*OUTPUT_DATA_WIDTH)-1:i*OUTPUT_DATA_WIDTH] = 
	                                                 data_read_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] > max_clip_r ? max_clip_r : 
                                                         data_read_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] < min_clip_r ? min_clip_r :
                                                         data_read_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH]; // data to downstream module
  end
endgenerate

// modules

// input fifo
FIFO #(
  .NUM_SLOTS     ( 4               ),
  .LOG_NUM_SLOTS ( 2               ),
  .DATA_WIDTH    ( GROUP_SIZE * DATA_WIDTH      )
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
    num_reads_per_iter_r <= 0;
    base_address_r       <= 0;
    min_clip_r           <= 0;
    max_clip_r           <= 0;
    module_enabled_r     <= 1'b0;
  end else begin
    if (configure) begin
      num_reads_per_iter_r      <= num_reads_per_iter;
      base_address_r            <= base_address;
      min_clip_r                <= min_clip;
      max_clip_r                <= max_clip;
      module_enabled_r     <= 1'b1;
    end else begin
      if (num_reads_per_iter_r == 1) begin
        module_enabled_r <= 0;
      end else begin
        if (perform_operation_w) begin
          num_reads_per_iter_r <= num_reads_per_iter_r - 1;
	  base_address_r <= base_address_r + 1;
        end
      end
    end
  end 
end

// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented.
// the module has a tics counter register to keep up with current cycle
//
// in this module whenever data is forwarded (written to memory) the associated information is shown as debug
//

`define DEBUG

`ifdef DEBUG
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (perform_operation_w) $display("WRITE: cycle %d, data_out %x", tics, data_out); 
      tics <= tics + 1;
    end
  end
`endif

endmodule
