/*
 * mostly from Microsoft document fatgen103.doc
 * http://www.microsoft.com/whdc/system/platform/firmware/fatgen.mspx
 */
//------------------------------------------------------------------------------
#define BOOTSIG0 0X55
#define BOOTSIG1 0XAA

#define CMD0    0X00
#define CMD1    0X01
#define CMD8    0X08
#define CMD9    0X09
#define CMD10   0X0A
#define CMD13   0X0D
#define CMD17   0X11
#define CMD24   0X18
#define CMD25   0X19
#define CMD32   0X20
#define CMD33   0X21
#define CMD38   0X26
#define CMD55   0X37
#define CMD58   0X3A
#define ACMD23  0X17
#define ACMD41  0X29


#define R1_READY_STATE        0X00
#define R1_IDLE_STATE         0X01
#define R1_ILLEGAL_COMMAND    0X04
#define DATA_START_BLOCK      0XFE
#define STOP_TRAN_TOKEN       0XFD
#define WRITE_MULTIPLE_TOKEN  0XFC
#define DATA_RES_MASK         0X1F
#define DATA_RES_ACCEPTED     0X05



//struct cid_t {
//  char mid;  // Manufacturer ID
//  char oid[2];  // OEM/Application ID
//  char pnm[5];  // Product name
//  char prv_n_prv_m;  //4:4 h_l Product revision n.m
//  char psn_0;  // Product serial number
//  char psn_1;  // Product serial number
//  char psn_2;  // Product serial number
//  char psn_3;  // Product serial number
//  char reserved_mdt_year_high;  //4:4 h_l Manufacturing date
//  char mdt_year_low_mdt_month;//4:4 h_l 
//  char crc_always1;//7:1 h_l 
//};

//------------------------------------------------------------------------------
// CSD for version 1.00 cards
//struct csd1_t {
//  char csd_ver_reserved1;//2:6
//  char taac;
//  char nsac;
//  char tran_speed;
//  char ccc_high;
//  char ccc_low_read_bl_len;//4:4
//  char read_bl_partial_write_blk_misalign_read_blk_misalign_dsr_imp_reserved2_c_size_high;//1:1:1:1:2:2
//  char c_size_mid;
//  char c_size_low_vdd_r_curr_min_vdd_r_curr_max;//2:3:3
//  char vdd_w_curr_min_vdd_w_cur_max_c_size_mult_high;//3:3:2
//  char c_size_mult_low_erase_blk_en_sector_size_high;//1:1:6
//  char sector_size_low_wp_grp_size;//1:7
//  char wp_grp_enable_reserved3_r2w_factor_write_bl_len_high;//1:2:3:2
//  char write_bl_len_low_write_partial_reserved4;//2:1:5
//  char file_format_grp_copy_perm_write_protect_tmp_write_protect_file_format_reserved5;//1:1:1:1:2:2
//  char crc_always1;//7:1
//};
//------------------------------------------------------------------------------
// CSD for version 2.00 cards
//struct csd2_t {
//  char csd_ver_reserved1;//2:6
//  char taac;
//  char nsac;
//  char tran_speed;
//  char ccc_high;
//  char ccc_low_read_bl_len;//4:4
//  char read_bl_partial_write_blk_misalign_read_blk_misalign_dsr_imp_reserved2;//1:1:1:1:4
//  char c_size_high_reserved3;//6:2
//  char c_size_mid;
//  char c_size_low;
//  char reserved4_erase_blk_en_sector_size_high;//1:1:6
//  char sector_size_low_wp_grp_size;//1:7
//  char wp_grp_enable_reserved5_r2w_factor_write_bl_len_high;//1:2:3:2
//  char write_bl_len_low_write_partial_reserved6;//2:1:5
//  char file_format_grp_copy_perm_write_protect_tmp_write_protect_file_format_reserved7;//1:1:1:1:2:2
//  char crc_always1;//7:1
//};
//------------------------------------------------------------------------------
// union of old and new style CSD register
//union csd_t {
//  csd1_t v1;
//  csd2_t v2;
//};


/** init timeout ms */
#define SD_INIT_TIMEOUT 300000
/** erase timeout ms */
#define SD_ERASE_TIMEOUT 300000
/** read timeout ms */
#define SD_READ_TIMEOUT 300000
/** write time out ms */
#define SD_WRITE_TIMEOUT 600000

// SD card errors

#define SD_CARD_ERROR_CMD0                1 /** timeout error for command CMD0 */
#define SD_CARD_ERROR_CMD8                2 /** CMD8 was not accepted - not a valid SD card*/
#define SD_CARD_ERROR_CMD17               3 /** card returned an error response for CMD17 (read block) */
#define SD_CARD_ERROR_CMD24               4 /** card returned an error response for CMD24 (write block) */
#define SD_CARD_ERROR_CMD25               5/**  WRITE_MULTIPLE_BLOCKS command failed */
#define SD_CARD_ERROR_CMD58               6/** card returned an error response for CMD58 (read OCR) */
#define SD_CARD_ERROR_ACMD23              7/** SET_WR_BLK_ERASE_COUNT failed */
#define SD_CARD_ERROR_ACMD41              8/** card's ACMD41 initialization process timeout */
#define SD_CARD_ERROR_BAD_CSD             9/** card returned a bad CSR version field */
#define SD_CARD_ERROR_ERASE               10/** erase block group command failed */
#define SD_CARD_ERROR_ERASE_SINGLE_BLOCK  11/** card not capable of single block erase */
#define SD_CARD_ERROR_ERASE_TIMEOUT       12/** Erase sequence timed out */
#define SD_CARD_ERROR_READ                13/** card returned an error token instead of read data */
#define SD_CARD_ERROR_READ_REG            14/** read CID or CSD failed */
#define SD_CARD_ERROR_READ_TIMEOUT        15/** timeout while waiting for start of read data */
#define SD_CARD_ERROR_STOP_TRAN           16/** card did not accept STOP_TRAN_TOKEN */
#define SD_CARD_ERROR_WRITE               17/** card returned an error token as a response to a write operation */
#define SD_CARD_ERROR_WRITE_BLOCK_ZERO    18/** attempt to write protected block zero */
#define SD_CARD_ERROR_WRITE_MULTIPLE      19/** card did not go ready for a multiple block write */
#define SD_CARD_ERROR_WRITE_PROGRAMMING   20/** card returned an error to a CMD13 status check after a write */
#define SD_CARD_ERROR_WRITE_TIMEOUT       21/** timeout occurred during write programming */
#define SD_CARD_ERROR_SCK_RATE            22/** incorrect rate selected */


/** Standard capacity V1 SD card */
#define SD_CARD_TYPE_SD1 1
/** Standard capacity V2 SD card */
#define SD_CARD_TYPE_SD2 2
/** High Capacity SD card */
#define SD_CARD_TYPE_SDHC 3

#define SD_CARD_TYPE_MMC 4

typedef int int8_t;
typedef int uint8_t;
typedef int int16_t;
typedef int uint16_t;
typedef int uint32_t;
typedef int size_t;

//------------------------------------------------------------------------------
/**
 * \class Sd2Card
 * \brief Raw access to SD and SDHC flash memory cards.
 */
class Sd2Card {
 public:
  //uint32_t block_;
  uint8_t chipSelectPin_;
  int polpha_;
  uint8_t errorCode_;
  //uint8_t inBlock_;
  //uint16_t offset_;
  //uint8_t partialBlockRead_;
  uint8_t status_;
  uint8_t type_;
  
  
  /** Construct an instance of Sd2Card. */
  uint8_t erase(uint32_t firstBlock, uint32_t lastBlock);
  uint8_t eraseSingleBlockEnable(void);
  /**
   * \return error code for last error. See Sd2Card.h for a list of error codes.
   */
  uint8_t errorCode(void) const {return errorCode_;}
  /** \return error data for last error. */
  uint8_t errorData(void) const {return status_;}
  /**
   * Initialize an SD flash memory card with default clock rate and chip
   * select pin.  See sd2Card::init(uint8_t sckRateID, uint8_t chipSelectPin).
   */

  /**
   * Initialize an SD flash memory card with the selected SPI clock rate
   * and the default SD chip select pin.
   * See sd2Card::init(uint8_t sckRateID, uint8_t chipSelectPin).
   */
  //void partialBlockRead(uint8_t value);
  /** Returns the current value, true or false, for partial block read. */
  //uint8_t partialBlockRead(void) const {return partialBlockRead_;}

  uint8_t readBlock(uint32_t block, char* dst) {
    uint16_t n;
    uint16_t offset = 0;
    uint16_t count = 512;

    //if (!inBlock_ || block != block_ || offset < offset_) {
      //block_ = block;
      // use address if not SDHC card
    if (type()!= SD_CARD_TYPE_SDHC) block <<= 9;
    if (cardCommand(CMD17, block)) {
      error(SD_CARD_ERROR_CMD17);
      goto fail;
    }
    if (!waitStartBlock()) {
      goto fail;
    }
      //offset_ = 0;
      //inBlock_ = 1;
    //}


    // skip data before offset
    //for (;offset_ < offset; offset_++) {
    //  spiRec();
    //}
    // transfer data
    for (uint16_t i = 0; i < 512; i++) {
      dst[i] = spiRec();
    }

    //offset_ += count;
    //if (!partialBlockRead_ || offset_ >= 512) {
      // read rest of data, checksum and set chip select high
    readEnd();
    //}
    return true;

   fail:
    chipSelectHigh();
    return false;
  }

  void readEnd(void) {
    //if (inBlock_) {
        // skip data and crc
      //while (offset_++ < 514) spiRec();
      spiRec();
      spiRec();
      chipSelectHigh();
      //inBlock_ = 0;
    //}
  }
  uint8_t setSckRate(uint8_t sckRateID);
  /** Return the card type: SD V1, SD V2 or SDHC */
  uint8_t type(void) const {return type_;}

  uint8_t writeBlock(uint32_t blockNumber, const char* src) {
    // use address if not SDHC card
    if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
    if (cardCommand(CMD24, blockNumber)) {
      error(SD_CARD_ERROR_CMD24);
      goto fail;
    }
    if (!writeData(DATA_START_BLOCK, src)) goto fail;

    // wait for flash programming to complete
    if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
      error(SD_CARD_ERROR_WRITE_TIMEOUT);
      goto fail;
    }
    // response is r2 so get and check two bytes for nonzero
    if (cardCommand(CMD13, 0) || spiRec()) {
      error(SD_CARD_ERROR_WRITE_PROGRAMMING);
      goto fail;
    }
    chipSelectHigh();
    return true;

   fail:
    chipSelectHigh();
    return false;
  }

  uint8_t writeData(const char* src) {
    // wait for previous write to finish
    if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
      error(SD_CARD_ERROR_WRITE_MULTIPLE);
      chipSelectHigh();
      return false;
    }
    return writeData(WRITE_MULTIPLE_TOKEN, src);
  }

  uint8_t writeStart(uint32_t blockNumber, uint32_t eraseCount);
  uint8_t writeStop(void);

  // private functions
  uint8_t cardAcmd(uint8_t cmd, uint32_t arg) {
    cardCommand(CMD55, 0);
    return cardCommand(cmd, arg);
  }
  void error(uint8_t code) {errorCode_ = code;}

  void chipSelectHigh() {
    SPI_CHIP_SELECT_HIGH();
  }
  void chipSelectLow() {
    SPI_WRITE_POLPHA(polpha_);
    SPI_CHIP_SELECT_LOW(chipSelectPin_);
  }

  void type(uint8_t value) {type_ = value;}
  uint8_t waitNotBusy(uint16_t timeoutMillis) {
    uint16_t t0 = millis();
    do {
      if (spiRec() == 0XFF) return true;
    }
    while (((uint16_t)millis() - t0) < timeoutMillis);
    return false;
  }

  uint8_t writeData(uint8_t token, const char* src) {
    spiSend(token);
    for (uint16_t i = 0; i < 512; i++) {
      spiSend(src[i]);
    }
    spiSend(0xff);  // dummy crc
    spiSend(0xff);  // dummy crc

    status_ = spiRec();
    if ((status_ & DATA_RES_MASK) != DATA_RES_ACCEPTED) {
      error(SD_CARD_ERROR_WRITE);
      chipSelectHigh();
      return false;
    }
    return true;
  }
  
  void printType(){
    switch(type_){
      case 1:print("sd1\r\n");break;
      case 2:print("sd2\r\n");break;
      case 3:print("sdhc\r\n");break;
      case 4:print("mmc\r\n");break;
    }
  }
  
  ///////////////////////////////////////////////////
  uint8_t cardCommand(uint8_t cmd, uint32_t arg) {
    // end read if in partialBlockRead mode
    readEnd();

    // select card
    chipSelectLow();

    // wait up to 300 ms if busy
    waitNotBusy(300);

    // send command
    spiSend(cmd | 0x40);

    // send argument
    for (int8_t s = 24; s >= 0; s -= 8) spiSend(arg >> s);

    // send CRC
    uint8_t crc = 0XFF;
    if (cmd == CMD0) crc = 0X95;  // correct crc for CMD0 with arg 0
    if (cmd == CMD8) crc = 0X87;  // correct crc for CMD8 with arg 0X1AA
    spiSend(crc);

    // wait for response
    for (uint8_t i = 0; ((status_ = spiRec()) & 0X80) && i != 0XFF; i++)
      ;
    return status_;
  }
  
  int millis(){
    return IORD(MYTIMER, 2);
  }
  

  int initError;
  uint8_t init(uint8_t chipSelectPin) {
    initError = 0;

    
    errorCode_ = 0;
    //inBlock_ = 0;
    //partialBlockRead_ = 0;
    type_ = 0;
    
    //offset_=0;
    IOWR(MYTIMER, 2, 0);
    
    errorCode_ = type_ = 0;//inBlock_ =  partialBlockRead_ = 
    chipSelectPin_ = chipSelectPin;
    // 16-bit init start time allows over a minute
    uint16_t t0 = (uint16_t)millis();
    uint32_t arg;


    // must supply min of 74 clock cycles with CS high.
    for (uint8_t i = 0; i < 10; i++) spiSend(0XFF);


    // command to go idle in SPI mode
    int ok = 0;
    for(int i=0;i<16;i++){
      polpha_ = i;
      chipSelectLow();
      
      for(int i=0;i<10;i++){
        status_ = cardCommand(CMD0, 0);
        if(status_ == R1_IDLE_STATE){
          ok = 1;
          break;
        }
        IOWR(MYTIMER, 2, 0);
        while(IORD(MYTIMER, 2)<1000);
      }
      if(ok){
        break;
      }
    }
    if(!ok){
      initError = 1;
      goto fail;
    }
    //while ((status_ = cardCommand(CMD0, 0)) != R1_IDLE_STATE) {
    //  if (((uint16_t)(millis() - t0)) > SD_INIT_TIMEOUT) {
    //    initError = 1;
    //    goto fail;
    //  }
    //}
    // check SD version
    if ((cardCommand(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND)) {


        IOWR(MYTIMER, 1, 0);
        while(true){
          status_ = cardCommand(CMD1, 0);
          if(status_ == R1_READY_STATE) {
            type(SD_CARD_TYPE_MMC);
            chipSelectHigh();
            return true;
          }
          int time = IORD(MYTIMER, 1);
          if(time>100000){//0.3s
            break;
          }
        }

      type(SD_CARD_TYPE_SD1);
    } else {
      // only need last byte of r7 response
      for (uint8_t i = 0; i < 4; i++) status_ = spiRec();
      if (status_ != 0XAA) {
        initError = 2;
        goto fail;
      }
      type(SD_CARD_TYPE_SD2);
    }
    // initialize card and send host supports SDHC if SD2
    arg = type() == SD_CARD_TYPE_SD2 ? 0X40000000 : 0;

    while ((status_ = cardAcmd(ACMD41, arg)) != R1_READY_STATE) {
      // check for timeout
      if (((uint16_t)(millis() - t0)) > SD_INIT_TIMEOUT) {
        initError = 3;
        goto fail;
      }
    }
    // if SD2 read OCR register to check for SDHC card
    if (type() == SD_CARD_TYPE_SD2) {
      if (cardCommand(CMD58, 0)) {
        initError = 4;
        goto fail;
      }
      if ((spiRec() & 0XC0) == 0XC0) type(SD_CARD_TYPE_SDHC);
      // discard rest of ocr - contains allowed voltage range
      for (uint8_t i = 0; i < 3; i++) spiRec();
    }
    chipSelectHigh();

    return true;

   fail:
    chipSelectHigh();
    return false;

  };
  uint8_t waitStartBlock(void) {
    IOWR(MYTIMER, 2, 0);
    uint16_t t0 = millis();
    while ((status_ = spiRec()) == 0XFF) {
      if (((uint16_t)millis() - t0) > SD_READ_TIMEOUT) {
        initError = 5;
        goto fail;
      }
    }
    if (status_ != DATA_START_BLOCK) {
      initError = 6;
      goto fail;
    }
    return true;

   fail:
    chipSelectHigh();
    return false;
  }

};
