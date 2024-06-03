// ACC module
//
// This module accumulates input data on a BRAM. The module runs for a given number of iterations. On every iteration
// the module performs a given number of "read" operations. On each operation, the input data is read and the 
// data is accumulated and stored in a memory. The memory has as many entries as number of "read" operations performed
// in each iteration. On the first iteration the data is stored in the memory (no accumulation). On the last iteration
// the accumulated data is forwarded through the output port.
//

`include "RTLinf.vh"

module ACC_RD_NB #(
    parameter DATA_WIDTH             = 12,                            // input data width (output width = input width)
    parameter OUT_DATA_WIDTH         = 16,
    parameter GROUP_SIZE             = 4,                            // group size
    parameter LOG_GS                 = 2,
    parameter LOG_MAX_ITERS          = 16,                           // number of bits for max iters register
    parameter NUM_ADDRESSES          = 4096,                         // number of addresses
    parameter LOG_MAX_ADDRESS        = 12,                           // number of address bits
    parameter LOG_MAX_READS_PER_ITER = 12,                           // number of bits for max reads per iter
    parameter HIGH_MODE              = "UV", // options: UV, UNZV, NZV
    parameter LOW_MODE               = "UV", // options: UV, UNZV, NZV
    localparam REP_INFO              = GROUP_SIZE + 1,
    localparam INPUT_WIDTH           = DATA_WIDTH + REP_INFO,   // input data width (activation + weight + rep. info)
    localparam OUTPUT_WIDTH          = GROUP_SIZE * OUT_DATA_WIDTH   // output data width ( result (2*data width) +  rep. info)

  )(
    input clk,                                                       // clock signal
    input rst,                                                       // reset signal
  
    input                                   configure,               // CONFIGURE interface:: configure signal
    input [LOG_MAX_ITERS-1:0]               num_iters,               // CONFIGURE interface:: number of iterations for reads
    input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration
  
    input [INPUT_WIDTH - 1 : 0]             data_in_high,            // IN interface:: data
    input                                   valid_in_high,           // IN interface:: valid in
    output                                  avail_out_high,          // IN interface:: avail
  
    input [INPUT_WIDTH - 1 : 0]             data_in_low,             // IN interface:: data
    input                                   valid_in_low,            // IN interface:: valid in
    output                                  avail_out_low,           // IN interface:: avail

    output [OUTPUT_WIDTH - 1 : 0]           data_out,                // OUT interface: data
    output                                  valid_out,               // OUT interface: valid
    input                                   avail_in                 // OUT interface: avail
  );

// wires (operation and iterations)
wire                                  perform_operation_w;               // whether we perform a "read" operation in this cycle
wire                                  first_iteration_w;                 // whether we are in the first iteration
wire                                  last_iteration_w;                  // whether we are in the last iteration

wire                                  is_last;
wire                                  is_last_high;
wire                                  is_last_low;

// wires (data added)
reg is_last_r;

wire [OUT_DATA_WIDTH - 1 : 0]             value_high_shifted;                      // contains the added values from mem and from input
wire [GROUP_SIZE * OUT_DATA_WIDTH-1 : 0]  data_added_w;                      // contains the added values from mem and from input

wire [ GROUP_SIZE - 1: 0]             rep_info_high;                           // Repetition info extracted from FIFO
wire [ GROUP_SIZE - 1: 0]             rep_info_low;                           // Repetition info extracted from FIFO
wire [ DATA_WIDTH - 1: 0]             value_in_high;
wire [ DATA_WIDTH - 1: 0]             value_in_low;


// data pipeline (read -> add -> write stages)
reg                                        read_r;
reg  [LOG_MAX_ADDRESS-1 : 0]               read_addr_r;
reg                                        read_first_iteration_r;
reg                                        read_last_iteration_r;
wire [GROUP_SIZE * OUT_DATA_WIDTH -1 : 0]  read_data_w;
reg [INPUT_WIDTH - 1 : 0]                  read_data_fifo_high_r;
reg [INPUT_WIDTH - 1 : 0]                  read_data_fifo_low_r;

//
reg                                        add_r;
reg [LOG_MAX_ADDRESS-1 : 0]                add_addr_r;
reg [INPUT_WIDTH - 1 : 0]                  add_data_fifo_high_r;
reg [INPUT_WIDTH - 1 : 0]                  add_data_fifo_low_r;

//
wire [OUT_DATA_WIDTH - 1 : 0]              data_joined_w;
wire [OUT_DATA_WIDTH - 1 : 0]              value_in[GROUP_SIZE - 1 : 0];
wire                                       valid_in;
reg                                        add_first_iteration_r;
reg                                        add_last_iteration_r;

//
reg [GROUP_SIZE - 1 : 0]                   write_r;
reg [LOG_MAX_ADDRESS-1 : 0]                write_addr_r;
reg [GROUP_SIZE * OUT_DATA_WIDTH - 1 : 0]  write_data_r;
reg                                        write_last_iteration_r;

//data pipeline
reg [LOG_MAX_ITERS-1:0]              num_iters_r;                        // number of iterations (down counter)
reg [LOG_MAX_ITERS-1:0]              num_iters_copy_r;                   // copy of number of iterations
reg [LOG_MAX_READS_PER_ITER-1:0]     num_reads_per_iter_r;               // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0]     num_reads_per_iter_copy_r;          // copy of number of reads per iteration
reg                                  module_enabled_r;                   // module enabled

reg                                  read_is_last;
reg                                  add_is_last;
reg                                  write_is_last;

genvar i;
integer j;
// combinational logic 

assign valid_in       = valid_in_high || valid_in_low;

// to upstream module (via FIFO)
assign avail_out_high = 1'b1;    // always available
assign avail_out_low  = 1'b1;    // always available

// module and iterations
//TODO: l
//assign perform_operation_w = write_is_last & module_enabled_r & avail_in;   // perform operation when enabled, with input data and output available
assign perform_operation_w = valid_in & module_enabled_r & avail_in;   // perform operation when enabled, with input data and output available

assign first_iteration_w   = num_iters_r == num_iters_copy_r;            // is this first iteration?
assign last_iteration_w    = num_iters_r == 1;                           // is this last iteration?

// to downstream module
assign data_out            = write_data_r;                               // output data
//TODO: l
assign valid_out           =  (|write_r) & is_last_r & write_last_iteration_r;           // valid out to downstream module
//assign valid_out           = write_r & write_last_iteration_r & write_is_last;   // valid out to downstream module


assign is_last        = is_last_high & is_last_low;
assign is_last_high   = data_in_high[INPUT_WIDTH - 1];           //last bit of input indicates if we have received the last element of the group
assign is_last_low    = data_in_low[INPUT_WIDTH - 1];             //last bit of input indicates if we have received the last element of the group



assign value_in_high            = add_data_fifo_high_r[DATA_WIDTH-1:0];
assign rep_info_high            = add_data_fifo_high_r[DATA_WIDTH + GROUP_SIZE - 1 : DATA_WIDTH];
assign value_in_low             = add_data_fifo_low_r[DATA_WIDTH-1:0];
assign rep_info_low             = add_data_fifo_low_r[DATA_WIDTH + GROUP_SIZE - 1 : DATA_WIDTH];

// adders (one per item in the group size)
for (i=0; i<GROUP_SIZE; i=i+1) begin
  assign value_high_shifted = {value_in_high,{4'b0}};

  assign value_in[i] = rep_info_high[i] && rep_info_low[i] ? data_joined_w : (rep_info_high[i] && !rep_info_low[i] ? value_high_shifted : ( !rep_info_high[i] && rep_info_low[i] ? value_in_low : 0));
  assign data_joined_w = value_high_shifted + value_in_low;

  assign data_added_w[((i+1)*OUT_DATA_WIDTH)-1:i*OUT_DATA_WIDTH] = add_first_iteration_r ?  value_in[i] : value_in[i] + read_data_w[((i+1)*OUT_DATA_WIDTH)-1:i*OUT_DATA_WIDTH]; 
end

// modules

// memory (unregistered output)
generate
  for (i=0; i<GROUP_SIZE; i=i+1) begin
    MEM #(
      .DATA_WIDTH      ( OUT_DATA_WIDTH  ),
      .NUM_ADDRESSES   ( NUM_ADDRESSES   ),
      .LOG_MAX_ADDRESS ( LOG_MAX_ADDRESS )
    ) mem (
      .clk             ( clk                                                  ),
      .rst             ( rst                                                  ),
      .data_write      ( write_data_r[i*OUT_DATA_WIDTH +: OUT_DATA_WIDTH] ),
      .addr_write      ( write_addr_r                                         ), 
      .write           ( write_r[i]                                          ), //TODO check
      .addr_read       ( read_addr_r                                          ),
      .data_read       ( read_data_w[i*OUT_DATA_WIDTH +: OUT_DATA_WIDTH]  ),
      .read            ( read_r                                               )
    );
  end
endgenerate 

// sequential logic

// configuration and iterations
// whenever we perform a "read" operation we decrement the number of reads per iteration
// When the reads per iteration reaches zero we decrement number of iterations and restore
// the reads per iteration. If number of iterations reaches zero
// then we disable the module. 
//

always @ (posedge clk) 
begin: control_logic

  is_last_r <= (add_data_fifo_high_r[INPUT_WIDTH - 1] & add_data_fifo_low_r[INPUT_WIDTH - 1]);

  // pipelined operations: READ -> ADD -> WRITE
  read_is_last           <= is_last;
  read_r                 <= perform_operation_w;      // read cycle
  read_addr_r            <= num_reads_per_iter_r;     // address is the current iteration cycle
  read_first_iteration_r <= first_iteration_w;        // first iteration
  read_last_iteration_r  <= last_iteration_w;         // last iteration
  read_data_fifo_high_r  <= data_in_high;              // we capture the input data for the next stage (add)
  read_data_fifo_low_r   <= data_in_low;              // we capture the input data for the next stage (add)

  //
  add_is_last            <= read_is_last;
  add_r                  <= read_r;                   // add cycle (one cycle after read cycle)
  add_addr_r             <= read_addr_r;              // we keep the address for the next stage (write)
  add_first_iteration_r  <= read_first_iteration_r;   // first iteration
  add_last_iteration_r   <= read_last_iteration_r;    // last iteration
  add_data_fifo_high_r   <= read_data_fifo_high_r;         // we keep the data from the fifo to this stage (add)
  add_data_fifo_low_r    <= read_data_fifo_low_r;         // we keep the data from the fifo to this stage (add)
  for(j = 0; j < GROUP_SIZE; j = j + 1) begin
    write_r[j] <= (rep_info_high[j]  || rep_info_low[j]) ? add_r : 1'b0;
  end
  //
  write_is_last          <= add_is_last;
  write_addr_r           <= add_addr_r;               // write address comes from previous stage
  write_data_r           <= data_added_w;             // data to write comes from the logic (data_added_w)
  write_last_iteration_r <= add_last_iteration_r;     // last iteration
  // end
end

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
      
      if (perform_operation_w  & (is_last)) begin
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

`ifdef DEBUG_ACC
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (write_r) $display("ACC: cycle %d writing %x in address %x", tics, write_data_r, write_addr_r);
      if (valid_out) $display("ACC: cycle %d forwarding %x", tics, data_out);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on
  
endmodule