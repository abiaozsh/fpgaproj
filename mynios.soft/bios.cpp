#include "inc/io.h"
#include "inc/system.h"
#include "inc/irq.h"
//#include "inc/uart.cpp"
//#include "inc/uartio.cpp"

void abort(void)
{
  while(1);
}

void jmp(int val){
  __asm__ __volatile__("jmp %[input1]"::[input1]"r"(val));
}

void setOff(int val){
  __asm__ __volatile__("stoff %[input1]"::[input1]"r"(val));
}

void* __eof__();
int malloc_index = 0;
void* malloc(int size){
  size = (size & (~0x03))+4;
  int idx = 0x40000;
  idx += malloc_index;
  malloc_index += size;
  return (void*)idx;
}

void mfree(int size){
  malloc_index -= size;
}

void flushCache(void* addr){
  int tmp = (int)addr;
  tmp = tmp >> 10;
  int status = sti(0);
  IOWR(CACHE_CTL, 0, 0x80000000 | tmp);
  volatile int a = *((int*)addr);
  IOWR(CACHE_CTL, 0, 0);
  if(status){
    sti(1);
  }
}

void memcpy(void *dst, const void *src, int len){
  for(int i=0;i<len;i++){
    ((char*)(dst))[i] = ((char*)(src))[i];
  }
}

#include "inc/keyScreen.cpp"
#include "inc/print.cpp"
#include "inc/spi.cpp"
#include "inc/Sd2CardROM.cpp"
#include "inc/FileSystemROM.cpp"

int main(){
  malloc_index = 0;
  stdioInit(1024);
  print("Hello from BIOS 1.0\r\n");
  print("press any key to load OS\r\n");
  getc();
  Sd2Card* sdcard = (Sd2Card*)malloc(sizeof(Sd2Card));
  SdVolume* sdvolume = (SdVolume*)malloc(sizeof(SdVolume));
  SdFile* file = (SdFile*)malloc(sizeof(SdFile));
  
  sdvolume->root = (SdFile*)malloc(sizeof(SdFile));
  
  int res = sdcard->init(0);
  int ok = 0;
  if(res){
    print("found sdcard 0\r\n");
    res = sdvolume->init(sdcard, 0);
    if(res){
      print("found volume 0\r\n");
      res = sdvolume->root->openRoot(sdvolume);
      if(res){
        print("found root\r\n");
        ok = 1;
      }else{
        print("root error\r\n");
      }
    }else{
      res = sdvolume->init(sdcard, 1);
      if(res){
        print("found volume 1\r\n");
        res = sdvolume->root->openRoot(sdvolume);
        if(res){
          print("found root\r\n");
          ok = 1;
        }else{
          print("root error\r\n");
        }
      }
    }
  }else{
    print("card error 0\r\n");
  }

  if(ok){
    res = file->open(sdvolume->root, "boot.bin", O_READ);
    if(res){
      print("open ok:");printInt(file->fileSize_);print("\r\n");
      
      char* prog_data = (char*)(0);
      
      //int (*prog)(void) = (int(*)(void))prog_data;

      print("start loading\r\n");
      for(int i=0;i<file->fileSize_;i++){
        int val = file->read();
        prog_data[i] = val;
        if((i & 1023)==0){
          putc('.');
        }
      }
      print("start\r\n");
      //asm("hlt 1");
      jmp(0);
      
    }else{
      print("open ng\r\n");
      printInt(file->fileError);print("\r\n");
    }
    
  }else{
      print("can not found volume\r\n");
  }
  
  while(1);
}
