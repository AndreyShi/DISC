
#ifndef _spi_h
#define _spi_h
#include "main.h"
class CSpiMaster
{

public:
//05.06.2018 
bool debug_flash;
char debug_23LCV1024;
long to_see_counter;
//long to_see_interval,to_see_set_point_plus,to_see_down,to_see_up;
long counter_debug;
bool sign;
//14.06.2018
long counter_correction_plus,counter_correction_mines;
//--------------
//18.01.2018 Spi units on bus	
//CS_MEM - PC4 AT45DB081B
// MOSI - PC5, MISO - PC6, SCK - PC7
// SPIC.CTRL 
// 1*******  double speed 
// 0*******  no double speed 
// *1******  ENABLE spi module
// *0******  DISABLE spi module
// **1***** lsb first
// **0***** msb first
// ***1**** master 
// ***0**** slave 
// ****00** 0 mode rising sample, falling setup
// ****01** 1 mode rising setup, falling sample
// ****10** 2 mode falling sample,rising setup
// ****11** 3 mode falling setup, rising sample
// ******00 clk/4   
// ******01 clk/16
// ******10 clk/64
// ******11 clk/128
// f main is 32 Mgh if DOUBLE_SPEED is used 
// clock for data's interchange is 16 Mgh (8)
// time interval for one bit is 62.5(125) ns for byte is 0.5(1) mks
// SPIC.INTCTRL - 0x00 disable interrupt
// SPIC.STATUS 
// 1******* if serial transfer is complete and one byte is completely 
// shifted in/out of the DATA register. Flag is cleared when STATUS is read 
// and DATA is read/written 
void init_spi_outer_flash(void){
//CS_23LCV1024 - ???? late....
PORTD.OUTSET =0x08;
PORTD.DIRSET =0x08;	
//CS_MEM - PC4 AT45DB081B
   PORTC.OUTSET =0x10;
   PORTC.DIRSET =0x10;
// MOSI - PC5, MISO - PC6, SCK - PC7
   PORTC.OUTCLR =0x20;
   PORTC.DIRSET =0x20;
   PORTC.OUTCLR =0x80;
   PORTC.DIRSET =0x80;
//--disable interrupt-
   SPIC.INTCTRL = 0x00;
//--SPIC.CTRL  11010000 double speed
   SPIC.CTRL = 0xd0;
//    SPIC.CTRL = 0xd3;
//--SPIC.CTRL  01010000  no double speed   
//   SPIC.CTRL = 0x50;
}
void cs_flash_low(void){
//CS_MEM - PC4 AT45DB081B
   PORTC.OUTCLR =0x10;
}
void cs_flash_high(void){
//CS_MEM - PC4 AT45DB041B
   PORTC.OUTSET =0x10;
}
bool common_hardware_spi(unsigned char spic_data,unsigned char *buf_adr){
unsigned char n;
//19.01.2018 timeout for 20 is about 4.5 mks
	       SPIC.DATA = spic_data;
           for(n=0;n<20;n++) {
// 1******* if serial transfer is complete  
            if (SPIC.STATUS & 0x80)break;
                             }
//------------address
           if (n>=20) return false; 
           SPIC.DATA=buf_adr[2];
           for(n=0;n<20;n++) {
// 1******* if serial transfer is complete  
            if (SPIC.STATUS & 0x80)break;
                             }
//------------address
           if (n>=20) return false; 
           SPIC.DATA = buf_adr[1];
           for(n=0;n<20;n++) {
// 1******* if serial transfer is complete  
            if (SPIC.STATUS & 0x80)break;
                             }
//------------address
           if (n>=20) return false; 
           SPIC.DATA = buf_adr[0];
           for(n=0;n<20;n++) {
// 1******* if serial transfer is complete 
            if (SPIC.STATUS & 0x80)break;
                             }
           if (n>=20) return false;
           return true; 
}
//16.05.2018 
void cs_23LCV1024_low(void){
	//CS_23LCV1024 - 
	PORTD.OUTCLR =0x08;
}
void cs_23LCV1024_high(void){
	//CS_23LCV1024 - 
	PORTD.OUTSET =0x08;
}
void init_spi_for_23LCV1024(void){
//CS_MEM - PC4 AT45DB081B
PORTC.OUTSET =0x10;
PORTC.DIRSET =0x10;	
//CS_23LCV1024 - ???? late....
PORTD.OUTSET =0x08;
PORTD.DIRSET =0x08;
// MOSI - PC5, MISO - PC6, SCK - PC7
PORTC.OUTCLR =0x20;
PORTC.DIRSET =0x20;
PORTC.OUTCLR =0x80;
PORTC.DIRSET =0x80;	
//--disable interrupt-
SPIC.INTCTRL = 0x00;
//--SPIC.CTRL  11010000 double speed
SPIC.CTRL = 0xd0;
}
enum RAM23LCV1024_OPCODE{
	READ=0x03,
    WRITE=0x02,
	EDIO=0x3B,
	RSTDIO=0xFF,
	RDMR=0x05,
	WRMR=0x01,
};
#pragma GCC push_options
#pragma GCC optimize ("O0")
#pragma GCC pop_options
char write_start_sequence_for_23LCV1024(void){
 char n,res;	
//Read data from memory array beginning at selected address	
//READ              0000 0011      0x03
//Write data to memory array beginning at selected address
//WRITE             0000 0010      0x02
//Enter Dual I/O access
//EDIO              0011 1011      0x3B
//Reset Dual I/O access
//RSTDIO             1111 1111      0xFF
//Read Mode Register
//RDMR              0000 0101      0x05
//Write Mode Register	
//WRMR              0000 0001      0x01	
//17.05.2018 in first Mode register must be read
//if 00xxxxxx byte mode
//if 01xxxxxx sequential mode
//if 10xxxxxx page mode 
//4096 pages of 32 bytes
init_spi_for_23LCV1024(); 
cs_23LCV1024_low();
//17.05.2018 timeout for 20 is about 15 mks
SPIC.DATA =RAM23LCV1024_OPCODE::RDMR;
for(n=0;n<20;n++) {
	// 1******* if serial transfer is complete
	if (SPIC.STATUS & 0x80)break;
}
if (n>=20){cs_23LCV1024_high(); return 1;}
         SPIC.DATA  = 0xff;
         for ( n=0;n<20;n++){
	         // 1******* if serial transfer is complete
	         if (SPIC.STATUS & 0x80){res= SPIC.DATA ;break; }
         }
cs_23LCV1024_high();		 
if (n>=20) {return 2;}
  //res=res&0xc0;
  if(res==0x40){ return 3;}
//-------------------------------	  
cs_23LCV1024_low();	  
  SPIC.DATA =RAM23LCV1024_OPCODE::WRMR;
  for(n=0;n<20;n++) {
	  // 1******* if serial transfer is complete
	  if (SPIC.STATUS & 0x80)break;
  }
  if (n>=20)  {cs_23LCV1024_high(); return 4;}
  SPIC.DATA =0x40;
  for(n=0;n<20;n++) {
	  // 1******* if serial transfer is complete
	  if (SPIC.STATUS & 0x80)break;
  }
  cs_23LCV1024_high();
  if (n>=20) { return 5;}  
//----------------------------------	  
cs_23LCV1024_low();
//17.05.2018 timeout for 20 is about 15 mks
SPIC.DATA =RAM23LCV1024_OPCODE::RDMR;
for(n=0;n<20;n++) {
	// 1******* if serial transfer is complete
	if (SPIC.STATUS & 0x80)break;
}
if (n>=20){cs_23LCV1024_high(); return 6;}
SPIC.DATA  = 0xff;
for ( n=0;n<20;n++){
	// 1******* if serial transfer is complete
	if (SPIC.STATUS & 0x80){res= SPIC.DATA ;break; }
}
cs_23LCV1024_high();
if (n>=20) { return 7;}
//res=res&0xc0;
if(res==0x40){ return 3;}

	return 8;//нет связи по SPI
}

bool save_table_to_23LCV1024(long address_counter,long data,bool plus){
//06.06.2018 it will take about 40 mks for all	
unsigned char *byte_pointer;
unsigned char n,addr[4],table[4];
int m;	
if(plus==true)
{address_counter=address_counter*4;}
else
{address_counter=(address_counter*4)+60000;}		
	byte_pointer=(unsigned char *)&address_counter;
	for(n=0;n<4;n++)addr[n]=byte_pointer[n];
	byte_pointer=(unsigned char *)&data;
	for(n=0;n<4;n++)table[n]=byte_pointer[n];
//time to send table to the 23LCV1024
cs_23LCV1024_low();
    SPIC.DATA =RAM23LCV1024_OPCODE::WRITE;
    for(n=0;n<20;n++) {
	    // 1******* if serial transfer is complete
	    if (SPIC.STATUS & 0x80)break;
    }
    if (n>=20){cs_23LCV1024_high(); return false;}	
//address		
		for(m=2;m>=0;m--){
	   SPIC.DATA =addr[m];
	   for(n=0;n<20;n++) {
		   // 1******* if serial transfer is complete
		   if (SPIC.STATUS & 0x80)break;
	   }
	   if (n>=20){cs_23LCV1024_high(); return false;}	
	   }
//dates	   
	  	for(m=0;m<4;m++){
		  	SPIC.DATA =table[m];
		  	for(n=0;n<20;n++) {
			  	// 1******* if serial transfer is complete
			  	if (SPIC.STATUS & 0x80)break;
		  	}
		  	if (n>=20){cs_23LCV1024_high(); return false;}
	  	} 
	cs_23LCV1024_high(); return true;   	
}
bool read_table_from_23LCV1024(long address_counter,long *data,bool plus){
//06.06.2018 it will take about 40 mks for all		
unsigned char *byte_pointer;
unsigned char n,addr[4],table[4];
int m;
if(plus==true)
{address_counter=address_counter*4;}
else
{address_counter=(address_counter*4)+60000;}
byte_pointer=(unsigned char *)&address_counter;
for(n=0;n<4;n++)addr[n]=byte_pointer[n];
//time to send table to the 23LCV1024
cs_23LCV1024_low();
SPIC.DATA =RAM23LCV1024_OPCODE::READ;
for(n=0;n<20;n++) {
	// 1******* if serial transfer is complete
	if (SPIC.STATUS & 0x80)break;
}
if (n>=20){cs_23LCV1024_high(); return false;}
//address
for(m=2;m>=0;m--){
	SPIC.DATA =addr[m];
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	if (n>=20){cs_23LCV1024_high(); return false;}
}
for(m = 0; m < 4; m++){
	SPIC.DATA = 0xff;
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	table[m]= SPIC.DATA;
	if (n>=20) {cs_23LCV1024_high(); return false;}
}
cs_23LCV1024_high();
//17.05.2017 it is only for debug
//table[0]=1;table[1]=0;table[2]=0;table[3]=0;
byte_pointer=(unsigned char *)data;
for(n=0;n<4;n++)byte_pointer[n]=table[n];
return true;
}
//#pragma GCC pop_options
bool read_bytes_from_23LCV1024(long address_counter,unsigned char *data,bool plus){
//26.12.2018 it will take about 280 mks from call to return for 264 bytes
	unsigned char *byte_pointer;
	unsigned char n,addr[4];
	int m;
	if(plus==false)
	{address_counter=address_counter+60000;}
	byte_pointer=(unsigned char *)&address_counter;
	for(n=0;n<4;n++)addr[n]=byte_pointer[n];
	//time to send table to the 23LCV1024
	cs_23LCV1024_low();
	// init.clr_B6();
	SPIC.DATA =RAM23LCV1024_OPCODE::READ;
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	if (n>=20){cs_23LCV1024_high(); return false;}
	//init.set_B6();	
	//address
	for(m=2;m>=0;m--){
		SPIC.DATA =addr[m];
		for(n=0;n<20;n++) {
			// 1******* if serial transfer is complete
			if (SPIC.STATUS & 0x80)break;
		}
		if (n>=20){cs_23LCV1024_high(); return false;}
	}
	//init.clr_B6();
	for(m = 0; m < 264; m++){
		SPIC.DATA = 0xff;
		for(n=0;n<20;n++) {
			// 1******* if serial transfer is complete
			if (SPIC.STATUS & 0x80)break;
		}
		data[m]= SPIC.DATA;
		if (n>=20) {cs_23LCV1024_high(); return false;}
	}
	cs_23LCV1024_high();
	//init.set_B6();
	return true;
}
bool write_bytes_to_23LCV1024(long address_counter,unsigned char *data,bool plus,int count = 264){
//02.01.2019 it will take about 304 mks for 264 bytes from call to return
	unsigned char *byte_pointer;
	unsigned char n,addr[4];
	int m;
if(plus==false)
{address_counter=address_counter+60000;}
byte_pointer=(unsigned char *)&address_counter;
for(n=0;n<4;n++)addr[n]=byte_pointer[n];	

	cs_23LCV1024_low();
	SPIC.DATA =RAM23LCV1024_OPCODE::WRITE;
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	if (n>=20){cs_23LCV1024_high(); return false;}
	//address
	for(m=2;m>=0;m--){
		SPIC.DATA =addr[m];
		for(n=0;n<20;n++) {
			// 1******* if serial transfer is complete
			if (SPIC.STATUS & 0x80)break;
		}
		if (n>=20){cs_23LCV1024_high(); return false;}
	}
	//dates
	for(m=0;m<count;m++){
		SPIC.DATA =data[m];
		for(n=0;n<20;n++) {
			// 1******* if serial transfer is complete
			if (SPIC.STATUS & 0x80)break;
		}
		if (n>=20){cs_23LCV1024_high(); return false;}
	}
	cs_23LCV1024_high(); return true;
}

//#pragma GCC pop_options
//--------------------
};
//------------------------------------------------------------------------------
extern CSpiMaster spi;

#endif
