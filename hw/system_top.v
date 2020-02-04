//Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2019.2 (lin64) Build 2700185 Thu Oct 24 18:45:48 MDT 2019
//Date        : Mon Nov  4 17:01:43 2019
//Host        : goeders-ssh0 running 64-bit Ubuntu 18.04.2 LTS
//Command     : generate_target system_wrapper.bd
//Design      : system_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module system_top
   (AC_BCLK,
    AC_MCLK,
    AC_MUTE_N,
    AC_PBLRC,
    AC_RECLRC,
    AC_SDATA_I,
    AC_SDATA_O,
    Bluetooth_UART_rxd,
    Bluetooth_UART_txd,
    DDR_addr,
    DDR_ba,
    DDR_cas_n,
    DDR_ck_n,
    DDR_ck_p,
    DDR_cke,
    DDR_cs_n,
    DDR_dm,
    DDR_dq,
    DDR_dqs_n,
    DDR_dqs_p,
    DDR_odt,
    DDR_ras_n,
    DDR_reset_n,
    DDR_we_n,
    FIXED_IO_ddr_vrn,
    FIXED_IO_ddr_vrp,
    FIXED_IO_mio,
    FIXED_IO_ps_clk,
    FIXED_IO_ps_porb,
    FIXED_IO_ps_srstb,
    IIC_0_scl_io,
    IIC_0_sda_io,
    Vaux14_v_n,
    Vaux14_v_p,
    Vaux15_v_n,
    Vaux15_v_p,
    Vaux6_v_n,
    Vaux6_v_p,
    Vaux7_v_n,
    Vaux7_v_p,
    axi_gpio_tft_control_tri_io,
    axi_gpio_tft_data_bus_tri_io,
    gpio_leds_tri_o,
    gpio_push_buttons_tri_i,
    gpio_slide_switches_tri_i,
    spi_tft_0_io0_io,
    spi_tft_0_io1_io,
    spi_tft_0_sck_io,
    spi_tft_0_ss_io);
  output [0:0]AC_BCLK;
  output AC_MCLK;
  output [0:0]AC_MUTE_N;
  output [0:0]AC_PBLRC;
  output [0:0]AC_RECLRC;
  input AC_SDATA_I;
  output [0:0]AC_SDATA_O;
  input Bluetooth_UART_rxd;
  output Bluetooth_UART_txd;
  inout [14:0]DDR_addr;
  inout [2:0]DDR_ba;
  inout DDR_cas_n;
  inout DDR_ck_n;
  inout DDR_ck_p;
  inout DDR_cke;
  inout DDR_cs_n;
  inout [3:0]DDR_dm;
  inout [31:0]DDR_dq;
  inout [3:0]DDR_dqs_n;
  inout [3:0]DDR_dqs_p;
  inout DDR_odt;
  inout DDR_ras_n;
  inout DDR_reset_n;
  inout DDR_we_n;
  inout FIXED_IO_ddr_vrn;
  inout FIXED_IO_ddr_vrp;
  inout [53:0]FIXED_IO_mio;
  inout FIXED_IO_ps_clk;
  inout FIXED_IO_ps_porb;
  inout FIXED_IO_ps_srstb;
  inout IIC_0_scl_io;
  inout IIC_0_sda_io;
  input Vaux14_v_n;
  input Vaux14_v_p;
  input Vaux15_v_n;
  input Vaux15_v_p;
  input Vaux6_v_n;
  input Vaux6_v_p;
  input Vaux7_v_n;
  input Vaux7_v_p;
  inout [2:0]axi_gpio_tft_control_tri_io;
  inout [7:0]axi_gpio_tft_data_bus_tri_io;
  output [3:0]gpio_leds_tri_o;
  input [3:0]gpio_push_buttons_tri_i;
  input [3:0]gpio_slide_switches_tri_i;
  inout spi_tft_0_io0_io;
  inout spi_tft_0_io1_io;
  inout spi_tft_0_sck_io;
  inout [1:0]spi_tft_0_ss_io;

  wire [0:0]AC_BCLK;
  wire AC_MCLK;
  wire [0:0]AC_MUTE_N;
  wire [0:0]AC_PBLRC;
  wire [0:0]AC_RECLRC;
  wire AC_SDATA_I;
  wire [0:0]AC_SDATA_O;
  wire Bluetooth_UART_rxd;
  wire Bluetooth_UART_txd;
  wire [14:0]DDR_addr;
  wire [2:0]DDR_ba;
  wire DDR_cas_n;
  wire DDR_ck_n;
  wire DDR_ck_p;
  wire DDR_cke;
  wire DDR_cs_n;
  wire [3:0]DDR_dm;
  wire [31:0]DDR_dq;
  wire [3:0]DDR_dqs_n;
  wire [3:0]DDR_dqs_p;
  wire DDR_odt;
  wire DDR_ras_n;
  wire DDR_reset_n;
  wire DDR_we_n;
  wire FIXED_IO_ddr_vrn;
  wire FIXED_IO_ddr_vrp;
  wire [53:0]FIXED_IO_mio;
  wire FIXED_IO_ps_clk;
  wire FIXED_IO_ps_porb;
  wire FIXED_IO_ps_srstb;
  wire IIC_0_scl_i;
  wire IIC_0_scl_io;
  wire IIC_0_scl_o;
  wire IIC_0_scl_t;
  wire IIC_0_sda_i;
  wire IIC_0_sda_io;
  wire IIC_0_sda_o;
  wire IIC_0_sda_t;
  wire Vaux14_v_n;
  wire Vaux14_v_p;
  wire Vaux15_v_n;
  wire Vaux15_v_p;
  wire Vaux6_v_n;
  wire Vaux6_v_p;
  wire Vaux7_v_n;
  wire Vaux7_v_p;
  wire [0:0]axi_gpio_tft_control_tri_i_0;
  wire [1:1]axi_gpio_tft_control_tri_i_1;
  wire [2:2]axi_gpio_tft_control_tri_i_2;
  wire [0:0]axi_gpio_tft_control_tri_io_0;
  wire [1:1]axi_gpio_tft_control_tri_io_1;
  wire [2:2]axi_gpio_tft_control_tri_io_2;
  wire [0:0]axi_gpio_tft_control_tri_o_0;
  wire [1:1]axi_gpio_tft_control_tri_o_1;
  wire [2:2]axi_gpio_tft_control_tri_o_2;
  wire [0:0]axi_gpio_tft_control_tri_t_0;
  wire [1:1]axi_gpio_tft_control_tri_t_1;
  wire [2:2]axi_gpio_tft_control_tri_t_2;
  wire [0:0]axi_gpio_tft_data_bus_tri_i_0;
  wire [1:1]axi_gpio_tft_data_bus_tri_i_1;
  wire [2:2]axi_gpio_tft_data_bus_tri_i_2;
  wire [3:3]axi_gpio_tft_data_bus_tri_i_3;
  wire [4:4]axi_gpio_tft_data_bus_tri_i_4;
  wire [5:5]axi_gpio_tft_data_bus_tri_i_5;
  wire [6:6]axi_gpio_tft_data_bus_tri_i_6;
  wire [7:7]axi_gpio_tft_data_bus_tri_i_7;
  wire [0:0]axi_gpio_tft_data_bus_tri_io_0;
  wire [1:1]axi_gpio_tft_data_bus_tri_io_1;
  wire [2:2]axi_gpio_tft_data_bus_tri_io_2;
  wire [3:3]axi_gpio_tft_data_bus_tri_io_3;
  wire [4:4]axi_gpio_tft_data_bus_tri_io_4;
  wire [5:5]axi_gpio_tft_data_bus_tri_io_5;
  wire [6:6]axi_gpio_tft_data_bus_tri_io_6;
  wire [7:7]axi_gpio_tft_data_bus_tri_io_7;
  wire [0:0]axi_gpio_tft_data_bus_tri_o_0;
  wire [1:1]axi_gpio_tft_data_bus_tri_o_1;
  wire [2:2]axi_gpio_tft_data_bus_tri_o_2;
  wire [3:3]axi_gpio_tft_data_bus_tri_o_3;
  wire [4:4]axi_gpio_tft_data_bus_tri_o_4;
  wire [5:5]axi_gpio_tft_data_bus_tri_o_5;
  wire [6:6]axi_gpio_tft_data_bus_tri_o_6;
  wire [7:7]axi_gpio_tft_data_bus_tri_o_7;
  wire [0:0]axi_gpio_tft_data_bus_tri_t_0;
  wire [1:1]axi_gpio_tft_data_bus_tri_t_1;
  wire [2:2]axi_gpio_tft_data_bus_tri_t_2;
  wire [3:3]axi_gpio_tft_data_bus_tri_t_3;
  wire [4:4]axi_gpio_tft_data_bus_tri_t_4;
  wire [5:5]axi_gpio_tft_data_bus_tri_t_5;
  wire [6:6]axi_gpio_tft_data_bus_tri_t_6;
  wire [7:7]axi_gpio_tft_data_bus_tri_t_7;
  wire [3:0]gpio_leds_tri_o;
  wire [3:0]gpio_push_buttons_tri_i;
  wire [3:0]gpio_slide_switches_tri_i;
  wire spi_tft_0_io0_i;
  wire spi_tft_0_io0_io;
  wire spi_tft_0_io0_o;
  wire spi_tft_0_io0_t;
  wire spi_tft_0_io1_i;
  wire spi_tft_0_io1_io;
  wire spi_tft_0_io1_o;
  wire spi_tft_0_io1_t;
  wire spi_tft_0_sck_i;
  wire spi_tft_0_sck_io;
  wire spi_tft_0_sck_o;
  wire spi_tft_0_sck_t;
  wire [0:0]spi_tft_0_ss_i_0;
  wire [1:1]spi_tft_0_ss_i_1;
  wire [0:0]spi_tft_0_ss_io_0;
  wire [1:1]spi_tft_0_ss_io_1;
  wire [0:0]spi_tft_0_ss_o_0;
  wire [1:1]spi_tft_0_ss_o_1;
  wire spi_tft_0_ss_t;

  IOBUF IIC_0_scl_iobuf
       (.I(IIC_0_scl_o),
        .IO(IIC_0_scl_io),
        .O(IIC_0_scl_i),
        .T(IIC_0_scl_t));
  IOBUF IIC_0_sda_iobuf
       (.I(IIC_0_sda_o),
        .IO(IIC_0_sda_io),
        .O(IIC_0_sda_i),
        .T(IIC_0_sda_t));
  IOBUF axi_gpio_tft_control_tri_iobuf_0
       (.I(axi_gpio_tft_control_tri_o_0),
        .IO(axi_gpio_tft_control_tri_io[0]),
        .O(axi_gpio_tft_control_tri_i_0),
        .T(axi_gpio_tft_control_tri_t_0));
  IOBUF axi_gpio_tft_control_tri_iobuf_1
       (.I(axi_gpio_tft_control_tri_o_1),
        .IO(axi_gpio_tft_control_tri_io[1]),
        .O(axi_gpio_tft_control_tri_i_1),
        .T(axi_gpio_tft_control_tri_t_1));
  IOBUF axi_gpio_tft_control_tri_iobuf_2
       (.I(axi_gpio_tft_control_tri_o_2),
        .IO(axi_gpio_tft_control_tri_io[2]),
        .O(axi_gpio_tft_control_tri_i_2),
        .T(axi_gpio_tft_control_tri_t_2));
  IOBUF axi_gpio_tft_data_bus_tri_iobuf_0
       (.I(axi_gpio_tft_data_bus_tri_o_0),
        .IO(axi_gpio_tft_data_bus_tri_io[0]),
        .O(axi_gpio_tft_data_bus_tri_i_0),
        .T(axi_gpio_tft_data_bus_tri_t_0));
  IOBUF axi_gpio_tft_data_bus_tri_iobuf_1
       (.I(axi_gpio_tft_data_bus_tri_o_1),
        .IO(axi_gpio_tft_data_bus_tri_io[1]),
        .O(axi_gpio_tft_data_bus_tri_i_1),
        .T(axi_gpio_tft_data_bus_tri_t_1));
  IOBUF axi_gpio_tft_data_bus_tri_iobuf_2
       (.I(axi_gpio_tft_data_bus_tri_o_2),
        .IO(axi_gpio_tft_data_bus_tri_io[2]),
        .O(axi_gpio_tft_data_bus_tri_i_2),
        .T(axi_gpio_tft_data_bus_tri_t_2));
  IOBUF axi_gpio_tft_data_bus_tri_iobuf_3
       (.I(axi_gpio_tft_data_bus_tri_o_3),
        .IO(axi_gpio_tft_data_bus_tri_io[3]),
        .O(axi_gpio_tft_data_bus_tri_i_3),
        .T(axi_gpio_tft_data_bus_tri_t_3));
  IOBUF axi_gpio_tft_data_bus_tri_iobuf_4
       (.I(axi_gpio_tft_data_bus_tri_o_4),
        .IO(axi_gpio_tft_data_bus_tri_io[4]),
        .O(axi_gpio_tft_data_bus_tri_i_4),
        .T(axi_gpio_tft_data_bus_tri_t_4));
  IOBUF axi_gpio_tft_data_bus_tri_iobuf_5
       (.I(axi_gpio_tft_data_bus_tri_o_5),
        .IO(axi_gpio_tft_data_bus_tri_io[5]),
        .O(axi_gpio_tft_data_bus_tri_i_5),
        .T(axi_gpio_tft_data_bus_tri_t_5));
  IOBUF axi_gpio_tft_data_bus_tri_iobuf_6
       (.I(axi_gpio_tft_data_bus_tri_o_6),
        .IO(axi_gpio_tft_data_bus_tri_io[6]),
        .O(axi_gpio_tft_data_bus_tri_i_6),
        .T(axi_gpio_tft_data_bus_tri_t_6));
  IOBUF axi_gpio_tft_data_bus_tri_iobuf_7
       (.I(axi_gpio_tft_data_bus_tri_o_7),
        .IO(axi_gpio_tft_data_bus_tri_io[7]),
        .O(axi_gpio_tft_data_bus_tri_i_7),
        .T(axi_gpio_tft_data_bus_tri_t_7));
  IOBUF spi_tft_0_io0_iobuf
       (.I(spi_tft_0_io0_o),
        .IO(spi_tft_0_io0_io),
        .O(spi_tft_0_io0_i),
        .T(spi_tft_0_io0_t));
  IOBUF spi_tft_0_io1_iobuf
       (.I(spi_tft_0_io1_o),
        .IO(spi_tft_0_io1_io),
        .O(spi_tft_0_io1_i),
        .T(spi_tft_0_io1_t));
  IOBUF spi_tft_0_sck_iobuf
       (.I(spi_tft_0_sck_o),
        .IO(spi_tft_0_sck_io),
        .O(spi_tft_0_sck_i),
        .T(spi_tft_0_sck_t));
  IOBUF spi_tft_0_ss_iobuf_0
       (.I(spi_tft_0_ss_o_0),
        .IO(spi_tft_0_ss_io[0]),
        .O(spi_tft_0_ss_i_0),
        .T(spi_tft_0_ss_t));
  IOBUF spi_tft_0_ss_iobuf_1
       (.I(spi_tft_0_ss_o_1),
        .IO(spi_tft_0_ss_io[1]),
        .O(spi_tft_0_ss_i_1),
        .T(spi_tft_0_ss_t));
  system system_i
       (.AC_BCLK(AC_BCLK),
        .AC_MCLK(AC_MCLK),
        .AC_MUTE_N(AC_MUTE_N),
        .AC_PBLRC(AC_PBLRC),
        .AC_RECLRC(AC_RECLRC),
        .AC_SDATA_I(AC_SDATA_I),
        .AC_SDATA_O(AC_SDATA_O),
        .Bluetooth_UART_rxd(Bluetooth_UART_rxd),
        .Bluetooth_UART_txd(Bluetooth_UART_txd),
        .DDR_addr(DDR_addr),
        .DDR_ba(DDR_ba),
        .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n),
        .DDR_ck_p(DDR_ck_p),
        .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n),
        .DDR_dm(DDR_dm),
        .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n),
        .DDR_dqs_p(DDR_dqs_p),
        .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n),
        .DDR_reset_n(DDR_reset_n),
        .DDR_we_n(DDR_we_n),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn),
        .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio),
        .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb),
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
        .IIC_0_scl_i(IIC_0_scl_i),
        .IIC_0_scl_o(IIC_0_scl_o),
        .IIC_0_scl_t(IIC_0_scl_t),
        .IIC_0_sda_i(IIC_0_sda_i),
        .IIC_0_sda_o(IIC_0_sda_o),
        .IIC_0_sda_t(IIC_0_sda_t),
        .Vaux14_v_n(Vaux14_v_n),
        .Vaux14_v_p(Vaux14_v_p),
        .Vaux15_v_n(Vaux15_v_n),
        .Vaux15_v_p(Vaux15_v_p),
        .Vaux6_v_n(Vaux6_v_n),
        .Vaux6_v_p(Vaux6_v_p),
        .Vaux7_v_n(Vaux7_v_n),
        .Vaux7_v_p(Vaux7_v_p),
        .axi_gpio_tft_control_tri_i({axi_gpio_tft_control_tri_i_2,axi_gpio_tft_control_tri_i_1,axi_gpio_tft_control_tri_i_0}),
        .axi_gpio_tft_control_tri_o({axi_gpio_tft_control_tri_o_2,axi_gpio_tft_control_tri_o_1,axi_gpio_tft_control_tri_o_0}),
        .axi_gpio_tft_control_tri_t({axi_gpio_tft_control_tri_t_2,axi_gpio_tft_control_tri_t_1,axi_gpio_tft_control_tri_t_0}),
        .axi_gpio_tft_data_bus_tri_i({axi_gpio_tft_data_bus_tri_i_7,axi_gpio_tft_data_bus_tri_i_6,axi_gpio_tft_data_bus_tri_i_5,axi_gpio_tft_data_bus_tri_i_4,axi_gpio_tft_data_bus_tri_i_3,axi_gpio_tft_data_bus_tri_i_2,axi_gpio_tft_data_bus_tri_i_1,axi_gpio_tft_data_bus_tri_i_0}),
        .axi_gpio_tft_data_bus_tri_o({axi_gpio_tft_data_bus_tri_o_7,axi_gpio_tft_data_bus_tri_o_6,axi_gpio_tft_data_bus_tri_o_5,axi_gpio_tft_data_bus_tri_o_4,axi_gpio_tft_data_bus_tri_o_3,axi_gpio_tft_data_bus_tri_o_2,axi_gpio_tft_data_bus_tri_o_1,axi_gpio_tft_data_bus_tri_o_0}),
        .axi_gpio_tft_data_bus_tri_t({axi_gpio_tft_data_bus_tri_t_7,axi_gpio_tft_data_bus_tri_t_6,axi_gpio_tft_data_bus_tri_t_5,axi_gpio_tft_data_bus_tri_t_4,axi_gpio_tft_data_bus_tri_t_3,axi_gpio_tft_data_bus_tri_t_2,axi_gpio_tft_data_bus_tri_t_1,axi_gpio_tft_data_bus_tri_t_0}),
        .gpio_leds_tri_o(gpio_leds_tri_o),
        .gpio_push_buttons_tri_i(gpio_push_buttons_tri_i),
        .gpio_slide_switches_tri_i(gpio_slide_switches_tri_i),
        .spi_tft_0_io0_i(spi_tft_0_io0_i),
        .spi_tft_0_io0_o(spi_tft_0_io0_o),
        .spi_tft_0_io0_t(spi_tft_0_io0_t),
        .spi_tft_0_io1_i(spi_tft_0_io1_i),
        .spi_tft_0_io1_o(spi_tft_0_io1_o),
        .spi_tft_0_io1_t(spi_tft_0_io1_t),
        .spi_tft_0_sck_i(spi_tft_0_sck_i),
        .spi_tft_0_sck_o(spi_tft_0_sck_o),
        .spi_tft_0_sck_t(spi_tft_0_sck_t),
        .spi_tft_0_ss_i({spi_tft_0_ss_i_1,spi_tft_0_ss_i_0}),
        .spi_tft_0_ss_o({spi_tft_0_ss_o_1,spi_tft_0_ss_o_0}),
        .spi_tft_0_ss_t(spi_tft_0_ss_t));
endmodule
