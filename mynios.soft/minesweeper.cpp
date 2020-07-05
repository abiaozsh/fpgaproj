#include "inc/io.h"
#include "inc/irq.h"
#include "inc/system.h"
#include "inc/key.h"
#include "inc/system.cpp"
#include "inc/uart.cpp"
#include "inc/uartio.cpp"
#include "inc/print.cpp"
#include "inc/spi.cpp"
#include "inc/Sd2Card.cpp"
#include "inc/FileSystem.cpp"

volatile int timeon;
volatile int hidon;
volatile int hid_value;

void irq_proc(){
  int irq = IORD(IRQ, 0);
  if(irq&1){
    timeon = 1;
  }
  if(irq&4){
    hid_value = IORD(HID, 0);
    hidon = 1;
  }
  IOWR(IRQ, 0, 1+2+4);
}

int currBuff;
int buffAddr;
int drawImg(int basex, int basey, int x, int y, int w, int h, short* block){
  int bx = (basex + x) / 2;
  int by = basey + y;
  int* src = (int*)block;
  for(int j=0;j<h;j++){
    int j_temp = j*w/2;
    int* addr;
    for(int i=0;i<w/2;i++){
      int x = bx + i;
      int y = by + j;
      addr = (int*)(&((int*)(buffAddr))[x+(y<<9)]);
      *addr = src[i+j_temp];//at 2Mbyte
    }
    flushCache(addr);
  }
}
int paint(int basex, int basey, int x, int y, int w, int h, short color){
  int bx = basex + x;
  int by = basey + y;
  for(int j=0;j<h;j++){
    int j_temp = j*w;
    short* addr;
    for(int i=0;i<w;i++){
      int x = bx + i;
      int y = by + j;
      addr = (short*)(&(((short*)buffAddr)[x+(y<<10)]));
      *addr = color;//at 2Mbyte
    }
    flushCache(addr);
  }
}
int getColorRGB(int r, int g, int b)
{
  int val = 0;
  val += (r >> 3) << (5 + 6);
  val += (g >> 2) << (5);
  val += (b >> 3);
  return val;
}

int loadImg(SdFile* file, SdVolume* currVolume, char* filename, char* arr){
  int res = file->open(currVolume->root, filename, O_READ);
  if(res){
    print("open ok\r\n");
    print(filename);
    print("\r\n");
    printInt(file->fileSize_);print("\r\n");
    for(int i=0;i<file->fileSize_;i++){
      arr[i] = file->read();
    }
  }else{
    print(filename);print(" open ng\r\n");
    printInt(file->fileError);print("\r\n");
  }
}

int screenInit2(int screenBase){
  IOWR(VGA, VGA_MODE, 0);//640 * 480
  IOWR(VGA, VGA_BASE, screenBase);//1024=2Mbyte
  buffAddr = screenBase*2048;
  for(int i=0;i<0x80000;i++){
    ((int*)(buffAddr))[i] = 0;//at 2Mbyte
  }
}


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


//---------------------------------------------------------------------------------------------------

int autoLevel; //0 no open, 1 open empty, 2: ai

int boardW = 30;
int boardH = 16;


int* mineBoard;
int* status;//0x00:close  0x10:open  0x01:flag mark  0x02:question mark

int isFailed;
int isWined;

int basex;
int basey;

int framex;
int framey;

int btnx;
int btny;

int lv1x;
int lv1y;
int lv2x;
int lv2y;
int lv3x;
int lv3y;

short* imgArr;
short* num;
short* ma;//flag
short* mb;//question mark
short* mc;//red mine
short* md;//x mine
short* me;//mine

short* mi;
short* mj;
short* mm;
short* mo;
short* mt;

int n = 20;
int time = 0;


int loadCur(SdFile* file, SdVolume* currVolume){
  short buff[32*32];
  loadImg(file, currVolume, "cur.img", (char*)buff);

  for(int j=0;j<32;j++){
    for(int i=0;i<32;i++){
      IOWR(VGA, (VGA_CURSOR_DATA+i+j*32), buff[i*32+j]);
    }
  }
}

int drawBG(){
  paint(framex, framey, 0+502-3, 0, 3, 322, getColorRGB(255, 255, 255));
  paint(framex, framey, 0, 0+322-3, 502, 3, getColorRGB(255, 255, 255));
  paint(framex, framey, 0, 0, 502, 3, getColorRGB(255, 255, 255));
  paint(framex, framey, 0, 0, 3, 322, getColorRGB(255, 255, 255));
  paint(framex, framey, 3, 3, 502-6, 322-6, getColorRGB(192, 192, 192));
  paint(framex, framey, 9, 9, 486, 2, getColorRGB(128, 128, 128));
  paint(framex, framey, 9, 9, 2, 37, getColorRGB(128, 128, 128));
  paint(framex, framey, 9+486-2, 9, 2, 37, getColorRGB(255, 255, 255));
  paint(framex, framey, 9, 9+37-2, 486, 2, getColorRGB(255, 255, 255));
  
  paint(framex, framey, 9, 53, 485, 2, getColorRGB(128, 128, 128));
  paint(framex, framey, 9, 53, 2, 261, getColorRGB(128, 128, 128));

  paint(framex, framey, 9+485-2, 53, 2, 261, getColorRGB(255, 255, 255));
  paint(framex, framey, 9, 53+261-2, 485, 2, getColorRGB(255, 255, 255));

}

void drawNum(int bx, int by, int val){
  int a;
  a = val % 10;
  val = val / 10;
  drawImg(bx+12*2, by, 0, 0, 12, 23, &num[a*12*23]);
  a = val % 10;
  val = val / 10;
  drawImg(bx+12*1, by, 0, 0, 12, 23, &num[a*12*23]);
  a = val % 10;
  val = val / 10;
  drawImg(bx+12*0, by, 0, 0, 12, 23, &num[a*12*23]);

}

void timing() {
  if(isFailed || isWined)return;
  time++;
  drawNum(framex+448, framey+16, time);
}



int inRange(int x, int y){
  if(x>=0 && x<boardW && y>=0 && y<boardH){
    return 1;
  }else{
    return 0;
  }
}

int getMine(int x, int y){
  if(inRange(x, y)){
    return mineBoard[x+y*boardW];
  }else{
    return 0;
  }
}

int getCount(int x, int y){
  int cnt = 0;
  cnt = cnt + getMine((x+1),(y+1));
  cnt = cnt + getMine((x  ),(y+1));
  cnt = cnt + getMine((x-1),(y+1));
  cnt = cnt + getMine((x+1),(y  ));
  cnt = cnt + getMine((x  ),(y  ));
  cnt = cnt + getMine((x-1),(y  ));
  cnt = cnt + getMine((x+1),(y-1));
  cnt = cnt + getMine((x  ),(y-1));
  cnt = cnt + getMine((x-1),(y-1));
  return cnt;
}

int getStatus(int x, int y){
  return status[x+y*boardW];
}
void setStatus(int x, int y, int val){
  status[x+y*boardW] = val;
}


int isWin(){
  int notyet=0;
  
  for(int j=0;j<boardH;j++){
    for(int i=0;i<boardW;i++){
      if(getMine(i, j)==0){
        if(getStatus(i, j)==0x10){//0x00:close  0x10:open  0x01:flag mark  0x02:question mark
        }else{
          notyet = 1;
        }
      }
    }
  }
  
  if(!notyet){
    return 1;
  }else{
    return 0;
  }
}


void finish(){
  for(int j=0;j<boardH;j++){
    for(int i=0;i<boardW;i++){
      if(getStatus(i, j)==0x01 && getMine(i, j)==0){//0x00:close  0x10:open  0x01:flag mark  0x02:question mark
        drawImg(basex, basey, i*16, j*16, 16, 16, md);//x mine
      }
      if(getMine(i, j)==1){
        drawImg(basex, basey, i*16, j*16, 16, 16, me);//mine
      }
    }
  }
  
  isFailed = true;
}

void open(int x, int y){
  if(inRange(x, y)){
    if(getStatus(x, y)==0){//0x00:close  0x10:open  0x01:flag mark  0x02:question mark
      setStatus(x, y, 0x10);
      if(isWin()){
        isWined = 1;
        finish();
        drawImg(btnx, btny, 0, 0, 26, 26, mm);
      }else{
        if(getMine(x, y)){
          finish();
          drawImg(btnx, btny, 0, 0, 26, 26, mj);
          drawImg(basex, basey, x*16, y*16, 16, 16, mc);//red mine
        }else{
          int cnt = getCount(x, y);
          drawImg(basex, basey, x*16, y*16, 16, 16, &imgArr[cnt*16*16]);
          if(cnt==0){
            open(x+1, y+1);
            open(x  , y+1);
            open(x-1, y+1);
            open(x+1, y  );
            //open(x  , y  );
            open(x-1, y  );
            open(x+1, y-1);
            open(x  , y-1);
            open(x-1, y-1);
          }
        }
      }
      //AI
    }
  }
}

int downx;
int downy;
int down;
int mineCount;
void onclick(int x, int y, int b1, int b2){
  if(isFailed || isWined)return;
  x = x / 16;
  y = y / 16;
  if(b1){
    downx = x;
    downy = y;
    down = 1;
    if(getStatus(x, y)==0){
      drawImg(basex, basey, x*16, y*16, 16, 16, &imgArr[0*16*16]);
    }
  }else{
    
    if(down==1){
      open(downx, downy);
      down = 0;
    }
  }
  
  if(b2){
    int status = getStatus(x, y);//0x00:close  0x10:open  0x01:flag mark  0x02:question mark
    if(status==0){
      setStatus(x, y, 0x01);
      drawImg(basex, basey, x*16, y*16, 16, 16, ma);//flag
      mineCount--;
    }
    if(status==0x01){
      setStatus(x, y, 0x02);
      drawImg(basex, basey, x*16, y*16, 16, 16, mb);//question mark
      mineCount++;
    }
    if(status==0x02){
      setStatus(x, y, 0x00);
      drawImg(basex, basey, x*16, y*16, 16, 16, &imgArr[9*16*16]);//block
    }
    
    drawNum(framex+18, framey+16, mineCount);

  }
}

void reset(){
  int total = boardW*boardH;
  isFailed = 0;
  isWined = 0;
  time = 0;
  mineCount = n;
  for(int i=0;i<total;i++){
    mineBoard[i] = 0;
  }
  for(int i=0;i<n;i++){
    mineBoard[i] = 1;
  }
  
  for(int i=0;i<total;i++){
    int pos = i+rnd(total-i);
    int tmp = mineBoard[pos];
    mineBoard[pos] = mineBoard[i];
    mineBoard[i] = tmp;
  }
  
  for(int i=0;i<total;i++){
    status[i] = 0;
  }

  drawImg(lv1x+0   , lv1y, 0, 0, 12, 23, &num[2*12*23]);
  drawImg(lv1x+12  , lv1y, 0, 0, 12, 23, &num[0*12*23]);
  drawImg(lv2x+0   , lv2y, 0, 0, 12, 23, &num[5*12*23]);
  drawImg(lv2x+12  , lv2y, 0, 0, 12, 23, &num[0*12*23]);
  drawImg(lv3x+0   , lv3y, 0, 0, 12, 23, &num[9*12*23]);
  drawImg(lv3x+12  , lv3y, 0, 0, 12, 23, &num[9*12*23]);

  drawNum(framex+448, framey+16, 0);

  //ok
  drawImg(btnx, btny, 0, 0, 26, 26, mi);
  
  for(int j=0;j<boardH;j++){
    for(int i=0;i<boardW;i++){
      drawImg(basex, basey, i*16, j*16, 16, 16, &imgArr[9*16*16]);
    }
  }
}


int main()
{
  print("Hello from minesweeper!\r\n");
  basex = 80;
  basey = 150;
  
  framex = basex-11;
  framey = basey-55;

  btnx = framex+240;
  btny = framey+15;

  lv1x = 200;
  lv1y = 50;
  lv2x = 300;
  lv2y = 50;
  lv3x = 400;
  lv3y = 50;


  mineBoard = (int*)malloc(boardW*boardH*4);
  status = (int*)malloc(boardW*boardH*4);

  imgArr = (short*)malloc(16*16*2 *10);
  ma = (short*)malloc(16*16*2);//flag
  mb = (short*)malloc(16*16*2);//question mark
  mc = (short*)malloc(16*16*2);//red mine
  md = (short*)malloc(16*16*2);//x mine
  me = (short*)malloc(16*16*2);//mine

  num = (short*)malloc(12*23*2 *10);

  mi = (short*)malloc(26*26*2);
  mj = (short*)malloc(26*26*2);
  mm = (short*)malloc(26*26*2);
  mo = (short*)malloc(26*26*2);
  mt = (short*)malloc(26*26*2);


  Sd2Card* sdcard = (Sd2Card*)malloc(sizeof(Sd2Card));
  SdVolume* sdvolume = (SdVolume*)malloc(sizeof(SdVolume));
  sdvolume->root = (SdFile*)malloc(sizeof(SdFile));
  SdFile* file = (SdFile*)malloc(sizeof(SdFile));

  int cs = 0;////////////////////////////////
  int res = sdcard->init(cs);
  if(res){
    print("sd ok\r\n");
    sdcard->printType();
    print("which partition?\r\n");
    int part = 1;//////////////////////////////////
    res = sdvolume->init(sdcard, part);
    if(res){
      print("volume ok\r\n");
      res = sdvolume->root->openRoot(sdvolume);

      if(res){
        print("root ok\r\n");
      }else{
        print("root ng\r\n");
      }
    }else{
      print("volume ng\r\n");
      printInt(sdvolume->error);
    }
  }else{
    print("sd ng\r\n");
    printInt(sdcard->errorCode());
  }

  if(res){
    loadImg(file, sdvolume,  "m0.img", (char*)(&imgArr[0*16*16]));
    loadImg(file, sdvolume,  "m1.img", (char*)(&imgArr[1*16*16]));
    loadImg(file, sdvolume,  "m2.img", (char*)(&imgArr[2*16*16]));
    loadImg(file, sdvolume,  "m3.img", (char*)(&imgArr[3*16*16]));
    loadImg(file, sdvolume,  "m4.img", (char*)(&imgArr[4*16*16]));
    loadImg(file, sdvolume,  "m5.img", (char*)(&imgArr[5*16*16]));
    loadImg(file, sdvolume,  "m6.img", (char*)(&imgArr[6*16*16]));
    loadImg(file, sdvolume,  "m7.img", (char*)(&imgArr[7*16*16]));
    loadImg(file, sdvolume,  "m8.img", (char*)(&imgArr[8*16*16]));
    loadImg(file, sdvolume,  "m9.img", (char*)(&imgArr[9*16*16]));
    //
    loadImg(file, sdvolume,  "ma.img", (char*)(ma));//flag
    loadImg(file, sdvolume,  "mb.img", (char*)(mb));//question mark
    loadImg(file, sdvolume,  "mc.img", (char*)(mc));//red mine
    loadImg(file, sdvolume,  "md.img", (char*)(md));//x mine
    loadImg(file, sdvolume,  "me.img", (char*)(me));//mine

    loadImg(file, sdvolume,  "n0.img", (char*)(&num[ 0*12*23]));
    loadImg(file, sdvolume,  "n1.img", (char*)(&num[ 1*12*23]));
    loadImg(file, sdvolume,  "n2.img", (char*)(&num[ 2*12*23]));
    loadImg(file, sdvolume,  "n3.img", (char*)(&num[ 3*12*23]));
    loadImg(file, sdvolume,  "n4.img", (char*)(&num[ 4*12*23]));
    loadImg(file, sdvolume,  "n5.img", (char*)(&num[ 5*12*23]));
    loadImg(file, sdvolume,  "n6.img", (char*)(&num[ 6*12*23]));
    loadImg(file, sdvolume,  "n7.img", (char*)(&num[ 7*12*23]));
    loadImg(file, sdvolume,  "n8.img", (char*)(&num[ 8*12*23]));
    loadImg(file, sdvolume,  "n9.img", (char*)(&num[ 9*12*23]));

    loadImg(file, sdvolume,  "mi.img", (char*)(mi));
    loadImg(file, sdvolume,  "mj.img", (char*)(mj));
    loadImg(file, sdvolume,  "mm.img", (char*)(mm));
    loadImg(file, sdvolume,  "mo.img", (char*)(mo));
    loadImg(file, sdvolume,  "mt.img", (char*)(mt));


    loadCur(file, sdvolume);
    
    screenInit2(1024);
    
    drawBG();
    reset();

    IOWR(MYTIMER, 4, 1000000);
    IOWR(MYTIMER, 0, 0);
    timeon = 0;
    hidon = 0;
    setIrq(irq_proc, 1);

    
    int oldbtn1 = 0;
    int oldbtn2 = 0;
    int oldbtn3 = 0;

    int x = 0;
    int y = 0;
    
    while(1){
      if(hidon){
        //hid_value = IORD(HID, 0);
        //if(hid_value){
        hidon = 0;
        if((hid_value & 0x0F000000) == 0x01000000){
          //key
          if(((hid_value & 0x00008000) != 0x00008000) && ((hid_value & 0x00800000) != 0x00800000)){
            int tmp = hid_value & 0xFF;
            if(tmp==KEY_ESC){
              //reset();
            }
          }
        }else{
          //mouse
          x+=(char)( hid_value     & 0x000000FF);
          y-=(char)((hid_value>>8) & 0x000000FF);
          int btn1 = hid_value & 0x00010000;
          int btn2 = hid_value & 0x00020000;
          int btn3 = hid_value & 0x00040000;//mid
          if(x<0){
            x=0;
          }
          if(x>640){
            x=640;
          }

          if(y<0){
            y=0;
          }
          if(y>480){
            y=480;
          }
          IOWR(VGA, VGA_CURSOR_X, x);
          IOWR(VGA, VGA_CURSOR_Y, y);
          
          if((oldbtn1 != btn1) || (oldbtn2 != btn2)){
            if(
              x>=basex && x<(basex+boardW*20) &&
              y>=basey && y<(basey+boardH*20)
              ){
              onclick(x-basex,y-basey,btn1,btn2);
            }
            
            if(
              x>=btnx && x<(btnx+26) &&
              y>=btny && y<(btny+26) && btn1
              ){
              reset();
            }
            if(
              x>=lv1x && x<(lv1x+28) &&
              y>=lv1y && y<(lv1y+23) && btn1
              ){
              n=20;
              reset();
            }
            if(
              x>=lv2x && x<(lv2x+28) &&
              y>=lv2y && y<(lv2y+23) && btn1
              ){
              n=50;
              reset();
            }
            if(
              x>=lv3x && x<(lv3x+28) &&
              y>=lv3y && y<(lv3y+23) && btn1
              ){
              n=99;
              reset();
            }
          }
          oldbtn1 = btn1;
          oldbtn2 = btn2;
          oldbtn3 = btn3;
        }
      }
      if(timeon){
        IOWR(MYTIMER, 0, 0);
        timeon = 0;
        timing();
      }
    }
  }else{
    while(1);
  }
  return 0;
}
