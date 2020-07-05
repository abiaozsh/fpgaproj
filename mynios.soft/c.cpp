//中断向量
//浮点数

//#define DEBUG

#include "inc/io.h"
#include "inc/system.h"
#include "inc/irq.h"
#include "inc/system.cpp"
#include "inc/uart.cpp"
#include "inc/uartio.cpp"
#include "inc/print.cpp"
#include "inc/soft-fp/soft-fp.h"


double scanDouble(){
  char buff[10];
  char buff2[10];
  int j;
  int idx = 0;
  int idx2 = 0;
  int neg = 0;
  char v;
  for(int i = 0; i < 10 ; i++)
  {
    v = uart_read();
    if(i==0 && v=='-'){
      neg = 1;
    }else{
      if(v<'0'||v>'9'){
        break;
      }
      buff[idx++] = v;
    }
  }
  
  if(v=='.'){
    for(int i = 0; i < 10 ; i++)
    {
      v = uart_read();
      if(v<'0'||v>'9'){
        break;
      }
      buff2[idx2++] = v;
    }
  }
  
  double val = 0;
  for(int i = 0; i < idx ; i++)
  {
    val+=num10d[10-idx+i]*((double)(int)(buff[i]-'0'));
  }

  for(int i=0;i<idx2;i++){
    v = buff2[i];
    if(v<'0'||v>'9'){
      break;
    }
    val+=num10f[i]*((double)(int)(v-'0'));
  }

  
  if(neg){
    val=-val;
  }
  return val;
}

void printDouble(double v){
  if(v<0){
    v=-v;
    uart_write('-');
  }
  int lv = (int)v;
  char buff[10];
  int idx=0;
  if(lv!=0){
    while(1){
      int rem = lv % 10;
      buff[idx++] = '0'+rem;
      lv = lv / 10;
      if(lv==0)break;
    }
    while(idx>0){
      uart_write(buff[--idx]);
    }
  }else{
    uart_write('0');
  }
  uart_write('.');
  double v1 = v - (double)(int)v;
  for(int i=0;i<10;i++){
    v1 = v1 * 10;
    uart_write('0'+(int)v1);
    v1 = v1 - (double)(int)v1;
  }
}
volatile double dummy;
volatile double dummyv1;
volatile double dummyv2;

int main()
{

  while(1){
    //出现1.0000001的情况，多是由于mem指令 写内存指令 写了寄存器
    print("v1?\r\n");
    double v1 = scanDouble();
    printDouble(v1);print("\r\n");
    print("v2?\r\n");
    double v2 = scanDouble();
    printDouble(v2);print("\r\n");
    {
      double a = v1+v2;
      double b = v1-v2;
      double c = v1*v2;
      double d = v1/v2;

      printDouble(a);print("\r\n");
      printDouble(b);print("\r\n");
      printDouble(c);print("\r\n");
      printDouble(d);print("\r\n");
    }
    int iv1 = (int)v1;
    int iv2 = (int)v2;
    {
      int a = iv1+iv2;
      int b = iv1-iv2;
      int c = iv1*iv2;
      int d = iv1/iv2;

      printInt(a);print("\r\n");
      printInt(b);print("\r\n");
      printInt(c);print("\r\n");
      printInt(d);print("\r\n");
    }
    
    IOWR(MYTIMER, 2, 0);
    for(int i=0;i<10;i++){
      dummy = dummyv1 + dummyv2;
      dummy = dummyv1 - dummyv2;
      dummy = dummyv1 * dummyv2;
      dummy = dummyv1 / dummyv2;
    }
    int time1 = IORD(MYTIMER, 2);
    print("time1:");printInt(time1);print("\r\n");
    
    IOWR(MYTIMER, 2, 0);
    for(int i=0;i<100;i++){
      dummy = dummyv1 + dummyv2;
      dummy = dummyv1 - dummyv2;
      dummy = dummyv1 * dummyv2;
      dummy = dummyv1 / dummyv2;
    }
    int time2 = IORD(MYTIMER, 2);
    print("time2:");printInt(time2);print("\r\n");
  }

  return 0;
  
}
