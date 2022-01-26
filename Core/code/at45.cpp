
#include "main.h"
#include "enum_struct.h"
#include "at45.h"
//18.01.2018----------------------------------
bool CFlash::Read(unsigned char *bptr,unsigned char *ptr,int size){
//--19.01.2018 it takes 10834*31.25=338 563 ns if 264 bytes are read  
// page is 264 bytes, page's range is 0-4095
//address is 3 reserve bits + 12(page's address) + 9 (byte's first address)
//3 bytes must be  bptr[2]=0,0,0,PA11,PA10,PA9,PA8,PA7;
//                 bptr[1]=PA6,PA5,PA4,PA3,PA2,PA1,PA0,BFA8;
//                 bptr[0]=BFA7,BFA6,BFA5,BFA4,BFA3,BFA2,BFA1,BFA0;
//	0 page 0 first byte  bptr[2]=0, bptr[1]=0, bptr[0]=0
//  0 page 256 first byte bptr[2]=0, bptr[1]=0x01, bptr[0]=0
//  1 page 256 first byte bptr[2]=0, bptr[1]=0x03, bptr[0]=0
	register unsigned char n;
	int i;
//	unsigned char spic_data=0xd2;
    unsigned char spic_data=CFlash::AT45_OPCODE::READ_RANDOM_FLASH;
	//  register unsigned char maska,k,l,j;
	//---------------------------------------
	cli();
	cs_flash_low();
	//------------operation
	//READ_MEMORY is main memory page read
	//
	if (common_hardware_spi(spic_data,bptr)==false) goto timeout_error;
	//--32 don't care bit--blank data SPIC.DATA =0xff; it starts tr
	
	SPIC.DATA=0xff ;
//19.01.2018 timeout for 20 is about 4.5 mks	
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	//------------blank data SPIC.DATA=0xff ; it starts tr
	if (n>=20) goto timeout_error;
	SPIC.DATA=0xff;
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	//------------blank data  SPDR=0xff; it starts tr
	//           if (n>=20) goto timeout_error;
	SPIC.DATA=0xff;
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	//------------blank data  SPDR=0xff; it starts reciever
	if (n>=20) goto timeout_error;
	SPIC.DATA=0xff;
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	//-----------------------------------------------------
	for(i = 0; i < size; i++){
		SPIC.DATA = 0xff;
		for(n=0;n<20;n++) {
			// 1******* if serial transfer is complete
			if (SPIC.STATUS & 0x80)break;
		}
		ptr[i]= SPIC.DATA;
		if (n>=20) goto timeout_error;
	}
	cs_flash_high();
	sei();


	return true;
	timeout_error: cs_flash_high();
	sei();
	return false;
}
bool  CFlash::Transfer_data_to_buffer1(unsigned char *ptr)
{
	unsigned char n;
	unsigned int i=0;
	unsigned char buf_adr[3];
	//unsigned char spic_data=0x84;
	unsigned char spic_data=CFlash::AT45_OPCODE::TRANSFER_DATA_TO_BUF1;
	buf_adr[2]=0;
	buf_adr[1]=0;
	buf_adr[0]=0;
	//cli();
	cs_flash_low();
	//common_hardware_spi(spic_data,buf_adr);
	if (common_hardware_spi(spic_data,buf_adr)==false) goto timeout_error;
	for(; i < 264; i++){
		SPIC.DATA=ptr[i];
		for(n=0;n<20;n++) {
			// 1******* if serial transfer is complete
			if (SPIC.STATUS & 0x80)break;
		}
		if (n>=20)break;
	}
timeout_error:	cs_flash_high();
	//sei();
	if (i==264) return true;

	 return false;

}
bool  CFlash::Read_buf1(unsigned char *n_){
	unsigned char n;
	int i=0;
	unsigned char buf_adr[3];
	//unsigned char spic_data=0xd1;
	unsigned char spic_data=CFlash::AT45_OPCODE::READ_BUF1;
	buf_adr[2]=0;
	buf_adr[1]=0;
	buf_adr[0]=0;
	cli();
	cs_flash_low();
	//common_hardware_spi(spic_data,buf_adr);
	if (common_hardware_spi(spic_data,buf_adr)==false) goto timeout_error;
	for(; i < 264; i++){
		SPIC.DATA = 0xff;
		for(n=0;n<20;n++) {
			// 1******* if serial transfer is complete
			if (SPIC.STATUS & 0x80)break;
		}
		n_[i]= SPIC.DATA;
		if (n>=20)break;
	}
timeout_error:	cs_flash_high();
	sei();
	if (i==264) return true;
	return false;
}
bool  CFlash::Builtin_erase_and_write_buffer1_to_page(unsigned char *bptr)
{
// page is 264 bytes, page's range is 0-4095
//address is 3 reserve bits + 12(page's address) + 9 (byte's first address)
//3 bytes must be  bptr[2]=0,0,0,PA11,PA10,PA9,PA8,PA7;
//                 bptr[1]=PA6,PA5,PA4,PA3,PA2,PA1,PA0,BFA8;
//                 bptr[0]=BFA7,BFA6,BFA5,BFA4,BFA3,BFA2,BFA1,BFA0;
//	0 page 0 first byte  bptr[2]=0, bptr[1]=0, bptr[0]=0
//  0 page 256 first byte bptr[2]=0, bptr[1]=0x01, bptr[0]=0
//  1 page 256 first byte bptr[2]=0, bptr[1]=0x03, bptr[0]=0	
// 4095 page 0 first byte 0	byte bptr[2]=0x1f, bptr[1]=0xfe,bptr[0]=0
	//unsigned char spic_data=0x83;
	unsigned char spic_data=CFlash::AT45_OPCODE::ERASE_AND_WRITE_BUF1_TO_FLASH;
	bool answer;

	//cli();
	cs_flash_low();
	//------------operation
	// The buffer to main memory page program with built-in erase command
	// allows data that is stored in one of the SRAM buffers to be written into
	// an erased or programmed page in the main memory array. It is not necessary
	// to pre-erase the page in main memory.
	answer=common_hardware_spi(spic_data,bptr);
	cs_flash_high();
	//sei();
	return answer;
}
bool CFlash::WaitStatus(void){
//19.01.2018 it takes 138 * 31.25=4.3125 mks if no flash on line
//it takes 115*31.25=3.59375 mks if flash is on line  
   bool result = false;
   unsigned char res,store,m;
   res=0x00;
//it takes 32*31.25=1 mks   
   init_spi_outer_flash();
//----------------------------   
cli();
     cs_flash_low(); 
//------------------------     
     SPIC.DATA = CFlash::AT45_OPCODE::STAT_REG;
//it is sending with timeout 37*31.25 = 1.15625 mks  
// clock for data's interchange is 16 Mgh (8)
// time interval for one bit is 62.5(125) ns for byte is 0.5(1) mks     
       for (m=0;m<5;m++){
// 1******* if serial transfer is complete 
            if (SPIC.STATUS & 0x80)break; 			
				}
//it is answer's waiting with timeout 37*31.25 = 1.15625 mks  
//       if (m<=4) {
           SPIC.DATA  = 0xff; 
       for (unsigned char g=0;g<5;g++){        
           // 1******* if serial transfer is complete 
            if (SPIC.STATUS & 0x80){res= SPIC.DATA ;break; }	
                          }
//       } 
//01.04.2016 only debug
//      status_to_see=res; 
            
  cs_flash_high();
sei();
// status byte  from  45db041b
// bit 7        bit 6  bit 5  bit 4 bit 3 bit 2 bit 1 bit 0
// ready/`busy  comp       0      1     1     x     x     x
//       store = res;
//       store = store>>3;
//       store = store & 0x07;
//       if (store==0x03) {
//        res=res&0x80;
//        if (res == 0x80){ result=true;}
//       }
// status byte  from  45db082e
// bit 7        bit 6  bit 5  bit 4 bit 3 bit 2             bit 1      bit 0
// ready/`busy  comp       1      0     0     1   sec.prot dis/en    264/256     
          store = res;
          store = store>>2;
          store = store & 0x0f;
          if (store==0x09) {
            res=res&0x81;
          if (res == 0x80){ result=true;}
          }
//only for debug
//result=false;
   return result;
}
bool CFlash::EPE_bit_and_compare_page_and_buf(bool *check_compare,bool *check_EPE){
    unsigned char first,second,m;
	first=second=0;
cli();
//it takes 19*31.25=0.6 mks
     cs_flash_low(); 
//------------------------     
     SPIC.DATA = STAT_REG;
//it is sending with timeout 345 ns * 5 = 1.725 mks  
// clock for data's interchange is 16 Mgh (8)
// time interval for one bit is 62.5(125) ns for byte is 0.5(1) mks     
       for (m=0;m<5;m++){
// 1******* if serial transfer is complete 
            if (SPIC.STATUS & 0x80)break; 			
				}
//it is answer's waiting with timeout 345 ns * 5 = 1.725 mks  
//       if (m<=4) {
           SPIC.DATA  = 0xff; 
       for (unsigned char g=0;g<5;g++){        
           // 1******* if serial transfer is complete 
            if (SPIC.STATUS & 0x80){
				first= SPIC.DATA ;
				break; 
			}	
        }
           SPIC.DATA  = 0xff; 
       for (unsigned char g=0;g<5;g++){        
           // 1******* if serial transfer is complete 
            if (SPIC.STATUS & 0x80){second= SPIC.DATA ;break; }	
                          }
//       } 
//01.04.2016 only debug
//      status_to_see=res; 
            
  cs_flash_high();
sei();
 
       if (first&0x40) *check_compare=false;
       else *check_compare=true;
       if(second&0x20) *check_EPE=false;
       *check_EPE=true;

       if (first&0x80) return true;
       return false;
}
void CFlash::write_counter_to_one_page(void){
//19.01.2018---------------------
if (myflash.WaitStatus()==true){
	char ch=0;
	for (int n=0;n<264;n++)	myflash.buf_datas[n]=ch++;
	if(myflash.Transfer_data_to_buffer1(myflash.buf_datas)==true){
		myflash.buf_addr[2]=0;
		myflash.buf_addr[1]=0;
		myflash.buf_addr[0]=0;
		myflash.Builtin_erase_and_write_buffer1_to_page(myflash.buf_addr);
	}
}
}
bool CFlash::read_datas_from_flash_using_page_num(unsigned char *ptr,unsigned int page_num,int s){
//28.12.2018 it takes 330 mks from call to return for 264 bytes	
int i,n,size;	
myflash.buf_addr[0]=0;
page_num=page_num<<1;
page_num=page_num&0xfffe;
myflash.buf_addr[1]=(unsigned char)page_num;
page_num=page_num>>8;
myflash.buf_addr[2]=(unsigned char)page_num&0x1f;
size=s;
    unsigned char spic_data=CFlash::AT45_OPCODE::READ_RANDOM_FLASH;
   // cli();
    cs_flash_low();
    //------------operation
    //READ_MEMORY is main memory page read
    //
    if (common_hardware_spi(spic_data,myflash.buf_addr)==false) goto timeout_error;
    //--32 don't care bit--blank data SPIC.DATA =0xff; it starts tr
    
    SPIC.DATA=0xff ;
    //19.01.2018 timeout for 20 is about 4.5 mks
    for(n=0;n<20;n++) {
	    // 1******* if serial transfer is complete
	    if (SPIC.STATUS & 0x80)break;
    }
    //------------blank data SPIC.DATA=0xff ; it starts tr
    if (n>=20) goto timeout_error;
    SPIC.DATA=0xff;
    for(n=0;n<20;n++) {
	    // 1******* if serial transfer is complete
	    if (SPIC.STATUS & 0x80)break;
    }
    //------------blank data  SPDR=0xff; it starts tr
    //           if (n>=20) goto timeout_error;
    SPIC.DATA=0xff;
    for(n=0;n<20;n++) {
	    // 1******* if serial transfer is complete
	    if (SPIC.STATUS & 0x80)break;
    }
    //------------blank data  SPDR=0xff; it starts reciever
    if (n>=20) goto timeout_error;
    SPIC.DATA=0xff;
    for(n=0;n<20;n++) {
	    // 1******* if serial transfer is complete
	    if (SPIC.STATUS & 0x80)break;
    }
    //-----------------------------------------------------
    for(i = 0; i < size; i++){
	    SPIC.DATA = 0xff;
	    for(n=0;n<20;n++) {
		    // 1******* if serial transfer is complete
		    if (SPIC.STATUS & 0x80)break;
	    }
	    ptr[i]= SPIC.DATA;
	    if (n>=20) goto timeout_error;
    }
    cs_flash_high();
   // sei();


    return true;
    timeout_error: cs_flash_high();
    //sei();
    return false;	
}
//#pragma GCC push_options
//#pragma GCC optimize ("O0")
//09.01.2019
bool CFlash::read_table_from_flash_using_random_address(long address_counter,long *data,unsigned int flash_page){
//10.01.2019 
//flash_0 + 0-header, data 1-228,
//        - 229-header,data 230-457
//flash_1 + 458-header,data 459-686,
//        - 687-header,data 688-915 
//flash_2 + 916-header,data 917-1144,
//        - 1145-header,data 1146-1373
//flash_3 + 1374-header,data 1375-1602,
//        - 1603-header,data 1604-1831	
//from 1 till 228 is 228*264=60192/4=15048 long
//if address_counter is 0 and flash_page is 230
//real= address_counter*4+230*264=60720
//60720/264=230 page's number
// (0*4+230*264)-230*264=0 byte's number inside page
//if address_counter is 1 and flash_page is 230
//real= address_counter*4+230*264=60724
//60724/264=230 page's number
// (1*4+230*264)-230*264=4 byte's number inside page 
// page is 264 bytes, page's range is 0-4095
//address is 3 reserve bits + 12(page's address) + 9 (byte's first address)
//3 bytes must be  bptr[2]=0,0,0,PA11,PA10,PA9,PA8,PA7;
//                 bptr[1]=PA6,PA5,PA4,PA3,PA2,PA1,PA0,BFA8;
//                 bptr[0]=BFA7,BFA6,BFA5,BFA4,BFA3,BFA2,BFA1,BFA0;
//11.01.2018 
long real,check;
unsigned int page;
unsigned int byte;
unsigned char *byte_pointer;
//16.01.2019
long mul=(long)flash_page;
mul=mul*264;
//--it takes 830*31.25=25.937 mks
//--it takes 692*31.25=21.625 mks if size optimization
	real = address_counter*4 + mul;
	page=real/264;
	check=page*264;
	byte=real-check;
myflash.buf_addr[0]=(unsigned char)byte;
page=page<<1;
page=page&0xfffe;
myflash.buf_addr[1]=(unsigned char)page;
if(byte&0x0100)
myflash.buf_addr[1]=myflash.buf_addr[1]|0x01;
page=page>>8;		
myflash.buf_addr[2]=(unsigned char)page&0x1f;
//---------													   
//*data=0;
if(myflash.Read(myflash.buf_addr,myflash.buf_datas,4)==true){
	byte_pointer = (unsigned char *)data;
	for(unsigned char n=0;n<4;n++)
		byte_pointer[n] = myflash.buf_datas[n];
}
else 
	return false;														   
return true;														   														   
}

bool CFlash::check_spi_at45()
{
	uint8_t manf_id = 0;
	begin_spi_at45();
	manf_id = transfer(0x9F);
	end_spi_at45();

	if(manf_id == Manufacturer_ID_at45_adesto)
		return false;
	else
		return true;
}

void CFlash::begin_spi_at45()
{
	PORTC.OUTCLR = 0x10;
}

void CFlash::end_spi_at45()
{
	PORTC.OUTSET = 0x10;
}

uint8_t CFlash::transfer(uint8_t data)
{
	send_byte_spi(data);
	return receive_byte_spi();
}

void CFlash::send_byte_spi(const uint8_t &data)
{
	SPIC.DATA = data;
	while (!(SPIC.STATUS & SPI_IF_bm)) ; // wait
}

uint8_t CFlash::receive_byte_spi()
{
	SPIC.DATA = 0x00;
	while (!(SPIC.STATUS & SPI_IF_bm)) ; // wait
	return SPIC.DATA;
}

/*
функция для чтения страницы флешь памяти At45
in  uint32_t address_flash - адрес страницы
out uint8_t *buf - буфер
int sz - размер буфера
*/
void CFlash::Read_flash_at45(uint32_t address_flash,uint8_t *buf,int sz)
{
	//Addressing Sequence for Standard DataFlash Page Size (264 bytes) стр 43 
	address_flash_test = address_flash;
	address_flash = is_264PAGE_SZ(address_flash);
	cli();
	begin_spi_at45();
	send_byte_spi(Main_Memory_Page_Read_at45);
	send_byte_spi(address_flash >> 16);
	send_byte_spi(address_flash >> 8);
	send_byte_spi(address_flash);

	//==dummy bytes
	for(int i = 0; i < 4;i++)
	send_byte_spi(0x00);
	//==
	for(int j = 0; j <sz;j++)
	buf[j] = receive_byte_spi();

	end_spi_at45();
	sei();
}
/*
функция для записи данных во флешь память ,использует метод Read-modify-Write	(стр 12 описания)
inuint32_t address_flash - адрес страницы
in uint8_t *buf - буфер для записи в память
in int sz - размер буфера
*/
void CFlash::Write_flash_at45(uint32_t address_flash,uint8_t *buf,int sz)
{ 
	//Addressing Sequence for Standard DataFlash Page Size (264 bytes) стр 43 
	address_flash = is_264PAGE_SZ(address_flash);
	cli();
	//Figure 25-2. Algorithm for Randomly Modifying Data
	begin_spi_at45();
	send_byte_spi(PagetoBuf1);
	send_byte_spi(address_flash >> 16);
	send_byte_spi(address_flash >> 8);
	send_byte_spi(address_flash);
	end_spi_at45();
	while(!is_ready_at45());
	begin_spi_at45();
	send_byte_spi(PageProg1);
	send_byte_spi(address_flash >> 16);
	send_byte_spi(address_flash >> 8);
	send_byte_spi(address_flash);

	for(int j = 0; j <sz;j++)
		send_byte_spi(buf[j]);
	end_spi_at45();
	/*begin_spi_at45();
	send_byte_spi(RMW_at45);
	send_byte_spi(address_flash >> 16);
	send_byte_spi(address_flash >> 8);
	send_byte_spi(address_flash);

	for(int j = 0; j <sz;j++)
	send_byte_spi(buf[j]);

	end_spi_at45(); */
	sei();
	while(!is_ready_at45());
}
/*
проверка готовности чипа at45 Ready/Busy Status
*/
bool CFlash::is_ready_at45()
{
	bool res;
	cli();
	begin_spi_at45();
	if(transfer(STATUS_at45) & 0x80)
		res = true;
	else
		res = false;
	end_spi_at45();
	sei();
	return res;
}

//#pragma GCC pop_options


bool CFlash::write_datas_to_flash_from_buffer1_using_page_num(unsigned char *ptr,unsigned int page_num){
//26.12.2018 from call to return it takes 320 mks																  
// page is 264 bytes, page's range is 0-4095
//address is 3 reserve bits + 12(page's address) + 9 (byte's first address)
//3 bytes must be  bptr[2]=0,0,0,PA11,PA10,PA9,PA8,PA7;
//                 bptr[1]=PA6,PA5,PA4,PA3,PA2,PA1,PA0,BFA8;
//                 bptr[0]=BFA7,BFA6,BFA5,BFA4,BFA3,BFA2,BFA1,BFA0;																  
																  
//myflash.buf_addr[2]=0;
//myflash.buf_addr[1]=0;
myflash.buf_addr[0]=0;																  
page_num=page_num<<1;
page_num=page_num&0xfffe;																  
myflash.buf_addr[1]=(unsigned char)page_num;
page_num=page_num>>8;
myflash.buf_addr[2]=(unsigned char)page_num&0x1f;
																  
//if (myflash.WaitStatus()==true);																  
//else return false;																  
if(myflash.Transfer_data_to_buffer1(ptr)==true){
//26.12.2018 from call to this place it takes 320 mks (for 264 bytes)
// and it small part takes 6 mks till return	
	//init.clr_B6();																  
	myflash.Builtin_erase_and_write_buffer1_to_page(myflash.buf_addr);
	//init.set_B6();																	  																  
}
else return false;
return true;
}
void CFlash::read_datas_from_myflash(void){
	if (myflash.WaitStatus()==true){
		myflash.debug_counter++;
		display.print_debug_one_local(myflash.debug_counter,0,15,224);
		//init.set_B6();
		if(myflash.Read(myflash.buf_addr,myflash.buf_datas,264)==true)
		{
		//init.clr_B6();
	if(myflash.checker>=264)myflash.checker=0;
	display.print_debug_one_local(myflash.buf_datas[myflash.checker++],0,25,224);
		}
	}
}
enum FLASH_fifo CFlash::Erase_block(unsigned char *bptr)
{
	register unsigned char n;
	//  register unsigned char maska,k,l,j;
	//---------------------------------------
	cli();
	cs_flash_low();
	//------------operation
	//
	SPIC.DATA = BLOCK_ERASE;
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	//------------adress
	if (n>=20) goto timeout_error;
	SPIC.DATA= bptr[2];
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	//------------adress
	if (n>=20) goto timeout_error;
	SPIC.DATA = bptr[1];
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	//------------adress
	if (n>=20) goto timeout_error;
	SPIC.DATA = bptr[0];
	for(n=0;n<20;n++) {
		// 1******* if serial transfer is complete
		if (SPIC.STATUS & 0x80)break;
	}
	cs_flash_high();
	sei();
	return NO_FLASH_ERROR;
	timeout_error: cs_flash_high();
	sei();
	return TIMEOUT_ERASE_ERROR;
}
bool  CFlash::read_header_from_flash(unsigned int plus,unsigned int mines,unsigned char *header_plus,unsigned char *header_mines){
    if(!myflash.read_datas_from_flash_using_page_num(header_plus,plus,15))
		return false;
	if(!myflash.read_datas_from_flash_using_page_num(header_mines,mines,15))
		return false;
	
return true;	
}

