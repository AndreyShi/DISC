/*
 * uart_flash_PD6_7.h
 *
 * Created: 14.06.2017 16:16:43
 *  Author: Андрей
 */ 

#ifndef UART_FLASH_PD6_7_H_
#define UART_FLASH_PD6_7_H_

#include "Strings.h"
#include "main.h"
#include "enum_struct.h"
#include "programm.h"
#include "display.h"
#include "io_altera.h"
extern "C"{
	extern unsigned char to_count_ones_in_byte(unsigned char);
	extern void  delay_mks(int d);
}
extern void delay_ms(long d);



class CUART_usb{
	public:
	unsigned char addr_page;
	uint16_t n; //кол-во принятых байт за одну сессию
	enum Take_byte_answer{
		START_LINK_USB=(unsigned char)0x00,
		FIRST_BYTE_ERROR=(unsigned char)0x01,
		FIRST_BYTE_IS_SYNG=(unsigned char)0x02,
		FIRST_BYTE_IS_NOT_ADRESS=(unsigned char)0x03,
		LENGHT_BYTE_ERROR=(unsigned char)0x0d,
		LENGHT_BYTE_TIMEOUT=(unsigned char)0x04,
		LENGHT_G_MAX_PACKET_LENGTH=(unsigned char)0x05,
		MAIN_STREAM_BUFFER_OVERFLOW=(unsigned char)0x0e,
		MAIN_STREAM_TIMEOUT=(unsigned char)0x06,
		CRC_IS_WRONG=(unsigned char)0x07,
		CRC_IS_OK=(unsigned char)0x08,
		ADRESS_RIO_ERROR=(unsigned char)0x09,
		EXTENDER_IS_OFF=(unsigned char)0x0a,
		EXTENDER_IS_ON=(unsigned char)0x0b,
		INTERNAL_ERROR=(unsigned char)0x0f,
		LENGTH_STREAM_TIMEOUT=(unsigned char)0x10,
		LENGTH_STREAM_BUFFER_OVERFLOW=(unsigned char)0x11,
		LENGTH_ONES_ERROR=(unsigned char)0x12,
		MAIN_ONES_ERROR=(unsigned char)0x13,
		FIRST_BYTE_ERROR_OVERFLOW=(unsigned char)0x14,
		FIRST_BYTE_ERROR_FRAME=(unsigned char)0x15,
		LENGHT_IS_NOT_5_OR_7=(unsigned char)0x16,
		BRIDGE_RS485_PK = 0x17,
		MAX_PACKET_LENGTH=(unsigned char)0xff,		
	};
	enum usb_read_write{
		PROGRAMM_=(char)0x00,
		FUNCTION_=(char)0x01,
		READ_=(char)0x02,
		WRITE_=(char)0x03,
		EEPROM_READ_BIN=(char)0x04,
		EEPROM_WRITE_BIN=(char)0x05,
		EEPROM_WRITE_BIN_ERROR=(char)0x06,
		TEST_=(char)0x55,
		READ_COUNTERS=(char)0x56,
		INIT_DEVICE=(char)0x57,
		SET_TIME = (char)0x07,
//17.04.2018 read correction 
        READ_CORRECTION=(char)0x08,
		SAVE_CORRECTION =(char)0x09,
		CORRECTION_PLUS=(char)0x0a,
		CORRECTION_MINES=(char)0x0b,
		CNC_CMD = (char)0x0c,
		GET_VERSION = (char)0x0d,
//08.10.2018		
		TASK_IS_DONE = (char)0x15,		
	};

	int Recievedblock(unsigned char * buf,int max_block_size);
	int Recievedchar(unsigned char & byte);
	void TransmitBlock(unsigned char * buf,int max_block_size);
	void Sendchar(unsigned char out);

	void display_txt_while_link();
	void reset_error();

	void transmitt_counters();
	void transmit_counters_Sync();
	enum FLASH_fifo glAnswer;
	bool StsLine;
	unsigned int n_prg;
	short cntfr_cur_load;// кол - во кадров текущей считываемой/записываемой программы
	
	bool to_see_frame;
	SEL_FUN_PRG type;
	//05.10.2016----------------------------
	enum FLASH_fifo main_loop_link_through_USB(void);
	enum Take_byte_answer answer;
	enum Take_byte_answer follower;
	void SET_U(Take_byte_answer m);
	Take_byte_answer take_bytes_new(const int &cnt_bytes);
	enum FLASH_fifo read_programm_function_and_answer_through_USB(SEL_FUN_PRG type,unsigned int n_prg,unsigned char block_num);
	enum FLASH_fifo read_programm_function_and_answer_through_USB_(SEL_FUN_PRG type,unsigned int n_prg,unsigned char block_num);
	enum FLASH_fifo write_programm_function_and_answer_through_USB(SEL_FUN_PRG type,unsigned int n_prg,unsigned char block_num);
	enum FLASH_fifo test_through_USB(void);
	void convmin_sec(void *p, int sz);
	//02.12.2016
	unsigned char read_one_byte_eeprom(unsigned char *addr_page,unsigned char *addr_byte);
	bool write_one_page_eeprom(unsigned char *addr_page);
	//06.12.2016
	enum FLASH_fifo read_eeprom_and_send_through_USB(void);
	enum FLASH_fifo send_byte_through_USB(unsigned char a);
	//08.12.2016
	enum FLASH_fifo recieve_datas_through_USB_and_write_to_eeprom(unsigned char *page,unsigned char *buf);
	bool write_one_page_eeprom_buf(unsigned char *addr_page,unsigned char *buf);
	//13.12.2016
	void erase_page_eeprom_buf(void);
	//31.01.2017
	void debug_mode_usb_if_key_BLANK1(void);
	void debug_write_read_eeprom(void);
	void handle_for_display();
	//17.04.2018
	enum FLASH_fifo read_correction_and_send_through_USB(unsigned char data,bool sign,long *counter);
	//--------------------------------------
	unsigned char recieve_byte,status_byte;
	bool flag_byte_is_recieved;
	//15.08.2016----------------
	unsigned char data[272],buf_to_write[264];
	//06.10.2016
	int timeout_usb_transmit;
	//12.10.2016
	bool mode_rs_232_usb;
	//11.11.2016
	unsigned char test;
	//13.12.2016
	bool answer_eeprom_write;
	//24.01.2017
	int error_number;
	//--------------------------
	unsigned char baudrate;
	int8_t trans_cnt;
	void init_uart1_portD(void){
	#ifdef __AVR_ATxmega256A3U__
//== совместимость с bootloader IAR
	USARTD1.CTRLA = 0;
	USARTD1.CTRLB = 0;
//==
	#endif
#ifdef USARTD1
	#undef	USARTD1
	#undef  USARTD1_RXC_vect
	#define USARTD1 USARTD0	
	#define USARTD1_RXC_vect USARTD0_RXC_vect
#else
	#define USARTD1 USARTD0
	#define USARTD1_RXC_vect USARTD0_RXC_vect
#endif
	PORTD.REMAP = 0x10;

	int baudctrla;
	int baudctrlb;
	switch(baudrate)
	{
	     case usb200000:
		 baudctrla = 9;
		 baudctrlb = 0;
	     break;
		 case usb115200:
		 baudctrla = 131;
		 baudctrlb = 208;
		 break;
		 case usb9600:
		 baudctrla = 12;
		 baudctrlb = 4 << 4;
		 break;
	     default:
		 baudctrla = 9;
		 baudctrlb = 0;
	     break;
	}
		PORTD.OUTSET =0x80;
		PORTD.DIRSET =0x80;
		USARTD1.CTRLC=0x03;
		USARTD1.BAUDCTRLA=baudctrla; // 9
		USARTD1.BAUDCTRLB=baudctrlb; // 0 
		USARTD1.CTRLB=0x18;
	}

	void initDMACH2_rx(){
		return;
		#ifdef __AVR_ATxmega256A3U__
		DMA.CTRL |= DMA_RESET_bm;  // reset DMA
		DMA.CH2.CTRLA = DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_1BYTE_gc;
		DMA.CH2.CTRLB = DMA_CH_TRNINTLVL_gm;  // прерывание приоритет HI

		DMA.CH2.ADDRCTRL = DMA_CH_SRCRELOAD_NONE_gc |
						   DMA_CH_SRCDIR_FIXED_gc |
						   DMA_CH_DESTRELOAD_BLOCK_gc |
						   DMA_CH_DESTDIR_INC_gc;

		DMA.CH2.TRIGSRC = DMA_CH_TRIGSRC_USARTD1_RXC_gc;

		DMA.CH2.TRFCNT = 8;//(uint16_t)(sizeof(pack_Rx));

		DMA.CH2.SRCADDR0 = (uint8_t)((uintptr_t)&USARTD1.DATA);
		DMA.CH2.SRCADDR1 = (uint8_t)(((uintptr_t)&USARTD1.DATA) >> 8);
		DMA.CH2.SRCADDR2 = (uint8_t)(((uint32_t)((uintptr_t)&USARTD1.DATA)) >> 16);

		DMA.CH2.DESTADDR0 = (uint8_t)((uintptr_t)&data);
		DMA.CH2.DESTADDR1 = (uint8_t)(((uintptr_t)&data) >> 8);
		DMA.CH2.DESTADDR2 = (uint8_t)(((uint32_t)((uintptr_t)&data)) >> 16);

		//DMA.CH0.CTRLA |= DMA_CH_ENABLE_bm; // Enable CH0

		DMA.CTRL |= DMA_ENABLE_bm; // Enable DMA
		#endif
	}

	void enable_receive_interrupt_D1(void){
		//31.01.2017 low level
		USARTD1.CTRLA=USART_RXCINTLVL_HI_gc;//0x10;
		//30.09.2016 medium level
		//     USARTD1.CTRLA=0x20;
		// high level
		//  USARTD1.CTRLA=0x30;
	}
	void disable_receive_interrupt_D1(void){
		USARTD1.CTRLA=0x00;
	}
	void recieve_interrupt_USARTD1(const unsigned char&cmd){
		USARTD1.CTRLA=cmd;
	}
	//22.11.2016 clear interrupt receive flag
	bool clear_interrupt_receive_flag_D1(void){
		// receiver disable,transmiter disable
		USARTD1.CTRLB=0x00;
		//32*31.25=1000 ns
		delay_mks(1);//__delay_cycles(320);
		// receiver enable,transmiter enable
		USARTD1.CTRLB=0x18;
		//---------------------------------
		char is_buf_full,data;
		for(int n=0;n<1000;n++) {
			is_buf_full=USARTD1.STATUS&0x80;
			if(is_buf_full==0x80) {
				data=USARTD1.DATA;
				data++;
			}
			else 
				return true;
			//32*31.25=1000 ns
			delay_mks(1);//__delay_cycles(320);
		}
		return false;
	}
	//-------------------------------------
	bool transmit_byte_uart1_portD(unsigned char symbol){
		char status=USARTD1.STATUS;
		if(status&0x20) {
			USARTD1.DATA=symbol;
			return true;
		}
		return false;
	}
	void recieve_byte_uart1_portD(void){
		char status=USARTD1.STATUS;
		if (status&0x80) {
			recieve_byte=USARTD1.DATA;
			status_byte=status&0x1c;
			status_byte=(status_byte>>2)&0x07;
			flag_byte_is_recieved=true;
			//2 ms
			//    __delay_cycles(63500);
			//   transmit_byte_uart1_portD(recieve_byte);

		}
	}

	void real_mode_usb(void)
	{

		cli();//__disable_interrupt();
		if(clear_interrupt_receive_flag_D1())
		{
			mode_rs_232_usb = true;
			display.print_flash_local(s00_LINPK,0,0,0);						  
			display.print_flash_local(sBaudrate[baudrate-1],0,25,0);
			error_number = 0;
			init_uart1_portD();
			enable_receive_interrupt_D1();
		}
		else 
		{
			//disable_receive_interrupt_D1();
			recieve_interrupt_USARTD1(USART_RXCINTLVL_OFF_gc);
			display.clear_ram_null();
			display.print_flash_local(sError,0,0,0);
			mode_rs_232_usb = false;
		}
		sei();//__enable_interrupt();

	}
	void disable_mode_usb(void)
	{
		recieve_interrupt_USARTD1(USART_RXCINTLVL_MED_gc);
		mode_rs_232_usb=false;
	}
	void send_cnt();
};
extern CUART_usb uart_usb;
//extern CPrg flash_prg;
#endif /* UART_FLASH_PD6_7_H_ */