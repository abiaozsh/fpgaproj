module uart_recv_hs(
    input sys_clk,
    input sys_rst_n,

    input uart_rxd,//UART接收端口

    output reg       uart_rec_req,//接收一帧数据完成标志信号 上升沿表示接收到
		input            uart_rec_ack,
    output reg [7:0] uart_data_out//接收的数据
  );

//parameter define
localparam BPS_CNT  = 25;        //2000000 bps  50000000/2000000
localparam BPS_CNT_HALF  = 12;        //2000000 bps  50000000/2000000

reg uart_rxd_last1;
reg uart_rxd_last2;
reg rx_flag; //接收过程标志信号
reg [7:0] clk_cnt; //系统时钟计数器

reg uart_rec_ack_buff;
//根据接收数据计数器来寄存uart接收端口数据
always @(posedge sys_clk or negedge sys_rst_n) begin 
  if ( !sys_rst_n) begin
    rx_flag <= 0;
    clk_cnt <= 0;
    uart_rxd_last1 <= 1;
    uart_rxd_last2 <= 1;
    uart_data_out <= 8'd0;
    uart_rec_req <= 0;
		uart_rec_ack_buff <= 0;
  end else begin
    uart_rxd_last1 <= uart_rxd;
    uart_rxd_last2 <= uart_rxd_last1;
		uart_rec_ack_buff <= uart_rec_ack;
		
    if(uart_rxd_last2 && !uart_rxd_last1) begin//检测到起始位
      rx_flag <= 1; //进入接收过程，标志位rx_flag拉高
    end else if(clk_cnt == (BPS_CNT * 9 + BPS_CNT_HALF)) begin
      rx_flag <= 0; //计数到停止位中间时，停止接收过程
    end

    
    if(rx_flag) begin //系统处于接收过程
      clk_cnt <= clk_cnt + 1'b1;
      case ( clk_cnt )
        (1*BPS_CNT+BPS_CNT_HALF) : uart_data_out[0] <= uart_rxd_last2; //寄存数据位最低位
        (2*BPS_CNT+BPS_CNT_HALF) : uart_data_out[1] <= uart_rxd_last2;
        (3*BPS_CNT+BPS_CNT_HALF) : uart_data_out[2] <= uart_rxd_last2;
        (4*BPS_CNT+BPS_CNT_HALF) : uart_data_out[3] <= uart_rxd_last2;
        (5*BPS_CNT+BPS_CNT_HALF) : uart_data_out[4] <= uart_rxd_last2;
        (6*BPS_CNT+BPS_CNT_HALF) : uart_data_out[5] <= uart_rxd_last2;
        (7*BPS_CNT+BPS_CNT_HALF) : uart_data_out[6] <= uart_rxd_last2;
        (8*BPS_CNT+BPS_CNT_HALF) : begin uart_data_out[7] <= uart_rxd_last2;uart_rec_req <= 1; end //寄存数据位最高位
      endcase
    end else begin
      clk_cnt <= 0;
    end
		
		if(uart_rec_ack_buff)begin
			uart_rec_req <= 0;
		end
		
  end
end

endmodule
