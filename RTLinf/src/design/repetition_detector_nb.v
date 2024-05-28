// Module repetition_detector
//
// This module implements the required logic to find the unique elements within a group
// of activations.
//

`include "RTLinf.vh"


module repetition_detector_nb#(
    parameter GROUP_SIZE             = 4,                      // group size
    parameter LOG_GS                 = 2,
    parameter ACT_WIDTH           = 4,         
    parameter DATA_WIDTH             = 8,                      // input and output data width
    parameter LOG_MAX_ITERS          = 16,                     // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16,                     // number of bits for max reads per iter
    parameter HIGH_MODE              = "UNZV", // options: UV, UNZV, NZV
    parameter LOW_MODE               = "UNZV", // options: UV, UNZV, NZV
    localparam INPUT_WIDTH           = GROUP_SIZE*DATA_WIDTH,   // number of bits for input (activation + weight + rep. info)
    localparam ZERO_INFO             = GROUP_SIZE,
    localparam REP_INFO_UV_MID       = GROUP_SIZE*GROUP_SIZE,
    localparam REP_INFO_NZV_MID      = ZERO_INFO,
    localparam REP_INFO_UNZV_MID     = REP_INFO_UV_MID + ZERO_INFO,
    localparam REP_INFO_HIGH_MID     = (HIGH_MODE=="UNZV") ? REP_INFO_UNZV_MID :  (HIGH_MODE=="NZV") ? REP_INFO_NZV_MID : REP_INFO_UV_MID,
    localparam REP_INFO_LOW_MID      = (LOW_MODE=="UNZV") ? REP_INFO_UNZV_MID :  (LOW_MODE=="NZV") ? REP_INFO_NZV_MID : REP_INFO_UV_MID,    
    localparam REP_INFO_UV_OUT       = GROUP_SIZE + 1,
    localparam REP_INFO_NZV_OUT      = LOG_GS + ZERO_INFO + 1,
    localparam REP_INFO_UNZV_OUT     = REP_INFO_UV_OUT + ZERO_INFO,
    localparam REP_INFO_HIGH_OUT     = (HIGH_MODE=="UNZV") ? REP_INFO_UNZV_OUT :  (HIGH_MODE=="NZV") ? REP_INFO_NZV_OUT : REP_INFO_UV_OUT,
    localparam REP_INFO_LOW_OUT      = (LOW_MODE=="UNZV") ? REP_INFO_UNZV_OUT :  (LOW_MODE=="NZV") ? REP_INFO_NZV_OUT : REP_INFO_UV_OUT    
)(
  input clk,
  input rst,

  input                               configure,           // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]           num_iters,           // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0]  num_reads_per_iter,  // CONFIGURE interface:: number of reads per iteration  
  
  input                               valid_in,             // ACTIVATION interface:: data
  input [INPUT_WIDTH-1:0]             data_in,              // ACTIVATION interface:: data
  output                              avail_out,            // OUT1 interface:: avail

  output [ACT_WIDTH -1:0]             data_high_out,        // ACTIVATION interface:: data
  output [REP_INFO_HIGH_OUT-1:0]      rdata_high_out,       // OUT1 interface:: rdata
  output                              valid_high_out,  
  input                               avail_in_high,        // OUT1 interface:: avail

  output [ACT_WIDTH -1:0]             data_low_out,         // ACTIVATION interface:: data
  output [REP_INFO_LOW_OUT-1:0]       rdata_low_out,        // OUT1 interface:: rdata
  output                              valid_low_out,
  input                               avail_in_low          // OUT1 interface:: avail
);


// wires
wire[ACT_WIDTH * GROUP_SIZE - 1 : 0]    data_in_high;
wire[ACT_WIDTH * GROUP_SIZE - 1 : 0]    data_in_low;

wire[REP_INFO_HIGH_MID - 1 : 0] rdata_eq_dis_high;
wire[REP_INFO_LOW_MID - 1 : 0]  rdata_eq_dis_low;

wire                             next_read_low;   // indicates if low group has been processed
wire                             next_read_high;  // indicates if high group has been processed

//FIFO
wire [INPUT_WIDTH - 1: 0]        data_write_w;                      // data to write to FIFO
wire                             write_enb_w;                       // write signal to FIFO
wire                             full_w;                            // full signal from FIFO
wire                             almost_full_w;                     // almost_full signal from FIFO
wire [INPUT_WIDTH - 1 : 0]       data_read_fifo;                    // data read from FIFO
wire                             read_enb_w;                        // next_read signal to FIFO
wire                             empty_w;                           // empty signal from FIFO
wire                             perform_operation_w;

// registers
reg [LOG_MAX_ITERS - 1 : 0]            num_iters_r;               // FIFO
reg [LOG_MAX_READS_PER_ITER - 1 : 0]   num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER - 1 : 0]   num_reads_per_iter_copy_r; // copy of number of reads per iteration
reg                                    module_enabled_r;          // module enabled
genvar i;

// combinational logic
assign data_write_w  = data_in;                                                 // data to FIFO
assign write_enb_w   = valid_in & ~full_w;                                      // write signal to FIFO
assign avail_out     = ~almost_full_w & ~full_w;                                // avail signal from FIFO       
assign read_enb_w    = perform_operation_w & (next_read_low & next_read_high);  // next_read signal to FIFO
assign valid_out     = perform_operation_w & (valid_high_out | valid_low_out);  // valid signal to downstream module
//
assign perform_operation_w = module_enabled_r & ~empty_w & avail_in_low & avail_in_high;

assign next_read_high = rdata_high_out[REP_INFO_HIGH_OUT-1];
assign next_read_low = rdata_low_out[REP_INFO_LOW_OUT-1];

//Split data in high and low bits
for(i = 0; i < GROUP_SIZE; i = i + 1) begin
  assign data_in_high[i*ACT_WIDTH +: ACT_WIDTH] = data_read_fifo[(i * DATA_WIDTH) + ACT_WIDTH +: ACT_WIDTH];
  assign data_in_low[i*ACT_WIDTH +: ACT_WIDTH]  = data_read_fifo[i * DATA_WIDTH +: ACT_WIDTH];
end

// extract repetition vectors
repetition_eq #(
  .GROUP_SIZE             ( GROUP_SIZE           ),
  .DATA_WIDTH             ( ACT_WIDTH            ),
  .HIGH_MODE              ( HIGH_MODE            ),
  .LOW_MODE               ( LOW_MODE             )
) repetition_eq_m (
  .data_in_high           ( data_in_high         ),
  .data_in_low            ( data_in_low          ),
  .rdata_high_out         ( rdata_eq_dis_high    ),
  .rdata_low_out          ( rdata_eq_dis_low     )
);

//HIGH
DISPATCHER #(
  .GROUP_SIZE             ( GROUP_SIZE            ),
  .LOG_GS                 ( LOG_GS                ),
  .DATA_WIDTH             ( ACT_WIDTH             ),
  .MODE                   ( HIGH_MODE             )
) dispatcher_high_m (
  .clk                    ( clk                   ),
  .rst                    ( rst                   ),
  .perform_operation      ( perform_operation_w   ),
  .new_group              ( read_enb_w            ),
  .data_in                ( data_in_high          ),
  .rdata_in               ( rdata_eq_dis_high     ),
  .data_out               ( data_high_out         ),
  .rdata_out              ( rdata_high_out        ),
  .valid_out              ( valid_high_out        )
);

//LOW
DISPATCHER #(
  .GROUP_SIZE             ( GROUP_SIZE            ),
  .LOG_GS                 ( LOG_GS                ),
  .DATA_WIDTH             ( ACT_WIDTH             ),
  .MODE                   ( LOW_MODE             )
) dispatcher_low_m (
  .clk                    ( clk                   ),
  .rst                    ( rst                   ),  
  .perform_operation      ( perform_operation_w   ),
  .new_group              ( read_enb_w            ),
  .data_in                ( data_in_low           ),
  .rdata_in               ( rdata_eq_dis_low      ),
  .data_out               ( data_low_out          ),
  .rdata_out              ( rdata_low_out         ), 
  .valid_out              ( valid_low_out         )
);


// sequential logic
always @ (posedge clk) 
begin: ITEARATION_CONTROL
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
      if (perform_operation_w & read_enb_w) begin   // when we trigger a read to bram update the counters for iteration control
        if (num_reads_per_iter_r == 1) begin
          if (num_iters_r == 1) begin
            module_enabled_r <= 0; 
          end else begin
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

// FIFO
FIFO #(
  .NUM_SLOTS     ( 4               ),
  .LOG_NUM_SLOTS ( 2               ),
  .DATA_WIDTH    ( INPUT_WIDTH     )
) fifo_in (
  .clk           ( clk             ),
  .rst           ( rst             ),
  .data_write    ( data_write_w    ),
  .write         ( write_enb_w     ),
  .full          ( full_w          ),
  .almost_full   ( almost_full_w   ),
  .data_read     ( data_read_fifo  ),
  .next_read     ( read_enb_w      ),
  .empty         ( empty_w         )
);



endmodule