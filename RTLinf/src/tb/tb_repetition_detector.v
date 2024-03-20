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



module tb_repetition_detector;
parameter DATA_WIDTH             = 8;
parameter GROUP_SIZE             = 4;
parameter LOG_MAX_ITERS          = 16;                     // number of bits for max iters register
parameter LOG_MAX_READS_PER_ITER = 16;                     // number of bits for max reads per iter
parameter ZERO_INFO              = GROUP_SIZE;             // number of bits for repetition detectoor

parameter HALF_CYCLE             = 25;
parameter CYCLE                  = HALF_CYCLE*2;
parameter DELTA                  = 2;
parameter CYCLE_AND_DELTA        = CYCLE+DELTA;


//input to repetition detector
reg                                   clk;
reg                                   rst;
reg                                   valid_in;
reg                                   ready_in;
reg                                   configure;
reg [LOG_MAX_ITERS-1:0]               num_iters;
reg [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter;
reg [DATA_WIDTH * GROUP_SIZE - 1 : 0] value_in;

//output
wire                                           valid_out;
wire                                           ready_out;
wire [GROUP_SIZE*DATA_WIDTH+ZERO_INFO-1:0]     data_out;
wire [DATA_WIDTH - 1 : 0] value_out_unpacked [GROUP_SIZE - 1 : 0];
wire [ZERO_INFO - 1 : 0]         zero_info_a;


//internal
integer i,ii;
integer j,jj;
integer kk,ll;
integer uv;
integer cont;
wire [DATA_WIDTH - 1 : 0] value_in_unpacked[GROUP_SIZE -1 :0];

//Assigns
genvar k;
for(k = 0; k < GROUP_SIZE; k = k + 1) begin
        assign value_in_unpacked[k] = value_in[k*DATA_WIDTH +: DATA_WIDTH];
        assign value_out_unpacked[k] = data_out[k*DATA_WIDTH +: DATA_WIDTH];
end

assign zero_info_a = data_out[GROUP_SIZE * DATA_WIDTH + ZERO_INFO - 1 : GROUP_SIZE * DATA_WIDTH];

//Function that assigns unique values to the input
function  [DATA_WIDTH * GROUP_SIZE - 1 : 0] create_values;
    input integer uv;
    input integer cont;
    begin 
        for(j = 0; j < GROUP_SIZE; j = j + 1) begin
            if(cont < uv) begin
                create_values[j*DATA_WIDTH +: DATA_WIDTH] = 0;
                cont = cont + 1;
            end 
            else create_values[j*DATA_WIDTH +: DATA_WIDTH] = j;
        end //End for j       
    end
endfunction


always begin
 #25 
 clk = !clk;
end

initial begin
    clk = 1;
    num_iters = 1;
    num_reads_per_iter = GROUP_SIZE;
    
    configure = 1;
    rst = 0;
           
    valid_in = 0;
    ready_in = 1;
    
    #CYCLE_AND_DELTA
    rst = 1;
    uv = 1;

    for(i = 0; i < num_iters * GROUP_SIZE + 1; i = i + 1) begin
        #CYCLE_AND_DELTA //Start proccesing
        configure = 0;
        cont = 0;
        value_in = create_values(uv, cont);

        if( i == num_iters * GROUP_SIZE) valid_in = 0;
        else valid_in = 1;
        uv = uv + 1;
    end


    #100
    $finish;
end

initial begin //Check loops
    #CYCLE_AND_DELTA

    for(ii = 0; ii < num_iters * GROUP_SIZE + 1; ii = ii + 1) begin
        #CYCLE_AND_DELTA//Start proccesing

        if(ii != 0) begin
           `assert(valid_out,1);
            for(jj = 0; jj < GROUP_SIZE; jj = jj + 1) begin
                `assert(value_in_unpacked[j],value_out_unpacked[j]);
            end //End for jj
            
            $display("Iter %d (time %t)", ii, $time);
            $display("--Values:");
            for(kk = 0; kk < GROUP_SIZE; kk = kk + 1) $write("%d ", value_out_unpacked[kk]);
        
            $display("\n--Repetition info matrix");
            for(ll = 0; ll < GROUP_SIZE; ll = ll + 1)  $write("%d ", zero_info_a[ll]);
            $write("\n");
        end //End if II=0 
    end

end

repetition_detector repetition_detector_0(
    .clk                 (clk),
    .rst                 (rst),
    .configure           (configure),
    .num_iters           (num_iters),
    .num_reads_per_iter  (num_reads_per_iter),
    .data_in             (value_in),
    .valid_in            (valid_in),
    .avail_out           (ready_out),
    .data_out            (data_out),
    .valid_out           (valid_out),
    .avail_in            (ready_in) 
);
endmodule