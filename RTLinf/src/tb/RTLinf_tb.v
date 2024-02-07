//
// TB_2: Testbench for n BRAM/READ activation modules plus one BRAM/READ weight modules and one distribute module.
// The distribute module has n lanes. Distribute conf mode set to 1 (input i goes to output i)
// n can be instantiated by the NUM_INPUTS/NUM_LANES defines below
//

`define LOG_MAX_ITERS          8     // LOG_MAX_ITERS and LOG_MAX_READS_PER_ITER must be equal (to avoid data lenght problems when reading weights)
`define LOG_MAX_READS_PER_ITER 8
`define NUM_ADDRESSES          4096
`define LOG_MAX_ADDRESS        12
`define DATA_WIDTH             8
`define GROUP_SIZE             4
`define NUM_INPUTS             1
`define NUM_LANES              1
`define NUM_OUTPUTS            1
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

  ) distribute_in (
    .clk                    ( clk_r                   ),
    .rst                    ( rst_r                   ),
    .configure              ( configure_r             ),
    .num_iters              ( num_iters_r             ),
    .num_reads_per_iter     ( num_reads_per_iter_r    ),
    .read_address           ( read_address_r          ),
    .write_address          ( write_address_r         ),
    .min_clip               ( min_clip_r              ),
    .max_clip               ( max_clip_r              ),
    .conf_mode_in           ( conf_mode_in_r          ),
    .conf_mode_out          ( conf_mode_out_r         )
  );

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
  conf_mode_in_r       <= 1;
  conf_mode_out_r      <= 1;
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






