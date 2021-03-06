
module vga_driver8m(
    input           sys_clk,
    input           sys_rst_n,    //复位信号

    //00 640*480 txt
    //01 640*480 img
    //1x 1024*768 img
    input [1:0] vga_mode,
    output reg blanking,
    input blockvga,
    
    output reg       read_line_req ,
    output reg       read_line_A_B ,
    output    [15:0] read_line_addr,//4kline total
    input     [15:0] read_line_base_addr,//start line

    input [15:0] read_pixelA_data,
    input [15:0] read_pixelB_data,
    output [9:0] read_pixel_addr,
    output       read_pixel_clk,

    //VGA接口
    output     reg     vga_hs,       //行同步信号
    output     reg     vga_vs,       //场同步信号
    output  [15:0]  vga_rgb      //红绿蓝三原色输出
    
  );

`include "config.v"

//wire define
wire vga_clk_25M;
wire vga_clk_65M;
wire         locked_w;              //PLL输出稳定信号
wire         rst_n_w;               //内部复位信号

//*****************************************************
//**                    main code
//***************************************************** 
//待PLL输出稳定之后，停止复位
assign rst_n_w = sys_rst_n && locked_w;

`ifdef IS_ALTERA
vga_pll  u_vga_pll(                  //时钟分频模块
  .inclk0         (sys_clk),    
  .areset         (~sys_rst_n),
    
  .c0             (vga_clk_25M), //VGA时钟 25M
  .c1             (vga_clk_65M), //65M 102*768
  .locked         (locked_w)
  ); 
`endif

//parameter define  
parameter  H25_SYNC   =  11'd96;    //行同步
parameter  H25_BACK   =  11'd48;    //行显示后沿
parameter  H25_DISP   =  11'd640;   //行有效数据
parameter  H25_TOTAL  =  11'd800;   //行扫描周期

parameter  V25_SYNC   =  11'd2;     //场同步
parameter  V25_BACK   =  11'd33;    //场显示后沿
parameter  V25_DISP   =  11'd480;   //场有效数据
parameter  V25_TOTAL  =  11'd525;   //场扫描周期


//1024*768 60FPS_65MHz
parameter  H65_SYNC   =  11'd136;   //行同步     
parameter  H65_BACK   =  11'd160;   //行显示后沿
parameter  H65_DISP   =  11'd1024;  //行有效数据
parameter  H65_TOTAL  =  11'd1344;  //行扫描周期  注意位宽长度,需要11位的位宽

parameter  V65_SYNC   =  11'd6;     //场同步
parameter  V65_BACK   =  11'd29;    //场显示后沿
parameter  V65_DISP   =  11'd768;   //场有效数据
parameter  V65_TOTAL  =  11'd806;   //场扫描周期


//wire define

//使能RGB565数据输出
wire vga_en  = h_active && v_active;
                 
//RGB565数据输出
assign vga_rgb = vga_en ?  pixel_data: 16'd0;//16'hffff  pixel_data

wire vga_clk;
assign vga_clk = vga_mode[1] ? vga_clk_65M : vga_clk_25M;

assign read_pixel_clk = vga_clk;

reg h_active;
reg v_active;
reg v_active_ram;

//reg define
reg  [10:0] cnt_h;
reg  [10:0] cnt_v;

reg [10:0]h_total;
reg [10:0]v_total;
reg [10:0]h_sync;
reg [10:0]v_sync;
reg [10:0]h_start;
reg [10:0]v_start;
reg [10:0]h_end;
reg [10:0]v_end;

reg [15:0] curr_read_line_base_addr;
assign read_line_addr = curr_read_line_base_addr + cnt_v - v_start + 1'b1;
wire [15:0]  pixel_data = blockvga ? 16'b0 : (read_line_addr[0]?read_pixelB_data:read_pixelA_data);

wire [10:0]temp_read_pixel_addr = (cnt_h-h_start);
assign read_pixel_addr = temp_read_pixel_addr[9:0];

//字符模式：
//80*30
//128*32:4096byte //BUFFA BUFFB
//8*16 char table 
//16byte per char
//128char = 2048byte
//提前2周期取字符，提前1周期取像素，8周期显示像素


//行计数器对像素时钟计数
always @(posedge vga_clk or negedge rst_n_w) begin
    if (!rst_n_w)begin
      cnt_h <= 0;
      cnt_v <= 0;
      vga_hs <= 0;
      vga_vs <= 0;
      h_active <= 0;
      v_active <= 0;
      h_total <= H25_TOTAL;
      v_total <= V25_TOTAL;
      h_sync <= H25_SYNC;
      v_sync <= V25_SYNC;
      h_start <= H25_SYNC + H25_BACK;
      v_start <= V25_SYNC + V25_BACK;
      h_end <= H25_SYNC + H25_BACK + H25_DISP;
      v_end <= V25_SYNC + V25_BACK + V25_DISP;
      blanking <= 0;
      curr_read_line_base_addr <= 0;
    end else begin
      if(vga_mode[1])begin
        h_total <= H65_TOTAL;
        v_total <= V65_TOTAL;
        h_sync <= H65_SYNC;
        v_sync <= V65_SYNC;
        h_start <= H65_SYNC + H65_BACK;
        v_start <= V65_SYNC + V65_BACK;
        h_end <= H65_SYNC + H65_BACK + H65_DISP;
        v_end <= V65_SYNC + V65_BACK + V65_DISP;
      end else begin
        h_total <= H25_TOTAL;
        v_total <= V25_TOTAL;
        h_sync <= H25_SYNC;
        v_sync <= V25_SYNC;
        h_start <= H25_SYNC + H25_BACK;
        v_start <= V25_SYNC + V25_BACK;
        h_end <= H25_SYNC + H25_BACK + H25_DISP;
        v_end <= V25_SYNC + V25_BACK + V25_DISP;
      end
    
      cnt_h <= cnt_h + 1'b1;
      if(cnt_h == h_total)begin
        cnt_h <= 10'd0;
        vga_hs <= 1;
        cnt_v <= cnt_v + 1'b1;
        if(cnt_v == v_total)begin
          cnt_v <= 10'd0;
          vga_vs <= 1;
        end
      end
    
      if(cnt_h == h_sync)begin
        vga_hs <= 0;
      end
      if(cnt_v == v_sync)begin
        vga_vs <= 0;
      end
      
        
      if(cnt_h == h_start)begin
        h_active <= 1;
        if(v_active_ram)begin
          read_line_A_B<=read_line_addr[0];
        end
        if(!blockvga)begin
          read_line_req<=1;
        end

      end
      if(cnt_h == h_end)begin
        h_active <= 0;
        read_line_req <= 0;
      end
      
      if(cnt_v == v_start)begin
        v_active <= 1;
      end

      if(cnt_v == v_end)begin
        v_active <= 0;
      end
      
      if(cnt_v == (v_start-1))begin
        v_active_ram <= 1;
      end

      if(cnt_v == (v_end-1))begin
        v_active_ram <= 0;
        curr_read_line_base_addr <= read_line_base_addr;
      end
      
      if(cnt_v == (v_start-11'b11))begin
        blanking <= 0;
      end
      
      if(cnt_v == (v_end+1'b1))begin
        blanking <= 1;
      end

    end
end

endmodule 