//中断向量
//浮点数
/*
push A
push B
pushadd B
pushsub B

getHmul
getLmul
getHadd
getLadd
getHsub
getLsub
*/

//#define DEBUG

#include "inc/io.h"
#include "inc/system.h"
#include "inc/irq.h"
#include "inc/system.cpp"
//#include "inc/uart.cpp"
//#include "inc/uartio.cpp"
#include "inc/keyScreen.cpp"
#include "inc/print.cpp"
#include "inc/soft-fp/soft-fp.h"


int screenInit2(int screenBase){
  IOWR(VGA, VGA_BASE, screenBase);//1024=2Mbyte
  int screen_base = screenBase*2048;
  for(int i=0;i<0x80000;i++){
    ((int*)(screen_base))[i] = 0;//at 2Mbyte
  }
}
int getpixel(int r, int g, int b)
{
  int val = 0;
  val += (r >> 3) << (5 + 6);
  val += (g >> 2) << (5);
  val += (b >> 3);
  return val;
}

//MaxIterations maxi = 256
//maxm=4
void f(int wid, int hei, double viswid, double vishei, double visx, double visy, int maxi, double maxm)
{
  int buffAddr = 0x200000;

	int i;
	int j;
	int index = 0;
	int clr;
	double ReaC;
	double ImaC;
	double dReaC;
	double dImaC;
	double ReaZ;
	double ImaZ;
	double ReaZ2;
	double ImaZ2;
	int n;

	n = 256 / maxi;

	dReaC = (viswid) / ((double)wid - 1);
	dImaC = (vishei) / ((double)hei - 1);

	for (j = 0; j < hei; j++)
	{
    short* addr;
		for (i = 0; i < wid; i++)
		{
      addr = (short*)(&((short*)(buffAddr))[i+(j<<10)]);
      *addr = getpixel(0, 255, 0);//at 2Mbyte
		}
    flushCache(addr);
	}

	ReaC = visy;
	for (j = 0; j < hei; j++)
	{
		ReaC += dReaC;
		ImaC = visx;
		for (i = 0; i < wid; i++)
		{
			ImaC += dImaC;
			ReaZ = 0;
			ImaZ = 0;
			ReaZ2 = 0;
			ImaZ2 = 0;
			clr = 1;

			while (clr++ < maxi && ReaZ2 + ImaZ2 < maxm)
			{
				ReaZ2 = ReaZ * ReaZ;
				ImaZ2 = ImaZ * ImaZ;
				ImaZ = 2 * ImaZ * ReaZ + ImaC;
				ReaZ = ReaZ2 - ImaZ2 + ReaC;
			}
      short* addr = (short*)(&((short*)(buffAddr))[i+(j<<10)]);
      *addr = getpixel(0, 0, clr * n);//at 2Mbyte
      flushCache(addr);
		}
	}
}

int main()
{
  f(256, 192 , 2, 2, -1, -1, 256, 4);
  while(1){
  }

  return 0;
  
}
