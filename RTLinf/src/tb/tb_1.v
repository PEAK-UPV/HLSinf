//
// TB_1: Testbench for a BRAM and a READ module
//
//

`define LOG_MAX_ITERS 8
`define LOG_MAX_READS_PER_ITER 16
`define NUM_ADDRESSES 4096
`define LOG_MAX_ADDRESS 12
`define DATA_WIDTH 8


module tb_1;

 // inputs
 reg clk_r;
 reg rst_r;
 reg configure_r;
 reg [`LOG_MAX_ITERS-1:0] num_iters_r;
 reg [`LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;
 reg [`LOG_MAX_ADDRESS-1:0] base_address_r;
 reg avail_in_r;
 
 reg write_r; // memory used only for reads
 reg [`LOG_MAX_ADDRESS-1:0] addr_write_r;
 reg [`DATA_WIDTH-1:0] data_write_r;
 
 // wires between MEM and READ
 wire                        read_w;
 wire [`LOG_MAX_ADDRESS-1:0] addr_read_w;
 wire [`DATA_WIDTH-1:0]      data_read_w;
 wire                        valid_in_w;

 // outputs
 wire valid_out_w;
 wire [`DATA_WIDTH-1:0] data_out_w;

 MEM #(
   .DATA_WIDTH        ( `DATA_WIDTH      ),
   .NUM_ADDRESSES     ( `NUM_ADDRESSES   ),
   .LOG_MAX_ADDRESS   ( `LOG_MAX_ADDRESS )
 ) mem_0 (
   .clk               ( clk_r        ),
   .rst               ( rst_r        ),
   .data_write        ( data_write_r ),
   .addr_write        ( addr_write_r ),
   .write             ( write_r      ),
   .addr_read         ( addr_read_w  ),
   .read              ( read_w       ),
   .data_read         ( data_read_w  ),
   .valid_out         ( valid_in_w   )
 );

 READ #(
   .DATA_WIDTH             ( `DATA_WIDTH             ),
   .LOG_MAX_ITERS          ( `LOG_MAX_ITERS          ),
   .LOG_MAX_READS_PER_ITER ( `LOG_MAX_READS_PER_ITER ),
   .LOG_MAX_ADDRESS        ( `LOG_MAX_ADDRESS        )
 ) read_0 (
   .clk                    ( clk_r                   ),
   .rst                    ( rst_r                   ),
   .configure              ( configure_r             ),
   .num_iters              ( num_iters_r             ),
   .num_reads_per_iter     ( num_reads_per_iter_r    ),
   .base_address           ( base_address_r          ),
   .valid_in               ( valid_in_w              ),
   .data_in                ( data_read_w             ),
   .address_out            ( addr_read_w             ),
   .request                ( read_w                  ),
   .avail_in               ( avail_in_r              ),
   .valid_out              ( valid_out_w             ),
   .data_out               ( data_out_w              )
 );

always #5 clk_r <= ~clk_r;

initial begin
  rst_r <= 1;
  clk_r <= 0;
  configure_r <= 0;
  num_iters_r <= 4;
  num_reads_per_iter_r <= 16;
  avail_in_r <= 1;
  base_address_r <= 32;
  
  write_r <= 0; // only reads
  addr_write_r <= 0;
  data_write_r <= 0;

  #20 
  rst_r <= 0;

  #20 
  rst_r <= 1;

  #40 
  configure_r <= 1;

  #10
  configure_r <= 0;

end

endmodule






