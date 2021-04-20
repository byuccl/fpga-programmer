set_property PACKAGE_PIN R18 [get_ports {gpio_push_buttons_tri_i[0]}]
set_property PACKAGE_PIN P16 [get_ports {gpio_push_buttons_tri_i[1]}]
set_property PACKAGE_PIN V16 [get_ports {gpio_push_buttons_tri_i[2]}]
set_property PACKAGE_PIN Y16 [get_ports {gpio_push_buttons_tri_i[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_push_buttons_tri_i[*]}]

set_property PACKAGE_PIN M14 [get_ports {gpio_leds_tri_o[0]}]
set_property PACKAGE_PIN M15 [get_ports {gpio_leds_tri_o[1]}]
set_property PACKAGE_PIN G14 [get_ports {gpio_leds_tri_o[2]}]
set_property PACKAGE_PIN D18 [get_ports {gpio_leds_tri_o[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_leds_tri_o[*]}]

set_property PACKAGE_PIN G15 [get_ports {gpio_slide_switches_tri_i[0]}]
set_property PACKAGE_PIN P15 [get_ports {gpio_slide_switches_tri_i[1]}]
set_property PACKAGE_PIN W13 [get_ports {gpio_slide_switches_tri_i[2]}]
set_property PACKAGE_PIN T16 [get_ports {gpio_slide_switches_tri_i[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_slide_switches_tri_i[*]}]

# PMOD JB is layed out for differential communication. Only 4 pins can
# be used for general-purpose I/O because of potential cross-talk issues.
# PMOD JB Used for SPI. All 4 pins are used.
# SPI Clock pin. ZYBO Board, JB-2
set_property PACKAGE_PIN U20 [get_ports spi_tft_0_sck_io]
set_property IOSTANDARD LVCMOS33 [get_ports spi_tft_0_sck_io]
#SPI MISO ZYBO Board, JB-4
set_property PACKAGE_PIN W20 [get_ports spi_tft_0_io1_io]
set_property IOSTANDARD LVCMOS33 [get_ports spi_tft_0_io1_io]
# SPI MOSI, ZYBO Board, JB-8.
set_property PACKAGE_PIN Y19 [get_ports spi_tft_0_io0_io]
set_property IOSTANDARD LVCMOS33 [get_ports spi_tft_0_io0_io]
# SPI slave-select[0] (TFT resistive touch controller chip). ZYBO Board, JB-10
set_property PACKAGE_PIN W19 [get_ports {spi_tft_0_ss_io[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {spi_tft_0_ss_io[0]}]
set_property PULLUP TRUE [get_ports {spi_tft_0_ss_io[0]}]

# PMOD JC is layed out for high-speed differential communication. As such,
# you can only used 4 of the 8 pins for general-purposed I/O. Otherwise,
# coupling is likely to create serious cross-talk.
# PMOD JC (all 4 pins used). 
# Used for slave-select pin for SPI, TFT control pins (DCX, RD, WR).
set_property PACKAGE_PIN W15 [get_ports {spi_tft_0_ss_io[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {spi_tft_0_ss_io[1]}]
# bit 0 is the TFT DCX pin. ZYBO Board, JC-4
set_property PACKAGE_PIN T10 [get_ports {axi_gpio_tft_control_tri_io[0]}]
# bit 1 is the TFT RD pin. ZYBO Board, JC-8
set_property PACKAGE_PIN Y14 [get_ports {axi_gpio_tft_control_tri_io[1]}]
# bit 2 is the TFT WR pin. ZYBO Board, JC-10
set_property PACKAGE_PIN U12 [get_ports {axi_gpio_tft_control_tri_io[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {axi_gpio_tft_control_tri_io[*]}]

# PMOD JD is also layed out for high-speed differential communcication.
# Used for UART Transmit pin (TXO)from bluetooth module (ZYNQ input). JD-2
set_property PACKAGE_PIN T15 [get_ports Bluetooth_UART_rxd]
set_property IOSTANDARD LVCMOS33 [get_ports Bluetooth_UART_rxd]
# Used for UART Receive pin (RXI) from the bluetooth module (ZYNQ output). JD-4
set_property PACKAGE_PIN R14 [get_ports Bluetooth_UART_txd]
set_property IOSTANDARD LVCMOS33 [get_ports Bluetooth_UART_txd]
# Used for Reset to blue-tooth modem, ZYBO Board.
#set_property PACKAGE_PIN U15 [get_ports bluetooth_radio_reset_tri_o]
#set_property IOSTANDARD LVCMOS33 [get_ports bluetooth_radio_reset_tri_o]

# PMOD JE is layed out for general-purpose I/O. All 8 pins can be used.
# Used as the parallel data bus for the TFT LCD display.
set_property PACKAGE_PIN V12 [get_ports {axi_gpio_tft_data_bus_tri_io[0]}]
set_property PACKAGE_PIN W16 [get_ports {axi_gpio_tft_data_bus_tri_io[1]}]
set_property PACKAGE_PIN J15 [get_ports {axi_gpio_tft_data_bus_tri_io[2]}]
set_property PACKAGE_PIN H15 [get_ports {axi_gpio_tft_data_bus_tri_io[3]}]
set_property PACKAGE_PIN V13 [get_ports {axi_gpio_tft_data_bus_tri_io[4]}]
set_property PACKAGE_PIN U17 [get_ports {axi_gpio_tft_data_bus_tri_io[5]}]
set_property PACKAGE_PIN T17 [get_ports {axi_gpio_tft_data_bus_tri_io[6]}]
set_property PACKAGE_PIN Y17 [get_ports {axi_gpio_tft_data_bus_tri_io[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {axi_gpio_tft_data_bus_tri_io[*]}]

set_property PACKAGE_PIN N18 [get_ports IIC_0_scl_io]
set_property IOSTANDARD LVCMOS33 [get_ports IIC_0_scl_io]
set_property PACKAGE_PIN N17 [get_ports IIC_0_sda_io]
set_property IOSTANDARD LVCMOS33 [get_ports IIC_0_sda_io]

# Must set some bank-compatible digital IOSTANDARD for the analog pins though this is bogus because Xilinx automatically selects the correct IOSTANDARD.
set_property IOSTANDARD LVCMOS33 [get_ports Vaux14_v_n]
set_property IOSTANDARD LVCMOS33 [get_ports Vaux14_v_p]
set_property IOSTANDARD LVCMOS33 [get_ports Vaux15_v_n]
set_property IOSTANDARD LVCMOS33 [get_ports Vaux15_v_p]
set_property IOSTANDARD LVCMOS33 [get_ports Vaux6_v_n]
set_property IOSTANDARD LVCMOS33 [get_ports Vaux6_v_p]
set_property IOSTANDARD LVCMOS33 [get_ports Vaux7_v_n]
set_property IOSTANDARD LVCMOS33 [get_ports Vaux7_v_p]

set_property PACKAGE_PIN K18 [get_ports {AC_BCLK[0]}]
set_property PACKAGE_PIN T19 [get_ports AC_MCLK]
set_property PACKAGE_PIN P18 [get_ports {AC_MUTE_N[0]}]
set_property PACKAGE_PIN L17 [get_ports {AC_PBLRC[0]}]
set_property PACKAGE_PIN M18 [get_ports {AC_RECLRC[0]}]
set_property PACKAGE_PIN M17 [get_ports {AC_SDATA_O[0]}]
set_property PACKAGE_PIN K17 [get_ports AC_SDATA_I]
set_property IOSTANDARD LVCMOS33 [get_ports AC*]

#This constraint ensures the MMCM located in the clock region connected to the ZYBO's HDMI port
#is used for the axi_dispctrl core driving the HDMI port
set_property LOC MMCME2_ADV_X0Y0 [get_cells system_i/axi_dispctrl_0/inst/DONT_USE_BUFR_DIV5.Inst_mmcme2_drp/mmcm_adv_inst]

#False path constraints for crossing clock domains in the Audio and Display cores.
#Synchronization between the clock domains is handled properly in logic.
#TODO: The following constraints should be changed to identify the proper pins
#      of the cores by their hierarchical pin names. Currently the global clock names are
#      used. Ultimately, it would be nice to have the cores automatically generate them.
#adi_i2s constaints:
set_false_path -from [get_clocks clk_fpga_0] -to [get_clocks clk_fpga_2]
set_false_path -from [get_clocks clk_fpga_2] -to [get_clocks clk_fpga_0]

#axi_dispctrl constraints:
#Note these constraints require that REFCLK be driven by the axi_lite clock (clk_fpga_0)
set_false_path -from [get_clocks clk_fpga_0] -to [get_clocks axi_dispctrl_1_PXL_CLK_O]
set_false_path -from [get_clocks axi_dispctrl_1_PXL_CLK_O] -to [get_clocks clk_fpga_0]

create_generated_clock -name vga_pxlclk -source [get_pins {system_i/processing_system7_0/inst/PS7_i/FCLKCLK[0]}] -multiply_by 1 [get_pins system_i/axi_dispctrl_0/inst/DONT_USE_BUFR_DIV5.BUFG_inst/O]
set_false_path -from [get_clocks vga_pxlclk] -to [get_clocks clk_fpga_0]
set_false_path -from [get_clocks clk_fpga_0] -to [get_clocks vga_pxlclk]
