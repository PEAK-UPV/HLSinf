// DISPATCHER  module
//
// This module gets GROUP SIZE activations and sends only the unique values to the next module with the 
// corresponding weight to multiply.
// 

//`include "RTLinf.vh"

module DISPATCHER #(
  parameter GROUP_SIZE             = 4,                                   // group size
  parameter DATA_WIDTH             = 4,                                   // input data width (output is 2x input width)
  parameter LOG_GS                 = 2,
  parameter MODE                   = "UNZV", // options: UV, UNZV, NZV
  localparam INPUT_WIDTH           = GROUP_SIZE * DATA_WIDTH,             // number of bits for input activation 
  localparam ZERO_INFO             = GROUP_SIZE,
  localparam REP_INFO_UV_IN        = GROUP_SIZE*GROUP_SIZE,
  localparam REP_INFO_NZV_IN       = ZERO_INFO,
  localparam REP_INFO_UNZV_IN      = REP_INFO_UV_IN + ZERO_INFO,
  localparam REP_INFO_IN           = (MODE=="UNZV") ? REP_INFO_UNZV_IN :  (MODE=="NZV") ? REP_INFO_NZV_IN : REP_INFO_UV_IN,    
  localparam REP_INFO_UV_OUT       = GROUP_SIZE + 1,
  localparam REP_INFO_NZV_OUT      = LOG_GS + ZERO_INFO + 1,
  localparam REP_INFO_UNZV_OUT     = REP_INFO_UV_OUT + ZERO_INFO,
  localparam REP_INFO_OUT          = (MODE=="UNZV") ? REP_INFO_UNZV_OUT :  (MODE=="NZV") ? REP_INFO_NZV_OUT : REP_INFO_UV_OUT  
)( 

  input clk,
  input rst,
  input                                   perform_operation,
  input                                   new_group,
  input [INPUT_WIDTH - 1 : 0]             data_in,             // IN interface:: activations data
  input [REP_INFO_IN - 1 : 0]             rdata_in,            // IN interface:: repetition info

  output [DATA_WIDTH - 1 : 0]             data_out,            // OUT interface:: activation
  output [REP_INFO_OUT - 1 : 0]           rdata_out,           // OUT interface:: repetition info
  output                                  valid_out,           // OUT1 interface:: valid
  output                                  next_read
);

if(MODE == "UNZV") begin
  DISPATCHER_UNZV #(
  .GROUP_SIZE             ( GROUP_SIZE           ),
  .LOG_GS                 ( LOG_GS               ),
  .DATA_WIDTH             ( DATA_WIDTH           )
  ) dispatcher_high_unzv_m (
  .clk                    ( clk                  ),
  .rst                    ( rst                  ),
  .perform_operation      ( perform_operation    ),
  .new_group              ( new_group            ),
  .data_in                ( data_in              ),
  .rdata_in               ( rdata_in             ),
  .data_out               ( data_out             ),
  .rdata_out              ( rdata_out            ),
  .valid_out              ( valid_out            )
);
end else begin
  DISPATCHER_NZV#(
  .GROUP_SIZE             ( GROUP_SIZE           ),
  .LOG_GS                 ( LOG_GS               ),
  .DATA_WIDTH             ( DATA_WIDTH           )
  ) dispatcher_high_nzv_m (
  .clk                    ( clk                  ),
  .rst                    ( rst                  ),
  .perform_operation      ( perform_operation    ),
  .new_group              ( new_group            ),
  .data_in                ( data_in              ),
  .rdata_in               ( rdata_in             ),
  .data_out               ( data_out             ),
  .rdata_out              ( rdata_out            ),
  .valid_out              ( valid_out            ),
  .next_read              ( next_read            )
);  
end
endmodule




module DISPATCHER_UNZV #(
    parameter GROUP_SIZE              = 4,                                   // group size
    parameter DATA_WIDTH              = 4,                                   // input data width (output is 2x input width)
    parameter LOG_GS                  = 2,
    localparam ZERO_INFO              = GROUP_SIZE,                          // number of bits for the zero detection
    localparam REP_INFO_IN            = GROUP_SIZE*GROUP_SIZE+ZERO_INFO,     // number of bits for the repetition info
    localparam REP_INFO_OUT           = GROUP_SIZE + ZERO_INFO +  1,          // number of bits for the out repetition info
    localparam INPUT_WIDTH            = GROUP_SIZE * DATA_WIDTH             // number of bits for input activation 
  )( 

    input clk,
    input rst,
    input                               perform_operation,
    input                               new_group,
    input [INPUT_WIDTH - 1 : 0]         data_in,             // IN interface:: activations data
    input [REP_INFO_IN - 1 : 0]         rdata_in,            // IN interface:: repetition info
    output [DATA_WIDTH - 1 : 0]         data_out,            // OUT interface:: activation
    output [REP_INFO_OUT - 1 : 0]       rdata_out,           // OUT interface:: repetition info
    output                              valid_out           // OUT1 interface:: valid
  );


  // wires
  wire [GROUP_SIZE-1:0]                  diag;                              // diagonal of the rep info matrix
  wire [GROUP_SIZE - 1 : 0]              equivalence_row;                   // vector of equivalences between the element actual and the rest
  wire [GROUP_SIZE * GROUP_SIZE - 1 : 0] rep_info;                          // matrix with condensed repetition detection information
  wire [ZERO_INFO - 1 : 0]               zer_info;                          // vector of zero elements
  wire                                   is_last;                           // indicates if is the last element of the group


  // registers 
  reg                                    new_group_r;            // element being processed in this cycle
  reg [DATA_WIDTH - 1 : 0]               data_out_r;
  reg [REP_INFO_IN - 1 : 0]              rdata_in_r;

  reg [LOG_GS - 1 : 0]                   element_actual;            // element being processed in this cycle
  reg [LOG_GS - 1 : 0]                   last_element;              // element to process in last cycle
  reg [LOG_GS - 1 : 0]                   next_element;              // element to process in next cycle
  reg                                    valid_out_r;

  reg                                     is_last_r;

  genvar i;
  genvar j;
  integer k;

  // combinational logic
  assign valid_out = valid_out_r;


  assign rep_info = rdata_in[GROUP_SIZE * GROUP_SIZE - 1 : 0];
  assign zer_info = rdata_in[REP_INFO_IN - 1 : GROUP_SIZE * GROUP_SIZE];

  assign data_out = data_out_r;
  assign equivalence_row = rep_info[GROUP_SIZE*element_actual +: GROUP_SIZE];

  assign rdata_out[ GROUP_SIZE - 1: 0]        =  rdata_in_r[GROUP_SIZE*element_actual +: GROUP_SIZE];
  assign rdata_out[ GROUP_SIZE +: GROUP_SIZE] =  rdata_in_r[REP_INFO_IN - 1 : GROUP_SIZE * GROUP_SIZE];
  assign rdata_out[ REP_INFO_OUT - 1]         =  is_last_r;

  assign next_read                            =  is_last;
  
  assign is_last = next_element >= last_element;

  for(i = 0; i < GROUP_SIZE; i = i + 1) begin
      assign diag[i] = rep_info[i*GROUP_SIZE+i];
  end
  //Get last element from group
  always @ (*) 
  begin: COMB_LAST_ELEMENT
      last_element = {LOG_GS{1'b0}};
      for(k = 0; k < GROUP_SIZE; k = k + 1) begin
          last_element = (diag[k])? k[LOG_GS-1:0] : last_element;
      end 
  end

  //Get next activation
  always @ (*) 
  begin: COMB_NEXT_ELEMENT
      next_element = {LOG_GS{1'b0}};
      for(k = GROUP_SIZE - 1; k >= 0; k = k - 1) begin
        if(new_group_r || k[LOG_GS-1:0] > element_actual) begin
          next_element =  diag[k] ? k[LOG_GS-1:0] : next_element;
        end else begin
          next_element = next_element;
        end
      end 
  end

  //Sequential
  //Get element actual
  always @ (posedge clk) begin
    if (~rst) begin
      new_group_r <= 1'b1;
      element_actual <= {LOG_GS{1'b0}};
      is_last_r <= 1'b0;
    end else begin
      valid_out_r <= perform_operation & (new_group_r || (element_actual < next_element));
      if (perform_operation) begin
        is_last_r <= is_last;
        new_group_r <= new_group;
        rdata_in_r <= rdata_in;
        data_out_r <= data_in[DATA_WIDTH*next_element +: DATA_WIDTH];
        element_actual <= next_element;
      end
    end
  end
endmodule

module DISPATCHER_NZV #(
    parameter GROUP_SIZE              = 4,                                   // group size
    parameter DATA_WIDTH              = 4,                                   // input data width (output is 2x input width)
    parameter LOG_GS                  = 2,
    localparam ZERO_INFO              = GROUP_SIZE,                          // number of bits for the zero detection
    localparam REP_INFO_IN            = ZERO_INFO,     // number of bits for the repetition info
    localparam REP_INFO_OUT           = LOG_GS + ZERO_INFO +  1,          // number of bits for the out repetition info
    localparam INPUT_WIDTH            = GROUP_SIZE * DATA_WIDTH             // number of bits for input activation 
  )( 

    input clk,
    input rst,
    input                                   perform_operation,
    input                                   new_group,
    input [INPUT_WIDTH - 1 : 0]             data_in,             // IN interface:: activations data
    input [REP_INFO_IN - 1 : 0]             rdata_in,            // IN interface:: repetition info
    output [DATA_WIDTH - 1 : 0]             data_out,            // OUT interface:: activation
    output [REP_INFO_OUT - 1 : 0]           rdata_out,           // OUT interface:: repetition info
    output                                  valid_out,           // OUT1 interface:: valid
    output                                  next_read
  );


  // wires
  wire [ZERO_INFO - 1 : 0]               zer_info;                          // vector of zero elements
  wire                                   is_last;                           // indicates if is the last element of the group


  // registers 
  reg                                    new_group_r;            // element being processed in this cycle
  reg [DATA_WIDTH - 1 : 0]               data_out_r;
  reg [REP_INFO_IN - 1 : 0]              rdata_in_r;

  reg [LOG_GS - 1 : 0]                   element_actual;            // element being processed in this cycle
  reg [LOG_GS - 1 : 0]                   last_element;              // element to process in last cycle
  reg [LOG_GS - 1 : 0]                   next_element;              // element to process in next cycle
  reg                                    valid_out_r;
  reg                                    is_last_r;

  genvar i;
  genvar j;
  integer k;

  // combinational logic
  assign valid_out = valid_out_r;
  assign zer_info = rdata_in[REP_INFO_IN - 1 : 0];


  assign data_out = data_out_r;

  assign rdata_out[0 +: ZERO_INFO]        =  rdata_in_r[0 +: ZERO_INFO];
  assign rdata_out[ZERO_INFO +: LOG_GS]   =  element_actual;
  assign rdata_out[REP_INFO_OUT - 1]      =  is_last_r;
  assign next_read                        =  is_last;
  assign is_last = next_element >= last_element;

  //Get last element from group
  always @ (*) 
  begin: COMB_LAST_ELEMENT
      last_element = {LOG_GS{1'b0}};
      for(k = 0; k < GROUP_SIZE; k = k + 1) begin
          last_element = (!zer_info[k])? k[LOG_GS-1:0] : last_element;
      end 
  end

  //Get next activation
  always @ (*) 
  begin: COMB_NEXT_ELEMENT
      next_element = {LOG_GS{1'b0}};
      for(k = GROUP_SIZE - 1; k >= 0; k = k - 1) begin
          next_element = (new_group_r || (k[LOG_GS-1:0] > element_actual))? ( !zer_info[k] ? k[LOG_GS-1:0] : next_element) : next_element;
      end 
  end


  //Sequential
  //Get element actual
  always @ (posedge clk) begin
    if (~rst) begin
      new_group_r <= 1'b1;
      element_actual <= {LOG_GS{1'b0}};
      is_last_r <= 1'b0;
    end else begin
      valid_out_r <= perform_operation & (new_group_r || (element_actual < next_element));
      if (perform_operation) begin
        is_last_r <= is_last;
        new_group_r <= new_group;
        rdata_in_r <= rdata_in;
        data_out_r <= data_in[DATA_WIDTH*next_element +: DATA_WIDTH];
        element_actual <= next_element;
      end
    end
  end
endmodule