#ifndef DISPLAY_H_
#define DISPLAY_H_


#include "main.h"
#include "led_key_sound.h"
#include "Cuart_PC2_PC3.h"

#include <avr/pgmspace.h>
#include <stdlib.h>
//#include "MenuLir.h"
//#include "type.h"
//#include "New.h"
extern "C"{
 void write_c_asm(unsigned char);
 void write_d_asm(unsigned char);
 void a0_to_one_cs_to_zero_asm(void);
 void cs_to_one_asm(void);
 void write_data_asm(unsigned char);
 void only_write_pulse_asm(void);
 char to_check_data_80000000(long data);
 }
extern volatile unsigned char invert_char;
#define INVERT_CHAR() invert_char = true;
#define NORMAL_CHAR() invert_char = false;

extern void delay_ms(long d);


class CDisp{

	public:
	char VERSION_ram[13];
	void init_VERSION();

	void only_test_to_see_big_digit(long counter);
	void put_char(char symbol, char page);
	void put_char_local(char symbol, char page,unsigned int *x,unsigned char *y);
	void put_string_local(unsigned char *str,char page,unsigned int *x_local,unsigned char *y_local);
	void put_char_local_independent(char symbol, char page,unsigned int x_local,unsigned char y_local);
	void put_growing_line(char page,char symbol,unsigned int x_local,unsigned char y_local);						   
	void put_button_(unsigned int x,unsigned char y, unsigned char npic);
	unsigned char rotate_button_pic_(BUTTON_PIC npic,int t);
	void clear_button_(unsigned int x,unsigned char y);
	void put_button_(unsigned int x,unsigned char y);
	void put_button_with_pic(unsigned int x,unsigned char y,BUTTON_PIC npic);
	void put_button_pic_(unsigned int x,unsigned char y,BUTTON_PIC npic);
	void clear_buttons(void);

	void put_axis_shot(unsigned int x,unsigned char y,unsigned char name_axis);

	void print_flash_local( const char * str , unsigned char page,unsigned int x, unsigned char y,char align = 0);
	void print_symbols_local(char symbol,int number,unsigned char page,unsigned int x_, unsigned char y_);
	void print_symbols_local_RA(char symbol,int number,unsigned char page,unsigned int x_, unsigned char y_);
	void print_ram_local( char * str, unsigned char page,unsigned int x, unsigned char y);
	void print_ram_local_RA( char * str, unsigned char page,unsigned int x, unsigned char y);

	void print_flash_( const char * str , unsigned char page);
	void print_flash_local_( const char * str , unsigned char page,unsigned int x, unsigned char y);
	void printf_flash2( const char * str , unsigned char page,unsigned int x_, unsigned char y_);

	void system_heart(void);
	void start_system_heart(void);
					 
void add_point(unsigned char * str, const short sz_str, const short place);
//------------MEnu
void show_about_programm();
void show_menu_hand();
void show_time(unsigned int x_,unsigned char y_);
void clear_ram_1();
void clear_ram_null(unsigned char page = 0);
int putdotinstring2(char * _Source,int n_after_dot,int numberofsymbol);
int strncmp_lir(const char *str1, const char *str2, int maxcount);
int strncpy_lir(char *Dest, const char *Source, int maxcount);
int strnlen_lir(const char *str, int maxcount = 50);
int strncat_lir(char * str1,const int &sz_str1, char * str2);
int strncatsym_lir(char * str1, const int & sz_str1, char sym);
void insert_sym(unsigned char* str, const short sz_str, const short place, const char& sym_);
bool delete_sym(unsigned char* str, const short sz_str, const char& sym_);
void formatting2(char *instr, int CurFormat);
void itoa_quick_lir(long *counter,unsigned char *txt_out);
int strncat_lir_offset(char * str1, const int &sz_str1, char * str2, unsigned short offset);
void print_hex(unsigned char* buff,int sz,int x , int y);

//----------------------------------------------------------------
#define SIZE_STRING_READOUT 240
unsigned char string_0[SIZE_STRING_READOUT];
unsigned char tmp_0[96],tmp_1[96];
char display_update;
//char runner_main_cycle,limit_runner_main_cycle;
unsigned int x,x_old;
unsigned char y,y_old;
unsigned char invert_display;
int runner,runner1,clock;
char dechi,second,minute,our;
unsigned char runner_led_or_keyboard,runner_led,runner_for_counters,runner_hardware;
bool run_0,run_1,run_2,run_3,run_4,run_5,run_6,run_7;
long only_to_see;

enum _ref_change_mode:uint8_t{
	NOTHING = 0,
	REF_NOT_FOUND =        250, // режим становиться активным когда выбирают абсолютную координату ,а она без реф метки
	SEARCH_REF_begin   =   251, // поиск реф метки в меню begin
	SEARCH_REF_findref =   252, // поиск реф метки в меню findref
	SEARCH_REF_hand    =   253, // поиск реф метки в меню hand
}ref_change_mode[N_AXIS];

bool ifSearchRef(int ch);
bool ifSearchRefAUTO();
bool ifSearchRefFINDREF();
bool ifSearchRefHAND();

//unsigned char ref_change_mode[N_AXIS];
int addr_heap_start;
int addr_heap_end;
int current_stack_position;
//-------------------------
unsigned char bufftime[15];
bool clearram;
unsigned char page_for_buttons;
//----------------------------------------------------------------
void put_large_symbol(const unsigned char digit, char page,const unsigned char &width,unsigned int x_local,unsigned char y_local);
void put_big_symbol(const unsigned char digit, char page,const unsigned char &width,unsigned int x_local,unsigned char y_local);
//------------------
bool print_error_big_overflow_type_long(long data,unsigned char n_axis,unsigned char place);
bool print_error_big_no_empty_space(long data,unsigned char n_axis,unsigned char place);
void print_encoder_fault(long data,unsigned char clear_rec,unsigned char place,unsigned char channel);
bool print_is_not_reference_executed(unsigned char place,const unsigned char& ch,unsigned char key_press);

char big_digit_display_update,big_digit_display_update_;
char cursor_param_display_update;
unsigned char page;
long speed;
int interval_limit;
long speed_100[N_AXIS],speed_sum[4];
long speed_sliping_[N_AXIS][60];
unsigned char axis_is_chosen[N_AXIS];
bool encoder_fault[N_AXIS];
bool encoder_fault_[N_AXIS];
unsigned char string_to_see[N_AXIS],blinker;
long offset_g53[N_AXIS];//0-G53,
bool go_to_blink_string[N_STR]; // номер строки с миганием 
long offset_g5x[5][N_AXIS];//0-G54,1-G55,2-G56,3-G57,4-G58

enum _STR_MODE:uint8_t{
	SPEED,     // скорость
	COORDINATE,// координаты (функций нет)
	PLUS,      // суммируемая ось
	RESULT,    // результат вычислений
	TIME,      // время
}position_to_see[N_STR];

char to_see_big_speed_[N_STR][15];
//----------------------------------------
void init_lcd_RGB();
void clear_ram_RGB(unsigned long data,unsigned int s_x,unsigned int e_x,unsigned int s_y,unsigned int e_y);
void send_pixel_color(char R,char G,char B);
void init_lcd(void){

// Software reset
  write_c(0xae);
  write_d(0xa5);

// Power Control
  write_c(0x61);
  write_d(0x0f);
  write_d(0x04);
  write_d(0xa5);
  write_d(0xa5);
//  Electronic Volume Set 1
  write_c(0x62);
  write_d(0x36);
  write_d(0x0b);
  write_d(0x0b);
  write_d(0xa5);
 //  Electronic Volume Set 2
  write_c(0x63);
  write_d(0x09);
  write_d(0x17);
  write_d(0xa5);
  write_d(0xa5);

 /*
 //   Set source and gate ON/OFF timing
  write_c(0x33);
  write_d(0x07);
  write_d(0x2c);
  write_d(0x09);
  write_d(0x2a);

 */

 //   Set source and gate ON/OFF timing
  write_c(0x33);
  write_d(0x00);
  write_d(0x00);
  write_d(0x00);
  write_d(0x00);


    write_c(0x32);
    write_d(0x50);
    write_d(0x00);
    write_d(0x00); // 1000xxxx normal black
    write_d(0x00);


// Sleep out
  write_c(0x12);
  write_d(0xa5);

// Memory Address Control
   write_c(0x24);
   write_d(0x01);
   write_d(0xa5);
   write_d(0xa5);
   write_d(0xa5);

// BPP select
   write_c(0x22);
   write_d(0x00);   // 1bpp (2gray scale)
   write_d(0xa5);
   write_d(0xa5);
   write_d(0xa5);


// Block Fill
  write_c(0x29);
  write_d(0x00);
  write_d(0xa5);
  write_d(0xa5);
  write_d(0xa5);

 delay_ms(16);//__delay_cycles(500000);


// Display on
   write_c(0x15);
   write_d(0xa5);
}


//17.05.2016 INVERSE-----------
void invert_in(void){
 write_c(0x1b);
 write_d(0xa5);
}
void invert_out(void){
 write_c(0x1a);
 write_d(0xa5);
}
//-----------------------------
void write_c(unsigned char cmd){
//    char timeout;
//    register char d;
//    a0 = 0, nrd = 1, nwr = 0
// no inline 120 cycles *62.5 ns = 7.5 mks

         //cli();//__disable_interrupt();
  //clr_A0();
#if (configUSE_RGB_disp == 0)
  PORTA.OUTCLR =0x40;
  //clr_nCS();
  PORTA.OUTCLR =0x20;
  //clr_nWR();
  PORTA.OUTCLR =0x80;

  PORTF.OUT=cmd;

  //set_nWR();
  PORTA.OUTSET =0x80;
  //set_nCS();
  PORTA.OUTSET =0x20;
#endif
#if (configUSE_RGB_disp == 1)
PORTA.OUTCLR =0x40;
//clr_nCS();
PORTA.OUTCLR =0x80;
//clr_nWR();
PORTA.OUTCLR =0x20;

PORTF.OUT=cmd;

//set_nWR();
PORTA.OUTSET =0x20;
//set_nCS();
PORTA.OUTSET =0x80;
#endif
//sei();//__enable_interrupt();

};

void write_d(unsigned char data){
//    char timeout;
//   register char d;
//    a0 = 1, nrd = 1, nwr = 0
// no inline 120 cycles *62.5 ns = 7.5 mks
//  inline 33*62.5 = 2.0625 mks

//cli();//__disable_interrupt();
 //set_A0();
#if (configUSE_RGB_disp == 0)
 PORTA.OUTSET =0x40;
 //clr_nCS();
 PORTA.OUTCLR =0x20;
 //clr_nWR();
 PORTA.OUTCLR =0x80;

 PORTF.OUT=data;

 //set_nWR();
 PORTA.OUTSET =0x80;
 //set_nCS();
 PORTA.OUTSET =0x20;
#endif
#if (configUSE_RGB_disp == 1)
PORTA.OUTSET =0x40;//set_A0();
PORTA.OUTCLR =0x80;//clr_nWR();
PORTA.OUTCLR =0x20;//clr_nCS();
PORTF.OUT=data;
PORTA.OUTSET =0x20;//set_nCS();
PORTA.OUTSET =0x80;//set_nWR();
#endif
//sei();//__enable_interrupt();
};

void display_port_initializer(void){
//  nRST  LCD_RES  (4) //PORTA output
//  nCS   CS_LCD   (5) //PORTA output
//  A0    LCD_A0   (6) //PORTA output
//  nWR   LCD_WR   (7) //PORTA output
//  nRD   LCD_RD   (0) //PORTB output
// PORTF data

// OUTPUTS for control's lines must be ones
   PORTA.OUTSET=0xf0;
//14.11.2017 analog ref   
//   PORTB.OUTSET=0x01;
//
   PORTF.OUTCLR=0xff;
// PORTF 11111111
   PORTF.DIR=0xff;
// PORTA 1111XXXX
   PORTA.DIRSET=0xf0;
// PORTB XXXXXXX1
//14.11.2017 analog ref   
//   PORTB.DIRSET=0x01;
   PORTB.DIRCLR=0x01;
//03.10.2017 HARDWARE RESET   
//   PORTA.OUTCLR=0x10;
//   delay_ms(1);
//   PORTA.OUTSET=0x10;
//   delay_ms(1);
 }

 void init_lcd_(){

//SOFTWARE RESET
  write_c(0xae);
  write_d(0xa5);
//---------------  


  write_c(0x61);
  write_d(0x0f);
  write_d(0x04);
  write_d(0xa5);
  write_d(0xa5);

  write_c(0x62);
  write_d(0x36);
  write_d(0x0b);
  write_d(0x0b);
  write_d(0xa5);
 //  Electronic Volume Set 2
  write_c(0x63);
  write_d(0x09);
  write_d(0x17);
  write_d(0xa5);
  write_d(0xa5);

 
 //   Set source and gate ON/OFF timing
  write_c(0x33);
  write_d(0x07);
  write_d(0x2c);
  write_d(0x09);
  write_d(0x2a);

 
 // Memory Address Control
   write_c(0x24);
   write_d(0x01);
   write_d(0xa5);
   write_d(0xa5);
   write_d(0xa5);

// BPP select
   write_c(0x22);
   write_d(0x00);   // 1bpp (2gray scale)
   write_d(0xa5);
   write_d(0xa5);
   write_d(0xa5);


// Block Fill
  write_c(0x29);
  write_d(0x00);
  write_d(0xa5);
  write_d(0xa5);
  write_d(0xa5);

 //   Set source and gate ON/OFF timing
  write_c(0x33);
  write_d(0x00);
  write_d(0x00);
  write_d(0x00);
  write_d(0x00);
//17.03.2016
//   write_c(0x31);
//    write_d(0x01);
//    write_d(0x3f);
//    write_d(0x00);
//    write_d(0x50);
//    write_d(0x02);
//    write_d(0x7f);
//    write_d(0x01);
//    write_d(0x3f);
//-----------------


    write_c(0x32);
// 18.03.2016 may be it is frame range adjust...
// 0x0f 194 Hz 0x50 38 Hz and 200 Hz is limit (0x0d)
// 200 0xc8 15 Hz 250 0xfa 12 Hz
    write_d(0x0f);
    write_d(0x00);
    write_d(0x00); // 1000xxxx normal black
    write_d(0x00);


// Sleep out
  write_c(0x12);
  write_d(0xa5);



 delay_ms(16);//__delay_cycles(500000);


// Display on
   write_c(0x15);
   write_d(0xa5);
} 

void clear_ram(unsigned char data,unsigned char page){
#if (configUSE_RGB_disp == 0)
//27.12.2017 it takes 6.3 ms
//21.03.2016 page 0 0x00, 1 0x01, 2 0x02, 3 0x03
// page's size is 320 (column from left to right)* 240 (line from up to down)
// column's step is 1 pixel, line's step is 8 pixel
//240 0xf0-1 = 0xef
 write_c(0x25);
 write_d(0x00);
 write_d(0xef);
 write_d(page);
 write_d(0xa5);
//---------------------
 write_c(0x26);
 write_d(0x00);
 write_d(160);
// 160+320=480 0x01e0-1 = 0x01df
 write_d(0x01);
 write_d(0xdf);
//---------------------
 write_c(0x2c);
 write_d(0xa5);
//320*(240/8)=9600

 //set_A0();
 PORTA.OUTSET =0x40;
 PORTF.OUT=data;
 //clr_nCS();
 PORTA.OUTCLR =0x20;
// a0_to_one_cs_to_zero_asm();
for(int n=0;n<9600;n++) {
	//clr_nWR();
	PORTA.OUTCLR =0x80;
	//set_nWR();
	PORTA.OUTSET =0x80;
}
  //set_nCS();
 PORTA.OUTSET =0x20;
#endif
#if (configUSE_RGB_disp == 1)
	clear_ram_RGB(0xffffff,0x0000,0x013f,0x0000,0x00ef);
#endif
};

void turn_on_page(unsigned char page){
//21.03.2016 page 0 0x00, 1 0x01, 2 0x02, 3 0x03
// page's size is 320 (column from left to right)* 240 (line from up to down)
// column's step is 1 pixel, line's step is 8 pixel
 write_c(0x21);
 write_d(page);
 write_d(0xa5);
 write_d(0xa5);
 write_d(0xa5);

};
//---------------------
void clear_rectangle(unsigned int x,unsigned char y,unsigned int l,unsigned char h,unsigned char data,unsigned char page ){
unsigned char x_l,x_h,l_l,l_h;
int m;
unsigned int x_;
if (x>319) x=0;
if (y>29)  y=0;
if (l>320) l=320;
if (h>30)  h=30;
//--number
   if (l==0 && h==0) {
    m=0;
   }
   else if (l!=0 && h!=0) {
   m=(l*h)-1;
   }
   else if (l!=0 && h==0) {
   m=l-1;
   }
   else if (l==0 && h!=0) {
   m=h-1;
   }
//------------
//y=y*8;
h=(h+y);
if(h!=0)h=h-1;
//------------
x=x+160;
x_=x;
 x_l=(unsigned char)x;
 x=x>>8;
 x_h=(unsigned char)x;
x=x_+l;
 if (l!=0)x=x-1;
 l_l=(unsigned char)x;
 x=x>>8;
 l_h=(unsigned char)x;
//240 0xf0-1 = 0xef
 write_c(0x25);
 //start y
 write_d(y);
 //end y ;
 write_d(h);
 write_d(page);
 write_d(0xa5);
//---------------------
 write_c(0x26);
 write_d(x_h);
 write_d(x_l);
// 160+320=480 0x01e0-1 = 0x01df
 write_d(l_h);
 write_d(l_l);
//---------------------
 write_c(0x2c);
 write_d(0xa5);
//320*(240/8)=9600

 //set_A0();
 PORTA.OUTSET =0x40;
 PORTF.OUT=data;
 //clr_nCS();
 PORTA.OUTCLR =0x20;
//a0_to_one_cs_to_zero_asm();
for(int n=-1;n<m;n++) {
//  cli();//__disable_interrupt();
 //clr_nWR();
 PORTA.OUTCLR =0x80;
 //set_nWR();
 PORTA.OUTSET =0x80;
//sei();//__enable_interrupt();
// write_d(data);
//only_write_pulse_asm();
}
  //set_nCS();
 PORTA.OUTSET =0x20;
 //cs_to_one_asm();
};
void gotoxy(unsigned int x_, unsigned char y_){
//it takes 56 * 31.25=1.750 mks
 x=x_*8+160;
 y=y_>>3;
 x_old=x_;
 y_old=y_;
}

//-----------------------------------------------------
//22.12.2017 only local variables inside function must be used...
void print_debug_one_local(long debug_a, unsigned char page,unsigned int x_,unsigned char y_);
//01.06.2018
void print_debug_one_local_shot(long debug_a, unsigned char page,unsigned int x_,unsigned char y_,char amount);
//------------------------------------
void print_coeff(long debug_a,char n_after_dot, unsigned char page,unsigned int x_,unsigned char y_,char align = 0,char clear = 0);
void print_debug_one_local_unsigned(unsigned long debug_a, unsigned char page,unsigned int x_,unsigned char y_);


  char itoa(unsigned char *str, long data){
	 int k = 0;
        unsigned char j, tmp = 0;
        if(data < 0){
            data = -data;
            tmp = 0xff;
        }

        do{
	        str[k++] = data%10 + 0x30;
            data /= 10;
        }while(data);

        //str[k] = data + 0x30;
        if(tmp) {
            str[k++] = '-';
        }

        for(j = 8; j < k>>1; j--){
            tmp = str[k + 1 + j];
            str[k + 1 + j] = str[j];
            str[j] = tmp;
        }

        return k;
    }
 char itoa_quick(unsigned char *str,long *data){

	 volatile int k = 0;
     long quot,qq,value;
     int rem;
     volatile unsigned char j, tmp = 0;
//26.04.2017 0x80000000 can't be converted to +
//range is  from -2147483648 to 2147483647
//it is special case
//      if (*data==0x80000000) {	
//05.03.2018	it takes in worst case 21*31.25=656.25 ns
// if input is 0x80000000 it can be removed from system 
// if that is used (assembler routine)
	char answer=to_check_data_80000000(*data);
	if(answer==0xff)*data=0;
//----------------------------------	  
        if(*data < 0){
            *data = -*data;
            tmp = 0xff;
        }
        value=*data;
//--------------------------
       do{
//mul 0.8
       quot=value>>1;
       quot=quot+(quot>>1);
       quot=quot+(quot>>4);
       quot=quot+(quot>>8);
       quot=quot+(quot>>16);
       qq=quot;
//div 8
       quot=quot>>3;
       rem = (value- ((quot<<1)+(qq&~0x7)));
      if (rem>9) {

        rem=rem-10;
        quot++;
       }
       str[k++]=rem+0x30;
       value=quot;
        }while(quot);
//---------------------------
        if(tmp)str[k++] = '-';
//---------------------------
        for(j = 8; j < k>>1; j--){
            tmp = str[k + 1 + j];
            str[k + 1 + j] = str[j];
            str[j] = tmp;
        }

        return k;
    }

char itoa_quick_unsigned(unsigned char *str, unsigned long *data){
	//27.04.2017 -2147483647  2106*31.25=65812.5 mks
	int k = 0;
	long quot,qq,value;
	int rem;
	unsigned char j, tmp = 0;
	//26.04.2017 0x80000000 can't be converted to +
	//range is  from -2147483648 to 2147483647
	//it is special case
	//      if (*data==0x80000000) {
	if (*data==2147483647) {
		str[0]=0x38;
		str[1]=0x34;
		str[2]=0x36;
		str[3]=0x33;
		str[4]=0x38;
		str[5]=0x34;
		str[6]=0x37;
		str[7]=0x34;
		str[8]=0x31;
		str[9]=0x32;
		str[10]='-';
		return 11;
	}

	if(*data < 0){
		*data = -*data;
		tmp = 0xff;
	}
	value=*data;
	//--------------------------
	do{
		//mul 0.8
		quot=value>>1;
		quot=quot+(quot>>1);
		quot=quot+(quot>>4);
		quot=quot+(quot>>8);
		quot=quot+(quot>>16);
		qq=quot;
		//div 8
		quot=quot>>3;
		rem = (value- ((quot<<1)+(qq&~0x7)));
		if (rem>9) {
			rem=rem-10;
			quot++;
		}
		str[k++]=rem+0x30;
		value=quot;
	}while(quot);
	//---------------------------
	if(tmp)str[k++] = '-';
	//---------------------------
	for(j = 8; j < k>>1; j--){
		tmp = str[k + 1 + j];
		str[k + 1 + j] = str[j];
		str[j] = tmp;
	}

	return k;
}


void strcpylir(char * _Dest, char const * _Source)
{
	int index;
	index = 0;
	while (_Source[index] != '\0')
	{
		_Dest[index] = _Source[index];
		index++;
	}
	_Dest[index] = 0x00;
}
};


extern CDisp display;



#endif /* DISPLAY_H_ */