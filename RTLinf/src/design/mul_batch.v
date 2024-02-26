`timescale 1ns / 1ps
// MUL module
//
// TODO
// 

//`include "RTLinf.vh"

module MUL_BATCH #(
  parameter GROUP_SIZE             = 4,                                       // group size
  parameter DATA_WIDTH             = 8,                                       // input data width (output is 2x input width)
  parameter LOG_MAX_ITERS          = 16,                                      // number of bits for max iters register
  parameter LOG_MAX_READS_PER_ITER = 16,                                      // number of bits for max reads per iter
  localparam REP_INFO              = GROUP_SIZE * GROUP_SIZE,                 // number of bits for repetition detector
  localparam INPUT                 = GROUP_SIZE * DATA_WIDTH + REP_INFO,      // MUL_BATCH input number of bits
  localparam DISPATCH_OUTPUT       = 2 * DATA_WIDTH + REP_INFO,               // DISPATCHER  module output number of bits
  localparam MUL_OUTPUT            = 2 * DATA_WIDTH + REP_INFO,               // MUL module output number of bits
  localparam OUTPUT                = GROUP_SIZE * 2 * DATA_WIDTH              // MUL_BATCH output number of bits
)(
  input clk,
  input rst,

  input                                    configure,               // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS - 1 : 0]            num_iters,               // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER - 1 : 0]   num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration

  input [INPUT - 1 : 0]                    act_data_in,             // ACTIVATION interface:: activations data
  input                                    act_valid_in,            // ACTIVATION interface:: activation valid in
  output                                   act_avail_out,           // ACTIVATION interface:: avail

  input [DATA_WIDTH - 1 : 0]               weight_data_in,          // WEIGHT interface:: weight data
  input                                    weight_valid_in,         // WEIGHT interface:: weight valid in
  output                                   weight_avail_out,        // WEIGHT interface:: avail

  output [OUTPUT - 1 : 0]                  data_out,                // OUT interface: data
  output                                   valid_out,               // OUT interface: valid
  input                                    avail_in                 // OUT interface: avail
);


// wires between DISPATCHER  and MUL modules
wire                          dis2mul_valid_w;
wire [DISPATCH_OUTPUT - 1 : 0] dis2mul_data_w;
wire                          dis2mul_avail_w;

// wires between MUL and AGRUPATE modules
wire                          mul2agr_valid_w;
wire [MUL_OUTPUT - 1 : 0]     mul2agr_data_w;
wire                          mul2agr_avail_w;

DISPATCHER  #(
  .GROUP_SIZE             ( GROUP_SIZE ),
  .DATA_WIDTH             ( DATA_WIDTH ),
  .LOG_MAX_ITERS          ( LOG_MAX_ITERS ),
  .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
) dispatch_m (
  .clk                    ( clk                  ),  //From top
  .rst                    ( rst                  ),  //From top
  .configure              ( configure            ),  //From top
  .num_iters              ( num_iters            ),  //From top
  .num_reads_per_iter     ( num_reads_per_iter   ),  //From top
  .act_data_in            ( act_data_in          ),  //From top
  .act_valid_in           ( act_valid_in         ),  //From top 
  .act_avail_out          ( act_avail_out        ),  //From top 
  .weight_data_in         ( weight_data_in       ),  //From top 
  .weight_valid_in        ( weight_valid_in      ),  //From top 
  .weight_avail_out       ( weight_avail_out     ),  //To top 
  .data_out               ( dis2mul_data_w       ),  //DISPATCHER  to MUL 
  .valid_out              ( dis2mul_valid_w      ),  //DISPATCHER  to MUL 
  .avail_in               ( dis2mul_avail_w      )   //MUL to DISPATCHER  
);

MUL #(
  .GROUP_SIZE             ( GROUP_SIZE ),
  .DATA_WIDTH             ( DATA_WIDTH ),
  .LOG_MAX_ITERS          ( LOG_MAX_ITERS ),
  .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
) mul_m (
  .clk                    ( clk                  ), //From top
  .rst                    ( rst                  ), //From top
  .configure              ( configure            ), //From top
  .num_iters              ( num_iters            ), //From top
  .num_reads_per_iter     ( num_reads_per_iter   ), //From top
  .data_in                ( dis2mul_data_w       ), //DISPATCHER  to MUL
  .valid_in               ( dis2mul_valid_w      ), //DISPATCHER  to MUL  
  .avail_out              ( dis2mul_avail_w      ), //DISPATCHER  to MUL 
  .data_out               ( mul2agr_data_w       ), //MUL to AGRUPATE 
  .valid_out              ( mul2agr_valid_w      ), //MUL to AGRUPATE
  .avail_in               ( mul2agr_avail_w      )  //AGRUPATE to MUL
);

AGRUPATE #(
  .GROUP_SIZE             ( GROUP_SIZE     ),
  .DATA_WIDTH             ( 2 * DATA_WIDTH ),
  .LOG_MAX_ITERS          ( LOG_MAX_ITERS  ),
  .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
) agrupate_m (
  .clk                    ( clk                  ), //From top
  .rst                    ( rst                  ), //From top
  .configure              ( configure            ), //From top
  .num_iters              ( num_iters            ), //From top
  .num_reads_per_iter     ( num_reads_per_iter   ), //From top
  .data_in                ( mul2agr_data_w       ), //MUL to AGRUPATE 
  .valid_in               ( mul2agr_valid_w      ), //MUL to AGRUPATE 
  .avail_out              ( mul2agr_avail_w      ), //AGRUPATE to MUL 
  .data_out               ( data_out             ), //AFRUPATE to TOP  
  .valid_out              ( valid_out            ), //AFRUPATE to TOP
  .avail_in               ( avail_in             )  //AFRUPATE to TOP 
);

endmodule

