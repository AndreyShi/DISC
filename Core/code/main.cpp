




#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdlib.h>
//#include <avr/eeprom.h>
//#include <stdio.h>
#include "main.h"
#include "led_key_sound.h"
#include "display.h"
#include "twitest.h"
#include "MenuLir.h"
#include "at45.h"
#include "programm.h"

#include "uart_flash_PD6_7.h"
#include "io_altera.h"
#include "counter.h"
#include "Cuart_PC2_PC3.h"
#include "config.h"
#include "keyfunc.h"
#include "screen_service.h"


TMsgKey MsgKey;
CInit init;

#ifdef STM32F429xx
CDispRGB display;
#else
CDisp display;
#endif

CSpiMaster spi;
CFlash myflash;

#ifdef STM32F429xx
CPrgSTM flash_prg;
#else
CPrg flash_prg;
#endif

Cled_key_sound l_k_s;
CUART_usb uart_usb;
Menu Lir;
CDouble coeff;
CIOAltera altera;
CPorta_ref ref_;
CLUFT mLuft[N_AXIS];
CCNT_SENSOR mCounter[N_AXIS];
CCounters pult_counters;
Cuart_PC2_PC3 uart_485;
CAnalog_to_digital_conversion atdc;
screen_service screen;
//--------------

//----------------------------
extern "C"{
 unsigned char get_adcal_calibration_low(void);
 unsigned char get_adcal_calibration_high(void);
// unsigned char get_adcal_temperature_low(void);
// unsigned char get_adcal_temperature_high(void);
 unsigned char to_count_ones_in_byte(unsigned char);
 void  delay_1_ms(void);
 void  delay_mks(int);
 unsigned int to_take_SP( void );
 unsigned int to_fill_SP_memory(void);
 int to_check_SP_memory(void);
 unsigned int to_take_address(char *);
 void write_c_asm(unsigned char);
 void write_d_asm(unsigned char);
 void a0_to_one_cs_to_zero_asm(void);
 void cs_to_one_asm(void);
 void write_data_asm(unsigned char);
 void only_write_pulse_asm(void);
 char to_get_byte_from_RAM (int addr);
 char to_check_data_80000000(long data);	
 char read_counter_step_0(char latch_cnt,char addr);
 char read_counter_step_1(char addr);
 char read_counter_step_2(char addr);
}
int stack_deep,stack_pointer,stack_size;
//#define STACK_START  0x24ff
//#define STACK_END   0X2000
//#define MIN_LIMIT_STACK_SIZE 1024
//int stack_deep,stack_pointer,stack_size;
//22.09.2017 it will work right only with 32 Mgh (one cycle is 32.5 ns) 
//delay_ms() and delay_mks() are kept in adca.s (assembler)
//delay_ms(1) 32029*31.25=1000.9 mks
//delay_ms(10) 320011*31.25=10000.3 mks
//delay_ms(50) 1599931*31.25=49997.8 mks
//delay_ms(100) 3199831*31.25=99994.7 mks 
//26.09.2017  delay_mks(0) - 0.5 mks
//if digit is placed inside(), time will be equal that digit (1)-1mks (10)-10 mks (65535)-65535 mks
//if variable will be placed inside(),time will be shorter to one cycle (31.25 ns.)
void delay_ms(long d){
    for (;d>0;d--) 
		delay_1_ms();
}
/*
инициализация RTC
*/
void init_my_interrupt_routine_RTC_OVF(void)
{
  CLK.RTCCTRL=0x0d;
//----------------
  RTC.CTRL=0x01;
//  unsigned int i = 490;
unsigned int i = 328; //326 
 init.wait_syng_inside_generator();
 RTC.PER=i;
 init.wait_syng_inside_generator();
 RTC.COMP=0x00;
 init.wait_syng_inside_generator();
 RTC.CNT=0x00;
 init.wait_syng_inside_generator();
//22.12.2017 disable RTC_OVF interrupt
   RTC.INTCTRL=0x00;
}
//22.12.2017 
void enable_RTC_overflow_interrupt(void){
	RTC.INTCTRL	= RTC_OVFINTLVL_LO_gc;	//31.01.2017 Low
}

/*
вектор прерывания по приходу реф метки канала 0
*/
#if (configUSE_Conf01 == 1)
ISR(PORTB_INT1_vect)
#else
ISR(PORTC_INT1_vect)
#endif
{	
	unsigned char a;
	if(!ref_.enable_measure_int[0] && !ref_.ref_ref_enable){
		ref_.disable_PB5_int_0();
		ref_.pRef[0]=true;
		Lir.ref_mode[0] = 0;
		Lir.size_2r2[0] = 0;
		#if (configUSE_NCshell == 1)
		Lir.axis_drive_stop_lir(1,1);
		MsgKey.set_out_ref_mode = 0;
		#endif		
	}
	else if(ref_.enable_measure_int[0]){
		ref_.ref_mark_measure[0]=true;
		ref_.counter_measure[0]=screen.channel_counter[0];
	}
	else if(ref_.ref_ref_enable){   //расстояние от метки до метки
		ref_.ref_ref_count++;		      //счетчик меток
		if(ref_.ref_ref_count == 1){
			Lir.size_2r2[0] = 0;
		}
		if(ref_.ref_ref_count >= 2 && Lir.curItem->id != idMenuEncoders){
			ref_.disable_PB5_int_0();
		}
	}	
	mCounter[0].ref_reset();
	a = PORTC.INTFLAGS & 0x01;//a=PORTB.INTFLAGS&0x01;
	PORTC.INTFLAGS = 0x02 | a;//PORTB.INTFLAGS=0x02|a;
}
/*
вектор прерывания по приходу реф метки канала 1
*/
 ISR(PORTA_INT0_vect){
	 //USART_DEBUG(0);
	unsigned char a;
	if(flash_prg.to_take_table || flash_prg.sync_corr_see) { 
		ref_.disable_PA0_int_1_corr();
		mCounter[1].ref_reset();
		mCounter[0].ref_reset();
		altera.set_flags(0x03);
		altera.set_flags(0x02);
		ref_.etalon_counter = 0;
		ref_.enable_to_go_for_correction = true;
		ref_.check_counter = 1; 
	}	 
	else if (!ref_.enable_measure_int[1] && !ref_.ref_ref_enable) {
		ref_.disable_PA0_int_1();
		ref_.pRef[1] = true;
		Lir.ref_mode[1] = 0;
		Lir.size_2r2[1] = 0;
		#if (configUSE_NCshell == 1)
		Lir.axis_drive_stop_lir(1,2);
		MsgKey.set_out_ref_mode = 0;
		#endif
	}
	else if (ref_.enable_measure_int[1]) {
		ref_.ref_mark_measure[1] = true;
		ref_.counter_measure[1] = screen.channel_counter[1];			
	}
	else if (ref_.ref_ref_enable){   //расстояние от метки до метки
		ref_.ref_ref_count++;		      //счетчик меток
		if(ref_.ref_ref_count == 1){
			Lir.size_2r2[1] = 0;
		}
		if(ref_.ref_ref_count >= 2 && Lir.curItem->id != idMenuEncoders){
			ref_.disable_PA0_int_1();
			
		}
	}
	mCounter[1].ref_reset();
	a = PORTA.INTFLAGS & 0x02;
	PORTA.INTFLAGS = 0x01 | a;	
 }
/*
вектор прерывания по приходу реф метки канала 2
*/
 ISR(PORTA_INT1_vect){
	 unsigned char a;
	 if (!ref_.enable_measure_int[2] && !ref_.ref_ref_enable){
		ref_.disable_PA1_int_2();
		ref_.pRef[2] = true;
		Lir.ref_mode[2] = 0;
		Lir.size_2r2[2] = 0;
		#if (configUSE_NCshell == 1)
		Lir.axis_drive_stop_lir(1,4);
		MsgKey.set_out_ref_mode = 0;
		#endif	
	 }
     else if (ref_.enable_measure_int[2]) {
		ref_.ref_mark_measure[2] = true;
		ref_.counter_measure[2] = screen.channel_counter[2];
     }	
	 else if (ref_.ref_ref_enable){   //расстояние от метки до метки
		 ref_.ref_ref_count++;		      //счетчик меток
		 if(ref_.ref_ref_count == 1){
			 Lir.size_2r2[2] = 0;
		 }
		 if(ref_.ref_ref_count >= 2 && Lir.curItem->id != idMenuEncoders){
			 ref_.disable_PA1_int_2();

		 }
	 } 
	 mCounter[2].ref_reset(); 
	 a = PORTA.INTFLAGS & 0x01;
	 PORTA.INTFLAGS = 0x02 | a;
 } 
/* 
вектор прерывания по приходу реф метки канала 3
*/
ISR(PORTB_INT0_vect){
	unsigned char a;
	if (!ref_.enable_measure_int[3] && !ref_.ref_ref_enable){
		ref_.disable_PB3_int_3();
		ref_.pRef[3] = true;
		Lir.ref_mode[3] = 0;
		Lir.size_2r2[3] = 0;
		#if (configUSE_NCshell == 1)
		Lir.axis_drive_stop_lir(1,8);
		MsgKey.set_out_ref_mode = 0;
		#endif
	}
	else if (ref_.enable_measure_int[3]) {
		ref_.ref_mark_measure[3] = true;
		ref_.counter_measure[3] = screen.channel_counter[3];
	}
	else if(ref_.ref_ref_enable){   //расстояние от метки до метки
		ref_.ref_ref_count++;		      //счетчик меток
		if(ref_.ref_ref_count == 1){
			Lir.size_2r2[3] = 0;
		}
		if(ref_.ref_ref_count >= 2 && Lir.curItem->id != idMenuEncoders){
			ref_.disable_PB3_int_3();
		}
	}  
	mCounter[3].ref_reset(); 
	a = PORTB.INTFLAGS & 0x02;
	PORTB.INTFLAGS = 0x01 | a;
}
/*
функция для работы с железом по TWI
выключение блока
*/
void work_with_twi(){
	unsigned char key;
	//==========================сохранение счетчиков перед выключением=========================
	if(init.save_is_frosted > 0){
		init.save_is_frosted--;
		switch(init.save_is_frosted){
			case 9:
			case 8:
			case 7:
			flash_prg.to_take_counters_to_the_structure();
			l_k_s.write_to_realtime((unsigned char *)&l_k_s.r_t_save_rel,0x10,16);
			l_k_s.write_to_realtime((unsigned char *)&l_k_s.r_t_save_abs,0x20,16);
			l_k_s.write_to_realtime((unsigned char *)&l_k_s.r_t_save_ref,0x30,5);
			flash_prg.write_eeprom(1905,sizeof(Lir.size_2r2),&Lir.size_2r2);
			display.clear_ram_null();
			//it is switch off
			PORTB.OUTCLR = 0x04; // нога на оптрон
			PORTB.DIRSET = 0x04;
			while(1){
				wdt_reset();
			}
		}
	}
	//===========================================================================================
	if(display.runner_led_or_keyboard <= 5)
	{
		if(display.runner_led_or_keyboard & 0x01)
		{
			//_delay_us(48);
			display.runner_led++;
			if(display.runner_led>=4) display.runner_led=0;
			char led=l_k_s.led[display.runner_led];
			switch (display.runner_led)
			{
				case 0x00:break;
				case 0x01:led=led|0x10;break;
				case 0x02:led=led|0x20;break;
				case 0x03:led=led|0x30;break;
			}
			// l_k_s.state_keycon=O_KEY_LED_TWI;
			l_k_s.state_keycon = l_k_s.send_pocket_to_led(led);
			l_k_s.state_keycon = O_KEY_LED_TWI;
			if (l_k_s.state_keycon != O_KEY_LED_TWI){
				display.clear_ram(0x00,0);
				while(1){
					cli();
					wdt_reset();
					//-----------------
					display.print_flash_local(PSTR("Велик уровень помех или отказ аппаратуры"),0,0,0);
					display.print_flash_local(PSTR("Перевключите блок"),0,0,16);
					for(int n=0;n<10;n++) {
						wdt_reset();
						delay_ms(100);
					}
					display.clear_ram(0x00,0);
					wdt_reset();
					delay_ms(100);
				}
			} 
		}
		else
		{
			//_delay_us(84);
			l_k_s.running++;
			if(l_k_s.running>100){
				l_k_s.running=100;
			}
			// l_k_s.state_keycon=l_k_s.read_key_code(&key);
			#if (configUSE_max7370 == 1)
			l_k_s.state_keycon = (TWI_TALK)readkeypress_max7370_filter(&key);
			#else
			l_k_s.state_keycon = (TWI_TALK)readkeypress_max7349(&key);
			#endif
			if(l_k_s.state_keycon == KEY_IS_HOLD || l_k_s.state_keycon == KEY_IS_PRESSED || l_k_s.state_keycon == KEY_IS_RELEASED)
			{			
				if(key != 0x00 && l_k_s.lock_key==FREE)
				{
					l_k_s.key_code=key;
					if (l_k_s.state_keycon == KEY_IS_PRESSED )
						l_k_s.lock_key = PROCESSING_PRESS;
					if (l_k_s.state_keycon == KEY_IS_HOLD )
						l_k_s.lock_key = PROCESSING_HOLDDOWN;
					if (l_k_s.state_keycon == KEY_IS_RELEASED )
						l_k_s.lock_key = PROCESSING_RELEASED;				
				}
			}			
		}
	}
	else if (display.runner_led_or_keyboard == 6){
		//_delay_us(60);
		if(l_k_s.answer_time==Cled_key_sound::REALTIME_CLOCK::CONTRAST_CAN_BE_CHANGED)
			l_k_s.change_contrast(init.contrast_value);
		l_k_s.answer_time=l_k_s.work_with_time();  
		display.runner_led_or_keyboard = 0; 
	} 
} 
/*
функция для работы с железом (считывание счетчиков, измерение скорости и т д)
*/
void  work_with_hardware(){	
	//init.set_B6();	
	//15.10.2018 it takes up to 8 mks
	if(MsgKey.all_ref_interrupts_must_be_disabled)
		MsgKey.all_ref_interrupts_will_be_disabled_and_var_set_to_starting_state();	
	//init.clr_B6();
	
	//15.10.2018 it takes up to 4 mks
	MsgKey.starting_line_for_PB5_int_0();
	MsgKey.starting_line_for_PA0_int_1();
	MsgKey.starting_line_for_PA1_int_2();
	MsgKey.starting_line_for_PB3_int_3();
	//init.clr_B6();
	MsgKey.correction_init();
	#if (configUSE_mcx514 == 1)
		MsgKey.read_counters_nova();
		MsgKey.read_counters_mcx514_logic();		
		Lir.servo_c_mcx514();
		Lir.Led_START_STOP();
	#endif
	#if (configUSE_mcx514 == 0)
		#if (configUSE_Cycle1ms == 0)
		MsgKey.read_counters_altera();
			#if (configUSE_NCshell == 1)
			Lir.overf_c();
			Lir.servo_c();
			#endif
		#endif
	#endif
	uart_usb.send_cnt();		
	//----------------------------------------------
	MsgKey.make_speed();
	//06.11.2018
	//==========контроль датчика
	Lir.control_encoders();	
	//=========
	#if (configUSE_NO_input == 0)
	init.p_in = altera.take_input();
	#endif
	#if (configUSE_NCshell == 1)
	Lir.input_handler_IRQ();
	Lir.prog_sw_zone_c(); 
	Lir.prog_sw_c();
	Lir.timers_c_10();
	Lir.check_dir_pusk();
	#endif
	Lir.extern_go_to_zero();

	if(MsgKey.mode == TMsgKey::MODE::FIND_REF || Lir.curItem->id == idMenuEncoders || Lir.curItem->id == idViewInputs)
		MsgKey.submodes_ref_mode_are_executed();
#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
	if(MsgKey.mode == TMsgKey::MODE::INPUT_PARAM && Lir.curItem->id != idMenuEncoders)
		flash_prg.item_correction_inside_INPUT_PARAM(); //в меню РАБОТА С ТАБЛИЦАМИ И СНЯТИЕ КОРРЕКЦИИ через память RAM
#endif
	//-------------------------------------------
	//init.clr_B6();
} 


/*
вектор прерывания по переполнению таймера 10ms обработка Счетчиков , I2C шина - Клавиатура, Часы, Светодиоды...
работа прерывания  от 0.470 ms до 0.860 ms
*/
ISR(RTC_OVF_vect)
{
 //init.set_B6();	
 sei();
 #if (configUSE_NCshell == 1)
 Lir.toggleGOTOVNOSTbSPPU();
 #endif
 #if (configUSE_PLC == 1)
 uart_485.Tx_PLC();
 #endif  
  wdt_reset();

  uart_usb.timeout_usb_transmit++;
  display.runner++;
  display.runner1++;
  display.clock++;
  display.speed++;
  display.runner_for_counters++;
  display.runner_hardware++;
  display.cursor_param_display_update++;
  Lir.cursorrunner++;
  display.interval_limit++;
  atdc.runner++;
  MsgKey.no_start_press_++;
  Lir.beep();
  uart_usb.trans_cnt++;
//30.11.2018  
  if (init.first_condition_to_switch_off == true)
	init.time_metre++;
//-------------------------------------------------------------    
  work_with_hardware(); // 480 mks

  if (init.to_see_enigma < 3){
	  for(int i = 0; i < N_AXIS;i++){
		if(!Lir.abs_ENC[i])
			screen.channel_counter[i]=mCounter[i].abs=mCounter[i].rel=mCounter[i].abs_pure=0;
		#if (configUSE_ABS_Encoder == 1)
		if(Lir.abs_ENC[i])
			mCounter[i].rel = 0;
		#endif
	  }
	  init.to_see_enigma++;
   }
   display.system_heart();
   display.runner_led_or_keyboard++; 
	//init.set_B6();	
   work_with_twi();  //160 mks	
   //init.clr_B6();
} 
/*
таймер серво цикл период 1мс
*/
#if (configUSE_Cycle1ms == 1)
ISR(TCC0_OVF_vect){
	//init.set_B6();	
	Lir.timers_c();	
	MsgKey.read_counters_altera();
	Lir.overf_c();
	Lir.servo_c();	
	//init.clr_B6();
}
#endif
/*
вектор прерывания по приходящей информации извне  ПК <-USB кабель->Контроллер
*/
ISR(USARTD1_RXC_vect)
{
	if(Lir.curItem->id == idLinkPK){
		uart_usb.recieve_interrupt_USARTD1(USART_RXCINTLVL_OFF_gc);
		if (!uart_usb.mode_rs_232_usb) 
			return;

		uart_usb.glAnswer = uart_usb.main_loop_link_through_USB(); 
		if (uart_usb.glAnswer != NO_FLASH_ERROR) {
			uart_usb.error_number++;
		}
		if (uart_usb.mode_rs_232_usb)
			uart_usb.enable_receive_interrupt_D1();
		else 
			uart_usb.recieve_interrupt_USARTD1(USART_RXCINTLVL_OFF_gc);
	}
	else{		
		if(uart_usb.take_bytes_new(5) == CUART_usb::CRC_IS_OK){
			uart_usb.glAnswer = NO_FLASH_ERROR;		 
		}			 
	}
}

/*
вектор прерывания по окончанию отправки пакета Tx rs485
*/
ISR(DMA_CH0_vect){	
	DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm;// очистка флага стр 61 описания
	//_delay_us(140);// как добавить 140 us что бы не использовать delay, добавил dymmu байты в пакет Tx
	init.adm2687_receiver();//пакет отправили ставим шину в режим "приёмник"
	uart_485.DMA_Rx();//активировать канал DMA1
	//init.set_B6();		
}
/*
вектор прерывания по окончанию приема пакета Rx rs485
*/
ISR(DMA_CH1_vect){
	//init.clr_B6();	
	DMA.CH1.CTRLB |= DMA_CH_TRNIF_bm;// очистка флага стр 61 описания
	uart_485.recieve_package_plc();//обработка пришедшего пакета Rx
}
/*
вектор прерывания по окончанию приема пакета Rx rs232
*/
#ifdef __AVR_ATxmega256A3U__
ISR(DMA_CH2_vect){
	DMA.CH2.CTRLB |= DMA_CH_TRNIF_bm;  // очистка флага стр 61 описания
	DMA.CH2.CTRLA |= DMA_CH_ENABLE_bm; // активация канала
}
#endif
/*
функция для загрузки данных из EEPROM в RAM
*/
void to_copy_parameters_from_eeprom_to_ram(void){

	//flash_prg.check_iar_eeprom();
	flash_prg.axis_is_chosen(&Lir.P2[0],&Lir.P2[1],&Lir.P2[2],&Lir.P2[3]);
	flash_prg.axis_is_chosen_to_screen(&Lir.Str[0],&Lir.Str[1],&Lir.Str[2],&Lir.Str[3]);
	flash_prg.to_copy_axis_param_from_EEPROM_to_RAM();
	flash_prg.to_copy_coeff_from_EEPROM_to_RAM();
	flash_prg.to_copy_luft_from_EEPROM_to_RAM();
	flash_prg.to_copy_G53_and_G5x_from_EEPROM_to_RAM();
	flash_prg.to_check_G53_and_G5x_when_system_is_started();	
	flash_prg.to_copy_speed_on_screen_param_to_RAM();  
	flash_prg.to_copy_contrast_to_RAM();              
	flash_prg.to_copy_to_see_time_toRAM(); //177
	flash_prg.to_copy_Bdrate_toRAM(); //178
//29.10.2018 to set some variables for angle axis
	flash_prg.to_set_var_for_angle_axis();
	flash_prg.to_copy_Rejim_zaxvata_ref_metki_toRAM(); //179
	flash_prg.to_copy_night_mode_toRAM();  //180
	flash_prg.to_copy_axis_for_CH_LH(); //181
	flash_prg.to_copy_sound(); // 185
	Lir.load_LH_CH_setup_from_eeprom();//186 - 187
//восьмая часть (общая для 540 и 541 )параметров с 796 по 923// Настройки абсолютных датчиков
	flash_prg.read_eeprom(796,sizeof(flash_prg.axis_cfg2),&flash_prg.axis_cfg2);
//21следующие с 1905 по 1920 // 16 байт
	flash_prg.read_eeprom(1905,sizeof(Lir.size_2r2),&Lir.size_2r2);
// 22 следущие с 1921 по 1936 // 16 байт
	//flash_prg.read_eeprom(1921,sizeof(Lir.R_r),&Lir.R_r);
#if (configUSE_NCshell == 1)
//занимаемый адрес с 188 по 315 ,
//вычисление объема занимаемой памяти в байтах (конечный адрес - начальный адрес) + 1 байт
//315 - 188 = 127 + 1 байт = 128
	flash_prg.read_eeprom(188,sizeof(flash_prg._Allp_param[0]) * N_AXIS,&flash_prg._Allp_param);// Зоны скорости
//вторая часть параметров с 316 по 443
	flash_prg.read_eeprom(316,sizeof(flash_prg._Allp_param2),&flash_prg._Allp_param2);// Параметры оси - Управление
//третья часть параметров с 444 по 571
	flash_prg.read_eeprom(444,sizeof(flash_prg._INAxis[0]) * N_AXIS,&flash_prg._INAxis);// Входы осевые
//четвертая часть параметров с 572 по 699
	flash_prg.read_eeprom(572,sizeof(flash_prg._OUTAxis[0]) * N_AXIS,&flash_prg._OUTAxis);// Выходы осевые
//пятая часть параметров с 700 по 731
	flash_prg.read_eeprom(700,sizeof(flash_prg._INGen),&flash_prg._INGen);// Входы общие
//шестая часть параметров с 732 по 763
	flash_prg.read_eeprom(732,sizeof(flash_prg._OUTGen),&flash_prg._OUTGen);// Выходы общие
//седьмая часть параметров с 764 по 795
	flash_prg.read_eeprom(764,sizeof(flash_prg._Allp_param3),&flash_prg._Allp_param3);// Зоны торможения
//восьмая часть (общая для 540 и 541 )параметров с 796 по 923
	//flash_prg.read_eeprom(796,sizeof(flash_prg.axis_cfg2),&flash_prg.axis_cfg2); // Настройки абсолютных датчиков
//девятая часть параметров с 924 по 1051 (128 байт ,80б - инструмент T00 T01, 48б - запас)
	flash_prg.read_eeprom(924,sizeof(Lir.T_instr[0]) * MAX_INSTR,&Lir.T_instr);
#if (configUSE_Shadow_axis == 1)
//десятая часть параметров с 1052 по....1083      // слепая ось Входы
	flash_prg.read_eeprom(1052,sizeof(flash_prg._INAxis[SW_AXIS0]),&flash_prg._INAxis[SW_AXIS0]);
//одинадцатая часть параметров с 1084 по....1115  // слепая ось Выходы
	flash_prg.read_eeprom(1084,sizeof(flash_prg._OUTAxis[SW_AXIS0]),&flash_prg._OUTAxis[SW_AXIS0]);
//двенадцатая часть параметров с 1116 по....1147  // слепая ось Зоны скорости
	flash_prg.read_eeprom(1116,sizeof(flash_prg._Allp_param[SW_AXIS0]),&flash_prg._Allp_param[SW_AXIS0]);
#endif
//тринадцатая часть параметров с 1148 по....1179  //
	flash_prg.read_eeprom(1148,sizeof(flash_prg._OUTGen2),&flash_prg._OUTGen2);
//четырнадцатая часть параметров с 1180 по....1211 //
	flash_prg.read_eeprom(1180,sizeof(flash_prg._INGen2),&flash_prg._INGen2);
#if (configUSE_Shadow_axis == 1)
// номер слепой оси 1 байт 1212
	flash_prg.read_eeprom(1212,sizeof(char),&Lir.P2[SW_AXIS0]);
#endif
// 15следующие с 1213 по 1372 // 160 байт
	flash_prg.read_eeprom(1213,sizeof(flash_prg._OUTAxis2),&flash_prg._OUTAxis2);
// 16следующие с 1373 по 1492 // 120 байт
	flash_prg.read_eeprom(1373,sizeof(flash_prg._Allp_param4),&flash_prg._Allp_param4);
// 17следующие с 1493 по 1612 // 120 байт
	flash_prg.read_eeprom(1493,sizeof(flash_prg._Allp_param5),&flash_prg._Allp_param5);
// 18следующие с 1613 по 1712 // 100 байт
	flash_prg.read_eeprom(1613,sizeof(flash_prg._Allp_param6),&flash_prg._Allp_param6);
// 19следующие c 1713 по 1744 //32 байта
	flash_prg.read_eeprom(1713,sizeof(flash_prg._Allp_param7),&flash_prg._Allp_param7);
// 20следующие c 1745 по 1904 // 160 байт
	flash_prg.read_eeprom(1745,sizeof(flash_prg._OUTAxis3),&flash_prg._OUTAxis3);
// 21следующие с 1905 по 1920 // 16 байт
	//flash_prg.read_eeprom(1905,sizeof(Lir.size_2r2),&Lir.size_2r2); общие для индикации и упр
// 22 следущие с 1921 по 1936 // 16 байт
	//flash_prg.read_eeprom(1921,sizeof(Lir.AS2r2),&Lir.AS2r2); общие для индикации и упр
// 23 следущие с 1937 по....
#endif
#if (configUSE_mcx514 == 1)
	flash_prg.read_eeprom(1937,sizeof(flash_prg.axis_cfg_nc),&flash_prg.axis_cfg_nc);
	// 24 следущие с 2065 по 2068 // 4 байта
	//flash_prg.read_eeprom(2065,sizeof(Lir._modals_cmd.F),&Lir._modals_cmd.F);
	// 25 следущие с 2069 по 2196 //128 байт
	flash_prg.read_eeprom(2069,sizeof(flash_prg.axis_cfg_nc2),&flash_prg.axis_cfg_nc2);
	// 26 следущие с 2197 по....
#endif
//13.11.2018	
	screen.to_convert_interval_for_speed_measure_to_digit();       
//----------------------------------------------
	flash_prg.init_eeprom_var();
}

/*
точка входа в программу
*/
#ifdef STM32F429xx
int main_t()
#else
int main()
#endif
{   
   cli(); 
   //29.12.2017 watchdog must be disabled
   CCP = CCP_IOREG_gc;
   WDT.CTRL = 0x01;
#ifdef __AVR_ATxmega256A3U__
   //29.12.2017 jtag must be disabled  
   CCP = CCP_IOREG_gc;
   MCU.MCUCR = 0x01;
#endif
   CCP = CCP_IOREG_gc;
   PMIC.CTRL = 0x07;  
 
   display.addr_heap_start= to_take_address(__malloc_heap_start);
   if(display.addr_heap_start > 0x5f00) {
	  //it must be error <it does not have enough SRAM,only 256 bytes left>
 	  while(1){ PORTF.DIRSET=0x01; PORTF.OUTTGL=0x01;}
   }
   display.addr_heap_end = to_take_address(__malloc_heap_end);
   if(display.addr_heap_end != 0x5fff) {
	  //it must be error <wrong heap's end address,it must be 0x5fff>
	   while(1){ PORTF.DIRSET=0x02; PORTF.OUTTGL=0x02;}
   }
   stack_pointer = to_take_SP();
   stack_deep = STACK_START - stack_pointer;
   stack_size = STACK_START - stack_deep - STACK_END;
   if(stack_deep > 0 && (stack_size > 0 && stack_size >= MIN_LIMIT_STACK_SIZE)){	
	 stack_deep = to_fill_SP_memory();
    } else {	 
	  while(1){ PORTF.DIRSET = 0x04; PORTF.OUTTGL = 0x04;}	 
    }  
   init.init_32khz();
   init.init_32mhz_and_DFLL();
   display.display_port_initializer();

   //03.10.2017 fuse2 will be read
   //if it come from boot loader no init 
#if (configUSE_RGB_disp == 0)
   {
   NVM.ADDR0=0X02;
   NVM.CMD=0X07;
   CCP=CCP_IOREG_gc;
   NVM.CTRLA=0x01;
   char result=NVM.DATA0&0x40;
   if(result==0x40)
		display.init_lcd_();
   } 
   display.init_lcd_();
#endif
#if (configUSE_RGB_disp == 1)
	display.init_lcd_RGB();
#endif    
   display.clear_ram(0x00,0);
   display.clear_ram(0x00,1);
   display.clear_ram(0x00,2);
   display.clear_ram(0x00,3);

   l_k_s.init_twi();
   for(;;){		
#if (configUSE_max7370 == 1)
		l_k_s.answer_start = (TWI_TALK)setup_keyboard_max7370_filter();
#else
		l_k_s.answer_start = l_k_s.init_keycon();
#endif
		if(l_k_s.answer_start == O_KEY_LED_TWI )//|| l_k_s.answer_start==OWNER_TWI) 
			break;
		delay_ms(20);
		display.print_flash_local(PSTR("Ошибка клавиатуры"),0,0,0);
		display.print_debug_one_local(l_k_s.answer_start,0,20,0);
		delay_ms(20);
	}
   display.clear_ram(0x00,0);
 
   l_k_s.lock_key = FREE;
   
   display.start_system_heart();
   init.B7_is_prepearing_for_ADC();
   l_k_s.read_RAM_PCF8583();
   init.input_B1_inc_f1();
   init.input_A3_inc_f2();
   init.input_A2_inc_f3();
   init.input_D4_inc_f0();

   ref_.follower = CPorta_ref::CORRECTION::SET_MODE_READOUT;
   flash_prg.to_take_table = false;
   
#if (configUSE_PLC == 1)
   uart_485.init_uart_PC2_PC3();
   uart_485.initDMACH0_tx();
   uart_485.initDMACH1_rx();
#endif
   sei();
   to_copy_parameters_from_eeprom_to_ram();
   uart_usb.init_uart1_portD();
   uart_usb.recieve_interrupt_USARTD1(USART_RXCINTLVL_MED_gc);	
   uart_usb.initDMACH2_rx();
   l_k_s.change_contrast(init.contrast_value);
   Lir.SET_C(idBegin);
#if (configUSE_mcx514 == 1)
   MsgKey.init_mx514();
#else
   altera.init_altera_bus_in_main();
#endif

   atdc.init_atdc_in_main();
   ADCA.CALL = get_adcal_calibration_low();
   ADCA.CALH = get_adcal_calibration_high();

   ref_.init_PB5_int_ref_0(true);
   ref_.init_PA0_int_ref_1(true);
   ref_.init_PA1_int_ref_2(true);
   ref_.init_PB3_int_ref_3(true);
   #if (configUSE_ABS_Encoder == 1)
   for(int i = 0 ; i < N_AXIS;i++)
	   Lir.set_ch_to_ABS(i);
   #endif
   l_k_s.read_RAM_PCF8583();
   if(l_k_s.r_t_save_ref.is_it_saved_by_finger_press == true){
	  l_k_s.counters_can_be_restored = true;	 
      }
   l_k_s.to_set_false_for_is_it_saved_by_finger_press();
   //====================Включение Watchdog
   CCP=CCP_IOREG_gc;
   WDT.CTRL = 0x13;
   wdt_reset();
   //======================================
   //====================Инициализация значения аналогового напряжения
   #if (configUSE_PLC == 1)
   uart_485.send_U(Lir.U_out);
   #endif
   //======================================
   //====================Включение RTC=====
   init_my_interrupt_routine_RTC_OVF(); 
   enable_RTC_overflow_interrupt();
   //======================================
   #if (configUSE_Cycle1ms == 1)
   //====================Включение TCC0====период слегка прыгает
   TCC0.CTRLA=0x05; // предделитель
   TCC0.PER=500-1;  // настройка периода 1 ms
   Lir.timer1MSEKON();
   //======================================
   #endif   
   while(1){
	 cli();
	  if(init.to_see_enigma >= 3){
		 if(l_k_s.counters_can_be_restored==true)
			 flash_prg.to_restore_counters();
		 break;
	     }
	sei();
   }
   sei();   
   if(l_k_s.counters_can_be_restored == 0){
	  for(int c = 0 ; c < N_AXIS; c++){ 
		if(flash_prg.axis_cfg[c].save == CPrg::SAVE::YES_SAVE && Lir.ch_use(c)){
			display.print_flash_local(PSTR("Произошло не штатное выключение."),0,0,0);
			if(screen.is_save_lives())
				display.print_flash_local(PSTR("Счётчики обнулены."),0,0,16);
			delay_ms(3500);
			display.print_symbols_local(0x20,32,0,0,0);
			display.print_symbols_local(0x20,18,0,0,16);
			break;
		}		
	 }
  }
  
   MsgKey.mode=TMsgKey::MODE::BEGIN;
   MsgKey.to_put_icons_in_begin_mode();
   
   //== включениe ручного режима при запуске устройства
   #if (configUSE_NCshell == 1)
   Lir.status2(PSTR("-"));
   Lir.SEToutRA = 1; //устанавливаем выход Ручной режим
   MsgKey._key_MODE_HAND_f();
   Lir.init_Feed();   
   #endif
   //==
   display.init_VERSION();

   for(;;) // главный цикл
   {	
		uart_usb.transmit_counters_Sync();	
	    MsgKey.key_handling();			
		Lir.Menu_actions();
		Lir.LirSnake();  
		Lir.check_pointer();
		uart_usb.display_txt_while_link();
#if (configUSE_NCshell == 1)
		Lir.input_handler();
		//Lir.check_dir_pusk_();
		Lir.print_alert_mes();	
		Lir.Analog_Corr_handler_ANIN();	

		if(Lir.UPD_show_Instr){
			Lir.show_Instr(0,Lir._resz_prg_v.SZ);
			Lir.UPD_show_Instr = 0;
		}		
#endif
		if(MsgKey.set_out_ref_mode){
			MsgKey.go_out_from_ref_mode();
			MsgKey.set_out_ref_mode = 0;
			}			
		
		if(display.runner_hardware == 1){		
			#if (configUSE_mcx514 == 1)			
			if(uart_usb.data[3] == CUART_usb::READ_COUNTERS && uart_usb.data[4] == CUART_usb::READ_COUNTERS){
				uart_usb.transmitt_counters();
				memset(uart_usb.data,0,272);
			};	
			#endif					
			display.runner_hardware = 0;			
		}
		unsigned char disp_rate = 2;
		if(Lir.get_Disp_upd())
		      disp_rate = 2;
		else
			  disp_rate = 6;     
			   
		if(display.runner_for_counters >= disp_rate)    
		{
			//1 100 Гц   100 кадров в секунду
			//2 50  Гц   50  кадров в секунду
			//3 33  Гц   33  кадра в секунду
			//4 23.25 Гц 23  кадра в секунду
			//5 20 Гц    20  кадров в сек
			//6 16.6 Гц  17  кадров в секунду
			if(Lir.is_BIG_Out_Axis()){
				//=============алгоритм рассчета с псевдо абсолютной линейки==========
				Lir.psevdo_abs_disp(ref_.sz_two_ref,Lir.cur_ch_calc_2r2);
				//=======================================================================================
				screen.show_big_digit(1);
				screen.show_big_digit(2);
				screen.show_big_digit(3);
				screen.show_big_digit(4);
				screen.show_big_digit_result();	
				#if (configUSE_NCshell == 1)	
				 Lir.show_small_axis(0,0);			 
				 Lir.debugOUT_INPUT(10);				 
				#endif			 
			}
			display.runner_for_counters = 0;
		}

		cli();

		if (display.big_digit_display_update_!=display.big_digit_display_update) 
		{
			switch(display.big_digit_display_update)
			 {
				case 0:display.run_0 = true;break;
				case 1:display.run_1 = true;break;
				case 2:display.run_2 = true;break;
				case 3:display.run_3 = true;break;
				case 4:display.run_4 = true;break;
				case 5:display.run_5 = true;break;
				case 6:display.run_6 = true;break;
				case 7:display.run_7 = true;break;
			}
		}
		display.big_digit_display_update_ = display.big_digit_display_update;
		sei();
  if(display.run_0)
     {	 
	 if(MsgKey.refresh_icons_in_begin_mode){
		MsgKey.to_put_icons_in_begin_mode(); 
		MsgKey.refresh_icons_in_begin_mode=false; 
	 }
//03.12.2018 switch off is coming...
	 cli();
	 int timeout_for_switch_off=init.time_metre;
	 sei();
	 if(timeout_for_switch_off >= 290){
		cli();
		init.save_is_frosted=10; 
		sei();	 
	 }
	 if(timeout_for_switch_off >= 100){
		display.print_flash_local(sOFF,0,0,224);		
		timeout_for_switch_off = timeout_for_switch_off/100;
		timeout_for_switch_off = 3 - timeout_for_switch_off;
		display.print_debug_one_local_shot(timeout_for_switch_off,0,18,224,1);	 
	 }
	 if(init.to_clear_switch_off_string){	
		display.print_symbols_local(' ',30,0,0,224);
		Lir.status(PSTR(""),0,0,1);//сброс буфера
		init.to_clear_switch_off_string=false;	 
	 }
//-----------------------------------	
	cli();
     for(unsigned char n=0;n<6;n++)
		l_k_s.real_time_to_see[n] = l_k_s.real_time[n];
     display.run_0=false;
	sei();

     }

  if  (display.run_1)
  {		
		display.blinker++;	
		for(int i = 0 ; i < N_AXIS ; i++){
			if(ref_.pRef[i] && Lir.is_corr_table(i)){
				flash_prg.to_set_max_for_angel_axis(i,coeff.coeff_corr[i].binary,coeff.coeff_corr[i].n_after_dot);
			}
		}
	    if(MsgKey.mode == TMsgKey::MODE::INPUT_PARAM){
#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
			if(Lir.curItem->id == idGetCorr)
				flash_prg.to_do_correction();
			if(Lir.curItem->id == idGetCorr_input)
				flash_prg.GetCorr_input_loop();
			if(Lir.curItem->id == idSeeCorr)
				flash_prg.to_see_correction();
			if(Lir.curItem->id == idWorkCorr_Input){
				flash_prg.WorkCorr_input_loop();	
				flash_prg.show_channels_in_see_corr(Lir.corr_see_cur_ch);		
			}
#endif			
	    }	
		if(MsgKey.mode == TMsgKey::MODE::FIND_REF){
			for(int c = 0; c < N_AXIS;c++){
				init.mRef_ext[c] = ref_.pRef[c];				
			}						
			if(MsgKey.mode_ref_submode_ref_ref)
				screen.to_display_mode_ref_submode_ref_ref(true,false);	
		}
		else
			 MsgKey.to_define_is_it_A_or_O(); 			

      	if(Lir.is_BIG_Out_Axis()){			
			screen.show_axis_symbol();
			#if (configUSE_NCshell == 1)				
				Lir.Set_drv_axis_inv();
			#endif					
			}	 
       cli();
       display.run_1=false;
       sei();	   
   }

 if(display.run_2)
     { 
		cli();
		display.run_2=false;
		sei();
     }

 if(display.run_3)
   {
#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
   if(MsgKey.mode == TMsgKey::MODE::INPUT_PARAM)
		MsgKey.to_see_indikator_channel_P2_1();
#endif
     cli();
     display.run_3=false;
     sei();
   }

 if(display.run_4)
     {
	 #if (configUSE_menuItem_Correction_and_Izmerenia == 1)
	  if(MsgKey.mode == TMsgKey::MODE::INPUT_PARAM)
		MsgKey.to_see_indikator_channel_P2_0();	 
	 #endif
	 	
	 if(Lir.is_BIG_Out_Axis()){ 
		static int per;
		int8_t disp_rate = 0;
		if(!Lir.get_Disp_upd())
			disp_rate = 16;
		
		if(per >= disp_rate){
			screen.speed_error[0] = screen.to_see_speed(1);
			screen.speed_error[1] = screen.to_see_speed(2);
			screen.speed_error[2] = screen.to_see_speed(3);
			screen.speed_error[3] = screen.to_see_speed(4);
			screen.speed_error_handler();
			per = 0;
		}
		per++;		
	 }
	 
     cli();
     display.run_4=false;
     sei();
     }

 if(display.run_5)
     {	   
		//uart_usb.transmit_counters_Sync();
		if(MsgKey.mode == TMsgKey::MODE::INPUT_PARAM)
		{			
			MsgKey.to_see_indikator_dif_P2_0_P2_1();
			MsgKey.to_see_MenuEncoders();	
			MsgKey.to_see_mSupply();	
			MsgKey.to_see_mTime();
			//MsgKey.to_see_mInputs();
			Lir.draw_menu_VIIN();
		}
//08.10.2018 		 
		if(MsgKey.mode == TMsgKey::MODE::BEGIN || Lir.curItem->id == idSupplyandBatt){ 
		atdc.start_in_main++;	
		cli();
		unsigned char goto_adc=atdc.runner;
		sei();
		if(goto_adc >= 19){		
			atdc.start_internal_1_10_Vcc_measure();
			atdc.take_result_ch2();
			//atdc.start_measure_corr();
			atdc.start_external_input_signal_measure();
			atdc.conv_result = atdc.take_result_ch0();
			cli();
			atdc.runner=0;
			sei();		
		}
		MsgKey.to_check_CR2032();
		}     	
      cli();display.run_5=false;sei();
     }

 if(display.run_6)
     {	 
       if(MsgKey.mode == TMsgKey::MODE::INPUT_PARAM){
	   #if (configUSE_menuItem_Correction_and_Izmerenia == 1)
		   MsgKey.to_see_graph();   
	   #endif
		   display.current_stack_position=to_check_SP_memory();
		   MsgKey.to_see_stack_and_go_next();
		   cli();
		   bool blink=l_k_s.it_was_setup_time;
		   if(blink==true)l_k_s.it_was_setup_time=false;
		   sei();
		   if (Lir.curItem->id != idTime && 
				Lir.curItem->id != idExecmd &&
				Lir.curItem->id != idHand && 
				Lir.curItem->id != idBegin&&
				Lir.curItem->id != idCmd&&
				Lir.curItem->id != idHandMove &&
				Lir.curItem->id != idHand_input) 
				{
					char time_seconds[9];
					if(blink){
						for(unsigned char n=0;n<9;n++)
							time_seconds[n] = 0x20;
						time_seconds[8] = 0;
					}
					else
						l_k_s.time_string_with_seconds(l_k_s.real_time_to_see,time_seconds);
					if(Lir.curItem->id == idLog)
						display.print_ram_local(time_seconds,2,32,224);
					else
						display.print_ram_local(time_seconds,0,32,224);
				}
		  }
		  if(Lir.is_BIG_Out_Axis() && init.to_see_time && Lir.is_Analog() == 0){	
				bool lock = 0;			
				for(int i  = 0 ; i < N_STR; i ++){
					if(display.position_to_see[i] == CDisp::TIME)
						lock = 1;
				}
				if(!lock){
					char time_seconds[9];
					l_k_s.time_string_with_seconds(l_k_s.real_time_to_see,time_seconds); 
					display.print_ram_local(time_seconds,0,32,224); 
				}
		    }
     cli();
     display.run_6=false;
     sei();
     }

 if(display.run_7)
     {
     cli();
     display.run_7=false;
     sei();
     }
 	 
    }
   };
