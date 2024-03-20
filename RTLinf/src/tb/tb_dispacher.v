`timescale 1ns / 1ps

//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 13.02.2024 10:34:26
// Design Name: 
// Module Name: tb_repetition_detector
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////
`define assert(signal, value) \
        if (signal !== value) begin \
            $display("ASSERTION FAILED in %m: %d != %d",signal, value); \
            $finish; \
        end



module TB_DISPACHER;
parameter DATA_WIDTH             = 8;
parameter GROUP_SIZE             = 4;
parameter LOG_MAX_ITERS          = 16;                     // number of bits for max iters register
parameter LOG_MAX_READS_PER_ITER = 16;                     // number of bits for max reads per iter
parameter ZERO_INFO               = GROUP_SIZE;  // number of bits for repetition detectoor

parameter HALF_CYCLE             = 25;
parameter CYCLE                  = HALF_CYCLE*2;
parameter DELTA                  = 2;
parameter CYCLE_AND_DELTA        = CYCLE+DELTA;


//input to dispacher
reg                                              clk;
reg                                              rst;
reg                                              valid_in;
reg                                              ready_in;
reg                                              weight_valid_in;
reg                                              configure;
reg [LOG_MAX_ITERS-1:0]                          num_iters;
reg [LOG_MAX_READS_PER_ITER-1:0]                 num_reads_per_iter;
wire [DATA_WIDTH * GROUP_SIZE - 1 + ZERO_INFO: 0] data_in;
reg  [DATA_WIDTH - 1 : 0]                        weight_data_in;
reg [DATA_WIDTH * GROUP_SIZE - 1 : 0]            value_in;
reg [ZERO_INFO  - 1 : 0]            zero_info_matrix;
//output
wire                                             valid_out;
wire                                             ready_out;
wire [DATA_WIDTH + ZERO_INFO-1:0]                 data_out;
wire [DATA_WIDTH - 1 : 0]                        value_out;
wire                                             weight_ready_out;
wire [DATA_WIDTH - 1 : 0]                        weight_out;

//internal
integer iters;
integer i,ii;
integer j,jj;
integer kk,ll;
integer cont;
wire [DATA_WIDTH - 1 : 0] value_in_unpacked[GROUP_SIZE -1 :0];

//Assigns
genvar k;

//input to dispatcher
assign data_in[GROUP_SIZE * DATA_WIDTH - 1 : 0] = value_in;
assign data_in[GROUP_SIZE * DATA_WIDTH + ZERO_INFO - 1: GROUP_SIZE * DATA_WIDTH] = zero_info_matrix;

//output
assign value_out = data_out[0+:DATA_WIDTH];
assign weight_out = data_out[DATA_WIDTH+:DATA_WIDTH];

//unpacked value in 
for(k = 0; k < GROUP_SIZE; k = k + 1) begin
    assign value_in_unpacked[k] = value_in[k*DATA_WIDTH +: DATA_WIDTH];
end


always begin
 #25 
 clk = !clk;
end


initial begin //Weights loop
    #CYCLE_AND_DELTA
    iters = 0;
    weight_valid_in = 0;
    while(iters < num_iters) begin
        #CYCLE_AND_DELTA //Start proccesing

        if (weight_ready_out) begin
            $display("****** new weight");
            weight_valid_in = 1;
            weight_data_in = 1+iters;
            iters = iters + 1;
        end else weight_valid_in = 0;
    end

end

initial begin // Activations
    clk = 1;
    num_iters = 2;
    num_reads_per_iter = GROUP_SIZE;
    
    configure = 1;
    rst = 0;
           
    valid_in = 0;
    ready_in = 1;
    
    #CYCLE_AND_DELTA
    rst = 1;
//UV 1
    for(i = 0; i < num_reads_per_iter; i = i + 1) begin
            #CYCLE_AND_DELTA //Start proccesing
            configure = 0;
            cont = 0;

            if(ready_out) begin 
                valid_in = 1;

                value_in [0 +: DATA_WIDTH] = 0;
                value_in [1 * DATA_WIDTH +: DATA_WIDTH] = i;
                value_in [2*DATA_WIDTH+:DATA_WIDTH] = i + 1;
                value_in [3*DATA_WIDTH+:DATA_WIDTH] = i + 1;
                
                for (j=0; j < GROUP_SIZE; j=j+1) begin
                    zero_info_matrix[j] = value_in[j*DATA_WIDTH+:DATA_WIDTH]==0;
                end
      

             end //End ready out
            else valid_in = 0;
    end // End for i
//UV 3
    for(i = 0; i < num_reads_per_iter; i = i + 1) begin
        #CYCLE_AND_DELTA //Start proccesing

        configure = 0;
        cont = 0;
        if(ready_out) begin 
            valid_in = 1;
    
            value_in [0+:DATA_WIDTH] = 0;
            value_in [1*DATA_WIDTH+:DATA_WIDTH] = 2+i;
            value_in [2*DATA_WIDTH+:DATA_WIDTH] = 1+i;
            value_in [3*DATA_WIDTH+:DATA_WIDTH] = 3+i;

                for (j=0; j < GROUP_SIZE; j=j+1) begin
                    zero_info_matrix[j] = value_in[j]==0;
                end
                
        end
        else valid_in = 0;
    end



    #10000
    $finish;
end

DISPATCHER dispacher_m(
    .clk                 (clk),
    .rst                 (rst),
    .configure           (configure),
    .num_iters           (num_iters),
    .num_reads_per_iter  (num_reads_per_iter),
    .act_data_in         (data_in),
    .act_valid_in        (valid_in),
    .act_avail_out       (ready_out),
    .weight_data_in      (weight_data_in),
    .weight_valid_in     (weight_valid_in),
    .weight_avail_out    (weight_ready_out),    
    .data_out            (data_out),
    .valid_out           (valid_out),
    .avail_in            (ready_in) 
);
endmodule