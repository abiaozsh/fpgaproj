#include "inc/io.h"
#include "inc/irq.h"
#include "inc/system.h"
#include "inc/system.cpp"
#include "inc/uart.cpp"
#include "inc/uartio.cpp"
#include "inc/print.cpp"
#include "inc/spi.cpp"
#include "inc/Sd2Card.cpp"
#include "inc/FileSystem.cpp"

//"use strict";
// need
// 8位计算能力 + - 移位
// 3k rom 512 ram
// keyboard/joystick input
// display
// random
// timer

int _block[] = {//
	0x070, 0x034, 0x038, 0x06C,//
	0x050, 0x021, 0x022, 0x043,//
	0x070, 0x034, 0x038, 0x06C,//
	0x050, 0x021, 0x022, 0x043,// ;1 长条
	0x0B0, 0x091, 0x084, 0x0A5,//
	0x0B0, 0x091, 0x084, 0x0A5,//
	0x0B0, 0x091, 0x084, 0x0A5,//
	0x0B0, 0x091, 0x084, 0x0A5,// ;2 方
	0x071, 0x0B4, 0x0A5, 0x068,//
	0x050, 0x091, 0x085, 0x046,//
	0x071, 0x0B4, 0x0A5, 0x068,//
	0x050, 0x091, 0x085, 0x046,// ;3 Z
	0x070, 0x084, 0x095, 0x069,//
	0x0B1, 0x042, 0x054, 0x0A5,//
	0x070, 0x084, 0x095, 0x069,//
	0x0B1, 0x042, 0x054, 0x0A5,// ;4 反Z
	0x070, 0x034, 0x088, 0x049,//
	0x072, 0x054, 0x025, 0x0A6,//
	0x050, 0x091, 0x035, 0x069,//
	0x0B0, 0x021, 0x042, 0x064,// ;5 反L
	0x0B0, 0x041, 0x034, 0x068,//
	0x070, 0x084, 0x025, 0x046,//
	0x071, 0x035, 0x058, 0x0A9,//
	0x050, 0x021, 0x092, 0x066,// ;6 L
	0x070, 0x0E4, 0x045, 0x068,//
	0x071, 0x054, 0x0F5, 0x046,//
	0x071, 0x054, 0x0C5, 0x069,//
	0x050, 0x0D1, 0x042, 0x065 // ;7 T
	};
int _chg[] = {0x000, 0x011, 0x045, 0x033, 0x041, 0x015, 0x066, 0x077, 0x068, 0x097, 0x0A6, 0x07B, 0x0C3, 0x09B, 0x03E, 0x0A8};

class Tetris {
  public:
	int Board[200];
	int PosX;
	int PosY;
	int NextShapeNo;
	int NowShapeNo;
	int NowDirectionNo;

  int (*CallBack_GetRandom)(void);
	//int CallBack_GetRandom(){
  //  return *rnd7;
	//};
  
  //int (*CallBack_DrawNextShape)(Tetris*);
	//int CallBack_DrawNextShape() {
  //  call GUI
	//},

	int Public_Init() {
		private_Clear();
		private_NextShape();
		private_NextShape();
	};
  
	int Public_Down() {
		PosY--;
		if (private_isTouch()) {
			PosY++;
			private_touchDo();
			return false;
		} else {
			return true;
		}
	};
  
	int Public_Move(int n) {
		PosX += n;
		if (private_isTouch()) {
			PosX -= n;
		}
	};
  
	int Public_Rotate() {
		NowDirectionNo = (NowDirectionNo + 1) & 3;
		if (private_isTouch()) {
			PosX--;
			if (private_isTouch()) {
				PosX--;
				if (private_isTouch()) {
					PosX--;
					if (private_isTouch()) {
						PosX += 3;
						NowDirectionNo = (NowDirectionNo + 3) & 3;
					}
				}
			}
		}
	};
  
	int private_touchDo() {
		// add to board
		for (int idx = 0; idx < 4; idx++) {
			int block = Public_GetBlock(NowShapeNo, NowDirectionNo, idx);
			int x = PosX + Public_GetBlockXout;
			int y = PosY - Public_GetBlockYout;
			private_setBoard(x, y, block);
		}

		// clear line
		for (int j = 19; j >= 0; j--) {
			int line = 1;
			for (int i = 0; i < 10; i++) {
				if (private_getBoard(i, j) == 0) {
					line = 0;
					break;
				}
			}
			if (line == 1) {
				for (int k = 0; k < 10; k++) {
					if (j - 1 >= 0) {
						int temp = private_getBoard(k, j - 1);
						temp = _chg[temp] & 0x0F;
						private_setBoard(k, j - 1, temp);
					}
					if (j + 1 < 20) {
						int temp = private_getBoard(k, j + 1);
						temp = (_chg[temp] >> 4) & 0x0F;
						private_setBoard(k, j + 1, temp);
					}
					private_setBoard(k, j, 0);
				}
				for (int k = j; k < 19; k++) {
					for (int l = 0; l < 10; l++) {
						private_setBoard(l, k, private_getBoard(l, k + 1));
					}
				}
			}
		}

		// is gameover
		for (int i = 0; i < 10; i++) {
			if (private_getBoard(i, 18) != 0) {
				private_Clear();
				break;
			}
		}
		private_NextShape();
	};
  
	int private_getBoard(int x, int y) {
		return Board[x + y * 10];
	};
  
	int private_setBoard(int x, int y, int val) {
		Board[x + y * 10] = val;
	};

	int Public_GetBlockXout;
	int Public_GetBlockYout;
	int Public_GetBlock(int ShapeNo, int DirectionNo, int idx) {
		int dat = _block[(ShapeNo << 4) + (DirectionNo << 2) + idx];
		Public_GetBlockYout = dat & 3;// (0000 00XX)
		dat >>= 2;
		Public_GetBlockXout = dat & 3;// (0000 XX00)
		dat >>= 2;
		return dat;// (xxxx 0000)
	};
  
	int private_isTouch() {
		for (int idx = 0; idx < 4; idx++) {
			int block = Public_GetBlock(NowShapeNo, NowDirectionNo, idx);
			int x = PosX + Public_GetBlockXout;
			int y = PosY - Public_GetBlockYout;
			if (y < 0 || y > 19 || x < 0 || x > 9) {
				return true;
			}
			if (private_getBoard(x, y) != 0) {
				return true;
			}
		}
		return false;
	};
  
	int private_NextShape() {
		NowShapeNo = NextShapeNo;
		NextShapeNo = CallBack_GetRandom();
		NowDirectionNo = 0;
		PosY = 19;
		PosX = 3;
	};
  
	int private_Clear() {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 20; j++) {
				private_setBoard(i, j, 0);
			}
		}
	}
};

int rnd7;
short* imgArr;
Tetris* tetris;

int CallBack_GetRandom() {
  return rnd7;//0~6
};

int currBuff;
int buffAddr;
int drawImg(int blockx, int blocky, short* block, int isNext){
  int basex = 50;
  int basey = 100;
  if(isNext){
    basex = 200;
  }
  basex = basex + blockx * 10;
  basey = basey + blocky * 20;
  int* src = (int*)block;
  for(int j=0;j<20;j++){
    int j_temp = j*10;
    int* addr;
    for(int i=0;i<10;i++){
      int x = basex + i;
      int y = basey + j;
      addr = (int*)(&((int*)(buffAddr))[x+(y<<9)]);
      *addr = src[i+j_temp];//at 2Mbyte
    }
    flushCache(addr);
  }
}
//int drawImg(int blockx, int blocky, short* block, int isNext){
//  int basex = 100;
//  int basey = 100;
//  if(isNext){
//    basex = 500;
//  }
//  basex = basex + blockx * 20;
//  basey = basey + blocky * 20;
//  for(int j=0;j<20;j++){
//    int j_temp = j*20;
//    for(int i=0;i<20;i++){
//      int x = basex + i;
//      int y = basey + j;
//      ((short*)(buffAddr))[x+(y<<10)] = block[i+j_temp];//at 2Mbyte
//    }
//  }
//}

#define PS2_TAB				9
#define PS2_ENTER			10
#define PS2_BACKSPACE			127
#define PS2_ESC				27
#define PS2_INSERT			0
#define PS2_DELETE			127
#define PS2_HOME			0
#define PS2_END				0
#define PS2_PAGEUP			25
#define PS2_PAGEDOWN			26
#define PS2_UPARROW			11
#define PS2_LEFTARROW			8
#define PS2_DOWNARROW			10
#define PS2_RIGHTARROW			21
#define PS2_F1				0
#define PS2_F2				0
#define PS2_F3				0
#define PS2_F4				0
#define PS2_F5				0
#define PS2_F6				0
#define PS2_F7				0
#define PS2_F8				0
#define PS2_F9				0
#define PS2_F10				0
#define PS2_F11				0
#define PS2_F12				0
#define PS2_SCROLL			0

int PS2Keymap_US[] = {
  // without shift
	0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
	0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '`', 0,
	0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
	0, 0, 'z', 's', 'a', 'w', '2', 0,
	0, 'c', 'x', 'd', 'e', '4', '3', 0,
	0, ' ', 'v', 'f', 't', 'r', '5', 0,
	0, 'n', 'b', 'h', 'g', 'y', '6', 0,
	0, 0, 'm', 'j', 'u', '7', '8', 0,
	0, ',', 'k', 'i', 'o', '0', '9', 0,
	0, '.', '/', 'l', ';', 'p', '-', 0,
	0, 0, '\'', 0, '[', '=', 0, 0,
	0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '\\', 0, 0,
	0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
	0, '1', 0, '4', '7', 0, 0, 0,
	'0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
	PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
	0, 0, 0, PS2_F7 };



int* lastBoard;
int* currBoard;
int* lastNext;
int* currNext;
void InitBoard() {
  lastBoard = (int*)malloc(200*4*2);
  currBoard = (int*)malloc(200*4);
  
  lastNext = (int*)malloc(16*4*2);
  currNext = (int*)malloc(16*4);
  for(int i=0;i<200*2;i++){
    lastBoard[i] = -1;
  }
  for(int i=0;i<16*2;i++){
    lastNext[i] = -1;
  }
}

void DrawBoard() {
  if(currBuff==0){
    buffAddr = 0x200000;
    currBuff = 1;
  }else{
    buffAddr = 0x400000;
    currBuff = 0;
  }

  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < 10; i++) {
      //ImgBoard[j][i].src = Tetris.Board[i * 20 + 19 - j] + ".bmp";
      int block = tetris->Board[i + (19 - j) * 10];
      currBoard[i+j*10] = block;
    }
  }
  
  for (int idx = 0; idx < 4; idx++) {
    int block = tetris->Public_GetBlock(tetris->NowShapeNo, tetris->NowDirectionNo, idx);
    int x = tetris->PosX + tetris->Public_GetBlockXout;
    int y = 19 - (tetris->PosY - tetris->Public_GetBlockYout);
    //ImgBoard[y][x].src = block + ".bmp";
    currBoard[x+y*10] = block;
  }
  
  int* lb = lastBoard;
  if(currBuff){
    lb+=200;
  }
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < 10; i++) {
      if(lb[i+j*10]!=currBoard[i+j*10]){
        lb[i+j*10] = currBoard[i+j*10];
        int block = lb[i+j*10];
        drawImg(i,j,&(imgArr[block*20*20]), false);
      }
    }
  }

  
  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      //ImgNext[j][i].src = "0.bmp";
      currNext[i+j*4] = 0;
    }
  }
  for (int idx = 0; idx < 4; idx++) {
    int block = tetris->Public_GetBlock(tetris->NextShapeNo, 0, idx);
    int i = tetris->Public_GetBlockXout;
    int j = tetris->Public_GetBlockYout;
    //ImgNext[j][i].src = block + ".bmp";
    currNext[i+j*4] = block;
  }
  
  int* ln = lastNext;
  if(currBuff){
    ln+=16;
  }
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 4; i++) {
      if(ln[i+j*4]!=currNext[i+j*4]){
        int block = ln[i+j*4] = currNext[i+j*4];
        drawImg(i,j,&(imgArr[block*20*20]), true);
      }
    }
  }

  if(currBuff){
    IOWR(VGA, VGA_BASE, 1024);
  }else{
    IOWR(VGA, VGA_BASE, 2048);
  }
}

void timing() {
	tetris->Public_Down();
	DrawBoard();
}

void keyDown(int k) {
	if (k == ' '){
		tetris->Public_Rotate();
	}else if (k == 'd'){
		tetris->Public_Move(1);
	}else if (k == 'a'){
		tetris->Public_Move(-1);
	}else if (k == 's'){
		tetris->Public_Down();
	}else if (k == 'x') {
		while (tetris->Public_Down())
			;
	}
	DrawBoard();
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
  IOWR(VGA, VGA_BASE, screenBase);//1024=2Mbyte
  int screen_base = screenBase*2048;
  for(int i=0;i<0x80000;i++){
    ((int*)(screen_base))[i] = 0;//at 2Mbyte
  }
}


volatile int timeon;
volatile int hidon;
volatile int hid_value;

void irq_proc(){
  int irq = IORD(IRQ, 0);
  if(irq&1){
    //timer
    timeon = 1;
    IOWR(IRQ, 0, 1);
    //putc('t');
  }
  if(irq&2){
    //int tmp = IORD(MYUART, 0);
    //IOWR(MYUART, 1 ,tmp);
    IOWR(IRQ, 0, 2);
  }
  if(irq&4){
    hid_value = IORD(HID, 0);
    hidon = 1;
    //putc('k');
    IOWR(IRQ, 0, 4);
  }
}



int main()
{
  malloc_index = 0;
  print("Hello from tetris!\r\n");
  

  imgArr = (short*)malloc(20*20*16*2);
  
  tetris = (Tetris*)malloc(sizeof(Tetris));
  Sd2Card* sdcard = (Sd2Card*)malloc(sizeof(Sd2Card));
  SdVolume* sdvolume = (SdVolume*)malloc(sizeof(SdVolume));
  sdvolume->root = (SdFile*)malloc(sizeof(SdFile));
  SdFile* file = (SdFile*)malloc(sizeof(SdFile));

  InitBoard();
  
  tetris->CallBack_GetRandom = CallBack_GetRandom;
  
  //tetris->CallBack_DrawNextShape = CallBack_DrawNextShape;

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
    loadImg(file, sdvolume,  "0.img", (char*)(&imgArr[ 0      ]));
    loadImg(file, sdvolume,  "1.img", (char*)(&imgArr[ 1*20*20]));
    loadImg(file, sdvolume,  "2.img", (char*)(&imgArr[ 2*20*20]));
    loadImg(file, sdvolume,  "3.img", (char*)(&imgArr[ 3*20*20]));
    loadImg(file, sdvolume,  "4.img", (char*)(&imgArr[ 4*20*20]));
    loadImg(file, sdvolume,  "5.img", (char*)(&imgArr[ 5*20*20]));
    loadImg(file, sdvolume,  "6.img", (char*)(&imgArr[ 6*20*20]));
    loadImg(file, sdvolume,  "7.img", (char*)(&imgArr[ 7*20*20]));
    loadImg(file, sdvolume,  "8.img", (char*)(&imgArr[ 8*20*20]));
    loadImg(file, sdvolume,  "9.img", (char*)(&imgArr[ 9*20*20]));
    loadImg(file, sdvolume, "10.img", (char*)(&imgArr[10*20*20]));
    loadImg(file, sdvolume, "11.img", (char*)(&imgArr[11*20*20]));
    loadImg(file, sdvolume, "12.img", (char*)(&imgArr[12*20*20]));
    loadImg(file, sdvolume, "13.img", (char*)(&imgArr[13*20*20]));
    loadImg(file, sdvolume, "14.img", (char*)(&imgArr[14*20*20]));
    loadImg(file, sdvolume, "15.img", (char*)(&imgArr[15*20*20]));
    
    screenInit2(1024);
    screenInit2(2048);

    tetris->Public_Init();
    DrawBoard();

    IOWR(MYTIMER, 4, 1000000);
    IOWR(MYTIMER, 0, 0);
    timeon = 0;
    hidon = 0;
    setIrq(irq_proc, 1);

    //1，去掉定时器，只有键盘鼠标，看下跑挂后的状态
    //2，去掉显示，只留uart输出，看下是否会跑挂
    while(1){
      rnd7++;
      if(rnd7==7){
        rnd7 = 0;
      }
      if(hidon){
      //hid_value = IORD(HID, 0);
      //if(hid_value){
        hidon = 0;
        
        if((hid_value & 0x0F000000) == 0x01000000){
          //key
          if(((hid_value & 0x00008000) != 0x00008000) && ((hid_value & 0x00800000) != 0x00800000)){
            int tmp = hid_value & 0xFF;
            int tmp2 = PS2Keymap_US[tmp];
            keyDown(tmp2);
          }
        }
      }
      if(timeon){
      //int time = IORD(MYTIMER, 0);
      //if(time>1000000){
        IOWR(MYTIMER, 0, 0);
        //print("time\r\n");
        timeon = 0;
        timing();
      }
    }
    
  }else{
    while(1);
  }
  return 0;
}
