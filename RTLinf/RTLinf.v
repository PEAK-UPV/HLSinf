// Module RTLinf
//
// This module implements the whole RTLinf accelerator
//
//

module RTLinf #(
  parameter GROUP_SIZE  = 2,             // group size
  parameter ACTIVATION_WIDTH = 8,        // activation width in bits
  parameter NUM_INPUTS  = 1,             // number of inputs
  parameter NUM_LANES   = 9,             // number of lanes
  parameter NUM_OUTPUTS = 1,             // number of outputs
  parameter LOG_MAX_ITERS   = 16,        // number of bits for max iters
  parameter LOG_MAX_READS_PER_ITER = 16, // number of bits for reads_per_iter
  parameter LOG_MAX_ADDRESS = 16         // number of bits for addresses
)(
  input clk,                                       // clock input
  input rst,                                       // reset input

  input [LOG_MAX_ITERS-1:0]               num_iters,
  input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter,
  input [LOG_MAX_ADDRESS*NUM_INPUTS-1:0]  read_address

);

// genvars
genvar x;

// wires

// registers

// combinational logic

// activation memories
generate
for ( x=0; x<NUM_INPUTS; x=x+1) begin
  MEM #(
    .DATA_WIDTH      ( GROUP_SIZE * ACTIVATION_WIDTH ),
    .LOG_MAX_ADDRESS ( LOG_MAX_ADDRESS               )
  ) act_mem_m (
    .clk             (),
    .data_write      (),
    .addr_write      (),
    .write           (),
    .addr_read       (),
    .read            (),
    .datsa_read      ()
  );
end
endgenerate

// activation read modules
generate
for (x=0; x<NUM_INPUTS; x=x+1) begin
    READ #(
      .DATA_WIDTH             ( GROUP_SIZE * ACTIVATION_WIDTH ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS                 ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER        ),
      .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS               )
    ) act_read_m (
      .clk                    (),
      .rst                    (),
      .configure              (),
      .num_iters              (),
      .num_reads_per_iter     (),
      .base_address           (),
      .valid_in               (),
      .data_in                (),
      .address_out            (),
      .request                (),
      .avail_in               (),
      .valid_out              (),
      .data_out               ()
    );
end
endgenerate  

// weights memory
MEM #(
  .DATA_WIDTH      ( GROUP_SIZE * ACTIVATION_WIDTH ),
  .LOG_MAX_ADDRESS ( LOG_MAX_ADDRESS               )
) weights_mem (
  .clk             (),
  .data_write      (),
  .addr_write      (),
  .write           (),
  .addr_read       (),
  .read            (),
  .datsa_read      ()
);

// weight read module
READ #(
  .DATA_WIDTH             ( GROUP_SIZE * ACTIVATION_WIDTH ),
  .LOG_MAX_ITERS          ( LOG_MAX_ITERS                 ),
  .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER        ),
  .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS               )
) weights_read_m (
  .clk                    (),
  .rst                    (),
  .configure              (),
  .num_iters              (),
  .num_reads_per_iter     (),
  .base_address           (),
  .valid_in               (),
  .data_in                (),
  .address_out            (),
  .request                (),
  .avail_in               (),
  .valid_out              (),
  .data_out               ()
);

// distribute_in module
DISTRIBUTE_IN #(
 .NUM_DATA_INPUTS        (),
 .DATA_WIDTH             (),
 .NUM_DATA_OUTPUTS       (),
 .LOG_MAX_ITERS          (),
 .LOG_MAX_READS_PER_ITER ()
) distribute_in_m (
 .clk                    (),
 .rst                    (),
 .configure              (),
 .conf_mode              (),
 .num_iters              (),
 .num_reads_per_iter     (),
 .act_data_in            (),
 .act_valid_in           (),
 .act_avail_out          (),
 .weights_data_in        (),
 .weights_valid_in       (),
 .weights_avail_out      (),
 .data_out               (),
 .valid_out              (),
 .avail_in               ()
);

// muls, align, acc
generate
  for (x=0; x<NUM_LANES; x=x+1) begin
    MUL #(
      .NUM_INPUTS             (),
      .DATA_WIDTH             (),
      .LOG_MAX_ITERS          (),
      .LOG_MAX_READS_PER_ITER ()
    ) mul_m (
      .clk                    (),
      .rst                    (),
      .configure              (),
      .num_iters              (),
      .num_reads_per_iter     (),
      .act_data_in            (),
      .act_valid_in           (),
      .act_avail_out          (),
      .weight_data_in         (),
      .weight_valid_in        (),
      .weight_avail_out       (),
      .data_out               (),
      .valid_out              (),
      .avail_in               ()
    );

  ALIGN #(
    .NUM_INPUTS             (),
    .DATA_WIDTH             (),
    .LOG_MAX_ITERS          (),
    .LOG_MAX_READS_PER_ITER ()
  ) align_m (
    .clk                    (),
    .rst                    (),
    .configure              (),
    .num_iters              (),
    .num_reads_per_iter     (),
    .data_in                (),
    .valid_in               (),
    .avail_out              (),
    .data_out               (),
    .valid_out              (),
    .avail_in               ()
  );  

  ACC #(
    .NUM_INPUTS             (),
    .DATA_WIDTH             (),
    .LOG_MAX_ITERS          (),
    .LOG_MAX_READS_PER_ITER ()
  ) acc_m (
    .clk                    (),
    .rst                    (),
    .configure              (),
    .num_iters              (),
    .num_reads_per_iter     (),
    .data_in                (),
    .valid_in               (),
    .avail_out              (),
    .data_out               (),
    .valid_out              (),
    .avail_in               ()
  );
  end    
endgenerate

// distribute_out module
DISTRIBUTE_OUT #(
 .NUM_DATA_INPUTS        (),
 .DATA_WIDTH             (),
 .NUM_DATA_OUTPUTS       (),
 .LOG_MAX_ITERS          (),
 .LOG_MAX_READS_PER_ITER ()
) distribute_out_m (
 .clk                    (),
 .rst                    (),
 .configure              (),
 .conf_mode              (),
 .num_iters              (),
 .num_reads_per_iter     (),
 .data_in                (),
 .valid_in               (),
 .avail_out              (),
 .data_out               (),
 .valid_out              (),
 .avail_in               ()
);

// write modules
generate
  for ( x=0; x<NUM_OUTPUTS; x=x+1) begin
    WRITE #(
      .NUM_INPUTS             (),
      .INPUT_DATA_WIDTH       (),
      .OUTPUT_DATA_WIDTH      (),
      .LOG_MAX_ITERS          (),
      .LOG_MAX_READS_PER_ITER (),
      .LOG_MAX_ADDRESS        ()
    ) write_mem_m (
      .clk                    (),
      .rst                    (),
      .configure              (),
      .num_iters              (),
      .num_reads_per_iter     (),
      .base_address           (),
      .min_clip               (),
      .max_clip               (),
      .data_in                (),
      .valid_in               (),
      .avail_out              (),
      .data_out               (),
      .address_out            (),
      .valid_out              ()
    );
  end
endgenerate
  
// output memories
generate
  for ( x=0; x<NUM_OUTPUTS; x=x+1) begin
    MEM #(
      .DATA_WIDTH      (),
      .LOG_MAX_ADDRESS ()
    ) output_mem_m (
      .clk             (),
      .data_write      (),
      .addr_write      (),
      .write           (),
      .addr_read       (),
      .read            (),
      .datsa_read      ()
    );
  end
endgenerate


// sequential logic

endmodule