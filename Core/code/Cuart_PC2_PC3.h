/* 
* Cuart_PC2_PC3.h
*
* Created: 27.08.2018 15:44:51
* Author: Dmitry
*/


#ifndef __CUART_PC2_PC3_H__
#define __CUART_PC2_PC3_H__
#include "main.h"

#define PLC_CMD          3  //байта
#define PLC_DATA_Tx      10 //байт
#define PLC_DATA_Rx      6  //байт
#define CRC_LENGTH       2  //байта

#define MB_BUFFER_LENGTH_Tx PLC_CMD + PLC_DATA_Tx + CRC_LENGTH // структура пакета на отправку в ПЛК
#define MB_BUFFER_LENGTH_Rx PLC_CMD + PLC_DATA_Rx + CRC_LENGTH // структура пакета при получении от ПЛК

class Cuart_PC2_PC3
{
public:
	Cuart_PC2_PC3(){
		//начальная инициализация для обхода ошибки
		pack_Rx.crc[1] = 0xf0;
		pack_Rx.crc[0] = 0x0A;
		 //инициализация напряжения переехала в другое место т к добавляется смещение а оно считывается позже
		//pack_Tx.a1 = 32768; //32768 - это 0 вольт для DAC8583
		//pack_Tx.a2 = 32768; //32768 - это 0 вольт для DAC8583		
	}
	enum TYPE_PACKAGE{
		NULL_P = 0x00,
		RPOSITION = 4,
		GET_PLCINFO = 0x11,
		PANEL = 0x60,
	};
	//unsigned char pack[MB_BUFFER_LENGTH];
	struct _packTx {
		char cmd[PLC_CMD];// адрес, тип, функция
		short m0_m15;
		short m64_m79; // сюда копируются входы пульта
		short m16_m31;
		short a1; 
		short a2; 
		char crc[CRC_LENGTH]; // crc
		long dummy;// для DMA прерывания  + 140 us
	}pack_Tx;//пакет на отправку size 15 + 4 байта dummy

	struct _packRx {
		char cmd[PLC_CMD];// адрес, тип, функция
		short m32_m47;
		short m80_m95;
		short m48_m63;
		char crc[CRC_LENGTH]; // crc
	}pack_Rx;//пакет приемный size 11
	
	//==
	uint16_t crc_errors_Rx;
	uint16_t crc_errors_Tx;
	uint16_t pack_empty;
	uint32_t erros_cnt_disp;
	//==
	enum STATE : int8_t{
		PLC,
		DRV0,
		DRV1,
		DRV2,
		DRV3,
	};
	struct _SM{
		STATE st;
		void set(STATE st_);
		STATE get();
	}SM;
	int Recievedblock(unsigned char * buf,int max_block_size);
	int Recievedchar(unsigned char & byte);
	void TransmitBlock(unsigned char * buf,int max_block_size);
	void Sendchar(unsigned char out);

	int mb_appendCRC16(unsigned char* buf,int nbyte);
	uint16_t mb_doCRC16 (uint8_t *buf, int nbyte);
	bool do_CRC_rx();
	bool pack_Rx_empty();
	void PLC_error_ON();
	uint16_t g_crc (uint8_t *buf, uint8_t len);

	void Tx_PLC();
	void Tx_DRV();
	bool send_package_plc(unsigned char addr,TYPE_PACKAGE type,char func);
	bool recieve_package_plc();
		
	void init_uart_PC2_PC3(bool _9bit = 0);
	void setSpd250kHz();
	void setSpd115_2kHz();
	void setSpd57_6kHz();
	void reset_USART_RX();
	void initDMACH0_tx();
	void initDMACH1_rx();
	void DMA_Tx();
	char DMA_Rx();

	void send_m0_m31(unsigned short *out);
	void send_U(int16_t *out);
	short U_to_DAC8563(int16_t U,const int8_t a);
	void copy_OUT();
	void copy_IN();
	//03.09.2018 

	void RxOn(){
		USARTC0_CTRLB=0x10;
	};
	void TxOn(){
		USARTC0_CTRLB=0x08;
	};

	void set9bit(){
		USARTC0_CTRLC = USART_CHSIZE0_bm | USART_CHSIZE1_bm | USART_CHSIZE2_bm;// 9 bit
	}

	void set8bit(){
		USARTC0_CTRLC = USART_CHSIZE0_bm | USART_CHSIZE1_bm;//8 bit
	}

//04.09.2018 receiver disable(simply way to flush receive buffer)
void flush_receive_buffer(void){
	USARTC0_CTRLB= USARTC0_CTRLB&0xef;
	delay_mks(1);
	USARTC0_CTRLB= USARTC0_CTRLB|0x10;
}
//---------------------------
 void set_ninth_bit_for_address_to_be_sent(void){
	 USARTC0_CTRLB= USARTC0_CTRLB|0x01; 	 
 }
void clear_ninth_bit_for_data_to_be_sent(void){
	USARTC0_CTRLB= USARTC0_CTRLB&0xfe;
}

private:
}; //Cuart_PC2_PC3
extern Cuart_PC2_PC3 uart_485;
#endif //__CUART_PC2_PC3_H__
