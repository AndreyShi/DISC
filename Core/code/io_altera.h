#ifndef _io_altera_h
#define _io_altera_h
#include "main.h"
#include "translate.h"
#include "counter.h"
#include "config.h"
extern "C"{
unsigned int to_take_address(char *);	
}
 class CIOAltera{
  
//	enum READ_ADR {
//                INPUT16_23  = 0,
//		        INPUT0_7  = 1,
//		        INPUT8_15 = 2,
//                STATUS    = 3,  // 0x80 - ref status, 0x40 - ssi_data
//                LOW_CNT   = 4,
//                HIGH_CNT  = 5,
//                H_LOW_CNT   = 6,
//                H_HIGH_CNT  = 7,
//                REF_LOW_CNT   = 8,
//                REF_HIGH_CNT  = 9,
//                REF_H_LOW_CNT   = 10,
//                REF_H_HIGH_CNT  = 11,
//                JOG_LOW_CNT   = 12,
//                JOG_HIGH_CNT   = 13,

//                LAST_READ_ADR = ~0
 //                     };
public:
	enum READ_ADR {
                 READ_LOW_CNT0 = 0,
		         READ_MID_CNT0=  1,
		         READ_HIGH_CNT0= 2,
				 READ_TOP_CNT0=28,
//----------------------------------
                 READ_LOW_CNT1 = 3,
		         READ_MID_CNT1=  4,
		         READ_HIGH_CNT1= 5,
				 READ_TOP_CNT1=29,
//----------------------------------
                 READ_LOW_CNT2 =6,
		         READ_MID_CNT2= 7,
		         READ_HIGH_CNT2=8,
				 READ_TOP_CNT2=30,
//----------------------------------
                 READ_LOW_CNT3=9,
		         READ_MID_CNT3=10,
		         READ_HIGH_CNT3=11,
				 READ_TOP_CNT3=31,
//----------------------------------
                 READ_ETALON_LOW=16,
				 READ_ETALON_MID=17,
				 READ_ETALON_HIGH=18,
                      };
	
	bool latch;
	enum WRITE_ADR{
                LATCH_CNT = 2,
				K_SET_LOW = 0,
				K_SET_HIGH = 1,
				K_SET_MID = 3,
				START_TO_GO = 4,
				READ_SSI = 4,
                      };
	enum READ_FLAG{
		        ETALON_IS_READY=15,
	};
    enum READ_INPUT{
                READ_INPUT_0_7=12,
                READ_INPUT_8_15=13,
				READ_INPUT_16_23=14,
                SIZE_INPUT_BUF=5,
    };
	enum WRITE_ALTERA{
		WRITE_OUTPUT_0_7=5,
		WRITE_OUTPUT_8_15=6,
		WRITE_OUTPUT_16_23=7,
		BIT_ABS_0 = 8,
		BIT_ABS_1 = 11,
		BIT_ABS_2 = 14,
		BIT_ABS_3 = 17,
	};
 unsigned char input_low_buf[SIZE_INPUT_BUF],input_high_buf[SIZE_INPUT_BUF],input_veryhigh_buf[SIZE_INPUT_BUF];
 unsigned char input_low,input_high,input_veryhigh;
 
public:
//15.01.2018
//unsigned char to_see_k_low,to_see_k_mid,to_see_k_high;
//26.04.2018
enum CORRECTION_IS_WORKING{
	TABLE_IS_ENDED_PLUS=0,
	SIGN_PLUS_WRONG=1,
	SIGN_MINES_WRONG=2,
	OK=3,
	DELTA_BELOW_ZERO=4,
	MULT_WRONG=5,
	INTERVAL_IS_ZERO=6,
	COEFF_IS_ZERO=7,
	RESULT_IS_UNREAL=16,
	UNREAL_DOT=17,
	SIGN_IS_WRONG=18,
	TABLE_IS_ENDED_MINES=19,
	NO_TABLE_PLUS_NO_TABLE_MINES=20,
	TABLE_IS_ENDED_PLUS_L=21,
	TABLE_IS_ENDED_MINES_R=22,
};
CORRECTION_IS_WORKING result,buf;
//04.01.2018
enum ERROR_CORR_REAL_TIME{
	NO_ERROR,
	NO_FLASH_ANSWER,
	TIMEOUT_WHEN_HEADER_IS_READ,
	NO_TABLE_PLUS_NO_TABLE_MINES_,
	DIFFER_COEFF,//коэффициент таблицы и осевой различаются
	TABLE_IS_ENDED_PLUS_,
	TABLE_IS_ENDED_MINES_,
	INTERVAL_IS_ZERO_,
	SIGN_PLUS_WRONG_,
	SIGN_MINES_WRONG_,
	DELTA_BELOW_ZERO_,
	RESULT_IS_UNREAL_,
	MULT_WRONG_,
	NO_FLASH_ANSWER_COMMON,
	NO_REF,
	TABLE_IS_ENDED_PLUS_L_,
	TABLE_IS_ENDED_MINES_R_,
	SOME_ERROR,
	CORR_OFF,
	CANT_BE_DONE,	
	};
ERROR_CORR_REAL_TIME error_corr_real_time[N_AXIS];//,mem[N_AXIS];


//---------------
//10.04.2018
CORRECTION_IS_WORKING  correction_is_working_skv(int8_t c);
//14.05.2018
CORRECTION_IS_WORKING  correction_is_working_complex(void);
//15.05.2018
CORRECTION_IS_WORKING  correction_is_working_right(void);
//04.02.2019

bool correction_can_be_done_in_real_time[N_AXIS],correction_can_be_done_in_real_time_[N_AXIS];
//---------------------------------------------------------
unsigned char  correction_is_taking(unsigned int *,unsigned long *);
unsigned char  correction_set_k(int n);
void set_flags(unsigned char command);
bool correction_is_waiting_flag(void);
//28.04.2018
long  coefficient_is_calculating_shot(long down,long counter,long interval);
unsigned char  coefficient_is_calculating_long(long long *d,long delta,long interval);
//15.01.2019
void to_see_corrections_errors(long *);
//-------------------------------------
long counter[N_AXIS];
long counter_display[N_AXIS];

  void init_altera_bus_in_main(void){

    //    mcu_bus.set_bus_keeper_if_input();
    //0x08-bus-keeper,0x18-pullup,0x10-pulldown
    PORTE.PIN0CTRL=0x18;
	PORTE.PIN1CTRL=0x18;
    PORTE.PIN2CTRL=0x18;
	PORTE.PIN3CTRL=0x18;
    PORTE.PIN4CTRL=0x18;
	PORTE.PIN5CTRL=0x18;
    PORTE.PIN6CTRL=0x18;
	PORTE.PIN7CTRL=0x18;
    //    mcu_bus.set_dir_input();
    PORTE.DIR=0x00;
    //    rw.set_pull_up_if_input();
    //    rw.set_dir_input();
    PORTR.PIN0CTRL=0x18;
    PORTR.DIRCLR=0x01;
    //    sync.set_pull_up_if_input();
    //    sync.set_dir_input();
    PORTR.PIN1CTRL=0x18;
    PORTR.DIRCLR=0x02;
//-------------------------
    //    rw.set();
    //    rw.set_dir_output();
    PORTR.OUTSET = 0x01;
    PORTR.DIRSET=0x01;
    //    sync.clr();
    //    sync.set_dir_output();
    PORTR.OUTCLR=0x02;
    PORTR.DIRSET=0x02;
        }


public:
      long read_cnt_quick(char channel);
      unsigned char read_input(READ_INPUT adr);
 

unsigned long take_input(void){
//23.11.2017 it takes 1473*31.25=46031.25 ns(iar)
//10.10.2018 it takes 37.6 mks
    unsigned long result;
    input_is_making(READ_INPUT_0_7,input_low_buf,&input_low);
    input_is_making(READ_INPUT_8_15,input_high_buf,&input_high);
	//должнобыть доделано в альтере входы с 16 по 23, пока заглушка в 0
    //input_is_making(READ_INPUT_16_23,input_veryhigh_buf,&input_veryhigh);	
	input_veryhigh = 0;
	//
	result = ((uint32_t)input_veryhigh << 16);
	result |= (input_high << 8);
	result = result | input_low;

    return result;
}
void  input_is_making(READ_INPUT adr,unsigned char *input,unsigned char *input_preliminary){

     unsigned char a,b,c,in0,in1,in2,in3,b_,c_,in4,in5,in6,in7;
//It takes 60*31.25=1875 ns
     a=~read_input(adr);

	// *input_preliminary = a;
	// return; // при работующем фильтре просходит 2 дополнительных нажатия , пока фильтр отключил
//-------------------------
//Buffer is refreshed. Last coming is on the top
   for(unsigned char n=SIZE_INPUT_BUF-1;n!=0;n--) {
       input[n]=input[n-1];
       }
       input[0]=a;
//------Filter is here. It looks like 5-bit shift register
//for each input.If all 5 outputs are the same - output will be passed next
//It is done every 10 ms.Before buffer is refreshed
//--debug
//       input[0]=0x00;input[1]=0x03;input[2]=0x00;
//--------------
//It takes 339*31.25=10593.75 ns
       in0=in1=in2=in3=in4=in5=in6=in7=0;
       for (unsigned char n=0;n<SIZE_INPUT_BUF-1;n++) {
        b=input[n];c=input[n+1];
        b_=b&0x01;c_=c&0x01;
        if (b_==c_) in0++;
        b_=b&0x02;c_=c&0x02;
        if (b_==c_) in1++;
        b_=b&0x04;c_=c&0x04;
        if (b_==c_) in2++;
        b_=b&0x08;c_=c&0x08;
        if (b_==c_) in3++;
//---------------------------
        b_=b&0x10;c_=c&0x10;
        if (b_==c_) in4++;
        b_=b&0x20;c_=c&0x20;
        if (b_==c_) in5++;
        b_=b&0x40;c_=c&0x40;
        if (b_==c_) in6++;
        b_=b&0x80;c_=c&0x80;
        if (b_==c_) in7++;
       }
 //---------------------------
   if (in0==SIZE_INPUT_BUF-1) {
        if (a&0x01)*input_preliminary=*input_preliminary|0x01;
        else *input_preliminary=*input_preliminary & ~0x01;
           }
   if (in1==SIZE_INPUT_BUF-1) {
        if (a&0x02)*input_preliminary=*input_preliminary|0x02;
        else *input_preliminary=*input_preliminary & ~0x02;
           }
   if (in2==SIZE_INPUT_BUF-1) {
        if (a&0x04)*input_preliminary=*input_preliminary|0x04;
        else *input_preliminary=*input_preliminary & ~0x04;
           }
   if (in3==SIZE_INPUT_BUF-1) {
        if (a&0x08)*input_preliminary=*input_preliminary|0x08;
        else *input_preliminary=*input_preliminary & ~0x08;
           }
    if (in4==SIZE_INPUT_BUF-1) {
        if (a&0x10)*input_preliminary=*input_preliminary|0x10;
        else *input_preliminary=*input_preliminary & ~0x10;
           }
   if (in5==SIZE_INPUT_BUF-1) {
        if (a&0x20)*input_preliminary=*input_preliminary|0x20;
        else *input_preliminary=*input_preliminary & ~0x20;
           }
   if (in6==SIZE_INPUT_BUF-1) {
        if (a&0x40)*input_preliminary=*input_preliminary|0x40;
        else *input_preliminary=*input_preliminary & ~0x40;
           }
   if (in7==SIZE_INPUT_BUF-1) {
        if (a&0x80)*input_preliminary=*input_preliminary|0x80;
        else *input_preliminary=*input_preliminary & ~0x80;
           }

}
long ref_ref_count(int counter_ref){
	if(counter_ref==0 || counter_ref==1) 
		return 0;
	return read_cnt_quick(4);
}
void to_send_outputs(unsigned char *out);
void set_ch_abs_bit(int ch,char data);
void write_reg(WRITE_ALTERA addr,unsigned char data);
char read_reg(READ_ADR addr);
};
extern CIOAltera altera;

class CPorta_ref{

public: 
  bool pRef[N_AXIS]; //наличие реф метки на канале
  bool ref_change_mode[N_AXIS]; 
//------------
  unsigned char start_ref,start_ref_;
  unsigned char ref_machine;
//  char save_is_frosted;
//09.04.2018
  char ref_blinker;
  long k_divider,set_point;
//28.05.2018
   bool table_plus; // 1 - выбрана таблица + , 0 - выбрана талица -
   bool ld_table;// 1 - загрузка таблицы + , 0 - загрузка таблицы -
   char tbl[2]; // [0] - 1 - таблица + опрошена , 0 - нет , [1] - 1 - таблица - опрошена , 0 - нет
   
   long k_divider_plus,set_point_plus;
   long k_divider_mines,set_point_mines;
   bool table_is_ready_plus,table_is_ready_mines;
   char t_r_p_auto, // 1 - таблица + автоматический режим, 0 - нет заполненна вручную
   t_r_m_auto;// 1 - таблица - автоматический режим, 0 - нет заполненна вручную
   char table_p_from_flash,table_m_from_flash;// 1 - таблица с флэшь

//07.06.2018
   long address_23LCV1024_mines,address_23LCV1024_plus; 
   long sz_two_ref; //переменная для записи значения расстояния между двумя метками
   //int to_see;  
//-------------------------    
  bool enable_to_go_for_correction;
//17.04.2018
  struct CORR{
//	 long time_to_think[1001]; 
//25.05.2018
     long  pt_plus[5]; 
	 long  pt_mines[5]; 	 
	  } corr;  
//10.04.2018 
enum CORRECTION{
	   WAIT=0,
	   SET_K=1,
	   START_TO_GO=2,
	   IS_MOVING=3,
	   STOP=4,
	   GO_TO_REF=5,
	   SET_POINT=6,
	   SET_MODE_CORRECTION=7,
	   SET_MODE_READOUT=8,
	   IF_STOP_PRESSED=9,
	   IS_MOVING_FIRST_TIME=10,
};
  CORRECTION follower,follower_safety,fol,follow;
enum SIGN{
	PLUS=0,
	MINES=1,  
};
enum FAULT{
	NO_FAULT=0,
	IF_PLUS_FAULT=1,
	IF_MINES_FAULT=2,
	SPEED_LIMIT=3,
	MULT=4,
	NO_CONNECTOR=5,
}; 
FAULT mistake;
 SIGN correction_sign; 
  int blink_time_set_k;
  unsigned int check_counter;//,check_counter_mem;
  bool channel_1_has_switch_for_ref;
//11.04.2018
  long etalon_counter,etalon_meaning,speed;
//  long debug; 
//16.04.2018
  bool inverter_set_k,inverter_set_point; 
//24.10.2018
  long counter_measure[N_AXIS];
  bool measurement_is_ended;      
//24.04.2018   
CPorta_ref::FAULT to_do_table_when_go_to_plus(void);
CPorta_ref::FAULT to_do_table_when_go_to_mines(void);
//23.10.2018
bool enable_measure_int[N_AXIS],ref_mark_measure[N_AXIS];
int ref_ref_count; //счетчик меток
bool ref_ref_enable;
//-----------------   
#pragma GCC push_options
#pragma GCC optimize ("O0")
   void init_PD3_int_save(void){
  PORTD.DIRCLR= 0x08;
// 00 011 010    00-no inverse 011 pull up when input 010 falling edge
  PORTD.PIN3CTRL=0x1a;
// int 0 must be defined for PD3
  PORTD.INT0MASK=0x08;
  }
 void enable_PD3_int_0_save(void){
//07.02.2017 if flag was set it must be cleared first
  PORTD.INTFLAGS=0x01;
// PD3 must be input
  PORTD.DIRCLR= 0x08;
// interrupt must be enabled
  PORTD.INTCTRL=0x03;

  }
  void disable_PD3_int_0_save(void){
// interrupt must be disabled
  PORTD.INTCTRL=0x00;
  }
//----------------------------------
  void init_PA0_int_ref_1(bool level){
  pRef[1]=false;
//if  PD1 is set to 0 ref can't go to ALTERA
// and counter can't be reset.
//if  PD1 is set to 1 ref can go to ALTERA
// mcu is waiting for pulse from ALTERA.
  PORTD.OUTCLR=0x02;
  PORTD.DIRSET= 0x02;
// 00 010 001   010 pull down when input 001 rising edge if interrupt
//  PORTA.PIN1CTRL=0x11;
 if(level==true) 
	//PORTA.PIN0CTRL=0x53; // 01 010 011    01-inverse 010 pull down when input 011 low level(if inverse high level)
	PORTA.PIN0CTRL=0x11;
 else 
	PORTA.PIN0CTRL=0x11;
// int 0 must be defined for PA0
  PORTA.INT0MASK=0x01;
  }
  void enable_PA0_int_1(bool additive_switch_is){
// int 0
//07.02.2017 if flag was set it must be cleared first
//but another channel must be saved(int1)
  unsigned char a=PORTA.INTFLAGS&0x02;
  PORTA.INTFLAGS=0x01|a;
// PA0 must be input
  PORTA.DIRCLR= 0x01;
//21.11.2017 PD1 must be set to one and ALTERA is waiting for ref pulse
//29.11.2017 but if additive switch is,it will be done later
 if(!additive_switch_is){
  PORTD.OUTSET=0x02;
  PORTD.DIRSET= 0x02;
 }
//interrupt must be enabled
//but another channel must be saved(int1)
  a=PORTA.INTCTRL&0x0c;
  PORTA.INTCTRL=0x03|a;
  pRef[1]=false;
  }
  //16.04.2018
   void enable_PA0_int_1_corr(bool additive_switch_is){
	   // int 0
	   //07.02.2017 if flag was set it must be cleared first
	   //but another channel must be saved(int1)
	   unsigned char a=PORTA.INTFLAGS&0x02;
	   PORTA.INTFLAGS=0x01|a;
	   // PA0 must be input
	   PORTA.DIRCLR= 0x01;
	   //21.11.2017 PD1 must be set to one and ALTERA is waiting for ref pulse
	   //29.11.2017 but if additive switch is,it will be done later
	   if(!additive_switch_is){
		   PORTD.OUTSET=0x02;
		   PORTD.DIRSET= 0x02;
//16.04.2018 channel 0		   
		//   PORTD.OUTSET=0x01;
		//   PORTD.DIRSET= 0x01;   
	   }
	   //interrupt must be enabled
	   //but another channel must be saved(int1)
	   a=PORTA.INTCTRL&0x0c;
	   PORTA.INTCTRL=0x03|a;
	   pRef[1]=false;
   }
  void disable_PA0_int_1_corr(void){
	  // interrupt must be disabled
	  unsigned char a=PORTA.INTCTRL&0x0c;
	  PORTA.INTCTRL=0x00|a;
	  // PD1 must be zero
	  PORTD.OUTCLR = 0x02;
	  //16.04.2018 PD0 TOO
	  PORTD.OUTCLR = 0x01;
  }   
 //---------------------------- 
  void disable_PA0_int_1(void){
   #if (configUSE_mcx514 == 1)
   return;
   #endif
// interrupt must be disabled
  unsigned char a=PORTA.INTCTRL&0x0c;
  PORTA.INTCTRL=0x00|a;
// PD1 must be zero
  PORTD.OUTCLR = 0x02;
  }
  void to_stop_PA0_int_1(void){
// interrupt must be disabled
  unsigned char a=PORTA.INTCTRL&0x0c;
  PORTA.INTCTRL=0x00|a;
  }
  void to_continue_PA0_int_1(void){
// interrupt must be enabled
  unsigned char a=PORTA.INTCTRL&0x0c;
  PORTA.INTCTRL=0x03|a;
  }
  bool is_enabled_PA0_int_1(void){
  unsigned char a=PORTA.INTCTRL&0x03;
  if (a==0x03) return true;
  else return false;
  }
//29.11.2017
void altera_on_PA0_int_1(void){
  PORTD.OUTSET=0x02;
  PORTD.DIRSET= 0x02;
}
void altera_off_PA0_int_1(void){
  PORTD.OUTCLR=0x02;
}
//-----------------------------
//17.04.2017 ------------------------
  void init_PA1_int_ref_2(bool limit){
  pRef[2]=false;
//if  PD2 is set to 0 ref can't go to ALTERA
// and counter can't be reset.
//if  PD2 is set to 1 ref can go to ALTERA
// mcu is waiting for pulse from ALTERA.
  PORTD.OUTCLR=0x04;
  PORTD.DIRSET= 0x04;
// 00 010 001   010 pull down when input 001 rising edge if interrupt
//  PORTA.PIN1CTRL=0x11;
// 01 010 011    01-inverse 010 pull down when input 011 low level(if inverse high level)
  if(limit==true)PORTA.PIN1CTRL=0x53;
  else  PORTA.PIN1CTRL=0x11;
// int 1 must be defined for PA1
  PORTA.INT1MASK=0x02;
  }
  void enable_PA1_int_2(bool additive_switch){
//07.02.2017 if flag was set it must be cleared first
  unsigned char a=PORTA.INTFLAGS&0x01;
  PORTA.INTFLAGS=0x02|a;
// PA1 must be input
  PORTA.DIRCLR= 0x02;
//21.11.2017 PD2 must be set to one and ALTERA is waiting for ref pulse
//29.11.2017 but if additive switch is,it will be done later
 if(!additive_switch){
  PORTD.OUTSET=0x04;
  PORTD.DIRSET= 0x04;
 }
// interrupt must be enabled
  a=PORTA.INTCTRL&0x03;
  PORTA.INTCTRL=0x0c|a;
  pRef[2]=false;
  }
  void disable_PA1_int_2(void){
   #if (configUSE_mcx514 == 1)
   return;
   #endif
// interrupt must be disabled
  unsigned char a=PORTA.INTCTRL&0x03;
  PORTA.INTCTRL=0x00|a;
// PD2 must be zero
  PORTD.OUTCLR = 0x04;

  }
  void to_stop_PA1_int_2(void){
// interrupt must be disabled
  unsigned char a=PORTA.INTCTRL&0x03;
  PORTA.INTCTRL=0x00|a;
  }
 void to_continue_PA1_int_2(void){
// interrupt must be enabled
  unsigned char a=PORTA.INTCTRL&0x03;
  PORTA.INTCTRL=0x0c|a;
  }
 bool is_enabled_PA1_int_2(void){
  unsigned char a=PORTA.INTCTRL&0x0c;
  if (a==0x0c) return true;
  else return false;
  }
 //29.11.2017
void altera_on_PA1_int_2(void){
  PORTD.OUTSET=0x04;
  PORTD.DIRSET= 0x04;
}
void altera_off_PA1_int_2(void){
 PORTD.OUTCLR=0x04;
}
//-----------------------------
//18.04.2017 ------------------------
  void init_PB3_int_ref_3(bool limit){
  pRef[3]=false;
//if  PD5 is set to 0 ref can't go to ALTERA
// and counter can't be reset.
//if  PD5 is set to 1 ref can go to ALTERA
// mcu is waiting for pulse from ALTERA.
  PORTD.OUTCLR=0x20;
  PORTD.DIRSET= 0x20;
// 00 010 001   010 pull down when input 001 rising edge if interrupt
//  PORTB.PIN3CTRL=0x11;
// 01 010 011    01-inverse 010 pull down when input 011 low level(if inverse high level)
if(limit==true)  PORTB.PIN3CTRL=0x53;
else  PORTB.PIN3CTRL=0x11;
// int 0 must be defined for PB3
  PORTB.INT0MASK=0x08;
  }
   void enable_PB3_int_3(bool additive_switch){
//07.02.2017 if flag was set it must be cleared first
  unsigned char a=PORTB.INTFLAGS&0x02;
  PORTB.INTFLAGS=0x01|a;
//21.11.2017 PD5 must be set to one and ALTERA is waiting for ref pulse
//29.11.2017 but if additive switch is,it will be done later
 if(!additive_switch){
  PORTD.OUTSET=0x20;
  PORTD.DIRSET= 0x20;
 }
// interrupt must be enabled
  a=PORTB.INTCTRL&0x0c;
  PORTB.INTCTRL=0x03|a;
  pRef[3]=false;
  }
  void disable_PB3_int_3(void){
   #if (configUSE_mcx514 == 1)
   return;
   #endif
// interrupt must be disabled
  unsigned char a=PORTB.INTCTRL&0x0c;
  PORTB.INTCTRL=0x00|a;
// PD5 must be ZERO
  PORTD.OUTCLR = 0x20;
  }
  void to_stop_PB3_int_3(void){
// interrupt must be disabled
  unsigned char a=PORTB.INTCTRL&0x0c;
  PORTB.INTCTRL=0x00|a;
  }
  void to_continue_PB3_int_3(void){
// interrupt must be enabled
  unsigned char a=PORTB.INTCTRL&0x0c;
  PORTB.INTCTRL=0x03|a;
  }
  bool is_enabled_PB3_int_3(void){
  unsigned char a=PORTB.INTCTRL&0x03;
  if (a==0x03) return true;
  else return false;
  }
 //29.11.2017
void altera_on_PB3_int_3(void){
  PORTD.OUTSET=0x20;
  PORTD.DIRSET= 0x20;
}
void altera_off_PB3_int_3(void){
  PORTD.OUTCLR=0x20;
}
//-----------------------------

/*
инициализация пина для рефметки канала 0
in bool level - срабатывание по уровню или срабатывание по фронту
*/
void init_PB5_int_ref_0(bool level){ //PB5->PC2 сделать

  pRef[0]=false;

   #if (configUSE_Conf01 == 1)
   //REN0
   PORTA.OUTCLR = 0x08;
   PORTA.DIRSET = 0x08;
   //
   //REF0
   if(level)
	   PORTB.PIN1CTRL = 0x53;
   else
	   PORTB.PIN1CTRL = 0x11;

   PORTB.INT1MASK = 0x02;
   //
   return;
   #endif
   #if (configUSE_PLC == 1)
   return;
   #endif

  //REN0
  PORTD.OUTCLR = 0x01;
  PORTD.DIRSET = 0x01;
  //
  //REF0
  if(level)
     PORTC.PIN2CTRL = 0x53;//PORTB.PIN4CTRL=0x53;  
  else  
	 PORTC.PIN2CTRL = 0x11;//PORTB.PIN4CTRL=0x11; 

  PORTC.INT1MASK = 0x04;//PORTB.INT1MASK=0x10;
  //
}

/*
включение рефметки канала 0
in bool additive_switch_is - вкл/выкл готовности принимать реф метку для альтеры 
*/
void enable_PB5_int_0(bool additive_switch_is){
unsigned char a;
#if (configUSE_Conf01 == 1)
	//FLAGS
	a=PORTB.INTFLAGS&0x01;
	PORTB.INTFLAGS=0x02|a;            
	//
	if(additive_switch_is == 0)
		altera_on_PB5_int_0();

	a=PORTB.INTCTRL&0x03;
	PORTB.INTCTRL=0x0c|a;
	pRef[0]=false;
	return;
#endif
#if (configUSE_PLC == 1)
	return;
#endif

  a=PORTC.INTFLAGS&0x01;//unsigned char a=PORTB.INTFLAGS&0x01;
  PORTC.INTFLAGS=0x02|a;              //PORTB.INTFLAGS=0x02|a;

 if(!additive_switch_is){
  altera_on_PB5_int_0();
  }

  a=PORTC.INTCTRL&0x03;//a=PORTB.INTCTRL&0x03;
  PORTC.INTCTRL=0x0c|a;//PORTB.INTCTRL=0x0c|a;
  pRef[0]=false;

  }

 void disable_PB5_int_0(void){
 #if (configUSE_mcx514 == 1)
 return;
 #endif
 #if (configUSE_Conf01 == 1)
 to_stop_PB5_int_0();
 altera_off_PB5_int_0();
 return;
 #endif
// interrupt must be disabled
  unsigned char a=PORTC.INTCTRL&0x03;//unsigned char a=PORTB.INTCTRL&0x03;
  PORTC.INTCTRL=0x00|a;              //PORTB.INTCTRL=0x00|a;
// PD0 must be ZERO
  PORTD.OUTCLR = 0x01;
  }
 void to_stop_PB5_int_0(void){
	#if (configUSE_Conf01 == 1)
	PORTB.INTCTRL &= ~PORT_INT1LVL_gm;
	return;
	#endif
// interrupt must be disabled
  unsigned char a=PORTC.INTCTRL&0x03;//unsigned char a=PORTB.INTCTRL&0x03;
  PORTC.INTCTRL=0x00|a;//PORTB.INTCTRL=0x00|a;
  }
  void to_continue_PB5_int_0(void){
  #if (configUSE_Conf01 == 1)
  PORTB.INTCTRL |= PORT_INT1LVL_gm;
  return;
  #endif
// interrupt must be enabled
  unsigned char a=PORTC.INTCTRL&0x03;//unsigned char a=PORTB.INTCTRL&0x03;
  PORTC.INTCTRL=0x0c|a;//PORTB.INTCTRL=0x0c|a;
  }
 bool is_enabled_PB5_int_0(void){

	#if (configUSE_Conf01 == 1)
	if (PORTB.INTCTRL & PORT_INT1LVL_gm)
		return true;
	else
		return false;
	#endif

  unsigned char a=PORTC.INTCTRL&0x0c;//unsigned char a=PORTB.INTCTRL&0x0c;
  if (a==0x0c) 
	 return true;
  else 
	 return false;
  }
//29.11.2017
void altera_on_PB5_int_0(void){
	#if (configUSE_Conf01 == 1)
	PORTA.OUTSET= 0x08;
	PORTA.DIRSET= 0x08;
	return;
	#endif
  PORTD.OUTSET= 0x01;
  PORTD.DIRSET= 0x01;
}
void altera_off_PB5_int_0(void){
	#if (configUSE_Conf01 == 1)
	PORTA.OUTCLR=0x08;
	return;
	#endif
   PORTD.OUTCLR=0x01;
}
/*
функция устанавливает уровень на пине для альтеры ref enable
in int ch - канал
*/
void altera_on_int(const int &ch){

	switch (ch)
	{
		case 0:
			altera_on_PB5_int_0();
			break;
		case 1:
			altera_on_PA0_int_1();
			break;
		case 2:
			altera_on_PA1_int_2();
			break;
		case 3:
			altera_on_PB3_int_3();
			break;
		default:
		break;
	}

}
/*
функция сбрасывает уровень на пине для альтеры ref enable
in int ch - канал
*/
void altera_off_int(const int & ch){

	switch (ch)
	{
		case 0:
			altera_off_PB5_int_0();
			break;
		case 1:
			altera_off_PA0_int_1();
			break;
		case 2:
			altera_off_PA1_int_2();
			break;
		case 3:
			altera_off_PB3_int_3();
			break;
		default:
		break;
	}
}
//29.11.2017

void is_it_switch_for_ref_if_correction(unsigned int *switch_number_for_ref,unsigned long *input){
     int m=1;
     unsigned int t;
     unsigned long i;	
  if(is_enabled_PA0_int_1()){
//30.11.2017 0x008f inv 15(P1.16),0x000f no inv (P1.16)
	  i=*input;
	  if (switch_number_for_ref[1]&0x0080)
		i=~i;
	  t=switch_number_for_ref[1]&0xff7f;
	  m=m<<t;
		  if(m&i)
			altera_on_PA0_int_1();
		  else 
			altera_off_PA0_int_1();	
	}
}
#pragma GCC pop_options
//------------------------------------
};
extern CPorta_ref ref_;
//08.10.2018
class CAnalog_to_digital_conversion{

public:
	unsigned char  result_l,result_h,start_in_main,runner,alarm_counter;
	bool conv_result,alarm;
	long  int_result_adc_ch2,int_result_adc_ch0;
	void init_atdc_in_main(void){
		ADCA.CTRLA=0x01;
		ADCA.CTRLB=0x70;
		ADCA.REFCTRL=0x33;				
		ADCA.PRESCALER=0x07;//f is 32 000 000 /512 = 62500 hz 1/62500=16 mks *7 112 mks +16=128 mks
	}
	void start_external_input_signal_measure(void){
		//*1111*** ADC15 (PB7,13 pin)
		ADCA.CH0.MUXCTRL=0x78;
		//1******1 start,single-ended positive input signal
		ADCA.CH0.CTRL=0x81;
	}
	void start_internal_1_10_Vcc_measure(void){
		//1/10 Vcc
		//*0010***
		ADCA.CH0.MUXCTRL=0x10; //CH2
		//1******0 start,internal positive input signal
		ADCA.CH0.CTRL=0x80;   //CH2
	}
	void start_measure_corra1(){
	//*1100*** ADC12 (PB4,10 pin)
		ADCA.CH0.MUXCTRL = 0x60;
		//1******1 start,single-ended positive input signal
		ADCA.CH0.CTRL = 0x81;
	}
	void start_measure_corra2(){
	//*1101*** ADC13 (PB5,11 pin)
		ADCA.CH0.MUXCTRL = 0x68;
		//1******1 start,single-ended positive input signal
		ADCA.CH0.CTRL = 0x81;
	}
	bool take_result_ch2(void)
	{
		//f is 32 000 000 /512 = 62500 hz 1/62500=16 mks *7 112 mks +16=128 mks
		// ADCA.PRESCALER=0x07;
		int n;
		unsigned int a,b,c,d;
		a=0;b=0;c=0;d=0;
		//timeout 5015 cycles  5015*31.25=156718.25 mks
		for(n=0;;n++) {
			if(ADCA.CH0.INTFLAGS&0x01) { //CH2
			   ADCA.CH0.INTFLAGS=0x01;  //CH2
			   result_l=ADCA.CH0.RESL;//CH2
			   result_h=ADCA.CH0.RESH&0x0f;//CH2
				if(result_h&0x01)  a=256;
				if(result_h&0x02)  b=512;
				if(result_h&0x04)  c=1024;
				if(result_h&0x08)  d=2048;
				int_result_adc_ch2=a+b+c+d+result_l;
				break;
			}
		if(n>330) break;
		}
		if (n>330){
			result_l=0;
			result_h=0;
			int_result_adc_ch2=0;
			return false;
        }
	return true;
	}
	bool take_result_ch0(void)
	{
		//08.10.2018 ch0 is voltage from CR2032	
		//f is 32 000 000 /512 = 62500 hz 1/62500=16 mks *7 112 mks +16=128 mks
		// ADCA.PRESCALER=0x07;
		int n;
		unsigned int a,b,c,d;
		a=0;b=0;c=0;d=0;
		//timeout 5015 cycles  5015*31.25=156718.25 mks
		for(n=0;;n++) {
			cli();
			bool tmp=ADCA.CH0.INTFLAGS&0x01;
			sei();
			if(tmp==0x01) {
				cli();
				ADCA.CH0.INTFLAGS=0x01;
				result_l=ADCA.CH0.RESL;
				result_h=ADCA.CH0.RESH&0x0f;
				sei();
				if(result_h&0x01)  a=256;
				if(result_h&0x02)  b=512;
				if(result_h&0x04)  c=1024;
				//      if(result_h&0x08)  d=2048;
				//13.11.2017 mines must be excluded
				if(result_h&0x08) 
					int_result_adc_ch0=0;
				else 
					int_result_adc_ch0=a+b+c+d+result_l;
				break;
			}
			if(n>330) 
				break;
		}
		if (n>330){
			return false;
		}
	return true;
	}
};
extern CAnalog_to_digital_conversion atdc;

#endif
