#ifndef _at45_h
#define _at45_h

#include "spi.h"
#include "display.h"
#define Manufacturer_ID_at45_adesto 0x1F      // стр 60 описания чипа AT45DB081E_adesto.pdf
#define STS_RESET_P_at45            0x1FFE00  // адрес во флешь для хранения данных о сбросах
#define CRC_appflash_at45           0x1FFE08  // адрес для хранения crc flash xmega
#define Main_Memory_Page_Read_at45  0xD2
#define RMW_at45                    0x58      // Read-Modify-Write internal buffer 1
#define STATUS_at45                 0xD7      // Read Status Reg At45
#define PagetoBuf1                  0x53      // Main Memory Page to Buffer 1 Transfer
#define PageProg1			      	0x82	  // Main Memory Page Program Through Buffer 1


//=========Correction addresing==
#define MAX_INTERVAL_PLUS     2000000000L
#define MAX_INTERVAL_MINUS   -2000000000L
#define MAX_INTERVAL_PLUS_A   30000L //максимальный размер интервала для автоматического режима
#define MAX_INTERVAL_MINUS_A -30000L //максимальный размер интервала для автоматического режима
#define OFFSET_CORR_DEST 0L
#define CORR_POINTS 500L
#define HEADER_CORR 264
#define PAGE_AT45 264
#define SZ_CH_CORR_HALF (HEADER_CORR + (CORR_POINTS * 4))
#define SZ_CH_CORR_HALF_p (SZ_CH_CORR_HALF % PAGE_AT45 ? \
						  ((SZ_CH_CORR_HALF / PAGE_AT45) + 1):\
						  (SZ_CH_CORR_HALF / PAGE_AT45))
#define SZ_CH_CORR_ALL  (2 * SZ_CH_CORR_HALF_p) // первая половина + , вторая половина -
#define CH0_CORR 0
#define CH1_CORR SZ_CH_CORR_ALL
#define CH2_CORR (2 * SZ_CH_CORR_ALL)
#define CH3_CORR (3 * SZ_CH_CORR_ALL)
//===============================

//========Programm adressing====
#define is_264PAGE_SZ(x) (x/264 << 9) | (x - ((x/264)*264))
#define MAX_PRG 20
#define MAX_SUBPRG 50
#define FRAME_FLASH_SZ 66UL
#define HEADER_PRG_SZ 132UL
#define PRG_SZ ((FRAME_FLASH_SZ * 500UL) + HEADER_PRG_SZ)    //33132
#define SUBPRG_SZ ((FRAME_FLASH_SZ * 100UL) + HEADER_PRG_SZ) //6732
#define SIGN_PRG 0XAAAA
#define OFFSET_PRG_DEST ((CH3_CORR + SZ_CH_CORR_ALL) * 264)//
#define AT_45_SIZE_1MB 1081344UL
//свободно 63096 байт формула рассчета: AT_45_SIZE_1MB - ((OFFSET_PRG_DEST)+((PRG_SZ) * (MAX_PRG)) + ((SUBPRG_SZ) * (MAX_SUBPRG)))
//=============================

class CFlash : public CSpiMaster
{
 enum AT45_OPCODE{
	 READ_RANDOM_FLASH=0xd2,
	 TRANSFER_DATA_TO_BUF1=0x84,
	 READ_BUF1=0xd1,
	 ERASE_AND_WRITE_BUF1_TO_FLASH=0x83,
	 STAT_REG=0xd7,	 
	 BLOCK_ERASE=0x50,
	 };
public:
  
    bool Read(unsigned char *bptr,unsigned char *ptr,int size);
	bool Transfer_data_to_buffer1(unsigned char *ptr);
//20.12.2018
	bool write_datas_to_flash_from_buffer1_using_page_num(unsigned char *ptr,unsigned int page_num);
//------------------------------------	
	bool Read_buf1(unsigned char *n_);
	bool Builtin_erase_and_write_buffer1_to_page(unsigned char *bptr);
	bool WaitStatus(void);
    bool EPE_bit_and_compare_page_and_buf(bool *check_compare,bool *check_EPE);
	void write_counter_to_one_page(void);
	void read_datas_from_myflash(void);
	enum FLASH_fifo Erase_block(unsigned char *);
	bool read_datas_from_flash_using_page_num(unsigned char *ptr,unsigned int page_num,int s);
	bool read_header_from_flash(unsigned int plus,unsigned int mines,unsigned char *header_plus,unsigned char *header_mines);
	bool read_table_from_flash_using_random_address(long address_counter,long *data,unsigned int flash_page);
    unsigned char buf_addr[3],buf_datas[264];
	long debug_counter;
    int checker;

	//===new
	bool check_spi_at45();
	void begin_spi_at45();
	void end_spi_at45();
	uint8_t transfer(uint8_t data);
	void send_byte_spi(const uint8_t &data);
	uint8_t receive_byte_spi();
	void Read_flash_at45(uint32_t address_flash,uint8_t *buf,int sz);
	void Write_flash_at45(uint32_t address_flash,uint8_t *buf,int sz);
	bool is_ready_at45();
	uint32_t address_flash_test;
	//==

};
extern CFlash myflash;
#endif
