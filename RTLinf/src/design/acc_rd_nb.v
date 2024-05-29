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
    parameter DATA_WIDTH             = 4,                            // input data width (output width = input width)
    parameter OUT_DATA_WIDTH         = 8,
    parameter GROUP_SIZE             = 4,                            // group size
    parameter LOG_GS                 = 2,
    parameter LOG_MAX_ITERS          = 16,                           // number of bits for max iters register
    parameter NUM_ADDRESSES          = 4096,                         // number of addresses
    parameter LOG_MAX_ADDRESS        = 12,                           // number of address bits
    parameter LOG_MAX_READS_PER_ITER = 12,                           // number of bits for max reads per iter
    parameter HIGH_MODE              = "UV", // options: UV, UNZV, NZV
    parameter LOW_MODE               = "UV", // options: UV, UNZV, NZV
    localparam ZERO_INFO             = GROUP_SIZE,
    localparam REP_INFO_UV           = GROUP_SIZE + 1,
    localparam REP_INFO_NZV          = LOG_GS + ZERO_INFO + 1,
    localparam REP_INFO_UNZV         = REP_INFO_UV + ZERO_INFO,
    localparam REP_INFO_HIGH         = (HIGH_MODE=="UNZV") ? REP_INFO_UNZV :  (HIGH_MODE=="NZV") ? REP_INFO_NZV : REP_INFO_UV,
    localparam REP_INFO_LOW          = (LOW_MODE=="UNZV") ? REP_INFO_UNZV :  (LOW_MODE=="NZV") ? REP_INFO_NZV : REP_INFO_UV,
    localparam INPUT_WIDTH_HIGH      = DATA_WIDTH + REP_INFO_HIGH,   // input data width (activation + weight + rep. info)
    localparam INPUT_WIDTH_LOW       = DATA_WIDTH + REP_INFO_LOW,    // input data width (activation + weight + rep. info)
    localparam OUTPUT_WIDTH          = GROUP_SIZE * OUT_DATA_WIDTH   // output data width ( result (2*data width) +  rep. info)

  )(
    input clk,                                                       // clock signal
    input rst,                                                       // reset signal
  
    input                                   configure,               // CONFIGURE interface:: configure signal
    input [LOG_MAX_ITERS-1:0]               num_iters,               // CONFIGURE interface:: number of iterations for reads
    input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration
  
    input [INPUT_WIDTH_HIGH - 1 : 0]        data_in_high,            // IN interface:: data
    input                                   valid_in_high,           // IN interface:: valid in
    output                                  avail_out_high,          // IN interface:: avail
  
    input [INPUT_WIDTH_LOW - 1 : 0]         data_in_low,             // IN interface:: data
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
wire [OUT_DATA_WIDTH - 1 : 0]               data_shifted_w_high[GROUP_SIZE - 1 : 0];                      // contains the added values from mem and from input
wire [GROUP_SIZE * OUT_DATA_WIDTH-1 : 0]  data_added_w;                      // contains the added values from mem and from input

// data pipeline (read -> add -> write stages)
reg                                        read_r;
reg  [LOG_MAX_ADDRESS-1 : 0]               read_addr_r;
reg                                        read_first_iteration_r;
reg                                        read_last_iteration_r;
wire [GROUP_SIZE * OUT_DATA_WIDTH -1 : 0]  read_data_w;

//
reg                                        add_r;
reg [LOG_MAX_ADDRESS-1 : 0]                add_addr_r;

//
wire [DATA_WIDTH * GROUP_SIZE - 1 : 0]     data_to_join_high_w;
wire [DATA_WIDTH * GROUP_SIZE - 1 : 0]     data_to_join_low_w;
wire [OUT_DATA_WIDTH - 1 : 0]              data_joined_w[GROUP_SIZE - 1 : 0];
wire valid_in;
reg                                        add_first_iteration_r;
reg                                        add_last_iteration_r;

//
reg                                        write_r;
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

reg                                  read_valid_in;
reg                                  add_valid_in;
reg                                  write_valid_in;
genvar i;

// combinational logic 

assign valid_in       = valid_in_high || valid_in_low;
assign processing     = valid_in || read_valid_in || add_valid_in || write_valid_in;

// to upstream module (via FIFO)
assign avail_out_high = 1'b1;    // always available
assign avail_out_low  = 1'b1;    // always available

// module and iterations
assign perform_operation_w = is_last & module_enabled_r & avail_in;   // perform operation when enabled, with input data and output available

assign first_iteration_w   = num_iters_r == num_iters_copy_r;            // is this first iteration?
assign last_iteration_w    = num_iters_r == 1;                           // is this last iteration?

// to downstream module
assign data_out            = write_data_r;                                       // output data
assign valid_out           = write_r & write_last_iteration_r & write_is_last;   // valid out to downstream module

assign is_last        = is_last_high & is_last_low;
assign is_last_high   = data_in_high[INPUT_WIDTH_HIGH - 1];           //last bit of input indicates if we have received the last element of the group
assign is_last_low    = data_in_low[INPUT_WIDTH_LOW - 1];             //last bit of input indicates if we have received the last element of the group

// adders (one per item in the group size)
for (i=0; i<GROUP_SIZE; i=i+1) begin
  assign data_shifted_w_high[i] = {data_to_join_high_w[i * DATA_WIDTH +: DATA_WIDTH],{4'b0}};

  assign data_joined_w[i] = data_shifted_w_high[i] + data_to_join_low_w[i * DATA_WIDTH +: DATA_WIDTH];

  assign data_added_w[((i+1)*OUT_DATA_WIDTH)-1:i*OUT_DATA_WIDTH] = add_first_iteration_r ?  data_joined_w[i] : data_joined_w[i] + read_data_w[((i+1)*OUT_DATA_WIDTH)-1:i*OUT_DATA_WIDTH]; 
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
      .write           ( write_is_last                                            ),
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
  // pipelined operations: READ -> ADD -> WRITE
  read_valid_in          <= valid_in;
  read_is_last           <= is_last;
  read_r                 <= perform_operation_w;      // read cycle
  read_addr_r            <= num_reads_per_iter_r;     // address is the current iteration cycle
  read_first_iteration_r <= first_iteration_w;        // first iteration
  read_last_iteration_r  <= last_iteration_w;         // last iteration
  //
  add_is_last            <= read_is_last;
  add_valid_in           <= read_valid_in;
  add_r                  <= read_r;                   // add cycle (one cycle after read cycle)
  add_addr_r             <= read_addr_r;              // we keep the address for the next stage (write)
  add_first_iteration_r  <= read_first_iteration_r;   // first iteration
  add_last_iteration_r   <= read_last_iteration_r;    // last iteration
  //
  write_is_last          <= add_is_last;
  write_valid_in         <= add_valid_in;
  write_r                <= add_r;                    // write cycle (one cycle after add cycle)
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

/* Modules */
if (HIGH_MODE == "UNZV") begin
  // ACC_HIGH
  ACC_RD_NB_UNZV #(
      .DATA_WIDTH   ( DATA_WIDTH  ),
      .GROUP_SIZE   ( GROUP_SIZE  )
  ) acc_rd_nb_unzv_high_m (
    .clk           ( clk                 ),
    .rst           ( rst                 ),
    .data_in       ( data_in_high[0 +: INPUT_WIDTH_HIGH - 1]        ),
    .data_out      ( data_to_join_high_w )
  );
end else begin
// ACC_LOW
  ACC_RD_NB_NZV #(
      .DATA_WIDTH   ( DATA_WIDTH  ),
      .LOG_GS       ( LOG_GS      ),
      .GROUP_SIZE   ( GROUP_SIZE  )
  ) acc_rd_nb_nz_high_m (
    .clk           ( clk                 ),
    .rst           ( rst                 ),
    .data_in       ( data_in_high[0 +: INPUT_WIDTH_HIGH - 1] ),
    .data_out      ( data_to_join_high_w )
  );
end

if (LOW_MODE == "UNZV") begin
  // ACC_HIGH
  ACC_RD_NB_UNZV #(
      .DATA_WIDTH   ( DATA_WIDTH  ),
      .GROUP_SIZE   ( GROUP_SIZE  )
  ) acc_rd_nb_unzv_low_m (
    .clk           ( clk                ),
    .rst           ( rst                ),
    .data_in       ( data_in_low[0 +: INPUT_WIDTH_LOW - 1]        ),
    .data_out      ( data_to_join_low_w )
  );
end else begin
// ACC_LOW
  ACC_RD_NB_NZV #(
      .DATA_WIDTH   ( DATA_WIDTH  ),
      .LOG_GS       ( LOG_GS      ),
      .GROUP_SIZE   ( GROUP_SIZE  )
  ) acc_rd_nb_nz_low_m (
    .clk           ( clk                ),
    .rst           ( rst                ),
    .data_in       ( data_in_low[0 +: INPUT_WIDTH_LOW - 1]),
    .data_out      ( data_to_join_low_w )
  );
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
  
  