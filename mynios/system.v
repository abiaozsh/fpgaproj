module system (
    input  wire        clk,  //   clk50.clk
    input  wire        reset_n,  //   reset.reset_n
		input clk_50M,
		input vga_clk_25M,
		input vga_clk_65M,
		input clk_100m,
		input clk_100m_shift,

    output        mycpu_uart_txd, //   mycpu.uart_txd
    input         mycpu_uart_rxd, //        .uart_rxd
    output [3:0]  debug4, 
    output [7:0]  debug8, 
    output [31:0] debug32,

    //SDRAM 芯片接口
    output        sdram_clk_out,            //SDRAM 芯片时钟
    output        sdram_cke,                //SDRAM 时钟有效
    output        sdram_cs_n,               //SDRAM 片选
    output        sdram_ras_n,              //SDRAM 行有效
    output        sdram_cas_n,              //SDRAM 列有效
    output        sdram_we_n,               //SDRAM 写有效
    output [ 1:0] sdram_ba,                 //SDRAM Bank地址
    output [12:0] sdram_addr,               //SDRAM 行/列地址
    inout  [15:0] sdram_data,               //SDRAM 数据
    output [ 1:0] sdram_dqm,                //SDRAM 数据掩码

    input   [2:0]  softspi_MISO,   // softspi.MISO
    output  [2:0]  softspi_MOSI,   //        .MOSI
    output  [2:0]  softspi_SCLK,   //        .SCLK
    output  [2:0]  softspi_CS,   //        .SS_n
    
    input          myuart_rxd,     //  myuart.rxd
    output         myuart_txd,     //        .txd

    input hid_dat,
    input hid_clk,
    input hid_str,

     //SDRAM2m 芯片接口
    //output        sdram2m_clk_out,            //SDRAM 芯片时钟
    //output        sdram2m_cke,                //SDRAM 时钟有效
    //output        sdram2m_cs_n,               //SDRAM 片选
    //output        sdram2m_ras_n,              //SDRAM 行有效
    //output        sdram2m_cas_n,              //SDRAM 列有效
    //output        sdram2m_we_n,               //SDRAM 写有效
    //output        sdram2m_ba,                 //SDRAM Bank地址
    //output [10:0] sdram2m_addr,               //SDRAM 行/列地址
    //inout  [15:0] sdram2m_data,               //SDRAM 数据

/*
    output cyok,
    output reg [7:0] cy_cmd,
    output reg [7:0] cy_dat,
    output reg [7:0] cy_snd_data0,
    output reg [7:0] cy_snd_data1,
     //input cy_SCL,
  //input cy_SDA,
  inout [7:0] cy_D,
  inout [7:0] cy_B,
  output cy_IFCLK_out                     ,
  input cy_to_fpga_CTL0_FLAGA        ,
  input cy_to_fpga_CTL2_FLAGC        ,
  input cy_to_fpga_CTL1_FLAGB        ,
  input cy_to_fpga_A7_FLAGD          ,
  output  cy_from_fpga_RDY1_SLWR       ,//output
  output  cy_from_fpga_RDY0_SLRD       ,//output
  input cy_A0_INT0                   ,
  output cy_A1_INT1                   ,
  output  cy_from_fpga_A2_SLOE         ,//output
  input cy_A3_WU2                    ,
  //output  cy_from_fpga_A4_FIFOADR0     ,//output
  output  cy_from_fpga_A5_FIFOADR1     ,//output
  //output  cy_from_fpga_A6_PKTEND       ,//output
*/
    //VGA接口                          
    output          vga_hs,         //行同步信号
    output          vga_vs,         //场同步信号
    output  [15:0]  vga_rgb,         //红绿蓝三原色输出 
     
    
    
    input dummy
  );
  
  
  
  wire sdrambus_debug8;
  assign debug32 = mycpu_debug32;
  wire [31:0] sdrambus_debug32;

  wire [7:0]  mycpu_debug8;
  wire [31:0]  mycpu_debug32;
  wire [31:0] avm_m0_address;
  wire        avm_m0_read;
  wire        avm_m0_write;
  wire [31:0] avm_m0_writedata;
  wire [ 3:0] avm_m0_byteenable;
  mycpu mycpu_0
    (
    .clk                (clk),                       //       clock.clk
    .clk_50M            (clk_50M),

    .reset_n            (reset_n), //       reset.reset_n
    .avm_m0_address     (avm_m0_address),              //          m0.address
    .avm_m0_read        (avm_m0_read),                 //            .read
    .avm_m0_write       (avm_m0_write),                //            .write
    .avm_m0_readdata    (avm_m0_readdata),             //            .readdata
    .avm_m0_writedata   (avm_m0_writedata),            //            .writedata
    .avm_m0_byteenable  (avm_m0_byteenable),           //            .byteenable
    .avm_m0_waitrequest (avm_m0_waitrequest),          //            .waitrequest
    .irq_req            (irq_req),                //        irq0.irq
    
    .uart_txd           (mycpu_uart_txd),                  // conduit_end.export
    .uart_rxd           (mycpu_uart_rxd),                  //            .export
    .debug8             (mycpu_debug8),                    //            .export
    .debug32            (mycpu_debug32),                    //            .export
      
    .debugin8           (spidebug8    ),
    .cache_life_addr    (cache_life_addr     ),
    .cache_life_data    (cache_life_data     ),
    .cacheAddrHigh_addr    (cacheAddrHigh_addr     ),
    .cacheAddrHigh_data    (cacheAddrHigh_data     ),
    .debugin32  (sdrambus_debug32)
      
  );

  wire [31:0] avm_m0_readdata;
  wire        avm_m0_waitrequest;
  assign avm_m0_waitrequest = (sdrambus_cs ? sdrambus_waitrequest : 1'b0) |
                              (mainSRAM_cs ? mainSRAM_waitrequest : 1'b0) |
                              (mytimer_cs  ? mytimer_waitrequest  : 1'b0) |
                              (myuart_cs   ? myuart_waitrequest   : 1'b0) |
                              (softspi_cs  ? softspi_waitrequest  : 1'b0) |
                              (vga_cs      ? vga_waitrequest      : 1'b0);
  
  
  assign avm_m0_readdata =  (sdrambus_cs ? sdrambus_readdata : 32'b0) |
                            (mainSRAM_cs ? mainSRAM_readdata : 32'b0) |
                            (irq_Ctrl_cs ? irq_Ctrl_readdata : 32'b0) |
                            (mytimer_cs  ? mytimer_readdata  : 32'b0) |
                            (myuart_cs   ? myuart_readdata   : 32'b0) |
                            (softspi_cs  ? softspi_readdata  : 32'b0) |
                            (hid_cs      ? hid_readdata      : 32'b0) |
                            (rnd_cs      ? rnd_readdata      : 32'b0);

  
  
  
  
  
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  
  

  wire        read_line_req         ;//input read_buffA_req,
  wire        read_line_A_B         ;//input read_buffB_req,
  wire [15:0] read_line_addr        ;//input [9:0] read_buff_addr,
  wire [15:0] read_pixelA_data      ;//output [15:0] buff_readA_data,
  wire [15:0] read_pixelB_data      ;//output [15:0] buff_readB_data,
  wire  [9:0] read_pixel_addr       ;//input [9:0]   buff_readB_addr,
  wire        read_pixel_clk        ;//input         buff_readB_clk,

  wire [3:0]  cache_life_addr;
  wire [15:0] cache_life_data;
  wire [3:0]  cacheAddrHigh_addr;
  wire [15:0] cacheAddrHigh_data;


  wire sdrambus_cs = avm_m0_address[31:25] == 7'h0;// 0000:0000 ~ 01FF:FFFF 32M byte

  wire [31:0] sdrambus_readdata;
  wire sdrambus_waitrequest;
  sdrambusvga sdrambusvga_inst (
    .clk                (clk),                       //       clock.clk
    .clk_100m           (clk_100m),
    .clk_100m_shift     (clk_100m_shift),
    .reset_n            (reset_n), //       reset.reset_n
    .avs_s0_address     (sdrambus_address ),
    .avs_s0_read        (sdrambus_read ),
    .avs_s0_write       (sdrambus_write ),
    .avs_s0_readdata    (sdrambus_readdata ),
    .avs_s0_writedata   (avm_m0_writedata ),
    .avs_s0_waitrequest (sdrambus_waitrequest ),
    .avs_s0_byteenable  (avm_m0_byteenable ),
    
    //SDRAM 芯片接口
    .sdram_clk_out      (sdram_clk_out),
    .sdram_cke          (sdram_cke    ),
    .sdram_cs_n         (sdram_cs_n   ),
    .sdram_ras_n        (sdram_ras_n  ),
    .sdram_cas_n        (sdram_cas_n  ),
    .sdram_we_n         (sdram_we_n   ),
    .sdram_ba           (sdram_ba     ),
    .sdram_addr         (sdram_addr   ),
    .sdram_data         (sdram_data   ),
    .sdram_dqm          (sdram_dqm    ),
      
    .read_line_req   (read_line_req       ),//input read_buffA_req,
    .read_line_A_B   (read_line_A_B       ),//input read_buffB_req,
    .read_line_addr  (read_line_addr      ),//input [9:0] read_buff_addr,
    .read_pixelA_data(read_pixelA_data    ),//output [15:0] buff_readA_data,
    .read_pixelB_data(read_pixelB_data    ),//output [15:0] buff_readB_data,
    .read_pixel_addr (read_pixel_addr     ),//input [9:0]   buff_readB_addr,
    .read_pixel_clk  (read_pixel_clk      ),//input         buff_readB_clk,

    .flush_cache    (flush_cache),
    .flush_page     (flush_page ),
    
    .debug8                (sdrambus_debug8 ),
    .cache_life_addr       (cache_life_addr     ),
    .cache_life_data       (cache_life_data     ),
    .cacheAddrHigh_addr    (cacheAddrHigh_addr     ),
    .cacheAddrHigh_data    (cacheAddrHigh_data     ),
    .debug32               (sdrambus_debug32)

  );
  wire [22:0] sdrambus_address;
  assign      sdrambus_address = avm_m0_address[24:2];//~[0]
  
  wire   sdrambus_read  = sdrambus_cs ? avm_m0_read  : 1'b0;
  wire   sdrambus_write = sdrambus_cs ? avm_m0_write : 1'b0;
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  
  
  //512dword = 2048byte
  wire mainSRAM_cs = avm_m0_address[31:16] == 16'h0200;
  
  wire [31:0] mainSRAM_readdata;
  mainSRAM  mainSRAM_inst (
    .clock ( clk ),
    .data ( avm_m0_writedata ),
    .address ( mainSRAM_address ),
    .byteena ( avm_m0_byteenable ),
    .wren ( mainSRAM_write ),
    .q ( mainSRAM_readdata )
  );
  
reg mainSRAM_read_ack;
wire mainSRAM_waitrequest = ((mainSRAM_cs && avm_m0_read) && !mainSRAM_read_ack);
always@(posedge clk or negedge reset_n) begin
  if(!reset_n) begin
    mainSRAM_read_ack <= 0;
  end else begin
    if((mainSRAM_cs && avm_m0_read) && !mainSRAM_read_ack)begin
      mainSRAM_read_ack <= 1;
    end
    
    if(!(mainSRAM_cs && avm_m0_read) && mainSRAM_read_ack)begin
      mainSRAM_read_ack <= 0;
    end

  end
end

  wire mainSRAM_write = mainSRAM_cs ? avm_m0_write : 1'b0;
  
  wire [13:0] mainSRAM_address = avm_m0_address[15:2];//~[12:0]

  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  //assign debug8[0] = flush_cache;//mouse_send_req_buff;
  //assign debug8[1] = mbitpos[1];//mouse_send_ack;
  //assign debug8[2] = mbitpos[2];//mouse_read_req_buff;
  //assign debug8[3] = mbitpos[3];//mouse_read_ack;
  //assign debug8[4] = send_err;
  //assign debug8[5] = read_err;
  //assign debug8[7:6] = mouse_send_state;

  wire cacheCtrl_cs = avm_m0_address[31:16] == 16'h0201;
  reg              flush_cache;
  reg   [14:0]     flush_page;
  always@(posedge clk or negedge reset_n) begin
    if(!reset_n) begin
      flush_cache <= 0;
      flush_page <= 0;
    end else begin
      if(cacheCtrl_cs && avm_m0_write)begin
        if         (avm_m0_address[15:2]==0)begin
          flush_cache <= avm_m0_writedata[31];
          flush_page = avm_m0_writedata[14:0];
        end else if(avm_m0_address[15:2]==1)begin
        end
      end
    end
  end
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  wire irq_Ctrl_cs = avm_m0_address[31:16] == 16'h0202;

  wire irq_req = irq_num!=0;
  reg [31:0] irq_num;
  wire [31:0] irq_Ctrl_readdata = irq_num;

  wire mytimer_irq_req;
  reg  mytimer_irq_req_buff;
  reg  mytimer_irq_ack;
  wire myuart_irq_req;
  reg  myuart_irq_ack;
  reg  hid_irq_req;
  reg  hid_irq_req_buff;
  reg  hid_irq_ack;

  always@(posedge clk or negedge reset_n) begin
    if(!reset_n) begin
      irq_num <= 0;
      myuart_irq_ack <= 0;
    end else begin
      if(irq_Ctrl_cs && avm_m0_write)begin
        if         (avm_m0_address[15:2]==0)begin
          irq_num <= irq_num & (~avm_m0_writedata);
        end else if(avm_m0_address[15:2]==1)begin
        end
      end
      mytimer_irq_req_buff = mytimer_irq_req;
      hid_irq_req_buff = hid_irq_req;
      
      if(mytimer_irq_req_buff && !mytimer_irq_ack)begin
        irq_num[0] <= 1;
        mytimer_irq_ack <= 1;
      end
      if(!mytimer_irq_req_buff && mytimer_irq_ack)begin
        mytimer_irq_ack <= 0;
      end

      if(myuart_irq_req && !myuart_irq_ack)begin
        irq_num[1] <= 1;
        myuart_irq_ack <= 1;
      end
      if(!myuart_irq_req && myuart_irq_ack)begin
        myuart_irq_ack <= 0;
      end
      
      if(hid_irq_req_buff && !hid_irq_ack)begin
        irq_num[2] <= 1;
        hid_irq_ack <= 1;
      end
      if(!hid_irq_req_buff && hid_irq_ack)begin
        hid_irq_ack <= 0;
      end

    end
  end
  
  
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  
  
  wire mytimer_cs = avm_m0_address[31:16] == 16'h0203;

  wire [31:0] mytimer_readdata;
  wire        mytimer_waitrequest;
  mytimer mytimer_inst (
    .clk                (clk),                       //       clock.clk
		.clk_50M            (clk_50M),
    .reset_n            (reset_n), //       reset.reset_n
    .avs_s0_address     (avm_m0_address ),
    .avs_s0_read        (mytimer_read ),
    .avs_s0_write       (mytimer_write ),
    .avs_s0_readdata    (mytimer_readdata ),
    .avs_s0_writedata   (avm_m0_writedata ),
    .avs_s0_waitrequest (mytimer_waitrequest ),
    .avs_s0_byteenable  (avm_m0_byteenable ),
    .irq_req            (mytimer_irq_req),       //  irq0.irq
    .irq_ack            (mytimer_irq_ack)
  );
  
  wire mytimer_read;
  assign mytimer_read = mytimer_cs ? avm_m0_read : 1'b0;
  wire mytimer_write;
  assign mytimer_write = mytimer_cs ? avm_m0_write : 1'b0;
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  
  wire myuart_cs = avm_m0_address[31:16] == 16'h0204;

  wire [31:0] myuart_readdata;
  wire        myuart_waitrequest;
  myuart myuart_inst (
    .clk                (clk),                       //       clock.clk
		.clk_50M            (clk_50M),
    .reset_n            (reset_n), //       reset.reset_n
    .avs_s0_address     (myuart_address ),
    .avs_s0_read        (myuart_read ),
    .avs_s0_write       (myuart_write ),
    .avs_s0_readdata    (myuart_readdata ),
    .avs_s0_writedata   (avm_m0_writedata ),
    .avs_s0_waitrequest (myuart_waitrequest ),
    .avs_s0_byteenable  (avm_m0_byteenable ),
    .irq_req            (myuart_irq_req),       //  irq0.irq
    .irq_ack            (myuart_irq_ack),

    
    .uart_rxd           (myuart_rxd),
    .uart_txd           (myuart_txd)
  );
  
  wire        myuart_address;
  assign myuart_address = avm_m0_address[2];//~[0]
  
  wire myuart_read;
  assign myuart_read = myuart_cs ? avm_m0_read : 1'b0;
  wire myuart_write;
  assign myuart_write = myuart_cs ? avm_m0_write : 1'b0;
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  //assign debug8 = spidebug8;
 
  wire softspi_cs = avm_m0_address[31:16] == 16'h0205;

  wire [31:0] softspi_readdata;
  wire        softspi_waitrequest;
  wire [7:0] spidebug8;
  softspi softspi_inst (
    .clk                (clk),                       //       clock.clk
		.clk_25M            (vga_clk_25M),
    .reset_n            (reset_n), //       reset.reset_n
    .avs_s0_address     (softspi_address ),
    .avs_s0_read        (softspi_read ),
    .avs_s0_write       (softspi_write ),
    .avs_s0_readdata    (softspi_readdata ),
    .avs_s0_writedata   (avm_m0_writedata ),
    .avs_s0_waitrequest (softspi_waitrequest ),
    .avs_s0_byteenable  (avm_m0_byteenable ),
    
    .debug4 (debug4),
    .debug8 (spidebug8),
    
    .MISO           (softspi_MISO),
    .MOSI           (softspi_MOSI),
    .SCLK           (softspi_SCLK),
    .CS             (softspi_CS)

  );
  
  wire   [13:0] softspi_address;
  assign softspi_address = avm_m0_address[15:2];//~[0]
  
  wire softspi_read = softspi_cs ? avm_m0_read : 1'b0;
  wire softspi_write = softspi_cs ? avm_m0_write : 1'b0;

  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  wire vga_cs = avm_m0_address[31:16] == 16'h0206;
  always@(posedge clk or negedge reset_n) begin
    if(!reset_n) begin
      vga_mode <= 1;
      blockvga <= 0;
      read_line_base_addr <= 0;
      write_cursor_ena <= 0;
      write_cursor_addr <= 0;
      write_cursor_data <= 0;
    end else begin
      write_cursor_ena <= 0;
      write_cursor_addr <= avm_m0_address[11:2];
      write_cursor_data <= avm_m0_writedata[15:0];
      if(vga_cs && avm_m0_write)begin
      
        case(avm_m0_address[15:13])
        3'b000 : begin // 0x0000 dw
          vga_mode <= avm_m0_writedata[0];
        end
        3'b001 : begin // 0x0800 dw
          blockvga <= avm_m0_writedata[0];
        end
        3'b010 : begin // 0x1000 dw
          read_line_base_addr <= avm_m0_writedata[15:0];
        end
        3'b011 : begin // 0x1800 dw
          write_cursor_ena <= 1;
        end
        3'b100 : begin // 0x2000 dw
          cursor_posX <= avm_m0_writedata[9:0];
        end
        3'b101 : begin // 0x2800 dw
          cursor_posY <= avm_m0_writedata[9:0];
        end
        endcase
        
      end
    end
  end
  wire vga_waitrequest = 0;
  
  //128m byte range 64k line * 2048byte per line
  reg vga_mode;
  reg blockvga;
  reg  [15:0] read_line_base_addr   ;//input [9:0] read_buff_addr,
  reg [15:0] write_cursor_data;
  reg [9:0] write_cursor_addr;
  reg write_cursor_ena;
  reg  [9:0]  cursor_posX;//0~1024
  reg  [9:0]  cursor_posY;//0~1024
  
  wire [15:0] read_cursor_data;
  wire [9:0]  read_cursor_addr;//32 * 32
  wire        read_cursor_clk;

  buff1024x16  buffCursor (
    .data      ( write_cursor_data  ),
    .wraddress ( write_cursor_addr  ),
    .wrclock   ( clk                ),
    .wren      ( write_cursor_ena   ),
    .rdaddress ( read_cursor_addr   ),
    .rdclock   ( read_cursor_clk    ),
    .q         ( read_cursor_data   )
  );
  
wire [7:0] vga_debug;
assign debug8 = vga_debug;

  wire blanking;
  vga_driver8m u_vga_driver8m(
    .sys_rst_n      (reset_n),
		.vga_clk_25M(vga_clk_25M),
		.vga_clk_65M(vga_clk_65M),

    .blockvga(blockvga),
    .vga_mode(vga_mode),
    .blanking(blanking),
    .read_line_base_addr (read_line_base_addr ),
    
    .read_line_req       (read_line_req       ),
    .read_line_A_B       (read_line_A_B       ),
    .read_line_addr      (read_line_addr      ),
    
    .read_pixelA_data    (read_pixelA_data    ),
    .read_pixelB_data    (read_pixelB_data    ),
    .read_pixel_addr     (read_pixel_addr     ),
    .read_pixel_clk      (read_pixel_clk      ),
    
    
    .cursor_posX(cursor_posX),//0~1024
    .cursor_posY(cursor_posY),//0~1024
    .read_cursor_data(read_cursor_data),
    .read_cursor_addr(read_cursor_addr),//32 * 32
    .read_cursor_clk (read_cursor_clk ),

    .vga_hs         (vga_hs),
    .vga_vs         (vga_vs),
    .vga_rgb        (vga_rgb),
    
    .debug(vga_debug)
  );

  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  
  wire hid_cs = avm_m0_address[31:16] == 16'h0207;
  
  reg [31:0] hid_data;

  always@(posedge hid_clk or negedge reset_n) begin
    if(!reset_n) begin
      hid_data <= 0;
    end else begin
      //hid_data <= hid_data+1'b1;
      hid_data <= {hid_dat,hid_data[31:1]};
    end
  end
  
  wire [31:0] hid_readdata = arrived ? hid_data : 0;
  wire pCheckOk = (hid_data[0] ^  hid_data[1] ^  hid_data[2] ^  hid_data[3] ^  hid_data[4] ^  hid_data[5] ^  hid_data[6] ^  hid_data[7] ^ 
                   hid_data[8] ^  hid_data[9] ^ hid_data[10] ^ hid_data[11] ^ hid_data[12] ^ hid_data[13] ^ hid_data[14] ^ hid_data[15] ^ 
                  hid_data[16] ^ hid_data[17] ^ hid_data[18] ^ hid_data[19] ^ hid_data[20] ^ hid_data[21] ^ hid_data[22] ^ hid_data[23] ^ 
                  hid_data[24]) == hid_data[28];
  
  reg arrived;
  reg hid_str_buff1_clk;
  reg hid_str_buff2_clk;
  always@(posedge clk or negedge reset_n) begin
    if(!reset_n) begin
      arrived <= 0;
      hid_str_buff1_clk <= 0;
      hid_str_buff2_clk <= 0;
    end else begin
    

      if(hid_cs && avm_m0_read)begin
        arrived <= 0;
      end
      
      hid_str_buff1_clk <= hid_str;
      hid_str_buff2_clk <= hid_str_buff1_clk;
      if(hid_str_buff1_clk && !hid_str_buff2_clk && pCheckOk)begin
        arrived <= 1;
      end

    end
  end

  
  reg hid_str_buff1;
  reg hid_str_buff2;
  
  reg hid_irq_ack_buff;
  always@(posedge clk_50M or negedge reset_n) begin
    if(!reset_n) begin
      hid_irq_req <= 0;
      hid_str_buff1 <= 0;
      hid_str_buff2 <= 0;
    end else begin
      hid_str_buff1 <= hid_str;
      hid_str_buff2 <= hid_str_buff1;
      
      hid_irq_ack_buff <= hid_irq_ack;
      
      if(hid_str_buff1 && !hid_str_buff2)begin
        hid_irq_req <= 1;
      end
      
      if(hid_irq_ack_buff)begin
        hid_irq_req <= 0;
      end
    end
  end


  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  
  wire rnd_cs = avm_m0_address[31:16] == 16'h0208;

  wire [31:0] rnd_readdata = {31'b0,a^b};

  wire a;
  ringOscillator ro_a(
    .enable(reset_n),
    .ro_out(a)
  );

  wire b;
  ringOscillator ro_b(
    .enable(reset_n),
    .ro_out(b)
  );

endmodule


module ringOscillator(
  input enable,
  output ro_out
);
//(* DONT_TOUCH = "1"*) 
wire      notgate_in/* synthesis syn_keep=1 */;
//(* DONT_TOUCH = "1"*) 
wire[4:0] notgate_out/* synthesis syn_keep=1 */;

and u1(notgate_in, notgate_out[4], enable);

not u2(notgate_out[0], notgate_in);

not u3(notgate_out[1], notgate_out[0]);

not u4(notgate_out[2], notgate_out[1]);

not u5(notgate_out[3], notgate_out[2]);

not u6(notgate_out[4], notgate_out[3]);

assign ro_out = notgate_out[4];


endmodule
