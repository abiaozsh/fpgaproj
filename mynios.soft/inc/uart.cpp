
int uart_read(int timeout){
  if(timeout!=-1){
    IOWR(MYTIMER, 0 ,0);
  }
  while(1){
    if(timeout!=-1){
      int timer = IORD(MYTIMER, 0);
      if(timer>timeout){
        return -1;
      }
    }
    int tmp = IORD(MYUART, 0);
    if(tmp&0x100){
      return tmp & 0xFF;
    }
  }
}

int uart_read(){
  uart_read(-1);
}

int uart_write(int val){
  while((IORD(MYUART, 1)) & 0x100);
  IOWR(MYUART, 1 ,val);
}


