module mytimer (
    input          clk,                // clock.clk
    input          clk_50M,
    input          reset_n,               // reset.reset
    
    input   [15:0] avs_s0_address,     //    s0.address
    input          avs_s0_read,        //      .read
    input          avs_s0_write,       //      .write
    output  [31:0] avs_s0_readdata,    //      .readdata
    input   [31:0] avs_s0_writedata,   //      .writedata
    output         avs_s0_waitrequest, //      .waitrequest
    input   [3:0]  avs_s0_byteenable,    //      .readdata
    output reg     irq_req,
    input          irq_ack
  );

  // TODO: Auto-generated HDL template

  assign avs_s0_waitrequest = 1'b0;
  
  reg [31:0] timer[8];
  reg [31:0] timer_trans[4];
  
  reg [5:0] tick;
  reg req_buff;
  reg ack;

  reg sync_req_buff;
  reg sync_ack;
  
  always @ (posedge clk_50M or negedge reset_n) begin
    if (!reset_n) begin
      tick <= 0;
      timer[0] <= 0;
      timer[1] <= 0;
      timer[2] <= 0;
      timer[3] <= 0;
      timer[4] <= 32'hFFFFFFFF;
      timer[5] <= 32'hFFFFFFFF;
      timer[6] <= 32'hFFFFFFFF;
      timer[7] <= 32'hFFFFFFFF;
      req_buff <= 0;
      ack <= 0;
      irq_req <= 0;
      sync_req_buff <= 0;
      sync_ack <= 0;
    end else begin
      tick <= tick + 1'b1;
      if(tick==50) begin
        tick<=0;
      end
      if(tick==0)begin
        timer[0]<=timer[0]+1'b1;
        timer[1]<=timer[1]+1'b1;
        timer[2]<=timer[2]+1'b1;
        timer[3]<=timer[3]+1'b1;
        if(timer[0]==timer[4])begin timer[0]<=0;irq_req<=1;end
        if(timer[1]==timer[5])begin timer[1]<=0;irq_req<=1;end
        if(timer[2]==timer[6])begin timer[2]<=0;irq_req<=1;end
        if(timer[3]==timer[7])begin timer[3]<=0;irq_req<=1;end
      end
      
      req_buff <= req;
      if(req_buff && !ack) begin
        timer[set]<=val;
        ack <= 1;
      end
      if(!req_buff && ack) begin
        ack <= 0;
      end
      
      if(irq_ack)begin
        irq_req<=0;
      end
      
      sync_req_buff <= sync_req;
      if(sync_req_buff && !sync_ack) begin
        timer_trans[0]<=timer[0];
        timer_trans[1]<=timer[1];
        timer_trans[2]<=timer[2];
        timer_trans[3]<=timer[3];
        sync_ack <= 1;
      end
      
      if(!sync_req_buff && sync_ack) begin
        sync_ack <= 0;
      end
      
      
    end
  end
  
  assign avs_s0_readdata = timer_out[avs_s0_address[3:2]];
  
  reg [31:0] timer_out[4];

  reg [2:0] set;
  reg [31:0] val;
  reg        req;
  
  reg sync_req;
  reg sync_ack_buff;
  
  always @ (posedge clk or negedge reset_n) begin
    if (!reset_n) begin
      set <= 0;
      val <= 0;
      req <= 0;
      sync_ack_buff <= 0;
      sync_req <= 0;
    end else begin
      if(avs_s0_write) begin
        set <= avs_s0_address[4:2];
        val <= avs_s0_writedata;
        req <= 1;
      end
      
      if(ack)begin
        req <= 0;
      end
      
      sync_ack_buff <= sync_ack;
      if(!sync_req && !sync_ack_buff) begin
        sync_req <= 1;
      end
      if(sync_ack_buff) begin
        timer_out[0]<=timer_trans[0];
        timer_out[1]<=timer_trans[1];
        timer_out[2]<=timer_trans[2];
        timer_out[3]<=timer_trans[3];
        sync_req <= 0;
      end
      
      
    end
  end

  
endmodule
