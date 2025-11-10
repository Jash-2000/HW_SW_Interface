//Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2022.1 (lin64) Build 3526262 Mon Apr 18 15:47:01 MDT 2022
//Date        : Thu Nov  6 18:42:07 2025
//Host        : edalinux15 running 64-bit Rocky Linux release 8.10 (Green Obsidian)
//Command     : generate_target bd_wrapper.bd
//Design      : bd_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module bd_wrapper
   (D0_AN,
    D0_SEG,
    D1_AN,
    D1_SEG,
    JA,
    LED,
    RGB0,
    RGB1,
    SW,
    btn,
    ddr3_addr,
    ddr3_ba,
    ddr3_cas_n,
    ddr3_ck_n,
    ddr3_ck_p,
    ddr3_cke,
    ddr3_dm,
    ddr3_dq,
    ddr3_dqs_n,
    ddr3_dqs_p,
    ddr3_odt,
    ddr3_ras_n,
    ddr3_reset_n,
    ddr3_we_n,
    io0_o,
    reset_spi,
    sck_o,
    spi_dc,
    ss_o,
    sys_clk_n,
    sys_clk_p);
  output [3:0]D0_AN;
  output [7:0]D0_SEG;
  output [3:0]D1_AN;
  output [7:0]D1_SEG;
  input [2:0]JA;
  output [15:0]LED;
  output [2:0]RGB0;
  output [2:0]RGB1;
  input [15:0]SW;
  input [3:0]btn;
  output [12:0]ddr3_addr;
  output [2:0]ddr3_ba;
  output ddr3_cas_n;
  output [0:0]ddr3_ck_n;
  output [0:0]ddr3_ck_p;
  output [0:0]ddr3_cke;
  output [1:0]ddr3_dm;
  inout [15:0]ddr3_dq;
  inout [1:0]ddr3_dqs_n;
  inout [1:0]ddr3_dqs_p;
  output [0:0]ddr3_odt;
  output ddr3_ras_n;
  output ddr3_reset_n;
  output ddr3_we_n;
  output io0_o;
  output [0:0]reset_spi;
  output sck_o;
  output [0:0]spi_dc;
  output [0:0]ss_o;
  input sys_clk_n;
  input sys_clk_p;

  wire [3:0]D0_AN;
  wire [7:0]D0_SEG;
  wire [3:0]D1_AN;
  wire [7:0]D1_SEG;
  wire [2:0]JA;
  wire [15:0]LED;
  wire [2:0]RGB0;
  wire [2:0]RGB1;
  wire [15:0]SW;
  wire [3:0]btn;
  wire [12:0]ddr3_addr;
  wire [2:0]ddr3_ba;
  wire ddr3_cas_n;
  wire [0:0]ddr3_ck_n;
  wire [0:0]ddr3_ck_p;
  wire [0:0]ddr3_cke;
  wire [1:0]ddr3_dm;
  wire [15:0]ddr3_dq;
  wire [1:0]ddr3_dqs_n;
  wire [1:0]ddr3_dqs_p;
  wire [0:0]ddr3_odt;
  wire ddr3_ras_n;
  wire ddr3_reset_n;
  wire ddr3_we_n;
  wire io0_o;
  wire [0:0]reset_spi;
  wire sck_o;
  wire [0:0]spi_dc;
  wire [0:0]ss_o;
  wire sys_clk_n;
  wire sys_clk_p;

  bd bd_i
       (.D0_AN(D0_AN),
        .D0_SEG(D0_SEG),
        .D1_AN(D1_AN),
        .D1_SEG(D1_SEG),
        .JA(JA),
        .LED(LED),
        .RGB0(RGB0),
        .RGB1(RGB1),
        .SW(SW),
        .btn(btn),
        .ddr3_addr(ddr3_addr),
        .ddr3_ba(ddr3_ba),
        .ddr3_cas_n(ddr3_cas_n),
        .ddr3_ck_n(ddr3_ck_n),
        .ddr3_ck_p(ddr3_ck_p),
        .ddr3_cke(ddr3_cke),
        .ddr3_dm(ddr3_dm),
        .ddr3_dq(ddr3_dq),
        .ddr3_dqs_n(ddr3_dqs_n),
        .ddr3_dqs_p(ddr3_dqs_p),
        .ddr3_odt(ddr3_odt),
        .ddr3_ras_n(ddr3_ras_n),
        .ddr3_reset_n(ddr3_reset_n),
        .ddr3_we_n(ddr3_we_n),
        .io0_o(io0_o),
        .reset_spi(reset_spi),
        .sck_o(sck_o),
        .spi_dc(spi_dc),
        .ss_o(ss_o),
        .sys_clk_n(sys_clk_n),
        .sys_clk_p(sys_clk_p));
endmodule
