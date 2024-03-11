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
  localparam REP_INFO               = GROUP_SIZE*GROUP_SIZE,              // number of bits for repetition detector
  localparam ZERO_INFO              = GROUP_SIZE,                         //  
  localparam INPUT_WIDTH            = GROUP_SIZE * DATA_WIDTH + REP_INFO + ZERO_INFO,  // number of bits for input (activation + weight + rep. info)
  localparam OUTPUT_WIDTH           = 2 * DATA_WIDTH + REP_INFO + ZERO_INFO            // number of bits for output ( result + weight + rep. info)
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

//FIFOs
wire [INPUT_WIDTH - 1: 0]   act_data_write_w;           // ACTIVATION FIFO :: data to write to FIFO
wire                        act_write_w;                // ACTIVATION FIFO ::write signal to FIFO
wire                        act_full_w;                 // ACTIVATION FIFO ::full signal from FIFO
wire                        act_almost_full_w;          // ACTIVATION FIFO ::almost_full signal from FIFO
wire [INPUT_WIDTH - 1: 0]   act_data_read_fifo;         // ACTIVATION FIFO ::data read from FIFO
wire                        act_next_read_w;            // ACTIVATION FIFO ::next_read signal to FIFO
wire                        act_empty_w;                // ACTIVATION FIFO ::empty signal from FIFO

wire [DATA_WIDTH -1 : 0]    weight_data_write_w;        // WEIGHT FIFO :: data to write to FIFO
wire                        weight_write_w;             // WEIGHT FIFO :: write signal to FIFO
wire                        weight_full_w;              // WEIGHT FIFO :: full signal from FIFO
wire                        weight_almost_full_w;       // WEIGHT FIFO :: almost_full signal from FIFO
wire [DATA_WIDTH -1 : 0]    weight_data_read_fifo;      // WEIGHT FIFO :: data read from FIFO
wire                        weight_next_read_w;         // WEIGHT FIFO :: next_read signal to FIFO
wire                        weight_empty_w;             // WEIGHT FIFO :: empty signal from FIFO

wire                        perform_operation_w;                  // whether we perform a "read" operation in this cycle
wire [REP_INFO - 1 : 0]     rep_info;                             // matrix with condensed repetition detection information
wire [ZERO_INFO - 1 : 0]    zer_info;                             // vextor of zeros
wire [REP_INFO - 1 : 0]     rep_info_filtered;                    // matrix with condensed repetition detection + zeros information

wire [DATA_WIDTH - 1 : 0]   act_data_in_unpacked[GROUP_SIZE-1:0]; // two dimentional data read from FIFO
wire [DATA_WIDTH - 1 : 0]   act_out;                              // Activation to send
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
reg [DATA_WIDTH-1:0]             first_to_send;             //indicates the first element to send

genvar i;
integer j;

//we have to perform an operation if we are still proccesing a batch or there exist a new batch to process
assign perform_operation_w = /*module_enabled_r &&*/ avail_in && ((~act_empty_w) || (left_elements > 0)) ;
assign valid_out = perform_operation_w;

//Assign output
assign act_out                                      = act_data_in_unpacked[send_next]; 
assign data_out[DATA_WIDTH - 1 : 0]                 = act_out;                //Activation
assign data_out[2 * DATA_WIDTH - 1 : DATA_WIDTH]    = weight_data_read_fifo;                           // Weight
assign data_out[2 * DATA_WIDTH + REP_INFO - 1 : 2 * DATA_WIDTH]  = rep_info;//[send_next * REP_INFO]; //Rep info
assign data_out[OUTPUT_WIDTH- 1 : 2 * DATA_WIDTH + REP_INFO] = zer_info;//[send_next * REP_INFO]; //Rep info

// ACT FIFO :: write and read
assign act_data_write_w  = act_data_in;
assign act_write_w       = act_valid_in;
assign act_avail_out     = ~act_almost_full_w && ~act_full_w;
assign act_next_read_w   = perform_operation_w && (one_un_v || (~first_iter && (left_elements <= 1)));


// WEIGHT FIFO :: write and read
assign weight_data_write_w  = weight_data_in;
assign weight_write_w       = weight_valid_in;
assign weight_avail_out     = ~weight_almost_full_w && ~weight_full_w;
assign weight_next_read_w   = perform_operation_w  && (num_reads_per_iter_r == 1);

for(i = 0; i < GROUP_SIZE; i = i + 1) begin
    //Unpack activation input data
    assign act_data_in_unpacked[i] = act_data_read_fifo[i * DATA_WIDTH +: DATA_WIDTH];
    assign rep_info_filtered[i*GROUP_SIZE+GROUP_SIZE-1:i*GROUP_SIZE] = zer_info[i]? {GROUP_SIZE{1'b0}} :  rep_info[i*GROUP_SIZE+GROUP_SIZE-1:i*GROUP_SIZE];
end
assign rep_info = act_data_read_fifo[GROUP_SIZE * DATA_WIDTH + REP_INFO - 1 : GROUP_SIZE * DATA_WIDTH] ;
assign zer_info = act_data_read_fifo[INPUT_WIDTH - 1 : GROUP_SIZE * DATA_WIDTH + REP_INFO] ;

//Combinational circuits

//See which element has to be sended next. The next element to send needs to be the one
//with the minor index.
always @ (*) 
begin: send_next_always
    first_to_send = 0;
    for(j = GROUP_SIZE - 1; j >= 0; j = j - 1) begin
        first_to_send = (!zer_info[j])? j : first_to_send;
    end
    send_next = first_to_send;
    for(j = GROUP_SIZE - 1; j >= 0; j = j - 1) begin
        send_next = (enable[j])? j : send_next;
    end 
end

//Allow next_read if unique values in a batch = 1 and avoid bubbles
always @ (*) 
begin: one_un_v_always
    one_un_v = 1;
    for(j = 1; j < GROUP_SIZE; j = j + 1) begin
        if(rep_info_filtered[j * GROUP_SIZE + j]) one_un_v = 0;
    end 
end

//Get the number of elements left to send
always @ (*) 
begin: left_elements_always
  left_elements = 0;
  if(!first_iter) begin
    for(j = GROUP_SIZE; j > 0; j = j - 1) begin
      if(enable[j]) left_elements = left_elements + 1;
    end 
  end
end

// sequential logic

//Get the elements that we need to send from the current batch
always @ (posedge clk) 
begin: enables_clk
  if(~rst) begin
    enable <= {GROUP_SIZE{1'b1}};
    first_iter <= 1;
  end else
    begin
      if(perform_operation_w)begin
        if (act_next_read_w) first_iter <= 1;
        else first_iter <= 0;
       
        enable[send_next] <= 0; 
         
         //If is the first batch iteration iteration reset the enable
        if(first_iter) begin
          for(j = 0; j < GROUP_SIZE; j = j + 1) begin
            enable [j] <= rep_info_filtered[j * GROUP_SIZE + j];
          end 
          enable [send_next] <= 0;
        end
      end //End perform_operation_w
    end
end

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
      if (perform_operation_w) begin
        if (num_reads_per_iter_r == 1) begin
          if (num_iters_r == 1) module_enabled_r <= 0;
          else begin
            num_iters_r <= num_iters_r - 1;
            num_reads_per_iter_r <= num_reads_per_iter_copy_r;
          end
        end else begin
          if(left_elements <= 1) num_reads_per_iter_r <= num_reads_per_iter_r - 1;
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


// modules

// ACT fifo
FIFO #(
  .NUM_SLOTS     ( 4                  ),
  .LOG_NUM_SLOTS ( 2                  ),
  .DATA_WIDTH    ( INPUT_WIDTH        )
) fifo_in_act (
  .clk           ( clk                ),
  .rst           ( rst                ),
  .data_write    ( act_data_write_w   ),
  .write         ( act_write_w        ),
  .full          ( act_full_w         ),
  .almost_full   ( act_almost_full_w  ),
  .data_read     ( act_data_read_fifo ),
  .next_read     ( act_next_read_w    ),
  .empty         ( act_empty_w        )
);


// input fifo
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

// synthesis translate_off

//`ifdef DEBUG_DISPATCHER 
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
              $write("%d ", rep_info_filtered[k*GROUP_SIZE + l]);
          end
           $write("\n");
        end
        
        $display("\n--Zero info");
        for(k = 0; k < GROUP_SIZE; k = k + 1) begin
              $write("%d ", zer_info[k]);
        end
        $write("\n");

        if(valid_out)$display("---Output data : \n Act: %d Weight %d", data_out[DATA_WIDTH-1:0], data_out[DATA_WIDTH+DATA_WIDTH-1:DATA_WIDTH]);
      end
      tics <= tics + 1;
    end
  end
//`endif

// synthesis translate_on

endmodule
