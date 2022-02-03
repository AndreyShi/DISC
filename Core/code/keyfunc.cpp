/*
 * keyfunc.cpp
 *
 * Created: 03.10.2018 11:26:28
 *  Author: Andrew
 */ 
 #include "keyfunc.h"
 #include "config.h"
 #include "mcx514.h"
 #include "twitest.h"
 #include "uart_flash_PD6_7.h"

void TMsgKey::SET_M(MODE mode_)
{
	old_mode = mode;
	mode = mode_;
}

void TMsgKey::apply()
{
#if (configUSE_NCshell == 1)
	Lir.on_off_debugOUT_INPUT();
	if(Lir.alert && Lir.alert != Menu::STANOK_NE_GOTOV){
		if(key == _key_STOP || key == _key_C){
			//==не можем сбросить ошибку пока шпиндель не перестанет вращаться
			if(Lir.alert == Menu::SHPINDEL_NE_OSTANOVLEN && Lir.SHPINDEL && 
			!(flash_prg._INGen.shpindel_vrashaetsia & YESNO_P_bm))
				Lir.lock_error_SNO = true;  //сбрасываем ошибку путем блокировки появления этой ошибки снова
			//==
			if(Lir.alert == Menu::PLC_ERROR)
				Lir.lock_error_PLC = true;								
			Lir.status(PSTR(""),0,0,0,2);
		}
		return;
	}	
	
	if(Lir.INmove()){ // нажатие кнопок во время любого движения
		if(Lir.INMovingKeys())
			return;		
	}
	if(key == _key_STOP && Lir.mPR.on)
		Lir.mPR.on = false;
	if(key == _key_STOP || key == _key_MODE_HAND){
		if(MsgKey.tracer_real == TMsgKey::CMD_CNTRL && Lir.curItem->id == idHand){
			if(Lir.outM5_delay()){
				Lir.entry_hand = Menu::INIT;
				return;	//если шпиндель остановили, то обработка кнопки стоп завершается
			}			
		}
	}
#endif
	switch(mode){
	case MODE::BEGIN:
		begin_();
		break;
	case MODE::FIND_REF:
		find_ref();
		break;
	case MODE::HAND:
		hand();
		break;
	case MODE::INPUT_PARAM:
		input_param();
		break;
	}	
}
void TMsgKey::begin_(){

		for(int i = 0 ; i<N_AXIS;i++){
		if((key != _key_START && key != _key_STOP && key != key_press_for_A_O1) 
			&& (display.ref_change_mode[i] == CDisp::REF_NOT_FOUND))
			return;
		}
		
			
	   switch(key){
	   case _key_X:
	   #if (configUSE_Calc == 1)
			if(Lir.gotomenuDiagnostika()){
				MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);
				l_k_s.ON_LED(_led_X);
				Lir.SET_C(idCalc);
				display.turn_on_page(1);
				display.page_for_buttons = 1;
				Lir.save_load_calc(2);
				Lir.Calcfrommenu = 0;
			}
		#endif
	   break;
	   case _key_Y:
	   #if (configUSE_Linear_holes == 1)
			if(Lir.gotomenuDiagnostika()){
				MsgKey.mode = TMsgKey::MODE::INPUT_PARAM;
				MsgKey.old_mode = TMsgKey::MODE::BEGIN;
				l_k_s.ON_LED(_led_Y);
				Lir.SET_C(idInpointLinear);
				Lir.entry_lh = 0;
			}
	   #endif
	   break;
	   case _key_F4:
	   break;
	   case _key_Z:
	   #if (configUSE_Circle_holes == 1)
		if(Lir.gotomenuDiagnostika()){
			MsgKey.mode = TMsgKey::MODE::INPUT_PARAM;
			MsgKey.old_mode = TMsgKey::MODE::BEGIN;
			l_k_s.ON_LED(_led_Z);
			Lir.SET_C(idInpointRadius);
			Lir.entry = 0;
		}
	   #endif
	   break;
		case   _key_MODE_DIAGNOSTICS:
		if(Lir.gotomenuDiagnostika()){  
			MsgKey.mode = TMsgKey::MODE::INPUT_PARAM;
			MsgKey.old_mode=TMsgKey::MODE::BEGIN;
			l_k_s.ON_LED(_led_MODE_DIAGNOSTICS);
			//==сброс всех выходов в ноль
			#if (configUSE_NCshell == 1)
			Lir.rst_outP();
			#endif
			//==
		}
		break;
//#if (configUSE_mcx514 == 1)
//		case _key_BLANK4_L:
//#endif
		case   _key_MODE_HAND:
			_key_MODE_HAND_f();					
			break;
		case _key_REF:	
			if(Lir.gotoModeRef()){
				MsgKey.mode = TMsgKey::MODE::FIND_REF;
				MsgKey.old_mode = TMsgKey::MODE::BEGIN;
				Lir.gotoModeRef_from_button();
			}				
			break;
		case _key_STOP:
		for(int i = 0; i <N_AXIS ; i++){
			if(display.ref_change_mode[i] == CDisp::REF_NOT_FOUND)
			{
				display.ref_change_mode[i] = CDisp::NOTHING;
				return;
			}
		}
		break;
		case _key_START:
		for(int i = 0; i <N_AXIS ; i++){
			if(display.ref_change_mode[i] == CDisp::REF_NOT_FOUND)
			{	
				key = key_press_for_A_O;		
				MsgKey.SET_M(TMsgKey::MODE::FIND_REF);					
				find_ref();	
				display.ref_change_mode[i] = CDisp::SEARCH_REF_begin; 	
				#if (configUSE_NCshell == 1)
				if(Lir.if_vibor_ref())
					Lir.lir541comRef();
				#endif
				return;
			}
		}

		if(!enable_to_invert){   
			if(MsgKey.no_start_press_>=500) 
				enable_to_invert=true;
		}
		if(enable_to_invert){    
			display.invert_display++;
			if(display.invert_display&1)
				display.invert_in();
			else 
				display.invert_out();
			flash_prg.to_save_night_mode_to_EEPROM(&display.invert_display);
		}
		break;
		case _key_UP:
		init.contrast_value--;
		if(init.contrast_value <MIN_contr)
			init.contrast_value = MAX_contr;
		flash_prg.to_save_contrast_to_EEPROM(&init.contrast_value);
		break;
		case _key_DOWN:
		init.contrast_value++;
		if(init.contrast_value >MAX_contr)
			init.contrast_value = MIN_contr;
		flash_prg.to_save_contrast_to_EEPROM(&init.contrast_value);
		break;
		case _key_C:
		Lir.reset_error();
		break;

		case _key_F:
		to_look_for_submode_in_begin_mode();
		break;

		case _key_BLANK1:
		if(display.position_to_see[0]==CDisp::SPEED)
			break;
		if(does_string_has_button[0]==false)
			break;
		//display.print_debug_one_local(follower_next,0,0,224);		
		if(follower_begin==SUBMODE_BEGIN::SET_TO_ZERO){
			 go_to_zero_common(0);
		}
		else if(follower_begin==SUBMODE_BEGIN::A_O){
			key_press_for_A_O = _key_BLANK1;
			key_press_for_A_O1 = _key_BLANK1;
			 go_to_abs_rel_common(0);
		}
		break;

		case _key_BLANK2:
		if(display.position_to_see[1]==CDisp::SPEED)
			break;
		if(does_string_has_button[1]==false)break;
		//display.print_debug_one_local(follower_next,0,0,224);
		if(follower_begin==SUBMODE_BEGIN::SET_TO_ZERO){
			go_to_zero_common(1);
		}
		else if(follower_begin==SUBMODE_BEGIN::A_O){
		key_press_for_A_O = _key_BLANK2;
		key_press_for_A_O1 = _key_BLANK2;
			go_to_abs_rel_common(1);
		}
		break;

		case _key_BLANK3:
		if(display.position_to_see[2]==CDisp::SPEED)
			break;
		if(does_string_has_button[2]==false)break;
		if(follower_begin==SUBMODE_BEGIN::SET_TO_ZERO){
			go_to_zero_common(2);
		}
		else if(follower_begin==SUBMODE_BEGIN::A_O){
		key_press_for_A_O = _key_BLANK3;
		key_press_for_A_O1 = _key_BLANK3;
			go_to_abs_rel_common(2);
		}
		break;

		case _key_BLANK4:
		if(display.position_to_see[3]==CDisp::SPEED)
			break;
		if(does_string_has_button[3]==false)break;
		if(follower_begin==SUBMODE_BEGIN::SET_TO_ZERO){
			go_to_zero_common(3);
		}
		else if(follower_begin==SUBMODE_BEGIN::A_O){
		key_press_for_A_O = _key_BLANK4;
		key_press_for_A_O1 = _key_BLANK4;
			go_to_abs_rel_common(3);
		}
		break;

		case _key_BLANK1_L:
		 if(go_to_input_string_mode(0)){
			SET_M(TMsgKey::MODE::INPUT_PARAM);	
			Lir.SET_C(idHand_input);
		 }
		break;

		case _key_BLANK2_L:
		if(go_to_input_string_mode(1)){
			SET_M(TMsgKey::MODE::INPUT_PARAM);
			Lir.SET_C(idHand_input);	
		}
		break;

		case _key_BLANK3_L:
		if(go_to_input_string_mode(2)){
			SET_M(TMsgKey::MODE::INPUT_PARAM);	
			Lir.SET_C(idHand_input);
		}
		break;

		case _key_BLANK4_L:
		if(go_to_input_string_mode(3)){
			SET_M(TMsgKey::MODE::INPUT_PARAM);	
			Lir.SET_C(idHand_input);
		}
		break;
	   }
}

void TMsgKey::find_ref(void){

		for(int c = 0; c< N_AXIS ;c++){
 			if(key != _key_STOP 
			   && (display.ref_change_mode[c] == CDisp::SEARCH_REF_begin || display.ref_change_mode[c] == CDisp::SEARCH_REF_hand) 
			   && !((key == _key_LEFT || key == _key_RIGHT || key == _key_START) 
			   #if (configUSE_NCshell == 1)
			   && !Lir.if_vibor_ref()
			   #endif
			   )){return;} // проверка для поиска выбора направления реф
			}
			
unsigned char n_ch;	
		switch(key){
			case _key_RIGHT:
			#if (configUSE_NCshell == 1)
			if(!Lir.if_vibor_ref()){
				Lir.DIR_FIND_REF = 2;
				 //включить светодиод
				 l_k_s.ON_LED(_led_RIGHT);
				 l_k_s.OFF_LED(_led_LEFT);
				}
			#endif
			break;
			case _key_LEFT:
			#if (configUSE_NCshell == 1)
			if(!Lir.if_vibor_ref()){
				Lir.DIR_FIND_REF = 1;
				//включить светодиод
				l_k_s.OFF_LED(_led_RIGHT);
				l_k_s.ON_LED(_led_LEFT);
			}
			#endif
			break;
			case _key_C:
			Lir.reset_error();
			break;
			case   _key_MODE_DIAGNOSTICS:
			if(Lir.gotomenuDiagnostika()){
				MsgKey.mode = TMsgKey::MODE::INPUT_PARAM;
				MsgKey.old_mode=TMsgKey::MODE::FIND_REF;
				l_k_s.ON_LED(_led_MODE_DIAGNOSTICS);
				l_k_s.OFF_LED(_led_MODE_HAND);
				l_k_s.OFF_LED(_led_REF);
				go_out_from_ref_mode();	
				//==сброс всех выходов в ноль
				#if (configUSE_NCshell == 1)
				Lir.rst_outP();
				#endif
				//==
			}
			break;
			case _key_START:
			#if (configUSE_NCshell == 1)
			if(Lir.if_vibor_ref() == 0){
				if(Lir.DIR_FIND_REF)
					Lir.lir541comRef(Lir.DIR_FIND_REF);	
				else
					Lir.status(PSTR("Выберите направление!"),300);
				}
			else
				Lir.lir541comRef();					
			#endif
			break;
			case _key_STOP:
				if(go_out_from_ref_mode()){
					common_go_to_mode_begin();
					Lir.SET_C(idBegin);
				}					
			break;
			case _key_REF:
				go_out_from_ref_mode();
				common_go_to_mode_begin();
				Lir.SET_C(idBegin);							
			break;
			case   _key_MODE_HAND:
				SET_M(TMsgKey::MODE::HAND);
		        if(Lir.gotoModeHand())
					Lir.init_hand(0);
				go_out_from_ref_mode();	
//26.11.2018
                //tracer_next=SUBMODE_HAND::OFFSET;
				#if (configUSE_NCshell == 0)
					tracer_next = tracer_real = SUBMODE_HAND::FUNC;
				#else
					tracer_next = tracer_real;
				#endif
				offset_number = 0;
				to_look_for_submode_of_operation_inside_HAND();
			    submode_change_offset_inside_HAND();				
//------------------------------------								
			    break;
			case _key_F:
				if(!submode_mode_REF)
					break;
				for(n_ch=0;n_ch<N_AXIS;n_ch++){
					if(display.go_to_blink_string[n_ch])
						break;
				}
				if(n_ch>=N_AXIS)
					to_look_for_submode_in_ref_mode();  	
				break;

			case _key_BLANK1:
			case _key_BLANK1_L:
				key_BLANK_ref_mode(0);								
				break;

			case _key_BLANK2:
			case _key_BLANK2_L:
				key_BLANK_ref_mode(1);
				break;

			case _key_BLANK3:
			case _key_BLANK3_L:
				key_BLANK_ref_mode(2);
				break;

			case _key_BLANK4:
			case _key_BLANK4_L:
				key_BLANK_ref_mode(3);
				break;
										
		}
}

void TMsgKey::hand(void){

	for(int i = 0 ; i<N_AXIS;i++){
		if((key != _key_START && key != _key_STOP && key != _key_BLANK2/*key_press_for_A_O1*/)
		&& (display.ref_change_mode[i] == CDisp::REF_NOT_FOUND))
		return;
	}

	switch(key){
		case _key_UP:
			#if (configUSE_PLC == 1)
			if(tracer_real == SUBMODE_HAND::DRIVE_CONTROL || tracer_real == SUBMODE_HAND::CMD_CNTRL)
				Lir.Feed_UP();
			#endif
		break;
		case _key_DOWN:
			#if (configUSE_PLC == 1)
			if(tracer_real == SUBMODE_HAND::DRIVE_CONTROL || tracer_real == SUBMODE_HAND::CMD_CNTRL)
				Lir.Feed_DOWN();
			#endif
		break;
		case _key_C:
		if(!Lir.reset_error()){
			
			if(tracer_next == SUBMODE_HAND::CMD_CNTRL){
				//очистка строки , переход в режим ввода
				//memset(Lir.frame_cmd[0],'\0',FRAME_SZ);//очистка буфера ввода
				MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);
				Lir.menu_BLANK2_hand();
			}
		}
		break;
		case _key_ENTER:
		#if (configUSE_NCshell == 1)
		if(tracer_next == SUBMODE_HAND::CMD_CNTRL){
			//memset(Lir.frame_cmd[0],'\0',FRAME_SZ);//очистка буфера ввода
			MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);
			Lir.menu_BLANK2_hand();
		}
		#endif
		break;
		case   _key_MODE_DIAGNOSTICS:
		if(MsgKey.tracer_next == TMsgKey::CMD_CNTRL)
			break;
		if(Lir.gotomenuDiagnostika()){
			MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);
			l_k_s.ON_LED(_led_MODE_DIAGNOSTICS);
			l_k_s.OFF_LED(_led_MODE_HAND);
			l_k_s.OFF_LED(_led_REF);
			if_it_was_submode_half_inside_HAND();
			Lir.menu_D_hand();//key_ENTER
			//==сброс всех выходов в ноль
			#if (configUSE_NCshell == 1)
			Lir.rst_outP();
			Lir.status(PSTR(""));
			#endif
			//==
		}
		break;
		case _key_REF:
		if(MsgKey.tracer_next == TMsgKey::CMD_CNTRL)
			break;
		if(Lir.gotoModeRef()){
			MsgKey.SET_M(TMsgKey::MODE::FIND_REF);
			Lir.gotoModeRef_from_button();
			//screen.to_clear_place_for_virtual_buttons(4);
			clear_G5X();
			if_it_was_submode_half_inside_HAND();
			Lir.menu_R_hand();
		}
		break;
		case _key_START:
		//== начать поиск реф метки
		for(int i = 0; i <N_AXIS ; i++){
			if(display.ref_change_mode[i] == CDisp::REF_NOT_FOUND)
			{
				key = key_press_for_A_O;
				MsgKey.SET_M(TMsgKey::MODE::FIND_REF);
				find_ref();
				display.ref_change_mode[i] = CDisp::SEARCH_REF_hand;
				#if (configUSE_NCshell == 1)
				if(Lir.if_vibor_ref())
					Lir.lir541comRef();
				#endif
				return;
			}
		}
		//==
		//==начать выполнение кадра из преднабора
		#if (configUSE_NCshell == 1)
		if(tracer_next == SUBMODE_HAND::CMD_CNTRL)
			Lir.menu_START_hand();
		#endif
		//==
		break;
		case _key_BLANK1_L:
		case _key_BLANK2_L:
		case _key_BLANK3_L:
		case _key_BLANK4_L:
		#if (configUSE_NCshell == 1)
		if(MsgKey.tracer_real == TMsgKey::DRIVE_CONTROL){
			if(Lir.extPULT.on)
				break;
			Lir.set_mh();			
		}
		#endif
		if(MsgKey.tracer_real == TMsgKey::FUNC){
			if(Lir.is_str_mh(MsgKey.key)){	
				Lir.invert_axis_in_modeFUNC(MsgKey.key);							
				if(Lir.strfunc_hand < N_STR){
					int8_t n_ch = Lir.get_channel(Lir.Str[Lir.strfunc_hand]);
					if(divide_by_2[n_ch])
						common_for_to_put_buttons(3,_pic_HALF);
					else
						common_for_to_put_buttons(3,_pic_FULL);
				}				
			}
		}		
		break;
		case _key_STOP:
		case _key_MODE_HAND:
		#if (configUSE_NCshell == 1)
		if(tracer_next == SUBMODE_HAND::CMD_CNTRL){			
			Lir.exit_hand_CMD_CTRL(0);			
			break;
		}
		#endif		
		for(int i = 0; i <N_AXIS ; i++){
			if(display.ref_change_mode[i] == CDisp::REF_NOT_FOUND)
			{
				display.ref_change_mode[i] = CDisp::NOTHING;
				return;
			}
		}
		//21.11.2018
		common_go_to_mode_begin();
		screen.to_clear_place_for_virtual_buttons(4);
		Lir.SET_C(idBegin);
		#if (configUSE_NCshell == 1)
		Lir.show_cmd(0,0,0);
		#endif
		clear_G5X();
		if_it_was_submode_half_inside_HAND();
		break;
		case _key_F:
		screen.update_icons_hand_mode();
		break;
		case _key_BLANK1:
		key_press_for_A_O = _key_BLANK1;
		key_press_for_A_O1 = _key_BLANK1;
		to_do_inside_submode_for_mode_HAND(0);
		break;
		case _key_BLANK2:
		key_press_for_A_O = _key_BLANK2;
		key_press_for_A_O1 = _key_BLANK2;
		to_do_inside_submode_for_mode_HAND(1);
		break;
		case _key_BLANK3:
		key_press_for_A_O = _key_BLANK3;
		key_press_for_A_O1 = _key_BLANK3;
		to_do_inside_submode_for_mode_HAND(2);
		break;
		case _key_BLANK4:
		key_press_for_A_O = _key_BLANK4;
		key_press_for_A_O1 = _key_BLANK4;
		to_do_inside_submode_for_mode_HAND(3);
		break;
	}
	
}

void TMsgKey::input_param(){
	Lir.number_key(key);
	cli();
	Lir.cursorrunner=0;
	sei();
	if(key == _key_STOP && Lir.curItem->id == idCalc){
		if(Lir.Calcfrommenu == 0){
			key = _key_MODE_DIAGNOSTICS;
			l_k_s.OFF_LED(_led_X);
			Lir.SET_C(idDiag);	//дальше по коду выход из меню
		}
		
		display.turn_on_page(0);
		display.page_for_buttons = 0;
		Lir.save_load_calc(1);
		if(Lir.Calcfrommenu == 1){
			Lir.SET_C(idDiag);
			return;
		}
	}
	#if (configUSE_Circle_holes == 1)
	if(key == _key_STOP && Lir.curItem->id == idInpointRadius){
		key = _key_MODE_DIAGNOSTICS;
		Lir.SET_C(idDiag);
		l_k_s.OFF_LED(_led_Z);
	}
	#endif
	#if (configUSE_Linear_holes == 1)
	if(key == _key_STOP && Lir.curItem->id == idInpointLinear){
		key = _key_MODE_DIAGNOSTICS;
		Lir.SET_C(idDiag);
		l_k_s.OFF_LED(_led_Y);
	}
	#endif

	switch(key){
		case _key_1:
		Lir.menu_1_LINKPK();
		break;
		case _key_2:
		Lir.menu_2_LINKPK();
		//Lir.menu_2_ABOU();
		break;
		case _key_F4:
			#if (configUSE_PLC == 1)
			Lir.key_F4_cmd();
			#endif
		break;
		case _key_BLANK1_L:
		case _key_BLANK2_L:
		case _key_BLANK3_L:
		case _key_BLANK4_L:
		pult_counters.reset_counters_rel(&key);
		#if (configUSE_NCshell == 1)
		Lir.change_pic_axis_mh(0);
		Lir.blank_left_input_cmd(key);
		#endif
		break;
		case _key_BLANK1:
		case _key_BLANK2:
		case _key_BLANK3:
		case _key_BLANK4:
		Lir.ref_to_ref_inEncoderMenu(key);
		Lir.input_to_ref_inViewinputsMenu(key);
		
		#if (configUSE_NCshell == 1)
		Lir.blank_right_EXE(key);
		Lir.blank_right_input_cmd(key);
		Lir.blank_right_input_EDITPR(key);
		Lir.BLANK1_mh(key);
		Lir.BLANK2_mh(key);
		Lir.BLANK3_mh(key);
		#endif
		#if (configUSE_Linear_holes == 1)
		Lir.menu_lh_blank(key);
		#endif
		#if (configUSE_Circle_holes == 1)
		Lir.menu_ch_blank(key);
		#endif
		break;
		case _key_C:
		#if (configUSE_NCshell == 1)
		Lir.delete_cmd_exe();
		Lir.menu_C_EDITPR();
		Lir.menu_C_DOUT();
		#endif
		if(Lir.curItem->id == idLinkPK )
			uart_usb.reset_error();
		break;
		case _key_F:
		if (Lir.curItem->id == idCalc)
		{
			Lir.mode_calc++;
			if (Lir.mode_calc > 2)
			Lir.mode_calc = 0;
		}
		#if (configUSE_NCshell == 1)
		if(Lir.curItem->id == idCmd || Lir.curItem->id == idEditPrgInput){
			Lir.change_func_cmd_F();
			break;
		}
		Lir.menu_F_EDITPR();
		Lir.menu_F_FINGE();
		Lir.menu_F_FOUTGE();
		Lir.menu_F_SOUTGE();
		Lir.menu_F_MOUTGE();
		Lir.menu_F_MINGE();
		Lir.menu_F_SINGE();
		Lir.menu_F_INAXI();
		Lir.menu_F_OUTAXI();
		Lir.menu_F_DISCZ();
		Lir.F_mh();
		#endif
		Lir.m_F_cntr_pr();		
		break;
		case   _key_MODE_DIAGNOSTICS:
		if(Lir.exitmenuDiagnostika(idBegin)){
			common_go_to_mode_begin();
			cli();
			ref_.disable_PA0_int_1_corr();
			ref_.disable_PB5_int_0();
			ref_.disable_PA1_int_2();
			ref_.disable_PB3_int_3();
			sei();
		}
		break;
		case   _key_MODE_HAND:
		Lir.goto_mode(TMsgKey::MODE::HAND ,TMsgKey::MODE::INPUT_PARAM,1);
		break;
		case   _key_REF:
		if(Lir.exitmenuDiagnostika(idRef)){
			MsgKey.SET_M(TMsgKey::MODE::FIND_REF);
			display.clear_ram(0x00,0);
			Lir.gotoModeRef_from_button();
		}
		break;
		case _key_START:
		Lir.key_start_actions(key);
		break;
		case  _key_STOP:
		{
			Lir.key_stop_actions();
			if(Lir.curItem->id == idMenuEncoders){
				for(int i = 0 ; i < N_AXIS;i++){
					if(stop_PM(i))
					goto without_command_key;
				}
			}
			if(Lir.curItem->id == idViewInputs){
				for(int i = 0 ; i < N_AXIS;i++){
					if(stop_ZM(i))
					goto without_command_key;
				}
			}
			Lir.menu_stop_DIAG();
			Lir.menu_stop_INSTRC();
			Lir.menu_stop_ABOU();
			Lir.menu_stop_LINPK();
			Lir.menu_stop_gs();
			Lir.menu_stop_REPA();
			Lir.menu_stop_REOF();
			Lir.menu_stop_pa();
			Lir.menu_stop_da();
			Lir.menu_stop_AXILIST();    //_key_STOP
			Lir.menu_stop_DBG();
			Lir.menu_stop_ENCM();
			Lir.menu_stop_SUPM();
			Lir.menu_stop_TIME();
			Lir.menu_stop_VIIN();
			Lir.menu_stop_CORR();
			Lir.menu_stop_IZMER();
			Lir.menu_stop_OFFSN();
			//Lir.menu_stop_FSET();
			Lir.menu_stop_OPT();
			#if (configUSE_NCshell == 1)
			Lir.menu_stop_DOUT();
			Lir.menu_stop_OUTGEN();    //_key_STOP
			Lir.menu_stop_FOUTGE();   //_key_STOP
			Lir.menu_stop_SOUTGE();    //_key_STOP
			Lir.menu_stop_MOUTGE();    //_key_STOP
			Lir.menu_stop_INGEN();
			Lir.menu_stop_FINGE();
			Lir.menu_stop_SINGE();
			Lir.menu_stop_MINGE();
			Lir.menu_stop_CHPA();
			Lir.menu_stop_SPDZ();
			Lir.menu_stop_G1_G0();
			Lir.menu_stop_VIXPM();
			Lir.menu_stop_VIBLUF();
			Lir.menu_stop_CNTRL();
			Lir.menu_stop_ZNBRK();
			Lir.menu_stop_INAXI();
			Lir.menu_stop_OUTAXI();
			Lir.menu_stop_DISCZ();
			Lir.menu_stop_ANALZ();
			Lir.menu_stop_LUBR();
			#endif
			
			
			#if (configUSE_SnakeGame == 1)
			Lir.exit_game();
			#endif
			without_command_key:
			if (Lir.curItem->id == idBegin){
				common_go_to_mode_begin();
				cli();
				ref_.disable_PA0_int_1_corr();
				ref_.disable_PB5_int_0();
				ref_.disable_PA1_int_2();
				ref_.disable_PB3_int_3();
				sei();
			}
			#if (configUSE_mcx514 == 1)
			Lir.m_stop_cntr();
			Lir.m_stop_cntr_hw();
			#endif
			#if (configUSE_NCshell == 1)
			Lir.stop_mh();
			Lir.menu_stop_exe();
			Lir.menu_stop_Prg_SubPrg();
			Lir.menu_stop_LISPRG();
			Lir.menu_stop_LISSUB();
			Lir.menu_stop_EDITPR();
			Lir.menu_STOP_DEALLP();
			Lir.menu_stop_ANOUT();
			Lir.menu_stop_ANIN();
			#endif
			Lir.m_stop_cntr_pr();
			#if (configUSE_ABS_Encoder == 1)
			Lir.m_stop_OABS();
			#endif
		}
		break;
		case _key_UP:
		{
			Lir.menu_up_DIAG();
			Lir.menu_up_gs();
			Lir.menu_up(idPultOcb,Lir.menu_cnt_pa,Lir.menu_pa_SZ,Lir.entry_pa);//Lir.menu_up_pa();
			Lir.menu_up(idScreenOcb,Lir.menu_cnt_da,Lir.menu_da_SZ,Lir.entry_da);//Lir.menu_up_da();
			Lir.menu_up(idIndicOpt,Lir.menu_cntr_cnt_pr,Lir.menu_cntr_SZ,Lir.entry_cntr_pr);//Lir.m_up_cntr_pr();
			Lir.menu_up_OPT();
			Lir.menu_up_AXILIST();	     //_key_UP
			Lir.menu_up_DBG();
			Lir.menu_up_CORR();
			Lir.menu_up_IZMER();
			Lir.menu_up_OFFSN();
			Lir.menu_up_VIIN();
			#if (configUSE_NCshell == 1)
			Lir.menu_up_DOUT();
			Lir.menu_up_MOUTGE();	   //_key_UP
			Lir.menu_up_SOUTGE();	   //_key_UP
			Lir.menu_up_FOUTGE();	   //_key_UP
			Lir.menu_up(idOutGen,Lir.menu_cnt_OUTGEN,Lir.menu_OUTGEN_SZ,Lir.entry_OUTGEN);//Lir.menu_up_OUTGEN();	    //_key_UP
			Lir.menu_up(idMInGen,Lir.menu_cnt_MINGE,Lir.menu_MINGE_SZ,Lir.entry_MINGE);//Lir.menu_up_MINGE();
			Lir.menu_up_SINGE();
			Lir.menu_up_FINGE();
			Lir.menu_up(idInGen,Lir.menu_cnt_INGEN,Lir.menu_INGEN_SZ,Lir.entry_INGEN);//Lir.menu_up_INGEN();
			Lir.menu_up_G1_G0();
			Lir.menu_up(idChpa,Lir.menu_cnt_CHPA,Lir.menu_CHPA_SZ,Lir.entry_CHPA);//Lir.menu_up_CHPA();
			Lir.menu_up(idSpdZ,Lir.menu_cnt_SPDZ,Lir.menu_SPDZ_SZ,Lir.entry_SPDZ);//Lir.menu_up_SPDZ();
			Lir.menu_up_CNTRL();
			Lir.menu_up_ZNBRK();
			Lir.menu_up_VIXPM();
			Lir.menu_up_VIBLUF();
			Lir.menu_up_INAXI();
			Lir.menu_up_OUTAXI();
			Lir.menu_up_LUBR();
			Lir.menu_up_DISCZ();
			Lir.menu_up_ANALZ();
			#endif
			#if (configUSE_NCshell == 1)
			Lir.menu_up_INSTRC();
			Lir.choice_exe_up();
			Lir.menu_up_LISPRG();
			Lir.menu_up_LISSUB();
			Lir.menu_up_Prg_SubPrg();
			Lir.menu_up_EDITPR();
			Lir.menu_up_ANOUT();
			Lir.menu_up_ANIN();
			Lir.key_UP_mh();
			#endif
			#if (configUSE_mcx514 == 1 )
			Lir.m_up_cntr();
			Lir.m_up_cntr_hw();
			#endif
			#if (configUSE_Circle_holes == 1)
			Lir.menu_ch_up();
			#endif
			#if (configUSE_Linear_holes == 1)
			Lir.menu_lh_up();
			#endif
			#if (configUSE_Calc == 1)
			if (Lir.curItem->id == idCalc){
				Lir.calc_cursor_up();
				return;
			}
			#endif
			
			Lir.direup();
			#if (configUSE_ABS_Encoder == 1)
			Lir.m_up_OABS();
			#endif
		}
		break;
		case _key_DOWN:
		{
			Lir.menu_down_DIAG();
			Lir.m_down_cntr_pr();
			Lir.menu_down_gs();
			Lir.menu_down_pa();
			Lir.menu_down_da();
			Lir.menu_down_OPT();
			Lir.menu_down_AXILIST();    //_key_DOWN
			Lir.menu_down_DBG();
			Lir.menu_down_CORR();
			Lir.menu_down_IZMER();
			Lir.menu_down_OFFSN();
			Lir.menu_down_VIIN();
			#if (configUSE_NCshell == 1)
			Lir.menu_down_DOUT();
			Lir.menu_down_MOUTGE();    //_key_DOWN
			Lir.menu_down_SOUTGE();    //_key_DOWN
			Lir.menu_down_FOUTGE();   //_key_DOWN
			Lir.menu_down_OUTGEN();    //_key_DOWN
			Lir.menu_down_MINGE();
			Lir.menu_down_SINGE();
			Lir.menu_down_FINGE();
			Lir.menu_down_INGEN();
			Lir.menu_down_G1_G0();
			Lir.menu_down_CHPA();
			Lir.menu_down_SPDZ();
			Lir.menu_down_CNTRL();
			Lir.menu_down_ZNBRK();
			Lir.menu_down_VIXPM();
			Lir.menu_down_VIBLUF();
			Lir.menu_down_INAXI();
			Lir.menu_down_OUTAXI();
			Lir.menu_down_LUBR();
			Lir.menu_down_DISCZ();
			Lir.menu_down_ANALZ();
			#endif
			#if (configUSE_NCshell == 1)
			Lir.menu_down_INSTRC();
			Lir.choice_exe_down();
			Lir.menu_down_Prg_SubPrg();
			Lir.menu_down_LISPRG();
			Lir.menu_down_LISSUB();
			Lir.menu_down_EDITPR();
			Lir.menu_down_ANOUT();
			Lir.menu_down_ANIN();
			Lir.key_DOWN_mh();
			#endif
			#if (configUSE_mcx514 == 1)
			Lir.m_down_cntr();
			Lir.m_down_cntr_hw();
			#endif
			#if (configUSE_Linear_holes == 1)
			Lir.menu_lh_down();
			#endif
			#if (configUSE_Circle_holes == 1)
			Lir.menu_ch_down();
			#endif
			#if (configUSE_Calc == 1)
			if (Lir.curItem->id == idCalc){
				Lir.calc_cursor_down();
				return;
			}
			#endif
			
			Lir.diredown();
			#if (configUSE_ABS_Encoder == 1)
			Lir.m_down_OABS();
			#endif
		}
		break;
		case _key_RIGHT:
		{
			Lir.m_right_cntr_pr();
			Lir.menu_right_pa();
			Lir.menu_right_da();
			Lir.menu_right_OPT();
			#if (configUSE_NCshell == 1)
			Lir.menu_right_gs();  //_key_LEFT
			Lir.menu_right_MOUTGE(); //_key_RIGHT
			Lir.menu_right_SOUTGE(); //_key_RIGHT
			Lir.menu_right_FOUTGE();  //_key_RIGHT
			Lir.menu_right_DOUT();
			Lir.menu_right_MINGE();
			Lir.menu_right_SINGE();
			Lir.menu_right_FINGE();
			Lir.menu_right_G1_G0();
			Lir.menu_right_CNTRL();
			Lir.menu_right_VIXPM();
			Lir.menu_right_VIBLUF();
			Lir.menu_right_INAXI();
			Lir.menu_right_OUTAXI();
			Lir.menu_right_LUBR();
			Lir.menu_right_DISCZ();
			Lir.menu_right_ANALZ();
			Lir.menu_right_ANIN();
			#endif
			#if (configUSE_NCshell == 1)
			Lir.set_RIGHT_dir_mh();
			Lir.menu_right_LISPRG();
			Lir.menu_right_LISSUB();
			Lir.menu_right_EDITPR();
			Lir.menu_right_exe();
			#endif
			#if (configUSE_mcx514 == 1)
			Lir.m_right_cntr();
			#endif
			#if (configUSE_Calc == 1)
			Lir.calc_copy_result();
			#endif
			#if (configUSE_Linear_holes == 1)
			Lir.change_param_lh_left_right('+');
			#endif
			#if (configUSE_Circle_holes == 1)
			Lir.change_param_ch_left_right('+');
			#endif
			
			Lir.direright();
			Lir.menu_right_VIIN();			
			#if (configUSE_ABS_Encoder == 1)
			Lir.m_right_OABS();
			#endif
		}
		break;
		case _key_LEFT:
		{
			Lir.m_left_cntr_pr();
			Lir.menu_left_pa();
			Lir.menu_left_da();
			Lir.menu_left_OPT();
			#if (configUSE_NCshell == 1)
			Lir.menu_left_gs();  //_key_LEFT
			Lir.menu_left_MOUTGE();  //_key_LEFT
			Lir.menu_left_SOUTGE();  //_key_LEFT
			Lir.menu_left_FOUTGE();   //_key_LEFT
			Lir.menu_left_DOUT();
			Lir.menu_left_MINGE();
			Lir.menu_left_SINGE();
			Lir.menu_left_FINGE();
			Lir.menu_left_G1_G0();
			Lir.menu_left_CNTRL();
			Lir.menu_left_VIXPM();
			Lir.menu_left_VIBLUF();
			Lir.menu_left_INAXI();
			Lir.menu_left_OUTAXI();
			Lir.menu_left_LUBR();
			Lir.menu_left_DISCZ();
			Lir.menu_left_ANALZ();
			Lir.menu_left_ANIN();
			#endif
			#if (configUSE_NCshell == 1)
			Lir.set_LEFT_dir_mh();
			Lir.menu_left_LISPRG();
			Lir.menu_left_LISSUB();
			Lir.menu_left_EDITPR();
			Lir.menu_left_exe();
			#endif
			#if (configUSE_mcx514 == 1)
			Lir.m_left_cntr();
			#endif
			#if (configUSE_Linear_holes == 1)
			Lir.change_param_lh_left_right('-');
			#endif
			#if (configUSE_Circle_holes == 1)
			Lir.change_param_ch_left_right('-');
			#endif
			
			Lir.direleft();
			Lir.menu_left_VIIN();			
			#if (configUSE_ABS_Encoder == 1)
			Lir.m_left_OABS();
			#endif
		}
		break;
		case _key_ENTER:
		{
			#if (configUSE_mcx514 == 1 )
			Lir.m_enter_cntr_hw();
			Lir.m_enter_cntr();
			#endif
			#if (configUSE_NCshell == 1)
			Lir.menu_enter_DOUT();
			Lir.menu_enter_MOUTGE();   //_key_ENTER
			Lir.menu_enter_SOUTGE();   //_key_ENTER
			Lir.menu_enter_FOUTGE();  //_key_ENTER
			Lir.menu_enter_OUTGEN();   //_key_ENTER
			Lir.menu_enter_MINGE();
			Lir.menu_enter_SINGE();
			Lir.menu_enter_FINGE();
			Lir.menu_enter_INGEN();
			#endif
			#if (configUSE_NCshell == 1)
			Lir.menu_enter_INSTRC();
			Lir.go_edit_frame_exe(key);
			Lir.move_axis_mh();
			Lir.menu_enter_EDITPR();
			Lir.menu_enter_LISSUB();
			Lir.menu_enter_LISPRG();
			Lir.menu_enter_Prg_SubPrg();
			Lir.menu_enter_ANOUT();
			Lir.menu_enter_ANIN();
			Lir.menu_enter_LUBR();
			#endif
			Lir.m_enter_cntr_pr();
			#if (configUSE_NCshell == 1)
			Lir.menu_enter_ANALZ();
			Lir.menu_enter_DISCZ();
			Lir.menu_enter_OUTAXI();
			Lir.menu_enter_INAXI();
			Lir.menu_enter_ZNBRK();
			Lir.menu_enter_CNTRL();
			Lir.menu_enter_VIXPM();
			Lir.menu_enter_VIBLUF();
			Lir.menu_enter_G1_G0();
			Lir.menu_enter_SPDZ();
			Lir.menu_enter_CHPA();
			#endif
			Lir.menu_enter_ABOU();
			Lir.menu_enter_AXILIST();   //_key_ENTER
			Lir.menu_enter_VIIN();
			Lir.menu_enter_DBG();
			Lir.menu_enter_CORR();
			Lir.menu_enter_IZMER();
			//Lir.menu_enter_FSET();
			Lir.menu_enter_OFFSN();
			#if (configUSE_Linear_holes == 1)
			Lir.enter_to_input_loop_lh();
			#endif
			#if (configUSE_Circle_holes == 1)
			Lir.enter_to_input_loop_ch();
			#endif
			Lir.menu_enter_da();
			Lir.menu_enter_gs(); // общие параметры
			Lir.menu_enter_OPT();
			Lir.menu_enter_DIAG();			
		}
		break;
		default:
		break;
	}
	Lir.take_buttons(key);
}


bool TMsgKey::go_to_input_string_mode(unsigned char s){
	unsigned char c;
	if(display.position_to_see[s] == CDisp::SPEED) 
		return false; 
	if(to_take_channel_number(Lir.Str[s],c)) 
		return false;	

	if(init.mRef_ext[c]){
		return false;
	//}else if(Lir.is_corr(c)){
	//	return false;
	}else{
		  Lir.init_cursor_pos();	
		  display.go_to_blink_string[s] = true;
		  what_channel_is_chosen_for_input = c; 
		  Lir.CurFormatM = flash_prg.axis_cfg[c].format;
		  return true;
	     }

return false;	
}
void TMsgKey::input_string_inside_begin_mode(void){
	Lir.number_key(key);
}

/*
функция для обработки нажатий кнопок в Ручном режиме (HAND MODE) только для _key_BLANK1 _key_BLANK2 _key_BLANK3 _key_BLANK4
*/
void TMsgKey::to_do_inside_submode_for_mode_HAND(unsigned char n_string){

	switch (tracer_real){
		case SUBMODE_HAND::CMD_CNTRL://обработка кнопок в INPUTPARAM
			break;
		
		case SUBMODE_HAND::DRIVE_CONTROL:
		#if (configUSE_NCshell == 1)
		if(key == _key_BLANK1){ // Выбор программ или вход в Программный режим
			if(Lir._choice_prg.cnt){ //если есть выбранная программа с кадрами
				MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);
				l_k_s.OFF_LED(_led_MODE_DIAGNOSTICS);
				l_k_s.ON_LED(_led_MODE_HAND);
				l_k_s.OFF_LED(_led_REF);
				if_it_was_submode_half_inside_HAND();
				Lir.menu_enter_hand();//key_ENTER
			}
			else{
				Lir.btn_zapusk_prg = 1;
				goto blank3;
				}
			return;
		}
		else if(key == _key_BLANK2){ // Вход в режим Преднабора
			tracer_next = SUBMODE_HAND::CMD_CNTRL;
			Lir.init_hand_CMD_CTRL(0);
			//== сразу переходим в режим ввода
			MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);
			Lir.menu_BLANK2_hand();
			//==								
		}
		else if(key == _key_BLANK3){ // Выбор программ
				Lir.btn_zapusk_prg = 0;
				blank3:
				MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);
				l_k_s.OFF_LED(_led_MODE_DIAGNOSTICS);
				l_k_s.ON_LED(_led_MODE_HAND);
				l_k_s.OFF_LED(_led_REF);
				//if_it_was_submode_half_inside_HAND();
				Lir.menu_BLANK3_hand(); 		
		}
		else if(key == _key_BLANK4){			
		}
		#endif
		break;

		case SUBMODE_HAND::FUNC:
		//#if (configUSE_NCshell == 1)
			if(key == _key_BLANK1){
				offset_number++;
				if(offset_number>5)
					offset_number=0;
				submode_change_offset_inside_HAND();
			}
			else if(key == _key_BLANK2){
				if(Lir.strfunc_hand < N_STR){
					if(Lir.strfunc_hand == 0){
						key_press_for_A_O = _key_BLANK1;
						key_press_for_A_O1 = _key_BLANK1;
					}else if(Lir.strfunc_hand == 1){
						key_press_for_A_O =_key_BLANK2;
						key_press_for_A_O1 =_key_BLANK2;
					}else if(Lir.strfunc_hand == 2){
						key_press_for_A_O = _key_BLANK3;
						key_press_for_A_O1 = _key_BLANK3;
					}else if(Lir.strfunc_hand == 3){
						key_press_for_A_O = _key_BLANK4;
						key_press_for_A_O1 = _key_BLANK4;
					}
					submode_change_a_or_r_inside_HAND(Lir.strfunc_hand);
					}
			}
			else if(key == _key_BLANK3){
				if(Lir.strfunc_hand < N_STR)
					submode_set_to_zero_inside_HAND(Lir.strfunc_hand);
			}
			else if(key == _key_BLANK4){
				if(Lir.strfunc_hand < N_STR)
					submode_to_see_half_or_full_inside_HAND(Lir.strfunc_hand);
			}
		//#endif
		break;

		case SUBMODE_HAND::OFFSET:
			break;
		case SUBMODE_HAND::ABS_OR_REL:
			break;
		case SUBMODE_HAND::TO_ZERO:
			break;
		case SUBMODE_HAND::TO_SEE_HALF_OR_FULL:
			break;
		case SUBMODE_HAND::TO_DIVIDE_CIRCUIT:
		case SUBMODE_HAND::TO_SEE_DIFFERENT_READOUT:
		case SUBMODE_HAND::TOOL:
			break;
	}
}
void TMsgKey::submode_change_offset_inside_HAND(void){
	switch (offset_number){
		case 0: display.print_flash_local(PSTR("G53"),0,36,40);break;
		case 1: display.print_flash_local(PSTR("G54"),0,36,40);break;
		case 2: display.print_flash_local(PSTR("G55"),0,36,40);break;
		case 3: display.print_flash_local(PSTR("G56"),0,36,40);break;
		case 4: display.print_flash_local(PSTR("G57"),0,36,40);break;
		case 5: display.print_flash_local(PSTR("G58"),0,36,40);break;
		default:offset_number=0; display.print_flash_local(PSTR("G53"),0,36,40);break;
	}
}
void TMsgKey::clear_G5X(void){
	display.print_flash_local(PSTR("   "),0,36,40);
}
void TMsgKey::submode_change_a_or_r_inside_HAND(unsigned char s){
	if(MsgKey.does_string_has_button[s] && display.position_to_see[s] == CDisp::COORDINATE)
		MsgKey.go_to_abs_rel_common(s);	
}
void TMsgKey::submode_set_to_zero_inside_HAND(unsigned char s){
	if(MsgKey.does_string_has_button[s] && display.position_to_see[s] == CDisp::COORDINATE)
       to_zero_common_inside_HAND(s);
}
void TMsgKey::submode_to_see_half_or_full_inside_HAND(unsigned char s){
	if(MsgKey.does_string_has_button[s] && display.position_to_see[s] == CDisp::COORDINATE)
	    to_make_half_common_inside_HAND(s);
	
}
/*
функция для определения абсолютная/относительная ось и отношения номера строки к каналу
и последующего деления или умножения относительной оси
in unsigned char n_string - номер строки
*/
void TMsgKey::to_make_half_common_inside_HAND(unsigned char n_string){
	unsigned char n_ch,place = 0;
		if(to_take_channel_number(Lir.Str[n_string],n_ch))
			return;
	
		if (n_string == 0)
			place = Y_ONE;
		else if (n_string == 1)
			place = Y_TWO;
		else if (n_string == 2)
			place = Y_THREE;
		else if (n_string == 3)
			place = Y_FOUR;

		place = Y_FOUR;//
#if (configUSE_NCshell == 1)
		place = Y_FOUR;//
#endif
		if(init.mRef_ext[n_ch] == 0)
			divide_or_mult(n_ch,place);
}

void TMsgKey::go_out_from_ref_mode_in_begin_mode(void)
{

    if(MsgKey.old_mode == TMsgKey::MODE::HAND){
	   MsgKey.old_mode = TMsgKey::MODE::FIND_REF;
	   MsgKey.mode = TMsgKey::MODE::HAND;
	   if(MsgKey.tracer_real == TMsgKey::FUNC)
			to_put_icons_in_mode_HAND(MsgKey.tracer_real);
	   else
			to_put_icons_in_mode_HAND(SUBMODE_HAND::ABS_OR_REL);
		}
		else{
			MsgKey.old_mode = TMsgKey::MODE::FIND_REF;
			MsgKey.mode = TMsgKey::MODE::BEGIN;
			to_put_icons_in_begin_mode();
			} 

	for(unsigned char n=0;n<N_AXIS;n++)
			display.go_to_blink_string[n]=false;

	MsgKey.all_ref_interrupts_must_be_disabled=true;

	MsgKey.follower_ref=TMsgKey::PURE_REF;
	MsgKey.to_stop_cycle_measurement();
	MsgKey.to_stop_cycle_ref_ref();
}

void TMsgKey::read_counters_altera()
{
	#if (configUSE_mcx514 == 1)
		return;
	#endif

	#if (configUSE_ABS_Encoder == 1)
		if(1)
	#else
		if(0)
	#endif
	{
		PORTE.OUT=CIOAltera::WRITE_ADR::LATCH_CNT;
		altera.latch = true;
	}
		else
			altera.latch = false;
	//init.set_B6(); 
	if(Lir.P2[0] != no_axis)
		MsgKey.to_take_datas_from_channel_0();
	//delay_mks(20);
	if(Lir.P2[1] != no_axis)
		MsgKey.to_take_datas_from_channel_1();
	//delay_mks(20);
	if(Lir.P2[2] != no_axis)
		MsgKey.to_take_datas_from_channel_2();
	//delay_mks(20);
	if(Lir.P2[3] != no_axis)
		MsgKey.to_take_datas_from_channel_3();
	//init.clr_B6();
	//==считывание ошибок датчика
	#if (configUSE_Alt_ENCerr  == 1)
	er = altera.read_reg((CIOAltera::READ_ADR)31); 
	#endif
	//==
}

void TMsgKey::read_counters_nova()
{
#if (configUSE_mcx514 == 1)
	//====160mks
	for(int c = 0; c < N_AXIS;c++){
		if(Lir.P2[c] == no_axis)
			continue;
		if(flash_prg._OUTAxis[c].typeZ == CPrg::IMPULSE){
			if(flash_prg.axis_cfg_nc[c].datchik == CPrg::DATCHIK::ENCODER || flash_prg.axis_cfg_nc[c].datchik == CPrg::DATCHIK::HANDWEEL){
				GetRp(Lir.plb2(c), &altera.counter[c]);				
				if(flash_prg.axis_cfg[c].direction == CPrg::DIRECTION::MINES)
					altera.counter[c] = -altera.counter[c];
			}
			GetLp(Lir.plb2(c), &Lir.LP_axis[c]);
		}else if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET || flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG){
			GetRp(Lir.plb2(c), &altera.counter[c]);
			}
	}
	//=======	
#endif		
}

void TMsgKey::read_counters_mcx514_logic()
{
#if (configUSE_mcx514 == 1)
	//=======130 mks
	for(int c = 0; c < N_AXIS;c++){
		if(Lir.P2[c] == no_axis)
			continue;
		if(flash_prg._OUTAxis[c].typeZ == CPrg::IMPULSE)
			pult_counters.read_cnt(&Lir.LP_axis[c],c);
		else if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET || flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
			pult_counters.read_cnt(&altera.counter[c],c);

		if(init.mRef_ext[c])
			screen.channel_counter[c] = mCounter[c].abs;
		else
			screen.channel_counter[c] = mCounter[c].rel;
	}
#endif	
}

void TMsgKey::init_mx514(void)
{
#if (configUSE_mcx514 == 1)
	PORTD.DIR=0xBF;
	PORTD.OUT=0x00;
	PORTR.DIR=0x03;
	PORTR.OUT=0x03;
	PORTE.DIR=0xFF;   //PORTE output bus
	PORTE.OUT=0x00;
	
	//_delay_ms(350); //MX514 reset time
	
	SetRp(MCX514_AXIS_X, 0x00);  //Set Real Position X=0
	SetRp(MCX514_AXIS_Y, 0x00);
	SetRp(MCX514_AXIS_Z, 0x00);
	SetRp(MCX514_AXIS_U, 0x00);

	SetLp(MCX514_AXIS_X, 0x00);  //Set Logical Position X=0
	SetLp(MCX514_AXIS_Y, 0x00);
	SetLp(MCX514_AXIS_Z, 0x00);
	SetLp(MCX514_AXIS_U, 0x00);


	// Setup Inputs Outputs
	SetModePIO1(MCX514_AXIS_X, 0x00);       //XPIO 1-7 inputs (page 202 manual)
	SetModePIO1(MCX514_AXIS_Y, 0x00);       //YPIO 1-7 inputs
	SetModePIO1(MCX514_AXIS_Z, 0x5555);     //ZPIO 1-7 outputs
	SetModePIO1(MCX514_AXIS_U, 0x5555);     //UPIO 1-7 outputs
	WriteReg5(0xAAAA);                      //Set outputs 1010 1010 1010 1010
	
	for(int c = 0; c < N_AXIS; c++){
		Lir.Set_Axis_Conf_mcx514(c);		
	}

	//Lir._modals_cmd.F = 10000000;
 /*  PORTD.OUT = 0xF0;
   PORTD.OUT = 0x01;

   PORTD.OUT = 0x80;
   PORTD.OUT |= 0x27; //установка битов
   PORTD.OUT &= ~0x27;//сброс битов
   PORTD.OUT |= 0x22;

    PORTD.OUT = 0x80;
    PORTD.OUTSET = 0x27; //установка битов
    PORTD.OUTCLR = 0x27;// сброс битов
    PORTD.OUTSET = 0x22;*/

  // PORTD.OUTCLR = ~0x01;
   /*#ifdef __AVR_ATxmega256A3U__
   display.print_flash_local(PSTR("загружен драйвер xmega256A3U"),0,5,0);
   #endif
   #ifdef __AVR_ATxmega384C3__
   display.print_flash_local(PSTR("загружен драйвер xmega384C3"),0,5,0);
   #endif
   display.print_flash_local(PSTR("загружен драйвер mcx514"),0,5,16);
   delay_ms(1000);   */
#endif
}



void TMsgKey::key_BLANK_ref_mode(const uint8_t & str)
{
	int ch = N_AXIS;
	ch = screen.to_take_ch_number(Lir.Str[str]);
	screen.to_clear_string_connectors(4);
	if (ch >= N_AXIS)
		return;
	else if(Lir.abs_ENC[ch] == 1)
		return;
	else if(display.position_to_see[str]==CDisp::SPEED)
		return;
	else if(display.position_to_see[str]==CDisp::RESULT)
		return;
	else if(!does_string_has_button[str])
		return;
	else if(flash_prg.axis_cfg[ch].is_coded_ref == CPrg::YESM){			
				to_count_pulses_from_ref_to_ref(str);
		}
	else {
			if(mode_ref_submode_ref_ref == 0)
				to_go_to_REF_from_key_press((CInit::AXIS_NAME)Lir.Str[str],str);
		}
}

void TMsgKey::stop_REF(const uint8_t & str)
{
	int ch = screen.to_take_ch_number(Lir.Str[str]);
	if(ch == N_AXIS)
		return;
	cli();
	switch(ch){
		case 0:
			ref_.disable_PB5_int_0();		
			enable_int[0]=false;
			break;
		case 1:
			altera.set_flags(0x00);
			ref_.disable_PA0_int_1();
			enable_int[1]=false;
			break;
		case 2:
			ref_.disable_PA1_int_2();
			enable_int[2]=false;
			break;
		case 3:
			ref_.disable_PB3_int_3();
			enable_int[3]=false;
			break;
		default:
			break;
	}
	sei();

	for(int i_str = 0,y_pos = 0; i_str< N_STR; i_str++,y_pos += 7){
		if(Lir.Str[str] == Lir.Str[i_str] && display.position_to_see[i_str] != CDisp::SPEED && display.position_to_see[i_str] != CDisp::RESULT)
			display.put_button_(X_BUTTON_PLACE,y_pos,_pic_REF);
	}
}

//27.11.2018
/*
функция для записи абсолютной координаты в выбранную координату G54,G55,G56,G57,G58
по нажатию на кнопки BLANK1...
*/
void TMsgKey::to_zero_common_inside_HAND(unsigned char n_string){
	unsigned char n_ch;
	CDouble::MULT_OR_DIV_ANSWER answer;	
	long counter;

		if(go_to_zero_common(n_string)) 
			return;
		if(to_take_channel_number(Lir.Str[n_string],n_ch))
			return;
//if A  it will be here
		cli();
		counter = screen.channel_counter[n_ch];
		//counter = mCounter[n_ch].abs_pure;// без коррекции
		sei();
         answer = coeff.mult(n_ch,&counter);
         if (answer!= CDouble::OK) return;
		//if -		

		if(init.mRef_ext[n_ch]){
			if(Lir.is_corr(n_ch) && flash_prg.axis_cfg[n_ch].is_coded_ref == CPrg::YESM){
				if(MsgKey.offset_number > 0){					
					counter -= display.offset_g5x[MsgKey.offset_number - 1][n_ch];//чтобы смещения не  суммировалось
					display.offset_g5x[MsgKey.offset_number - 1][n_ch] += counter;
					}
			}else{
				if(flash_prg.axis_cfg[n_ch].is_coded_ref == CPrg::YESM)
					counter += Lir.size_2r2[n_ch];
				counter -= display.offset_g53[n_ch];
				if(flash_prg.axis_cfg[n_ch].type == CPrg::TYPE::CIRCLE)
					flash_prg.to_check_angle_axis_inside_HAND(&counter,n_ch,1);

				if(MsgKey.offset_number > 0)
					display.offset_g5x[MsgKey.offset_number - 1][n_ch] = counter;
				}	
				
		flash_prg.write_eeprom(92,sizeof(display.offset_g5x),&display.offset_g5x);					
		}			
}


void TMsgKey::to_measure_distance_from_ref_to_switch(unsigned char n_string){
	// CInit::AXIS_NAME symbol;
	 unsigned char n;
	 if(!mode_ref_submode_measure) {
		for(n=0;n<N_AXIS;n++)
			screen.measure_string[n]=false;
		}
	 switch(n_string){
		 case 0: 
		 //symbol=(CInit::AXIS_NAME)Lir.Str[0];
		 if(!mode_ref_submode_measure)
			screen.measure_string[0]=true;
		 break;	
		 case 1: 
		 //symbol=(CInit::AXIS_NAME)Lir.Str[1];
		 if(!mode_ref_submode_measure)
			screen.measure_string[1]=true;
		 break;
		 case 2: 
		 //symbol=(CInit::AXIS_NAME)Lir.Str[2];
		 if(!mode_ref_submode_measure)
			screen.measure_string[2]=true;
		 break;	
		 case 3: 
		 //symbol=(CInit::AXIS_NAME)Lir.Str[3];
		 if(!mode_ref_submode_measure)
			screen.measure_string[3]=true;
		 break;
		 default:
		 return;
	 }
	 unsigned char n_ch; 
	 if(to_take_channel_number(Lir.P2[n_string],n_ch))
			return;
	 ref_.counter_measure[n_string] = 0; // обнуление счетчика вход - реф
	 is_it_switch_for_ref[n_ch] = is_it_switch_or_no_for_ref(&n_ch); // получаем номер входа и записывает TRUE или FALSE в is_it_switch_for_ref

	 if(!mode_ref_submode_measure){
		 for(n=0;n<N_AXIS;n++)
			screen.measure_chan[n]=false;	
			 
		if(n_ch < N_AXIS){
			enable_measure[n_ch]=true;			
			screen.measure_chan[n_ch]=true;
			Lir.trig_input[n_ch] = 0;
			mode_ref_submode_measure=true;
		}
		else{
			for(n=0;n<N_AXIS;n++)
				enable_measure[n]=false;
			}			
		}
		else
			stop_ZM(n_string);		

}
/*
функция вызывается по завершению измерения от концевика до рефметки
*/
void TMsgKey::to_stop_and_to_see_measurement_result(void){
	unsigned char n;
	for(n=0;n<N_AXIS;n++){
		if(screen.measure_chan[n])
			break;
		}   
	if(n<N_AXIS){
		screen.counter_measure=ref_.counter_measure[n];	
		screen.to_display_mode_ref_submode_measure(false,true);
		for(int i = 0 ; i < N_AXIS;i++){
			if(screen.measure_string[i]){
				screen.measure_string[i] = false;
				break;
				}
			}
		} 
		else 
			screen.to_display_mode_ref_submode_measure(false,false);
				 
	all_ref_interrupts_must_be_disabled=true;
	for(unsigned char n=0;n<N_AXIS;n++){
		screen.measure_chan[n]=false;
	}
	mode_ref_submode_measure=false;		
}
/*
функция для запуска режима измерения от реф метки до реф метки
unsigned char s - номер строки (для запуска из  меню  "РЕФ")
*/
void TMsgKey::to_count_pulses_from_ref_to_ref(unsigned char s){
	// CInit::AXIS_NAME symbol;
	 unsigned char c;
	  if(mode_ref_submode_ref_ref == 0) {
		 for(c=0;c<N_AXIS;c++)
			screen.measure_string[c]=false;
	  }
	  
	   unsigned char n_ch;
	   if (Lir.curItem->id == idMenuEncoders){
		   if(to_take_channel_number(Lir.P2[s],n_ch))
		   return;
	   }
	   else
	   {
		   if(to_take_channel_number(Lir.Str[s],n_ch))
		   return;
	   }
	
	 if(Lir.ref_mode[0] || Lir.ref_mode[1] || Lir.ref_mode[2] || Lir.ref_mode[3]){	
		stop_PM(s);
		display.ref_change_mode[n_ch] = CDisp::NOTHING; 
		return;
		}
		
	 if(s >= 0 && s <= 3){
		 if(mode_ref_submode_ref_ref == 0)
			screen.measure_string[s] = true;
	 }
	 else 
		 return;		  
	  
	  if(mode_ref_submode_ref_ref == 0){
		 for(c=0;c<N_AXIS;c++)
			screen.measure_chan[c] = false;	   			  
		enable_ref_ref[n_ch] = true;
		mode_ref_submode_ref_ref = true;
		screen.measure_chan[n_ch] = true;//break;
		 if (Lir.curItem->id == idMenuEncoders)
			display.ref_change_mode[n_ch] = CDisp::NOTHING;
		 else{
			//==показ зоны если есть
			for(int s_ = 0,j = 2 ; s_ < N_STR; s_++,j+=7){
				if(Lir.Str[s] == Lir.Str[s_] && display.position_to_see[s_] != CDisp::SPEED && display.position_to_see[s_] != CDisp::RESULT)
				screen.switches_to_see(&n_ch,j,true);
			}
			//==
			//==записать зону в переменную для выдачи REF ENABLE только в зоне
			is_it_switch_for_ref[n_ch]=is_it_switch_or_no_for_ref(&n_ch);
			//==
			//if(display.ref_change_mode[n_ch] == CDisp::NOTHING || display.ref_change_mode[n_ch] == CDisp::REF_NOT_FOUND)
			display.ref_change_mode[n_ch] = CDisp::SEARCH_REF_findref; // установка режима поиска
		 }
	  }
	  else{
		  stop_PM(s);	
		  display.ref_change_mode[n_ch] = CDisp::NOTHING; 
	  }
}
/*
функция вызываемая по нажатию кнопки 'F' для переключения подрежимов a/o , ->0 в стартовом режиме
*/
void TMsgKey::to_look_for_submode_in_begin_mode(void){
	switch(follower_begin){
		case SUBMODE_BEGIN::SET_TO_ZERO:
			follower_begin=SUBMODE_BEGIN::A_O;	
			to_put_icons_in_begin_mode();			
			break;	
		case SUBMODE_BEGIN::A_O:
			follower_begin=SUBMODE_BEGIN::SET_TO_ZERO;
			to_put_icons_in_begin_mode();				
			break;			
	}
}
/*
функция для отображения/переключения иконок кнопок _key_BLANK1...4 в begin Mode
*/
bool TMsgKey::to_put_icons_in_begin_mode(void){
 CInit::AXIS_NAME symbol;	
 common_to_put_icons_information();
 unsigned char to_count_channels=0;
 unsigned char n_pic;

	 if(follower_begin==SUBMODE_BEGIN::SET_TO_ZERO) 
		n_pic=_pic_TO_ZERO;
	 else if(follower_begin==SUBMODE_BEGIN::A_O)
		n_pic=_pic_ABS_REL;
	 else{
		follower_begin=SUBMODE_BEGIN::SET_TO_ZERO;
		n_pic = _pic_TO_ZERO;
		}

	 for(unsigned char n=0;n<N_STR;n++)
		does_string_has_button[n]=false;
	 for(unsigned char n=0;n<N_STR;n++){
		 	symbol=string[n];
		 	unsigned char m; 
			if(to_take_channel_number(symbol,m))
			 continue;
			 	if(screen.does_it_live[m]){
					 if(display.position_to_see[n]!= CDisp::SPEED && display.position_to_see[n]!= CDisp::RESULT)
						to_count_channels++;
					 common_for_to_put_buttons(n,n_pic);
		 		}
	 	    }
	 
	if(to_count_channels==0)
		return false;
	return true;	 
}

/*
функция для переключения подрежимов в режиме 'Выхода в реф' по кнопке 'F'
*/
void TMsgKey::to_look_for_submode_in_ref_mode(void){

	//screen.to_clear_place_for_virtual_buttons(4);
	//screen.to_see_connectors_and_switches_in_ref_mode(true);
	int ch = N_AXIS;
	for(int i = 0 ; i < N_STR;i++){
		ch = screen.to_take_ch_number(Lir.Str[i]);
		if(ch != N_AXIS){
			if(follower_ref == SUBMODE_REF::PURE_REF){
				if(display.position_to_see[i] != CDisp::SPEED && display.position_to_see[i] != CDisp::RESULT){
					if(flash_prg.axis_cfg[ch].is_coded_ref == CPrg::YESM)
						common_for_to_put_buttons(i,_pic_REF_REF);
					else
						common_for_to_put_buttons(i,_pic_REF);
				}
			}
			//else if  другой мод
		}
	}
}

bool TMsgKey::to_put_icons_in_ref_mode(void){	
	return true;
}
/*
функция для переключения подрежимов в режиме 'Ручной' по кнопке 'F'
*/
void TMsgKey::to_look_for_submode_of_operation_inside_HAND(void){	

	tracer_real=tracer_next;

	switch (tracer_next){
		case SUBMODE_HAND::FUNC:
			to_put_icons_in_mode_HAND(tracer_next);
		#if (configUSE_NCshell == 1)
			tracer_next = SUBMODE_HAND::DRIVE_CONTROL;
		#endif
			break;
		case SUBMODE_HAND::CMD_CNTRL:
			to_put_icons_in_mode_HAND(tracer_next);
			break;
		case SUBMODE_HAND::DRIVE_CONTROL:	
			to_put_icons_in_mode_HAND(tracer_next);
			tracer_next = SUBMODE_HAND::FUNC;
			break;
		case SUBMODE_HAND::OFFSET:
			to_put_icons_in_mode_HAND(tracer_next);
			tracer_next = SUBMODE_HAND::ABS_OR_REL;
			//tracer_next=SUBMODE_HAND::TO_SEE_HALF_OR_FULL;
			break;
		case SUBMODE_HAND::ABS_OR_REL:
			if(to_put_icons_in_mode_HAND(tracer_next)){
				tracer_next = SUBMODE_HAND::TO_ZERO;
				break;
			}
		case SUBMODE_HAND::TO_ZERO:
			if(to_put_icons_in_mode_HAND(tracer_next)){
				tracer_next = SUBMODE_HAND::TO_SEE_HALF_OR_FULL;
				break;
			}
		case SUBMODE_HAND::TO_SEE_HALF_OR_FULL:
			if(to_put_icons_in_mode_HAND(tracer_next)){
					tracer_next = SUBMODE_HAND::OFFSET;
				#if (configUSE_NCshell == 1)
					tracer_next = SUBMODE_HAND::DRIVE_CONTROL;
				#endif
				break;
			}
			tracer_next=SUBMODE_HAND::OFFSET;
			#if (configUSE_NCshell == 1)
			tracer_next=SUBMODE_HAND::DRIVE_CONTROL;
			#endif
			to_look_for_submode_of_operation_inside_HAND();	
			break;
		case SUBMODE_HAND::TO_DIVIDE_CIRCUIT:
		case SUBMODE_HAND::TO_SEE_DIFFERENT_READOUT:
		case SUBMODE_HAND::TOOL:
		break;
	}

}
/*
Функция рисует и заполняет иконки напротив кнопок BLANK_1 BLANK_2 BLANK_3 BLANK_4
в ручном режиме (HAND MODE)
*/
bool TMsgKey::to_put_icons_in_mode_HAND(SUBMODE_HAND submode){
	CInit::AXIS_NAME symbol;
	unsigned char n;

    common_to_put_icons_information();

	switch (submode){
		case SUBMODE_HAND::FUNC:		
			//добавить отображение иконок a/o ,->0
			display.put_button_(X_BUTTON_PLACE, Y_ONE,_pic_OFFSET);
			does_string_has_button[0] = true;
			common_for_to_put_buttons(1,_pic_ABS_REL);
			common_for_to_put_buttons(2,_pic_TO_ZERO);
		#if (configUSE_NCshell == 1)
			//очистить квадрат если он есть
			if (Lir._resz_prg_v.SZ){//проверка если на экране 4ыре строки
				for (int y = 11; y < 15; y++)
					display.print_symbols_local(' ', 10, 0, 30, y * 16);
				}
		#endif	
			common_for_to_put_buttons(3,_pic_FULL);						
		break;
		case SUBMODE_HAND::CMD_CNTRL:
		#if (configUSE_NCshell == 1)
		Lir.show_func_cmd_3btn(0);
		#endif
		break;
		case SUBMODE_HAND::DRIVE_CONTROL:
			display.put_button_(X_BUTTON_PLACE,Y_ONE);
			display.put_button_pic_(X_BUTTON_PLACE,Y_ONE,_pic_AUTO);
			display.put_button_(X_BUTTON_PLACE,Y_TWO);
			display.put_button_pic_(X_BUTTON_PLACE,Y_TWO,_pic_PRED);
			display.put_button_(X_BUTTON_PLACE,Y_THREE);
			display.put_button_pic_(X_BUTTON_PLACE,Y_THREE,_pic_PROG);
			Lir.entry_hand = Menu::INIT;
			return true;

		case SUBMODE_HAND::OFFSET:	
			display.put_button_(X_BUTTON_PLACE,Y_ONE);
			display.put_button_pic_(X_BUTTON_PLACE,Y_ONE,_pic_OFFSET);	
			return true;

		case SUBMODE_HAND::ABS_OR_REL:
			for (n=0;n<N_AXIS;n++){
				symbol=string[n];
				if(symbol<=CInit::AXIS_NAME::AXIS_D)
					common_for_to_put_buttons(n,_pic_ABS_REL);
				}
			for (n=0;n<N_AXIS;n++){
				if(does_string_has_button[n]==true)
					break;
				}
			if(n<N_AXIS) 
				return true;
			else 
				return false;
	
		case SUBMODE_HAND::TO_ZERO:
			for (n=0;n<N_AXIS;n++){
				symbol=string[n];
				if(symbol<=CInit::AXIS_NAME::AXIS_D)
					common_for_to_put_buttons(n,_pic_TO_ZERO);
				}
			for (n=0;n<N_AXIS;n++){
				if(does_string_has_button[n]==true)
				break;
				}
			if(n<N_AXIS) 
				return true;
			else 
				return false;

		case SUBMODE_HAND::TO_SEE_HALF_OR_FULL:
			for (n=0;n<N_STR;n++){
				symbol=string[n];
				if(symbol<CInit::AXIS_NAME::AXIS_A){
					int8_t n_ch = Lir.get_channel(symbol);
					if(divide_by_2[n_ch])
						common_for_to_put_buttons(n,_pic_HALF);
					else
						common_for_to_put_buttons(n,_pic_FULL);
					}
				}
			for (n=0;n<N_AXIS;n++){
				if(does_string_has_button[n]==true)
				break;
				}
			if(n<N_AXIS) 
				return true;
			else 
				return false;

	case SUBMODE_HAND::TO_DIVIDE_CIRCUIT:
	case SUBMODE_HAND::TO_SEE_DIFFERENT_READOUT:
	case SUBMODE_HAND::TOOL:
	break;	
	
	}
return false;
}

void TMsgKey::key_handling()
{
			cli();
			char k_c=l_k_s.key_code;
			char l_k=l_k_s.lock_key;
			sei();
			//===========================================НАЖАТИЕ КНОПКИ====================================================
			if(l_k == PROCESSING_PRESS){
				MsgKey.key = k_c;
				MsgKey.apply();
				#if (configUSE_max7370 == 1)
				Lir.beep_on();				
				#endif
				cli();
				l_k_s.lock_key =FREE;
				l_k_s.key_code =0x00;
				k_c=0x00;
				l_k=0x00;
				sei();
			}
			//==========================================УДЕРЖАНИЕ КНОПКИ===================================================
			if(l_k == PROCESSING_HOLDDOWN){
				MsgKey.key = k_c;
				MsgKey.apply();
				if(k_c == _key_STOP){
					cli();
					init.first_condition_to_switch_off=true;
					sei();
				}
				cli();
				l_k_s.lock_key = FREE;
				l_k_s.key_code = 0x00;
				k_c = 0x00;
				l_k = 0x00;
				sei();
			}
			//==========================================ОТПУСКАНИЕ КНОПКИ===================================================
			if(l_k == PROCESSING_RELEASED){
				l_k_s.first_step=false;
				l_k_s.first_condition=false;
				if(k_c==_key_STOP){
					cli();
					init.first_condition_to_switch_off=false;
					if(init.time_metre>=100)
					init.to_clear_switch_off_string=true;
					init.time_metre=0;
					sei();
				}
				#if (configUSE_NCshell == 1)
				//if(k_c == _key_ENTER && l_k == PROCESSING_RELEASED)
				Lir.move_axis_mh_RELEASE();
				#endif
				cli();
				l_k_s.lock_key = FREE;
				l_k_s.key_code = 0x00;
				k_c = 0x00;
				l_k = 0x00;
				sei();
			}
}

/*
функция записывает в глобальную переменную switch_number_for_ref_[*n_ch] номер входа Зоны реф метки
in unsigned char *n_ch - номер канала
return bool - функция возвращает есть ли на канале зона метки TRUE or FALSE
*/
bool TMsgKey::is_it_switch_or_no_for_ref(unsigned char *n_ch){
	unsigned int n,n_inv;
    if(!(flash_prg.axis_cfg[*n_ch].ref_pulse_zone & YESNO_P_bm)){
		n=(unsigned int)flash_prg.axis_cfg[*n_ch].ref_pulse_zone & DATA_P_bm;	
		if(n>=16) 
			return false;	
		n_inv=flash_prg.axis_cfg[*n_ch].ref_pulse_zone & INVERSE_P_bm;	
		if(n_inv>1)
			return false;
		if(n_inv)
			n_inv=0x0000;
		else 
			n_inv=0x0080;
		switch_number_for_ref[*n_ch]=n|n_inv;
	}
	else 
		return false;
//0-P1.1,15-P1.16
	return true;
}





void TMsgKey::to_see_indikator_channel_P2_1(void){
	 	 if(Lir.curItem->id == idIndikator){
		 	 //channel P2.1
		 	 if(flash_prg.mode_graph==false){
			 	 cli();
			 	 long tmp=mCounter[1].rel;
			 	 sei();
			 	 coeff.mult(1,&tmp);
			 	 flash_prg.new_technical_task(tmp,1);
			 	 display.clear_rectangle(0,13,320,1,0x80,0);
		 	 }
	 	 }
	
}
void TMsgKey::to_see_indikator_channel_P2_0(void){
	 if(Lir.curItem->id == idIndikator){
		 //channel P2.0
		 if(flash_prg.mode_graph==false){
			 cli();
			 long tmp=mCounter[0].rel;
			 sei();
			 coeff.mult(0,&tmp);
			 flash_prg.new_technical_task(tmp,0);
			 display.clear_rectangle(0,6,320,1,0x80,0);
		 }
	 }	
	
}
void TMsgKey::to_see_indikator_dif_P2_0_P2_1(void){
#if (configUSE_menuItem_Correction_and_Izmerenia == 0)
return;
#endif

 if(Lir.curItem->id == idIndikator){
	 //diff P2.0 and P2.1
	 if(flash_prg.mode_graph==false){
		 cli();
		 long tmp0=mCounter[0].rel;
		 long tmp1=mCounter[1].rel;
		 sei();
		 coeff.mult(0,&tmp0);
		 coeff.mult(1,&tmp1);
		 tmp0 = tmp0 - tmp1;
		 flash_prg.new_technical_task(tmp0,2);
		 display.clear_rectangle(0,20,320,1,0x80,0);
	 }
 }		
}
void TMsgKey::to_see_MenuEncoders(void){
//09.10.2018 it can take up to 7 ms		

  if (Lir.curItem->id == idMenuEncoders)
	{
	long data;
	if(Lir.cur_ch_calc_2r2 < N_AXIS){
		Lir.sz_refref[Lir.cur_ch_calc_2r2] = screen.pulses_;
		//if(Lir.sz_refref[Lir.cur_ch_calc_2r2] < 3) // 
		//	Lir.sz_refref[Lir.cur_ch_calc_2r2] = 0;
	}

	Lir.print_long_format(ref_.ref_ref_count,CPrg::FORMAT::FORMAT_END,0,33,128,RIGHT_ALIGN,2,3);

	for(int  i = 0 ;  i < N_AXIS; i ++){
		if (Lir.P2[i] != no_axis){
			cli();
			altera.counter_display[i]=altera.counter[i];
			long tmp = display.speed_100[i];
			sei();
			tmp = (tmp*10)/4;  // получаем Гц

			if(screen.interval_for_speed[i]==60)
				tmp=tmp/6;
			else if(screen.interval_for_speed[i]==30)
				tmp=tmp/3;
			display.print_debug_one_local(tmp,0,11,16 *(i + 9)); //Частота(Гц)

			data = mCounter[i].rel;
			coeff.mult(i,&data);
			
			Lir.print_long_format(data,Lir.get_format_from_ch(i),0,11,16 *(i + 2),LEFT_ALIGN,0,11);			  //Индикация

			if(display.encoder_fault_[i]){//Счетчик Altera
				display.print_flash_local(sError,0,23,16 *(i + 2));
				display.print_symbols_local(' ',3,0,29,16 *(i + 2));
			}
			else
				display.print_debug_one_local_shot(altera.counter_display[i],0,23,16 *(i + 2),9);

			tmp = coeff.new_gap[i].binary;
			coeff.mult(i,&tmp);
			Lir.print_long_format(tmp,Lir.get_format_from_ch(i),0,32,16 *(i + 2),0,0,7);//Люфт
							 
			if(screen.measure_chan[i]){
				if(display.blinker&0x04)
					INVERT_CHAR()
				else
					NORMAL_CHAR()
			}
			Lir.print_long_format(Lir.sz_refref[i],CPrg::FORMAT::FORMAT_END,0,23,16 *(i + 9),LEFT_ALIGN,0,10); //Реф - Реф
			NORMAL_CHAR()
			}
		}
	}	
}

void TMsgKey::to_see_mInputs(void)
{
	/*if (Lir.curItem->id == idViewInputs)
	{

		int n = 15;
		display.print_debug_bit_int(init.p_in, Lir.bit_view,(unsigned char *)&n);
		Lir.print_long_format(Lir.bit_view + 1,CPrg::FORMAT_END,0,24,16 *2,RIGHT_ALIGN,1);
	
		long data;
		for(int  c = 0 ;  c <N_AXIS; c ++){
			if (Lir.P2[c] != no_axis)
			{
				data = mCounter[c].rel;
				coeff.mult(c,&data);
				if(screen.measure_chan[c]){
					if(display.blinker&0x04)
						INVERT_CHAR()
					else
						NORMAL_CHAR()
				}
				Lir.print_long_format(data,Lir.get_format_from_ch(c),0,11,16 * (c + 6),LEFT_ALIGN,0,11); //Индикация
				NORMAL_CHAR()
				if(!(flash_prg.axis_cfg[c].ref_pulse_zone & YESNO_P_bm )){
					data = ref_.counter_measure[c];
					if(data<0)
						data=-data;
					
					coeff.mult(c,&data);
					Lir.print_long_format(data,Lir.get_format_from_ch(c),0,23,16 * (c + 6),LEFT_ALIGN,0,11);
				}
				if(ref_.measurement_is_ended){
					//MsgKey.to_stop_and_to_see_measurement_result();
					all_ref_interrupts_must_be_disabled=true;
					if(screen.measure_chan[c])
						screen.measure_chan[c]=false;
					signed char s = Lir.get_str_from_ch(c);
					if(s != N_STR){
						screen.measure_string[s] = false;
					}
					mode_ref_submode_measure=false;
				}
			}
		}
	} */
}
void TMsgKey::to_see_mSupply(void)
{
//09.10.2018 it can take up to 0.85 ms	
	if (Lir.curItem->id == idSupplyandBatt)
	{
	// show_voltage_CR2032
	//unsigned char cur_str[15];
	//15.11.2017 2860 mv - 2047
	//           2750 mv - 1965
	//           2700 mv - 1930
	//           2500 mv - 1790
	//           2000 mv - 1430
	long a=(atdc.int_result_adc_ch0 * 310) / 2047;	//(atdc.int_result_adc_ch0*286)/2047;
	if(atdc.int_result_adc_ch0 > 700)
		Lir.print_long_format(a,CPrg::LINEAR_2,0,21,16,LEFT_ALIGN,0,15);
	else
		display.print_flash_local(PSTR("нет батарейки"),0,21,16);
	// show_voltage_supply
	a = atdc.int_result_adc_ch2*2580/2047; //atdc.int_result_adc_ch2*2880/2047;
	Lir.print_long_format(a,CPrg::LINEAR_2,0,21,32,LEFT_ALIGN,0,5);
	}
}

void TMsgKey::to_see_mTime(void)
{
//09.10.2018 it can take up to 1.18 ms		
	if (Lir.curItem->id == idTime)
	{
//show_current_time 
	 char time_seconds[9];
	 l_k_s.time_string_with_seconds(l_k_s.real_time_to_see,time_seconds);
	 display.gotoxy(16,16);
	 display.print_ram_local((char*)time_seconds,0,16,16);		
//show_weekday 
	unsigned char weekday = l_k_s.real_time_to_see[4]>>5;
		 switch(weekday){
			 case 0:display.print_flash_local(sSunday,0,16,32);break;
			 case 1:display.print_flash_local(sMonday,0,16,32);break;
			 case 2:display.print_flash_local(sTuesday,0,16,32);break;
			 case 3:display.print_flash_local(sWednesday,0,16,32);break;
			 case 4:display.print_flash_local(sThursday,0,16,32);break;
			 case 5:display.print_flash_local(sFriday,0,16,32);break;
			 case 6:display.print_flash_local(sSaturday,0,16,32);break;
			 default:
					display.print_flash_local(PSTR( "     ?     "),0,16,32);break;
		 }
//show_month
		char day_month[7];
		l_k_s.day_and_month_string(l_k_s.real_time_to_see,day_month);
		display.print_ram_local(day_month,0,16,48);
//show_year 
	//20.11.2017 00****** - leap year
	//           01****** - next
	//           10****** - next next
	//           11****** - next next next
	//l_k_s.year is saved into RAM. It is last leap year.
	//When leap year is coming,time must be refreshed from computer.
	int year=l_k_s.real_time_to_see[3]>>6;
	int tmp=l_k_s.year+year;
	if(l_k_s.year>=2016 && l_k_s.year<2041)
		display.print_debug_one_local_shot(tmp,0,16,64,4);
	else 
		display.print_flash_local(PSTR(" ?  "),0,16,64);
	}		
}

void TMsgKey::see_data_str(const uint8_t& str)
{
	char day_month[15] = {0};
	l_k_s.day_and_month_string(l_k_s.real_time_to_see,day_month);

	int year=l_k_s.real_time_to_see[3]>>6;
	long tmp=l_k_s.year+year;
	day_month[5] = '.';

	display.itoa_quick_lir(&tmp,(unsigned char*)&day_month[6]);

	display.print_ram_local(day_month,0,23,(str * 56) + 40);
}

void TMsgKey::to_see_graph(void){
	 if(Lir.curItem->id == idIndikator || Lir.curItem->id == idGraph){
		 //17.08.2018 it takes 2.840 ms for 10 -56789.329
		 //init.set_B6();
		 //third axes
		 cli();
		 long x=mCounter[0].rel;
		 long y=mCounter[3].rel;		 
		 sei();
		 CDouble::MULT_OR_DIV_ANSWER answer=coeff.mult(3,&y);
		 answer=coeff.mult(0,&x);
		 if (answer== CDouble::OK){
			 display.only_test_to_see_big_digit(y);
		 }
		 //init.clr_B6();
		 //init.set_B6();
		 if(flash_prg.mode_graph==true){
			 //22.08.2018 it takes 4500 mks
			 if(flash_prg.clear_place_for_graph==true){
				 display.clear_rectangle(0,0,320,21,0x00,0);
				 flash_prg.clear_place_for_graph=false;
			 }
			 //22.08.2018 it will be started from here(graph)
			 flash_prg.graph(y,x);
			 
		 }
		 else {
			 if(flash_prg.clear_place_for_graph==true){
				 display.clear_rectangle(0,0,320,21,0x00,0);
				 flash_prg.clear_place_for_graph=false;
			 }
		 }
		 //init.clr_B6();
		 if(flash_prg.sign_for_graph==false)
		 display.print_debug_one_local_shot(flash_prg.compare,0,0,8*24,6);
		 else display.print_debug_one_local_shot(-flash_prg.compare,0,0,8*24,6);
	 }
	 //init.set_B6();	
	
	
}

void TMsgKey::to_see_stack_and_go_next(void){

		if(Lir.curItem->id == idAbout){
			#if (configUSE_NCshell == 1)
				if(!Lir.DBG_)
					return;
			#endif
			#if (configUSE_Lang == 0)
			long is_used,is_left;
			
			display.print_flash_local(sStack_left,0,0,96);
			display.print_flash_local(sStack_used,0,0,112);
			is_left=display.current_stack_position - STACK_END;
			display.print_debug_one_local(is_left,0,15,96);
			is_used= 0x24ff - display.current_stack_position;
			display.print_debug_one_local(is_used,0,19,112);
			Lir.sn_win = 0;
			#endif
		}	
	
}

void TMsgKey::_key_MODE_HAND_f()
{
	tracer_real = SUBMODE_HAND::FUNC;
	#if (configUSE_NCshell == 1)
	//Lir.show_freq(0,Lir.get_channel(Lir.Str[0]),Lir.gl_F,0);
	tracer_real = SUBMODE_HAND::DRIVE_CONTROL;
	#endif
	
	MsgKey.SET_M(TMsgKey::MODE::HAND);
	if(Lir.gotoModeHand())
		Lir.init_hand(0);
	screen.to_clear_place_for_virtual_buttons(4);
	//tracer_next=SUBMODE_HAND::OFFSET;
	tracer_next=tracer_real;
	//offset_number=0;
	to_look_for_submode_of_operation_inside_HAND();
	submode_change_offset_inside_HAND();
	//------------------------------------
}

void TMsgKey::all_ref_interrupts_will_be_disabled_and_var_set_to_starting_state(void){
 cli();
 //USART_DEBUG(0);
 ref_.disable_PB5_int_0();
 altera.set_flags(0x00);//сброс флагов ref to ref
 ref_.disable_PA0_int_1();
 ref_.disable_PA1_int_2();
 ref_.disable_PB3_int_3();
 sei();
 MsgKey.all_ref_interrupts_must_be_disabled=false;
 MsgKey.enable_int[0]=false;
 MsgKey.enable_int[1]=false;
 MsgKey.enable_int[2]=false;
 MsgKey.enable_int[3]=false;

 for(unsigned char n=0;n<N_AXIS;n++){
	ref_.enable_measure_int[n]=false;
	Lir.ref_mode[n] = 0;
	}

 ref_.measurement_is_ended=false;
 ref_.ref_ref_enable=false;
 ref_.ref_ref_count=0;	
}
/*
инициализация реф метки канала 0
*/
void TMsgKey::starting_line_for_PB5_int_0(void){
	 if(MsgKey.enable_int[0]){
		 cli();
		 ref_.init_PB5_int_ref_0(true);
		 ref_.enable_PB5_int_0(MsgKey.is_it_switch_for_ref[0]);
		 ref_.enable_measure_int[0]=false;
		 Lir.ref_mode[0] = SIMPLE_RM;
		 sei();
		 MsgKey.enable_int[0]=false;
	 }
	 else if(MsgKey.enable_measure[0]){
		 cli();
		 ref_.init_PB5_int_ref_0(false);
		 ref_.enable_PB5_int_0(true);
		 ref_.enable_measure_int[0]=true;
		 ref_.ref_mark_measure[0]=false;
		 Lir.ref_mode[0] = DISTANCE_TO_SWITCH_RM;
		 sei();
		 MsgKey.enable_measure[0]=false;
	 }
	 else if(MsgKey.enable_ref_ref[0]){
		 cli();
		 altera.set_flags(0x04);
		 ref_.init_PB5_int_ref_0(false);
		 ref_.enable_PB5_int_0(MsgKey.is_it_switch_for_ref[0]);
		 ref_.ref_ref_count = 0;
		 screen.counter_ref_ = 0;
		 ref_.sz_two_ref = 0;
		 ref_.ref_ref_enable = true;
		 Lir.ref_mode[0] = SIMPLE_RM;
		 sei();
		 MsgKey.enable_ref_ref[0] = false;
	 }		
}
/*
инициализация реф метки канала 1
*/
void TMsgKey::starting_line_for_PA0_int_1(void){
	if(MsgKey.enable_int[1]){ // если нажимаем поиск реф метки
		cli();
		ref_.init_PA0_int_ref_1(true);
		ref_.enable_PA0_int_1(MsgKey.is_it_switch_for_ref[1]);
		ref_.enable_measure_int[1]=false;
		sei();
		MsgKey.enable_int[1]=false;
		Lir.ref_mode[1] = SIMPLE_RM;
	}
	//1.11.2018  // если нажимаем 'ЗМ'
	else if(MsgKey.enable_measure[1]){
		cli();
		ref_.init_PA0_int_ref_1(false);
		ref_.enable_PA0_int_1(false);
		ref_.enable_measure_int[1]=true;
		ref_.ref_mark_measure[1]=false;
		sei();
		MsgKey.enable_measure[1]=false;
		Lir.ref_mode[1] = DISTANCE_TO_SWITCH_RM;
	}	
	else if(MsgKey.enable_ref_ref[1]){
		cli();
		altera.set_flags(0x08); // добавить адрес
		ref_.init_PA0_int_ref_1(false);
		ref_.enable_PA0_int_1(MsgKey.is_it_switch_for_ref[1]);
		ref_.ref_ref_count = 0;
		screen.counter_ref_ = 0;
		ref_.sz_two_ref = 0;
		ref_.ref_ref_enable=true;
		Lir.ref_mode[1] = SIMPLE_RM;
		sei();
		MsgKey.enable_ref_ref[1]=false;
	}	
}
/*
инициализация реф метки канала 2
*/
void TMsgKey::starting_line_for_PA1_int_2(void){
	if(MsgKey.enable_int[2]){
		cli();
		ref_.init_PA1_int_ref_2(true);
		ref_.enable_PA1_int_2(MsgKey.is_it_switch_for_ref[2]);
		ref_.enable_measure_int[2]=false;
		sei();
		MsgKey.enable_int[2] =false;
		Lir.ref_mode[2] = SIMPLE_RM;
	}
	//2.11.2018
	else if(MsgKey.enable_measure[2]){
		cli();
		ref_.init_PA1_int_ref_2(false);
		ref_.enable_PA1_int_2(false);
		ref_.enable_measure_int[2]=true;
		ref_.ref_mark_measure[2]=false;
		sei();
		MsgKey.enable_measure[2]=false;
		Lir.ref_mode[2] = DISTANCE_TO_SWITCH_RM;
	}
	else if(MsgKey.enable_ref_ref[2]){
		cli();
		altera.set_flags(0x10); // добавить адрес
		ref_.init_PA1_int_ref_2(false);
		ref_.enable_PA1_int_2(MsgKey.is_it_switch_for_ref[2]);
		ref_.ref_ref_count = 0;
		screen.counter_ref_ = 0;
		ref_.sz_two_ref = 0;
		ref_.ref_ref_enable=true;
		Lir.ref_mode[2] = SIMPLE_RM;
		sei();
		MsgKey.enable_ref_ref[2]=false;
	}		
} 
/*
инициализация реф метки канала 3
*/
void TMsgKey::starting_line_for_PB3_int_3(void){
	if(MsgKey.enable_int[3]){
		cli();
		ref_.init_PB3_int_ref_3(true);
		ref_.enable_PB3_int_3(MsgKey.is_it_switch_for_ref[3]);
		ref_.enable_measure_int[3]=false;
		sei();
		MsgKey.enable_int[3]=false;
		Lir.ref_mode[3] = SIMPLE_RM;
	}
	else if(MsgKey.enable_measure[3]){
		cli();
		ref_.init_PB3_int_ref_3(false);
		ref_.enable_PB3_int_3(false);
		ref_.enable_measure_int[3]=true;
		ref_.ref_mark_measure[3]=false;
		sei();
		MsgKey.enable_measure[3]=false;
		Lir.ref_mode[3] = DISTANCE_TO_SWITCH_RM;
	}	
	else if(MsgKey.enable_ref_ref[3]){
		cli();
		altera.set_flags(0x20);
		ref_.init_PB3_int_ref_3(false);
		ref_.enable_PB3_int_3(MsgKey.is_it_switch_for_ref[3]);
		ref_.ref_ref_count = 0;
		screen.counter_ref_ = 0;
		ref_.sz_two_ref = 0;
		ref_.ref_ref_enable=true;
		Lir.ref_mode[3] = SIMPLE_RM;
		sei();
		MsgKey.enable_ref_ref[3]=false;
	}
}

void TMsgKey::correction_init()
{
		for(unsigned char n=0;n<N_AXIS;n++){
			altera.correction_can_be_done_in_real_time_[n] = altera.correction_can_be_done_in_real_time[n];
		}
		if(init.to_see_enigma < 3){
			for(unsigned char n=0;n<N_AXIS;n++)
				altera.correction_can_be_done_in_real_time[n] = false;
		}
		else {
			for(unsigned char n=0;n<N_AXIS;n++){
				altera.correction_can_be_done_in_real_time[n] = true;
			}
		}

		if((MsgKey.mode == TMsgKey::MODE::FIND_REF || MsgKey.mode == TMsgKey::MODE::INPUT_PARAM )
			&& Lir.curItem->id != idHand_input){
			for(unsigned char n=0;n<N_AXIS;n++){
				altera.correction_can_be_done_in_real_time[n] = false;
				altera.error_corr_real_time[n] = CIOAltera::ERROR_CORR_REAL_TIME::CANT_BE_DONE;
			}
		}
		if(MsgKey.mode != TMsgKey::MODE::FIND_REF){
			//05.01.2018 it will be switch on later
			for(int i = 0 ; i < N_AXIS; i++){
				#if (configUSE_menuItem_Correction_and_Izmerenia == 0)
				altera.correction_can_be_done_in_real_time[i] = false;
				altera.error_corr_real_time[i] = CIOAltera::ERROR_CORR_REAL_TIME::CANT_BE_DONE;
				#endif
				#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
				if(!ref_.pRef[i]) {
					altera.correction_can_be_done_in_real_time[i] = false;
					altera.error_corr_real_time[i] = CIOAltera::ERROR_CORR_REAL_TIME::NO_REF;
				}
				if(ref_.pRef[i] && flash_prg.axis_cfg[i].is_correction == CPrg::YES){
					if(altera.error_corr_real_time[i] != CIOAltera::ERROR_CORR_REAL_TIME::NO_TABLE_PLUS_NO_TABLE_MINES_){
						if(altera.error_corr_real_time[i] == CIOAltera::ERROR_CORR_REAL_TIME::CANT_BE_DONE)
						altera.error_corr_real_time[i] = CIOAltera::ERROR_CORR_REAL_TIME::NO_ERROR;
					}
				}
				#endif
			}
		}
}

/*
в этой функции распределяется поток либо режим от метки до метки , либо режим от концевика до метки
переменная ref_.ref_ref_enable
*/
void TMsgKey::submodes_ref_mode_are_executed(void){
  
  if (Lir.curItem->id != idMenuEncoders){
	Lir.zonePM_switch_toref();  
	#if (configUSE_NCshell == 1)
	Lir.zone_limit_switch_toref();
	#endif
  }
  sei();

  if(ref_.ref_ref_enable){
	  if(ref_.ref_ref_count > 0){ 
		cli();
		screen.counter_ref_=ref_.ref_ref_count;
		sei();
		if(ref_.ref_ref_count >= 2){  // расстояние посчитано только если прошли как минимум две метки
		//======считывание расстояния от метки до метки
			if(screen.measure_chan[0]){
				screen.pulses_=altera.read_cnt_quick(5);
				Lir.cur_ch_calc_2r2 = 0;
				}
			else if(screen.measure_chan[1]){
				screen.pulses_=altera.read_cnt_quick(6);
				Lir.cur_ch_calc_2r2 = 1;
				}
			else if(screen.measure_chan[2]){
				screen.pulses_=altera.read_cnt_quick(7);
				Lir.cur_ch_calc_2r2 = 2;
			}
			else if(screen.measure_chan[3]){
				screen.pulses_=altera.read_cnt_quick(8);
				Lir.cur_ch_calc_2r2 = 3;
			}
		//==================если поиск расстояния от метки до метки в меню РЕЖИМ ОТЛАДКИ-> Датчики
		if (Lir.curItem->id == idMenuEncoders){
			if(screen.pulses_ >= 8388608)
				screen.pulses_ = 16777217 - screen.pulses_;
			else
				screen.pulses_++;
			init.ref_mode[Lir.cur_ch_calc_2r2] = false;		
			ref_.pRef[Lir.cur_ch_calc_2r2] = false;			
			return;
			}
		//===================================================
		//==================если поиск кодированных меток
			if(screen.pulses_ > 2 && screen.pulses_ != 16777215) // фильтр альтеры
			{
				ref_.sz_two_ref = screen.pulses_;				
				MsgKey.set_out_ref_mode = 1;
				all_ref_interrupts_must_be_disabled = 1;			
				if(Lir.cur_ch_calc_2r2 >= 0 && Lir.cur_ch_calc_2r2 <= 3){
					#if (configUSE_NCshell == 1)
					Lir.axis_drive_stop_lir(1,Lir.plb2(Lir.cur_ch_calc_2r2));
					#endif
					init.mRef_ext[Lir.cur_ch_calc_2r2] = 1;
					ref_.pRef[Lir.cur_ch_calc_2r2] = 1;
					init.ref_mode[Lir.cur_ch_calc_2r2] = 1;
					Lir.ref_mode[Lir.cur_ch_calc_2r2] = 0;

				   }
			} 
		//================================================
		sei();
		}
		}	
	}
}

void TMsgKey::make_speed(void){
//Buffer is refreshed. Last coming is on the top
//screen.interval_for_speed[0]=screen_service::SPEED::SIZE_FOR_SPEED;
                     
		for(int c = 0 ; c < N_AXIS; c++){
			for(int n = screen.interval_for_speed[c] - 1; n > 0 ;n--) {
				display.speed_sliping_[c][n] = display.speed_sliping_[c][n - 1];
			}
			cli();
			display.speed_sliping_[c][0] = mCounter[c].delta ;//< 0 ? -mCounter[c].delta : mCounter[c].delta;
			sei();
			display.speed_100[c] = 0;
			for(int n = 0;n < screen.interval_for_speed[c];n++){
				display.speed_100[c] += display.speed_sliping_[c][n];// + display.speed_100[fs];
			}
		}
		
		/*display.speed_100[fs] = 0;
		for(int n = 0;n < screen.interval_for_speed[fs];n++){
			display.speed_100[fs] += display.speed_sliping_[fs][n];// + display.speed_100[fs];
			}
		fs++;
		if(fs >= 4)
			fs = 0; */

//display.print_debug_one_local_shot(display.speed_100[0],0,0,0,10);

}
void TMsgKey::to_take_datas_from_channel_0(void){
	bool dis_0 = false;
	cli();
	if(ref_.is_enabled_PB5_int_0()){
		ref_.to_stop_PB5_int_0();
		dis_0 = true;
		sei();
		}
	sei();
	//altera.counter[0] = altera.read_cnt_quick(0);
	altera.counter[0] +=1;
	pult_counters.read_cnt(&altera.counter[0],0);//Lir.read_cnt_simple(altera.counter_0,0);
	if(dis_0 == true){ 
		cli();
		ref_.to_continue_PB5_int_0();
		sei();
		}
	if(init.mRef_ext[0])
		screen.channel_counter[0] = mCounter[0].abs;
	else 
		screen.channel_counter[0] = mCounter[0].rel;	
}
void TMsgKey::to_take_datas_from_channel_1(void){
	bool dis_0=false;
	cli();
	if(ref_.is_enabled_PA0_int_1()){
		ref_.to_stop_PA0_int_1();
		dis_0=true;
		sei();
		}
	sei();
	altera.counter[1]=altera.read_cnt_quick(1);
	pult_counters.read_cnt(&altera.counter[1],1);
	if(dis_0==true){ 
		cli();
		ref_.to_continue_PA0_int_1();
		sei();
		}
	if(init.mRef_ext[1])	
		screen.channel_counter[1] = mCounter[1].abs;
	else 
		screen.channel_counter[1] = mCounter[1].rel;
}
void TMsgKey::to_take_datas_from_channel_2(void){
//PA1_int_2()	
	bool dis_0=false;
	cli();
	if(ref_.is_enabled_PA1_int_2()){
		ref_.to_stop_PA1_int_2();
		dis_0=true;
		sei();
		}
	sei();
	altera.counter[2]=altera.read_cnt_quick(2);
	pult_counters.read_cnt(&altera.counter[2],2);
	if(dis_0==true){ 
		cli();
		ref_.to_continue_PA1_int_2();
		sei();
		}
	if(init.mRef_ext[2])	
		screen.channel_counter[2] = mCounter[2].abs;
	else 
		screen.channel_counter[2] = mCounter[2].rel;
}
void TMsgKey::to_take_datas_from_channel_3(void){
//init_PB3_int_ref_3	
	bool dis_0=false;
	cli();
	if(ref_.is_enabled_PB3_int_3()){
		ref_.to_stop_PB3_int_3();
		dis_0=true;
		sei();
		}
	sei();
	altera.counter[3]=altera.read_cnt_quick(3);
	pult_counters.read_cnt(&altera.counter[3],3);
	if(dis_0==true){ 
		cli();
		ref_.to_continue_PB3_int_3();
		sei();
		}
	if(init.mRef_ext[3])	
		screen.channel_counter[3] = mCounter[3].abs;
	else 
		screen.channel_counter[3] = mCounter[3].rel;	
}

//#pragma GCC pop_options
//08.10.2018
void TMsgKey::to_check_CR2032(void){
	if (!atdc.conv_result)
		atdc.alarm_counter++;
	else {
		if (atdc.start_in_main < 100){
			//15.11.2017 2860 mv - 2047
			//           2750 mv - 1965
			//           2700 mv - 1930
			//           2500 mv - 1790
			//           2000 mv - 1430
			// 100*0.07=7 s
			if (atdc.int_result_adc_ch0 < 1790){
				display.print_flash_local(sBatLow,0,0,224); //208
				atdc.alarm=true;
			}
		}
		else
			atdc.start_in_main=100;
	}
	//-----------------------------
	if(atdc.alarm_counter>100){
		display.print_flash_local(sBatNoCntrl,0,0,224);
		atdc.alarm=true;
		atdc.alarm_counter=0;
	}	
};
