#include "../../Drivers/Display/ili9341.h"
#include "../../Core/code/display.h"

#ifndef CDISP
#define CDISP
class CDispRGB :public CDisp{
public:
	void print_flash_local( const char * str , unsigned char page,unsigned int x, unsigned char y,char align = 0){
		::put_char('K',x, y,BLACK,WHITE);
	};
	void clear_ram(unsigned char data,unsigned char page){
		ILI9341_FillScreen(WHITE);
	}
};
extern CDispRGB display;

#endif
