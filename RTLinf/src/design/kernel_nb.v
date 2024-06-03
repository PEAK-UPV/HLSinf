`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/17/2024 10:21:12 AM
// Design Name: 
// Module Name: kernel_rd
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


module KERNEL_NB #(
  parameter GROUP_SIZE             = 4,    // group size
  parameter LOG_GS                 = 2,
  parameter DATA_WIDTH             = 8,    // data width
  parameter ACT_WIDTH              = 4,
  parameter NUM_INPUTS             = 9,    // number of inputs
  parameter NUM_LANES              = 9,    // number of lanes
  parameter NUM_OUTPUTS            = 9,    // number of outputs
  parameter LOG_MAX_ITERS          = 16,   // number of bits for max iters
  parameter LOG_MAX_READS_PER_ITER = 16,   // number of bits for reads_per_iter
  parameter LOG_MAX_ADDRESS        = 12,   // number of bits for addresses
  parameter NUM_ADDRESSES          = 4096, // number of addresses in memories
  parameter HIGH_MODE              = "UV", // options: UV, UNZV, NZV
  parameter LOW_MODE               = "UV", // options: UV, UNZV, NZV
  localparam REP_INFO             = GROUP_SIZE + 1

)(
  input                                   clk,                // clock input
  input                                   rst,                // reset input
  
  output [NUM_INPUTS*LOG_MAX_ADDRESS-1:0]      act_addr,          // read address to BRAM memories (activations)
  output [NUM_INPUTS-1:0]                      act_read,          // read signal to BRAM memories (activations)
  input [NUM_INPUTS*GROUP_SIZE*DATA_WIDTH-1:0] act_data,          // data input from BRAM memories (activations)
  input [NUM_INPUTS-1:0]                       act_valid,         // valid input from BRAM memories
  
  output [LOG_MAX_ADDRESS-1:0]                 weight_addr,       // read address to BRAM memory (weights)
  output                                       weight_read,       // read signal to BRAM memory (weights)
  input [NUM_LANES*DATA_WIDTH-1:0]             weight_data,       // data input from BRAM memory (weights)
  input                                        weight_valid,      // valid input from BRAM memory
   
  input                                   configure,          // configure signal
  input [LOG_MAX_ITERS-1:0]               num_iters,          // num iterations
  input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter, // num reads per iteration
  input [LOG_MAX_ADDRESS-1:0]             read_address,       // read address
  input [LOG_MAX_ADDRESS-1:0]             write_address,      // write address
  input [DATA_WIDTH-1:0]                  min_clip,           // min cliping value
  input [DATA_WIDTH-1:0]                  max_clip,           // max cliping value
  input                                   conf_mode_in,       // conf mode for distribute_in module
  input                                   conf_mode_out,      // conf mode for distribute_out module
  
  output [NUM_OUTPUTS*GROUP_SIZE*DATA_WIDTH-1:0] data_out,        // data output to BRAM memories
  output [NUM_OUTPUTS*LOG_MAX_ADDRESS-1:0]       addr_out,        // address output to BRAM memories
  output [NUM_OUTPUTS-1:0]                       valid_out        // valid signals to BRAM memories
);  

// genvars
genvar x;


// wires between READ and RD modules
wire [NUM_INPUTS-1:0]                       act_read2rd_valid_w; //TODO es Z
wire [GROUP_SIZE*DATA_WIDTH-1:0]            act_read2rd_data_w[NUM_INPUTS-1:0];
wire [NUM_INPUTS-1:0]                       act_read2rd_avail_w; //TODO es Z

// wires between WEIGHT_READ and DISTRIBUTE_IN modules
wire                                        weight_read2distr_valid_w;
wire [NUM_LANES*DATA_WIDTH-1:0]             weight_read2distr_data_w;
wire                                        weight_read2distr_avail_w;



// [high] wires between RD and DISTRIBUTE_IN modules
wire [NUM_INPUTS-1:0]                       act_rd2distr_valid_high_w;
wire [NUM_INPUTS-1:0]                       act_rd2distr_avail_high_w; 
wire [4-1:0]                                act_rd2distr_data_high_w[NUM_INPUTS-1:0];
wire [REP_INFO-1:0]                         act_rd2distr_rdata_high_w[NUM_INPUTS-1:0];
wire [NUM_INPUTS*4-1:0]                     act_rd2distr_combined_data_high_w;
wire [NUM_INPUTS*REP_INFO-1:0]              act_rd2distr_combined_rdata_high_w;
wire [NUM_INPUTS-1:0]                       act_rd2distr_combined_valid_high_w;


// [low] wires between RD and DISTRIBUTE_IN modules
wire [NUM_INPUTS-1:0]                       act_rd2distr_valid_low_w;
wire [NUM_INPUTS-1:0]                       act_rd2distr_avail_low_w; 
wire [4-1:0]                                act_rd2distr_data_low_w[NUM_INPUTS-1:0];
wire [REP_INFO-1:0]                         act_rd2distr_rdata_low_w[NUM_INPUTS-1:0];
wire [NUM_INPUTS*4-1:0]                     act_rd2distr_combined_data_low_w;
wire [NUM_INPUTS*REP_INFO-1:0]              act_rd2distr_combined_rdata_low_w;
wire [NUM_INPUTS-1:0]                       act_rd2distr_combined_valid_low_w;


//[high] wires between DISTRIBUTE_IN and MUL modules
wire [NUM_LANES*ACT_WIDTH-1:0]              act_distr2mul_data_high_w;
wire [NUM_LANES*REP_INFO-1:0]               act_distr2mul_rdata_high_w;
wire [NUM_LANES-1:0]                        act_distr2mul_valid_high_w;
wire [NUM_LANES-1:0]                        act_distr2mul_avail_high_w; 
wire [NUM_LANES*DATA_WIDTH-1:0]             weight_distr2mul_data_high_w;
wire [NUM_LANES-1:0]                        weight_distr2mul_valid_high_w;
wire [NUM_LANES-1:0]                        weight_distr2mul_avail_high_w;

//[low] wires between DISTRIBUTE_IN and MUL modules
wire [NUM_LANES*ACT_WIDTH-1:0]              act_distr2mul_data_low_w;
wire [NUM_LANES*REP_INFO-1:0]               act_distr2mul_rdata_low_w;
wire [NUM_LANES-1:0]                        act_distr2mul_valid_low_w;
wire [NUM_LANES-1:0]                        act_distr2mul_avail_low_w; 
wire [NUM_LANES*DATA_WIDTH-1:0]             weight_distr2mul_data_low_w;
wire [NUM_LANES-1:0]                        weight_distr2mul_valid_low_w;
wire [NUM_LANES-1:0]                        weight_distr2mul_avail_low_w;

//[high] wires between MUL and ALIGN modules
wire [(ACT_WIDTH + DATA_WIDTH) + REP_INFO - 1:0]   mul2align_data_high_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                                    mul2align_valid_high_w;
wire [NUM_LANES-1:0]                                    mul2align_avail_high_w;

//[low] wires between MUL and ALIGN modules
wire [(ACT_WIDTH + DATA_WIDTH) + REP_INFO - 1:0]  mul2align_data_low_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                                  mul2align_valid_low_w;
wire [NUM_LANES-1:0]                                  mul2align_avail_low_w;

//[high]  wires between ALIGN and ACC modules
wire [(ACT_WIDTH + DATA_WIDTH) + REP_INFO - 1:0] align2acc_data_high_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                                  align2acc_valid_high_w;
wire [NUM_LANES-1:0]                                  align2acc_avail_high_w;

//[low] wires between ALIGN and ACC modules
wire [(ACT_WIDTH + DATA_WIDTH) + REP_INFO - 1:0]  align2acc_data_low_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                                  align2acc_valid_low_w;
wire [NUM_LANES-1:0]                                  align2acc_avail_low_w;

// wires between ACC and DISTRIBUTE_IN modules
wire [GROUP_SIZE * 2 * DATA_WIDTH - 1:0]      acc2distr_data_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                          acc2distr_valid_w;
wire [NUM_LANES-1:0]                          acc2distr_avail_w;
wire [NUM_LANES*2*GROUP_SIZE*DATA_WIDTH-1:0]  acc2distr_combined_data_w;

// wires between DISTRIBUTE_IN and WRITE modules
wire [NUM_OUTPUTS*2*GROUP_SIZE*DATA_WIDTH-1:0] distr2write_data_w;
wire [NUM_OUTPUTS-1:0]                         distr2write_valid_w;
wire [NUM_OUTPUTS-1:0]                         distr2write_avail_w;

// output wires from WRITE modules
wire [GROUP_SIZE*DATA_WIDTH-1:0]               write2mem_data_w[NUM_OUTPUTS-1:0];
wire [LOG_MAX_ADDRESS-1:0]                     write2mem_addr_w[NUM_OUTPUTS-1:0];
wire [NUM_OUTPUTS-1:0]                         write2mem_valid_w;

// combinational logic

genvar i;

// module output to BRAM memories
generate
  for (i=0; i<NUM_OUTPUTS; i=i+1) begin
    assign data_out[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH] = write2mem_data_w[i];
    assign addr_out[((i+1)*LOG_MAX_ADDRESS)-1:i*LOG_MAX_ADDRESS]             = write2mem_addr_w[i];
    assign valid_out[i]                                                      = write2mem_valid_w[i];
  end
 endgenerate

// combined data and valid signal between RD and DISTRIBUTE_IN modules
generate
  for (i=0; i<NUM_INPUTS; i=i+1) begin
    assign act_rd2distr_combined_data_high_w[((i+1)*4)-1:i*4]  = act_rd2distr_data_high_w[i];
    assign act_rd2distr_combined_data_low_w[((i+1)*4)-1:i*4]   = act_rd2distr_data_low_w[i];
    assign act_rd2distr_combined_rdata_high_w[((i+1)*REP_INFO)-1:i*REP_INFO] = act_rd2distr_rdata_high_w[i];
    assign act_rd2distr_combined_rdata_low_w[((i+1)*REP_INFO)-1:i*REP_INFO]  = act_rd2distr_rdata_low_w[i];
    assign act_rd2distr_combined_valid_high_w[i]                                     = act_rd2distr_valid_high_w[i];
    assign act_rd2distr_combined_valid_low_w[i]                                      = act_rd2distr_valid_low_w[i];
  end
endgenerate

// combined data between ACC and DISTRIBUTE_OUT modules
generate
  for (i=0; i<NUM_LANES; i=i+1) begin
    assign acc2distr_combined_data_w[((i+1)*2*GROUP_SIZE*DATA_WIDTH)-1:i*2*GROUP_SIZE*DATA_WIDTH] = acc2distr_data_w[i];
  end
endgenerate

// activation READ modules
generate
for ( i=0; i<NUM_INPUTS; i=i+1) begin

  READ #(
    .DATA_WIDTH             ( GROUP_SIZE * DATA_WIDTH       ),
    .LOG_MAX_ITERS          ( LOG_MAX_ITERS                 ),
    .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER        ),
    .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS               ),
    .TYPE                   ( "activations"                 )
  ) act_read_m (
    .clk                    ( clk                           ),
    .rst                    ( rst                           ),
    .configure              ( configure                     ),
    .num_iters              ( num_iters                     ),
    .num_reads_per_iter     ( num_reads_per_iter            ),
    .base_address           ( read_address                  ),
    .valid_in               ( act_valid[i]                  ),
    .data_in                ( act_data[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH] ),
    .address_out            ( act_addr[((i+1)*LOG_MAX_ADDRESS)-1:i*LOG_MAX_ADDRESS]             ),
    .request                ( act_read[i]                   ),
    .avail_in               ( act_read2rd_avail_w[i]     ),
    .valid_out              ( act_read2rd_valid_w[i]     ),
    .data_out               ( act_read2rd_data_w[i]      )
  );
end
endgenerate  

// weight READ module
READ #(
  .DATA_WIDTH             ( NUM_LANES * DATA_WIDTH        ),
  .LOG_MAX_ITERS          ( LOG_MAX_ITERS                 ),
  .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER        ),
  .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS               ),
  .TYPE                   ( "weights"                     )
) weight_read_m (
  .clk                    ( clk                           ),
  .rst                    ( rst                           ),
  .configure              ( configure                     ),
  .num_iters              ( 8'b00000001                   ),   // weights are read, one per iteration (thus, only one iteration)
  .num_reads_per_iter     ( num_iters                     ),
  .base_address           ( read_address                  ),
  .valid_in               ( weight_valid                  ),
  .data_in                ( weight_data                   ),
  .address_out            ( weight_addr                   ),
  .request                ( weight_read                   ),
  .avail_in               ( weight_read2distr_avail_w     ),
  .valid_out              ( weight_read2distr_valid_w     ),
  .data_out               ( weight_read2distr_data_w      )
);


// repetition detector modules
generate
for ( i=0; i<NUM_INPUTS; i=i+1) begin 
  repetition_detector_nb #(
    .HIGH_MODE              ( HIGH_MODE                     ),
    .LOW_MODE               ( LOW_MODE                      ),
    .GROUP_SIZE             ( GROUP_SIZE                    ),
    .LOG_GS                 ( LOG_GS                        ),
    .DATA_WIDTH             ( DATA_WIDTH                    ),
    .LOG_MAX_ITERS          ( LOG_MAX_ITERS                 ),
    .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER        )
  ) repetition_detector_m (
    .clk                    ( clk                           ),
    .rst                    ( rst                           ),
    .configure              ( configure                     ),
    .num_iters              ( num_iters                     ),
    .num_reads_per_iter     ( num_reads_per_iter            ),
    .valid_in               ( act_read2rd_valid_w[i]        ),
    .data_in                ( act_read2rd_data_w[i]         ),
    .data_high_out          ( act_rd2distr_data_high_w[i]   ),
    .rdata_high_out         ( act_rd2distr_rdata_high_w[i]  ),
    .valid_high_out         ( act_rd2distr_valid_high_w[i]  ),
    .avail_in_high          ( act_rd2distr_avail_high_w[i]       ),
    .data_low_out           ( act_rd2distr_data_low_w[i]    ),
    .rdata_low_out          ( act_rd2distr_rdata_low_w[i]   ),
    .valid_low_out          ( act_rd2distr_valid_low_w[i]   ),
    .avail_in_low           ( act_rd2distr_avail_low_w[i]       ),
    .avail_out              ( act_read2rd_avail_w[i]        )
  );
end
endgenerate  

/**************************high*************************/

// DISTRIBUTE_IN module
DISTRIBUTE_IN_RD #(
 .NUM_DATA_INPUTS        ( NUM_INPUTS             ),
 .GROUP_SIZE             ( GROUP_SIZE             ),
 .DATA_WIDTH             ( DATA_WIDTH             ),
 .ACT_WIDTH              ( ACT_WIDTH              ),
 .NUM_DATA_OUTPUTS       ( NUM_LANES              ),
 .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
 .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER ),
 .REP_INFO               ( REP_INFO           )
) distribute_in_high_m (
 .clk                    ( clk                                ),
 .rst                    ( rst                                ),
 .configure              ( configure                          ),
 .conf_mode              ( conf_mode_in                       ),
 .num_iters              ( num_iters                          ),
 .num_reads_per_iter     ( num_reads_per_iter                 ), 
 .act_data_in            ( act_rd2distr_combined_data_high_w  ),
 .act_rdata_in           ( act_rd2distr_combined_rdata_high_w ),
 .act_valid_in           ( act_rd2distr_combined_valid_high_w ),
 .act_avail_out          ( act_rd2distr_avail_high_w          ),
 .weights_data_in        ( weight_read2distr_data_w           ),
 .weights_valid_in       ( weight_read2distr_valid_w          ),
 .weights_avail_out      ( weight_read2distr_avail_w          ),
 .data_out               ( act_distr2mul_data_high_w          ),
 .rdata_out              ( act_distr2mul_rdata_high_w         ),
 .valid_out              ( act_distr2mul_valid_high_w         ),
 .avail_in               ( act_distr2mul_avail_high_w         ),
 .weights_data_out       ( weight_distr2mul_data_high_w       ),
 .weights_valid_out      ( weight_distr2mul_valid_high_w      ),
 .weights_avail_in       ( weight_distr2mul_avail_high_w      )
);

// MUL, ALIGN  modules
generate
  for (i=0; i<NUM_LANES; i=i+1) begin
    MUL_RD #(
      .GROUP_SIZE             ( GROUP_SIZE             ),
      .DATA_WIDTH             ( DATA_WIDTH             ),
      .ACT_WIDTH              ( ACT_WIDTH              ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER ),
      .REP_INFO               ( REP_INFO           )
    ) mul_high_m (
      .clk                    ( clk                   ),
      .rst                    ( rst                   ),
      .configure              ( configure             ),
      .num_iters              ( num_iters             ),
      .num_reads_per_iter     ( num_reads_per_iter    ),
      .act_data_in            ( {act_distr2mul_rdata_high_w[((i+1)*REP_INFO)-1:i*REP_INFO], act_distr2mul_data_high_w[((i+1)*4)-1:i*4]} ),
      .act_valid_in           ( act_distr2mul_valid_high_w[i]                                                      ),
      .act_avail_out          ( act_distr2mul_avail_high_w[i]                                                      ),
      .weight_data_in         ( weight_distr2mul_data_high_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH]                    ),
      .weight_valid_in        ( weight_distr2mul_valid_high_w[i]                                                   ),
      .weight_avail_out       ( weight_distr2mul_avail_high_w[i]                                                   ),
      .data_out               ( mul2align_data_high_w[i]                                                           ),
      .valid_out              ( mul2align_valid_high_w[i]                                                          ),
      .avail_in               ( mul2align_avail_high_w[i]                                                          )
    );

    ALIGN #(
      .GROUP_SIZE             ( 1                              ),
      .DATA_WIDTH             ( REP_INFO + ACT_WIDTH + DATA_WIDTH ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS                  ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER         )
  ) align_high_m (
    .clk                    ( clk                       ),
    .rst                    ( rst                       ),
    .data_in                ( mul2align_data_high_w[i]  ),
    .valid_in               ( mul2align_valid_high_w[i] ),
    .avail_out              ( mul2align_avail_high_w[i] ),
    .data_out               ( align2acc_data_high_w[i]  ),
    .valid_out              ( align2acc_valid_high_w[i] ),
    .avail_in               ( align2acc_avail_high_w[i] )
  );  
  end
endgenerate

/**************************low*************************/

// DISTRIBUTE_IN module
DISTRIBUTE_IN_RD #(
 .NUM_DATA_INPUTS        ( NUM_INPUTS             ),
 .GROUP_SIZE             ( GROUP_SIZE             ),
 .DATA_WIDTH             ( DATA_WIDTH             ),
 .ACT_WIDTH              ( ACT_WIDTH              ),
 .NUM_DATA_OUTPUTS       ( NUM_LANES              ),
 .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
 .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER ),
 .REP_INFO               ( REP_INFO           )
) distribute_in_low_m (
 .clk                    ( clk                                ),
 .rst                    ( rst                                ),
 .configure              ( configure                          ),
 .conf_mode              ( conf_mode_in                       ),
 .num_iters              ( num_iters                          ),
 .num_reads_per_iter     ( num_reads_per_iter                 ),
 .act_data_in            ( act_rd2distr_combined_data_low_w   ),
 .act_rdata_in           ( act_rd2distr_combined_rdata_low_w  ),
 .act_valid_in           ( act_rd2distr_combined_valid_low_w  ),
 .act_avail_out          ( act_rd2distr_avail_low_w           ),
 .weights_data_in        ( weight_read2distr_data_w           ),
 .weights_valid_in       ( weight_read2distr_valid_w          ),
 .weights_avail_out      ( weight_read2distr_avail_w          ),
 .data_out               ( act_distr2mul_data_low_w           ),
 .rdata_out              ( act_distr2mul_rdata_low_w          ),
 .valid_out              ( act_distr2mul_valid_low_w          ),
 .avail_in               ( act_distr2mul_avail_low_w          ),
 .weights_data_out       ( weight_distr2mul_data_low_w        ),
 .weights_valid_out      ( weight_distr2mul_valid_low_w       ),
 .weights_avail_in       ( weight_distr2mul_avail_low_w       )
);


// MUL, ALIGN  modules
generate
  for (i=0; i<NUM_LANES; i=i+1) begin
    MUL_RD #(
      .GROUP_SIZE             ( GROUP_SIZE             ),
      .DATA_WIDTH             ( DATA_WIDTH             ),
      .ACT_WIDTH         (   4                      ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER ),
      .REP_INFO               ( REP_INFO          )
    ) mul_low_m (
      .clk                    ( clk                   ),
      .rst                    ( rst                   ),
      .configure              ( configure             ),
      .num_iters              ( num_iters             ),
      .num_reads_per_iter     ( num_reads_per_iter    ),
      .act_data_in            ( {act_distr2mul_rdata_low_w[((i+1)*REP_INFO)-1:i*REP_INFO], act_distr2mul_data_low_w[(i+1)*4-1:i*4]} ),
      .act_valid_in           ( act_distr2mul_valid_low_w[i]                                                      ),
      .act_avail_out          ( act_distr2mul_avail_low_w[i]                                                      ),
      .weight_data_in         ( weight_distr2mul_data_low_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH]                    ),
      .weight_valid_in        ( weight_distr2mul_valid_low_w[i]                                                   ),
      .weight_avail_out       ( weight_distr2mul_avail_low_w[i]                                                   ),
      .data_out               ( mul2align_data_low_w[i]                                                           ),
      .valid_out              ( mul2align_valid_low_w[i]                                                          ),
      .avail_in               ( mul2align_avail_low_w[i]                                                          )
    );

    ALIGN #(
      .GROUP_SIZE             ( 1                              ),
      .DATA_WIDTH             ( REP_INFO + 4 + DATA_WIDTH ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS                  ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER         )
  ) align_low_m (
    .clk                    ( clk                      ),
    .rst                    ( rst                      ),
    .data_in                ( mul2align_data_low_w[i]  ),
    .valid_in               ( mul2align_valid_low_w[i] ),
    .avail_out              ( mul2align_avail_low_w[i] ),
    .data_out               ( align2acc_data_low_w[i]  ),
    .valid_out              ( align2acc_valid_low_w[i] ),
    .avail_in               ( align2acc_avail_low_w[i] )
  );  
  end
endgenerate

/***********END SPLIT PATHS****************************/

//TODO ACC join
generate
  for (i=0; i<NUM_LANES; i=i+1) begin //TODO ACC  join
    ACC_RD_NB #(
      .HIGH_MODE              ( HIGH_MODE              ),
      .LOW_MODE               ( LOW_MODE               ),
      .GROUP_SIZE             ( GROUP_SIZE             ),
      .LOG_GS                 ( LOG_GS                 ),
      .DATA_WIDTH             ( ACT_WIDTH+DATA_WIDTH   ),
      .OUT_DATA_WIDTH         ( 2*DATA_WIDTH           ),
      .NUM_ADDRESSES          ( NUM_ADDRESSES          ),
      .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS        ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
    ) acc_m (
      .clk                    ( clk                       ),
      .rst                    ( rst                       ),
      .configure              ( configure                 ),
      .num_iters              ( num_iters                 ),
      .num_reads_per_iter     ( num_reads_per_iter        ),
      .data_in_high           ( align2acc_data_high_w[i]  ),
      .valid_in_high          ( align2acc_valid_high_w[i] ),
      .avail_out_high         ( align2acc_avail_high_w[i] ),
      .data_in_low            ( align2acc_data_low_w[i]   ),
      .valid_in_low           ( align2acc_valid_low_w[i]  ),
      .avail_out_low          ( align2acc_avail_low_w[i]  ),      
      .data_out               ( acc2distr_data_w[i]       ),
      .valid_out              ( acc2distr_valid_w[i]      ),
      .avail_in               ( acc2distr_avail_w[i]      )
    );
  end    
endgenerate
// distribute_out module
DISTRIBUTE_OUT #(
 .NUM_DATA_INPUTS        ( NUM_LANES                 ),
 .GROUP_SIZE             ( GROUP_SIZE                ),
 .DATA_WIDTH             ( 2*DATA_WIDTH              ),
 .NUM_DATA_OUTPUTS       ( NUM_OUTPUTS               )
) distribute_out_m (
 .clk                    ( clk                       ),
 .rst                    ( rst                       ),
 .configure              ( configure                 ),
 .conf_mode              ( conf_mode_out             ),
 .data_in                ( acc2distr_combined_data_w ),
 .valid_in               ( acc2distr_valid_w         ),
 .avail_out              ( acc2distr_avail_w         ),
 .data_out               ( distr2write_data_w        ),
 .valid_out              ( distr2write_valid_w       ),
 .avail_in               ( distr2write_avail_w       )
);

// WRITE modules (these modules are directly connected to external BRAMs
generate
  for ( i=0; i<NUM_OUTPUTS; i=i+1) begin
    WRITE #(
      .GROUP_SIZE             ( GROUP_SIZE             ),
      .DATA_WIDTH             ( 2*DATA_WIDTH           ),
      .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS        )
    ) write_mem_m (
      .clk                    ( clk                                                                             ),
      .rst                    ( rst                                                                             ),
      .configure              ( configure                                                                       ),
      .base_address           ( write_address                                                                   ),
      .min_clip               ( min_clip                                                                        ),
      .max_clip               ( max_clip                                                                        ),
      .data_in                ( distr2write_data_w[((i+1)*2*GROUP_SIZE*DATA_WIDTH)-1:i*2*GROUP_SIZE*DATA_WIDTH] ),
      .valid_in               ( distr2write_valid_w[i] ),
      .avail_out              ( distr2write_avail_w[i] ),
      .data_out               ( write2mem_data_w[i]    ),
      .address_out            ( write2mem_addr_w[i]    ),
      .valid_out              ( write2mem_valid_w[i]   )
    );
  end
endgenerate

endmodule