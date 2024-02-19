// Module DISTRIBUTE_IN
//
// This module distributes input data to outputs based on specific configuration.
// The module has as NUM_DATA_INPUTS activation input ports. Every such input port
// has an independent FIFO. Activation pixels are stored in those FIFOs.
// The module has NUM_WEIGHT_INPUTS ports as well which is equal to the number of output ports (NUM_DATA_OUTPUTS)
// A single FIFO is used to store the input weights.
//
// The module works a given number of iterations and a number of "reads" per iteration. A "read" is consumed
// when all input data is available and the outputs are available (as well). On the first "read" of a cycle
// we forward both activations and weights, on the remaining "reads" for the iteration we only care on data.
// When all "reads" operations of one iteration are consumed we set up a new iteration. When all iterations
// are consumed the module disables itself.
// 
// Output ports are grouped into two output ports. The first one collects all output activations and the second one
// collects all output weights.
//
// Assignement of input activations to outputs depends on the conf_mode parameter. Current modes:
//    - CONF_MODE_1: Every input activation i is sent to output data i
//

`include "RTLinf.vh"

`define CONF_MODE_0 0
`define CONF_MODE_1 1

module DISTRIBUTE_IN #(
    parameter NUM_DATA_INPUTS        = 9,                // number of data inputs
    parameter GROUP_SIZE             = 4,                // group size
    parameter DATA_WIDTH             = 8,                // input and output data width
    parameter NUM_DATA_OUTPUTS       = 9,                // number of data outputs
    parameter LOG_MAX_ITERS          = 16,               // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16,               // number of bits for max reads per iter
    localparam NUM_WEIGHT_INPUTS     = NUM_DATA_OUTPUTS  // The number of input weights equals the number of outputs
) (
  input clk,
  input rst,

  input                                    configure,                    // CONFIGURE interface:: configure signal
  input                                    conf_mode,                    // CONFIGURE interface:: configuration mode
  input [LOG_MAX_ITERS-1:0]                num_iters,                    // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0]       num_reads_per_iter,           // CONFIGURE interface:: number of reads per iteration  

  input [NUM_DATA_INPUTS*GROUP_SIZE*DATA_WIDTH-1:0]   act_data_in,                  // ACTIVATION interface:: data
  input  [NUM_DATA_INPUTS-1:0]             act_valid_in,                 // ACTIVATION interface:: valid
  output [NUM_DATA_INPUTS-1:0]             act_avail_out,                // ACTIVATION interface:: avail

  input [NUM_WEIGHT_INPUTS*DATA_WIDTH-1:0] weights_data_in,              // WEIGHTS interface:: data
  input                                    weights_valid_in,             // WEIGHTS interface:: valid
  output                                   weights_avail_out,            // WEIGHTS interface:: avail

  output [NUM_DATA_OUTPUTS*GROUP_SIZE*DATA_WIDTH-1:0] data_out,                     // OUT1 interface:: data
  output [NUM_DATA_OUTPUTS-1:0]            valid_out,                    // OUT1 interface:: valid
  input  [NUM_DATA_OUTPUTS-1:0]            avail_in,                     // OUT1 interface:: avail
  
  output [NUM_DATA_OUTPUTS*DATA_WIDTH-1:0] weights_data_out,             // OUT2 interface:: data
  output [NUM_DATA_OUTPUTS-1:0]            weights_valid_out,            // OUT2 interface:: valid
  input  [NUM_DATA_OUTPUTS-1:0]            weights_avail_in              // OUT2 interface:: avail

);

// wires
wire [(GROUP_SIZE*DATA_WIDTH) - 1: 0] data_write_w[NUM_DATA_INPUTS-1:0]; // data to write to FIFO
wire [NUM_DATA_INPUTS-1:0]            write_w;                           // write signal to FIFO
wire [NUM_DATA_INPUTS-1:0]            full_w;                            // full signal from FIFO
wire [NUM_DATA_INPUTS-1:0]            almost_full_w;                     // almost_full signal from FIFO
wire [(GROUP_SIZE*DATA_WIDTH) - 1: 0] data_read_w[NUM_DATA_INPUTS-1:0];  // data read from FIFO
wire [NUM_DATA_INPUTS-1:0]            next_read_w;                       // next_read signal to FIFO
wire [NUM_DATA_INPUTS-1:0]            empty_w;                           // empty signal from FIFO
//
wire [NUM_WEIGHT_INPUTS*DATA_WIDTH - 1: 0] weights_data_write_w;         // data to write to weights FIFO
wire                                       weights_write_w;              // write signal to weights FIFO
wire                                       weights_full_w;               // full signal from weights FIFO
wire                                       weights_almost_full_w;        // almost_full signal from weights FIFO
wire [NUM_WEIGHT_INPUTS*DATA_WIDTH - 1: 0] weights_data_read_w;          // data read from weights FIFO
wire                                       weights_next_read_w;          // next_read signal to weights FIFO
wire                                       weights_empty_w;              // empty signal from weights FIFO
//
wire first_read_cycle_w;                                                 // whether we are in the first "read" cycle/operation
wire perform_operation_w;                                                // when set indicates a "read" operation is performed (input data sent to output)


// registers
reg [LOG_MAX_ITERS-1:0]          num_iters_r;                            // FIFO
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;                   // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_copy_r;              // copy of number of reads per iteration
reg                              conf_mode_r;                            // configuration mode
reg                              module_enabled_r;                       // module enabled

genvar i;

// combinational logic (activation FIFOs)
generate
  for (i=0; i<NUM_DATA_INPUTS; i=i+1) begin
    assign data_write_w[i] = act_data_in[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH];   // data to write to the FIFO
    assign write_w[i]      = act_valid_in[i];                                  // FIFO write signal
    assign act_avail_out[i]= ~almost_full_w[i] & ~full_w[i];                   // avail signal from FIFO       
    assign next_read_w[i]  = perform_operation_w;
  end
endgenerate

// combinational logic (weights FIFOs)
assign weights_data_write_w = weights_data_in;                                     // data to write to the weights FIFO
assign weights_write_w      = weights_valid_in;                                    // weights FIFO write signal
assign weights_avail_out    = ~weights_almost_full_w & ~weights_full_w;            // avail signal from weights FIFO       
assign weights_next_read_w  = perform_operation_w & first_read_cycle_w;

// combinational logic (perform operation)
// first_read_cycle_w indicates whether we are on the first "read" cycle of an iteration
// perform_operation_w indicates whether in this cycle we perform the operation. We perform the operation if we have all data at the inputs
// available (on a first read cycle we need to have both weights and activations, on the remaining cycles we need only the activations)
// configuration mode is also taken into account. In conf_mode_0 only input 0 is taken into account, in conf_mode_1 all inputs must have data
assign first_read_cycle_w  = module_enabled_r & (num_reads_per_iter_r == num_reads_per_iter_copy_r);

assign perform_operation_w = (conf_mode_r == `CONF_MODE_1) ? first_read_cycle_w ? ~weights_empty_w & ~(|empty_w) & |avail_in & |weights_avail_in : ~(|empty_w) & (|avail_in) : 
                             (conf_mode_r == `CONF_MODE_0) ? first_read_cycle_w ? ~weights_empty_w & ~(empty_w[0]) & |avail_in & |weights_avail_in : ~empty_w[0] & (|avail_in) : 0;
                             
// combinational logic (outputs of the module)
// in CONF_MODE_0 input #0 is broadcasted to all outputs
// in CONF_MODE_1 input i is forwarded to output i
generate
  for (i=0; i<NUM_DATA_OUTPUTS;i=i+1) begin
    assign data_out[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH] = (conf_mode_r == `CONF_MODE_0) ? data_read_w[0] : (conf_mode_r == `CONF_MODE_1) ? data_read_w[i] : 0;
    assign valid_out[i] = perform_operation_w;
  end
  assign weights_data_out = weights_data_read_w;
  for (i=0; i<NUM_DATA_OUTPUTS;i=i+1) begin
    assign weights_valid_out[i] = first_read_cycle_w & perform_operation_w;
  end
endgenerate

// modules

// every activation input port has an independent FIFO
generate
  for (i=0; i<NUM_DATA_INPUTS; i=i+1) begin
    FIFO #(
      .NUM_SLOTS     ( 4               ),
      .LOG_NUM_SLOTS ( 2               ),
      .DATA_WIDTH    ( GROUP_SIZE*DATA_WIDTH      )
    ) fifo_in_data (
      .clk           ( clk             ),
      .rst           ( rst             ),
      .data_write    ( data_write_w[i] ),
      .write         ( write_w[i]      ),
      .full          ( full_w[i]       ),
      .almost_full   ( almost_full_w[i]),
      .data_read     ( data_read_w[i]  ),
      .next_read     ( next_read_w[i]  ),
      .empty         ( empty_w[i]      )
    );
  end
endgenerate

// All input weights are stored in the same FIFO
FIFO #(
  .NUM_SLOTS         ( 4                           ),
  .LOG_NUM_SLOTS     ( 2                           ),
  .DATA_WIDTH        ( NUM_DATA_OUTPUTS*DATA_WIDTH )
) fifo_in_weights (
  .clk               ( clk                         ),
  .rst               ( rst                         ),
  .data_write        ( weights_data_write_w        ),
  .write             ( weights_write_w             ),
  .full              ( weights_full_w              ),
  .almost_full       ( weights_almost_full_w       ),
  .data_read         ( weights_data_read_w         ),
  .next_read         ( weights_next_read_w         ),
  .empty             ( weights_empty_w             )
);

// sequential logic

// configuration and iterations
//
always @ (posedge clk) begin
  if (~rst) begin
    num_iters_r          <= 0;
    num_reads_per_iter_r <= 0;
    conf_mode_r          <= `CONF_MODE_0;
    module_enabled_r     <= 1'b0;
  end else begin
    if (configure) begin
      num_iters_r               <= num_iters;
      num_reads_per_iter_r      <= num_reads_per_iter;
      num_reads_per_iter_copy_r <= num_reads_per_iter;
      conf_mode_r               <= conf_mode;
      module_enabled_r          <= 1'b1;
    end else begin
      if (perform_operation_w) begin
        if (num_reads_per_iter_r == 1) begin
          if (num_iters_r == 1) module_enabled_r <= 0;
          else begin
            num_iters_r          <= num_iters_r - 1;
            num_reads_per_iter_r <= num_reads_per_iter_copy_r;
          end
        end else begin
          num_reads_per_iter_r <= num_reads_per_iter_r - 1;
        end
      end
    end
  end 
end

// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented
// the module has a tics counter register to keep up with current cycle
//
// in this module whenever a "read" cycle is performed the associated information is shown as debug
//

// synthesis translate_off

`ifdef DEBUG_DISTRIBUTE_IN
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (perform_operation_w) $display("DISTRIBUTE_IN: cycle %d, first_cycle: %d activations: valid %b data %x . weights: valid %b data %x, first_cycle %d module enabled %d", tics, first_read_cycle_w, valid_out, data_out, weights_valid_out, weights_data_out, first_read_cycle_w, module_enabled_r);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on

endmodule

// Module DISTRIBUTE_OUT
//
// This module distributes input data to outputs based on specific configuration.
// The module has as NUM_DATA_INPUTS input ports. Every such input port
// has an independent FIFO. 
//
// The module works whenever there is data at its input. A "read" is consumed
// when all input data is available and the outputs are available (as well). On every "read" of a cycle
// we forward input data. 
// 
// Depending on the configuration mode the inputs may be reduced to the outputs or simply forwarded. In CONF_MODE_0 groups
// of 9 inputs are reduced and forwarded (inputs [0:8] to output 0, ...). In CONF_MODE_1 every input i is 
// forwarded to output i.
//
module DISTRIBUTE_OUT #(
    parameter NUM_DATA_INPUTS        = 8,        // number of data inputs
    parameter GROUP_SIZE             = 4,        // group size
    parameter DATA_WIDTH             = 8,        // input and output data width
    parameter NUM_DATA_OUTPUTS       = 8         // number of data outputs
) (
  input                                               clk,                  // clock signal
  input                                               rst,                  // reset signal

  input                                               configure,            // CONFIGURE interface:: configure signal
  input                                               conf_mode,            // CONFIGURE interface:: configuration mode

  input [NUM_DATA_INPUTS*GROUP_SIZE*DATA_WIDTH-1:0]   data_in,              // IN interface:: data
  input [NUM_DATA_INPUTS-1:0]                         valid_in,             // IN interface:: valid
  output [NUM_DATA_INPUTS-1:0]                        avail_out,            // IN interface:: avail

  output [NUM_DATA_OUTPUTS*GROUP_SIZE*DATA_WIDTH-1:0] data_out,             // OUT interface:: data
  output [NUM_DATA_OUTPUTS-1:0]                       valid_out,            // OUT interface:: valid
  input  [NUM_DATA_OUTPUTS-1:0]                       avail_in              // OUT interface:: avail
);

// wires
wire [(GROUP_SIZE*DATA_WIDTH) - 1: 0] data_write_w[NUM_DATA_INPUTS-1:0]; // data to write to FIFO
wire [NUM_DATA_INPUTS-1:0]            write_w;                           // write signal to FIFO
wire [NUM_DATA_INPUTS-1:0]            full_w;                            // full signal from FIFO
wire [NUM_DATA_INPUTS-1:0]            almost_full_w;                     // almost_full signal from FIFO
wire [(GROUP_SIZE*DATA_WIDTH) - 1: 0] data_read_w[NUM_DATA_INPUTS-1:0];  // data read from FIFO
wire [NUM_DATA_INPUTS-1:0]            next_read_w;                       // next_read signal to FIFO
wire [NUM_DATA_INPUTS-1:0]            empty_w;                           // empty signal from FIFO
//
wire perform_operation_w;                                                // when set indicates a "read" operation is performed (input data sent to output)
//
wire [(GROUP_SIZE*DATA_WIDTH)-1:0] added_inputs_w[NUM_DATA_INPUTS-1:0];  // added inputs (for CONF_MODE_0)

// registers
reg                              conf_mode_r;                            // configuration mode

genvar i;
genvar j;

// adders
generate
  assign added_inputs_w[0] = data_read_w[0];
  for (i=1; i<NUM_DATA_INPUTS; i=i+1) begin
    for (j=0; j<GROUP_SIZE; j=j+1) begin
      assign added_inputs_w[i][((j+1)*DATA_WIDTH)-1:j*DATA_WIDTH] = added_inputs_w[i-1][((j+1)*DATA_WIDTH)-1:j*DATA_WIDTH] + data_read_w[i][((j+1)*DATA_WIDTH)-1:j*DATA_WIDTH];
    end
  end
endgenerate

// combinational logic (FIFOs)
generate
  for (i=0; i<NUM_DATA_INPUTS; i=i+1) begin
    assign data_write_w[i] = data_in[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH];   // data to write to the FIFO
    assign write_w[i]      = valid_in[i];                                                        // FIFO write signal
    assign avail_out[i]    = ~almost_full_w[i] & ~full_w[i];                                     // avail signal from FIFO       
    assign next_read_w[i]  = perform_operation_w;
  end
endgenerate

// combinational logic (perform operation)
// perform_operation_w indicates whether in this cycle we perform the operation. We perform the operation 
// if we have all data at the inputs available and all outputs available
assign perform_operation_w = ~(|empty_w) & (&avail_in);

// combinational logic (outputs of the module)
// CONF_MODE_0: All inputs added and sent to output #0
// CONF_MODE_1: Every input i forwarded to output i
//
generate
  for (i=0; i<NUM_DATA_OUTPUTS;i=i+1) begin
    assign data_out[((i+1)*GROUP_SIZE*DATA_WIDTH)-1:i*GROUP_SIZE*DATA_WIDTH] = (conf_mode_r == `CONF_MODE_0) ? added_inputs_w[NUM_DATA_INPUTS-1] : (conf_mode_r == `CONF_MODE_1) ? data_read_w[i] : 0;
  end
  assign valid_out[0] = perform_operation_w;
  for (i=1; i<NUM_DATA_OUTPUTS;i=i+1) begin
    assign valid_out[i] = (conf_mode_r == `CONF_MODE_0) ? 1'b0 : perform_operation_w;
  end
endgenerate

// modules

// every input port has an independent FIFO
generate
  for (i=0; i<NUM_DATA_INPUTS; i=i+1) begin
    FIFO #(
      .NUM_SLOTS     ( 4               ),
      .LOG_NUM_SLOTS ( 2               ),
      .DATA_WIDTH    ( GROUP_SIZE*DATA_WIDTH      )
    ) fifo_in_data (
      .clk           ( clk             ),
      .rst           ( rst             ),
      .data_write    ( data_write_w[i] ),
      .write         ( write_w[i]      ),
      .full          ( full_w[i]       ),
      .almost_full   ( almost_full_w[i]),
      .data_read     ( data_read_w[i]  ),
      .next_read     ( next_read_w[i]  ),
      .empty         ( empty_w[i]      )
    );
  end
endgenerate

// sequential logic

// configuration and iterations
//
always @ (posedge clk) begin
  if (~rst) begin
    conf_mode_r          <= `CONF_MODE_0;
  end else begin
    if (configure) begin
      conf_mode_r               <= conf_mode;
    end
  end 
end

// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented
// the module has a tics counter register to keep up with current cycle
//
// in this module whenever a "read" cycle is performed the associated information is shown as debug
//

// synthesis translate_off

`ifdef DEBUG_DISTRIBUTE_OUT
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (perform_operation_w) $display("DISTRIBUTE_OUT: cycle %d, data forwarded: %x ai0 %x ai1 %x", tics, data_out, added_inputs_w[0], added_inputs_w[1]);
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on

endmodule
