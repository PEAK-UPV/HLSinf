-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
-- Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- ==============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

entity k_conv2D_control_s_axi is
generic (
    C_S_AXI_ADDR_WIDTH    : INTEGER := 8;
    C_S_AXI_DATA_WIDTH    : INTEGER := 32);
port (
    ACLK                  :in   STD_LOGIC;
    ARESET                :in   STD_LOGIC;
    ACLK_EN               :in   STD_LOGIC;
    AWADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    AWVALID               :in   STD_LOGIC;
    AWREADY               :out  STD_LOGIC;
    WDATA                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    WSTRB                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH/8-1 downto 0);
    WVALID                :in   STD_LOGIC;
    WREADY                :out  STD_LOGIC;
    BRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    BVALID                :out  STD_LOGIC;
    BREADY                :in   STD_LOGIC;
    ARADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    ARVALID               :in   STD_LOGIC;
    ARREADY               :out  STD_LOGIC;
    RDATA                 :out  STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    RRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    RVALID                :out  STD_LOGIC;
    RREADY                :in   STD_LOGIC;
    interrupt             :out  STD_LOGIC;
    ptr_data              :out  STD_LOGIC_VECTOR(63 downto 0);
    H                     :out  STD_LOGIC_VECTOR(31 downto 0);
    W                     :out  STD_LOGIC_VECTOR(31 downto 0);
    rows                  :out  STD_LOGIC_VECTOR(31 downto 0);
    I                     :out  STD_LOGIC_VECTOR(31 downto 0);
    O                     :out  STD_LOGIC_VECTOR(31 downto 0);
    I_ITER                :out  STD_LOGIC_VECTOR(31 downto 0);
    o_iter_first          :out  STD_LOGIC_VECTOR(31 downto 0);
    o_iter_last           :out  STD_LOGIC_VECTOR(31 downto 0);
    enable_relu           :out  STD_LOGIC_VECTOR(31 downto 0);
    ptr_kernel            :out  STD_LOGIC_VECTOR(63 downto 0);
    ptr_bias              :out  STD_LOGIC_VECTOR(63 downto 0);
    ptr_out               :out  STD_LOGIC_VECTOR(63 downto 0);
    global_offset         :out  STD_LOGIC_VECTOR(31 downto 0);
    enable_upper_padding  :out  STD_LOGIC_VECTOR(31 downto 0);
    enable_lower_padding  :out  STD_LOGIC_VECTOR(31 downto 0);
    enable_maxpooling     :out  STD_LOGIC_VECTOR(31 downto 0);
    enable_avgpooling     :out  STD_LOGIC_VECTOR(31 downto 0);
    enable_clipping       :out  STD_LOGIC_VECTOR(31 downto 0);
    enable_shift          :out  STD_LOGIC_VECTOR(31 downto 0);
    min_clip              :out  STD_LOGIC_VECTOR(31 downto 0);
    max_clip              :out  STD_LOGIC_VECTOR(31 downto 0);
    dir_shift             :out  STD_LOGIC_VECTOR(31 downto 0);
    pos_shift             :out  STD_LOGIC_VECTOR(31 downto 0);
    ap_start              :out  STD_LOGIC;
    ap_done               :in   STD_LOGIC;
    ap_ready              :in   STD_LOGIC;
    ap_continue           :out  STD_LOGIC;
    ap_idle               :in   STD_LOGIC
);
end entity k_conv2D_control_s_axi;

-- ------------------------Address Info-------------------
-- 0x00 : Control signals
--        bit 0  - ap_start (Read/Write/COH)
--        bit 1  - ap_done (Read)
--        bit 2  - ap_idle (Read)
--        bit 3  - ap_ready (Read)
--        bit 4  - ap_continue (Read/Write/SC)
--        bit 7  - auto_restart (Read/Write)
--        others - reserved
-- 0x04 : Global Interrupt Enable Register
--        bit 0  - Global Interrupt Enable (Read/Write)
--        others - reserved
-- 0x08 : IP Interrupt Enable Register (Read/Write)
--        bit 0  - enable ap_done interrupt (Read/Write)
--        bit 1  - enable ap_ready interrupt (Read/Write)
--        others - reserved
-- 0x0c : IP Interrupt Status Register (Read/TOW)
--        bit 0  - ap_done (COR/TOW)
--        bit 1  - ap_ready (COR/TOW)
--        others - reserved
-- 0x10 : Data signal of ptr_data
--        bit 31~0 - ptr_data[31:0] (Read/Write)
-- 0x14 : Data signal of ptr_data
--        bit 31~0 - ptr_data[63:32] (Read/Write)
-- 0x18 : reserved
-- 0x1c : Data signal of H
--        bit 31~0 - H[31:0] (Read/Write)
-- 0x20 : reserved
-- 0x24 : Data signal of W
--        bit 31~0 - W[31:0] (Read/Write)
-- 0x28 : reserved
-- 0x2c : Data signal of rows
--        bit 31~0 - rows[31:0] (Read/Write)
-- 0x30 : reserved
-- 0x34 : Data signal of I
--        bit 31~0 - I[31:0] (Read/Write)
-- 0x38 : reserved
-- 0x3c : Data signal of O
--        bit 31~0 - O[31:0] (Read/Write)
-- 0x40 : reserved
-- 0x44 : Data signal of I_ITER
--        bit 31~0 - I_ITER[31:0] (Read/Write)
-- 0x48 : reserved
-- 0x4c : Data signal of o_iter_first
--        bit 31~0 - o_iter_first[31:0] (Read/Write)
-- 0x50 : reserved
-- 0x54 : Data signal of o_iter_last
--        bit 31~0 - o_iter_last[31:0] (Read/Write)
-- 0x58 : reserved
-- 0x5c : Data signal of enable_relu
--        bit 31~0 - enable_relu[31:0] (Read/Write)
-- 0x60 : reserved
-- 0x64 : Data signal of ptr_kernel
--        bit 31~0 - ptr_kernel[31:0] (Read/Write)
-- 0x68 : Data signal of ptr_kernel
--        bit 31~0 - ptr_kernel[63:32] (Read/Write)
-- 0x6c : reserved
-- 0x70 : Data signal of ptr_bias
--        bit 31~0 - ptr_bias[31:0] (Read/Write)
-- 0x74 : Data signal of ptr_bias
--        bit 31~0 - ptr_bias[63:32] (Read/Write)
-- 0x78 : reserved
-- 0x7c : Data signal of ptr_out
--        bit 31~0 - ptr_out[31:0] (Read/Write)
-- 0x80 : Data signal of ptr_out
--        bit 31~0 - ptr_out[63:32] (Read/Write)
-- 0x84 : reserved
-- 0x88 : Data signal of global_offset
--        bit 31~0 - global_offset[31:0] (Read/Write)
-- 0x8c : reserved
-- 0x90 : Data signal of enable_upper_padding
--        bit 31~0 - enable_upper_padding[31:0] (Read/Write)
-- 0x94 : reserved
-- 0x98 : Data signal of enable_lower_padding
--        bit 31~0 - enable_lower_padding[31:0] (Read/Write)
-- 0x9c : reserved
-- 0xa0 : Data signal of enable_maxpooling
--        bit 31~0 - enable_maxpooling[31:0] (Read/Write)
-- 0xa4 : reserved
-- 0xa8 : Data signal of enable_avgpooling
--        bit 31~0 - enable_avgpooling[31:0] (Read/Write)
-- 0xac : reserved
-- 0xb0 : Data signal of enable_clipping
--        bit 31~0 - enable_clipping[31:0] (Read/Write)
-- 0xb4 : reserved
-- 0xb8 : Data signal of enable_shift
--        bit 31~0 - enable_shift[31:0] (Read/Write)
-- 0xbc : reserved
-- 0xc0 : Data signal of min_clip
--        bit 31~0 - min_clip[31:0] (Read/Write)
-- 0xc4 : reserved
-- 0xc8 : Data signal of max_clip
--        bit 31~0 - max_clip[31:0] (Read/Write)
-- 0xcc : reserved
-- 0xd0 : Data signal of dir_shift
--        bit 31~0 - dir_shift[31:0] (Read/Write)
-- 0xd4 : reserved
-- 0xd8 : Data signal of pos_shift
--        bit 31~0 - pos_shift[31:0] (Read/Write)
-- 0xdc : reserved
-- (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

architecture behave of k_conv2D_control_s_axi is
    type states is (wridle, wrdata, wrresp, wrreset, rdidle, rddata, rdreset);  -- read and write fsm states
    signal wstate  : states := wrreset;
    signal rstate  : states := rdreset;
    signal wnext, rnext: states;
    constant ADDR_AP_CTRL                     : INTEGER := 16#00#;
    constant ADDR_GIE                         : INTEGER := 16#04#;
    constant ADDR_IER                         : INTEGER := 16#08#;
    constant ADDR_ISR                         : INTEGER := 16#0c#;
    constant ADDR_PTR_DATA_DATA_0             : INTEGER := 16#10#;
    constant ADDR_PTR_DATA_DATA_1             : INTEGER := 16#14#;
    constant ADDR_PTR_DATA_CTRL               : INTEGER := 16#18#;
    constant ADDR_H_DATA_0                    : INTEGER := 16#1c#;
    constant ADDR_H_CTRL                      : INTEGER := 16#20#;
    constant ADDR_W_DATA_0                    : INTEGER := 16#24#;
    constant ADDR_W_CTRL                      : INTEGER := 16#28#;
    constant ADDR_ROWS_DATA_0                 : INTEGER := 16#2c#;
    constant ADDR_ROWS_CTRL                   : INTEGER := 16#30#;
    constant ADDR_I_DATA_0                    : INTEGER := 16#34#;
    constant ADDR_I_CTRL                      : INTEGER := 16#38#;
    constant ADDR_O_DATA_0                    : INTEGER := 16#3c#;
    constant ADDR_O_CTRL                      : INTEGER := 16#40#;
    constant ADDR_I_ITER_DATA_0               : INTEGER := 16#44#;
    constant ADDR_I_ITER_CTRL                 : INTEGER := 16#48#;
    constant ADDR_O_ITER_FIRST_DATA_0         : INTEGER := 16#4c#;
    constant ADDR_O_ITER_FIRST_CTRL           : INTEGER := 16#50#;
    constant ADDR_O_ITER_LAST_DATA_0          : INTEGER := 16#54#;
    constant ADDR_O_ITER_LAST_CTRL            : INTEGER := 16#58#;
    constant ADDR_ENABLE_RELU_DATA_0          : INTEGER := 16#5c#;
    constant ADDR_ENABLE_RELU_CTRL            : INTEGER := 16#60#;
    constant ADDR_PTR_KERNEL_DATA_0           : INTEGER := 16#64#;
    constant ADDR_PTR_KERNEL_DATA_1           : INTEGER := 16#68#;
    constant ADDR_PTR_KERNEL_CTRL             : INTEGER := 16#6c#;
    constant ADDR_PTR_BIAS_DATA_0             : INTEGER := 16#70#;
    constant ADDR_PTR_BIAS_DATA_1             : INTEGER := 16#74#;
    constant ADDR_PTR_BIAS_CTRL               : INTEGER := 16#78#;
    constant ADDR_PTR_OUT_DATA_0              : INTEGER := 16#7c#;
    constant ADDR_PTR_OUT_DATA_1              : INTEGER := 16#80#;
    constant ADDR_PTR_OUT_CTRL                : INTEGER := 16#84#;
    constant ADDR_GLOBAL_OFFSET_DATA_0        : INTEGER := 16#88#;
    constant ADDR_GLOBAL_OFFSET_CTRL          : INTEGER := 16#8c#;
    constant ADDR_ENABLE_UPPER_PADDING_DATA_0 : INTEGER := 16#90#;
    constant ADDR_ENABLE_UPPER_PADDING_CTRL   : INTEGER := 16#94#;
    constant ADDR_ENABLE_LOWER_PADDING_DATA_0 : INTEGER := 16#98#;
    constant ADDR_ENABLE_LOWER_PADDING_CTRL   : INTEGER := 16#9c#;
    constant ADDR_ENABLE_MAXPOOLING_DATA_0    : INTEGER := 16#a0#;
    constant ADDR_ENABLE_MAXPOOLING_CTRL      : INTEGER := 16#a4#;
    constant ADDR_ENABLE_AVGPOOLING_DATA_0    : INTEGER := 16#a8#;
    constant ADDR_ENABLE_AVGPOOLING_CTRL      : INTEGER := 16#ac#;
    constant ADDR_ENABLE_CLIPPING_DATA_0      : INTEGER := 16#b0#;
    constant ADDR_ENABLE_CLIPPING_CTRL        : INTEGER := 16#b4#;
    constant ADDR_ENABLE_SHIFT_DATA_0         : INTEGER := 16#b8#;
    constant ADDR_ENABLE_SHIFT_CTRL           : INTEGER := 16#bc#;
    constant ADDR_MIN_CLIP_DATA_0             : INTEGER := 16#c0#;
    constant ADDR_MIN_CLIP_CTRL               : INTEGER := 16#c4#;
    constant ADDR_MAX_CLIP_DATA_0             : INTEGER := 16#c8#;
    constant ADDR_MAX_CLIP_CTRL               : INTEGER := 16#cc#;
    constant ADDR_DIR_SHIFT_DATA_0            : INTEGER := 16#d0#;
    constant ADDR_DIR_SHIFT_CTRL              : INTEGER := 16#d4#;
    constant ADDR_POS_SHIFT_DATA_0            : INTEGER := 16#d8#;
    constant ADDR_POS_SHIFT_CTRL              : INTEGER := 16#dc#;
    constant ADDR_BITS         : INTEGER := 8;

    signal waddr               : UNSIGNED(ADDR_BITS-1 downto 0);
    signal wmask               : UNSIGNED(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal aw_hs               : STD_LOGIC;
    signal w_hs                : STD_LOGIC;
    signal rdata_data          : UNSIGNED(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal ar_hs               : STD_LOGIC;
    signal raddr               : UNSIGNED(ADDR_BITS-1 downto 0);
    signal AWREADY_t           : STD_LOGIC;
    signal WREADY_t            : STD_LOGIC;
    signal ARREADY_t           : STD_LOGIC;
    signal RVALID_t            : STD_LOGIC;
    -- internal registers
    signal int_ap_idle         : STD_LOGIC;
    signal int_ap_continue     : STD_LOGIC;
    signal int_ap_ready        : STD_LOGIC;
    signal int_ap_done         : STD_LOGIC;
    signal int_ap_start        : STD_LOGIC := '0';
    signal int_auto_restart    : STD_LOGIC := '0';
    signal int_gie             : STD_LOGIC := '0';
    signal int_ier             : UNSIGNED(1 downto 0) := (others => '0');
    signal int_isr             : UNSIGNED(1 downto 0) := (others => '0');
    signal int_ptr_data        : UNSIGNED(63 downto 0) := (others => '0');
    signal int_H               : UNSIGNED(31 downto 0) := (others => '0');
    signal int_W               : UNSIGNED(31 downto 0) := (others => '0');
    signal int_rows            : UNSIGNED(31 downto 0) := (others => '0');
    signal int_I               : UNSIGNED(31 downto 0) := (others => '0');
    signal int_O               : UNSIGNED(31 downto 0) := (others => '0');
    signal int_I_ITER          : UNSIGNED(31 downto 0) := (others => '0');
    signal int_o_iter_first    : UNSIGNED(31 downto 0) := (others => '0');
    signal int_o_iter_last     : UNSIGNED(31 downto 0) := (others => '0');
    signal int_enable_relu     : UNSIGNED(31 downto 0) := (others => '0');
    signal int_ptr_kernel      : UNSIGNED(63 downto 0) := (others => '0');
    signal int_ptr_bias        : UNSIGNED(63 downto 0) := (others => '0');
    signal int_ptr_out         : UNSIGNED(63 downto 0) := (others => '0');
    signal int_global_offset   : UNSIGNED(31 downto 0) := (others => '0');
    signal int_enable_upper_padding : UNSIGNED(31 downto 0) := (others => '0');
    signal int_enable_lower_padding : UNSIGNED(31 downto 0) := (others => '0');
    signal int_enable_maxpooling : UNSIGNED(31 downto 0) := (others => '0');
    signal int_enable_avgpooling : UNSIGNED(31 downto 0) := (others => '0');
    signal int_enable_clipping : UNSIGNED(31 downto 0) := (others => '0');
    signal int_enable_shift    : UNSIGNED(31 downto 0) := (others => '0');
    signal int_min_clip        : UNSIGNED(31 downto 0) := (others => '0');
    signal int_max_clip        : UNSIGNED(31 downto 0) := (others => '0');
    signal int_dir_shift       : UNSIGNED(31 downto 0) := (others => '0');
    signal int_pos_shift       : UNSIGNED(31 downto 0) := (others => '0');


begin
-- ----------------------- Instantiation------------------


-- ----------------------- AXI WRITE ---------------------
    AWREADY_t <=  '1' when wstate = wridle else '0';
    AWREADY   <=  AWREADY_t;
    WREADY_t  <=  '1' when wstate = wrdata else '0';
    WREADY    <=  WREADY_t;
    BRESP     <=  "00";  -- OKAY
    BVALID    <=  '1' when wstate = wrresp else '0';
    wmask     <=  (31 downto 24 => WSTRB(3), 23 downto 16 => WSTRB(2), 15 downto 8 => WSTRB(1), 7 downto 0 => WSTRB(0));
    aw_hs     <=  AWVALID and AWREADY_t;
    w_hs      <=  WVALID and WREADY_t;

    -- write FSM
    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                wstate <= wrreset;
            elsif (ACLK_EN = '1') then
                wstate <= wnext;
            end if;
        end if;
    end process;

    process (wstate, AWVALID, WVALID, BREADY)
    begin
        case (wstate) is
        when wridle =>
            if (AWVALID = '1') then
                wnext <= wrdata;
            else
                wnext <= wridle;
            end if;
        when wrdata =>
            if (WVALID = '1') then
                wnext <= wrresp;
            else
                wnext <= wrdata;
            end if;
        when wrresp =>
            if (BREADY = '1') then
                wnext <= wridle;
            else
                wnext <= wrresp;
            end if;
        when others =>
            wnext <= wridle;
        end case;
    end process;

    waddr_proc : process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (aw_hs = '1') then
                    waddr <= UNSIGNED(AWADDR(ADDR_BITS-1 downto 0));
                end if;
            end if;
        end if;
    end process;

-- ----------------------- AXI READ ----------------------
    ARREADY_t <= '1' when (rstate = rdidle) else '0';
    ARREADY <= ARREADY_t;
    RDATA   <= STD_LOGIC_VECTOR(rdata_data);
    RRESP   <= "00";  -- OKAY
    RVALID_t  <= '1' when (rstate = rddata) else '0';
    RVALID    <= RVALID_t;
    ar_hs   <= ARVALID and ARREADY_t;
    raddr   <= UNSIGNED(ARADDR(ADDR_BITS-1 downto 0));

    -- read FSM
    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                rstate <= rdreset;
            elsif (ACLK_EN = '1') then
                rstate <= rnext;
            end if;
        end if;
    end process;

    process (rstate, ARVALID, RREADY, RVALID_t)
    begin
        case (rstate) is
        when rdidle =>
            if (ARVALID = '1') then
                rnext <= rddata;
            else
                rnext <= rdidle;
            end if;
        when rddata =>
            if (RREADY = '1' and RVALID_t = '1') then
                rnext <= rdidle;
            else
                rnext <= rddata;
            end if;
        when others =>
            rnext <= rdidle;
        end case;
    end process;

    rdata_proc : process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (ar_hs = '1') then
                    rdata_data <= (others => '0');
                    case (TO_INTEGER(raddr)) is
                    when ADDR_AP_CTRL =>
                        rdata_data(7) <= int_auto_restart;
                        rdata_data(4) <= int_ap_continue;
                        rdata_data(3) <= int_ap_ready;
                        rdata_data(2) <= int_ap_idle;
                        rdata_data(1) <= int_ap_done;
                        rdata_data(0) <= int_ap_start;
                    when ADDR_GIE =>
                        rdata_data(0) <= int_gie;
                    when ADDR_IER =>
                        rdata_data(1 downto 0) <= int_ier;
                    when ADDR_ISR =>
                        rdata_data(1 downto 0) <= int_isr;
                    when ADDR_PTR_DATA_DATA_0 =>
                        rdata_data <= RESIZE(int_ptr_data(31 downto 0), 32);
                    when ADDR_PTR_DATA_DATA_1 =>
                        rdata_data <= RESIZE(int_ptr_data(63 downto 32), 32);
                    when ADDR_H_DATA_0 =>
                        rdata_data <= RESIZE(int_H(31 downto 0), 32);
                    when ADDR_W_DATA_0 =>
                        rdata_data <= RESIZE(int_W(31 downto 0), 32);
                    when ADDR_ROWS_DATA_0 =>
                        rdata_data <= RESIZE(int_rows(31 downto 0), 32);
                    when ADDR_I_DATA_0 =>
                        rdata_data <= RESIZE(int_I(31 downto 0), 32);
                    when ADDR_O_DATA_0 =>
                        rdata_data <= RESIZE(int_O(31 downto 0), 32);
                    when ADDR_I_ITER_DATA_0 =>
                        rdata_data <= RESIZE(int_I_ITER(31 downto 0), 32);
                    when ADDR_O_ITER_FIRST_DATA_0 =>
                        rdata_data <= RESIZE(int_o_iter_first(31 downto 0), 32);
                    when ADDR_O_ITER_LAST_DATA_0 =>
                        rdata_data <= RESIZE(int_o_iter_last(31 downto 0), 32);
                    when ADDR_ENABLE_RELU_DATA_0 =>
                        rdata_data <= RESIZE(int_enable_relu(31 downto 0), 32);
                    when ADDR_PTR_KERNEL_DATA_0 =>
                        rdata_data <= RESIZE(int_ptr_kernel(31 downto 0), 32);
                    when ADDR_PTR_KERNEL_DATA_1 =>
                        rdata_data <= RESIZE(int_ptr_kernel(63 downto 32), 32);
                    when ADDR_PTR_BIAS_DATA_0 =>
                        rdata_data <= RESIZE(int_ptr_bias(31 downto 0), 32);
                    when ADDR_PTR_BIAS_DATA_1 =>
                        rdata_data <= RESIZE(int_ptr_bias(63 downto 32), 32);
                    when ADDR_PTR_OUT_DATA_0 =>
                        rdata_data <= RESIZE(int_ptr_out(31 downto 0), 32);
                    when ADDR_PTR_OUT_DATA_1 =>
                        rdata_data <= RESIZE(int_ptr_out(63 downto 32), 32);
                    when ADDR_GLOBAL_OFFSET_DATA_0 =>
                        rdata_data <= RESIZE(int_global_offset(31 downto 0), 32);
                    when ADDR_ENABLE_UPPER_PADDING_DATA_0 =>
                        rdata_data <= RESIZE(int_enable_upper_padding(31 downto 0), 32);
                    when ADDR_ENABLE_LOWER_PADDING_DATA_0 =>
                        rdata_data <= RESIZE(int_enable_lower_padding(31 downto 0), 32);
                    when ADDR_ENABLE_MAXPOOLING_DATA_0 =>
                        rdata_data <= RESIZE(int_enable_maxpooling(31 downto 0), 32);
                    when ADDR_ENABLE_AVGPOOLING_DATA_0 =>
                        rdata_data <= RESIZE(int_enable_avgpooling(31 downto 0), 32);
                    when ADDR_ENABLE_CLIPPING_DATA_0 =>
                        rdata_data <= RESIZE(int_enable_clipping(31 downto 0), 32);
                    when ADDR_ENABLE_SHIFT_DATA_0 =>
                        rdata_data <= RESIZE(int_enable_shift(31 downto 0), 32);
                    when ADDR_MIN_CLIP_DATA_0 =>
                        rdata_data <= RESIZE(int_min_clip(31 downto 0), 32);
                    when ADDR_MAX_CLIP_DATA_0 =>
                        rdata_data <= RESIZE(int_max_clip(31 downto 0), 32);
                    when ADDR_DIR_SHIFT_DATA_0 =>
                        rdata_data <= RESIZE(int_dir_shift(31 downto 0), 32);
                    when ADDR_POS_SHIFT_DATA_0 =>
                        rdata_data <= RESIZE(int_pos_shift(31 downto 0), 32);
                    when others =>
                        NULL;
                    end case;
                end if;
            end if;
        end if;
    end process;

-- ----------------------- Register logic ----------------
    interrupt            <= int_gie and (int_isr(0) or int_isr(1));
    ap_start             <= int_ap_start;
    int_ap_done          <= ap_done;
    ap_continue          <= int_ap_continue;
    ptr_data             <= STD_LOGIC_VECTOR(int_ptr_data);
    H                    <= STD_LOGIC_VECTOR(int_H);
    W                    <= STD_LOGIC_VECTOR(int_W);
    rows                 <= STD_LOGIC_VECTOR(int_rows);
    I                    <= STD_LOGIC_VECTOR(int_I);
    O                    <= STD_LOGIC_VECTOR(int_O);
    I_ITER               <= STD_LOGIC_VECTOR(int_I_ITER);
    o_iter_first         <= STD_LOGIC_VECTOR(int_o_iter_first);
    o_iter_last          <= STD_LOGIC_VECTOR(int_o_iter_last);
    enable_relu          <= STD_LOGIC_VECTOR(int_enable_relu);
    ptr_kernel           <= STD_LOGIC_VECTOR(int_ptr_kernel);
    ptr_bias             <= STD_LOGIC_VECTOR(int_ptr_bias);
    ptr_out              <= STD_LOGIC_VECTOR(int_ptr_out);
    global_offset        <= STD_LOGIC_VECTOR(int_global_offset);
    enable_upper_padding <= STD_LOGIC_VECTOR(int_enable_upper_padding);
    enable_lower_padding <= STD_LOGIC_VECTOR(int_enable_lower_padding);
    enable_maxpooling    <= STD_LOGIC_VECTOR(int_enable_maxpooling);
    enable_avgpooling    <= STD_LOGIC_VECTOR(int_enable_avgpooling);
    enable_clipping      <= STD_LOGIC_VECTOR(int_enable_clipping);
    enable_shift         <= STD_LOGIC_VECTOR(int_enable_shift);
    min_clip             <= STD_LOGIC_VECTOR(int_min_clip);
    max_clip             <= STD_LOGIC_VECTOR(int_max_clip);
    dir_shift            <= STD_LOGIC_VECTOR(int_dir_shift);
    pos_shift            <= STD_LOGIC_VECTOR(int_pos_shift);

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_start <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_AP_CTRL and WSTRB(0) = '1' and WDATA(0) = '1') then
                    int_ap_start <= '1';
                elsif (ap_ready = '1') then
                    int_ap_start <= int_auto_restart; -- clear on handshake/auto restart
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_idle <= '0';
            elsif (ACLK_EN = '1') then
                if (true) then
                    int_ap_idle <= ap_idle;
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_ready <= '0';
            elsif (ACLK_EN = '1') then
                if (true) then
                    int_ap_ready <= ap_ready;
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ap_continue <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_AP_CTRL and WSTRB(0) = '1' and WDATA(4) = '1') then
                    int_ap_continue <= '1';
                elsif (ap_done = '1' and int_ap_continue = '0' and int_auto_restart = '1') then
                    int_ap_continue <= '1'; -- auto restart
                else
                    int_ap_continue <= '0'; -- self clear
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_auto_restart <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_AP_CTRL and WSTRB(0) = '1') then
                    int_auto_restart <= WDATA(7);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_gie <= '0';
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_GIE and WSTRB(0) = '1') then
                    int_gie <= WDATA(0);
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_ier <= "00";
            elsif (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_IER and WSTRB(0) = '1') then
                    int_ier <= UNSIGNED(WDATA(1 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_isr(0) <= '0';
            elsif (ACLK_EN = '1') then
                if (int_ier(0) = '1' and ap_done = '1') then
                    int_isr(0) <= '1';
                elsif (w_hs = '1' and waddr = ADDR_ISR and WSTRB(0) = '1') then
                    int_isr(0) <= int_isr(0) xor WDATA(0); -- toggle on write
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ARESET = '1') then
                int_isr(1) <= '0';
            elsif (ACLK_EN = '1') then
                if (int_ier(1) = '1' and ap_ready = '1') then
                    int_isr(1) <= '1';
                elsif (w_hs = '1' and waddr = ADDR_ISR and WSTRB(0) = '1') then
                    int_isr(1) <= int_isr(1) xor WDATA(1); -- toggle on write
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_PTR_DATA_DATA_0) then
                    int_ptr_data(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_ptr_data(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_PTR_DATA_DATA_1) then
                    int_ptr_data(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_ptr_data(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_H_DATA_0) then
                    int_H(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_H(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_W_DATA_0) then
                    int_W(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_W(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ROWS_DATA_0) then
                    int_rows(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_rows(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_I_DATA_0) then
                    int_I(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_I(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_O_DATA_0) then
                    int_O(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_O(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_I_ITER_DATA_0) then
                    int_I_ITER(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_I_ITER(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_O_ITER_FIRST_DATA_0) then
                    int_o_iter_first(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_o_iter_first(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_O_ITER_LAST_DATA_0) then
                    int_o_iter_last(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_o_iter_last(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ENABLE_RELU_DATA_0) then
                    int_enable_relu(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_enable_relu(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_PTR_KERNEL_DATA_0) then
                    int_ptr_kernel(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_ptr_kernel(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_PTR_KERNEL_DATA_1) then
                    int_ptr_kernel(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_ptr_kernel(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_PTR_BIAS_DATA_0) then
                    int_ptr_bias(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_ptr_bias(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_PTR_BIAS_DATA_1) then
                    int_ptr_bias(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_ptr_bias(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_PTR_OUT_DATA_0) then
                    int_ptr_out(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_ptr_out(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_PTR_OUT_DATA_1) then
                    int_ptr_out(63 downto 32) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_ptr_out(63 downto 32));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_GLOBAL_OFFSET_DATA_0) then
                    int_global_offset(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_global_offset(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ENABLE_UPPER_PADDING_DATA_0) then
                    int_enable_upper_padding(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_enable_upper_padding(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ENABLE_LOWER_PADDING_DATA_0) then
                    int_enable_lower_padding(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_enable_lower_padding(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ENABLE_MAXPOOLING_DATA_0) then
                    int_enable_maxpooling(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_enable_maxpooling(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ENABLE_AVGPOOLING_DATA_0) then
                    int_enable_avgpooling(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_enable_avgpooling(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ENABLE_CLIPPING_DATA_0) then
                    int_enable_clipping(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_enable_clipping(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_ENABLE_SHIFT_DATA_0) then
                    int_enable_shift(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_enable_shift(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_MIN_CLIP_DATA_0) then
                    int_min_clip(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_min_clip(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_MAX_CLIP_DATA_0) then
                    int_max_clip(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_max_clip(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_DIR_SHIFT_DATA_0) then
                    int_dir_shift(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_dir_shift(31 downto 0));
                end if;
            end if;
        end if;
    end process;

    process (ACLK)
    begin
        if (ACLK'event and ACLK = '1') then
            if (ACLK_EN = '1') then
                if (w_hs = '1' and waddr = ADDR_POS_SHIFT_DATA_0) then
                    int_pos_shift(31 downto 0) <= (UNSIGNED(WDATA(31 downto 0)) and wmask(31 downto 0)) or ((not wmask(31 downto 0)) and int_pos_shift(31 downto 0));
                end if;
            end if;
        end if;
    end process;


-- ----------------------- Memory logic ------------------

end architecture behave;
