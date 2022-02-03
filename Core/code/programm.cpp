
//--------------------
#include "main.h"
//#include "translate.h"
#include "led_key_sound.h"
#include "display.h"
#include "programm.h"
#include "at45.h"
#include "io_altera.h"
#include "stdlib.h"
#include "config.h"
#include "Strings.h"
#include "uart_flash_PD6_7.h"
#include "keyfunc.h"
#include "config.h"
#include <avr/pgmspace.h>
//22.01.2018
extern "C"{
void  delay_mks(int);
};
//15.08.2018 new technical task is coming...
void CPrg::new_technical_task(long counter,char place){
	    bool zero_clean=false;
		int cleaning,painting;
	    unsigned char y=(unsigned char)place;
		long counter_memory=0; 
		if(place==0){y=2;
		 display.print_debug_one_local(counter,0,19,32);
		 counter_memory=counter_memory_0;
		            }
		if(place==1){y=9;
			display.print_debug_one_local(counter,0,19,90);
			counter_memory=counter_memory_1;
		}	
		if(place==2){y=16;
			display.print_debug_one_local(counter,0,19,148);
			counter_memory=counter_memory_2;
			}
				
					
	   	if((counter>=0 && counter_memory<0)||(counter<=0 && counter_memory>0)) 
		 zero_clean=true;
		 if(counter==0 || zero_clean==true){
//cleaning takes 5.6 ms			 
			//  init.set_B6();
		 for(int n=0;n<320;n++){
			 if(n<155 || n>165)
			 display.put_growing_line(0,0x00,n,y);
			 if(n<=40 || n>=280)
			 display.put_growing_line(0,0x00,n,y+1); 
		 }
		// display.is_growing++;
		if(zero_clean==true && place==0 ){
		  for(int n=0;n<10;n++){
			  display.put_growing_line(0,0xff,155+n,y);
		  }
		  }
		  //init.clr_B6();	 
		 }
		  if(counter>0 && counter<=100 ){
//it takes 1.7 ms	
           if(counter<=10){
			 switch (counter){
			 case 1:cleaning=10;painting=10;break;
			 case 2:cleaning=20;painting=20;break;
			 case 3:cleaning=30;painting=30;break; 
			 case 4:cleaning=40;painting=40;break;
			 case 5:cleaning=50;painting=50;break;
			 case 6:cleaning=60;painting=60;break;
			 case 7:cleaning=70;painting=70;break;
			 case 8:cleaning=80;painting=80;break;
			 case 9:cleaning=90;painting=90;break;
			 case 10:cleaning=100;painting=100;break;
			 default:cleaning=10;painting=10;	 
			 }
			 for(int n=cleaning;n<100;n++)display.put_growing_line(0,0x00,n+170,y);   
			 for(int n=0;n<painting;n++)display.put_growing_line(0,0xff,n+170,y);  
		   }
		  else {
			// init.set_B6();
			 for(int n=counter;n<100;n++)display.put_growing_line(0,0x00,n+170,y);
             for(int n=0;n<counter;n++)display.put_growing_line(0,0xff,n+170,y);	
			// init.clr_B6();
		  }
		   display.print_debug_one_local_shot(counter,0,35,y*8,5);
		 }
//16.08.2018		 
	  if(counter>100 && counter<=1000 ){
		 int new_counter=counter/10; 
		 for(int n=new_counter;n<100;n++)display.put_growing_line(0,0x00,n+170,y);
		 for(int n=0;n<new_counter;n++)display.put_growing_line(0,0xff,n+170,y);  
		  display.print_debug_one_local_shot(counter,0,35,y*8,5); 	 
	  }
//----------------------------------------		 
		  if(counter<0 && counter>=-100 ){
		 	 //init.set_B6();
			  if(counter>=-10){
				 switch (counter){
					 case -1:cleaning=-10;painting=10;break;
					 case -2:cleaning=-20;painting=20;break;
					 case -3:cleaning=-30;painting=30;break;
					 case -4:cleaning=-40;painting=40;break;
					 case -5:cleaning=-50;painting=50;break;
					 case -6:cleaning=-60;painting=60;break;
					 case -7:cleaning=-70;painting=70;break;
					 case -8:cleaning=-80;painting=80;break;
					 case -9:cleaning=-90;painting=90;break;
					 case -10:cleaning=-100;painting=100;break;
					 default:cleaning=-10;painting=10;
				 }  
				 for(int n=0;n<100+cleaning;n++)display.put_growing_line(0,0x00,n+50,y);
				 for(int n=0;n<painting;n++)display.put_growing_line(0,0xff,150-n,y);  
			  }
			 else{ 
		 	 for(int n=0;n<100+counter;n++)display.put_growing_line(0,0x00,n+50,y); 	
			 for(int n=0;n<counter*(-1);n++)display.put_growing_line(0,0xff,150-n,y); 
			     }
		  display.print_debug_one_local_shot(counter,0,0,y*8,5);
		   // init.clr_B6();
		 }
//16.08.2018
		if(counter<-100 && counter>=-1000 ){
			int new_counter=counter/10;
			 for(int n=0;n<100+new_counter;n++)display.put_growing_line(0,0x00,n+50,y);
			 for(int n=0;n<new_counter*(-1);n++)display.put_growing_line(0,0xff,150-n,y);
			display.print_debug_one_local_shot(counter,0,0,y*8,5);
		}
//---------------------------------------- 
		 
		 
	//display.is_growing++;
	//if(display.is_growing>319)display.is_growing=0;	
if(place == 0)counter_memory_0 = counter;
if(place == 1)counter_memory_1 = counter;
if(place == 2)counter_memory_2 = counter;	
}
void CPrg::to_take_buttons_code_for_new_technical_task(char k_c){
	 unsigned int x=157+160;
	 unsigned char y=0;
	switch (k_c){
		case _key_STOP:
			Lir.entry_INDI = Menu::INIT;
			Lir.SET_C(idIzmer);
		break;
		case  _key_BLANK1:
		
		cli();
		mCounter[0].rel=0;
		sei();
		for(int n=0;n<10;n++){
			display.put_growing_line(0,0xff,155+n,2);
		}
		display.put_char_local(0x30,0,&x,&y);
		break;
		case  _key_BLANK2:
		cli();
		mCounter[1].rel=0;
		sei();
		y=8;
		//for(int n=0;n<10;n++){
		//	display.put_growing_line(0,0xff,155+n,9);
		//}
		display.put_char_local(0x30,0,&x,&y);
		break;
		case  _key_BLANK4:
		 if(mode_graph==false){
			cli();
			mCounter[3].rel=0;
			sei();
			go_to_zero_ch_3=true;
		 }
		break;
		/*case _key_BLANK1_L:
		 if(mode_graph==false){
		 mode_graph=true;
		 clear_place_for_graph=true;
		 start_point_for_graph=true;	 
		 }
		 else {mode_graph=false; 
		 clear_place_for_graph=true;	 
		 }
		break; */
//29.08.2018 sign_for_graph=false it is plus		
		case _key_BLANK3_L:
		if(sign_for_graph==false)sign_for_graph=true;
		else sign_for_graph=false;
		break;
		case _key_BLANK4_L:
		if(mode_graph==false) resolution++;
		break;  
		
	}
	switch (resolution){
	case 0: compare=100; break;
	case 1: compare=1000; break;
	case 2: compare=5000;break;
	case 3: compare=10000;break;
	case 4: compare=20000;break;
	case 5: compare=50000;break;
	case 6: compare=100;resolution=0;break;
	default:compare=100;resolution=0;break;	
	}
	//if(sign_for_graph==false)
	//display.print_debug_one_local_shot(compare,0,0,8*24,6);
	//else display.print_debug_one_local_shot(-compare,0,0,8*24,6); 
}
void CPrg::graph(long y,long x){
//25.08.2018 if plus it takes 3.640 ms	
	//long delta_gr;
	
	//bool change=false;
	unsigned int clean_x=160;
	unsigned char clean_y=0;
	
	if(inner_step>=10){
			display.clear_rectangle(0,0,320,21,0x00,0);
			inner_step=0;
			display.print_debug_one_local_shot(header,0,19,0,8);		
			}
	else {	
	if(start_point_for_graph==true){
	//if(sign_for_graph==true)y=-y;	
		if(go_to_zero_ch_3==true){
			y_=y_start=y;
			x_=x_start=x;
			inner_step=0;
			step_10=0;
			multiplicator=0;
			go_to_zero_ch_3=false;
			}
		else{
			y_=y_start=y;
			x_=x_start=x;
			inner_step=0;
			step_10=0;	
			multiplicator=0;	
			}
		start_point_for_graph=false; 
		display.print_debug_one_local_shot(y_,0,19,0,8);		
	}
	else {
	//if(sign_for_graph==true)y=-y;	
		if(sign_for_graph==false)
			delta_gr=y-y_;
		else
			delta_gr=y_-y;

		if(delta_gr>=compare){
			inner_step++;
			//y_=y_start+inner_step*compare+step_10;	
			if(sign_for_graph==false)
				y_=y_start+inner_step*compare+step_10;
			else 
				y_=y_start+inner_step*compare*(-1)+step_10*(-1);
			}		
		}
	}
//	display.print_debug_one_local_shot(y_,0,0,224,8);
//	display.print_debug_one_local_shot(y_start,0,7,224,8);
//	display.print_debug_one_local_shot(inner_step,0,14,224,8);
//	display.print_debug_one_local_shot(step_10,0,18,224,8);

	    if(inner_step>=10)
		{
			header=y_;
			multiplicator++;
			step_10=10*compare*multiplicator;
			return;
		}

	  for(int n=0;n<10;n++){
		 display.put_growing_line(0,0xff,155+n,(inner_step*2+2));
		   } 
//25.08.2018 it must be clean...
//	for(int n=50;n<155;n++)display.put_growing_line(0,0x00,n,(inner_step*2+2));
//	for(int n=170;n<270;n++)display.put_growing_line(0,0x00,n,(inner_step*2+2));	   
		    
//it is plus
      if(x>=0){
      if(x>100)	{
//clean mines side	  	  
	  for(int n=50;n<155;n++)display.put_growing_line(0,0x00,n,(inner_step*2+2)); 
//----------------	   	  		
	  for(int n=0;n<100;n++)display.put_growing_line(0,0xff,n+170,(inner_step*2+2));		
	  display.print_debug_one_local_shot(x,0,34,(inner_step*2*8+8),6);
	  }
	  if(x>0 && x<=100){
//clean mines side	  		  
	  for(int n=50;n<155;n++)display.put_growing_line(0,0x00,n,(inner_step*2+2));
//----------------	    
	  for(int n=0;n<x;n++)display.put_growing_line(0,0xff,n+170,(inner_step*2+2));
//clean tail	  
	  for(int n=x+170;n<270;n++)display.put_growing_line(0,0x00,n,(inner_step*2+2));
//it is digit	  
	  display.print_debug_one_local_shot(x,0,34,(inner_step*2*8+8),3);  
	  }
	  if (x==0){
//clean plus and mines sides	 		  
	for(int n=50;n<155;n++)display.put_growing_line(0,0x00,n,(inner_step*2+2));
	for(int n=170;n<270;n++)display.put_growing_line(0,0x00,n,(inner_step*2+2));
//--------------------------			  
	  display.print_debug_one_local_shot(x,0,34,(inner_step*2*8+8),1);   
	  }
//clean mines digit	
     clean_y=(unsigned char)inner_step*2+1;
     for(int n=0;n<7;n++){  
	  	display.put_char_local(0x20,0,&clean_x,&clean_y);
		clean_x=clean_x+8;  
	  }
	  }
	 else {
//it is mines
//clean plus side
for(int n=170;n<270;n++)display.put_growing_line(0,0x00,n,(inner_step*2+2));
       if(x>=-100){	
//clean head,if mines head is going first
       for(int n=50;n<150+x;n++)display.put_growing_line(0,0x00,n,(inner_step*2+2));
//put 0xff, tail is black	     
	   for(int n=0;n<x*(-1);n++)display.put_growing_line(0,0xff,150-n,(inner_step*2+2));
//it is digit	   
       display.print_debug_one_local_shot(x,0,0,(inner_step*2*8+8),4);  
	   }
	   if(x<-100){
      for(int n=0;n<100;n++)display.put_growing_line(0,0xff,150-n,(inner_step*2+2));
	  display.print_debug_one_local_shot(x,0,0,(inner_step*2*8+8),6); 
	   }
//clean plus digit
clean_y=(unsigned char)inner_step*2+1;
clean_x=270+160;
for(int n=0;n<6;n++){
	display.put_char_local(0x20,0,&clean_x,&clean_y);
	clean_x=clean_x+8;
}	   
	   
	   
       } 
     
}

void CPrg::upload_flash_to_ram()
{
	switch(Lir.cnt_StrCh){
		case 0:
		up_or_down=UPLOAD_FLASH_0_TO_RAM;
		break;
		case 1:
		up_or_down=UPLOAD_FLASH_1_TO_RAM;
		break;
		case 2:
		up_or_down=UPLOAD_FLASH_2_TO_RAM;
		break;
		case 3:
		up_or_down=UPLOAD_FLASH_3_TO_RAM;
		break;
		default:
		up_or_down=UPLOAD_FLASH_0_TO_RAM;
		break;
	}
	Lir.corr_see_cur_ch = Lir.cnt_StrCh; //копируем в переменную
	execute=EXEC::THIRD;
	cli();timeout_to_go=0;
	correction_must_be_done=true;
	to_take_task_for_exchange();
	//exchange=EXCHANGE::TO_TAKE_TASK;
	sei();
	time_to_flash=0;
	stop_flashing=false;
	error_when_work_with_RAM_or_FLASH=false;
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
#pragma GCC pop_options
//18.04.2018 correction's results can be seen...
//10.08.2018 special case for up and down keys.
void CPrg::to_take_only_up_down_code_and_to_see_correction(char k_c){
	cli();
	bool table_plus=ref_.table_plus;
	int to_see=to_see_corr;
	int set_point_plus=ref_.set_point_plus;
	int set_point_mines=ref_.set_point_mines;
	sei();
switch (k_c){
		case _key_UP :{
			to_see=to_see-10;
			if(to_see<0)to_see=0;
			display.clear_rectangle(0,4,272,10,0x00,0);
			break;
		}
		case _key_DOWN:{
			to_see=to_see+10;
			if(table_plus==true){
				if(to_see>=set_point_plus)to_see=to_see-10;
				if(to_see<0)to_see=0;
			}
			else{
					if(to_see>=set_point_mines)to_see=to_see-10;
					if(to_see<0)to_see=0;
				}
				display.clear_rectangle(0,4,272,10,0x00,0);
				break;
			}
}
			
cli();
to_see_corr=to_see;
sei();
}			

void CPrg::to_take_buttons_code_and_to_see_correction(char k_c){
	cli();
	bool table_plus=ref_.table_plus;
	int to_see=to_see_corr;
	int set_point_plus=ref_.set_point_plus;
	int set_point_mines=ref_.set_point_mines;
	bool t_i_u=table_is_used;
	
	sei();
switch (k_c){
	case _key_STOP:
		//02.08.2018 it will be decided later what to do
		cli();
		flash_prg.to_set_var_for_angle_axis();
		//ref_.table_m_from_flash = 0;
		//ref_.table_p_from_flash = 0;
		flash_prg.sync_corr_see = false;
		//ref_.follower=CPorta_ref::CORRECTION::STOP;
		ref_.follower = CPorta_ref::CORRECTION::SET_MODE_READOUT;
		ref_.enable_to_go_for_correction=false;
		flash_prg.to_take_table =false;
		sei();
		Lir.entry_SEECOR = Menu::INIT;
		Lir.SET_C(idCorr);
	break;
    case _key_BLANK1_L: // Номер точки
	Lir.WorkCorr_entry = NUM_POINT;
	Lir.CurFormatM = flash_prg.axis_cfg[Lir.cnt_StrCh].format;
	Lir.SET_C(idWorkCorr_Input);
	break;
	case _key_BLANK2_L: // Создание таблицы  Размер интервалов -> Кол - во интервалов
	Lir.WorkCorr_entry = SET_INTERVAL;
	Lir.CurFormatM = flash_prg.axis_cfg[Lir.cnt_StrCh].format; 
	Lir.SET_C(idWorkCorr_Input);  //входим в режим ввода с клавиатуры
	break;
    case  _key_BLANK1: // СИНХ (только для авто режима)
	if(!flash_prg.sync_corr_see && !ref_.is_enabled_PA0_int_1()){

		flash_prg.sync_corr_see = true;
		ref_.follower = CPorta_ref::CORRECTION::START_TO_GO;
		cli();
		altera.set_flags(0x02);
		sei();
		}
		else{
			ref_.follower = CPorta_ref::CORRECTION::STOP;
			flash_prg.sync_corr_see = false;
			}
   break;
   case _key_PLUS_MINUS:// +/-
		if(error_when_work_with_RAM_or_FLASH)
			break;
		if(table_plus)		
			table_plus=false;
		else 
			table_plus=true;
		//display.clear_rectangle(0,4,272,10,0x00,0);
		display.print_symbols_local(' ',14,0,18,16 * 8);
		Lir.update_table_corr = 1;	
   break;
   case  _key_BLANK2: // Переключение канала
   {
		char tmp = Lir.P2[0]+Lir.P2[1]+Lir.P2[2]+Lir.P2[3];
		if(tmp == (no_axis * 4))
			break;

		do {
			Lir.cnt_StrCh++;

			if(Lir.cnt_StrCh == 4)
				Lir.cnt_StrCh = 0;
		} while (Lir.P2[Lir.cnt_StrCh] == no_axis);	
		//flash_prg.to_set_max_for_angel_axis(Lir.cnt_StrCh,coeff.new_coeff_[0].binary,coeff.new_coeff_[0].n_after_dot);					
   }		
   break;
//----------------------------------
//10.08.2018
//	_key_LEFT    
//	_key_RIGHT   
//	_key_ENTER
    case _key_LEFT:
		 if(error_when_work_with_RAM_or_FLASH==true)break;
         to_see=0;
         //display.clear_rectangle(0,4,272,10,0x00,0);
		 Lir.update_table_corr = 1;		
         break;		
	case _key_RIGHT:
		 if(error_when_work_with_RAM_or_FLASH==true)break;
	     if(table_plus==true){
		 to_see=set_point_plus-1; 
		 if(to_see<0)to_see=0;	
	      }
	     else {
		 to_see=set_point_mines-1;
		 if(to_see<0)to_see=0;	 	 
	         }
	     //display.clear_rectangle(0,4,272,10,0x00,0);
	     Lir.update_table_corr = 1;
	     break;
	case 	_key_ENTER:{
		if(execute==EXEC::FIRST){
		 if(error_when_work_with_RAM_or_FLASH==true)break;	 
		if(table_plus==true){
			to_see=set_point_plus/2;
			if(to_see<0)to_see=0;
		}
		else {
			to_see=set_point_mines/2;
			if(to_see<0)to_see=0;
		}
		//display.clear_rectangle(0,4,272,10,0x00,0);
		Lir.update_table_corr = 1;
	    break;
	      }
	}
//-----------------------------------	
	case _key_UP :
		if(error_when_work_with_RAM_or_FLASH==true){
			//USART_DEBUG(1);
			break;
		}
			to_see=to_see-5;
		if(to_see<0)to_see=0;
		 //display.clear_rectangle(0,4,272,10,0x00,0);
		 Lir.update_table_corr = 1;
	     break;
	case _key_DOWN:
		if(error_when_work_with_RAM_or_FLASH==true){
			//USART_DEBUG(2);
		break;	
		}
			to_see=to_see+5;
		if(table_plus==true){
			if(to_see>=set_point_plus)to_see=to_see-5;
			if(to_see<0)to_see=0;
		}
		else{
			if(to_see>=set_point_mines)to_see=to_see-5;
			if(to_see<0)to_see=0;	
		}
		//display.clear_rectangle(0,4,272,10,0x00,0);
		Lir.update_table_corr = 1;
	    break;
	case _key_BLANK3: //СЧИТ
		display.put_char_local_independent(' ',0,16,0);
		t_save = 0;
		to_see = 0;
		memset(ref_.tbl,0,2);
		ref_.ld_table = ref_.table_plus;
		upload_flash_to_ram();
		display.print_symbols_local(' ',14,0,18,16 * 8);
		display.print_symbols_local(' ',14,0,18,16 * 9);
		//display.clear_rectangle(0,4,272,10,0x00,0);
		Lir.update_table_corr = 1;
	    break;
//31.05.2018 four tables is coming...	
	case _key_BLANK4://СОХР
	display.put_char_local_independent(' ',0,16,0);
	t_save = 0;
	switch(Lir.cnt_StrCh){
		case 0:
			up_or_down=DOWNLOAD_RAM_TO_FLASH_0;
			break;
		case 1:
			up_or_down=DOWNLOAD_RAM_TO_FLASH_1;
			break;
		case 2:
			up_or_down=DOWNLOAD_RAM_TO_FLASH_2;
			break;
		case 3:
			up_or_down=DOWNLOAD_RAM_TO_FLASH_3;
			break;
		default:
			up_or_down=DOWNLOAD_RAM_TO_FLASH_0;
			break;
	}

	execute=EXEC::THIRD;
	cli();timeout_to_go=0;
	correction_must_be_done=true;
	to_take_task_for_exchange();
	//exchange=EXCHANGE::TO_TAKE_TASK;
	sei();
	time_to_flash=0;
	stop_flashing=false;
	error_when_work_with_RAM_or_FLASH=false;
	break;

	case  _key_C:   
	to_stop_if_key_C_is_pressed=true;
	break;
	
        }
        
cli();
ref_.table_plus=table_plus;
to_see_corr=to_see;
table_is_used=t_i_u;
sei();
}

//06.06.2018
void CPrg::to_read_five_from_23LCV1024(void){
//06.06.2018 it takes 200 mks 		
	if(ref_.table_is_ready_mines==true){
	if(ref_.table_plus==false){
		for(int n=0;n<5;n++){
	ref_.address_23LCV1024_mines=(long)(to_see_corr+n);
	spi.read_table_from_23LCV1024(ref_.address_23LCV1024_mines,&ref_.corr.pt_mines[n],false);
		}
	  }
    }
	if(ref_.table_is_ready_plus==true){
	if(ref_.table_plus==true){		
		for(int n=0;n<5;n++){
	ref_.address_23LCV1024_plus=(long)(to_see_corr+n);	
	spi.read_table_from_23LCV1024(ref_.address_23LCV1024_plus,&ref_.corr.pt_plus[n],true);
	//long tmp;
	//spi.read_table_from_23LCV1024(ref_.address_23LCV1024_plus,&tmp,true);
	//flash_prg.corr_plus[n]=tmp;
		}
	  }		
	}		
}


void CPrg::to_see_correction(void){
//05.06.2018 worst case it takes 8.9 ms	
	//CDouble::MULT_OR_DIV_ANSWER answer;
	long delta;
	long k_div;
	int n,m;
	 long tmp,rab,et;
	n=0;
//06.06.2018 all variables must be used correct,
//because it can be interrupted by another program
	cli();
	long k_divider_plus=(long)ref_.k_divider_plus;
	long k_divider_mines=(long)ref_.k_divider_mines;
	bool table_is_ready_plus=ref_.table_is_ready_plus;
	int set_point_plus=ref_.set_point_plus;
	bool table_is_ready_mines=ref_.table_is_ready_mines;
	int set_point_mines=ref_.set_point_mines;
	bool table_plus=ref_.table_plus;
	m=to_see_corr;
	sei();
	uint8_t cur_see_ch = Lir.corr_see_cur_ch;
if(table_plus==true) 
	k_div = k_divider_plus;
else  
	k_div = k_divider_mines;

display.print_flash_local(PSTR("           "),0,8,16);
display.print_flash_local(PSTR("     "),0,18,16*8);
display.print_flash_local(PSTR("     "),0,18,16*9);
//04.06.2018	
	 if(table_plus){
		 if(table_is_ready_plus){
			if(ref_.table_p_from_flash){ //ФЛЭШЬ
				if(ref_.t_r_p_auto){  //режим А
					cur_see_ch = Lir.corr_see_cur_ch;
					display.put_char_local_independent('А',0,24,0);
					flash_prg.to_set_max_for_angel_axis(cur_see_ch,coeff.coeff_corr[cur_see_ch].binary,coeff.coeff_corr[cur_see_ch].n_after_dot);
					flash_prg.to_set_max_for_angel_axis(0,coeff.coeff_corr[cur_see_ch].binary,coeff.coeff_corr[cur_see_ch].n_after_dot);
				}
				else{ // режим ручной
					display.put_char_local_independent(' ',0,24,0);	
					cur_see_ch = Lir.corr_see_cur_ch;									
				}
			}
			else{ //ОЗУ
				if(ref_.t_r_p_auto){  //режим А
					display.put_char_local_independent('А',0,24,0);
					cur_see_ch = 1;
					flash_prg.to_set_max_for_angel_axis(cur_see_ch,coeff.new_coeff_[0].binary,coeff.new_coeff_[0].n_after_dot);
				}
				else{ // режим ручной
					display.put_char_local_independent(' ',0,24,0);
					cur_see_ch = Lir.corr_see_cur_ch;
				}
			}

			tmp = k_divider_plus;
			// Размер интервалов
			coeff.mult(cur_see_ch,&tmp); //Lir.corr_see_cur_ch

			Lir.print_long_format(tmp,Lir.get_format_from_ch(cur_see_ch),0,18,16*8); //Lir.corr_see_cur_ch			 
		    display.print_debug_one_local_shot(set_point_plus,0,18,16*9,8); // Кол - во интервалов
		 
		 if(set_point_plus!=0) {
				display.print_debug_one_local_shot(m+1,0,8,16,8);
				int k= set_point_plus-m-1;
				switch (k){
					 case 0:break;	
					 case 1:
					 case 2:
					 case 3:
					 case 4:{
						display.gotoxy(13,16);
						display.put_char('-', 0);
						display.print_debug_one_local_shot(set_point_plus,0,15,16,8);
						} 
					break;
					default:{
						display.gotoxy(13,16);
						display.put_char('-', 0);
						display.print_debug_one_local_shot(m+5,0,15,16,8);
					}	 
				}
			}
		}
		else
			display.put_char_local_independent(' ',0,24,0);
	 }
	 if(!table_plus){
		 if(table_is_ready_mines==true){
			if(ref_.table_m_from_flash){ //ФЛЭШЬ
				if(ref_.t_r_m_auto){  //режим А
					cur_see_ch = Lir.corr_see_cur_ch;
					display.put_char_local_independent('А',0,24,0);
					flash_prg.to_set_max_for_angel_axis(cur_see_ch,coeff.coeff_corr[cur_see_ch].binary,coeff.coeff_corr[cur_see_ch].n_after_dot);
					flash_prg.to_set_max_for_angel_axis(0,coeff.coeff_corr[cur_see_ch].binary,coeff.coeff_corr[cur_see_ch].n_after_dot);
				}
				else{ // режим ручной
					display.put_char_local_independent(' ',0,24,0);
					cur_see_ch = Lir.corr_see_cur_ch;
				}
			}
			else{ //ОЗУ
				if(ref_.t_r_m_auto){  //режим А
					display.put_char_local_independent('А',0,24,0);
					cur_see_ch = 1;
					flash_prg.to_set_max_for_angel_axis(cur_see_ch,coeff.new_coeff_[0].binary,coeff.new_coeff_[0].n_after_dot);
				}
				else{ // режим ручной
					display.put_char_local_independent(' ',0,24,0);
					cur_see_ch = Lir.corr_see_cur_ch;
				}
			}


			// Размер интервалов
			 tmp = k_divider_mines;		  
			 coeff.mult(cur_see_ch,&tmp); //Lir.corr_see_cur_ch
			 Lir.print_long_format(tmp,Lir.get_format_from_ch(cur_see_ch),0,18,16*8); //Lir.corr_see_cur_ch        
			 display.print_debug_one_local_shot(set_point_mines,0,18,16*9,8);// Кол - во интервалов

			 if(set_point_mines!=0) {
				 display.print_debug_one_local_shot(m+1,0,8,16,8);
				 int k= set_point_mines-m-1;
				 switch (k){
					 case 0:break;
					 case 1:
					 case 2:
					 case 3:
					 case 4:{
						 display.gotoxy(13,16);
						 display.put_char('-', 0);
						 display.print_debug_one_local_shot(set_point_mines,0,15,16,8);
						 }
					 break;
					 default:{
						 display.gotoxy(13,16);
						 display.put_char('-', 0);
						 display.print_debug_one_local_shot(m+5,0,15,16,8);
					 }
				 }
			 }
		 }
		 else
			display.put_char_local_independent(' ',0,24,0);
	 }	  
//	display.gotoxy(14,16);
//	display.put_char('-', 0);
//	display.print_debug_one_local_shot(m+5,0,16,16);
//	init.clr_B6();
static char update_table_corr_two;
if(Lir.update_table_corr || update_table_corr_two){
    if(error_when_work_with_RAM_or_FLASH==false){

		if(!correction_must_be_done){			
			Lir.update_table_corr = 0;			
		}
		
		if(update_table_corr_two)
			update_table_corr_two = 0;
		else
			update_table_corr_two = 1;

		 for(;n<5;n++,m++){
//29.05.2018-----------------	 
			 if(table_plus==true){
				 //if(table_is_ready_plus==false)
				//	break;
				// else{
					 if(m>=set_point_plus || table_is_ready_plus==false){
						display.print_symbols_local(' ',40 - 7,0,0,16*(n+2));
						continue;//break;
					 }
				// }
			 }
			 else if(table_plus==false){
				 //if(table_is_ready_mines==false)
				//	 break;
				 //else{
					 if(m>=set_point_mines || table_is_ready_mines==false){
						display.print_symbols_local(' ',40 - 7,0,0,16*(n+2));
						continue;//break;
					 }
				// }
			 }
			// рабочий
			rab = k_div * (m + 1);
			//rab = rab * k_minus;
			coeff.mult(cur_see_ch,&rab); //Lir.corr_see_cur_ch
			Lir.print_long_format(rab,Lir.get_format_from_ch(cur_see_ch),0,10,16*(n+2),1,0,11);//Lir.corr_see_cur_ch	 		 	 
			//=======

			// эталон
			 cli();
			if(table_plus == true)
				et = ref_.corr.pt_plus[n];//tmp=corr_plus[n];
			else 
				et = ref_.corr.pt_mines[n];
			sei(); 


			if((ref_.t_r_m_auto && !table_plus) || (ref_.t_r_p_auto && table_plus)){
				/*if(ref_.table_m_from_flash){
					coeff.mult(255,&et,coeff.coeff_corr[cur_see_ch].n_after_dot,coeff.coeff_corr[cur_see_ch].binary);
				}
				else if(ref_.table_p_from_flash){
					coeff.mult(255,&et,coeff.coeff_corr[cur_see_ch].n_after_dot,coeff.coeff_corr[cur_see_ch].binary);
				}
				else
					coeff.mult(0,&et);  */
				Lir.print_long_format(et,Lir.get_format_from_ch(0),0,22,16*(n+2),1,0,11);  
			}
			else{
				coeff.mult(cur_see_ch,&et);  
				Lir.print_long_format(et,Lir.get_format_from_ch(cur_see_ch),0,22,16*(n+2),1,0,11);  
			}
			//======

			// дельта
			//delta=k_div*(m+1)-tmp2;
			 delta = rab - et;
				if(table_plus==true) 
					delta=-delta;
			// answer=coeff.mult(Lir.corr_see_cur_ch,&delta);
			if(!update_table_corr_two)
				Lir.print_long_format(delta,Lir.get_format_from_ch(cur_see_ch),0,32,16*(n+2),1,0,9);  //Lir.corr_see_cur_ch              
		   }			   	   
	   }
	}
//======		 
     if(flash_prg.sync_corr_see){
		ref_.ref_blinker++;
		if(ref_.ref_blinker&0x04) 
			INVERT_CHAR()//invert_char=1;
		else  
			NORMAL_CHAR()//invert_char=0;			
	 }
	 else
		flash_prg.to_take_table =false;


	 display.print_flash_local(PSTR("СИНХ"),0,35,12);
	 NORMAL_CHAR()

    show_channels_in_see_corr(Lir.corr_see_cur_ch);

//28.05.2018	
 	if(table_plus==true)
		display.print_flash_local(PSTR("+"),0,5,16);
 	else 
		display.print_flash_local(PSTR("-"),0,5,16);

	display.print_ram_local(Lir.Str_Channel[Lir.cnt_StrCh],0,36,64);//Отображение в кнопке
   
//27.12.2018
cli();
char d_23=spi.debug_23LCV1024;
exchange_error_=exchange_error;
sei();
if(d_23!=3){
	display.print_flash_local(PSTR(" Память 23LCV1024 неисправна"),0,0,32);
	display.print_flash_local(PSTR(" Код ошибки:"),0,0,48);
	display.print_debug_one_local(d_23,0,13,48);
	error_when_work_with_RAM_or_FLASH=true;
}
if(execute==EXEC::THIRD){
	switch (exchange_error_){
		case EXCHANGE_ERRORS::WRITE_FIRST_TIME_MINES:
		display.print_flash_local(PSTR(" Ошибка памяти 45db082-"),0,0,64);
		stop_flashing=true;error_when_work_with_RAM_or_FLASH=true;
		break;
		case EXCHANGE_ERRORS::WRITE_FIRST_TIME_PLUS:
		display.print_flash_local(PSTR(" Ошибка памяти 45db082+"),0,0,64);
		stop_flashing=true;error_when_work_with_RAM_or_FLASH=true;
		break;
		case EXCHANGE_ERRORS::READ_FROM_23LCV1024:
		display.print_flash_local(PSTR(" Ошибка чтения 23lcv1024"),0,0,64);
		stop_flashing=true;error_when_work_with_RAM_or_FLASH=true;
		break;
		case EXCHANGE_ERRORS::TIMEOUT_WHEN_WRITE_TO_FLASH:
		display.print_flash_local(PSTR(" Ошибка записи 45db082e"),0,0,64);
		stop_flashing=true;error_when_work_with_RAM_or_FLASH=true;
		break;
		case EXCHANGE_ERRORS::TIMEOUT_WHEN_READ_FROM_FLASH_FIRST_TIME:
		display.print_flash_local(PSTR(" Ошибка чтения 45db082e"),0,0,64);
		stop_flashing=true;error_when_work_with_RAM_or_FLASH=true;
		break;
		case EXCHANGE_ERRORS::WRITE_TO_23LCV1024:
		display.print_flash_local(PSTR(" Ошибка записи 23lcv1024"),0,0,64);
		stop_flashing=true;error_when_work_with_RAM_or_FLASH=true;
		break;
		case EXCHANGE_ERRORS::READ_FROM_45DB081B:
		display.print_flash_local(PSTR(" Ошибка чтения 45db082+"),0,0,64);
		stop_flashing=true;error_when_work_with_RAM_or_FLASH=true;
		break;
		case EXCHANGE_ERRORS::TASK_IS_ENDED:
		//display.print_flash_local(PSTR(" Задание выполнено"),0,0,64);
		stop_flashing=true;//error_when_work_with_RAM_or_FLASH=true;
		 cli();to_see_corr=0;sei();
		break;
		case EXCHANGE_ERRORS::OK:stop_flashing=false; break;
		default:stop_flashing=false; break;
		
	}
	
}
//-----------------------------------------------	 
 

	   switch(execute){
	   case  EXEC::FIRST:{
/*	   display.print_flash_local(PSTR("ВВОД"),0,35,128);break;  */
	   break;	   
	   }
	   case  EXEC::SECOND:{
// 		    cli(); int tmp=timeout_to_go;sei();
// 		    if(tmp==0) { execute=EXEC::FIRST;
// 			 display.print_flash_local(PSTR("             "),0,25,128);break;}
// 			display.print_flash_local(PSTR("     "),0,35,128);
// 			display.print_flash_local(PSTR("ВВОД ЕЩЕ РАЗ"),0,25,128);
// 		    display.print_debug_one_local((tmp/100)+1,0,39,128); 
		    // display.print_flash_local(PSTR(""),0,27,128);break;
		     break;
	     }
	  case  EXEC::THIRD:{
//10.07.2018 it is clear ENTER		  
			 //display.print_flash_local(PSTR("               "),0,25,128);
			 
//----------------------------			 
			 time_to_flash++;
//10.07.2018 it is flashing			 
			 if(time_to_flash&0x02)
				display.print_flash_local(PSTR("             "),0,0,16*7);
			 else{ 
					switch(up_or_down){
						case UPLOAD_FLASH_0_TO_RAM:
						display.print_flash_local(PSTR(" ФЛЕШ 0 В ОЗУ"),0,0,16*7);break;
						case UPLOAD_FLASH_1_TO_RAM:
						display.print_flash_local(PSTR(" ФЛЕШ 1 В ОЗУ"),0,0,16*7);break;
						case UPLOAD_FLASH_2_TO_RAM:
						display.print_flash_local(PSTR(" ФЛЕШ 2 В ОЗУ"),0,0,16*7);break;
						case UPLOAD_FLASH_3_TO_RAM:
						display.print_flash_local(PSTR(" ФЛЕШ 3 В ОЗУ"),0,0,16*7);break;
						case DOWNLOAD_RAM_TO_FLASH_0:
						display.print_flash_local(PSTR("ОЗУ ВО ФЛЕШ 0"),0,0,16*7);break;
						case DOWNLOAD_RAM_TO_FLASH_1:
						display.print_flash_local(PSTR("ОЗУ ВО ФЛЕШ 1"),0,0,16*7);break;
						case DOWNLOAD_RAM_TO_FLASH_2:
						display.print_flash_local(PSTR("ОЗУ ВО ФЛЕШ 2"),0,0,16*7);break;
						case DOWNLOAD_RAM_TO_FLASH_3:
						display.print_flash_local(PSTR("ОЗУ ВО ФЛЕШ 3"),0,0,16*7);break;
					}					
				}
//-------------------------			 
//05.06.2018 28*70=1960 ms			 
			 if((time_to_flash>15 && stop_flashing==true)||to_stop_if_key_C_is_pressed==true) {
				display.print_flash_local(PSTR("             "),0,0,16*7);
			    cli();correction_must_be_done=false;sei();	 
				execute=EXEC::FIRST;
				to_stop_if_key_C_is_pressed=false;
				
			 }
			 break;
		 } 
	   
	   }	 
}
/*
int8_t c - канал
*/
void CPrg::show_channels_in_see_corr(int8_t c)
{
	cli();
	long corr_simple;
	long real;
	long et;
	//long corr_complex=counter1.correction_complex;
	//long corr_right=counter1.correction_right;	
	real = mCounter[c].abs;	
	et = mCounter[0].abs;
	corr_simple = mCounter[c].correction_simple;
	altera.buf = altera.result;

	real = Lir.conv_c_i_c(real,c,ref_.pRef[c]);
	et = Lir.conv_c_i_c(et,c,ref_.pRef[c]);		

	bool table_plus = ref_.table_plus;
	uint8_t cur_see_ch = c;//Lir.corr_see_cur_ch;
	sei();

	if(table_plus && ref_.t_r_p_auto && !ref_.table_m_from_flash){
		cur_see_ch = 1;
	}

	if(!table_plus && ref_.t_r_m_auto && !ref_.table_m_from_flash){
		cur_see_ch = 1;
	}

	display.print_ram_local(Lir.Str_Channel[cur_see_ch],0,12,11 * 16);    //Рабочий

	if((ref_.t_r_m_auto && !ref_.table_plus) || (ref_.t_r_p_auto && ref_.table_plus))
		display.print_ram_local(Lir.Str_Channel[0],0,12,12 * 16);			       //Эталон
	else
		display.print_ram_local(Lir.Str_Channel[cur_see_ch],0,12,12 * 16);//Рабочий == Эталон

	display.print_ram_local(Lir.Str_Channel[cur_see_ch],0,12,14 * 16);    //Коррек. канал

	//Рабочий канал 1:
	coeff.mult(cur_see_ch,&real);
	Lir.print_long_format(real,Lir.get_format_from_ch(cur_see_ch),0,27,176,1,0,11);
	
	//Коррек. канал 1:
	if((ref_.t_r_p_auto && corr_simple >= 0 )||(ref_.t_r_m_auto && corr_simple < 0 )){
		/*if(ref_.table_m_from_flash){
			coeff.mult(255,&corr_simple,coeff.coeff_corr[cur_see_ch].n_after_dot,coeff.coeff_corr[cur_see_ch].binary);
		}
		else if(ref_.table_p_from_flash){
			coeff.mult(255,&corr_simple,coeff.coeff_corr[cur_see_ch].n_after_dot,coeff.coeff_corr[cur_see_ch].binary);
		}
		else
			coeff.mult(0,&corr_simple);  
		} */
		;}
	else
		coeff.mult(cur_see_ch,&corr_simple);  

	Lir.print_long_format(corr_simple,Lir.get_format_from_ch(cur_see_ch),0,27,224,1,0,11);
		

	//Эталон канал 0:
	if((table_plus && ref_.t_r_p_auto) || (!table_plus && ref_.t_r_m_auto)){//только для таблицы сделанный с эталоном
		if(ref_.table_m_from_flash){
			coeff.mult(255,&et,coeff.coeff_corr[cur_see_ch].n_after_dot,coeff.coeff_corr[cur_see_ch].binary);
		}
		else if(ref_.table_p_from_flash){
			coeff.mult(255,&et,coeff.coeff_corr[cur_see_ch].n_after_dot,coeff.coeff_corr[cur_see_ch].binary);
		}
		else
			coeff.mult(0,&et);  

		Lir.print_long_format(et,Lir.get_format_from_ch(0),0,27,192,1,0,11);
	}
	else
		{
			//et = real;
			et = corr_simple;
			Lir.print_long_format(et,Lir.get_format_from_ch(cur_see_ch),0,27,192,1,0,11);
		}

	//Дельта:
	long delta_ = real - et;
	if(ref_.table_plus == true)
		delta_= -delta_;
	Lir.print_long_format(delta_,Lir.get_format_from_ch(cur_see_ch),0,27,208,1,0,11);

	

	switch(altera.buf){
		case 0: display.put_char_local_independent('>',0,28,224);break; // "out +" TABLE_IS_ENDED_PLUS=0,
		case 19:display.put_char_local_independent('<',0,28,224);break; // "out -" TABLE_IS_ENDED_MINES=19,

		case 21:display.put_char_local_independent('<',0,28,224);break; // TABLE_IS_ENDED_PLUS_L=21,
		case 22:display.put_char_local_independent('>',0,28,224);break; // TABLE_IS_ENDED_MINES_R=22,
		default:display.put_char_local_independent(' ',0,28,224);break;
	}
}

//#pragma GCC pop_options
void CPrg::WorkCorr_input_loop(void)
{   
	static unsigned char trigger,mem_pos_ukazetel;//;,i2;
	int x_cursor;
	int y_cursor;
	static unsigned int point;
	static long interval;

	if(~trigger&1)
	{
		Lir.ResultForInputNum = 0;
		if(Lir.WorkCorr_entry == NUM_POINT){
			if((ref_.table_plus && ref_.table_is_ready_plus) || ((ref_.table_plus == false) && ref_.table_is_ready_mines )){  // если таблица + или - ок ,то переходим в режим редактирования таблицы
				display.print_flash_local(PSTR("Номер точки:"),0,0,112);
				point = 0;
			}
			else{
				Lir.SET_C(idSeeCorr);
				return;
			}
		}
		if(Lir.WorkCorr_entry == SET_INTERVAL){
			interval = 0;
			trigger&=~(1<<2);	
			trigger&=~(1<<3);		
		}
		trigger |= 1;	// i = 1;
	}

	if(Lir.WorkCorr_entry == SET_INTERVAL)     
	 {
		if(~trigger&4)
		{
			Lir.CurFormatM = flash_prg.axis_cfg[Lir.cnt_StrCh].format;
			Lir.key_point_counter = 0;			
			display.print_symbols_local(' ',15,0,18,128);
			trigger |= 4; 			
		}
		 x_cursor = 18;
		 y_cursor = 128;
	 }
	if(Lir.WorkCorr_entry == SET_COUNT_INTERVAL)
	{
		if(~trigger&8)
		{
			display.print_symbols_local(' ',15,0,18,144);
			trigger |= 8;
		}
		x_cursor = 18;
		y_cursor = 144;
	}
	if(Lir.WorkCorr_entry == NUM_POINT){
	x_cursor = 12;
	y_cursor = 112;
	}
	if(Lir.WorkCorr_entry == EDIT_POINT || Lir.WorkCorr_entry == NEXT_POINT || Lir.WorkCorr_entry == PREV_POINT)
	{
		if(~trigger&2)
		{
			display.print_flash_local(PSTR("Введите отклонение:"),0,0,112);
			trigger |= 2; //i2 = ~i2;
		}
		x_cursor = 19;
		y_cursor = 112;
	}


	Lir.flashing_cursor(x_cursor,y_cursor);
	
	if(Lir.ResultForInputNum == input::ok)
	{
		if(Lir.WorkCorr_entry == SET_INTERVAL)
			{
				// здесь забираем размер интервалов
				interval = Lir.String_to_bin(Lir.bufASCIIstr,Lir.cnt_StrCh);//Lir.strtoi_lir(Lir.bufASCIIstr);

				coeff.div(Lir.cnt_StrCh,&interval);//Lir.cnt_StrCh// 1
				if(interval > MAX_INTERVAL_PLUS || interval < MAX_INTERVAL_MINUS){ 					
					display.print_flash_local(PSTR(" макс    "),0,x_cursor,y_cursor);
					delay_ms(700);
					Lir.init_cursor_after_input(idWorkCorr_Input,18,128,10);				
					return;
					}
				
				if(interval < 0 && ref_.table_plus)	
					interval = -interval;

				if(interval > 0 && !ref_.table_plus)
					interval = -interval;
					
				Lir.WorkCorr_entry = SET_COUNT_INTERVAL;
				Lir.init_cursor_after_input(idWorkCorr_Input,18,128,0);
				trigger&=~(1<<2);
				return;
			}
		if(Lir.WorkCorr_entry == SET_COUNT_INTERVAL)
			{
				// здесь забираем кол-во интервалов и формируем таблицу в чипе озу, нужна ли очистка чипа?
				int count_interval = Lir.strtoi_lir(Lir.bufASCIIstr);
				long etalon = 0;
				if(count_interval > CORR_POINTS) 
					count_interval = CORR_POINTS;
				if(count_interval < 0)   
					count_interval = 0;
				count_interval  = Lir.check_max_set_point(interval,count_interval,Lir.cnt_StrCh);
				cli();
				if(ref_.table_plus)
				{
					ref_.k_divider_plus = interval;
					ref_.set_point_plus = count_interval;
				}
				else
				{
					ref_.k_divider_mines = interval;
					ref_.set_point_mines = count_interval;	
				}
				sei();

				for(int point = 0; point < count_interval; point++)
				{
					etalon += interval;
					cli();
					// 3,47 mks one point ,if points(count_interval) will be 15 000 the writing time will be equal about to 52 ms
					flash_prg.write_bytes_to_23LCV1024(point*4,(unsigned char*)&etalon,ref_.table_plus,sizeof(etalon));
					sei();
				}

				cli();
				if(ref_.table_plus){
					ref_.table_is_ready_plus = true;
					ref_.t_r_p_auto = 0;
					}
				else{
					ref_.table_is_ready_mines = true;
					ref_.t_r_m_auto = 0;
					}
				sei();

				Lir.init_cursor_after_input(idSeeCorr,18,144,0);
				trigger&=~(1<<3);
				trigger&=~(1<<0);
				
				to_see_corr=0;
				//display.clear_rectangle(0,4,272,10,0x00,0);
				
				Lir.update_table_corr = 1;
				Lir.corr_see_cur_ch = Lir.cnt_StrCh;
				display.put_char_local_independent('*',0,16,0);
				t_save = 1;
			}
		if(Lir.WorkCorr_entry == NUM_POINT || Lir.WorkCorr_entry == NEXT_POINT || Lir.WorkCorr_entry == PREV_POINT)
			{	
				// здесь забираем номер точки ,проверяем ее ,и показываем точку в таблице с помощью '>'
				unsigned char y_pos = 16;
				if(Lir.WorkCorr_entry == NUM_POINT)
					point = Lir.strtoi_lir(Lir.bufASCIIstr);  
				if(Lir.WorkCorr_entry == NEXT_POINT)
					point++;
				if(Lir.WorkCorr_entry == PREV_POINT)
					point--;

				if(point == 0) 
					point = 1;
				int a = point;
				if(a < 0)
					{
					Lir.ResultForInputNum = input::cancel_;
					return;
					}

				if(ref_.table_plus)
					a = ref_.set_point_plus;		
				else 
					a = ref_.set_point_mines;	

				if(point > (unsigned int)a)
					point = a;

				for(unsigned int i = 0 ; i < CORR_POINTS; i += 5)
				{
					if(point > i && point < i + 6)
					{
						y_pos = (16 * (point - i)) + y_pos;

						//--------------------------смещение указателя '>' в центр----------------
						if(y_pos == 32 && point > 3){
							y_pos = 64;
							i = i - 2;
						}
						if(y_pos == 48 && point > 3){ 
							y_pos = 64;
							i = i - 1;
						}

						if(y_pos == 80 && point < (unsigned int)a - 1){
							y_pos = 64;
							i = i + 1;
						}
						if(y_pos == 96 && point < (unsigned int)a){
							y_pos = 64;
							i = i + 2;
						}
						//------------------------------------------------------------------------
						if(mem_pos_ukazetel != 0){
							display.put_char_local_independent(' ',0,33,mem_pos_ukazetel);	// стираем предыдущий указатель
						}

						mem_pos_ukazetel = y_pos;
						to_see_corr = i;
						delay_ms(20);                // время для обновления для прерывания //Lir.go_to_menuItem_and_out(1,&Lir.CorrSee,&Lir.mWorkCorrInput,1);
						Lir.update_table_corr = 1;	
						to_see_correction();
						to_see_correction();					
						break;
					}
				}
				
				Lir.WorkCorr_entry = EDIT_POINT; // переходим к режиму редактирования точки
				Lir.init_cursor_after_input(idWorkCorr_Input,19,112,14);
				display.put_char_local_independent('<',0,33,y_pos);			
				trigger&=~(1<<1);//i2 = 0;
				return;
			}
		if(Lir.WorkCorr_entry == EDIT_POINT)
			{
				// здесь забираем дискрету(inc) точки(point) и грузим в отдельный чип озу по адресу(point)
				display.put_char_local_independent(' ',0,33,mem_pos_ukazetel);
				long inc = Lir.String_to_bin(Lir.bufASCIIstr,Lir.corr_see_cur_ch);//Lir.strtol_lir(Lir.bufASCIIstr);				
				coeff.div(Lir.corr_see_cur_ch,&inc);
				long long r = inc;
				if(Lir.cursor_pos != 0){
					if(ref_.table_plus){
						r = (ref_.k_divider_plus  * point) + r;
					}
					else{
						r = (ref_.k_divider_mines * point) - r;
					}
					if(r > MAX_INTERVAL_PLUS || r < MAX_INTERVAL_MINUS){
						if(ref_.table_plus)
							inc = ref_.k_divider_plus  * point;
						else
							inc = ref_.k_divider_mines  * point;
					}
					else
						inc = r;

					cli();// 3,47 mks one point
					flash_prg.write_bytes_to_23LCV1024((point - 1)*4,(unsigned char*)&inc,ref_.table_plus,sizeof(inc));
					sei();
					Lir.update_table_corr = 1;
					display.put_char_local_independent('*',0,16,0);
					t_save = 1;
				}
				//------------------------конец ввода возвращаемся к CorrSee----------
				//Lir.init_cursor_after_input(Lir.CorrSee,0,112,40);	
				//trigger&=~(1<<0);	//i = false;
				//--------------------------------------------------------------------
				//------------------------переход к следущей точке--------------------
				Lir.init_cursor_after_input(idWorkCorr_Input,19,112,14);
				Lir.WorkCorr_entry = NEXT_POINT;
				Lir.ResultForInputNum = input::ok;
				//--------------------------------------------------------------------

			}		
	}

	if(Lir.ResultForInputNum == input::cancel_)
	{
		if(Lir.WorkCorr_entry == EDIT_POINT || Lir.WorkCorr_entry == NUM_POINT)
		{
			Lir.init_cursor_after_input(idSeeCorr,0,112,30);
			display.put_char_local_independent(' ',0,33,mem_pos_ukazetel);		
		}
		if(Lir.WorkCorr_entry == SET_INTERVAL || Lir.WorkCorr_entry == SET_COUNT_INTERVAL)
		{
			Lir.init_cursor_after_input(idSeeCorr,x_cursor,y_cursor,14);
			//display.put_char_local_independent(' ',0,23,mem_pos_ukazetel);
		}
		trigger = 0;		
	}	
}


//19.12.2018
void CPrg::item_correction_inside_INPUT_PARAM(void){
if(flash_prg.timeout_to_go<=0)
	flash_prg.timeout_to_go=0;
else 
	flash_prg.timeout_to_go--;
//-----------------------------	
 if(flash_prg.test_23LCV1024())
 {	
	 if(correction_must_be_done==true){
	   exchange_error=flash_prg.to_exchange_45DB081B_with_23LCV1024();
	  switch (exchange_error){
		case  EXCHANGE_ERRORS::READ_FROM_45DB081B:
		case  EXCHANGE_ERRORS::READ_FROM_23LCV1024:  
		case  EXCHANGE_ERRORS::WRITE_TO_23LCV1024:
        case  EXCHANGE_ERRORS::TIMEOUT_WHEN_READ_FROM_FLASH_FIRST_TIME:
		case  EXCHANGE_ERRORS::WRITE_FIRST_TIME_MINES:  
		case  EXCHANGE_ERRORS::TIMEOUT_WHEN_WRITE_TO_FLASH:
	    case  EXCHANGE_ERRORS::WRITE_FIRST_TIME_PLUS:
		case  EXCHANGE_ERRORS::TASK_IS_ENDED:
		correction_must_be_done=false;break;
		default:break;  
	  }
	  
	  //return;
	 }	 
	 // init.set_B6();
	 //08.07.2018 it is used only for readout(to display)
	 flash_prg.to_read_five_from_23LCV1024();
	 //init.clr_B6();
 }
	
  if(MsgKey.mode != TMsgKey::MODE::FIND_REF)
	altera.correction_is_taking(MsgKey.switch_number_for_ref,&init.p_in);
   cli();
   bool to_take_t=flash_prg.to_take_table;
   sei();
   if(to_take_t==false && flash_prg.table_is_used==true){
	 altera.result= altera.correction_is_working_skv(Lir.corr_see_cur_ch);
   }
	
}

void CPrg::check_iar_eeprom(void)
{
#define IAR_EEPROM_FIRST_BYTE1 0x80
#define IAR_EEPROM_FIRST_BYTE2 0x00
	bool error = false;
	unsigned char byte;
	byte = read_one_byte_eeprom_address_int(0,&error);
	unsigned char buf[32];
	memset(buf,0xff,sizeof(buf));
	if(byte == IAR_EEPROM_FIRST_BYTE1)    // fist check
	{
		Lir.ResetParametrsinRAM();
		NVM.CTRLB &= ~NVM_EEMAPEN_bm ;
		EEPROM_EraseAll();
	}
	else
	{
			byte = read_one_byte_eeprom_address_int(1920,&error);   // second check
			if(byte == IAR_EEPROM_FIRST_BYTE2)
			{
				Lir.ResetParametrsinRAM();
				NVM.CTRLB &= ~NVM_EEMAPEN_bm ;
				EEPROM_EraseAll();
			}
	}
}

//01.06.2018
void CPrg::to_do_correction(void){
//04.05.2018 worst case it takes 3.64 ms	 
	   cli();
	   altera.counter_display[1]=altera.counter[1];
	   long rel =mCounter[1].rel;
	   altera.counter_display[0]=altera.counter[0];
	   bool enable_to_go_for_correction=ref_.enable_to_go_for_correction;
	   ref_.fol=ref_.follower;
	   bool enable=ref_.is_enabled_PA0_int_1();
	   int k_divider=ref_.k_divider;
	   int set_point=ref_.set_point;
	   sei();

	   if(enable ){
		   ref_.ref_blinker++;
		   if(ref_.ref_blinker&0x04) INVERT_CHAR()//invert_char=1;
		   else  NORMAL_CHAR()//invert_char=0;
		   //display.print_debug_one_local(mCounter[1].rel_,0,18,144);
		   long data = rel;
		   coeff.mult(1,&data);
		   Lir.print_long_format(data,Lir.get_format_from_ch(1),0,18,9*16,LEFT_ALIGN,0,11);

		   display.print_debug_one_local(altera.counter_display[1],0,18,128);
	   }
	   else {NORMAL_CHAR()
		   //display.print_debug_one_local(mCounter[1].rel_,0,18,144);
		   long data = rel;
		   coeff.mult(1,&data);
		   Lir.print_long_format(data,Lir.get_format_from_ch(1),0,18,9*16,LEFT_ALIGN,0,11);
		   display.print_debug_one_local(altera.counter_display[1],0,18,128);
	   }
	   NORMAL_CHAR()
	   //display.print_debug_one_local(coeff.new_gap[1].binary,0,18,160);
	   //==Люфт==
	   long tmp_=flash_prg.axis_cfg[1].gap;
	   coeff.mult(1,&tmp_);
	   Lir.print_long_format(tmp_,flash_prg.axis_cfg[1].format,0,18,16 * 10,LEFT_ALIGN,0,11);

	   //==
	   //display.print_debug_one_local(ref_.k_divider,0,22,80);
	   if(enable_to_go_for_correction==true){
		   ref_.ref_blinker++;
		   if(ref_.ref_blinker&0x04) INVERT_CHAR()//invert_char=1;
		   else  NORMAL_CHAR()//invert_char=0;
		   display.print_flash_local(sSTOP,0,35,176);
		   NORMAL_CHAR()
	   }
	   display.print_debug_one_local(k_divider,0,21,32); // Дискрет в интервале
	   long tmp = k_divider;	   
	   coeff.mult(1,&tmp);
	   Lir.print_long_format(tmp,Lir.get_format_from_ch(1),0,21,0,0,0,10); // Размер интервалов
	   display.print_debug_one_local(set_point,0,21,16);
	   NORMAL_CHAR()
	   cli();
	   long counter=ref_.etalon_counter;
	   long meaning=ref_.etalon_meaning;
	   long speed=ref_.speed;
	   sei();
	   
	   if(counter>0){
		   display.print_debug_one_local(counter,0,18,80);
		   display.print_debug_one_local(meaning,0,18,96);
		   
	   }
	   else {
		   display.print_flash_local(PSTR("           "),0,18,80);  // Номер интервала:
		   display.print_flash_local(PSTR("           "),0,18,96);	// Эталон значение:
	   }
	   //	 }
	   display.print_debug_one_local(speed,0,21,192);
	   
	   //12.04.2018
	   display.print_debug_one_local(altera.counter_display[0],0,18,64);
	   //24.04.2018
	   //display.print_debug_one_local(ref_.mistake,0,21,208);
//13.12.2018 it must be clear,what has been happened
        switch (ref_.mistake){
		case CPorta_ref::NO_FAULT:
		display.print_flash_local(PSTR("нет            "),0,21,208);break;
		case CPorta_ref::SPEED_LIMIT:
		display.print_flash_local(PSTR("предел скорости"),0,21,208);break;
		case CPorta_ref::IF_MINES_FAULT:
		display.print_flash_local(PSTR("период -"),0,21,208);break;
		case CPorta_ref::IF_PLUS_FAULT:
		display.print_flash_local(PSTR("период +"),0,21,208);break;
		case CPorta_ref::MULT:
		display.print_flash_local(PSTR("умножение"),0,21,208);break;
		case CPorta_ref::NO_CONNECTOR:
		display.print_flash_local(PSTR("не готов"),0,21,208);break;
		}
//	cli();	 ref_.check_counter_mem=ref_.check_counter; sei();
//	 display.print_debug_one_local(ref_.check_counter_mem,0,0,224);   
	   
}


void CPrg::GetCorr_input_loop(void)
{
  cli(); 
  ref_.follow=ref_.follower; 
  sei();	
		
	static bool i;

	int x , y ;
	if(ref_.follow== CPorta_ref::CORRECTION::SET_POINT)
	{	
		x = 21;
		y = 16;
		Lir.flashing_cursor(x,y);
	}
	if(ref_.follow== CPorta_ref::CORRECTION::SET_K)
	{
		x = 21;
		y = 0;
		Lir.flashing_cursor(x,y);
	}
	if(!i)
	{
		Lir.ResultForInputNum = 0;
		display.print_flash_local(sProbel10,0,x,y);
		i = true;
	}

	if(Lir.ResultForInputNum == input::ok)
	{		
		long result;
		if(ref_.follow == CPorta_ref::CORRECTION::SET_K)
		{
			result = Lir.String_to_bin(Lir.bufASCIIstr,1);
			coeff.div(1,&result);

			ref_.inverter_set_k=false;
			if(result >MAX_INTERVAL_PLUS_A || result <MAX_INTERVAL_MINUS_A){
				//display.print_flash_local(PSTR(" макс    "),0,x,y);
				//delay_ms(700);
				//Lir.init_cursor_after_input(Lir.mGetCorrInput,x,y,10);
				Lir.status(PSTR("Макс"),700,0);
				Lir.ResultForInputNum = input_idle;
				return;
			}
			result  = Lir.check_max_k_divider(result);
			cli();
			ref_.k_divider = result;
			sei();	
		}
		if(ref_.follow == CPorta_ref::CORRECTION::SET_POINT)
		{
			result = Lir.strtoi_lir(Lir.bufASCIIstr);
			ref_.inverter_set_point=false;
			if(result >CORR_POINTS) 
				result = CORR_POINTS;
			if(result <0 )   
				result = 0;

			result  = Lir.check_max_set_point(ref_.k_divider,result,1);
			cli();
			ref_.set_point = result;
			sei();
		}
		
		Lir.init_cursor_after_input(idGetCorr,x,y);
		//ref_.follow=CPorta_ref::CORRECTION::WAIT;
		i = false;		
		
	}

	if(Lir.ResultForInputNum == input::cancel_)
	{
		if(ref_.follow== CPorta_ref::CORRECTION::SET_K)ref_.inverter_set_k=false;
		if(ref_.follow== CPorta_ref::CORRECTION::SET_POINT)ref_.inverter_set_point=false;
		Lir.init_cursor_after_input(idGetCorr,x,y);
		//ref_.follow=CPorta_ref::CORRECTION::WAIT;
		i = false;	
	}
	
}

//06.04.2018 correction is started to be...
void CPrg::to_take_buttons_code_for_correction(char k_c){
//08.06.2018 All variables must be taken right	
   cli();
   bool enable_to_go_for_correction=ref_.enable_to_go_for_correction;
   bool to_take_t=to_take_table;
   ref_.follow=ref_.follower;
   sei();	
	
	switch (k_c){
	case _key_STOP:
			cli();
			ref_.follower=CPorta_ref::CORRECTION::STOP;
			ref_.enable_to_go_for_correction=false;
			flash_prg.to_take_table=false;
			sei();
			Lir.entry_GTCOR = Menu::INIT;
			Lir.SET_C(idCorr);
	break;
	case _key_UP:
	case _key_DOWN:	
	case _key_RIGHT:
	case _key_LEFT:
	break;	
	case _key_ENTER:
	break;
	case _key_BLANK1:// L
		if(ref_.follow==CPorta_ref::CORRECTION::WAIT)
			break;
			if(enable_to_go_for_correction==false){
				if( ref_.inverter_set_k==false)	{	
					ref_.follow=CPorta_ref::CORRECTION::SET_K; // Размер интервалов
					ref_.inverter_set_k=true;
					Lir.SET_C(idGetCorr_input);
					Lir.CurFormatM = flash_prg.axis_cfg[1].format;
					Lir.key_point_counter = 0;
			}
		}
	break;
	case _key_BLANK2:// N
	if(ref_.follow==CPorta_ref::CORRECTION::WAIT)break;
	if(enable_to_go_for_correction==false){
		if( ref_.inverter_set_point==false){
			ref_.follow=CPorta_ref::CORRECTION::SET_POINT;// Кол - во интервалов
			ref_.inverter_set_point=true;
			Lir.SET_C(idGetCorr_input);
		}
	}
	break;
	case _key_BLANK3: //ПУСК
	if(enable_to_go_for_correction==false){
	ref_.table_m_from_flash = 0;
	ref_.table_p_from_flash = 0;
	Lir.cnt_StrCh = 0;
		cli();bool enable=ref_.is_enabled_PA0_int_1();sei();	
		if(enable){
			ref_.follow=CPorta_ref::CORRECTION::STOP;
			}
			else {
				if(to_take_t==true)
					ref_.follow=CPorta_ref::CORRECTION::GO_TO_REF;

				unsigned char n_ch=1;	
				if(MsgKey.is_it_switch_or_no_for_ref(&n_ch)){
					cli();ref_.channel_1_has_switch_for_ref=true;sei();	
					display.print_flash_local(PSTR("Вход зоны REF   :"),0,1,22*8);
					display.print_debug_one_local_shot((axis_cfg[1].ref_pulse_zone & DATA_P_bm )+1,0,18,22*8,2);
					if(axis_cfg[1].ref_pulse_zone & INVERSE_P_bm){
						display.print_flash_local(PSTR("!"),0,20,22*8);	   	
						}
					}
					else
						ref_.channel_1_has_switch_for_ref=false;
				}
	}
	break;
	case _key_BLANK2_L:
	case _key_BLANK3_L:
	case _key_BLANK4_L:
	break;
	
	case _key_BLANK4:
//11.04.2018
	if(enable_to_go_for_correction==true&& ref_.blink_time_set_k==0){
	
		ref_.follow=CPorta_ref::CORRECTION::IF_STOP_PRESSED;
		
		//ref_.enable_to_go_for_correction=false;
	}
	break;
 	}
	cli();
	ref_.follower=ref_.follow;
	sei();
}

void  CPrg::only_for_debug(void){
		PORTF.OUT=coeff.answer;
		coeff.set_axis_coeff(0,1000,7);
		flash_prg.debug_=1000000;
		for(int n=0;n<1000;n++){
			coeff.answer=coeff.mult(0,&flash_prg.debug_);
			//coeff.answer=coeff.div(0,&flash_prg.debug_);
			PORTF.OUT=coeff.answer;
			if (coeff.answer==CTranslate::MULT_OR_DIV_ANSWER::OK){
				PORTF.OUT=(unsigned char)flash_prg.debug_;
				flash_prg.debug_*=1000;
			}
			else {
				flash_prg.debug_=0;
			}
		}			
}
//22.02.2018
void CPrg::to_save_type_of_axis(uint16_t *P20,uint16_t *P21,uint16_t *P22,uint16_t *P23){
	cli();
	if(*P20>=AXIS_A) {
		axis_cfg[0].type=TYPE::CIRCLE;
		mCounter[0].increment_0_360_one_turn=true;
	}
	else{ 
		axis_cfg[0].type=TYPE::LINEAR;
		mCounter[0].increment_0_360_one_turn=false;
	}

	if(*P21>=AXIS_A) {
		axis_cfg[1].type=TYPE::CIRCLE;
		mCounter[1].increment_0_360_one_turn=true;
	}
	else { 
		axis_cfg[1].type=TYPE::LINEAR;
		mCounter[1].increment_0_360_one_turn=false;
	}

	if(*P22>=AXIS_A) {
		axis_cfg[2].type=TYPE::CIRCLE;
		mCounter[2].increment_0_360_one_turn=true;
	}
	else {
		axis_cfg[2].type=TYPE::LINEAR;
		mCounter[2].increment_0_360_one_turn=false;
	}	

	if(*P23>=AXIS_A) {
		axis_cfg[3].type=TYPE::CIRCLE;
		mCounter[3].increment_0_360_one_turn=true;
	}
	else {
		axis_cfg[3].type=TYPE::LINEAR;
		mCounter[3].increment_0_360_one_turn=false;
	}
    sei();		

}
//21.02.2018 
void  CPrg::change_format_axis(int *num,char right_left){	
	
	if(!(*num==0||*num==1||*num==2||*num==3)) return;
	 FORMAT n= (FORMAT)axis_cfg[*num].format;
	if (axis_cfg[*num].type==TYPE::LINEAR){
		if(right_left==1){
		switch (n){
		case LINEAR_1:     n=LINEAR_2;break;
		case LINEAR_2:     n=LINEAR_3;break;
		case LINEAR_3:     n=LINEAR_4;break;
		case LINEAR_4:     n=LINEAR_6;break;
		case LINEAR_6:     n=LINEAR_INCH_4;break;
		case LINEAR_INCH_4:n=LINEAR_1;break;
		default:n=LINEAR_3;		
		}
		}
		else {
			switch (n){
				case LINEAR_1:     n=LINEAR_INCH_4;break;
				case LINEAR_2:     n=LINEAR_1;break;
				case LINEAR_3:     n=LINEAR_2;break;
				case LINEAR_4:     n=LINEAR_3;break;
				case LINEAR_6:     n=LINEAR_4;break;
				case LINEAR_INCH_4:n=LINEAR_6;break;
				default:n=LINEAR_3;
			}		
		}	
	}
	else {
		if(right_left==1){
			switch (n){
				case CIRCLE_1:        n=CIRCLE_2;break;				
				case CIRCLE_2:        n=CIRCLE_3;break;
				case CIRCLE_3:        n=CIRCLE_4;break;
				case CIRCLE_4:        n=CIRCLE_MINUTO;break;
				case CIRCLE_MINUTO:   n=CIRCLE_SECUNDO;break;
				case CIRCLE_SECUNDO:  n=CIRCLE_SECUNDO_1;break;
				case CIRCLE_SECUNDO_1:n=CIRCLE_1;break;
				default:n=CIRCLE_3;
			}
		}
		else {
			switch (n){
				case CIRCLE_1:        n=CIRCLE_SECUNDO_1;break;
				case CIRCLE_2:        n=CIRCLE_1;break;
				case CIRCLE_3:        n=CIRCLE_2;break;
				case CIRCLE_4:        n=CIRCLE_3;break;
				case CIRCLE_MINUTO:   n=CIRCLE_4;break;
				case CIRCLE_SECUNDO:  n=CIRCLE_MINUTO;break;
				case CIRCLE_SECUNDO_1:n=CIRCLE_SECUNDO;break;
				default:n=CIRCLE_3;
			}
		}
	}

#if (configUSE_ExtFormats == 0)
	if(n == LINEAR_6)
	{
		if(right_left==1)
			n = LINEAR_INCH_4;
		else
			n = LINEAR_4;
	}
	if(n == CIRCLE_SECUNDO_1)
	{
		if(right_left==1)
			n = CIRCLE_1;
		else
			n = CIRCLE_SECUNDO;
	}
#endif

#if (configUSE_INCH == 0)
	if(n == LINEAR_INCH_4)
	{
		if(right_left==1)
			n = LINEAR_3;
		else 
			n = LINEAR_2;
	}
#endif


//	if(right_left==1){
//		n++;
//		if(n>FORMAT::CIRCLE_SECUNDO){n=0;}
//	}
//	else {
//		n--;
//		if(n<FORMAT::LINEAR_3){n=8;}
//	}
	axis_cfg[*num].format=n;
	return;
	//show_format_axis(1);
//	display.print_flash_local(PSTR("          "),0,25,16);
//	display.print_debug_one_local( flash_prg.axis_cfg[1].format,0,35,192);
}


void  CPrg::change_mode_axis(int *num){
	if(!(*num==0||*num==1||*num==2||*num==3)) 
		return;
	if (axis_cfg[*num].type==TYPE::LINEAR){
		if(axis_cfg[*num].r_d==R_D::RADIUS)
			axis_cfg[*num].r_d=R_D::DIAMETER;
		else 
			axis_cfg[*num].r_d=R_D::RADIUS;
	}
}

void  CPrg::change_direction_axis(int *num){
	if(!(*num==0||*num==1||*num==2||*num==3)) 
		return;
	if (axis_cfg[*num].direction==DIRECTION::PLUS)
		axis_cfg[*num].direction=DIRECTION::MINES;
	else 
		axis_cfg[*num].direction=DIRECTION::PLUS;
}
void  CPrg::change_save_axis(int *num){
	if(!(*num==0||*num==1||*num==2||*num==3)) 
		return;
	if (axis_cfg[*num].save==SAVE::YES_SAVE)
		axis_cfg[*num].save=SAVE::NO_SAVE;
	else 
		axis_cfg[*num].save=SAVE::YES_SAVE;
}

//28.02.2018
void CPrg::change_encoder_fault_axis(int *num){
	if(!(*num==0||*num==1||*num==2||*num==3)) return;
	if (axis_cfg[*num].encoder_fault== ENCODER_FAULT::YES_FAULT)
		axis_cfg[*num].encoder_fault = ENCODER_FAULT::NO_FAULT;
	else 
		axis_cfg[*num].encoder_fault = ENCODER_FAULT::YES_FAULT;
	
}
//01.03.2018
void CPrg::change_readout_speed_axis(int *num){
		if(!(*num==0||*num==1||*num==2||*num==3)) return;
		if (axis_cfg[*num].readout_speed == READOUT_SPEED::YES_SPEED)
			axis_cfg[*num].readout_speed = READOUT_SPEED::NO_SPEED;
		else
			axis_cfg[*num].readout_speed = READOUT_SPEED::YES_SPEED;
}
/*
const signed char type - IN_P входной параметр , OUT_P - выходной параметр
const char dir - 1 - увеличение вправо, 0 - уменьшение влево
bool no_INV - 0 - инверсия есть  ,1 - инверсия отключена
char& P - байт, состоит из 8 битов:
Yes/No         Inv        P_or_m      Num
0(0x80)        0(0x40)    0(0x20)     0 - 31(0x1F)
*/
void CPrg::changeP(char& P,const signed char type,const char dir,bool no_INV)
{
	signed char n = P & DATA_P_bm;
	char inf = P & (YESNO_P_bm | INVERSE_P_bm | P_OR_m_bm);
	signed char MIN = 0;
	signed char MAX = 23;

	if(inf & P_OR_m_bm){  // входы выходы P
		if(type == OUT_P)
			MAX = 23;
	}
	else{// входы выходы m
		MAX = 31;
	}

	if(no_INV)
		inf |= INVERSE_P_bm;

	if (dir == 1) {
		if (inf & YESNO_P_bm)
		n = MAX;
		n++;
		if (n > MAX) {
			if (inf & YESNO_P_bm) {
				inf &= ~YESNO_P_bm;
				n = MIN;
			} else{
				inf |= YESNO_P_bm;
				n = MAX;
			}
		}
	}else {
		if (inf & YESNO_P_bm)
		n = MIN;
		n--;
		if (n < MIN) {
			if (inf & YESNO_P_bm) {
				inf &= ~YESNO_P_bm;
				n = MAX;
			}else {
				inf |= YESNO_P_bm;
				n = MIN;
			}
		}
	}
	P = n | inf;
}
/*
P - байт, состоит из :
Yes/No       Inv      P_or_m    Num
0(0x80)      0(0x40)  0(0x20)   макс 31(0x1F)
*/
void CPrg::show_P(char P,char type, int x, int y, char align)
{
	if (P & YESNO_P_bm) {
		if (align){
			display.print_flash_local(sNO, 0, x + 4, y, align); //"    нет"
			display.print_symbols_local(' ',4,0,x,y);
			}
		else{
			display.print_flash_local(sNO, 0, x, y, align); //"нет    "
			display.print_symbols_local(' ',4,0,x + 3,y);
			}
	}
	else {
		char str[8] = { 0 };
		int ptr = 0;
		if(type == IN_P)
			str[ptr++]= '<'; //'>'
		else if (type == OUT_P)
			str[ptr++]= '>'; //'<'

		long data = P & DATA_P_bm;
		if(P & P_OR_m_bm){
			data++;
			str[ptr++] = 'P';
			str[ptr++] = '1';
			str[ptr++] = '.';
		}
		else{
			str[ptr++] = 'm';
			if(type == IN_P)
				data += 32;
			}

		Lir.lltoa_lir(data, (unsigned char*)str + ptr);

		if (!(P & INVERSE_P_bm))
		display.strncatsym_lir(str, sizeof(str), '!');

		if (align) {
			Lir.add_filler((uint8_t*)str, 8, 8, ' ', 0);
			display.print_ram_local_RA(str, 0, x, y);
		}
		else {
			Lir.add_filler((uint8_t*)str, 8, 8, ' ', 1);
			display.print_ram_local(str, 0, x, y);
		}
	}
}

void CPrg::change_inv_P(char& P)
{
	if (P & YESNO_P_bm)
	;
	else if (P & INVERSE_P_bm)
		P &= ~INVERSE_P_bm;
	else
		P |= INVERSE_P_bm;
}
/*
переключение нет->P->m->нет
если ПЛК не используется то нет->P->нет
*/
void CPrg::change_MP(char& P,char type)
{

	if(P & YESNO_P_bm){
		P&=~ YESNO_P_bm;
		goto p;
	}
	else if (!(P & P_OR_m_bm)){
		P |= YESNO_P_bm;
		return;
		p:
		P |= P_OR_m_bm; // установить P
		if (type == IN_P && ((P & DATA_P_bm) > 23)) {
			P &= ~DATA_P_bm;
			P |= DATA_P_bm & 23;
		}
		else if (type == OUT_P && ((P & DATA_P_bm) > 23)) {
			P &= ~DATA_P_bm;
			P |= DATA_P_bm & 23;
		}
	}
	else if (P & P_OR_m_bm) {				
		#if (configUSE_PLC == 0)
			if (!(P & YESNO_P_bm))
				P|= YESNO_P_bm;
			return;
		#endif
		P &= ~P_OR_m_bm; // установить m
	}	
}
/*
чтение EEPROM
*/
void CPrg::read_eeprom(int addres,int sz,void * data)
{
	bool error = false;
	unsigned char *p = (unsigned char*)data;
	for(int i = 0 ; i < sz; i ++){
		p[i] = flash_prg.read_one_byte_eeprom_address_int(addres + i,&error);
	}
}
/*
запись в EEPROM , запись блоками по 32(EEPROM_PAGESIZE) байта
*/
void CPrg::write_eeprom(int addres, int sz, void * data)
{	
	//flash_prg.write_block_to_eeprom_address_int(addres,(unsigned char*)data,tmp_sz);//запись не больше 32 байт
	int tmp_sz = 0;
	int written_sz = sz;
	unsigned char* p = (unsigned char*)data;
	for (int add = 0; add < sz; add += EEPROM_PAGESIZE ) {
		if (written_sz > EEPROM_PAGESIZE) 
			tmp_sz = EEPROM_PAGESIZE;
		else
			tmp_sz = written_sz;
		flash_prg.write_block_to_eeprom_address_int(addres + add,p,tmp_sz);//запись не больше 32 байт
		written_sz -= tmp_sz;
		p = p + tmp_sz;
	}
}

void CPrg::init_eeprom_var()
{
#if (configUSE_Shadow_axis == 1)
	if(Lir.P2[SW_AXIS0] > CInit::NO_AXIS)
		Lir.P2[SW_AXIS0] = CInit::NO_AXIS;
#endif

#if (configUSE_NCshell == 1)
	init_spd_bit_fields();	
#endif
	init_offsets();
}
/*
обнуление битовых полей скорости если они все равны 0xFF
*/
void CPrg::init_spd_bit_fields()
{
	#if (configUSE_NCshell == 1)
	//==проверка что в памяти лежат все 0xFF
	for(int c = 0 ; c < N_AXIS + N_SA; c++){
		char *p = &_Allp_param[c].start_G1;
		for(int pc = 0; pc < 17;pc++){  // 17 параметров со start_G1 по start_LUFT
			if(p[pc] != 0xFF)
				return;
		}
	}
	//==
	//==обнуляем переменные
	for(int c = 0 ; c < N_AXIS + N_SA; c++){
		char *p = &_Allp_param[c].start_G1;
		for(int pc = 0; pc < 17;pc++){ // 17 параметров со start_G1 по start_LUFT
			p[pc] = 0;
		}
	}
	//==

	//==записываем в память
	flash_prg.write_eeprom(188,sizeof(flash_prg._Allp_param[0]) * N_AXIS,&flash_prg._Allp_param);
	#if (configUSE_Shadow_axis == 1)
	flash_prg.write_eeprom(1116,sizeof(flash_prg._Allp_param[SW_AXIS0]),&flash_prg._Allp_param[SW_AXIS0]);
	#endif
	//==
	#endif
}
/*
обнуление смещений G53 - G58
*/
void CPrg::init_offsets()
{
	//==проверка что в памяти лежат все смещения -1

	//==
}

//----------
void  CPrg::init_to_default_state_axis_cfg(int num){
	axis_cfg[num].type=TYPE::LINEAR;
	axis_cfg[num].format= FORMAT::LINEAR_3;
	axis_cfg[num].r_d=R_D::RADIUS;
	axis_cfg[num].direction=DIRECTION::PLUS;
	axis_cfg[num].save=SAVE::NO_SAVE;
	axis_cfg[num].encoder_fault= ENCODER_FAULT::NO_FAULT;
	axis_cfg[num].readout_speed = READOUT_SPEED::NO_SPEED;
	axis_cfg[num].extern_to_zero = YESNO_P_bm  | INVERSE_P_bm;
	axis_cfg[num].ref_pulse_zone = YESNO_P_bm  | INVERSE_P_bm;;
	axis_cfg[num].gap=0;
	axis_cfg[num].interval_to_measure_speed=RANGE::INTERVAL0_6;
}
void  CPrg::to_copy_axis_config(int destination,int sourse){
	axis_cfg[destination]=axis_cfg[sourse];
}
//29.10.2018
/*
функция устанавливает максимум для всех счетчиков если ось круговая в зависимости от осевых коэффициентов
*/
void  CPrg::to_set_var_for_angle_axis(void){
	cli();
	long max_value;
	for(int i = 0 ; i < N_AXIS; i++){
		if(axis_cfg[i].type == TYPE::CIRCLE){
			max_value = Lir.get_max_circle(i);
			coeff.div(i, &max_value);			
			mCounter[i].max_value = (MAX_LONG_PLUS / max_value) * max_value;
			#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
			mCounter[i].max_value = max_value;
			#endif
			mCounter[i].increment_0_360_one_turn = true;
		}
		else
			mCounter[i].increment_0_360_one_turn = false;
	}
	sei();
}
/*
функция устанавливает максимальное значение для конкретного счетчика круговой оси
in uint8_t ch - канал для установки
long bin - коэффициент (целое число)
char n_a_d -  коэффициент (число знаков после точки)
*/
void CPrg::to_set_max_for_angel_axis(uint8_t ch,long bin,char n_a_d)
{
	cli();
	long max_value;	
	if(axis_cfg[ch].type==TYPE::CIRCLE){
		max_value = Lir.get_max_circle(ch);
		coeff.div(255, &max_value,n_a_d,bin);
		
		mCounter[ch].max_value = (MAX_LONG_PLUS / max_value ) * max_value;
		#if (configUSE_menuItem_Correction_and_Izmerenia == 1) //для коррекции и индикации
		mCounter[ch].max_value = max_value;
		#endif
		mCounter[ch].increment_0_360_one_turn = true;
		
	}
	else
		mCounter[ch].increment_0_360_one_turn = false;
	sei();
}

void CPrg::load_coeff_to_page_buf(unsigned char *buf,int ch)
{
  long coeff_ = 1;
  char n_after_dot_ = 0;

	coeff_ = coeff.get_coeff_bin(ch);
	n_after_dot_ = coeff.get_coeff_n_after_dot(ch);
			
	unsigned char *byte_pointer = (unsigned char *)&coeff_;

	int n=0;
	for(;n<4;n++)
		buf[n] = byte_pointer[n];

	buf[n] = n_after_dot_;
}

bool CPrg::to_copy_axis_param_from_EEPROM_to_RAM(void){
	bool need_to_write = false;

	flash_prg.read_eeprom(8,sizeof(flash_prg.axis_cfg[0]) * N_AXIS,&flash_prg.axis_cfg);
		
        for(int n = 0;n < N_AXIS;n++){
			if((axis_cfg[n].type == TYPE::LINEAR || axis_cfg[n].type == TYPE::CIRCLE));
			else{ 
				axis_cfg[n].type = TYPE::LINEAR; 
				need_to_write = true;
				}

			if(axis_cfg[n].format >= FORMAT::FORMAT_END){
				axis_cfg[n].type = TYPE::LINEAR;
				axis_cfg[n].format = FORMAT::LINEAR_3;
				need_to_write = true;	
			}
			if(axis_cfg[n].type == TYPE::LINEAR){
				switch (axis_cfg[n].format)
				{
					case FORMAT::LINEAR_1:		
					case FORMAT::LINEAR_2:
					case FORMAT::LINEAR_3:
					case FORMAT::LINEAR_4:
					case FORMAT::LINEAR_6:
					case FORMAT::LINEAR_INCH_4:
					break;
					default:
					axis_cfg[n].format = FORMAT::LINEAR_3;
					need_to_write = true;
				}	
			}
			else {
				switch (axis_cfg[n].format)
				{
					case FORMAT::CIRCLE_1:
					case FORMAT::CIRCLE_2:
					case FORMAT::CIRCLE_3:
					case FORMAT::CIRCLE_4:
					case FORMAT::CIRCLE_SECUNDO_1:
					case FORMAT::CIRCLE_SECUNDO:
					case FORMAT::CIRCLE_MINUTO:
					break;
					default:
					axis_cfg[n].format = FORMAT::CIRCLE_3;
					need_to_write = true;
				}			
			}

			if(axis_cfg[n].is_correction == CPrg::YES && configUSE_menuItem_Correction_and_Izmerenia == 0 ){
				axis_cfg[n].is_correction = CPrg::NO;
				altera.error_corr_real_time[n] = CIOAltera::ERROR_CORR_REAL_TIME::CORR_OFF;
				need_to_write=true;			
			}
			if(configUSE_NO_input == 1){
				if(!(axis_cfg[n].ref_pulse_zone & YESNO_P_bm) || !(axis_cfg[n].extern_to_zero & YESNO_P_bm)){
					axis_cfg[n].ref_pulse_zone = 0xFF;
					axis_cfg[n].extern_to_zero = 0xFF;
					need_to_write = true;
					}
				}
		}//for

        if(need_to_write == true){
			to_save_axis_param_to_EEPROM(0,(unsigned char*)&axis_cfg[0]);	
			to_save_axis_param_to_EEPROM(1,(unsigned char*)&axis_cfg[1]);
			to_save_axis_param_to_EEPROM(2,(unsigned char*)&axis_cfg[2]);
			to_save_axis_param_to_EEPROM(3,(unsigned char*)&axis_cfg[3]);	
		}
	return true;
}
bool CPrg::read_channel_0_from_EEPROM(unsigned char *tmp){
	bool error;
	tmp[0] = read_one_byte_eeprom_address_int(8,&error);
	tmp[1] = read_one_byte_eeprom_address_int(9,&error);
	tmp[2] = read_one_byte_eeprom_address_int(10,&error);
	tmp[3] = read_one_byte_eeprom_address_int(11,&error);
//05.03.2018
	tmp[4] = read_one_byte_eeprom_address_int(12,&error);
	tmp[5] = read_one_byte_eeprom_address_int(13,&error);
	tmp[6] = read_one_byte_eeprom_address_int(14,&error);
	tmp[7] = read_one_byte_eeprom_address_int(15,&error);
	tmp[8] = read_one_byte_eeprom_address_int(16,&error);
	tmp[9] = read_one_byte_eeprom_address_int(17,&error);
	tmp[10] = read_one_byte_eeprom_address_int(18,&error);
	tmp[11] = read_one_byte_eeprom_address_int(19,&error);
	
	return error;
}
bool CPrg::read_channel_1_from_EEPROM(unsigned char *tmp){
	bool error;
	tmp[0] = read_one_byte_eeprom_address_int(20,&error);
	tmp[1] = read_one_byte_eeprom_address_int(21,&error);
	tmp[2] = read_one_byte_eeprom_address_int(22,&error);
	tmp[3] = read_one_byte_eeprom_address_int(23,&error);
//05.03.2018
tmp[4] = read_one_byte_eeprom_address_int(24,&error);
tmp[5] = read_one_byte_eeprom_address_int(25,&error);
tmp[6] = read_one_byte_eeprom_address_int(26,&error);
tmp[7] = read_one_byte_eeprom_address_int(27,&error);
tmp[8] = read_one_byte_eeprom_address_int(28,&error);
tmp[9] = read_one_byte_eeprom_address_int(29,&error);
tmp[10] = read_one_byte_eeprom_address_int(30,&error);
tmp[11] = read_one_byte_eeprom_address_int(31,&error);
	
	
	
	return error;
}
bool CPrg::read_channel_2_from_EEPROM(unsigned char *tmp){
	bool error;
	tmp[0] = read_one_byte_eeprom_address_int(32,&error);
	tmp[1] = read_one_byte_eeprom_address_int(33,&error);
	tmp[2] = read_one_byte_eeprom_address_int(34,&error);
	tmp[3] = read_one_byte_eeprom_address_int(35,&error);
//05.03.2018
tmp[4] = read_one_byte_eeprom_address_int(36,&error);
tmp[5] = read_one_byte_eeprom_address_int(37,&error);
tmp[6] = read_one_byte_eeprom_address_int(38,&error);
tmp[7] = read_one_byte_eeprom_address_int(39,&error);
tmp[8] = read_one_byte_eeprom_address_int(40,&error);
tmp[9] = read_one_byte_eeprom_address_int(41,&error);
tmp[10] = read_one_byte_eeprom_address_int(42,&error);
tmp[11] = read_one_byte_eeprom_address_int(43,&error);	
	return error;
}
bool CPrg::read_channel_3_from_EEPROM(unsigned char *tmp){
	bool error;
	tmp[0] = read_one_byte_eeprom_address_int(44,&error);
	tmp[1] = read_one_byte_eeprom_address_int(45,&error);
	tmp[2] = read_one_byte_eeprom_address_int(46,&error);
	tmp[3] = read_one_byte_eeprom_address_int(47,&error);
//05.03.2018
tmp[4] = read_one_byte_eeprom_address_int(48,&error);
tmp[5] = read_one_byte_eeprom_address_int(49,&error);
tmp[6] = read_one_byte_eeprom_address_int(50,&error);
tmp[7] = read_one_byte_eeprom_address_int(51,&error);
tmp[8] = read_one_byte_eeprom_address_int(52,&error);
tmp[9] = read_one_byte_eeprom_address_int(53,&error);
tmp[10] = read_one_byte_eeprom_address_int(54,&error);
tmp[11] = read_one_byte_eeprom_address_int(55,&error);	
	return error;
}



//01.03.2018
bool CPrg::to_copy_coeff_from_EEPROM_to_RAM(void){
		bool channel[N_AXIS];
		//read_coeff_from_EEPROM((unsigned char*)&coeff.new_coeff_[0],(unsigned char*)&coeff.new_coeff_[1],
		 //                     (unsigned char*)&coeff.new_coeff_[2],(unsigned char*)&coeff.new_coeff_[3]);
							  
		int start_EEPROM = 56;
		bool error = false;
		for (int ch = 0; ch < N_AXIS;ch++) {
			unsigned char* p = reinterpret_cast<unsigned char*>(&coeff.new_coeff_[ch]);
			for (int i = 0; i < 5; i++) {
				p[i] = flash_prg.read_one_byte_eeprom_address_int(start_EEPROM + i + (ch * 5), &error);
			}
		}					  
							  
//01.03.2017 it must be checked
          for (int num=0;num<N_AXIS;num++)							   
			  channel[num]=check_coeff(num);
//------------------------------			  
		  for (int num=0;num<N_AXIS;num++){
		     if(channel[num]==true){
			   switch (num){
				   case 0:
				   to_save_coeff_to_EEPROM(0,(unsigned char*)&coeff.new_coeff_[0]);break;
				   case 1:
				   to_save_coeff_to_EEPROM(1,(unsigned char*)&coeff.new_coeff_[1]);break;
				   case 2:
				   to_save_coeff_to_EEPROM(2,(unsigned char*)&coeff.new_coeff_[2]);break;
				   case 3:
				   to_save_coeff_to_EEPROM(3,(unsigned char*)&coeff.new_coeff_[3]);break;   
					}
			      }
               }					   
return true;	
}
bool CPrg::check_coeff(int num){
	long data=1;
	unsigned char dot=0;
	bool datas_are_changed=false;
	 switch (num){
		 case 0: 
			data=coeff.new_coeff_[0].binary;dot=coeff.new_coeff_[0].n_after_dot;
			break;
		 case 1: 
			data=coeff.new_coeff_[1].binary;dot=coeff.new_coeff_[1].n_after_dot;
			break;
		 case 2: 
			data=coeff.new_coeff_[2].binary;dot=coeff.new_coeff_[2].n_after_dot;
			break;
		 case 3: 
			data=coeff.new_coeff_[3].binary;dot=coeff.new_coeff_[3].n_after_dot;
			break;
	 }
	    if(data<=0)
			{data=1;dot=0;datas_are_changed=true;}
			
	      /*  switch (dot){
			case 0:
				if(data>=100){ 
				data=1;// data=99;  old
				datas_are_changed=true;
				}
				break;
			case 1:if(data>=1000){ data=999;datas_are_changed=true;}
				break;   
			case 2:if(data>=10000){ data=9999;datas_are_changed=true;}
				   else if (data<5){
				    data=5;
					datas_are_changed=true;
					}
				break;
			case 3:if(data>=100000){ data=99999;datas_are_changed=true;}
				   else if(data<51){ 
				   data=51;
				   datas_are_changed=true;
				   }
			    break;		
			case 4:if(data>=1000000){ data=999999;datas_are_changed=true;}
			       else if(data<500){ data=501;datas_are_changed=true;}
			    break;
			case 5:if(data>=10000000){ data=9999999;datas_are_changed=true;}
			       else if(data<5000){ data=5001;datas_are_changed=true;}
			    break;
			case 6:if(data>=100000000){ data=99999999;datas_are_changed=true;}
			       else if(data<50000){ data=50001;datas_are_changed=true;}
			    break;	
			case 7:if(data>=1000000000){ data=999999999;datas_are_changed=true;}
			       else if(data<500000){ data=500001;datas_are_changed=true;}
			    break;	
			case 8:if(data>=1000000000){ data=999999999;datas_are_changed=true;}
			else if(data<5000000){ data=5000001;datas_are_changed=true;}
			break;	
					
	        default:{ data=1;dot=0;datas_are_changed=true;break;} 			
			} */
	 
	  switch (num){
		  case 0: coeff.new_coeff_[0].binary=data;coeff.new_coeff_[0].n_after_dot=dot;break;
		  case 1: coeff.new_coeff_[1].binary=data;coeff.new_coeff_[1].n_after_dot=dot;break;
		  case 2: coeff.new_coeff_[2].binary=data;coeff.new_coeff_[2].n_after_dot=dot;break;
		  case 3: coeff.new_coeff_[3].binary=data;coeff.new_coeff_[3].n_after_dot=dot;break;
	  }
	return datas_are_changed;
}

bool CPrg::read_coeff_from_EEPROM(unsigned char *tmp_0,unsigned char *tmp_1,
                                  unsigned char *tmp_2,unsigned char *tmp_3){
		bool error;
		tmp_0[0] = read_one_byte_eeprom_address_int(56,&error);
		tmp_0[1] = read_one_byte_eeprom_address_int(57,&error);
		tmp_0[2] = read_one_byte_eeprom_address_int(58,&error);
		tmp_0[3] = read_one_byte_eeprom_address_int(59,&error);
		tmp_0[4] = read_one_byte_eeprom_address_int(60,&error);
//-------------------------------------------------------------		
			tmp_1[0] = read_one_byte_eeprom_address_int(61,&error);
			tmp_1[1] = read_one_byte_eeprom_address_int(62,&error);
			tmp_1[2] = read_one_byte_eeprom_address_int(63,&error);
			tmp_1[3] = read_one_byte_eeprom_address_int(64,&error);
			tmp_1[4] = read_one_byte_eeprom_address_int(65,&error);
//-------------------------------------------------------------	
			tmp_2[0] = read_one_byte_eeprom_address_int(66,&error);
			tmp_2[1] = read_one_byte_eeprom_address_int(67,&error);
			tmp_2[2] = read_one_byte_eeprom_address_int(68,&error);
			tmp_2[3] = read_one_byte_eeprom_address_int(69,&error);
			tmp_2[4] = read_one_byte_eeprom_address_int(70,&error);
//-------------------------------------------------------------
            tmp_3[0] = read_one_byte_eeprom_address_int(71,&error);
            tmp_3[1] = read_one_byte_eeprom_address_int(72,&error);
            tmp_3[2] = read_one_byte_eeprom_address_int(73,&error);
            tmp_3[3] = read_one_byte_eeprom_address_int(74,&error);
            tmp_3[4] = read_one_byte_eeprom_address_int(75,&error);			
		return error;								  
								  
		}
//25.09.2018
void CPrg::to_check_G53_and_G5x_when_system_is_started(void){
bool datas_are_changed_g53,datas_are_changed_g5x;
long data; 
datas_are_changed_g53=datas_are_changed_g5x=false;
for(int n=0;n<N_AXIS;n++){
	data=display.offset_g53[n];
	if(axis_cfg[n].type==TYPE::CIRCLE){
	if (data<0){data=-data;
	datas_are_changed_g53=true;	
	display.offset_g53[n]=data;
	           }
	}
	if(to_check_offsets(n,&data)==true){datas_are_changed_g53=true;
	display.offset_g53[n]=data;
	}
}
for(int n=0;n<N_AXIS;n++){
	for(int s=0;s<5;s++){
	data=display.offset_g5x[s][n];
	if(axis_cfg[n].type==TYPE::CIRCLE){
		if (data<0){data=-data;
		datas_are_changed_g5x=true;
		display.offset_g5x[s][n]=data;
		}
	}
	if(to_check_offsets(n,&data)==true){datas_are_changed_g5x=true;
	display.offset_g5x[s][n]=data;
	}
	}
}
if(datas_are_changed_g53==true || datas_are_changed_g5x==true)
    to_copy_G53_and_G5x_from_RAM_to_EEPROM();
		
}

bool CPrg::to_copy_speed_on_screen_param_to_RAM()
{
bool error;
unsigned char tmp[N_AXIS];

int start_EEPROM=172;

	for(int n=0;n<N_AXIS;n++) {
		tmp[n] = flash_prg.read_one_byte_eeprom_address_int(start_EEPROM+n,&error);
		if(tmp[n] == 255)
			tmp[n] = CDisp::COORDINATE;
		}

	for (int n=0;n<N_AXIS;n++){
			display.position_to_see[n] = static_cast<CDisp::_STR_MODE>(tmp[n]);
	}

	return error;
}

bool CPrg::to_copy_contrast_to_RAM()
{
	bool error;
	int start_EEPROM=176;
	unsigned char tmp;

	tmp = flash_prg.read_one_byte_eeprom_address_int(start_EEPROM,&error);

	if((tmp>=MIN_contr) && (tmp<=MAX_contr))
	{
		init.contrast_value = tmp;
	}
	else
	{
		init.contrast_value = 25;
	}
	return error;
}

bool CPrg::to_copy_to_see_time_toRAM()
{
	bool error;
	int start_EEPROM=177;
	unsigned char tmp;

	tmp = flash_prg.read_one_byte_eeprom_address_int(start_EEPROM,&error);

	if(tmp == 0 || tmp == 1)
	{
		init.to_see_time = tmp;
	}
	else 
	{
		init.to_see_time = 0;
	}

	return error;
}

bool CPrg::to_copy_Bdrate_toRAM()
{
	bool error;
	int start_EEPROM=178;
	unsigned char tmp;

	tmp = flash_prg.read_one_byte_eeprom_address_int(start_EEPROM,&error);

	if(tmp == usb115200 || tmp == usb200000 || tmp == usb9600)
	{
		uart_usb.baudrate = tmp;
	}
	else
	{
		uart_usb.baudrate = usb115200;
	}

	return error;
}
//не используется
bool CPrg::to_copy_Rejim_zaxvata_ref_metki_toRAM()
{
	bool error;
	int start_EEPROM=179;
	unsigned char tmp;

	tmp = flash_prg.read_one_byte_eeprom_address_int(start_EEPROM,&error);

	if(tmp == 0 || tmp == 1)
	{
		MsgKey.submode_mode_REF = tmp;
	}
	else
	{
		MsgKey.submode_mode_REF = 0;
	}

	MsgKey.submode_mode_REF = 1;

	return error;
}

bool CPrg::to_copy_night_mode_toRAM()
{
	bool error;
	int start_EEPROM=180;
	unsigned char tmp;

	tmp = flash_prg.read_one_byte_eeprom_address_int(start_EEPROM,&error);
	
	display.invert_display = tmp;

	if(display.invert_display&1)
		display.invert_in();
	else
		display.invert_out();

	return error;
}
/*
функция для копирования назначенных осей в режимах Отверстия на окружности и Отверстия на прямой
считывает 181 182 183 184 адрес EEPROM
*/
bool CPrg::to_copy_axis_for_CH_LH()
{
	bool error = 0;
#if (configUSE_Linear_holes == 1)
	int start_EEPROM=181;
	unsigned char tmp_primary_axis_lh;
	unsigned char tmp_secondary_axis_lh;

	unsigned char tmp_primary_axis_ch;
	unsigned char tmp_secondary_axis_ch;

	tmp_primary_axis_lh = read_one_byte_eeprom_address_int(start_EEPROM,&error);
	tmp_secondary_axis_lh = read_one_byte_eeprom_address_int(start_EEPROM + 1,&error);

	tmp_primary_axis_ch = read_one_byte_eeprom_address_int(start_EEPROM + 2,&error);
	tmp_secondary_axis_ch = read_one_byte_eeprom_address_int(start_EEPROM + 3,&error);

	if(tmp_primary_axis_lh <= no_axis)
		Lir.primary_axis_lh = tmp_primary_axis_lh;
	else
		Lir.primary_axis_lh = no_axis;

	if(tmp_secondary_axis_lh <= no_axis)
		Lir.secondary_axis_lh = tmp_secondary_axis_lh;
	else
		Lir.secondary_axis_lh = no_axis;

	if(tmp_primary_axis_ch <= no_axis)
		Lir.primary_axis_ch = tmp_primary_axis_ch;
	else
		Lir.primary_axis_ch = no_axis;


	if(tmp_secondary_axis_ch <= no_axis)
		Lir.secondary_axis_ch = tmp_secondary_axis_ch;
	else
		Lir.secondary_axis_ch = no_axis;
#endif
		return error;
}
/*
функция копирования параметра для настройки 'Звук' и других параметров
считывает 185 адрес EEPROM
*/
bool CPrg::to_copy_sound()
{
	bool error;
	int start_EEPROM=185;
	unsigned char tmp;

	tmp = flash_prg.read_one_byte_eeprom_address_int(start_EEPROM,&error);
	
	Lir.sound_and_other = tmp;

	return error;
}
/*
функция копирования параметров для настройки управление осями
только для версии с mcx514
считывает c 186 по 218
*/
#if (configUSE_mcx514 == 1)
bool CPrg::to_copy_Cntrl_cfg_from_eeprom_mcx514()
{
	bool error;
	int start_EEPROM=186;
	int k = 0;
	char tmp[4];
	long *plong;
	for(int i = 0; i < N_AXIS;i++){
		for( int j = 0; j < 4;j++,k++)
		tmp[j] = read_one_byte_eeprom_address_int(start_EEPROM + k,&error);
		
		plong = (long*)&tmp;
		flash_prg.axis_cfg_nc[i].move_oborot = *plong;
		

		for( int j = 0; j < 4;j++,k++)
		tmp[j] = read_one_byte_eeprom_address_int(start_EEPROM + k,&error);

		plong = (long*)&tmp;
		flash_prg.axis_cfg_nc[i].imp_oborot = *plong;

		if(flash_prg.axis_cfg_nc[i].imp_oborot < 1)
		flash_prg.axis_cfg_nc[i].imp_oborot = 200;
		if(flash_prg.axis_cfg_nc[i].move_oborot < 1)
		flash_prg.axis_cfg_nc[i].move_oborot = 10000;
	}
	return error;
}
#endif
//25.09.2018 
bool CPrg::to_check_offsets(int channel,long *data){
	
switch (axis_cfg[channel].format)
{
	case FORMAT::LINEAR_1:
	case FORMAT::LINEAR_2:
	case FORMAT::LINEAR_3:
	case FORMAT::LINEAR_4:
	case FORMAT::LINEAR_INCH_4:
	if(*data>=100000000 || *data<=-100000000){*data=0;return true;}
	return false;	
	case  FORMAT::CIRCLE_3:
	//1 is 0.001 degrees, 1000 is 1.000 degrees
	if (*data<360000)return false;
	*data%=(long)360000;
	return true;
	case  FORMAT::CIRCLE_2:
	//1 is 0.01 degrees, 100 is 1.00 degrees
	if(*data<36000)return false;
	*data%=(long)36000;
	return true;
	case  FORMAT::CIRCLE_1:
	//1 is 0.1 degrees, 100 is 10.0 degrees
	if(*data<3600)return false;
	*data%=(long)3600;
	return true;
	case  FORMAT::CIRCLE_4:
	//1 is 0.0001 degrees, 10000 is 1.0000 degrees
	if(*data<3600000)return false;
	*data%=(long)3600000;
	return true;
	case  FORMAT::CIRCLE_SECUNDO:
	//1 is secundo, 60 is 1 minuto, 3600  is 1  degrees
	if(*data<1296000)return false;
	*data%=(long)1296000;
	return true;
	case  FORMAT::CIRCLE_MINUTO:
	//1 is minuto, 60 is 1 degrees, 360 degrees is one circuit
	if(*data<21600)return false;
	*data%=(long)21600;
	return true;
	default:*data=0;return true;
}	
}	
//18.09.2018
bool CPrg::to_copy_G53_and_G5x_from_EEPROM_to_RAM(void){
	read_eeprom(76,sizeof(display.offset_g53),&display.offset_g53);
	read_eeprom(92,sizeof(display.offset_g5x),&display.offset_g5x);
	
return 0;		
}
bool CPrg::G53_and_G5x_to_clear(void){
//	static char t;
long tmp=0;
int m;	
	for (int n=0;n<4;n++){
	display.offset_g53[n]=tmp;	
	//tmp++;
	}
	  for(m=0;m<5;m++)	{
		  for(int i=0;i<4;i++)display.offset_g5x[m][i]=tmp; 
		  //for(int i=0;i<4;i++)display.offset_g5x[m][i]=tmp++;	 
	  }
//	  t++;
//	 if(t&1) return to_copy_G53_and_G5x_from_RAM_to_EEPROM();
//	 return false;
 return to_copy_G53_and_G5x_from_RAM_to_EEPROM();
     
}
	
bool CPrg::to_copy_G53_and_G5x_from_RAM_to_EEPROM(void){

	write_eeprom(76,sizeof(display.offset_g53),&display.offset_g53);
	write_eeprom(92,sizeof(display.offset_g5x),&display.offset_g5x);
return true;		 
}
//-------------------------------------------------------
bool CPrg::to_copy_luft_from_EEPROM_to_RAM(void){
		bool channel[N_AXIS];
		bool error;
// 		error=read_luft_from_EEPROM((unsigned char*)&coeff.new_gap[0],(unsigned char*)&coeff.new_gap[1],
// 		(unsigned char*)&coeff.new_gap[2],(unsigned char*)&coeff.new_gap[3]);
// 		coeff.new_gap[0].binary=axis_cfg[0].gap;
// 		coeff.new_gap[1].binary=axis_cfg[1].gap;
// 		coeff.new_gap[2].binary=axis_cfg[2].gap;
// 		coeff.new_gap[3].binary=axis_cfg[3].gap;
//28.03.2018 it must be checked
		for (int num=0;num<N_AXIS;num++)
		channel[num]=check_luft(num,GAP::SAVE_GAP_TO_EEPROM);
//-----------------------------	
            //   for (int num=0;num<N_AXIS;num++)
           //    to_save_luft_to_RAM(num,coeff.new_gap[num].binary);
		if(channel[0]==true||channel[1]==true
		   ||channel[2]==true||channel[3]==true)error=true;
		   else error=false;
return error;		
}
bool CPrg::read_luft_from_EEPROM(unsigned char *tmp_0,unsigned char *tmp_1,
                                 unsigned char *tmp_2,unsigned char *tmp_3){
bool error;
		tmp_0[0] = flash_prg.read_one_byte_eeprom_address_int(76,&error);
		tmp_0[1] = flash_prg.read_one_byte_eeprom_address_int(77,&error);
		tmp_0[2] = flash_prg.read_one_byte_eeprom_address_int(78,&error);
		tmp_0[3] = flash_prg.read_one_byte_eeprom_address_int(79,&error);
//-------------------------------------------------------------
		tmp_1[0] = flash_prg.read_one_byte_eeprom_address_int(80,&error);
		tmp_1[1] = flash_prg.read_one_byte_eeprom_address_int(81,&error);
		tmp_1[2] = flash_prg.read_one_byte_eeprom_address_int(82,&error);
		tmp_1[3] = flash_prg.read_one_byte_eeprom_address_int(83,&error);
//-------------------------------------------------------------
		tmp_2[0] = flash_prg.read_one_byte_eeprom_address_int(84,&error);
		tmp_2[1] = flash_prg.read_one_byte_eeprom_address_int(85,&error);
		tmp_2[2] = flash_prg.read_one_byte_eeprom_address_int(86,&error);
		tmp_2[3] = flash_prg.read_one_byte_eeprom_address_int(87,&error);
//-------------------------------------------------------------
		tmp_3[0] = flash_prg.read_one_byte_eeprom_address_int(88,&error);
		tmp_3[1] = flash_prg.read_one_byte_eeprom_address_int(89,&error);
		tmp_3[2] = flash_prg.read_one_byte_eeprom_address_int(90,&error);
		tmp_3[3] = flash_prg.read_one_byte_eeprom_address_int(91,&error);
//-------------------------------------------------------------
return error;
 }
bool CPrg::check_luft(int channel,GAP save){
	long data=0;
	bool datas_are_changed=false;
	bool datas_are_negative=false;
	bool error=false;
	CDouble::MULT_OR_DIV_ANSWER answer;
	switch (channel){
		case 0: data = axis_cfg[0].gap;break;
		case 1: data = axis_cfg[1].gap;break;
		case 2: data = axis_cfg[2].gap;break;
		case 3: data = axis_cfg[3].gap;break;
	}
	
	if(data<0){data=0;datas_are_negative=true;}
		
	answer=coeff.mult(channel,&data);
	if(answer==CDouble::MULT_OR_DIV_ANSWER::OK)
		datas_are_changed = luft_limit_after_input(channel,&data);
	else {
		data=0;
		datas_are_changed = true;
		}
	 
	answer=coeff.div(channel,&data);
	if(answer!=CDouble::MULT_OR_DIV_ANSWER::OK){  
		data=0;
		datas_are_changed=true;
	} 
	 		   
	if((datas_are_changed==true && save==GAP::SAVE_GAP_TO_EEPROM)||(datas_are_negative==true && save==GAP::SAVE_GAP_TO_EEPROM)){
		switch(channel){
			case 0:axis_cfg[0].gap = data;
				error=to_save_axis_param_to_EEPROM(0,(unsigned char*)&axis_cfg[0]);break;
			case 1:axis_cfg[1].gap = data;
				error=to_save_axis_param_to_EEPROM(1,(unsigned char*)&axis_cfg[1]);break;
			case 2:axis_cfg[2].gap = data;
				error=to_save_axis_param_to_EEPROM(2,(unsigned char*)&axis_cfg[2]);break;
			case 3:axis_cfg[3].gap = data;
				error=to_save_axis_param_to_EEPROM(3,(unsigned char*)&axis_cfg[3]);break;
		}
	}		
			
	switch (channel){
		case 0: coeff.new_gap[0].binary=axis_cfg[0].gap=data;
				to_save_luft_to_RAM(0,coeff.new_gap[0].binary);
				break;
		case 1: coeff.new_gap[1].binary=axis_cfg[1].gap=data;
				to_save_luft_to_RAM(1,coeff.new_gap[1].binary);
				break;
		case 2: coeff.new_gap[2].binary=axis_cfg[2].gap=data;
				to_save_luft_to_RAM(2,coeff.new_gap[2].binary);
				break;
		case 3: coeff.new_gap[3].binary=axis_cfg[3].gap=data;
				to_save_luft_to_RAM(3,coeff.new_gap[3].binary);
				break;
		}		
return error;	
}
//02.04.2018
bool  CPrg::luft_limit_after_input(int channel,long *data){
	bool datas_are_changed=false;
	switch (axis_cfg[channel].format)
	{
		case FORMAT::LINEAR_1:
		//1 is 0.1 mm, 10 is 1.0 mm,100 is 10.0 mm, 1000 is 100.0 mm
		if(*data>1000){*data=1000;datas_are_changed=true;}
		break;
		case FORMAT::LINEAR_2:
		//1 is 0.01 mm, 10 is 0.10 mm, 100 is 1.00 mm,1000 is 10.00 mm, 10000 is 100.00 mm
		if(*data>10000){*data=1000;datas_are_changed=true;}
		break;
		case FORMAT::LINEAR_3:
		//1000 is 1.000 mm 10000 is 10.0 mm
		if(*data>10000){*data=10000;datas_are_changed=true;}
		break;
		case FORMAT::LINEAR_4:
		//1 is 0.0001 mm, 10000 is 1.0000 mm,
		if(*data>10000){*data=10000;datas_are_changed=true;}
		break;
		case  FORMAT::CIRCLE_3:
		//1 is 0.001 degrees, 1000 is 1.000 degrees
		if(*data>1000){*data=1000;datas_are_changed=true;}
		break;
		case  FORMAT::CIRCLE_2:
		//1 is 0.01 degrees, 100 is 1.00 degrees
		if(*data>100){*data=100;datas_are_changed=true;}
		break;
		case  FORMAT::CIRCLE_1:
		//1 is 0.1 degrees, 100 is 10.0 degrees
		if(*data>100){*data=100;datas_are_changed=true;}
		break;
		case  FORMAT::CIRCLE_4:
		//1 is 0.0001 degrees, 10000 is 1.0000 degrees
		if(*data>10000){*data=10000;datas_are_changed=true;}
		break;
		case  FORMAT::CIRCLE_SECUNDO:
		//1 is secundo, 60 is 1 minuto, 3600  is 1  degrees
		if(*data>3600){*data=3600;datas_are_changed=true;}
		break;
		
	}		
	return datas_are_changed;
}
//-------------------------
bool  CPrg::to_save_axis_param_to_EEPROM(int channel,unsigned char *tmp ){
	  bool error;
	  int n;
	  switch(channel){
		case 0:n=8;break;
		case 1:n=20;break;
		case 2:n=32;break;
		case 3:n=44;break;
		default:return false;
	  } 
	   error=write_block_to_eeprom_address_int(n,tmp,12);
	return error;  	  
}

bool CPrg::to_save_speed_screen_param_to_EEPROM(uint8_t *param)
{
bool error;

	for(int x =0 ;x<N_AXIS;x++)
		error = write_block_to_eeprom_address_int(172+x,&param[x],1);

	return error;
}

bool CPrg::to_save_contrast_to_EEPROM(unsigned char *param)
{
	bool error;
	error = write_block_to_eeprom_address_int(176,param,1);
	return error;
}

bool CPrg::to_save_Bdrate_toEEPROM(unsigned char *param)
{
	bool error;
	error = write_block_to_eeprom_address_int(178,param,1);
	return error;
}

bool CPrg::to_save_Rejim_zaxvata_ref_metok_toEEPROM(bool *param)
{
	bool error;
	error = write_block_to_eeprom_address_int(179,(unsigned char*)param,1);
	return error;
}

bool CPrg::to_save_sound()
{
	bool error;
	error = write_block_to_eeprom_address_int(185,&Lir.sound_and_other,1);
	return error;
}

bool CPrg::to_save_night_mode_to_EEPROM(unsigned char *param)
{
	bool error;
	error = write_block_to_eeprom_address_int(180,(unsigned char*)param,1);
	return error;
}

bool CPrg::to_save_primary_axis_lh_to_EEPROM(unsigned char *param)
{
	bool error;
	error = write_block_to_eeprom_address_int(181,(unsigned char*)param,1);
	return error;
}

bool CPrg::to_save_secondary_axis_lh_to_EEPROM(unsigned char *param)
{
	bool error;
	error = write_block_to_eeprom_address_int(182,(unsigned char*)param,1);
	return error;
}

bool CPrg::to_save_primary_axis_ch_to_EEPROM(unsigned char *param)
{
	bool error;
	error = write_block_to_eeprom_address_int(183,(unsigned char*)param,1);
	return error;
}

bool CPrg::to_save_secondary_axis_ch_to_EEPROM(unsigned char *param)
{
	bool error;
	error = write_block_to_eeprom_address_int(184,(unsigned char*)param,1);
	return error;
}

bool CPrg::to_save_to_see_time_to_EEPROM(bool *param)
{
	bool error;


	error = write_block_to_eeprom_address_int(177,(unsigned char*)param,1);

	return error;
}

bool  CPrg::to_save_coeff_to_EEPROM(int channel,unsigned char *tmp )
{
	  bool error;
	  int n;
	  switch(channel){
		  case 0:n=56;break;
		  case 1:n=61;break;
		  case 2:n=66;break;
		  case 3:n=71;break;
		  default:return false;
	  }
	  error=write_block_to_eeprom_address_int(n,tmp,5);
	  return error;	
}
//28.03.2018
bool  CPrg::to_save_luft_to_EEPROM(int channel,unsigned char *tmp )
{
	bool error;
	int n;
	switch(channel){
		case 0:n=76;break;
		case 1:n=80;break;
		case 2:n=84;break;
		case 3:n=88;break;
		default:return false;
	}
	error=write_block_to_eeprom_address_int(n,tmp,4);
	return error;
}
void CPrg::to_save_luft_to_RAM(int channel,long gap){
	cli();
		  mLuft[channel].gap=gap;
	sei();	  
};

bool CPrg::axis_is_chosen(uint16_t *P20,uint16_t *P21,
		uint16_t *P22,uint16_t *P23){
	bool need_to_write=false;
	unsigned char tmp[4];
	bool error;
	 tmp[0] = flash_prg.read_one_byte_eeprom_address_int(0,&error);
	 tmp[1] = flash_prg.read_one_byte_eeprom_address_int(1,&error);
	 tmp[2] = flash_prg.read_one_byte_eeprom_address_int(2,&error);
	 tmp[3] = flash_prg.read_one_byte_eeprom_address_int(3,&error);
	 	*P20=(unsigned int)tmp[0];
	 	*P21=(unsigned int)tmp[1];
	 	*P22=(unsigned int)tmp[2];
	 	*P23=(unsigned int)tmp[3];
	 	if(*P20>13){need_to_write=true;*P20=0;tmp[0]=0;}
	 	if(*P21>13){need_to_write=true;*P21=1;tmp[1]=1;}
	 	if(*P22>13){need_to_write=true;*P22=13;tmp[2]=13;}
	 	if(*P23>13){need_to_write=true;*P23=13;tmp[3]=13;}
	if(need_to_write==true){
         error=write_block_to_eeprom_address_int(0,tmp,4);
	}
to_save_type_of_axis(P20,P21,P22,P23);
return error;		
}

bool CPrg::axis_is_chosen_to_screen(uint16_t *Str1,uint16_t *Str2,uint16_t *Str3,uint16_t *Str4)
{
    	bool need_to_write=false;
    	unsigned char tmp[4];
    	bool error;
    	tmp[0] = flash_prg.read_one_byte_eeprom_address_int(4,&error);
    	tmp[1] = flash_prg.read_one_byte_eeprom_address_int(5,&error);
    	tmp[2] = flash_prg.read_one_byte_eeprom_address_int(6,&error);
    	tmp[3] = flash_prg.read_one_byte_eeprom_address_int(7,&error);
    	*Str1=(unsigned int)tmp[0];
    	*Str2=(unsigned int)tmp[1];
    	*Str3=(unsigned int)tmp[2];
    	*Str4=(unsigned int)tmp[3];
    	if(*Str1 > no_axis){need_to_write=true;*Str1=0;tmp[0]=0;}
    	if(*Str2 > no_axis){need_to_write=true;*Str2=1;tmp[1]=1;}
    	if(*Str3 > no_axis){need_to_write=true;*Str3=no_axis;tmp[2]=no_axis;}
    	if(*Str4 > no_axis){need_to_write=true;*Str4=no_axis;tmp[3]=no_axis;}

#if (configUSE_mcx514 == 1)
		//*Str4 = no_axis;
#endif

    	if(need_to_write==true){
	    	error=write_block_to_eeprom_address_int(4,tmp,4);
    	}

    	return error;
}

unsigned char CPrg::read_one_byte_eeprom_address_page(unsigned char addr_page,unsigned char addr_byte){
	unsigned char a,b,result;
//25.01.2018 it is EEPROM like memory from 0x1000-0x1fff (4096) 	
	//NVM.CTRLB=0x08;
//it is EEPROM 128 pages every 32 bytes	
	NVM.CTRLB=0x00;
//-------------------------------------	
	NVM.CMD=0x06;
	//---------------
	a=addr_byte&0x1f;
	b=addr_page&0x07;
	b=(b<<5)&0xe0;
	a=a|b;
	NVM.ADDR0=a;
	b=addr_page&0x78;
	b=(b>>3)&0x0f;
	NVM.ADDR1=b;
	//--------------
	cli();//__disable_interrupt();
	CCP=CCP_IOREG_gc;
	NVM.CTRLA=0x01;
	sei();//__enable_interrupt();
	result=NVM.DATA0;
	return result;
}

unsigned char CPrg::read_one_byte_eeprom_address_int(unsigned int addr,bool *error){

	return flash_prg.read_one_byte_eeprom_address_int(addr,error);
	//25.01.2018 it takes 58 cycles 58*31.25=1812.5 ns
	unsigned char result;
	//25.01.2018 it is EEPROM like memory from 0x1000-0x1fff (4096)
	//NVM.CTRLB=0x08;
	//it is EEPROM 128 pages every 32 bytes
	NVM.CTRLB=0x00;
	//25.01.2018  
	NVM.CMD=0x06;
	//---------------
	if (addr >= 4096){*error = true; return 0xff;}
	NVM.ADDR0=(unsigned char)addr;
	addr=addr>>8;
	NVM.ADDR1=(unsigned char)addr;
	//--------------
	cli();//__disable_interrupt();
	CCP=CCP_IOREG_gc;
	NVM.CTRLA=0x01;
	sei();//__enable_interrupt();
	result=NVM.DATA0;
	*error=false;
	return result;
}

//25.01.2018
/*
максимум 32 байта за раз (страница)
*/
bool CPrg::write_block_to_eeprom_address_int(unsigned int addr_start,unsigned char *buf,unsigned char size){
unsigned int page,addr_end,page_end,addr_next_page;
unsigned char n,size_first,n_first;
unsigned char size_second=0;
bool two=false;
bool exceed_limit;
unsigned char tmp[32];
unsigned char pointer;
		if (addr_start>=4096){ return false;}
	    if (size==0){ return false;}
	    if (size>32){return false;}
//-------------------------------------
//it must be defined,one or two pages must be written 
//addr 0-4095, size 1-32
//in first where is first address to write?
  page=addr_start/32;
  addr_end=addr_start+size-1;
  page_end=addr_end/32;
  if (addr_end >= 4096){
	return false;  
  }
  else if(page!=page_end){
	  addr_next_page=addr_start;
	 for(n=0;n<size;) {
	   addr_next_page=addr_next_page+1;
	   page_end=addr_next_page/32;
	   n++;
	   if(page!=page_end){exceed_limit=true;break;}  
	 }
//	addr_start,size_first,n_first
      size_first=n;
	   if(exceed_limit==true) n_first=32-size_first;
	   else {
		  return false;
	        }
      n_first=32-size_first;
// 	addr_next_page,size_second
      size_second=size-n;
	  two=true;
  }
  else if (page==page_end){
      addr_next_page=addr_start;
	  exceed_limit=false;
	  for(n=0;n<size;) {
		  addr_next_page=addr_next_page+1;
		  page_end=addr_next_page/32;
		  n++;
		  if(page!=page_end){exceed_limit=true;break;}
	  } 
//	addr_start,size_first,n_first	   
	 size_first=n;
	 if(exceed_limit==true) n_first=32-size_first;	
	 else {
		n_first=addr_start-(page*32); 
	 }
	 
  }
//first page must be read from eeprom range page is from 0-127
 
  for (n=0;n<32;n++) 
    tmp[n]=read_one_byte_eeprom_address_page((unsigned char)page,n);
 	
   for (pointer=0;pointer<size_first;)
         tmp[n_first++]=buf[pointer++];
//to flash buffer
NVM.CMD=0x36;
cli();//__disable_interrupt();
CCP=CCP_IOREG_gc;
NVM.CTRLA=0x01;
sei();//__enable_interrupt();
for(n=0;n<100;n++) {
	//200 mks
	delay_mks(200);//__delay_cycles(6350);
	n_first=NVM.STATUS&0x80;
	if(n_first==0x00) break;
}
if(n>=100)return false;
 //to load buffer
 NVM.CMD=0x33;
 for(n=0;n<32;n++) {
	 NVM.ADDR0=n;
	 NVM.DATA0=tmp[n];
 }

//erase and write
NVM.CMD=0x35; 
//---------------
NVM.ADDR0=(unsigned char)addr_start;
addr_start=addr_start>>8;
NVM.ADDR1=(unsigned char)addr_start;

//---------------
//tmp_=(unsigned char)page;
//b=tmp_&0x07;
//b=(b<<5)&0xe0;
//NVM.ADDR0=b;
//a=tmp_&0x78;
//a=(a>>3)&0x0f;
//NVM.ADDR1=a;
//--------------
cli();//__disable_interrupt();
CCP=CCP_IOREG_gc;
NVM.CTRLA=0x01;
sei();//__enable_interrupt();
for(n=0;n<100;n++) {
	//200 mks
	delay_mks(200);//__delay_cycles(6350);
	n_first=NVM.STATUS&0x80;
	if(n_first==0x00) break;
}
if(n>=100)return false;
if(two==false)	return true;
//26.01.2018 if second page must be written
  for (n=0;n<32;n++)
  tmp[n]=read_one_byte_eeprom_address_page((unsigned char)page_end,n); 
  for (n=0;n<size_second;n++)
            tmp[n]=buf[pointer++];
//to flash buffer
NVM.CMD=0x36;
cli();//__disable_interrupt();
CCP=CCP_IOREG_gc;
NVM.CTRLA=0x01;
sei();//__enable_interrupt();
for(n=0;n<100;n++) {
	//200 mks
	delay_mks(200);//__delay_cycles(6350);
	n_first=NVM.STATUS&0x80;
	if(n_first==0x00) break;
}
if(n>=100)return false;					
 //to load buffer
 NVM.CMD=0x33;
 for(n=0;n<32;n++) {
	 NVM.ADDR0=n;
	 NVM.DATA0=tmp[n];
 }		
//erase and write
NVM.CMD=0x35;
//---------------
NVM.ADDR0=(unsigned char)addr_end;
addr_end=addr_end>>8;
NVM.ADDR1=(unsigned char)addr_end;
//--------------
cli();//__disable_interrupt();
CCP=CCP_IOREG_gc;
NVM.CTRLA=0x01;
sei();//__enable_interrupt();
for(n=0;n<100;n++) {
	//200 mks
	delay_mks(200);//__delay_cycles(6350);
	n_first=NVM.STATUS&0x80;
	if(n_first==0x00) break;
}
if(n>=100)return false;
return true;	
	
}
/*! \brief Write one byte to EEPROM using IO mapping.
 *
 *  This function writes one byte to EEPROM using IO-mapped access.
 *  If memory mapped EEPROM is enabled, this function will not work.
 *  This function will cancel all ongoing EEPROM page buffer loading
 *  operations, if any.
 *
 *  \param  pageAddr  EEPROM Page address, between 0 and EEPROM_SIZE/EEPROM_PAGESIZE
 *  \param  byteAddr  EEPROM Byte address, between 0 and EEPROM_PAGESIZE.
 *  \param  value     Byte value to write to EEPROM.
 */
void CPrg::EEPROM_WriteByte(uint8_t pageAddr, uint8_t byteAddr, uint8_t value)
{//NVM.CTRLB |= NVM_EEMAPEN_bm 
	/*  Flush buffer to make sure no unintetional data is written and load
	 *  the "Page Load" command into the command register.
	 */
	EEPROM_FlushBuffer();
	NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

	/* Calculate address */
	uint16_t address = byteAddr;//(uint16_t)(pageAddr*EEPROM_PAGESIZE)|(byteAddr & (EEPROM_PAGESIZE-1));

	/* Set address to write to. */
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	/* Load data to write, which triggers the loading of EEPROM page buffer. */
	NVM.DATA0 = value;

	/*  Issue EEPROM Atomic Write (Erase&Write) command. Load command, write
	 *  the protection signature and execute command.
	 */
	NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
	NVM_EXEC();
}
/*! \brief Flush temporary EEPROM page buffer.
 *
 *  This function flushes the EEPROM page buffers. This function will cancel
 *  any ongoing EEPROM page buffer loading operations, if any.
 *  This function also works for memory mapped EEPROM access.
 *
 *  \note The EEPROM write operations will automatically flush the buffer for you.
 */
void CPrg::EEPROM_FlushBuffer(void)
{
	/* Wait until NVM is not busy. */
	EEPROM_WaitForNVM();

	/* Flush EEPROM page buffer if necessary. */
	if ((NVM.STATUS & NVM_EELOAD_bm) != 0) {
		NVM.CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
		NVM_EXEC();
	}
}


bool  CPrg::to_write_to_eeprom_axis_is_chosen(uint16_t *P20,uint16_t *P21,
		uint16_t *P22,uint16_t *P23){
	unsigned char tmp[4];
	bool error;
		 tmp[0] = flash_prg.read_one_byte_eeprom_address_int(0,&error);
		 tmp[1] = flash_prg.read_one_byte_eeprom_address_int(1,&error);
		 tmp[2] = flash_prg.read_one_byte_eeprom_address_int(2,&error);
		 tmp[3] = flash_prg.read_one_byte_eeprom_address_int(3,&error);
		if(*P20==(unsigned int)tmp[0] && *P21==(unsigned int)tmp[1]
		   && *P22==(unsigned int)tmp[2] && *P23==(unsigned int)tmp[3]) return true;
		 		tmp[0] = (unsigned char)*P20;
		 		tmp[1] = (unsigned char)*P21;
		 		tmp[2] = (unsigned char)*P22;
		 		tmp[3] = (unsigned char)*P23;  
		   
		  error=write_block_to_eeprom_address_int(0,tmp,4);


		
	return error;								   
	}

bool CPrg::to_write_to_eeprom_axis_to_screen(uint16_t *Str1,uint16_t *Str2,uint16_t *Str3,uint16_t *Str4)
{
     unsigned char tmp[4];
     bool error;
     tmp[0] = flash_prg.read_one_byte_eeprom_address_int(4,&error);
     tmp[1] = flash_prg.read_one_byte_eeprom_address_int(5,&error);
     tmp[2] = flash_prg.read_one_byte_eeprom_address_int(6,&error);
     tmp[3] = flash_prg.read_one_byte_eeprom_address_int(7,&error);
     if(*Str1==(unsigned int)tmp[0] && *Str2==(unsigned int)tmp[1]
     && *Str3==(unsigned int)tmp[2] && *Str4==(unsigned int)tmp[3]) return true;
     tmp[0] = (unsigned char)*Str1;
     tmp[1] = (unsigned char)*Str2;
     tmp[2] = (unsigned char)*Str3;
     tmp[3] = (unsigned char)*Str4;
     
     error=write_block_to_eeprom_address_int(4,tmp,4);
     
     return error;
}

bool CPrg::reset_eeprom_param(void)
{
  unsigned char tmp[8] ={0,1,13,13,0,1,13,13}; 
  bool error;
  error=write_block_to_eeprom_address_int(0,tmp,8);
//19.09.2018  
  if(error==false) return false;
//27.02.2018--------------------------------------
   for(int n=0;n<N_AXIS;n++){
   init_to_default_state_axis_cfg(n);
   error=to_save_axis_param_to_EEPROM(n,(unsigned char*)&axis_cfg[n]);
//19.09.2018
   if(error==false) return false;
//--------------------------------   
     switch (n){
	     case 0: coeff.new_coeff_[0].binary=1;coeff.new_coeff_[0].n_after_dot=0;
		         error=to_save_coeff_to_EEPROM(0,(unsigned char*)&coeff.new_coeff_[0]);
				 //19.09.2018
				 if(error==false) return false;
				 break;
	     case 1: coeff.new_coeff_[1].binary=1;coeff.new_coeff_[1].n_after_dot=0;
		         error=to_save_coeff_to_EEPROM(1,(unsigned char*)&coeff.new_coeff_[1]);
				 //19.09.2018
				 if(error==false) return false;
				 break;
	     case 2: coeff.new_coeff_[2].binary=1;coeff.new_coeff_[2].n_after_dot=0;
		         error=to_save_coeff_to_EEPROM(2,(unsigned char*)&coeff.new_coeff_[2]);
				 //19.09.2018
				 if(error==false) return false;
				 break;
	     case 3: coeff.new_coeff_[3].binary=1;coeff.new_coeff_[3].n_after_dot=0;
		         error=to_save_coeff_to_EEPROM(3,(unsigned char*)&coeff.new_coeff_[3]);
				  //19.09.2018
				  if(error==false) return false;
				 break;
     }
   }
   to_save_speed_screen_param_to_EEPROM(reinterpret_cast<uint8_t*>(display.position_to_see));

   reset_eeprom_and_ram_options();
//------------------------------------------------  
  return true;
}

bool CPrg::reset_eeprom_and_ram_options(void)
{
// reset in ram
	init.contrast_value = 25;
	init.to_see_time = false;	
	uart_usb.baudrate = usb115200;
	MsgKey.submode_mode_REF = 1;
// reset in eeprom
	flash_prg.to_save_contrast_to_EEPROM(&init.contrast_value);                  //176
	flash_prg.to_save_to_see_time_to_EEPROM(&init.to_see_time);                   //177
	flash_prg.to_save_Bdrate_toEEPROM(&uart_usb.baudrate);                        //178
	flash_prg.to_save_Rejim_zaxvata_ref_metok_toEEPROM(&MsgKey.submode_mode_REF); //179
	return true;
}

void CPrg::to_check_angle_axis_inside_HAND(long *counter,unsigned char n_ch,bool inverse)
{
	if(!*counter)// если равно нулю то не обрабатываем
		return;
	if(flash_prg.axis_cfg[n_ch].type == CPrg::LINEAR)
		return;

	if(inverse){
		long max = Lir.get_max_circle(n_ch);
		if (inverse)
			*counter = *counter + max;
	}  		
}
/*
функция преобразует счетчик в круговой циклический для вывода на экран
counter - должен быть умноженным на K
*/
void CPrg::endless_circle(long *counter,unsigned char c)
{
	if(*counter == 0)
		return;
	if(flash_prg.axis_cfg[c].type == CPrg::LINEAR)
		return;
	long max = Lir.get_max_circle(c);
	*counter %= max;
	if(*counter < 0)
		*counter = *counter + max;  
}

void CPrg::divide_by_2_counter_rel(unsigned char c)
{
	if(Lir.is_corr(c) || Lir.is_rel_coded_ref(c)){
		cli();
		long cnt = screen.channel_counter[c];
		sei();
		cnt = Lir.conv_c_i(cnt,c,0);
		Lir.R_off[c] += cnt / 2;
		cli();
		//0.240ms == 240 mks
		l_k_s.write_to_realtime((unsigned char *)&Lir.R_off[c],80 + (c * sizeof(long)),sizeof(long));//80 == 0x50		
		sei();
	}
	else{
		cli();
		mCounter[c].rel /= 2;
		sei();
	}
}

void CPrg::mult_by_2_counter_rel(unsigned char c)
{
	if(Lir.is_corr(c) || Lir.is_rel_coded_ref(c)){
		cli();
		long cnt = screen.channel_counter[c];
		sei();
		cnt = Lir.conv_c_i(cnt,c,0);
		Lir.R_off[c] -= cnt;// * 2;
		cli();
		//0.240ms == 240 mks
		l_k_s.write_to_realtime((unsigned char *)&Lir.R_off[c],80 + (c * sizeof(long)),sizeof(long));//80 == 0x50
		sei();
	}
	else{
		cli();
		mCounter[c].rel *= 2;
		sei();
	}
}

enum FLASH_fifo CPrg::is_at45DB081B_ready(void){
	if(myflash.WaitStatus()==true) return READY;
	return BUSY;
 }

bool  CPrg::to_restore_counters(void){
	bool restore=false;
	for(int c = 0 ; c < N_AXIS ; c++){
		if(axis_cfg[c].save == SAVE::YES_SAVE){
			mCounter[c].rel = l_k_s.r_t_save_rel.rel[c];
			mCounter[c].abs_pure = l_k_s.r_t_save_abs.abs[c];
			ref_.pRef[c] = l_k_s.r_t_save_ref.ref[c];
			init.ref_mode[c] = 0;  //false
			restore = true;
		}
	}
	return restore;
}


 CPrg::EXCHANGE_ERRORS  CPrg::to_exchange_45DB081B_with_23LCV1024(void){
//21.12.2018 (264+60000)/264=228,27(27)  229 pages (0-228) for one side(+ or -) for one channel
//ref_.table_is_ready_plus(mines)- 1 byte
//ref_.k_divider_plus(mines)-2 bytes(will be extended to 4 for compatibility in future)
//ref_.set_point_plus(mines)-2 bytes(will be extended to 4 for compatibility in future)
//first page is used like header and rezerv.
unsigned char *byte_pointer;
long tmp;
int n,m; 
bool result;
// return EXCHANGE_ERRORS::TASK_IS_ENDED;	
	    switch (exchange){
		case EXCHANGE::UPLOAD_FLASH_TO_RAM_MINES:	//загрузка пзу- -> озу
			return UPLOAD_FLASH_TO_RAM_MINES_f();				
		case EXCHANGE::UPLOAD_FLASH_TO_RAM_PLUS:  //загрузка пзу+ -> озу
			return UPLOAD_FLASH_TO_RAM_PLUS_f();
		case EXCHANGE::DOWNLOAD_RAM_TO_FLASH_PLUS: //загрузка озу+ -> пзу
		if(read_page_from_23LCV1024==true && block_plus!=1){
		 //init.set_B6();	
		 result=spi.read_bytes_from_23LCV1024(pages_counter,plus,true);
		 //init.clr_B6();
		 if(result==false)return EXCHANGE_ERRORS::READ_FROM_23LCV1024;
		 pages_counter=pages_counter+264;
		 read_page_from_23LCV1024=false;	
		}
		timeout_for_flash--;
		result=myflash.WaitStatus();
		if(result==false){
		if(timeout_for_flash<=0)return EXCHANGE_ERRORS::TIMEOUT_WHEN_WRITE_TO_FLASH;		
		}
		else if(result==true){
		block_plus--;
		if(block_plus<=0){
		//myflash.read_datas_from_flash_using_page_num(to_see_data0,0);
		//myflash.read_datas_from_flash_using_page_num(to_see_data1,flash_start-1);	
		//myflash.read_datas_from_flash_using_page_num(to_see_data2,flash_start);	
		//	return EXCHANGE_ERRORS::TASK_IS_ENDED;
		exchange=EXCHANGE::DOWNLOAD_RAM_TO_FLASH_MINES_FIRST_PAGE;	
		return EXCHANGE_ERRORS::OK;//
			}
//25.12.2018
		flash_start=flash_start+1;
		//init.set_B6();
		myflash.write_datas_to_flash_from_buffer1_using_page_num(plus,flash_start);
		//init.clr_B6();
		timeout_for_flash=10;
		read_page_from_23LCV1024=true;	
		}
		return EXCHANGE_ERRORS::OK;//
		//break;
		case EXCHANGE::DOWNLOAD_RAM_TO_FLASH_MINES: //загрузка озу- -> пзу
		if(read_page_from_23LCV1024==true && block_mines!=1){
				//init.set_B6();
				result=spi.read_bytes_from_23LCV1024(pages_counter,mines,false);
				//init.clr_B6();
				if(result==false)return EXCHANGE_ERRORS::READ_FROM_23LCV1024;
				pages_counter=pages_counter+264;
				read_page_from_23LCV1024=false;
			}
		timeout_for_flash--;
		result=myflash.WaitStatus();
		if(result==false){
		if(timeout_for_flash<=0)return EXCHANGE_ERRORS::TIMEOUT_WHEN_WRITE_TO_FLASH;
		}
		else if(result==true){
			block_mines--;
			if(block_mines<=0){
				//myflash.read_datas_from_flash_using_page_num(to_see_data1,229);
				//myflash.read_datas_from_flash_using_page_num(to_see_data1,flash_start-1);
				//myflash.read_datas_from_flash_using_page_num(to_see_data2,flash_start_mines);
				return EXCHANGE_ERRORS::TASK_IS_ENDED;	
			}
		//25.12.2018
		flash_start_mines=flash_start_mines+1;
		//init.set_B6();
		myflash.write_datas_to_flash_from_buffer1_using_page_num(mines,flash_start_mines);
		//init.clr_B6();
		timeout_for_flash=10;
		read_page_from_23LCV1024=true;
	        }
	    return EXCHANGE_ERRORS::OK;//	
		
		//break;
//===================================================чтение заголовка таблицы коррекции =================================================
		case EXCHANGE::UPLOAD_FLASH_TO_RAM_PREPEARING:
		
		if(ref_.tbl[0] && ref_.tbl[1]) //если таблицы опрошены
			return EXCHANGE_ERRORS::TASK_IS_ENDED;
		
//28.12.2018
        result=myflash.WaitStatus();
        if(!result){
	        if(timeout_for_flash<=0)
				return EXCHANGE_ERRORS::TIMEOUT_WHEN_READ_FROM_FLASH_FIRST_TIME;
        }
        else if(result){
		
		if(ref_.ld_table){				
			myflash.read_datas_from_flash_using_page_num(plus,flash_start,264);
			read_flash=to_convert_header_bytes_from_FLASH_to_int(plus,true);			

			switch(read_flash){
				case  EXCHANGE_ERRORS::IS_NO_TABLE_PLUS_IN_FLASH:
				ref_.k_divider_plus=0;
				ref_.table_is_ready_plus=false;
				ref_.set_point_plus=0;
				//==переходим к считыванию таблицы -
				return goNext_TAble_f(0);
				//==
				return EXCHANGE_ERRORS::TASK_IS_ENDED;

				case  EXCHANGE_ERRORS::GO_NEXT_TABLE_PLUS_IN_FLASH:
				exchange=EXCHANGE::UPLOAD_FLASH_TO_RAM_PLUS;
				pages_counter=0;
				ref_.t_r_p_auto = plus[14];
				ref_.table_p_from_flash = 1;
				//проверить есть ли коэффициент, делитель , кол -во точек
				get_bin_n_after_dot_from_header(Lir.cnt_StrCh,1,plus,mines);
				block_plus=((set_point_plus*4-4)/264)+1;
				return EXCHANGE_ERRORS::OK;

				default:
				return EXCHANGE_ERRORS::OK;
			}
		}
		else
		{
			myflash.read_datas_from_flash_using_page_num(mines,flash_start_mines,264);
			read_flash=to_convert_header_bytes_from_FLASH_to_int(mines,false);

			switch(read_flash){

				case  EXCHANGE_ERRORS::IS_NO_TABLE_MINES_IN_FLASH:
				ref_.k_divider_mines=0;
				ref_.table_is_ready_mines=false;
				ref_.set_point_mines=0;
				//==переходим к считыванию таблицы +
				return goNext_TAble_f(1);
				//==
				return EXCHANGE_ERRORS::TASK_IS_ENDED;

				case  EXCHANGE_ERRORS::GO_NEXT_TABLE_MINES_IN_FLASH:
				exchange=EXCHANGE::UPLOAD_FLASH_TO_RAM_MINES;
				pages_counter=0;
				ref_.t_r_m_auto = mines[14];
				ref_.table_m_from_flash = 1;
				get_bin_n_after_dot_from_header(Lir.cnt_StrCh,0,plus,mines);
				block_mines=((set_point_mines*4-4)/264)+1;
				return EXCHANGE_ERRORS::OK;

				default:
				return EXCHANGE_ERRORS::OK;
			}
		}
		}
		return EXCHANGE_ERRORS::OK;		
		case EXCHANGE::PREPARE_FOR_TO_DOWNLOAD_RAM_TO_FLASH:
		if(ref_.table_is_ready_plus==true){
			for(n=0;n<264;n++)
				plus[n]=0;
			if(ref_.set_point_plus!=0 && ref_.set_point_plus<=CORR_POINTS)
			{
				m=0;
				plus[m++]=1;
				tmp=(long)ref_.k_divider_plus;
				byte_pointer=(unsigned char *)&tmp;
				for(n=0;n<4;n++)
					plus[m++]=byte_pointer[n];
				tmp=(long)ref_.set_point_plus;
				byte_pointer=(unsigned char *)&tmp;
				for(n=0;n<4;n++)
					plus[m++]=byte_pointer[n];
				if(ref_.t_r_p_auto)
					load_coeff_to_page_buf(plus + m,0);
				else
					load_coeff_to_page_buf(plus + m,Lir.cnt_StrCh);
				plus[14] = ref_.t_r_p_auto;
					
				block_plus=(ref_.set_point_plus*4-4)/264;
				block_plus=block_plus+2;
			}
			else {
					ref_.k_divider_plus=0;
					ref_.set_point_plus=0;
					block_plus=1;	
			     }
		
		}
		else {
				ref_.k_divider_plus=0;
				ref_.set_point_plus=0;
				block_plus=1;
				for(int n=0;n<264;n++)
					plus[n]=0;		
			 }

		if(ref_.table_is_ready_mines==true){
			for(n=0;n<264;n++)
				mines[n]=0;
			if(ref_.set_point_mines!=0 && ref_.set_point_mines<=CORR_POINTS)
			{
				m=0;
				mines[m++]=1;
				tmp=(long)ref_.k_divider_mines;
				byte_pointer=(unsigned char *)&tmp;
				for(n=0;n<4;n++)
					mines[m++]=byte_pointer[n];
				tmp=(long)ref_.set_point_mines;
				byte_pointer=(unsigned char *)&tmp;
				for(n=0;n<4;n++)
					mines[m++]=byte_pointer[n];
				if(ref_.t_r_m_auto)
					load_coeff_to_page_buf(mines + m,0);
				else
					load_coeff_to_page_buf(mines + m,Lir.cnt_StrCh);
				mines[14] = ref_.t_r_m_auto;

				block_mines=(ref_.set_point_mines*4-4)/264;
				block_mines=block_mines+2;
			}
			else {
					ref_.k_divider_mines=0;
					ref_.set_point_mines=0;
					block_mines=1;
			     }	
			
		}
		else {
				ref_.k_divider_mines=0;
				ref_.set_point_mines=0;	
				block_mines=1;
				for(int n=0;n<264;n++)
					mines[n]=0;			
		     }

		exchange=EXCHANGE::DOWNLOAD_RAM_TO_FLASH_PLUS_FIRST_PAGE;
		case EXCHANGE::DOWNLOAD_RAM_TO_FLASH_PLUS_FIRST_PAGE:
		pages_counter=0;
		read_page_from_23LCV1024=true;
		result=myflash.WaitStatus();
		if(result==false) return EXCHANGE_ERRORS::WRITE_FIRST_TIME_PLUS;
		myflash.write_datas_to_flash_from_buffer1_using_page_num(plus,flash_start);
		exchange=EXCHANGE::DOWNLOAD_RAM_TO_FLASH_PLUS;
		timeout_for_flash=10;
		return EXCHANGE_ERRORS::OK; 
		case EXCHANGE::DOWNLOAD_RAM_TO_FLASH_MINES_FIRST_PAGE:
		pages_counter=0;
		read_page_from_23LCV1024=true;
		result=myflash.WaitStatus();
		if(result==false) return EXCHANGE_ERRORS::WRITE_FIRST_TIME_MINES;
		myflash.write_datas_to_flash_from_buffer1_using_page_num(mines,flash_start_mines);
		exchange=EXCHANGE::DOWNLOAD_RAM_TO_FLASH_MINES;
		timeout_for_flash=10;
		return EXCHANGE_ERRORS::OK;
		
		 
	   }	
	return EXCHANGE_ERRORS::OK;			
} 

CPrg::EXCHANGE_ERRORS CPrg::UPLOAD_FLASH_TO_RAM_MINES_f()
{
	bool result;
	for(int n=0;n<5;n++){
		flash_start_mines++;
		result=myflash.read_datas_from_flash_using_page_num(mines,flash_start_mines,264);
		if(result==false) return EXCHANGE_ERRORS::READ_FROM_45DB081B;
		//init.set_B6();
		result=spi.write_bytes_to_23LCV1024(pages_counter,mines,false);
		//init.clr_B6();
		if(result==false) return EXCHANGE_ERRORS::WRITE_TO_23LCV1024;
		pages_counter=pages_counter+264;
		block_mines--;
		if(block_mines<=0){
			ref_.k_divider_mines=k_divider_mines;
			ref_.table_is_ready_mines=true;
			ref_.set_point_mines=set_point_mines;
			//USART_DEBUG(1);
			//==переходим к считыванию таблицы +
			return goNext_TAble_f(1);
			//==
			return EXCHANGE_ERRORS::TASK_IS_ENDED;
		}
	}
	return EXCHANGE_ERRORS::OK;
}

CPrg::EXCHANGE_ERRORS CPrg::UPLOAD_FLASH_TO_RAM_PLUS_f()
{
	bool result;
	for(int n=0;n<5;n++){
		flash_start++;
		result=myflash.read_datas_from_flash_using_page_num(plus,flash_start,264);
		if(result==false) return EXCHANGE_ERRORS::READ_FROM_45DB081B;
		//init.set_B6();
		result=spi.write_bytes_to_23LCV1024(pages_counter,plus,true);
		//init.clr_B6();
		if(result==false) return EXCHANGE_ERRORS::WRITE_TO_23LCV1024;
		pages_counter=pages_counter+264;
		block_plus--;
		if(block_plus<=0){
			ref_.k_divider_plus=k_divider_plus;
			ref_.table_is_ready_plus=true;
			ref_.set_point_plus=set_point_plus;
			//USART_DEBUG(2);
			//==переходим к считыванию таблицы -
			return goNext_TAble_f(0);
			//==
			return EXCHANGE_ERRORS::TASK_IS_ENDED;
		}
	}
	return EXCHANGE_ERRORS::OK;
}

CPrg::EXCHANGE_ERRORS CPrg::goNext_TAble_f(int8_t plus)
{
	if(plus){
		if(!ref_.tbl[1]){
			ref_.tbl[1] = 1;
			ref_.ld_table = 1;
			exchange=EXCHANGE::UPLOAD_FLASH_TO_RAM_PREPEARING;
			return EXCHANGE_ERRORS::OK;
		}
	}
	else{
		if(!ref_.tbl[0]){
			ref_.tbl[0] = 1;
			ref_.ld_table = 0;
			exchange=EXCHANGE::UPLOAD_FLASH_TO_RAM_PREPEARING;
			return EXCHANGE_ERRORS::OK;
		}
	}
	
	return EXCHANGE_ERRORS::OK;
}

/*
in const int &ch - канал
*/
bool CPrg::to_do_correction_in_realtime_ch(const int &ch)
{
	unsigned int p_plus;   // адрес во флеш памяти таблицы + откуда считывать
	unsigned int p_minus;  // адрес во флеш памяти таблицы - откуда считывать
	switch (ch)
	{
	case 0:
		p_plus  = CH0_CORR;//0;
		p_minus = CH0_CORR + SZ_CH_CORR_HALF_p;//229;
		break;
	case 1:
		p_plus  = CH1_CORR;//458;
		p_minus = CH1_CORR + SZ_CH_CORR_HALF_p;//687;
		break;
	case 2:
		p_plus  = CH2_CORR;//916;
		p_minus = CH2_CORR + SZ_CH_CORR_HALF_p;//1145;
		break;
	case 3:
		p_plus  = CH3_CORR;//1374;
		p_minus = CH3_CORR + SZ_CH_CORR_HALF_p;//1603;
		break;
	default:
		return false;
	}

	//05.01.2019 it takes 62 mks till to debug
	//04.01.2019 in first header must be read from flash
	//for channel_0 start page for + is 0, for - 229
	unsigned char header_plus[15],header_mines[15];
	long tmp;
	//long delta;
	long result;
	if(!myflash.WaitStatus()){
		altera.error_corr_real_time[ch]=CIOAltera::ERROR_CORR_REAL_TIME::NO_FLASH_ANSWER;
		return false;
	}
	if(!myflash.read_header_from_flash(p_plus,p_minus,header_plus,header_mines)){
		altera.error_corr_real_time[ch]=CIOAltera::ERROR_CORR_REAL_TIME::TIMEOUT_WHEN_HEADER_IS_READ;
		return false;
	}
	tmp = Lir.conv_c_i_c(mCounter[ch].abs_pure,ch, ref_.pRef[ch]);
	
	altera.error_corr_real_time[ch] = to_do_correction_common_in_realtime(&tmp,&result,header_plus,header_mines,p_plus + 1,p_minus + 1,ch);
	if(Lir.is_corr_table(ch) == false){
		//mCounter[ch].abs_pure = tmp;
		//mCounter[ch].delta = delta;

		mCounter[ch].abs = tmp;
		mCounter[ch].rel = tmp;
		
		if(altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_PLUS_||
		   altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_MINES_||
		   altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_MINES_R_||
		   altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_PLUS_L_ )
			{altera.correction_can_be_done_in_real_time[ch] = false;}
		return false;
	}
	else if (Lir.is_corr_table(ch) == true){
		//	counter0.abs=result is kept correction's data.
		//  if it will come to light first time it must be defined.
		//  delta between current and previous must be correct !!! (counter0.rel)
		//  first time marker must be used !!!
		if (altera.correction_can_be_done_in_real_time_[ch] == false && altera.correction_can_be_done_in_real_time[ch] == true){
			mCounter[ch].corr_prev = result;			
			mCounter[ch].rel = result;// - Lir.R_r[ch];//new
			mCounter[ch].abs = result;
			to_see_working_corr[ch] = 0;
			//mCounter[ch].delta_corr = 0;
			mCounter[ch].delta_corr = mCounter[ch].delta; //new
		} else{	
			mCounter[ch].delta_corr = result - mCounter[ch].corr_prev;
			mCounter[ch].corr_prev = result;
			mCounter[ch].abs = result;
			mCounter[ch].rel = result;// - Lir.R_r[ch];//new
			to_see_working_corr[ch]++;
		}		
	}	
	//05.02.2018 debug
	//	counter0.delta_corr=counter0.delta;
	//	counter0.abs +=  counter0.delta;
	//---------------------------------
	return true;
}

//#pragma GCC push_options
//#pragma GCC optimize ("O0")
/*
in     long *abs                     - указател на абсолютный счетчик канала
in_out long *result                  - указатель на результат
in     unsigned char *header_plus    - указатель на массив заголовка таблицы +
in     unsigned char *header_mines	 - указатель на массив заголовка таблицы -
in     unsigned int start_page_plus  - адрес начала таблицы +
in     unsigned int start_page_mines - адрес начала таблицы -
*/
CIOAltera::ERROR_CORR_REAL_TIME CPrg::to_do_correction_common_in_realtime(long *abs,
                                 long *result,unsigned char *header_plus,
								 unsigned char *header_mines,
								 unsigned int start_page_plus,unsigned int start_page_mines,uint8_t ch){
	long k=0;
	bool plus=true;
	long counter=*abs;
	//to_see_interval=counter;
	long up,down;
	bool first_element;
	long delta_0,delta_1;
	long interval,res;
	CDouble::MULT_OR_DIV_ANSWER answer;
	bool flash_answer,flash_answer_;
	 //init.clr_B6();
	long inner_result = 0;			 
	read_flash = to_convert_header_bytes_from_FLASH_to_int(header_plus,true);

	if(read_flash == EXCHANGE_ERRORS::GO_NEXT_TABLE_PLUS_IN_FLASH)
		table_is_ready_plus = true;
	else 
		table_is_ready_plus = false;

	read_flash_mines = to_convert_header_bytes_from_FLASH_to_int(header_mines,false);

	if(read_flash_mines == EXCHANGE_ERRORS::GO_NEXT_TABLE_MINES_IN_FLASH)
		table_is_ready_mines = true;
	else 
		table_is_ready_mines = false;

	if(table_is_ready_plus == false && table_is_ready_mines == false)
		return CIOAltera::ERROR_CORR_REAL_TIME::NO_TABLE_PLUS_NO_TABLE_MINES_;
	if(counter < 0){
		if(table_is_ready_mines == true){
			plus = false;
			k = (long)k_divider_mines;
		}else if(table_is_ready_mines == false && table_is_ready_plus == true){
			return CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_PLUS_L_;
		}
	//else  return CORRECTION_IS_WORKING::TABLE_IS_ENDED_PLUS;
	}else if(counter > 0){
	//plus_table:
		if(table_is_ready_plus == true){
			plus = true;
			k = (long)k_divider_plus;
		}else if(table_is_ready_plus == false && table_is_ready_mines == true){			
			return CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_MINES_R_;
		}
	}else if(counter == 0){
		*result = inner_result;
		return CIOAltera::ERROR_CORR_REAL_TIME::NO_ERROR;
	}

	if(k < 0)
		k = -k;
	interval = k;
	if(counter < 0)
		counter = -counter;
	if(k == 0)
		return CIOAltera::ERROR_CORR_REAL_TIME::INTERVAL_IS_ZERO_;
	//to_see_interval=counter;
	k = counter / k;
	//first check
	if(plus == true){
		if(k >= set_point_plus){ 
			return CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_PLUS_;
			}
	}
	else{
		if(k >= set_point_mines) {
			return CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_MINES_;
			}
	}
//09.01.2019	
	switch(k){
		case 0:
			down = 0;
			if(plus == true)
				flash_answer = myflash.read_table_from_flash_using_random_address(0,&up,start_page_plus); //spi.read_table_from_23LCV1024(0,&up,true);
			else
				flash_answer = myflash.read_table_from_flash_using_random_address(0,&up,start_page_mines);// spi.read_table_from_23LCV1024(0,&up,false);

			if(flash_answer == false)
				return CIOAltera::ERROR_CORR_REAL_TIME::NO_FLASH_ANSWER_COMMON;
		
			first_element = true;
		break;

		default:
			if(plus == true)
				flash_answer = myflash.read_table_from_flash_using_random_address(k-1,&down,start_page_plus); //spi.read_table_from_23LCV1024(k-1,&down,true);
			else 
				flash_answer = myflash.read_table_from_flash_using_random_address(k-1,&down,start_page_mines); //spi.read_table_from_23LCV1024(k-1,&down,false);

			if(plus == true)
				flash_answer_ = myflash.read_table_from_flash_using_random_address(k,&up,start_page_plus); //spi.read_table_from_23LCV1024(k,&up,true);
			else 
				flash_answer_ = myflash.read_table_from_flash_using_random_address(k,&up,start_page_mines); //spi.read_table_from_23LCV1024(k,&up,false);

			if(flash_answer == false || flash_answer_ == false)
				return CIOAltera::ERROR_CORR_REAL_TIME::NO_FLASH_ANSWER_COMMON;

			first_element = false;
		break;
	}	
	if(plus == true){
		if(up >= 0 && down >= 0)
			;
		else 
			return CIOAltera::ERROR_CORR_REAL_TIME::SIGN_PLUS_WRONG_; 	
	}else{
		if(up <= 0 && down <= 0){
			up = -up;
			down = -down;
			}
		else 
			return CIOAltera::ERROR_CORR_REAL_TIME::SIGN_MINES_WRONG_;
		}	
	delta_0 = up - down;
	if(delta_0 < 0)
		return CIOAltera::ERROR_CORR_REAL_TIME::DELTA_BELOW_ZERO_;	
	if(first_element == true){
		delta_0 = 0;
		delta_1 = up - interval;
//11.01.2019 it takes about 25 mks		
		res = altera.coefficient_is_calculating_shot(0,counter,interval);
		if(res > 1000)
			return CIOAltera::ERROR_CORR_REAL_TIME::RESULT_IS_UNREAL_;	
	}else{
		long a = interval * k;
		delta_0 = down - a;
		delta_1 = up - (interval * (k + 1));
		delta_1 = delta_1 - delta_0;
		res = altera.coefficient_is_calculating_shot(a,counter,interval);
		if(res > 1000)
			return CIOAltera::ERROR_CORR_REAL_TIME::RESULT_IS_UNREAL_;
		}
	coeff.set_axis_coeff(4,res,3);
	//11.01.2018 it takes about 90 mks
	answer = coeff.mult(4,&delta_1);
	if (answer != CDouble::OK){
		return CIOAltera::ERROR_CORR_REAL_TIME::MULT_WRONG_;
	}
	inner_result = counter + delta_0 + delta_1;
	if(plus == false) 
		inner_result = -inner_result;
	*result = inner_result;	

//== берем коэффициент из заголовка флэшь
	if(get_bin_n_after_dot_from_header(ch,plus,header_plus,header_mines))
		return CIOAltera::ERROR_CORR_REAL_TIME::DIFFER_COEFF;
//===

	return CIOAltera::ERROR_CORR_REAL_TIME::NO_ERROR;													 
}
//#pragma GCC pop_options







 
