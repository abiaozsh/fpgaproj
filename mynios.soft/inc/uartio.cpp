int stdioInit(int p0){
  
}

int getc(){
  uart_read();
}

void putc(int val){
  uart_write(val);
}


