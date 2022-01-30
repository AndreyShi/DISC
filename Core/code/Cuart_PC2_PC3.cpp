/* 
* Cuart_PC2_PC3.cpp
*
* Created: 27.08.2018 15:44:51
* Author: Dmitry
*/


#include "Cuart_PC2_PC3.h"
#include "MenuLir.h"
#include "uart_flash_PD6_7.h"

int Cuart_PC2_PC3::Recievedblock(unsigned char * buf,int max_block_size)
{
	for(int x = 0;x<max_block_size;x++)
	{
		if(!Recievedchar(buf[x]))
		return 0;
	}
	return 1;
}

int Cuart_PC2_PC3::Recievedchar(unsigned char & byte)
{	
	int waiting_RXCIF = 0;
	while(!(USARTC0.STATUS & USART_RXCIF_bm))
	{
		if(waiting_RXCIF == 2500)
		return 0;
		waiting_RXCIF++;
	}
	byte = USARTC0.DATA;
	return 1;
}

void Cuart_PC2_PC3::TransmitBlock(unsigned char * buf,int max_block_size)
{
	for(int y = 0;y<max_block_size;y++)
	Sendchar(buf[y]);
}

void Cuart_PC2_PC3::Sendchar(unsigned char out)
{
	while(!(USARTC0.STATUS & USART_DREIF_bm) );//оператор ! делает результат условия противоположным
	USARTC0.DATA = out;
}




uint16_t Cuart_PC2_PC3::g_crc(uint8_t *buf, uint8_t len){
	uint16_t CRC_;
	uint8_t i, k;

	CRC_ = 0xFFFF;
	for (i=0; i<len; i++) {
		CRC_ ^= buf[i];
		for (k=0; k<8; k++) {
			if (CRC_ & 0x01) {
				CRC_ >>= 1;
				CRC_ ^= 0xA001;
			} else CRC_ >>= 1;
		}
	}
	return CRC_;
}
/*
функция для обмена информацией с ПЛК
2200 если отправка + прием
если только отправка то 
*/
void Cuart_PC2_PC3::Tx_PLC()
{
	if(!Lir.get_Extern_PLC())
		return;
	if(uart_usb.mode_rs_232_usb)
		return;

	uart_485.send_package_plc(1,Cuart_PC2_PC3::PANEL,6);
	//=====вместо этого используется DMA режим
	//_delay_us(150 + 370 + 120);// 150 + 370// 1bit - 4us при 250kbit/s  //1с/250000 = 4us	
	//uart_485.recieve_package_plc();
	//=====
}

void Cuart_PC2_PC3::Tx_DRV()
{

}

int Cuart_PC2_PC3::mb_appendCRC16(unsigned char* buf,int nbyte)
{
	uint16_t crc;
	crc = mb_doCRC16(buf, nbyte);
	buf[nbyte] = (uint8_t)crc;
	buf[nbyte+1] = crc >> 8;
	return nbyte+2;
}

uint16_t Cuart_PC2_PC3::mb_doCRC16 (uint8_t *buf, int nbyte)
{
	uint16_t crc;
	signed char i, k;

	/* Рассчет контрольной суммы (16 бит) */
	crc = 0xFFFF;
	for (i=0; i<nbyte; i++) {
		crc ^= buf[i];
		for (k=0; k<8; k++) {
			if (crc & 0x01) {
				crc >>= 1;
				crc ^= 0xA001;         /* 1010 0000 0000 0001 */
			}
			else crc >>= 1;
		}
	}
	return crc;
}
/*
проверка CRC rx пакета
return true - ошибка
*/
bool Cuart_PC2_PC3::do_CRC_rx(){
	short crc,rCRC;
	char* p = (char*)&rCRC;
	crc = mb_doCRC16((unsigned char*)&pack_Rx, PLC_CMD + PLC_DATA_Rx);
	memcpy(p, &pack_Rx.crc[0], 1);
	memcpy(p + 1, &pack_Rx.crc[1], 1);
	//rCRC = (pack_Rx.crc[1] << 8) | pack_Rx.crc[0];	
	if (crc != rCRC)
		return true;
	else
		return false;
}
/*

return 1 - пакет пуст
return 0 - пакет содержит информацию
*/
bool Cuart_PC2_PC3::pack_Rx_empty()
{
	/*short crc = mb_doCRC16((unsigned char*)&pack_Rx, PLC_CMD + PLC_DATA_Rx);
	
	if(crc == -4086) // число -4086 означает что пакет пустой в 0
		return 1;
	else
		return 0; */
	
	if(pack_Rx.cmd[0] == 0 && pack_Rx.cmd[1] == 0 && pack_Rx.cmd[2] == 0 &&
		pack_Rx.m32_m47 == 0 && pack_Rx.m48_m63 == 0 && pack_Rx.m80_m95 == 0 &&
		pack_Rx.crc[0] == 0 && pack_Rx.crc[1] == 0)
			return 1;
		else
			return 0;	
}

/*
срабатывание ошибки на обрыв связи
*/
void Cuart_PC2_PC3::PLC_error_ON()
{
#if (configUSE_PLC == 1)
	//pack_empty > 100 * 10 мсек , если в течении 1 одной секунды подряд пустые пакеты ,то ошибка.
	//Ошибки не прошедший проверку CRC просто записываются в счетчик
	if(!Lir.lock_error_PLC && pack_empty > 100){
		pack_empty = 0;
		Lir.alert = Menu::PLC_ERROR;
		Lir.outM5_delay();
		if(Lir.INmove())
			Lir.move_axis_STOP();
		Lir.lock_error_PLC = true;  
	}  
#endif 
}

bool Cuart_PC2_PC3::send_package_plc(unsigned char addr,TYPE_PACKAGE type,char func)
{ 
	#if (configUSE_PLC == 1)	
	pack_Tx.cmd[0] = addr;
	pack_Tx.cmd[1] = type;
	pack_Tx.cmd[2] = func;
	copy_IN();
	mb_appendCRC16((unsigned char*)&pack_Tx,PLC_CMD + PLC_DATA_Tx);
	init.adm2687_transmitter();
	//Lir.timer1MSEKOFF();
	//init.set_B6();	
	DMA_Tx();//TransmitBlock((unsigned char*)&pack_Tx,MB_BUFFER_LENGTH); //0,270 ms			//
	if(do_CRC_rx()){
		if(!Lir.lock_error_PLC){
			if(!pack_Rx_empty())
				crc_errors_Tx++;
			else
				pack_empty++;
		}
		PLC_error_ON();
	}
	else{
		if(crc_errors_Tx + crc_errors_Rx)
			erros_cnt_disp += crc_errors_Tx + crc_errors_Rx;		
		Lir.lock_error_PLC = false;
		crc_errors_Tx = 0;
		crc_errors_Rx = 0;
		pack_empty = 0;
	}
	memset(&pack_Rx,0,sizeof(pack_Rx));
	if(Lir.curItem->id == idViewOutputs){
		if(Lir.entry_DOUT == Menu::IDLE)
			Lir.entry_DOUT = Menu::UPDATE;
	}
	// ждем ответ от PLC																	//
	#endif																					//
	return 0;																				//
}																							//
																							//
bool Cuart_PC2_PC3::recieve_package_plc()													//
{																							//
	#if (configUSE_PLC == 1)																//
	//DMA_Rx(); 										                                    //
	//init.adm2687_receiver();																//																
	//Recievedblock((unsigned char*)&pack_Rx,MB_BUFFER_LENGTH_Rx);//0,500 ms	
	//_delay_us(500);	
	//Lir.timer1MSEKON();
	//init.clr_B6();
	if (do_CRC_rx()){
		if(!Lir.lock_error_PLC && !pack_Rx_empty())
			crc_errors_Rx++;
		PLC_error_ON();
		reset_USART_RX();	
		return 0;
	} else {
		copy_OUT();
		short* p = (short*)&init.m_in;
		memcpy(p, &pack_Rx.m32_m47, sizeof(short));
		memcpy(p + 1, &pack_Rx.m48_m63, sizeof(short));		
		if(crc_errors_Tx + crc_errors_Rx)
			erros_cnt_disp += crc_errors_Tx + crc_errors_Rx;
		Lir.lock_error_PLC = false;
		crc_errors_Rx = 0;
		crc_errors_Tx = 0;
		pack_empty = 0;
		return 1;
	}
	#endif
	return 0;
}

void Cuart_PC2_PC3::init_uart_PC2_PC3(bool _9bit){
	//PC2 is receiver
	init.C2_is_input();
	//PC3 is transmitter
	init.set_C3();
	//if PD3 is 0, chip adm2687 is receiver
	//if PD3 is 1, chip adm2687 is transmitter
	init.adm2687_transmitter();
	//asynchronous,parity mode disabled,1 stop bit,8 bit
	if(_9bit == 0)
		USARTC0_CTRLC = USART_CHSIZE0_bm | USART_CHSIZE1_bm;//8 bit//asynchronous,parity mode disabled,1 stop bit,9 bit
	else
		USARTC0_CTRLC = USART_CHSIZE0_bm | USART_CHSIZE1_bm | USART_CHSIZE2_bm;// 9 bit

	setSpd250kHz();
	USARTC0_CTRLB = USART_TXEN_bm | USART_RXEN_bm;
	init.uart_485 = true;
}
/*установка скорости 250 000 */
void Cuart_PC2_PC3::setSpd250kHz()
{
	USARTC0_BAUDCTRLA = 7; //speed 250000  //formula: Hz_bdrate * 16x = Hz_CPU , x - USARTC0_BAUDCTRLA
	USARTC0_BAUDCTRLB = 0x00;
	//setSpd115_2kHz();
}
/*установка скорости 115 200 */
void Cuart_PC2_PC3::setSpd115_2kHz()
{
	USARTC0_BAUDCTRLA = 131; 
	USARTC0_BAUDCTRLB = 208;
}
/*установка скорости 57 600*/
void Cuart_PC2_PC3::setSpd57_6kHz()
{
	USARTC0_BAUDCTRLA = 135;
	USARTC0_BAUDCTRLB = 224;
}

/*
сброс приемника, после возникновения ошибки на линии rx необходимо сбросить приемник USART
*/
void Cuart_PC2_PC3::reset_USART_RX()
{
	USARTC0_CTRLB &= ~USART_RXEN_bm;//сброс приемника USART
	USARTC0_CTRLB |= USART_RXEN_bm; //установка приемника USART
}


void Cuart_PC2_PC3::initDMACH0_tx()
{
	DMA.CTRL |= DMA_RESET_bm;  // reset DMA
	DMA.CH0.CTRLA = DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_1BYTE_gc;
	DMA.CH0.CTRLB = DMA_CH_TRNINTLVL_gm;  // прерывание приоритет HI

	DMA.CH0.ADDRCTRL = DMA_CH_SRCRELOAD_BLOCK_gc | 
					   DMA_CH_SRCDIR_INC_gc | 
					   DMA_CH_DESTRELOAD_NONE_gc | 
					   DMA_CH_DESTDIR_FIXED_gc;

	DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_USARTC0_DRE_gc;

	DMA.CH0.TRFCNT = (uint16_t)(sizeof(pack_Tx));//проверить  с -1

	DMA.CH0.SRCADDR0 = (uint8_t)((uintptr_t)&pack_Tx);
	DMA.CH0.SRCADDR1 = (uint8_t)(((uintptr_t)&pack_Tx) >> 8);
	DMA.CH0.SRCADDR2 = (uint8_t)(((uint32_t)((uintptr_t)&pack_Tx)) >> 16);

	DMA.CH0.DESTADDR0 = (uint8_t)((uintptr_t)&USARTC0.DATA);
	DMA.CH0.DESTADDR1 = (uint8_t)(((uintptr_t)&USARTC0.DATA) >> 8);
	DMA.CH0.DESTADDR2 = (uint8_t)(((uint32_t)((uintptr_t)&USARTC0.DATA)) >> 16);

	//DMA.CH0.CTRLA |= DMA_CH_ENABLE_bm; // Enable CH0

	DMA.CTRL |= DMA_ENABLE_bm; // Enable DMA
}

void Cuart_PC2_PC3::initDMACH1_rx()
{
	DMA.CTRL |= DMA_RESET_bm;  // reset DMA
	DMA.CH1.CTRLA = DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_1BYTE_gc;
	DMA.CH1.CTRLB = DMA_CH_TRNINTLVL0_bm;// прерывание LO //DMA_CH_TRNINTLVL_gm;  // прерывание приоритет HI

	DMA.CH1.ADDRCTRL = DMA_CH_SRCRELOAD_NONE_gc |
					   DMA_CH_SRCDIR_FIXED_gc |
					   DMA_CH_DESTRELOAD_BLOCK_gc |
	                   DMA_CH_DESTDIR_INC_gc;

	DMA.CH1.TRIGSRC = DMA_CH_TRIGSRC_USARTC0_RXC_gc;

	DMA.CH1.TRFCNT = (uint16_t)(sizeof(pack_Rx));

	DMA.CH1.SRCADDR0 = (uint8_t)((uintptr_t)&USARTC0.DATA);
	DMA.CH1.SRCADDR1 = (uint8_t)(((uintptr_t)&USARTC0.DATA) >> 8);
	DMA.CH1.SRCADDR2 = (uint8_t)(((uint32_t)((uintptr_t)&USARTC0.DATA)) >> 16);

	DMA.CH1.DESTADDR0 = (uint8_t)((uintptr_t)&pack_Rx);
	DMA.CH1.DESTADDR1 = (uint8_t)(((uintptr_t)&pack_Rx) >> 8);
	DMA.CH1.DESTADDR2 = (uint8_t)(((uint32_t)((uintptr_t)&pack_Rx)) >> 16);

	//DMA.CH0.CTRLA |= DMA_CH_ENABLE_bm; // Enable CH0

	DMA.CTRL |= DMA_ENABLE_bm; // Enable DMA
}

void Cuart_PC2_PC3::DMA_Tx()
{
	DMA.CH0.CTRLA |= DMA_CH_ENABLE_bm;
	DMA.CH0.CTRLA |= DMA_CH_TRFREQ_bm; // передаем первый байт , следущие с помощью триггера
}

char Cuart_PC2_PC3::DMA_Rx()
{
	DMA.CH1.CTRLA |= DMA_CH_ENABLE_bm;
	//DMA.CH1.CTRLA |= DMA_CH_TRFREQ_bm; //передачу инициируют сама переферия
	return 0;
}


/*
копируем выходы в пакет Tx, пакет отправляется каждые 10 msek функцией send_package_plc
*/
void Cuart_PC2_PC3::send_m0_m31(unsigned short *out)
{
	memcpy(&uart_485.pack_Tx.m0_m15,out,sizeof(short));
	memcpy(&uart_485.pack_Tx.m16_m31,out + 1,sizeof(short));
}
/*
копируем Напряжение в пакет Tx, пакет отправляется каждые 10 msek функцией send_package_plc
*/
void Cuart_PC2_PC3::send_U(int16_t *U)
{	
#if (configUSE_PLC == 1)
	uart_485.pack_Tx.a1 = U_to_DAC8563(U[0] + (flash_prg._Allp_param7.U_off_a[0]/10/*отброс последнего разряда*/),0);
	uart_485.pack_Tx.a2 = U_to_DAC8563(U[1] + (flash_prg._Allp_param7.U_off_a[1]/10/*отброс последнего разряда*/),1);
#endif
}
/*
конвертирование напряжения DAC8563
0В это середина кода , 32768
все что больше это от 0 до 10.695В
все что меньше 32768 это от 0 до -10.695В
in int16_t U - напряжение
const int8_t a - текущий выход a1 - 0, a2 - 1
return       - двоичный код для ЦАП
*/
short Cuart_PC2_PC3::U_to_DAC8563(int16_t U,const int8_t a)
{
	
	long DAC_;
	DAC_ = (U * 30638L) / 10000;  // разрядность 32768 / макс напряжение 10695 мВольт ,0.695 В уходит на смещение
	DAC_ += 32768;
	#if (configUSE_PLC == 1)
	if(DAC_ > 3 && DAC_ < 65533)
		DAC_ += (flash_prg._Allp_param7.U_off_a[a] % 10) / 3; // прибавляем младший разряд смещений 0.000 0
	#endif
	return (short)DAC_;
}

//=====копирование m80 - m95  на выходы пульта
void Cuart_PC2_PC3::copy_OUT()
{
#if (configUSE_PLC == 1)
	short m80_m95;
	memcpy(&m80_m95,&pack_Rx.m80_m95,sizeof(short));
	if(m80_m95 != Lir.mem_m80_m95){
		Lir.p_out |= m80_m95; //установка бита
		for(int i = 0 ; i < 16;i++){ // m80-m95 16 bits
			long BIT_n = Lir.plb2l(i);
			if(BIT_n & Lir.mem_m80_m95){
				if(!(m80_m95 & BIT_n))
					Lir.p_out &= ~BIT_n; //сброс бита
			}
		}
		Lir.outP_all();
		Lir.mem_m80_m95 = m80_m95;
	}
#endif
}
//=====копирование входов пульта на m64 - m79 
void Cuart_PC2_PC3::copy_IN()
{	
	short m64_m79;
	memcpy(&m64_m79,&pack_Tx.m64_m79,sizeof(short));
	if(init.p_in != Lir.mem_in){
		m64_m79 |= init.p_in; //установка бита
		for(int i = 0 ; i < 16;i++){ // m64-m79 16 bits
			long BIT_n = Lir.plb2l(i);
			if(BIT_n & Lir.mem_in){
				if(!(init.p_in & BIT_n))
					m64_m79 &= ~BIT_n; //сброс бита
			}
		}
		memcpy(&pack_Tx.m64_m79,&m64_m79,sizeof(short));
		Lir.mem_in = init.p_in;
	}
}


void Cuart_PC2_PC3::_SM::set(STATE st_)
{
	st = st_;
}

Cuart_PC2_PC3::STATE Cuart_PC2_PC3::_SM::get()
{
	return st;
}
