module uart_send_hs(
    input sys_clk,
    input sys_rst_n,
    
    output  reg uart_txd,//UART发送端口

    input       uart_send_req,//发送使能信号
    output reg  uart_send_ack,
    input [7:0] uart_data_in//待发送数据
  );

//parameter define
localparam BPS_CNT  = 25; //2000000 bps  50000000/2000000
localparam BPS_CNT_HALF  = 12;        //2000000 bps  50000000/2000000

reg uart_send_req_buff;
reg [7:0] clk_cnt;//系统时钟计数器

//根据发送数据计数器来给uart发送端口赋值
always @(posedge sys_clk or negedge sys_rst_n) begin
  if (!sys_rst_n) begin
    clk_cnt <= 0;
    uart_send_req_buff <= 0;
    uart_txd <= 1'b1;
		uart_send_ack <= 0;
  end else begin
    uart_send_req_buff <= uart_send_req;

    if(uart_send_req_buff && !uart_send_ack) begin //检测到发送使能上升沿
      clk_cnt <= clk_cnt + 1'b1;
      case(clk_cnt)
        (0 * BPS_CNT               ): uart_txd <= 1'b0; //起始位 
        (1 * BPS_CNT               ): uart_txd <= uart_data_in[0]; //数据位最低位
        (2 * BPS_CNT               ): uart_txd <= uart_data_in[1];
        (3 * BPS_CNT               ): uart_txd <= uart_data_in[2];
        (4 * BPS_CNT               ): uart_txd <= uart_data_in[3];
        (5 * BPS_CNT               ): uart_txd <= uart_data_in[4];
        (6 * BPS_CNT               ): uart_txd <= uart_data_in[5];
        (7 * BPS_CNT               ): uart_txd <= uart_data_in[6];
        (8 * BPS_CNT               ): uart_txd <= uart_data_in[7]; //数据位最高位
        (9 * BPS_CNT               ): uart_txd <= 1'b1; //停止位
				(9 * BPS_CNT + BPS_CNT_HALF): uart_send_ack <= 1;
        default: ;
      endcase
    end
		
		if(!uart_send_req_buff && uart_send_ack) begin
		  uart_send_ack <= 0;
		end
  end
end

endmodule

