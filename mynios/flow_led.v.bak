 
module flow_led(
  input sys_clk,
  input key1,
  input key2,
  output led,
 
	 //flash
	 //input  flash_data0,
	 //output flash_sdo,
	 //output flash_sce,
	 //output flash_dclk,
/*
set_location_assignment PIN_K1 -to flash_data0
set_location_assignment PIN_D1 -to flash_sdo
set_location_assignment PIN_E2 -to flash_sce
set_location_assignment PIN_K2 -to flash_dclk
set_global_assignment -name CYCLONEII_RESERVE_NCEO_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name RESERVE_DATA0_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name RESERVE_DATA1_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name RESERVE_FLASH_NCE_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name RESERVE_DCLK_AFTER_CONFIGURATION "USE AS REGULAR IO"
*/
  //uart接口
  input uart_rxd,
  output uart_txd,
 
  output segled_clk,
  output segled_dat, 
  output segled_str,

  output [7:0] debug,


  //SDRAM 芯片接口
  output        sdram_clk_out,                //SDRAM 芯片时钟
  output        sdram_cke,                //SDRAM 时钟有效
  output        sdram_cs_n,               //SDRAM 片选
  output        sdram_ras_n,              //SDRAM 行有效
  output        sdram_cas_n,              //SDRAM 列有效
  output        sdram_we_n,               //SDRAM 写有效
  output [ 1:0] sdram_ba,                 //SDRAM Bank地址
  output [12:0] sdram_addr,               //SDRAM 行/列地址
  inout  [15:0] sdram_data,               //SDRAM 数据
  output [ 1:0] sdram_dqm,                //SDRAM 数据掩码
 
	output uart2_txd,
	input uart2_rxd,
 
		input  wire        spi_MISO,        //     spi.MISO
		output wire        spi_MOSI,        //        .MOSI
		output wire        spi_SCLK,        //        .SCLK
		output wire [2:0]  spi_SS_n         //        .SS_n

);
wire sys_rst_n;
assign sys_rst_n = key1;
wire rst_n;
wire clk_100m;
wire clk_100m_shift;
wire locked;
//待PLL输出稳定之后，停止系统复位
assign rst_n = sys_rst_n & locked;
assign	sdram_clk_out = clk_100m_shift;//out_clk;                //将相位偏移时钟输出给sdram芯片
//例化PLL, 产生各模块所需要的时钟
sdram_pll(
  .inclk0             (sys_clk),
  .areset             (~sys_rst_n),
 
  .c0                 (clk_100m),
  .c1                 (clk_100m_shift),
  .locked             (locked)
);

wire [7:0] seg_data0;
wire [7:0] seg_data1;
wire [7:0] seg_data2;
wire [7:0] seg_data3;
seg_led_hex595 ins_seg_led_hex595(
  .sys_clk(sys_clk), 
  .sys_rst_n(sys_rst_n),
 
  .clk(segled_clk),
  .dat(segled_dat),
  .str(segled_str),
 
  .data0(seg_data0),
  .data1(seg_data1),
  .data2(seg_data2),
  .data3(seg_data3)
);

 
assign seg_data3 = debug32[31:24];//outpin32
assign seg_data2 = debug32[23:16];//outpin32
assign seg_data1 = debug32[15:8]; //outpin32
assign seg_data0 = debug32[7:0];  //outpin32

wire [31:0] outpin32;
wire [7:0] inpin8;
assign inpin8[0] = key2;

wire [7:0] debug8;
wire [31:0] debug32;

assign led = debug[0];
assign debug = debug8;

wire [7:0] myuart_debug8;
wire [31:0] myuart_debug32;
 
    sys u0 (
        .clk50_clk      (sys_clk),        //     clk.clk
        .reset_reset_n  (rst_n),  //   reset.reset_n
        .clk100_clk     (clk_100m),     //  clk100.clk

        .pio0out_export (outpin32), // pio0out.export
        .pio1in_export  (inpin8),  //  pio1in.export

        .mycpu_uart_rxd (uart_rxd), //        .uart_rxd
        .mycpu_uart_txd (uart_txd), //   mycpu.uart_txd
        .mycpu_debug8   (debug8),    //        .debug
        .mycpu_debug32  (debug0),   //        .debug0

        .sdram_addr   (sdram_addr),   // sdram_0.addr
        .sdram_ba     (sdram_ba),     //        .ba
        .sdram_cas_n  (sdram_cas_n),  //        .cas_n
        .sdram_cke    (sdram_cke),    //        .cke
        .sdram_cs_n   (sdram_cs_n),   //        .cs_n
        .sdram_dq     (sdram_data),     //        .dq
        .sdram_dqm    (sdram_dqm),    //        .dqm
        .sdram_ras_n  (sdram_ras_n),  //        .ras_n
        .sdram_we_n   (sdram_we_n),   //        .we_n
				
				.myuart_rxd     (uart2_rxd),     //  myuart.rxd
        .myuart_txd     (uart2_txd),      //        .txd

        .softspi_MISO        (spi_MISO),        //     spi.MISO
        .softspi_MOSI        (spi_MOSI),        //        .MOSI
        .softspi_SCLK        (spi_SCLK),        //        .SCLK
        .softspi_SS_n        (spi_SS_n)         //        .SS_n    
		 );


endmodule
            

  