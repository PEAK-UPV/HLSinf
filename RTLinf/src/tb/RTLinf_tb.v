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


`define NUM_ADDRESSES         1024
`define LOG_MAX_ADDRESS         10

`define GROUP_SIZE               4
`define DATA_WIDTH               8

`define NUM_KERNELS              2
`define LOG_NUM_KERNELS          1

`define NUM_ACT_MEMORIES         2
`define LOG_NUM_ACT_MEMORIES     1

`define NUM_WEIGHT_MEMORIES      2
`define LOG_NUM_WEIGHT_MEMORIES  1

`define NUM_INPUTS               1
`define NUM_LANES                9
`define NUM_OUTPUTS              1

`define NUM_ITERS                2
`define LOG_MAX_ITERS            8     // LOG_MAX_ITERS and LOG_MAX_READS_PER_ITER must be equal (to avoid data lenght problems when reading weights)
`define NUM_OPS_PER_ITER        16
`define LOG_MAX_READS_PER_ITER   8

module RTLinf_tb;

  // inputs
  reg                                 clk_r;
  reg                                 rst_r;
  reg [`NUM_KERNELS-1:0]              configure_r;
  reg [`LOG_MAX_ITERS-1:0]            num_iters_r;
  reg [`LOG_MAX_READS_PER_ITER-1:0]   num_reads_per_iter_r;
  reg                                 conf_mode_in_r;
  reg                                 conf_mode_out_r;
  reg [`DATA_WIDTH-1:0]               min_clip_r;
  reg [`DATA_WIDTH-1:0]               max_clip_r;
  //
  reg                                 cmd_act_assign_r;
  reg                                 cmd_act_unassign_r;
  reg [`LOG_NUM_KERNELS+1-1:0]        cmd_act_read_port_r;
  reg [`LOG_NUM_KERNELS+1-1:0]        cmd_act_write_port_r;
  reg [`LOG_NUM_ACT_MEMORIES-1:0]     cmd_act_memory_r;
  //
  reg                                 cmd_weight_assign_r;
  reg                                 cmd_weight_unassign_r;
  reg [`LOG_NUM_KERNELS-1:0]          cmd_weight_read_port_r;
  reg                                 cmd_weight_write_port_r;
  reg [`LOG_NUM_WEIGHT_MEMORIES-1:0]  cmd_weight_memory_r;
  //
  reg                                 act_read_r;
  reg [`LOG_MAX_ADDRESS-1:0]          act_read_addr_r;
  //
  reg                                   act_write_r;
  reg [`LOG_MAX_ADDRESS-1:0]            act_write_addr_r;
  reg [(`GROUP_SIZE * `DATA_WIDTH)-1:0] act_write_data_r;
  //
  reg                                   weight_write_r;
  reg [`LOG_MAX_ADDRESS-1:0]            weight_write_addr_r;
  reg [(`NUM_LANES * `DATA_WIDTH)-1:0]  weight_write_data_r;
  
  // outputs
  wire [(`GROUP_SIZE * `DATA_WIDTH)-1:0] act_read_data_w;
  wire                                   act_read_valid_w;
      
  RTLinf #(
    .GROUP_SIZE             ( `GROUP_SIZE             ),
    .DATA_WIDTH             ( `DATA_WIDTH             ),
    .NUM_KERNELS            ( `NUM_KERNELS            ),
    .LOG_NUM_KERNELS        ( `LOG_NUM_KERNELS        ),
    .NUM_INPUTS             ( `NUM_INPUTS             ),
    .NUM_LANES              ( `NUM_LANES              ),
    .NUM_OUTPUTS            ( `NUM_OUTPUTS            ),
    .NUM_ACT_MEMORIES       ( `NUM_ACT_MEMORIES       ),
    .LOG_NUM_ACT_MEMORIES   ( `LOG_NUM_ACT_MEMORIES   ),
    .NUM_WEIGHT_MEMORIES    ( `NUM_WEIGHT_MEMORIES    ),
    .LOG_NUM_WEIGHT_MEMORIES( `LOG_NUM_WEIGHT_MEMORIES),
    .LOG_MAX_ITERS          ( `LOG_MAX_ITERS          ),
    .LOG_MAX_READS_PER_ITER ( `LOG_MAX_READS_PER_ITER ),
    .LOG_MAX_ADDRESS        ( `LOG_MAX_ADDRESS        ),
    .NUM_ADDRESSES          ( `NUM_ADDRESSES          )

  ) RTLinf_m (
    .clk                    ( clk_r                   ),
    .rst                    ( rst_r                   ),
    //
    .act_read               ( act_read_r              ),
    .act_read_addr          ( act_read_addr_r         ),
    .act_read_data          ( act_read_data_w         ),
    .act_read_valid         ( act_read_valid_w        ),
    //
    .act_write              ( act_write_r             ),
    .act_write_addr         ( act_write_addr_r        ),
    .act_write_data         ( act_write_data_r        ),
    //
    .weight_write           ( weight_write_r          ),
    .weight_write_addr      ( weight_write_addr_r     ),
    .weight_write_data      ( weight_write_data_r     ),
    //
    .configure              ( configure_r             ),
    .num_iters              ( num_iters_r             ),
    .num_reads_per_iter     ( num_reads_per_iter_r    ),
    .min_clip               ( min_clip_r              ),
    .max_clip               ( max_clip_r              ),
    .conf_mode_in           ( conf_mode_in_r          ),
    .conf_mode_out          ( conf_mode_out_r         ),
    //
    .cmd_act_assign         ( cmd_act_assign_r        ),
    .cmd_act_unassign       ( cmd_act_unassign_r      ),
    .cmd_act_read_port      ( cmd_act_read_port_r     ),
    .cmd_act_write_port     ( cmd_act_write_port_r    ),
    .cmd_act_memory         ( cmd_act_memory_r        ),
    //
    .cmd_weight_assign         ( cmd_weight_assign_r        ),
    .cmd_weight_unassign       ( cmd_weight_unassign_r      ),
    .cmd_weight_read_port      ( cmd_weight_read_port_r     ),
    .cmd_weight_write_port     ( cmd_weight_write_port_r    ),
    .cmd_weight_memory         ( cmd_weight_memory_r        )
  );
  
  
always #5 clk_r <= ~clk_r;

integer j, m;

initial begin
  rst_r                <= 1;
  clk_r                <= 0;
  configure_r          <= 0;
  num_iters_r          <= `NUM_ITERS;
  num_reads_per_iter_r <= `NUM_OPS_PER_ITER;
  conf_mode_in_r       <= 0;  // input 0 broadcasted to all lanes
  conf_mode_out_r      <= 0;  // all inputs added and sent to all outputs
  min_clip_r           <= 0;
  max_clip_r           <= 255;
  //
  cmd_act_assign_r    <= 0;
  cmd_act_unassign_r  <= 0;
  cmd_act_read_port_r <= 0;
  cmd_act_write_port_r <= 0;
  cmd_act_memory_r <= 0;
  //
  cmd_weight_assign_r <= 0;
  cmd_weight_unassign_r <= 0;
  cmd_weight_read_port_r <= 0;
  cmd_weight_write_port_r <= 0;
  cmd_weight_memory_r <= 0;
  //  
  act_read_r            <= 0;
  act_read_addr_r       <= 0;
  act_write_r           <= 0;
  act_write_data_r      <= 0;
  act_write_addr_r      <= 0;
  weight_write_r        <= 0;
  weight_write_addr_r   <= 0;
  weight_write_data_r   <= 0;
  
  // reset
  #20 rst_r <= 0;
  #20 rst_r <= 1;
  
  #200  // some cycles to let BRAMs to be ready
    
  // we write (fill) every activation memory with data, first we assign the last read and write ports to the memory. The last read and write ports are the local ports which we do have external access
  for (m=0; m<`NUM_ACT_MEMORIES; m=m+1) begin
    cmd_act_assign_r <= 1; cmd_act_read_port_r <= `NUM_KERNELS; cmd_act_write_port_r <= `NUM_KERNELS; cmd_act_memory_r <= m; #10
    cmd_act_assign_r <= 0;
    //
    act_write_r <= 1; act_write_addr_r = 0; act_write_data_r <= 32'h03_02_01_00; #10;
    act_write_r <= 1; act_write_addr_r = 1; act_write_data_r <= 32'h07_06_05_04; #10;
    act_write_r <= 1; act_write_addr_r = 2; act_write_data_r <= 32'h0b_0a_09_08; #10;
    act_write_r <= 1; act_write_addr_r = 3; act_write_data_r <= 32'h0f_0e_0d_0c; #10;
    act_write_r <= 1; act_write_addr_r = 4; act_write_data_r <= 32'h13_12_11_10; #10;
    act_write_r <= 1; act_write_addr_r = 5; act_write_data_r <= 32'h17_16_15_14; #10;
    act_write_r <= 1; act_write_addr_r = 6; act_write_data_r <= 32'h1b_1a_19_18; #10;
    act_write_r <= 1; act_write_addr_r = 7; act_write_data_r <= 32'h1f_1e_1d_1c; #10;
    act_write_r <= 1; act_write_addr_r = 8; act_write_data_r <= 32'h23_22_21_20; #10;
    act_write_r <= 1; act_write_addr_r = 9; act_write_data_r <= 32'h27_26_25_24; #10;
    act_write_r <= 1; act_write_addr_r =10; act_write_data_r <= 32'h2b_2a_29_28; #10;
    act_write_r <= 1; act_write_addr_r =11; act_write_data_r <= 32'h2f_2e_2d_2c; #10;
    act_write_r <= 1; act_write_addr_r =12; act_write_data_r <= 32'h33_32_31_30; #10;
    act_write_r <= 1; act_write_addr_r =13; act_write_data_r <= 32'h37_36_35_34; #10;
    act_write_r <= 1; act_write_addr_r =14; act_write_data_r <= 32'h3b_3a_39_38; #10;
    act_write_r <= 1; act_write_addr_r =15; act_write_data_r <= 32'h3f_3e_3d_3c; #10;
    act_write_r <= 0;
    //
    // unasign the ports/memory
    cmd_act_unassign_r <= 1; cmd_act_read_port_r <= `NUM_KERNELS; cmd_act_write_port_r <= `NUM_KERNELS; cmd_act_memory_r <= m; #10
    cmd_act_unassign_r <= 0;
  end
  
  // we now write (fill every weight memory with data, first we assign the write port to the memory
  for (m=0; m<`NUM_WEIGHT_MEMORIES; m=m+1) begin
    cmd_weight_assign_r <= 1; cmd_weight_read_port_r <= 0; cmd_weight_write_port_r <= 0; cmd_weight_memory_r <= m; 
    #10 cmd_weight_assign_r <= 0;
    
    weight_write_r <= 1; weight_write_addr_r = 0; weight_write_data_r <= 72'h00_00_00_00_00_00_00_00_01; #10;
    weight_write_r <= 1; weight_write_addr_r = 1; weight_write_data_r <= 72'h00_00_01_00_00_00_00_00_00; #10;
    weight_write_r <= 0;
 
    // unasign the ports/memory
    cmd_weight_unassign_r <= 1; cmd_weight_read_port_r <= 0; cmd_weight_write_port_r <= 0; cmd_weight_memory_r <= m; #10
    cmd_weight_unassign_r <= 0;
  end
  
  
  // we assign activation memory {0} to read port {0} and write port {0}
  cmd_act_assign_r <= 1; cmd_act_read_port_r <= 0; cmd_act_write_port_r <= 0; cmd_act_memory_r <= 0; #10
  cmd_act_assign_r <= 0;
  
  // we assign activation memory {1} to read port {1} and write port {1}
  cmd_act_assign_r <= 1; cmd_act_read_port_r <= 1; cmd_act_write_port_r <= 1; cmd_act_memory_r <= 1; #10
  cmd_act_assign_r <= 0;  
  
  // we assign weight memory {0} to read port {0} and write port {0}
  cmd_weight_assign_r <= 1; cmd_weight_read_port_r <= 0; cmd_weight_write_port_r <= 0; cmd_weight_memory_r <= 0; #10
  cmd_weight_assign_r <= 0;
  
  // we assign weight memory {1} to read port {1} and write port {1}
  cmd_weight_assign_r <= 1; cmd_weight_read_port_r <= 1; cmd_weight_write_port_r <= 1; cmd_weight_memory_r <= 1; #10
  cmd_weight_assign_r <= 0;
  
  // now we trigger the two kernels
  #40 configure_r[0] <= 1'b1; configure_r[1] <= 1'b1;
  #10 configure_r[0] <= 1'b0; configure_r[1] <= 1'b0;

  // we wait long for the finish of the kernels
  #1000

  // we unassign activation memory {0} from read port {0} and write port {0}
  cmd_act_unassign_r <= 1; cmd_act_read_port_r <= 0; cmd_act_write_port_r <= 0; cmd_act_memory_r <= 0; #10
  cmd_act_unassign_r <= 0;
  
  // we unassign activation memory {1} from read port {1} and write port {1}
  cmd_act_unassign_r <= 1; cmd_act_read_port_r <= 1; cmd_act_write_port_r <= 1; cmd_act_memory_r <= 1; #10
  cmd_act_unassign_r <= 0;  
 
  // we now read every activation memory
  for (m=0; m<`NUM_ACT_MEMORIES; m=m+1) begin
    cmd_act_assign_r <= 1; cmd_act_read_port_r <= `NUM_KERNELS; cmd_act_write_port_r <= `NUM_KERNELS; cmd_act_memory_r <= m; #10
    cmd_act_assign_r <= 0;
    //
    for (j=0; j<16; j=j+1) begin
      act_read_r <= 1; act_read_addr_r = j; #10;
      $display("Data read from memory %d addr %x: -> valid %d data %x", m, j, act_read_valid_w, act_read_data_w);
    end
    act_read_r <= 0;
    // unasign the ports/memory
    cmd_act_unassign_r <= 1; cmd_act_read_port_r <= `NUM_KERNELS; cmd_act_write_port_r <= `NUM_KERNELS; cmd_act_memory_r <= m; #10
    cmd_act_unassign_r <= 0;
  end


end

endmodule






