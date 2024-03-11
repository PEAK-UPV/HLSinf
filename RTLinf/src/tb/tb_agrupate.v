`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 21.02.2024 16:50:00
// Design Name: 
// Module Name: TB_AGRUPATE
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


module TB_AGRUPATE;
parameter DATA_WIDTH             = (2*8);
parameter GROUP_SIZE             = 4;
parameter LOG_MAX_ITERS          = 16;                     // number of bits for max iters register
parameter LOG_MAX_READS_PER_ITER = 16;                     // number of bits for max reads per iter 
parameter HALF_CYCLE             = 25;
parameter CYCLE                  = HALF_CYCLE*2;
parameter DELTA                  = 2;
parameter CYCLE_AND_DELTA        = CYCLE+DELTA;

parameter REP_INFO              = GROUP_SIZE*GROUP_SIZE;      // number of bits for repetition detectoor
parameter ZERO_INFO             = GROUP_SIZE;                 //    
parameter INPUT_WIDTH           = DATA_WIDTH + REP_INFO + ZERO_INFO; // input data width (activation + weight + rep. info)
parameter OUTPUT_WIDTH          = GROUP_SIZE * DATA_WIDTH;     // output data width ( result (2*data width) +  rep. info)


//input to dispacher
reg                                              clk;
reg                                              rst;
reg                                              valid_in;
reg                                              ready_in;

reg                                              configure;
reg [LOG_MAX_ITERS-1:0]                          num_iters;
reg [LOG_MAX_READS_PER_ITER-1:0]                 num_reads_per_iter;
reg [DATA_WIDTH - 1 : 0]                         result_in;
reg [REP_INFO - 1 : 0]                           rep_info_matrix;
reg [ZERO_INFO - 1 : 0]                          zer_info_matrix;
wire [INPUT_WIDTH: 0]                            data_in;

//output
wire                                             valid_out;
wire                                             ready_out;
wire [OUTPUT_WIDTH - 1:0]                        data_out;

//internal
integer i,j;
wire [DATA_WIDTH - 1 : 0] value_out_unpacked[GROUP_SIZE -1 :0];

//Assigns
genvar k;

//input to dispatcher
assign data_in[DATA_WIDTH - 1 : 0] = result_in;
assign data_in[REP_INFO + DATA_WIDTH - 1 : DATA_WIDTH]   = rep_info_matrix;
assign data_in[OUTPUT_WIDTH - 1 : REP_INFO + DATA_WIDTH] = zer_info_matrix;

//output
for(k = 0; k < GROUP_SIZE; k = k + 1) begin
    assign value_out_unpacked[k] = data_out[k*DATA_WIDTH +: DATA_WIDTH];
end


always begin
 #25 
 clk = !clk;
end


initial begin
    clk = 1;
    num_iters = 2;
    num_reads_per_iter = GROUP_SIZE;
    
    configure = 1;
    rst = 0;
           
    valid_in = 0;
    ready_in = 1;
    
    #CYCLE_AND_DELTA
    rst = 1;


    for(i = 0; i < 2*num_reads_per_iter; i = i + 1) begin
        #CYCLE_AND_DELTA //Start proccesing

        configure = 0;
        if(ready_out) begin 
            valid_in = 1;
            
            result_in = 1+i;

            rep_info_matrix = 0;
            zer_info_matrix = 0;
            for(j = 0; j < GROUP_SIZE ; j = j + 1) begin
                rep_info_matrix[j*GROUP_SIZE+j] = 1;
            end
            zer_info_matrix[0] = 1;
            

            #CYCLE_AND_DELTA
            result_in = 3+i;
            
            #CYCLE_AND_DELTA
            result_in = 4+i;
        end
        else valid_in = 0;
    end

//UV 3
/*
    for(i = 0; i < 2*num_reads_per_iter; i = i + 1) begin
        #CYCLE_AND_DELTA //Start proccesing

        configure = 0;
        if(ready_out) begin 
            valid_in = 1;

            rep_info_matrix = 0;
            for(j = 0; j < GROUP_SIZE ; j = j + 1) begin
                rep_info_matrix[j*GROUP_SIZE+j] = 1;
                zer_info_matrix[j] = 0;
            end
            rep_info_matrix[2] = 1;
            rep_info_matrix[2*GROUP_SIZE+2] = 0;

            result_in = 1+i;

            #CYCLE_AND_DELTA
            result_in = 2+i;

            #CYCLE_AND_DELTA
            result_in = 3+i;
        end
        else valid_in = 0;
    end
*/
//UV 1
    for(i = 0; i < 2*num_reads_per_iter; i = i + 1) begin
            #CYCLE_AND_DELTA //Start proccesing
            configure = 0;

            if(ready_out) begin 
                valid_in = 1;

                #CYCLE_AND_DELTA
                result_in = i + 1;

                rep_info_matrix = 0;
                for(j = 0; j < GROUP_SIZE ; j = j + 1) begin
                    rep_info_matrix[j] = 1;
                    zer_info_matrix[j] = 0;
                end        

             end //End ready out
            else valid_in = 0;
    end // End for i

    #10000
    $finish;
end

AGRUPATE agrupate_m(
    .clk                 (clk),
    .rst                 (rst),
    .configure           (configure),
    .num_iters           (num_iters),
    .num_reads_per_iter  (num_reads_per_iter),
    .data_in             (data_in),
    .valid_in            (valid_in),
    .avail_out           (ready_out),
    .data_out            (data_out),
    .valid_out           (valid_out),
    .avail_in            (ready_in) 
);
endmodule
