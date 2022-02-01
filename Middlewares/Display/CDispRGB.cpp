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

};
void CDispRGB::print_symbols_local_RA(char symbol,int number,unsigned char page,unsigned int x_, unsigned char y_){

};
void CDispRGB::print_ram_local( char * str, unsigned char page,unsigned int x, unsigned char y){

};
void CDispRGB::print_ram_local_RA( char * str, unsigned char page,unsigned int x, unsigned char y){

};
void CDispRGB::clear_ram(unsigned char data,unsigned char page){
	ILI9341_FillScreen(WHITE);
};
