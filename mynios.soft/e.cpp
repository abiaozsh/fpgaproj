#include "inc/io.h"
#include "inc/irq.h"
#include "inc/system.h"
#include "inc/system.cpp"
#include "inc/irq.h"
#include "inc/uart.cpp"
#include "inc/uartio.cpp"
#include "inc/print.cpp"

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


volatile int hid_value;
volatile int hid_arrived;

int skip;
void irq_proc(){
  int irq = IORD(IRQ, 0);
  //putc('(');
  if(irq&1){
    //timer
    //putc('_');
    IOWR(IRQ, 0, 1);
  }
  if(irq&2){
    int tmp = IORD(MYUART, 0);
    //putc(tmp);
    IOWR(IRQ, 0, 2);
  }
  if(irq&4){
    int tmp = IORD(HID, 0);
    hid_value = tmp;
    hid_arrived = 1;
    //putc('f');
    IOWR(IRQ, 0, 4);
  }
  //putc(')');
}

int screenInit2(int screenBase){
  IOWR(VGA, VGA_BASE, screenBase);//1024=2Mbyte
  int screen_base = screenBase*2048;
  for(int i=0;i<0x80000;i++){
    ((int*)(screen_base))[i] = 0;//at 2Mbyte
  }
}
//鼠标箭头 加到vga

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
    for(int i=0;i<10;i++){
      int x = basex + i;
      int y = basey + j;
    }
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

int main(){
  
  //IOWR(MYTIMER, 4, 1000000);
  //IOWR(MYTIMER, 0, 0);
  setIrq(irq_proc ,1);
  
  screenInit2(1024);
  int buffAddr = 0x200000;

  for(int j=0;j<32;j++){
    for(int i=0;i<32;i++){
      if(j<16 && i < 16){
        if(i>j){
          IOWR(VGA, (VGA_CURSOR_DATA+i+j*32), 0x00FF);
        }else if(i<j){
          IOWR(VGA, (VGA_CURSOR_DATA+i+j*32), 0xFF00);
        }else{
          IOWR(VGA, (VGA_CURSOR_DATA+i+j*32), 0x0001);
        }
      }
      else{
        IOWR(VGA, (VGA_CURSOR_DATA+i+j*32), 0x0000);
      }
    }
  }

  //print("Hello from video demo\r\n");
  int x = 0;
  int y = 0;
  while(1){
    if(hid_arrived){
      hid_arrived = 0;
      if((hid_value & 0x0F000000) == 0x01000000){
        //key
        if(((hid_value & 0x00008000) != 0x00008000) && ((hid_value & 0x00800000) != 0x00800000)){
          int tmp = hid_value & 0xFF;
          putc(PS2Keymap_US[tmp]);
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
        if(x>1024){
          x=1024;
        }

        if(y<0){
          y=0;
        }
        if(y>768){
          y=768;
        }
        IOWR(VGA, VGA_CURSOR_X, x);
        IOWR(VGA, VGA_CURSOR_Y, y);
        if(btn1 || btn2 || btn3){
          printInt(x);putc(' ');printInt(y);putc('\r');putc('\n');
          for(int j=0;j<16;j++){
            for(int i=0;i<16;i++){
              short* addr = (short*)(&((short*)(buffAddr))[x+i+((y+j)<<10)]);
              if(i==j){
                *addr = 0;//at 2Mbyte
              }else{
                *addr = getpixel(btn1?0xFF:0, btn2?0xFF:0, btn3?0xFF:0);//at 2Mbyte
              }
              flushCache(addr);
            }
          }
        }
        //printInt(x);
        //print(" ");
        //printInt(y);
        //print("\r\n");

      }
      //printHex(hid_value);
      
      
      
      //putc('\r');
      //putc('\n');
    }
    
    
  };
}
