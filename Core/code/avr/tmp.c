#include "io.h"
unsigned char reg_avr;


void  delay_mks(int d){

}


void restore_y_pair(int y_par){

}

char to_check_data_80000000(long data){
	return 0;
}


unsigned int to_take_address(char *ptr){
	return ptr;
}

void  delay_1_ms(void){}

unsigned int to_take_SP( void ){
	return 0x24fE;
}

unsigned int to_fill_SP_memory(void){
	return 1;
}


unsigned char get_adcal_calibration_low(void){
	return 1;
}
unsigned char get_adcal_calibration_high(void){
	return 1;
}

int to_check_SP_memory(void){
	return 1;
}


VPORT_t vport0;/* Virtual Port */
VPORT_t vport1;/* Virtual Port */
VPORT_t vport2; /* Virtual Port */
VPORT_t vport3; /* Virtual Port */
OCD_t ocd; /* On-Chip Debug System */
CLK_t clk; /* Clock System */
SLEEP_t sleep; /* Sleep Controller */
OSC_t osc; /* Oscillator */
DFLL_t dfllrc32m; /* DFLL */
DFLL_t dfllrc2m; /* DFLL */
PR_t pr; /* Power Reduction */
RST_t rst; /* Reset */
WDT_t wdt; /* Watch-Dog Timer */
MCU_t mcu; /* MCU Control */
PMIC_t pmic; /* Programmable Multi-level Interrupt Controller */
PORTCFG_t portcfg; /* I/O port Configuration */
AES_t aes; /* AES Module */
CRC_t crc; /* Cyclic Redundancy Checker */
DMA_t dma; /* DMA Controller */
EVSYS_t evsys; /* Event System */
NVM_t nvw; /* Non-volatile Memory Controller */
ADC_t adca; /* Analog-to-Digital Converter */
ADC_t adcb; /* Analog-to-Digital Converter */
DAC_t dacb; /* Digital-to-Analog Converter */
AC_t aca; /* Analog Comparator */
AC_t acb; /* Analog Comparator */
RTC_t rtc; /* Real-Time Counter */
TWI_t twic; /* Two-Wire Interface */
TWI_t twie; /* Two-Wire Interface */
USB_t usb; /* Universal Serial Bus */
PORT_t porta; /* I/O Ports */
PORT_t portb; /* I/O Ports */
PORT_t portc; /* I/O Ports */
PORT_t portd; /* I/O Ports */
PORT_t porte; /* I/O Ports */
PORT_t portf; /* I/O Ports */
PORT_t portr; /* I/O Ports */
TC0_t tcc0; /* 16-bit Timer/Counter 0 */
TC2_t tcc2; /* 16-bit Timer/Counter type 2 */
TC1_t tcc1; /* 16-bit Timer/Counter 1 */
AWEX_t awexc; /* Advanced Waveform Extension */
HIRES_t hiresc; /* High-Resolution Extension */
USART_t usartc0; /* Universal Synchronous/Asynchronous Receiver/Transmitter */
USART_t usartc1; /* Universal Synchronous/Asynchronous Receiver/Transmitter */
SPI_t spic; /* Serial Peripheral Interface */
IRCOM_t ircom; /* IR Communication Module */
TC0_t tcd0; /* 16-bit Timer/Counter 0 */
TC2_t tcd2; /* 16-bit Timer/Counter type 2 */
TC1_t tcd1; /* 16-bit Timer/Counter 1 */
HIRES_t hiresd; /* High-Resolution Extension */
USART_t usartd0; /* Universal Synchronous/Asynchronous Receiver/Transmitter */
USART_t usartd1; /* Universal Synchronous/Asynchronous Receiver/Transmitter */
SPI_t spid; /* Serial Peripheral Interface */
TC0_t tce0; /* 16-bit Timer/Counter 0 */
TC2_t tce2; /* 16-bit Timer/Counter type 2 */
TC1_t tce1; /* 16-bit Timer/Counter 1 */
AWEX_t awexe; /* Advanced Waveform Extension */
HIRES_t hirese; /* High-Resolution Extension */
USART_t usarte0; /* Universal Synchronous/Asynchronous Receiver/Transmitter */
USART_t usarte1; /* Universal Synchronous/Asynchronous Receiver/Transmitter */
SPI_t spie; /* Serial Peripheral Interface */
TC0_t tcf0; /* 16-bit Timer/Counter 0 */
TC2_t tcf2; /* 16-bit Timer/Counter type 2 */
HIRES_t hiresf; /* High-Resolution Extension */
USART_t usartf0; /* Universal Synchronous/Asynchronous Receiver/Transmitter */


