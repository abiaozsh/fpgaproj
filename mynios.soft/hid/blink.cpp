#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

//basecount = freq / baute rate

void delay(uint16_t ticks)
{
  for(uint16_t i = 0 ; i < ticks; i++){
    TCNT0 = 0;
    while(TCNT0<(80));
  }
}

//8000000 / 115200 = 69.4
//8000000 / 115200 * 1.5 = 104.16666666666666666666666666667
#define TCCR0_Value_115200 1
#define CNT_Value_115200 63
#define CNT_1_5_115200 95
//#define CNT_Value_115200 64
//#define CNT_1_5_115200 96


//1000000 / 9600 = 104.16666666666666666666666666667
//1000000 / 9600 = 156.25
#define TCCR0_Value_9600 2
#define CNT_Value_9600 104
#define CNT_1_5_9600 156


#define TCCR0_Value TCCR0_Value_115200
#define CNT_Value CNT_Value_115200
#define CNT_1_5 CNT_1_5_115200


//pd0 rxd		usb txd
//pd1 txd		usb rxd
#define DDR_Send DDRD
#define PORT_Send PORTD
#define BIT_Send _BV(1)
#define DDR_Recv DDRD
#define PIN_Recv PIND
#define BIT_Recv _BV(0)

void SerialInit(){
	//UCSR0B = 0;//not forget turnoff usart0 on mega328p
	DDR_Send |= BIT_Send;
	DDR_Recv &= ~BIT_Recv;
	PORT_Send |= BIT_Send;
}
void TimerInit()
{
  // COM1A1 COM1A0 COM1B1 COM1B0 – – WGM11 WGM10
//CS02 CS01 CS00 Description
//0   0 0 0 No clock source (Timer/Counter stopped)
//1 8Mhz      0 0 1 clkI/O/(No prescaling)
//2 1Mhz      0 1 0 clkI/O/8 (From prescaler)
//3 1/8Mhz    0 1 1 clkI/O/64 (From prescaler)
//4 1/32Mhz   1 0 0 clkI/O/256 (From prescaler)
//5 1/128Mhz  1 0 1 clkI/O/1024 (From prescaler)
  TCCR0 = TCCR0_Value;
}


void SerialSend(uint8_t val){
	//cli();
	TCCR0 = TCCR0_Value;
	TCNT0 = 0;
	PORT_Send &= ~BIT_Send;
  while(TCNT0<CNT_Value);TCNT0 = 0;//startbit
	uint8_t chkbit = 0x01;
	for(uint8_t i = 8 ; i > 0 ; i--)
	{
		if(val&chkbit)
    {
      PORT_Send |= BIT_Send;
    }else{
      PORT_Send &= ~BIT_Send;
    }
    chkbit<<=1;
    while(TCNT0<CNT_Value);TCNT0 = 0;
	}
	PORT_Send |= BIT_Send;
  while(TCNT0<CNT_Value);TCNT0 = 0;//stopbit
	//sei();
}

void printByte(int16_t val){
  char* chardata = (char*)"0123456789ABCDEF";
  SerialSend(chardata[(val>>12)&0x0F]);
  SerialSend(chardata[(val>>8)&0x0F]);
  SerialSend(chardata[(val>>4)&0x0F]);
  SerialSend(chardata[(val)&0x0F]);
}
void printBin(int16_t val){
  for(int8_t i = 15;i>=0;i--){
    SerialSend('0'+((val>>i)&1)); // Status Byte
  }
}
PROGMEM prog_uint32_t num10s[] = {
1000000000,
100000000,
10000000,
1000000,
100000,
10000,
1000,
100,
10,
1,
};

void SendInt(int32_t val){
  if(val<0){
		SerialSend('-');
    val = -val;
  }
	uint32_t num = val;
	for(uint8_t idx = 0; idx < 10 ; idx++)
	{
		uint8_t outNum = 0;
		uint32_t CmpNum = pgm_read_dword_near(num10s + idx);
		for(uint8_t i = 0; i < 10 ; i++)
		{
			if(num>=CmpNum)
			{
				num -= CmpNum;
				outNum++;
			}
			else
			{
				break;
			}
		}
		SerialSend('0' + outNum);
	}
}

uint8_t SerialRead()
{
	//cli();
	uint8_t val = 0;
	TCCR0 = TCCR0_Value;
	while(PIN_Recv&BIT_Recv){
	}
	TCNT0 = 0;
	while(TCNT0<CNT_1_5);TCNT0 = 0;
	for(uint8_t i = 8 ; i > 0 ; i--)
	{
		val>>=1;val |= (PIN_Recv&BIT_Recv?0x80:0);
		while(TCNT0<CNT_Value);TCNT0 = 0;
	}
	//sei();
	return val;
}

volatile uint16_t key_data[3];
volatile uint16_t key_value = 0;
volatile uint8_t key_pos = 0;
volatile uint8_t key_idx = 0;
volatile uint8_t key_done = 0;
volatile uint8_t key_count = 0;


//E0 F0 xx
//D2+B0 keyboard
ISR(INT1_vect){
  TCNT1 = 0;
  if(PIND & _BV(3)){
  }else{
    //fall edge
    if(PINB & _BV(7)){
      key_value |= 1<<(key_count);
    }else{
      key_value |= 0;
    }
    key_count++;
    if(key_count==11){
      key_count = 0;
      uint16_t v = key_value>>1;
      key_data[key_pos] = v;
      key_value = 0;
      key_pos++;
      if((v&0x80)==0){
        key_done = 1;
        key_idx = key_pos;
        key_pos=0;
      }
    }
  }
}


volatile uint16_t mouse_data[5];
volatile uint16_t mouse_value = 0;
volatile uint8_t mouse_pos = 0;
volatile uint8_t mouse_count = 0;
volatile uint8_t mouse_skip = 2;

#define mouseCLK PIND & _BV(2)
#define mouseDAT PINB & _BV(6)

//D3+D7 mouse
ISR(INT0_vect){
  if(mouseCLK){
  }else{
    if(mouse_skip){
      mouse_skip--;
    }else{
      //fall edge
      if(mouseDAT){
        mouse_value |= 1<<(mouse_count);
      }else{
        mouse_value |= 0;
      }
      mouse_count++;
      if(mouse_count==11){
        mouse_count = 0;
        mouse_data[mouse_pos] = (mouse_value>>2)&0xFF;
        mouse_value = 0;
        mouse_pos++;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////


uint8_t clk() {
  return mouseCLK;
}
void clkH() {
  PORTD |= _BV(2);
  DDRD &= ~_BV(2);
}
void clkL() {
  PORTD &= ~_BV(2);
  DDRD  |= _BV(2);
}

uint8_t dat() {
  return mouseDAT;
}
void datH() {
  PORTB |= _BV(6);
  DDRB &= ~_BV(6);
}
void datL() {
  PORTB &= ~_BV(6);
  DDRB  |= _BV(6);
}


void write(uint16_t data) {
  char i;
  //5 1/128Mhz  1 0 1 clkI/O/1024 (From prescaler)

  clkH();//pull_high(_clock_pin);
  
  delay(20);
  datL();//pull_low(_data_pin);
  clkL();//pull_low(_clock_pin);
  
  delay(20);
  clkH();//pull_high(_clock_pin); // Start Bit

  
  TCCR0 = 5;//串口会自己重置的
  // clock is low, and we are clear to send data
  for (i=0; i < 9; i++) {
    TCNT0 = 0;
    while (clk()){//digitalRead(_clock_pin)
      if(TCNT0>250){
	    datH();
        return;
      }
    }
    if (data & 0x01) {
      datH();//pull_high(_data_pin);
    } else {
      datL();//pull_low(_data_pin);
    }
    // wait for clock cycle
    TCNT0 = 0;
    while (!clk()){//digitalRead(_clock_pin)
      if(TCNT0>250){
        datH();
        return;
      }
    }
    data = data >> 1;
  }
  TCNT0 = 0;
  while ((!clk()) || (!dat())){ // digitalRead(_clock_pin) digitalRead(_data_pin)wait for mouse to switch modes
    if(TCNT0>250){
      return;
    }
  }
  clkL();//pull_low(_clock_pin); // put a hold on the incoming data.
}


int16_t read_movement_x(int16_t status,int16_t x) {
  //int16_t x = (read_byte()>>3)&0xFF;
  if (bitRead(status, 4)) {
    for(uint8_t i = 8; i < 16; ++i) {
      x |= (1<<i);
    }
  }
  return x;
}

int16_t read_movement_y(int16_t status,int16_t y) {
  //int16_t y = (read_byte()>>3)&0xFF;
  if (bitRead(status, 5)) {
    for(uint8_t i = 8; i < 16; ++i) {
      y |= (1<<i);
    }
  }
  return y;
}


/////////////////////////////////////////////////////////////////////////////////////////////

#define PORT_DAT_ON  DDRC |=  _BV(5)
#define PORT_DAT_OFF DDRC &= ~_BV(5)

#define PORT_CLK_ON  DDRC |=  _BV(4)
#define PORT_CLK_OFF DDRC &= ~_BV(4)

#define PORT_STR_ON  DDRC |=  _BV(3)
#define PORT_STR_OFF DDRC &= ~_BV(3)

uint8_t SendByte(uint8_t data){
  uint8_t p = 0;
  for(uint8_t i=0;i<8;i++)
  {
    delay(1);
    if(data&1)
    {
      PORT_DAT_ON;
      p = p ^ 1;
    }
    else
    {
      PORT_DAT_OFF;
    }
    delay(1);
    data>>=1;
    PORT_CLK_ON; //shift clock up
    delay(1);
    PORT_CLK_OFF; //shift clock down
  }
  return p;
}

int main()
{
	//datL();
	//TCCR0 = 5;//串口会自己重置的
    //TCNT0 = 0;
    //while (clk()){//digitalRead(_clock_pin)
    //  if(TCNT0>250){
    //    break;
    //  }
    //}
	//datH();
	//while(1);

	TimerInit(); //do later
	SerialInit();
  MCUCR |= _BV(ISC00);
  GICR  |= _BV(INT0);
  MCUCR |= _BV(ISC10);
  GICR  |= _BV(INT1);
  PORT_DAT_OFF;
  PORT_CLK_OFF;
  PORT_STR_OFF;
  PORTC &= ~_BV(5);
  PORTC &= ~_BV(4);
  PORTC &= ~_BV(3);
  TCCR1B = 3;
  
  sei();
  
  uint8_t serialOut = 0;
  uint16_t old_mouse_data1 = 0;
  
  if(serialOut){
    SerialSend('h');
    SerialSend('e');
    SerialSend('l');
    SerialSend('l');
    SerialSend('o');
    SerialSend('\r');
    SerialSend('\n');
  }
  while(1){

    
    int16_t data[5];
    GICR &=~_BV(INT0);
    write(0x01EB);
    mouse_value = 0;
    mouse_pos = 0;
    mouse_count = 0;
    mouse_skip = 1;
    clkH();
    GICR |= _BV(INT0);
    
    if(TCNT1>10000){
      key_count = 0;
      key_value = 0;
      key_pos=0;
    }
    
    TCCR0 = TCCR0_Value;
    for(uint16_t i = 0 ; i < 500; i++){
      TCNT0 = 0;
      while(TCNT0<(80)){
        
        if(key_done == 1){
          if(serialOut){
            SerialSend(' ');    printByte(key_data[0]);
            SerialSend(' ');    printByte(key_data[1]);
            SerialSend(' ');    printByte(key_data[2]);

            SerialSend(' ');    printBin(key_data[0]);
            SerialSend(' ');    printBin(key_data[1]);
            SerialSend(' ');    printBin(key_data[2]);
            SerialSend(' ');
          }else{
            uint8_t p = 0;
            p = p ^ SendByte(key_data[key_idx-1]);
            p = p ^ SendByte(key_data[key_idx-2]);
            p = p ^ SendByte(key_data[key_idx-3]);
            p = p ^ 1;
            SendByte((p<<4)|1);
            delay(1);
            PORT_STR_ON ;
            delay(1);
            PORT_STR_OFF;

          }
          key_done = 0;
          key_data[0] = 0;
          key_data[1] = 0;
          key_data[2] = 0;
        }
      }
    }
    int16_t x = read_movement_x(mouse_data[1],mouse_data[2]);
    int16_t y = read_movement_y(mouse_data[1],mouse_data[3]);
    
    uint8_t btnChange = old_mouse_data1 != mouse_data[1];
    old_mouse_data1 = mouse_data[1];
    
    if(btnChange || mouse_data[2] || mouse_data[3]){

      if(serialOut){
        SerialSend('\r');
        SerialSend('\n');

        SerialSend(' ');    printByte(mouse_data[0]);
        SerialSend(' ');    printByte(mouse_data[1]);
        SerialSend(' ');    printByte(mouse_data[2]);
        SerialSend(' ');    printByte(mouse_data[3]);
        SerialSend(' ');    printByte(mouse_data[4]);

        SerialSend(' ');    printBin(mouse_data[0]);
        SerialSend(' ');    printBin(mouse_data[1]);
        SerialSend(' ');    printBin(mouse_data[2]);
        SerialSend(' ');    printBin(mouse_data[3]);
        SerialSend(' ');    printBin(mouse_data[4]);
        SerialSend(' ');
        SendInt(x); // X Movement Data
        SerialSend(',');
        SendInt(y); // Y Movement Data
      }else{
        uint8_t p = 0;
        p = p ^ SendByte(x);
        p = p ^ SendByte(y);
        p = p ^ SendByte(mouse_data[1]);
        SendByte((p<<4) | 0);
        delay(1);
        PORT_STR_ON ;
        delay(1);
        PORT_STR_OFF;
      }
      
    }
  }

	return 0;
}














