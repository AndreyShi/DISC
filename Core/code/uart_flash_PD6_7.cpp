/*
 * uart_flash_PD6_7.cpp
 *
 * Created: 15.06.2017 9:31:42
 *  Author: Андрей
 */ 
//#define F_CPU 100000UL  // 100 kHz

#include "uart_flash_PD6_7.h"
#include "led_key_sound.h"
#include "display.h"
#include "MenuLir.h"
#include "keyfunc.h"
#include "config.h"
#include <avr/wdt.h>


int CUART_usb::Recievedblock(unsigned char * buf,int max_block_size)
{
	n = 0;
	for(int x = 0; x < max_block_size;x++)
	{
		if(!Recievedchar(buf[x]))
			return 0;
		n++;
	}
	return 1;
}

int CUART_usb::Recievedchar(unsigned char & byte)
{
	int waiting_RXCIF = 0;
	while(!(USARTD1.STATUS & USART_RXCIF_bm))
	{
		if(waiting_RXCIF == 2500) 
			return 0;
		waiting_RXCIF++;
	}
	byte = USARTD1.DATA;
	return 1;
}

void CUART_usb::TransmitBlock(unsigned char * buf,int max_block_size)
{
	for(int y = 0;y<max_block_size;y++)
		Sendchar(buf[y]);
}

void CUART_usb::Sendchar(unsigned char out)
{
	while(!(USARTD1.STATUS & USART_DREIF_bm));//оператор ! делает результат условия противоположным
	USARTD1.DATA = out;
}


void CUART_usb::display_txt_while_link()
{	
#if (configUSE_mcx514 == 1)
	if(Lir.curItem->id == idHand && MsgKey.tracer_real==TMsgKey::SUBMODE_HAND::DRIVE_CONTROL){
		if(data[3] == CNC_CMD && data[4] == CNC_CMD){
			Lir.strcpylir((char*)Lir.bufASCIIstr,(const char*)&data[8]);
			Lir.add_frame();
			memset(Lir.bufASCIIstr,0,IN_BUF_SZ);
			//Lir.show_cmd(0);
			Lir.frame_CURSOR_exe = 2;
			Lir.execute_exe(0);
			memset(data,0,272);
		}
	}
#endif
	if(Lir.curItem->id != idLinkPK){
		if (uart_usb.glAnswer == NO_FLASH_ERROR) {
			if(data[3] == READ_COUNTERS && data[4] == READ_COUNTERS)
			{
				test++;
				if(test&0x01)
					display.print_flash_local(PSTR("+"),0,0,16*14);
				else
					display.print_flash_local(PSTR(" "),0,0,16*14);
				//memset(data,0,272);
			}
			else if(data[3] == TEST_ && data[4] == TEST_){
				test_through_USB();
				memset(data,0,272);
			}
		}
	}
	if(Lir.curItem->id == idLinkPK){
		//------------------------------------------
		if (uart_usb.to_see_frame == true) {
			if(data[4] == READ_){
				if (data[3] == PROGRAMM_)
					display.print_flash_local(PSTR("чтение   программы:"),0,0,16);
				else if (data[3] == FUNCTION_)
					display.print_flash_local(PSTR("чтение подпрограммы:"),0,0,16);
			}
			else if(data[4] == WRITE_){
				if (data[3] == FUNCTION_) 
					display.print_flash_local(PSTR("запись подпрограммы:"),0,0,16);
				else if (data[3] == PROGRAMM_) 
					display.print_flash_local(PSTR("запись программы:"),0,0,16);
				}
			display.print_debug_one_local(uart_usb.n_prg + 1,0,20,16);
			display.print_flash_local(PSTR("кадр:"),0,0,32);
			int cnt = uart_usb.data[6] * 2 > cntfr_cur_load ? uart_usb.data[6] * 2 - 1 : uart_usb.data[6] * 2;
			display.print_debug_one_local(cnt,0,6,32);
			uart_usb.to_see_frame = false;
			}
		
		
		//------------------------------------------
		if (uart_usb.glAnswer != NO_FLASH_ERROR) {
			if(uart_usb.follower == CUART_usb::START_LINK_USB){
				if(uart_usb.glAnswer == MAIN_ONES_ERROR_)
					display.print_flash_local(s06_LINPK,0,0,48);
				else{
					display.print_flash_local(s07_LINPK,0,0,48);
					display.print_debug_one_local(uart_usb.glAnswer,0,12,48);
				}
				display.print_flash_local(s08_LINPK,0,0,64);
				display.print_debug_one_local(uart_usb.error_number,0,18,64);
			}
			
			uart_usb.glAnswer = NO_FLASH_ERROR;
			display.print_symbols_local(' ',40,0,0,16 * 6);
			for(int i = 0; i < 40;i++){
				if(data[i] == '\0')
					break;
				display.put_char_local_independent(data[i],0,i,16 * 6);
				} 
			//display.print_hex(data,10,0 ,16 * 7);
			memset(data,0,272);
		}

		if (data[3] == TEST_ && data[4] == TEST_) {
			test++;
			display.print_symbols_local(0x20,40,0,0,16);
			display.print_symbols_local(0x20,40,0,0,32);
			display.print_flash_local(s01_LINPK,0,0,16);
			if(test&0x01)
				display.print_flash_local(PSTR("+"),0,6,16);
			else
				display.print_flash_local(PSTR(" "),0,6,16);
			memset(data,0,272);		
		}
		else if (data[3] == EEPROM_READ_BIN && data[4] == EEPROM_READ_BIN) {
			display.print_flash_local(s02_LINPK,0,0,16);
			display.print_flash_local(s03_LINPK,0,0,32);
			Lir.print_long_format(addr_page*32,CPrg::FORMAT_END,0,16,32,0,0,3);
			memset(data,0,272);
		}
		else if (data[3]==EEPROM_WRITE_BIN && data[4] == EEPROM_WRITE_BIN) {
				if(data[5] == data[6]) {					
					if(data[6] == 0) {
						display.print_flash_local(s04_LINPK,0,0,16);
						display.print_flash_local(s05_LINPK,0,0,32);
					}
					Lir.print_long_format((data[6]+1)*32,CPrg::FORMAT_END,0,16,32,0,0,3);
				}
				memset(data,0,272);
			}
	
	}
}
/*
сброс ошибок по кнопке 'C'
*/
void CUART_usb::reset_error()
{
	uart_usb.error_number = 0;
	for(int i = 0; i <2;i++)
		display.print_symbols_local(' ',40,0,0,16*(3 + i));
}

void CUART_usb::transmitt_counters()
{
	unsigned char *buf;
	unsigned short crc;
	unsigned char buf_send[16];

	for(int i = 0; i<N_AXIS; i++){	
		//buf=(unsigned char*)&screen.channel_counter[i];
		//buf=(unsigned char*)&mCounter[i].delta;
		buf = (unsigned char*)&screen.str_counter_before_out[i];
		for(int j = 0; j < 4;j++){
			buf_send[j + (i * 4)] = buf[j];
		}
	}
	crc = Lir.crc16(buf_send,16);

	TransmitBlock(buf_send,16);
	TransmitBlock(reinterpret_cast<unsigned char*>(&crc),sizeof(short));

	/*for(int i = 0; i<N_AXIS; i ++){
		buf=(unsigned char*)&mCounter[i].rel;
		TransmitBlock(buf,sizeof(long));
	} */
	
}

void CUART_usb::transmit_counters_Sync()
{
	#if (configUSE_NCshell == 0 || configUSE_mcx514 == 1)//только для индикации и mcx514
	if (Lir.curItem->id != idLinkPK){
		if(uart_usb.trans_cnt >= 3){ // 3 - 40мс - 50мс периодичность
			//init.set_B6();
			Sendchar(0x0A);
			for(int s = 0 ; s < N_STR;s++){
				long tmp = Lir.hex2bcd_l(screen.str_counter_before_out[s]);
				TransmitBlock((uint8_t*)&tmp,sizeof(long));
			}
			Sendchar(0x0B);
			//init.clr_B6();
			uart_usb.trans_cnt = 0;
		}		
	}
	#endif
}

//#include "programm.h"
enum FLASH_fifo CUART_usb::main_loop_link_through_USB(void)
{
unsigned char usb_flash_error,block_num;
to_see_frame=false;
int timeout_usb_transmit_;
static enum FLASH_fifo answer_flash ;
   
	answer = take_bytes_new(272);		     
           
	if (answer == CRC_IS_OK){
		if(data[3] == WRITE_){
			//сброс для входа в бутлоадер
			cli();
			while (1);
		}		   			
        if(data[4] == READ_){
		      to_see_frame = true;
              if (data[3] == PROGRAMM_){ 
				  type = PROGRAMM;
                  block_num = data[6];
                  }
              else if (data[3] == FUNCTION_){ 
				    type = FUNCTION;
                    block_num = data[6];
                  }
				n_prg = (unsigned int)data[5];
				if(block_num == 0){
				#if (configUSE_NCshell == 1)
					Menu::header_prg header_toPk;
					uint32_t addres = Lir.get_addres_prg_cur(data[3],n_prg) - HEADER_PRG_SZ;
					myflash.Read_flash_at45(addres, (uint8_t*)&header_toPk, sizeof(header_toPk));
					TransmitBlock((uint8_t*)&header_toPk,sizeof(header_toPk));
					//==CRC16
					unsigned short crc16 = Lir.crc((uint8_t*)&header_toPk,sizeof(header_toPk));
					TransmitBlock((uint8_t*)&crc16,sizeof(crc16));
					//==
					if(header_toPk.sign == (short)0xAAAA)
						cntfr_cur_load = header_toPk.cnt;
				#endif
				}
				else{
				#if (configUSE_NCshell == 1)	
					struct block2frame541 {
						unsigned char fr[2][FRAME_FLASH_SZ];
					}block;				
					uint32_t addres = Lir.get_addres_prg_cur(data[3],n_prg) + ((block_num - 1) * (FRAME_FLASH_SZ * 2));
					myflash.Read_flash_at45(addres, (uint8_t*)&block,FRAME_FLASH_SZ * 2);	
					//при отправке в ПК	конвертируем символы 0x01 в '\''(минута) и 0x02 в '\"' (секунда)
					convmin_sec((uint8_t*)&block, sizeof(block));		
					TransmitBlock((uint8_t*)&block,sizeof(block));
					//==CRC16
					unsigned short crc16 = Lir.crc((uint8_t*)&block,sizeof(block));
					TransmitBlock((uint8_t*)&crc16,sizeof(crc16));
					//==
				#endif
				}
				answer_flash = NO_FLASH_ERROR;//read_programm_function_and_answer_through_USB(type,n_prg,block_num);
           }
		   else if(data[3] == CNC_CMD && data[4] == CNC_CMD)
					answer_flash = NO_FLASH_ERROR;
			else if(data[3] == GET_VERSION && data[4] == GET_VERSION){
					TransmitBlock((unsigned char*)display.VERSION_ram,13);
					answer_flash = NO_FLASH_ERROR;
					}
          else if (data[4] == WRITE_){
              to_see_frame=true;
              if (data[3] == FUNCTION_) { 
					type = FUNCTION;
					n_prg = (unsigned int)data[5];
					block_num = data[6];
                }
              else if (data[3] == PROGRAMM_) {
					type = PROGRAMM;
					n_prg = (unsigned int)data[5];
					block_num = data[6];
                }
				if(block_num == 0){
					//==CRC16
					#if (configUSE_NCshell == 1)
					uint16_t crc16 = Lir.crc(data + 3,5 + sizeof(Menu::header_prg));
					uint16_t crc_pk = 0;
					memcpy(&crc_pk,data + 8 + sizeof(Menu::header_prg),2);
					TransmitBlock((unsigned char*)&crc16,2);
					if(crc_pk != crc16)						
						return CRC_READ_ERROR;
					//==
					Menu::header_prg header_fromPk;
					memcpy(&header_fromPk,data + 8,sizeof(Menu::header_prg));
					if(header_fromPk.sign == (short)0xAAAA)
						cntfr_cur_load = header_fromPk.cnt;
					else
						return READ_ERROR;
					uint32_t addres = Lir.get_addres_prg_cur(data[3],n_prg) - HEADER_PRG_SZ;//рассчет адреса					
					myflash.Write_flash_at45(addres,8 + data, sizeof(Menu::header_prg));//записываем заголовок во flash	
					if(Lir._choice_prg.addres_prg == (addres + HEADER_PRG_SZ )){ //обновить текущую
						Lir._choice_prg.cnt = header_fromPk.cnt;
						//==обнуляем индекс выбранного кадра ,скроллинг и кол -во вызванных подпрограмм
						Lir.entry_scroll_EDITPR = 0;
						Lir.entry_cnt_EDITPR = 0;
						Lir.call_sub = 0;
						//==
						//==обнуляем установленный кадровый таймер
						Lir.timer_frm = 0;
						Lir.timer_sv = 0;
						//==
						}
					#endif				
					return NO_FLASH_ERROR;						
					}
					else{
						#if (configUSE_NCshell == 1)
						struct block2frame541 {
							unsigned char fr[2][FRAME_FLASH_SZ];
						};
						//==CRC16
						uint16_t crc16 = Lir.crc(data + 3,5 + sizeof(block2frame541));
						uint16_t crc_pk = 0;
						memcpy(&crc_pk,data + 8 + sizeof(block2frame541),2);
						TransmitBlock((unsigned char*)&crc16,2);
						if(crc_pk != crc16)
							return CRC_READ_ERROR;
						//==
						uint32_t addres = Lir.get_addres_prg_cur(data[3],n_prg) + ((block_num - 1) * (FRAME_FLASH_SZ * 2));//рассчет адреса
						Lir.append_crc(Lir.crc(data + 8 , display.strnlen_lir((const char*)data + 8 ,FRAME_FLASH_SZ - 2)),data,8 + (FRAME_FLASH_SZ - 2));
						Lir.append_crc(Lir.crc(data + 8 + FRAME_FLASH_SZ, display.strnlen_lir((const char*)data + 8 + FRAME_FLASH_SZ,FRAME_FLASH_SZ - 2)),data,8 + FRAME_FLASH_SZ + (FRAME_FLASH_SZ - 2));
						myflash.Write_flash_at45(addres,data + 8, sizeof(block2frame541));//записываем блок во flash	
						#endif
						return NO_FLASH_ERROR;
					}
           //answer_flash=write_programm_function_and_answer_through_USB(type,n_prg,block_num);
                }
//23.11.2016 
          else if (data[3] == TEST_ && data[4] == TEST_) {
				answer_flash = test_through_USB();
          }
          else if (data[3] == EEPROM_READ_BIN && data[4] == EEPROM_READ_BIN) {
				answer_flash = read_eeprom_and_send_through_USB();
          }
          else if (data[3] == EEPROM_WRITE_BIN && data[4] == EEPROM_WRITE_BIN) {
				if(data[5] == data[6]) {
					block_num = data[6];
					for(int n=0;n<32;n++) {
						buf_to_write[n] = data[n+8];
					}
					answer_flash = recieve_datas_through_USB_and_write_to_eeprom(&block_num, buf_to_write);
				}
				else
					answer_flash = WRITE_PAGE_ERROR;
          }
		  else if (data[3] == SET_TIME && data[4] == SET_TIME)
		  {		    
            int low=data[14];
			int high=data[15];
			high=(high<<8)|low;			 
			 cli();
			l_k_s.setup_time[0]=0;
			l_k_s.setup_time[1]=data[13]; 
			l_k_s.setup_time[2]=data[12]; 
			l_k_s.setup_time[3]=data[11];
			l_k_s.setup_time[4]=data[10];
			l_k_s.setup_time[5]=data[8];
			l_k_s.setup_time[6]=data[9];
			l_k_s.follower=Cled_key_sound::SETUP_TIME;
			l_k_s.year=high-(data[8]>>6);
			l_k_s.it_was_setup_time=true;  
			 sei(); 
			if(transmit_byte_uart1_portD(usb_read_write::TASK_IS_DONE))  
				answer_flash = NO_FLASH_ERROR;
			else
				answer_flash = TIMEOUT_TRANSFER_ERROR;
		  }
//18.04.2018 		  
		  else if(data[3] == usb_read_write::READ_CORRECTION && data[4] == usb_read_write::READ_CORRECTION ){
//28.05.2018 it must be solved later,now two buffers are used,one for plus,another for mines					  
		     
		 if(data[6] == usb_read_write::CORRECTION_PLUS)
			answer_flash=read_correction_and_send_through_USB(data[5],true,&spi.counter_correction_plus);
		 else if(data[6] == usb_read_write::CORRECTION_MINES)
			answer_flash = read_correction_and_send_through_USB(data[5],false,&spi.counter_correction_mines);
         else return WRONG_SIGN_CORRECTION;		  
		  display.print_flash_local(PSTR("Передано точек +:"),0,0,16);
		  display.print_debug_one_local(spi.counter_correction_plus,0,18,16);
		  display.print_flash_local(PSTR("Передано точек -:"),0,0,32);
		  display.print_debug_one_local(spi.counter_correction_mines,0,18,32);
				  } 

//		  else if (data[0]==SAVE_CORRECTION && data[1]==SAVE_CORRECTION) {
	      else if (data[3] == SAVE_CORRECTION) {
		  display.print_flash_local(PSTR("запись коррекции в озу"),0,0,16);
		  display.print_flash_local(PSTR("                      "),0,0,32);
          if(data[4] == 0x0a){
			display.print_flash_local(PSTR("точек +: "),0,0,32);  
		  }
		  else if(data[4] == 0x0b){
			     display.print_flash_local(PSTR("точек -: "),0,0,32);
		     }		  
		  answer_flash = NO_FLASH_ERROR;
		  }
	} else if(answer == BRIDGE_RS485_PK){//пересылка сообщения в Rs485		
		uint16_t crc_ = Lir.crc(data,n);
		data[n] = *(((uint8_t*)&crc_) + 1);
		data[n + 1] = (uint8_t)crc_;
		init.adm2687_transmitter();
		cli();
		uart_485.TransmitBlock(data,n + 2);
		sei();
		return BRIDGE;
	} else { //06.10.2016 it is place for error's service

         if (answer_flash == TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ)
          return answer_flash;
          switch(answer){
			case FIRST_BYTE_ERROR: answer_flash = FIRST_BYTE_ERROR_;break;
			case LENGTH_STREAM_BUFFER_OVERFLOW: answer_flash = LENGTH_STREAM_BUFFER_OVERFLOW_;break;
			case LENGTH_STREAM_TIMEOUT: answer_flash = LENGTH_STREAM_TIMEOUT_;break;
			case LENGTH_ONES_ERROR: answer_flash = LENGTH_ONES_ERROR_;break;
			case MAIN_STREAM_BUFFER_OVERFLOW: answer_flash = MAIN_STREAM_BUFFER_OVERFLOW_;break;
			case MAIN_STREAM_TIMEOUT: answer_flash = MAIN_STREAM_TIMEOUT_;break;
			case MAIN_ONES_ERROR: answer_flash = MAIN_ONES_ERROR_;break;
			case FIRST_BYTE_ERROR_OVERFLOW: answer_flash = FIRST_BYTE_ERROR_OVERFLOW_;break;
			case FIRST_BYTE_ERROR_FRAME: answer_flash = FIRST_BYTE_ERROR_FRAME_;break;
			case LENGHT_IS_NOT_5_OR_7: answer_flash = LENGHT_IS_NOT_5_OR_7_;break;
			default: answer_flash = UNKNOWN_FLASH_FIFO;break;
          }
           cli();//__disable_interrupt();
           timeout_usb_transmit=0;
           sei();//__enable_interrupt();
           usb_flash_error=(unsigned char)answer_flash;
           for( ;;){
				if (transmit_byte_uart1_portD(usb_flash_error))
						break;
					cli();//__disable_interrupt();
						timeout_usb_transmit_=timeout_usb_transmit;
					 sei();//__enable_interrupt();
						if (timeout_usb_transmit_>=2){
							answer_flash=TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ;
							break;
							}
                   }
           }

return answer_flash;
}
enum FLASH_fifo CUART_usb::read_correction_and_send_through_USB(unsigned char type_block,
bool sign,long  *counter){
static enum FLASH_fifo  answer;
char tmp[4];
int n;
int k;
int s;
long number;
long start_address,table_datas,end_address;
unsigned char *byte_pointer;
//14.06.2018
if(sign==true){
 k=-ref_.k_divider_plus;
 s=ref_.set_point_plus;	
}
else{
k=ref_.k_divider_mines;
s=ref_.set_point_mines;		
}
//---------------------------------
tmp[0]=(char)k;
tmp[1]=k>>8;
tmp[2]=(char)s;
tmp[3]=s>>8; 
   if(type_block==0){
	   *counter=0;
for (n=0;n<4;n++) {
	answer=send_byte_through_USB(tmp[n]);
	if(answer!=READY) return answer;
   }
   return NO_FLASH_ERROR;
   }
 number=*counter;
//delay_ms(100);
//return NO_FLASH_ERROR;
//14.06.2018 only up to 4000 can be transmitted 
         switch (type_block){
		 case 1:start_address=0;break;	 
		 case 2:start_address=1000;break;
		 case 3:start_address=2000;break;
		 case 4:start_address=3000;break;
		 case 5:start_address=4000;break;
		 case 6:start_address=5000;break;
		 case 7:start_address=6000;break;
		 case 8:start_address=7000;break;
		 case 9:start_address=8000;break;
		 case 10:start_address=9000;break;
		 case 11:start_address=10000;break;	
		 case 12:start_address=11000;break;
		 case 13:start_address=12000;break;
		 case 14:start_address=13000;break;	
		 case 15:start_address=14000;break;	
		 default: return WRONG_BLOCK_NUMBER;
		 }
//------------------------------------------------		 
	if(sign==true)end_address=ref_.set_point_plus-start_address;
	else end_address=ref_.set_point_mines-start_address;
	if(end_address<=0)return WRONG_BLOCK_NUMBER;
	if(end_address>1000)end_address=start_address+1000;
	else end_address=start_address+end_address;
//-------------------------------------------------	
   for(;;){
	   
    if(sign==true)spi.read_table_from_23LCV1024(start_address,&table_datas,true);
	else spi.read_table_from_23LCV1024(start_address,&table_datas,false);
	byte_pointer=(unsigned char *)&table_datas;
	//for(n=0;n<4;n++)tmp[n]=byte_pointer[n];
	 for(n=0;n<4;n++)answer=send_byte_through_USB(byte_pointer[n]);
     if(answer!=READY) {*counter=number;return answer;}
	 start_address++;
	 number++;
	 if(start_address>=end_address)break;
   }
*counter=number;

return NO_FLASH_ERROR;
};

void CUART_usb::send_cnt()
{
	if(uart_usb.data[3] == CUART_usb::READ_COUNTERS && uart_usb.data[4] == CUART_usb::READ_COUNTERS)
	{
		uart_usb.transmitt_counters();
		memset(uart_usb.data,0,272);
		uart_usb.data[3] = 0;
		uart_usb.data[4] = 0;
	};
}

/*
установка режима обмена с ПК
*/
void CUART_usb::SET_U(Take_byte_answer m)
{
	follower = m;
}

CUART_usb::Take_byte_answer CUART_usb::take_bytes_new(const int &cnt_bytes)
{		
	Recievedblock(data,cnt_bytes);
	if(follower == START_LINK_USB){
		if(data[0] == 192 || data[0] == 128)
			return LENGTH_STREAM_TIMEOUT;
		int sum = 0;
		for(int i = 3 ; i < 7; i++)
			sum += to_count_ones_in_byte(data[i]);

		if (sum != data[7]) 
			return MAIN_ONES_ERROR;

		return CRC_IS_OK;	
	}
	else if(follower == BRIDGE_RS485_PK)
		return BRIDGE_RS485_PK;
	else
		return INTERNAL_ERROR;
}

enum FLASH_fifo CUART_usb::read_programm_function_and_answer_through_USB(SEL_FUN_PRG type,unsigned int n_prg,unsigned char block_num){
unsigned char buf[33];
static unsigned int limit,timeout_usb_transmit_,frame,block;
 static enum FLASH_fifo  answer_flash;
          if (type==PROGRAMM) {
             switch (block_num){
             case 0:limit=121;frame=0;break;
             case 1:limit=242;frame=121;break;
             case 2:limit=363;frame=242;break;
             case 3:limit=484;frame=363;break;
             case 4:limit=512;frame=484;break;
             }
             }
           if (type==FUNCTION){
             limit=104;frame=0;
                              }
//----------------
     for(block=0;frame<limit;frame++) {
         cli();//__disable_interrupt();
         timeout_usb_transmit=0;
          sei();//__enable_interrupt();
          //  answer_flash=flash_prg.read_frame_from_at45DB041B(type,n_prg,frame+1,buf);
//12.10.2016 only for debug
//            for (unsigned char tmp=0;tmp<33;tmp++)
//            buf[tmp]=0x55;
//-------------------------
            switch (answer_flash){
            case NO_FLASH_ERROR:
            case ERROR_CHECK_ONES:
            case ERROR_CHECK_31_32:
              answer_flash=NO_FLASH_ERROR;
            for(unsigned char n=0;n<33;){
//16.11.2016 debug
            if(n==31) {
            data[39]=buf[n];
            }
//06.10.2016 if 38400 33*260=8580 mks timeout 10 ms
       if (transmit_byte_uart1_portD(buf[n])) n++;
         cli();//__disable_interrupt();
         timeout_usb_transmit_=timeout_usb_transmit;
          sei();//__enable_interrupt();
       if (timeout_usb_transmit_>=2){answer_flash=TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ;break;}
            }
            break;
            default:
             break;
            }
       if(answer_flash==NO_FLASH_ERROR){
           block++;
 //debug---------------------------------
       //display.gotoxy(7,32);
       //display.print_debug_one(frame+1, 0);
	     display.print_debug_one_local(frame+1,0,7,32);
//       display.gotoxy(1,120);
//       display.print_debug_one(buf[0], 0);
//       display.gotoxy(5,120);
//       display.print_debug_one(buf[1], 0);
//       display.gotoxy(9,120);
//       display.print_debug_one(buf[2], 0);
//       display.gotoxy(13,120);
//       display.print_debug_one(buf[3], 0);
//---------------------------------------



       if(block==121) {
         for(unsigned char n=0;n<7;){
//12.10.2016 rs232 to usb: only 4000 bytes block
// 121*33=3993+7
       if (transmit_byte_uart1_portD(0x55)) n++;
         cli();//__disable_interrupt();
         timeout_usb_transmit_=timeout_usb_transmit;
          sei();//__enable_interrupt();
       if (timeout_usb_transmit_>=2){answer_flash=TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ;break;}
            }
//          __disable_interrupt();
//         timeout_usb_transmit=0;
//          __enable_interrupt();
          block=0;


//          for(;;){
//           __disable_interrupt();
//         timeout_usb_transmit_=timeout_usb_transmit;
//          __enable_interrupt();
//         if  (timeout_usb_transmit_>=0)break;
//                 }
       }

            }
       else {
//06.10.2016 it is place for error's service
         if (answer_flash==TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ)
           return answer_flash;
           cli();//__disable_interrupt();
           timeout_usb_transmit=0;
           sei();//__enable_interrupt();
           buf[0]=(unsigned char)answer_flash;
           for( ;;){
    if (transmit_byte_uart1_portD(buf[0])) break;
          cli();//__disable_interrupt();
         timeout_usb_transmit_=timeout_usb_transmit;
          sei();//__enable_interrupt();
    if (timeout_usb_transmit_>=2){answer_flash=TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ;break;}
                   }
          return answer_flash;
            }
            }
return answer_flash;

}

enum FLASH_fifo CUART_usb::write_programm_function_and_answer_through_USB(SEL_FUN_PRG type,unsigned int n_prg,unsigned char page){
enum FLASH_fifo answer_flash=WRITE_PAGE_OK;
unsigned char sum,how_many_ones;
unsigned char buf_to_answer[5];
int timeout_usb_transmit_;
//14.11.2016 prepare to answer
    enum ANSWER{
       OK=(unsigned char)0x00,
       CRC_IS_WRONG=(unsigned char)0x01,
       HARDWARE_ERROR_LIR581=(unsigned char)0x02,
       FLASH_IS_BUSY=(unsigned char)0x03, 
       MAY_BE_LINK_ERROR =(unsigned char)0x04,   
       };
//it is length
//       buf_to_answer[0]=0x00;
       buf_to_answer[0]=page;
       buf_to_answer[1]=0x02;    
//debug---------------------------------
      // display.gotoxy(7,32);
      // display.print_debug_one((page+1)*8, 0);
	  display.print_debug_one_local((page+1)*8,0,7,32);
//--------------------------------------
//it will be one's check
//       for(int n=0;n<264;n++) {
//          ch=buf_to_write[n]=data[n+5];
//          if(ch!=0x54) answer_flash=BYTE_IS_NOT_85; 
//       }
//16.11.2016 it is ones check
   for(int n=0;n<264;n++) {
          buf_to_write[n]=data[n+8];     
       }
   for (int n=0;n<8;n++ ) {
       sum=0;
       for (int m=0;m<26;m++) {
//it takes 31 cycles 31*31.25=968.75 ns            
        unsigned char a=buf_to_write[n*33+m];	
        how_many_ones=to_count_ones_in_byte(a);
        sum=sum+how_many_ones;
        }
   if (buf_to_write[n*33+32]!=buf_to_write[n*33+31]){answer_flash=BYTE_IS_NOT_85;
   break;}
   if(sum!=buf_to_write[n*33+32]) {answer_flash=BYTE_IS_NOT_85;
   break;}
   }
//  buf_to_answer[0]=sum;

//answer_flash=BYTE_IS_NOT_85;

//08.09.2016--- both must be the same------
//13.10.2016 if frame will be written empty0 will be full (0x00)
//   if (frame_from_flash[25]==0)check_ones=true;
//   else  check_ones=false;
//--------------------------------------------------------------

// if (answer_flash==WRITE_PAGE_OK||answer_flash==BYTE_IS_NOT_85) {
   if (answer_flash==WRITE_PAGE_OK) {
// answer_flash=flash_prg.page_is_coming_outside_and_must_be_written(n_prg,type,buf_to_write,page);
 answer_flash= NO_FLASH_ERROR;

 }
//it will be receipt (answer to computer)
//14.11.2016------------------
          switch (answer_flash) {
          case NUM_PRG_TOO_BIG_D:
          case PAGE_LIMIT_EXCEED:
          buf_to_answer[2]=MAY_BE_LINK_ERROR;
          break;
          case TIMEOUT_WRITE_TO_BUF1_ERROR:
          case TIMEOUT_TRANSFER_TO_BUF:
          buf_to_answer[2]=HARDWARE_ERROR_LIR581;
          break;
          case BYTE_IS_NOT_85:
          buf_to_answer[2]= CRC_IS_WRONG;
          break;
          case WRITE_PAGE_OK:
          buf_to_answer[2]= OK;
          break;
          case TIMEOUT_CYCLE_WAIT_WRITE:
          case TIMEOUT_CYCLE_WAIT_COMPARE:
          case TIMEOUT_CYCLE_WAIT_ERASE:
          buf_to_answer[2]= FLASH_IS_BUSY;
          break;
          case NO_FLASH_ERROR:
          buf_to_answer[2]= OK;
          break;
          case CHECK_BUFFER_PAGE_ERROR:
          case CHECK_EPE_ERROR:
          case CHECK_ERROR:
          buf_to_answer[2]=HARDWARE_ERROR_LIR581;
          break;
          default:
          buf_to_answer[2]= FLASH_IS_BUSY;
          break;
          }
//----------------       
	 sum=0;
     how_many_ones=to_count_ones_in_byte(buf_to_answer[0]);
     sum=sum+how_many_ones;
     how_many_ones=to_count_ones_in_byte(buf_to_answer[1]);
     sum=sum+how_many_ones;
     how_many_ones=to_count_ones_in_byte(buf_to_answer[2]);
     sum=sum+how_many_ones;
     buf_to_answer[3]=sum;
//----------------
           cli();//__disable_interrupt();
           timeout_usb_transmit=0;
           sei();//__enable_interrupt();
        for(unsigned char n=0;n<4;){
			if (transmit_byte_uart1_portD(buf_to_answer[n])) 
				n++;
			cli();//__disable_interrupt();
			timeout_usb_transmit_=timeout_usb_transmit;
			sei();//__enable_interrupt();
			if (timeout_usb_transmit_>=2){
				answer_flash=TRANSMITER_TIMEOUT_COMP_LINK;
				break;
			}
         }
//----------------
    for(int n=0;n<264;n++) {
          buf_to_write[n]=0;
       }
return answer_flash;

}
enum FLASH_fifo CUART_usb::test_through_USB(void){
unsigned char buf_to_answer[8];
int timeout_usb_transmit_;
enum FLASH_fifo answer_flash=NO_FLASH_ERROR;
//debug
//return answer_flash;
//length
 buf_to_answer[0]=0;
 buf_to_answer[1]=8;
//length's ones
 buf_to_answer[2]=1;
//type
 buf_to_answer[3]=data[3];//TEST_;
 buf_to_answer[4]=data[4];//TEST_;
 buf_to_answer[5]=data[5];//TEST_;
 buf_to_answer[6]=data[6];//TEST_;
//type's ones
 buf_to_answer[7]=data[7];//16;
//-------------------------------
          cli();//__disable_interrupt();
           timeout_usb_transmit=0;
         sei();// __enable_interrupt();
        for(unsigned char n=0;n<8;){
   if (transmit_byte_uart1_portD(buf_to_answer[n])) n++;
   cli();//__disable_interrupt();
   timeout_usb_transmit_=timeout_usb_transmit;
   sei();//__enable_interrupt();
   if (timeout_usb_transmit_>=2){answer_flash=TRANSMITER_TIMEOUT_COMP_LINK;break;}
            }
return answer_flash;

}
//конвертируем символы 0x01 в '\''(минута) и 0x02 в '\"' (секунда)
//для отправки в ПК
void CUART_usb::convmin_sec(void *p, int sz)
{
	char *p_ch = (char*)p;
	for(int i = 0 ; i < sz; i++){
		if(p_ch[i] == 0x01)
			p_ch[i] = '\'';
		if(p_ch[i] == 0x02)
			p_ch[i] = '\"';
	}
}

enum FLASH_fifo CUART_usb::read_programm_function_and_answer_through_USB_(SEL_FUN_PRG type,unsigned int n_prg,unsigned char block_num)
{
unsigned char buf[33];
static unsigned int limit,timeout_usb_transmit_,frame,block;
static enum FLASH_fifo answer_flash;
bool speed;
          if (type==PROGRAMM) {
             switch (block_num){
             case 0:limit=121;frame=0;break;
             case 1:limit=242;frame=121;break;
             case 2:limit=363;frame=242;break;
             case 3:limit=484;frame=363;break;
             case 4:limit=512;frame=484;break;
             }
             }
           if (type==FUNCTION){
             limit=104;frame=0;
                              }
//----------------
     for(block=0;frame<limit;frame++) {
         cli();//__disable_interrupt();
         timeout_usb_transmit=0;
         sei();//__enable_interrupt();
         if(frame==0 || frame==121 || frame==242 || frame==484) {
         //       answer_flash=flash_prg.read_frame_from_at45DB041B(type,n_prg,frame+1,buf);
         }
//12.10.2016 only for debug
//            for (unsigned char tmp=0;tmp<33;tmp++)
//            buf[tmp]=0x55;
//-------------------------
            switch (answer_flash){
            case NO_FLASH_ERROR:
            case ERROR_CHECK_ONES:
            case ERROR_CHECK_31_32:
              answer_flash=NO_FLASH_ERROR;
            for(unsigned char n=0;n<33;){
//16.11.2016 debug
 //           if(n==31) {
 //           data[36]=buf[n];
 //           }
//06.10.2016 if 38400 33*260=8580 mks timeout 10 ms
           speed=false;
           if(n==32 ) {
         //  if(frame==0 || frame==121 || frame==242 || frame==484)
           speed=true;
           }
           if(speed) {
               for(;;) {
               if(transmit_byte_uart1_portD(buf[n])){
               //answer_flash=flash_prg.read_frame_from_at45DB041B(type,n_prg,frame+1,buf);
               n++;
               answer_flash=NO_FLASH_ERROR;
               break; 
               }
               }
               }
       else {
       if (transmit_byte_uart1_portD(buf[n])) n++;
         cli();//__disable_interrupt();
         timeout_usb_transmit_=timeout_usb_transmit;
          sei();//__enable_interrupt();
       if (timeout_usb_transmit_>=2){answer_flash=TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ;break;}
            }
            }
            break;
            default:
             break;
            }
//-------if(frame==0 || frame==121 || frame==242 || frame==484) {
//            }
//-----------------------

       if(answer_flash==NO_FLASH_ERROR){
           block++;
 //debug---------------------------------
       //display.gotoxy(7,32);
       //display.print_debug_one(frame+1, 0);
	   display.print_debug_one_local(frame+1,0,7,32);
//       display.gotoxy(1,120);
//       display.print_debug_one(buf[0], 0);
//       display.gotoxy(5,120);
//       display.print_debug_one(buf[1], 0);
//       display.gotoxy(9,120);
//       display.print_debug_one(buf[2], 0);
//       display.gotoxy(13,120);
//       display.print_debug_one(buf[3], 0);
//---------------------------------------



       if(block==121) {
         for(unsigned char n=0;n<7;){
//12.10.2016 rs232 to usb: only 4000 bytes block
// 121*33=3993+7
       if (transmit_byte_uart1_portD(0x55)) n++;
         cli();//__disable_interrupt();
         timeout_usb_transmit_=timeout_usb_transmit;
          sei();//__enable_interrupt();
       if (timeout_usb_transmit_>=2){answer_flash=TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ;break;}
            }
//          __disable_interrupt();
//         timeout_usb_transmit=0;
//          __enable_interrupt();
          block=0;


//          for(;;){
//           __disable_interrupt();
//         timeout_usb_transmit_=timeout_usb_transmit;
//          __enable_interrupt();
//         if  (timeout_usb_transmit_>=0)break;
//                 }
       }

            }
       else {
//06.10.2016 it is place for error's service
         if (answer_flash==TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ)
           return answer_flash;
           cli();//__disable_interrupt();
           timeout_usb_transmit=0;
           sei();//__enable_interrupt();
           buf[0]=(unsigned char)answer_flash;
           for( ;;){
    if (transmit_byte_uart1_portD(buf[0])) break;
          cli();//__disable_interrupt();
         timeout_usb_transmit_=timeout_usb_transmit;
          sei();//__enable_interrupt();
    if (timeout_usb_transmit_>=2){answer_flash=TRANSMITER_TIMEOUT_WHEN_FRAME_IS_READ;break;}
                   }
           return answer_flash;
            }
            }
return answer_flash;

}
unsigned char CUART_usb::read_one_byte_eeprom(unsigned char *addr_page,unsigned char *addr_byte){
unsigned char a,b,result;
//NVM.CTRLB=0x08;
NVM.CMD=0x06;
//---------------
a=*addr_byte&0x1f;
b=*addr_page&0x07;
b=(b<<5)&0xe0;
a=a|b;
NVM.ADDR0=a;
b=*addr_page&0x78;
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
bool CUART_usb::write_one_page_eeprom(unsigned char *addr_page){
unsigned char a,b,n;
//bool result;
//NVM.CTRLB=0x08;
//to load buffer
NVM.CMD=0x33;
for(n=0;n<32;n++) {
     NVM.ADDR0=n;
     NVM.DATA0=0xff;
}
//erase and write
NVM.CMD=0x35;
//---------------
b=*addr_page&0x07;
b=(b<<5)&0xe0;
NVM.ADDR0=b;
a=*addr_page&0x78;
a=(a>>3)&0x0f;
NVM.ADDR1=a;
//--------------
cli();//__disable_interrupt();
CCP=CCP_IOREG_gc;
NVM.CTRLA=0x01;
sei();//__enable_interrupt();
for(n=0;n<100;n++) {
  //200 mks
    delay_mks(200);//__delay_cycles(6350);
    a=NVM.STATUS&0x80;
    if(a==0x00) return true;
}
return false;    
}
enum FLASH_fifo CUART_usb::read_eeprom_and_send_through_USB(void){
unsigned char a,m;
unsigned char addr_byte;
//enum FLASH_fifo answer; 
	cli();
	addr_page=0x00;
	for (m=0;m<125;m++) {
		addr_byte=0x00;
		for (char n=0;n<32;n++) {
			a=uart_usb.read_one_byte_eeprom(&addr_page,&addr_byte);
			//answer=send_byte_through_USB(a);
			Sendchar(a);
			//if(answer!=READY) 
			//	return answer;
			addr_byte++;
		}
		addr_page++;
		wdt_reset();
		//Lir.print_long_format(addr_page*32,CPrg::FORMAT_END,0,16,32,0,0,3);
	}
	for (;m<128;m++) {
		addr_byte=0x00;
		for (char n=0;n<32;n++) {
			a=uart_usb.read_one_byte_eeprom(&addr_page,&addr_byte);
			//answer=send_byte_through_USB(a);
			Sendchar(a);
			//if(answer!=READY) 
			//	return answer;
			addr_byte++;
		}
		addr_page++;
		//Lir.print_long_format(addr_page*32,CPrg::FORMAT_END,0,16,32,0,0,3);
	} 
	sei();
	//we must wait (16+4) ms
	return NO_FLASH_ERROR;
}

enum FLASH_fifo CUART_usb::send_byte_through_USB(unsigned char a){
	int timeout_usb_transmit_;
    cli();
    timeout_usb_transmit=0;
    sei(); 
	  
    for(;;){
		if (transmit_byte_uart1_portD(a)) 
			return READY;
        cli();
        timeout_usb_transmit_=timeout_usb_transmit;
        sei();
		if (timeout_usb_transmit_>=2)
		    return TRANSMITER_TIMEOUT_WHEN_EEPROM_IS_READ;
    }
}
enum FLASH_fifo CUART_usb::
recieve_datas_through_USB_and_write_to_eeprom(unsigned char *page,unsigned char *buf){
unsigned char sum,how_many_ones,buf_to_answer[5];
int timeout_usb_transmit_;
    enum ANSWER{
       OK=(unsigned char)0x00,
       CRC_IS_WRONG=(unsigned char)0x01,
       HARDWARE_ERROR_LIR581=(unsigned char)0x02,
       FLASH_IS_BUSY=(unsigned char)0x03, 
       MAY_BE_LINK_ERROR =(unsigned char)0x04,   
       };
//13.12.2016--page buf must be erased only if write was unsuccessfull
if(answer_eeprom_write==false) erase_page_eeprom_buf();
//-----------------------------------------------------
answer_eeprom_write=write_one_page_eeprom_buf(page,buf);
//12.12.2016----------------
//it is length
//       buf_to_answer[0]=0x00;
       buf_to_answer[0]=*page;
       buf_to_answer[1]=0x02;    
//------------------------------------ 
       if(answer_eeprom_write==true) {
			buf_to_answer[2]=OK; 
			//display.print_debug_one_local((*page+1)*32,0,16,32);
			//Lir.print_long_format((*page+1)*32,CPrg::FORMAT_END,0,16,32,0,0,3); 
			//отображение в функции handle_for_display();                               
       }
       else  
			buf_to_answer[2]=HARDWARE_ERROR_LIR581;
//----------------       
     sum=0;
     how_many_ones=to_count_ones_in_byte(buf_to_answer[0]);
     sum=sum+how_many_ones;
     how_many_ones=to_count_ones_in_byte(buf_to_answer[1]);
     sum=sum+how_many_ones;
     how_many_ones=to_count_ones_in_byte(buf_to_answer[2]);
     sum=sum+how_many_ones;
     buf_to_answer[3]=sum;
//----------------
           cli();
           timeout_usb_transmit=0;
           sei();
        for(unsigned char n=0;n<4;){
			if (transmit_byte_uart1_portD(buf_to_answer[n])) 
				n++;
			cli();
			timeout_usb_transmit_=timeout_usb_transmit;
			sei();
			if (timeout_usb_transmit_>=2)
				return TRANSMITER_TIMEOUT_COMP_LINK;
            }

if(answer_eeprom_write==true)
	return NO_FLASH_ERROR;
return EEPROM_WRITE_ERROR;
}
bool CUART_usb::write_one_page_eeprom_buf(unsigned char *addr_page,unsigned char *buf){
unsigned char a,b,n;
//bool result;
//NVM.CTRLB=0x08;
//to load buffer
NVM.CMD=0x33;
for(n=0;n<32;n++) {
     NVM.ADDR0=n;
     NVM.DATA0=buf[n];
}
//erase and write
NVM.CMD=0x35;
//---------------
b=*addr_page&0x07;
b=(b<<5)&0xe0;
NVM.ADDR0=b;
a=*addr_page&0x78;
a=(a>>3)&0x0f;
NVM.ADDR1=a;
//--------------
cli();//__disable_interrupt();
CCP=CCP_IOREG_gc;
NVM.CTRLA=0x01;
sei();//__enable_interrupt();
for(n=0;n<100;n++) {
  //200 mks
    delay_mks(200);//__delay_cycles(6350);
    a=NVM.STATUS&0x80;
    if(a==0x00) return true;
}
return false;    
}
void CUART_usb::erase_page_eeprom_buf(void){
unsigned char a,n;
NVM.CMD=0x36;
cli();//__disable_interrupt();
CCP=CCP_IOREG_gc;
NVM.CTRLA=0x01;
sei();//__enable_interrupt();
for(n=0;n<100;n++) {
//200 mks *100= 20 ms
    delay_mks(200);//__delay_cycles(6350);
    a=NVM.STATUS&0x80;
    if(a==0x00) return;
}
return;    
}
void CUART_usb::debug_mode_usb_if_key_BLANK1(void){
//uart_usb.debug_mode_usb_if_key_BLANK1();
//  __disable_interrupt();
// if(uart_usb.clear_interrupt_receive_flag_D1()){
//    if(uart_usb.mode_rs_232_usb==false) uart_usb.mode_rs_232_usb=true;
//    else  uart_usb.mode_rs_232_usb=false;
//    if (uart_usb.mode_rs_232_usb==true){
//       display.gotoxy(0, 0);
//       display.print_flash((__flash_ptr)"–ежим св¤зь через usb",0);
//       uart_usb.error_number=0;  
//       uart_usb.init_uart1_portD();      
//       uart_usb.enable_receive_interrupt_D1();     
//    }
//     else {
//       uart_usb.disable_receive_interrupt_D1();
//       display.clear_ram(0,0);
//    }
//    }
//    else {
//       uart_usb.disable_receive_interrupt_D1();
//       display.clear_ram(0,0);
//    }
// __enable_interrupt();
}

void CUART_usb::debug_write_read_eeprom(void){
//02.12.2016 read one byte eeprom
// 128 pages every 32 bytes 128*32=4096 
unsigned char a=0x00;
unsigned char b=0x00;
unsigned char addr_page,addr_byte;
bool result;
//02.12.2016 read one byte eeprom
// 128 pages every 32 bytes 128*32=4096 
addr_page=0x00;
addr_byte=0x00;
//for (unsigned char m=0;m<128;m++) {
//  addr_byte=0x00;
//for (char n=0;n<32;n++) {
  result=uart_usb.write_one_page_eeprom(&addr_page);

  if(result==true) {
          addr_page=0x00;
          addr_byte=0x00;
      a=uart_usb.read_one_byte_eeprom(&addr_page,&addr_byte);
          addr_byte=0x1f;
      b=uart_usb.read_one_byte_eeprom(&addr_page,&addr_byte); 
  }
  else a=0x01;
//  addr_byte++;
//}
//addr_page++;
//}
 //display.gotoxy(0,140);
 //display.print_debug_one(a, 0);
 display.print_debug_one_local(a,0,0,140);
 //display.gotoxy(5,140);
 //display.print_debug_one(b, 0);
 display.print_debug_one_local(b,0,5,140);
}
/*
функция для отображения длительно непрерывно меняющейся информации на экране 
во время связи по USB
uart_usb.StsLine - глобальная переменная по ней определяется состояние передачи  0 - нет передачи 1 - передача активна
*/
void CUART_usb::handle_for_display()
{
	if(uart_usb.StsLine){
		if(data[3]==EEPROM_WRITE_BIN && data[4]==EEPROM_WRITE_BIN) {		
			Lir.print_long_format((data[6]+1)*32,CPrg::FORMAT_END,0,16,32,0,0,3);
			if(data[6] == 127)
				uart_usb.StsLine = 0;
		}
	}
}



