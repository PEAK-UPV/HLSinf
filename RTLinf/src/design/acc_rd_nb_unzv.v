// ACC module
//
// This module accumulates input data on a BRAM. The module runs for a given number of iterations. On every iteration
// the module performs a given number of "read" operations. On each operation, the input data is read and the 
// data is accumulated and stored in a memory. The memory has as many entries as number of "read" operations performed
// in each iteration. On the first iteration the data is stored in the memory (no accumulation). On the last iteration
// the accumulated data is forwarded through the output port.
//

`include "RTLinf.vh"

module ACC_RD_NB_UNZV #(
    parameter DATA_WIDTH             = 4,                            // input data width (output width = input width)
    parameter GROUP_SIZE             = 4,                            // group size
    localparam REP_INFO              = GROUP_SIZE + GROUP_SIZE,
    localparam INPUT_WIDTH           = DATA_WIDTH + REP_INFO,   // input data width (activation + weight + rep. info)
    localparam OUTPUT_WIDTH          = GROUP_SIZE * DATA_WIDTH   // output data width ( result (2*data width) +  rep. info)

  )(
    input clk,                                                       // clock signal
    input rst,                                                       // reset signal
    
    input [INPUT_WIDTH - 1 : 0]        data_in,            // IN interface:: data  
    output [OUTPUT_WIDTH - 1 : 0]      data_out            // OUT interface: data
  );



// wires 
wire [ GROUP_SIZE - 1: 0]     rep_info;                           // Repetition info extracted from FIFO
wire [ GROUP_SIZE - 1: 0]     zer_info;                          // Repetition info extracted from FIFO
wire [ DATA_WIDTH - 1: 0]     value_in;

// pipeline (read -> add -> write stages)
reg [ INPUT_WIDTH - 1 : 0]    read_data_fifo_r;
reg [INPUT_WIDTH - 1 : 0]     add_data_fifo_r;

wire [DATA_WIDTH - 1 : 0]     data_to_join[GROUP_SIZE - 1 : 0];

genvar i;

// combinational logic 
for (i=0; i<GROUP_SIZE; i=i+1) begin
    assign data_out [i * DATA_WIDTH +: DATA_WIDTH] = data_to_join[i];
    assign data_to_join[i] =  rep_info[i] ? value_in : zer_info[i] ? 0 : data_to_join[i];
end


// get input
assign value_in  = add_data_fifo_r[DATA_WIDTH-1:0];
assign rep_info  = add_data_fifo_r[DATA_WIDTH + GROUP_SIZE - 1 : DATA_WIDTH];
assign zer_info  = add_data_fifo_r[INPUT_WIDTH - 1 : DATA_WIDTH + GROUP_SIZE];

always @ (posedge clk) 
begin: control_logic
  // pipelined operations: READ -> ADD -> WRITE
  read_data_fifo_r  <= data_in;            // we capture the input data for the next stage (add)
  add_data_fifo_r   <= read_data_fifo_r;            // we keep the data from the fifo to this stage (add)
  // end
end
endmodule
  
  