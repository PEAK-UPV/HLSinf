// DISPATCHER  module
//
// This module gets GROUP SIZE activations and sends only the unique values to the next module with the 
// corresponding weight to multiply.
// 

//`include "RTLinf.vh"

module DISPATCHER #(
  parameter GROUP_SIZE             = 4,                                   // group size
  parameter DATA_WIDTH             = 8,                                   // input data width (output is 2x input width)
  parameter LOG_MAX_ITERS          = 16,                                  // number of bits for max iters register
  parameter LOG_MAX_READS_PER_ITER = 16,                                  // number of bits for max reads per iter
  localparam REP_INFO               = GROUP_SIZE*GROUP_SIZE,               // number of bits for repetition detector
  localparam INPUT_WIDTH            = GROUP_SIZE * DATA_WIDTH + REP_INFO,  // number of bits for input (activation + weight + rep. info)
  localparam OUTPUT_WIDTH           = 2 * DATA_WIDTH + REP_INFO                // number of bits for output ( result + weight + rep. info)
)( 
  input clk,
  input rst,

  input                                    configure,               // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS-1:0]                num_iters,               // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER-1:0]       num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration

  input [INPUT_WIDTH - 1 : 0]              act_data_in,             // ACTIVATION interface:: activations data
  input                                    act_valid_in,            // ACTIVATION interface:: activation valid in
  output                                   act_avail_out,           // ACTIVATION interface:: avail

  input [DATA_WIDTH - 1 : 0]               weight_data_in,          // WEIGHT interface:: weight data
  input                                    weight_valid_in,         // WEIGHT interface:: weight valid in
  output                                   weight_avail_out,        // WEIGHT interface:: avail

  output [OUTPUT_WIDTH - 1 : 0]           data_out,                 // OUT interface: data (act + weight + rep_info)
  output                                  valid_out,                // OUT interface: valid
  input                                   avail_in                  // OUT interface: avail
);


// wires
genvar i;

//FIFOs
wire [DATA_WIDTH -1 : 0]    weight_data_write_w;        // WEIGHT FIFO :: data to write to FIFO
wire                        weight_write_w;             // WEIGHT FIFO :: write signal to FIFO
wire                        weight_full_w;              // WEIGHT FIFO :: full signal from FIFO
wire                        weight_almost_full_w;       // WEIGHT FIFO :: almost_full signal from FIFO
wire [DATA_WIDTH -1 : 0]    weight_data_read_fifo;      // WEIGHT FIFO :: data read from FIFO
wire                        weight_next_read_w;         // WEIGHT FIFO :: next_read signal to FIFO
wire                        weight_empty_w;             // WEIGHT FIFO :: empty signal from FIFO

wire                        perform_operation_w;                  // whether we perform a "read" operation in this cycle

reg [INPUT_WIDTH-1:0] act_data_r;
reg act_valid_r;


// Activations from fifo
wire [DATA_WIDTH-1:0] data_w[GROUP_SIZE-1:0];
for (i=0; i<GROUP_SIZE; i=i+1) begin
  assign data_w[i] = act_data_r[(DATA_WIDTH*(i+1))-1 : i*DATA_WIDTH];
end

// Repetition info from fifo
wire [GROUP_SIZE-1:0] rep_w[GROUP_SIZE-1:0];
for (i=0; i<GROUP_SIZE; i=i+1) begin
  assign rep_w[i] = act_data_r[(DATA_WIDTH*GROUP_SIZE)+(GROUP_SIZE*(i+1))-1 : (DATA_WIDTH*GROUP_SIZE)+(GROUP_SIZE*i)];
end


// Enable register bits for each repetition vector
reg [GROUP_SIZE-1:0] rep_enable_r;

// Enable bits for each repetition vector
wire [GROUP_SIZE-1:0] rep_enable_w;
for (i=0; i<GROUP_SIZE;i=i+1) begin
  assign rep_enable_w[i] = rep_w[i][i] & rep_enable_r[i];
end

// enabled entry to send
wire [1:0] enabled_w;
assign enabled_w = rep_enable_w[0] ? 0 : rep_enable_w[1] ? 1 : rep_enable_w[2] ? 2 : 3;

// only one remaining
wire only_one_w;
assign only_one_w = ( rep_enable_w[3] & ~rep_enable_w[2] & ~rep_enable_w[1] & ~rep_enable_w[0]) |
                    (~rep_enable_w[3] &  rep_enable_w[2] & ~rep_enable_w[1] & ~rep_enable_w[0]) |
                    (~rep_enable_w[3] & ~rep_enable_w[2] &  rep_enable_w[1] & ~rep_enable_w[0]) |
                    (~rep_enable_w[3] & ~rep_enable_w[2] & ~rep_enable_w[1] &  rep_enable_w[0]);         
  

always @ (posedge clk) begin
  if (~rst) begin
    act_data_r <= 0;
    act_valid_r <= 0;
  end else begin
    if (act_valid_in) begin
      act_valid_r <= act_valid_in;
      act_data_r  <= act_data_in;
    end else begin 
      if (only_one_w) begin
        act_valid_r <= 0;
      end
    end 
  end
end

// whether if we perform an operation
assign perform_operation_w = act_valid_r; // | (|rep_enable_w));
                    
// output data
assign valid_out                             = perform_operation_w;
assign data_out[DATA_WIDTH-1:0]              = data_w[enabled_w];
assign data_out[2*DATA_WIDTH-1:DATA_WIDTH]   = weight_data_read_fifo;
assign data_out[OUTPUT_WIDTH-1:2*DATA_WIDTH] = act_data_r[INPUT_WIDTH-1:DATA_WIDTH*GROUP_SIZE];

// next read to the fifo
wire next_read_w;
assign next_read_w = perform_operation_w & only_one_w;


always @ (posedge clk) begin
 if (~rst) begin
   rep_enable_r <= 4'b1111;
 end else begin
   if (next_read_w) begin
     rep_enable_r[0] <= 1'b1;
     rep_enable_r[1] <= 1'b1;
     rep_enable_r[2] <= 1'b1;
     rep_enable_r[3] <= 1'b1;
   end else begin
     if (valid_out) begin
       if (rep_enable_w[0]) rep_enable_r[0] <= 1'b0;
       else if (rep_enable_w[1]) rep_enable_r[1] <= 1'b0;
       else if (rep_enable_w[2]) rep_enable_r[2] <= 1'b0;
       else if (rep_enable_w[3]) rep_enable_r[3] <= 1'b0;
     end
   end
 end
end




// registers
reg [LOG_MAX_ITERS-1:0]          num_iters_r;               // 
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_copy_r; // copy of number of reads per iteration
reg                              module_enabled_r;          // module enabled

reg [GROUP_SIZE-1:0]             enable;                    // Indicates if element i is an unique value and needs to be proccesed or not 
reg [DATA_WIDTH-1:0]             send_next;                 // Next element to send. 
reg [DATA_WIDTH-1:0]             left_elements;             // Number of elements from batch to send
reg                              first_iter;                // Indicates if is the first batch and don't need to activate read_next
reg                              one_un_v;                  //indicates if the batch only have one unique element and the next_read needs to be selected

integer j;

// ACT FIFO :: write and read
assign act_data_write_w  = act_data_in;
assign act_write_w       = act_valid_in;
assign act_avail_out     = 1'b1; // always available    //~act_almost_full_w && ~act_full_w;
assign act_next_read_w   = next_read_w;

// WEIGHT FIFO :: write and read
assign weight_data_write_w  = weight_data_in;
assign weight_write_w       = weight_valid_in;
assign weight_avail_out     = ~weight_almost_full_w && ~weight_full_w;
assign weight_next_read_w   = perform_operation_w  & (num_reads_per_iter_r == 1) & next_read_w;

// modules

// weights fifo
FIFO #(
  .NUM_SLOTS     ( 4          ),
  .LOG_NUM_SLOTS ( 2          ),
  .DATA_WIDTH    ( DATA_WIDTH )
) fifo_in_weight (
  .clk           ( clk                    ),
  .rst           ( rst                    ),
  .data_write    ( weight_data_write_w    ),
  .write         ( weight_write_w         ),
  .full          ( weight_full_w          ),
  .almost_full   ( weight_almost_full_w   ),
  .data_read     ( weight_data_read_fifo  ),
  .next_read     ( weight_next_read_w     ),
  .empty         ( weight_empty_w         )
);
// sequential logic

// configuration and iterations
// then we disable the module. 
//
always @ (posedge clk) begin
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
      if (next_read_w) begin //perform_operation_w) begin
        if (num_reads_per_iter_r == 1) begin
          if (num_iters_r == 1) module_enabled_r <= 0;
          else begin
            num_iters_r <= num_iters_r - 1;
            num_reads_per_iter_r <= num_reads_per_iter_copy_r;
          end
        end else begin
          ///if(left_elements <= 1) num_reads_per_iter_r <= num_reads_per_iter_r - 1;
          if (next_read_w) num_reads_per_iter_r <= num_reads_per_iter_r - 1;
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

`ifdef DEBUG_DISPATCHER 
  reg [15:0] tics;
    integer k,l;
  always @ (posedge clk) begin
    if (~rst) tics <= 0;
    else begin
      if (perform_operation_w) begin
        $display("\nREP: cycle %d", tics);

        $display("--Values_in:");
        for(k = 0; k < GROUP_SIZE; k = k + 1) begin
              $write("%d ", act_data_in_unpacked[k]);
        end

        $display("\n--Repetition info");
        for(k = 0; k < GROUP_SIZE; k = k + 1) begin
          for(l = 0; l < GROUP_SIZE; l = l + 1) begin   
              $write("%d ", rep_info[k*GROUP_SIZE + l]);
          end
           $write("\n");
        end

        if(valid_out)$display("---Output data : \n Act: %d Weight %d", tics, data_out[DATA_WIDTH-1:0], data_out[DATA_WIDTH+DATA_WIDTH-1:DATA_WIDTH]);
      end
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on

endmodule
