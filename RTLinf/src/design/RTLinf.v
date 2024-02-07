// Module RTLinf
//
// This module implements the whole RTLinf accelerator
//
//

module RTLinf #(
  parameter GROUP_SIZE             = 4,    // group size
  parameter DATA_WIDTH             = 8,    // data width
  parameter NUM_INPUTS             = 9,    // number of inputs
  parameter NUM_LANES              = 9,    // number of lanes
  parameter NUM_OUTPUTS            = 9,    // number of outputs
  parameter LOG_MAX_ITERS          = 16,   // number of bits for max iters
  parameter LOG_MAX_READS_PER_ITER = 16,   // number of bits for reads_per_iter
  parameter LOG_MAX_ADDRESS        = 16,   // number of bits for addresses
  parameter NUM_ADDRESSES          = 65536 // number of addresses in memories
)(
  input                                   clk,                // clock input
  input                                   rst,                // reset input

  input                                   configure,          // configure signal
  input [LOG_MAX_ITERS-1:0]               num_iters,          // num iterations
  input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter, // num reads per iteration
  input [LOG_MAX_ADDRESS-1:0]             read_address,       // read address
  input [LOG_MAX_ADDRESS-1:0]             write_address,      // write address
  input [DATA_WIDTH-1:0]                  min_clip,           // min cliping value
  input [DATA_WIDTH-1:0]                  max_clip,           // max cliping value
  input                                   conf_mode_in,       // conf mode for distribute_in module
  input                                   conf_mode_out,      // conf mode for distribute_out module
  
  output [NUM_OUTPUTS*GROUP_SIZE*DATA_WIDTH-1:0] data,        // data output to BRAM memories
  output [NUM_OUTPUTS*LOG_MAX_ADDRESS-1:0]       addr,        // address output to BRAM memories
  output [NUM_OUTPUTS-1:0]                       valid        // valid signals to BRAM memories
);  

// genvars
genvar x;

// wires between MEM and READ modules (activations)
wire [NUM_INPUTS-1:0]             act_mem2read_read_w;
wire [LOG_MAX_ADDRESS-1:0]        act_mem2read_addr_w[NUM_INPUTS-1:0];
wire [GROUP_SIZE*DATA_WIDTH-1:0]  act_mem2read_data_w[NUM_INPUTS-1:0];
wire [NUM_INPUTS-1:0]             act_mem2read_valid_w[NUM_INPUTS-1:0];

// wires between MEM and READ modules (weights)
wire                              weight_mem2read_read_w;
wire [LOG_MAX_ADDRESS-1:0]        weight_mem2read_addr_w;
wire [NUM_LANES*DATA_WIDTH-1:0]   weight_mem2read_data_w;
wire                              weight_mem2read_valid_w;

// wires between READ and DISTRIBUTE_IN modules
wire [NUM_INPUTS-1:0]                       act_read2distr_valid_w;
wire [GROUP_SIZE*DATA_WIDTH-1:0]            act_read2distr_data_w[NUM_INPUTS-1:0];
wire [NUM_INPUTS-1:0]                       act_read2distr_avail_w;
wire                                        weight_read2distr_valid_w;
wire [NUM_LANES*DATA_WIDTH-1:0]             weight_read2distr_data_w;
wire                                        weight_read2distr_avail_w;
wire [NUM_INPUTS*GROUP_SIZE*DATA_WIDTH-1:0] act_read2distr_combined_data_w;
wire [NUM_INPUTS-1:0]                       act_read2distr_combined_valid_w;

// wires between DISTRIBUTE_IN and MUL modules
wire [NUM_LANES*GROUP_SIZE*DATA_WIDTH-1:0]  act_distr2mul_data_w;
wire [NUM_LANES-1:0]                        act_distr2mul_valid_w;
wire [NUM_LANES-1:0]                        act_distr2mul_avail_w; 
wire [NUM_LANES*DATA_WIDTH-1:0]             weight_distr2mul_data_w;
wire [NUM_LANES-1:0]                        weight_distr2mul_valid_w;
wire [NUM_LANES-1:0]                        weight_distr2mul_avail_w;

// wires between MUL and ALIGN modules
wire [2*GROUP_SIZE*DATA_WIDTH-1:0]          mul2align_data_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                        mul2align_valid_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                        mul2align_avail_w[NUM_LANES-1:0];

// wires between ALIGN and ACC modules
wire [2*GROUP_SIZE*DATA_WIDTH-1:0]          align2acc_data_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                        align2acc_valid_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                        align2acc_avail_w[NUM_LANES-1:0];

// wires between ACC and DISTRIBUTE_IN modules
wire [2*GROUP_SIZE*DATA_WIDTH-1:0]          acc2distr_data_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                        acc2distr_valid_w;
wire [NUM_LANES-1:0]                        acc2distr_avail_w;
wire [NUM_LANES*2*GROUP_SIZE*DATA_WIDTH-1:0]acc2distr_combined_data_w;

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
    assign data[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH] = write2mem_data_w[i];
    assign addr[((i+1)*LOG_MAX_ADDRESS)-1:i*LOG_MAX_ADDRESS] = write2mem_addr_w[i];
    assign valid[i] = write2mem_valid_w[i];
  end
 endgenerate

// combined data and valid signal between READ and DISTRIBUTE_IN modules
generate
  for (i=0; i<NUM_INPUTS; i=i+1) begin
    assign act_read2distr_combined_data_w[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH] = act_read2distr_data_w[i];
    assign act_read2distr_combined_valid_w[i] = act_read2distr_valid_w[i];
  end
endgenerate

// combined data between ACC and DISTRIBUTE_OUT modules
generate
  for (i=0; i<NUM_LANES; i=i+1) begin
    assign acc2distr_combined_data_w[((i+1)*2*GROUP_SIZE*DATA_WIDTH)-1:i*2*GROUP_SIZE*DATA_WIDTH] = acc2distr_data_w[i];
  end
endgenerate

// activation MEM and read modules
generate
for ( i=0; i<NUM_INPUTS; i=i+1) begin
  MEM #(
    .DATA_WIDTH      ( GROUP_SIZE * DATA_WIDTH ),
    .NUM_ADDRESSES   ( NUM_ADDRESSES           ),
    .LOG_MAX_ADDRESS ( LOG_MAX_ADDRESS         )
  ) act_mem_m (
    .clk             ( clk                     ),
    .rst             ( rst                     ),
    .data_write      ( 0                       ),
    .addr_write      ( 0                       ),
    .write           ( 0                       ),
    .addr_read       ( act_mem2read_addr_w[i]  ),
    .read            ( act_mem2read_read_w[i]  ),
    .data_read       ( act_mem2read_data_w[i]  ),
    .valid_out       ( act_mem2read_valid_w[i] )
  );
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
    .valid_in               ( act_mem2read_valid_w[i]       ),
    .data_in                ( act_mem2read_data_w[i]        ),
    .address_out            ( act_mem2read_addr_w[i]        ),
    .request                ( act_mem2read_read_w[i]        ),
    .avail_in               ( act_read2distr_avail_w[i]     ),
    .valid_out              ( act_read2distr_valid_w[i]     ),
    .data_out               ( act_read2distr_data_w[i]      )
  );
end
endgenerate  

// weight MEM and READ modules
MEM #(
  .DATA_WIDTH      ( NUM_LANES * DATA_WIDTH ),
  .NUM_ADDRESSES   ( NUM_ADDRESSES           ),
  .LOG_MAX_ADDRESS ( LOG_MAX_ADDRESS         )
) weight_mem_m (
  .clk             ( clk                     ),
  .rst             ( rst                     ),
  .data_write      ( 0                       ),
  .addr_write      ( 0                       ),
  .write           ( 0                       ),
  .addr_read       ( weight_mem2read_addr_w  ),
  .read            ( weight_mem2read_read_w  ),
  .data_read       ( weight_mem2read_data_w  ),
  .valid_out       ( weight_mem2read_valid_w )
);

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
  .num_iters              ( 1                             ),   // weights are read, one per iteration (thus, only one iteration)
  .num_reads_per_iter     ( num_iters                     ),
  .base_address           ( read_address                  ),
  .valid_in               ( weight_mem2read_valid_w       ),
  .data_in                ( weight_mem2read_data_w        ),
  .address_out            ( weight_mem2read_addr_w        ),
  .request                ( weight_mem2read_read_w        ),
  .avail_in               ( weight_read2distr_avail_w     ),
  .valid_out              ( weight_read2distr_valid_w     ),
  .data_out               ( weight_read2distr_data_w      )
);

// DISTRIBUTE_IN module
DISTRIBUTE_IN #(
 .NUM_DATA_INPUTS        ( NUM_INPUTS             ),
 .GROUP_SIZE             ( GROUP_SIZE             ),
 .DATA_WIDTH             ( DATA_WIDTH             ),
 .NUM_DATA_OUTPUTS       ( NUM_LANES              ),
 .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
 .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
) distribute_in_m (
 .clk                    ( clk                             ),
 .rst                    ( rst                             ),
 .configure              ( configure                       ),
 .conf_mode              ( conf_mode_in                    ),
 .num_iters              ( num_iters                       ),
 .num_reads_per_iter     ( num_reads_per_iter              ),
 .act_data_in            ( act_read2distr_combined_data_w  ),
 .act_valid_in           ( act_read2distr_combined_valid_w ),
 .act_avail_out          ( act_read2distr_avail_w          ),
 .weights_data_in        ( weight_read2distr_data_w        ),
 .weights_valid_in       ( weight_read2distr_valid_w       ),
 .weights_avail_out      ( weight_read2distr_avail_w       ),
 .data_out               ( act_distr2mul_data_w            ),
 .valid_out              ( act_distr2mul_valid_w           ),
 .avail_in               ( act_distr2mul_avail_w           ),
 .weights_data_out       ( weight_distr2mul_data_w         ),
 .weights_valid_out      ( weight_distr2mul_valid_w        ),
 .weights_avail_in       ( weight_distr2mul_avail_w        )
);

// MUL, ALIGN, ACC modules
generate
  for (i=0; i<NUM_LANES; i=i+1) begin
    MUL #(
      .GROUP_SIZE             ( GROUP_SIZE ),
      .DATA_WIDTH             ( DATA_WIDTH ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
    ) mul_m (
      .clk                    ( clk                  ),
      .rst                    ( rst                  ),
      .configure              ( configure            ),
      .num_iters              ( num_iters            ),
      .num_reads_per_iter     ( num_reads_per_iter   ),
      .act_data_in            ( act_distr2mul_data_w[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH] ),
      .act_valid_in           ( act_distr2mul_valid_w[i] ),
      .act_avail_out          ( act_distr2mul_avail_w[i] ),
      .weight_data_in         ( weight_distr2mul_data_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH] ),
      .weight_valid_in        ( weight_distr2mul_valid_w[i] ),
      .weight_avail_out       ( weight_distr2mul_avail_w[i] ),
      .data_out               ( mul2align_data_w[i]           ),
      .valid_out              ( mul2align_valid_w[i]          ),
      .avail_in               ( mul2align_avail_w[i]          )
    );

    ALIGN #(
      .GROUP_SIZE             ( GROUP_SIZE ),
      .DATA_WIDTH             ( 2*DATA_WIDTH ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
  ) align_m (
    .clk                    ( clk                  ),
    .rst                    ( rst                  ),
    .configure              ( configure            ),
    .num_iters              ( num_iters            ),
    .num_reads_per_iter     ( num_reads_per_iter   ),
    .data_in                ( mul2align_data_w[i]  ),
    .valid_in               ( mul2align_valid_w[i] ),
    .avail_out              ( mul2align_avail_w[i] ),
    .data_out               ( align2acc_data_w[i]  ),
    .valid_out              ( align2acc_valid_w[i] ),
    .avail_in               ( align2acc_avail_w[i] )
  );  

  ACC #(
    .GROUP_SIZE             ( GROUP_SIZE             ),
    .DATA_WIDTH             ( 2*DATA_WIDTH           ),
    .NUM_ADDRESSES          ( NUM_ADDRESSES          ),
    .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
    .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
  ) acc_m (
    .clk                    ( clk                    ),
    .rst                    ( rst                    ),
    .configure              ( configure              ),
    .num_iters              ( num_iters              ),
    .num_reads_per_iter     ( num_reads_per_iter     ),
    .data_in                ( align2acc_data_w[i]    ),
    .valid_in               ( align2acc_valid_w[i]   ),
    .avail_out              ( align2acc_avail_w[i]   ),
    .data_out               ( acc2distr_data_w[i]    ),
    .valid_out              ( acc2distr_valid_w[i]   ),
    .avail_in               ( acc2distr_avail_w[i]   )
  );
  end    
endgenerate

// distribute_out module
DISTRIBUTE_OUT #(
 .NUM_DATA_INPUTS        ( NUM_LANES                 ),
 .GROUP_SIZE             ( GROUP_SIZE                ),
 .DATA_WIDTH             ( 2*DATA_WIDTH              ),
 .NUM_DATA_OUTPUTS       ( NUM_OUTPUTS               ),
 .LOG_MAX_ITERS          ( LOG_MAX_ITERS             ),
 .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER    )
) distribute_out_m (
 .clk                    ( clk                       ),
 .rst                    ( rst                       ),
 .configure              ( configure                 ),
 .conf_mode              ( conf_mode_out             ),
 .num_iters              ( num_iters                 ),
 .num_reads_per_iter     ( num_reads_per_iter        ),
 .data_in                ( acc2distr_combined_data_w ),
 .valid_in               ( acc2distr_valid_w         ),
 .avail_out              ( acc2distr_avail_w         ),
 .data_out               ( distr2write_data_w        ),
 .valid_out              ( distr2write_valid_w       ),
 .avail_in               ( distr2write_avail_w       )
);

// WRITE and MEM  modules
generate
  for ( i=0; i<NUM_OUTPUTS; i=i+1) begin
    WRITE #(
      .GROUP_SIZE             ( GROUP_SIZE             ),
      .DATA_WIDTH             ( 2*DATA_WIDTH           ),
      .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS        ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER )
    ) write_mem_m (
      .clk                    ( clk                                                                             ),
      .rst                    ( rst                                                                             ),
      .configure              ( configure                                                                       ),
      .num_reads_per_iter     ( num_reads_per_iter                                                              ),
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
  
    MEM #(
      .DATA_WIDTH      ( GROUP_SIZE * DATA_WIDTH        ),
      .NUM_ADDRESSES   ( NUM_ADDRESSES     ),
      .LOG_MAX_ADDRESS ( LOG_MAX_ADDRESS   )
    ) output_mem_m (
      .clk             ( clk                  ),
      .rst             ( rst                  ),
      .data_write      ( write2mem_data_w[i]  ),
      .addr_write      ( write2mem_addr_w[i]  ),
      .write           ( write2mem_valid_w[i] ),
      .addr_read       ( 0                    ),
      .read            ( 0                    ),
      .data_read       (                      ),
      .valid_out       (                      )
    );
  end
endgenerate

endmodule
