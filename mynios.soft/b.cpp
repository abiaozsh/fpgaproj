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

int rnd(int max){
  int v = 0;
  for(int i=0;i<16;i++){
    v <<= 1;
    int r = IORD(RND, 0);
    v |= r;
  }
  v = v * max;
  v = v >> 16;
  return v;
}



int main()
{

  
  
  while(1){
    int total = 20*15;
    int* arr = (int*)malloc(total*4);
    print("n?\r\n");
    int n = scanInt();

    for(int i=0;i<total;i++){
      arr[i] = 0;
    }
    for(int i=0;i<n;i++){
      arr[i] = 1;
    }
    
    for(int i=0;i<total;i++){
      int pos = i+rnd(total-i);
      int tmp = arr[pos];
      arr[pos] = arr[i];
      arr[i] = tmp;
    }
    
    int linecnt = 0;
    for(int i=0;i<total;i++){
      putc('0'+arr[i]);
      linecnt++;
      if(linecnt==20){
        linecnt = 0;
        print("\r\n");
      }
    }
    
  }

  return 0;
  
}
