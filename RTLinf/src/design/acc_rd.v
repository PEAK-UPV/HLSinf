// ACC module
//
// This module accumulates input data on a BRAM. The module runs for a given number of iterations. On every iteration
// the module performs a given number of "read" operations. On each operation, the input data is read and the 
// data is accumulated and stored in a memory. The memory has as many entries as number of "read" operations performed
// in each iteration. On the first iteration the data is stored in the memory (no accumulation). On the last iteration
// the accumulated data is forwarded through the output port.
//

`include "RTLinf.vh"

module ACC_RD #(
    parameter REPETITION_DETECTION   = "UV",                         // options: no, UV, UNZV, NZV
    parameter DATA_WIDTH             = 8,                            // input data width (output width = input width)
    parameter LOG_GS                 = 2,
    parameter GROUP_SIZE             = 4,                            // group size
    parameter LOG_MAX_ITERS          = 16,                           // number of bits for max iters register
    parameter NUM_ADDRESSES          = 4096,                         // number of addresses
    parameter LOG_MAX_ADDRESS        = 12,                           // number of address bits
    parameter LOG_MAX_READS_PER_ITER = 12,                           // number of bits for max reads per iter
    localparam UNZV_mode             =  (REPETITION_DETECTION =="UNZV") ? 1 : 0,
    localparam NZV_mode              =  (REPETITION_DETECTION =="NZV") ? 1 : 0,      
    localparam REP_INFO              = GROUP_SIZE + 1,
    localparam INPUT_WIDTH           = DATA_WIDTH + REP_INFO,        // input data width (activation + weight + rep. info)
    localparam OUTPUT_WIDTH          = GROUP_SIZE * DATA_WIDTH       // output data width ( result (2*data width) +  rep. info)

  )(
    input clk,                                                       // clock signal
    input rst,                                                       // reset signal
  
    input                                   configure,               // CONFIGURE interface:: configure signal
    input [LOG_MAX_ITERS-1:0]               num_iters,               // CONFIGURE interface:: number of iterations for reads
    input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration
  
    input [INPUT_WIDTH - 1 : 0]             data_in,                 // IN interface:: data
    input                                   valid_in,                // IN interface:: valid in
    output                                  avail_out,               // IN interface:: avail
  
    output [OUTPUT_WIDTH - 1 : 0]           data_out,                // OUT interface: data
    output                                  valid_out,               // OUT interface: valid
    input                                   avail_in                 // OUT interface: avail
  );

if (UNZV_mode) begin
  ACC_RD_UNZV #(
    .GROUP_SIZE             ( GROUP_SIZE             ),
    .DATA_WIDTH             ( DATA_WIDTH             ),
    .NUM_ADDRESSES          ( NUM_ADDRESSES          ),
    .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS        ),
    .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
    .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
  ) acc_unzv_m (
    .clk                    ( clk                    ),
    .rst                    ( rst                    ),
    .configure              ( configure              ),
    .num_iters              ( num_iters              ),
    .num_reads_per_iter     ( num_reads_per_iter     ),
    .data_in                ( data_in                ),
    .valid_in               ( valid_in               ),
    .avail_out              ( avail_out              ),
    .data_out               ( data_out               ),
    .valid_out              ( valid_out              ),
    .avail_in               ( avail_in               )
  );
end else if (NZV_mode) begin
  ACC_RD_NZV #(
    .GROUP_SIZE             ( GROUP_SIZE             ),
    .LOG_GS                 ( LOG_GS                 ),
    .DATA_WIDTH             ( DATA_WIDTH             ),
    .NUM_ADDRESSES          ( NUM_ADDRESSES          ),
    .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS        ),
    .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
    .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
  ) acc_nzv_m (
    .clk                    ( clk                    ),
    .rst                    ( rst                    ),
    .configure              ( configure              ),
    .num_iters              ( num_iters              ),
    .num_reads_per_iter     ( num_reads_per_iter     ),
    .data_in                ( data_in                ),
    .valid_in               ( valid_in               ),
    .avail_out              ( avail_out              ),
    .data_out               ( data_out               ),
    .valid_out              ( valid_out              ),
    .avail_in               ( avail_in               )
  );
end else begin
  ACC_RD_UV #(
    .GROUP_SIZE             ( GROUP_SIZE             ),
    .DATA_WIDTH             ( DATA_WIDTH             ),
    .NUM_ADDRESSES          ( NUM_ADDRESSES          ),
    .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS        ),
    .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
    .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
  ) acc_uv_m (
    .clk                    ( clk                    ),
    .rst                    ( rst                    ),
    .configure              ( configure              ),
    .num_iters              ( num_iters              ),
    .num_reads_per_iter     ( num_reads_per_iter     ),
    .data_in                ( data_in                ),
    .valid_in               ( valid_in               ),
    .avail_out              ( avail_out              ),
    .data_out               ( data_out               ),
    .valid_out              ( valid_out              ),
    .avail_in               ( avail_in               )
  );
end
endmodule
  
  
