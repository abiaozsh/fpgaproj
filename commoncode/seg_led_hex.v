module seg_led_hex(
  input sys_clk,
  input sys_rst_n,
  
  output reg  [5:0]     seg_sel,
  output reg  [7:0]     seg_led,

  input [7:0] data0,
  input [7:0] data1,
  input [7:0] data2
);

localparam  TOP    = 16'd65000  ;        // 对数码管驱动时钟(5MHz)计数1ms所需的计数值

//每当计数器对数码管驱动时钟计数时间达1ms，输出一个时钟周期的脉冲信号
reg [15:0] timer;        // 数码管驱动时钟计数器
reg flag;        // 标志信号（标志着cnt0计数达1ms）
always @ (posedge sys_clk or negedge sys_rst_n) begin
  if (!sys_rst_n) begin
    timer <= 0;
    flag <= 0;
  end else begin
    if (timer == TOP) begin
      timer <= 0;
      flag <= 1'b1;
    end else begin
      timer <= timer + 1'b1;
      flag <= 0;
    end
  end
end

//cnt_sel从0计数到5，用于选择当前处于显示状态的数码管
reg [2:0] cnt_sel;        // 数码管位选计数器
always @ (posedge sys_clk or negedge sys_rst_n) begin
  if (!sys_rst_n) begin
    cnt_sel <= 0;
  end else begin
    if(flag) begin
      if(cnt_sel == 5)begin
        cnt_sel <= 3'b0;
      end else begin
        cnt_sel <= cnt_sel + 1'b1;
      end
    end
  end
end

reg [3:0] num_disp;        // 当前数码管显示的数据
//控制数码管位选信号，使6位数码管轮流显示
always @ (posedge sys_clk or negedge sys_rst_n) begin
  if (!sys_rst_n) begin
    seg_sel  <= 6'b111111;              //位选信号低电平有效
    num_disp <= 4'b0;
  end else begin
    case (cnt_sel)
      3'd0 :begin
          seg_sel  <= 6'b111110;  //显示数码管最低位
          num_disp <= data0[3:0];  //显示的数据
      end
      3'd1 :begin
          seg_sel  <= 6'b111101;  //显示数码管第1位
          num_disp <= data0[7:4];
      end
      3'd2 :begin
          seg_sel  <= 6'b111011;  //显示数码管第2位
          num_disp <= data1[3:0];
      end
      3'd3 :begin
          seg_sel  <= 6'b110111;  //显示数码管第3位
          num_disp <= data1[7:4];
      end
      3'd4 :begin
          seg_sel  <= 6'b101111;  //显示数码管第4位
          num_disp <= data2[3:0];
      end
      3'd5 :begin
          seg_sel  <= 6'b011111;  //显示数码管最高位
          num_disp <= data2[7:4];
      end
      default :;
    endcase
  end
end

//控制数码管段选信号，显示字符
always @ (posedge sys_clk or negedge sys_rst_n) begin
  if (!sys_rst_n) begin
    seg_led <= 0;
  end else begin
    case (num_disp)
      //  a0
      //f0  b0
      //  g1
      //e0  c0
      //  d0  h1
      //0                hgfedcba
      0 : seg_led <= 8'b11000000; //显示数字 0
      //  a1
      //f1  b0
      //  g1
      //e1  c0
      //  d1  h1
      //1                hgfedcba
      1 : seg_led <= 8'b11111001; //显示数字 1
      //  a0
      //f1  b0
      //  g0
      //e0  c1
      //  d0  h1
      //2                hgfedcba
      2 : seg_led <= 8'b10100100; //显示数字 2
      //  a0
      //f1  b0
      //  g0
      //e1  c0
      //  d0  h1
      //3                hgfedcba
      3 : seg_led <= 8'b10110000; //显示数字 3
      //  a1
      //f0  b0
      //  g0
      //e1  c0
      //  d1  h1
      //4                hgfedcba
      4 : seg_led <= 8'b10011001; //显示数字 4
      //  a0
      //f0  b1
      //  g0
      //e1  c0
      //  d0  h1
      //5                hgfedcba
      5 : seg_led <= 8'b10010010; //显示数字 5
      //  a0
      //f0  b1
      //  g0
      //e0  c0
      //  d0  h1
      //6                hgfedcba
      6 : seg_led <= 8'b10000010; //显示数字 6
      //  a0
      //f1  b0
      //  g1
      //e1  c0
      //  d1  h1
      //7                hgfedcba
      7 : seg_led <= 8'b11111000; //显示数字 7
      //  a0
      //f0  b0
      //  g0
      //e0  c0
      //  d0  h1
      //8                hgfedcba
      8 : seg_led <= 8'b10000000; //显示数字 8
      //  a0
      //f0  b0
      //  g0
      //e1  c0
      //  d0  h1
      //9               hgfedcba
      9 : seg_led <= 8'b10010000; //显示数字 9
      //  a0
      //f0  b0
      //  g0
      //e0  c0
      //  d1  h1
      //A               hgfedcba
      10: seg_led <= 8'b10001000; //显示数字 A
      //  a1
      //f0  b1
      //  g0
      //e0  c0
      //  d0  h1
      //b               hgfedcba
      11: seg_led <= 8'b10000011; //显示数字 b
      //  a0
      //f0  b1
      //  g1
      //e0  c1
      //  d0  h1
      //C               hgfedcba
      12: seg_led <= 8'b11000110; //显示数字 C
      //  a1
      //f1  b0
      //  g0
      //e0  c0
      //  d0  h1
      //d               hgfedcba
      13: seg_led <= 8'b10100001; //显示数字 d
      //  a0
      //f0  b1
      //  g0
      //e0  c1
      //  d0  h1
      //E               hgfedcba
      14: seg_led <= 8'b10000110; //显示数字 E
      //  a0
      //f0  b1
      //  g0
      //e0  c1
      //  d1  h1
      //F               hgfedcba
      15: seg_led <= 8'b10001110; //显示数字 F
    endcase
  end
end

endmodule 