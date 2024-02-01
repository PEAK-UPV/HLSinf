//
// TB_3: Testbench for n BRAM/READ activation modules, one BRAM/READ weight modules, one distribute module and one MUL module per lane.
// The distribute module has n lanes. Distribute conf mode set to 1 (input i goes to output i)
// n can be instantiated by the NUM_INPUTS/NUM_LANES defines below
//

`define LOG_MAX_ITERS          4
`define LOG_MAX_READS_PER_ITER 8
`define NUM_ADDRESSES          4096
`define LOG_MAX_ADDRESS        12
`define DATA_WIDTH             8
`define GROUP_SIZE             2
`define NUM_INPUTS             1
`define NUM_LANES              1
`define NUM_ITERS              2
`define NUM_OPS_PER_ITER       4

module tb_2;

  // inputs
  reg                                 clk_r;
  reg                                 rst_r;
  reg                                 configure_r;
  reg [`LOG_MAX_ITERS-1:0]            num_iters_r;
  reg [`LOG_MAX_READS_PER_ITER-1:0]   num_reads_per_iter_r;
  reg [`LOG_MAX_ADDRESS-1:0]          base_address_r;
  reg                                 conf_mode_r;
  
  reg [`NUM_LANES-1:0]                avail_in_r;
  reg [`NUM_LANES-1:0]                weight_avail_in_r;
  
  // wires between MEM and READ for activations
  wire                                 read_w[`NUM_INPUTS-1:0];
  wire [`LOG_MAX_ADDRESS-1:0]          addr_read_w[`NUM_INPUTS-1:0];
  wire [(`GROUP_SIZE*`DATA_WIDTH)-1:0] data_read_w[`NUM_INPUTS-1:0];
  wire                                 valid_in_w[`NUM_INPUTS-1:0];

  // wires beteen MEM and READ for weights
  wire                                  weight_read_w;
  wire [`LOG_MAX_ADDRESS-1:0]           weight_addr_read_w;
  wire [(`NUM_LANES * `DATA_WIDTH)-1:0] weight_data_read_w;
  wire                                  weight_valid_in_w;
 
  // wires between READ and DISTRIBUTE_IN
  wire                                 valid_out_w[`NUM_INPUTS-1:0];
  wire [(`GROUP_SIZE*`DATA_WIDTH)-1:0] data_out_w[`NUM_INPUTS-1:0];
  wire [`NUM_INPUTS-1:0]               avail_in_w;
  wire                                 weight_valid_out_w;
  wire [`NUM_LANES*`DATA_WIDTH-1:0]    weight_data_out_w;
  wire                                 weight_avail_in_w;
  // combined inputs between READ and DISTRIBUTE_IN
  wire [`NUM_INPUTS*`GROUP_SIZE*`DATA_WIDTH-1:0]  act_data_in_w;
  wire [`NUM_INPUTS-1:0]                          act_valid_in_w;

  genvar i;

  // combinational logic
  generate
    for (i=0; i<`NUM_INPUTS; i=i+1) begin
      assign act_data_in_w[((i+1)*`GROUP_SIZE*`DATA_WIDTH)-1:i*`GROUP_SIZE*`DATA_WIDTH] = data_out_w[i];
      assign act_valid_in_w[i] = valid_out_w[i];
    end
  endgenerate

  // distribute_in output wires
  wire [(`NUM_LANES * `GROUP_SIZE * `DATA_WIDTH) - 1 : 0] distribute_data_out_w;
  wire [`NUM_LANES-1:0]                                   distribute_valid_out_w;
  wire [`NUM_LANES * `DATA_WIDTH - 1 : 0]                 distribute_weight_data_out_w;
  wire [`NUM_LANES-1:0]                                   distribute_weight_valid_out_w;  

  // output wires for mul modules
  wire [2 * (`GROUP_SIZE * `DATA_WIDTH) - 1 : 0]          mul_data_out_w[`NUM_LANES-1:0];
  wire [`NUM_LANES-1:0]                                   mul_valid_out_w;

  // MEM and READ modules for activations
  generate
    for (i=0; i<`NUM_INPUTS; i=i+1) begin
      MEM #(
        .DATA_WIDTH        ( `GROUP_SIZE * `DATA_WIDTH      ),
        .NUM_ADDRESSES     ( `NUM_ADDRESSES   ),
        .LOG_MAX_ADDRESS   ( `LOG_MAX_ADDRESS )
      ) mem_activations (
        .clk               ( clk_r        ),
        .rst               ( rst_r        ),
        .data_write        ( ),
        .addr_write        ( ),
        .write             ( 1'b0            ),
        .addr_read         ( addr_read_w[i]  ),
        .read              ( read_w[i]       ),
        .data_read         ( data_read_w[i]  ),
        .valid_out         ( valid_in_w[i]   )
      );

      READ #(
        .DATA_WIDTH             ( `GROUP_SIZE * `DATA_WIDTH             ),
        .LOG_MAX_ITERS          ( `LOG_MAX_ITERS          ),
        .LOG_MAX_READS_PER_ITER ( `LOG_MAX_READS_PER_ITER ),
        .LOG_MAX_ADDRESS        ( `LOG_MAX_ADDRESS        )
      ) read_activations (
        .clk                    ( clk_r                   ),
        .rst                    ( rst_r                   ),
        .configure              ( configure_r             ),
        .num_iters              ( num_iters_r             ),
        .num_reads_per_iter     ( num_reads_per_iter_r    ),
        .base_address           ( base_address_r          ),
        .valid_in               ( valid_in_w[i]           ),
        .data_in                ( data_read_w[i]          ),
        .address_out            ( addr_read_w[i]          ),
        .request                ( read_w[i]               ),
        .avail_in               ( avail_in_w[i]           ),
        .valid_out              ( valid_out_w[i]          ),
        .data_out               ( data_out_w[i]           )
      );
    end
  endgenerate

  // MEM and READ modules for weights
  MEM #(
    .DATA_WIDTH        ( `NUM_LANES * `DATA_WIDTH             ),
    .NUM_ADDRESSES     ( `NUM_ADDRESSES          ),
    .LOG_MAX_ADDRESS   ( `LOG_MAX_ADDRESS        )
  ) mem_weights (
    .clk               ( clk_r                   ),
    .rst               ( rst_r                   ),
    .data_write        (             ),
    .addr_write        (             ),
    .write             ( 1'b0            ),
    .addr_read         ( weight_addr_read_w  ),
    .read              ( weight_read_w       ),
    .data_read         ( weight_data_read_w  ),
    .valid_out         ( weight_valid_in_w   )
  );

  READ #(
    .DATA_WIDTH             ( `NUM_LANES * `DATA_WIDTH             ),
    .LOG_MAX_ITERS          ( `LOG_MAX_ITERS          ),
    .LOG_MAX_READS_PER_ITER ( `LOG_MAX_READS_PER_ITER ),
    .LOG_MAX_ADDRESS        ( `LOG_MAX_ADDRESS        )
  ) read_weights (
    .clk                    ( clk_r                   ),
    .rst                    ( rst_r                   ),
    .configure              ( configure_r             ),
    .num_iters              ( num_iters_r             ),
    .num_reads_per_iter     ( num_reads_per_iter_r    ),
    .base_address           ( base_address_r          ),
    .valid_in               ( weight_valid_in_w      ),
    .data_in                ( weight_data_read_w     ),
    .address_out            ( weight_addr_read_w     ),
    .request                ( weight_read_w          ),
    .avail_in               ( weight_avail_in_w      ),
    .valid_out              ( weight_valid_out_w     ),
    .data_out               ( weight_data_out_w      )
  );

  // distribute module
  DISTRIBUTE_IN #(
    .NUM_DATA_INPUTS        ( `NUM_INPUTS             ),
    .GROUP_SIZE             ( `GROUP_SIZE             ),
    .DATA_WIDTH             ( `DATA_WIDTH             ),
    .NUM_DATA_OUTPUTS       ( `NUM_LANES              ),
    .LOG_MAX_ITERS          ( `LOG_MAX_ITERS          ),
    .LOG_MAX_READS_PER_ITER ( `LOG_MAX_READS_PER_ITER )
  ) distribute_in (
    .clk                    ( clk_r                   ),
    .rst                    ( rst_r                   ),
    .configure              ( configure_r             ),
    .conf_mode              ( conf_mode_r             ),
    .num_iters              ( num_iters_r             ),
    .num_reads_per_iter     ( num_reads_per_iter_r    ),
    .act_data_in            ( act_data_in_w           ),
    .act_valid_in           ( act_valid_in_w          ),
    .act_avail_out          ( avail_in_w              ),
    .weights_data_in        ( weight_data_out_w      ),
    .weights_valid_in       ( weight_valid_out_w     ),
    .weights_avail_out      ( weight_avail_in_w      ),
    .data_out               ( distribute_data_out_w              ),
    .valid_out              ( distribute_valid_out_w             ),
    .avail_in               ( avail_in_r              ),
    .weights_data_out       ( distribute_weight_data_out_w      ),
    .weights_valid_out      ( distribute_weight_valid_out_w     ),
    .weights_avail_in       ( weight_avail_in_r      )
  );

  // mul modules
  generate
  for (i=0; i<`NUM_LANES; i=i+1) begin
    MUL #(
      .GROUP_SIZE           ( `GROUP_SIZE ),
      .DATA_WIDTH           ( `DATA_WIDTH ),
      .LOG_MAX_ITERS        ( `LOG_MAX_ITERS ),
      .LOG_MAX_READS_PER_ITER ( `LOG_MAX_READS_PER_ITER )
    ) mul (
      .clk                    ( clk_r ),
      .rst                    ( rst_r ),
      .configure              ( configure_r ),
      .num_iters              ( num_iters_r ),
      .num_reads_per_iter     ( num_reads_per_iter_r ),
      .act_data_in            ( ),
      .act_valid_in           ( ),
      .act_avail_out          ( ),
      .weight_data_in         ( ),
      .weight_valid_in        ( ),
      .weight_avail_out       ( ),
      .data_out               ( ),
      .valid_out              ( ),
      .avail_in               ( )
    );
  end
  endgenerate

always #5 clk_r <= ~clk_r;

integer j;

initial begin
  rst_r <= 1;
  clk_r <= 0;
  configure_r <= 0;
  num_iters_r <= `NUM_ITERS;
  num_reads_per_iter_r <= `NUM_OPS_PER_ITER;
  for (j=0; j<`NUM_LANES; j=j+1) begin
    avail_in_r[j] <= 1;
    weight_avail_in_r[j] <= 1;
  end
  base_address_r <= 32;
  conf_mode_r = 1;
  
  #20 
  rst_r <= 0;

  #20 
  rst_r <= 1;

  $display("GROUP_SIZE %d NUM_INPUTS %d NUM_LANES %d NUM_ITERS %d NUM_OPS_PER_ITER %d", `GROUP_SIZE, `NUM_INPUTS, `NUM_LANES, `NUM_ITERS, `NUM_OPS_PER_ITER);

  #40 
  configure_r <= 1;

  #10
  configure_r <= 0;

  #20
  $display("no available!!!!");
  avail_in_r[0] <= 0;

  #1000
  $display("available again");
  avail_in_r[0] <= 1;

end

endmodule






