#include "inc/io.h"
#include "inc/system.h"
#include "inc/irq.h"
#include "inc/system.cpp"
#include "inc/uart.cpp"
#include "inc/uartio.cpp"

//int uart_write(int val){
//  while((IORD(MYUART, 1)) & 0x100);
//  IOWR(MYUART, 1 ,val);
//}
//
//int uart_read(){
//  while(1){
//    int tmp = IORD(MYUART, 0);
//    if(tmp&0x100){
//      return tmp & 0xFF;
//    }
//  }
//}

int printByte(int val){
  char* chardata = "0123456789ABCDEF";
  uart_write(chardata[(val>>4)&0x0F]);
  uart_write(chardata[(val)&0x0F]);
}

int main(){
  stdioInit(1024);
  uart_write('t');
  uart_write('e');
  uart_write('s');
  uart_write('t');
  uart_write('A');
  while(1){
    int val1 = uart_read();
    uart_write(val1);
    //if(val1=='a'){
    //  uart_write('?');
    //  int val2 = uart_read()-'0';
    //  int val3 = uart_read()-'0';
    //  uart_write(' ');
    //  printByte(val2);
    //  uart_write(' ');
    //  printByte(val3);
    //  uart_write('\r');
    //  uart_write('\n');
    //  
    //  int val4 = val2/val3;
    //  int val5 = val2*val3;
    //  uart_write(' ');
    //  printByte(val4);
    //  uart_write(' ');
    //  printByte(val5);
    //  uart_write('\r');
    //  uart_write('\n');
    //
    //}else{
    //  uart_write('*');
    //}
  }
}
