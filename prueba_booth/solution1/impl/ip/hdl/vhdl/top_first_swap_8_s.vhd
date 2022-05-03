-- ==============================================================
-- RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
-- Version: 2020.2
-- Copyright (C) Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- 
-- ===========================================================

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity top_first_swap_8_s is
port (
    ap_start : IN STD_LOGIC;
    ap_ready : OUT STD_LOGIC;
    p_read : IN STD_LOGIC_VECTOR (7 downto 0);
    p_read1 : IN STD_LOGIC_VECTOR (7 downto 0);
    output_r_address0 : OUT STD_LOGIC_VECTOR (0 downto 0);
    output_r_ce0 : OUT STD_LOGIC;
    output_r_we0 : OUT STD_LOGIC;
    output_r_d0 : OUT STD_LOGIC_VECTOR (7 downto 0);
    output_r_address1 : OUT STD_LOGIC_VECTOR (0 downto 0);
    output_r_ce1 : OUT STD_LOGIC;
    output_r_we1 : OUT STD_LOGIC;
    output_r_d1 : OUT STD_LOGIC_VECTOR (7 downto 0);
    ap_return : OUT STD_LOGIC_VECTOR (0 downto 0) );
end;


architecture behav of top_first_swap_8_s is 
    constant ap_const_lv64_0 : STD_LOGIC_VECTOR (63 downto 0) := "0000000000000000000000000000000000000000000000000000000000000000";
    constant ap_const_boolean_1 : BOOLEAN := true;
    constant ap_const_lv64_1 : STD_LOGIC_VECTOR (63 downto 0) := "0000000000000000000000000000000000000000000000000000000000000001";
    constant ap_const_logic_0 : STD_LOGIC := '0';
    constant ap_const_logic_1 : STD_LOGIC := '1';
    constant ap_const_lv8_0 : STD_LOGIC_VECTOR (7 downto 0) := "00000000";

attribute shreg_extract : string;
    signal icmp_ln870_fu_53_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal ap_ce_reg : STD_LOGIC;


begin



    ap_ready <= ap_start;
    ap_return <= icmp_ln870_fu_53_p2;
    icmp_ln870_fu_53_p2 <= "1" when (p_read = ap_const_lv8_0) else "0";
    output_r_address0 <= ap_const_lv64_1(1 - 1 downto 0);
    output_r_address1 <= ap_const_lv64_0(1 - 1 downto 0);

    output_r_ce0_assign_proc : process(ap_start)
    begin
        if ((ap_start = ap_const_logic_1)) then 
            output_r_ce0 <= ap_const_logic_1;
        else 
            output_r_ce0 <= ap_const_logic_0;
        end if; 
    end process;


    output_r_ce1_assign_proc : process(ap_start)
    begin
        if ((ap_start = ap_const_logic_1)) then 
            output_r_ce1 <= ap_const_logic_1;
        else 
            output_r_ce1 <= ap_const_logic_0;
        end if; 
    end process;

    output_r_d0 <= 
        ap_const_lv8_0 when (icmp_ln870_fu_53_p2(0) = '1') else 
        p_read1;
    output_r_d1 <= 
        p_read1 when (icmp_ln870_fu_53_p2(0) = '1') else 
        p_read;

    output_r_we0_assign_proc : process(ap_start)
    begin
        if ((ap_start = ap_const_logic_1)) then 
            output_r_we0 <= ap_const_logic_1;
        else 
            output_r_we0 <= ap_const_logic_0;
        end if; 
    end process;


    output_r_we1_assign_proc : process(ap_start)
    begin
        if ((ap_start = ap_const_logic_1)) then 
            output_r_we1 <= ap_const_logic_1;
        else 
            output_r_we1 <= ap_const_logic_0;
        end if; 
    end process;

end behav;
