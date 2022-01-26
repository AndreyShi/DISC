/*
 * twitest.cpp
 *
 * Created: 16.07.2019 9:22:19
 *  Author: Andrew
 */ 
 #include "twitest.h"
 #include "config.h"

 void settime(unsigned char *timefromPC)
{
	int yearnow;
	TWI_sendaddres(PCF8583,Writemode);
	TWI_senddata(0x01);
	TWI_senddata(timefromPC[10]);// set msec 00
	TWI_senddata(timefromPC[9]);// set sec 00
	TWI_senddata(timefromPC[8]);// set min 40
	TWI_senddata(timefromPC[7]);// set hour 15
	TWI_senddata(timefromPC[5]);// set year/day
	TWI_senddata(timefromPC[6]);// set month
	TWI_stoplink();

	yearnow = timefromPC[11];
	yearnow = yearnow | timefromPC[12] << 8;

}

TWI_TALK send_pocket_to_led_test2(char led)
{
		//pocket is adress 0x80 and first data byte and second data byte
		//char slave_adr=0x70;
		char state,arbitration_lost,bus_error,write_interrupt_flag;
		TWI_TALK answer;

		int counter;
		TWIC.MASTER.ADDR=0x80;
		for(counter=0;;counter++) {
			state=TWIC.MASTER.STATUS;
			arbitration_lost = state&0x08;
			bus_error = state&0x04;
			write_interrupt_flag=state&0x40;
			if ( arbitration_lost==0x08) return ARBITRATION_LOST_TWI;
			if ( bus_error==0x04) return BUS_ERROR_TWI;
			if (write_interrupt_flag==0x40) break;
			if (counter>=100) return TIMEOUT_TWI;

		}
		answer= l_k_s.get_acknowledge_twi_led();
		if(answer!=SLAVE_ADRESS_IS_SENT_TWI) {
			l_k_s.send_stop_twi();
			return answer;
		}
		TWIC.MASTER.DATA=led;
		for(counter=0;;counter++) {
			state=TWIC.MASTER.STATUS;
			write_interrupt_flag=state&0x40;
			if (write_interrupt_flag==0x40) break;
			if (counter>=120) return TIMEOUT_TWI;

		}
		answer= l_k_s.get_acknowledge_twi_led();

		l_k_s.send_stop_twi();
		return O_KEY_LED_TWI;
}

unsigned char send_pocket_to_led_test(char led)
{
	//PORTB.OUTSET = PIN6_bm;
	TWIC.MASTER.ADDR=0x80;	
	int count = 20;
	while(count)
	{		
		if(TWIC_MASTER_STATUS & TWI_MASTER_WIF_bm)
		{	
			TWIC_MASTER_DATA = led;
			break;
		}
		if(TWIC_MASTER_STATUS & TWI_MASTER_ARBLOST_bm)return 0x05;
		if(TWIC_MASTER_STATUS & TWI_MASTER_BUSERR_bm)return 0x06;
		delay_mks(5);	
		count--;
		if(!count)return 0x07;
	}	
	TWIC.MASTER.CTRLC=0x03;
	//PORTB.OUTCLR = PIN6_bm;
	return 0x14;
}

void readdatafromrtcclock(void)
{

    TWI_sendaddres(PCF8583,Writemode);
    TWI_senddata(0x01);
    TWI_sendaddres(PCF8583,Readmode);
	TWI_readdata(YesMasterAck);
   TWI_readdata(YesMasterAck);	
    TWI_readdata(YesMasterAck);
    TWI_readdata(YesMasterAck);
  TWI_readdata(YesMasterAck);
   TWI_readdata(NoMasterAckStop);
	
}

void init_rtcclock(void)
{
	TWI_sendaddres(PCF8583,Writemode);
	TWI_senddata(0x00); // Control and status register of PCF8583(00h)
	TWI_senddata(0x08);// read date and month count directly
	TWI_stoplink();
	
}


/*
функция для считывания кнопки с микросхемой max7370 с фильтром первого нажатия , 
происходили самопроизвольные нажатия во время тестирования блоков (приводная помеха высокой частоты на корпус)
какието блоки работали хорошо ,какието нет , железом от помех избавиться не удалось, поэтому
применен программный фильтр + к этому изменена задержка автоповтора в setup_keyboard_max7370();
*/
uint8_t readkeypress_max7370_filter(unsigned char *key)
{

#define REPEAT_KEY_FIFO_NOT_EMPTY 126
#define REPEAT_KEY  62
#define EMPTY_FIFO  63

#define HOLD_KEY    17
#define RELEASE_KEY 19
#define PRESS_KEY   18
#define EMPTY_KEY_  24 

	uint8_t byte = 0 ;
	static uint8_t prev_key;
	static bool lock;
	static bool first_press_is_hold;
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x00);
	TWI_sendaddres(max7370,Readmode);
	byte = TWI_readdata(NoMasterAckStop);

	if(byte != EMPTY_FIFO){
		
		if(!lock && (byte != REPEAT_KEY) && (byte != REPEAT_KEY_FIFO_NOT_EMPTY) &&  ((byte >> 6) != 1)){
			//*key = byte;
			prev_key = byte;
			lock = 1;
			first_press_is_hold = 0;
			return EMPTY_KEY_;	
		}
		else
			lock = 0;
		//===
		//USARTD0.DATA = byte;

		if(byte == REPEAT_KEY || byte == REPEAT_KEY_FIFO_NOT_EMPTY){
			*key = prev_key;
			if(!first_press_is_hold){
				first_press_is_hold = 1;
				return PRESS_KEY;
				}
			else
				return HOLD_KEY;
		}

		if((byte >> 6) == 1){
			*key = prev_key;
			prev_key = 0;
			return RELEASE_KEY;
		} 
	}

	*key = 0;
	return EMPTY_KEY_;
}
/*
функция для считывания кнопки с микросхемой max7370
*/
uint8_t readkeypress_max7370(unsigned char *key)
{
		uint8_t byte = 0 ;
		static uint8_t prev_key;
		TWI_sendaddres(max7370,Writemode);
		TWI_senddata(0x00);
		TWI_sendaddres(max7370,Readmode);
		byte = TWI_readdata(NoMasterAckStop);

		if(byte != EMPTY_FIFO){
			//USARTD0.DATA = byte;

			if(byte == REPEAT_KEY || byte == REPEAT_KEY_FIFO_NOT_EMPTY){
				*key = prev_key;
				return HOLD_KEY;
			}

			if((byte >> 6) == 1){
				*key = prev_key;
				prev_key = 0;
				return RELEASE_KEY;
			}

			if((byte >> 6) == 0){
				*key = byte;
				prev_key = byte;
				return PRESS_KEY;
			}
		}

		*key = 0;
		return EMPTY_KEY_;
}

/*
функция для установки режима автоповтора max7349
*/
void set_auto_repeat_max7349(const uint8_t &param)
{
	cli();
	//автоповтор
	TWI_sendaddres(max7349,Writemode);
	TWI_senddata(0x02);
	TWI_senddata(param);// 0x81
	TWI_stoplink();
	sei();
}
/*
функция для установки звука на микросхеме max7370
ROW7 как выход
*/
void set_sound_max7370()
{
	cli();
	//Set ROW7 low
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x3A);
	TWI_senddata(0x7F); // ROW7
	TWI_stoplink();
	//Key-Switch Array Size Register (0x30)
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x30);
	TWI_senddata(0x7F); // ROW0 to ROW6 are key switches
	TWI_stoplink();

	//GPIO register 0x34
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x34);
	TWI_senddata(0x80); // ROW7 is output
	TWI_stoplink();
	sei();
}

void on_sound_max7370()
{
	cli();
	//Set ROW7 on
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x3A);
	TWI_senddata(0x80); // ROW7
	TWI_stoplink();
	sei();
}

void off_sound_max7370()
{
	cli();
	//Set ROW7 off
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x3A);
	TWI_senddata(0x7F); // ROW7
	TWI_stoplink();
	sei();
}
/*
Функция по воспроизведению звука(писка) 
используется чип max7370, во время работы функции _delay_ms
происходит срабатывание обработчика прерывания поэтому 
задержка немного увеличивается  

in unsigned char ms - длительность писка в миллисекундах
*/
void peak_sound_max7370(unsigned char ms)
{

	on_sound_max7370();
	while (0 < ms)
	{
		_delay_ms(1);
		--ms;
	}
	off_sound_max7370();
}

/*
функция для считывания кнопки с микросхемой max7349
*/
uint8_t readkeypress_max7349(unsigned char *key)
{
	uint8_t byte = 0 ;
	static uint8_t prev_key;
	static int8_t cnt_release;
	TWI_sendaddres(max7349,Writemode);
	TWI_senddata(0x00);
	TWI_sendaddres(max7349,Readmode);
	byte = TWI_readdata(NoMasterAckStop);

	/*
	считанное кол -во нулей из Fifo буфера микросхемы max7349
	что бы система поняла что это отпускание кнопки
	*/
	#define RELEASE_DELAY   4

	//while(!(USARTD0.STATUS & USART_DREIF_bm) );//оператор ! делает результат условия противоположным
	
	//if(byte)
	//USARTD0.DATA = byte;

	if(byte & 0x3F){
		 *key = byte & 0x3F; // KEY_CODE
		 byte = byte >> 6;

		if(byte == 0){				
 			if(*key != prev_key){ // PRESS
 				prev_key = *key;
				cnt_release = 0;
				return PRESS_KEY;
 				}
			if(*key == prev_key){ // HOLD
				cnt_release = 0;
				return HOLD_KEY;
			}
		}
	}
	else{
		if(prev_key){            // RELEASE	
			cnt_release++;
				if(cnt_release >= RELEASE_DELAY){
					*key = prev_key;
					prev_key = 0;
					cnt_release =0;
					return RELEASE_KEY;	
				}		
			}
		}
	*key = 0;
	return EMPTY_KEY;
}

/*
функция инициализирует микросхему max7370
*/
uint8_t setup_keyboard_max7370()
{
		//регистр конфигурации
	/*sendaddres(max7370,Writemode);
	senddata(0x01);
	senddata(0x81);
	stoplink();*/

	//задержка дребезга
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x02);
	TWI_senddata(0x22);
	TWI_stoplink(); 

	//автоповтор
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x05);
	TWI_senddata(0xF9);// 0x81
	TWI_stoplink();

	/*sendaddres(max7370,Writemode);
	senddata(0x00);
	stoplink();*/
	set_sound_max7370();
	return O_KEY_LED_TWI;
}

/*
функция инициализирует микросхему max7370 для фильтрации первого нажатия 
сохранен автоповтор , теперь  первое нажатие для системы это первый код автоповтора,
задержки выставлены так что пользователь не ощущает момент пропуска кода кнопки, 
между первым нажатием(кодом кнопки) и первым кодом автоповтора: 80 ms
*/
uint8_t setup_keyboard_max7370_filter(void)
{
	//регистр конфигурации
	/*sendaddres(max7370,Writemode);
	senddata(0x01);
	senddata(0x81);
	stoplink();*/

	//задержка дребезга
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x02);
	TWI_senddata(0x24);//0x22
	TWI_stoplink(); 

	//автоповтор
	TWI_sendaddres(max7370,Writemode);
	TWI_senddata(0x05);
	TWI_senddata(0xF0);// 0x81 // F9
	TWI_stoplink();

	/*sendaddres(max7370,Writemode);
	senddata(0x00);
	stoplink();*/
	set_sound_max7370();
	return O_KEY_LED_TWI;
}

void TWI_sendaddres(uint8_t addr,int rw)
{
if(rw)
  {
  addr = addr<<1|0x01;
  TWIC_MASTER_ADDR = addr;
  }
else
  {

  addr = addr<<1;
  TWIC_MASTER_ADDR = addr;
  }
}

void TWI_senddata(uint8_t data)
{
	//while (!(TWIC_MASTER_STATUS & TWI_MASTER_WIF_bm));
	int count = 500;
	while(count--)
	{
		if(TWIC_MASTER_STATUS & TWI_MASTER_WIF_bm)
			break;
	}
	TWIC_MASTER_DATA = data;
}

void TWI_stoplink()
{
  TWIC_MASTER_CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
}

uint8_t TWI_readdatafirst()
{
uint8_t byte;
while(!(TWIC_MASTER_STATUS & TWI_MASTER_RIF_bm));
byte = TWIC_MASTER_DATA;
TWIC_MASTER_CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
return byte;
}

uint8_t TWI_readdata(int MasterAck)
{
uint8_t byte;
	int     count = 750;
    //while(!(TWIC_MASTER_STATUS & TWI_MASTER_RIF_bm));
	while(count--)
	{
		if(TWIC_MASTER_STATUS & TWI_MASTER_RIF_bm)
			break;
	} 
    byte = TWIC_MASTER_DATA;
    if(MasterAck)
	    TWIC_MASTER_CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
    else
	    TWIC_MASTER_CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
    return byte;
}

int TWI_check(uint8_t addr)
{
   addr = addr<<1;
   TWIC_MASTER_ADDR = addr;
   while(TWIC_MASTER_STATUS & TWI_MASTER_RXACK_bm)
   {
		 return 0;
   }
   return 1;
}

void convertBCDtoASCII(char bcd, unsigned char * ascii)
{
//convert from BCD to ASCII for LIR display
	unsigned char ubcd;
	ubcd = bcd;

	if (0x10 <= ubcd)
	{
		ascii[0] = (ubcd >> 4) | 0x30;
		ascii[1] = (ubcd << 4);
		ascii[1] = (ascii[1] >> 4) | 0x30;
		return;
	}
	if (0x10 > ubcd)
	{
		ascii[1] = ubcd + 0x30;
		ascii[0] = 0x30;
		return;
	}
	
}

void BCDtoTimeandDate(char day, char mounth, char msec, char min, char hour,unsigned char * time)
{
//convert the read data from PCF8583 to a string *time for use in printram()
// static int count;
//  clockt++;
  cli();
  int runner = 0;
  sei();
    if(runner >= 100)
       {	  
		  msec=0x20;
       }
    else
	{
	if(runner>=50) msec = 0x3A;			  
    else msec=0x20;
	}

    //convertBCDtoASCII(day, &time[0]);
	//time[2] = 0x2E;
	//convertBCDtoASCII(mouth, &time[3]);
	//time[5] = 0x20;	
	convertBCDtoASCII(hour, &time[0]);
	time[2] = msec;
	convertBCDtoASCII(min, &time[3]);	
	time[5]=0x00;
}

void convertInttoASCII(int number, unsigned char * ascii)
{
/*convert from int (2 bytes,number from 0 to 65535) to a string of ASCII 
characters for LIR display in HEX format*/
	//number = 39664;
	int n = 0;
	static unsigned char low;
	static unsigned char hi;
	static unsigned char lowbyte;
	static unsigned char hibyte;
	if (number <= 255)
	{
		hi = HI(number);
		low = LO(number);
		convertHextoASCII(hi, ascii,n);
		convertHextoASCII(low, ascii,n);
		n = 0;
		ascii[2] = 0x00;
		return;
	}
	if (number > 255 && number <= 65535)
	{
		hibyte = HIBYTE(number);
		hi = HI(hibyte);
		low = LO(hibyte);
		convertHextoASCII(hi, ascii, n);
		convertHextoASCII(low, ascii, n);

		lowbyte = LOBYTE(number);
		hi = HI(lowbyte);
		low = LO(lowbyte);
		convertHextoASCII(hi, ascii, n);
		convertHextoASCII(low, ascii, n);

		n = 0;
		ascii[4] = 0x00;
		return;
	}

}

void convertHextoASCII(unsigned char symbol, unsigned char * ascii,int &n)
{
//convert from Hex (byte) to ASCII (byte)
	if (symbol > 9 && symbol < 16)
	{
		ascii[n] = symbol + 87;
		n++;
		return;
	}
	if (symbol >= 0 && symbol < 10)
	{
		ascii[n] = symbol + 48;
		n++;
		return;
	}
}

void ReadTwiPCF8583(volatile int &twitask)
{
//reading minutes, hours, milliseconds, day, month, in BCD format from PCF8583 using the Twi bus
switch(twitask)
{

    case ReadMonth:
	{
	    TWI_sendaddres(PCF8583,Readmode);
	    TWI_readdata(NoMasterAckStop);//69us
	    twitask = SendAddresMsek;
	    break;
	}
    case SendAddresMonth:
	{
	    TWI_sendaddres(PCF8583,Writemode);
	    TWI_senddata(0x06);
	    TWI_stoplink();
	    twitask = ReadMonth;
	    break;
	}
    case ReadDay:
	{
	    TWI_sendaddres(PCF8583,Readmode);
	    TWI_readdata(NoMasterAckStop);//69us
	    twitask=SendAddresMonth;
	    break;
	}
    case SendAddresDay:
	{
		TWI_sendaddres(PCF8583,Writemode);
		TWI_senddata(0x05);
		TWI_stoplink();
		twitask=ReadDay;
		break;
	}
	case ReadHour:
	{
		TWI_sendaddres(PCF8583,Readmode);
		TWI_readdata(NoMasterAckStop);//69us
		twitask=SendAddresDay;
		break;
	}
	case SendAddresHour:
	{
		TWI_sendaddres(PCF8583,Writemode);
		TWI_senddata(0x04);
		TWI_stoplink();
		twitask=ReadHour;
		break;
	}
	case ReadMin:
	{
		TWI_sendaddres(PCF8583,Readmode);
		TWI_readdata(NoMasterAckStop);//69us
		twitask=SendAddresHour;
		break;
	}
	case SendAddresMin:
	{
		TWI_sendaddres(PCF8583,Writemode);
		TWI_senddata(0x03);
		TWI_stoplink();
		twitask=ReadMin;
		break;
	}
	case ReadMsek:
	{
		TWI_sendaddres(PCF8583,Readmode);
		TWI_readdata(NoMasterAckStop);//69us
		twitask=SendAddresMin;
		break;
	}
	case SendAddresMsek:
	{
		TWI_sendaddres(PCF8583,Writemode);
		TWI_senddata(0x01);
		TWI_stoplink();
		twitask=ReadMsek;
		break;
	}
	default:
	break;
}

}