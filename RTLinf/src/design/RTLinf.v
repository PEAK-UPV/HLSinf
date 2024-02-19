// Module RTLinf
//
// This module implements the whole RTLinf accelerator basic kernel
//
// The module implements NUM_INPUTS input read streams that feed a DISTRIBUTE_IN module. This module
// distributes incoming data to NUM_LANES multipliers, aligners, and accumulator modules. All these 
// lanes feed a DISTRIBUTE_OUT module which collects all data and forwards it to NUM_OUTPUTS write modules.
//
// The module runs for a given number of iterations and within each iteration performs a given number of operational cycles.
// READ, DISTRIBUTE_IN, MUL, ALIGN, and ACC run for each iteration whereas DISTRIBUTE_OUT and WRITE run whenever input
// data is available at the module (indeed, the ACC module only forwards data in the last iteration).
// 
// DISTRIBUTE_IN module forwards on the first operation cycle of an iteration a weight value (read from external memory).
// 
// DISTRIBUTE_IN module can work in two modes (conf_mode_in paraneter):
//    CONF_MODE_0 only input 0 is read and data is broadcasterd to all lanes
//    CONF_MODE_1 every input is read and sent to the output with the same port number (input i to output i)
// 
// DISTRIBUTE_OUT module can work in two modes (conf_mode_out parameter):
//    CONF_MODE_0 all inputs are accumulated on every cycle and output is sent through output port 0
//    CONF_MODE_1 all inputs are forwarded to the same port identifier (input i to output i)
//
// Input data (activations and weights) and output data (produced activations) is external to the module.
//
// IMPORTANT: The module is implemented with customized RAM memories. Current memory supported configurations are 4Kx32, 4Kx64 and 4Kx72. This
// means the following configurations of the module are possible:
//   - NUM_ADDRESSES 4096, DATA_WIDTH 8, GROUP_SIZE 4, NUM_INPUTS 9, NUM_LANES 9, NUM_OUTPUTS 9
//
// Any other configuration needing a different memory configuration is not currently supported.

`include "RTLinf.vh"

module KERNEL #(
  parameter GROUP_SIZE             = 4,    // group size
  parameter DATA_WIDTH             = 8,    // data width
  parameter NUM_INPUTS             = 9,    // number of inputs
  parameter NUM_LANES              = 9,    // number of lanes
  parameter NUM_OUTPUTS            = 9,    // number of outputs
  parameter LOG_MAX_ITERS          = 16,   // number of bits for max iters
  parameter LOG_MAX_READS_PER_ITER = 16,   // number of bits for reads_per_iter
  parameter LOG_MAX_ADDRESS        = 12,   // number of bits for addresses
  parameter NUM_ADDRESSES          = 4096  // number of addresses in memories
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
wire [NUM_LANES-1:0]                        mul2align_valid_w;
wire [NUM_LANES-1:0]                        mul2align_avail_w;

// wires between ALIGN and ACC modules
wire [2*GROUP_SIZE*DATA_WIDTH-1:0]          align2acc_data_w[NUM_LANES-1:0];
wire [NUM_LANES-1:0]                        align2acc_valid_w;
wire [NUM_LANES-1:0]                        align2acc_avail_w;

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

// read modules will be configured depending on the conf_mode_in mode. If set to zero then
// only read module 0 will be configured, if set to one then all memory modules will be enabled
wire only_input_0_configured_w = ~conf_mode_in;

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

// combined data and valid signal between READ and DISTRIBUTE_IN modules
generate
  for (i=0; i<NUM_INPUTS; i=i+1) begin
    assign act_read2distr_combined_data_w[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH] = act_read2distr_data_w[i];
    assign act_read2distr_combined_valid_w[i]                                                      = act_read2distr_valid_w[i];
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
    .configure              ( only_input_0_configured_w & (i!=0) ? 0 : configure                     ),
    .num_iters              ( num_iters                     ),
    .num_reads_per_iter     ( num_reads_per_iter            ),
    .base_address           ( read_address                  ),
    .valid_in               ( act_valid[i]                  ),
    .data_in                ( act_data[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH] ),
    .address_out            ( act_addr[((i+1)*LOG_MAX_ADDRESS)-1:i*LOG_MAX_ADDRESS]             ),
    .request                ( act_read[i]                   ),
    .avail_in               ( act_read2distr_avail_w[i]     ),
    .valid_out              ( act_read2distr_valid_w[i]     ),
    .data_out               ( act_read2distr_data_w[i]      )
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
  .num_iters              ( 1                             ),   // weights are read, one per iteration (thus, only one iteration)
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
      .act_valid_in           ( act_distr2mul_valid_w[i]                                                      ),
      .act_avail_out          ( act_distr2mul_avail_w[i]                                                      ),
      .weight_data_in         ( weight_distr2mul_data_w[((i+1)*DATA_WIDTH)-1:i*DATA_WIDTH]                    ),
      .weight_valid_in        ( weight_distr2mul_valid_w[i]                                                   ),
      .weight_avail_out       ( weight_distr2mul_avail_w[i]                                                   ),
      .data_out               ( mul2align_data_w[i]                                                           ),
      .valid_out              ( mul2align_valid_w[i]                                                          ),
      .avail_in               ( mul2align_avail_w[i]                                                          )
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
    .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS        ),
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


// Module RTLinf (será el final)
//
//

module RTLinf #(
  parameter GROUP_SIZE             = 4,    // group size (data is read in groups of GROUP SIZE items)
  parameter DATA_WIDTH             = 8,    // data width (each data and weight item data width)
  parameter NUM_KERNELS            = 4,    // number of kernels
  parameter LOG_NUM_KERNELS        = 2,    // number of bits to encode a kernel id
  parameter NUM_ACT_MEMORIES       = 4,    // number of activation memories
  parameter LOG_NUM_ACT_MEMORIES   = 2,    // number of bits to encode an activation memory id
  parameter NUM_WEIGHT_MEMORIES    = 4,    // number of weight memories
  parameter LOG_NUM_WEIGHT_MEMORIES= 2,    // number of bits to encode a weight memory id
  parameter NUM_INPUTS             = 9,    // number of inputs of each kernel
  parameter NUM_LANES              = 9,    // number of lanes of each kernel
  parameter NUM_OUTPUTS            = 9,    // number of outputs of each kernel
  parameter LOG_MAX_ITERS          = 16,   // number of bits for max iters
  parameter LOG_MAX_READS_PER_ITER = 16,   // number of bits for reads_per_iter
  parameter LOG_MAX_ADDRESS        = 12,   // number of bits for addresses
  parameter NUM_ADDRESSES          = 4096, // number of addresses in memories
  //
  localparam WIDTH_ACT_READ_SELECTS     = (NUM_ACT_MEMORIES * (LOG_NUM_KERNELS+1)),
  localparam WIDTH_ACT_WRITE_SELECTS    = (NUM_ACT_MEMORIES * (LOG_NUM_KERNELS+1)),
  localparam WIDTH_WEIGHT_READ_SELECTS  = NUM_WEIGHT_MEMORIES * LOG_NUM_KERNELS,
  localparam WIDTH_WEIGHT_WRITE_SELECTS = NUM_WEIGHT_MEMORIES
) (
  input                                   clk,                // clock input
  input                                   rst,                // reset input
  // configuration
  input                                   configure,          // configure signal
  input [LOG_MAX_ITERS-1:0]               num_iters,          // num iterations
  input [LOG_MAX_READS_PER_ITER-1:0]      num_reads_per_iter, // num reads per iteration
  input [DATA_WIDTH-1:0]                  min_clip,           // min cliping value
  input [DATA_WIDTH-1:0]                  max_clip,           // max cliping value
  input                                   conf_mode_in,       // conf mode for distribute_in module
  input                                   conf_mode_out,      // conf mode for distribute_out module
  //
  input                                   cmd_act_assign,     // assign an activation memory to a read and a write port
  input                                   cmd_act_unassign,   // unassign an activation memory froma  read and a write port
  input [LOG_NUM_KERNELS+1-1:0]           cmd_act_read_port,  // activation read port to assign/unassing
  input [LOG_NUM_KERNELS+1-1:0]           cmd_act_write_port, // activation write port to assign/unassign
  input [LOG_NUM_ACT_MEMORIES-1:0]        cmd_act_memory,     // activation memory to assign/unassign
  //
  //
  input                                   cmd_weight_assign,     // assign a weight memory to a read and a write port
  input                                   cmd_weight_unassign,   // unassign a weight memory froma  read and a write port
  input [LOG_NUM_KERNELS-1:0]             cmd_weight_read_port,  // weight read port to assign/unassing
  input                                   cmd_weight_write_port, // weight write port to assign/unassign (there is only one)
  input [LOG_NUM_WEIGHT_MEMORIES-1:0]     cmd_weight_memory,     // weight memory to assign/unassign
  //
  // read port to activations
  input [LOG_MAX_ADDRESS-1:0]             act_read_addr,      // read address to activation memory bank
  input                                   act_read,           // read signal to activation memory bank
  output [GROUP_SIZE*DATA_WIDTH-1:0]      act_read_data,      // data from activation memory bank
  output                                  act_read_valid,     // valid signal from activation memory bank
  // write port to activations
  input [LOG_MAX_ADDRESS-1:0]             act_write_addr,     // write address to activation memory bank
  input                                   act_write,          // write signal to activation memory bank
  input [GROUP_SIZE*DATA_WIDTH-1:0]       act_write_data,     // write data to activation memory bank
  // write port to weights
  input [LOG_MAX_ADDRESS-1:0]             weight_write_addr,  // write address to weight memory bank
  input                                   weight_write,       // write signal to weight memory bank
  input [NUM_LANES*DATA_WIDTH-1:0]        weight_write_data   // write data to weight memory bank
  
);

genvar i;

// wires for reads between kernels and activation memory bank (notice we add the module local read port)
wire [((NUM_KERNELS+1) * LOG_MAX_ADDRESS)-1 : 0]         act_read_addr_w;
wire [NUM_KERNELS+1-1 : 0]                               act_read_w;
wire [((NUM_KERNELS+1) * (GROUP_SIZE*DATA_WIDTH))-1 : 0] act_read_data_w;
wire [NUM_KERNELS+1-1 : 0]                               act_read_valid_w;
//
// local port
assign act_read_addr_w[((NUM_KERNELS+1) * LOG_MAX_ADDRESS)-1 -: LOG_MAX_ADDRESS] = act_read_addr;
assign act_read_w[NUM_KERNELS+1-1] = act_read;
assign act_read_data = act_read_data_w[((NUM_KERNELS+1) * (GROUP_SIZE*DATA_WIDTH))-1 -: (GROUP_SIZE*DATA_WIDTH)];
assign act_read_valid = act_read_valid_w[NUM_KERNELS+1-1];

// wires for writes between kernels and activation memory bank (notice we add the module local write port)
wire [((NUM_KERNELS+1) * LOG_MAX_ADDRESS)-1 : 0]         act_write_addr_w;
wire [NUM_KERNELS+1-1 : 0]                               act_write_w;
wire [((NUM_KERNELS+1) * (GROUP_SIZE*DATA_WIDTH))-1 : 0] act_write_data_w;
//
// local port
assign act_write_addr_w[((NUM_KERNELS+1) * LOG_MAX_ADDRESS)-1 -: LOG_MAX_ADDRESS] = act_write_addr;
assign act_write_w[NUM_KERNELS+1-1] = act_write;
assign act_write_data_w[((NUM_KERNELS+1) * (GROUP_SIZE*DATA_WIDTH))-1 -: (GROUP_SIZE*DATA_WIDTH)] = act_write_data;

// wires for reads between kernels and weight memory bank
wire [(NUM_KERNELS * LOG_MAX_ADDRESS)-1 : 0]             weight_read_addr_w;
wire [NUM_KERNELS-1 : 0]                                 weight_read_w;
wire [(NUM_KERNELS * (NUM_LANES*DATA_WIDTH))-1 : 0]      weight_read_data_w;
wire [NUM_KERNELS-1 : 0]                                 weight_read_valid_w;

// wires for writes between this module and the weight memory bank
wire [LOG_MAX_ADDRESS-1 : 0]                             weight_write_addr_w;
wire                                                     weight_write_w;
wire [(NUM_LANES*DATA_WIDTH)-1 : 0]                      weight_write_data_w;
// local port (the only one)
assign weight_write_addr_w = weight_write_addr;
assign weight_write_w      = weight_write;
assign weight_write_data_w = weight_write_data;

// kernels
generate
  for (i=0; i<NUM_KERNELS; i=i+1) begin
    KERNEL #(
      .GROUP_SIZE             ( GROUP_SIZE             ),
      .DATA_WIDTH             ( DATA_WIDTH             ),
      .NUM_INPUTS             ( NUM_INPUTS             ),
      .NUM_LANES              ( NUM_LANES              ),
      .NUM_OUTPUTS            ( NUM_OUTPUTS            ),
      .LOG_MAX_ITERS          ( LOG_MAX_ITERS          ),
      .LOG_MAX_READS_PER_ITER ( LOG_MAX_READS_PER_ITER ),
      .LOG_MAX_ADDRESS        ( LOG_MAX_ADDRESS        ),
      .NUM_ADDRESSES          ( NUM_ADDRESSES          )
    ) kernel_m (
      .clk                    ( clk                    ),
      .rst                    ( rst                    ),
      //
      .act_addr               ( act_read_addr_w[((i+1)*LOG_MAX_ADDRESS)-1 -: LOG_MAX_ADDRESS]                ),
      .act_read               ( act_read_w[i]                                                                ),
      .act_data               ( act_read_data_w[((i+1)*(GROUP_SIZE*DATA_WIDTH))-1 -: GROUP_SIZE*DATA_WIDTH]  ),
      .act_valid              ( act_read_valid_w[i]                                                          ),
      //
      .weight_addr            ( weight_read_addr_w[((i+1)*LOG_MAX_ADDRESS)-1 -: LOG_MAX_ADDRESS]             ),
      .weight_read            ( weight_read_w[i]                                                             ),
      .weight_data            ( weight_read_data_w[((i+1)*(NUM_LANES*DATA_WIDTH))-1 -: NUM_LANES*DATA_WIDTH] ),
      .weight_valid           ( weight_read_valid_w[i]                                                       ),
      //
      .configure              ( configure              ),
      .num_iters              ( num_iters              ),
      .num_reads_per_iter     ( num_reads_per_iter     ),
      .read_address           ( 0                      ),  // TODO
      .write_address          ( 0                      ),  // TODO
      .min_clip               ( min_clip               ),
      .max_clip               ( max_clip               ),
      .conf_mode_in           ( conf_mode_in           ),
      .conf_mode_out          ( conf_mode_out          ),
      //
      .data_out               ( act_write_data_w       ),
      .addr_out               ( act_write_addr_w       ),
      .valid_out              ( act_write_w            )
    );
  end
  
  // Activation memory bank
  MEMORY_BANK #(
    .DATA_WIDTH          ( GROUP_SIZE * DATA_WIDTH ),
    .NUM_ADDRESSES       ( NUM_ADDRESSES           ),
    .LOG_MAX_ADDRESS     ( LOG_MAX_ADDRESS         ),
    .NUM_MEMORIES        ( NUM_ACT_MEMORIES        ),
    .LOG_NUM_MEMORIES    ( LOG_NUM_ACT_MEMORIES    ),
    .NUM_READ_PORTS      ( NUM_KERNELS + 1         ),
    .LOG_NUM_READ_PORTS  ( LOG_NUM_KERNELS + 1     ),
    .NUM_WRITE_PORTS     ( NUM_KERNELS + 1         ),
    .LOG_NUM_WRITE_PORTS ( LOG_NUM_KERNELS + 1     )
  ) activation_mem_m (
  .clk                   ( clk                     ),
  .rst                   ( rst                     ),
  //
  .cmd_assign            ( cmd_act_assign          ),
  .cmd_unassign          ( cmd_act_unassign        ),
  .read_port             ( cmd_act_read_port       ),
  .write_port            ( cmd_act_write_port      ),
  .memory                ( cmd_act_memory          ),
  //
  .write_data_in         ( act_write_data_w        ),
  .write_addr_in         ( act_write_addr_w        ),
  .write_in              ( act_write_w             ),
  //
  .read_data_out         ( act_read_data_w         ),
  .read_valid_out        ( act_read_valid_w        ),
  .read_addr_in          ( act_read_addr_w         ),
  .read_in               ( act_read_w              )            
);

// Weight memory bank
  MEMORY_BANK #(
    .DATA_WIDTH          ( NUM_LANES * DATA_WIDTH ),
    .NUM_ADDRESSES       ( NUM_ADDRESSES           ),
    .LOG_MAX_ADDRESS     ( LOG_MAX_ADDRESS         ),
    .NUM_MEMORIES        ( NUM_WEIGHT_MEMORIES     ),
    .LOG_NUM_MEMORIES    ( LOG_NUM_WEIGHT_MEMORIES ),
    .NUM_READ_PORTS      ( NUM_KERNELS             ),
    .LOG_NUM_READ_PORTS  ( LOG_NUM_KERNELS         ),
    .NUM_WRITE_PORTS     ( 1                       ),
    .LOG_NUM_WRITE_PORTS ( 1                       )
  ) weight_mem_m (
    .clk                 ( clk                     ),
    .rst                 ( rst                     ),
    //
    .cmd_assign          ( cmd_weight_assign       ),
    .cmd_unassign        ( cmd_weight_unassign     ),
    .read_port           ( cmd_weight_read_port    ),
    .write_port          ( cmd_weight_write_port   ),
    .memory              ( cmd_weight_memory       ),
    //
    .write_data_in       ( weight_write_data_w     ),
    .write_addr_in       ( weight_write_addr_w     ),
    .write_in            ( weight_write_w          ),
    //
    .read_data_out       ( weight_read_data_w      ),
    .read_valid_out      ( weight_read_valid_w     ),
    .read_addr_in        ( weight_read_addr_w      ),
    .read_in             ( weight_read_w           )            
         
);
endgenerate

endmodule
