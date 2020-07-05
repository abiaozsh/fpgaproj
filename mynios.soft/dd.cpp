//中断向量
//浮点数

#include "inc/io.h"
#include "inc/system.h"
#include "inc/system.cpp"
#include "inc/uart.cpp"
#include "inc/print.cpp"
#include "inc/spi.cpp"
#include "inc/Sd2Card.cpp"
#include "inc/FileSystem.cpp"



int main(){

  malloc_index = 0;

  //screenInit(1024);

  Sd2Card* sdcard = (Sd2Card*)malloc(sizeof(Sd2Card));//at8M
  SdVolume* sdvolume = (SdVolume*)malloc(sizeof(SdVolume));//at8M~
  sdvolume->root = (SdFile*)malloc(sizeof(SdFile));//at8M~
  SdVolume* currVolume;
  int res;

  SdFile* file = (SdFile*)malloc(sizeof(SdFile));//at8M~

  while(1){
    
    char str[10];
    int res;
    print("cmd?\r\n");
    scan(str,-1,-1);
    print(str);
    
    if(equal(str,"i",-1)){
      print("which sd?\r\n");
      int cs = scanInt();
      printInt(cs);
      res = sdcard->init(cs);
      if(res){
        print("sd ok\r\n");

        sdcard->printType();
        print("which partition?\r\n");
        int part = scanInt();
        printInt(part);
        res = sdvolume->init(sdcard, part);
        if(res){
          print("volume ok\r\n");
          res = sdvolume->root->openRoot(sdvolume);
          currVolume = sdvolume;
          if(res){
            print("root ok\r\n");
          }else{
            print("root ng\r\n");
          }
        }else{
          print("volume ng\r\n");
          printInt(sdvolume->initError);print(",");
          if(sdvolume->initError==4){
            sdvolume->printErrorSector();
          }
          //printInt(sdvolume->cacheBlockNumber_);
          //print("[");
          //for(int i=0;i<512;i++){
          //  printByte(sdvolume->cacheBuffer_.data[i]);
          //}
          //print("]");
        }
      }else{
        print("sd ng\r\n");
        print("sdcard->initError");printInt(sdcard->initError);print("\r\n");
      }
    }

    if(equal(str,"dir",-1)){
      currVolume->root->dirList();
    }
    
    if(equal(str,"mkdir",-1)){
      print("dir name?\r\n");
      char filename[12];
      scan(filename,-1,-1);
      res = file->makeDir(currVolume->root, filename);
      if(res){
        print("mkdir ok\r\n");
      }else{
        print("mkdir ng\r\n");
      }
    }
    if(equal(str,"del",-1)){
      print("name?\r\n");
      char filename[12];
      scan(filename,-1,-1);
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
      scan(filename,-1,-1);
      res = file->open(currVolume->root, filename, O_CREAT | O_WRITE | O_APPEND);
      if(res){
        print("open ok\r\n");
        while(1){
          int val = uart_read();
          file->write(val);
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

    if(equal(str,"print",-1)){
      print("which file?\r\n");
      char filename[12];
      scan(filename,-1,-1);
      print("[");print(filename);print("]");
      res = file->open(currVolume->root, filename, O_READ);
      if(res){
        print("open ok\r\n");
        printInt(file->fileSize_);print("\r\n");
        for(int i=0;i<file->fileSize_;i++){
          int val = file->read();
          uart_write(val);
        }
      }else{
        print("open ng\r\n");
        printInt(file->fileError);print("\r\n");
      }
    }
    
    if(equal(str,"load",-1)){
      print("which file?\r\n");
      char filename[12];
      scan(filename,-1,-1);
      res = file->open(currVolume->root, filename, O_READ);
      if(res){
        print("open ok\r\n");
        for(int i=0;i<0x200000;i++){
          int val = file->read();
          ((char*)(0x200000))[i] = val;//at 2Mbyte
          if((i&0x3FF)==0){
            printInt(i);print("\r\n");
          }
        }
      }else{
        print("open ng\r\n");
        printInt(file->fileError);print("\r\n");
      }
    }
    
    if(equal(str,"upload",-1)){
      print("filename?\r\n");
      char filename[12];
      scan(filename,-1,-1);
      print(filename);
      print("length?\r\n");
      int len = scanInt();
      printInt(len);
      res = file->open(currVolume->root, filename, O_CREAT | O_WRITE | O_APPEND);
      if(res){
        print("open ok\r\n");
        for(int i=0;i<len;i++){
          int val = uart_read();
          file->write(val);
          uart_write(val);
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

    if(equal(str,"run",-1)){
      print("which file?\r\n");
      char filename[12];
      scan(filename,-1,-1);
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

    
    if(equal(str,"clr",-1)){
      screenInit(1024);
    }

  }
}
