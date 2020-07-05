module sdrambus(
    input          clk,                // clock.clk
    input          reset_n,               // reset.reset
    
    input   [22:0]     avs_s0_address,     //    s0.address
    input              avs_s0_read,        //      .read
    input              avs_s0_write,       //      .write
    output  reg [31:0] avs_s0_readdata,    //      .readdata
    input   [31:0]     avs_s0_writedata,   //      .writedata
    output             avs_s0_waitrequest, //      .waitrequest
    input   [3:0]      avs_s0_byteenable,    //      .readdata

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


    input             buffDMAread_req        ,
    output reg        buffDMAread_ack        ,
    input      [15:0] buffDMAread_addr       ,//64k sub page * 256word = 16M word
    output            buffDMAread_clk        ,
    input             buffDMAread_A_B        ,
    output reg [15:0] buffDMAread_wrdata     ,
    output reg  [7:0] buffDMAread_wraddress  ,
    output reg        buffDMAreadA_wren      ,
    output reg        buffDMAreadB_wren      ,
    
    output [7:0] debug8
    
);
wire sys_clk = clk;
wire sys_rst_n = reset_n;

wire rst_n;
wire clk_50m;
wire clk_100m;
wire clk_100m_shift;
wire locked;
//待PLL输出稳定之后，停止系统复位
assign rst_n = sys_rst_n & locked;

//例化PLL, 产生各模块所需要的时钟
pll_clk u_pll_clk(
  .inclk0             (sys_clk),
  .areset             (~sys_rst_n),
  
  .c0                 (clk_50m),
  .c1                 (clk_100m),
  .c2                 (clk_100m_shift),
  .locked             (locked)
);

wire sdram_clk;
assign sdram_clk = clk_100m;
assign  sdram_clk_out = clk_100m_shift;//out_clk;                //将相位偏移时钟输出给sdram芯片


reg         sdram_wr_req    ;    //写SDRAM请求信号              input 
wire        sdram_wr_ack    ;    //写SDRAM响应信号              output
reg [23:0]  sdram_rw_addr   ;  //SDRAM写操作的地址            input 
reg [ 9:0]  sdram_wr_burst  ;   //写sdram时数据突发长度      input 
reg [15:0]  sdram_din       ;      //写入SDRAM的数据              input 
reg [1:0]   sdram_mask      ;
reg         sdram_rd_req    ;    //读SDRAM请求信号              input 
wire        sdram_rd_ack    ;    //读SDRAM响应信号              output
reg [ 9:0]  sdram_rd_burst  ;   //读sdram时数据突发长度      input 
wire [15:0] sdram_dout      ;      //从SDRAM读出的数据            output
wire        sdram_init_done ;  //SDRAM 初始化完成标志       output

//SDRAM控制器
sdram_controller ins_sdram_controller(
  .clk        (sdram_clk),      //sdram 控制器时钟
  .rst_n        (rst_n),      //系统复位

  //SDRAM 芯片接口
  .sdram_cke       (sdram_cke),    //SDRAM 时钟有效
  .sdram_cs_n      (sdram_cs_n),    //SDRAM 片选
  .sdram_ras_n     (sdram_ras_n),    //SDRAM 行有效  
  .sdram_cas_n     (sdram_cas_n),    //SDRAM 列有效
  .sdram_we_n      (sdram_we_n),    //SDRAM 写有效
  .sdram_ba        (sdram_ba),      //SDRAM Bank地址
  .sdram_addr      (sdram_addr),    //SDRAM 行/列地址
  .sdram_data      (sdram_data),    //SDRAM 数据
  .sdram_dqm       (sdram_dqm),    //SDRAM 数据
  
  //SDRAM 控制器端口  
  .sdram_rw_addr     (sdram_rw_addr),   //sdram 地址
  .sdram_wr_req      (sdram_wr_req),   //sdram 写请求
  .sdram_wr_ack      (sdram_wr_ack),   //sdram 写响应
  .sdram_wr_burst    (sdram_wr_burst),        //写sdram时数据突发长度
  .sdram_din         (sdram_din),      //写入sdram中的数据
  .sdram_mask        (sdram_mask),
  .sdram_rd_req      (sdram_rd_req),   //sdram 读请求
  .sdram_rd_ack      (sdram_rd_ack),    //sdram 读响应
  .sdram_rd_burst    (sdram_rd_burst),        //读sdram时数据突发长度
  .sdram_dout        (sdram_dout),     //从sdram中读出的数据
  
  .block_auto_refresh (0),//write_en
  
  .sdram_init_done  (sdram_init_done)  //sdram 初始化完成标志

);


reg interface_step;
reg        read_sdram_req;
reg [23:0] rdwr_sdram_addr;
reg        write_single_sdram_req;
reg [31:0] write_single_sdram_data;
reg [3:0]  write_single_sdram_mask;
reg avs_s0_read_ack;
reg avs_s0_write_ack;
reg read_sdram_ack_buff;
reg write_single_sdram_ack_buff;

assign debug8[0] = avs_s0_waitrequest;
assign debug8[1] = avs_s0_read;
assign debug8[2] = avs_s0_read_ack;
assign debug8[3] = avs_s0_write;
assign debug8[4] = avs_s0_write_ack;


assign avs_s0_waitrequest = (avs_s0_read && !avs_s0_read_ack) || (avs_s0_write && !avs_s0_write_ack);
always@(posedge clk or negedge sys_rst_n) begin
  if(!sys_rst_n) begin
    avs_s0_read_ack <= 0;
    avs_s0_write_ack <= 0;
    avs_s0_readdata <= 0;
    interface_step <= 0;
    read_sdram_req <= 0;
    rdwr_sdram_addr <= 0;
    write_single_sdram_req <= 0;
    write_single_sdram_data <= 0;
    write_single_sdram_mask <= 2'b00;
    read_sdram_ack_buff <= 0;
    write_single_sdram_ack_buff <= 0;
  end else begin
    read_sdram_ack_buff <= read_sdram_ack;
    write_single_sdram_ack_buff <= write_single_sdram_ack;
    
    if(avs_s0_read && !avs_s0_read_ack)begin
      if(interface_step==0)begin
        interface_step <= 1;
        rdwr_sdram_addr <= {avs_s0_address,1'b0};
        read_sdram_req <= 1;
      end else if(interface_step==1)begin
        if(read_sdram_ack_buff)begin
          read_sdram_req <= 0;
          avs_s0_readdata <= readBuffer;
          avs_s0_read_ack <= 1;
          interface_step <= 0;
        end
      end
    end
    
    if(!avs_s0_read && avs_s0_read_ack)begin
      avs_s0_read_ack <= 0;
    end
    
    if(avs_s0_write && !avs_s0_write_ack)begin
      if(interface_step==0)begin
        interface_step <= 1;
        rdwr_sdram_addr <= {avs_s0_address,1'b0};
        write_single_sdram_data <= avs_s0_writedata;
        write_single_sdram_mask <= ~avs_s0_byteenable;
        write_single_sdram_req <= 1;
      end else if(interface_step==1)begin
        if(write_single_sdram_ack_buff)begin
          write_single_sdram_req <= 0;
          avs_s0_write_ack <= 1;
          interface_step <= 0;
        end
      end
        
    end
    
    if(!avs_s0_write && avs_s0_write_ack)begin
      avs_s0_write_ack <= 0;
    end
    
  end
end




assign buffDMAread_clk = sdram_clk;

reg read_sdram_req_buff;
reg buffDMAread_req_buff;
reg write_single_sdram_req_buff;
reg write_sdram_req_buff;

reg read_sdram_ack;
reg write_single_sdram_ack;
reg write_sdram_ack;

reg sdram_page_delay;
reg        sdram_timer0;
reg [8:0]  sdram_timer9;

reg [1:0]  sdram_step;

reg [31:0] readBuffer;

assign debug8[5] = read_sdram_req;
assign debug8[6] = read_sdram_req_buff;
assign debug8[7] = read_sdram_ack;

//sdram_rd_req sdram_rd_burst sdram_rw_addr
always@(posedge sdram_clk or negedge sys_rst_n) begin // sdram 主控
  if(!sys_rst_n) begin
    sdram_timer9 <= 0;
    sdram_timer0 <= 0;
    sdram_step <= 0;
    
    read_sdram_req_buff <= 0;
    write_single_sdram_req_buff <= 0;
    buffDMAread_req_buff <= 0;
    
    read_sdram_ack <= 0;
    write_single_sdram_ack <= 0;
    buffDMAread_ack <= 0;
    
    sdram_rd_req <= 0;
    sdram_wr_req <= 0;
    sdram_rd_burst <= 0;
    sdram_rw_addr <= 0;
  end else begin
    read_sdram_req_buff <= read_sdram_req;
    write_single_sdram_req_buff <= write_single_sdram_req;
    buffDMAread_req_buff <= buffDMAread_req;
    
    buffDMAreadA_wren<=0;
    buffDMAreadB_wren<=0;
    
    if (read_sdram_req_buff && !read_sdram_ack)begin
      if         (sdram_step == 0) begin
        sdram_step <= 1;
        sdram_rw_addr <= rdwr_sdram_addr;
        sdram_rd_burst <= 2;
        sdram_rd_req <= 1;
      end else if(sdram_step == 1) begin
        if(sdram_rd_ack)begin
          sdram_step <= 2;
          readBuffer[15: 0] <= sdram_dout;
        end
      end else if(sdram_step == 2) begin
        readBuffer[31:16] <= sdram_dout;
        sdram_rd_req <= 0;
        sdram_step <= 0;
        read_sdram_ack <= 1;
      end
    end
    if(!read_sdram_req_buff && read_sdram_ack)begin
      read_sdram_ack <= 0;
    end

    if (write_single_sdram_req && !write_single_sdram_ack)begin
      if         (sdram_step==0)begin
        sdram_step <= 1;
        sdram_rw_addr <= rdwr_sdram_addr;
        sdram_wr_burst <= 2;
        sdram_wr_req <= 1;
      end else if(sdram_step==1)begin
        if(sdram_wr_ack)begin
          sdram_step <= 2;
          sdram_din <= write_single_sdram_data[15:0];
          sdram_mask <= write_single_sdram_mask[1:0];
        end
      end else if(sdram_step==2)begin
        sdram_din <= write_single_sdram_data[31:16];
        sdram_mask <= write_single_sdram_mask[3:2];
        sdram_wr_req <= 0;
        sdram_step <= 0;
        write_single_sdram_ack <= 1;
      end
    end
    
    if(!write_single_sdram_req_buff && write_single_sdram_ack)begin
      write_single_sdram_ack <= 0;
    end
    
    if (buffDMAread_req_buff && !buffDMAread_ack)begin
      sdram_timer0 <= 1;
      if(sdram_timer0 == 0)begin
        sdram_rw_addr <= {buffDMAread_addr,8'b0}; //16+8
        sdram_rd_burst <= 256;
        sdram_timer9 <= 0;
        sdram_page_delay <= 0;
        sdram_rd_req <= 1;//只需要置高一个周期就可以了
      end else begin
        if(sdram_rd_ack || sdram_page_delay)begin
          sdram_timer9 <= sdram_timer9 + 1'b1;
          if(!sdram_page_delay)begin
            buffDMAread_wrdata    <= sdram_dout;
            buffDMAread_wraddress <= sdram_timer9[7:0];
            if(buffDMAread_A_B)begin
              buffDMAreadA_wren <= 1;
            end else begin
              buffDMAreadB_wren <= 1;
            end
          end
          if(sdram_timer9==255)begin 
            sdram_page_delay <= 1;
            sdram_rd_req <= 0;
          end else if(sdram_timer9==263)begin 
            sdram_timer0 <= 0;
            buffDMAread_ack <= 1;
          end
        end
      end
    end
    
    if(!buffDMAread_req_buff && buffDMAread_ack)begin
      buffDMAread_ack <= 0;
    end
  end
end

endmodule 