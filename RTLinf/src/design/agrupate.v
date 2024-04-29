// AGR module
//
// This module agrupates the results to obtain GROUP SIZE results. This module can send a batch each 1 - GROUP_SIZE cicle.
//

`include "RTLinf.vh"


module AGRUPATE#(
  parameter GROUP_SIZE             = 4,                          // group size
  parameter LOG_GROUP_SIZE         = 2,                          // number of bits forgroup size
  parameter DATA_WIDTH             = (2 * 8),                    // input value width (output is 2x input width)
  parameter LOG_MAX_ITERS          = 16,                         // number of bits for max iters register
  parameter LOG_MAX_READS_PER_ITER = 16,                         // number of bits for max reads per iter
  localparam REP_INFO              = GROUP_SIZE*GROUP_SIZE,      // number of bits for repetition detectoor
  localparam INPUT_WIDTH           = DATA_WIDTH + REP_INFO+LOG_GROUP_SIZE+LOG_GROUP_SIZE,      // input data width (activation + weight + rep. info)
  localparam OUTPUT_WIDTH          = GROUP_SIZE * DATA_WIDTH     // output data width ( result (2*data width) +  rep. info)

)(
  input clk,
  input rst,

  input                                     configure,               // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS - 1 : 0]             num_iters,               // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER - 1 : 0]    num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration

  input [INPUT_WIDTH - 1 : 0]               data_in,                 // ACTIVATION & WEIGHT interface:: activ ations data
  input                                     valid_in,                // ACTIVATION & WEIGHT interface:: activ ation valid in
  output                                    avail_out,               // ACTIVATION & WEIGHT interface:: avail 

  output [OUTPUT_WIDTH - 1 : 0]             data_out,                // OUT interface: data
  output                                    valid_out,               // OUT interface: valid
  input                                     avail_in                 // OUT interface: avail
);

// wires
wire [INPUT_WIDTH - 1: 0]          data_write_w;                      // data to write to FIFO
wire                               write_w;                           // write signal to FIFO
wire                               full_w;                            // full signal from FIFO
wire                               almost_full_w;                     // almost_full signal from FIFO
wire [INPUT_WIDTH- 1: 0]           data_read_w;                       // data read from FIFO
wire                               next_read_w;                       // next_read signal to FIFO
wire                               empty_w;                           // empty signal from FIFO
wire                               perform_operation_w;               // whether we perform a "read" operation in this cycle

wire [ DATA_WIDTH - 1: 0]          value_in;                         // Result value extracted from FIFO   
wire [ REP_INFO - 1: 0]            rep_info;                         // Repetition info extracted from FIFO
wire [LOG_GROUP_SIZE:0]            pos_actual;  
wire [LOG_GROUP_SIZE:0]            last_uv; 

wire [ GROUP_SIZE - 1: 0]          writing;                          // Indicates the elements that will be stored in the current cycle
wire                                 send;            //TODO

// registers
reg [DATA_WIDTH - 1: 0]          output_values[GROUP_SIZE - 1 : 0];     // Unpacked output values
reg                              send_r;                                //Registered version of send variable

genvar i;
integer j;

// ** Combinational logic **
assign perform_operation_w =(~empty_w ) & avail_in;

// FIFO write and read
assign data_write_w = data_in;
assign write_w = valid_in;
assign avail_out = ~almost_full_w & ~full_w;
assign next_read_w = perform_operation_w;

// Extract input from FIFO
assign value_in  = data_read_w[DATA_WIDTH - 1 : 0];
assign rep_info  = data_read_w[DATA_WIDTH + REP_INFO - 1 : DATA_WIDTH];
assign pos_actual = data_read_w[DATA_WIDTH + REP_INFO + LOG_GROUP_SIZE - 1 : DATA_WIDTH + REP_INFO];
assign last_uv = data_read_w[INPUT_WIDTH - 1 : DATA_WIDTH + REP_INFO];

// Assign Output
for(i = 0; i < GROUP_SIZE; i = i + 1) begin
    assign data_out[i * DATA_WIDTH +: DATA_WIDTH ] = output_values[i]; 
end
assign valid_out = perform_operation_w & send_r; 

// if all the elements are stored in this cycle it means that we need to send the batch
assign send = pos_actual >= last_uv;

//Get the elements to write in this cycle.
for(i = 0; i < GROUP_SIZE; i = i + 1) begin
    assign writing[i] = rep_info[pos_actual * GROUP_SIZE + i];
end


// **Sequential logic**

always @ (posedge clk) 
begin
    if (~rst) begin
        send_r <= 0;
    end else begin
        if(perform_operation_w)begin
            send_r <= send;

            for(j = 0; j < GROUP_SIZE; j =  j + 1) begin
                if(writing[j]) begin
                    output_values[j] <= value_in;
                end // End if writing
            end // End for j      

        end // End if perform_operation_w
    end // End if else reset
end


// Modules

// FIFO
FIFO #(
  .NUM_SLOTS     ( 4                       ),
  .LOG_NUM_SLOTS ( 2                       ),
  .DATA_WIDTH    ( INPUT_WIDTH             )
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


endmodule
