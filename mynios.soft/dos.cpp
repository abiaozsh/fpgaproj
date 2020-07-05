
#include "inc/io.h"
#include "inc/system.h"
#include "inc/irq.h"
#include "inc/system.cpp"
//#include "inc/uart.cpp"
//#include "inc/uartio.cpp"
#include "inc/keyScreen.cpp"
#include "inc/print.cpp"
#include "inc/spi.cpp"
#include "inc/Sd2Card.cpp"
#include "inc/FileSystem.cpp"



void initDisk(Sd2Card** sdcard,int cardidx, SdVolume** sdvolumes, int* totalVolume){
  int res;
  res = sdcard[cardidx]->init(cardidx);
  if(res){
    print("found card ");printInt(cardidx);print("\r\n");
    SdVolume* tempVol = (SdVolume*)malloc(sizeof(SdVolume));
    
    res = tempVol->init(sdcard[cardidx], 0);
    if(res){
      print("found volume 0\r\n");
      sdvolumes[*totalVolume] = tempVol;
      *totalVolume = *totalVolume + 1;
      return;
    }else{
      res = tempVol->init(sdcard[cardidx], 1);
      if(res){
        print("found volume 1\r\n");
        sdvolumes[*totalVolume] = tempVol;
        *totalVolume = *totalVolume + 1;
        return;
      }
    }
    //mfree(sizeof(SdVolume));
  }else{
    print("card error ");printInt(cardidx);print("\r\n");
  }
}


int main(){
  malloc_index = 0;
  stdioInit(1024);
  print("Hello from My DOS 1.0\r\n");

  Sd2Card* sdcards[3];
  sdcards[0] = (Sd2Card*)malloc(sizeof(Sd2Card));
  sdcards[1] = (Sd2Card*)malloc(sizeof(Sd2Card));
  sdcards[2] = (Sd2Card*)malloc(sizeof(Sd2Card));
  SdVolume* sdvolumes[26];
  int totalVolume = 0;
  SdVolume* currVolume;
  int icurrVolume;
  initDisk(sdcards, 0, sdvolumes, &totalVolume);
  initDisk(sdcards, 1, sdvolumes, &totalVolume);
  initDisk(sdcards, 2, sdvolumes, &totalVolume);
  int* hzkPtr = (int*)0;
  int res;
  for(int i=0;i<totalVolume;i++){
    sdvolumes[i]->root = (SdFile*)malloc(sizeof(SdFile));
    res = sdvolumes[i]->root->openRoot(sdvolumes[i]);
    if(res){
      print("found root");printInt(i);print("\r\n");
      //sdvolumes[i]->root->dirList();
    }else{
      print("root error");printInt(i);print("\r\n");
    }
  }
  currVolume = sdvolumes[0];
  icurrVolume = 0;

  SdFile* file = (SdFile*)malloc(sizeof(SdFile));//at8M~
  

  while(1){
    print("vol:");printInt(icurrVolume);
    print(" cmd?\r\n");
    char str[10];
    int res;
    scan(str,-1);
    print(str);
    
    if(equal(str,"clr",-1)){
      stdioInit(1024);
    }

    if(equal(str,"vol",-1)){
      print("which volume?");printInt(totalVolume);print("\r\n");
      int v = scanInt();
      icurrVolume = v;
      currVolume = sdvolumes[v];
      print("curr volume:");printInt(v);print("\r\n");
    }

    if(equal(str,"dir",-1)){
      currVolume->root->dirList();
    }

    if(equal(str,"print",-1)){
      print("which file?\r\n");
      char filename[12];
      scan(filename,-1);
      print("[");print(filename);print("]");
      res = file->open(currVolume->root, filename, O_READ);
      if(res){
        print("open ok\r\n");
        printInt(file->fileSize_);print("\r\n");
        for(int i=0;i<file->fileSize_;i++){
          int val = file->read();
          putc(val);
        }
      }else{
        print("open ng\r\n");
        printInt(file->fileError);print("\r\n");
      }
    }
    
    #ifdef hasGB
    if(equal(str,"printgb",-1)){
      printgb("ÎÄ¼þÃû?\r\n");
      char filename[12];
      scan(filename,-1);
      print("[");print(filename);print("]");
      res = file->open(currVolume->root, filename, O_READ);
      if(res){
        print("open ok\r\n");
        printInt(file->fileSize_);print("\r\n");
        int lastVal = 0;
        int idx = 0;
        while(1){
          int val = file->read();
          if(idx!=0){
            int tmp1 = lastVal&0xFF;
            int tmp2 = val    &0xFF;
            if(tmp1>=0xA0 && tmp2>=0xA0){
              int i = tmp1 - 0xA0;
              int j = tmp2 - 0xA0;
              #ifdef putcgb
              #endif
              putcgb(94 * (i - 1) + (j - 1));
              val = file->read();
              lastVal = val;
              //printByte(i);
              //printByte(j);
              idx++;
            }else{
              putc(tmp1);
              lastVal = val;
            }
            if(idx>file->fileSize_){
              break;
            }
          }
          idx++;
        }
      }else{
        print("open ng\r\n");
        printInt(file->fileError);print("\r\n");
      }
    }
    #endif
    
    int buff[128];
    
    if(equal(str,"loadimg",-1)){
      print("which file?\r\n");
      char filename[12];
      scan(filename,-1);
      res = file->open(currVolume->root, filename, O_READ);
      if(res){
        print("open ok\r\n");
        for(int i=0;i<0x200000/4;i+=128){
          file->read((char*)buff,512);
          for(int j=0;j<128;j++){
            ((int*)(0x200000))[i+j] = buff[j];//at 2Mbyte
          }
        }
        scan(filename,-1);
      }else{
        print("open ng\r\n");
        printInt(file->fileError);print("\r\n");
      }
    }
    
    #ifdef hasGB
    if(equal(str,"ttt",-1)){
      print("i:");
      int i = scanInt();;
      print("j:");
      int j = scanInt();;
      putcgb(94 * (i - 1) + (j - 1));
    }
    
    if(equal(str,"loadhzk",-1)){
      if(hzkPtr){
        print("loaded\r\n");
      }else{
        res = file->open(currVolume->root, "HZK16", O_READ);
        //16*16bit / word
        //32byte / word
        //8363 word
        //522.6875 page
        //#ifdef setGbCharTable
        hzkPtr = (int*)malloc(8363*32);
        setGbCharTable(hzkPtr);
        //#endif
        if(res){
          print("open ok\r\n");
          for(int i=0;i<523*128;i+=128){
            file->read((char*)buff,512);
            for(int j=0;j<128;j++){
              hzkPtr[i+j] = buff[j];//at 2Mbyte
            }
          }
        }else{
          print("open ng\r\n");
          printInt(file->fileError);print("\r\n");
        }
      }
    }
    #endif
    
    if(equal(str,"run",-1)){
      print("which file?\r\n");
      char filename[12];
      scan(filename,-1);
      res = file->open(currVolume->root, filename, O_READ);
      if(res){
        print("open ok:");printInt(file->fileSize_);print("\r\n");
        
        char* prog_data = (char*)malloc(file->fileSize_);
        
        //int (*prog)(void) = (int(*)(void))prog_data;

        for(int i=0;i<file->fileSize_;i++){
          int val = file->read();
          prog_data[i] = val;
          if(i & 511==0){
            print("loading:");printInt(i);print("\r\n");
          }
        }

        setOff((int)prog_data);
        jmp(0x80000000);
        
      }else{
        print("open ng\r\n");
        printInt(file->fileError);print("\r\n");
      }
    }
    
    
    if(equal(str,"del",-1)){
      print("name?\r\n");
      char filename[12];
      scan(filename,-1);
      res = file->del(currVolume->root, filename);
      if(res){
        print("del ok\r\n");
      }else{
        print("del ng\r\n");
        print("file             ");printInt((int)file);print("\r\n");
        print("currVolume->root ");printInt((int)currVolume->root);print("\r\n");
        
        print("file->delError             ");printInt(file->delError);print("\r\n");
        print("file->removeError          ");printInt(file->removeError);print("\r\n");
        print("file->truncateError        ");printInt(file->truncateError);print("\r\n");
        print("file->vol_->freeChainError ");printInt(file->vol_->freeChainError);print("\r\n");
        print("file->vol_->fatPutError    ");printInt(file->vol_->fatPutError);print("\r\n");
      }
    }

    if(equal(str,"write",-1)){
      print("filename?\r\n");
      char filename[12];
      scan(filename,-1);
      res = file->open(currVolume->root, filename, O_CREAT | O_WRITE | O_APPEND);
      if(res){
        print("open ok\r\n");
        while(1){
          int val = getc();
          file->write(val);
          putc(val);
          if(val==0){
            break;
          }
        }
        res = file->close();
        currVolume->root->sync();
        if(res){
          print("close ok\r\n");
        }else{
          print("close ng\r\n");
        }
      }else{
        print("open ng\r\n");
        printInt(file->fileError);print("\r\n");
      }
    }

    if(equal(str,"upload",-1)){
      print("filename?\r\n");
      char filename[12];
      scan(filename,-1);
      print(filename);
      print("length?\r\n");
      int len = scanInt();
      printInt(len);
      res = file->open(currVolume->root, filename, O_CREAT | O_WRITE | O_APPEND);
      if(res){
        print("open ok\r\n");
        for(int i=0;i<len;i++){
          int val = getc();
          file->write(val);
          putc(val);
        }
        res = file->close();
        currVolume->root->sync();
        if(res){
          print("close ok\r\n");
        }else{
          print("close ng\r\n");
        }
      }else{
        print("open ng\r\n");
        printInt(file->fileError);print("\r\n");
      }
    }

    //if(equal(str,"mkdir",-1)){
    //  print("dir name?\r\n");
    //  char filename[12];
    //  scan(filename,-1);
    //  res = file->makeDir(currVolume->root, filename);
    //  if(res){
    //    print("mkdir ok\r\n");
    //  }else{
    //    print("mkdir ng\r\n");
    //  }
    //}


  }
}
