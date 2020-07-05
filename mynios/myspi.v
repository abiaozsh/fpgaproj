// myuart.v

// This file was auto-generated as a prototype implementation of a module
// created in component editor.  It ties off all outputs to ground and
// ignores all inputs.  It needs to be edited to make it do something
// useful.
// 
// This file will not be automatically regenerated.  You should check it in
// to your version control system if you want to keep it.

`timescale 1 ps / 1 ps
module myspi (
		input  wire        clk,                // clock.clk
		input  wire        reset_n,              // reset.reset

		input  wire        avs_s0_address,     //    s0.address 0 receive 1 send
		input  wire        avs_s0_read,        //      .read
		output wire [31:0] avs_s0_readdata,    //      .readdata
		input  wire        avs_s0_write,       //      .write
		input  wire [31:0] avs_s0_writedata,   //      .writedata
		output wire        avs_s0_waitrequest, //      .waitrequest
		output wire        ins_irq0_irq,       //  irq0.irq
		input wire 			   miso,
		output reg			   mosi,
		output reg			   sck,
		output reg			   cs1,
		output reg			   cs2,
		output reg			   cs3,
		output [7:0] debug8,
		output [31:0] debug32
		
	);

	// TODO: Auto-generated HDL template

	assign avs_s0_waitrequest = 1'b0;

	assign avs_s0_readdata = avs_s0_address == 0 ? ({23'b0,rec_flg,rec_data}) : ({23'b0,uart_busy,8'b0});

	assign ins_irq0_irq = 1'b0;

  reg rec_flg;
  reg [7:0] rec_data;
  always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
      rec_flg <= 0;
    end else begin
      if(rec_flg == 1 && avs_s0_read && avs_s0_address == 0)begin
        rec_flg <= 0;
      end
    
      if(uart_rec)begin
        rec_flg <= 1;
        rec_data <= uart_data_out;
      end
      
      uart_send <= 0;
      if(avs_s0_write && avs_s0_address == 1)begin
        uart_send <= 1;
        uart_data_in <= avs_s0_writedata[7:0];
      end
      
    end
  end

	assign debug8[0] = uart_rec;
	assign debug8[1] = uart_rxd;
  assign debug32[7:0] = uart_data_out;
	
	wire uart_rec;
	wire [7:0] uart_data_out;
	reg uart_send;
	reg [7:0] uart_data_in;
  wire uart_busy;
	uart_hs (
	  .sys_clk(clk), 
	  .sys_rst_n(reset_n),

	  .uart_txd(uart_txd),
	  .uart_rxd(uart_rxd),
	  
	  .uart_rec(uart_rec),
	  .uart_data_out(uart_data_out),

	  .uart_send(uart_send),
	  .uart_data_in(uart_data_in),
    .busy(uart_busy)
	);
  
endmodule
