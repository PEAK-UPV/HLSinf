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



module tb_acc_nb;
parameter ACT_WIDTH              = 4;
parameter DATA_WIDTH             = 8;
parameter LOG_GS                 = 2;
parameter GROUP_SIZE             = 4;
parameter LOG_MAX_ITERS          = 16;                     // number of bits for max iters register
parameter LOG_MAX_READS_PER_ITER = 16;                     // number of bits for max reads per iter
parameter HIGH_MODE              = "UNZV";                 // options: UV, UNZV, NZV
parameter LOW_MODE               = "UNZV";                 // options: UV, UNZV, NZV
localparam ZERO_INFO             = GROUP_SIZE;
localparam REP_INFO_UV           = GROUP_SIZE + 1;
localparam REP_INFO_NZV          = LOG_GS + ZERO_INFO + 1;
localparam REP_INFO_UNZV         = REP_INFO_UV + ZERO_INFO;
localparam REP_INFO_HIGH         = (HIGH_MODE=="UNZV") ? REP_INFO_UNZV :  (HIGH_MODE=="NZV") ? REP_INFO_NZV : REP_INFO_UV;
localparam REP_INFO_LOW          = (LOW_MODE=="UNZV")  ? REP_INFO_UNZV :  (LOW_MODE=="NZV") ? REP_INFO_NZV : REP_INFO_UV;

parameter HALF_CYCLE             = 25;
parameter CYCLE                  = HALF_CYCLE*2;
parameter DELTA                  = 2;
parameter CYCLE_AND_DELTA        = CYCLE+DELTA;


//input to acc
reg                                   clk;
reg                                   rst;
reg                                   ready_in; 
reg                                   configure;
reg [LOG_MAX_ITERS-1:0]               num_iters;
reg [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter;
reg                                   ready_to_acc;
reg                                   valid_high; 
reg                                   valid_low; 
reg [ACT_WIDTH+DATA_WIDTH-1:0]        value_high_to_acc; 
reg [ACT_WIDTH+DATA_WIDTH-1:0]        value_low_to_acc; 
reg [REP_INFO_UV - 1-1 : 0]           rdata_high_uv; 
reg [REP_INFO_UV - 1-1 : 0]           rdata_low_uv; 
reg                                   rdata_high_last; 
reg                                   rdata_low_last; 
reg [ZERO_INFO - 1 : 0]               rdata_high_zero; 
reg [ZERO_INFO - 1 : 0]               rdata_low_zero; 
wire [REP_INFO_HIGH-1:0]              rdata_high_to_acc;
wire [REP_INFO_LOW-1:0]               rdata_low_to_acc; 
//output
wire avail_out_high;
wire avail_out_low;
wire [8*2-1:0]data_out_acc;
wire valid_out;


integer i, j;
assign rdata_high_to_acc[REP_INFO_UV - 1 - 1 : 0] = rdata_high_uv;
assign rdata_low_to_acc[REP_INFO_UV - 1 - 1 : 0]  = rdata_low_uv;
assign rdata_high_to_acc[REP_INFO_HIGH - 1 - 1 : REP_INFO_UV - 1] = rdata_high_zero;
assign rdata_low_to_acc[REP_INFO_LOW - 1 - 1 : REP_INFO_UV - 1]   = rdata_low_zero;
assign rdata_high_to_acc[REP_INFO_HIGH - 1] = rdata_high_last;
assign rdata_low_to_acc[REP_INFO_LOW - 1]   = rdata_low_last;
//internal

always begin
 #25 
 clk = !clk;
end

initial begin
    clk = 1;
    rst = 0;
           
    valid_high = 0;
    valid_low = 0;
    ready_in = 1;
    ready_to_acc = 1;
    #CYCLE_AND_DELTA        
    num_iters = 3;
    num_reads_per_iter = 1;
    configure = 1;
    rst = 1;
    
    #CYCLE_AND_DELTA
    configure = 0;
    

//4L,4H
    for(i = 0; i < 4; i = i + 1) begin
        #CYCLE_AND_DELTA //Start proccesing
        valid_high = 1;
        valid_low = 1;
        value_high_to_acc = 2;
        value_low_to_acc = 2;
        for(j = 0; j < 4; j = j + 1) begin
            rdata_high_uv[j] = j == i? 1'b1 : 1'b0; 
            rdata_low_uv[j] = j == i? 1'b1 : 1'b0; 
        end
        //is last
        rdata_high_last = i == 3 ? 1'b1 : 1'b0;
        rdata_low_last = i == 3 ? 1'b1 : 1'b0;
        rdata_high_zero  = 4'b0;
        rdata_low_zero = 4'b0;
    end
    #CYCLE_AND_DELTA //Start proccesing

    valid_high = 0;
    valid_low  = 0;
    
    for(i = 0; i < 4; i = i + 1) begin
        #CYCLE_AND_DELTA //Start proccesing
        valid_high = 1;
        valid_low = 1;
        value_high_to_acc = 2;
        value_low_to_acc = 2;
        for(j = 0; j < 4; j = j + 1) begin
            rdata_high_uv[j] = j == i? 1'b1 : 1'b0; 
            rdata_low_uv[j] = j == i? 1'b1 : 1'b0; 
        end
        //is last
        rdata_high_last = i == 3 ? 1'b1 : 1'b0;
        rdata_low_last = i == 3 ? 1'b1 : 1'b0;
        rdata_high_zero  = 4'b0;
        rdata_low_zero = 4'b0;
    end
    #CYCLE_AND_DELTA //Start proccesing

    valid_high = 0;
    valid_low  = 0;    
//4L,3H
/*
    for(i = 0; i < 4; i = i + 1) begin
        valid_high = i < 3 ? 1 : 0;
        valid_low = 1;
        value_high_to_acc = 2;
        value_low_to_acc = 2;
        for(j = 0; j < 4; j = j + 1) begin
            rdata_high_uv[j] = j == i? 1'b1 : 1'b0; 
            if(j == 0) begin
                rdata_low_uv[j] = 1; 
                rdata_low_uv[3] = 1; 
            end
            else rdata_low_uv[j] = j == i? 1'b1 : 1'b0; 
        end
        //is last
        rdata_high_uv[REP_INFO_UV - 1] = i == 2 ? 1'b1 : 1'b0;
        rdata_low_uv[REP_INFO_UV - 1] = i == 3 ? 1'b1 : 1'b0;
        rdata_high_zero  = 4'b0;
        rdata_low_zerp = 4'b0;
    end
    */
//4L,2H

//4L,1H
//1L,1H


    #100
    $finish;
end

/*
initial begin //Check loops
    #CYCLE_AND_DELTA

    for(ii = 0; ii < num_iters * num_reads_per_iter + 1; ii = ii + 1) begin
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
            for(kk = 0; kk < GROUP_SIZE; kk = kk + 1) begin
                for(ll = 0; ll < GROUP_SIZE; ll = ll + 1)  $write("%d ", rep_info[kk*GROUP_SIZE + ll]);
                $write("\n");
            end
        end //End if II=0 
    end

end
*/
    ACC_RD_NB #(
      .HIGH_MODE              ( HIGH_MODE              ),
      .LOW_MODE               ( LOW_MODE               ),
      .GROUP_SIZE             ( GROUP_SIZE             ),
      .LOG_GS                 ( LOG_GS                 ),
      .DATA_WIDTH             ( ACT_WIDTH+DATA_WIDTH   ),
      .OUT_DATA_WIDTH         ( 2*DATA_WIDTH           ),
      .NUM_ADDRESSES   ( 1024   ),
      .LOG_MAX_ADDRESS ( 10 ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
    ) acc_m (
      .clk                    ( clk                       ),
      .rst                    ( rst                       ),
      .configure              ( configure                 ),
      .num_iters              ( num_iters                 ),
      .num_reads_per_iter     ( num_reads_per_iter        ),
      .data_in_high           ( {rdata_high_to_acc,value_high_to_acc }   ),
      .valid_in_high          ( valid_high ),
      .avail_out_high         ( avail_out_high ),
      .data_in_low            ( {rdata_low_to_acc,value_low_to_acc}   ),
      .valid_in_low           ( valid_low  ),
      .avail_out_low          ( avail_out_low  ),      
      .data_out               ( data_out_acc       ),
      .valid_out              ( valid_out ),
      .avail_in               ( ready_to_acc)
    );

endmodule
