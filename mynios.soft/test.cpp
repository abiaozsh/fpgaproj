#include "inc/io.h"
#include "inc/system.h"
#include "inc/noirq.h"
#include "inc/system.cpp"


int uart_write(int val){
  while((IORD(MYUART, 1)) & 0x100);
  IOWR(MYUART, 1 ,val);
}


int printHex(int val){
  char* chardata = "0123456789ABCDEF";
  uart_write(chardata[(val>>28)&0x0F]);
  uart_write(chardata[(val>>24)&0x0F]);
  uart_write(chardata[(val>>20)&0x0F]);
  uart_write(chardata[(val>>16)&0x0F]);
  uart_write(chardata[(val>>12)&0x0F]);
  uart_write(chardata[(val>>8)&0x0F]);
  uart_write(chardata[(val>>4)&0x0F]);
  uart_write(chardata[(val)&0x0F]);
  uart_write('\r');
  uart_write('\n');

}


int testbne(int a,int b){  if(a==b){    return 1;  }else{    return 2;  }}
int testbeq(int a,int b){  if(a!=b){    return 1;  }else{    return 2;  }}
int testbge(int a,int b){  if(a<b){    return 1;  }else{    return 2;  }}
int testblt(int a,int b){  if(a>=b){    return 1;  }else{    return 2;  }}
int testbltu(unsigned int a, unsigned int b){  if(a>=b){    return 1;  }else{    return 2;  }}
int testbgeu(unsigned int a, unsigned int b){  if(a<b){    return 1;  }else{    return 2;  }}

int testandi(int a){  return a & 0x55AA;}
int testandhi(int a){  return a & 0x55AA0000;}
int testaddi(int a){  return a + 0x55AA;}
int testaddi2(int a){  return a + 0xAA55;}
int testorhi(int a){  return a | 0x55AA0000;}
int testori(int a){  return a | 0x55AA;}
int testxori(int a){  return a ^ 0x55AA;}
int testxorhi(int a){  return a ^ 0x55AA0000;}

int testor(int a,int b){  return a|b;  }

//muli reg, reg, ins           @          10 @                      0 @  36 @          100100      0x24
//cmpeqi reg, reg, ins         @          10 @                      0 @  32 @          100000      0x20
//cmplti reg, reg, ins         @          10 @                      0 @  16 @          010000      0x10
//cmpltui reg, reg, ins        @          10 @                      0 @  48 @          110000      0x30
//cmpgei reg, reg, ins         @          10 @                      0 @   8 @          001000      0x08
//cmpnei reg, reg, ins         @          10 @                      0 @  24 @          011000      0x18
//cmpgeui reg, reg, ins        @          10 @                      0 @  40 @          101000      0x28
volatile int a;
void dummy(){
  a = 1;
  return;
}

int main(){
  //asm("hlt 1");
  //asm("hlt 0");
  
  asm("and r17, r0, r0");
  asm("and r16, r0, r0");
  dummy();
  asm("or r17, r17, r16");
  asm("addi r4, r0, 115");
  asm("hlt 1");
//00001424 [46110120] () or r17, r17, r16
//00001428 [00800e61] () addi r4, r0, 115

  uart_write('t');
  uart_write('e');
  uart_write('s');
  uart_write('t');
  uart_write('\r');
  uart_write('\n');
  int res;

  res = testbne(0x00000000,0x11111111);printHex(res);
  res = testbne(0x11111111,0x11111111);printHex(res);
  uart_write('\r');  uart_write('\n');

  res = testbeq(0x00000000,0x11111111);printHex(res);
  res = testbeq(0x11111111,0x11111111);printHex(res);
  uart_write('\r');  uart_write('\n');
  
  res = testbge(0x00000000,0x11111111);printHex(res);
  res = testbge(0x11111111,0x11111111);printHex(res);
  res = testbge(0x11111111,0x00000000);printHex(res);
  uart_write('\r');  uart_write('\n');

  res = testblt(0x00000000,0x11111111);printHex(res);
  res = testblt(0x11111111,0x11111111);printHex(res);
  res = testblt(0x11111111,0x00000000);printHex(res);
  uart_write('\r');  uart_write('\n');

  res = testbltu(0x00000000,0x11111111);printHex(res);
  res = testbltu(0x11111111,0x11111111);printHex(res);
  res = testbltu(0x11111111,0x00000000);printHex(res);
  uart_write('\r');  uart_write('\n');
  
  res = testbgeu(0x00000000,0x11111111);printHex(res);
  res = testbgeu(0x11111111,0x11111111);printHex(res);
  res = testbgeu(0x11111111,0x00000000);printHex(res);
  uart_write('\r');  uart_write('\n');
  
  //asm("hlt 1");
  //asm("hlt 0");
  res = testandi(0x11111111);printHex(res);
  res = testandi(0x00000000);printHex(res);
  res = testandhi(0x11111111);printHex(res);
  res = testandhi(0x00000000);printHex(res);
  uart_write('\r');  uart_write('\n');
  
  res = testor(0x00000000,0x00000000);printHex(res);
  res = testor(0x00000000,0x11111111);printHex(res);
  res = testor(0x11111111,0x11111111);printHex(res);
  res = testor(0x11111111,0x00000000);printHex(res);
  uart_write('\r');  uart_write('\n');
  
//int testaddi(int a){  return a + 0x55AA;}
//int testaddi2(int a){  return a + 0xAA55;}
//int testorhi(int a){  return a | 0x55AA0000;}
//int testori(int a){  return a | 0x55AA;}
//int testxori(int a){  return a ^ 0x55AA;}
//int testxorhi(int a){  return a ^ 0x55AA0000;}

  while(1){
    
  }
}
