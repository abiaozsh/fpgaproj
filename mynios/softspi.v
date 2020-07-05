module softspi (
    input  clk,                // clock.clk
    input  clk_25M,
    input  reset_n,               // reset.reset
    
    input   [13:0] avs_s0_address,     //    s0.address
    input          avs_s0_read,        //      .read
    input          avs_s0_write,       //      .write
    output  [31:0] avs_s0_readdata,    //      .readdata
    input   [31:0] avs_s0_writedata,   //      .writedata
    output         avs_s0_waitrequest, //      .waitrequest
    input   [3:0]  avs_s0_byteenable,    //      .readdata

    output [7:0]   debug8,
    output [3:0]   debug4,

    input      [2:0]  MISO,
    output     [2:0]  MOSI,
    output     [2:0]  SCLK,
    output reg [2:0]  CS
    
);

  //reg [7:0] clk_count;
  //reg [7:0] clk_delay;
  
  reg [3:0] clk_count;
  parameter clk_delay = 15;
  
  
  assign debug8 = clk_delay;
  assign debug4[0]=~read_req_buff;  // 0  00101100
  assign debug4[1]=~read_ack;       // 0
  assign debug4[2]=~write_req_buff; // 1
  assign debug4[3]=~write_ack;      // 1
  
  reg _MOSI_;
  assign MOSI[0] = _MOSI_;//(~CS[0]) ?  : 1'b1;
  assign MOSI[1] = _MOSI_;//(~CS[1]) ?  : 1'b1;
  assign MOSI[2] = _MOSI_;//(~CS[2]) ?  : 1'b1;
  reg _SCLK_;
  assign SCLK[0] = _SCLK_;//(~CS[0]) ?  : 1'b0;
  assign SCLK[1] = _SCLK_;//(~CS[1]) ?  : 1'b0;
  assign SCLK[2] = _SCLK_;//(~CS[2]) ?  : 1'b0;

  
  wire rst_sd_n = reset_n & reset_by_cpu_n;
  
  wire _MISO_ = (~CS[0]) ? MISO[0] :
                (~CS[1]) ? MISO[1] :
                (~CS[2]) ? MISO[2] : 0;

  reg [7:0] read_data;
  reg read_req_buff;
  reg read_ack;
  reg write_req_buff;
  reg write_ack;
  reg manual_req_buff;
  reg manual_ack;
  reg [3:0] count;
  reg       state;
  always @ (posedge clk_25M or negedge rst_sd_n) begin
    if (!rst_sd_n) begin
      _MOSI_ <= 1;
      _SCLK_ <= 0;
      read_req_buff <= 0;
      read_ack <= 0;
      write_req_buff <= 0;
      write_ack <= 0;
      count <= 0;
      state <= 0;
      clk_count <= 0;
    end else begin
      read_req_buff <= read_req;
      write_req_buff <= write_req;
      manual_req_buff <= manual_req;

      if(manual_req_buff && !manual_ack)begin
        _MOSI_ <= _MOSI_data;
        _SCLK_ <= _SCLK_data;
        manual_ack <= 1;
      end
      if(!manual_req_buff && manual_ack)begin
        manual_ack <= 0;
      end

      
      
      if(read_req_buff && !read_ack)begin
        _MOSI_ <= 1;
        case(rpha)
        0:begin
          if(         state==0)begin
            _SCLK_ <= ~rpol;
            clk_count <= clk_count+1'b1;
            if(clk_count==clk_delay)begin
              clk_count<=0;
              state <= 1;
            end
            if(clk_count==0)begin
              read_data[count] <= _MISO_;
            end
          end else if(state==1)begin
            _SCLK_ <= rpol;
            clk_count <= clk_count+1'b1;
            if(clk_count==clk_delay)begin
              clk_count<=0;
              state <= 0;
              count <= count + 1'b1;
              if(count == 7)begin
                count <= 0;
                read_ack <= 1;
              end
            end
          end
        end
        1:begin
          if(         state==0)begin
            _SCLK_ <= ~rpol;
            clk_count <= clk_count+1'b1;
            if(clk_count==clk_delay)begin
              clk_count<=0;
              state <= 1;
            end
          end else if(state==1)begin
            _SCLK_ <= rpol;
            clk_count <= clk_count+1'b1;
            if(clk_count==clk_delay)begin
              clk_count<=0;
              state <= 0;
              count <= count + 1'b1;
              if(count == 7)begin
                count <= 0;
                read_ack <= 1;
              end
            end
            if(clk_count==0)begin
              read_data[count] <= _MISO_;
            end
          end
        end
        endcase
      end
      if(!read_req_buff && read_ack)begin
        read_ack <= 0;
      end

      if(write_req_buff && !write_ack)begin
        case(wpha)
        0:begin
          if(         state==0)begin
            _SCLK_ <= ~wpol;
            clk_count <= clk_count+1'b1;
            if(clk_count==clk_delay)begin
              clk_count<=0;
              state <= 1;
            end
            if(clk_count==0)begin
              _MOSI_ <= write_data[count];
            end
          end else if(state==1)begin
            _SCLK_ <= wpol;
            clk_count <= clk_count+1'b1;
            if(clk_count==clk_delay)begin
              clk_count<=0;
              state <= 0;
              count <= count + 1'b1;
              if(count == 7)begin
                count <= 0;
                write_ack <= 1;
              end
            end
          end
        end
        1:begin
          if(         state==0)begin
            _SCLK_ <= ~wpol;
            clk_count <= clk_count+1'b1;
            if(clk_count==clk_delay)begin
              clk_count<=0;
              state <= 1;
            end
          end else if(state==1)begin
            _SCLK_ <= wpol;
            clk_count <= clk_count+1'b1;
            if(clk_count==clk_delay)begin
              clk_count<=0;
              state <= 0;
              count <= count + 1'b1;
              if(count == 7)begin
                count <= 0;
                write_ack <= 1;
              end
            end
            if(clk_count==0)begin
              _MOSI_ <= write_data[count];
            end
          end
        end
        endcase
      end
      if(!write_req_buff && write_ack)begin
        write_ack <= 0;
      end

    end
  end

  assign avs_s0_waitrequest = 1'b0;
  
  assign avs_s0_readdata = avs_s0_address == 0 ? {read_data_valid,read_data[0],read_data[1],read_data[2],read_data[3],read_data[4],read_data[5],read_data[6],read_data[7]} : 
                           avs_s0_address == 1 ? {write_data_done,8'b0} : {6'b0,MISO};

  
  reg rpol;
  reg rpha;
  reg wpol;
  reg wpha;

  reg       read_req;
  reg       write_req;
  reg       manual_req;
  reg       _MOSI_data;
  reg       _SCLK_data;

  reg       read_data_valid;
  reg [7:0] write_data;
  reg       write_data_done;
  reg read_ack_buff;
  reg write_ack_buff;
  reg manual_ack_buff;
  reg read_ack_buff2;
  reg write_ack_buff2;
  reg manual_ack_buff2;
  reg reset_by_cpu_n;
  always @ (posedge clk or negedge reset_n) begin
    if (!reset_n) begin
      CS <= 3'b111;
      read_req <= 0;
      write_req <= 0;
      manual_req <= 0;
      read_ack_buff <= 0;
      write_ack_buff <= 0;
      read_ack_buff2 <= 0;
      write_ack_buff2 <= 0;
      write_data_done <= 0;
      read_data_valid <= 0;
      rpol <= 0;
      rpha <= 0;
      wpol <= 0;
      wpha <= 0;
      reset_by_cpu_n <= 1;
      _MOSI_data <= 0;
      _SCLK_data <= 0;
    end else begin
      read_ack_buff <= read_ack;
      write_ack_buff <= write_ack;
      manual_ack_buff <= manual_ack;
      
      read_ack_buff2 <= read_ack_buff;
      write_ack_buff2 <= write_ack_buff;
      manual_ack_buff2 <= manual_ack_buff;
      
      if(avs_s0_read) begin
        if(avs_s0_address==0) begin
          read_data_valid <= 0;
        end
        if(avs_s0_address==1) begin
          write_data_done <= 0;
        end
      end

      if(avs_s0_write) begin
        if(avs_s0_address==0) begin
          read_req<=1;
        end
        if(avs_s0_address==1) begin
          write_data<={avs_s0_writedata[0],avs_s0_writedata[1],avs_s0_writedata[2],avs_s0_writedata[3],avs_s0_writedata[4],avs_s0_writedata[5],avs_s0_writedata[6],avs_s0_writedata[7]};
          write_req<=1;
        end
        if(avs_s0_address==2) begin
          CS<=avs_s0_writedata[2:0];
        end
        
        if(avs_s0_address==3) begin
          CS <= 3'b111;
          reset_by_cpu_n<=avs_s0_writedata[0];
        end
        
        if(avs_s0_address==4) begin
          {wpol,wpha,rpol,rpha}<=avs_s0_writedata[3:0];
        end
        
        if(avs_s0_address==5) begin
          _MOSI_data <= avs_s0_writedata[0];
          manual_req <= 1;
        end
        
        if(avs_s0_address==6) begin
          _SCLK_data <= avs_s0_writedata[0];
          manual_req <= 1;
        end

        
        
      end

      if(!read_ack_buff2 && read_ack_buff)begin
        read_req<=0;
        read_data_valid <= 1;
      end
      
      if(!write_ack_buff2 && write_ack_buff)begin
        write_req<=0;
        write_data_done <= 1;
      end

      if(!manual_ack_buff2 && manual_ack_buff)begin
        manual_req <= 0;
      end
      

    end
  end

endmodule
