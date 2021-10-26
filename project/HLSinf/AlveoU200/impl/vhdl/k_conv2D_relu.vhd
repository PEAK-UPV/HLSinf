-- ==============================================================
-- RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
-- Version: 2020.2
-- Copyright (C) Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- 
-- ===========================================================

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity k_conv2D_relu is
port (
    ap_clk : IN STD_LOGIC;
    ap_rst : IN STD_LOGIC;
    ap_start : IN STD_LOGIC;
    start_full_n : IN STD_LOGIC;
    ap_done : OUT STD_LOGIC;
    ap_continue : IN STD_LOGIC;
    ap_idle : OUT STD_LOGIC;
    ap_ready : OUT STD_LOGIC;
    start_out : OUT STD_LOGIC;
    start_write : OUT STD_LOGIC;
    enable_relu_dout : IN STD_LOGIC_VECTOR (31 downto 0);
    enable_relu_empty_n : IN STD_LOGIC;
    enable_relu_read : OUT STD_LOGIC;
    H : IN STD_LOGIC_VECTOR (31 downto 0);
    W : IN STD_LOGIC_VECTOR (31 downto 0);
    out_conv_dout : IN STD_LOGIC_VECTOR (127 downto 0);
    out_conv_empty_n : IN STD_LOGIC;
    out_conv_read : OUT STD_LOGIC;
    out_relu_din : OUT STD_LOGIC_VECTOR (127 downto 0);
    out_relu_full_n : IN STD_LOGIC;
    out_relu_write : OUT STD_LOGIC );
end;


architecture behav of k_conv2D_relu is 
    constant ap_const_logic_1 : STD_LOGIC := '1';
    constant ap_const_logic_0 : STD_LOGIC := '0';
    constant ap_ST_fsm_state1 : STD_LOGIC_VECTOR (3 downto 0) := "0001";
    constant ap_ST_fsm_state2 : STD_LOGIC_VECTOR (3 downto 0) := "0010";
    constant ap_ST_fsm_pp0_stage0 : STD_LOGIC_VECTOR (3 downto 0) := "0100";
    constant ap_ST_fsm_state8 : STD_LOGIC_VECTOR (3 downto 0) := "1000";
    constant ap_const_boolean_1 : BOOLEAN := true;
    constant ap_const_lv32_0 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000000";
    constant ap_const_lv32_1 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000001";
    constant ap_const_lv32_2 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000010";
    constant ap_const_boolean_0 : BOOLEAN := false;
    constant ap_const_lv1_1 : STD_LOGIC_VECTOR (0 downto 0) := "1";
    constant ap_const_lv1_0 : STD_LOGIC_VECTOR (0 downto 0) := "0";
    constant ap_const_lv31_0 : STD_LOGIC_VECTOR (30 downto 0) := "0000000000000000000000000000000";
    constant ap_const_lv31_1 : STD_LOGIC_VECTOR (30 downto 0) := "0000000000000000000000000000001";
    constant ap_const_lv32_20 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000100000";
    constant ap_const_lv32_3F : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000111111";
    constant ap_const_lv32_40 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001000000";
    constant ap_const_lv32_5F : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001011111";
    constant ap_const_lv32_60 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001100000";
    constant ap_const_lv32_7F : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001111111";
    constant ap_const_lv32_17 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000010111";
    constant ap_const_lv32_1E : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000011110";
    constant ap_const_lv8_FF : STD_LOGIC_VECTOR (7 downto 0) := "11111111";
    constant ap_const_lv23_0 : STD_LOGIC_VECTOR (22 downto 0) := "00000000000000000000000";
    constant ap_const_lv32_37 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000110111";
    constant ap_const_lv32_3E : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000111110";
    constant ap_const_lv32_36 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000110110";
    constant ap_const_lv32_57 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001010111";
    constant ap_const_lv32_5E : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001011110";
    constant ap_const_lv32_56 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001010110";
    constant ap_const_lv32_77 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001110111";
    constant ap_const_lv32_7E : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001111110";
    constant ap_const_lv32_76 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000001110110";
    constant ap_const_lv5_4 : STD_LOGIC_VECTOR (4 downto 0) := "00100";
    constant ap_const_lv32_3 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000011";

attribute shreg_extract : string;
    signal real_start : STD_LOGIC;
    signal start_once_reg : STD_LOGIC := '0';
    signal ap_done_reg : STD_LOGIC := '0';
    signal ap_CS_fsm : STD_LOGIC_VECTOR (3 downto 0) := "0001";
    attribute fsm_encoding : string;
    attribute fsm_encoding of ap_CS_fsm : signal is "none";
    signal ap_CS_fsm_state1 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_state1 : signal is "none";
    signal internal_ap_ready : STD_LOGIC;
    signal enable_relu_blk_n : STD_LOGIC;
    signal ap_CS_fsm_state2 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_state2 : signal is "none";
    signal out_conv_blk_n : STD_LOGIC;
    signal ap_CS_fsm_pp0_stage0 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_pp0_stage0 : signal is "none";
    signal ap_enable_reg_pp0_iter1 : STD_LOGIC := '0';
    signal ap_block_pp0_stage0 : BOOLEAN;
    signal icmp_ln37_reg_494 : STD_LOGIC_VECTOR (0 downto 0);
    signal out_relu_blk_n : STD_LOGIC;
    signal ap_enable_reg_pp0_iter4 : STD_LOGIC := '0';
    signal icmp_ln37_reg_494_pp0_iter3_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal i_reg_139 : STD_LOGIC_VECTOR (30 downto 0);
    signal grp_fu_170_p2 : STD_LOGIC_VECTOR (31 downto 0);
    signal tobool_i_i_fu_176_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal tobool_i_i_reg_481 : STD_LOGIC_VECTOR (0 downto 0);
    signal add_ln37_fu_182_p2 : STD_LOGIC_VECTOR (30 downto 0);
    signal ap_enable_reg_pp0_iter0 : STD_LOGIC := '0';
    signal ap_block_state3_pp0_stage0_iter0 : BOOLEAN;
    signal ap_block_state4_pp0_stage0_iter1 : BOOLEAN;
    signal ap_block_state5_pp0_stage0_iter2 : BOOLEAN;
    signal ap_block_state6_pp0_stage0_iter3 : BOOLEAN;
    signal ap_block_state7_pp0_stage0_iter4 : BOOLEAN;
    signal ap_block_pp0_stage0_11001 : BOOLEAN;
    signal icmp_ln37_fu_192_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln37_reg_494_pp0_iter1_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln37_reg_494_pp0_iter2_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal trunc_ln42_fu_197_p1 : STD_LOGIC_VECTOR (31 downto 0);
    signal trunc_ln42_reg_498 : STD_LOGIC_VECTOR (31 downto 0);
    signal trunc_ln42_reg_498_pp0_iter2_reg : STD_LOGIC_VECTOR (31 downto 0);
    signal trunc_ln42_1_reg_504 : STD_LOGIC_VECTOR (31 downto 0);
    signal trunc_ln42_1_reg_504_pp0_iter2_reg : STD_LOGIC_VECTOR (31 downto 0);
    signal trunc_ln42_2_reg_510 : STD_LOGIC_VECTOR (31 downto 0);
    signal trunc_ln42_2_reg_510_pp0_iter2_reg : STD_LOGIC_VECTOR (31 downto 0);
    signal trunc_ln42_3_reg_516 : STD_LOGIC_VECTOR (31 downto 0);
    signal trunc_ln42_3_reg_516_pp0_iter2_reg : STD_LOGIC_VECTOR (31 downto 0);
    signal icmp_ln81_fu_245_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_reg_522 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_reg_522_pp0_iter2_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_1_fu_251_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_1_reg_527 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_1_reg_527_pp0_iter2_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_2_fu_277_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_2_reg_532 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_2_reg_532_pp0_iter2_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_3_fu_283_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_3_reg_537 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_3_reg_537_pp0_iter2_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_4_fu_309_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_4_reg_542 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_4_reg_542_pp0_iter2_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_5_fu_315_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_5_reg_547 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_5_reg_547_pp0_iter2_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_6_fu_341_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_6_reg_552 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_6_reg_552_pp0_iter2_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_7_fu_347_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_7_reg_557 : STD_LOGIC_VECTOR (0 downto 0);
    signal icmp_ln81_7_reg_557_pp0_iter2_reg : STD_LOGIC_VECTOR (0 downto 0);
    signal select_ln174_fu_429_p3 : STD_LOGIC_VECTOR (31 downto 0);
    signal select_ln174_reg_582 : STD_LOGIC_VECTOR (31 downto 0);
    signal select_ln174_1_fu_436_p3 : STD_LOGIC_VECTOR (31 downto 0);
    signal select_ln174_1_reg_587 : STD_LOGIC_VECTOR (31 downto 0);
    signal select_ln174_2_fu_443_p3 : STD_LOGIC_VECTOR (31 downto 0);
    signal select_ln174_2_reg_592 : STD_LOGIC_VECTOR (31 downto 0);
    signal select_ln174_3_fu_450_p3 : STD_LOGIC_VECTOR (31 downto 0);
    signal select_ln174_3_reg_597 : STD_LOGIC_VECTOR (31 downto 0);
    signal ap_block_pp0_stage0_subdone : BOOLEAN;
    signal ap_condition_pp0_exit_iter0_state3 : STD_LOGIC;
    signal ap_enable_reg_pp0_iter2 : STD_LOGIC := '0';
    signal ap_enable_reg_pp0_iter3 : STD_LOGIC := '0';
    signal ap_block_state1 : BOOLEAN;
    signal ap_block_pp0_stage0_01001 : BOOLEAN;
    signal grp_fu_150_p0 : STD_LOGIC_VECTOR (31 downto 0);
    signal grp_fu_155_p0 : STD_LOGIC_VECTOR (31 downto 0);
    signal grp_fu_160_p0 : STD_LOGIC_VECTOR (31 downto 0);
    signal grp_fu_165_p0 : STD_LOGIC_VECTOR (31 downto 0);
    signal zext_ln23_fu_188_p1 : STD_LOGIC_VECTOR (31 downto 0);
    signal tmp_1_fu_231_p4 : STD_LOGIC_VECTOR (7 downto 0);
    signal trunc_ln81_fu_241_p1 : STD_LOGIC_VECTOR (22 downto 0);
    signal tmp_3_fu_257_p4 : STD_LOGIC_VECTOR (7 downto 0);
    signal trunc_ln81_1_fu_267_p4 : STD_LOGIC_VECTOR (22 downto 0);
    signal tmp_5_fu_289_p4 : STD_LOGIC_VECTOR (7 downto 0);
    signal trunc_ln81_2_fu_299_p4 : STD_LOGIC_VECTOR (22 downto 0);
    signal tmp_7_fu_321_p4 : STD_LOGIC_VECTOR (7 downto 0);
    signal trunc_ln81_3_fu_331_p4 : STD_LOGIC_VECTOR (22 downto 0);
    signal or_ln81_fu_369_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal grp_fu_150_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal and_ln81_4_fu_373_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal or_ln81_1_fu_384_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal grp_fu_155_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal and_ln81_5_fu_388_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal or_ln81_2_fu_399_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal grp_fu_160_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal and_ln81_6_fu_403_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal or_ln81_3_fu_414_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal grp_fu_165_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal and_ln81_7_fu_418_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal and_ln81_fu_379_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal and_ln81_1_fu_394_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal and_ln81_2_fu_409_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal and_ln81_3_fu_424_p2 : STD_LOGIC_VECTOR (0 downto 0);
    signal grp_fu_150_ce : STD_LOGIC;
    signal ap_block_pp0_stage0_00001 : BOOLEAN;
    signal grp_fu_155_ce : STD_LOGIC;
    signal grp_fu_160_ce : STD_LOGIC;
    signal grp_fu_165_ce : STD_LOGIC;
    signal grp_fu_170_ce : STD_LOGIC;
    signal ap_CS_fsm_state8 : STD_LOGIC;
    attribute fsm_encoding of ap_CS_fsm_state8 : signal is "none";
    signal ap_NS_fsm : STD_LOGIC_VECTOR (3 downto 0);
    signal ap_idle_pp0 : STD_LOGIC;
    signal ap_enable_pp0 : STD_LOGIC;
    signal ap_ce_reg : STD_LOGIC;

    component k_conv2D_fcmp_32ns_32ns_1_2_no_dsp_1 IS
    generic (
        ID : INTEGER;
        NUM_STAGE : INTEGER;
        din0_WIDTH : INTEGER;
        din1_WIDTH : INTEGER;
        dout_WIDTH : INTEGER );
    port (
        clk : IN STD_LOGIC;
        reset : IN STD_LOGIC;
        din0 : IN STD_LOGIC_VECTOR (31 downto 0);
        din1 : IN STD_LOGIC_VECTOR (31 downto 0);
        ce : IN STD_LOGIC;
        opcode : IN STD_LOGIC_VECTOR (4 downto 0);
        dout : OUT STD_LOGIC_VECTOR (0 downto 0) );
    end component;


    component k_conv2D_mul_32s_32s_32_2_1 IS
    generic (
        ID : INTEGER;
        NUM_STAGE : INTEGER;
        din0_WIDTH : INTEGER;
        din1_WIDTH : INTEGER;
        dout_WIDTH : INTEGER );
    port (
        clk : IN STD_LOGIC;
        reset : IN STD_LOGIC;
        din0 : IN STD_LOGIC_VECTOR (31 downto 0);
        din1 : IN STD_LOGIC_VECTOR (31 downto 0);
        ce : IN STD_LOGIC;
        dout : OUT STD_LOGIC_VECTOR (31 downto 0) );
    end component;



begin
    fcmp_32ns_32ns_1_2_no_dsp_1_U430 : component k_conv2D_fcmp_32ns_32ns_1_2_no_dsp_1
    generic map (
        ID => 1,
        NUM_STAGE => 2,
        din0_WIDTH => 32,
        din1_WIDTH => 32,
        dout_WIDTH => 1)
    port map (
        clk => ap_clk,
        reset => ap_rst,
        din0 => grp_fu_150_p0,
        din1 => ap_const_lv32_0,
        ce => grp_fu_150_ce,
        opcode => ap_const_lv5_4,
        dout => grp_fu_150_p2);

    fcmp_32ns_32ns_1_2_no_dsp_1_U431 : component k_conv2D_fcmp_32ns_32ns_1_2_no_dsp_1
    generic map (
        ID => 1,
        NUM_STAGE => 2,
        din0_WIDTH => 32,
        din1_WIDTH => 32,
        dout_WIDTH => 1)
    port map (
        clk => ap_clk,
        reset => ap_rst,
        din0 => grp_fu_155_p0,
        din1 => ap_const_lv32_0,
        ce => grp_fu_155_ce,
        opcode => ap_const_lv5_4,
        dout => grp_fu_155_p2);

    fcmp_32ns_32ns_1_2_no_dsp_1_U432 : component k_conv2D_fcmp_32ns_32ns_1_2_no_dsp_1
    generic map (
        ID => 1,
        NUM_STAGE => 2,
        din0_WIDTH => 32,
        din1_WIDTH => 32,
        dout_WIDTH => 1)
    port map (
        clk => ap_clk,
        reset => ap_rst,
        din0 => grp_fu_160_p0,
        din1 => ap_const_lv32_0,
        ce => grp_fu_160_ce,
        opcode => ap_const_lv5_4,
        dout => grp_fu_160_p2);

    fcmp_32ns_32ns_1_2_no_dsp_1_U433 : component k_conv2D_fcmp_32ns_32ns_1_2_no_dsp_1
    generic map (
        ID => 1,
        NUM_STAGE => 2,
        din0_WIDTH => 32,
        din1_WIDTH => 32,
        dout_WIDTH => 1)
    port map (
        clk => ap_clk,
        reset => ap_rst,
        din0 => grp_fu_165_p0,
        din1 => ap_const_lv32_0,
        ce => grp_fu_165_ce,
        opcode => ap_const_lv5_4,
        dout => grp_fu_165_p2);

    mul_32s_32s_32_2_1_U434 : component k_conv2D_mul_32s_32s_32_2_1
    generic map (
        ID => 1,
        NUM_STAGE => 2,
        din0_WIDTH => 32,
        din1_WIDTH => 32,
        dout_WIDTH => 32)
    port map (
        clk => ap_clk,
        reset => ap_rst,
        din0 => W,
        din1 => H,
        ce => grp_fu_170_ce,
        dout => grp_fu_170_p2);





    ap_CS_fsm_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                ap_CS_fsm <= ap_ST_fsm_state1;
            else
                ap_CS_fsm <= ap_NS_fsm;
            end if;
        end if;
    end process;


    ap_done_reg_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                ap_done_reg <= ap_const_logic_0;
            else
                if ((ap_continue = ap_const_logic_1)) then 
                    ap_done_reg <= ap_const_logic_0;
                elsif ((ap_const_logic_1 = ap_CS_fsm_state8)) then 
                    ap_done_reg <= ap_const_logic_1;
                end if; 
            end if;
        end if;
    end process;


    ap_enable_reg_pp0_iter0_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                ap_enable_reg_pp0_iter0 <= ap_const_logic_0;
            else
                if (((ap_const_boolean_0 = ap_block_pp0_stage0_subdone) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0) and (ap_const_logic_1 = ap_condition_pp0_exit_iter0_state3))) then 
                    ap_enable_reg_pp0_iter0 <= ap_const_logic_0;
                elsif (((enable_relu_empty_n = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
                    ap_enable_reg_pp0_iter0 <= ap_const_logic_1;
                end if; 
            end if;
        end if;
    end process;


    ap_enable_reg_pp0_iter1_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                ap_enable_reg_pp0_iter1 <= ap_const_logic_0;
            else
                if ((ap_const_boolean_0 = ap_block_pp0_stage0_subdone)) then
                    if ((ap_const_logic_1 = ap_condition_pp0_exit_iter0_state3)) then 
                        ap_enable_reg_pp0_iter1 <= (ap_const_logic_1 xor ap_condition_pp0_exit_iter0_state3);
                    elsif ((ap_const_boolean_1 = ap_const_boolean_1)) then 
                        ap_enable_reg_pp0_iter1 <= ap_enable_reg_pp0_iter0;
                    end if;
                end if; 
            end if;
        end if;
    end process;


    ap_enable_reg_pp0_iter2_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                ap_enable_reg_pp0_iter2 <= ap_const_logic_0;
            else
                if ((ap_const_boolean_0 = ap_block_pp0_stage0_subdone)) then 
                    ap_enable_reg_pp0_iter2 <= ap_enable_reg_pp0_iter1;
                end if; 
            end if;
        end if;
    end process;


    ap_enable_reg_pp0_iter3_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                ap_enable_reg_pp0_iter3 <= ap_const_logic_0;
            else
                if ((ap_const_boolean_0 = ap_block_pp0_stage0_subdone)) then 
                    ap_enable_reg_pp0_iter3 <= ap_enable_reg_pp0_iter2;
                end if; 
            end if;
        end if;
    end process;


    ap_enable_reg_pp0_iter4_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                ap_enable_reg_pp0_iter4 <= ap_const_logic_0;
            else
                if ((ap_const_boolean_0 = ap_block_pp0_stage0_subdone)) then 
                    ap_enable_reg_pp0_iter4 <= ap_enable_reg_pp0_iter3;
                elsif (((enable_relu_empty_n = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
                    ap_enable_reg_pp0_iter4 <= ap_const_logic_0;
                end if; 
            end if;
        end if;
    end process;


    start_once_reg_assign_proc : process(ap_clk)
    begin
        if (ap_clk'event and ap_clk =  '1') then
            if (ap_rst = '1') then
                start_once_reg <= ap_const_logic_0;
            else
                if (((internal_ap_ready = ap_const_logic_0) and (real_start = ap_const_logic_1))) then 
                    start_once_reg <= ap_const_logic_1;
                elsif ((internal_ap_ready = ap_const_logic_1)) then 
                    start_once_reg <= ap_const_logic_0;
                end if; 
            end if;
        end if;
    end process;


    i_reg_139_assign_proc : process (ap_clk)
    begin
        if (ap_clk'event and ap_clk = '1') then
            if (((ap_enable_reg_pp0_iter0 = ap_const_logic_1) and (ap_const_boolean_0 = ap_block_pp0_stage0_11001) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0) and (icmp_ln37_fu_192_p2 = ap_const_lv1_1))) then 
                i_reg_139 <= add_ln37_fu_182_p2;
            elsif (((enable_relu_empty_n = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
                i_reg_139 <= ap_const_lv31_0;
            end if; 
        end if;
    end process;
    process (ap_clk)
    begin
        if (ap_clk'event and ap_clk = '1') then
            if (((ap_const_boolean_0 = ap_block_pp0_stage0_11001) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0))) then
                icmp_ln37_reg_494 <= icmp_ln37_fu_192_p2;
                icmp_ln37_reg_494_pp0_iter1_reg <= icmp_ln37_reg_494;
            end if;
        end if;
    end process;
    process (ap_clk)
    begin
        if (ap_clk'event and ap_clk = '1') then
            if ((ap_const_boolean_0 = ap_block_pp0_stage0_11001)) then
                icmp_ln37_reg_494_pp0_iter2_reg <= icmp_ln37_reg_494_pp0_iter1_reg;
                icmp_ln37_reg_494_pp0_iter3_reg <= icmp_ln37_reg_494_pp0_iter2_reg;
                icmp_ln81_1_reg_527_pp0_iter2_reg <= icmp_ln81_1_reg_527;
                icmp_ln81_2_reg_532_pp0_iter2_reg <= icmp_ln81_2_reg_532;
                icmp_ln81_3_reg_537_pp0_iter2_reg <= icmp_ln81_3_reg_537;
                icmp_ln81_4_reg_542_pp0_iter2_reg <= icmp_ln81_4_reg_542;
                icmp_ln81_5_reg_547_pp0_iter2_reg <= icmp_ln81_5_reg_547;
                icmp_ln81_6_reg_552_pp0_iter2_reg <= icmp_ln81_6_reg_552;
                icmp_ln81_7_reg_557_pp0_iter2_reg <= icmp_ln81_7_reg_557;
                icmp_ln81_reg_522_pp0_iter2_reg <= icmp_ln81_reg_522;
                trunc_ln42_1_reg_504_pp0_iter2_reg <= trunc_ln42_1_reg_504;
                trunc_ln42_2_reg_510_pp0_iter2_reg <= trunc_ln42_2_reg_510;
                trunc_ln42_3_reg_516_pp0_iter2_reg <= trunc_ln42_3_reg_516;
                trunc_ln42_reg_498_pp0_iter2_reg <= trunc_ln42_reg_498;
            end if;
        end if;
    end process;
    process (ap_clk)
    begin
        if (ap_clk'event and ap_clk = '1') then
            if (((icmp_ln37_reg_494 = ap_const_lv1_1) and (ap_const_boolean_0 = ap_block_pp0_stage0_11001) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0))) then
                icmp_ln81_1_reg_527 <= icmp_ln81_1_fu_251_p2;
                icmp_ln81_2_reg_532 <= icmp_ln81_2_fu_277_p2;
                icmp_ln81_3_reg_537 <= icmp_ln81_3_fu_283_p2;
                icmp_ln81_4_reg_542 <= icmp_ln81_4_fu_309_p2;
                icmp_ln81_5_reg_547 <= icmp_ln81_5_fu_315_p2;
                icmp_ln81_6_reg_552 <= icmp_ln81_6_fu_341_p2;
                icmp_ln81_7_reg_557 <= icmp_ln81_7_fu_347_p2;
                icmp_ln81_reg_522 <= icmp_ln81_fu_245_p2;
                trunc_ln42_1_reg_504 <= out_conv_dout(63 downto 32);
                trunc_ln42_2_reg_510 <= out_conv_dout(95 downto 64);
                trunc_ln42_3_reg_516 <= out_conv_dout(127 downto 96);
                trunc_ln42_reg_498 <= trunc_ln42_fu_197_p1;
            end if;
        end if;
    end process;
    process (ap_clk)
    begin
        if (ap_clk'event and ap_clk = '1') then
            if (((ap_const_boolean_0 = ap_block_pp0_stage0_11001) and (icmp_ln37_reg_494_pp0_iter2_reg = ap_const_lv1_1))) then
                select_ln174_1_reg_587 <= select_ln174_1_fu_436_p3;
                select_ln174_2_reg_592 <= select_ln174_2_fu_443_p3;
                select_ln174_3_reg_597 <= select_ln174_3_fu_450_p3;
                select_ln174_reg_582 <= select_ln174_fu_429_p3;
            end if;
        end if;
    end process;
    process (ap_clk)
    begin
        if (ap_clk'event and ap_clk = '1') then
            if ((ap_const_logic_1 = ap_CS_fsm_state2)) then
                tobool_i_i_reg_481 <= tobool_i_i_fu_176_p2;
            end if;
        end if;
    end process;

    ap_NS_fsm_assign_proc : process (real_start, ap_done_reg, ap_CS_fsm, ap_CS_fsm_state1, enable_relu_empty_n, ap_CS_fsm_state2, ap_enable_reg_pp0_iter1, ap_enable_reg_pp0_iter4, ap_enable_reg_pp0_iter0, icmp_ln37_fu_192_p2, ap_block_pp0_stage0_subdone, ap_enable_reg_pp0_iter3)
    begin
        case ap_CS_fsm is
            when ap_ST_fsm_state1 => 
                if ((not(((ap_done_reg = ap_const_logic_1) or (real_start = ap_const_logic_0))) and (ap_const_logic_1 = ap_CS_fsm_state1))) then
                    ap_NS_fsm <= ap_ST_fsm_state2;
                else
                    ap_NS_fsm <= ap_ST_fsm_state1;
                end if;
            when ap_ST_fsm_state2 => 
                if (((enable_relu_empty_n = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then
                    ap_NS_fsm <= ap_ST_fsm_pp0_stage0;
                else
                    ap_NS_fsm <= ap_ST_fsm_state2;
                end if;
            when ap_ST_fsm_pp0_stage0 => 
                if ((not(((ap_enable_reg_pp0_iter0 = ap_const_logic_1) and (ap_const_boolean_0 = ap_block_pp0_stage0_subdone) and (ap_enable_reg_pp0_iter1 = ap_const_logic_0) and (icmp_ln37_fu_192_p2 = ap_const_lv1_0))) and not(((ap_enable_reg_pp0_iter4 = ap_const_logic_1) and (ap_const_boolean_0 = ap_block_pp0_stage0_subdone) and (ap_enable_reg_pp0_iter3 = ap_const_logic_0))))) then
                    ap_NS_fsm <= ap_ST_fsm_pp0_stage0;
                elsif ((((ap_enable_reg_pp0_iter0 = ap_const_logic_1) and (ap_const_boolean_0 = ap_block_pp0_stage0_subdone) and (ap_enable_reg_pp0_iter1 = ap_const_logic_0) and (icmp_ln37_fu_192_p2 = ap_const_lv1_0)) or ((ap_enable_reg_pp0_iter4 = ap_const_logic_1) and (ap_const_boolean_0 = ap_block_pp0_stage0_subdone) and (ap_enable_reg_pp0_iter3 = ap_const_logic_0)))) then
                    ap_NS_fsm <= ap_ST_fsm_state8;
                else
                    ap_NS_fsm <= ap_ST_fsm_pp0_stage0;
                end if;
            when ap_ST_fsm_state8 => 
                ap_NS_fsm <= ap_ST_fsm_state1;
            when others =>  
                ap_NS_fsm <= "XXXX";
        end case;
    end process;
    add_ln37_fu_182_p2 <= std_logic_vector(unsigned(i_reg_139) + unsigned(ap_const_lv31_1));
    and_ln81_1_fu_394_p2 <= (tobool_i_i_reg_481 and and_ln81_5_fu_388_p2);
    and_ln81_2_fu_409_p2 <= (tobool_i_i_reg_481 and and_ln81_6_fu_403_p2);
    and_ln81_3_fu_424_p2 <= (tobool_i_i_reg_481 and and_ln81_7_fu_418_p2);
    and_ln81_4_fu_373_p2 <= (or_ln81_fu_369_p2 and grp_fu_150_p2);
    and_ln81_5_fu_388_p2 <= (or_ln81_1_fu_384_p2 and grp_fu_155_p2);
    and_ln81_6_fu_403_p2 <= (or_ln81_2_fu_399_p2 and grp_fu_160_p2);
    and_ln81_7_fu_418_p2 <= (or_ln81_3_fu_414_p2 and grp_fu_165_p2);
    and_ln81_fu_379_p2 <= (tobool_i_i_reg_481 and and_ln81_4_fu_373_p2);
    ap_CS_fsm_pp0_stage0 <= ap_CS_fsm(2);
    ap_CS_fsm_state1 <= ap_CS_fsm(0);
    ap_CS_fsm_state2 <= ap_CS_fsm(1);
    ap_CS_fsm_state8 <= ap_CS_fsm(3);
        ap_block_pp0_stage0 <= not((ap_const_boolean_1 = ap_const_boolean_1));

    ap_block_pp0_stage0_00001_assign_proc : process(out_conv_empty_n, out_relu_full_n, ap_enable_reg_pp0_iter1, icmp_ln37_reg_494, ap_enable_reg_pp0_iter4, icmp_ln37_reg_494_pp0_iter3_reg)
    begin
                ap_block_pp0_stage0_00001 <= (((icmp_ln37_reg_494_pp0_iter3_reg = ap_const_lv1_1) and (ap_enable_reg_pp0_iter4 = ap_const_logic_1) and (out_relu_full_n = ap_const_logic_0)) or ((icmp_ln37_reg_494 = ap_const_lv1_1) and (ap_enable_reg_pp0_iter1 = ap_const_logic_1) and (out_conv_empty_n = ap_const_logic_0)));
    end process;


    ap_block_pp0_stage0_01001_assign_proc : process(out_conv_empty_n, out_relu_full_n, ap_enable_reg_pp0_iter1, icmp_ln37_reg_494, ap_enable_reg_pp0_iter4, icmp_ln37_reg_494_pp0_iter3_reg)
    begin
                ap_block_pp0_stage0_01001 <= (((icmp_ln37_reg_494_pp0_iter3_reg = ap_const_lv1_1) and (ap_enable_reg_pp0_iter4 = ap_const_logic_1) and (out_relu_full_n = ap_const_logic_0)) or ((icmp_ln37_reg_494 = ap_const_lv1_1) and (ap_enable_reg_pp0_iter1 = ap_const_logic_1) and (out_conv_empty_n = ap_const_logic_0)));
    end process;


    ap_block_pp0_stage0_11001_assign_proc : process(out_conv_empty_n, out_relu_full_n, ap_enable_reg_pp0_iter1, icmp_ln37_reg_494, ap_enable_reg_pp0_iter4, icmp_ln37_reg_494_pp0_iter3_reg)
    begin
                ap_block_pp0_stage0_11001 <= (((icmp_ln37_reg_494_pp0_iter3_reg = ap_const_lv1_1) and (ap_enable_reg_pp0_iter4 = ap_const_logic_1) and (out_relu_full_n = ap_const_logic_0)) or ((icmp_ln37_reg_494 = ap_const_lv1_1) and (ap_enable_reg_pp0_iter1 = ap_const_logic_1) and (out_conv_empty_n = ap_const_logic_0)));
    end process;


    ap_block_pp0_stage0_subdone_assign_proc : process(out_conv_empty_n, out_relu_full_n, ap_enable_reg_pp0_iter1, icmp_ln37_reg_494, ap_enable_reg_pp0_iter4, icmp_ln37_reg_494_pp0_iter3_reg)
    begin
                ap_block_pp0_stage0_subdone <= (((icmp_ln37_reg_494_pp0_iter3_reg = ap_const_lv1_1) and (ap_enable_reg_pp0_iter4 = ap_const_logic_1) and (out_relu_full_n = ap_const_logic_0)) or ((icmp_ln37_reg_494 = ap_const_lv1_1) and (ap_enable_reg_pp0_iter1 = ap_const_logic_1) and (out_conv_empty_n = ap_const_logic_0)));
    end process;


    ap_block_state1_assign_proc : process(real_start, ap_done_reg)
    begin
                ap_block_state1 <= ((ap_done_reg = ap_const_logic_1) or (real_start = ap_const_logic_0));
    end process;

        ap_block_state3_pp0_stage0_iter0 <= not((ap_const_boolean_1 = ap_const_boolean_1));

    ap_block_state4_pp0_stage0_iter1_assign_proc : process(out_conv_empty_n, icmp_ln37_reg_494)
    begin
                ap_block_state4_pp0_stage0_iter1 <= ((icmp_ln37_reg_494 = ap_const_lv1_1) and (out_conv_empty_n = ap_const_logic_0));
    end process;

        ap_block_state5_pp0_stage0_iter2 <= not((ap_const_boolean_1 = ap_const_boolean_1));
        ap_block_state6_pp0_stage0_iter3 <= not((ap_const_boolean_1 = ap_const_boolean_1));

    ap_block_state7_pp0_stage0_iter4_assign_proc : process(out_relu_full_n, icmp_ln37_reg_494_pp0_iter3_reg)
    begin
                ap_block_state7_pp0_stage0_iter4 <= ((icmp_ln37_reg_494_pp0_iter3_reg = ap_const_lv1_1) and (out_relu_full_n = ap_const_logic_0));
    end process;


    ap_condition_pp0_exit_iter0_state3_assign_proc : process(icmp_ln37_fu_192_p2)
    begin
        if ((icmp_ln37_fu_192_p2 = ap_const_lv1_0)) then 
            ap_condition_pp0_exit_iter0_state3 <= ap_const_logic_1;
        else 
            ap_condition_pp0_exit_iter0_state3 <= ap_const_logic_0;
        end if; 
    end process;


    ap_done_assign_proc : process(ap_done_reg, ap_CS_fsm_state8)
    begin
        if ((ap_const_logic_1 = ap_CS_fsm_state8)) then 
            ap_done <= ap_const_logic_1;
        else 
            ap_done <= ap_done_reg;
        end if; 
    end process;

    ap_enable_pp0 <= (ap_idle_pp0 xor ap_const_logic_1);

    ap_idle_assign_proc : process(real_start, ap_CS_fsm_state1)
    begin
        if (((ap_const_logic_1 = ap_CS_fsm_state1) and (real_start = ap_const_logic_0))) then 
            ap_idle <= ap_const_logic_1;
        else 
            ap_idle <= ap_const_logic_0;
        end if; 
    end process;


    ap_idle_pp0_assign_proc : process(ap_enable_reg_pp0_iter1, ap_enable_reg_pp0_iter4, ap_enable_reg_pp0_iter0, ap_enable_reg_pp0_iter2, ap_enable_reg_pp0_iter3)
    begin
        if (((ap_enable_reg_pp0_iter0 = ap_const_logic_0) and (ap_enable_reg_pp0_iter4 = ap_const_logic_0) and (ap_enable_reg_pp0_iter1 = ap_const_logic_0) and (ap_enable_reg_pp0_iter3 = ap_const_logic_0) and (ap_enable_reg_pp0_iter2 = ap_const_logic_0))) then 
            ap_idle_pp0 <= ap_const_logic_1;
        else 
            ap_idle_pp0 <= ap_const_logic_0;
        end if; 
    end process;

    ap_ready <= internal_ap_ready;

    enable_relu_blk_n_assign_proc : process(enable_relu_empty_n, ap_CS_fsm_state2)
    begin
        if ((ap_const_logic_1 = ap_CS_fsm_state2)) then 
            enable_relu_blk_n <= enable_relu_empty_n;
        else 
            enable_relu_blk_n <= ap_const_logic_1;
        end if; 
    end process;


    enable_relu_read_assign_proc : process(enable_relu_empty_n, ap_CS_fsm_state2)
    begin
        if (((enable_relu_empty_n = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state2))) then 
            enable_relu_read <= ap_const_logic_1;
        else 
            enable_relu_read <= ap_const_logic_0;
        end if; 
    end process;


    grp_fu_150_ce_assign_proc : process(ap_CS_fsm_pp0_stage0, ap_block_pp0_stage0_11001)
    begin
        if (((ap_const_boolean_0 = ap_block_pp0_stage0_11001) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0))) then 
            grp_fu_150_ce <= ap_const_logic_1;
        else 
            grp_fu_150_ce <= ap_const_logic_0;
        end if; 
    end process;

    grp_fu_150_p0 <= trunc_ln42_reg_498;

    grp_fu_155_ce_assign_proc : process(ap_CS_fsm_pp0_stage0, ap_block_pp0_stage0_11001)
    begin
        if (((ap_const_boolean_0 = ap_block_pp0_stage0_11001) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0))) then 
            grp_fu_155_ce <= ap_const_logic_1;
        else 
            grp_fu_155_ce <= ap_const_logic_0;
        end if; 
    end process;

    grp_fu_155_p0 <= trunc_ln42_1_reg_504;

    grp_fu_160_ce_assign_proc : process(ap_CS_fsm_pp0_stage0, ap_block_pp0_stage0_11001)
    begin
        if (((ap_const_boolean_0 = ap_block_pp0_stage0_11001) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0))) then 
            grp_fu_160_ce <= ap_const_logic_1;
        else 
            grp_fu_160_ce <= ap_const_logic_0;
        end if; 
    end process;

    grp_fu_160_p0 <= trunc_ln42_2_reg_510;

    grp_fu_165_ce_assign_proc : process(ap_CS_fsm_pp0_stage0, ap_block_pp0_stage0_11001)
    begin
        if (((ap_const_boolean_0 = ap_block_pp0_stage0_11001) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0))) then 
            grp_fu_165_ce <= ap_const_logic_1;
        else 
            grp_fu_165_ce <= ap_const_logic_0;
        end if; 
    end process;

    grp_fu_165_p0 <= trunc_ln42_3_reg_516;

    grp_fu_170_ce_assign_proc : process(real_start, ap_done_reg, ap_CS_fsm_state1, enable_relu_empty_n, ap_CS_fsm_state2)
    begin
        if (((not(((ap_done_reg = ap_const_logic_1) or (real_start = ap_const_logic_0))) and (ap_const_logic_1 = ap_CS_fsm_state1)) or ((enable_relu_empty_n = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_state2)))) then 
            grp_fu_170_ce <= ap_const_logic_1;
        else 
            grp_fu_170_ce <= ap_const_logic_0;
        end if; 
    end process;

    icmp_ln37_fu_192_p2 <= "1" when (signed(zext_ln23_fu_188_p1) < signed(grp_fu_170_p2)) else "0";
    icmp_ln81_1_fu_251_p2 <= "1" when (trunc_ln81_fu_241_p1 = ap_const_lv23_0) else "0";
    icmp_ln81_2_fu_277_p2 <= "0" when (tmp_3_fu_257_p4 = ap_const_lv8_FF) else "1";
    icmp_ln81_3_fu_283_p2 <= "1" when (trunc_ln81_1_fu_267_p4 = ap_const_lv23_0) else "0";
    icmp_ln81_4_fu_309_p2 <= "0" when (tmp_5_fu_289_p4 = ap_const_lv8_FF) else "1";
    icmp_ln81_5_fu_315_p2 <= "1" when (trunc_ln81_2_fu_299_p4 = ap_const_lv23_0) else "0";
    icmp_ln81_6_fu_341_p2 <= "0" when (tmp_7_fu_321_p4 = ap_const_lv8_FF) else "1";
    icmp_ln81_7_fu_347_p2 <= "1" when (trunc_ln81_3_fu_331_p4 = ap_const_lv23_0) else "0";
    icmp_ln81_fu_245_p2 <= "0" when (tmp_1_fu_231_p4 = ap_const_lv8_FF) else "1";

    internal_ap_ready_assign_proc : process(ap_CS_fsm_state8)
    begin
        if ((ap_const_logic_1 = ap_CS_fsm_state8)) then 
            internal_ap_ready <= ap_const_logic_1;
        else 
            internal_ap_ready <= ap_const_logic_0;
        end if; 
    end process;

    or_ln81_1_fu_384_p2 <= (icmp_ln81_3_reg_537_pp0_iter2_reg or icmp_ln81_2_reg_532_pp0_iter2_reg);
    or_ln81_2_fu_399_p2 <= (icmp_ln81_5_reg_547_pp0_iter2_reg or icmp_ln81_4_reg_542_pp0_iter2_reg);
    or_ln81_3_fu_414_p2 <= (icmp_ln81_7_reg_557_pp0_iter2_reg or icmp_ln81_6_reg_552_pp0_iter2_reg);
    or_ln81_fu_369_p2 <= (icmp_ln81_reg_522_pp0_iter2_reg or icmp_ln81_1_reg_527_pp0_iter2_reg);

    out_conv_blk_n_assign_proc : process(out_conv_empty_n, ap_CS_fsm_pp0_stage0, ap_enable_reg_pp0_iter1, ap_block_pp0_stage0, icmp_ln37_reg_494)
    begin
        if (((icmp_ln37_reg_494 = ap_const_lv1_1) and (ap_const_boolean_0 = ap_block_pp0_stage0) and (ap_enable_reg_pp0_iter1 = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0))) then 
            out_conv_blk_n <= out_conv_empty_n;
        else 
            out_conv_blk_n <= ap_const_logic_1;
        end if; 
    end process;


    out_conv_read_assign_proc : process(ap_CS_fsm_pp0_stage0, ap_enable_reg_pp0_iter1, icmp_ln37_reg_494, ap_block_pp0_stage0_11001)
    begin
        if (((icmp_ln37_reg_494 = ap_const_lv1_1) and (ap_const_boolean_0 = ap_block_pp0_stage0_11001) and (ap_enable_reg_pp0_iter1 = ap_const_logic_1) and (ap_const_logic_1 = ap_CS_fsm_pp0_stage0))) then 
            out_conv_read <= ap_const_logic_1;
        else 
            out_conv_read <= ap_const_logic_0;
        end if; 
    end process;


    out_relu_blk_n_assign_proc : process(out_relu_full_n, ap_block_pp0_stage0, ap_enable_reg_pp0_iter4, icmp_ln37_reg_494_pp0_iter3_reg)
    begin
        if (((icmp_ln37_reg_494_pp0_iter3_reg = ap_const_lv1_1) and (ap_enable_reg_pp0_iter4 = ap_const_logic_1) and (ap_const_boolean_0 = ap_block_pp0_stage0))) then 
            out_relu_blk_n <= out_relu_full_n;
        else 
            out_relu_blk_n <= ap_const_logic_1;
        end if; 
    end process;

    out_relu_din <= (((select_ln174_3_reg_597 & select_ln174_2_reg_592) & select_ln174_1_reg_587) & select_ln174_reg_582);

    out_relu_write_assign_proc : process(ap_enable_reg_pp0_iter4, icmp_ln37_reg_494_pp0_iter3_reg, ap_block_pp0_stage0_11001)
    begin
        if (((icmp_ln37_reg_494_pp0_iter3_reg = ap_const_lv1_1) and (ap_enable_reg_pp0_iter4 = ap_const_logic_1) and (ap_const_boolean_0 = ap_block_pp0_stage0_11001))) then 
            out_relu_write <= ap_const_logic_1;
        else 
            out_relu_write <= ap_const_logic_0;
        end if; 
    end process;


    real_start_assign_proc : process(ap_start, start_full_n, start_once_reg)
    begin
        if (((start_once_reg = ap_const_logic_0) and (start_full_n = ap_const_logic_0))) then 
            real_start <= ap_const_logic_0;
        else 
            real_start <= ap_start;
        end if; 
    end process;

    select_ln174_1_fu_436_p3 <= 
        ap_const_lv32_0 when (and_ln81_1_fu_394_p2(0) = '1') else 
        trunc_ln42_1_reg_504_pp0_iter2_reg;
    select_ln174_2_fu_443_p3 <= 
        ap_const_lv32_0 when (and_ln81_2_fu_409_p2(0) = '1') else 
        trunc_ln42_2_reg_510_pp0_iter2_reg;
    select_ln174_3_fu_450_p3 <= 
        ap_const_lv32_0 when (and_ln81_3_fu_424_p2(0) = '1') else 
        trunc_ln42_3_reg_516_pp0_iter2_reg;
    select_ln174_fu_429_p3 <= 
        ap_const_lv32_0 when (and_ln81_fu_379_p2(0) = '1') else 
        trunc_ln42_reg_498_pp0_iter2_reg;
    start_out <= real_start;

    start_write_assign_proc : process(real_start, start_once_reg)
    begin
        if (((start_once_reg = ap_const_logic_0) and (real_start = ap_const_logic_1))) then 
            start_write <= ap_const_logic_1;
        else 
            start_write <= ap_const_logic_0;
        end if; 
    end process;

    tmp_1_fu_231_p4 <= out_conv_dout(30 downto 23);
    tmp_3_fu_257_p4 <= out_conv_dout(62 downto 55);
    tmp_5_fu_289_p4 <= out_conv_dout(94 downto 87);
    tmp_7_fu_321_p4 <= out_conv_dout(126 downto 119);
    tobool_i_i_fu_176_p2 <= "0" when (enable_relu_dout = ap_const_lv32_0) else "1";
    trunc_ln42_fu_197_p1 <= out_conv_dout(32 - 1 downto 0);
    trunc_ln81_1_fu_267_p4 <= out_conv_dout(54 downto 32);
    trunc_ln81_2_fu_299_p4 <= out_conv_dout(86 downto 64);
    trunc_ln81_3_fu_331_p4 <= out_conv_dout(118 downto 96);
    trunc_ln81_fu_241_p1 <= out_conv_dout(23 - 1 downto 0);
    zext_ln23_fu_188_p1 <= std_logic_vector(IEEE.numeric_std.resize(unsigned(i_reg_139),32));
end behav;
