// AGR module
//
// This module agrupates the results to obtain GROUP SIZE results. This module can send a batch each 1 - GROUP_SIZE cicle.
//

`include "RTLinf.vh"


module AGRUPATE#(
  parameter GROUP_SIZE             = 4,                          // group size
  parameter LOG_GROUP_SIZE         = 2,                          // number of bits forgroup size
  parameter DATA_WIDTH             = (2 * 8),                    // input value width (output is 2x input width)
  parameter LOG_MAX_ITERS          = 16,                         // number of bits for max iters register
  parameter LOG_MAX_READS_PER_ITER = 16,                         // number of bits for max reads per iter
  localparam ZERO_INFO             = GROUP_SIZE*GROUP_SIZE,      // number of bits for repetition detectoor
  localparam INPUT_WIDTH           = DATA_WIDTH + ZERO_INFO,      // input data width (activation + weight + rep. info)
  localparam OUTPUT_WIDTH          = GROUP_SIZE * DATA_WIDTH     // output data width ( result (2*data width) +  rep. info)

)(
  input clk,
  input rst,

  input                                     configure,               // CONFIGURE interface:: configure signal
  input [LOG_MAX_ITERS - 1 : 0]             num_iters,               // CONFIGURE interface:: number of iterations for reads
  input [LOG_MAX_READS_PER_ITER - 1 : 0]    num_reads_per_iter,      // CONFIGURE interface:: number of reads per iteration

  input [INPUT_WIDTH - 1 : 0]               data_in,                 // ACTIVATION & WEIGHT interface:: activ ations data
  input                                     valid_in,                // ACTIVATION & WEIGHT interface:: activ ation valid in
  output                                    avail_out,               // ACTIVATION & WEIGHT interface:: avail 

  output [OUTPUT_WIDTH - 1 : 0]             data_out,                // OUT interface: data
  output                                    valid_out,               // OUT interface: valid
  input                                     avail_in                 // OUT interface: avail
);

// wires
wire [INPUT_WIDTH - 1: 0]          data_write_w;                      // data to write to FIFO
wire                               write_w;                           // write signal to FIFO
wire                               full_w;                            // full signal from FIFO
wire                               almost_full_w;                     // almost_full signal from FIFO
wire [INPUT_WIDTH- 1: 0]           data_read_w;                       // data read from FIFO
wire                               next_read_w;                       // next_read signal to FIFO
wire                               empty_w;                           // empty signal from FIFO
wire                               perform_operation_w;               // whether we perform a "read" operation in this cycle

wire [ DATA_WIDTH - 1: 0]          value_in;                         // Result value extracted from FIFO   
wire [ ZERO_INFO - 1: 0]            zero_info;                         // Repetition info extracted from FIFO
wire [ GROUP_SIZE - 1: 0]          processed_elements;               // Indicates the stored elements + elements that will be stored
wire [ GROUP_SIZE - 1: 0]          writing;                          // Indicates the elements that will be stored in the current cycle
wire [ GROUP_SIZE - 1: 0]          diagonal;                         // Diagonal of the zero_info
wire                                 send;            //TODO

// registers
reg [LOG_MAX_ITERS-1:0]          num_iters_r;               // FIFO
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_r;      // number of reads per iteration (down counter)
reg [LOG_MAX_READS_PER_ITER-1:0] num_reads_per_iter_copy_r; // copy of number of reads per iteration
reg                              module_enabled_r;          // module enabled

reg [GROUP_SIZE - 1 : 0]         stored;                                // Indicate the stored elements
reg [GROUP_SIZE - 1 : 0]         enable;                                // Each bit indicates if the element needs to be stored or has already been stored

reg [GROUP_SIZE - 1 : 0]         enable_r;                                // Each bit indicates if the element needs to be stored or has already been stored
reg [LOG_GROUP_SIZE : 0]         row;                                   // Indicates the row actual to process
reg [DATA_WIDTH - 1: 0]          output_values[GROUP_SIZE - 1 : 0];     // Unpacked output values
reg                              first_iter;                            //Indicates if we are on the first iteration of a batch
reg                              send_r;                                //Registered version of send variable

genvar i;
integer j;

// ** Combinational logic **
assign perform_operation_w = /*module_enabled_r &*/(~empty_w || (|enable_r)) & avail_in;

// FIFO write and read
assign data_write_w = data_in;
assign write_w = valid_in;
assign avail_out = ~almost_full_w & ~full_w;
assign next_read_w = perform_operation_w;

// Extract input from FIFO
assign value_in  = data_read_w[DATA_WIDTH - 1 : 0];
assign zero_info  = data_read_w[INPUT_WIDTH - 1 : DATA_WIDTH];

// Assign Output
for(i = 0; i < GROUP_SIZE; i = i + 1) begin
    assign data_out[i * DATA_WIDTH +: DATA_WIDTH ] = output_values[i]; 
end
assign valid_out = perform_operation_w & send_r; 

// if all the elements are stored in this cycle it means that we need to send the batch
assign send = &processed_elements; 

//Get the elements to write in this cycle.
for(i = 0; i < GROUP_SIZE; i = i + 1) begin
    assign writing[i] = zero_info[i];
    assign processed_elements[i] = stored[i] + writing[i];
end

//See which element has to be stored next. The next element to store needs to be the one
//with the minor index.
always @ (*) 
begin:row_always
    row = 0;
    for(j = GROUP_SIZE - 1; j >= 0; j = j - 1) begin
        row = (enable[j])? j : row;
    end 
end


// **Sequential logic**

always @ (posedge clk) 
begin
    if (~rst) begin
        stored <= 0;
        first_iter <= 1;
        send_r <= 0;
        enable_r <= 0;
    end else begin
        if(perform_operation_w)begin
            enable_r <= enable;
            send_r <= send;

            for(j = 0; j < GROUP_SIZE; j =  j + 1) begin
                if(writing[j]) begin
                    output_values[j] <= value_in;
                    //stored[j] <= 1;
                end // End if writing
            end // End for j      

            if(send) begin
                first_iter <= 1;
                stored <= 0;
            end // End send

            else begin
                first_iter <= 0; 
                
                for(j = 0; j < GROUP_SIZE; j =  j + 1) begin
                    if(writing[j]) begin
                        output_values[j] <= value_in;
                        stored[j] <= 1;
                    end // End if writing
                end // End for j       
            end // End else 
        end // End if perform_operation_w
    end // End if else reset
end

// *Enable calculation*
// Enable indicates which elements are still enable to process. On the first iteration of
// a batch, we need to redefine the enable to the diagonal of the rep_info (which indicates 
// which elements have an unique value). Then, if we have stored an elements, we need to deactivate
// that element.
//
always @ (posedge clk) 
begin:enable_calculation
    if (~rst) begin
        enable <= 0;
    end else begin
        if(perform_operation_w) begin
            for(j = 1; j < GROUP_SIZE; j =  j + 1) begin
                if(first_iter) begin
                    enable[j] <= !zero_info[j]; 
                end
                else begin
                    if(writing[j]) enable[j] <= 0;   
                end
            end // End for j
        end // End if perform_operation_w
    end // End if else reset
end

// configuration and iterations
// whenever we perform a "read" operation we decrement the number of reads per iteration
// When the reads per iteration reaches zero we decrement number of iterations and restore
// the reads per iteration. If number of iterations reaches zero
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
      if (perform_operation_w) begin
        if (num_reads_per_iter_r == 1) begin
          if (num_iters_r == 1) module_enabled_r <= 0;
          else begin
            num_iters_r <= num_iters_r - 1;
            num_reads_per_iter_r <= num_reads_per_iter_copy_r;
          end
        end else begin
          if(send) num_reads_per_iter_r <= num_reads_per_iter_r - 1;
        end
      end
    end
  end 
end



// Modules

// FIFO
FIFO #(
  .NUM_SLOTS     ( 4                       ),
  .LOG_NUM_SLOTS ( 2                       ),
  .DATA_WIDTH    ( INPUT_WIDTH             )
) fifo_in (
  .clk           ( clk                     ),
  .rst           ( rst                     ),
  .data_write    ( data_write_w            ),
  .write         ( write_w                 ),
  .full          ( full_w                  ),
  .almost_full   ( almost_full_w           ),
  .data_read     ( data_read_w             ),
  .next_read     ( next_read_w             ),
  .empty         ( empty_w                 )
);


endmodule
