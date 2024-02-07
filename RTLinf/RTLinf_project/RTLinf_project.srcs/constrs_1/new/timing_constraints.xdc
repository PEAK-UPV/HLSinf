create_clock -period 3.33 -name Clk -waveform {0.000 1.667} [get_ports clk]

# Rising Edge System Synchronous Inputs
#
# A Single Data Rate (SDR) System Synchronous interface is
# an interface where the external device and the FPGA use
# the same clock, and a new data is captured one clock cycle
# after being launched
#
# input      __________            __________
# clock   __|          |__________|          |__
#           |
#           |------> (tco_min+trce_dly_min)
#           |------------> (tco_max+trce_dly_max)
#         __________      ________________
# data    __________XXXXXX_____ Data _____XXXXXXX
#

set input_clock     Clk;   # Name of input clock
set tco_max         0.000;          # Maximum clock to out delay (external device)
set tco_min         0.000;          # Minimum clock to out delay (external device)
set trce_dly_max    0.000;          # Maximum board trace delay
set trce_dly_min    0.000;          # Minimum board trace delay
set input_ports     {rst configure num_iters num_reads_per_iter read_address write_address min_clip max_clip conf_mode_in conf_mode_out};  # List of input ports

# Input Delay Constraint
#set_input_delay -clock $input_clock -max [expr $tco_max + $trce_dly_max] [get_ports $input_ports];
#set_input_delay -clock $input_clock -min [expr $tco_min + $trce_dly_min] [get_ports $input_ports];

# Report Timing Template
#report_timing -from [get_ports $input_ports] -max_paths 20 -nworst 1 -delay_type min_max -name sys_sync_rise_in  -file sys_sync_rise_in.txt;



# Rising Edge System Synchronous Outputs
#
# A System Synchronous design interface is a clocking technique in which the same
# active-edge of a system clock is used for both the source and destination device.
#
# dest        __________            __________
# clk    ____|          |__________|
#                                  |
#     (trce_dly_max+tsu) <---------|
#             (trce_dly_min-thd) <-|
#                        __    __
# data   XXXXXXXXXXXXXXXX__DATA__XXXXXXXXXXXXX
#

set destination_clock Clk;     # Name of destination clock
set tsu               0.000;            # Destination device setup time requirement
set thd               0.000;            # Destination device hold time requirement
set trce_dly_max      0.000;            # Maximum board trace delay
set trce_dly_min      0.000;            # Minimum board trace delay
set output_ports      {data addr valid};   # List of output ports

  
# Output Delay Constraint
#set_output_delay -clock $destination_clock -max [expr $trce_dly_max + $tsu] [get_ports $output_ports];
#set_output_delay -clock $destination_clock -min [expr $trce_dly_min - $thd] [get_ports $output_ports];

# Report Timing Template
#report_timing -to [get_ports $output_ports] -max_paths 20 -nworst 1 -delay_type min_max -name sys_sync_rise_out -file sys_sync_rise_out.txt;
