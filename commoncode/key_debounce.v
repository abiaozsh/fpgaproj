
module key_debounce(
  input            sys_clk,
  input            sys_rst_n,
  input            key,
  output reg       key_value
);

  reg [19:0] counter;
  localparam top =20'd1000000;//50000000/1000000 = 1/50 s

  always @(posedge sys_clk or negedge sys_rst_n) begin 
    if (!sys_rst_n) begin 
      key_value <= key;
      counter <= 0;
    end else begin
      if(counter==0)begin
        key_value <= key;
        if(key_value!=key) begin //按键变动触发计时+锁止
          counter <= counter+1'b1;
        end
      end else if(counter==top)begin//计时到顶 解锁
        key_value <= key;
        counter <= 0;
      end else begin
        counter <= counter+1'b1;
      end
    end
  end
endmodule

