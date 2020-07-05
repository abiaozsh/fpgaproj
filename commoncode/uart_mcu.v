module uart_mcu(
    input  sys_clk  ,
    input  sys_rst_n,
    
    input  uart_rxd,
    output uart_txd,
    
    output busy,
    //output [7:0] debug_port0,
    //output [7:0] debug_port1,
    //output [7:0] debug_port2,
    //output debug_pin0,
    //output debug_pin1,
    //output debug_pin2,
    //output debug_pin3,
    //output debug_pin6,
    //output debug_pin7,

    /*
  .sdram_clk_out     (sdram_clk_out),
  .sdram_cke			(sdram_cke),		//SDRAM 时钟有效
  .sdram_cs_n			(sdram_cs_n),		//SDRAM 片选
  .sdram_ras_n		(sdram_ras_n),		//SDRAM 行有效	
  .sdram_cas_n		(sdram_cas_n),		//SDRAM 列有效
  .sdram_we_n			(sdram_we_n),		//SDRAM 写有效
  .sdram_ba			  (sdram_ba),			//SDRAM Bank地址
  .sdram_addr			(sdram_addr),		//SDRAM 行/列地址
  .sdram_data			(sdram_data),		//SDRAM 数据	
  .sdram_dqm		(sdram_dqm),
  .sdram_prob_refresh (sdram_prob_refresh),
*/
    
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
    output        sdram_prob_refresh,
    
    
		output reg out_clk,
		output reg out_rst,

		input [7:0] in_pin0,
		input [7:0] in_pin1,
		input [7:0] in_pin2,
		input [7:0] in_pin3,
		input [7:0] in_pin4,
		input [7:0] in_pin5,
		input [7:0] in_pin6,
		input [7:0] in_pin7,

		output reg [7:0] out_pin0,
		output reg [7:0] out_pin1,
		output reg [7:0] out_pin2,
		output reg [7:0] out_pin3,
		output reg [7:0] out_pin4,
		output reg [7:0] out_pin5,
		output reg [7:0] out_pin6,
		output reg [7:0] out_pin7

		);

assign busy = command != 0 && command_done==0;
    
    
wire uart_rec;
wire [7:0] uart_data_r;
reg uart_send;
reg [7:0] uart_data_w;

reg [7:0] uw_reg0;
reg [7:0] uw_reg1;
reg [7:0] uw_reg2;
reg [7:0] uw_reg3;
reg [7:0] uw_reg4;
reg [7:0] uw_reg5;
reg [7:0] uw_reg6;
reg [7:0] uw_reg7;

reg [7:0] ur_reg0;
reg [7:0] ur_reg1;
reg [7:0] ur_reg2;
reg [7:0] ur_reg3;
reg [7:0] ur_reg4;
reg [7:0] ur_reg5;
reg [7:0] ur_reg6;
reg [7:0] ur_reg7;

reg uart_rec_last;
wire uart_rec_rise;
assign uart_rec_rise = uart_rec && !uart_rec_last;
always @(posedge sys_clk or negedge sys_rst_n) begin
  if (!sys_rst_n) begin
		uart_rec_last<=0;
  end else begin
		uart_rec_last <= uart_rec;
	end
end
///////////
uart_hs ins_uart_hs(
    .sys_clk        (sys_clk), 
    .sys_rst_n      (sys_rst_n),
    .uart_rxd       (uart_rxd),
    .uart_txd       (uart_txd),

    .uart_rec       (uart_rec),
    .uart_data_out  (uart_data_r),
    .uart_send      (uart_send),
    .uart_data_in   (uart_data_w)
  );

reg [7:0] command;
reg [7:0] data;
reg [15:0] data_index;
reg data_arrive;
always @(posedge sys_clk or negedge sys_rst_n) begin
  if (!sys_rst_n) begin
    command <= 0;
    data <= 0;
    data_index<=0;
    data_arrive<=0;
  end else begin
    data_arrive <= 0;
    if (uart_rec_rise) begin //串口数据到达
      if(command==8'b0) begin
        command <= uart_data_r;
        data_index <= 16'hFFFF;
      end else begin
        data_arrive <= 1;
        data_index <= data_index + 1'b1;
        data <= uart_data_r;
      end
    end else begin
      if(command_done)begin
        command <= 0;
      end
    end
  end
end

reg command_done;
reg [10:0] timer;
reg [7:0] reg_temp;
reg [9:0] timer2;
reg [23:0] read_address;
reg hibit;

reg [15:0] data_temp;
reg [24:0] count_temp;

reg [7:0] sum;

always @(posedge sys_clk or negedge sys_rst_n) begin
  if (!sys_rst_n) begin
    out_pin0<=0;
    out_pin1<=0;
    out_pin2<=0;
    out_pin3<=0;
    out_pin4<=0;
    out_pin5<=0;
    out_pin6<=0;
    out_pin7<=0;
    
    uw_reg0<=0;
    uw_reg1<=0;
    uw_reg2<=0;
    uw_reg3<=0;
    uw_reg4<=0;
    uw_reg5<=0;
    uw_reg6<=0;
    uw_reg7<=0;
    
    uart_send<=0;
    uart_data_w<=0;
    out_clk<=0;
    out_rst<=0;
    command_done <= 0;
    timer<=0;
    timer2<=0;
    
    sdram_c_address <= 0;
    sdram_c_data_in <= 0;
    sdram_c_read_req <= 0;
    sdram_c_write_req <= 0;
    sdram_c_write_en <= 0;
    sdram_c_write_latch_address <=0;
    
    sum <= 0;
    
		data_temp<=0;
    count_temp<=0;
    
		//debug_pin7<=0;
		//debug_pin6<=0;
//
		//debug_pin3<=0;
		//debug_pin2<=0;
		//debug_pin1<=0;
    //debug_pin0<=0;
    //
    //debug_port0<=0;
    //debug_port1<=0;
    //debug_port2<=0;
    
  end else begin
    //debug_port0 <= data_index[7:0];
    //debug_port1 <= data_index[15:8];
    //debug_port2 <= command;
    //debug_pin0<=sdram_c_read_req;
    //debug_pin1<=sdram_c_read_ack;
    //debug_pin2<=sdram_c_write_req;
    //debug_pin3<=sdram_c_write_ack;
    //
    //debug_pin6<=sdram_c_write_en;
    //debug_pin7<=sdram_c_write_latch_address;
    
    if(command_done)begin
      uart_send<=0;
      if          (command == 8'h00) begin 
        command_done<=0;
      end
    end else begin//command_done==0
      if          (command == 8'h00) begin
      end else if (command == 8'h10) begin out_clk<=1; command_done<=1;
      end else if (command == 8'h11) begin out_clk<=0; command_done<=1;
      end else if (command == 8'h12) begin out_rst<=1; command_done<=1;
      end else if (command == 8'h13) begin out_rst<=0; command_done<=1;

      end else if (command == 8'h20) begin uart_send<=1; uart_data_w<=in_pin0; command_done<=1;
      end else if (command == 8'h21) begin uart_send<=1; uart_data_w<=in_pin1; command_done<=1;
      end else if (command == 8'h22) begin uart_send<=1; uart_data_w<=in_pin2; command_done<=1;
      end else if (command == 8'h23) begin uart_send<=1; uart_data_w<=in_pin3; command_done<=1;
      end else if (command == 8'h24) begin uart_send<=1; uart_data_w<=in_pin4; command_done<=1;
      end else if (command == 8'h25) begin uart_send<=1; uart_data_w<=in_pin5; command_done<=1;
      end else if (command == 8'h26) begin uart_send<=1; uart_data_w<=in_pin6; command_done<=1;
      end else if (command == 8'h27) begin uart_send<=1; uart_data_w<=in_pin7; command_done<=1;

      end else if (command == 8'h30) begin if(data_arrive)begin out_pin0<=data; command_done<=1;end
      end else if (command == 8'h31) begin if(data_arrive)begin out_pin1<=data; command_done<=1;end
      end else if (command == 8'h32) begin if(data_arrive)begin out_pin2<=data; command_done<=1;end
      end else if (command == 8'h33) begin if(data_arrive)begin out_pin3<=data; command_done<=1;end
      end else if (command == 8'h34) begin if(data_arrive)begin out_pin4<=data; command_done<=1;end
      end else if (command == 8'h35) begin if(data_arrive)begin out_pin5<=data; command_done<=1;end
      end else if (command == 8'h36) begin if(data_arrive)begin out_pin6<=data; command_done<=1;end
      end else if (command == 8'h37) begin if(data_arrive)begin out_pin7<=data; command_done<=1;end

      end else if (command == 8'h40) begin if(data_arrive)begin uw_reg0<=data; command_done<=1;end
      end else if (command == 8'h41) begin if(data_arrive)begin uw_reg1<=data; command_done<=1;end
      end else if (command == 8'h42) begin if(data_arrive)begin uw_reg2<=data; command_done<=1;end
      end else if (command == 8'h43) begin if(data_arrive)begin uw_reg3<=data; command_done<=1;end
      end else if (command == 8'h44) begin if(data_arrive)begin uw_reg4<=data; command_done<=1;end
      end else if (command == 8'h45) begin if(data_arrive)begin uw_reg5<=data; command_done<=1;end
      end else if (command == 8'h46) begin if(data_arrive)begin uw_reg6<=data; command_done<=1;end
      end else if (command == 8'h47) begin if(data_arrive)begin uw_reg7<=data; command_done<=1;end

      end else if (command == 8'h50) begin uart_send<=1; uart_data_w<=ur_reg0; command_done<=1;
      end else if (command == 8'h51) begin uart_send<=1; uart_data_w<=ur_reg1; command_done<=1;
      end else if (command == 8'h52) begin uart_send<=1; uart_data_w<=ur_reg2; command_done<=1;
      end else if (command == 8'h53) begin uart_send<=1; uart_data_w<=ur_reg3; command_done<=1;
      end else if (command == 8'h54) begin uart_send<=1; uart_data_w<=ur_reg4; command_done<=1;
      end else if (command == 8'h55) begin uart_send<=1; uart_data_w<=ur_reg5; command_done<=1;
      end else if (command == 8'h56) begin uart_send<=1; uart_data_w<=ur_reg6; command_done<=1;
      end else if (command == 8'h57) begin uart_send<=1; uart_data_w<=ur_reg7; command_done<=1;

      end else if (command == 8'hA0) begin//sdram write
        timer<=timer+1'b1;
        if(timer==0)begin
          sdram_c_address <= {uw_reg4,uw_reg3,uw_reg2};
          sdram_c_data_in <= {uw_reg1,uw_reg0};
          sdram_c_write_req<=1;
        end else begin
          if(sdram_c_write_ack)begin
            ur_reg7<=timer[7:0];
            timer<=0;
            sdram_c_write_req<=0;
            command_done<=1;
          end
        end

      end else if (command == 8'hA1) begin//sdram read
        timer<=timer+1'b1;
        if(timer==0)begin
          sdram_c_address <= {uw_reg4,uw_reg3,uw_reg2};
          sdram_c_read_req<=1;
        end else begin
          if(sdram_c_read_ack)begin
            ur_reg7 <= timer[7:0];
            timer <= 0;
            ur_reg0 <= sdram_c_data_out[7:0];
            ur_reg1 <= sdram_c_data_out[15:8];
            sdram_c_read_req<=0;
            command_done <= 1;
          end
        end

      end else if (command == 8'hA2) begin//sdram long write
        if(data_arrive)begin
          if (data_index[0]==0)begin
            reg_temp <= data;//锁存低字节
          end else begin
            sdram_c_write_en <= 1;
            sdram_c_data_in<={data,reg_temp};
            if         (data_index==1)begin
              sdram_c_write_latch_address <= 1;
              sdram_c_address <= {uw_reg4,uw_reg3,uw_reg2};
            end else if(data_index==513)begin
              sdram_c_write_en <= 0;
              uart_send<=1;
              uart_data_w<=8'h41;//'A' 8'h41;
              command_done <= 1;
            end
          end
        end else begin
          sdram_c_write_en <= 0;
          sdram_c_write_latch_address <= 0;
        end

      end else if (command == 8'hA3) begin//sdram long read
        timer2<=timer2+1'b1;
        uart_send<=0;
        if(timer2==600)begin//25 * 10 +50
          timer2<=0;
        end
        if(timer2==0)begin
          if(timer==0)begin//锁存地址
            read_address    <= {uw_reg4,uw_reg3,uw_reg2};
            sdram_c_address <= {uw_reg4,uw_reg3,uw_reg2};
            sum <= 0;
          end else begin
            if(timer[0]==1)begin
              read_address <= read_address+1'b1;
            end
            sdram_c_address <= read_address;
          end
          sdram_c_read_req<=1;
        end else begin
          if(sdram_c_read_req && sdram_c_read_ack)begin
            timer<=timer+1'b1;
            sdram_c_read_req<=0;
            sum <= sum + ((timer[0]==1)?sdram_c_data_out[15:8]:sdram_c_data_out[7:0]);
            uart_send<=1;
            uart_data_w<=(timer[0]==1)?sdram_c_data_out[15:8]:sdram_c_data_out[7:0];
            if(timer==1024)begin
              uart_send<=1;
              uart_data_w<=sum;
              timer2<=0;
              timer<=0;
              command_done<=1;
            end
          end
        end
        
      end else if (command == 8'hA4) begin//sdram long write ok
        timer2<=timer2+1'b1;
        if(timer2==2)begin
          timer2<=0;
        end
        if(timer2==0)begin
          data_temp <= data_temp+1'b1;
          count_temp <= count_temp + 1'b1;

          if(count_temp==0)begin
            sdram_c_write_en<=1;
            sdram_c_data_in<=data_temp;
            sdram_c_write_latch_address<=1;
            sdram_c_address<=16;
          
          end else if(count_temp==128)begin
            count_temp<=0;
            sdram_c_write_en<=0;
            command_done <= 1;
          end else begin
            sdram_c_write_en<=1;
            sdram_c_write_latch_address<=0;
            sdram_c_data_in<=data_temp;
          end
        end else begin
          sdram_c_write_en<=0;
          sdram_c_write_latch_address<=0;
        end
      end else if (command == 8'hA5) begin//sdram long write3 error
        data_temp <= data_temp+1'b1;
        count_temp <= count_temp + 1'b1;
        //debug_port0<=count_temp;
        
        if(count_temp==0)begin
          sdram_c_write_en<=1;
          sdram_c_data_in<=data_temp;
          sdram_c_write_latch_address<=1;
          sdram_c_address<=16;
        
        end else if(count_temp==128)begin
          count_temp<=0;
          sdram_c_write_en<=0;
          command_done <= 1;
        end else begin
          sdram_c_write_en<=1;
          sdram_c_write_latch_address<=0;
          sdram_c_data_in<=data_temp;
        end


      end else begin
        command_done<=1;
        uart_send<=0;
      end
    end



  end
end









reg [23:0] sdram_c_address;
reg [15:0] sdram_c_data_in;
wire [15:0] sdram_c_data_out;
reg  sdram_c_read_req;
wire  sdram_c_read_ack;
reg  sdram_c_write_req;
wire  sdram_c_write_ack;
reg sdram_c_write_en;
reg sdram_c_write_latch_address;
//wire [7:0] probe_timer8;
//wire [7:0] probe_locked_time;
//wire [7:0] probe_sdram_init_done_timer;
//wire [7:0] probe_readBuffer0;
wire sdram_c_vga;
sdram ins_sdram(
  .sys_clk    (sys_clk  ),       // 时钟信号
  .sys_rst_n  (sys_rst_n),       // 复位信号

  //SDRAM 芯片接口
  .sdram_clk_out     (sdram_clk_out),
  .sdram_cke			(sdram_cke),		//SDRAM 时钟有效
  .sdram_cs_n			(sdram_cs_n),		//SDRAM 片选
  .sdram_ras_n		(sdram_ras_n),		//SDRAM 行有效	
  .sdram_cas_n		(sdram_cas_n),		//SDRAM 列有效
  .sdram_we_n			(sdram_we_n),		//SDRAM 写有效
  .sdram_ba			  (sdram_ba),			//SDRAM Bank地址
  .sdram_addr			(sdram_addr),		//SDRAM 行/列地址
  .sdram_data			(sdram_data),		//SDRAM 数据	
  .sdram_dqm		(sdram_dqm),
  
  //.sdram_prob_refresh  (sdram_prob_refresh),
  //.debug_port0(debug_port0),
  //.debug_port1(debug_port1),
  //.debug_port2(debug_port2),
  //.debug_pin0(debug_pin0),
  //.debug_pin1(debug_pin1),
  //.debug_pin2(debug_pin2),
  //.debug_pin3(debug_pin3),
  //.debug_pin6(debug_pin6),
  //.debug_pin7(debug_pin7),

  
  .clk        (sys_clk),//in
  .address    (sdram_c_address),//in
  .data_in    (sdram_c_data_in),//in
  .data_out   (sdram_c_data_out),//out
  .read_req   (sdram_c_read_req),//in
  .read_ack   (sdram_c_read_ack),//out
  .write_req  (sdram_c_write_req),//in
  .write_ack  (sdram_c_write_ack),//out
  .write_en   (sdram_c_write_en),//in
  .write_latch_address(sdram_c_write_latch_address),//in

  //.probe_timer8 (probe_timer8),
  //.probe_locked_time (probe_locked_time),
  //.probe_sdram_init_done_timer (probe_sdram_init_done_timer),
  //.probe_readBuffer0 (probe_readBuffer0),
  .vga        (sdram_c_vga)//out
);



endmodule
