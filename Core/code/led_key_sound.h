/*
 * led_key_sound.h
 *
 * Created: 14.06.2017 16:09:35
 *  Author: Андрей
 */ 


#ifndef LED_KEY_SOUND_H_
#define LED_KEY_SOUND_H_

#include "main.h"
extern void delay_ms(long d);
extern "C"{
	extern void delay_mks(int d);
}

class Cled_key_sound{
	//char state_key;
	char beep_count;
	char pause_key;
	
	public:
	//31.01.2017
	char pwm_on_off;
	//-----------
	int runner_for_debug;
	char led[4];
	unsigned char naf_3_4;
	//    Cled_key_sound(){
	//    state_key=SCAN_READ_ZERO;
	//    lock_key=FREE;
	//    soundwou=1;
	//    }
	friend  void CInit::set_A1(void);
	friend  void CInit::clr_A1(void);
	TWI_TALK get_acknowledge_twi(void);
	TWI_TALK get_acknowledge_twi_led(void);
	TWI_TALK send_adress_for_read_from_keycon(char );
	TWI_TALK  read_from_keycon(char *);
	TWI_TALK init_keycon(void);
	TWI_TALK answer_start;
	TWI_TALK read_key_code(unsigned char *);
	unsigned char send_slave_adress_to_keycon_(char );
	void run_led_key_sound(void);
	void test_led(void);
	void OFF_LED(char  );
	void ON_LED(char  );
	bool  IS_LED(unsigned char );
	void  OFF_ALL_LED(void);
	//--16.05.2016--------
	void switch_on_STOP(void){
		OFF_LED(_led_START);
		ON_LED(_led_STOP);
	}
	void switch_on_START(void){
		OFF_LED(_led_STOP);
		ON_LED(_led_START);
	}
	//30.01.2017
	void  to_switch_on_PWM01_TMS429(void);
	void  to_switch_off_PWM01_TMS429(void);
	void  to_switch_on_PWM02_TMS429(void);
	void  to_switch_off_PWM02_TMS429(void);
	//-------------------------------------
	void  set_sound(void);
	void delete_sound(void);
	//17.03.2017--------------
	unsigned char read_from_realtime_clock(unsigned char *buf,char n_bytes);
	unsigned char write_to_realtime_clock(unsigned char start_adress,unsigned char *buf,char n_bytes);
	unsigned char send_byte_to_realtime_clock(unsigned char byte_to_send);
	void to_talk_with_realtime_clock_device(unsigned char *buf);
	unsigned char write_to_realtime(unsigned char *buf,unsigned char address,char num);
	unsigned char to_read_counters_from_realtime(unsigned char *buf,unsigned char address,char num);
	unsigned char to_read_time_from_realtime_clock_device(unsigned char *buf);
	unsigned char to_setup_time_to_realtime_clock_device(unsigned char *buf);
	TWI_TALK change_contrast(unsigned char contrast_value);
	enum KEY_PRESS{
		FIRST_PRESS=0,
		FIRST_HOLD=1,
		SECOND_HOLD=2,
		THIRD_HOLD=3,
		//FORTH_HOLD=4,
	};
	KEY_PRESS follower_key_press;
	int limit,running;
	bool first_condition,first_step,do_holddown;
	void start_holddown(void){
		cli();//__disable_interrupt();
		running=0;
		first_step=true;
		follower_key_press=FIRST_PRESS;
		limit=27;
		sei();//__enable_interrupt();
	}
	//23.03.2017
	struct R_T_0{
		long rel[N_AXIS];
	} r_t_save_rel;
	struct R_T_1{
		long abs[N_AXIS];
	} r_t_save_abs;
	struct R_T_2{
		bool ref[N_AXIS];
		bool is_it_saved_by_finger_press;
	} r_t_save_ref;
unsigned char setup_time[10];
int year;
//03.12.2018
bool counters_can_be_restored;
//14.11.2017
enum REALTIME_CLOCK{
	SendAddressSek=  0,
	ReadSek      =  1,
	SendAddressMin =  2,
	ReadMin       =  3,
	SendAddressHour=  4,
	ReadHour      =  5,
	SendAddressDay =  6,
	ReadDay       =  7,
	SendAddressMonth= 8,
	ReadMonth      = 9,
	CONTRAST_CAN_BE_CHANGED=10,
	SETUP_TIME=11,
	ReadYearDay=12,
	ReadWeekdaysMonth=13,
	SETUP_YEAR=14,
	WRITE_COUNTERS=15,
};
REALTIME_CLOCK follower;
bool it_was_setup_time;
unsigned char real_time[10],real_time_to_see[10];
Cled_key_sound::REALTIME_CLOCK answer_time;
Cled_key_sound::REALTIME_CLOCK work_with_time(void);
unsigned char to_send_address_to_PCF8583(unsigned char address);
unsigned char to_read_bytes_from_PCF8583(unsigned char *buf,char num);
void time_string_with_seconds(unsigned char *incoming,char *output);
void time_string_without_seconds(unsigned char *incoming,char *output);
void convertBCDtoASCII(unsigned char bcd, char * ascii);
void day_and_month_string(unsigned char *incoming,char *output);
void read_RAM_PCF8583(void);
void to_set_false_for_is_it_saved_by_finger_press(void);
//------------------------

//------------------------
	char lock_key;
	char soundwou;
	char key_code;
	char memory_key_code;
	char key_follower,key_follower_next,limit_holddown;
	TWI_TALK state_key;
	TWI_TALK state_keycon;
	unsigned int time_interval_main_loop;
	char h_input_altera,l_input_altera;
	long counter_altera,counter_altera_ref,counter_handweel;
	unsigned char buf_for_real_time_clock[20];
	//long counter_max;
	void init_twi_test(void){
		//PC0 SDA line WIREDANDPULL
		PORTC.PIN0CTRL=0x38;
		PORTC.OUTSET=0x01;
		PORTC.DIRSET =0x01;
		//PC1 SCL line
		PORTC.PIN1CTRL=0x38;
		//PORTC.OUTCLR=0x02;
		PORTC.OUTSET=0x02;
		PORTC.DIRSET =0x02;
	}
	void init_twi(void){
		PORTC.PIN0CTRL=0x38;
		PORTC.OUTSET=0x01;
		PORTC.DIRSET =0x01;
		PORTC.PIN1CTRL=0x38;
		PORTC.OUTSET=0x02;
		PORTC.DIRSET =0x02;

		TWIC.CTRL=0x00;
		TWIC.MASTER.BAUD=0x2f;
		//TWIC.MASTER.CTRLA=0x38;
		TWIC_MASTER_CTRLA=TWI_MASTER_ENABLE_bm; //enable master
		TWIC.MASTER.CTRLB=0x00;
		TWIC.MASTER.CTRLC=0x00;
		TWIC_MASTER_STATUS=TWI_MASTER_RIF_bm|TWI_MASTER_WIF_bm|TWI_MASTER_ARBLOST_bm|TWI_MASTER_BUSERR_bm|TWI_MASTER_BUSSTATE0_bm; //initially clear all flags and select bus state as IDLE
		/*PORTC.PIN0CTRL=0x38;
		PORTC.OUTSET=0x01;
		PORTC.DIRSET =0x01;
		PORTC.PIN1CTRL=0x38;
		PORTC.OUTSET=0x02;
		PORTC.DIRSET =0x02;
		TWIC.CTRL=0x00;
		TWIC.MASTER.CTRLB=0x00;
		TWIC.MASTER.CTRLC=0x00;
		TWIC.MASTER.BAUD=0x2f;
		TWIC.MASTER.CTRLA=0x38;*/
	}
	//17.03.2017--start realtime clock device
	void switch_on_100_kgh(void){
		TWIC.MASTER.CTRLA=0x00;
		TWIC.MASTER.BAUD=0x9b;
		//enable TWI master
		TWIC.MASTER.CTRLA=0x38;
	}
	void switch_on_295_kgh(void){
		TWIC.MASTER.CTRLA=0x00;
		TWIC.MASTER.BAUD=0x2f;
		//enable TWI master
		TWIC.MASTER.CTRLA=0x38;
	}

	//-----------------------------
	TWI_TALK set_state_to_idle_twi(void){
		//is it idle state ?
		char state;
		state = TWIC.MASTER.STATUS;
		state = state&0x03;
		if (state != 0x01) {
			//set to ******01 (idle state)
			//TWIC.MASTER.STATUS = TWIC.MASTER.STATUS & ~0x02;
			//TWIC.MASTER.STATUS = TWIC.MASTER.STATUS | 0x01;
			TWIC.MASTER.STATUS = 0x01;
		}
		state = TWIC.MASTER.STATUS&0x03;
		switch(state) {
			case 0x00: return UNKNOWN_TWI;
			case 0x01: return IDLE_TWI;
			case 0x02: return OWNER_TWI;
			case 0x03: return BUSY_TWI;
		}
		return TWI_STATE_ERROR;
	}
	unsigned char set_state_to_idle_twi_realtime_clock(void){
		char state;
		//set to ******01 (idle state)
		TWIC.MASTER.STATUS = TWIC.MASTER.STATUS & ~0x02;
		TWIC.MASTER.STATUS = TWIC.MASTER.STATUS | 0x01;

		state = TWIC.MASTER.STATUS&0x03;
		switch(state) {
			case 0x00: return UNKNOWN_TWI;
			case 0x01: return IDLE_TWI;
			case 0x02: return OWNER_TWI;
			case 0x03: return BUSY_TWI;
		}
		return TWI_STATE_ERROR;
	}
	TWI_TALK send_slave_adress_to_keycon(char slave_adr){
		//char slave_adr=0x70;
		char state,arbitration_lost,bus_error,write_interrupt_flag;

		TWIC.MASTER.ADDR = slave_adr;

		for(int counter=0;;counter++) {
			state=TWIC.MASTER.STATUS;
			arbitration_lost = state&0x08;
			bus_error = state&0x04;
			write_interrupt_flag=state&0x40;
			//     clock_hold=state&0x20;
			if (arbitration_lost == 0x08) return ARBITRATION_LOST_TWI;
			if (bus_error == 0x04) return BUS_ERROR_TWI;
			if (write_interrupt_flag == 0x40) break;
			//     if (clock_hold==0x20) break;
			//100 is 65 mks
			//  if (counter>=100) return TIMEOUT_TWI;
			if (counter>=250) return TIMEOUT_TWI;
		}
		//   TWIC.MASTER.STATUS=TWIC.MASTER.STATUS|0x40;
		//wait acknowledge
		//  init.set_B1();
		return get_acknowledge_twi();
	}
	unsigned char send_slave_adress_to_keycon_for_to_read(char slave_adr){
		//char slave_adr=0x70;
		char state,arbitration_lost,bus_error,read_interrupt_flag;

		//char clock_hold;

		TWIC.MASTER.ADDR=slave_adr;

		for(int counter=0;;counter++) {
			state=TWIC.MASTER.STATUS;
			arbitration_lost = state&0x08;
			bus_error = state&0x04;
			read_interrupt_flag=state&0x80;
			//     clock_hold=state&0x20;
			if ( arbitration_lost==0x08) return ARBITRATION_LOST_TWI;
			if ( bus_error==0x04) return BUS_ERROR_TWI;
			if (read_interrupt_flag==0x80) break;
			//     if (clock_hold==0x20) break;
			//100 is 65 mks
			//  if (counter>=100) return TIMEOUT_TWI;
			if (counter>=250) return TIMEOUT_TWI;
		}
		//   TWIC.MASTER.STATUS=TWIC.MASTER.STATUS|0x40;
		//wait acknowledge
		//  init.set_B1();
		return get_acknowledge_twi();


	}









	TWI_TALK send_command_to_keycon_write(char command,char command_data){
		//char clock_hold;
		char state,arbitration_lost,bus_error,write_interrupt_flag;
		//char command=0x04;
		//char command_data=0x81;
		//unsigned char answer;
		TWI_TALK answer;

		int count = 500;
		while(count)
		{
			if(TWIC.MASTER.STATUS & TWI_MASTER_CLKHOLD_bm)
				break;
			count--;
		}
		if(!count)
			return CLOCK_HOLD_ERROR_TWI;
		else
			TWIC.MASTER.DATA=command;
			

		for(int counter=0;;counter++) {
			state=TWIC.MASTER.STATUS;
			arbitration_lost = state&0x08;
			bus_error = state&0x04;
			write_interrupt_flag=state&0x40;
			if ( arbitration_lost==0x08) return ARBITRATION_LOST_TWI;
			if ( bus_error==0x04) return BUS_ERROR_TWI;
			if (write_interrupt_flag==0x40) break;
			if (counter>=100) return TIMEOUT_TWI;
		}
		answer=get_acknowledge_twi();
		if (answer==TIMEOUT_ACKNOWLEDGE_TWI) return answer;

		count = 500;
		while(count)
		{
			if(TWIC.MASTER.STATUS & TWI_MASTER_CLKHOLD_bm)
				break;
			count--;
		}
		if(!count)
			return CLOCK_HOLD_ERROR_TWI;
		else
			TWIC.MASTER.DATA=command_data;


		for(int counter=0;;counter++) {
			state = TWIC.MASTER.STATUS;
			arbitration_lost = state&0x08;
			bus_error = state&0x04;
			write_interrupt_flag = state&0x40;
			if (arbitration_lost == 0x08) return ARBITRATION_LOST_TWI;
			if (bus_error == 0x04) return BUS_ERROR_TWI;
			if (write_interrupt_flag == 0x40) break;
			if (counter >= 100) return TIMEOUT_TWI;
		}
		return get_acknowledge_twi();
	}
	void send_stop_twi(void){
		TWIC.MASTER.CTRLC=0x03;
	}
	void send_stop_twi_read(void){
		TWIC.MASTER.CTRLC=0x07;
	}
	
	
	#pragma GCC push_options
	#pragma GCC optimize ("O0")
	TWI_TALK send_pocket_to_led(char led){
		//pocket is adress 0x80 and first data byte and second data byte
		//char slave_adr=0x70;
		char state,arbitration_lost,bus_error,write_interrupt_flag;
		//unsigned char answer;
		TWI_TALK answer;
		//char clock_hold;
		int counter;
		TWIC.MASTER.ADDR=0x80;
		for(counter=0;;counter++) {
			state=TWIC.MASTER.STATUS;
			arbitration_lost = state&0x08;
			bus_error = state&0x04;
			write_interrupt_flag=state&0x40;
			//     clock_hold=state&0x20;
			if ( arbitration_lost==0x08) return ARBITRATION_LOST_TWI;
			if ( bus_error==0x04) return BUS_ERROR_TWI;
			if (write_interrupt_flag==0x40) break;
			//     if (clock_hold==0x20) break;
			if (counter>=100) return TIMEOUT_TWI;
		}

		answer= get_acknowledge_twi_led();
		if(answer!=SLAVE_ADRESS_IS_SENT_TWI) {
			send_stop_twi();
			return answer;
		}

		TWIC.MASTER.DATA=led;

		for(counter=0;;counter++) {
			state=TWIC.MASTER.STATUS;
			write_interrupt_flag=state&0x40;
			//     clock_hold=state&0x20;
			if (write_interrupt_flag==0x40) break;
			//     if (clock_hold==0x20) break;
			if (counter>=120) return TIMEOUT_TWI;
		}
		answer= get_acknowledge_twi_led();
		send_stop_twi();
		return O_KEY_LED_TWI;		
	}
	#pragma GCC pop_options

	void to_restore_adress_for_to_read(void){
		send_slave_adress_to_keycon(0x70);
		send_adress_for_read_from_keycon(0x00);
		send_stop_twi();
	}

	void to_set_autorepeat_for_keycon_240ms(void){
		// autorepeat delay is 24 *10 ms =240 ms then frequency is 4*10 =40 ms n=0x82
		send_slave_adress_to_keycon(0x70);
		send_command_to_keycon_write(0x02,0x82);
		send_stop_twi();
		to_restore_adress_for_to_read();
		limit_holddown = 16;
	}

	void to_set_autorepeat_for_keycon_160ms(void){
		// autorepeat delay is 16 *10 ms =160 ms then frequency is 4*10 =40 ms n=0x81
		send_slave_adress_to_keycon(0x70);
		send_command_to_keycon_write(0x02,0x81);
		send_stop_twi();
		to_restore_adress_for_to_read();
		limit_holddown = 11;
	}

	void to_set_autorepeat_for_keycon_80ms(void){
		// autorepeat delay is 8 *10 ms =160 ms then frequency is 4*10 =40 ms n=0x80
		send_slave_adress_to_keycon(0x70);
		send_command_to_keycon_write(0x02,0x80);
		send_stop_twi();
		to_restore_adress_for_to_read();
		limit_holddown = 7;
	}


};

extern  Cled_key_sound l_k_s;

#endif /* LED_KEY_SOUND_H_ */