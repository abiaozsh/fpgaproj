void SPI_CHIP_SELECT_HIGH(){
  IOWR(SOFTSPI, SOFTSPI_CS, 0x07);
}
void SPI_CHIP_SELECT_LOW(int chip){
  IOWR(SOFTSPI, SOFTSPI_CS, ~(1<<chip));
}

void SPI_WRITE_POLPHA(int val){
  IOWR(SOFTSPI, SOFTSPI_POLPHA, val);
}

//------------------------------------------------------------------------------
//volatile int spi_debug0;
int spiRec() {
  IORD(SOFTSPI, SOFTSPI_READ);
  IOWR(SOFTSPI, SOFTSPI_READ, 1);
  //spi_debug0 = 0;
  while(1){
    //spi_debug0++;
    int tmp = IORD(SOFTSPI, SOFTSPI_READ);
    if(tmp & 0x100){
      return tmp & 0xFF;
    }
  }
}

//volatile int spi_debug1;
//------------------------------------------------------------------------------
void spiSend(int data) {
  IORD(SOFTSPI, SOFTSPI_WRITE);
  IOWR(SOFTSPI, SOFTSPI_WRITE, data);
  //spi_debug1 = 0;
  while(1){
    //spi_debug1++;
    int tmp = IORD(SOFTSPI, SOFTSPI_WRITE);
    if(tmp & 0x100){
      return;
    }
  }
}
/*
inline void _dly()
{
  IOWR(MYTIMER, 0, 0);
  while(1)
  {
	  int time = IORD(MYTIMER, 0);
	  if(time > 10){
		  break;
	  }
  }
}

//------------------------------------------------------------------------------
int spiRec() {
  int data = 0;
  IOWR(SOFTSPI, SOFTSPI_MOSI, 1);
  int i;
  for (i = 0; i < 8; i++) {
    IOWR(SOFTSPI, SOFTSPI_SCK, 1);
    _dly();
    data <<= 1;
    
    if (IORD(SOFTSPI, SOFTSPI_MISO)) data |= 1;

    IOWR(SOFTSPI, SOFTSPI_SCK, 0);
    _dly();
  }
  return data;
}
//------------------------------------------------------------------------------
void spiSend(int data) {
  int i;
  for (i = 0; i < 8; i++) {
    IOWR(SOFTSPI, SOFTSPI_SCK, 0);
    _dly();
    if(data & 0x80)
    {
      IOWR(SOFTSPI, SOFTSPI_MOSI, 1);
    }
    else
    {
      IOWR(SOFTSPI, SOFTSPI_MOSI, 0);
    }
    data <<= 1;
    IOWR(SOFTSPI, SOFTSPI_SCK, 1);
    _dly();
  }
  IOWR(SOFTSPI, SOFTSPI_SCK, 0);
}
*/

