

#include "screen_service.h"
#include "keyfunc.h"
#include "config.h"
/*
функция показывает на экране символы осей и надписи под осями
*/
void screen_service::show_axis_symbol(){
//new edition fop gnu is started 11.10.2018
//if 4 axis and one inverse it takes 1080 mks
//bool ref=false;
unsigned char v;  //v номер канала
CInit::AXIS_NAME symbol = (CInit::AXIS_NAME)no_axis;
unsigned char y = 0;
unsigned char y_d = 0;
unsigned char y_k = 0;
unsigned char y_shot = 0;


for(int s = 0 ; s < N_STR; s++){ //s номер строки

	if(s == 0){
		symbol=(CInit::AXIS_NAME)Lir.Str[0];
		y_shot = 0 ;
		y   = 25;
		y_d = 0;
		y_k = 41;
		}
	if(s == 1){
		symbol=(CInit::AXIS_NAME)Lir.Str[1];
		y_shot = 7 ;
		y   = 80;
		y_d = 56;
		y_k = 96;
		}
	if(s == 2){
		symbol=(CInit::AXIS_NAME)Lir.Str[2];
		y_shot = 14 ;
		y   = 142;
		y_d = 118;
		y_k = 158;
		}
	if(s == 3){
		symbol=(CInit::AXIS_NAME)Lir.Str[3];
		y_shot = 21 ;
		y   = 192;
		y_d = 168;
		y_k = 208;		
		}
	#if (configUSE_NCshell == 1)
		if(Lir.block_str[s])
		continue;
	#endif


	if( symbol<=CInit::AXIS_NAME::AXIS_D){				
		if(display.position_to_see[s]==CDisp::SPEED){
			if(Lir.curItem->id != idHandMove && Lir.curItem->id != idExecmd && Lir.entry_hand != Menu::WORKING_EXE)
				display.put_axis_shot(0,y_shot,(unsigned char)symbol);

			if(symbol<CInit::AXIS_NAME::AXIS_A)
				display.put_char_local_independent('F',0,4,y);
			else 
				display.put_char_local_independent('S',0,4,y);
		}
		else {
			for (v = 0;v < N_AXIS;v++) {
				if(symbol == Lir.P2[v]) // получаем номер канал
					break;
				}
		if (v < N_AXIS) {
#if (configUSE_mcx514 == 1)
		if(flash_prg.axis_cfg_nc[v].datchik != CPrg::DATCHIK::NO_DAT){			
			long tmp = 0;

			if(flash_prg.axis_cfg_nc[v].datchik == CPrg::DATCHIK::HANDWEEL){
				tmp = Lir.mul_MAXO(v,4);	
				}
			else if(flash_prg.axis_cfg_nc[v].datchik == CPrg::DATCHIK::ENCODER){
				tmp = altera.counter[v];
				coeff.mult(v,&tmp);
				long tmp_LP = Lir.mul_LP(v,Lir.LP_axis[v]);
				tmp = tmp_LP - tmp;
				}

			Lir.print_long_format(tmp,Lir.get_format_from_ch(v),0,31,y_k,RIGHT_ALIGN,0,11);
			if(flash_prg.axis_cfg_nc[v].datchik == CPrg::DATCHIK::ENCODER)
				display.print_flash_local(PSTR("д"),0,33,y_k);
			if(flash_prg.axis_cfg_nc[v].datchik == CPrg::DATCHIK::HANDWEEL)
				display.print_flash_local(PSTR("м"),0,33,y_k); 
		}
#endif
#if (configUSE_debug_ABS_G53 == 1)
#if (configUSE_Linear_holes == 1)
			//===for_debug			
			if((Lir.curItem->id == idInpointLinearPUSK && (symbol == Lir.primary_axis_lh || symbol == Lir.secondary_axis_lh))  || 
			   (Lir.curItem->id == idInpointRadiusPUSK && (symbol == Lir.primary_axis_ch || symbol == Lir.secondary_axis_ch))){
				if(ref_.pRef[v]){
					//if(flash_prg.axis_cfg[v].direction == CPrg::DIRECTION::PLUS)
						Lir.print_long_format(mCounter[v].abs + display.offset_g53[v],Lir.get_format_from_ch(v),0,32,y_k,RIGHT_ALIGN,0,11);
					//if(flash_prg.axis_cfg[v].direction == CPrg::DIRECTION::MINES)
					//	Lir.print_long_format(-mCounter[v].abs + display.offset_g53[v],Lir.get_format_from_ch(v),0,32,y_k,RIGHT_ALIGN,0,11);

					display.print_flash_local(PSTR("АБС"),0,34,y_k);
				}
			}
			//===
#endif
#endif
			if((MsgKey.mode ==  TMsgKey::MODE::FIND_REF && 
				display.position_to_see[s] != CDisp::RESULT && 
				display.ifSearchRef(v) && !(flash_prg.axis_cfg[v].ref_pulse_zone & YESNO_P_bm)))
				{
					if (Lir.true_input[v])
						INVERT_CHAR()						
					else
					    NORMAL_CHAR()
						display.print_flash_local(sZona,0,13,y_k);
					NORMAL_CHAR()
				}
			

			if (display.go_to_blink_string[s] && display.blinker&0x04) 
				INVERT_CHAR()
			else  
				NORMAL_CHAR()
			if(Lir.curItem->id != idHandMove && Lir.curItem->id != idExecmd && Lir.entry_hand != Menu::WORKING_EXE){		
					if((MsgKey.tracer_real == TMsgKey::FUNC && MsgKey.mode == TMsgKey::HAND) || 
						(MsgKey.mode == TMsgKey::FIND_REF && display.ifSearchRefHAND())) 
						;//не обновляем осевые буквы в этих режимах
					else{		
						display.put_axis_shot(0,y_shot,(unsigned char)symbol);						
						Lir.strfunc_hand = N_STR;//сбрасываем текущий выбор оси для FUNC
						}					
				}
			NORMAL_CHAR();

			//if(!MsgKey.mode_ref_submode_ref_ref){
			//==мигание кнопки РМ 	
			if(flash_prg.axis_cfg[v].is_coded_ref == CPrg::NOM && 
			   display.position_to_see[s] != CDisp::RESULT && 
			   display.position_to_see[s] != CDisp::SPEED)
			{
				if(display.ifSearchRef(v))
				{
					if(display.blinker&0x04)
						display.clear_button_(X_BUTTON_PLACE,y_shot);
					else
						display.put_button_(X_BUTTON_PLACE,y_shot,_pic_REF);
				}
			}
			//==
		//	}
			if(display.position_to_see[s] == CDisp::RESULT)
				;//букву О или А не отображаем;
			else if(!init.mRef_ext[v])
				put_small_character_A_or_O_when_REF_NOT_FOUND(4,y,v);				    
			else  {
					if(display.ifSearchRef(v)){
					   #if (configUSE_NCshell == 1)
					   if(Lir.DIR_FIND_REF){
						   l_k_s.OFF_LED(_led_RIGHT);
						   l_k_s.OFF_LED(_led_LEFT);
						   Lir.DIR_FIND_REF = 0;
					   }
					   #endif
						if(display.ref_change_mode[v] != CDisp::SEARCH_REF_findref)						
							MsgKey.go_out_from_ref_mode_in_begin_mode();
						char pos_tmp[N_STR];// = {41,96,158,208};
						pos_tmp[0] = 41;pos_tmp[1] = 96;pos_tmp[2] = 158;pos_tmp[3] = 208;
						for(int s_ = 0 ; s_ < N_STR;s_++){
							if(symbol == Lir.Str[s_]){
								if(display.ref_change_mode[v] == CDisp::SEARCH_REF_findref && display.position_to_see[s_] != CDisp::RESULT && display.position_to_see[s_] != CDisp::SPEED){
									if(flash_prg.axis_cfg[v].is_coded_ref == CPrg::NOM)
										display.put_button_(X_BUTTON_PLACE,s_ * 7,_pic_REF);
									else
										display.put_button_(X_BUTTON_PLACE,s_ * 7,_pic_REF_REF);
								}									
								display.print_symbols_local(' ',32,0,0,pos_tmp[s_]);
							}
						}																
					}						
					Lir.ref_mode[v] = 0;
					init.ref_mode[v]=true; //установка на экране абсолютной оси в главном меню
					display.ref_change_mode[v] = CDisp::NOTHING;
					display.put_char_local_independent('A',0,4,y);
				  }
				//11.05.2017 r or d
			if(symbol < CInit::AXIS_NAME::AXIS_A && flash_prg.axis_cfg[v].r_d == CPrg::R_D::DIAMETER){					
					display.put_char_local_independent('Д',0,4,y_d);// old 'D'
					}
			if(symbol < CInit::AXIS_NAME::AXIS_A ){
					if(MsgKey.divide_by_2[v]){
						display.put_char_local_independent(':',0,2,y_k);// old 'D'
						display.put_char_local_independent('2',0,3,y_k);// old 'D'	
						}
						else
							display.print_symbols_local(' ',2,0,2,y_k);
					}			   

			if(flash_prg.axis_cfg[v].is_correction==CPrg::CORR::YES && MsgKey.mode != TMsgKey::FIND_REF && configUSE_menuItem_Correction_and_Izmerenia == 1)
				{
				if(ref_.pRef[v]){
					if(altera.error_corr_real_time[v] == CIOAltera::ERROR_CORR_REAL_TIME::NO_ERROR)
						display.print_flash_local(PSTR("K "),0,0,y_k);
					else if(altera.error_corr_real_time[v]==CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_PLUS_L_)
						display.print_flash_local(PSTR("< "),0,0,y_k);
					else if(altera.error_corr_real_time[v]==CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_PLUS_)
						display.print_flash_local(PSTR("> "),0,0,y_k);
					else if(altera.error_corr_real_time[v]==CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_MINES_)
						display.print_flash_local(PSTR("< "),0,0,y_k);
					else if(altera.error_corr_real_time[v]==CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_MINES_R_)
						display.print_flash_local(PSTR("> "),0,0,y_k);
					else if(altera.error_corr_real_time[v]==CIOAltera::ERROR_CORR_REAL_TIME::CANT_BE_DONE)
						display.print_flash_local(PSTR("  "),0,0,y_k);
					else
						display.print_debug_one_local_shot(altera.error_corr_real_time[v],0,0,y_k,2);
				}
				else
					display.print_flash_local(PSTR("  "),0,0,y_k);
				}
			 //==Суммирование осей
			 if(display.position_to_see[s] == CDisp::RESULT){
				char str[4] = {'?','+','?'};
				char *str_p = reinterpret_cast<char*>(&str);
					for(int  i = 0 ; i < N_STR;i++){
						if(display.position_to_see[i] == CDisp::PLUS){
							memcpy_P(str_p,&sAxis_screen[Lir.Str[i]][2],1);
							str_p++;
							str_p++;
						}							
					}
					display.print_ram_local(str,0,0,y_k);
				}	
			//==		
			}
		}
	}
}// for
return;// the end
}

void screen_service::to_clear_string_connectors(unsigned char num_place){
//19.10.2018 if 4 axis will be displayed it will take 2 ms.		
//   switch(place){
//	   case 0:place=5;break;
//	   case 7:place=12;break;
//	   case 14:place=19;break;
//	   case 21:place=26;break;
//	   default:return;
//   }		
	 switch(num_place){
		 case 4:
		 if(Lir.Str[0] != no_axis && display.position_to_see[0] != CDisp::RESULT && display.position_to_see[0] != CDisp::SPEED)
			display.print_symbols_local(' ',32,0,0,41);//display.clear_rectangle(0,5,320,2,0x00,0);				
		 if(Lir.Str[1] != no_axis && display.position_to_see[1] != CDisp::RESULT && display.position_to_see[1] != CDisp::SPEED)	 
			display.print_symbols_local(' ',32,0,0,96);//display.clear_rectangle(0,12,320,2,0x00,0);			
		 if(Lir.Str[2] != no_axis && display.position_to_see[2] != CDisp::RESULT && display.position_to_see[2] != CDisp::SPEED)
			display.print_symbols_local(' ',32,0,0,158);//display.clear_rectangle(0,19,320,2,0x00,0);	
		 if(Lir.Str[3] != no_axis && display.position_to_see[3] != CDisp::RESULT && display.position_to_see[3] != CDisp::SPEED)	 
			display.print_symbols_local(' ',32,0,0,208);//display.clear_rectangle(0,26,320,2,0x00,0); 
		 break;
		 case 0:display.clear_rectangle(0,5,320,2,0x00,0);break;
		 case 1:display.clear_rectangle(0,12,320,2,0x00,0);break;
		 case 2:display.clear_rectangle(0,19,320,2,0x00,0);break;
		 case 3:display.clear_rectangle(0,26,320,2,0x00,0);break; 
		default:break; 
	                 }
}


void screen_service::to_clear_place_for_virtual_buttons(unsigned char cnt){
//18.10.018 if 4 it takes 632 mks from call to return	
	display.clear_rectangle(X_BUTTON_PLACE,Y_ONE,48,5,0x00,0);
	display.clear_rectangle(X_BUTTON_PLACE,Y_TWO,48,5,0x00,0);
	display.clear_rectangle(X_BUTTON_PLACE,Y_THREE,48,5,0x00,0);
#if (configUSE_NCshell == 1)
	if(MsgKey.tracer_real != TMsgKey::CMD_CNTRL && MsgKey.tracer_real != TMsgKey::DRIVE_CONTROL){
		//if (!Lir._resz_prg_v.SZ)
			display.clear_rectangle(X_BUTTON_PLACE,Y_FOUR,48,5,0x00,0);
	}

	if(MsgKey.tracer_next == TMsgKey::DRIVE_CONTROL && Lir._resz_prg_v.SZ == 0)
		display.clear_rectangle(X_BUTTON_PLACE,Y_FOUR,48,5,0x00,0);

#else	
	display.clear_rectangle(X_BUTTON_PLACE,Y_FOUR,48,5,0x00,0);	
#endif	 
}

void screen_service::update_icons_hand_mode()
{
	for(unsigned char n=0;n<N_AXIS;n++)
		MsgKey.does_string_has_button[n]=false;
	screen.to_clear_place_for_virtual_buttons(4);
	MsgKey.to_look_for_submode_of_operation_inside_HAND();
}

void screen_service::convert_channel_counter_to_text_string(unsigned char n_ch,long counter,unsigned char *txt_out)
{
	int f = flash_prg.axis_cfg[n_ch].format;
	//unsigned char txt_out[15] = { 0 };

	if((f>=CPrg::FORMAT::LINEAR_3 && f<=CPrg::FORMAT::CIRCLE_4) || 
		f==CPrg::FORMAT::LINEAR_INCH_4 ||
		f==CPrg::FORMAT::LINEAR_6)
	{
		
		display.itoa_quick_lir(&counter,txt_out);
	    display.formatting2((char*)txt_out,f);
	}
	if(f==CPrg::FORMAT::CIRCLE_SECUNDO || f==CPrg::FORMAT::CIRCLE_MINUTO || f==CPrg::FORMAT::CIRCLE_SECUNDO_1)
	{
		Lir.convert_long_to_secundo2(counter, txt_out, f);
	}
}
/*
функция работает когда мигает 'ЗМ' или когда расстояние от метки до концевика измерено
*/
void screen_service::to_display_mode_ref_submode_measure(bool start,bool to_see_measurement_result){
unsigned char place,n_string,place_for_text_y,n_ch;
//unsigned char txt_out[22] = {0};
CInit::AXIS_NAME symbol;
CDouble::MULT_OR_DIV_ANSWER answer;	
static unsigned char blinker;
	
	if(start==true)	
		blinker++;
	else 
		blinker=0x0;

	for(n_string=0;n_string<N_AXIS;n_string++)	{
		if(measure_string[n_string]==true){
			break;
			}
		}

	if(n_string>=N_AXIS)
		return;	

	switch(n_string){
	case 0:
		place = Y_ONE; place_for_text_y=40;symbol=(CInit::AXIS_NAME)Lir.Str[0];break;
	case 1:
		place = Y_TWO; place_for_text_y=96;symbol=(CInit::AXIS_NAME)Lir.Str[1];break;
	case 2:
		place = Y_THREE; place_for_text_y=152;symbol=(CInit::AXIS_NAME)Lir.Str[2];break;
	case 3:
		place = Y_FOUR; place_for_text_y=208;symbol=(CInit::AXIS_NAME)Lir.Str[3];break;		
	}
	if(start==true){
		if(blinker&04)
			display.clear_button_(X_BUTTON_PLACE,place);
		else 
			display.put_button_(X_BUTTON_PLACE,place,_pic_ZONE_REF); 	
	}
	else 
		display.put_button_(X_BUTTON_PLACE,place,_pic_ZONE_REF);


	if(to_see_measurement_result==true){
//x-25	
      display.print_flash_local(PSTR("s-r:"),0,25,place_for_text_y);
	  n_ch=to_take_ch_number(symbol);

	  if(n_ch>=N_AXIS){ 
		display.print_flash_local(PSTR("no channel"),0,29,place_for_text_y);
		return;
	  }

	  if(screen.counter_measure<0)
		screen.counter_measure=-screen.counter_measure;
	  
	  answer=coeff.mult(n_ch,&screen.counter_measure);
	  if (answer!= CDouble::OK){
		display.print_flash_local(PSTR("mult bad"),0,29,place_for_text_y);return;   
	   }

	   Lir.print_long_format(screen.counter_measure,Lir.get_format_from_ch(n_ch),0,29,place_for_text_y,LEFT_ALIGN,0,11);
	}	
}
/*
Функция для отображения режима 'PM' ,результата, мигание кнопки 'PM'
*/
void screen_service::to_display_mode_ref_submode_ref_ref(bool start,bool to_see_ref_ref_result){

	unsigned char place,n_string;		

	for(n_string=0;n_string<N_AXIS;n_string++){
		if(measure_string[n_string])
			break;
		}

	if(n_string>=N_AXIS)
		return;

	switch(n_string){
		case 0:place=Y_ONE;break;
		case 1:place=Y_TWO;break;
		case 2:place=Y_THREE;break;
		case 3:place=Y_FOUR;break;
	}
	if(start){
		if(display.blinker&0x04)
			display.clear_button_(X_BUTTON_PLACE,place);
		else 
			display.put_button_(X_BUTTON_PLACE,place,_pic_REF_REF);
	}
	else 
		display.put_button_(X_BUTTON_PLACE,place,_pic_REF_REF);
	//Lir.print_long_format(counter_ref,CPrg::FORMAT_END,0,20,place_for_text_y,0,0,4); //кол-во меток
}
void screen_service::to_display_symbol_string(unsigned char n_ch){
//unsigned char place;
//char final[12];
unsigned char txt_out[20],sb;
char txt[20];
int n;
//CInit::AXIS_NAME symbol;
CDouble::MULT_OR_DIV_ANSWER answer;	
long counter;
cli();
   counter=screen.channel_counter[n_ch];
sei();  
   for(n=0;n<20;n++)txt_out[n]='?';
   txt_out[10]=0;
	 answer=coeff.mult(n_ch,&counter);
	 if (answer== CDouble::OK){
	  convert_channel_counter_to_text_string(n_ch,counter,txt_out);	
	   for(n_ch=0;n_ch<10;n_ch++){
		   if(txt_out[n_ch]==0)break;
	   }
	   txt_out[n_ch]=0;
	 }
	  sb=0;
	  for(n=0;;n++){ 
	  sb=txt[n]=txt_out[n];
	  if(sb==0)break;
	  }
	  display.print_ram_local(txt,0,8,16);
	  unsigned int m=n;
	  n=10-n;
	  if(n>=0)
	  display.print_symbols_local(0x20,n,0,8+m,16); 	  
}
void screen_service::to_display_channel_counter(unsigned char string_number){

// 496 mks
CInit::AXIS_NAME symbol = (CInit::AXIS_NAME)-1;
signed char n_ch = -1;
   if(!((string_number>0) && (string_number<5)))return;

   if(string_number == 1) symbol = (CInit::AXIS_NAME )Lir.Str[0];
   if(string_number == 2) symbol = (CInit::AXIS_NAME )Lir.Str[1];
   if(string_number == 3) symbol = (CInit::AXIS_NAME )Lir.Str[2];
   if(string_number == 4) symbol = (CInit::AXIS_NAME )Lir.Str[3];
   if(symbol>CInit::AXIS_NAME::AXIS_D)return;

	if (Lir.P2[0] == symbol)  n_ch =  0; 
	if (Lir.P2[1] == symbol)  n_ch =  1; 
	if (Lir.P2[2] == symbol)  n_ch =  2; 
	if (Lir.P2[3] == symbol)  n_ch =  3; 
	
	if(n_ch == -1) return;
unsigned int coord_y;
	if(string_number == 1) coord_y = 0;
	if(string_number == 2) coord_y = 56;
	if(string_number == 3) coord_y = 112;
	if(string_number == 4) coord_y = 168;

	//display.print_debug_one_local(n_ch,0,8,coord_y);

long counter;
unsigned char txt_out[15] = {0};
CDouble::MULT_OR_DIV_ANSWER answer;	

	cli();
	counter=screen.channel_counter[n_ch];
	sei();
	display.print_debug_one_local(counter/4,0,8,coord_y+16);

	answer=coeff.mult(n_ch,&counter);
	if (answer== CDouble::OK){
		convert_channel_counter_to_text_string(n_ch,counter,txt_out);
	}
	else
	{
		txt_out[0] = '?';
		txt_out[1] = '\0';
	}
	display.print_ram_local((char*)txt_out,0,8,coord_y); 

	unsigned int n = display.strnlen_lir((char*)txt_out,15);
	unsigned int m = n;
	n=10-n;
	if(n>=0)
	display.print_symbols_local(0x20,n,0,8+m,coord_y);

}
/*
функция для отображения больших координат и большой скорости
*/
unsigned char screen_service::show_big_digit(unsigned char n_string){
//08.11.2018 if linear inch -1234.5678" it takes 1.730 ms.
//if angle 0.0000 359.5649 it takes 1.6 ms
//if angle 359.34'25" it takes 1.730 ms		
	long counter;
	unsigned char v;
	bool goout=false;
	//bool inverse;
	//CInit::AXIS_NAME symbol;	
	if(n_string>4 || n_string==0)
		return 1;

	int n_str = n_string - 1;

	#if (configUSE_NCshell == 1)
		if(Lir.block_str[n_str])
			return 0;
	#endif

	if(display.position_to_see[n_str] == CDisp::RESULT)
		return 0;

    if(Lir.Str[n_str] == no_axis){
	//==показ времени
		if(display.position_to_see[n_str] == CDisp::TIME){
			char time_seconds[9],reverse_order[9];
			l_k_s.time_string_with_seconds(l_k_s.real_time_to_see,time_seconds);
			int index = 8;
			for(int i=0;i<8;i++)
				reverse_order[i]=time_seconds[--index];
			Lir.strings_out(n_string,8,reverse_order);
			MsgKey.see_data_str(n_str);	
			return 0;
			}
		else
			return 2;
	//==
	}
	
	v= Lir.get_channel(Lir.Str[n_str]);//to_take_ch_number(symbol); 
	if (v>=N_AXIS)
		return 3;
	if(display.position_to_see[n_str]==CDisp::SPEED)  
	{
		//display.put_speed_big_digit(n_string);return 4;
		if(!screen.speed_error_handler_if_big_digit(n_string - 1) && !Lir.Update_big_digit[n_string - 1])
		{
			screen.put_speed_big_digit(n_string);
			return 4;
		}
	    else 
		return 11;		
	}
					
	if(display.print_is_not_reference_executed(n_string,v,MsgKey.key_press_for_A_O1))
		return 5;							
	cli();
	counter = screen.channel_counter[v];
	sei();	

	counter = Lir.conv_c_i(counter,v,init.mRef_ext[v]);
	str_counter_before_out[n_str] = counter;
	//---------------------------------
	/*if (counter > MAX_LONG_PLUS  || counter < MAX_LONG_MINES) {
		flash_prg.to_set_axis_errors(v);
		counter=0;
	}  */
	//27.04.2017 to check encoder first...
	encoder_fault[n_string] = flash_prg.is_encoder_fault(v);
	if (encoder_fault[n_string]) {
		goout = true;
		unsigned char clear = 0;
		if (encoder_fault_[n_string] == 0)
		clear = 1;
		display.print_encoder_fault(counter,clear,n_string,v);
	}
	encoder_fault_[n_string] = encoder_fault[n_string];
	if (goout)
		return 10;

	error_overflow_type_long[n_string] = flash_prg.to_check_axis_errors(v);
	if (error_overflow_type_long[n_string] == 0) {		
		#if (configUSE_NCshell == 1)
		if(Lir.cms.on && Lir.cms.cur_fync == 0){ // доделать режим вывода
			if(Lir.cms.cur_move_show[v] == _pic_REM_CORR)
				counter = Lir.cnt_z[v];
			else if(Lir.cms.cur_move_show[v] == _pic_ZAD_CORR){
				if(Lir._modals_cmd.G90 == 0)
					counter = Lir.cnt_impulse_cur[v];
				else if(Lir._modals_cmd.G90 == 1)	
					counter = Lir.cnt_CNT_prev[v] + Lir.cnt_impulse_cur[v];//counter = Lir.tmp_pc[v] + Lir.cnt_impulse_cur[v];
				flash_prg.endless_circle(&counter,v);	
				}			
			}
		//==вывод ошибки программного ограничения
		if(Lir.print_prog_sw_error(n_str,v,counter))
			return 0;
		//==
		#endif
		error_overflow_no_empty_space[n_string] = you_can_see_big_digit(counter,v,n_string); //вывод на экран
		if (error_overflow_no_empty_space[n_string]){
			unsigned char clear=0;
			if (error_overflow_no_empty_space_[n_string] == 0)
				 clear=1;
			display.print_error_big_no_empty_space(counter,clear,n_string);
		}
		error_overflow_no_empty_space_[n_string]=error_overflow_no_empty_space[n_string];
	}
	else {
		unsigned char clear=0;
		if (error_overflow_type_long_[n_string] == 0) 
			clear=1;
		display.print_error_big_overflow_type_long(counter,clear,n_string);
	}
	error_overflow_type_long_[n_string]=error_overflow_type_long[n_string];
	return 0;
}

void screen_service::show_big_digit_result()
{
	for(int i = 0 ; i < N_STR;i++){
		if(display.position_to_see[i] == CDisp::RESULT){
			long cnt = 0;
			for(int a = 0 ; a < N_STR;a++){
				if(display.position_to_see[a] == CDisp::PLUS)
					cnt += str_counter_before_out[a];
			}
			int n_string  = i + 1;
			error_overflow_no_empty_space[n_string] = you_can_see_big_digit(cnt,Lir.get_channel(Lir.Str[i]),n_string);
			if (error_overflow_no_empty_space[n_string]){
				unsigned char clear=0;
				if (!error_overflow_no_empty_space_[n_string])
					clear=1;
				display.print_error_big_no_empty_space(cnt,clear,n_string);
			}
			error_overflow_no_empty_space_[n_string]=error_overflow_no_empty_space[n_string];
		}
	}
}

/*
функция проверяет bin ,конвертирует bin в ASCII строку  ,проверка ASCII строки ,делает выравнивание по правому краю
и выводит ASCII строку на экран большими или очень большими цифрами
in long data - счетчик
in unsigned char n_channel - номер канала (0 - 3)
in unsigned char place - номер строки (1 - 4)

return value bool - ошибка (1 - да/ 0 - нет)
*/
bool screen_service::you_can_see_big_digit(long data,unsigned char n_channel,unsigned char place){

	unsigned char string_size,n,index;
	unsigned char txt_out[15] = {0};
	char reverse_order[13];

	if(data>99999999 || data<-99999999) 
		return true;
	
	convert_channel_counter_to_text_string(n_channel,data,txt_out);
	for(n=0;n<15;n++){
	  if(txt_out[n]==0)
		break;
	}	
	index=string_size=n;
	for(n=0;n<string_size;n++)
		reverse_order[n]=txt_out[--index];
	if(n==0 || n==1 || n==2)
		return true;	
	reverse_order[string_size]=0;
	index=n;
	Lir.strings_out(place,string_size,reverse_order);     
	return false;
}	
/*
функция для отображения скорости под осью
*/
unsigned char screen_service::to_see_speed(unsigned char n_string){
	unsigned char v;
	long speed_0;
	unsigned char mm_string[17] = {0};
	//bool get_off_dot = false;
	//bool inches=false;

	if(n_string > 4 || n_string == 0)
		return 1;	
	int n_str = n_string - 1;

	if(Lir.Str[n_str] > CInit::AXIS_NAME::AXIS_D)
		return 2;

	v = to_take_ch_number(Lir.Str[n_str]);
	if (v >= N_AXIS)
		return 3;

	if(flash_prg.axis_cfg[v].readout_speed == CPrg::READOUT_SPEED::NO_SPEED)
		return 4;

	speed_0 = Lir.get_spd(v);

	if(speed_0 > 999999999 || speed_0 < -999999999)
		return 6;
	//===убираем знак минус
	if(speed_0 < 0)
		speed_0 *= -1;
	//===	
	to_convert_long_to_string_for_speed(&speed_0,mm_string,v);
	Lir.add_filler(mm_string,17,16,' ',1);
	if(display.position_to_see[n_str] == CDisp::SPEED){
		for(int i=0;i<15;i++)
			display.to_see_big_speed_[n_str][i] = mm_string[i];
	}
	//==если выбрано одна строка для скорости а вторая для координат то под координатами скорость не выводится
	for(int i = 0 ; i < N_STR;i++){		
		if(n_str != i){
			if(Lir.Str[n_str] == Lir.Str[i]){
				if(display.position_to_see[i] == CDisp::SPEED)
					return 0; // выходим, скорость под координатами не выводим
			}
		}
	}
	//==
	mm_string[10] = '\0';	

	#if (configUSE_NCshell == 1)
	if(Lir.block_str[n_str]){ // если блокировка
		if(Lir.small_axis_view)
			display.print_ram_local((char*)mm_string,0,19,(n_str *16));
		return 0;
	}
	#endif

	if(display.position_to_see[n_str] != CDisp::SPEED && display.position_to_see[n_str] != CDisp::RESULT)	
		display.print_ram_local((char*)mm_string,0,24,(n_str * 56) + 40);
	return 0; 
}
/*
функция для отображения скорости вместо координат оси , если стоит параметр "скорость" в оси экран и
в настройках оси "Отображение скорости" - да
*/
unsigned char  screen_service::put_speed_big_digit(unsigned char n_string){
	char tmp[13],reverse_order[13];
	unsigned char v,index,string_size;	
	
	for(v=0;v<13;v++)
		tmp[v]=0;
	       switch (n_string){
		   case 1:
				for(v=0;v<11;v++)
				tmp[v]=display.to_see_big_speed_[0][2+v];
				break;
		   case 2:
				for(v=0;v<11;v++)
				tmp[v]=display.to_see_big_speed_[1][2+v];
				break;
		   case 3:
				for(v=0;v<11;v++)
				tmp[v]=display.to_see_big_speed_[2][2+v];
				break;
		   case 4:
				for(v=0;v<11;v++)
				tmp[v]=display.to_see_big_speed_[3][2+v];
				break;
		   default:return 1;	      
		   } 
		for(v=0;v<13;v++){
			if(tmp[v]==0 || tmp[v]==0x20)
				break;
		}	
	index=string_size=v;
	for(v=0;v<string_size;v++)
		reverse_order[v]=tmp[--index];
	//if(v==0 || v==1 || v==2)return true;
	 if(v==0)
		 return 2;
	reverse_order[string_size]=0;
//	index=n;
	Lir.strings_out(n_string,string_size,reverse_order);
    
return 0;
}

void screen_service::speed_error_handler(void)
{
//CInit::AXIS_NAME symbol[N_AXIS];
unsigned char v;
int y_pos = 0;	
static unsigned char speed_error_memory[N_AXIS];

for(int i = 0 ;i<N_AXIS;i++)
{
  //speed_error[i] = 5; //test
	v = to_take_ch_number(Lir.Str[i]);
	//v = to_take_ch_number_simple(symbol[i]);
	if (v < N_AXIS){
		if(flash_prg.axis_cfg[v].readout_speed == CPrg::READOUT_SPEED::NO_SPEED) 
			continue;
		if(speed_error_memory[i] == speed_error[i]) 
			continue;
		if(display.position_to_see[i] != CDisp::SPEED && Lir.find_same_symbol(Lir.Str,i)) 
			continue;
		if(display.position_to_see[i] != CDisp::SPEED && MsgKey.follower_ref==TMsgKey::SUBMODE_REF::PURE_REF)
			continue; 
		if(display.position_to_see[i] == CDisp::SPEED){
			Lir.see_error_in_rectangle(speed_error_memory,speed_error,i);
			continue;
		}//Lir.debug++;
 
		if( i == 0 ) y_pos = 40;
		if( i == 1 ) y_pos = 96;
		if( i == 2 ) y_pos = 152;
		if( i == 3 ) y_pos = 208;

		switch(speed_error[i]){
			case 0:
			case 4:
//22.11.2018 			
			if(speed_error_memory[i]!=0 && MsgKey.follower_ref==TMsgKey::SUBMODE_REF::PURE_REF && display.go_to_blink_string[i]){
				display.print_symbols_local(0x20,40,0,0,y_pos);	   
				screen.connectors_to_see(&v,i*7);
				screen.switches_to_see(&v,i*7+2,true);		
				}
			/*if(speed_error_memory[i]!=0 && MsgKey.follower_ref==TMsgKey::SUBMODE_REF::TO_MEASURE_DISTANCE_FROM_REF_TO_SWITCH){
				display.print_symbols_local(0x20,40,0,0,y_pos);	 
				screen.connectors_to_see(&v,i*7);
				screen.switches_to_see(&v,i*7+2,true);      
			   }		*/
			if(speed_error_memory[i]!=0 && MsgKey.follower_ref==TMsgKey::SUBMODE_REF::PURE_REF && !display.go_to_blink_string[i]){
				display.print_symbols_local(0x20,40,0,0,y_pos);
				}
			if(speed_error_memory[i]!=0 && !l_k_s.IS_LED(_led_REF)){
				display.print_symbols_local(0x20,40,0,0,y_pos);	   		
				}
			
			speed_error_memory[i] = speed_error[i];
			break;
			
			default:
			display.print_symbols_local(' ',40,0,2,y_pos);
			speed_error_memory[i] = speed_error[i];
			break;
			case 1:
			display.print_flash_local(speed_error_str,0,2,y_pos);
			display.print_debug_one_local_shot(speed_error[i],0,29,y_pos,1);
            break;
			case 5:
			display.print_flash_local(speed_error_str,0,1,y_pos);
			display.print_flash_local(PSTR("умножение"),0,29,y_pos);
			speed_error_memory[i] = speed_error[i];
			break;
			case 6:
			display.print_flash_local(speed_error_str,0,1,y_pos);
			display.print_flash_local(PSTR("предел"),0,29,y_pos);
			speed_error_memory[i] = speed_error[i];
			break;
		}
	}
}
}
bool screen_service::speed_error_handler_if_big_digit(unsigned char n_string){
//y can be 0,7,14,21
unsigned char v;
int y_pos_rect = 0;
int y_pos_txt = 0;
static bool err_no_speed_memory[N_AXIS];
static bool err_no_speed[N_AXIS];
static unsigned char counter;
//static unsigned char speed_error_memory[N_AXIS];
    
	v = to_take_ch_number(Lir.Str[n_string]);

	if (v < N_AXIS){
		if(flash_prg.axis_cfg[v].readout_speed==CPrg::READOUT_SPEED::NO_SPEED ) err_no_speed[n_string] = true;
		if(flash_prg.axis_cfg[v].readout_speed==CPrg::READOUT_SPEED::YES_SPEED) err_no_speed[n_string] = false;

		if( n_string == 0 ) {y_pos_rect = 0;y_pos_txt = 24;}
		if( n_string == 1 ) {y_pos_rect = 7;y_pos_txt = 80;}
		if( n_string == 2 ) {y_pos_rect = 14;y_pos_txt = 136;}
		if( n_string == 3 ) {y_pos_rect = 21;y_pos_txt = 192;}

		if(err_no_speed_memory[n_string] != err_no_speed[n_string])
		{			
			if(err_no_speed[n_string] == true)
			{
				display.clear_rectangle(40,y_pos_rect,232,5,0x00,0);
				display.print_flash_local(s_CheckAxisParam,0,6,y_pos_txt);
				err_no_speed_memory[n_string] = true;
			}
			if(err_no_speed[n_string] == false)
				err_no_speed_memory[n_string] = false;
		}
		else
		{
			if(!Lir.Update_error) //!
			{
				int c = Lir.GetCountSpeedStr(reinterpret_cast<uint8_t*>(display.position_to_see));				
				if(err_no_speed[n_string] == true)
				{
					display.clear_rectangle(40,y_pos_rect,232,5,0x00,0);
					display.print_flash_local(s_CheckAxisParam,0,6,y_pos_txt);
				}
				if(counter == c) {Lir.Update_error = true;counter = 0;}
				counter++;
			}			
		}		
	}
	return err_no_speed[n_string];
}

bool screen_service::speed_error_handler_if_big_digit_(unsigned char n_string){
//y can be 0,7,14,21
unsigned char v;
//static bool err_no_speed_memory[N_AXIS];
static bool err_no_speed[N_AXIS];
int y_pos_rect = 0;
int y_pos_txt = 0;
static unsigned char counter_speed_error_handler;
v = to_take_ch_number(Lir.Str[n_string]);
	if (v < N_AXIS){
		          switch(n_string){
					  case 0: counter_speed_error_handler=0;break;
					  case 1: counter_speed_error_handler=1;break;
					  case 2: counter_speed_error_handler=2;break;
					  case 3: counter_speed_error_handler=3;break;
					  default: counter_speed_error_handler=0;
				  }
		if(flash_prg.axis_cfg[v].readout_speed==CPrg::READOUT_SPEED::NO_SPEED && Lir.Update_error ==false){
		// err_no_speed[n_string] = true;
		 if( n_string == 0 ) {y_pos_rect = 0;y_pos_txt = 24;}
		 else if( n_string == 1 ) {y_pos_rect = 7;y_pos_txt = 80;}
		 else if( n_string == 2 ) {y_pos_rect = 14;y_pos_txt = 136;}
		 else if( n_string == 3 ) {y_pos_rect = 21;y_pos_txt = 192;}
		 display.clear_rectangle(40,y_pos_rect,232,5,0x00,0);
		 display.print_flash_local(s_CheckAxisParam,0,6,y_pos_txt);
		 if(counter_speed_error_handler==3)
			Lir.Update_error =true;
		 err_no_speed[v]=true;
		 return true;
		           }
		else if(flash_prg.axis_cfg[v].readout_speed==CPrg::READOUT_SPEED::YES_SPEED){
			   		 err_no_speed[v]=false;    
		           }
		if(Lir.Update_error ==true)	{
			      if(err_no_speed[v]==true)return true;
				  else return false;
		           }
	               }
return false;
	
}

void screen_service::put_small_character_A_or_O_when_REF_NOT_FOUND(short x,short y,const unsigned char &ch)
{
	char rel = 'O';
	#if (configUSE_Lang == 1)
		rel = 'I';
	#endif
	if(display.ref_change_mode[ch] == CDisp::SEARCH_REF_begin)
		display.put_char_local_independent(' ',0,x,y);
	else if(display.ref_change_mode[ch] == CDisp::SEARCH_REF_findref )
		display.put_char_local_independent(' ',0,x,y);
	else if(display.ref_change_mode[ch] == CDisp::SEARCH_REF_hand )
		display.put_char_local_independent(' ',0,x,y);
	else if(display.ref_change_mode[ch] == CDisp::REF_NOT_FOUND )
		display.put_char_local_independent(rel,0,x,y);
	else
		display.put_char_local_independent(rel,0,x,y);
}
