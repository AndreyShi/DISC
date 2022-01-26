/*
 * display.cpp
 *
 * Created: 15.06.2017 8:52:51
 *  Author: Андрей
 */ 



#include "symbols.h"
#include "DigitBig.h"
#include "DigitSmall.h"
#include "DigitLarge.h"
#include "separator.h"
#include "button.h"
#include "button_pic.h"
#include "AxisSmall.h"
#include "AxisBig.h"
#include "symbolsnew.h"
#include "Strings.h"
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "programm.h"
#include "display.h"
#include <math.h>
#include "config.h"
#include "MenuLir.h"
#include "screen_service.h"

//#include "error_log.h"

volatile unsigned char invert_char;

//03.02.2017-------------
void CDisp::system_heart(void){
	if (display.big_digit_display_update>=display.display_update){
		bool run=run_0|run_1|run_2|run_3|run_4|run_5|run_6|run_7;
		if(run==false){
			display.big_digit_display_update=0;
		}
	} else  {
		bool run=run_0|run_1|run_2|run_3|run_4|run_5|run_6|run_7;
		if(run==false){
			display.big_digit_display_update++;
		}
	}
}
void CDisp::start_system_heart(void){
	display.display_update=6;
	display.big_digit_display_update=0;
	display.big_digit_display_update_=0;
}



void CDisp:: put_button_(unsigned int x,unsigned char y,unsigned char npic){
	if((BUTTON_PIC)npic == _pic_EMPTY) {
		clear_button_(x,y);
	}
	else{
		//put_button_(x,y); // кладется рамка стирая все внутри рамки
		//put_button_pic_(x,y,(BUTTON_PIC)npic); // поверх кладется иконка, (получается мерцание если закцилено)
		put_button_with_pic(x,y,(BUTTON_PIC)npic);		
	}
}
void CDisp::clear_button_(unsigned int x,unsigned char y){
	//05.04.2016 240 bytes 48 from left to right *5 from top to bottom
	// it takes 140 mks
	unsigned char x_l,x_h,l_l,l_h;
	//int m;
	unsigned int x_;
	//unsigned int x;
	//unsigned char y;
	//unsigned int l=48;
	//y=0;
	//x=75;
	//m=240;
	//240 0xf0-1 = 0xef
	write_c(0x25);
	//start y
	write_d(y);
	//end y ;
	write_d(y+4);
	//memory page inside display
	write_d(0);
	write_d(0xa5);

	x=x+160;
	x_=x;
	x_l=(unsigned char)x;
	x=x>>8;
	x_h=(unsigned char)x;
	x=x_+47;
	// if (l!=0)x=x-1;
	l_l=(unsigned char)x;
	x=x>>8;
	l_h=(unsigned char)x;
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

	// PORTF.OUT=0xff;
	PORTF.OUT=0x00;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=0;n<240;n++) {

		//clr_nWR();
		// PORTF.OUT=string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
}
void CDisp::put_button_(unsigned int x,unsigned char y){
	//05.04.2016 240 bytes 48 from left to right *5 from top to bottom
	//    for (unsigned char i=0;i<240;i++) {
	//    button_border[i]=button[i];
	//    }
	//it takes 200 mks
	#if (configUSE_RGB_disp == 1)
	//x += 10;
	write_c(0x2a);
	write_d(x >> 8);
	write_d(x);
	write_d((x + (40 - 1)) >> 8);
	write_d(x + (40 - 1));
	y = (y/7 * (8 * 6 + 8));
	write_c(0x2b);
	write_d(y >> 8); //start hi
	write_d(y); //lo
	write_d((y + (40 * 6) - 1) >> 8); //end hi
	write_d(y + (40 * 6) - 1);  //lo
	
	write_c(0x2c);//GO
	for (int half = 0; half < 40 * 6; half += 40) {
		for (unsigned char bit = 1; bit != 0; bit *= 2) {
			for (int byte = half; byte < (40 + half); byte++) {
				if (pgm_read_byte_near(&button[byte]) & bit)
					send_pixel_color(0x00,0x00,0x00); //black
				else
					send_pixel_color(0xfc,0xfc,0xfc); //white
			}
		}
	}
	#endif
	
	#if (configUSE_RGB_disp == 0)
	unsigned char x_l,x_h,l_l,l_h;
	unsigned int x_;
	write_c(0x25);
	//start y
	write_d(y);
	//end y ;
	write_d(y);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	x=x+160;
	x_=x;
	x_l=(unsigned char)x;
	x=x>>8;
	x_h=(unsigned char)x;
	x=x_+39;
	// if (l!=0)x=x-1;
	l_l=(unsigned char)x;
	x=x>>8;
	l_h=(unsigned char)x;
	//---------------------
	write_c(0x26);
	write_d(x_h);
	write_d(x_l);
	// 160+320=480 0x01e0-1 = 0x01df
	write_d(l_h);
	write_d(l_l);
	//---------------------

	//----------------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=0;n<40;n++) {
		//clr_nWR();
		PORTF.OUT= pgm_read_byte_near(&button[n]);
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//return;

	//----------------
	write_c(0x25);
	//start y
	write_d(y+1);
	//end y ;
	write_d(y+1);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=40;n<80;n++) {
		//clr_nWR();
		PORTF.OUT= pgm_read_byte_near(&button[n]);
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//----------------
	write_c(0x25);
	//start y
	write_d(y+2);
	//end y ;
	write_d(y+2);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=80;n<120;n++) {
		//clr_nWR();
		PORTF.OUT= pgm_read_byte_near(&button[n]);
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//----------------
	write_c(0x25);
	//start y
	write_d(y+3);
	//end y ;
	write_d(y+3);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=120;n<160;n++) {
		//clr_nWR();
		PORTF.OUT= pgm_read_byte_near(&button[n]);
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;

	//----------------
	write_c(0x25);
	//start y
	write_d(y+4);
	//end y ;
	write_d(y+4);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=160;n<200;n++) {
		//clr_nWR();
		PORTF.OUT= pgm_read_byte_near(&button[n]);
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	#endif
}
//функцию которая накладывает иконку на рамку  в RAM(string_0) и выводит на дисплей
void CDisp::put_button_with_pic(unsigned int x,unsigned char y,BUTTON_PIC npic)
{	
	memcpy_P(string_0, button, 200); // копирование из  flash в RAM рамки

	for(int byte = 0,offset = 0 ; byte < 96; byte += 32,offset += 40)// копирование из  flash в RAM значка
		memcpy_P(string_0 + 44 + offset, button_pic[npic] + byte, 32);

	#if (configUSE_RGB_disp == 1)
	//x += 10;
	write_c(0x2a);
	write_d(x >> 8);
	write_d(x);
	write_d((x + (40 - 1)) >> 8);
	write_d(x + (40 - 1));
	y = (y/7 * (8 * 5 + 8));
	write_c(0x2b);
	write_d(y >> 8); //start hi
	write_d(y); //lo
	write_d((y + (40 * 5) - 1) >> 8); //end hi
	write_d(y + (40 * 5) - 1);  //lo
	
	write_c(0x2c);//GO
	for (int half = 0; half < 40 * 5; half += 40) {
		for (unsigned char bit = 1; bit != 0; bit *= 2) {
			for (int byte = half; byte < (40 + half); byte++) {
				if (string_0[byte] & bit)
					send_pixel_color(0x00,0x00,0x00); //black
				else
					send_pixel_color(0xfc,0xfc,0xfc); //white
			}
		}
	}
	#endif
	
	#if (configUSE_RGB_disp == 0)
	unsigned char x_l,x_h,l_l,l_h;
	unsigned int x_;
	write_c(0x25);
	//start y
	write_d(y);
	//end y ;
	write_d(y);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	x=x+160;
	x_=x;
	x_l=(unsigned char)x;
	x=x>>8;
	x_h=(unsigned char)x;
	x=x_+39;
	// if (l!=0)x=x-1;
	l_l=(unsigned char)x;
	x=x>>8;
	l_h=(unsigned char)x;
	//---------------------
	write_c(0x26);
	write_d(x_h);
	write_d(x_l);
	// 160+320=480 0x01e0-1 = 0x01df
	write_d(l_h);
	write_d(l_l);
	//---------------------

	//----------------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=0;n<40;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//return;

	//----------------
	write_c(0x25);
	//start y
	write_d(y+1);
	//end y ;
	write_d(y+1);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=40;n<80;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//----------------
	write_c(0x25);
	//start y
	write_d(y+2);
	//end y ;
	write_d(y+2);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=80;n<120;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//----------------
	write_c(0x25);
	//start y
	write_d(y+3);
	//end y ;
	write_d(y+3);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=120;n<160;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;

	//----------------
	write_c(0x25);
	//start y
	write_d(y+4);
	//end y ;
	write_d(y+4);
	//memory page inside display
	write_d(page_for_buttons);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=160;n<200;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	#endif
}

unsigned char CDisp::rotate_button_pic_(BUTTON_PIC npic,int t)
{
unsigned char byte[8];
unsigned char byte_to_display = 0;

static char counter;
static int counter2;
unsigned char  mask;

if (!(t % 8))
{
	mask = 0x80;
	counter = 0;
	switch (t)
	{
		case 0:
		counter2 = 0;
		break;
		case 32:
		counter2 = 32;
		break;
		case 64:
		counter2 = 64;
		break;
		case 96:
		counter2 = 96;
		break;
		case 128:
		counter2 = 128;
		break;
		default:
		counter2++;
		break;
	}
}
else
{
	counter++;
	if (counter == 1) mask = 0x40;
	if (counter == 2) mask = 0x20;
	if (counter == 3) mask = 0x10;
	if (counter == 4) mask = 0x08;
	if (counter == 5) mask = 0x04;
	if (counter == 6) mask = 0x02;
	if (counter == 7) mask = 0x01;
}

	for(int x = 0,y = counter2; x < 8 ;x++,y+=4)
	{
		byte[x]  = pgm_read_byte_near(&button_pic_new_2019[npic][y]);
		if(byte[x] & mask) 
			byte_to_display |= (char)pow(2,x);  

	}

	return byte_to_display;
}
void CDisp::put_button_pic_(unsigned int x,unsigned char y,BUTTON_PIC npic){
	//06.04.2016 it takes 224 mks
	x=x+4;
	y=y+1;
	//unsigned char tmp[96];
	unsigned char register i;

	if(invert_char) {
		for(i=0;i<96;i++)
		//tmp_0[i]=~button_pic[npic][i];
		tmp_0[i] = ~pgm_read_byte_near(&button_pic[npic][i]); //if use avr gcc
		//tmp_0[i] = rotate();
	}

	else {
		for(i=0;i<96;i++)
		//tmp_0[i]=button_pic[npic][i];
		tmp_0[i] = pgm_read_byte_near(&button_pic[npic][i]); //if use avr gcc
	}

	#if (configUSE_RGB_disp == 1)
	//x += 10;
	write_c(0x2a);
	write_d(x >> 8);
	write_d(x);
	write_d((x + (32 - 1)) >> 8);
	write_d(x + (32 - 1));
	y = (y/7 * (8 * 3 + 32)) + 8;
	write_c(0x2b);
	write_d(y >> 8); //start hi
	write_d(y); //lo
	write_d((y + (32 * 3) - 1) >> 8); //end hi
	write_d(y + (32 * 3) - 1);  //lo
	
	write_c(0x2c);//GO
	for (int half = 0; half < 32 * 3; half += 32) {
		for (unsigned char bit = 1; bit != 0; bit *= 2) {
			for (int byte = half; byte < (32 + half); byte++) {
				if (tmp_0[byte] & bit)
					send_pixel_color(0x00,0x00,0x00); //black
				else
					send_pixel_color(0xfc,0xfc,0xfc); //white
			}
		}
	}
	#endif

	#if (configUSE_RGB_disp == 0)
	//----------------------------------------------
	unsigned char x_l,x_h,l_l,l_h;
	unsigned int x_;
	write_c(0x25);
	//start y
	write_d(y);
	//end y ;
	write_d(y);
	//memory page inside display
	write_d(0);
	write_d(0xa5);
	//----------------
	x=x+160;
	x_=x;
	x_l=(unsigned char)x;
	x=x>>8;
	x_h=(unsigned char)x;
	x=x_+31;
	// if (l!=0)x=x-1;
	l_l=(unsigned char)x;
	x=x>>8;
	l_h=(unsigned char)x;
	//---------------------
	write_c(0x26);
	write_d(x_h);
	write_d(x_l);
	// 160+320=480 0x01e0-1 = 0x01df
	write_d(l_h);
	write_d(l_l);
	//---------------------

	//---------------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=0;n<32;n++) {
		//clr_nWR();
		PORTF.OUT= tmp_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//return;
	//----------------
	write_c(0x25);
	//start y
	write_d(y+1);
	//end y ;
	write_d(y+1);
	//memory page inside display
	write_d(0);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=32;n<64;n++) {
		//clr_nWR();
		PORTF.OUT= tmp_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//----------------
	write_c(0x25);
	//start y
	write_d(y+2);
	//end y ;
	write_d(y+2);
	//memory page inside display
	write_d(0);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=64;n<96;n++) {
		//clr_nWR();
		PORTF.OUT= tmp_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	#endif
}
/*
функция выводит на экран ошибку "переполнение,сброс <C>"  если счетчик>MAX_LONG_PLUS || счетчик<MAX_LONG_MINES 
in long data - последнее значение счетчика
in unsigned char clear_rec - очистка поля перед выводом ошибки (1 - да/ 0 - нет)
in unsigned char place - номер строки куда выводить (1 - 4)
*/
bool CDisp::print_error_big_overflow_type_long(long data,unsigned char clear_rec,unsigned char place){

	int n = place - 1;
	int mul = 0;

	if(n == 1)
		mul = 4;
	else if (n == 2)
		mul = 7;
	else if (n == 3)
		mul = 11;

	if (clear_rec==1)
		clear_rectangle(40,n * 7,232,5,0x00,0);
	display.print_flash_local(sPerepolnenie_sbros,0,6,16 * mul);
	display.print_flash_local(sSchetchik,0,6,16 + (16 * mul));
	display.print_debug_one_local(data,0,15,16 + (16 * mul));
	return true;
}
/*
функция выводит на экран ошибку "счетчик не в диапазоне"  если счетчик>99999999 || счетчик<-99999999 и др причинам (см функцию you_can_see_big_digit() )
in long data - последнее значение счетчика 
in unsigned char clear_rec - очистка поля перед выводом ошибки (1 - да/ 0 - нет)
in unsigned char place - номер строки куда выводить (1 - 4)
*/
bool CDisp::print_error_big_no_empty_space(long data,unsigned char clear_rec,unsigned char place){
	
	int n = place - 1;
	int mul = 0;

	if(n == 1)
	 mul = 4;
	else if (n == 2)
	 mul = 7;
	else if (n == 3)
	 mul = 11;

	if (clear_rec==1)
		clear_rectangle(40,n * 7,232,5,0x00,0);
	display.print_flash_local(sSchetchik_ne_v_diapozone,0,6,16 * mul);
	display.print_flash_local(sSchetchik,0,6,16 + (16 * mul));
	display.print_debug_one_local(data,0,15,16 + (16 * mul));
	return true;
}
/*
функция выводит на экран ошибку "ошибка датчика.разъём:"  нет датчика на разъеме либо потеря связи с датчиком
in long data - последнее значение счетчика перед потерей связи
in unsigned char clear_rec - очистка поля перед выводом ошибки (1 - да/ 0 - нет)
in unsigned char place - номер строки куда выводить (1 - 4)
in unsigned char channel - номер канала (0 - 3)
*/
void CDisp::print_encoder_fault(long data,unsigned char clear_rec,unsigned char place,unsigned char channel){
	static unsigned char tmp;
	char time_seconds[9];
	l_k_s.time_string_with_seconds(l_k_s.real_time_to_see,time_seconds);

	tmp = (place - 1) * 56;
	if (clear_rec == 1){
		clear_rectangle(40,(place - 1) * 7,232,5,0x00,0);
		Lir.print_long_format(data,Lir.get_format_from_ch(channel),0,14,tmp + 16,0,0,9);
		display.print_ram_local(time_seconds,0,25,tmp + 16);		
	}
	display.print_flash_local(sOshibka_datchika_portP2,0,6,tmp);
	display.print_flash_local(sSchetchik,0,6,tmp + 16);
	
}
/*
функция выводит на экран ошибку "ось не вышла в '0'"
in unsigned char place - номер строки для вывода 1 - 4
in const unsigned char& ch - канал 

out return value bool - есть ошибка/нет ошибки
*/
bool CDisp::print_is_not_reference_executed(unsigned char place,const unsigned char& ch,unsigned char key){

//#define REF_NOT_FOUND 250

  unsigned char y_clear_rect = 0;
  unsigned char y_txt = 0;
  static unsigned char trigger[N_AXIS];
  //_key_BLANK1               =        35  ,
  //_key_BLANK2               =        43  ,
  //_key_BLANK3               =        44  ,
  //_key_BLANK4               =        36  ,
 // MsgKey.key_press_for_A_O1		

		if(display.ref_change_mode[ch] == REF_NOT_FOUND ){
			if(!trigger[ch]){
				if(key == _key_BLANK1 ){
					y_clear_rect = 0;
					y_txt = 0;
				}
				if(key == _key_BLANK2 ){
					y_clear_rect = 7;
					y_txt = 56;
				}
				if(key == _key_BLANK3 ){
					y_clear_rect = 14;
					y_txt = 112;
				}
				if(key == _key_BLANK4 ){
					y_clear_rect = 21;
					y_txt = 172;
				}
				clear_rectangle(40,y_clear_rect,232,5,0x00,0);
				display.print_flash_local(sOsb_ne_vishla_v_0,0,6,y_txt);
				display.print_flash_local(sVivesti_v_0_PUSK,0,6,y_txt + 16);
				trigger[ch] = 1;
				screen.encoder_fault_[place] = 0;
			}
			return true;
		}
		else{
			trigger[ch] = 0;
			return false;
		}
}
/*
В режиме поиска реф для индикации
*/
bool CDisp::ifSearchRef(int c)
{
	if(c >= N_AXIS)
		return 0;

	if(ref_change_mode[c] == SEARCH_REF_begin || 
		ref_change_mode[c] == SEARCH_REF_findref ||
		ref_change_mode[c] == SEARCH_REF_hand)
		return 1;
	else
		return 0;	
}


/*
В режиме поиска реф в движении в движении
*/
bool CDisp::ifSearchRefAUTO()
{
#if (configUSE_NCshell == 1)
	for(int ch = 0; ch < N_AXIS;ch++){
		if((ref_change_mode[ch] == SEARCH_REF_begin || 
			ref_change_mode[ch] == SEARCH_REF_hand ||
		    ref_change_mode[ch] == SEARCH_REF_findref ) 
			&& Lir.sts_c[ch])
			return 1;		
	}
#endif
	return 0;

}
/*
выбрали ось для выхода в ноль в меню ПОИСК РЕФ (без движения )для систем управления
*/
bool CDisp::ifSearchRefFINDREF()
{
	bool res = false;
 #if (configUSE_NCshell == 1)
	for(int c = 0; c < N_AXIS;c++){
		if(ref_change_mode[c] == SEARCH_REF_findref && !Lir.sts_c[c])
			res = true;
	}
#endif
	return res;
}
/*
если на каком то канале есть поиск реф в ручном режиме 1
*/
bool CDisp::ifSearchRefHAND()
{
	for(int c = 0; c < N_AXIS;c++){
		if(ref_change_mode[c] == SEARCH_REF_hand)
			return true;
	}
	return false;
}

/*
digit - индекс выводимого элемента
page  - страница памяти экрана
width - ширина символа в пикселях (ширина цифр в файле DigitLarge.h 24 pixels, а ширина точки 12 pixels)
x - координата символа по x
y - координата символа по y
*/
void CDisp::put_large_symbol(const unsigned char digit, char page,const unsigned char &width,unsigned int x,unsigned char y)
{
	
	// 09.01.2019 //  1 digit 264 us

	unsigned char x_l,x_h,l_l,l_h;
	unsigned int x_;
	unsigned char y_coord = 0;
	unsigned int full_size = width * 8;  // вычисление полного размера символа = ширина символа*кол-во строк(высота символа)
	for(unsigned int i = width; i<=full_size; i += width){
	
	write_c(0x25);
	//start y
	write_d(y+y_coord);
	//end y ;
	write_d(y+y_coord);
	y_coord++;
	//memory page inside display
	write_d(page);
	write_d(0xa5);
	if(i == width)
	{
	//----------------
	x=x+160;
	x_=x;
	x_l=(unsigned char)x;
	x=x>>8;
	x_h=(unsigned char)x;
	x=x_+31;
	// if (l!=0)x=x-1;
	l_l=(unsigned char)x;
	x=x>>8;
	l_h=(unsigned char)x;
	//---------------------
	write_c(0x26);
	write_d(x_h);
	write_d(x_l);
	// 160+320=480 0x01e0-1 = 0x01df
	write_d(l_h);
	write_d(l_l);
	}
	//---------------------

	//----------------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(unsigned int n = i - width; n < i;n++) {
		//clr_nWR();
		PORTF.OUT = (invert_char) ? ~(pgm_read_byte(&DigitLarge[digit][n])) :pgm_read_byte(&DigitLarge[digit][n]);//string_0[n];
		cli();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//return;
	}
	return;
}
/*
digit - индекс выводимого элемента
page  - страница памяти экрана
width - ширина символа в пикселях (ширина цифр в файле _new_0.h 24 pixels, а ширина точки 12 pixels)
x - координата символа по x  по 1 пикселю
y - координата символа по y  по 8 пикселей
*/
void CDisp::put_big_symbol(const unsigned char digit, char page,const unsigned char &width,unsigned int x,unsigned char y)
{		
	PGM_P p = (const char *)_new_0;
	    if(digit >=0 && digit <= 9)
			p = (const char *)_new_0 + (digit*120);		
		else if(digit == 0x0B)
			p = (const char *)_white_space;
	    else if(digit == 0x0A) // '-'
			p = (const char *)_new_mines;
		else if(digit == 0x0C) // '.'
			p = (const char *)_new_dot;
		else if(digit == 0x0D) // '°'
			p = (const char *)_new_degrees;
		else if(digit == 0x0E) //0x01 '\''
			p = (const char *)_new_minute;
		else if(digit == 0x0F) //0x02 '\"' 
			p = (const char *)_new_second;
		else if(digit == 0x10) //':'
			p = (const char *)_new_colon;
#if (configUSE_RGB_disp == 0)
	unsigned char x_l,x_h,l_l,l_h;
	unsigned int x_;
	unsigned char y_coord = 0;
	unsigned int full_size = width * 5;  // вычисление полного размера символа = ширина символа*кол-во строк(высота символа)
	for(unsigned int i = width; i<=full_size; i += width){		
		write_c(0x25);
		write_d(y+y_coord);
		write_d(y+y_coord);
		y_coord++;
		write_d(page);
		write_d(0xa5);
		if(i == width)
		{
			x=x+160;
			x_=x;
			x_l=(unsigned char)x;
			x=x>>8;
			x_h=(unsigned char)x;
			x=x_+31;
			l_l=(unsigned char)x;
			x=x>>8;
			l_h=(unsigned char)x;
			write_c(0x26);
			write_d(x_h);
			write_d(x_l);
			write_d(l_h);
			write_d(l_l);
		}
		write_c(0x2c);
		write_d(0xa5);
		PORTA.OUTSET =0x40;
		PORTA.OUTCLR =0x20;
		for(unsigned int n = i - width; n < i;n++) {
			PORTF.OUT = (invert_char) ? ~(pgm_read_byte(p + n)) :pgm_read_byte(p + n);//string_0[n];
			cli();
			PORTA.OUTCLR = 0x80;
			PORTA.OUTSET = 0x80;
			sei();
		}
		PORTA.OUTSET =0x20;
	}
#endif
#if (configUSE_RGB_disp == 1)
	//x += 10; 
	char R = 0;
	char G = 0;
	char B = 0;
	if(y == 0)
	 R = 0xfc;
	else if( y == 7)
	 G = 0xfc;
	else if( y == 14)
	 B = 0xfc;
	else if( y == 21){
	 R = 0xfc;
	 B = 0xfc;
	 }


	write_c(0x2a);
	write_d(x >> 8);
	write_d(x);
	write_d((x + (width - 1)) >> 8);
	write_d(x + (width - 1));
	y = (y/7 * (8 * 5 + 16)); 
	write_c(0x2b);
	write_d(y >> 8); //start hi
	write_d(y); //lo
	write_d((y + (width * 5) - 1) >> 8); //end hi
	write_d(y + (width * 5) - 1);  //lo
	 
	write_c(0x2c);//GO
	for (int half = 0; half < width * 5; half += width) {
		for (unsigned char bit = 1; bit != 0; bit *= 2) {
			for (int byte = half; byte < (width + half); byte++) {
				if (pgm_read_byte(p + byte) & bit)
					send_pixel_color(R,G,B); //black
				else
					send_pixel_color(0xfc,0xfc,0xfc); //white
			}
		}
	}
#endif
	return;
}

//28.04.2017


/*
unsigned char *str   - указатель на буфер строки
const short sz_str   - размер буфера со строкой
const short place    - место куда надо поставить точку
*/
void CDisp::add_point(unsigned char * str, const short sz_str, const short place)
{
	char n = strnlen_lir((char*)str);
	short pos_p = place - 1;
	if (n) {
		if (n + 1 < sz_str) {
			int i = n;
			for (;i >= pos_p ; i-- ) {
				str[i + 1] = str[i];
			}
			str[i + 1] = '.';
		}
	}
}

void CDisp::show_about_programm()
{
	print_flash_local(s00_ABOU,0,0,0);
	print_flash_local(s01_ABOU,0,0,16);
	print_flash_local(s02_ABOU,0,0,32);
	print_flash_local(s03_ABOU,0,0,48);
	print_flash_local(s04_ABOU,0,0,64);
	print_ram_local((char*)p_DATE,0,14,64);
	print_flash_local(s05_ABOU,0,0,80);
	Lir.menu_2_ABOU();
	//print_ram_local((char*)p_TIME,0,16,80);
	//print_flash_local(PSTR(" GNU компилятор:"),0,0,128);
	//print_ram_local((char*)__VERSION__,0,17,128);	
	//print_debug_one_local(__GNUC__,0,0,96);
	print_flash_local(s06_ABOU,0,0,224);			
	print_ram_local(VERSION_ram,0,18,224);
}

void CDisp::show_menu_hand()
{
//clear_ram(0,0);
 printf_flash2(PSTR("Ручной режим    "),0,0,0);
}


void CDisp::show_time(unsigned int x_,unsigned char y_)
{
    display.print_ram_local((char*)bufftime,0,x_,y_);
}


void CDisp::clear_ram_1()
{
if(display.clearram)
{
	display.clear_ram(0,0);
	display.clearram = false;
}
}

void CDisp::clear_ram_null(unsigned char page)
{
display.clear_ram(0,page);
Lir.status(PSTR(""),0,0,1);//сброс буфера
}

int CDisp::putdotinstring2(char * str,int n_after_dot,int n)
{
	if (n_after_dot == 0) return 0;

	int x = 0;
	int f = n - n_after_dot;

	int a = 0;
	int b = 0;
	if (f == -4) a = 6;
	if (f == -3) a = 5;
	if (f == -2) a = 4;
	if (f == -1) a = 3;
	if (f == 0)  a = 2;
	if (f == 1)  a = 1;
	if (f == 2)  a = 0;
	if (f == 3) { a = 0; b = 1; }
	if (f == 4) { a = 0; b = 2; }
	if (f == 5) { a = 0; b = 3; }
	if (f == 6) { a = 0; b = 4; }

	char buf[15] = { 0 };
	if (f > 0)
	{
		x = n;
		while (x)
		{
			str[f + x] = str[f + x - 1];
			x--;
		}
		str[f] = '.';
	}
	if (f <= 0)
	{
		buf[0] = '0';
		buf[1] = '.';
		for (int d = 2;d < a;d++)
		buf[d] = '0';

		for (int x = 0; x < n; x++)
		{
			buf[x + a] = str[x + b];
		}
		strncpy_lir(str,buf,13);
	}
	return 1;
}

void CDisp::init_VERSION()
{
	//00 - инкрементные; без коррекции; 16 входов;
	//char VERSION_ram[13] = {"540.2502.00"};
	//char VERSION_ram[13] = {"540.2550.00"}; c 24.11.2020
	strncpy_lir(VERSION_ram,"540.2600.00",13);

	if(configUSE_mcx514 == 1)
		VERSION_ram[2] = '2';
	else if(configUSE_LIR541 == 1)
		VERSION_ram[2] = '1';

	if(configUSE_max7370 == 0)
		VERSION_ram[4] = '1';

	if(VERSION_ram[2] == '0'){ //ЛИР540
		//01 - инкрементные;  с коррекцией; 16 входов;
		if(configUSE_ABS_Encoder == 0 && configUSE_menuItem_Correction_and_Izmerenia == 1 && configUSE_NO_input == 0)
			VERSION_ram[10] = '1';
		//02 -   абсолютные; без коррекции; 16 входов;
		if(configUSE_ABS_Encoder == 1 && configUSE_menuItem_Correction_and_Izmerenia == 0 && configUSE_NO_input == 0)
			VERSION_ram[10] = '2';
		//03 - инкрементные; без коррекции; 00 входов;
		if(configUSE_ABS_Encoder == 0 && configUSE_menuItem_Correction_and_Izmerenia == 0 && configUSE_NO_input == 1)
			VERSION_ram[10] = '3';
		//90 - английский язык
		if(configUSE_Lang == 1)
			VERSION_ram[9] = '9';
	}
	if(VERSION_ram[2] == '1'){ //ЛИР541
		//01 - с подключением ПЛК + база + коррекция; 
		if(configUSE_PLC == 1 && configUSE_menuItem_Correction_and_Izmerenia == 1)
			VERSION_ram[10] = '2';
		//01 - с подключением ПЛК + база;
		else if(configUSE_PLC == 1)
			VERSION_ram[10] = '1';
	}

	if(VERSION_ram[2] == '2'){ //ЛИР542
		//01 - с подключением ПЛК + база;
		if(configUSE_PLC == 1)
			VERSION_ram[10] = '1';
	}
}

void CDisp::only_test_to_see_big_digit(long counter){

  unsigned char string_size,n,index;
  unsigned char txt_out[15] = {0};
  char reverse_order[13];

  if(counter>99999999 || counter<-99999999) 
		return;
  
  screen.convert_channel_counter_to_text_string(3,counter,txt_out);

	for(n=0;n<15;n++){
		if(txt_out[n]==0)
			break;
	}

	index=string_size=n;

	for(n=0;n<string_size;n++)
		reverse_order[n]=txt_out[--index];

	if(n==0 || n==1 || n==2)
		return;

	reverse_order[string_size]=0;
	index=n;
	Lir.put_big_string(string_size,reverse_order,22);	
}

//---------------------------
void CDisp::put_char_local(char symbol, char page,unsigned int *x_local,unsigned char *y_local){
 
	unsigned int k = 0;
	unsigned int s=symbol;
	unsigned int i;
	for(i = 0; i < 16; i++){
		//07.09.2017 it works only with strings which is placed in first 64k Flash memory
		tmp_0[i] =(invert_char) ? ~pgm_read_byte_near(&_txtnew[s][k++]):pgm_read_byte_near(&_txtnew[s][k++]);
	}
#if (configUSE_RGB_disp == 0)
		unsigned char x_l,x_h,l_l,l_h;
		unsigned int x_,x1;
		unsigned int l=8;
		
	write_c(0x25);
	write_d(*y_local);
	write_d(*y_local+1);
	write_d(page);
	write_d(0xa5);
	x1=*x_local;
	x_=x1;
	x_l=(unsigned char)x1;
	x1=x1>>8;
	x_h=(unsigned char)x1;
	x1=x_+l-1;
	l_l=(unsigned char)x1;
	x1=x1>>8;
	l_h=(unsigned char)x1;
	write_c(0x26);
	write_d(x_h);
	write_d(x_l);
	write_d(l_h);
	write_d(l_l);
	write_c(0x2c);
	write_d(0xa5);
	PORTA.OUTSET =0x40;
	PORTA.OUTCLR =0x20;
	for(int i=0;i<16;i++) {
		PORTF.OUT=tmp_0[i];
		PORTA.OUTCLR =0x80;
		PORTA.OUTSET =0x80;
	}
	PORTA.OUTSET =0x20;
#endif
#if (configUSE_RGB_disp == 1)
	unsigned short x = *x_local;
	write_c(0x2a);
	write_d(x >> 8);
	write_d(x);
	write_d((x + (8 - 1)) >> 8);
	write_d(x + (8 - 1));
	unsigned short y = *y_local;
	write_c(0x2b);
	write_d(y >> 8); //start hi
	write_d(y); //lo
	write_d((y + ((8 * 2) - 1)) >> 8); //end hi
	write_d(y + (8 * 2) - 1);  //lo
	
	write_c(0x2c);
	for (int half = 0; half < 16; half += 8) {
		for (unsigned char bit = 1; bit != 0; bit *= 2) {
			for (int byte = half; byte < (8 + half); byte++) {
				if (tmp_0[byte] & bit) 
					send_pixel_color(0x00,0x00,0x00); //black														
				else 
					send_pixel_color(0xfc,0xfc,0xfc); //white
			}
		}
	}
#endif
}
/*
char symbol - код символа
char page - страница памяти экрана
unsigned int x_local - координата по оси x
unsigned char y_local - координата по оси y
*/
void CDisp::put_char_local_independent(char symbol, char page,unsigned int x_,unsigned char y_){
//27.12.2017 it takes 1147 cycles 1147*31.25=35843.75 ns 
//26.12.2017
#if (configUSE_RGB_disp == 0)
x_=x_*8+160;
y_=y_>>3;
#endif
#if (configUSE_RGB_disp == 1)
x_=x_*8;
#endif
put_char_local(symbol,page,&x_,&y_);
}



void CDisp::put_char(char symbol, char page){

	//it takes 1771*31.25=55.34375 mks

	unsigned char x_l,x_h,l_l,l_h;
	//int m;
	unsigned int x_,x1;
	//unsigned int x;
	//unsigned char y;
	unsigned int l=8;
	unsigned int i, k = 0;
	//unsigned char tmp[16];
	//symbol = 192;
	unsigned int s=symbol;
	for(i = 0; i < 16; i++){
//07.09.2017 it works only with strings which is placed in first 64k Flash memory
		tmp_0[i] =(invert_char) ? ~pgm_read_byte_near(&_txtnew[s][k++]):pgm_read_byte_near(&_txtnew[s][k++]);
	}
	//-----------------------
	//m=l;
	//240 0xf0-1 = 0xef
	write_c(0x25);
	//start y
	write_d(y);
	//end y ;
	write_d(y+1);
	//memory page inside display
	write_d(page);
	write_d(0xa5);
	//-----------------------
	//x=x+160;
	x1=x;
	x_=x1;
	x_l=(unsigned char)x1;
	x1=x1>>8;
	x_h=(unsigned char)x1;
	x1=x_+l-1;
	// if (l!=0)x=x-1;
	l_l=(unsigned char)x1;
	x1=x1>>8;
	l_h=(unsigned char)x1;
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
	//---------------------
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int i=0;i<16;i++) {
		//clr_nWR();
		PORTF.OUT=tmp_0[i];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//-----------------------------
}

void CDisp::put_growing_line(char page,char symbol,unsigned int x,unsigned char y){
//		unsigned int x_=x*8+160;
        unsigned int x_=x+160;
		unsigned char y_=y;
	    unsigned int x1;
	    unsigned char x_l,x_h,l_l,l_h;
		unsigned int l=1;
	//240 0xf0-1 = 0xef
	write_c_asm(0x25);
	//start y
	write_d_asm(y_);
	//end y ;
	write_d_asm(y_);
	//memory page inside display
	write_d_asm(page);
	write_d_asm(0xa5);
	x1=x_;
	x_l=(unsigned char)x1;
	x1=x1>>8;
	x_h=(unsigned char)x1;
	x1=x_+l-1;
	// if (l!=0)x=x-1;
	l_l=(unsigned char)x1;
	x1=x1>>8;
	l_h=(unsigned char)x1;	
	write_c_asm(0x26);
	write_d_asm(x_h);
	write_d_asm(x_l);
	// 160+320=480 0x01e0-1 = 0x01df
	write_d_asm(l_h);
	write_d_asm(l_l);
	//---------------------
	write_c_asm(0x2c);
	write_d_asm(0xa5);	
		 a0_to_one_cs_to_zero_asm();

			 write_data_asm(symbol);
	
		 cs_to_one_asm();
}
//---------------------------------------------------
void CDisp::put_string_local(unsigned char *str,char page,unsigned int *x_local,unsigned char *y_local){

	unsigned char i,j,symbol,k;
	unsigned int l=88;
	unsigned char x_l,x_h,l_l,l_h;
	unsigned int x_,x1;
	//clear string takes 1541 for one  1541*31.25=48.15 mks
	for(i = 0; i < 88; i++)tmp_0[i]=0x00;
	for(i = 0; i < 88; i++)tmp_1[i]=0x00;
	//fill string takes 2062 for one  2062*31.25=64.43 mks string (-256)
	for (j=0,i = 0;j<32;j++) {
		symbol=str[j];
		if (symbol==0x00) break;
		for(k=0; k < 8; ){
		//tmp_0[i++] = pgm_read_byte(&_txtnew[symbol][k++]);
tmp_0[i++] =(invert_char) ? ~pgm_read_byte(&_txtnew[symbol][k++]):
                            pgm_read_byte(&_txtnew[symbol][k++]);
			//tmp_0[i++] = (invert_char) ? ~_txtnew[symbol][k++] : _txtnew[symbol][k++];
		}
	}
	for (j=0,i = 0;j<32;j++) {
		symbol=str[j];
		if (symbol==0x00) break;
		for(k=8; k < 16;){
//		tmp_1[i++] = pgm_read_byte(&_txtnew[symbol][k++]);
tmp_1[i++] =(invert_char) ? ~pgm_read_byte(&_txtnew[symbol][k++]):
                             pgm_read_byte(&_txtnew[symbol][k++]);		
			//tmp_1[i++] = (invert_char) ? ~_txtnew[symbol][k++] : _txtnew[symbol][k++];
		}
	}
	//------------till now it takes 48.15*2 +64.43*2=225.16 mks
	//------from here till end 5259*31.25=164.43 mks
	//m=l;
	//240 0xf0-1 = 0xef
	write_c_asm(0x25);
	//start y
	write_d_asm(*y_local);
	//end y ;
	write_d_asm(*y_local);
	//memory page inside display
	write_d_asm(page);
	write_d_asm(0xa5);
	//x=x+160;
	x1=*x_local;
	x_=x1;
	x_l=(unsigned char)x1;
	x1=x1>>8;
	x_h=(unsigned char)x1;
	x1=x_+l-1;
	// if (l!=0)x=x-1;
	l_l=(unsigned char)x1;
	x1=x1>>8;
	l_h=(unsigned char)x1;
	//---------------------
	write_c_asm(0x26);
	write_d_asm(x_h);
	write_d_asm(x_l);
	// 160+320=480 0x01e0-1 = 0x01df
	write_d_asm(l_h);
	write_d_asm(l_l);
	//---------------------
	write_c_asm(0x2c);
	write_d_asm(0xa5);
	//---------------------
	//set_A0();
	//PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	//PORTA.OUTCLR =0x20;
	 a0_to_one_cs_to_zero_asm();	
	for(int i=0;i<88;i++) {
		//clr_nWR();
		//PORTF.OUT=tmp_0[i];
		//cli();//__disable_interrupt();
		//PORTA.OUTCLR =0x80;
		//set_nWR();
		//PORTA.OUTSET =0x80;
		//sei();//__enable_interrupt();
		// write_d(data);
		write_data_asm(tmp_0[i]);
		
	}
	//set_nCS();
	//PORTA.OUTSET =0x20;
	cs_to_one_asm();
	//-----------------------------

	//m=l;
	//240 0xf0-1 = 0xef
	write_c_asm(0x25);
	//start y
	write_d_asm(*y_local+1);
	//end y ;
	write_d_asm(*y_local+1);
	//memory page inside display
	write_d_asm(page);
	write_d_asm(0xa5);
	//---------------------
	write_c_asm(0x2c);
	write_d_asm(0xa5);
	//---------------------
	//set_A0();
	//PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	//PORTA.OUTCLR =0x20;
	 a0_to_one_cs_to_zero_asm();	
	for(int i=0;i<88;i++) {
		//clr_nWR();
		//PORTF.OUT=tmp_1[i];
		//cli();//__disable_interrupt();
		//PORTA.OUTCLR =0x80;
		//set_nWR();
		//PORTA.OUTSET =0x80;
		//sei();//__enable_interrupt();
		// write_d(data);
		write_data_asm(tmp_1[i]);
	}
	//set_nCS();
	//PORTA.OUTSET =0x20;
    cs_to_one_asm();
}
void CDisp:: put_axis_shot(unsigned int x,unsigned char y,unsigned char name_axis){
	//12.10.2018 16 axis's name
	// it takes 7217*31.25=225531.25 ns but has 500 bytes economy
	// if inverse 7537*31.25=235531.25 ns
	unsigned char i,k=0;
	for(i = 0; i < 160; i++){		
		string_0[i] = (invert_char) ? ~(pgm_read_byte(&axis_big[name_axis][k++])) :pgm_read_byte(&axis_big[name_axis][k++]); // 580ns        
	}

	#if (configUSE_RGB_disp == 1)
	//x += 10;
	write_c(0x2a);
	write_d(x >> 8);
	write_d(x);
	write_d((x + (32 - 1)) >> 8);
	write_d(x + (32 - 1));
	y = (y/7 * (8 * 5 + 16));
	write_c(0x2b);
	write_d(y >> 8); //start hi
	write_d(y); //lo
	write_d((y + (32 * 5) - 1) >> 8); //end hi
	write_d(y + (32 * 5) - 1);  //lo
	
	write_c(0x2c);//GO
	for (int half = 0; half < 32 * 5; half += 32) {
		for (unsigned char bit = 1; bit != 0; bit *= 2) {
			for (int byte = half; byte < (32 + half); byte++) {
				if (string_0[byte] & bit)
					send_pixel_color(0x00,0x00,0x00); //black
				else
					send_pixel_color(0xfc,0xfc,0xfc); //white
			}
		}
	}
	#endif

	#if (configUSE_RGB_disp == 0)
	unsigned char x_l,x_h,l_l,l_h;
	unsigned int x_;
	write_c(0x25);
	//start y
	write_d(y);
	//end y ;
	write_d(y);
	//memory page inside display
	write_d(0);
	write_d(0xa5);
	//----------------
	x=x+160;
	x_=x;
	x_l=(unsigned char)x;
	x=x>>8;
	x_h=(unsigned char)x;
	x=x_+31;
	// if (l!=0)x=x-1;
	l_l=(unsigned char)x;
	x=x>>8;
	l_h=(unsigned char)x;
	//---------------------
	write_c(0x26);
	write_d(x_h);
	write_d(x_l);
	// 160+320=480 0x01e0-1 = 0x01df
	write_d(l_h);
	write_d(l_l);
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=0;n<32;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	write_c(0x25);
	//start y
	write_d(y+1);
	//end y ;
	write_d(y+1);
	//memory page inside display
	write_d(0);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=32;n<64;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//----------------
	write_c(0x25);
	//start y
	write_d(y+2);
	//end y ;
	write_d(y+2);
	//memory page inside display
	write_d(0);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=64;n<96 ;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	//----------------
	write_c(0x25);
	//start y
	write_d(y+3);
	//end y ;
	write_d(y+3);
	//memory page inside display
	write_d(0);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=96 ;n<128;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;

	//----------------
	write_c(0x25);
	//start y
	write_d(y+4);
	//end y ;
	write_d(y+4);
	//memory page inside display
	write_d(0);
	write_d(0xa5);
	//----------------
	write_c(0x2c);
	write_d(0xa5);
	//320*(240/8)=9600
	//set_A0();
	PORTA.OUTSET =0x40;
	// PORTF.OUT=0xff;
	//clr_nCS();
	PORTA.OUTCLR =0x20;
	for(int n=128;n<160;n++) {
		//clr_nWR();
		PORTF.OUT= string_0[n];
		cli();//__disable_interrupt();
		PORTA.OUTCLR =0x80;
		//set_nWR();
		PORTA.OUTSET =0x80;
		sei();//__enable_interrupt();
		// write_d(data);
	}
	//set_nCS();
	PORTA.OUTSET =0x20;
	#endif

}

void CDisp::print_ram_local( char * str, unsigned char page,unsigned int x_,unsigned char y_){
	unsigned char j = 0;
#if (configUSE_RGB_disp == 0)
	x_=x_*8+160;
	y_=y_>>3;
#endif
#if (configUSE_RGB_disp == 1)	
	x_=x_*8;
#endif
	do{
		put_char_local(str[j],page,&x_,&y_);
		x_=x_+8;
	}while(str[++j] != '\0');
}

void CDisp::print_ram_local_RA(char * str, unsigned char page,unsigned int x_, unsigned char y_)
{
	unsigned char j = 0;
#if (configUSE_RGB_disp == 0)
	x_=x_*8+160;
	y_=y_>>3;
#endif
#if (configUSE_RGB_disp == 1)	
	x_=x_*8;
#endif
	signed char n = display.strnlen_lir((char*)str) - 1;
	char tmp;
	for (int i = 0; n > i; i++, n--) {
		tmp = str[i];
		str[i] = str[n];
		str[n] = tmp;
	}
	
	do{
		put_char_local(str[j],page,&x_,&y_);
		x_=x_-8;
	}while(str[++j] != '\0');
}

//22.12.2017 if string has symbols which are the same( twenty  0x20 for example)
//x_ can be 0-319,y_ can be 0-239
void  CDisp::print_symbols_local(char symbol,int number,unsigned char page,unsigned int x_, unsigned char y_){
	if (number<=0)return;
#if (configUSE_RGB_disp == 0)
	x_=x_*8+160;
	y_=y_>>3;
#endif
#if (configUSE_RGB_disp == 1)	
	x_=x_*8;
#endif
	do{
		put_char_local(symbol,page,&x_,&y_);
		x_=x_+8;
		number--;
	}while(number != 0);
}

void  CDisp::print_symbols_local_RA(char symbol,int number,unsigned char page,unsigned int x_, unsigned char y_){
if (number<=0)return;
#if (configUSE_RGB_disp == 0)
x_=x_*8+160;
y_=y_>>3;
#endif
#if (configUSE_RGB_disp == 1)	
	x_=x_*8;
#endif
do{
	put_char_local(symbol,page,&x_,&y_);
	x_=x_-8;
	number--;
}while(number != 0);
}
/*
const char * str - указатель на строку
unsigned char page - номер страницы памяти экрана
unsigned int x_ - координата по x
unsigned char y_ - координата по y
char align - выравнивание текста; 1 - по правому краю  ,0 - по левому краю
*/
void  CDisp::print_flash_local( const char * str , unsigned char page,unsigned int x_, unsigned char y_,char align){

	//malloc takes 101 cycles.101*31.25=3156.25 ns
	if(str == NULL){
		print_ram_local((char *)"?",0,x_,y_);
		return;
	}
	char buffer[50]={0};
	/*char *buffer=(char*)malloc(50);	
	if(buffer == 0)
	{
		clear_ram_null();
		print_ram_local((char *)"ошибка malloc",0,0,0);
		delay_ms(2000);
	} */
//30.08.2017 It must be funny, but if *str does not have 0x00 in end of buf
//it will be endless loop inside strcpy_P(buffer,str);
//07.09.2017 it works only with strings which are placed in first 64k Flash memory
	strcpy_P(buffer,str);
	buffer[49] = '\0';
	if(align == 0)
		display.print_ram_local(buffer,page,x_,y_);
	if(align == 1)
		display.print_ram_local_RA(buffer,page,x_,y_);

//free takes 57 cycles 57*31.25=1781.25 ns
	//free(buffer);	
}

void CDisp::print_flash_( const char * str, unsigned char page){
//17.10.2017 it takes 7465 cycles if probel10 is written. 7465*31.25=233281.25 ns	
	//unsigned char j = 0;
	//	char buffer[50];
	//malloc takes 101 cycles.101*31.25=3156.25 ns
	//char *buffer=(char*)malloc(50);
	//30.08.2017 It must be funny, but if *str	does not have 0x00 in end of buf 
	//it will be endless loop inside strcpy_P(buffer,str);
	//07.09.2017 it works only with strings which are placed in first 64k Flash memory
	//strcpy_P(buffer,str);
	
	do{
		put_char(pgm_read_byte(str),page);
		x=x+8;
		
	}while(pgm_read_byte(str++) != '\0');
	//free takes 57 cycles 57*31.25=1781.25 ns
	//free(buffer);
}

void CDisp::print_flash_local_(const char * str , unsigned char page,unsigned int x, unsigned char y)
{
	do{
		put_char_local(pgm_read_byte(str),page,&x,&y);
		x=x+8;
		
	}while(pgm_read_byte(str++) != '\0');
}

void CDisp::printf_flash2(const char * str , unsigned char page,unsigned int x_, unsigned char y_)
{   
   // gotoxy(x_, y_);
   // print_flash(str,page);	
   print_flash_local(str,page,x_,y_);	
}


//======================================================

void CDisp::init_lcd_RGB()
{
	//software reset
	write_c(0x01);
	_delay_ms(10);
	
	write_c(0xe0);
	write_d(0x01);      //START PLL
	_delay_ms(50);      //delay50ms(2013 08 05 New )
	write_c(0xe0);
	write_d(0x03);      //LOCK PLL

	write_c(0xb0);      //SET LCD MODE    SET TFT 18Bits MODE
	write_d(0x0c);      //SET TFT MODE & hsync+Vsync+DEN MODE
	write_d(0x00);      //SET TFT MODE & hsync+Vsync+DEN MODE
	write_d(0x01);      //SET horizontal size=320-1 HightByte
	write_d(0x3f);      //SET horizontal size=320-1 LowByte
	write_d(0x00);      //SET vertical size=240-1 HightByte
	write_d(0xef);      //SET vertical size=240-1 LowByte
	write_d(0x00);      //SET even/odd line RGB seq.=RGB


	write_c(0xf0);		//SET pixel data I/F format=8bit
	write_d(0x00);
	write_c(0x3a);		// SET R G B format = 6 6 6
	write_d(0x60);
	
	write_c(0x36);		// SET read from frame buffer to the display is RGB
	write_d(0x02);    //old 0x08
	
	write_c(0xe2);		//SET the MN of PLL
	write_d(0x1d);		// Multiplier (M)
	write_d(0x02);		// Divider (D)
	write_d(0x54);

	write_c(0xe6);		//SET PCLK freq=4.94MHz    ; pixel clock frequency
	write_d(0x01);		//00
	write_d(0x40);		//ce
	write_d(0xff);		//94

	write_c(0xb4);		//SET HBP,
	write_d(0x01);      //SET HSYNC Total = 440
	write_d(0xb8);
	write_d(0x00);      //SET HBP = 68
	write_d(0x44);
	write_d(0x07);      //SET VBP 16 = 15 + 1
	write_d(0x00);      //SET Hsync pulse start position
	write_d(0x00);
	write_d(0x00);      //SET Hsync pulse subpixel start position


	write_c(0xb6);      //SET VBP,
	write_d(0x01);      //SET Vsync total 265 = 264 + 1
	write_d(0x08);
	write_d(0x00);      //SET VBP = 19
	write_d(0x13);
	write_d(0x07);      //SET Vsync pulse 8 = 7 + 1
	write_d(0x00);      //SET Vsync pulse start position
	write_d(0x00);

	write_c(0x2a);		//SET column address
	write_d(0x00);      //SET start column address=0
	write_d(0x00);
	write_d(0x01);      //SET end column address=320
	write_d(0x3f);

	write_c(0x2b);		//SET page address
	write_d(0x00);      //SET start page address=0
	write_d(0x00);
	write_d(0x00);      //SET end page address=240
	write_d(0xef);

	write_c(0x29);		//SET display on
	write_c(0x2c);
	clear_ram_RGB(0xffffff,0x0000,0x013f,0x0000,0x00ef);
}

void CDisp::clear_ram_RGB(unsigned long data,unsigned int s_x,unsigned int e_x,unsigned int s_y,unsigned int e_y)
{
	//21.03.2016 it takes 5.3 ms
	//21.03.2016 page 0 0x00, 1 0x01, 2 0x02, 3 0x03
	// page's size is 320 (column from left to right)* 240 (line from up to down)
	// column's step is 1 pixel, line's step is 8 pixel
	//240 0xf0-1 = 0xef
	write_c(0x2a);    //SET page address
	write_d((s_x)>>8);    //SET start page address=0
	write_d(s_x);
	write_d((e_x)>>8);    //SET end page address=320
	write_d(e_x);

	write_c(0x2b);    //SET column address
	write_d((s_y)>>8);    //SET start column address=0
	write_d(s_y);
	write_d((e_y)>>8);    //SET end column address=240
	write_d(e_y);
	//---------------------
	write_c(0x2c);
	//write_d(0xa5);
	//320*(240/8)=9600

	unsigned int x,y;

	for (x=0;x<240;x++){
		for (y= 0;y<320;y++)
			send_pixel_color(data >> 16,data >> 8,data);
	}
}

void CDisp::send_pixel_color(char R,char G,char B)
{
	//write_d((0x00fc00)>>16);   // color is red
	//write_d((0x00fc00)>>8);    // color is green
	//write_d(0x00fc00);         // color is blue
	write_d(R);
	write_d(G);
	write_d(B);
}

void CDisp::print_debug_one_local(long debug_a, unsigned char page,unsigned int x_,unsigned char y_)
{
	//05.03.2018
	// -2147483645  12821*31.25=400656.25 ns
  /*  int  i,j,n;
	unsigned char tmp_str[15],str[15];
	//--05.03.2018--------------------
	// -2147483645 1289*31.25=40281.25 ns
	i = itoa_quick(tmp_str, &debug_a);
	
	for(j = i-1,n=0; j >= 0;) {
		str[n++]=tmp_str[j--];
	}
	str[n]=0x00;
	
	//25.12.2017
	x_=x_*8+160;
	y_=y_>>3;
	//-----------	
	put_string_local(str,page,&x_,&y_);  */

	//new metod 440 mks for -2147483645
	#define SZ_STR 15
	unsigned char txt_out[SZ_STR] = {0};

	itoa_quick_lir(&debug_a,txt_out);
	Lir.add_filler(txt_out,15,10,' ',1);
	print_ram_local((char*)txt_out,page,x_,y_);
}

void CDisp::print_debug_one_local_shot(long debug_a, unsigned char page,unsigned int x_,unsigned char y_,char amount)
{
	//05.03.2018
	// -2147483645  12821*31.25=400656.25 ns
	/*int  i,j,n;
	unsigned char tmp_str[15],str[15];
	//--05.03.2018--------------------
	// -2147483645 1289*31.25=40281.25 ns
	i = itoa_quick(tmp_str, &debug_a);
	
	for(j = i-1,n=0; j >= 0;) {
		str[n++]=tmp_str[j--];
	}
	str[n]=0x00;
	//25.12.2017
	x_=x_*8+160;
	y_=y_>>3;
	//-----------
	put_string_local_shot(str,page,&x_,&y_,&amount); */

	//new metod 440 mks for -2147483645
	#define SZ_STR 15
	unsigned char txt_out[SZ_STR] = {0};

	itoa_quick_lir(&debug_a,txt_out);
	if(amount > 0)
		amount--;
	else
		amount = 0;
	Lir.add_filler(txt_out,15,amount,' ',1);
	print_ram_local((char*)txt_out,page,x_,y_);
}

/*
	старая функция специально предназначенная для вывода на экран коэффициента 
	коэффициент может быть с 8 разрядами после точки
	long debug_a - коэффициент 
	char n_after_dot - кол - во разрядов после точки
	unsigned char page - страница памяти RAM экрана
	unsigned int x_ - координата x
	unsigned char y_ - координата y
	char align - выравнивание 0 - слева  ,1 - справа
	char clear - размер поля для очистки 
*/
void CDisp::print_coeff(long debug_a,char n_after_dot, unsigned char page,unsigned int x_,unsigned char y_,char align /*= 0*/,char clear /*= 0*/)
{
	int  i,j,n;
	unsigned char tmp_str[15],str[15];
	i = itoa_quick(tmp_str, &debug_a);
	for(j = i-1,n=0; j >= 0;) {
		str[n++] = tmp_str[j--];
	}
	str[n]=0x00;
	//putdotinstring2((char*)str,n_after_dot,i);  // - 186bytes
	Lir.add_filler(str, sizeof(str), n_after_dot, '0', 0);
	if(n_after_dot > 0)
		insert_sym(str, sizeof(str), n_after_dot, '.');

	if(align == 0){
		if(clear)
			Lir.add_filler(str,sizeof(str),clear,' ',1);
		print_ram_local((char*)str,page,x_,y_);
	}

	if(align == 1){
		if(clear)
			Lir.add_filler(str,sizeof(str),clear,' ',0);
		print_ram_local_RA((char*)str,page,x_,y_);
	}

}


void CDisp::print_debug_one_local_unsigned(unsigned long debug_a, unsigned char page,unsigned int x_,unsigned char y_)
{
	//27.04.2017
	// -2147483645  21266*31.25=664593.75 mks
	int  i,j,n;
	unsigned char tmp_str[15],str[15];
	//--04.04.2016--------------------
	//(debug_a=-256) 765*31.25=23.9 mks
	i = itoa_quick_unsigned(tmp_str, &debug_a);
	// order from left to right 109*31.25=3.4 mks
	for(j = i-1,n=0; j >= 0;) {
		str[n++]=tmp_str[j--];
	}
	str[n]=0x00;
	//    str[0]=0x30; str[1]=0x31;str[2]=0x32; str[3]=0x00;
	//  12610*31.25=394 mks (debug_a=-256)
	//25.12.2017
	x_=x_*8+160;
	y_=y_>>3;
	//-----------
	put_string_local(str,page,&x_,&y_); 
}

int CDisp::strncmp_lir(const char *str1, const char *str2, int maxcount)
{
	int a = strnlen_lir(str1);
	int b = strnlen_lir(str2);
	
	if (a != b) return 1;

	for (int x = 0; x < a; x++)
	{
		if (str1[x] != str2[x]) return 1;
	}
	return 0;
}
/*
функция копирует из Source в Dest
*/
int CDisp::strncpy_lir(char *Dest, const char *Source, int size_Dest)
{
	for (int x = 0; x < size_Dest; x++)
	{
		Dest[x] = Source[x];
		if (Source[x] == 0x00) 
			break;
	}
	return 1;
}
/*
функция считает размер строки без учета завершающего символа '\0'
т.к. счет начинается с нуля
*/
int CDisp::strnlen_lir(const char *str, int maxcount)
{
	int x = 0;
	for ( ;x < maxcount; x++)
	{
		if (str[x] == 0x00)
			break;
	}
	return x;
}
/*
функция конкатенирует строку 1 со строкой 2,
если суммарный размер конкатенируемых строк больше размера строки 1, то
результат будет обрезан до размера строки 1
*/
int CDisp::strncat_lir(char * str1,const int &sz_str1, char * str2)
{
	for (int i = 0; i < sz_str1 - 1; i++)
	{
		if (str1[i] == '\0')
		{
			for (int j = 0; i < sz_str1 - 1; j++,i++)
			{
				if (str2[j] == '\0') 
					break;
				str1[i] = str2[j];
			}
			str1[i] = '\0';
			return 1;
		}
	}
	return 0;
}
/*
функция конкатенирует один символ в конец строки 1
out char * str1 - указатель на первую строку
in  const int & sz_str1 - размер 1ой строки в байтах
in  char sym_ - ASCII код символа
*/
int CDisp::strncatsym_lir(char * str1, const int & sz_str1, char sym_)
{
	for (int i = 0; i < sz_str1 - 1; i++)
	{
		if (str1[i] == '\0')
		{
			str1[i] = sym_;
			str1[i + 1] = '\0';
			return 1;
		}
	}
	return 0;
}
/*
функция вставляет символ в массив
out char * str1 - указатель на строку
in  const short sz_str - размер строки
in  const short place - место в массиве куда вставить, отсчет с конца, (по формуле place = str_sz(размер с нулем в конце) - place)
in  char sym_ - ASCII код символа, что вставить
*/
void CDisp::insert_sym(unsigned char* str, const short sz_str, const short place, const char& sym_)
{
	char n = strnlen_lir((char*)str,sz_str);
	//if (n) {
		if (n + 1 < sz_str) {
			int i = n;
			for (int j = 0; i >= 0; i--) {
				str[i + 1] = str[i];
				if (j == place) {
					str[i] = sym_;
					break;
				}
				j++;
			}
		}
	//}
}
/*
функция удаляет символ в массиве
out char * str1 - указатель на строку
in  const short sz_str - размер строки
in  char sym_ - ASCII код символа, который нужно удалить
*/
bool CDisp::delete_sym(unsigned char* str, const short sz_str, const char& sym_)
{
	for (int i = 0; str[i] != '\0'; i++) {
		if (sz_str - 1 <= i)
			break;
		if (str[i] == sym_) {
			for (int j = i; str[j] != '\0'; j++) {
				if (sz_str - 1 <= j)
					break;
				str[j] = str[j + 1];
			}
			return true;
		}
	}
	return false;
}

/*
функция ставит точку в строчный массив и добавляет нули слева в соответствии с форматом
out char *txt_out - указатель на строчный массив с числом 
in  int f - текущий формат
*/
void CDisp::formatting2(char *txt_out, int f)
{
    int16_t cnt_zero;
    bool minus = false;

    if (f == CPrg::FORMAT::LINEAR_1 || f == CPrg::FORMAT::CIRCLE_1)
		cnt_zero = 1;
    else if (f == CPrg::FORMAT::LINEAR_2 || f == CPrg::FORMAT::CIRCLE_2)
		cnt_zero = 2;
    else if (f == CPrg::FORMAT::LINEAR_3 || f == CPrg::FORMAT::CIRCLE_3)
		cnt_zero = 3;
    else if (f == CPrg::FORMAT::LINEAR_4 || f == CPrg::FORMAT::CIRCLE_4 || f == CPrg::FORMAT::LINEAR_INCH_4)
		cnt_zero = 4;
    else if (f == CPrg::FORMAT::LINEAR_6)
		cnt_zero = 6;
    else
		cnt_zero = 0;

    if (display.delete_sym((uint8_t*)txt_out, 15, '-'))
		minus = true;
    Lir.add_filler((uint8_t*)txt_out, 15, cnt_zero, '0');
    display.insert_sym((uint8_t*)txt_out, 15, cnt_zero,'.');
    
    if(minus)
		display.insert_sym((uint8_t*)txt_out, 15, display.strnlen_lir((char*)txt_out), '-');

    if (f == CPrg::FORMAT::CIRCLE_1 || f == CPrg::FORMAT::CIRCLE_2 || f == CPrg::FORMAT::CIRCLE_3 || f == CPrg::FORMAT::CIRCLE_4)
		display.strncatsym_lir((char*)txt_out, 15, '°');
    else if (f == CPrg::FORMAT::LINEAR_INCH_4)
		display.strncatsym_lir((char*)txt_out, 15, 0x02);

    return;
}

void CDisp::itoa_quick_lir(long *counter,unsigned char *txt_out)
{
	int  i,j,n;
	unsigned char tmp_str[15];
	long tmp = *counter;

	i = display.itoa_quick(tmp_str, counter);
	
	for(j = i-1,n=0; j >= 0;) {
		txt_out[n++] = tmp_str[j--];
	}
	txt_out[n]=0x00;

	*counter = tmp;
}
/*
функция выполняет конкатенацию 1ой строки с 2ой со возможностью указать смещение в 1ой строке 
str1     -  указатель на строку 1
sz_str1  -  размер строки 1
str2     -  указатель строки 2
offset   -  смещение в строке 1 откуда начать соединение со строкой 2

out return value int - 0 - конкатенация не прошла / 1 - конкатенация прошла
*/
int CDisp::strncat_lir_offset(char * str1, const int &sz_str1, char * str2, unsigned short offset)
{
	for (int i = 0; i < sz_str1 - 1; i++)
	{
		if (i == (short)offset)
		{
			for (int j = 0; i < sz_str1 - 1; j++, i++)
			{
				if (str2[j] == '\0') break;
				str1[i] = str2[j];
			}
			str1[i] = '\0';
			return 1;
		}
	}
	return 0;
}

void CDisp::print_hex(unsigned char* buf,int sz,int x , int y)
{
	int d = 0;
	char str[40]= {0};
	for(int i = 0; i < sz;i++){
		if(buf[i] >= '0' && buf[i] <= '9'){
			str[d++] = ((buf[i] & 0xf0) >> 4) + 48;
			str[d++] = (buf[i] & 0x0f) + 48;
		}
		else if(buf[i] >='А' && buf[i] <='я'){
			str[d++] = ((buf[i] & 0xf0) >> 4) + 192;
			str[d++] = (buf[i] & 0x0f) + 192;
		}
		str[d++] = ' ';
	}
	print_ram_local(str,0,x,y);
}



//----------------------------------
