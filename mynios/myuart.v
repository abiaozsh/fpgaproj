module myuart (
		input         clk,                // clock.clk
    input         clk_50M,
		input         reset_n,              // reset.reset

		input         avs_s0_address,     //    s0.address 0 receive 1 send
		input         avs_s0_read,        //      .read
		input         avs_s0_write,       //      .write
		output [31:0] avs_s0_readdata,    //      .readdata
		input  [31:0] avs_s0_writedata,   //      .writedata
		output        avs_s0_waitrequest, //      .waitrequest
		input  [3:0]  avs_s0_byteenable,    //      .readdata
		output   reg  irq_req,       //  irq0.irq
    input         irq_ack,
    
		input     uart_rxd,
		output    uart_txd

	);

	assign avs_s0_waitrequest = 1'b0;

	assign avs_s0_readdata = avs_s0_address == 0 ? ({23'b0,rec_flg,rec_data}) : ({23'b0,uart_busy,8'b0});

  reg rec_flg;
  reg [7:0] rec_data;
	wire uart_busy = uart_send_req && !uart_send_ack;
  reg uart_rec_req_buff;
  always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
      rec_flg <= 0;
			uart_send_req <= 0;
      irq_req <= 0;
    end else begin
      uart_rec_req_buff <= uart_rec_req;
    
      if(rec_flg == 1 && avs_s0_read && avs_s0_address == 0)begin
        rec_flg <= 0;
      end
    
      if(uart_rec_req_buff && !uart_rec_ack)begin
        irq_req <= 1;
        rec_flg <= 1;
        rec_data <= uart_data_out;
				uart_rec_ack <= 1;
      end
      
			if(irq_ack)begin
        irq_req <= 0;
      end
      
      if(!uart_rec_req_buff && uart_rec_ack)begin
				uart_rec_ack <= 0;
      end
			
      
      
      if(avs_s0_write && avs_s0_address == 1)begin
        uart_send_req <= 1;
        uart_data_in <= avs_s0_writedata[7:0];
      end
      
			if(uart_send_ack)begin
			  uart_send_req <= 0;
			end
			
    end
  end

	wire uart_rec_req;
	reg  uart_rec_ack;
	wire [7:0] uart_data_out;
	reg uart_send_req;
	wire uart_send_ack;
	reg [7:0] uart_data_in;
	uart_hs uart_hs_inst (
	  .sys_clk(clk_50M),
	  .sys_rst_n(reset_n),

	  .uart_txd(uart_txd),
	  .uart_rxd(uart_rxd),
	  
	  .uart_rec_req(uart_rec_req),
		.uart_rec_ack(uart_rec_ack),
	  .uart_data_out(uart_data_out),

	  .uart_send_req(uart_send_req),
    .uart_send_ack(uart_send_ack),
	  .uart_data_in(uart_data_in)
	);

endmodule
