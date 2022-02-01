#include "../../Drivers/Display/ili9341.h"
#include "../../Core/code/display.h"

#ifndef CDISP
#define CDISP
class CDispRGB :public CDisp{
public:
	void print_flash_local( const char * str , unsigned char page,unsigned int x, unsigned char y,char align = 0){
		::put_char('K',x, y,BLACK,WHITE);
	};
};
extern CDispRGB display;

#endif
