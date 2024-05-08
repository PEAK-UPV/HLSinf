// AGR module
//
// This module agrupates the results to obtain GROUP SIZE results. This module can send a batch each 1 - GROUP_SIZE cicle.
//

`include "RTLinf.vh"


module AGRUPATE#(
  parameter GROUP_SIZE             = 4,                          // group size
  parameter DATA_WIDTH             = (2 * 8),                    // input value width (output is 2x input width)
  parameter LOG_MAX_ITERS          = 16,                         // number of bits for max iters register
  parameter LOG_MAX_READS_PER_ITER = 16,                         // number of bits for max reads per iter
  parameter REP_INFO               = GROUP_SIZE + 1,             // number of bits for repetition detectoor
  localparam INPUT_WIDTH           = DATA_WIDTH + REP_INFO,      // input data width (activation + weight + rep. info)
  localparam OUTPUT_WIDTH          = GROUP_SIZE * DATA_WIDTH     // output data width ( result (2*data width) +  rep. info)

)(
  input clk,
  input rst,

  input [INPUT_WIDTH - 1 : 0]     data_in,                 // ACTIVATION & WEIGHT interface:: activ ations data
  input                           valid_in,                // ACTIVATION & WEIGHT interface:: activ ation valid in
  output                          avail_out,               // ACTIVATION & WEIGHT interface:: avail 

  output [OUTPUT_WIDTH - 1 : 0]   data_out,                // OUT interface: data
  output                          valid_out,               // OUT interface: valid
  input                           avail_in                 // OUT interface: avail
);

// wires
wire                      perform_operation_w;                // whether we perform a "read" operation in this cycle

wire [ DATA_WIDTH - 1: 0] value_in;                           // Result value extracted from FIFO   
wire [ GROUP_SIZE - 1: 0] rep_info;                           // Repetition info extracted from FIFO
wire                      send;                               // Send if is last

// registers
reg [DATA_WIDTH - 1: 0]   output_values[GROUP_SIZE - 1 : 0];    // Unpacked output values
reg                       send_r;                               //Registered version of send variable

genvar  i;
integer j;

// ** Combinational logic **
assign perform_operation_w = valid_in;
assign avail_out = 1'b1; // always available

assign value_in        = data_in[DATA_WIDTH-1:0];
assign rep_info        = data_in[DATA_WIDTH + GROUP_SIZE - 1 : DATA_WIDTH];
assign send            = data_in[INPUT_WIDTH - 1]; //last bit of input indicates if we have received the last element of the group

assign valid_out = perform_operation_w & send_r; 

for(i = 0; i < GROUP_SIZE; i = i + 1) begin
    assign data_out[i * DATA_WIDTH +: DATA_WIDTH ] = output_values[i]; 
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
                if(rep_info[j]) begin
                    output_values[j] <= value_in;
                end // End if writing
            end // End for j      
        end // End if perform_operation_w
        else send_r <= 0;
    end // End if else reset
end

endmodule
