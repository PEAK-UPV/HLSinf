//
// This is the testbench for RTLinf accelerator. The accelerator currently has two operational modes.
//
// Mode 0 where 1 input is read, broadcasted to all lanes, and all lanes then reduced to one single output
// Mode 1 where n inputs are read in parallel, each input is forwarded to a lane and each lane forwards data to an output (inputs must be equal to lanes and outputs)
//
// IMPORTANT: The module is implemented with customized RAM memories. Current memory supported configurations are 4Kx32, 4Kx64 and 4Kx72. This
// means the following configurations of the module are possible:
//   - NUM_ADDRESSES 4096, DATA_WIDTH 8, GROUP_SIZE 4, NUM_INPUTS 9, NUM_LANES 9, NUM_OUTPUTS 9
//
// Any other configuration needing a different memory configuration is not currently supported

`define LOG_MAX_ITERS          8     // LOG_MAX_ITERS and LOG_MAX_READS_PER_ITER must be equal (to avoid data lenght problems when reading weights)
`define LOG_MAX_READS_PER_ITER 8
`define NUM_ADDRESSES          4096
`define LOG_MAX_ADDRESS        12
`define DATA_WIDTH             8
`define GROUP_SIZE             4
`define NUM_INPUTS             9
`define NUM_LANES              9
`define NUM_OUTPUTS            9
`define NUM_ITERS              2
`define NUM_OPS_PER_ITER       4

module RTLinf_tb;

  // inputs
  reg                                 clk_r;
  reg                                 rst_r;
  reg                                 configure_r;
  reg [`LOG_MAX_ITERS-1:0]            num_iters_r;
  reg [`LOG_MAX_READS_PER_ITER-1:0]   num_reads_per_iter_r;
  reg [`LOG_MAX_ADDRESS-1:0]          read_address_r;
  reg [`LOG_MAX_ADDRESS-1:0]          write_address_r;
  reg                                 conf_mode_in_r;
  reg                                 conf_mode_out_r;
  reg [`DATA_WIDTH-1:0]               min_clip_r;
  reg [`DATA_WIDTH-1:0]               max_clip_r;
  // outputs
  wire [`NUM_OUTPUTS*`GROUP_SIZE*`DATA_WIDTH-1:0] data_w;
  wire [`NUM_OUTPUTS*`LOG_MAX_ADDRESS-1:0]        addr_w;
  wire [`NUM_OUTPUTS-1:0]                         valid_w;

  // wires between activation memories and RTLinf  
  wire [`NUM_INPUTS*`GROUP_SIZE*`DATA_WIDTH-1:0] act_data_w;
  wire [`NUM_INPUTS*`LOG_MAX_ADDRESS-1:0]        act_addr_w;
  wire [`NUM_INPUTS-1:0]                         act_read_w;
  wire [`NUM_INPUTS-1:0]                         act_valid_w;
  
  // wires between  weight memory and RTLinf  
  wire [`NUM_LANES*`DATA_WIDTH-1:0]              weight_data_w;
  wire [`LOG_MAX_ADDRESS-1:0]                    weight_addr_w;
  wire                                           weight_read_w;
  wire                                           weight_valid_w;
      
  RTLinf #(
    .GROUP_SIZE             ( `GROUP_SIZE             ),
    .DATA_WIDTH             ( `DATA_WIDTH             ),
    .NUM_INPUTS             ( `NUM_INPUTS             ),
    .NUM_LANES              ( `NUM_LANES              ),
    .NUM_OUTPUTS            ( `NUM_OUTPUTS            ),
    .LOG_MAX_ITERS          ( `LOG_MAX_ITERS          ),
    .LOG_MAX_READS_PER_ITER ( `LOG_MAX_READS_PER_ITER ),
    .LOG_MAX_ADDRESS        ( `LOG_MAX_ADDRESS        ),
    .NUM_ADDRESSES          ( `NUM_ADDRESSES          )

  ) RTLinf_m (
    .clk                    ( clk_r                   ),
    .rst                    ( rst_r                   ),
    
    .act_read               ( act_read_w              ), // module input (act)
    .act_addr               ( act_addr_w              ), // module input (act)
    .act_data               ( act_data_w              ), // module input (act)
    .act_valid              ( act_valid_w             ), // module input (act)
    
    .weight_read            ( weight_read_w           ), // module input (weight)
    .weight_addr            ( weight_addr_w           ), // module input (weight)
    .weight_data            ( weight_data_w           ), // module input (weight)
    .weight_valid           ( weight_valid_w          ), // module input (weight)
    
    .configure              ( configure_r             ),
    .num_iters              ( num_iters_r             ),
    .num_reads_per_iter     ( num_reads_per_iter_r    ),
    .read_address           ( read_address_r          ),
    .write_address          ( write_address_r         ),
    .min_clip               ( min_clip_r              ),
    .max_clip               ( max_clip_r              ),
    .conf_mode_in           ( conf_mode_in_r          ),
    .conf_mode_out          ( conf_mode_out_r         ),
    .data_out               ( data_w                  ),  // module output
    .addr_out               ( addr_w                  ),  // module output
    .valid_out              ( valid_w                 )   // module output
  );
  
  // memories
  genvar i;
  
  // input actvivation memories
  generate
    for (i=0; i<`NUM_INPUTS; i=i+1) begin
      MEM #(
        .DATA_WIDTH      ( `GROUP_SIZE * `DATA_WIDTH ),
        .NUM_ADDRESSES   ( `NUM_ADDRESSES            ),
        .LOG_MAX_ADDRESS ( `LOG_MAX_ADDRESS          )
      ) act_mem_m (
         .clk            ( clk_r                     ),
         .rst            ( rst_r                     ),
         .data_write     ( 32'b0                     ),
         .addr_write     ( 12'b0                     ),
         .write          ( 1'b0                      ),
         .addr_read      ( act_addr_w[((i+1)*`LOG_MAX_ADDRESS)-1:i*`LOG_MAX_ADDRESS]               ),
         .read           ( act_read_w[i]                                                           ),
         .data_read      ( act_data_w[((i+1)*`GROUP_SIZE*`DATA_WIDTH)-1:i*`GROUP_SIZE*`DATA_WIDTH] ),
         .valid_out      ( act_valid_w[i]                                                          )
       );
     end
   endgenerate
   
   // input weight memory
   MEM #(
     .DATA_WIDTH      ( `NUM_LANES * `DATA_WIDTH ),
     .NUM_ADDRESSES   ( `NUM_ADDRESSES           ),
     .LOG_MAX_ADDRESS ( `LOG_MAX_ADDRESS         )
   ) weight_mem_m (
     .clk             ( clk_r                     ),
     .rst             ( rst_r                     ),
     .data_write      ( 0                         ),
     .addr_write      ( 0                         ),
     .write           ( 1'b0                      ),
     .addr_read       ( weight_addr_w             ),
     .read            ( weight_read_w             ),
     .data_read       ( weight_data_w             ),
     .valid_out       ( weight_valid_w            )
    );
  
  // output memories
  generate
    for (i=0; i<`NUM_OUTPUTS; i=i+1) begin
    MEM #(
      .DATA_WIDTH      ( `GROUP_SIZE * `DATA_WIDTH   ),
      .NUM_ADDRESSES   ( `NUM_ADDRESSES              ),
      .LOG_MAX_ADDRESS ( `LOG_MAX_ADDRESS            )
    ) output_mem_m (
      .clk             ( clk_r                       ),
      .rst             ( rst_r                       ),
      .data_write      ( data_w[((i+1)*`GROUP_SIZE * `DATA_WIDTH)-1:i*`GROUP_SIZE*`DATA_WIDTH]  ),
      .addr_write      ( addr_w[((i+1)*`LOG_MAX_ADDRESS)-1:i*`LOG_MAX_ADDRESS]                  ),
      .write           ( valid_w[i]                                                             ),
      .addr_read       ( 1'b0                 ),
      .read            ( 1'b0                 ),
      .data_read       (                      ),
      .valid_out       (                      )
    );    
    end
  endgenerate

always #5 clk_r <= ~clk_r;

integer j;

initial begin
  rst_r <= 1;
  clk_r <= 0;
  configure_r          <= 0;
  num_iters_r          <= `NUM_ITERS;
  num_reads_per_iter_r <= `NUM_OPS_PER_ITER;
  read_address_r       <= 0;
  write_address_r      <= 0;
  conf_mode_in_r       <= 1; //0;  // input 0 broadcasted to all lanes
  conf_mode_out_r      <= 1; //0;  // all inputs added and sent to all outputs
  min_clip_r           <= 0;
  max_clip_r           <= 255;
  
  #20 
  rst_r <= 0;

  #20 
  rst_r <= 1;

  $display("GROUP_SIZE %d NUM_INPUTS %d NUM_LANES %d NUM_OUTPUTS %d NUM_ITERS %d NUM_OPS_PER_ITER %d", `GROUP_SIZE, `NUM_INPUTS, `NUM_LANES, `NUM_OUTPUTS, `NUM_ITERS, `NUM_OPS_PER_ITER);

  #40 
  configure_r <= 1;

  #10
  configure_r <= 0;


end

endmodule






