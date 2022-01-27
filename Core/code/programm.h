#ifndef _programm_h
#define _programm_h




#include "enum_struct.h"
#include "at45.h"
#include "main.h"
#define FLASH_TIMEOUT 6000
#define EEPROM_PAGESIZE 32
#include "counter.h"
#include "display.h"
#include "translate.h"
#include "io_altera.h"
#include "config.h"

#define YESNO_P_bm   0x80 // 1 - нет  ,0 - есть 
#define INVERSE_P_bm 0x40 // 1 - нет инверсии  ,0 - есть инверсия
#define P_OR_m_bm    0x20 // 1 - Пульт ,0 - Контроллер
#define DATA_P_bm    0x1F // максимальный номер в параметре 31 (32)
#define IN_P  1
#define OUT_P 0
#define NO_P 2
#define LV_P 3
#define HV_P 4
/*! \brief Non-Volatile Memory Execute Command
 *
 *  This macro set the CCP register before setting the CMDEX bit in the
 *  NVM.CTRLA register.
 *
 *  \note The CMDEX bit must be set within 4 clock cycles after setting the
 *        protection byte in the CCP register.
 */
#ifdef __AVR_ATxmega256A3U__
#define NVM_EXEC()	asm("push r30"      "\n\t"	\
						"push r31"      "\n\t"	\
						"push r16"      "\n\t"	\
						"push r18"      "\n\t"	\
						"ldi r30, 0xCB" "\n\t"	\
						"ldi r31, 0x01" "\n\t"	\
						"ldi r16, 0xD8" "\n\t"	\
						"ldi r18, 0x01" "\n\t"	\
						"out 0x34, r16" "\n\t"	\
						"st Z, r18"	    "\n\t"	\
						"pop r18"       "\n\t"	\
						"pop r16"       "\n\t"	\
						"pop r31"       "\n\t"	\
						"pop r30"       "\n\t"	\
)
#else
#define NVM_EXEC()
#endif
//#include "MenuLir.h"
// derived class :base class
class CPrg : public CFlash
{
    enum Flash_macro{
        START_ADRESS_FOR_PROGRAMM = 0,
        START_ADRESS_FOR_FUNCTION = 3200,
        START_ADRESS_FOR_CORRECTION= 3850,
        END_FLASH=4096,
         };
public:
//31.12.2018
   long k_divider_plus,set_point_plus;
   bool table_is_ready_plus; 
   long k_divider_mines,set_point_mines;
   bool table_is_ready_mines;  
//03.12.2018
    bool  to_restore_counters(void);
//------------------------------------	
	long debug_;
//21.08.2018
    bool mode_graph,clear_place_for_graph,start_point_for_graph,change,sign_for_graph;
	bool go_to_zero_ch_3;
	int resolution;
	long compare = 100;
	void graph(long y,long x);
	long y_start,x_start,y_,x_,header; 
	int inner_step,multiplicator;
	long y_graph_mem;
	long delta_gr,delta_10ms;
	long step_10;
	void upload_flash_to_ram();	
//------------------------------------
//--------------------------------------
struct _AXIS_CFG{
//02.03.2018 sum of these fields must be 32 (4 bytes) (29.03.2018)+8=12	
	unsigned int type          : 3;  
	unsigned int format        : 5;   
	unsigned int r_d           : 1;
	unsigned int direction     : 1;
	unsigned int save          : 1;
	unsigned int encoder_fault : 1;
	unsigned int readout_speed : 1;
    unsigned int rezerv1	   : 4;
	unsigned int rezerv2	   : 4;
	unsigned int rezerv3	   : 1;
	unsigned int rezerv4	   : 1;
	unsigned int rezerv5	   : 1;
	unsigned int rezerv6	   : 1;	
    unsigned int is_correction : 1;
	unsigned int is_coded_ref  : 1;
    unsigned int interval_to_measure_speed: 2; 
	unsigned int rezerv7       : 3;
	long gap;
	char ref_pulse_zone;
	char extern_to_zero;
	short rezerv8;	
}axis_cfg[N_AXIS];

struct _AXIS_CFG2 {
	char bit_enc; //кол-во бит
	char HZ_enc;  //частота опроса
	short rezerv0;
	long rezerv1;
	long rezerv2;
	long rezerv3;
	long rezerv4;
	long rezerv5;
	long rezerv6;
	long rezerv7;
}axis_cfg2[N_AXIS];
#if (configUSE_NCshell == 1)
//== параметры дискретного задания скорости Allp_param
struct Allp_param {
	char start_G1; //4 bits
	char spd_G1[ZONES];

	char start_G0;//4 bits
	char spd_G0[ZONES];

	char start_PM;//4 bits
	char limit_PM;//4 bits
	char zone_PM;//4 bits
	char stop_PM;//4 bits

	char start_LUFT;//4 bits
	char rezerv;//4 bits
	short rezerv1;
	long rezerv2;
	long rezerv3;
	long rezerv4;
}_Allp_param[N_AXIS + N_SA];
//==
//==параметры настройки Управления осью
struct Allp_param2 {
	unsigned  long ref_dir : 2;  // 0 -,  1 + ,2 или 3 - выбор
	unsigned  long dovodka : 1;
	unsigned  long viborka_lufta : 1;
	unsigned  long VNE_DOPUSKA : 1;
	unsigned  long rezerv : 27;
	long dopusk;
	long limit_plus;
	long limit_minus;
	short delay_VIBOR_PUSK;
	short v_Pozition;
	long rezerv2;
	long rezerv3;
	long rezerv4;
}_Allp_param2[N_AXIS];
//==
//==параметры зоны торможения оси
struct Allp_param3 {
	long zone[ZONES];
	long rezerv;
	long rezerv1;
	long rezerv2;
}_Allp_param3[N_AXIS];
//==
//==группа параметров аналогового задания скорости c Allp_param4 по Allp_param6
struct Allp_param4 {
	long start_G1;
	long spdG1[ZONES];
}_Allp_param4[N_AXIS + N_SA];

struct Allp_param5 {
	long start_G0;
	long spdG0[ZONES];
}_Allp_param5[N_AXIS + N_SA];

struct Allp_param6 {
	long start_PM;
	long limit_PM;
	long zone_PM;
	long stop_PM;
	long start_LUFT;
}_Allp_param6[N_AXIS + N_SA];
//==
struct INAxis {
	char axis_on_brake;
	char vibor_axis;
	char otmena_vibor_axis; 
	char Pusk_plus;
	char Pusk_minus;
	char Tolchok_plus;
	char Tolchok_minus;
	char Ogranichitel_plus;
	char Ogranichitel_minus;
	char rezerv0;
	char rezerv1;
	char rezerv2;
	long rezerv3;
	long rezerv4;
	long rezerv5;
	long rezerv6;
	long rezerv7;
}_INAxis[N_AXIS + N_SA];

struct OUTAxis {
	char vibor_axis;
	char Pusk_plus;
	char Pusk_minus;
	char Pusk;
	char Smazka;
	char Out_A;
	char Out_B;
	char Out_C;
	char Out_D;
	int8_t typeZ; //тип задания: дискретное/аналоговое/modbus/частотное/импульсное
	int8_t time_lubr;
	int8_t rezerv2;
	long dist_lubr;
	long rezerv4;
	long rezerv5;
	long rezerv6;
	long rezerv7;
}_OUTAxis[N_AXIS + N_SA];

struct OUTAxis2 {
	int8_t AnalOut;
	int8_t Polar ;
	char rezerv0;
	char rezerv1;
	long MaxSpdG1;
	long MaxUG1;
	long MaxSpdG0;
	long MaxUG0;
	long rezerv2;
	long rezerv3;
	long rezerv4;
}_OUTAxis2[N_AXIS + N_SA];

struct OUTAxis3 {
	long rezerv0;
	long rezerv1;
	long rezerv2; 
	long rezerv3;
	long rezerv4;
	long rezerv5;
	long rezerv6;
	long rezerv7;
}_OUTAxis3[N_AXIS + N_SA];

struct INGen {
	char gotov_stanok;
	char yskorenniy_xod_G0;
	char stop_podacha;
	char smena_kadra;
	char pause;
	char start_prog;
	char vneshnee_ypravlenie; //внешний пульт
	char po_chasovoy_M3;
	char protiv_chasovoy_M4;
	char stop_M5;
	char tolchok_plus;
	char tolchok_minus;
	char shpindel_vrashaetsia;
	char instrum_zajat;
	short rezerv0;
	long rezerv1;
	long rezerv2;
	long rezerv3;
	long rezerv4;
}_INGen;

struct INGen2 {
	char M102;
	char M112;
	char M122;
	char M132;
	char M142;
	char M152;
	char M162;
	char M172;
	long rezerv0;
	long rezerv1;
	long rezerv2;
	long rezerv3;
	long rezerv4;
	long rezerv5;
}_INGen2;

struct OUTGen {
	char yskorenniy_xod_G0;
	char podacha_G1;
	char texnolog_stop_M0;
	char konec_program_M2;
	char rychnoy_rejim;
	char auto_rejim;
	char pozitionirovanie;
	char gotovnostbUCIPU;
	char po_chasovoy_M3;
	char protiv_chasovoy_M4;
	char stop_M5;
	char Tolchok;
	char M3_M4_cherez_M5 : 1;
	char rezerv0 : 7;//13
	char rezerv1;
	short pause_M5;
	long rezerv2;
	long rezerv3;
	long rezerv4;
	long rezerv5;
}_OUTGen;

struct OUTGen2 {
	char M100off_M101on;
	char M110off_M111on;
	char M120off_M121on;
	char M130off_M131on;
	char M140off_M141on;
	char M150off_M151on;
	char M160off_M161on;
	char M170off_M171on;
	long rezerv0;
	long rezerv1;
	long rezerv2;
	long rezerv3;
	long rezerv4;
	long rezerv5;
}_OUTGen2;

struct Allp_param7{
	long U_off_a[2];//смещение напряжения [0]-a1 , [1]-a2
	short min_percent_feed;
	short MAX_percent_feed;
	short MAX_code;
	short min_code;
	long korr_podach : 1; // цифровой 1, аналоговый 0
	long rezerv0 : 31;
	long rezerv1;
	long rezerv2;
	long rezerv3;
}_Allp_param7;
#endif

#if (configUSE_mcx514 == 1)
enum _PULSE_OUTPUT_TYPE{
	STEP_DIR = 7,
	Quad_A_B = 0,
};
struct _AXIS_CFG_NC{
	unsigned long datchik      : 2;
	unsigned long dir          : 1;
	unsigned long sig_type     : 3;
	unsigned long rezerv       : 26;
	long move_oborot;
	long imp_oborot;
	long MaxSpd;
	long t;
	long InitSpd;
	long rezerv1;
	long rezerv2;
}axis_cfg_nc[N_AXIS];

struct _AXIS_CFG_NC2{
	long hw_oborot_mm;
	long hw_oborot_imp;
	long F_amplifire;
	long rezerv2;
	long rezerv3;
	long rezerv4;
	long rezerv5;
	long rezerv6;
}axis_cfg_nc2[N_AXIS];
#endif

enum FORMAT{
	 LINEAR_3=0,            //0.000
	 LINEAR_2=1,            //0.00
	 LINEAR_1=2,            //0.0
	 CIRCLE_3=3,            //0.000°
	 CIRCLE_2=4,            //0.00°
	 CIRCLE_1=5,            //0.0°
	 LINEAR_4=6,            //0.0000
	 CIRCLE_4=7,            //0.0000°
	 CIRCLE_SECUNDO=8,      //0°00'00"
	 LINEAR_INCH_4=9,       //0.0000"
	 CIRCLE_MINUTO=10,      //0°00'
	 LINEAR_5 = 11,         //0.00000	
	 LINEAR_6 = 12,         //0.000000
	 LINEAR_7 = 13,         //0.0000000
	 LINEAR_8 = 14,         //0.00000000
	 CIRCLE_SECUNDO_1 = 15, //0°00'00.0"
	 FORMAT_END=16,
	};
enum TYPE{
	 LINEAR=1,
	 CIRCLE=0,
	};
enum R_D{
	RADIUS=1,
	DIAMETER=0,
};
enum DIRECTION{
	MINES=0,
	PLUS=1,
	};
enum SAVE{
	YES_SAVE=0,
	NO_SAVE=1,
	};
enum ENCODER_FAULT{
	YES_FAULT=0,
	NO_FAULT=1,
};
enum READOUT_SPEED{
	YES_SPEED=0,
	NO_SPEED=1,
};
enum INV{
	YES_INV=0,
	NO_INV=1,
};
enum EXIST{
	YES_EXIST=0,
	NO_EXIST=1,
	};
enum GAP{
	SAVE_GAP_TO_EEPROM=0,
	NO_SAVE_GAP_TO_EEPROM=1,
	};
enum RANGE{
	INTERVAL0_1=0,
	INTERVAL0_3=1,
	INTERVAL0_6=3,
	};	
enum CORR{
	YES=0,
	NO=1,
	};
enum CODED_M{
	YESM=0,
	NOM=1,
};
enum DATCHIK{
	END = 0,
	ENCODER = 1,	
	HANDWEEL = 2,
	NO_DAT = 3,
};	
enum TYPEZ:int8_t{
	DISCRET = -1,
	ANALOG = 0,
	IMPULSE = 1,  //MCX514
	MODBUS = 2,
};
enum ANALOUT:int8_t{
	NO_AOUT = -1,
	A1 = 0,
	A2 = 1,
};
enum POLART:int8_t{
	PLUS_pl = -1,
	MINUS_pl = 0,
	PL_MN_pl = 1,
	MN_PL_pl = 2,
};
//06.04.2018
void to_take_buttons_code_for_correction(char k_c);
void to_take_buttons_code_and_to_see_correction(char k_c);
//15.08.2018
void to_take_buttons_code_for_new_technical_task(char k_c);
//10.08.2018
void to_take_only_up_down_code_and_to_see_correction(char k_c);
//15.08.2018 new technical task is coming...
void new_technical_task(long counter,char place);
long counter_memory_0,counter_memory_1,counter_memory_2;
//-------------------------------------------------------------
void to_see_correction(void);
void show_channels_in_see_corr(int8_t c);
void WorkCorr_input_loop(void);
//01.06.2018
void to_do_correction(void);
void GetCorr_input_loop(void);
//06.06.2018
void to_read_five_from_23LCV1024(void);
//19.12.2018
void item_correction_inside_INPUT_PARAM(void);
/*! \brief Wait for any NVM access to finish, including EEPROM.
 *
 *  This function is blcoking and waits for any NVM access to finish,
 *  including EEPROM. Use this function before any EEPROM accesses,
 *  if you are not certain that any previous operations are finished yet,
 *  like an EEPROM write.
 */
void EEPROM_WaitForNVM( void )
{
	do {
		/* Block execution while waiting for the NVM to be ready. */
	} while ((NVM.STATUS & NVM_NVMBUSY_bm) == NVM_NVMBUSY_bm);
}
/*! \brief Erase entire EEPROM memory.
 *
 *  This function erases the entire EEPROM memory block to 0xFF.
 */
void EEPROM_EraseAll( void )
{
	/* Wait until NVM is not busy. */
	EEPROM_WaitForNVM();

	/* Issue EEPROM Erase All command. */
	NVM.CMD = NVM_CMD_ERASE_EEPROM_gc;
	NVM_EXEC();
}

void check_iar_eeprom(void );
//03.01.2018
//void to_do_correction_in_realtime_ch0(void);
//--------------------------
int to_see_corr;
bool t_save; // 1 - звездочка (таблица изменена или создана, надо сохранить), 0 - нет
//26.04.2018
bool table_is_used = 1;
bool to_take_table;
bool sync_corr_see;
//31.05.2018
enum UPLOAD_DOWNLOAD{
	UPLOAD_FLASH_0_TO_RAM=0,
	UPLOAD_FLASH_1_TO_RAM=1,
	UPLOAD_FLASH_2_TO_RAM=2,
	UPLOAD_FLASH_3_TO_RAM=3,
	DOWNLOAD_RAM_TO_FLASH_0=4,
	DOWNLOAD_RAM_TO_FLASH_1=5,
	DOWNLOAD_RAM_TO_FLASH_2=6,
	DOWNLOAD_RAM_TO_FLASH_3=7,
};
UPLOAD_DOWNLOAD up_or_down;
enum EXEC{
	FIRST=0,
	SECOND=1,
	THIRD=2,
	};
EXEC execute;
//20.12.2018
enum EXCHANGE{
			UPLOAD_FLASH_TO_RAM_PREPEARING,
			DOWNLOAD_RAM_TO_FLASH_PLUS_FIRST_PAGE,
			DOWNLOAD_RAM_TO_FLASH_PLUS,
			DOWNLOAD_RAM_TO_FLASH_MINES,
			PREPARE_FOR_TO_DOWNLOAD_RAM_TO_FLASH,
			DOWNLOAD_RAM_TO_FLASH_MINES_FIRST_PAGE,
			UPLOAD_FLASH_TO_RAM_PLUS,
			UPLOAD_FLASH_TO_RAM_MINES,
			
	       // TO_DO_TASK,  
	};
EXCHANGE exchange;
enum EXCHANGE_ERRORS{
	   OK,
	   WRITE_FIRST_TIME_PLUS,
	   READ_FROM_23LCV1024,
	   TIMEOUT_WHEN_WRITE_TO_FLASH,
	   TASK_IS_ENDED,
	   WRITE_FIRST_TIME_MINES,
	   TIMEOUT_WHEN_READ_FROM_FLASH_FIRST_TIME,
	   IS_NO_TABLE_PLUS_IN_FLASH,
	   GO_NEXT_TABLE_PLUS_IN_FLASH,
	   WRITE_TO_23LCV1024,
	   READ_FROM_45DB081B,
	   IS_NO_TABLE_MINES_IN_FLASH,
	   GO_NEXT_TABLE_MINES_IN_FLASH,
	
	};
EXCHANGE_ERRORS exchange_error,exchange_error_,read_flash,read_flash_mines;
//20.12.2018
EXCHANGE_ERRORS to_exchange_45DB081B_with_23LCV1024(void);

EXCHANGE_ERRORS UPLOAD_FLASH_TO_RAM_MINES_f();
EXCHANGE_ERRORS UPLOAD_FLASH_TO_RAM_PLUS_f();
EXCHANGE_ERRORS goNext_TAble_f(int8_t plus);
//04.01.2019
bool  to_do_correction_in_realtime_ch(const int &ch);
long to_see_working_corr[N_AXIS];
//long to_see_rising_edge,to_see_res,to_see_interval,to_see_delta_0,to_see_delta_1;
//long to_see_delta_1_before,to_see_coeff;
//------------------------------------------
CIOAltera::ERROR_CORR_REAL_TIME to_do_correction_common_in_realtime(long *abs,long *delta_corr,unsigned char *header_plus,
unsigned char *header_mines,unsigned int start_page_plus,unsigned int start_page_mines,uint8_t ch);
unsigned int flash_start,pages_counter,flash_start_mines;
int block_plus,block_mines,timeout_for_flash;
unsigned char plus[264],mines[264];//массив для буфера считывания с флэшь 

bool read_page_from_23LCV1024,stop_flashing,to_stop_if_key_C_is_pressed;
bool error_when_work_with_RAM_or_FLASH;
//---------------------------------
int timeout_to_go;
bool correction_must_be_done;
int time_to_flash;	
//----------------------------------	
 int ch;  // текущий выбранный канал для редактирования параметров
					
 void only_for_debug(void);
 void init_to_default_state_axis_cfg(int num);
 void to_copy_axis_config(int destination,int sourse);     
 bool axis_is_chosen(uint16_t *P20,uint16_t *P21,uint16_t *P22,uint16_t *P23);
 bool axis_is_chosen_to_screen(uint16_t *Str1,uint16_t *Str2,uint16_t *Str3,uint16_t *Str4);
 bool to_write_to_eeprom_axis_is_chosen(uint16_t *P20,uint16_t *P21,uint16_t *P22,uint16_t *P23);
 bool to_write_to_eeprom_axis_to_screen(uint16_t *Str1,uint16_t *Str2,uint16_t *Str3,uint16_t *Str4);
 bool reset_eeprom_param(void); 
 bool reset_eeprom_and_ram_options(void);   
 unsigned char read_one_byte_eeprom_address_page(unsigned char addr_page,unsigned char addr_byte);
 unsigned char read_one_byte_eeprom_address_int(unsigned int addr,bool *error);
 bool write_block_to_eeprom_address_int(unsigned int addr_start,unsigned char *buf,unsigned char size);
 void EEPROM_WriteByte( uint8_t pageAddr, uint8_t byteAddr, uint8_t value );
 void EEPROM_FlushBuffer( void );
 void change_format_axis(int *num, char right_left);
 void to_save_type_of_axis(uint16_t *P20,uint16_t *P21,uint16_t *P22,uint16_t *P23);
 void change_mode_axis(int *num);
 void change_direction_axis(int *num);
 void change_save_axis(int *num);
 void change_encoder_fault_axis(int *num);
 void change_readout_speed_axis(int *num);
 //05.03.2018


 //--------------------------------
 void changeP(char& P,const signed char type, char dir,bool no_INV = 0);
 void show_P(char P,char type, int x, int y, char align);
 void change_inv_P(char& P);
 void change_MP(char& P,char type);
 //--------------------------------
 void read_eeprom(int addres,int sz,void * data);
 void write_eeprom(int addres, int sz, void * data);
 void init_eeprom_var();
 void init_spd_bit_fields();
 void init_offsets();


 bool to_copy_axis_param_from_EEPROM_to_RAM(void);
 bool to_save_axis_param_to_EEPROM(int channel,unsigned char *tmp );
 bool to_save_speed_screen_param_to_EEPROM(uint8_t *param);
 bool to_save_contrast_to_EEPROM(unsigned char *param);
 bool to_save_Bdrate_toEEPROM(unsigned char *param);
 bool to_save_Rejim_zaxvata_ref_metok_toEEPROM(bool *param);
 bool to_save_sound(); 
 bool to_save_night_mode_to_EEPROM(unsigned char *param); // 180 - b4

 bool to_save_primary_axis_lh_to_EEPROM(unsigned char *param);   //181
 bool to_save_secondary_axis_lh_to_EEPROM(unsigned char *param); //182
 bool to_save_primary_axis_ch_to_EEPROM(unsigned char *param);   //183
 bool to_save_secondary_axis_ch_to_EEPROM(unsigned char *param); //184

 bool to_save_to_see_time_to_EEPROM(bool *param);
 bool read_channel_0_from_EEPROM(unsigned char *tmp);
 bool read_channel_1_from_EEPROM(unsigned char *tmp);
 bool read_channel_2_from_EEPROM(unsigned char *tmp);
 bool read_channel_3_from_EEPROM(unsigned char *tmp);
 //01.03.2018
 bool to_save_coeff_to_EEPROM(int channel,unsigned char *tmp );
 //28.03.2018
 bool to_save_luft_to_EEPROM(int channel,unsigned char *tmp );
 void to_save_luft_to_RAM(int channel,long gap);
 bool to_copy_luft_from_EEPROM_to_RAM(void);
 //18.09.2018
 bool to_copy_G53_and_G5x_from_EEPROM_to_RAM(void);
 bool to_copy_G53_and_G5x_from_RAM_to_EEPROM(void);
 bool G53_and_G5x_to_clear(void);
 //25.09.2018
 void to_check_G53_and_G5x_when_system_is_started(void);
 bool to_copy_speed_on_screen_param_to_RAM();
 bool to_copy_contrast_to_RAM();
 bool to_copy_to_see_time_toRAM();
 bool to_copy_Bdrate_toRAM();
 bool to_copy_Rejim_zaxvata_ref_metki_toRAM();
 bool to_copy_night_mode_toRAM();// 180 address eeprom
 bool to_copy_axis_for_CH_LH(); // 181 , 182, 183 , 184 address eeprom
 bool to_copy_sound(); // 185 address eeprom last
 bool to_copy_Cntrl_cfg_from_eeprom_mcx514(); //186 - 218
 bool to_check_offsets(int channel,long *data);	
 //------------------------------------------------
 bool read_luft_from_EEPROM(unsigned char *tmp_0,unsigned char *tmp_1,
                            unsigned char *tmp_2,unsigned char *tmp_3);
 bool check_luft(int channel,GAP save);
 //02.04.2018
 bool luft_limit_after_input(int channel,long *data);							
 //---------------------------------------------
 bool to_copy_coeff_from_EEPROM_to_RAM(void);
 bool read_coeff_from_EEPROM(unsigned char *tmp_0,unsigned char *tmp_1,
                             unsigned char *tmp_2,unsigned char *tmp_3);
 bool check_coeff(int num);
 //29.10.2018
 void to_set_var_for_angle_axis(void);	
 void to_set_max_for_angel_axis(uint8_t ch,long bin,char n_a_d);
 void load_coeff_to_page_buf(unsigned char *buf,int ch);						 
 //------------------------------ 
 long debug; 
 bool write_to_eeprom_ok;  
    enum FLASH_fifo full_chip_ERASE(void);
    enum FLASH_fifo is_at45DB081B_ready(void);
//24.12.2018	
void to_take_task_for_exchange(void){	//поставитьCORR_POINTS
	if(up_or_down==UPLOAD_FLASH_0_TO_RAM){
		flash_start = CH0_CORR;//0;
		flash_start_mines = CH0_CORR + SZ_CH_CORR_HALF_p;//229;
		exchange=EXCHANGE::UPLOAD_FLASH_TO_RAM_PREPEARING;
		}
	else if(up_or_down==UPLOAD_FLASH_1_TO_RAM){
		flash_start=CH1_CORR;//458;
		flash_start_mines=CH1_CORR + SZ_CH_CORR_HALF_p;//687;
		exchange=EXCHANGE::UPLOAD_FLASH_TO_RAM_PREPEARING;
		}
	else if(up_or_down==UPLOAD_FLASH_2_TO_RAM){
		flash_start=CH2_CORR;//916;
		flash_start_mines=CH2_CORR + SZ_CH_CORR_HALF_p;//1145;
		exchange=EXCHANGE::UPLOAD_FLASH_TO_RAM_PREPEARING;
		}
	else if(up_or_down==UPLOAD_FLASH_3_TO_RAM){
		flash_start=CH3_CORR;//1374;
		flash_start_mines=CH3_CORR + SZ_CH_CORR_HALF_p;//1603;
		exchange=EXCHANGE::UPLOAD_FLASH_TO_RAM_PREPEARING;
		}
	else if(up_or_down==DOWNLOAD_RAM_TO_FLASH_0){
		flash_start=CH0_CORR;//0;
		flash_start_mines=CH0_CORR + SZ_CH_CORR_HALF_p;//229;
		exchange=EXCHANGE::PREPARE_FOR_TO_DOWNLOAD_RAM_TO_FLASH;
		}
	else if(up_or_down==DOWNLOAD_RAM_TO_FLASH_1){
		flash_start=CH1_CORR;//458;
		flash_start_mines=CH1_CORR + SZ_CH_CORR_HALF_p;//687;
		exchange=EXCHANGE::PREPARE_FOR_TO_DOWNLOAD_RAM_TO_FLASH;
		}
	else if(up_or_down==DOWNLOAD_RAM_TO_FLASH_2){
		flash_start=CH2_CORR;//916
		flash_start_mines=CH2_CORR + SZ_CH_CORR_HALF_p;//1145;
		exchange=EXCHANGE::PREPARE_FOR_TO_DOWNLOAD_RAM_TO_FLASH;
		}
	else if(up_or_down==DOWNLOAD_RAM_TO_FLASH_3){
		flash_start=CH3_CORR;//1374;
		flash_start_mines=CH3_CORR + SZ_CH_CORR_HALF_p;//1603;
		exchange=EXCHANGE::PREPARE_FOR_TO_DOWNLOAD_RAM_TO_FLASH;
	}		
}
//31.12.2018
/*
функция для конвертирования заголовка таблицы , массив байтов-> в переменные
unsigned char *incoming - считанные байты данных
bool plus - таблица + или -

добавить проверку на кол - во точек делитель коэффициент
*/
EXCHANGE_ERRORS to_convert_header_bytes_from_FLASH_to_int(unsigned char *incoming,bool plus){
unsigned char *byte_pointer;
long data=0;
unsigned char n,m;	
	
if (plus==true){
	if (incoming[0]==1){
		byte_pointer=(unsigned char *)&data;
		for(n=1,m=0;n<5;n++) // 4 байта
			byte_pointer[m++]=incoming[n];	
		k_divider_plus=data;
		byte_pointer=(unsigned char *)&data;
		for(n=5,m=0;n<9;n++) 
			byte_pointer[m++]=incoming[n];
		set_point_plus=data;
		if((k_divider_plus<=MAX_INTERVAL_PLUS && k_divider_plus>0) && (set_point_plus>0 && set_point_plus<=CORR_POINTS))
			return EXCHANGE_ERRORS::GO_NEXT_TABLE_PLUS_IN_FLASH;	 	
	}	
	k_divider_plus=0;
	table_is_ready_plus=false;
	set_point_plus=0;
	return 	 EXCHANGE_ERRORS::IS_NO_TABLE_PLUS_IN_FLASH;
	}
else{	
		if (incoming[0]==1){
			byte_pointer=(unsigned char *)&data;
			for(n=1,m=0;n<5;n++) byte_pointer[m++]=incoming[n];
			k_divider_mines=data;
			byte_pointer=(unsigned char *)&data;
			for(n=5,m=0;n<9;n++) byte_pointer[m++]=incoming[n];
			set_point_mines=data;
			if((k_divider_mines>=MAX_INTERVAL_MINUS && k_divider_mines<0)
			&& (set_point_mines>0 && set_point_mines<=CORR_POINTS))
			{
				return EXCHANGE_ERRORS::GO_NEXT_TABLE_MINES_IN_FLASH;
			}	
		}
		k_divider_mines=0;
		table_is_ready_mines=false;
		set_point_mines=0;
		return 	 EXCHANGE_ERRORS::IS_NO_TABLE_MINES_IN_FLASH;	
    }
return 	 EXCHANGE_ERRORS::OK;
}
//--------------------------------------------------------------------
/*
коэффициент сохраняется в заголовке таблицы с индекса 9
*/
bool get_bin_n_after_dot_from_header(uint8_t ch, bool plus,unsigned char * header_plus,unsigned char * header_minus){

	long coeff_ =  1;
	char n_after_d = 0;
	if(plus){
		//if(header_plus[14]){   // если таблица снята в авто режиме то умножаем на коэфффициент эталона
			coeff_ = *(long *)&header_plus[9];
			n_after_d = header_plus[13];
	//	}
	}
	else
	{
	  // if(header_minus[14]){ // если таблица снята в авто режиме то умножаем на коэфффициент эталона
			coeff_ = *(long *)&header_minus[9];	
			n_after_d = header_minus[13];
	 //  }
	}

	if(coeff_ == 0){
		coeff_ = 1;
		n_after_d = 0;
		}

	coeff.coeff_corr[ch].binary = coeff_;
	coeff.coeff_corr[ch].n_after_dot = n_after_d;

	//==проверка табличного коэффициента и осевого
	if(coeff.coeff_corr[ch] == coeff.new_coeff_[ch])
	   return false;
	else
		return true;

}
//06.06.2018 test for 23LCV1024
//long corr_plus[5];
bool test_23LCV1024(void){
  //spi.debug_flash=myflash.WaitStatus();
  //init.set_B6();
  spi.debug_23LCV1024=spi.write_start_sequence_for_23LCV1024();
  //init.clr_B6();
  if(spi.debug_23LCV1024==3){
	  return true;
	  spi.save_table_to_23LCV1024(spi.counter_debug,display.speed,spi.sign);
	  spi.read_table_from_23LCV1024(spi.counter_debug,&spi.to_see_counter,spi.sign);
	  spi.counter_debug++;
	  if(spi.counter_debug>=10000){spi.counter_debug=0;
		  if(spi.sign==false)spi.sign=true;
		  else spi.sign=false;
	  }
	  if(display.speed!=spi.to_see_counter)spi.debug_flash=true;  
  }	
  return false;	
}



/*
сброс ошибок
return 1 - сброс произошел , 0 - нет
*/
bool reset_axis_errors(void){
	bool reset = 0;
	cli();
	for(int c = 0 ; c< N_AXIS;c++){
		if(mCounter[c].error){
			mCounter[c].error = false;
			reset = 1;
		}
		if(mCounter[c].ovfl_abs){
			mCounter[c].ovfl_abs = false;
			reset = 1;
		}
		if(mCounter[c].ovfl_rel){
			mCounter[c].ovfl_rel = false;
			reset = 1;
		}
	}
	sei();
	return reset;
}
bool to_check_axis_errors(unsigned char v){
	cli();
	if (mCounter[v].error==false)
		return false;
	else 
		return true;
	sei();
	return true;
}
void to_set_axis_errors(unsigned char v){
	cli();
		mCounter[v].error=true;
		mCounter[v].rel=mCounter[v].abs_pure=0;
	sei();
}
bool is_encoder_fault(unsigned char v){
	if(v >= N_AXIS)
		return false;	
   if(display.encoder_fault[v] && axis_cfg[v].encoder_fault == ENCODER_FAULT::YES_FAULT){
		#if (configUSE_mcx514 == 1)
		if(_OUTAxis[v].typeZ == CPrg::IMPULSE && (axis_cfg_nc[v].datchik == CPrg::NO_DAT || axis_cfg_nc[v].datchik == CPrg::HANDWEEL))
			return false;
		#endif
		return true;
		}
   return false;	
}
void to_check_angle_axis(long *counter,unsigned char v){
// CIRCLE_3=3,//0.000
// CIRCLE_2=4,//0.00
// CIRCLE_1=5,//0.0
// CIRCLE_4=7,//0.0000
// CIRCLE_SECUNDO=8,
// CIRCLE_MINUTO=10,	
	switch(axis_cfg[v].format){
	case FORMAT::CIRCLE_3:*counter=*counter+360000;break;
	case FORMAT::CIRCLE_2:*counter=*counter+36000;break;
	case FORMAT::CIRCLE_1:*counter=*counter+3600;break;
	case FORMAT::CIRCLE_4:*counter=*counter+3600000;break;
	case FORMAT::CIRCLE_SECUNDO_1:*counter=*counter+12960000;break;
	case FORMAT::CIRCLE_SECUNDO:*counter=*counter+1296000;break;
	case FORMAT::CIRCLE_MINUTO:*counter=*counter+21600;break;
	default:return;	
	}
}
void to_check_angle_axis_inside_HAND(long *counter,unsigned char n_ch,bool inverse);
void to_format_angle_axis_inside_HAND(long *counter,unsigned char n_ch){
		switch(axis_cfg[n_ch].format){
			case FORMAT::CIRCLE_3:*counter %= (long)360000;break;
			case FORMAT::CIRCLE_2:*counter %= (long)36000;break;
			case FORMAT::CIRCLE_1:*counter %= (long)3600;break;
			case FORMAT::CIRCLE_4:*counter %= (long)3600000;break;
			case FORMAT::CIRCLE_SECUNDO_1:*counter %= (long)12960000;break;
			case FORMAT::CIRCLE_SECUNDO:*counter %= (long)1296000;break;
			case FORMAT::CIRCLE_MINUTO:*counter %= (long)21600;break;
			default:return;
	}
}
void endless_circle(long *counter,unsigned char c);


void divide_by_2_counter_rel(unsigned char n_ch);
void mult_by_2_counter_rel(unsigned char n_ch);

void to_take_counters_to_the_structure(void){

	for(int c = 0; c < N_AXIS;c++){
		l_k_s.r_t_save_rel.rel[c] = mCounter[c].rel;
		l_k_s.r_t_save_abs.abs[c] = mCounter[c].abs_pure;
		l_k_s.r_t_save_ref.ref[c] = ref_.pRef[c];
	}
	l_k_s.r_t_save_ref.is_it_saved_by_finger_press = true;	
}

//==========================

//04.09.2013 end of class Cprg
};
//============================

    extern CPrg flash_prg;
//    extern CDisp display;

#endif

