/*
 * twitest.h
 *
 * Created: 30.10.2017 11:18:30
 *  Author: Andrew
 */ 


#ifndef TWITEST_H_
#define TWITEST_H_
#include "led_key_sound.h"
#include <avr/pgmspace.h>

#define LO(w)           ((uint8_t)(((uint32_t)(w)) & 0x0f))
#define HI(w)           ((uint8_t)((((uint32_t)(w)) >> 4) & 0xff))
#define LOBYTE(w)       ((uint8_t)(((uint32_t)(w)) & 0xff))
#define HIBYTE(w)       ((uint8_t)((((uint32_t)(w)) >> 8) & 0xff))

#define SendAddresMsek  0
#define ReadMsek        1
#define SendAddresMin   2
#define ReadMin         3
#define SendAddresHour  4
#define ReadHour        5
#define SendAddresDay   6
#define ReadDay         7
#define SendAddresMonth 8
#define ReadMonth       9
#define max7349         0x38 
#define max7370         0x38 
#define PCF8583         0x50


#define Writemode       0
#define Readmode        1
#define NoMasterAckStop 0
#define YesMasterAck    1


void TWI_sendaddres(uint8_t addr, int rw);
void TWI_senddata(uint8_t data);
uint8_t TWI_readdata(int MasterAck);
void TWI_stoplink();
int TWI_check(uint8_t addr);
uint8_t TWI_readdatafirst();
void settime(unsigned char *timefromPC);


void init_rtcclock(void);
void readdatafromrtcclock(void);
void convertBCDtoASCII(char bcd, unsigned char *ascii);
void BCDtoTimeandDate(char day,char mouth,char sec, char min, char hour, unsigned char*time);

void convertInttoASCII(int number, unsigned char *ascii);
void convertHextoASCII(unsigned char symbol, unsigned char *ascii,int &);
void ReadTwiPCF8583(volatile int &);
uint8_t readkeypress(void);
uint8_t readkeypress_max7370_filter(unsigned char *key);
uint8_t readkeypress_max7370(unsigned char *key);
uint8_t readkeypress_max7349(unsigned char *key);
void set_auto_repeat_max7349(const uint8_t &param);
void set_sound_max7370();
void on_sound_max7370();
void off_sound_max7370();
void peak_sound_max7370(unsigned char ms);
uint8_t setup_keyboard_max7370(void);
uint8_t setup_keyboard_max7370_filter(void);


#endif /* TWITEST_H_ */