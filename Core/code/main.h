/*
 * main.h
 *
 * Created: 14.06.2017 16:07:24
 *  Author: Андрей
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
int main_t();
#define USART_DEBUG(x) USARTD0.DATA = x;
//#ifdef cli
//#undef cli
//#define cli() SREG &= ~(0b10000000);
//#endif

//#ifdef sei
//#undef sei
//#define sei() SREG |= (0b10000000);
//#endif

#define N_AXIS 4  // кол - во каналов с датчиком, в циклах использовать 'с'
#define N_STR  4  // кол - во отображаемых строк, в циклах использовать 's'

#define ZONES 5 //кол-во зон торможения, индексы с 0-3 зоны с 1по4  , индекс 4 - зона стоп

#if (configUSE_Shadow_axis == 1)
#define N_SA   1  // кол - во скрытых(Shadow) осей без датчика
#define SW_AXIS0 ((N_AXIS + N_SA) - 1) //скрытая ось в массиве
#else
#define N_SA   0
#define SW_AXIS0 (N_AXIS + N_SA) //скрытая ось в массиве
#endif

#define AXIS1_bit    1  //канал 0
#define AXIS2_bit    2	//канал 1
#define AXIS3_bit    4	//канал 2
#define AXIS4_bit    8	//канал 3
#define AXIS5_bit    16  // слепая ось (без датчика)


//24.04.2017
#define MAX_LONG_PLUS 2000000000L
#define MAX_LONG_MINES -2000000000L

#define SPEED_AVEREGE 10
extern volatile unsigned char invert_char;
#define INVERT_CHAR() invert_char = true;
#define NORMAL_CHAR() invert_char = false;

#define X_BUTTON_PLACE 275
#define Y_ONE 0
#define Y_TWO 7
#define Y_THREE 14
#define Y_FOUR 21
//--21.04.2016
#define STACK_START  0x24ff
#define STACK_END   0X2000
#define MIN_LIMIT_STACK_SIZE 1024
//-------------------------------
extern void delay_ms(long d);
extern "C"{
extern	void  delay_mks(int d);
extern char  to_get_byte_from_RAM (int addr);
}
enum BUTTON_PIC {
	_pic_ABS_REL,
	_pic_TO_ZERO,
	_pic_OFFSET,
	_pic_ZONE_REF,
	_pic_REF_REF,
	_pic_L_CORR,
	_pic_N_CORR,
	_pic_HALF,
	_pic_FULL,
	_pic_REF,
	_pic_CUR_CORR, //текущие координаты
	_pic_ZAD_CORR, //задание
	_pic_REM_CORR, //осталось проехать
	_pic_PRED,
	_pic_PROG,
	_pic_COMMENT,
	_pic_FR_OFF,
	_pic_FR_ON,
	_pic_HANDWEEL_OFF,
	_pic_AUTO,	
	_pic_SHOW_PROG,
	_pic_HANDWEEL_ON,
	_pic_HWx100,
	_pic_HWx10,	
	_pic_HWx1,
	_pic_EMPTY,
};
//----------------------------------
enum NAMELED{
	 _led_BLANK1_L           = 0,
	 _led_BLANK2_L           = 4,
	 _led_BLANK3_L           = 8,
	 _led_BLANK4_L           =  12,
	 _led_START              =  1,
	 _led_STOP               =  5,
	 _led_MODE_HAND          =  9,
	 _led_MODE_DIAGNOSTICS   =  13,
	 _led_REF                =  2,
	 _led_F                  =  6,
	 _led_X                  =  3,
	 _led_Y                  =  7,
	 _led_Z                  =  10,
	 _led_4                  =  15,
	 _led_LEFT               =  14,
	 _led_RIGHT              =  11
};

    enum FLASH_fifo{
               FIFO_LIMIT=0x08,
               //FLASH_BUSY_LIMIT=10,
               NO_FLASH_ERROR=(unsigned char)0x00,
               FLASH_ERROR_0=0x81,
               FLASH_ERROR_1=0x82,
               FLASH_ERROR_2=0x83,
               FLASH_ERROR_3=0x84,
               FLASH_ERROR_4=0x85,
               FLASH_ERROR_5=0x86,
               FLASH_ERROR_6=0x87,
               FLASH_ERROR_7=0x88,
               FLASH_ERROR_8=0x89,
               FLASH_ERROR_9=0x8a,
               PAGE_ADRESS_WRONG = (unsigned char)0x01,
               BUF_ADRESS_WRONG = (unsigned char)0x02,
               PAGE_TO_BUF_WRONG = (unsigned char)0x03,
               WRITE_ERROR = (unsigned char)0x04,
               FLASH_IS_BUSY = (unsigned char)0x05,
               PAGE_TO_BUF_TIMEOUT = (unsigned char)0x06,
               PAGE_ADRESS_WRONG_R  = (unsigned char)0x07,
               BUF_ADRESS_WRONG_R = (unsigned char)0x08,
               READ_ERROR = (unsigned char)0x09,
               FLASH_IS_BUSY_R= (unsigned char)0x0a,
               COMPARE_IS_GOOD=(unsigned char)0x0b,
               COMPARE_IS_NOT_GOOD=(unsigned char)0x0c,
               CRC_READ_ERROR=(unsigned char)0x0d,
               TIMEOUT_READ_ERROR=(unsigned char)0x0e,
               NUM_FRAME_OR_PRG_TOO_BIG_W=(unsigned char)0x0f,
               NUM_FRAME_OR_PRG_TOO_BIG_R=(unsigned char)0x10,
               BUSY=(unsigned char)0x11,
               READY=(unsigned char)0x12,
               TIMEOUT_ERASE_ERROR=(unsigned char)0x13,
               WRITE_INSERT_ERROR=(unsigned char)0x14,
               GHANGE_MODE_TO_REPLACE=(unsigned char)0x15,
               TIMEOUT_WRITE_TO_BUF1_ERROR=(unsigned char)0x16,
               TIMEOUT_TRANSFER_ERROR=(unsigned char)0x17,
               TIMEOUT_CYCLE_WAIT_ERASE=(unsigned char)0x18,
               TIMEOUT_CYCLE_WAIT_READ_PAGE=(unsigned char)0x19,
               PAGE_LIMIT_EXCEED=(unsigned char)0x1a,
               WRITE_BUF1_TO_PAGE_ERR=(unsigned char)0x1b,
               TRANSFER_DATA_TO_BUF1_ERR=(unsigned char)0x1c,
               INSERT_MISTAKE=(unsigned char)0x1d,
               FULL_CHIP_ERASE_TIMEOUT=(unsigned char)0x1e,
               TIMEOUT_CYCLE_WAIT_ERASE_FIRST=(unsigned char)0x1f,
               NOT_EMPTY=(unsigned char)0x20,
               INSERT_FRAME_TO_PROGRAMM_NOT_ALLOWED=(unsigned char)0x21,
               INSERT_FRAME_TO_FUNCTION_NOT_ALLOWED=(unsigned char)0x22,
               PAGE_LIMIT_EXCEED_LAST=(unsigned char)0x23,
               END_CHECK_ERASE_WRITE=(unsigned char)0x24,
               PAGE_ADRESS_WRONG_DELETE=(unsigned char)0x25,
               FLASH_IS_BUSY_DELETE=(unsigned char)0x26,
               TIMEOUT_ERASE_ERROR_DELETE=(unsigned char)0x27,
               ERROR_CHECK_ONES=(unsigned char)0x28,
               ERROR_CHECK_31_32=(unsigned char)0x29,
               NUM_PRG_TOO_BIG_D=(unsigned char)0x30,
               I_GREATER_OR_EQUEL_EMPTY_PLACE=(unsigned char)0x31,
               FRAME_IS_ABSENT=(unsigned char)0x32,
               DELETE_ONE_FRAME_SUCSESS=(unsigned char)0x33,
               DELETE_ONE_FRAME_ERROR=(unsigned char)0x34,
               TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ=(unsigned char)0x34,
               FIRST_BYTE_ERROR_=(unsigned char)0x35,
               LENGTH_STREAM_BUFFER_OVERFLOW_=(unsigned char)0x36,
               LENGTH_STREAM_TIMEOUT_=(unsigned char)0x37,
               LENGTH_ONES_ERROR_=(unsigned char)0x38,
               MAIN_STREAM_BUFFER_OVERFLOW_=(unsigned char)0x39,
               MAIN_STREAM_TIMEOUT_=(unsigned char)0x3a,
               MAIN_ONES_ERROR_=(unsigned char)0x3b,
               UNKNOWN_FLASH_FIFO=(unsigned char)0x3c,
               BYTE_IS_NOT_85=(unsigned char)0x3d,
               WRITE_PAGE_OK=(unsigned char)0x3e,
               WRITE_PAGE_ERROR=(unsigned char)0x3f,
               TIMEOUT_TRANSFER_TO_BUF=(unsigned char)0x40,
               TRANSMITER_TIMEOUT_COMP_LINK=(unsigned char)0x41,
               TIMEOUT_READ_FROM_BUF1_ERROR=(unsigned char)0x42,
               CHECK_ERROR=(unsigned char)0x43,
               TIMEOUT_CYCLE_WAIT_WRITE=(unsigned char)0x44,
               TIMEOUT_CYCLE_WAIT_COMPARE=(unsigned char)0x45,
               CHECK_BUFFER_PAGE_ERROR=(unsigned char)0x46,
               CHECK_EPE_ERROR=(unsigned char)0x47,
               TRANSMITER_TIMEOUT_WHEN_EEPROM_IS_READ=(unsigned char)0x48,
               EEPROM_WRITE_ERROR=(unsigned char)0x49,
               EEPROM_RECIEVE_ERROR=(unsigned char)0x4a,
               FIRST_BYTE_ERROR_OVERFLOW_=(unsigned char)0x4b,
               FIRST_BYTE_ERROR_FRAME_=(unsigned char)0x4c,
               TRANSFER_DATA_TO_BUFFER1_WRITE_INSERT_ERROR =(unsigned char)0x4d,
               WRITE_INSERT_ERROR_NEW =(unsigned char)0x4E,
               WRITE=255,
               READ=254,
               DELETE=253,
               MODE_INSERT=252,
			   TIME_IS_SET = (unsigned char)0x4f,
			   WRONG_BLOCK_NUMBER=(unsigned char)0x50,
			   WRONG_SIGN_CORRECTION=(unsigned char)0x51,
			   LENGHT_IS_NOT_5_OR_7_=(unsigned char)0x52,
			   BRIDGE,
    };
enum NAMEKEY{
//30.03
     KEY_UP_OK                =        64 ,
     EMPTY_KEY                =        0  ,
// 	_key_REF                  =        5  ,
	_key_REF                  =        20 ,
//	_key_HANDWEEL             =        13 ,
//	_key_MODE_DIAGNOSTICS     =        21 ,
    _key_MODE_DIAGNOSTICS     =        12 ,
//	_key_MODE_HAND            =        29 ,
    _key_MODE_HAND            =        4 ,
	_key_START                =        37  ,
	_key_STOP                 =        45  ,

//	_key_MODE_AUTO            =        4   ,
//	_key_MODE_SINGLE_BLOCK    =        12  ,
//	_key_SKIP_BLOCK           =        20  ,
//now it is F key
//	_key_MODE_PRG             =        28  ,
//-----------------------------------------
	_key_BLANK1               =        35  ,
	_key_BLANK2               =        43  ,
	_key_BLANK3               =        44  ,
	_key_BLANK4               =        36  ,
	_key_1                    =        27  ,
	_key_2                    =        19  ,
	_key_3                    =        11  ,
	_key_4                    =        42  ,
	_key_5                    =        41  ,
	_key_6                    =        46  ,
	_key_7                    =        34  ,
	_key_8                    =        33  ,
	_key_9                    =        38  ,
	_key_0                    =        25  ,
	_key_POINT                =        30  ,
	_key_PLUS_MINUS           =        26  ,
	_key_X                    =        18  ,
	_key_Y                    =        22  ,
	_key_Z                    =        2   ,
	_key_F                    =        28   ,
	_key_UP                   =        17  ,
	_key_DOWN                 =        1   ,
	_key_LEFT                 =        10  ,
	_key_RIGHT                =        14  ,
	_key_ENTER                =        9   ,
	_key_LEFT_UP              =        _key_LEFT + KEY_UP_OK ,
	_key_RIGHT_UP             =        _key_RIGHT + KEY_UP_OK,
	_key_ENTER_UP             =        _key_ENTER + KEY_UP_OK,
//12.04.2017
    _key_C                    =        3   ,
    _key_BLANK1_L             =        5   ,
	_key_BLANK2_L             =        13  ,
	_key_BLANK3_L             =        21  ,
	_key_BLANK4_L             =        29  ,
	_key_F4                   =        6  ,

};
 enum STEP_KEY_PRESS{
      SCAN_READ_ZERO,
      SCAN_READ_FIRST,
      SCAN_READ_THIRD,
      SCAN_READ_ERROR,
//      KEY_IS_PRESSED,
//      KEY_IS_RELEASED,
      WAIT_RELEASE,
      TIME_OUT_EXIT,
      PROCESSING_PRESS,
      PROCESSING_RELEASED,
      PROCESSING_HOLDDOWN,
      FREE,

};

 enum TWI_TALK{
     UNKNOWN_TWI=(unsigned char)0x00,
     IDLE_TWI=(unsigned char)0x01,
     OWNER_TWI=(unsigned char)0x02,
     BUSY_TWI=(unsigned char)0x03,
     TWI_STATE_ERROR=(unsigned char)0x04,
     ARBITRATION_LOST_TWI=(unsigned char)0x05,
     BUS_ERROR_TWI=(unsigned char)0x06,
     TIMEOUT_TWI=(unsigned char)0x07,
     RECEIVED_ACKNOWLEDGE=(unsigned char)0x08,
     TIMEOUT_ACKNOWLEDGE_TWI=(unsigned char)0x09,
     CLOCK_HOLD_ERROR_TWI=(unsigned char)0x0a,
     COMMAND_IS_SENT_TWI=(unsigned char)0x0b,
     SLAVE_ADRESS_IS_SENT_TWI=(unsigned char)0x0c,
     DATA_IS_READ_TWI=(unsigned char)0x0d,
     KEY_IS_NOT_PRESSED=(unsigned char)0x0e,
//     KEY_IS_PRESSED=(unsigned char)0x0f,
//     EMPTY=(unsigned char)0x0f,
     SDA_LINE_IS_LOW=(unsigned char)0x10,
     KEY_IS_HOLD=(unsigned char)0x11,
     KEY_IS_PRESSED=(unsigned char)0x12,
     KEY_IS_RELEASED=(unsigned char)0x13,
     O_KEY_LED_TWI=(unsigned char)0x14,
     START_KEY_SEQUENCE=(unsigned char)0x15,
     WAIT_HOLDDOWN=(unsigned char)0x16,
     HOLDDOWN=(unsigned char)0x17,
     EMPTY_CODE=(unsigned char)0x18,
     RELEASE=(unsigned char)0x19,
     ERROR_KEY=(unsigned char)0x1a,
     DATA_IS_WRITTEN_TWI=(unsigned char)0x1b,
 };

class CInit{

public:
//28.08.2018
   bool uart_485;
//16.08.2018   
  bool ref_mode[N_AXIS];  //отображение оси в 1- абсолютной системе или 0 - относительной  
  bool mRef_ext[N_AXIS];  // 1 - если есть метка и выбрана абсолютная координата на экране , 0 - если выбрана относительная координата или нет метки
//---------------    
   unsigned char contrast_value = 1;
   long frequensy_debug,frequensy_debug_;
//09.10.2018
   unsigned long p_in; // входа пульта
   unsigned long m_in ; //входные маркеры контроллера
   bool to_see_time; //отображение времени
//30.11.2018
   char save_is_frosted;//на 7 - выключение блока
   bool first_condition_to_switch_off,to_clear_switch_off_string;
   int time_metre; 
//03.12.2018
   unsigned char to_see_enigma ;//= 4;  
int check_stack(int *addr,int *end_check){
	char symbol,example=0;
	for (int n=0;n<*end_check;n++){
		 symbol=to_get_byte_from_RAM(*addr);
		 if(symbol==example){
	     *addr=*addr+1;
		 example++;		 
		 }
		 else break;
	}
//first address where data is not the same with example 
//it means that it is stack's boundary	
	return *addr;
	
}
enum AXIS_NAME{
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_U,
	AXIS_V,
	AXIS_W,
	AXIS_P,
	AXIS_Q,
	AXIS_R,
	AXIS_A,
	AXIS_B,
	AXIS_C,
	AXIS_D,
	NO_AXIS,	 
};
#define no_axis CInit::NO_AXIS   
   
  void init_32khz(void){
// Enable the internal 32 kHz RC oscillator
OSC.CTRL|=OSC_RC32KEN_bm;
// Wait for the internal 32 kHz RC oscillator to stabilize
while (1)
{

  PORTD.DIRSET=0x80;
  if ((OSC.STATUS & OSC_RC32KRDY_bm)==0)
  {
//25.11.2014 it will be frequency
//in endless loop
  PORTD.OUTTGL=0x80;
   }
  else {

    break;

       }
  }

}
 void init_32mhz_and_DFLL(void){
//unsigned char n;
// Enable the internal 32 mHz RC oscillator+32 kHz RC oscillator was enabled before
OSC.CTRL|=OSC_RC32MEN_bm;
// Internal 32 MHz RC osc. calibration reference clock source: 32.768 kHz Internal Osc.
OSC.DFLLCTRL = 0x00;
// Enable the autocalibration of the internal 32 MHz RC oscillator
DFLLRC32M.CTRL|=DFLL_ENABLE_bm;
// Wait for the internal 32 mHz RC oscillator to stabilize
while (1)
{
   PORTD.DIRSET=0x80;
  if ((OSC.STATUS & OSC_RC32MRDY_bm)==0)
   {
//25.11.2014 it will be frequency on buzzer
//in endless loop
              PORTD.OUTTGL=0x80;
   }
  else {

    break;
       }
}
  CCP=CCP_IOREG_gc;
//32 mgh generator is chosen
  CLK.CTRL =CLK_SCLKSEL_RC32M_gc;
//configuration must not be changed until next reset
  CCP=CCP_IOREG_gc;
  CLK.LOCK=0x01;

}
void  init_PORTC(void){
// PORTC initialization
// OUT register
// Bit0: Output, enables output buffers of data latch when 0
// Bit1: Input
// Bit2: Input
// Bit3: Output
// Bit4: Input
// Bit5: Input
// Bit6: Input
// Bit7: Input
PORTC.OUT=0x01;
PORTC.DIR=0x09;
}
void toggle_A0(void){
  PORTA.DIRSET=0x01;
  PORTA.OUTTGL=0x01;
}
void set_A0(void){
  PORTA.DIRSET=0x01;
  PORTA.OUTSET=0x01;
}
void clr_A0(void){
  PORTA.DIRSET=0x01;
  PORTA.OUTCLR=0x01;
}
void set_A1(void){
  PORTA.DIRSET=0x02;
  PORTA.OUTSET=0x02;
}
void clr_A1(void){
  PORTA.DIRSET=0x02;
  PORTA.OUTCLR=0x02;
}
void set_A2(void){
  PORTA.DIRSET=0x04;
  PORTA.OUTSET=0x04;
}
void clr_A2(void){
  PORTA.DIRSET=0x04;
  PORTA.OUTCLR=0x04;
}
//26.04.2017
bool input_B1_inc_f1(void){

	 #if (configUSE_Conf01 == 1)
	 return false;
	 #endif
//inc_f1
 PORTB.PIN1CTRL=0x18;
 PORTB.DIRCLR=0x02;
//unsigned char a;
//  a=PORTB.IN & 0x02;
// if (a&0x02) return true;
// else return false;
   if(PORTB.IN & 0x02)return true;
   else return false;
}
bool input_A3_inc_f2(void){ 

	 #if (configUSE_Conf01 == 1)
	 return false;
	 #endif
//inc_f2
 PORTA.PIN3CTRL=0x18;
 PORTA.DIRCLR=0x08;
//unsigned char a;
//  a=PORTA.IN & 0x08;
// if (a&0x08) return true;
// else return false;
    if(PORTA.IN & 0x08)return true;
	else return false;
}
bool input_A2_inc_f3(void){

	 #if (configUSE_Conf01 == 1)
	 return false;
	 #endif
//inc_f3
 PORTA.PIN2CTRL=0x18;
 PORTA.DIRCLR=0x04;

   if(PORTA.IN & 0x04)
	return true;
   else 
	return false;  
}
// PD4->PC3 сделать
// для mcx514 PC3->PD4
bool input_D4_inc_f0(void){ 

 #if (configUSE_PLC == 1)
 #if (configUSE_mcx514 == 0)
	return false;
 #endif
 #endif
 #if (configUSE_Conf01 == 1)
	return false;
 #endif
 #if (configUSE_mcx514 == 0)
 PORTC.PIN3CTRL=0x18;//PORTD.PIN4CTRL=0x18;
 PORTC.DIRCLR=0x08;//PORTD.DIRCLR=0x10;

   if(PORTC.IN & 0x08)//if(PORTD.IN & 0x10)
		return true;
   else 
		return false;
 #endif
  #if (configUSE_mcx514 == 1)
  PORTD.PIN4CTRL=0x18;
  PORTD.DIRCLR=0x10;

  if(PORTD.IN & 0x10)
	return true;
  else
	return false;
  #endif
}

//--------------
void set_D7(void){
  PORTD.DIRSET=0x80;
  PORTD.OUTSET=0x80;
}
void clr_D7(void){
  PORTD.DIRSET=0x80;
  PORTD.OUTCLR=0x80;
}
void set_B5(void){
  PORTB.DIRSET=0x20;
  PORTB.OUTSET=0x20;
}
void clr_B5(void){
  PORTB.DIRSET=0x20;
  PORTB.OUTCLR=0x20;
}
void set_B4(void){
  PORTB.DIRSET=0x10;
  PORTB.OUTSET=0x10;
}
void clr_B4(void){
  PORTB.DIRSET=0x10;
  PORTB.OUTCLR=0x10;
}
void set_B6(void){
	PORTB.DIRSET=0x40;
	PORTB.OUTSET=0x40;
}
void clr_B6(void){
	PORTB.DIRSET=0x40;
	PORTB.OUTCLR=0x40;
}


void read_only_altera_bus(void){
        PORTB.OUTSET=0x80;
        PORTB.DIRSET=0x80;
}
void wait_syng_inside_generator(){
  for(;;){
    if(RTC.STATUS==0x00)return;
//it will give 1 mks loop
    delay_mks(1);
  PORTD.DIRSET=0x80;
  PORTD.OUTTGL=0x80;
         }
}
//27.08.2018
void set_C2(void){
 PORTC.OUTSET=0x04;
 PORTC.DIRSET=0x04;	
}
void clr_C2(void){
 PORTC.OUTCLR=0x04;
 PORTC.DIRSET=0x04;	
}
void C2_is_input(void){
 PORTC.DIRCLR=0x04;		
}
void set_C3(void){
	PORTC.OUTSET=0x08;
	PORTC.DIRSET=0x08;
}
void clr_C3(void){
	PORTC.OUTCLR=0x08;
	PORTC.DIRSET=0x08;
}

void adm2687_transmitter(void){
	#if (configUSE_mcx514 == 0)
	PORTD.OUTSET = 0x01;
	PORTD.DIRSET = 0x01;	
	#else
	PORTB.OUTSET = 0x20;
	PORTB.DIRSET = 0x20;
	#endif
}
void adm2687_receiver(void){
	#if (configUSE_mcx514 == 0)
	PORTD.OUTCLR = 0x01;
	PORTD.DIRSET = 0x01;
	#else
	PORTB.OUTCLR = 0x20;
	PORTB.DIRSET = 0x20;
	#endif
}
//09.10.2018
void B7_is_prepearing_for_ADC(void){
//0x18 pullup,0x10 pulldown,0 n/a
PORTB.PIN7CTRL=0;
PORTB.DIRCLR=0x80;	
}

};
extern CInit init;
#endif /* MAIN_H_ */
