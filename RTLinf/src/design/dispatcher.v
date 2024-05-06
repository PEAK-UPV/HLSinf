// DISPATCHER  module
//
// This module gets GROUP SIZE activations and sends only the unique values to the next module with the 
// corresponding weight to multiply.
// 

//`include "RTLinf.vh"

module DISPATCHER #(
  parameter GROUP_SIZE              = 4,                                   // group size
  parameter DATA_WIDTH              = 8,                                   // input data width (output is 2x input width)
  localparam REP_INFO               = GROUP_SIZE*GROUP_SIZE,               // number of bits for repetition detector
  localparam INPUT_WIDTH            = GROUP_SIZE * DATA_WIDTH + REP_INFO,  // number of bits for input (activation + rep. info)
  localparam OUTPUT_WIDTH           = DATA_WIDTH + REP_INFO + 1 //only one           // number of bits for output ( result + rep. info)
)( 
  input clk,
  input rst,

  input [INPUT_WIDTH - 1 : 0]              act_data_in,             // ACTIVATION interface:: activations data
  input                                    act_valid_in,            // ACTIVATION interface:: activation valid in
  output                                   act_avail_out,           // ACTIVATION interface:: avail

  output [OUTPUT_WIDTH - 1 : 0]           data_out,                 // OUT interface: data (act + weight + rep_info)
  output                                  valid_out,                // OUT interface: valid
  input                                   avail_in                  // OUT interface: avail
);


// wires
genvar i;

//FIFOs
wire                        perform_operation_w;                  // whether we perform a "read" operation in this cycle

reg [INPUT_WIDTH-1:0] act_data_r;
reg                   act_valid_r;


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
assign perform_operation_w = act_valid_r;
                    
// output data
assign valid_out                             = perform_operation_w;
assign data_out[DATA_WIDTH-1:0]              = data_w[enabled_w];
assign data_out[OUTPUT_WIDTH-2:DATA_WIDTH] = act_data_r[INPUT_WIDTH-1:DATA_WIDTH*GROUP_SIZE];
assign data_out[OUTPUT_WIDTH-1] = only_one_w;
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


// ACT FIFO :: write and read
assign act_data_write_w  = act_data_in;
assign act_write_w       = act_valid_in;
assign act_avail_out     = 1'b1; // always available    //~act_almost_full_w && ~act_full_w;
assign act_next_read_w   = next_read_w;


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

        if(valid_out)$display("---Output data : \n Act: %d", tics, data_out[DATA_WIDTH-1:0]);
      end
      tics <= tics + 1;
    end
  end
`endif

// synthesis translate_on

endmodule
