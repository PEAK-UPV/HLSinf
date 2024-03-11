// Module repetition_detector
//
// This module implements the required logic to find the unique elements within a group
// of activations.
//

`include "RTLinf.vh"


module repetition_detector#(
    parameter GROUP_SIZE             = 4,                      // group size
    parameter DATA_WIDTH             = 8,                      // input and output data width
    parameter LOG_MAX_ITERS          = 16,                     // number of bits for max iters register
    parameter LOG_MAX_READS_PER_ITER = 16,                     // number of bits for max reads per iter
    localparam REP_INFO              = GROUP_SIZE*GROUP_SIZE,  // number of bits for repetition detector
    localparam ZERO_INFO             = GROUP_SIZE,             //
    localparam INPUT_WIDTH           = GROUP_SIZE*DATA_WIDTH,  // number of bits for input (activation + weight + rep. info)
    localparam OUTPUT_WIDTH          = INPUT_WIDTH + REP_INFO +  ZERO_INFO // number of bits for output ( result + weight + rep. info)    
)(
  input clk,
  input rst,

  input                               configure,           // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]           num_iters,           // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0]  num_reads_per_iter,  // CONFIGURE interface:: number of reads per iteration  

  input [INPUT_WIDTH-1:0]             data_in,     // ACTIVATION interface:: data
  input                               valid_in,    // ACTIVATION interface:: valid
  output                              avail_out,   // ACTIVATION interface:: avail

  output [OUTPUT_WIDTH-1:0]           data_out,    // OUT1 interface:: data
  output                              valid_out,   // OUT1 interface:: valid
  input                               avail_in     // OUT1 interface:: avail

);


// wires
wire [INPUT_WIDTH - 1: 0]              data_write_w;                      // data to write to FIFO
wire                                   write_enb_w;                       // write signal to FIFO
wire                                   full_w;                            // full signal from FIFO
wire                                   almost_full_w;                     // almost_full signal from FIFO
wire [INPUT_WIDTH - 1 : 0]             data_read_fifo;                    // data read from FIFO
wire                                   read_enb_w;                        // next_read signal to FIFO
wire                                   empty_w;                           // empty signal from FIFO
wire                                   perform_operation_w;

wire [REP_INFO - 1 : 0]                equivalences;                      // matrix of equivalences between the values of the GS elements
wire [REP_INFO - 1 : 0]                rep_info;                          // matrix with condensed repetition detection information
wire [ZERO_INFO - 1 : 0]               zer_info;                          // vector of zero elements
wire [DATA_WIDTH - 1 : 0]              data_in_unpacked[GROUP_SIZE-1:0];  // two dimentional data read from FIFO

// registers
reg [LOG_MAX_ITERS - 1 : 0]            num_iters_r;               // FIFO
reg [LOG_MAX_READS_PER_ITER - 1 : 0]   num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER - 1 : 0]   num_reads_per_iter_copy_r; // copy of number of reads per iteration
reg                                    module_enabled_r;          // module enabled
reg [GROUP_SIZE-1:0]                   diag;                      // diagonal of the rep info matrix

genvar i;
genvar j;
integer k;
integer l;

// combinational logic
assign data_out[GROUP_SIZE * DATA_WIDTH - 1:0] = data_read_fifo;
assign data_out[GROUP_SIZE * DATA_WIDTH + REP_INFO - 1 : GROUP_SIZE * DATA_WIDTH] = rep_info;
assign data_out[OUTPUT_WIDTH - 1 : GROUP_SIZE * DATA_WIDTH + REP_INFO] = zer_info;

assign data_write_w  = data_in;                                         // data to FIFO
assign write_enb_w   = valid_in;                                        // write signal to FIFO
assign avail_out     = ~almost_full_w & ~full_w;                        // avail signal from FIFO       
assign read_enb_w    = perform_operation_w;                             // next_read signal to FIFO
assign valid_out     = perform_operation_w;                             // valid signal to downstream module
//
assign perform_operation_w = module_enabled_r & (~empty_w) & avail_in;


for(i = 0; i < GROUP_SIZE; i = i + 1) begin
    assign zer_info[i] = data_in_unpacked[i] == 0;
    assign data_in_unpacked[i] = data_read_fifo[i * DATA_WIDTH +: DATA_WIDTH];
end

/*Repetition information calculation*/
//Calculation of the repetition information performed in several steps.
// The output represents a matrix of GS x GS, where each row i represents the equivalence of the 
//element i by the element j. There will be only GS ones. 
// E.g., 3 2 3 3 will result on:
// 1 0 1 1
// 0 1 0 0
// 0 0 0 0
// 0 0 0 0


//--Step 1: calculate the equivalences.
// E.g., 3 2 3 3 will result on: 
// 1 0 1 1
// 0 1 0 0
// 0 0 1 1
// 0 0 0 1
for(i = 0; i < GROUP_SIZE; i = i + 1) begin
  for(j = 0; j < GROUP_SIZE; j = j + 1) begin
        /*EQUIVALENCES*/
        //First we build the upper part of the matrix checking if all elements are equal or not. For this, we compare the actual element selected with
        //the following values, and then we create a bit matrix where each row are related to an element. 
        //E.g., For A B A the maxtrix would be: // x 0 1 // 0 x 0 // 0 x 0
        if(i == j) assign equivalences[i*GROUP_SIZE+j] = 1;                             //diagonal
        if(j > i)  assign equivalences[i*GROUP_SIZE+j] = !(data_in_unpacked[i] ^ data_in_unpacked[j]);  //upper part
        if(j < i)  assign equivalences[i*GROUP_SIZE+j] = 0;                             //lower part
    end
end

//--Step 2: calculate the diagonal of the matrix.
// E.g., 3 2 3 3 will result on:
// 1 0 0 0
// 0 1 0 0
// 0 0 0 0
// 0 0 0 0
always @ (*) begin
  diag = {GROUP_SIZE{1'b1}}; ;
  for(k = 1; k < GROUP_SIZE; k = k + 1) begin
    for(l = 0; l < k; l = l + 1) begin   
      diag[k] = diag[k] & !equivalences[l*GROUP_SIZE+k];
    end
  end
end

//--Step 3: bind step 1 and step 2
for(i = 0; i < GROUP_SIZE; i = i + 1) begin
  for(j = 0; j < GROUP_SIZE; j = j + 1) begin
        if(j >= i) assign rep_info[i*GROUP_SIZE+j] = diag[i] && equivalences[i*GROUP_SIZE+j];  //upper part
        if(j < i)  assign rep_info[i*GROUP_SIZE+j] = 0;                                        //lower part
    end
end

// sequential logic
always @ (posedge clk) 
begin: ITEARATION_CONTROL
  if (~rst) begin
    num_iters_r          <= 0;
    num_reads_per_iter_r <= 0;
    module_enabled_r     <= 1'b0;
  end else begin
    if (configure) begin
      num_iters_r          <= num_iters;
      num_reads_per_iter_r <= num_reads_per_iter;
      num_reads_per_iter_copy_r <= num_reads_per_iter;
      module_enabled_r     <= 1'b1;
    end else begin
      if (perform_operation_w) begin   // when we trigger a read to bram update the counters for iteration control
        if (num_reads_per_iter_r == 1) begin
          if (num_iters_r == 1) begin
            module_enabled_r <= 0; 
          end else begin
            num_iters_r <= num_iters_r - 1;
            num_reads_per_iter_r <= num_reads_per_iter_copy_r;
          end
        end else begin
          num_reads_per_iter_r <= num_reads_per_iter_r - 1;
        end
      end
    end
  end 
end


/* Modules */

// FIFO
FIFO #(
  .NUM_SLOTS     ( 4                       ),
  .LOG_NUM_SLOTS ( 2                       ),
  .DATA_WIDTH    ( DATA_WIDTH * GROUP_SIZE )
) fifo_in (
  .clk           ( clk             ),
  .rst           ( rst             ),
  .data_write    ( data_write_w    ),
  .write         ( write_enb_w     ),
  .full          ( full_w          ),
  .almost_full   ( almost_full_w   ),
  .data_read     ( data_read_fifo  ),
  .next_read     ( read_enb_w      ),
  .empty         ( empty_w         )
);


// debug support. When enabled (through the DEBUG define) the module will generate
// debug information on every specific cycle, depending on the debug conditions implemented
// the module has a tics counter register to keep up with current cycle
//
// in this module whenever a "read" cycle is performed the associated information is shown as debug
//


`ifdef DEBUG_REP
  reg [15:0] tics;

  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (perform_operation_w) begin
        $display("REP: cycle %d output data %x", tics, data_out);

        $display("--Values:");
        for(k = 0; k < GROUP_SIZE; k = k + 1) begin
              $write("%d ", data_in_unpacked[k]);
        end

        $display("\n--Step one, equivalences matrix");
        for(k = 0; k < GROUP_SIZE; k = k + 1) begin
          for(l = 0; l < GROUP_SIZE; l = l + 1) begin   
              $write("%d ", equivalences[k*GROUP_SIZE + l]);
          end
           $write("\n");
        end

        $display("--Step two, diagonal");
        for(k = 0; k < GROUP_SIZE; k = k + 1) begin
              $write("%d ", diag[k]);
        end

        $display("\n--Step three, final info");
        for(k = 0; k < GROUP_SIZE; k = k + 1) begin
          for(l = 0; l < GROUP_SIZE; l = l + 1) begin   
              $write("%d ", rep_info[k*GROUP_SIZE + l]);
          end
           $write("\n");
        end
      end
      tics <= tics + 1;
    end
  end
`endif

endmodule