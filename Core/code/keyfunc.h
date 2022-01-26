/*
 * keyfunc.h
 *
 * Created: 03.10.2018 11:26:02
 *  Author: Andrew
 */ 


#ifndef KEYFUNC_H_
#define KEYFUNC_H_

 #include "led_key_sound.h"
 #include "io_altera.h"
 #include "display.h"
 #include "main.h"
 #include <avr/pgmspace.h>
 #include "MenuLir.h"
 #include "counter.h"
 #include "screen_service.h"
 #include "programm.h"
 extern void to_copy_parameters_from_eeprom_to_ram(void);
extern "C"{
int to_check_SP_memory(void);	
}
class TMsgKey
{
public:
	TMsgKey(){};
	~TMsgKey(){};

	char key;
	char er; // альтера ошибки датчика
	void key_handling();
	//October 2018
	bool is_it_switch_for_ref[N_AXIS];//,is_it_switch_for_ref_[N_AXIS];
	bool is_it_switch_or_no_for_ref(unsigned char *ch);
	void to_look_for_submode_in_ref_mode(void);

//21.11.2018
   	void to_look_for_submode_in_begin_mode(void);
	bool to_put_icons_in_begin_mode(void);
//----------------------------------------------	   
	bool to_put_icons_in_ref_mode(void);
	void to_measure_distance_from_ref_to_switch(unsigned char);
	void to_count_pulses_from_ref_to_ref(unsigned char);
	void to_stop_and_to_see_measurement_result(void);
//31.10.2018
    void all_ref_interrupts_will_be_disabled_and_var_set_to_starting_state(void);
	void starting_line_for_PB5_int_0(void);
	void submodes_ref_mode_are_executed(void);
	void to_take_datas_from_channel_0(void);
    void make_speed(void);
   unsigned char interval_for_speed[N_AXIS];
//------------------------------------------	
	void to_take_datas_from_channel_1(void);
	void starting_line_for_PA0_int_1(void);
	void to_take_datas_from_channel_2(void);
	void starting_line_for_PA1_int_2(void);	
	void to_take_datas_from_channel_3(void);
	void starting_line_for_PB3_int_3(void);
	void correction_init();
//---------------------------------------------	
	unsigned int switch_number_for_ref[N_AXIS];//, switch_number_for_ref_[N_AXIS];
	bool enable_int[N_AXIS]; // активация портов для режима поиска реф метки
	bool all_ref_interrupts_must_be_disabled;
//23.10.2018
    bool enable_measure[N_AXIS];	
	bool mode_ref_submode_measure;
	//bool measurement_is_ended;
//29.10.2018
    bool submode_mode_REF = 1;
	bool mode_ref_submode_ref_ref;// 1 - режим поиска КМ ,0 - нет
	bool enable_ref_ref[N_AXIS]; // включение режима измерение от метки до метки
//1.11.2018
    bool does_string_has_button[N_STR];	
//06.11.2018
    int offset_number;	// 0 - G53, 1 - G54, 2 - G55, 3 - G56, 4 - G57, 5 - G58
//13.11.2018
    unsigned char fs;
//21.11.2018
    CInit::AXIS_NAME string[N_AXIS];
	bool refresh_icons_in_begin_mode;	
	unsigned int no_start_press_;
	bool enable_to_invert;
//23.11.2018
    unsigned char what_channel_is_chosen_for_input;		
//29.11.2018
    bool divide_by_2[N_AXIS]; //1 - данные канала разделились на два  ,0 - нет
	unsigned char key_press_for_A_O,key_press_for_A_O1;
//---------------------------------------------	
	enum MODE
	{
		BEGIN       = 0,
		FIND_REF    = 1,
		HAND        = 2,
		INPUT_PARAM = 3,
	}mode,old_mode;
	void SET_M(MODE mode_);
	enum SUBMODE_REF{
		PURE_REF=0,
		}follower_ref;
    enum SUBMODE_BEGIN{
		SET_TO_ZERO=0,
		A_O=1,
    }follower_begin;		
	enum SUBMODE_HAND{
	     OFFSET=0,
	     TOOL=1,
	     ABS_OR_REL=2,
	     TO_ZERO=3,
	     TO_SEE_HALF_OR_FULL=4,
	     TO_SEE_DIFFERENT_READOUT=5,
		 TO_DIVIDE_CIRCUIT=6,
		 DRIVE_CONTROL = 7,//режим ручного управления
		 CMD_CNTRL = 9,    //режим преднабора
		 FUNC = 10,        //режимы A/0 ,->0, G53, /2 
	     }tracer_real, //текущий режим
		 tracer_next;  //следующий режим
//------------------------------------		 
	void apply();
	void apply_(void);
	void begin_(void);
	void input_param(void);
	void find_ref(void);
	void hand(void);
	void to_see_indikator_channel_P2_1(void);
	void to_see_indikator_channel_P2_0(void);
	void to_see_indikator_dif_P2_0_P2_1(void);
	void to_see_MenuEncoders(void);
	void to_see_mInputs(void);
	void to_see_mSupply(void);
	void to_see_mTime(void);	
	void see_data_str(const uint8_t& str);
	void to_see_graph(void);
	void to_see_stack_and_go_next(void);
	void _key_MODE_HAND_f();
//08.10.2018
    void to_check_CR2032(void);
//23.11.2018
    bool go_to_input_string_mode(unsigned char n_string);
	void input_string_inside_begin_mode(void);
//26.11.2018
    void to_look_for_submode_of_operation_inside_HAND(void);
	bool to_put_icons_in_mode_HAND(SUBMODE_HAND submode);
	void to_do_inside_submode_for_mode_HAND(unsigned char n_string);
	void submode_change_offset_inside_HAND(void);
	void clear_G5X(void);
	void submode_change_a_or_r_inside_HAND(unsigned char n_string);	
	void submode_set_to_zero_inside_HAND(unsigned char n_string);
	void to_zero_common_inside_HAND(unsigned char n_string);
	void submode_to_see_half_or_full_inside_HAND(unsigned char n_string);
	void to_make_half_common_inside_HAND(unsigned char n_string);
	void go_out_from_ref_mode_in_begin_mode(void);
	void read_counters_altera();
	void read_counters_nova();
	void read_counters_mcx514_logic();
	void init_mx514(void);
	void start_find_ref();
	void stop_find_ref();
	void key_BLANK_ref_mode(const uint8_t & str);
	void stop_REF(const uint8_t & str);
//--------------------------------------	
#pragma GCC push_options
#pragma GCC optimize ("O0")	
	void to_check_stack(void){
		int tmp;
		tmp=to_check_SP_memory();
	display.print_debug_one_local(tmp,0,10,208);		
	}
#pragma GCC pop_options	
/*
in unsigned int symbol - символ канала 
out unsigned char & ch -  номер канала которому пренадлежит символ
return bool - возвращает либо 0 либо 1 , 0 - канал найден , 1 - канал не найден
внимание! функция может вернуть только значение  0 или 1 
*/
bool to_take_channel_number(unsigned int symbol,unsigned char & ch)
{	
	if(symbol>CInit::AXIS_NAME::AXIS_D) {
		ch = CInit::AXIS_NAME::NO_AXIS;
		return 1;
		}
	
	for (unsigned char n = 0; n<N_AXIS; n++) {
		if(symbol==Lir.P2[n]){
			ch = n;
			return 0;
		}			
     }
	ch = CInit::AXIS_NAME::NO_AXIS;
	return 1;
}
/*
приостановка режима 'ZM' или 'PM' без выхода из режима 'выхода в реф'
*/
bool pause_ZP_PM(void){
   if(follower_ref == TMsgKey::PURE_REF){
	for(unsigned char n=0;n<N_AXIS;n++){
		if(stop_PM(n))
			return 0;
		}
	}
	return 1;
}

/*
функция для выхода из реф режима ,по нажатию кнопки <СТОП> обрабатывается возвращаемое значение
так же здесь выходим ,если зашли в режим поиска метки из режима hand или begin
*/
bool set_out_ref_mode;
char go_out_from_ref_mode(void){

	char exit_ref = 1;

	for(int i = 0;i<N_AXIS;i++){
		if(display.ifSearchRef(i)){
			if(display.ref_change_mode[i] == CDisp::SEARCH_REF_hand){
				exit_ref = 0;
				screen.to_clear_string_connectors(4);
				SET_M(MODE::HAND);
				if(tracer_real == TMsgKey::FUNC){
					to_put_icons_in_mode_HAND(tracer_real);//обновить иконки режима
				}else{
					for(int str = 0 ; str< N_STR;str++){
						if(Lir.P2[i] == Lir.Str[str]){
							common_for_to_put_buttons(str,_pic_ABS_REL);
						}
					}
				}
			}
			else if(display.ref_change_mode[i] == CDisp::SEARCH_REF_begin){
				screen.to_clear_string_connectors(4);
				follower_begin = SUBMODE_BEGIN::A_O;
				common_go_to_mode_begin();
					Lir.SET_C(idBegin);
					exit_ref = 0;
			}				
			else if(display.ref_change_mode[i] == CDisp::SEARCH_REF_findref){	
					screen.to_clear_string_connectors(4);
					for(int str = 0 ; str< N_STR;str++){
						if(Lir.P2[i] == Lir.Str[str]){						
							common_for_to_put_buttons(str,_pic_REF);
						}
					}
					exit_ref = 0;
			}

			display.ref_change_mode[i] = CDisp::NOTHING;
			ref_.ref_change_mode[i] = 0;
			if(ref_.pRef[i] == 0)
				init.ref_mode[i] = 0;
			#if (configUSE_NCshell == 1)
			if(Lir.DIR_FIND_REF){
				l_k_s.OFF_LED(_led_RIGHT);
				l_k_s.OFF_LED(_led_LEFT);
				Lir.DIR_FIND_REF = 0;
			}	
			#endif		
		}
	}

	for(int c = 0 ; c <N_AXIS;c++){
		if(ref_.pRef[c] == 0)
			init.ref_mode[c] = false;
		}

	for(unsigned char n=0;n<N_AXIS;n++){
		display.go_to_blink_string[n]=false;
		if(ref_.pRef[n] == 0)
			Lir.ref_mode[n] = 0;
		if(screen.measure_string[n]){
			screen.measure_string[n]=false;
			exit_ref = 0;
			if (Lir.curItem->id != idMenuEncoders){
				if(mode == MODE::HAND){
					if(tracer_real != TMsgKey::FUNC)
						common_for_to_put_buttons(n,_pic_REF_REF);
				}
				else
					common_for_to_put_buttons(n,_pic_REF_REF);
			}
		}
	}
	all_ref_interrupts_must_be_disabled=true;
	//follower_real=follower_next=SUBMODE_REF::PURE_REF;

	//to_stop_cycle_measurement();
	to_stop_cycle_ref_ref();
	if(exit_ref){
		screen.to_clear_place_for_virtual_buttons(4);
		screen.to_clear_string_connectors(4);
	}
	
	return exit_ref;	
}
void to_stop_cycle_measurement(void){

	all_ref_interrupts_must_be_disabled=true;
	for(unsigned char n=0;n<N_AXIS;n++){
		screen.measure_chan[n]=false;
		}
	mode_ref_submode_measure=false;		
	
}
void to_stop_cycle_ref_ref(void){
	all_ref_interrupts_must_be_disabled=true;
	for(unsigned char n=0;n<N_AXIS;n++){
		screen.measure_chan[n]=false;
		}
	mode_ref_submode_ref_ref=false;
}
/*
включение режима поиска реф метки
*/
void to_go_to_REF_from_key_press(CInit::AXIS_NAME symbol,unsigned char n_string){
	if(symbol<=CInit::AXIS_NAME::AXIS_D){
		unsigned char n_ch;
		//unsigned char coord_y = 2;
		if(to_take_channel_number(symbol,n_ch))
			return;

		for(int i = 0,j = 2 ; i < N_STR; i++,j+=7){
			if(Lir.Str[n_string] == Lir.Str[i] && 
			display.position_to_see[i] != CDisp::SPEED &&
			display.position_to_see[i] != CDisp::RESULT)
				screen.switches_to_see(&n_ch,j,true);
		}

		if(n_ch<N_AXIS){
			//display.go_to_blink_string[n_string]=true;
			is_it_switch_for_ref[n_ch] = is_it_switch_or_no_for_ref(&n_ch);
			#if (configUSE_NCshell == 0)
			enable_int[n_ch] = true;
			#endif
			#if (configUSE_NCshell == 1)
			ref_.pRef[n_ch] = false;
			#endif
		}
		if(display.ref_change_mode[n_ch] == CDisp::NOTHING || display.ref_change_mode[n_ch] == CDisp::REF_NOT_FOUND)
			display.ref_change_mode[n_ch] = CDisp::SEARCH_REF_findref; // установка режима поиска
		else{
			stop_REF(n_string);
			display.ref_change_mode[n_ch] = CDisp::NOTHING;
			ref_.ref_change_mode[n_ch] = 0;
			init.ref_mode[n_ch] = 0;
			ref_.pRef[n_ch] = 0;
			Lir.ref_mode[n_ch] = 0;
			}
	}	
}
void  common_for_to_put_buttons(unsigned char n, unsigned char npic){

	if(n==0){
		display.put_button_(X_BUTTON_PLACE,Y_ONE,npic);
		does_string_has_button[0] = true;
		}
	else if(n==1){
		display.put_button_(X_BUTTON_PLACE,Y_TWO,npic);
		does_string_has_button[1] = true;
		}
	else if(n==2){
		display.put_button_(X_BUTTON_PLACE,Y_THREE,npic);
		does_string_has_button[2] = true;
		}
	else if(n==3){
		display.put_button_(X_BUTTON_PLACE,Y_FOUR,npic);
		does_string_has_button[3] = true;
		}		
}
void common_to_put_icons_information(void){          
	string[0] = (CInit::AXIS_NAME)Lir.Str[0];
	string[1] = (CInit::AXIS_NAME)Lir.Str[1];
	string[2] = (CInit::AXIS_NAME)Lir.Str[2];
	string[3] = (CInit::AXIS_NAME)Lir.Str[3];
	for(unsigned char n=0;n < N_AXIS;n++){
		if(Lir.P2[n] <= CInit::AXIS_NAME::AXIS_D)
			screen.does_it_live[n] = true;
		else
			screen.does_it_live[n] = false;
	}
}
/*
функция останавливает режим 'ЗМ'
*/
char stop_ZM(unsigned char n_string){

	int n;
	for(n=0;n<N_AXIS;n++){
		if(screen.measure_string[n])
			break;
	}
	if(n < N_AXIS){
		if(n_string==n){
			screen.measure_string[n]=false;
			if (Lir.curItem->id != idViewInputs)
				common_for_to_put_buttons(n,_pic_ZONE_REF);
			to_stop_cycle_measurement();
			return 1;
		}
	}
	return 0;
}
/*
функция останавливает режим 'КМ' расстояние от метки до метки
unsigned char n - номер строки
*/
char stop_PM(unsigned char n){		
	if(screen.measure_string[n]){
		screen.measure_string[n]=false;
		if (Lir.curItem->id != idMenuEncoders)
			common_for_to_put_buttons(n,_pic_REF_REF);
		to_stop_cycle_ref_ref();
		return 1;
	}			
	return 0;
}
/*
функция останавливает режим 'PМ'
*/
char stop_PM_ZM(unsigned char n_string){
	
	for(int n=0;n<N_AXIS;n++){
		if(screen.measure_string[n]){
			if(n_string==n){
				screen.measure_string[n]=false;
				if(follower_ref == TMsgKey::PURE_REF){
					common_for_to_put_buttons(n,_pic_REF_REF);
					to_stop_cycle_ref_ref();				
				}
				return 1;
			}
		}			
	}		
	return 0;
}


void common_go_to_mode_begin(void){
	SET_M(TMsgKey::MODE::BEGIN);
	l_k_s.OFF_LED(_led_MODE_HAND);
	l_k_s.OFF_LED(_led_MODE_DIAGNOSTICS);
	l_k_s.OFF_LED(_led_REF);
	refresh_icons_in_begin_mode=true;	
	Lir.Update_error = false;
}
/*
функция обнуляет относительный счетчик если выбрана относительная координата (0 - относительная  1 - абсолютная ,в переменной  init.ref_mode)
in  unsigned char n_string - номер строки
out return value bool - true обнуление произошло тк init.ref_mode == 0(относ.) / false обнуление не произошло тк init.ref_mode == 1(абс.)
*/
bool go_to_zero_common(unsigned char s){	
	unsigned char c;	
		if(to_take_channel_number(Lir.Str[s],c))
			return false; 
		if(init.ref_mode[c] == 0){		//выбрана относительная				
			if(Lir.is_corr(c) || Lir.is_rel_coded_ref(c)){
				cli();
				long cnt = screen.channel_counter[c];
				sei();
				cnt = Lir.conv_c_i(cnt,c,0);
				Lir.R_off[c] += cnt;
				//flash_prg.write_eeprom(1921,sizeof(Lir.R_r),&Lir.R_r);
				cli();
				l_k_s.write_to_realtime((unsigned char *)&Lir.R_off[c],80 + (c * sizeof(long)),sizeof(long));//80 == 0x50	
				sei();
			}
			else{
				cli();
				mCounter[c].rel = 0;
				sei();
				#if (configUSE_mcx514 == 1)
					cli();
					mCounter[c].ref_reset();
					SetRp(Lir.plb2(c),0);
					SetLp(Lir.plb2(c),0);
					sei();
				#endif
			}
			return true;
		}
	return false;
}

/*
функция вызываемая по нажатию кнопки (режим выхода в реф или переключение на абсолютную/относительную координату)
in unsigned char n_string - номер строки
*/ 
void go_to_abs_rel_common(unsigned char s){
	unsigned char c;
	if(to_take_channel_number(Lir.Str[s],c))
		return;
	if(display.ref_change_mode[c] == CDisp::REF_NOT_FOUND)
	{
		display.ref_change_mode[c] = CDisp::NOTHING;
		if(!init.ref_mode[c])
			return;
	}
	if(init.ref_mode[c] == false)
	{
		init.ref_mode[c] = true;
		ref_.ref_change_mode[c] = true;
		return;
	}
	if(init.ref_mode[c] == true)
	{
		init.ref_mode[c] = false;
		ref_.ref_change_mode[c] = false;
		return;
	}
}
void to_define_is_it_A_or_O(void){

	for(int c = 0 ; c < N_AXIS; c++){
		if(ref_.ref_change_mode[c] == true && ref_.pRef[c] == false){
			display.ref_change_mode[c] = CDisp::REF_NOT_FOUND;
			ref_.ref_change_mode[c] = false;
			init.ref_mode[c] = false;
		}
		init.mRef_ext[c] = ref_.pRef[c] & init.ref_mode[c];
	}
}
void divide_or_mult(unsigned char n_ch,unsigned char place){ 

	if (divide_by_2[n_ch] == false) {
		display.put_button_(X_BUTTON_PLACE,place,_pic_HALF);
		flash_prg.divide_by_2_counter_rel(n_ch);
		divide_by_2[n_ch] = true;
		}
		else{
		display.put_button_pic_(X_BUTTON_PLACE,place,_pic_FULL);
		flash_prg.mult_by_2_counter_rel(n_ch);
		divide_by_2[n_ch] = false;
		}
 }

void if_it_was_submode_half_inside_HAND(void){
	for(unsigned char n=0;n<N_AXIS;n++){
		if(divide_by_2[n] == true){
			flash_prg.mult_by_2_counter_rel(n);
			divide_by_2[n] = false; 	
		}	
	} 		
}
};

extern TMsgKey MsgKey;

#endif /* KEYFUNC_H_ */