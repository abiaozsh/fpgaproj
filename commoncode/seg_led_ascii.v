module seg_led_ascii(
  input sys_clk,
  input sys_rst_n,
    
    output   reg  [5:0]     seg_sel,        // 数码管位选，最左侧数码管为最高位
    output   reg  [7:0]     seg_led,         // 数码管段选

    input                   en     ,        // 数码管使能信号
    input         [7:0]    char0   ,        // 6位数码管要显示的数值
    input         [7:0]    char1   ,        // 6位数码管要显示的数值
    input         [7:0]    char2   ,        // 6位数码管要显示的数值
    input         [7:0]    char3   ,        // 6位数码管要显示的数值
    input         [7:0]    char4   ,        // 6位数码管要显示的数值
    input         [7:0]    char5           // 6位数码管要显示的数值

    );

//parameter define
localparam  CLK_DIVIDE = 4'd15     ;        // 时钟分频系数
localparam  MAX_NUM    = 10'd1023  ;        // 对数码管驱动时钟(5MHz)计数1ms所需的计数值

//reg define
reg    [ 3:0]             clk_cnt  ;        // 时钟分频计数器
reg                       dri_clk  ;        // 数码管的驱动时钟,5MHz
reg    [9:0]             cnt0     ;        // 数码管驱动时钟计数器
reg                       flag     ;        // 标志信号（标志着cnt0计数达1ms）
reg    [2:0]              cnt_sel  ;        // 数码管位选计数器
reg    [7:0]              num_disp ;        // 当前数码管显示的数据


//对系统时钟10分频，得到的频率为5MHz的数码管驱动时钟dri_clk
always @(posedge clk or negedge rst_n) begin
   if(!rst_n) begin
       clk_cnt <= 4'd0;
       dri_clk <= 1'b1;
   end
   else if(clk_cnt == CLK_DIVIDE) begin
       clk_cnt <= 4'd0;
       dri_clk <= ~dri_clk;
   end
   else begin
       clk_cnt <= clk_cnt + 1'b1;
       dri_clk <= dri_clk;
   end
end

//每当计数器对数码管驱动时钟计数时间达1ms，输出一个时钟周期的脉冲信号
always @ (posedge dri_clk or negedge rst_n) begin
    if (rst_n == 1'b0) begin
        cnt0 <= 0;
        flag <= 1'b0;
     end
    else if (cnt0 < MAX_NUM - 1'b1) begin
        cnt0 <= cnt0 + 1'b1;
        flag <= 1'b0;
     end
    else begin
        cnt0 <= 0;
        flag <= 1'b1;
     end
end

//cnt_sel从0计数到5，用于选择当前处于显示状态的数码管
always @ (posedge dri_clk or negedge rst_n) begin
    if (rst_n == 1'b0)
        cnt_sel <= 3'b0;
    else if(flag) begin
        if(cnt_sel < 3'd5)
            cnt_sel <= cnt_sel + 1'b1;
        else
            cnt_sel <= 3'b0;
    end
    else
        cnt_sel <= cnt_sel;
end

//控制数码管位选信号，使6位数码管轮流显示
always @ (posedge dri_clk or negedge rst_n) begin
    if(!rst_n) begin
        seg_sel  <= 6'b111111;              //位选信号低电平有效
        num_disp <= 0;           
    end else if(!en) begin
            seg_sel  <= 6'b111111;          //使能信号为0时，所有数码管均不显示
            num_disp <= 0;
    end else begin
        case (cnt_sel)
            3'd0 :begin
                seg_sel  <= 6'b111110;  //显示数码管最低位
                num_disp <= char0;  //显示的数据
            end
            3'd1 :begin
                seg_sel  <= 6'b111101;  //显示数码管第1位
                num_disp <= char1;
            end
            3'd2 :begin
                seg_sel  <= 6'b111011;  //显示数码管第2位
                num_disp <= char2;
            end
            3'd3 :begin
                seg_sel  <= 6'b110111;  //显示数码管第3位
                num_disp <= char3;
            end
            3'd4 :begin
                seg_sel  <= 6'b101111;  //显示数码管第4位
                num_disp <= char4;
            end
            3'd5 :begin
                seg_sel  <= 6'b011111;  //显示数码管最高位
                num_disp <= char5;
            end
            default :begin
                seg_sel  <= 6'b111111;
                num_disp <= 0;
            end
        endcase
    end
end

//控制数码管段选信号，显示字符
always @ (posedge dri_clk or negedge rst_n) begin
    if (!rst_n) begin
        seg_led <= 8'b11111111;
    end else begin
        case (num_disp)//0~9  48~57
            //  a1
            //f1  b1
            //  g1
            //e1  c1
            //  d1  h1
            //0x00             hgfedcba
            0  : seg_led <= 8'b11111111;
            //  a0
            //f0  b0
            //  g1
            //e0  c0
            //  d0  h1
            //0                hgfedcba
            48 : seg_led <= 8'b11000000; //显示数字 0
            //  a1
            //f1  b0
            //  g1
            //e1  c0
            //  d1  h1
            //1                hgfedcba
            49 : seg_led <= 8'b11111001; //显示数字 1
            //  a0
            //f1  b0
            //  g0
            //e0  c1
            //  d0  h1
            //2                hgfedcba
            50 : seg_led <= 8'b10100100; //显示数字 2
            //  a0
            //f1  b0
            //  g0
            //e1  c0
            //  d0  h1
            //3                hgfedcba
            51 : seg_led <= 8'b10110000; //显示数字 3
            //  a1
            //f0  b0
            //  g0
            //e1  c0
            //  d1  h1
            //4                hgfedcba
            52 : seg_led <= 8'b10011001; //显示数字 4
            //  a0
            //f0  b1
            //  g0
            //e1  c0
            //  d0  h1
            //5                hgfedcba
            53 : seg_led <= 8'b10010010; //显示数字 5
            //  a0
            //f0  b1
            //  g0
            //e0  c0
            //  d0  h1
            //6                hgfedcba
            54 : seg_led <= 8'b10000010; //显示数字 6
            //  a0
            //f1  b0
            //  g1
            //e1  c0
            //  d1  h1
            //7                hgfedcba
            55 : seg_led <= 8'b11111000; //显示数字 7
            //  a0
            //f0  b0
            //  g0
            //e0  c0
            //  d0  h1
            //8                hgfedcba
            56 : seg_led <= 8'b10000000; //显示数字 8
            //  a0
            //f0  b0
            //  g0
            //e1  c0
            //  d0  h1
            //9               hgfedcba
            57 : seg_led <= 8'b10010000; //显示数字 9
            //  a0
            //f0  b1
            //  g0
            //e0  c1
            //  d1  h1
            //其他字符            hgfedcba
            default:seg_led <= 8'b10001110;
        endcase
    end
end

endmodule 