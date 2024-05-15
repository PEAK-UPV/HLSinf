// Module repetition_detector
//
// This module implements the required logic to find the unique elements within a group
// of activations.
//

`include "RTLinf.vh"


module repetition_detector#(
    parameter UNZV_mode              = 0, 
    parameter GROUP_SIZE             = 4,                      // group size
    parameter LOG_GS                 = 2,
    parameter DATA_WIDTH             = 8,                      // input and output data width
    parameter LOG_MAX_ITERS          = 16,                     // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16,                     // number of bits for max reads per iter
    localparam ZERO_INFO             = GROUP_SIZE,             
    localparam REP_INFO              = UNZV_mode ? (GROUP_SIZE + ZERO_INFO + 1)  : (GROUP_SIZE + 1),         // row of equivalences + index_element sending + is_last    
    localparam INPUT_WIDTH           = GROUP_SIZE*DATA_WIDTH   // number of bits for input (activation + weight + rep. info)
)(
  input clk,
  input rst,

  input                               configure,           // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]           num_iters,           // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0]  num_reads_per_iter,  // CONFIGURE interface:: number of reads per iteration  

  input [INPUT_WIDTH-1:0]             data_in,     // ACTIVATION interface:: data
  input                               valid_in,    // ACTIVATION interface:: valid
  output                              avail_out,   // ACTIVATION interface:: avail

  output [DATA_WIDTH-1:0]             data_out,    // OUT1 interface:: data
  output [REP_INFO-1:0]               rdata_out,   // OUT1 interface:: rdata
  output                              valid_out,   // OUT1 interface:: valid
  input                               avail_in     // OUT1 interface:: avail

);


if (UNZV_mode) begin
    repetition_detector_unzv #(
      .GROUP_SIZE             ( GROUP_SIZE               ),
      .LOG_GS                 ( LOG_GS                   ),
      .DATA_WIDTH             ( DATA_WIDTH               ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS            ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER   )
    ) repetition_detector_unzv_m (
      .clk                    ( clk                      ),
      .rst                    ( rst                      ),
      .configure              ( configure                ),
      .num_iters              ( num_iters                ),
      .num_reads_per_iter     ( num_reads_per_iter       ),
      .valid_in               ( valid_in                 ),
      .data_in                ( data_in                  ),
      .avail_in               ( avail_in                 ),
      .valid_out              ( valid_out                ),
      .data_out               ( data_out                 ),
      .avail_out              ( avail_out                ),
      .rdata_out              ( rdata_out                )
    );
end else begin
    repetition_detector_uv #(
      .GROUP_SIZE             ( GROUP_SIZE               ),
      .LOG_GS                 ( LOG_GS                   ),
      .DATA_WIDTH             ( DATA_WIDTH               ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS            ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER   )
    ) repetition_detector_uv_m (
      .clk                    ( clk                      ),
      .rst                    ( rst                      ),
      .configure              ( configure                ),
      .num_iters              ( num_iters                ),
      .num_reads_per_iter     ( num_reads_per_iter       ),
      .valid_in               ( valid_in                 ),
      .data_in                ( data_in                  ),
      .avail_in               ( avail_in                 ),
      .valid_out              ( valid_out                ),
      .data_out               ( data_out                 ),
      .avail_out              ( avail_out                ),
      .rdata_out              ( rdata_out                )
    );
end

endmodule