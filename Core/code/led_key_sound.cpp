/*
 * led_key_sound.cpp
 *
 * Created: 15.06.2017 9:30:52
 *  Author: Андрей
 */ 

//#define F_CPU 100000UL   // 100 kHz

#include "led_key_sound.h"
#include "MenuLir.h"

void Cled_key_sound::run_led_key_sound(void){

}


TWI_TALK Cled_key_sound::get_acknowledge_twi(void){
unsigned char received_acknowledge;
  for(int counter=0 ;;counter++) {
   received_acknowledge=TWIC.MASTER.STATUS&0x10;
  if (received_acknowledge==0x00) {
//      init.clr_B1();
//      init.set_B1();
//      init.clr_B1();
//      init.set_B1();
      return SLAVE_ADRESS_IS_SENT_TWI;}
   else {
// 15  gives 10 mks 100 61 mks 1000 600 mks
       if (counter>=15) {
  //                       init.clr_B1();
  //                       init.set_B1();
       return TIMEOUT_ACKNOWLEDGE_TWI;}
   }
   }

}
#pragma GCC push_options
#pragma GCC optimize ("O0")
TWI_TALK Cled_key_sound::get_acknowledge_twi_led(void){
unsigned char received_acknowledge;
  for(int counter=0 ;;counter++) {
   received_acknowledge=TWIC.MASTER.STATUS&0x10;
  if (received_acknowledge==0x10) {
//      init.clr_B1();
//      init.set_B1();
//      init.clr_B1();
//      init.set_B1();
      return SLAVE_ADRESS_IS_SENT_TWI;}
   else {
// 15  gives 10 mks 100 61 mks 1000 600 mks
       if (counter>=15) {
//                         init.clr_B1();
//                         init.set_B1();
       return TIMEOUT_ACKNOWLEDGE_TWI;}
   }
   }

}
#pragma GCC pop_options







TWI_TALK Cled_key_sound::send_adress_for_read_from_keycon(char command){
//char clock_hold;
char state,arbitration_lost,bus_error,write_interrupt_flag;
//char command=0x04;
//char command_data=0x81;
/*state=TWIC.MASTER.STATUS;
     clock_hold=state&0x20;
  if (clock_hold==0x20){
     TWIC.MASTER.DATA=command;
     }
  else 
	return CLOCK_HOLD_ERROR_TWI; */

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
//100 is 65 mks
  if (counter>=250) return TIMEOUT_TWI;
                            }
return get_acknowledge_twi();
}

TWI_TALK  Cled_key_sound::read_from_keycon(char *received_data){
//char slave_adr=0x70;
char state,arbitration_lost,bus_error;
char read_interrupt_flag;
char clock_hold;
 TWIC.MASTER.ADDR=0x71;
//30.03.2015 when adress is sent noone flag is set
// acknowledge clock is generated and 8 clock followed
// data from slave is coming  clock line goes to low and clock_hold is set
// when data is read  hight level acknowledge  is sent with stop
 for(int counter=0;;counter++) {
     state=TWIC.MASTER.STATUS;
     arbitration_lost = state&0x08;
     bus_error = state&0x04;
     clock_hold=state&0x20;
     read_interrupt_flag=read_interrupt_flag&0x80;
     if ( arbitration_lost==0x08)
		return ARBITRATION_LOST_TWI;                                
     if ( bus_error==0x04)
		return BUS_ERROR_TWI;                           
     if (clock_hold==0x20)
        break;
	 if (counter>=201)
		return TIMEOUT_TWI; 
 }
*received_data=TWIC.MASTER.DATA;
return DATA_IS_READ_TWI;
}


TWI_TALK Cled_key_sound::init_keycon(void){
   // unsigned char answer,answer1;
TWI_TALK answer,answer1;   
     answer=set_state_to_idle_twi();
   if ( answer==IDLE_TWI) {
     answer1=send_slave_adress_to_keycon(0x70);
     if (answer1==SLAVE_ADRESS_IS_SENT_TWI) {
         answer1=send_command_to_keycon_write(0x04,0x81);
         l_k_s.send_stop_twi();
   //20 mks 20/0.03125=640
         delay_mks(20);//__delay_cycles(640);
         }
     else {
		 return answer1;
         }
      if(answer1== SLAVE_ADRESS_IS_SENT_TWI){
     answer1=send_slave_adress_to_keycon(0x70);
                                            }
      else {
		  return answer1;
          }


     if (answer1==SLAVE_ADRESS_IS_SENT_TWI) {
   //debounce time 10 ms
         answer1=l_k_s.send_command_to_keycon_write(0x01,0x41);// 0x41
   //debounce time 15 ms
   //      answer1=l_k_s.send_command_to_keycon_write(0x01,0x46);
   //debounce time 20 ms
   //      answer1=send_command_to_keycon_write(0x01,0x4b);
   //debounce time 25 ms
   //      answer1=l_k_s.send_command_to_keycon_write(0x01,0x50);
   //debounce time 30 ms
   //      answer1=l_k_s.send_command_to_keycon_write(0x01,0x55);
   //debounce time 39 ms
   //     answer1=l_k_s.send_command_to_keycon_write(0x01,0x5d);
   //debounce time 40 ms
   //     answer1=l_k_s.send_command_to_keycon_write(0x01,0x5f);
         l_k_s.send_stop_twi();
   //20 mks 20/0.03125=640
         delay_mks(20);//__delay_cycles(640);
         }
    else {
			return answer1;
          }
     if (answer1!=SLAVE_ADRESS_IS_SENT_TWI) {
         return answer1;
          }
         }
     else {
		   return answer; 
            //  init.set_D7();
            //   delay_mks(100);//__delay_cycles(640*5);
            //  init.clr_D7();
           }
   //20 mks 20/0.03125=640
   //      __delay_cycles(640);
//        l_k_s.send_slave_adress_to_keycon(0x70);
//        l_k_s.send_command_to_keycon_write(0x05,0x7c);
//        l_k_s.send_stop_twi();
//        l_k_s.send_slave_adress_to_keycon(0x70);
//        l_k_s.send_command_to_keycon_write(0x05,0x7f);
//        l_k_s.send_stop_twi();
         answer=l_k_s.send_slave_adress_to_keycon(0x70);
		 if (answer!=SLAVE_ADRESS_IS_SENT_TWI) return answer;
//09.02.2017 debounce is 10 ms
// autorepeat delay is 8 *10 ms =80 ms then frequency is 4*10 =40 ms
//        l_k_s.send_command_to_keycon_write(0x02,0x80);
// autorepeat delay is 16 *10 ms =160 ms then frequency is 4*10 =40 ms
         answer=l_k_s.send_command_to_keycon_write(0x02,0x95);  // 0x97 0x81
		 if (answer!=SLAVE_ADRESS_IS_SENT_TWI) return answer;
         limit_holddown=8;
// autorepeat delay is 24 *10 ms =240 ms then frequency is 4*10 =40 ms
//        l_k_s.send_command_to_keycon_write(0x02,0x82);
//        limit_holddown=16;

        l_k_s.send_stop_twi();
//
        answer=l_k_s.send_slave_adress_to_keycon(0x70);
		if (answer!=SLAVE_ADRESS_IS_SENT_TWI) return answer;
        answer=l_k_s.send_adress_for_read_from_keycon(0x00);
        l_k_s.send_stop_twi();
if (answer!=SLAVE_ADRESS_IS_SENT_TWI) return answer; 
else return O_KEY_LED_TWI;

}



void  Cled_key_sound::to_switch_on_PWM01_TMS429(void) {
//31.01.2017 bit 7 max7349 to one(high impedance)
//It takes 210 mks from call to return
//  pwm_on_off =pwm_on_off|0x80;
//  send_slave_adress_to_keycon(0x70);
//  send_command_to_keycon_write(0x05,pwm_on_off);
//  send_stop_twi();
//  to_restore_adress_for_to_read();
}
void  Cled_key_sound::to_switch_off_PWM01_TMS429(void) {
//31.01.2017 bit 7 max7349 to zero
//It takes 210 mks from call to return
  pwm_on_off =pwm_on_off&0x7f;
  send_slave_adress_to_keycon(0x70);
  send_command_to_keycon_write(0x05,pwm_on_off);
  send_stop_twi();
  to_restore_adress_for_to_read();
}
void  Cled_key_sound::to_switch_on_PWM02_TMS429(void) {
//31.01.2017 bit 6 max7349 to one(high impedance)
//It takes 210 mks from call to return
  pwm_on_off =pwm_on_off|0x40;
  send_slave_adress_to_keycon(0x70);
  send_command_to_keycon_write(0x05,pwm_on_off);
  send_stop_twi();
  to_restore_adress_for_to_read();
}
void  Cled_key_sound::to_switch_off_PWM02_TMS429(void) {
//31.01.2017 bit 6 max7349 to zero
//It takes 210 mks from call to return
  pwm_on_off =pwm_on_off&0xbf;
  send_slave_adress_to_keycon(0x70);
  send_command_to_keycon_write(0x05,pwm_on_off);
  send_stop_twi();
  to_restore_adress_for_to_read();
}



TWI_TALK  Cled_key_sound::read_key_code(unsigned char *scan_code) {
unsigned char answer1;
 TWI_TALK state_local;
     char ans;
     state_local=read_from_keycon(&ans);
      send_stop_twi_read();

	 // while(!(USARTD0.STATUS & USART_DREIF_bm) );
	 //	USARTD0.DATA = ans;

     if(state_local!=DATA_IS_READ_TWI) {
		*scan_code=0x00;
		return state_local;
     }

     answer1=ans&0x40;
     if(answer1==0x00){
		if ( ans!=0x00 && ans<64 )
			state_local= KEY_IS_PRESSED;
        else  
            state_local= KEY_IS_NOT_PRESSED ;
     }
//-----------------------------------
     if(state_local==KEY_IS_PRESSED ){
        if(key_follower==0) 
           state_key=START_KEY_SEQUENCE;
	 }
//-----------------------------
     switch (state_key) {
		case  START_KEY_SEQUENCE:
			key_follower++;
			memory_key_code=ans;
			break;
		case WAIT_HOLDDOWN:
			key_follower++;
			if ( key_follower<=limit_holddown) {
				 if(state_local==KEY_IS_PRESSED && key_follower>6) {
					 state_key=HOLDDOWN;
					 key_follower_next=9;
					 }
				}
				else 
					state_key=RELEASE;
			break;
		case HOLDDOWN:{
			 key_follower_next--;
			if(state_local==KEY_IS_PRESSED ) {
				key_follower_next=9;
				break;
			}
			if (key_follower_next<=0) {
				state_key=RELEASE;
				break;
			}
			}
		default:
		break;
     }
//--------------
     switch (state_key){
	 case  START_KEY_SEQUENCE:
		state_key=WAIT_HOLDDOWN;
		*scan_code= ans;
		 return KEY_IS_PRESSED;
     case WAIT_HOLDDOWN :
		*scan_code=0;
		return EMPTY_CODE;
     case HOLDDOWN :
		*scan_code=memory_key_code;
		 return KEY_IS_HOLD;
     case ERROR_KEY:
		*scan_code=0;
		key_follower=0;
		return EMPTY_CODE;
     case RELEASE:
		*scan_code=memory_key_code;
		key_follower=0;
		state_key= ERROR_KEY;
		return KEY_IS_RELEASED;
     default:
		 state_key= ERROR_KEY;
		*scan_code=0;
		key_follower=0;
		return EMPTY_CODE;
}
// return state_key;
}














/*



#define IS_LED(number_led)(\
                            (number_led < 4)  ? ((1 << number_led) & led[0]) :\
                                    (number_led < 8)  ? ((1 << number_led - 4) & led[1]) :\
                                    (number_led < 12) ? ((1 << number_led - 8) & led[2]) :\
                                    (number_led < 16) ? ((1 << number_led - 12) & led[3]) : 0\
#define OFF_ALL_LED() {led[0] = led[1] = led[2] = led[3] = 0;}
#define ON_ALL_LED() {led[0] = led[1] = led[2] = led[3] = 0xf;}
*/
bool  Cled_key_sound::IS_LED(unsigned char number_led){
                          if  (number_led < 4) {
                              if  ((1 << number_led) & led[0]) return true;
                              else return false;
                                                }
                          if  (number_led < 8) {
                              if  ((1 << (number_led-4)) & led[1]) return true;
                              else return false;
                                                }
                          if  (number_led < 12) {
                              if  ((1 << (number_led-8)) & led[2]) return true;
                              else return false;
                                                }
                          if  (number_led < 16) {
                              if  ((1 << (number_led-12)) & led[3]) return true;
                              else return false;
                                                }
                           return false;}


void Cled_key_sound::ON_LED(char number_led){

                   if(number_led < 4){
                        led[0] |= ( 1 << number_led );
                    }
                    else if(number_led < 8){
                        led[1] |= ( 1 << (number_led - 4) );
                    }
                    else if(number_led < 12){
                        led[2] |= ( 1 << (number_led - 8) );
                    }
                    else if(number_led < 16){
                        led[3] |= ( 1 << (number_led - 12) );
                    };

}

void Cled_key_sound::OFF_LED(char  number_led){
                    if(number_led < 4){
                        led[0] &= ~(1 << number_led);
                    }
                    else if(number_led < 8){
                        led[1] &= ~(1 << (number_led - 4));
                    }
                    else if(number_led < 12){
                        led[2] &= ~(1 << (number_led - 8));
                    }
                    else if(number_led < 16){
                        led[3] &= ~(1 << (number_led - 12));
                    };
}
//11.04.2016 set sound
void  Cled_key_sound::set_sound(void){
       l_k_s.send_slave_adress_to_keycon(0x70);
 // sounder register: LEVEL (D1=1),  DURATION 125ms (D7=1,D6=0,D5=0)
       l_k_s.send_command_to_keycon_write(0x06,0x82);
       l_k_s.send_stop_twi();
//
        l_k_s.send_slave_adress_to_keycon(0x70);
        l_k_s.send_adress_for_read_from_keycon(0x00);
        l_k_s.send_stop_twi();
}
//11.04.2016 delete sound
void  Cled_key_sound::delete_sound(void){
       l_k_s.send_slave_adress_to_keycon(0x70);
       l_k_s.send_command_to_keycon_write(0x06,0x00);
       l_k_s.send_stop_twi();
//
        l_k_s.send_slave_adress_to_keycon(0x70);
        l_k_s.send_adress_for_read_from_keycon(0x00);
        l_k_s.send_stop_twi();
}
void Cled_key_sound:: OFF_ALL_LED(void) {led[0] = led[1] = led[2] = led[3] = 0;}

unsigned char  Cled_key_sound::read_from_realtime_clock(unsigned char *received_data,char n_bytes){
//char slave_adr=0x70;
char state,arbitration_lost,bus_error;
//char read_interrupt_flag;
//char write_interrupt_flag;
//char received_acknowledge;
//bool error=false;
//bool timeout=false;
char clock_hold;
int n=0;

 TWIC.MASTER.ADDR=0xa1;

for(int counter=0;;counter++) {
     state=TWIC.MASTER.STATUS;
     arbitration_lost = state&0x08;
     bus_error = state&0x04;
     clock_hold=state&0x20;
     if ( arbitration_lost==0x08){
      return ARBITRATION_LOST_TWI;
                                 }
     if ( bus_error==0x04) {
      return BUS_ERROR_TWI;
                           }
      if (clock_hold==0x20){
         break;
     }
  if (counter>=500){
     return TIMEOUT_TWI;
     }
 }
  unsigned char answer= get_acknowledge_twi();
         if(answer!=SLAVE_ADRESS_IS_SENT_TWI)return answer;

//         received_data[0]=TWIC.MASTER.DATA;

//30.03.2015 when adress is sent noone flag is set
// acknowledge clock is generated and 8 clock followed
// data from slave is coming  clock line goes to low and clock_hold is set
// when data is read  hight level acknowledge  is sent with stop
 for(int counter=0;;counter++) {
     state=TWIC.MASTER.STATUS;
     arbitration_lost = state&0x08;
     bus_error = state&0x04;
     clock_hold=state&0x20;
//     read_interrupt_flag=read_interrupt_flag&0x80;
//     write_interrupt_flag=state&0x40;
     if ( arbitration_lost==0x08){
//      error=false;
//      break;
      return ARBITRATION_LOST_TWI;
                                 }
     if ( bus_error==0x04) {
      return BUS_ERROR_TWI;
                           }
//     if (write_interrupt_flag==0x40){
//      error=true;
//      break;
//           }
// if (read_interrupt_flag==0x80){
//      error=true;
//      break;
//           }
      if (clock_hold==0x20){
//if (read_interrupt_flag==0x80){
     counter=0;
     received_data[n++]=TWIC.MASTER.DATA;
     if(n>=n_bytes) {
//     TWIC.MASTER.CTRLC=0x00;
     send_stop_twi_read();
         break;
     }
//it is acknowledge
     TWIC.MASTER.CTRLC=0x02;
//      error=true;
     }
// if (counter>counter_max) {
//      counter_max=counter;
//  }
  if (counter>=500){
//      error=false;
//      timeout=true;
//      break;
     return TIMEOUT_TWI;
 }

 }

//*received_data=TWIC.MASTER.DATA;
return DATA_IS_READ_TWI;

}

unsigned char  Cled_key_sound::write_to_realtime_clock(unsigned char start_adress,
                                               unsigned char *buf_to_write,char n_bytes){
//char slave_adr=0x70;
//char state,arbitration_lost,bus_error;
//char read_interrupt_flag;
//char write_interrupt_flag;
//char received_acknowledge;
//bool error=false;
//bool timeout=false;
//char clock_hold;
int n=0;
unsigned char answer;
unsigned char byte_to_send;
bool start=true;
// TWIC.MASTER.DATA=0xa0;
//30.03.2015 when adress is sent noone flag is set
// acknowledge clock is generated and 8 clock followed
// data from slave is coming  clock line goes to low and clock_hold is set
// when data is read  hight level acknowledge  is sent with stop
// for(int counter=0;;counter++) {
//     state=TWIC.MASTER.STATUS;
//     arbitration_lost = state&0x08;
//     bus_error = state&0x04;
//     clock_hold=state&0x20;
//     if ( arbitration_lost==0x08){
//      return ARBITRATION_LOST_TWI;
//                                 }
//     if ( bus_error==0x04) {
//      return BUS_ERROR_TWI;
//                           }
//      if (clock_hold==0x20){
//         break;
//     }
//  if (counter>=500){
//     return TIMEOUT_TWI;
//     }
// }
   for (int counter=0;;counter++) {
            if(start==true){byte_to_send=start_adress;start=false;}
            else {byte_to_send=buf_to_write[n++];}
            answer=send_byte_to_realtime_clock(byte_to_send);
              if(n>=n_bytes) {
              break;
                   }
            if (answer!=DATA_IS_WRITTEN_TWI) return (unsigned char) answer;
            answer= get_acknowledge_twi();
            if(answer!=SLAVE_ADRESS_IS_SENT_TWI)return (unsigned char) answer;
         }
return  (unsigned char)answer;
}

unsigned char Cled_key_sound::send_byte_to_realtime_clock(unsigned char byte_to_send) {
char state,arbitration_lost,bus_error;
char write_interrupt_flag;

    TWIC.MASTER.DATA=byte_to_send;

 for(int counter=0;;counter++) {
     state=TWIC.MASTER.STATUS;
     arbitration_lost = state&0x08;
     bus_error = state&0x04;
     write_interrupt_flag=state&0x40;
     if ( arbitration_lost==0x08) return ARBITRATION_LOST_TWI;
     if ( bus_error==0x04) return BUS_ERROR_TWI;
     if (write_interrupt_flag==0x40) break;
//100 is 65 mks
  if (counter>=250) return TIMEOUT_TWI;
}

return DATA_IS_WRITTEN_TWI;

}
void Cled_key_sound::to_talk_with_realtime_clock_device(unsigned char *buf){
//switch_on_100_kgh();
switch_on_295_kgh();
set_state_to_idle_twi();
// set_state_to_idle_twi_realtime_clock();
// unsigned char answer=0;
// unsigned char answer1=0;
// unsigned char answer2=0;
 unsigned char buf1[20];
// char data=0x02;
      for (int n=0;n<16;n++) {
      buf1[n]=buf[n];

      }
     // buf1[0]=0xaa;

//17.03.2017 it takes 120 mks (only adrress)
//0xa0 means write

//      init.set_A2();
//      answer=send_slave_adress_to_keycon(0xa0);
//      answer1=write_to_realtime_clock(0x10,buf1,16);
      send_stop_twi();
      delay_mks(31);//__delay_cycles(1000);
//      init.clr_A2();


//      answer2=read_from_realtime_clock(buf1,8);
//      answer2=read_from_realtime_clock(buf1,8);

//      send_stop_twi();
/**/
  for (int n=0;n<16;n++) {
      buf1[n]=0xff;
      }
//      __delay_cycles(1000);
//    init.set_A2();
//      answer=send_slave_adress_to_keycon(0xa0);
//      answer1=send_adress_for_read_from_keycon(0x10);
//    init.clr_A2();
     send_stop_twi();
     delay_mks(31);//__delay_cycles(1000);

//     init.set_A2();
//      answer2=read_from_realtime_clock(buf1,16);
//     init.clr_A2();
//      send_stop_twi();
     delay_mks(31);//__delay_cycles(1000);

          for (int n=0;n<16;n++) {
      buf[n]=buf1[n];
      }

//      answer=send_slave_adress_to_keycon_for_to_read(0xa1);
//      answer1=send_adress_for_read_from_keycon(0x08);
//      answer2=read_from_realtime_clock(buf1,1);

//      send_stop_twi();
//      buf[0]=(unsigned char) answer;
//      buf[1]=(unsigned char) answer1;
//      buf[2]=(unsigned char) answer2;
//      buf[3]=buf1[0];
//      buf[4]=buf1[1];
//      buf[5]=buf1[2];
//      buf[6]=buf1[3];
//      buf[7]=buf1[4];
//     buf[8]=buf1[5];
//      buf[9]=buf1[6];
//      buf[10]=buf1[7];
//      buf[11]=buf1[8];

switch_on_295_kgh();
//init_keycon();
set_state_to_idle_twi();
// to_restore_adress_for_to_read();

//  __delay_cycles(1000);

// set_state_to_idle_twi_realtime_clock();
//return answer;


}
unsigned char Cled_key_sound::write_to_realtime(unsigned char *buf,
                                                           unsigned char address,char num){
//switch_on_100_kgh();
//switch_on_295_kgh();
//set_state_to_idle_twi();
// set_state_to_idle_twi_realtime_clock();
// unsigned char buf1[20];
//      for (char n=0;n<16;n++) {
//      buf1[n]=buf[n];
//      }
//17.03.2017 it takes 120 mks (only address)
//0xa0 means write
//SLAVE_ADRESS_IS_SENT_TWI
//DATA_IS_WRITTEN_TWI
//      init.set_A2();
unsigned char answer;
      answer=send_slave_adress_to_keycon(0xa0);
      if (answer==SLAVE_ADRESS_IS_SENT_TWI) {
//           answer=write_to_realtime_clock(0x10,buf,16);
        answer=write_to_realtime_clock(address,buf,num);
      }
      send_stop_twi();
      delay_mks(31);//__delay_cycles(1000);
      return answer;
//switch_on_295_kgh();
//init_keycon();
//set_state_to_idle_twi();
// to_restore_adress_for_to_read();

//  __delay_cycles(1000);

// set_state_to_idle_twi_realtime_clock();
//return answer;
}
unsigned char Cled_key_sound::to_read_counters_from_realtime(unsigned char *buf,
                             unsigned char address,char num){
// unsigned char buf1[20];
unsigned char answer;
//  for (char n=0;n<16;n++) {
//      buf1[n]=0xff;
//      }
//      __delay_cycles(1000);
//    init.set_A2();
      answer=send_slave_adress_to_keycon(0xa0);
      if (answer==SLAVE_ADRESS_IS_SENT_TWI) {
      answer=send_adress_for_read_from_keycon(address);
      }
//    init.clr_A2();
     send_stop_twi();
     delay_mks(31);//__delay_cycles(1000);
     if (answer!=SLAVE_ADRESS_IS_SENT_TWI) return answer;

//     init.set_A2();
     answer=read_from_realtime_clock(buf,num);
//     init.clr_A2();
//      send_stop_twi();
     delay_mks(31);//__delay_cycles(1000);
     return answer;
//          for (char n=0;n<16;n++) {
//      buf[n]=buf1[n];
//      }
}
unsigned char Cled_key_sound::to_read_time_from_realtime_clock_device(unsigned char *buf){
unsigned char answer;
    answer=send_slave_adress_to_keycon(0xa0);
    if (answer==SLAVE_ADRESS_IS_SENT_TWI) {
      send_adress_for_read_from_keycon(0x01);
    }
//    init.clr_A2();
     send_stop_twi();
     delay_mks(31);//__delay_cycles(1000);
    if (answer!=SLAVE_ADRESS_IS_SENT_TWI) return answer;
//     init.set_A2();
    answer=read_from_realtime_clock(buf,6);
//     init.clr_A2();
//      send_stop_twi();
    delay_mks(31);//__delay_cycles(1000);
    return answer;
}
unsigned char Cled_key_sound::to_setup_time_to_realtime_clock_device(unsigned char *buf){
unsigned char answer;
//stop count
    buf[0]=0x80;
//27.03.2017 only for debug
//it must be done in another place
    for(int n=1;n<7;n++) {
    buf[n]=0x00;
    }
//------------------------
      answer=send_slave_adress_to_keycon(0xa0);
     if (answer==SLAVE_ADRESS_IS_SENT_TWI) {
      write_to_realtime_clock(0x0,buf,7);
      }
      send_stop_twi();
      delay_mks(31);//__delay_cycles(1000);
  if (answer!= DATA_IS_WRITTEN_TWI) return answer;
//start count
    buf[0]=0x0;
    answer=send_slave_adress_to_keycon(0xa0);
    if (answer==SLAVE_ADRESS_IS_SENT_TWI) {
      answer=write_to_realtime_clock(0x0,buf,1);
      }
      send_stop_twi();
      delay_mks(31);//__delay_cycles(1000);
     return answer;
}
TWI_TALK Cled_key_sound::change_contrast(unsigned char contrast_value){
//unsigned char answer;
 TWI_TALK answer;
      answer=send_slave_adress_to_keycon(0x5c);
     if (answer==SLAVE_ADRESS_IS_SENT_TWI) {
      send_byte_to_realtime_clock(contrast_value);
      }
      send_stop_twi();
     // delay_mks(31);//__delay_cycles(1000);
 return answer;
}
Cled_key_sound::REALTIME_CLOCK Cled_key_sound::work_with_time(void){
	//      SendAddresMsek  0
	//      ReadMsek        1
	//      SendAddresMin   2
	//      ReadMin         3
	//      SendAddresHour  4
	//      ReadHour        5
	//      SendAddresDay   6
	//      ReadDay         7
	//      SendAddresMonth 8
	//      ReadMonth       9

	// init.set_B5();
	//     l_k_s.to_read_counters_from_realtime((unsigned char *)&l_k_s.r_t_save_ref,0x30,1);
	//     l_k_s.to_setup_time_to_realtime_clock_device(l_k_s.buf_for_real_time_clock);
	// init.clr_B5();
	//   unsigned char answer;

	switch(l_k_s.follower) {
		case SETUP_TIME:
		l_k_s.write_to_realtime((unsigned char *)&l_k_s.setup_time,0x0,7);
		l_k_s.follower=SETUP_YEAR;
		break;
		case SETUP_YEAR:
		l_k_s.write_to_realtime((unsigned char *)&l_k_s.year,0x40,2);
		l_k_s.follower=SendAddressSek;
		break;
		case SendAddressSek:
		to_send_address_to_PCF8583(0x02);
		l_k_s.follower=ReadSek;
		break;
		case ReadSek:
		to_read_bytes_from_PCF8583(real_time,1);
		l_k_s.follower=ReadMin;
		//break;//-
		case ReadMin:
		to_read_bytes_from_PCF8583(real_time+1,1);
		l_k_s.follower=ReadHour;
		//break;//-
		case ReadHour:
		to_read_bytes_from_PCF8583(real_time+2,1);
		l_k_s.follower=ReadYearDay;
		break;
		//case SendAddressDay:
		//to_send_address_to_PCF8583(0x04);
		//l_k_s.follower=ReadDay;
		//break;
		case ReadYearDay:
		to_read_bytes_from_PCF8583(real_time+3,1);
		//l_k_s.follower=SendAddressMonth;
		l_k_s.follower=ReadWeekdaysMonth;
		break;
		//case SendAddressMonth:
		//to_send_address_to_PCF8583(0x05);
		//l_k_s.follower=ReadMonth;
		//break;
		case ReadWeekdaysMonth:
		to_read_bytes_from_PCF8583(real_time+4,1);
		//l_k_s.follower=SendAddressMsek;
		l_k_s.follower=CONTRAST_CAN_BE_CHANGED;
		break;
		case CONTRAST_CAN_BE_CHANGED:
		l_k_s.follower=SendAddressSek;
		// l_k_s.follower=WRITE_COUNTERS;
		break;
		case WRITE_COUNTERS :
		l_k_s.follower=SendAddressSek;
		break;
		case SendAddressHour:
		case SendAddressMin:
		case SendAddressDay:
		case SendAddressMonth:
		case ReadMonth:
		case ReadDay:
		break;
	}
	return l_k_s.follower;
}

unsigned char Cled_key_sound::to_send_address_to_PCF8583(unsigned char address){
	// unsigned char buf1[20];
	unsigned char answer;
	//  for (char n=0;n<16;n++) {
	//      buf1[n]=0xff;
	//      }
	//      __delay_cycles(1000);
	//    init.set_B6();
	answer=send_slave_adress_to_keycon(0xa0);
	if (answer==SLAVE_ADRESS_IS_SENT_TWI) {
		answer=send_adress_for_read_from_keycon(address);
	}
	//    init.clr_B6();
	send_stop_twi();

	//     __delay_cycles(1000);
	if (answer!=SLAVE_ADRESS_IS_SENT_TWI) return answer;
	return answer;
}
unsigned char Cled_key_sound::to_read_bytes_from_PCF8583(unsigned char *buf,char num){
	unsigned char answer;
	//     init.set_B6();
	answer=read_from_realtime_clock(buf,num);
	//     init.clr_B6();
	//      send_stop_twi();
	//     __delay_cycles(1000);
	return answer;
	//          for (char n=0;n<16;n++) {
	//      buf[n]=buf1[n];
	//      }
}
void Cled_key_sound::time_string_with_seconds(unsigned char *incoming,char *output){
	char tmp[2];
	convertBCDtoASCII(incoming[0],tmp);
	output[5]=0x3a;
	output[6]=tmp[0];
	output[7]=tmp[1];
	//output[7]=0x30;
	output[8]=0;
	convertBCDtoASCII(incoming[1],tmp);
	output[2]=0x3a;
	output[3]=tmp[0];
	output[4]=tmp[1];
	convertBCDtoASCII(incoming[2],tmp);
	output[0]=tmp[0];
	output[1]=tmp[1];
}
void Cled_key_sound::time_string_without_seconds(unsigned char *incoming,char *output){
	char tmp[2];
	unsigned char a;
	convertBCDtoASCII(incoming[1],tmp);
	output[3]=tmp[0];
	output[4]=tmp[1];
	output[5]=0;
	convertBCDtoASCII(incoming[2],tmp);
	output[0]=tmp[0];
	output[1]=tmp[1];
	a=incoming[0]&0x01;
	if(a)
	output[2]=0x3a;
	else output[2]=0x20;

}


void Cled_key_sound::convertBCDtoASCII(unsigned char bcd, char * ascii)
{
	//convert from BCD to ASCII for LIR display
	unsigned char ubcd;
	ubcd = bcd;

	if (ubcd >= 0x10)
	{
		ascii[0] = (ubcd >> 4) | 0x30;
		ascii[1] = (ubcd << 4);
		ascii[1] = ((unsigned char)ascii[1] >> 4) | 0x30;
		return;
	}
	if (ubcd < 0x10)
	{
		ascii[1] = ubcd + 0x30;
		ascii[0] = 0x30;
		return;
	}

}

void Cled_key_sound::day_and_month_string(unsigned char *incoming,char *output){
	//20 ноября 2017
	//unsigned char day=l_k_s.real_time_to_see[3];
	//unsigned char month=l_k_s.real_time_to_see[4];
	//  unsigned char month=l_k_s.real_time_to_see[6]&0x1f;
	char tmp[2];
	convertBCDtoASCII(incoming[3]&0x3f,tmp);
	output[1]=tmp[1];
	output[0]=tmp[0];
	output[2]=0x2e;
	convertBCDtoASCII(incoming[4]&0x1f,tmp);
	output[4]=tmp[1];
	output[3]=tmp[0];
	output[5]=0x0;
}
/*
PCF8583 RAM size 256 байт
0x00 - 0x0F зарезервированы самим чипом
0x10 - 0xFF для пользователя
*/
void  Cled_key_sound::read_RAM_PCF8583(void){
	l_k_s.to_read_counters_from_realtime((unsigned char *)&l_k_s.r_t_save_rel,0x10,16);
	l_k_s.to_read_counters_from_realtime((unsigned char *)&l_k_s.r_t_save_abs,0x20,16);
	l_k_s.to_read_counters_from_realtime((unsigned char *)&l_k_s.r_t_save_ref,0x30,5);
	l_k_s.to_read_counters_from_realtime((unsigned char *)&l_k_s.year,0x40,2);
	l_k_s.to_read_counters_from_realtime((unsigned char *)&Lir.R_off,0x50,16);
}
void Cled_key_sound::to_set_false_for_is_it_saved_by_finger_press(void){
	l_k_s.r_t_save_ref.is_it_saved_by_finger_press=false;
	l_k_s.write_to_realtime((unsigned char *)&l_k_s.r_t_save_ref,0x30,5);
}

