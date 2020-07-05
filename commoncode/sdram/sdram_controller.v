module sdram_controller(
    input         clk,              //SDRAM控制器时钟，100MHz
    input         rst_n,            //系统复位信号，低电平有效

    // FPGA与SDRAM硬件接口
    output            sdram_cke,        // SDRAM 时钟有效信号
    output            sdram_cs_n,       // SDRAM 片选信号
    output            sdram_ras_n,      // SDRAM 行地址选通脉冲
    output            sdram_cas_n,      // SDRAM 列地址选通脉冲
    output            sdram_we_n,       // SDRAM 写允许位
    output reg [ 1:0] sdram_ba,         // SDRAM L-Bank地址线
    output reg [12:0] sdram_addr,       // SDRAM 地址总线
    inout  [15:0]     sdram_data,        // SDRAM 数据总线
    output  [ 1:0]    sdram_dqm,
    
    //TODO 读写地址合并 burst 合并
    
    //SDRAM 控制器写端口  
    input  [23:0] sdram_rw_addr,    //SDRAM写操作的地址
    input         sdram_wr_req,     //写SDRAM请求信号
    output        sdram_wr_ack,     //写SDRAM响应信号
    input  [ 9:0] sdram_wr_burst,   //写sdram时数据突发长度
    input  [15:0] sdram_din,        //写入SDRAM的数据
    input  [ 1:0] sdram_mask,
    
    //SDRAM 控制器读端口  
    input         sdram_rd_req,     //读SDRAM请求信号
    output        sdram_rd_ack,     //读SDRAM响应信号
    input  [ 9:0] sdram_rd_burst,   //读sdram时数据突发长度
    output [15:0] sdram_dout,       //从SDRAM读出的数据
    
    input block_auto_refresh,
    
    output        sdram_init_done  //SDRAM 初始化完成标志

    );

// SDRAM 初始化过程各个状态
localparam I_NOP   = 5'd0; //等待上电200us稳定期结束
localparam I_PRE   = 5'd1; //预充电状态
localparam I_TRP   = 5'd2; //等待预充电完成       tRP
localparam I_AR    = 5'd3; //自动刷新            
localparam I_TRF   = 5'd4; //等待自动刷新结束    tRC
localparam I_MRS   = 5'd5; //模式寄存器设置
localparam I_TRSC  = 5'd6; //等待模式寄存器设置完成 tRSC
localparam I_DONE  = 5'd7; //初始化完成

// SDRAM 工作过程各个状态
localparam W_IDLE    = 4'd0; //空闲
localparam W_ACTIVE  = 4'd1; //行有效
localparam W_TRCD    = 4'd2; //行有效等待
localparam W_READ    = 4'd3; //读操作
localparam W_CL      = 4'd4; //潜伏期
localparam W_RD      = 4'd5; //读数据
localparam W_WRITE   = 4'd6; //写操作
localparam W_WD      = 4'd7; //写数据
localparam W_TWR     = 4'd8; //写回
localparam W_PRE     = 4'd9; //预充电
localparam W_TRP     = 4'd10;//预充电等待
localparam W_AR      = 4'd11;//自动刷新
localparam W_TRFC    = 4'd12;//自动刷新等待


localparam TRP_CLK  = 10'd4; //预充电有效周期
localparam TRC_CLK  = 10'd6; //自动刷新周期
localparam TRSC_CLK = 10'd6; //模式寄存器设置时钟周期
localparam TRCD_CLK = 10'd2; //行选通周期
localparam TCL_CLK  = 10'd3; //列潜伏期
localparam TWR_CLK  = 10'd2; //写入校正//写回周期结束

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SDRAM 状态控制模块

wire       sdram_ref_ack;		        //SDRAM自动刷新请求应答信号


//SDRAM初始化完成标志 
assign sdram_init_done = (init_state == I_DONE);

//SDRAM 自动刷新应答信号
assign sdram_ref_ack = (work_state == W_AR);

//写SDRAM响应信号
assign sdram_wr_ack = 
  ((work_state == W_TRCD) & (sdram_rd_wr==WRITE)) | //W_TRCD 行有效等待 && write 
  (work_state == W_WRITE) |
  ((work_state == W_WD) & (cnt_clk < sdram_wr_burst - 1));

//读SDRAM响应信号
assign sdram_rd_ack = (work_state == W_RD) & (cnt_clk >= 10'd1) & (cnt_clk < sdram_rd_burst + 2'd1);


wire done_200us; //上电后200us输入稳定期结束标志位
//SDRAM上电后200us稳定期结束后,将标志信号拉高
assign done_200us = (cnt_200us == 15'd20000);

reg [14:0] cnt_200us;//SDRAM 上电稳定期200us计数器
//上电后计时200us,等待SDRAM状态稳定
always @ (posedge clk or negedge rst_n) begin
  if(!rst_n) begin
    cnt_200us <= 15'd0;
  end else begin
    if(cnt_200us < 15'd20000) begin
      cnt_200us <= cnt_200us + 1'b1;
    end else begin
      cnt_200us <= cnt_200us;
    end
  end
end

//替换成10位 9:0
reg [10:0] cnt_refresh;//刷新计数寄存器
//刷新计数器循环计数7812ns (60ms内完成全部8192行刷新操作)
always @ (posedge clk or negedge rst_n) begin
  if(!rst_n) begin
    cnt_refresh <= 11'd0;
  end else begin
    if(cnt_refresh == 11'd780) begin // 64ms/8192 =7812ns
      cnt_refresh <= 11'd0;
    end else begin
      cnt_refresh <= cnt_refresh + 1'b1;
    end
  end
end


reg sdram_ref_req; //SDRAM 自动刷新请求信号
//SDRAM 刷新请求
always @ (posedge clk or negedge rst_n)begin
  if(!rst_n) begin
    sdram_ref_req <= 1'b0;
  end else begin
    if(cnt_refresh == 11'd780 && !block_auto_refresh) begin
      sdram_ref_req <= 1'b1;//刷新计数器计时达7812ns时产生刷新请求
    end else if(sdram_ref_ack) begin
      sdram_ref_req <= 1'b0;//收到刷新请求响应信号后取消刷新请求 
    end
  end
end

//    output reg [9:0] cnt_clk,	        //时钟计数器
reg [9:0] cnt_clk;                 // 延时计数器
//延时计数器对时钟计数
always @ (posedge clk or negedge rst_n) begin
  if(!rst_n) begin
    cnt_clk <= 10'd0;
  end else begin
    if(!cnt_rst_n) begin                //在cnt_rst_n为低电平时延时计数器清零
      cnt_clk <= 10'd0;
    end else begin
      cnt_clk <= cnt_clk + 1'b1;
    end
  end
end


reg cnt_rst_n; //延时计数器复位信号，低有效
//计数器控制逻辑
always @ (*) begin
  case (init_state)
    I_NOP: //延时计数器清零(cnt_rst_n低电平复位)
      cnt_rst_n <= 1'b0;
    I_PRE: //预充电：延时计数器启动(cnt_rst_n高电平启动)
      cnt_rst_n <= 1'b1; //等待预充电延时计数结束后，清零计数器
    I_TRP://自动刷新：延时计数器启动
      cnt_rst_n <= (cnt_clk == TRP_CLK) ? 1'b0 : 1'b1;
    I_AR://等待自动刷新延时计数结束后，清零计数器
      cnt_rst_n <= 1'b1;
    I_TRF:
      cnt_rst_n <= (cnt_clk == TRC_CLK) ? 1'b0 : 1'b1;
    I_MRS: //模式寄存器设置：延时计数器启动
      cnt_rst_n <= 1'b1;//等待模式寄存器设置延时计数结束后，清零计数器
    I_TRSC: 
      cnt_rst_n <= (cnt_clk == TRSC_CLK) ? 1'b0:1'b1;
    I_DONE: //初始化完成后,判断工作状态
      begin
        case (work_state)
          W_IDLE://行有效：延时计数器启动
            cnt_rst_n <= 1'b0;
          W_ACTIVE: //行有效延时计数结束后，清零计数器
            cnt_rst_n <= 1'b1;
          W_TRCD: //潜伏期延时计数结束后，清零计数器
            cnt_rst_n <= (cnt_clk == TRCD_CLK-1)   ? 1'b0 : 1'b1;
          W_CL: //读数据延时计数结束后，清零计数器
            cnt_rst_n <= (cnt_clk == TCL_CLK-1)    ? 1'b0 : 1'b1;
          W_RD: //写数据延时计数结束后，清零计数器
            cnt_rst_n <= (cnt_clk == sdram_rd_burst+2)  ? 1'b0 : 1'b1;
          W_WD: //写回周期延时计数结束后，清零计数器
            cnt_rst_n <= (cnt_clk == sdram_wr_burst-1) ? 1'b0 : 1'b1;
          W_TWR: //预充电等待延时计数结束后，清零计数器
            cnt_rst_n <= (cnt_clk == TWR_CLK )    ? 1'b0 : 1'b1;
          W_TRP: //自动刷新等待延时计数结束后，清零计数器
            cnt_rst_n <= (cnt_clk == TRP_CLK) ? 1'b0 : 1'b1;
          W_TRFC:
            cnt_rst_n <= (cnt_clk == TRC_CLK)   ? 1'b0 : 1'b1;
          default:
            cnt_rst_n <= 1'b0;
        endcase
      end
    default: 
      cnt_rst_n <= 1'b0;
	endcase
end


reg [ 3:0] init_ar_cnt;                 //初始化过程自动刷新计数器
//初始化过程中对自动刷新操作计数
always @ (posedge clk or negedge rst_n) begin
  if(!rst_n) begin
    init_ar_cnt <= 4'd0;
  end else begin
    if(init_state == I_NOP) begin
      init_ar_cnt <= 4'd0;
    end else if(init_state == I_AR) begin
      init_ar_cnt <= init_ar_cnt + 1'b1;
    end else begin
      init_ar_cnt <= init_ar_cnt;
    end
  end
end

//    output reg [4:0] init_state,	    //SDRAM初始化状态
reg [4:0] init_state;              // SDRAM初始化状态
//SDRAM的初始化状态机
always @ (posedge clk or negedge rst_n) begin
  if(!rst_n) begin
    init_state <= I_NOP;
  end else begin
    case (init_state)
      I_NOP: //上电复位后200us结束则进入下一状态
        init_state <= done_200us  ? I_PRE : I_NOP;
      I_PRE: //预充电状态
        init_state <= I_TRP;
      I_TRP: //预充电等待，TRP_CLK个时钟周期
        init_state <= (cnt_clk == TRP_CLK)  ? I_AR  : I_TRP;
      I_AR : //自动刷新
        init_state <= I_TRF;
      I_TRF: //等待自动刷新结束,TRC_CLK个时钟周期
        init_state <= (cnt_clk == TRC_CLK) ? ((init_ar_cnt == 4'd8) ? I_MRS : I_AR) : I_TRF;//连续8次自动刷新操作
      I_MRS: //模式寄存器设置
        init_state <= I_TRSC;
      I_TRSC: //等待模式寄存器设置完成，TRSC_CLK个时钟周期
        init_state <= (cnt_clk == TRSC_CLK) ? I_DONE : I_TRSC;
      I_DONE: //SDRAM的初始化设置完成标志
        init_state <= I_DONE;
      default: 
        init_state <= I_NOP;
    endcase
  end
end

reg [3:0] work_state;              // SDRAM工作状态
reg       sdram_rd_wr;             // SDRAM读/写控制信号,低电平为写，高电平为读
localparam READ   = 1'd1;//高电平为读
localparam WRITE  = 1'd0;//低电平为写

//work_state (sdram_wr_req)
//W_IDLE -> W_ACTIVE 
//sdram_rd_wr <= WRITE
//
//3+n+7+(9自动刷新？)
//work_state
//W_ACTIVE 1 输出行有效指令
//W_TRCD   1 TRCD_CLK(2)-1 等待
//W_WRITE  1 输出写操作指令
//W_WD	 n 写数据
//W_TWR    2 TWR_CLK(2) 写回周期
//W_PRE    1 输出预充电指令
//W_TRP    4 TRP_CLK(4) 预充电等待
//W_IDLE 


//SDRAM的工作状态机,工作包括读、写以及自动刷新操作
always @ (posedge clk or negedge rst_n) begin
  if(!rst_n)begin
    work_state <= W_IDLE;          //空闲状态
  end else begin
    case(work_state)
      W_IDLE: //定时自动刷新请求，跳转到自动刷新状态
        if         (sdram_ref_req && sdram_init_done) begin
          work_state <= W_AR;
          sdram_rd_wr <= READ;
        end else if(sdram_wr_req && sdram_init_done) begin
          work_state <= W_ACTIVE;
          sdram_rd_wr <= WRITE;
        end else if(sdram_rd_req && sdram_init_done) begin
          work_state <= W_ACTIVE;
          sdram_rd_wr <= READ;
        end else begin 
          work_state <= W_IDLE;
          sdram_rd_wr <= READ;
        end
      W_ACTIVE: //行有效，跳转到行有效等待状态
        work_state <= W_TRCD;
      W_TRCD: 
        if(cnt_clk == TRCD_CLK-1) begin//行有效等待结束，判断当前是读还是写
          if(sdram_rd_wr == READ) begin
            work_state <= W_READ;//读：进入读操作状态
          end else begin          
            work_state <= W_WRITE;//写：进入写操作状态
          end
        end else begin
          work_state <= W_TRCD;
        end
      W_READ: //读操作，跳转到潜伏期
        work_state <= W_CL;
      W_CL: //潜伏期：等待潜伏期结束，跳转到读数据状态
        work_state <= (cnt_clk == TCL_CLK-1) ? W_RD:W_CL;
      W_RD: //读数据：等待读数据结束，跳转到预充电状态
        work_state <= (cnt_clk == sdram_rd_burst+2) ? W_PRE:W_RD;
      W_WRITE: //写操作：跳转到写数据状态
        work_state <= W_WD;
      W_WD: //写数据：等待写数据结束，跳转到写回周期状态
        work_state <= (cnt_clk == sdram_wr_burst-1) ? W_TWR:W_WD;
      W_TWR: //写回周期：写回周期结束，跳转到预充电状态
        work_state <= (cnt_clk == TWR_CLK ) ? W_PRE:W_TWR;
      W_PRE: //预充电：跳转到预充电等待状态
        work_state <= W_TRP;
      W_TRP: //预充电等待：预充电等待结束，进入空闲状态
        work_state <= (cnt_clk == TRP_CLK) ? W_IDLE:W_TRP;
      W_AR: //自动刷新操作，跳转到自动刷新等待
        work_state <= W_TRFC;
      W_TRFC: //自动刷新等待：自动刷新等待结束，进入空闲状态
        work_state <= (cnt_clk == TRC_CLK) ? W_IDLE:W_TRFC;
      default:
        work_state <= W_IDLE;
		endcase
  end
end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SDRAM 状态控制模块 end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SDRAM 命令控制模块

//reg define
reg  [ 4:0] sdram_cmd_r;                //SDRAM操作指令

//SDRAM控制信号命令
localparam CMD_INIT    = 5'b01111; // INITIATE
localparam CMD_A_REF   = 5'b10001; // AOTO REFRESH
localparam CMD_ACTIVE  = 5'b10011; // ACTIVE COMMAND
localparam CMD_READ    = 5'b10101; // READ_COMMAND
localparam CMD_NOP     = 5'b10111; // NOP COMMAND
localparam CMD_LMR     = 5'b10000; // LOAD MODE REGISTER
localparam CMD_PRGE    = 5'b10010; // PRECHARGE
localparam CMD_WRITE   = 5'b10100; // WRITE_COMMAND
localparam CMD_B_STOP  = 5'b10110; // BURST STOP

//SDRAM 控制信号线赋值
assign {sdram_cke,sdram_cs_n,sdram_ras_n,sdram_cas_n,sdram_we_n} = sdram_cmd_r;

//SDRAM 读/写地址总线控制
wire [23:0] sys_addr;                   //SDRAM读写地址 
assign sys_addr = sdram_rw_addr;

//SDRAM 操作指令控制
always @ (posedge clk or negedge rst_n) begin
    if(!rst_n) begin
            sdram_cmd_r <= CMD_INIT;
            sdram_ba    <= 2'b11;
            sdram_addr  <= 13'h1fff;
    end
    else
        case(init_state)
                                        //初始化过程中,以下状态不执行任何指令
            I_NOP,I_TRP,I_TRF,I_TRSC: begin
                    sdram_cmd_r <= CMD_NOP;
                    sdram_ba    <= 2'b11;
                    sdram_addr  <= 13'h1fff;    
                end
            I_PRE: begin               //预充电指令
                    sdram_cmd_r <= CMD_PRGE;
                    sdram_ba    <= 2'b11;
                    sdram_addr  <= 13'h1fff;
                end 
            I_AR: begin
                                        //自动刷新指令
                    sdram_cmd_r <= CMD_A_REF;
                    sdram_ba    <= 2'b11;
                    sdram_addr  <= 13'h1fff;
                end                 
            I_MRS: begin               //模式寄存器设置指令
                    sdram_cmd_r <= CMD_LMR;
                    sdram_ba    <= 2'b00;
                    sdram_addr  <= {    //利用地址线设置模式寄存器,可根据实际需要进行修改
                        3'b000,         //预留
                        1'b0,           //读写方式 A9=0，突发读&突发写
                        2'b00,          //默认，{A8,A7}=00
                        3'b011,         //CAS潜伏期设置，这里设置为3，{A6,A5,A4}=011
                        1'b0,           //突发传输方式，这里设置为顺序，A3=0
                        3'b111          //突发长度，     页突发3'b111，{A2,A1,A0}=011    ***********************************************************************************************
                    };
                end 
            I_DONE:                    //SDRAM初始化完成
                    case(work_state)    //以下工作状态不执行任何指令
                        W_IDLE,W_TRCD,W_CL,W_TWR,W_TRP,W_TRFC: begin
                                sdram_cmd_r <= CMD_NOP;
                                sdram_ba    <= 2'b11;
                                sdram_addr  <= 13'h1fff;
                            end
                        W_ACTIVE: begin//行有效指令
                                sdram_cmd_r <= CMD_ACTIVE;
                                sdram_ba    <= sys_addr[23:22];
                                sdram_addr  <= sys_addr[21:9];
                            end
                        W_READ: begin  //读操作指令
                                sdram_cmd_r <= CMD_READ;
                                sdram_ba    <= sys_addr[23:22];
                                sdram_addr  <= {4'b0000,sys_addr[8:0]};//sdram_addr  <= 13'b0100 0000 0000; 高电平 允许自动预充电
                            end
                        W_RD: begin    //突发传输终止指令
                                if(cnt_clk == sdram_rd_burst-4) 
                                    sdram_cmd_r <= CMD_B_STOP;
                                else begin
                                    sdram_cmd_r <= CMD_NOP;
                                    sdram_ba    <= 2'b11;
                                    sdram_addr  <= 13'h1fff;
                                end
                            end                             
                        W_WRITE: begin //写操作指令
                                sdram_cmd_r <= CMD_WRITE;
                                sdram_ba    <= sys_addr[23:22];
                                sdram_addr  <= {4'b0000,sys_addr[8:0]};//sdram_addr  <= 13'h0400; 高电平 允许自动预充电
                            end     
                        W_WD: begin    //突发传输终止指令
                                if(cnt_clk == sdram_wr_burst-1) 
                                    sdram_cmd_r <= CMD_B_STOP;
                                else begin
                                    sdram_cmd_r <= CMD_NOP;
                                    sdram_ba    <= 2'b11;
                                    sdram_addr  <= 13'h1fff;
                                end
                            end
                        W_PRE:begin    //预充电指令
                                sdram_cmd_r <= CMD_PRGE;
                                sdram_ba    <= sys_addr[23:22];
                                sdram_addr  <= 13'h0000;//sdram_addr  <= 13'h0400; 高电平 允许自动预充电
                            end
                        W_AR: begin    //自动刷新指令
                                sdram_cmd_r <= CMD_A_REF;
                                sdram_ba    <= 2'b11;
                                sdram_addr  <= 13'h1fff;
                            end
                        default: begin
                                sdram_cmd_r <= CMD_NOP;
                                sdram_ba    <= 2'b11;
                                sdram_addr  <= 13'h1fff;
                            end
                    endcase
            default: begin
                    sdram_cmd_r <= CMD_NOP;
                    sdram_ba    <= 2'b11;
                    sdram_addr  <= 13'h1fff;
                end
        endcase
end


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SDRAM 命令控制模块 end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SDRAM 数据读写模块

//reg define
reg        sdram_out_en;                //SDRAM数据总线输出使能
reg [15:0] sdram_din_r;                 //寄存写入SDRAM中的数据
reg  [1:0] sdram_mask_r;
reg [15:0] sdram_dout_r;                //寄存从SDRAM中读取的数据


//SDRAM 双向数据线作为输入时保持高阻态
assign sdram_data = sdram_out_en ? sdram_din_r : 16'hzzzz;
assign sdram_dqm = sdram_out_en ? sdram_mask_r : 2'b00;

//输出SDRAM中读取的数据
assign sdram_dout = sdram_dout_r;

//SDRAM 数据总线输出使能
always @ (posedge clk or negedge rst_n) begin
  if(!rst_n) begin
    sdram_out_en <= 1'b0;
  end else if((work_state == W_WRITE) | (work_state == W_WD)) 
    sdram_out_en <= 1'b1; //向SDRAM中写数据时,输出使能拉高
  else 
    sdram_out_en <= 1'b0;
end

//将待写入数据送到SDRAM数据总线上
always @ (posedge clk or negedge rst_n) begin
  if(!rst_n) begin
    sdram_din_r <= 16'd0;
    sdram_mask_r <= 0;
  end else if((work_state == W_WRITE) | (work_state == W_WD))begin
    sdram_din_r <= sdram_din;   //寄存写入SDRAM中的数据
    sdram_mask_r <= sdram_mask;
  end
end

//读数据时,寄存SDRAM数据线上的数据
always @ (posedge clk or negedge rst_n) begin
  if(!rst_n) 
    sdram_dout_r <= 16'd0;
  else if(work_state == W_RD) 
    sdram_dout_r <= sdram_data;     //寄存从SDRAM中读取的数据
end
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SDRAM 数据读写模块

endmodule 