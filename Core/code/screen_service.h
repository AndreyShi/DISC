/* void convert_channel_counter_to_text_string(unsigned char n_ch,unsigned char *txt_out);
* screen_service.h
*
* Created: 10.10.2018 16:18:00
* Author: Dmitry
*/


#ifndef __SCREEN_SERVICE_H__
#define __SCREEN_SERVICE_H__
#include "main.h"
#include "display.h"
#include "MenuLir.h"
#include "programm.h"


const char speed_error_str[] PROGMEM = {"Ошибка измерения скорости:       "};
									   

class screen_service
{

public:
enum SPEED{
	SIZE_FOR_SPEED=30,
	};

long channel_counter[N_AXIS];
long str_counter_before_out[N_STR];
bool does_it_live[N_AXIS];
bool measure_chan[N_AXIS];   // 1 - режим КМ или режим измерения расстояния от РМ до Концевика (для каналов)
bool measure_string[N_AXIS]; // 1 - режим КМ или режим измерения расстояния от РМ до Концевика (для строк)
long counter_measure;
long pulses_;
int counter_ref_;
bool encoder_fault[5],encoder_fault_[5];
bool error_overflow_type_long[5],error_overflow_type_long_[5];
bool error_overflow_no_empty_space[5],error_overflow_no_empty_space_[5];
int8_t interval_for_speed[N_AXIS];
unsigned char speed_error[N_AXIS];
void show_axis_symbol();	
void to_clear_place_for_virtual_buttons(unsigned char);
void update_icons_hand_mode();
void convert_channel_counter_to_text_string(unsigned char n_ch,long counter,unsigned char *txt_out);
void to_clear_string_connectors(unsigned char );
//22.10.2018
void to_display_mode_ref_submode_measure(bool start,bool to_see_measurement_result);
//29.10.2018
void to_display_symbol_string(unsigned char n_ch);
//30.10.2018
void to_display_mode_ref_submode_ref_ref(bool start,bool to_see_ref_ref_result);
//1.11.2018
void to_display_channel_counter(unsigned char string_number);
//05.11.2018
unsigned char show_big_digit(unsigned char n_string);
void show_big_digit_result();
bool you_can_see_big_digit(long data,unsigned char n_channel,unsigned char place);
//09.11.2018
unsigned char to_see_speed(unsigned char n_string);	
//19.11.2018
unsigned char put_speed_big_digit(unsigned char n_string);
//21.11.2018
void speed_error_handler(void);
unsigned char to_take_ch_number_simple(const CInit::AXIS_NAME symbol);
//22.11.2018
bool speed_error_handler_if_big_digit(unsigned char n_string);
bool speed_error_handler_if_big_digit_(unsigned char n_string);

void put_small_character_A_or_O_when_REF_NOT_FOUND(short x,short y,const unsigned char &ch);
//-----------------------------------------------------

unsigned char to_take_ch_number(unsigned int symbol){

	//to_copy_symbol_channel();

	if(symbol>CInit::AXIS_NAME::AXIS_D) 
		return N_AXIS;
	unsigned char n_ch;
	for (n_ch=0;n_ch<N_AXIS;n_ch++) {
		if(symbol==Lir.P2[n_ch])
			return n_ch;
	}
return N_AXIS;	
}


bool connectors_to_see(unsigned char *n_ch,unsigned char place){

	switch(place){
		case 0:place=5;break;
		case 7:place=12;break;
		case 14:place=19;break;
		case 21:place=26;break;
		default:return false;
		}

	//if(*n_ch<N_AXIS)
	//	display.print_flash_local(PSTR("Датчик:"),0,1,place*8);

    switch(*n_ch){
		case 0:
			display.print_flash_local(PSTR("P2.0"),0,0,place*8);
			return true;
	    case 1:
			display.print_flash_local(PSTR("P2.1"),0,0,place*8);
			return true;
	    case 2:
			display.print_flash_local(PSTR("P2.2"),0,0,place*8);
			return true;
	    case 3:
			display.print_flash_local(PSTR("P2.3"),0,0,place*8);
			return true;
	    default:
		return false;
		}
}
bool switches_to_see(unsigned char *n_ch,unsigned char place,bool zone){
	
	if(*n_ch>=N_AXIS)
		return false;

	switch(place){
		case 2:place=5;break;
		case 9:place=12;break;
		case 16:place=19;break;
		case 23:place=26;break;
		default:return false;
	    }
	if(!(flash_prg.axis_cfg[*n_ch].ref_pulse_zone & YESNO_P_bm))
		flash_prg.show_P(flash_prg.axis_cfg[*n_ch].ref_pulse_zone, IN_P, 18, place*8,0);
	return true;
}
void to_convert_long_to_string_for_speed(long *data_to_conv,unsigned char *txt_out,unsigned char n){
	//unsigned char tmp[17] = {0};										 
	int f;
	bool linear=false;
	switch(flash_prg.axis_cfg[n].format){
		  case CPrg::FORMAT::LINEAR_1:
		  case CPrg::FORMAT::LINEAR_2:
		  case CPrg::FORMAT::LINEAR_3:
		  f=(int)CPrg::FORMAT::LINEAR_1;
		  linear=true;
		  break;
		  case CPrg::FORMAT::LINEAR_4:
		  f=(int)CPrg::FORMAT::LINEAR_2;
		  linear=true;
		  break;
		  case CPrg::FORMAT::LINEAR_INCH_4:
		  coeff.div(255,data_to_conv,1,254,1);
		  f=(int)CPrg::FORMAT::LINEAR_2;
		  linear=true;
		  break;
		  case CPrg::FORMAT::CIRCLE_3:
		   f=(int)CPrg::FORMAT::LINEAR_3;
		   break;
		  case CPrg::FORMAT::CIRCLE_4:
		  f=(int)CPrg::FORMAT::LINEAR_4;
		  break;  
		  case CPrg::FORMAT::CIRCLE_2:
		   f=(int)CPrg::FORMAT::LINEAR_2;
		   break; 
		  case CPrg::FORMAT::CIRCLE_1:
		  f=(int)CPrg::FORMAT::LINEAR_1;
		  break; 
		  case CPrg::FORMAT::CIRCLE_SECUNDO:
		  f=(int)CPrg::FORMAT::LINEAR_3;
		  break;
		  case CPrg::FORMAT::CIRCLE_MINUTO:
		  f=(int)CPrg::FORMAT::LINEAR_2;
		  break;
		  
		  default: f=(int)CPrg::FORMAT::LINEAR_1; 	  
		  }
	if(linear)
		txt_out[0] = 'F';
	else
		txt_out[0] = 'S';
	txt_out[1] = '=';

	display.itoa_quick_lir(data_to_conv,txt_out + 2);
	display.formatting2((char *)txt_out + 2,f);								
}
void to_convert_interval_for_speed_measure_to_digit(void){
   for(unsigned char n=0;n<N_AXIS;n++){
		if(flash_prg.axis_cfg[n].interval_to_measure_speed == CPrg::RANGE::INTERVAL0_6)
			interval_for_speed[n] = 60;
		else if(flash_prg.axis_cfg[n].interval_to_measure_speed == CPrg::RANGE::INTERVAL0_3)
			interval_for_speed[n] = 30;
		else 
			interval_for_speed[n] = 10;
    }	
}
bool is_save_lives(void){
	for(unsigned char n=0;n < N_AXIS;n++){ 
		if(Lir.P2[n] <= CInit::AXIS_NAME::AXIS_D && flash_prg.axis_cfg[n].save == CPrg::SAVE::YES_SAVE) 
			return true;
	   }
	return false;	
}
protected:
private:
	

}; //screen_service
extern screen_service screen;
#endif //__SCREEN_SERVICE_H__
