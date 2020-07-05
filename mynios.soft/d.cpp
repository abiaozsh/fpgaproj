#include "inc/io.h"
#include "inc/system.h"
#include "inc/system.cpp"
//#include "inc/uart.cpp"
//#include "inc/uartio.cpp"
#include "inc/keyScreen.cpp"
#include "inc/print.cpp"
#include "inc/spi.cpp"


int cardCommand(int cmd, int arg) {
  // send command
  spiSend(cmd | 0x40);

  // send argument
  for (int s = 24; s >= 0; s -= 8) spiSend(arg >> s);

  // send CRC
  int crc = 0XFF;
  crc = 0X95;  // correct crc for CMD0 with arg 0
  spiSend(crc);

  // wait for response
  //for (uint8_t i = 0; ((status_ = spiRec()) & 0X80) && i != 0XFF; i++)
    ;
  return 0;
}

int main()
{
  malloc_index = 0;
  
  stdioInit(1024);

	char str[20];

	print("Hello from Nios II!\r\n");
  
	while(1){
    scan(str,-1);
    print(str);print("\r\n");
    
    if(equal(str,"cs",-1)){
      print("which sd?\r\n");
      for (int i = 0; i < 10; i++) spiSend(0XFF);
      int cs = scanInt();
      printInt(cs);
      SPI_CHIP_SELECT_LOW(cs);
      print("done\r\n");
    }

    if(equal(str,"c",-1)){
      cardCommand(0, 0);
      print("done\r\n");
    }
    
    if(equal(str,"clk",-1)){
      IOWR(SOFTSPI, SOFTSPI_SCK, 1);
      print("ok\r\n");
      IOWR(SOFTSPI, SOFTSPI_SCK, 0);
      print("ok\r\n");
    }
    
    if(equal(str,"p",-1)){
      print("p?\r\n");
      int p = scanInt();
      IOWR(SOFTSPI, SOFTSPI_POLPHA, p);
      print("done\r\n");
    }

    if(equal(str,"r",-1)){
      int res = spiRec();
      print("res:");printByte(res);
      //print("spi_debug0:");printInt(spi_debug0);
      print("\r\n");
    }

    if(equal(str,"auto",-1)){
      int pha[3];
      pha[0] = -1;
      pha[1] = -1;
      pha[2] = -1;
      for(int cs=0;cs<3;cs++){
        for (int i = 0; i < 10; i++) spiSend(0XFF);
        print("cs:");printInt(cs);print("\r\n");
        SPI_CHIP_SELECT_LOW(cs);
        int ok = 0;
        for(int p=0;p<16;p++){
          print("p:");printInt(p);print("\r\n");
          IOWR(SOFTSPI, SOFTSPI_POLPHA, p);
          
          cardCommand(0, 0);
          for(int i=0;i<10;i++){
            print("c\r\n");
            int res = spiRec();
            print("res:");printByte(res);print("\r\n");
            if(res==1){
              ok=1;
              pha[cs] = p;
              break;
            }
          }
          if(ok){
            break;
          }
        }
      }
      print("report\r\n");
      print("p0:");printInt(pha[0]);print("\r\n");
      print("p1:");printInt(pha[1]);print("\r\n");
      print("p2:");printInt(pha[2]);print("\r\n");
    }


	}
  return 0;
}
