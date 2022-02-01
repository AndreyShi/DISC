#include "../../Drivers/Display/ili9341.h"
#include "../../Core/code/display.h"

#ifndef CDISP
#define CDISP
class CDispRGB :public CDisp{
public:
	void print_flash_local( const char * str , unsigned char page,unsigned int x, unsigned char y,char align = 0);
	void print_symbols_local(char symbol,int number,unsigned char page,unsigned int x_, unsigned char y_);
	void print_symbols_local_RA(char symbol,int number,unsigned char page,unsigned int x_, unsigned char y_);
	void print_ram_local( char * str, unsigned char page,unsigned int x, unsigned char y);
	void print_ram_local_RA( char * str, unsigned char page,unsigned int x, unsigned char y);
	void put_char_local(char symbol, char page,unsigned int *x_local,unsigned char *y_local);
	void clear_ram(unsigned char data,unsigned char page);


	void put_button_(unsigned int x,unsigned char y, unsigned char npic);
	void clear_button_(unsigned int x,unsigned char y);
	void put_button_(unsigned int x,unsigned char y);
	void put_button_pic_(unsigned int x,unsigned char y,BUTTON_PIC npic);
	void put_button_with_pic(unsigned int x,unsigned char y,BUTTON_PIC npic);
	void clear_rectangle(unsigned int x,unsigned char y,unsigned int l,unsigned char h,unsigned char data,unsigned char page );
};
extern CDispRGB display;

#endif
