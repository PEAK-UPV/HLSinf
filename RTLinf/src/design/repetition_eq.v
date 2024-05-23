// Module repetition_detector
//
// This module implements the required logic to find the unique elements within a group
// of activations.
//

`include "RTLinf.vh"

//ALL combinational
module repetition_eq#(
    parameter GROUP_SIZE             = 4,                      // group size
    parameter LOG_GS                 = 2,
    parameter DATA_WIDTH             = 8,                      // input and output data width
    parameter HIGH_MODE              = "UV", // options: UV, UNZV, NZV
    parameter LOW_MODE               = "UV", // options: UV, UNZV, NZV
    localparam INPUT_WIDTH           = GROUP_SIZE*DATA_WIDTH,   // number of bits for input (activation + weight + rep. info)
    localparam ZERO_INFO             = GROUP_SIZE,
    localparam REP_INFO_UV           = GROUP_SIZE*GROUP_SIZE,
    localparam REP_INFO_NZV          = ZERO_INFO,
    localparam REP_INFO_UNZV         = REP_INFO_UV + ZERO_INFO,
    localparam REP_INFO_HIGH         = (HIGH_MODE=="UNZV") ? REP_INFO_UNZV :  (HIGH_MODE=="NZV") ? REP_INFO_NZV : REP_INFO_UV,
    localparam REP_INFO_LOW          = (LOW_MODE=="UNZV") ? REP_INFO_UNZV :  (LOW_MODE=="NZV") ? REP_INFO_NZV : REP_INFO_UV    
  )(
    input [INPUT_WIDTH-1:0]             data_in_high,     // ACTIVATION interface:: data
    input [INPUT_WIDTH-1:0]             data_in_low,     // ACTIVATION interface:: data

    output [REP_INFO_HIGH-1:0]          rdata_high_out,  // OUT1 interface:: rdata
    output [REP_INFO_LOW-1:0]           rdata_low_out   // OUT1 interface:: rdata
  );

  genvar i;

  /************ HIGH part **********/
  if (HIGH_MODE=="UNZV") begin
      repetition_eq_unzv #(
        .GROUP_SIZE             ( GROUP_SIZE               ),
        .LOG_GS                 ( LOG_GS                   ),
        .DATA_WIDTH             ( DATA_WIDTH               )
      ) repetition_eq_high_m (
        .data_in                ( data_in_high             ),
        .rdata_out              ( rdata_high_out           )
      );
  end else if (HIGH_MODE=="NZV") begin
    repetition_eq_nzv #(
        .GROUP_SIZE             ( GROUP_SIZE               ),
        .LOG_GS                 ( LOG_GS                   ),
        .DATA_WIDTH             ( DATA_WIDTH               )
      ) repetition_eq_high_m (
        .data_in                ( data_in_high             ),
        .rdata_out              ( rdata_high_out           )
      );
  end else begin
      repetition_eq_uv #(
        .GROUP_SIZE             ( GROUP_SIZE               ),
        .LOG_GS                 ( LOG_GS                   ),
        .DATA_WIDTH             ( DATA_WIDTH               )
      ) repetition_eq_high_m (
        .data_in                ( data_in_high             ),
        .rdata_out              ( rdata_high_out           )
      );
  end


  /************ LOW part **********/
  if (LOW_MODE=="UNZV") begin
      repetition_eq_unzv #(
        .GROUP_SIZE             ( GROUP_SIZE               ),
        .LOG_GS                 ( LOG_GS                   ),
        .DATA_WIDTH             ( DATA_WIDTH               )
      ) repetition_eq_low_m (
        .data_in                ( data_in_low              ),
        .rdata_out              ( rdata_low_out            )
      );
  end else if (LOW_MODE=="NZV") begin
    repetition_eq_nzv #(
        .GROUP_SIZE             ( GROUP_SIZE               ),
        .LOG_GS                 ( LOG_GS                   ),
        .DATA_WIDTH             ( DATA_WIDTH               )
      ) repetition_eq_low_m (
        .data_in                ( data_in_low              ),
        .rdata_out              ( rdata_low_out            )
      );
  end else begin
      repetition_eq_uv #(
        .GROUP_SIZE             ( GROUP_SIZE               ),
        .LOG_GS                 ( LOG_GS                   ),
        .DATA_WIDTH             ( DATA_WIDTH               )
      ) repetition_eq_low_m (
        .data_in                ( data_in_low              ),
        .rdata_out              ( rdata_low_out            )
      );
  end

endmodule

//UNZV Combinational repetition detector fragment that finds repeated elements and returns the 
// matrix with the equivalences
module repetition_eq_unzv#(
    parameter GROUP_SIZE             = 4,                      // group size
    parameter LOG_GS                 = 2,
    parameter DATA_WIDTH             = 4,                      // input and output data width
    localparam INPUT_WIDTH           = GROUP_SIZE*DATA_WIDTH,   // number of bits for input (activation + weight + rep. info)
    localparam ZERO_INFO             = GROUP_SIZE,
    localparam REP_INFO              = GROUP_SIZE*GROUP_SIZE+ZERO_INFO
  )(
    input [INPUT_WIDTH-1:0]             data_in,     // ACTIVATION interface:: data
    output [REP_INFO-1:0]               rdata_out    // OUT1 interface:: rdata
  );
  
  
  wire [GROUP_SIZE * GROUP_SIZE - 1 : 0] equivalences;                      // matrix of equivalences between the values of the GS elements
  wire [GROUP_SIZE * GROUP_SIZE - 1 : 0] rep_info;                          // matrix with condensed repetition detection information
  wire [DATA_WIDTH - 1 : 0]              data_in_unpacked[GROUP_SIZE-1:0];  // two dimentional data read from FIFO
  wire [ZERO_INFO - 1 : 0]               zer_info;                          // vector of zero elements
  
  // registers
  reg [GROUP_SIZE-1:0]                   diag;                      // diagonal of the rep info matrix
  
  genvar i;
  genvar j;
  integer k;
  integer l;
  
  
  // combinational logic
  assign rdata_out[GROUP_SIZE * GROUP_SIZE - 1 : 0]        = rep_info;
  assign rdata_out[REP_INFO - 1 : GROUP_SIZE*GROUP_SIZE]   = zer_info;
  
  for(i = 0; i < GROUP_SIZE; i = i + 1) begin
      assign zer_info[i] = data_in_unpacked[i] == 0;
      assign data_in_unpacked[i] = data_in[i * DATA_WIDTH +: DATA_WIDTH];
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
          if(i == j) assign equivalences[i*GROUP_SIZE+j] = 1'b1;                             //diagonal
          if(j > i)  assign equivalences[i*GROUP_SIZE+j] = !(data_in_unpacked[i] ^ data_in_unpacked[j]);  //upper part
          if(j < i)  assign equivalences[i*GROUP_SIZE+j] = 1'b0;                             //lower part
      end
  end
  
  //--Step 2: calculate the diagonal of the matrix.
  // E.g., 3 2 3 3 will result on:
  // 1 0 0 0
  // 0 1 0 0
  // 0 0 0 0
  // 0 0 0 0
  always @ (*) begin
    diag = {GROUP_SIZE{1'b1}};
    diag[0] = zer_info[0]? 1'b0 : 1'b1;
    for(k = 1; k < GROUP_SIZE; k = k + 1) begin
      for(l = 0; l < k; l = l + 1) begin   
        diag[k] = zer_info[k]? 1'b0 : diag[k] & !equivalences[l*GROUP_SIZE+k];
      end
    end
  end
  
  //--Step 3: bind step 1 and step 2
  for(i = 0; i < GROUP_SIZE; i = i + 1) begin
    for(j = 0; j < GROUP_SIZE; j = j + 1) begin
          if(j >= i) assign rep_info[i*GROUP_SIZE+j] = diag[i] && equivalences[i*GROUP_SIZE+j];  //upper part
          if(j < i)  assign rep_info[i*GROUP_SIZE+j] = 1'b0;                                        //lower part
      end
  end
endmodule

//NZV Combinational repetition detector fragment that finds repeated elements and returns the 
// matrix with the equivalences
module repetition_eq_nzv#(
    parameter GROUP_SIZE             = 4,                      // group size
    parameter LOG_GS                 = 2,
    parameter DATA_WIDTH             = 4,                      // input and output data width
    localparam INPUT_WIDTH           = GROUP_SIZE*DATA_WIDTH,   // number of bits for input (activation + weight + rep. info)
    localparam ZERO_INFO             = GROUP_SIZE
  )(
    input [INPUT_WIDTH-1:0]             data_in,     // ACTIVATION interface:: data
    output [ZERO_INFO-1:0]              rdata_out    // OUT1 interface:: rdata
  );


  wire [DATA_WIDTH - 1 : 0]                 data_in_unpacked[GROUP_SIZE-1:0];  // two dimentional data read from FIFO
  wire [ZERO_INFO - 1 : 0]                  zer_info;                          // vector of zero elements

  genvar i;
  genvar j;
  integer k;
  integer l;


  // combinational logic
  assign rdata_out[GROUP_SIZE-1:0] = zer_info;

  for(i = 0; i < GROUP_SIZE; i = i + 1) begin
      assign zer_info[i] = data_in_unpacked[i] == 0;
  end
endmodule

//UV Combinational repetition detector fragment that finds repeated elements and returns the 
// matrix with the equivalences
module repetition_eq_uv#(
  parameter GROUP_SIZE             = 4,                      // group size
  parameter LOG_GS                 = 2,
  parameter DATA_WIDTH             = 4,                      // input and output data width
  localparam INPUT_WIDTH           = GROUP_SIZE*DATA_WIDTH,  // number of bits for input (activation + weight + rep. info)
  localparam REP_INFO              = GROUP_SIZE*GROUP_SIZE
  )(
    input [INPUT_WIDTH-1:0]        data_in,     // ACTIVATION interface:: data
    output [REP_INFO-1:0]          rdata_out    // OUT1 interface:: rdata
  );


  wire [GROUP_SIZE * GROUP_SIZE - 1 : 0] equivalences;                      // matrix of equivalences between the values of the GS elements
  wire [GROUP_SIZE * GROUP_SIZE - 1 : 0] rep_info;                          // matrix with condensed repetition detection information
  wire [DATA_WIDTH - 1 : 0]              data_in_unpacked[GROUP_SIZE-1:0];  // two dimentional data read from FIFO

  // registers
  reg [GROUP_SIZE-1:0]                   diag;                      // diagonal of the rep info matrix

  genvar i;
  genvar j;
  integer k;
  integer l;

  // combinational logic
  assign rdata_out[REP_INFO - 1 : 0] = rep_info;

  for(i = 0; i < GROUP_SIZE; i = i + 1) begin
      assign data_in_unpacked[i] = data_in[i * DATA_WIDTH +: DATA_WIDTH];
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
          if(i == j) assign equivalences[i*GROUP_SIZE+j] = 1'b1;                             //diagonal
          if(j > i)  assign equivalences[i*GROUP_SIZE+j] = !(data_in_unpacked[i] ^ data_in_unpacked[j]);  //upper part
          if(j < i)  assign equivalences[i*GROUP_SIZE+j] = 1'b0;                             //lower part
      end
  end

  //--Step 2: calculate the diagonal of the matrix.
  // E.g., 3 2 3 3 will result on:
  // 1 0 0 0
  // 0 1 0 0
  // 0 0 0 0
  // 0 0 0 0
  always @ (*) begin
    diag = {GROUP_SIZE{1'b1}};
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
          if(j < i)  assign rep_info[i*GROUP_SIZE+j] = 1'b0;                                        //lower part
      end
  end
endmodule