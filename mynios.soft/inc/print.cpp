
int print(const char* str){
  int idx = 0;
  while(1){
    char tmp = str[idx];
    if(tmp=='\0')break;
    putc(tmp);
    idx++;
  }
}


int print(const char* str, int len){
  int idx = 0;
  int i;
  for(i=0;i<len;i++){
    char tmp = str[i];
    putc(tmp);
  }
}


int printgb(const char* str){
  int idx = 0;
  while(1){
    int tmp1 = str[idx]&0xFF;
    int tmp2 = str[idx]&0xFF;
    if(tmp1=='\0')break;
    if(tmp1>=0xA0 && tmp2>=0xA0){
      int i = tmp1 - 0xA0;
      int j = tmp2 - 0xA0;
      #ifdef putcgb
      putcgb(94 * (i - 1) + (j - 1));
      #endif
      idx+=2;
    }else{
      putc(tmp1);
      idx++;
    }
  }
}



int printByte(int val){
  char* chardata = "0123456789ABCDEF";
  putc(chardata[(val>>4)&0x0F]);
  putc(chardata[(val)&0x0F]);
}

int printHex(int val){
  char* chardata = "0123456789ABCDEF";
  putc(chardata[(val>>28)&0x0F]);
  putc(chardata[(val>>24)&0x0F]);
  putc(chardata[(val>>20)&0x0F]);
  putc(chardata[(val>>16)&0x0F]);
  putc(chardata[(val>>12)&0x0F]);
  putc(chardata[(val>>8)&0x0F]);
  putc(chardata[(val>>4)&0x0F]);
  putc(chardata[(val)&0x0F]);
}

int printBin(int data2){
  for(int i=31;i>=0;i--){
    putc((data2>>i)&1?'1':'0'); // Status Byte
  }
}


int equal(const char* a,const  char* b, int maxlen){
  int i = 0;
  while(1){
    if(a[i]!=b[i]){
      return 0;
    }
    if(a[i]=='\0' && b[i]=='\0'){
      break;
    }
    if(a[i]=='\0' || b[i]=='\0'){
      return 0;
    }
    
    i++;
    if(maxlen>0){
      if(i>=maxlen){
        break;
      }
    }
  }
  return 1;
}

int scan(char* buff, int maxlen){
  int idx = 0;
  while(1){
    int c = getc();
    if(c==-1){
      return 0;
    }
    if(maxlen>=0){
      if(idx>=maxlen){
        buff[idx] = (char)0;
        return idx;
      }
    }
    buff[idx] = (char)c;
    if(c==0x0A){
      buff[idx] = 0;
      return idx;
    }
    idx++;
  }
  
}

int num10s[] = {
1000000000,
100000000,
10000000,
1000000,
100000,
10000,
1000,
100,
10,
1,
};

double num10d[] = {
1000000000,
100000000,
10000000,
1000000,
100000,
10000,
1000,
100,
10,
1,
};
double num10f[] = {
0.1,
0.01,
0.001,
0.0001,
0.00001,
0.000001,
0.0000001,
0.00000001,
0.000000001,
0.0000000001,
0.00000000001,
};

void printInt(int val)
{
  int idx;
  int i;
  int num;
  if(val>=0){
    num = val;
    putc('+');
  }else{
    num = -val;
    putc('-');
  }
  
  putc('0');
  int outNum;
  int flg = 0;
  for(idx = 0; idx < 10 ; idx++)
  {
    outNum = 0;
    int CmpNum = num10s[idx];
    for(i = 0; i < 10 ; i++)
    {
      if(num>=CmpNum)
      {
        num -= CmpNum;
        outNum++;
      }
      else
      {
        break;
      }
    }
    if(flg==1){
      putc('0' + outNum);
    }else{
      if(outNum!=0){
        putc('0' + outNum);
        flg = 1;
      }
    }
  }
}

void printInt(int val, int newline){
  printInt(val);
  putc('\r');
  putc('\n');
}

int scanInt(){
  char buff[10];
  int i;
  int j;
  int idx = 0;
  int neg = 0;
  char v;
  while(1){
    v = getc();
    if(v=='-'||(v>='0'&&v<='9')){
      break;
    }
  }
  for(i = 0; i < 10 ; i++)
  {
    if(i==0 && v=='-'){
      neg = 1;
    }else{
      if(v<'0'||v>'9'){
        break;
      }
      buff[idx++] = v;
    }
    v = getc();
  }
  
  int val = 0;
  
  for(i = 0; i < idx ; i++)
  {
    for(j = 0; j < (int)(buff[i]-'0') ; j++)
    {
      val+=num10s[10-idx+i];
    }
  }
  if(neg){
    val=-val;
  }
  return val;
}

