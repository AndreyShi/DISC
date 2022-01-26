
#include "io_altera.h"
#include "led_key_sound.h"
#include "display.h"
#include "counter.h"
#include "MenuLir.h"
//17.05.2018
//#include "spi.h"
#include "at45.h"
#include "keyfunc.h"

//
//CIOAltera altera;
//CPorta_ref ref_;
//CAnalog_to_digital_conversion atdc;
extern "C"{
	char read_counter_step_0(char latch_cnt,char addr);
	char read_counter_step_1(char addr);
	char read_counter_step_2(char addr); 
}
//#pragma GCC push_options
//#pragma GCC optimize ("O0")
long CIOAltera::read_cnt_quick(char channel){
//07.03.2018 it takes 6 mks from call to return	
	  long counter = 0;//=0;
	  char *buf;
	  buf=(char *)&counter;
	  //buf[3] = 0;
	  char h,m,l,t;
	  //&tmp=counter;
	  t=READ_TOP_CNT0;
	  switch(channel){
		  //===============счeтчики альтеры 0 - 3 канал================
		  case 0:h=READ_HIGH_CNT0;m=READ_MID_CNT0;l=READ_LOW_CNT0;t=READ_TOP_CNT0;break;
		  case 1:h=READ_HIGH_CNT1;m=READ_MID_CNT1;l=READ_LOW_CNT1;t=READ_TOP_CNT1;break;
		  case 2:h=READ_HIGH_CNT2;m=READ_MID_CNT2;l=READ_LOW_CNT2;t=READ_TOP_CNT2;break;
		  case 3:h=READ_HIGH_CNT3;m=READ_MID_CNT3;l=READ_LOW_CNT3;t=READ_TOP_CNT3;break;
		  //============================================================
		  case 4:h=READ_ETALON_HIGH;m=READ_ETALON_MID;l=READ_ETALON_LOW;break;
		  case 5:h = 21;m = 20;l = 19;break; //альтера счетчик от метки до метки 0 канал
		  case 6:h = 24;m = 23;l = 22;break; //альтера счетчик от метки до метки 1 канал
		  case 7:h = 27;m = 26;l = 25;break; //альтера счетчик от метки до метки 2 канал
		  case 8:h = 30;m = 29;l = 28;break; //альтера счетчик от метки до метки 3 канал
		  default:
		  return 0;
	  }
	  cli();
       //buf[2]= read_counter_step_0( WRITE_ADR::LATCH_CNT,h);
	 if(channel >= 0 && channel <= 3){
		if(Lir.abs_ENC[(unsigned)channel])		
			buf[3] = read_reg((READ_ADR)t);//24..31bit
	 }

	  if(!latch)
		 PORTE.OUT=LATCH_CNT;
	  buf[2] = read_reg((READ_ADR)h); //16..23bit
	  buf[1] = read_reg((READ_ADR)m); //8..15bit
	  buf[0] = read_reg((READ_ADR)l); //0..7bit
	  sei();
	  return counter;
  }
 // #pragma GCC pop_options
#pragma GCC push_options
#pragma GCC optimize ("O0")  
unsigned char  CIOAltera::read_input(READ_INPUT adr){

	#if (configUSE_mcx514 == 1)
	return 0;
	#endif

	PORTR.OUTSET=0x01;
	PORTE.OUT=adr;
	PORTE.DIR=0xff;
	PORTR.OUTCLR=0x02;
	PORTR.OUTSET=0x02;
	PORTE.DIR=0x00;
	PORTR.OUTCLR=0x01;
	PORTR.OUTCLR=0x02;
	unsigned char data = PORTE.IN;
	PORTR.OUTSET=0x01;
	PORTE.DIR=0xff;
	return data;		
}

void CIOAltera::to_send_outputs(unsigned char *out)
{
	#if (configUSE_mcx514 == 1)
	return;
	#endif

	unsigned char high=out[2];
	unsigned char mid=out[1];
	unsigned char low=out[0];
	write_reg(WRITE_ALTERA::WRITE_OUTPUT_0_7,low);
	write_reg(WRITE_ALTERA::WRITE_OUTPUT_8_15,mid);
	write_reg(WRITE_ALTERA::WRITE_OUTPUT_16_23,high);
}

void CIOAltera::set_ch_abs_bit(int ch,char data)
{
	WRITE_ALTERA cmd;

	if(ch == 0)
		cmd = WRITE_ALTERA::BIT_ABS_0;
	else if(ch == 1)
		cmd = WRITE_ALTERA::BIT_ABS_1;
	else if(ch == 2)
		cmd = WRITE_ALTERA::BIT_ABS_2;
	else if(ch == 3)
		cmd = WRITE_ALTERA::BIT_ABS_3;
	else 
		return;
	cli();
	write_reg(cmd,data);
	sei();
}
/*
функция для записи в регистры альтеры данных
WRITE_ALTERA addr - адрес регистра альтеры
unsigned char data - данные
*/
void CIOAltera::write_reg(WRITE_ALTERA addr,unsigned char data)
{
	//char *a=0;
	char a[1];
	//alt_rw=1 data line from xmega to altera
	PORTR.OUTSET=0x01;
	//address to data line
	PORTE.OUT=addr;
	//set data line to output
	PORTE.DIR=0xff;
	//it is delay 15 cycles
	to_take_address(a);
	
	//write address to altera
	PORTR.OUTCLR=0x02;
	PORTR.OUTSET=0x02;
	//	for(int n=0;n<100;n++){
	//		to_take_address(a);
	//	}
	//data to data line
	PORTE.OUT = data;//
	//it is delay 15 cycles
	to_take_address(a);
	//	for(int n=0;n<100;n++){
	//	to_take_address(a);
	//	}
	// write data to altera
	PORTR.OUTCLR=0x02;
}

char CIOAltera::read_reg(READ_ADR addr)
{
	char res;
	PORTR.OUTSET=0x01;
	PORTE.DIR=0xff;
	PORTR.OUTCLR=0x02;
	PORTR.OUTSET=0x02;
	PORTE.OUT=0x01;
	PORTR.OUTCLR=0x02;
	PORTE.OUT=addr;
	PORTR.OUTCLR=0x02;
	PORTR.OUTSET=0x02;
	PORTE.OUT=0x00;
	PORTE.DIR=0x00;
	PORTR.OUTCLR=0x01;
	PORTR.OUTCLR=0x02;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	res = PORTE.IN;
	PORTR.OUTSET=0x01;
	PORTE.DIR=0xff;
	return res;
}

#pragma GCC pop_options
//10.04.2018 
//#pragma GCC push_options
//#pragma GCC optimize ("O0")
CIOAltera::CORRECTION_IS_WORKING  CIOAltera::correction_is_working_right(void){
//24.05.2018 it works correct only if  k of equalization was set up.
// k of equalization. In one value of length must be the one number of discrete
// for etalon and working channel.  	
	//15.04.2018 worst case it takes...1365*31.25=42656,25 ns
	long k=(long)ref_.k_divider;
	long interval,half_interval;
	long delta;
	//11.05.2018
	long result_long,compare;
	//-----------
	long counter=mCounter[1].abs;
	long up,down;
	up=down=0;
	bool first_element;
	if(k<0)k=-k;
	interval=k;
	half_interval=interval/2;
	if(counter<0)counter=-counter;
	if(k==0)return CORRECTION_IS_WORKING::INTERVAL_IS_ZERO;
	
	k=counter/k;
	//first check
	//if(k>=ref_.set_point) return CORRECTION_IS_WORKING::TABLE_IS_ENDED;
	switch(k){
		case 0:
		down= 0;
		//up=ref_.corr.time_to_think[0];
		first_element=true;
		break;
		default:
		//down= ref_.corr.time_to_think[k-1];
		//up=ref_.corr.time_to_think[k];
		first_element=false;
		break;
	}
	//second check
	if(mCounter[1].abs>=0){
		if(up>=0 && down>=0);
		else return CORRECTION_IS_WORKING::SIGN_PLUS_WRONG;
	}
	else{
		if(up<=0 && down<=0){up=-up;down=-down;}
		else return CORRECTION_IS_WORKING::SIGN_MINES_WRONG;
	}
//14.05.2018 it is only check
	delta=up-down;
	if(delta<=0)return CORRECTION_IS_WORKING::DELTA_BELOW_ZERO;
//----------------------------
	if(first_element==true){
	 if(counter<=half_interval)	delta=0;
	 else{ 
		delta=up-interval;
	 }	
	 }
	else{
		result_long=interval*k;
		compare=result_long+half_interval;
		if(counter<=compare){
		delta=down-result_long;	
		}
		else{
		delta=up-(result_long+interval);		
		}
	}
//23.05.2018	
	result_long=counter+delta;
	mCounter[1].correction_right=counter+delta;
	if(mCounter[1].abs<0) mCounter[1].correction_right=-mCounter[1].correction_right;
//--------------------------------------------------------------------------	
	return CORRECTION_IS_WORKING::OK;
}
CIOAltera::CORRECTION_IS_WORKING  CIOAltera::correction_is_working_complex(void){
//15.04.2018 worst case it takes 10896*31.25=340500 ns.	
	long k=(long)ref_.k_divider;
	long interval;
	long delta;
	//11.05.2018
	long long result;
	long result_long;
	//-----------
	long counter=mCounter[1].abs;
	long up,down;
	up=down=0;
	unsigned char dot;
	bool first_element;
	if(k<0)k=-k;
	interval=k;
	if(counter<0)counter=-counter;
	if(k==0)return CORRECTION_IS_WORKING::INTERVAL_IS_ZERO;
	
	k=counter/k;
	//first check
	//if(k>=ref_.set_point) return CORRECTION_IS_WORKING::TABLE_IS_ENDED;
	switch(k){
		case 0:
		down= 0;
		//up=ref_.corr.time_to_think[0];
		first_element=true;
		break;
		default:
		//down= ref_.corr.time_to_think[k-1];
		//up=ref_.corr.time_to_think[k];
		first_element=false;
		break;
	}
	//second check
	if(mCounter[1].abs>=0){
		if(up>=0 && down>=0);
		else return CORRECTION_IS_WORKING::SIGN_PLUS_WRONG;
	}
	else{
		if(up<=0 && down<=0){up=-up;down=-down;}
		else return CORRECTION_IS_WORKING::SIGN_MINES_WRONG;
	}
	//coefficient must be defined for up, and for down
//14.05.2018 it is only check	
	delta=up-down;
	if(delta<0)return CORRECTION_IS_WORKING::DELTA_BELOW_ZERO;
//----------------------------	
	//dot=coefficient_is_calculating_shot(&result,delta,interval);
	if(first_element==true){
	coefficient_is_calculating_long(&result,up,interval);
	if(result>2147483647 || result<50000000)
	return CORRECTION_IS_WORKING::RESULT_IS_UNREAL;
	result_long=(long)result;	
	}
	else{	
	delta=interval*k;
	coefficient_is_calculating_long(&result,down,delta);
    if(result>2147483647 || result<50000000)
    return CORRECTION_IS_WORKING::RESULT_IS_UNREAL;
	down=(long)result;
	delta=interval*(k+1);
	coefficient_is_calculating_long(&result,up,delta);
	if(result>2147483647 || result<50000000)
	return CORRECTION_IS_WORKING::RESULT_IS_UNREAL;
	up=(long)result;
	result_long=(up+down)/2;	
	}
	//26.04.2018 check must be done
	if(result_long==0) return CORRECTION_IS_WORKING::COEFF_IS_ZERO;
    dot=9;
	coeff.set_axis_coeff(4,result_long,dot);
	CDouble::MULT_OR_DIV_ANSWER answer;
	  result_long=mCounter[1].abs;
	//result_long=counter1.abs+2000000000;
	// result=99999999;
	answer=coeff.mult(4,&result_long);
	if (answer!= CDouble::OK){
		return CORRECTION_IS_WORKING::MULT_WRONG;
	}
	mCounter[1].correction_complex=result_long;
	return CORRECTION_IS_WORKING::OK;
}
/*
функция работает "Работа с таблицами"
изначально всегда работала с каналом 1 (Рабочим) в тандеме с каналом Эталон 0
int8_t c - канал
*/
CIOAltera::CORRECTION_IS_WORKING  CIOAltera::correction_is_working_skv(int8_t c){
//24.05.2018 in worst case it takes 5500*31.25=171 825 ns.
//it was checked by oscilloscope 04.06.2018  	
	long k = 0;
	long interval;
	long delta_0,delta_1;
	long result_long;
	long result;
	long counter = mCounter[c].abs;
	long up,down;
	bool first_element;
//25.05.2018
    bool plus = false;
	
	counter = Lir.conv_c_i_c(counter,c, ref_.pRef[c]);

	if(ref_.table_is_ready_plus == false && ref_.table_is_ready_mines == false)
	    return CORRECTION_IS_WORKING::NO_TABLE_PLUS_NO_TABLE_MINES;

    if(counter < 0){ 
		if(ref_.table_is_ready_mines == true){
			plus = false;
			k = (long)ref_.k_divider_mines;
		}
		else if(ref_.table_is_ready_mines==false && ref_.table_is_ready_plus==true){
				return CORRECTION_IS_WORKING::TABLE_IS_ENDED_PLUS_L;   // <
		}
	//else  return CORRECTION_IS_WORKING::TABLE_IS_ENDED_PLUS;
	}
	else if(counter > 0){
		if(ref_.table_is_ready_plus == true){
			plus = true;
			k = (long)ref_.k_divider_plus;
		}
		else if(ref_.table_is_ready_plus == false && ref_.table_is_ready_mines == true){
				return CORRECTION_IS_WORKING::TABLE_IS_ENDED_MINES_R;	  // >
		} 
	}
	else if(counter == 0){
		mCounter[c].correction_simple = 0;	 
		return CORRECTION_IS_WORKING::OK; 	 
	}  
//	}
//-----------------------------------------------------------		
	if(k<0)
		k=-k;

	interval=k;

	if(counter<0)
		counter=-counter;

	if(k==0)
		return CORRECTION_IS_WORKING::INTERVAL_IS_ZERO;
	
	k=counter/k;
	//first check
	if(plus==true){
		if(k>=ref_.set_point_plus) 
			return CORRECTION_IS_WORKING::TABLE_IS_ENDED_PLUS; // >
	 }
	else{
		if(k>=ref_.set_point_mines) 
			return CORRECTION_IS_WORKING::TABLE_IS_ENDED_MINES;	 //<
	}
	switch(k){
		case 0:
		down= 0;
//06.06.2018 to	23LCV1024 10000 points
        if(plus==true)		
			spi.read_table_from_23LCV1024(0,&up,true);
		else 
			spi.read_table_from_23LCV1024(0,&up,false);	 
		first_element=true;
		break;
		default:
		if(plus==true)
			spi.read_table_from_23LCV1024(k-1,&down,true);	// 23LCV1024 SPI Озу память
		else 
			spi.read_table_from_23LCV1024(k-1,&down,false);
		if(plus==true)
			spi.read_table_from_23LCV1024(k,&up,true);
		else 
			spi.read_table_from_23LCV1024(k,&up,false);
		first_element=false;
		break;
	}
	//second check
	if(plus==true){
		if(up>=0 && down>=0);
		else 
		return CORRECTION_IS_WORKING::SIGN_PLUS_WRONG;
	}
	else{
		if(up<=0 && down<=0){
			up=-up;
			down=-down;
			}
		else 
			return CORRECTION_IS_WORKING::SIGN_MINES_WRONG;
	}
	//14.05.2018 it is only check
	delta_0=up-down;
//	if(delta_0<=0)return CORRECTION_IS_WORKING::DELTA_BELOW_ZERO;
//04.05.2018 may be it will be better...
    if(delta_0<0)
		return CORRECTION_IS_WORKING::DELTA_BELOW_ZERO;
	//----------------------------
	if(first_element==true){
		 delta_0=0;
		 delta_1=up-interval;
		 result=coefficient_is_calculating_shot(0,counter,interval);
		 if(result>1000)
			return CORRECTION_IS_WORKING::RESULT_IS_UNREAL;
	}
	else{
		 long a=interval*k;	
		 delta_0=down-a;
	     delta_1=up-(interval*(k+1));
	     delta_1=delta_1-delta_0;
	     result=coefficient_is_calculating_shot(a,counter,interval);
	     if(result>1000)
			 return CORRECTION_IS_WORKING::RESULT_IS_UNREAL;	
	}

	 coeff.set_axis_coeff(4,result,3); 
	 CDouble::MULT_OR_DIV_ANSWER answer;
	 //result_long=counter1.abs;
	 //result_long=counter1.abs+2000000000;
	 // result=99999999;
	 answer=coeff.mult(4,&delta_1);
	 if (answer!= CDouble::OK){
		return CORRECTION_IS_WORKING::MULT_WRONG;	 
	 }
	 result_long=counter+delta_0+delta_1;

	 mCounter[c].correction_simple=result_long;
	 if(plus==false) 
		mCounter[c].correction_simple=-mCounter[c].correction_simple;	 

return CORRECTION_IS_WORKING::OK;    		
}
//#pragma GCC pop_options
long  CIOAltera::coefficient_is_calculating_shot(long down,long counter,long interval){	
long delta=counter-down;
long result;
char dot=0;
         if(delta<0 || delta>interval) 
			return 0;
		 while(1){
			  result=delta/interval;
				  dot++;
				  if(dot>3){break;}
				  delta=delta*10;
		  }
return result;		  
}
unsigned char  CIOAltera::coefficient_is_calculating_long(long long *d,long del,long interval){
	unsigned char dot=0;
	long long result=*d;
	long long delta=del;
	while(1){
		result=delta/interval;
		//if(result*interval==delta) break;
		//else {
			dot++;
			if(dot>9){dot--;break;}
			delta=delta*10;
		//}
	}
	*d=result;
	return dot;
}

unsigned char  CIOAltera::correction_is_taking(unsigned int *switch_number_for_ref,unsigned long *input){
//04.06.2018 worst case it takes 150 mks from call to return.
cli();
bool enable_to_go_for_correction=ref_.enable_to_go_for_correction;
ref_.follower_safety=ref_.follower;
sei();	
	     switch (ref_.follower_safety){
		 case CPorta_ref::CORRECTION::SET_MODE_READOUT:
			 set_flags(0x00);		 
		 break;	
		 case CPorta_ref::CORRECTION::SET_MODE_CORRECTION:
			 set_flags(0x02);
		 break; 
		 case CPorta_ref::CORRECTION::WAIT:
//14.12.2018 
			if(ref_.channel_1_has_switch_for_ref)
				ref_.is_it_switch_for_ref_if_correction(switch_number_for_ref,input);
			if(enable_to_go_for_correction){
			//ref_.mistake=CPorta_ref::FAULT::NO_FAULT;	 
				ref_.follower_safety=CPorta_ref::CORRECTION::IS_MOVING_FIRST_TIME;	
				ref_.channel_1_has_switch_for_ref=false;
				if(flash_prg.sync_corr_see){
					ref_.follower_safety = CPorta_ref::CORRECTION::STOP;					
					enable_to_go_for_correction = 0;
				}		 	 
			}
		 break; 	 
		 case CPorta_ref::CORRECTION::SET_K:
		 //correction_set_k();
		 //ref_.follower=CPorta_ref::CORRECTION::WAIT;
		 return 1;
		 case CPorta_ref::CORRECTION::SET_POINT:
		 return 2;
		 case CPorta_ref::CORRECTION::START_TO_GO:
			ref_.mistake=CPorta_ref::FAULT::NO_CONNECTOR;
			ref_.check_counter=3;
			ref_.follower_safety=CPorta_ref::CORRECTION::WAIT;
//17.12.2018 rising edge if false high level if true.		 
			ref_.init_PA0_int_ref_1(true);
//--------------------------------------
			if(ref_.channel_1_has_switch_for_ref==true)
				ref_.enable_PA0_int_1_corr(true);		 
			else 
				ref_.enable_PA0_int_1_corr(false);
		
//if connector is absent or axis is on ref it will be error 		 
			if(ref_.check_counter==1){
				ref_.follower_safety=CPorta_ref::CORRECTION::IS_MOVING_FIRST_TIME;
				ref_.channel_1_has_switch_for_ref=false;	
			}
			else  
				ref_.mistake=CPorta_ref::FAULT::NO_FAULT; 
		 break;
		 case CPorta_ref::CORRECTION::GO_TO_REF:
//13.12.2018		 
//		 ref_.mistake=CPorta_ref::FAULT::NO_FAULT;
//11.12.2018 it must be inverted for to work in right direction	
//inside this function	 
		 altera.correction_set_k(ref_.k_divider);
//--------------------------------------------------------------		 
		 ref_.follower_safety=CPorta_ref::CORRECTION::START_TO_GO;
//28.05.2018
		//==выбор направления путем ввода отрицательного/положительного делителя
        if(ref_.k_divider>=0) {
			 ref_.table_is_ready_plus=false;
			 ref_.set_point_plus=0;
			 ref_.t_r_p_auto = 0;
		 }
         else {
			 ref_.table_is_ready_mines=false;
			 ref_.set_point_mines=0; 
			 ref_.t_r_m_auto = 0;
		 } 
		 //==
//-----------------		 
	     break;
		 case  CPorta_ref::CORRECTION::IS_MOVING_FIRST_TIME:
		 //ref_.check_counter_mem=ref_.check_counter;
		   if (ref_.mistake==CPorta_ref::FAULT::NO_CONNECTOR){
			   ref_.follower_safety=CPorta_ref::CORRECTION::STOP;
			   enable_to_go_for_correction=false;
			   break;  
		   }
		 else if(ref_.mistake==CPorta_ref::FAULT::NO_FAULT)
		  ref_.follower_safety=CPorta_ref::CORRECTION::IS_MOVING;
		 case CPorta_ref::CORRECTION::IS_MOVING:{
//it must be done something	
      
		 //ref_.mistake=CPorta_ref::FAULT::NO_FAULT;
         bool result=correction_is_waiting_flag();
		 
	     if(result){
		 set_flags(0x03);
		 set_flags(0x02);
		 //PORTE.OUT=CIOAltera::WRITE_ADR::LATCH_CNT;	 
		 ref_.etalon_meaning = read_cnt_quick(4);// считывание эталон значения
		 
		if(mCounter[1].abs<0)
			ref_.correction_sign=CPorta_ref::SIGN::MINES;
		else 
			ref_.correction_sign=CPorta_ref::SIGN::PLUS;

		 {
		if(ref_.correction_sign==CPorta_ref::SIGN::PLUS){
			//ref_.etalon_meaning--;//корректировка альтеры
			ref_.mistake= ref_.to_do_table_when_go_to_plus();
			}	
		else{
			//ref_.etalon_meaning--;//корректировка альтеры
			ref_.mistake= ref_.to_do_table_when_go_to_mines();	
			}
		if (ref_.mistake!=CPorta_ref::FAULT::NO_FAULT){
		  ref_.follower_safety=CPorta_ref::CORRECTION::STOP;
		  enable_to_go_for_correction=false;
		  break;	
		}
		//==выбор направления путем автоматического определения через переменную ref_.etalon_meaning
		/*if(!ref_.etalon_counter){
			if(ref_.etalon_meaning	> 0){
				ref_.table_is_ready_plus=false;
				ref_.set_point_plus=0;
				ref_.t_r_p_auto = 0;				
			}
			else{
				ref_.table_is_ready_mines=false;
				ref_.set_point_mines=0;
				ref_.t_r_m_auto = 0;
			}
		} */
		//===
//		ref_.etalon_meaning_one_step_ago=ref_.etalon_meaning; 
		}
		// зачем эталон умножать перед записью в озу?
  		 CDouble::MULT_OR_DIV_ANSWER answer;
		 answer=coeff.mult(0,&ref_.etalon_meaning);
 		 if (answer!= CDouble::OK){
 			 ref_.follower_safety=CPorta_ref::CORRECTION::STOP;
 			 enable_to_go_for_correction=false;
 			 ref_.mistake=CPorta_ref::FAULT::MULT;
 			 break;
 		 }
//------------------------------
//25.05.2018//06.06.2018 to	23LCV1024 10000 points
         if(ref_.correction_sign==CPorta_ref::SIGN::PLUS)
			spi.save_table_to_23LCV1024(ref_.etalon_counter,ref_.etalon_meaning,true);
		 else
			spi.save_table_to_23LCV1024(ref_.etalon_counter,ref_.etalon_meaning,false);
		  ref_.etalon_counter++;		 
//		 ref_.corr.time_to_think_plus[ref_.etalon_counter++]= ref_.etalon_meaning;
//		 else ref_.corr.time_to_think_mines[ref_.etalon_counter++]= ref_.etalon_meaning;
//----------------------------------------------
		 if(ref_.etalon_counter>=ref_.set_point){ 
			ref_.follower_safety=CPorta_ref::CORRECTION::STOP;
			enable_to_go_for_correction=false;
			ref_.mistake=CPorta_ref::FAULT::NO_FAULT;
//28.05.2018
//12.12.2018 
         if(ref_.k_divider!=0 && ref_.set_point!=0){		 
			 //if(ref_.etalon_meaning>=0){
			  if(ref_.k_divider>=0){
				 ref_.k_divider_plus=ref_.k_divider;
				 ref_.set_point_plus=ref_.set_point;
				 ref_.table_is_ready_plus=true;
				 ref_.t_r_p_auto = 1;
				}
				else { 
						ref_.k_divider_mines=-ref_.k_divider;
						if(ref_.k_divider_mines > 0)
							ref_.k_divider_mines *= -1;
						ref_.set_point_mines=ref_.set_point;
						ref_.table_is_ready_mines=true;	
						ref_.t_r_m_auto = 1; 
					 }
		    }	 
		 else {
				 ref_.table_is_ready_plus=false;	
				 ref_.table_is_ready_mines=false; 
				 ref_.t_r_p_auto = 0;
				 ref_.t_r_m_auto = 0; 
			  }
//----------------			   
		 break;
		 } 
		 }
//13.04.2018 overflow error
       
        long a = mCounter[1].delta;
		ref_.speed = mCounter[1].delta;	
		if(a<0)
			a=-a;
		long b=(long)ref_.k_divider;
		if(b<0)
			b=-b;
		if(a>=b) { 
		 ref_.follower_safety=CPorta_ref::CORRECTION::STOP;
		 ref_.speed=mCounter[1].delta;
		 ref_.mistake=CPorta_ref::FAULT::SPEED_LIMIT;	
		 enable_to_go_for_correction=false;
		 }
//--------------------------------------		  
		 break;}
		 case CPorta_ref::CORRECTION::IF_STOP_PRESSED:{
		 ref_.follower_safety=CPorta_ref::CORRECTION::STOP;
		 enable_to_go_for_correction=false;
		 ref_.mistake=CPorta_ref::FAULT::NO_FAULT;
		 //28.05.2018
		 if(ref_.etalon_meaning>=0){
			 ref_.k_divider_plus=ref_.k_divider;
			 ref_.set_point_plus=ref_.etalon_counter;
			 ref_.table_is_ready_plus=true;
			 ref_.t_r_p_auto = 1;
		 }
		 else {
			 ref_.k_divider_mines=-ref_.k_divider;
			 if(ref_.k_divider_mines > 0)
				ref_.k_divider_mines *= -1;
			 ref_.set_point_mines=ref_.etalon_counter;
			 ref_.table_is_ready_mines=true;
			 ref_.t_r_m_auto = 1;
			 }
         }
		 break;
		 case CPorta_ref::CORRECTION::STOP:
//it must be done something
		  //correction_set_start(0x0);
		   ref_.disable_PA0_int_1_corr();
		   set_flags(0x03);
		   set_flags(0x02);
		 // ref_.follower=CPorta_ref::CORRECTION::WAIT;
		 ref_.follower_safety=CPorta_ref::CORRECTION::SET_MODE_CORRECTION;
		 if(flash_prg.sync_corr_see){
			ref_.follower_safety=CPorta_ref::CORRECTION::SET_MODE_READOUT;
			flash_prg.sync_corr_see = false;
			}
		  break; 	 
		 }
// ref_.follower=CPorta_ref::CORRECTION::STOP;
cli();
ref_.enable_to_go_for_correction=enable_to_go_for_correction;
ref_.follower=ref_.follower_safety;
sei();	 
return 0;
}
#pragma GCC push_options
#pragma GCC optimize ("O0")
CPorta_ref::FAULT CPorta_ref::to_do_table_when_go_to_plus(){

long ref,counter,delta,multiplicand;
	ref=ref_.etalon_meaning;
	counter=mCounter[0].abs_pure;
if(counter<0){
		counter=-counter;
		ref=16777216-ref;
	}
	ref = ref + 2;//it must be known DENIS, ?????
delta=counter-ref;
if(delta<0)
	delta=-delta;

if(delta<1000000) {
	 ref_.etalon_meaning=ref;
	 return CPorta_ref::NO_FAULT; 
	 }
multiplicand=counter/16777216; 
ref=ref+(multiplicand*16777216);		 
delta=counter-ref;
if(delta<0)
	delta=-delta;
if(delta<1000000) {
	ref_.etalon_meaning=ref;
	return CPorta_ref::NO_FAULT; 
	}	 	 
return CPorta_ref::IF_PLUS_FAULT;
}
CPorta_ref::FAULT CPorta_ref::to_do_table_when_go_to_mines(){
//24.04.2018 in worst case it takes 390 cycles 390*31.25=12187.5 ns
	long ref,counter,delta,multiplicand;
	ref=ref_.etalon_meaning;
	counter=mCounter[0].abs_pure;
//25.04.2018 only for debug
//ref=-ref;
//counter=-counter;
//-------------------------	
	if(counter<0){
	counter=-counter;
//14.01.2019 it must be known DENIS		
//	ref=16777216-ref;
    ref=16777216-ref;	
	}
	delta=counter-ref;
	if(delta<0)delta=-delta;
	if(delta<1000000) {
	ref_.etalon_meaning=-ref;
	return CPorta_ref::NO_FAULT; 
	}
	multiplicand=counter/16777216;
	ref=ref+(multiplicand*16777216);
	delta=counter-ref;
	if(delta<0)
	delta=-delta;
	if(delta<1000000) {
	ref_.etalon_meaning=-ref;
	return CPorta_ref::NO_FAULT; 
	}
	return CPorta_ref::IF_MINES_FAULT;
}
#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC optimize ("O0")
bool CIOAltera::correction_is_waiting_flag(void){
//	char *a=0;	
    char a[1];
		//rw.set();
		PORTR.OUTSET=0x01;
		//mcu_bus.set_port(adr);
		PORTE.OUT=READ_FLAG::ETALON_IS_READY;
		//mcu_bus.set_dir_output();
		PORTE.DIR=0xff;
		//adr_sync();
		PORTR.OUTCLR=0x02;
		PORTR.OUTSET=0x02;
		//mcu_bus.set_dir_input();
		PORTE.DIR=0x00;
		//it is delay 15 cycles
		to_take_address(a);
		//__delay_cycles(5);
		//rw.clr();
		PORTR.OUTCLR=0x01;
		//data_sync();
		PORTR.OUTCLR=0x02;
		//it is delay 15 cycles
		to_take_address(a);
		//__delay_cycles(5);
		//unsigned char data = mcu_bus.get_port();
		unsigned char data=PORTE.IN;
		//rw.set();
		PORTR.OUTSET=0x01;
		//__delay_cycles(5);
		//mcu_bus.set_dir_output();
		PORTE.DIR=0xff;
		if (data==0x01)return true;
		else return false;

}
/*
0x00 - сброс всех флагов
0x02 , 0x03 - установка Коррекци
0x04 - 0 канал,0x08 - 1 канал ,0x10 - 2 канал ,0x20 3 канал,  - установка флагов КМ 
*/
void CIOAltera::set_flags(unsigned char command){
	#if (configUSE_mcx514 == 1)
	return;
	#endif
//char *a=0;
  char a[1];	
//alt_rw=1 data line from xmega to altera
PORTR.OUTSET=0x01;
//address to data line
PORTE.OUT=WRITE_ADR::START_TO_GO;
//set data line to output
PORTE.DIR=0xff;
//it is delay 15 cycles
to_take_address(a);	
//write address to altera
PORTR.OUTCLR=0x02;
PORTR.OUTSET=0x02;
//data to data line
PORTE.OUT=command;
//it is delay 15 cycles
to_take_address(a);
// write data to altera
PORTR.OUTCLR=0x02;	
	
}

unsigned char  CIOAltera::correction_set_k(int n){
	unsigned char low,high,mid;
	//int data=ref_.k_divider;
	int data=-n;
//	char *a=0;
    char a[1];
	low=(unsigned char)data;
	data=data>>8;
	mid=(unsigned char)data;
	if(mid&0x80)high=0xff;
	else high=0x00;
//	to_see_k_low=low;
//	to_see_k_mid=mid;
//	to_see_k_high=high;
//alt_rw=1 data line from xmega to altera	
	PORTR.OUTSET=0x01;
//address to data line 
   PORTE.OUT=WRITE_ADR::K_SET_LOW;
//set data line to output
   PORTE.DIR=0xff;
//it is delay 15 cycles   
    to_take_address(a);
//write address to altera
    PORTR.OUTCLR=0x02;
	PORTR.OUTSET=0x02;
//data to data line		   
    PORTE.OUT=low;
//it is delay 15 cycles
   to_take_address(a);
// write data to altera
   PORTR.OUTCLR=0x02;
//it is delay 15 cycles
to_take_address(a);   
//second byte    
//address to data line
PORTE.OUT=WRITE_ADR::K_SET_MID;      	   
//it is delay 15 cycles
to_take_address(a);
//write address to altera
PORTR.OUTCLR=0x02;
PORTR.OUTSET=0x02;
//data to data line
PORTE.OUT=mid;
//it is delay 15 cycles
to_take_address(a);
// write data to altera
PORTR.OUTCLR=0x02;  
//third byte 
//address to data line
PORTE.OUT=WRITE_ADR::K_SET_HIGH;
//it is delay 15 cycles
to_take_address(a);
//write address to altera
PORTR.OUTCLR=0x02;
PORTR.OUTSET=0x02;
//data to data line
PORTE.OUT=high;
//it is delay 15 cycles
to_take_address(a);
// write data to altera
PORTR.OUTCLR=0x02;
  
	return 0;
}
#pragma GCC pop_options

void CIOAltera::to_see_corrections_errors(long *to_see_working_corr){
//15.01.2019 0 channel	
//if(altera.error_corr_real_time_[0]!=mem[0]){
if(altera.error_corr_real_time[0]==ERROR_CORR_REAL_TIME::NO_ERROR){
display.put_char_local_independent(0x20,0,0,224);
display.put_char_local_independent(0x20,0,1,224);
}
else{
display.print_debug_one_local_shot(altera.error_corr_real_time[0],0,0,224,2);		
}
//}
//---------1 channel
//if(altera.error_corr_real_time_[1]!=mem[1]){
	if(altera.error_corr_real_time[1]==ERROR_CORR_REAL_TIME::NO_ERROR){
		display.put_char_local_independent(0x20,0,3,224);
		display.put_char_local_independent(0x20,0,4,224);
	}
	else{
		display.print_debug_one_local_shot(altera.error_corr_real_time[1],0,3,224,2);
	}
//}
//---------2 channel
//if(altera.error_corr_real_time_[2]!=mem[2]){
	if(altera.error_corr_real_time[2]==ERROR_CORR_REAL_TIME::NO_ERROR){
		display.put_char_local_independent(0x20,0,6,224);
		display.put_char_local_independent(0x20,0,7,224);
	}
	else{
		display.print_debug_one_local_shot(altera.error_corr_real_time[2],0,6,224,2);
	}
//}
//---------3 channel
//if(altera.error_corr_real_time_[3]!=mem[3]){
	if(altera.error_corr_real_time[3]==ERROR_CORR_REAL_TIME::NO_ERROR){
		display.put_char_local_independent(0x20,0,9,224);
		display.put_char_local_independent(0x20,0,10,224);
	}
	else{
		display.print_debug_one_local_shot(altera.error_corr_real_time[3],0,9,224,2);
	}
//}
//---------common
//display.print_debug_one_local_shot(flash_prg.to_see_rising_edge,0,5,224,2);
display.print_debug_one_local_shot(to_see_working_corr[0]/10,0,19,208,5);
display.print_debug_one_local_shot(to_see_working_corr[1]/10,0,19,224,5);
display.print_debug_one_local_shot(to_see_working_corr[2]/10,0,25,208,5);
display.print_debug_one_local_shot(to_see_working_corr[3]/10,0,25,224,5);	
//for(unsigned char n=0;n<N_AXIS;n++)	mem[n]=altera.error_corr_real_time[n];
}



