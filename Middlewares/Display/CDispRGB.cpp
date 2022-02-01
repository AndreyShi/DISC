#include "CDispRGB.h"


void CDispRGB::print_flash_local( const char * str , unsigned char page,unsigned int x, unsigned char y,char align){

	//malloc takes 101 cycles.101*31.25=3156.25 ns
	if(str == NULL){
		print_ram_local((char *)"?",0,x,y);
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
		display.print_ram_local(buffer,page,x,y);
	if(align == 1)
		display.print_ram_local_RA(buffer,page,x,y);

//free takes 57 cycles 57*31.25=1781.25 ns
	//free(buffer);

}

void CDispRGB::print_symbols_local(char symbol,int number,unsigned char page,unsigned int x_, unsigned char y_){
	if (number<=0)return;

	x_=x_*8;

	do{
		put_char_local(symbol,page,&x_,&y_);
		x_=x_+8;
		number--;
	}while(number != 0);
};
void CDispRGB::print_symbols_local_RA(char symbol,int number,unsigned char page,unsigned int x_, unsigned char y_){
	if (number<=0)return;

	x_=x_*8;

	do{
		put_char_local(symbol,page,&x_,&y_);
		x_=x_-8;
		number--;
	}while(number != 0);
};
void CDispRGB::print_ram_local( char * str, unsigned char page,unsigned int x, unsigned char y){
	unsigned char j = 0;

	x=x*8;

		do{
			put_char_local(str[j],page,&x,&y);
			x= x + 8;
		}while(str[++j] != '\0');
};
void CDispRGB::print_ram_local_RA( char * str, unsigned char page,unsigned int x, unsigned char y){

	unsigned char j = 0;
	x=x*8;
	signed char n = display.strnlen_lir((char*)str) - 1;
	char tmp;
	for (int i = 0; n > i; i++, n--) {
		tmp = str[i];
		str[i] = str[n];
		str[n] = tmp;
	}

	do{
		put_char_local(str[j],page,&x,&y);
		x=x-8;
	}while(str[++j] != '\0');

};
void CDispRGB::put_char_local(char symbol, char page,unsigned int *x_local,unsigned char *y_local){
	::put_char(symbol, *x_local, *y_local,BLACK,WHITE);
};
void CDispRGB::clear_ram(unsigned char data,unsigned char page){
	ILI9341_FillScreen(WHITE);
};



void CDispRGB::put_button_(unsigned int x,unsigned char y, unsigned char npic){
	if((BUTTON_PIC)npic == _pic_EMPTY) {
			clear_button_(x,y);
		}
		else{
			//put_button_(x,y); // кладется рамка стирая все внутри рамки
			//put_button_pic_(x,y,(BUTTON_PIC)npic); // поверх кладется иконка, (получается мерцание если закцилено)
			put_button_with_pic(x,y,(BUTTON_PIC)npic);
		}
};


void CDispRGB::clear_button_(unsigned int x,unsigned char y){
	::ILI9341_DrawRectangle(x, y, 40, 40, WHITE);
};

void CDispRGB::put_button_(unsigned int x,unsigned char y){
	::put_button(x, y * 8,BLACK,WHITE);
};

void CDispRGB::put_button_pic_(unsigned int x,unsigned char y,BUTTON_PIC npic){
	::put_button_pic(npic, x + 4 , y * 8 + 8,BLACK,WHITE);
};

void CDispRGB::put_button_with_pic(unsigned int x,unsigned char y,BUTTON_PIC npic){
	::put_button_with_pic(x,y * 8,npic,BLACK,WHITE);
};

void CDispRGB::clear_rectangle(unsigned int x,unsigned char y,unsigned int l,unsigned char h,unsigned char data,unsigned char page ){
	//::ILI9341_DrawRectangle(x, y, l, h, GREEN);
}
























