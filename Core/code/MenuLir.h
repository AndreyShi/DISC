/*
 * Menu.cpp
 *
 * Created: 24.11.2017 12:21:39
 *  Author: Andrew
 */ 

 //const unsigned char _txtnew[][16] PROGMEM=
 #ifndef MENULIR_H_
 #define MENULIR_H_
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stddef.h>
#include "display.h"
#include "Strings.h"
//23.01.2018 axis parameters are defined inside it...
#include "programm.h"
#include <string.h>
#include "config.h"
//#include "error_log.h"
#include "mcx514.h"
#include "at45.h"

#define menu Lir
#define sym result[1]
//-------------------
#define in_mem 0
#define on_display 1
#define null 0

#define idBegin              30
#define idHand               31
#define idAbout              33
#define idLinkPK             34
#define idResetParametrs     39
#define idRef                40
#define idMenuEncoders       46
#define idGetCorr            48
#define idSeeCorr            49
#define idIndikator          50
#define idGraph              51
#define idsaveoffset         54
#define idResetOffset        55
#define idpassG53            56
#define idTime               59
#define idSupplyandBatt      60
#define idViewInputs         61
#define idInpointRadiusInput 62
#define idCalc               63
#define idHand_input         64
#define idGetCorr_input      65
#define idWorkCorr_Input     66
#define idLog                67
#define idCalcfrom_menu      68
#define idInpointRadius      71
#define idInpointRadiusPUSK  72
#define idInpointLinear      73
#define idInpointLinearInput 74
#define idInpointLinearPUSK  75
#define idCmd                76
#define idExecmd             77
#define idCntrlopt           79
#define idInputCntrlOpt      80
#define idHandMove           81 
#define idIndicOpt           82
#define idInputIndicOpt      83
#define idHandWeelOpt        84
#define idInputHandWeelOpt   85 
#define idDiag               86
#define idDiagInput          87
#define idAllp               88
#define idPultOcb            89
#define idScreenOcb          90
#define idG0                 91
#define idG1                 92
#define	idOption             93
#define idOptionInput        94
#define idAxislist           95
#define idDebugMode          96
#define idCorr               97
#define idIzmer              98
#define idCoordOffset        99
#define idCoordOffsetInput    100
#define idAxisListOffset      101
#define idAxisListOffsetInput 102
#define idPrgOrSubPrg         103
#define idChpa                104
#define idSpdZ                105
#define idCntrl              106
#define idCntrlInput         107
#define idZoneBrake          108
#define idZoneBrakeInput     109
#define idVixodPM            110
#define idViborkaLuft        111
#define idInAxis             112
#define idOutAxis            113
#define idListProg           114
#define idListSubProg        115
#define idEditPrg            116
#define idEditPrgInput       117
#define idViewOutputs        118
#define idInGen              119
#define idOutGen             120
#define idFInGen             121
#define idSInGen             122
#define idMInGen             123
#define	idFOutGen            124
#define	idSOutGen            125
#define	idMOutGen            126
#define idOABS               127
#define idInstrC             128
#define idInstrCInput        129
#define idDiscZ              130
#define idAnalZ              131
#define idAnalZInput         132
#define idG1Input            133
#define idG0Input            134
#define idVixodPMInput       135
#define idViborkaLuftInput   136
#define idViewOutputsInput   137
#define idDeallp             138
#define idAnout              139
#define idAnoutInput         140
#define idLubric             141
#define idLubricInput        142
#define idModbZ              143
#define idAnin               144
#define idAninInput          145
#define SnakeInit            555
#define SnakeLoop            556

#define IN_BUF_SZ   64 // размер буфера ввода
#define BUF_CLC_RES 30 // размер буфера результата калькулятора
#define mystr display


enum input
{
	input_idle = 0,
	ok  = 1,
	cancel_ = 2
};

enum errors
{	
    no_error         = 0,
	integer_overflow = 1
};

#define INIT_CH 0
#define CHANGE_FOCUS_MENU_CH 1
#define IDLE_CH 2
#define CHANGE_PARAM_CH 3
#define INPUT_LOOP_CH 4
#define PUSK_CH 5
#define LOAD_COORD_HOLE_CH 6

#define INIT_CMD  0
#define IDLE_CMD  1
#define INPUT_CMD 2
#define EDIT_CMD  3
#define INIT_EDIT_CMD  4

#define INIT_MVD   0
#define UPDATE_MVD 1
#define IDLE_MVD   2

#define INIT_CNTR   0
#define UPDATE_CNTR 1
#define IDLE_CNTR   2

#define INIT_CNTR_PR   0
#define UPDATE_CNTR_PR 1
#define IDLE_CNTR_PR   2

//Меню МАХОВИК ОСИ
#define INIT_CNTR_HW   0
#define UPDATE_CNTR_HW 1
#define IDLE_CNTR_HW   2

#define INIT_MH            0
#define CHOICE_MH          1
#define IDLE_MH            2
#define UPDATE_AXIS_PIC_MH 3
#define NO_DIR_MH          0
#define DIR_MH_RIGHT       1
#define DIR_MH_LEFT       -1

#define MAXLONGLONG (0x7fffffffffffffff)
#define MINLONGLONG (~MAXLONGLONG)

#define NUM_POINT              0
#define EDIT_POINT             1
#define NEXT_POINT             2
#define PREV_POINT             3
#define SET_INTERVAL           15
#define SET_COUNT_INTERVAL     16

#define LEFT_ALIGN   0
#define RIGHT_ALIGN  1

//===ref_mods RM(режим поиска реф метки)====
#define SIMPLE_RM             1
#define DISTANCE_TO_SWITCH_RM 2
//====
#define MAX_FRAME 1
#define FRAME_SZ  60
#define STR_FRAME_SZ_DISPLAY 30

#if (configUSE_NCshell == 1)
#define vibor_axis_ON(c) !(flash_prg._INAxis[c].vibor_axis & YESNO_P_bm)
#define vibor_axis_OFF(c) (flash_prg._INAxis[c].vibor_axis & YESNO_P_bm)
#endif

class Menu 
{   
     public:
	 Menu()
	 :inputnum(9)
	 ,curI(0)
	 ,curItem(&curI)
	 ,cur_ch_calc_2r2(N_AXIS)
	 {
		#if (configUSE_NCshell == 1)
		for(int c = 0;c <N_AXIS;c++)
			cms.cur_move_show[c] = _pic_CUR_CORR;
		#endif
	 };
	 ~Menu(){};

	 unsigned int inputnum;
	 
	 bool direction;
	 unsigned char cursorrunner;
	 unsigned char x_cur_offset, y_cur_offset;
	 int CurFormatM;
	 errors errorno;
     char key_point_counter;
	 int8_t beep_cnt;
	/*
	sound_and_other - переменная хранящая в себе всякие настройки в битовом виде
	0x01 - звук, 0x02 - обновление координат, 0x04 - захват ошибки датчика ,0x08 - внешний ПЛК
	0x10 - очистка преднабора
	*/
	 unsigned char sound_and_other;
	 long sz_refref[N_AXIS];
	 bool clr_cursor;
	 enum m_state:uint8_t {
		//== жизненный цикл меню
		 INIT,
		 UPDATE,
		 IDLE,
		//==
		 //==состояния для редактора программ
		 DEL_FRAME,
		 ADD_FRAME,
		 REP_FRAME,
		 INS_FRAME,
		 COM_FRAME,
		 FID_FRAME,
		 EDT_FRAME,
		//==
		 EXIT_EXE,    
		 EXECUTE_EXE,
		 INIT_WORK_EXE,
		 WORKING_EXE,  //это состояние опрашивается в прерывании
		 UPDATE_BLANK_FUNC,
		 AUTO_EXECUTE_EXE,
		 ONE_FRM_DOWN,
		 //==состояния задержек во время выполнения программы
		 DELAY_FRM,
		 DELAY_M5M3M4, 
		 DELAY_WAIT_SHPINDEL,
		 DELAY_INPUT_M,
		//==
	 };
	 enum ALERT{
		NO_ALERT,
		CLEAR_ALERT, // стереть аварийное сообщение
		INSTRUMENT_NE_ZAJAT,
		SHPINDEL_NE_VRASHAETSIA,
		SHPINDEL_NE_OSTANOVLEN,
		OCb_NA_TORMOZE,
		OCb_VNE_DOPUSKA,
		STANOK_NE_GOTOV,
		PLC_ERROR,
		MOVE_ERROR,//ошибка движения (есть команда но датчик стоит)
		DREIF_ERROR,//ошибка направления
		ENC_ERROR,//ошибка датчика(на будущее)
	 }alert; // аварийные сообщения 
	 void print_alert_mes();
	 //=================================================CRC16MODBUS reverse
	 unsigned short crc(unsigned char *data, unsigned short length);
	 void append_crc(uint16_t crc16,unsigned char *data,int p);
	 bool check_crc(uint16_t crc16, unsigned char* data, int p);
	 //===============================================
	 void status(const char *str,int delay = 0,unsigned char page = 0,bool rst_buf_mem = 0,char A = 0);
	 bool status2(const char *str,int delay = 0,unsigned char page = 0);
	 void status_RAZRABOTKA();
	 bool reset_error();
	 void updENC_error();
	 int ConvertASCIIlinear_circle_tolong(long &data, int format, unsigned char *input_str);
	 int ConvertASCIIlinear_circle_tolonglong(long long&data, int format, char *str);
	 void cutting_str(char *input_str,int&n_sym, const char&n_after_dot,const int& f);
	 void put_large_string(unsigned char size_txt_string,char *buf,unsigned char y);
	 void put_big_string(unsigned char size_txt_string,char *buf,unsigned char y);
	 void strings_out(uint8_t place,uint8_t string_size,char *reverse_order);

	 void l_d(long &data,int8_t c,char action);
//---------------------------------------------	 
	 void ConvertASCIIcircle_secundo_tolong_new(long &data,int format,unsigned char *input_str);
	 void get_grdusminsec(char * input_str, char * out_str, const unsigned char& size, const unsigned char znak);
	 long strtol_lir(unsigned char * str);
	 int  strtoi_lir(unsigned char * str);
	 long long strtoll_lir(unsigned char * str);
	 int lltoa_lir(long long  data, unsigned char * str_c);

	 void init_cursor_pos(const int &inputnum = 11);
	 void flashing_cursor(unsigned int x_, unsigned char y_,char page = 0);
	 void number_key(char &);
	 void key_C_ENTER_STOP(const unsigned char & k_c);
	 bool Activation_Reset_Parametrs(char &_key_START);
	 void strcpylir(char*_Dest, char const* _Source);
	 bool checkInputStr(char * str);
	 void ChangeSpeedOnScreen();
	 void UpdateSpeedOnScreen();
	 void ChangeSettings(unsigned char &baudrate);
	 void ShowResetParmetrs();
	 void ShowResetOffset();
	 bool gotomenuDiagnostika();
	 bool gotoModeHand();
	 bool gotoModeRef();
	 bool gotoModeRef_from_button();
	 bool ResetParametrsinRAM();
	 void showActionResetparam();
	 void showActionResetparam2();
	 void showActionResetOffset();
	 char result[6];
	 char ResultForInputNum;
	
	 unsigned char bufASCIIstr[IN_BUF_SZ];
	 unsigned char buf_for_reset[4];
	 unsigned int cursor_pos;
	 unsigned char tgrc;

	 char Str_Channel[N_AXIS][4]; //отображение X.1 Y.2
	 int8_t cnt_StrCh;		      //выбранный канал по кнопке BLANK 2
	 int8_t corr_see_cur_ch;	  //скачанный с флешь или созданный в озу 
	 bool update_table_corr;
	
	 unsigned char WorkCorr_entry;
	 //----------------------
	 uint16_t P2[N_AXIS + N_SA];// ОСИ ПУЛЬТА (линейные ,круговые, слепые)
	 uint16_t Str[N_STR];	   // ОСЕВЫЕ СТРОКИ ПУЛЬТА
	 //----------------------
	 bool ch_use(const int ch); 
	 void change_axis_name(const int ch);
	 int incdec_axis(const char dir, signed char axis);

     typedef struct  context 
        {
	        int id;                     // тип int в AVRGCC 2 байта
			context (int id_){
				id = id_;
			}					
        }  context ;		
		void SET_C(int id);
		//==главный указатель
		const context curI;
		const context *curItem;				
		//==
		char Calcfrommenu;
		void take_buttons(char& k_c);
		void Menu_actions();		
		void menu_up(const int id,int8_t &cnt,const uint8_t SZ,m_state &entry);
		void menu_down(const int id,int8_t &cnt,const uint8_t SZ,m_state &entry);
		void skip_menu_item(bool updown,int8_t &cnt,int num);
//---------------------------------	
		void Draw_menu(const char** menu_, uint8_t cnt,const unsigned char& page);		
		void init_cursor_after_input(int id,int x,int y,int count = 12);		
		int show_axis_above_menu2(int x_place_axis,char only_axis = 0 ,char type = 0);
		bool strncmp_lir(const char *str1, const char *str2, int maxcount);
		void key_start_actions(char &k_c);
		void key_stop_actions();		
		void gotoStart();
		void GoToMenu(int id);	
		void LirSnake();
		void UpdateTypeAxis(int ch);
		void UpdateStr();
		void UpdateFormatAxis(int ch);
		void UpdateFormatAxis2(int ch);
		void WritetoEepromAxisOptions(int ch);
		
		bool exitmenuDiagnostika(int id);
		void check_pointer();
		void show_interval_izmer_speed(int ch,int x_offset,int y_offset,char align = 0);
		bool find_same_symbol(uint16_t *symbols,int cur);
		int find_same_symbol_and_return_str(CInit::AXIS_NAME symbols[],int cur);
		int GetCountSpeedStr(uint8_t *param);
		bool Update_error;
		bool Update_big_digit[N_AXIS];
		//unsigned char Clear_rect_after_error;
		bool see_error_in_rectangle(unsigned char speed_error_memory[],unsigned char speed_error[],int i);		
		void goto_mode(const int& in, const int& from,const char &clear);		
		long String_to_bin(unsigned char*input_str,const unsigned char& ch);
		char Key_Point_min_sec();
		char trigger_Point_min_sec;
		void Negative_input();
		bool Update_input_str;
		unsigned char find_grd_min_sec();
		void checklimit_for_rectangle_Output(long &counter);
		void change_sound();
		void change_speed_out_disp();
		void change_Zaxvat_oshibki_datchika();
		void change_Extern_PLC();
		void change_Erase_Prednabor();
		bool get_Extern_PLC();
		bool get_Erase_Prednabor();
		bool get_Disp_upd();
		uint16_t plb2(uint8_t stepb);
		uint32_t plb2l(uint8_t stepb);
		uint64_t plb2ll(uint8_t stepb);
		void set_ch_to_ABS(int ch);
		bool abs_ENC[N_AXIS]; // тип энкодера на канале  1 - абсолютный, 0 - инкрементный/квазиабсолютный(кодированный)
		void control_encoders();
		long get_max_circle(int ch);
		bool is_Analog();
		void upd_F_display();
		void beep();
		void beep_on();
		long conv_c_i(long cnt,uint8_t c, bool ref);
		void set_offset(long &cnt,uint8_t c);
		long conv_c_i_c(long cnt,uint8_t c, bool ref);
		long get_spd(uint8_t c);
		
		int iForm(uint8_t c);
		long cForm(long tmp,uint8_t c);
		uint8_t hex2bcd(uint8_t x);
		long hex2bcd_l(long in);
		bool Encs_fault();
		bool is_rel_coded_ref(uint8_t c);
		//===============================================HAND================================================================
		m_state entry_hand;
		uint8_t strfunc_hand; // текущая выбранная строка в подрежиме FUNC 0 - первая 1 - вторая 2 третья 3 четвертая  4 - блокировка выделения
		bool error_on_hand;// 1 - была ошибка во время начала исполнения кадра
		void init_hand(const unsigned char& page);
		int init_hand_CMD_CTRL(const unsigned char& page);
		void exit_hand_CMD_CTRL(const unsigned char& page);
		void actions_hand(const unsigned char& page);
		void execute_hand();
		void initWork_hand(const unsigned char & page);
		void blank_right_input_hand(const char& BLANK);//key_BLANK1 key_BLANK2 key_BLANK3 key_BLANK4
		void menu_enter_hand();//key_ENTER
		void menu_D_hand();//key_D
		void menu_R_hand();//key_R
		void menu_BLANK2_hand(); //key_BLANK3
		void menu_BLANK3_hand(); //key_BLANK3
		void menu_START_hand();//key_START
		void show_func_blank_hand(const char& page);
		void invert_axis_in_modeFUNC(char key);
		char convStoKey(int s);
		bool is_str_mh(char key);
		bool is_BIG_Out_Axis();
		//===================================================================================================================
		//=============================================HAND_INPUT (HNDI)=====================================================
		void actions_input_HNDI();
		void hand_input();
		void get_bin_from_str_for_hand_input(const unsigned char& ch);
		void set_counter_rel(const unsigned char& ch,long &bin);
		//===================================================================================================================
#if (configUSE_NCshell == 1)
		//===============================================МЕНЮ "ОТЛАДАКА ->ВЫХОДЫ DOUT"=======================================
		m_state entry_DOUT;
		int8_t menu_cnt_DOUT;
		int8_t menu_cnt_up_down_DOUT;  // 0 - P1  выходы ,  1 , 2- маркеры 
		int32_t p_out; //выходы пульта
		int32_t m_out; //выходы контроллера
		int16_t U_out[2];// напряжение ЦАП ,массив из двух значений [0] для >а1,[1] для >а2
		uint8_t menu_DOUT_SZ;
		short mem_m80_m95;  // переменная для сохранения текущего состояния маркеров относящихся к выходам пульта
		void actions_DOUT(const unsigned char& page);
		void actions_DOUT_input(const unsigned char& page);
		void init_menu_DOUT(const unsigned char& page);
		void draw_menu_DOUT(const unsigned char& page);
		void menu_up_DOUT();      //_key_UP
		void menu_down_DOUT();	  //_key_DOWN
		void menu_left_DOUT();	  //_key_LEFT
		void menu_right_DOUT();   //_key_RIGHT
		void menu_enter_DOUT();   //_key_ENTER
		void menu_stop_DOUT();    //_key_STOP
		void menu_C_DOUT();       //_key_C
		//===================================================================================================================
		//===============================================МЕНЮ "КОНФИГУРАЦИЯ ВХОДОВ ОБЩАЯ INGEN"===========================
		m_state entry_INGEN;
		int8_t menu_cnt_INGEN = 1;
		uint8_t menu_INGEN_SZ;
		void actions_INGEN(const unsigned char& page);
		void init_menu_INGEN(const unsigned char& page);
		void draw_menu_INGEN(const unsigned char& page);
		void menu_up_INGEN();	   //_key_UP
		void menu_down_INGEN();    //_key_DOWN
		void menu_enter_INGEN();   //_key_ENTER
		void menu_stop_INGEN();    //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ "ОБЩИЕ ФУНКЦИИ ВХОДОВ FINGE"================================
		m_state entry_FINGE;
		int8_t menu_cnt_FINGE = 1;
		uint8_t menu_FINGE_SZ;
		void actions_FINGE(const unsigned char& page);
		void init_menu_FINGE(const unsigned char& page);
		void draw_menu_FINGE(const unsigned char& page);
		void draw_param_FINGE(const unsigned char& page, int x, char align);
		void menu_up_FINGE();	   //_key_UP
		void menu_down_FINGE();    //_key_DOWN
		void menu_left_FINGE();  //_key_LEFT
		void menu_right_FINGE(); //_key_RIGHT
		void menu_F_FINGE();       //_key_F
		void menu_enter_FINGE();   //_key_ENTER
		void menu_stop_FINGE();    //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ "ВХОДЫ УПРАВЛЕНИЯ ШПИНДЕЛЕМ SINGE"==========================
		m_state entry_SINGE;
		int8_t menu_cnt_SINGE = 1;
		uint8_t menu_SINGE_SZ;
		void actions_SINGE(const unsigned char& page);
		void init_menu_SINGE(const unsigned char& page);
		void draw_menu_SINGE(const unsigned char& page);
		void draw_param_SINGE(const unsigned char& page, int x, char align);
		void menu_up_SINGE();	   //_key_UP
		void menu_down_SINGE();    //_key_DOWN
		void menu_left_SINGE();    //_key_LEFT
		void menu_right_SINGE();   //_key_RIGHT
		void menu_F_SINGE();       //_key_F
		void menu_enter_SINGE();   //_key_ENTER
		void menu_stop_SINGE();    //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ "M-ФУНКЦИИ ВХОДОВ MINGE"====================================
		m_state entry_MINGE;
		int8_t menu_cnt_MINGE = 1;
		uint8_t menu_MINGE_SZ;
		void actions_MINGE(const unsigned char& page);
		void init_menu_MINGE(const unsigned char& page);
		void draw_menu_MINGE(const unsigned char& page);
		void draw_param_MINGE(const unsigned char& page, int x, char align);
		void menu_up_MINGE();	   //_key_UP
		void menu_down_MINGE();    //_key_DOWN
		void menu_left_MINGE();	   //_key_LEFT
		void menu_right_MINGE();   //_key_RIGHT
		void menu_F_MINGE();	   //_key_F
		void menu_enter_MINGE();   //_key_ENTER
		void menu_stop_MINGE();    //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ "КОНФИГУРАЦИЯ ВЫХОДОВ ОБЩАЯ OUTGEN"=========================
		m_state entry_OUTGEN;
		int8_t menu_cnt_OUTGEN = 1;
		uint8_t menu_OUTGEN_SZ;
		void actions_OUTGEN(const unsigned char& page);
		void init_menu_OUTGEN(const unsigned char& page);
		void draw_menu_OUTGEN(const unsigned char& page);
		void menu_up_OUTGEN();	    //_key_UP
		void menu_down_OUTGEN();    //_key_DOWN
		void menu_enter_OUTGEN();   //_key_ENTER
		void menu_stop_OUTGEN();    //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ "ОБЩИЕ ФУНКЦИИ ВЫХОДОВ FOUTGE"================================
		m_state entry_FOUTGE;
		int8_t menu_cnt_FOUTGE = 1;
		uint8_t menu_FOUTGE_SZ;
		void actions_FOUTGE(const unsigned char& page);
		void init_menu_FOUTGE(const unsigned char& page);
		void draw_menu_FOUTGE(const unsigned char& page);
		void draw_param_FOUTGE(const unsigned char& page, int x, char align);
		void menu_up_FOUTGE();	   //_key_UP
		void menu_down_FOUTGE();   //_key_DOWN
		void menu_left_FOUTGE();   //_key_LEFT
		void menu_right_FOUTGE();  //_key_RIGHT
		void menu_F_FOUTGE();      //_key_F
		void menu_enter_FOUTGE();  //_key_ENTER
		void menu_stop_FOUTGE();   //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ "ВЫХОДЫ УПРАВЛЕНИЯ ШПИНДЕЛЕМ SOUTGE"========================
		m_state entry_SOUTGE;
		int8_t menu_cnt_SOUTGE = 1;
		uint8_t menu_SOUTGE_SZ;
		void actions_SOUTGE(const unsigned char& page);
		void init_menu_SOUTGE(const unsigned char& page);
		void draw_menu_SOUTGE(const unsigned char& page);
		void draw_param_SOUTGE(const unsigned char& page, int x, char align);
		void menu_up_SOUTGE();	    //_key_UP
		void menu_down_SOUTGE();    //_key_DOWN
		void menu_left_SOUTGE();    //_key_LEFT
		void menu_right_SOUTGE();   //_key_RIGHT
		void menu_F_SOUTGE();		//_key_F
		void menu_enter_SOUTGE();   //_key_ENTER
		void menu_stop_SOUTGE();    //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ "M-ФУНКЦИИ ВЫХОДОВ MOUTGE"====================================
		m_state entry_MOUTGE;
		int8_t menu_cnt_MOUTGE = 1;
		uint8_t menu_MOUTGE_SZ;
		void actions_MOUTGE(const unsigned char& page);
		void init_menu_MOUTGE(const unsigned char& page);
		void draw_menu_MOUTGE(const unsigned char& page);
		void draw_param_MOUTGE(const unsigned char& page, int x, char align);
		void menu_up_MOUTGE();	   //_key_UP
		void menu_down_MOUTGE();    //_key_DOWN
		void menu_left_MOUTGE();  //_key_LEFT
		void menu_right_MOUTGE(); //_key_RIGHT
		void menu_F_MOUTGE();
		void menu_enter_MOUTGE();   //_key_ENTER
		void menu_stop_MOUTGE();    //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ "ПОДАЧАG1 & УСКОРЕННЫЙ ХОДG0"==================================
		m_state entry_G1_G0;
		int8_t menu_cnt_G1 = 1;
		int8_t menu_cnt_G0 = 1;
		int8_t menu_cnt_G1h;
		int8_t menu_cnt_G0h;
		uint8_t menu_G1_G0_SZ;
		void actions_m_G1_G0(const unsigned char & page);
		void actions_G1_G0_input_pr(const unsigned char& page);
		void exit_G1_G0_input_pr(const unsigned char& page,int x_,int y_);
		void init_menu_G1_G0(const unsigned char & page);
		void draw_menu_G1_G0(const unsigned char & page);
		void draw_param_G1_G0_Discret(const unsigned char & page);
		void draw_param_G1_G0_Analog(const unsigned char & page);
		void menu_up_G1_G0();	 //_key_UP
		void menu_down_G1_G0();  //_key_DOWN
		void menu_left_G1_G0();  //_key_LEFT
		void menu_right_G1_G0(); //_key_RIGHT
		void menu_enter_G1_G0(); //_key_ENTER
		void menu_1_G1_G0();     //_key_1 (не используется)
		void menu_0_G1_G0();     //_key_0 (не используется)
		void menu_stop_G1_G0();  //_key_STOP
		int gFi(uint8_t c);
		//===============================================МЕНЮ "ВЫХОД В PM(VIXPM)"===============================
		m_state entry_VIXPM;
		int8_t menu_cnt_VIXPM = 1;
		int8_t menu_cnt_VIXPMh;
		uint8_t menu_VIXPM_SZ;
		void actions_VIXPM(const unsigned char& page);
		void actions_VIXPM_input_pr(const unsigned char& page);
		void init_menu_VIXPM(const unsigned char& page);
		void draw_menu_VIXPM(const unsigned char& page);
		void draw_param_VIXPM_Discret(const unsigned char& page);
		void draw_param_VIXPM_Analog(const unsigned char& page);
		void menu_up_VIXPM();	 //_key_UP
		void menu_down_VIXPM();  //_key_DOWN
		void menu_left_VIXPM();  //_key_LEFT
		void menu_right_VIXPM(); //_key_RIGHT
		void menu_enter_VIXPM(); //_key_ENTER
		void menu_stop_VIXPM();  //_key_STOP
		//================================================================================================================
		//==============================================МЕНЮ "выборка Люфта(VIBLUF)"======================================
		m_state entry_VIBLUF;
		int8_t menu_cnt_VIBLUF = 1;
		int8_t menu_cnt_VIBLUFh;
		uint8_t menu_VIBLUF_SZ;
		void actions_VIBLUF(const unsigned char& page);
		void actions_VIBLUF_input_pr(const unsigned char& page);
		void init_menu_VIBLUF(const unsigned char& page);
		void draw_menu_VIBLUF(const unsigned char& page);
		void draw_param_VIBLUF_Discret(const unsigned char& page);
		void draw_param_VIBLUF_Analog(const unsigned char& page);
		void menu_up_VIBLUF();	  //_key_UP
		void menu_down_VIBLUF();  //_key_DOWN
		void menu_left_VIBLUF();  //_key_LEFT
		void menu_right_VIBLUF(); //_key_RIGHT
		void menu_enter_VIBLUF(); //_key_ENTER
		void menu_stop_VIBLUF();  //_key_STOP
		//================================================================================================================
		
		//===============================================МЕНЮ Выбора параметров ЛИР541 CHPA===============================
		m_state entry_CHPA;
		int8_t menu_cnt_CHPA = 1;
		uint8_t menu_CHPA_SZ;
		void actions_CHPA(const unsigned char& page);
		void init_menu_CHPA(const unsigned char& page);
		void draw_menu_CHPA(const unsigned char& page);
		void menu_up_CHPA();	  //_key_UP
		void menu_down_CHPA();    //_key_DOWN
		void menu_enter_CHPA();   //_key_ENTER
		void menu_stop_CHPA();    //_key_STOP
		//================================================================================================================
		//==============================================МЕНЮ "ЗОНЫ СКОРОСТИ"SPDZ==========================================
		m_state entry_SPDZ;
		int8_t menu_cnt_SPDZ = 1;
		uint8_t menu_SPDZ_SZ;
		void actions_SPDZ(const unsigned char& page);
		void init_menu_SPDZ(const unsigned char& page);
		void draw_menu_SPDZ(const unsigned char& page);
		void menu_up_SPDZ();	  //_key_UP
		void menu_down_SPDZ();    //_key_DOWN
		void menu_enter_SPDZ();   //_key_ENTER
		void menu_stop_SPDZ();    //_key_STOP	
		//================================================================================================================
		//=============================================МЕНЮ "УПРАВЛЕНИЕ"CNTRL=============================================
		m_state entry_CNTRL;
		int8_t menu_cnt_CNTRL = 1;
		uint8_t menu_CNTRL_SZ;
		void actions_CNTRL(const unsigned char& page);
		void actions_CNTRL_input_pr(const unsigned char& page);
		void init_menu_CNTRL(const unsigned char& page);
		void draw_menu_CNTRL(const unsigned char& page);
		void draw_param_CNTRL(const unsigned char& page, int x, char align);
		void menu_up_CNTRL();	  //_key_UP
		void menu_down_CNTRL();    //_key_DOWN
		void menu_enter_CNTRL();   //_key_ENTER
		void menu_left_CNTRL();    //_key_LEFT
		void menu_right_CNTRL();   //_key_RIGHT
		void menu_stop_CNTRL();    //_key_STOP
		long circle_endless_inout(long tmp,int ch,bool in);
		//================================================================================================================
		//=============================================МЕНЮ "ЗОНЫ ТОРМОЖЕНИЯ ОСИ"ZNBRK====================================
		m_state entry_ZNBRK;
		int8_t menu_cnt_ZNBRK = 1;
		uint8_t menu_ZNBRK_SZ;
		void actions_ZNBRK(const unsigned char& page);
		void actions_ZNBRK_input_pr(const unsigned char& page);
		void init_menu_ZNBRK(const unsigned char& page);
		void draw_menu_ZNBRK(const unsigned char& page);
		void draw_param_ZNBRK(const unsigned char& page, int x, char align);
		void menu_up_ZNBRK();	   //_key_UP
		void menu_down_ZNBRK();    //_key_DOWN
		void menu_enter_ZNBRK();   //_key_ENTER
		void menu_stop_ZNBRK();    //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ ВХОДЫ ОСИ IN_AXIS(INAXI)====================================
		m_state entry_INAXI;
		int8_t menu_cnt_INAXI = 1;
		uint8_t menu_INAXI_SZ;
		void actions_INAXI(const unsigned char& page);
		void init_menu_INAXI(const unsigned char& page);
		void draw_menu_INAXI(const unsigned char& page);
		void draw_param_INAXI(const unsigned char& page, int x, char align);
		void menu_up_INAXI();	   //_key_UP
		void menu_down_INAXI();    //_key_DOWN
		void menu_left_INAXI();	   //_key_LEFT
		void menu_right_INAXI();   //_key_RIGHT
		void menu_F_INAXI();
		void menu_enter_INAXI();   //_key_ENTER
		void menu_stop_INAXI();    //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ ВЫХОДЫ ОСИ OUT_AXIS(OUTAXI)=================================
		m_state entry_OUTAXI;
		int8_t menu_cnt_OUTAXI = 1;
		uint8_t menu_OUTAXI_SZ;
		void actions_OUTAXI(const unsigned char& page);
		void init_menu_OUTAXI(const unsigned char& page);
		void draw_menu_OUTAXI(const unsigned char& page);
		void draw_param_OUTAXI(const unsigned char& page, int x, char align);
		void menu_up_OUTAXI();	    //_key_UP
		void menu_down_OUTAXI();    //_key_DOWN
		void menu_left_OUTAXI();	//_key_LEFT
		void menu_right_OUTAXI();   //_key_RIGHT
		void menu_F_OUTAXI();       //_key_F
		void menu_enter_OUTAXI();   //_key_ENTER
		void menu_stop_OUTAXI();    //_key_STOP
		//================================================================================================================
		//==============================================МЕНЮ СМАЗКА(LUBR)=================================================
		m_state entry_LUBR;
		int8_t menu_cnt_LUBR = 1;
		uint8_t menu_LUBR_SZ;
		void actions_LUBR(const unsigned char& page);
		void actions_LUBR_input(const unsigned char& page);
		void init_menu_LUBR(const unsigned char& page);
		void draw_menu_LUBR(const unsigned char& page);
		void draw_param_LUBR(const unsigned char& page, int x, char align);
		void menu_up_LUBR();	    //_key_UP
		void menu_down_LUBR();      //_key_DOWN
		void menu_left_LUBR();	    //_key_LEFT
		void menu_right_LUBR();     //_key_RIGHT
		void menu_enter_LUBR();   //_key_ENTER
		void menu_stop_LUBR();    //_key_STOP
		//================================================================================================================
		//==============================================МЕНЮ ДИСКРЕТНОЕ ЗАДАНИЕ ОСИ (DISCZ)===============================
		m_state entry_DISCZ;
		int8_t menu_cnt_DISCZ = 1;
		uint8_t menu_DISCZ_SZ;
		void actions_DISCZ(const unsigned char& page);
		void init_menu_DISCZ(const unsigned char& page);
		void draw_menu_DISCZ(const unsigned char& page);
		void draw_param_DISCZ(const unsigned char& page, int x, char align);
		void menu_up_DISCZ();	   //_key_UP
		void menu_down_DISCZ();    //_key_DOWN
		void menu_left_DISCZ();	   //_key_LEFT
		void menu_right_DISCZ();   //_key_RIGHT
		void menu_F_DISCZ();       //_key_F
		void menu_enter_DISCZ();   //_key_ENTER
		void menu_stop_DISCZ();    //_key_STOP
		//================================================================================================================
		//==============================================МЕНЮ АНАЛОГОВОЕ ЗАДАНИЕ ОСИ (ANALZ)===============================
		m_state entry_ANALZ;
		int8_t menu_cnt_ANALZ = 1;
		uint8_t menu_ANALZ_SZ;
		void actions_ANALZ(const unsigned char& page);
		void actions_ANALZ_input_pr(const unsigned char& page);
		void init_menu_ANALZ(const unsigned char& page);
		void draw_menu_ANALZ(const unsigned char& page);		
		void draw_param_ANALZ(const unsigned char& page, int x, char align);
		void menu_up_ANALZ();	   //_key_UP
		void menu_down_ANALZ();    //_key_DOWN
		void menu_left_ANALZ();	   //_key_LEFT
		void menu_right_ANALZ();   //_key_RIGHT
		void menu_enter_ANALZ();   //_key_ENTER
		void menu_stop_ANALZ();    //_key_STOP
		//================================================================================================================
		//==============================================МЕНЮ MODBUS ЗАДАНИЕ ОСИ (MODBZ)===================================
		m_state entry_MODBZ;
		int8_t menu_cnt_MODBZ = 1;
		uint8_t menu_MODBZ_SZ;
		void actions_MODBZ(const unsigned char& page);
		void actions_MODBZ_input_pr(const unsigned char& page);
		void init_menu_MODBZ(const unsigned char& page);
		void draw_menu_MODBZ(const unsigned char& page);
		void draw_param_MODBZ(const unsigned char& page, int x, char align);
		void menu_up_MODBZ();	   //_key_UP
		void menu_down_MODBZ();    //_key_DOWN
		void menu_left_MODBZ();	   //_key_LEFT
		void menu_right_MODBZ();   //_key_RIGHT
		void menu_enter_MODBZ();   //_key_ENTER
		void menu_stop_MODBZ();    //_key_STOP
		//================================================================================================================
#endif
		//===============================================МЕНЮ "СИСТЕМА КООРДИНАТ(FSET)"===================================
		m_state entry_FSET;
		int8_t menu_cnt_FSET = 1;
		uint8_t menu_FSET_SZ;
		void actions_input_FSET(const unsigned char & page);
		uint8_t get_ch_FSET();
		//==================================================================================================================
		//==============================================МЕНЮ "Аналоговый выход"(ANOUT)======================================
		m_state entry_ANOUT;
		int8_t menu_cnt_ANOUT = 1;
		uint8_t menu_ANOUT_SZ;
		void actions_ANOUT();
		void debug_counters_ANOUT();
		void actions_input_ANOUT();
		void init_menu_ANOUT();
		void draw_menu_ANOUT();
		void draw_param_ANOUT(char x,char align);
		void menu_up_ANOUT();	   //_key_UP
		void menu_down_ANOUT();    //_key_DOWN
		void menu_enter_ANOUT();   //_key_ENTER
		void menu_stop_ANOUT();    //_key_STOP
		//==================================================================================================================
		//==============================================МЕНЮ "Аналоговый вход"(ANIN)========================================
		m_state entry_ANIN;
		int8_t menu_cnt_ANIN = 4;
		uint8_t menu_ANIN_SZ;
		void actions_ANIN();
		void debug_counters_ANIN();
		void actions_input_ANIN();
		void init_menu_ANIN();
		void draw_menu_ANIN();
		void draw_param_ANIN(char x,char align);
		void menu_up_ANIN();	  //_key_UP
		void menu_down_ANIN();    //_key_DOWN
		void menu_enter_ANIN();   //_key_ENTER
		void menu_stop_ANIN();    //_key_STOP
		void menu_left_ANIN();	  //_key_LEFT
		void menu_right_ANIN();   //_key_RIGHT
		void Analog_Corr_handler_ANIN();
		//==================================================================================================================
		//===============================================МЕНЮ "СМЕЩЕНИЯ КООРДИНАТ(OFFSN)"===================================
		m_state entry_OFFSN;
		int8_t menu_cnt_OFFSN = 1;
		int8_t menu_column_OFFSN;
		bool mOFFSNPassG53Ok;
		void actions_OFFSN(const unsigned char & page);
		void actions_input_OFFSN(const unsigned char & page);
		void init_menu_OFFSN(const unsigned char & page);
		void draw_menu_OFFSN(const unsigned char & page);
		void predprosmotr_offset(const unsigned char& page,int x,int y); // 464 bytes in flash
		void menu_up_OFFSN();	   //_key_UP
		void menu_down_OFFSN();    //_key_DOWN
		void menu_enter_OFFSN();   //_key_ENTER
		void menu_stop_OFFSN();    //_key_STOP
		//=================================================================================================================
		//===============================================МЕНЮ "КОРРЕКЦИЯ НА ИНСТРУМЕНТ(INSTRC)"============================
		m_state entry_INSTRC;
		int8_t menu_cnt_INSTRC;   // 0 - 9текущий выбранный инструмент в меню
		int8_t menu_cnt_INSTRC_rl;//выбор 0 радиус или 1 длина
		int8_t menu_column_INSTRC;
		struct _T_instr{
			long r;// радиус инструмента
			long l;// длина инструмента
		}T_instr[MAX_INSTR];
		void actions_INSTRC(const unsigned char & page);
		void actions_input_INSTRC(const unsigned char & page);
		void init_menu_INSTRC(const unsigned char & page);
		void draw_menu_INSTRC(const unsigned char & page);
		void show_cur_instr(int y);
		void menu_up_INSTRC();	    //_key_UP
		void menu_down_INSTRC();    //_key_DOWN
		void menu_left_INSTRC();	//_key_LEFT
		void menu_right_INSTRC();   //_key_RIGHT
		void menu_enter_INSTRC();   //_key_ENTER
		void menu_stop_INSTRC();    //_key_STOP
		//=================================================================================================================
		//===============================================МЕНЮ "ИЗМЕРЕНИЯ(IZMER)"===========================================
		m_state entry_IZMER;
		int8_t menu_cnt_IZMER = 1;
		uint8_t menu_IZMER_SZ;
		void actions_IZMER(const unsigned char & page);
		void init_menu_IZMER(const unsigned char & page);
		void draw_menu_IZMER(const unsigned char & page);
		void menu_up_IZMER();	   //_key_UP
		void menu_down_IZMER();    //_key_DOWN
		void menu_enter_IZMER();   //_key_ENTER
		void menu_stop_IZMER();    //_key_STOP
		//================================================================================================================
		//===============================================ИНДИКАТОР (idIndikator)INDI======================================
		m_state entry_INDI;
		void actions_INDI(const unsigned char & page);
		void init_menu_INDI(const unsigned char & page);
		//================================================================================================================
		//===============================================ГРАФИК (idGraph)GRPH=============================================
		m_state entry_GRPH;
		void actions_GRPH(const unsigned char & page);
		void init_menu_GRPH(const unsigned char & page);
		void command_GRPH(char key);
		//================================================================================================================
		//===============================================idResetParametrs(REPA)===========================================
		void menu_stop_REPA();
		//================================================================================================================
		//===============================================idResetOffset(REOF)===========================================
		void menu_stop_REOF();
		//================================================================================================================
		//===============================================МЕНЮ "КОРРЕКЦИЯ(CORR)"===========================================
		m_state entry_CORR;
		int8_t menu_cnt_CORR = 1;
		uint8_t menu_CORR_SZ;
		void actions_CORR(const unsigned char & page);
		void init_menu_CORR(const unsigned char & page);
		void draw_menu_CORR(const unsigned char & page);
		void menu_up_CORR();	  //_key_UP
		void menu_down_CORR();    //_key_DOWN
		void menu_enter_CORR();   //_key_ENTER
		void menu_stop_CORR();    //_key_STOP
		//================================================================================================================
		//===============================================Снятие коррекции (idGetCorr)GTCOR================================
		m_state entry_GTCOR;
		void actions_GTCOR(const unsigned char & page);
		void init_menu_GTCOR(const unsigned char & page);
		//================================================================================================================
		//===============================================Работа с таблицами (idSeeCorr)SEECOR=============================
		m_state entry_SEECOR;
		void actions_SEECOR(const unsigned char & page);
		void init_menu_SEECOR(const unsigned char & page);
		bool is_corr_table(uint8_t c);
		bool is_corr(uint8_t c);
		//================================================================================================================
		//===============================================РЕЖИМ СВЯЗЬ С ПК (idLinkPk)LINPK=================================
		void init_menu_LINPK(const unsigned char & page);
		void menu_1_LINKPK();
		void menu_2_LINKPK();
		void menu_stop_LINPK();
		void timer1MSEKON();
		void timer1MSEKOFF();
		//кнопка сброса ошибок находится в другом файле функция uart_usb.reset_error();
		//================================================================================================================
		//===============================================О ПРОГРАММЕ (idAbout)ABOUT========================================
		void init_menu_ABOU(const unsigned char & page);
		void menu_2_ABOU();
		void menu_stop_ABOU();
		void menu_enter_ABOU();
		//================================================================================================================
		//===============================================МЕНЮ ПРЕОБРАЗОВАТЕЛИ ПЕРЕМЕЩЕНИЯ(ENCM)===========================
		void UpdateMenuEncoders();
		void menu_stop_ENCM();
		//================================================================================================================
		//===============================================МЕНЮ ПИТАНИЕ И БАТАРЕЯ(SUPM)=====================================
		void UpdateMenuSupply();
		void menu_stop_SUPM();
		//================================================================================================================
		//===============================================МЕНЮ ДАТА И ВРЕМЯ(TIME)==========================================
		void UpdateMenuTime();
		void menu_stop_TIME();
		//================================================================================================================
		//===============================================МЕНЮ ОТЛАДАКА ->ВХОДЫ(VIIN)======================================
		int8_t menu_cnt_VIIN;
		int8_t menu_cnt_up_down_VIIN;  // 0 - P1  входы ,  1 , 2 - маркеры
		unsigned long mem_in;  // переменная для сохранения текущего состояния
		void InitMenuViewInputs();
		void draw_menu_VIIN();
		void draw_Zone_ref_metki(int y);
		void draw_Zone_ref_metki_static(int y);
		void print_bit(uint32_t data,uint8_t BIT_n,int x ,int y);
		void menu_left_VIIN();
		void menu_right_VIIN();
		void menu_up_VIIN();
		void menu_down_VIIN();
		void menu_enter_VIIN();
		void menu_stop_VIIN();
		char inP(char Pin,bool RISE = false,bool FALL = false);	
		//================================================================================================================
		//===============================================МЕНЮ "РЕЖИМ ОТЛАДКИ(DBG)"========================================
		m_state entry_DBG;
		int8_t menu_cnt_DBG = 1;
		uint8_t menu_DBG_SZ;
		bool block_SPPU_OUT;
		void actions_DBG(const unsigned char & page);
		void init_menu_DBG(const unsigned char & page);
		void draw_menu_DBG(const unsigned char & page);
		void menu_up_DBG();	     //_key_UP
		void menu_down_DBG();    //_key_DOWN
		void menu_enter_DBG();   //_key_ENTER
		void menu_stop_DBG();    //_key_STOP
		//===================================================================================================================
		//===============================================МЕНЮ "СПИСОК ОСЕЙ ПУЛЬТА(AXILIST)"==================================
		m_state entry_AXILIST;
		int8_t menu_cnt_AXILIST = 1;
		uint8_t menu_AXILIST_SZ;
		void actions_AXILIST(const unsigned char & page);
		void init_menu_AXILIST(const unsigned char & page);
		void draw_menu_AXILIST(const unsigned char & page);
		uint8_t getChfromAxisList(const int8_t& str);
		void menu_up_AXILIST();	     //_key_UP
		void menu_down_AXILIST();    //_key_DOWN
		void menu_enter_AXILIST();   //_key_ENTER
		void menu_stop_AXILIST();    //_key_STOP
		//===================================================================================================================
		//===============================================МЕНЮ "НАСТРОЙКИ(OPT)"===============================================
		m_state entry_OPT;
		int8_t menu_cnt_OPT = 1;
		uint8_t menu_OPT_SZ;
		void actions_OPT(const unsigned char & page);
		void actions_input_OPT(const unsigned char & page);
		void init_menu_OPT(const unsigned char & page);
		void draw_menu_OPT(const unsigned char & page);
		void draw_param_OPT(const unsigned char & page, int x, char align);
		void menu_up_OPT();	     //_key_UP
		void menu_down_OPT();    //_key_DOWN
		void menu_enter_OPT();   //_key_ENTER
		void menu_stop_OPT();    //_key_STOP
		void menu_left_OPT();    //_key_LEFT
		void menu_right_OPT();   //_key_RIGHT
		//===================================================================================================================
		//===============================================МЕНЮ "СТРОКА ИНДИКАЦИИ(da)"=========================================
		m_state entry_da;
		int8_t menu_cnt_da = 1;
		uint8_t menu_da_SZ;
		void actions_m_da(const unsigned char & page);
		void init_menu_da(const unsigned char & page);
		void draw_menu_da(const unsigned char & page);
		void draw_param_da(const unsigned char & page);
		void menu_up_da();	  //_key_UP
		void menu_down_da();  //_key_DOWN
		void menu_left_da();  //_key_LEFT
		void menu_right_da(); //_key_RIGHT
		void menu_enter_da();  //_key_ENTER
		void menu_stop_da();  //_key_STOP
		void change_axisD();
		//===================================================================================================================
		//===============================================МЕНЮ "ПУЛЬТ - ОСЬ(pa)"==============================================
		m_state entry_pa;
		int8_t menu_cnt_pa = 1;
		uint8_t menu_pa_SZ;
		void actions_m_pa(const unsigned char & page);
		void init_menu_pa(const unsigned char & page);
		void draw_menu_pa(const unsigned char & page);
		void draw_param_pa(const unsigned char & page);
		void menu_up_pa();	  //_key_UP
		void menu_down_pa();  //_key_DOWN
		void menu_left_pa();  //_key_LEFT
		void menu_right_pa(); //_key_RIGHT
		void menu_stop_pa();  //_key_STOP
		void change_axisP();
		//===================================================================================================================
		//===============================================МЕНЮ "ОБЩИЕ ПАРАМЕТРЫ"(gs)==============================================
		m_state entry_gs;
		int8_t menu_cnt_gs = 1;
		uint8_t menu_gs_SZ;
		void actions_m_gs(const unsigned char & page);
		void actions_m_gs_input(const unsigned char & page); //ввода в меню нету
		void init_menu_gs(const unsigned char & page);
		void draw_menu_gs(const unsigned char & page);
		void draw_param_gs(const unsigned char& page);
		void menu_up_gs();	  //_key_UP
		void menu_down_gs();  //_key_DOWN
		void menu_enter_gs(); //_key_ENTER
		void menu_stop_gs();  //_key_STOP
		void menu_left_gs();  //_key_LEFT
		void menu_right_gs();  //_key_LEFT
		//===================================================================================================================
		//===============================================НАЧАЛЬНЫЙ ЭКРАН (idBegin)BEGN=======================================
		m_state entry_BEGN = IDLE;
		void actions_m_BEGN(const unsigned char & page);
		void init_menu_BEGN(const unsigned char & page);
		void draw_menu_BEGN(const unsigned char & page);
		//===================================================================================================================
		//===============================================МЕНЮ "ДИАГНОСТИКА"DIAG==============================================
		m_state entry_DIAG;
		int8_t menu_cnt_DIAG = 0;
		uint8_t menu_DIAG_SZ;
		bool mDiagPassOk;
		void actions_m_DIAG(const unsigned char & page);
		void actions_m_DIAG_input(const unsigned char & page);
		void init_menu_DIAG(const unsigned char & page);
		void draw_menu_DIAG(const unsigned char & page);
		void menu_up_DIAG();	//_key_UP
		void menu_down_DIAG();  //_key_DOWN
		void menu_enter_DIAG(); //_key_ENTER
		void menu_stop_DIAG();  //_key_STOP
		//===================================================================================================================
		//===============================================МЕНЮ "ПАРАМЕТРЫ ОСИ(ИНДИКАЦИЯ)pr"===================================
		m_state entry_cntr_pr;
		int8_t menu_cntr_cnt_pr = 1;
		int8_t menu_cntr_SZ;
		void actions_cntr_pr(const unsigned char & page);
		void actions_Cntrl_input_pr(const unsigned char & page);
		void init_m_cntr_pr(const unsigned char & page);
		void draw_m_cntr_pr(const unsigned char & page);
		void draw_param_cntr_pr(const unsigned char & page,int x,char align);
		void m_up_cntr_pr();	  //_key_UP
		void m_down_cntr_pr();    //_key_DOWN
		void m_F_cntr_pr();		  //_key_F
		void m_enter_cntr_pr();   //_key_ENTER
		void m_stop_cntr_pr();    //_key_STOP
		void m_left_cntr_pr();    //_key_LEFT
		void m_right_cntr_pr();   //_key_RIGHT
		//===================================================================================================================
		//===============================================НАСТРОЙКИ ДАТЧИКА (абсолютного) OABS================================
		m_state entry_OABS;
		int8_t menu_OABS_cnt = 1;
		int8_t menu_OABS_SZ;
		void actions_OABS(const unsigned char & page);
		void actions_OABS_input(const unsigned char & page);
		void init_OABS(const unsigned char & page);
		void draw_menu_OABS(const unsigned char & page);
		void draw_param_OABS(const unsigned char & page,int x,char align);
		void m_up_OABS();	   //_key_UP
		void m_down_OABS();    //_key_DOWN
		void m_enter_OABS();   //_key_ENTER
		void m_stop_OABS();    //_key_STOP
		void m_left_OABS();    //_key_LEFT
		void m_right_OABS();   //_key_RIGHT
		//===================================================================================================================
		//================================================= форматированный вывод======================================
		void print_long_format(long data,int format,const unsigned char& page,unsigned char x,unsigned char y,char align = LEFT_ALIGN ,char zero = 0,char clear = 0,char cut_zero = 0);
		//=============================================================================================================
		//=================================================CRC16MODBUS==================
		unsigned short crc16(unsigned char *data, unsigned short length);
		//==============================================================================
		void psevdo_abs_disp(long &Xi,unsigned int &ch);
		void read_cnt_simple(long incoming,const int &ch);
		long size_2r2[N_AXIS]; // расстояние между двумя кодированными метками
		long R_off[N_AXIS];    //смещение для относительного счетчика
		bool upd_ref_cnt[N_AXIS]; // 1 - обновить относительный счетчик
		unsigned int cur_ch_calc_2r2;
		char get_ch_from_str(int s);
		char get_str_from_ch(int i);
		char get_format(const int &AXIS);
		char get_format_from_ASCII(const char &sym_fr);
		char get_channel_from_ASCII(const char &sym_fr);
		char get_ASCII_from_channel(int ch);
		char get_format_from_ch(const int &channel);
		char get_channel(const int &AXIS);
		int get_ch_mcx514(char ch);	
		char get_ch_fromaxis_mcx514(int axis,char ch);	
		void get_counters_rel(long* counter);
		void set_counters_rel(const long* counter);
		void reset_counters_rel();
		void add_zero_l(unsigned char *str,const short sz_str,const short cnt_zero);
		void add_filler(unsigned char * str, const short sz_str, const short cnt, const char filler,const char dir = 0);
		void convert_long_to_secundo2(long data,unsigned char *str,int format);
		void extern_go_to_zero();
		void zonePM_switch_toref();
		void zone_limit_switch_toref();
		void start_measure_sw_to_ref(int ch,bool res);
		bool trig_input[N_AXIS];
		bool true_input[N_AXIS];
		char ref_mode[N_AXIS];   //режим выхода в реф
		void goto_Relative_coord(const char &prim_axis,const char &second_axis);
		void ref_to_ref_inEncoderMenu(const char& BLANK);
		void input_to_ref_inViewinputsMenu(const char& BLANK);
		long mul_di(const int &f,const long &a,const long &b);
		long div_di(const int &f,const long &a,const long &b);
		long check_max_k_divider(long result);
		long check_max_set_point(long divider,long result,uint8_t ch);
		long abs_lir(const long &in);
#if (configUSE_NCshell == 1)
		//==============================================МЕНЮ "программы и подпрограммы"===================================
		m_state entry_Prg_SubPrg;
		int8_t menu_Prg_SubPrg = 0;
		void actions_Prg_SubPrg(const unsigned char & page);
		void init_menu_Prg_SubPrg(const unsigned char & page);
		void draw_menu_Prg_SubPrg(const unsigned char & page);
		void menu_up_Prg_SubPrg();	  //_key_UP
		void menu_down_Prg_SubPrg();  //_key_DOWN
		void menu_enter_Prg_SubPrg(); //_key_ENTER
		void menu_stop_Prg_SubPrg();  //_key_STOP
		void show_comm_in_list(char PrgorSubPrg,int i,int8_t scroll,int x);
		bool check_prg(char PrgorSubPrg);
		//================================================================================================================
		//===============================================МЕНЮ "Список программ LISPRG"====================================
		m_state entry_LISPRG;
		int8_t menu_LISPRG = 0;
		int8_t menu_LISPRG_SZ;
		int8_t menu_scroll_LISPRG;
		void actions_LISPRG(const unsigned char & page);
		void init_menu_LISPRG(const unsigned char & page);
		void draw_menu_LISPRG(const unsigned char & page);
		void menu_up_LISPRG();	  //_key_UP
		void menu_down_LISPRG();  //_key_DOWN
		void menu_left_LISPRG();//_key_LEFT
		void menu_right_LISPRG();//_key_RIGHT
		void menu_enter_LISPRG(); //_key_ENTER
		void menu_stop_LISPRG();  //_key_STOP
		//================================================================================================================
		//===============================================МЕНЮ "Список подпрограмм LISSUB"=================================
		m_state entry_LISSUB;
		int8_t menu_LISSUB = 0;
		int8_t menu_LISSUB_SZ;
		int8_t menu_scroll_LISSUB;
		void actions_LISSUB(const unsigned char & page);
		void init_menu_LISSUB(const unsigned char & page);
		void draw_menu_LISSUB(const unsigned char & page);
		void menu_up_LISSUB();	  //_key_UP
		void menu_down_LISSUB();  //_key_DOWN
		void menu_left_LISSUB();//_key_LEFT
		void menu_right_LISSUB();//_key_RIGHT
		void menu_enter_LISSUB(); //_key_ENTER
		void menu_stop_LISSUB();  //_key_STOP
		//================================================================================================================
		//==============================================РЕДАКТОР ПРОГРАММ/ПОДПРОГРАММ EDITPR==============================
		m_state entry_EDITPR;
		int entry_cnt_EDITPR;    //счетчик курсора
		int entry_scroll_EDITPR; //счетчик сдвига
		
		int ecEDITPR_tmp; //счетчик курсора , на время работы подпрограммы
		int esEDITPR_tmp; //счетчик сдвига	, на время работы подпрограммы
		
		char PrgorSubPrg; // 0 - Редактирование программ  ,1 - Редактирование подпрограмм
		char type_edit_input; //0-ввод,1-замена,2-вставка,3-редактирование,4 - поиск
		short Max_Frame; //500 для программ , 100 для подпрограмм
		char func_blanc_EDITPR; // режимы доп функций
		bool UPD_2nd;
		char cnt_2ndstr;   // кол -во двухстрочных кадров на экране ниже курсора
		struct header_prg {
			char name[36];//имя программы если есть
			short cnt;	  //кол-во кадров в программе
			short sign;   //сигнатура 0xAAAA
			//char rezerv[92];
		}_header_prg; 
		//save_header_prg;// заголовок для восстановления
		struct resize_prg_v{ 
			int8_t y_offset; // смещение по y
			int8_t SZ;		 // кол -во выводимых строк программы
			int8_t ALIGN;	 // отступ снизу (в строках)		
		}_resz_prg_v;// свойства для поля вывода кадров программы
		
		bool block_str[N_STR]; // блокировка вывода строки на экране вовремя преднабора или вовремя уменьшения координат
		bool small_axis_view;  // вывод осей в уменьшенно виде
		bool set_view;// 1 - опция включена(уменьшенные 4 координаты) 0 - выключена
		void show_small_axis(int x,int y);// функция где выводятся оси в уменьшенном виде по нажатию кнопки "П"

		char ch_mem;
		char init_resz_prg_v();		
		void actions_EDITPR(const unsigned char& page);
		void actions_EDITPR_input(const unsigned char& page);
		void exit_input_EDITPR();
		void init_input_EDITPR(const unsigned char& page);//инициализация ввода кадра
		void init_rep_EDITPR(const unsigned char& page);  //инициализация замены кадра
		void init_ins_EDITPR(const unsigned char& page);  //инициализация вставки кадра
		void init_edit_EDITPR(const unsigned char& page); //инициализация изменить кадра
		void init_find_EDITPR(const unsigned char& page); //инициализация поиска кадра
		void clr_info(const unsigned char& page);
		void upd_2nd_str(const unsigned char& page,int x_, int y_);
		void add_frame_EDITPR();  //добавление кадра
		void repl_frame_EDITPR(); //замена кадра
		void ins_frame_EDITPR();  //вставка кадра
		void com_frame_EDITPR(uint32_t addres);  //закомментировать кадр
		void find_frame_EDITPR(); //поиск кадра
		void erase_all_EDITPR();//очистка программы
		void choice_frame_EDITPR();//выбор программы
		void update_prg_stop_EDITPR();//обновление программы если нажали стоп
		uint32_t get_addres_prg();
		uint32_t get_addres_prg_cur(char PrgorSub,int8_t num);
		void init_menu_EDITPR(const unsigned char& page);
		void draw_prg_EDITPR(const unsigned char& page, char inv,bool OnDisp);
		void draw_prg(char sz_disp_out,short max_prg, int entry_scroll_EDITPR,int index, char inv,char page ,int offset, uint32_t addres,bool OnDisp);
		void delete_frm_EDITPR(); //удаление кадра
		void IncDec_frm_number(char * frm,char incDec); //Увеличение/уменьшение номера кадра
		void menu_up_EDITPR();	  //_key_UP
		void scroll_up(int& cur, int& scroll,int UP_ALIGN, const int mn_sz, const int SZ);
		void scroll_dw(int& cur, int& scroll, const int DW_ALIGN,const int mn_sz, int SZ);
		void menu_down_EDITPR();  //_key_DOWN
		void menu_left_EDITPR();  //_key_LEFT
		void menu_right_EDITPR(); //_key_RIGHT
		void page_up_EDITPR();	  
		void page_down_EDITPR();  
		void menu_enter_EDITPR(); //_key_ENTER
		void menu_stop_EDITPR();  //_key_STOP
		void menu_C_EDITPR();     //_key_C
		void menu_F_EDITPR();     //_key_F
		void blank_right_input_EDITPR(const char& BLANK);//key_BLANK1 key_BLANK2 key_BLANK3 key_BLANK4
		void show_func_blank_EDITPR(const char& page);
		void show_info_prg_EDITPR(const char& page);
		void out_1nd_frame(char* frame_cmd, unsigned char page, int x, int y, const int y_max_up, const int y_max_down);
		void out_2nd_frame(char* frame_cmd, unsigned char page, int x, int y, const int y_max_up, const int y_max_down);
		void out_num_frame(char* frame_cmd, unsigned char page, int x, int y, const int y_max_up, const int y_max_down);
		//================================================================================================================
		//===============================================DIALOG_ERASE_ALL_PROGRAMM (DEALLP)===============================
		void menu_init_DEALLP();
		void menu_START_DEALLP();
		void menu_STOP_DEALLP();
		//================================================================================================================
		//===============================================COMMAND LINE(ввод команды)=======================================
		m_state entry_cmd;
		uint8_t func_cmd; // текущий набор кнопок
		uint8_t trigger_func_cmd;// вспомогательная переменная для переключения набора кнопок 
		short cnt_frame;
		char frame_cmd[MAX_FRAME][FRAME_SZ]; //буфер преднабора
		bool if_input_used_cmd;// 1- если был использован ввод преднабора
		void show_cmd(const unsigned char & page,bool show,bool showInstr,bool inv = 0);
		void put_F();
		void show_Instr(const unsigned char & page,int8_t SZ);
		bool UPD_show_Instr;
		int  get_one_cmd(char *frm,char *one_cmd, int MAX_SZ);
		void show_current_frame_input(const unsigned char & page,long cnt);
		void clr_cmd(const unsigned char & page);
		void add_frame();
		void add_edit_frame();
		void exe_F_only(unsigned char *str,const int& sz);// надо доделать
		void set_blank_cmd(char *frame,int sz_frame);
		void set_XYZIJ_format_cmd(char *frame,int sz_frame);
		void set_num_str(uint8_t *frame, int sz_frame,const short & cnt);
		int  to_format_cmd(char * buf, char * frame, int & i,int format);
		void set_num_frames();

		void actions_cmd(const unsigned char & page);
		void init_cmd(const unsigned char &page);
		void input_cmd(const unsigned char &page);
		void edit_input_cmd(const unsigned char &page);
		void show_func_cmd(const char & page);  // функции для ввода кадров
		void show_func_cmd_3btn(const char& page);
		void update_func_cmd(const char & page);
		void update_func_cmd_3btn(const char& page);
		void exit_cmd(const char & page);

		void blank_left_input_cmd(const char& BLANK); //key BLANK1_L ,BLANK2_L,BLANK3_L,BLANK4_L
		void blank_right_input_cmd(const char& BLANK);//key_BLANK1 key_BLANK2 key_BLANK3 key_BLANK4
		void change_func_cmd_F(); //key F
		void key_F4_cmd();        //key F4
		void detect_format();	
		bool if_Type_Circ();	
		//===================================================================================================================
		//==============================================EXECUTE COMMAND======================================================		
		m_state entry_exe;
		int8_t frame_CURSOR_exe = MAX_FRAME - 1;
		int8_t scroll_exe;
		uint8_t func_exe;
		bool first_frame;// текущий кадр - первый
		bool last_frame;// текущий кадр - последний
		char call_sub;//кол-во вызванных подпрограмм
		
		uint16_t timer_frm; // сколько осталось задержке
		uint16_t timer_sv; // задержка после кадра команда G4.xx , где xx врме в сек
		uint16_t cnt_tf;// периоды для вычитания из задержки (timer_frm)

		int16_t cnt_galet;// задержка для переключения осей
		char axis_d;// текущий канал с задержкой

		int16_t cnt_M3M4;// задержка для M3 M4 через M5
		bool M3_mem;// выбранное направление на время задержки

		int16_t cnt_wait_shpindel;// задержка на ожидание включения шпинделя
		int16_t cnt_wait_shpindel_off;// задержка на ожидание выключения шпинделя

		int cnt_wait_afterG0G1[N_AXIS]; // пауза после G0G1
		enum SHPINDEL_DIR:int8_t{
			spSTOP, // M5
			spCW,//M3
			spCCW,//M4
		};
		int16_t cnt_M5;// задержка для параметра "Время для М5, сек"
		bool lock_error_SNO; // блокировка ошибки "шпиндель не остановлен"
		bool lock_error_PLC; // блокировка ошибки "контроллер не отвечает"
		bool lock_error_STN; // блокировка ошибки "станок не готов"

		char *p_M102_gl; // ожидание входа
		
		long cnt_s[N_AXIS];	// сколько проехали за период
		long cnt_z[N_AXIS]; // сколько осталось проехать (задание)
		long cnt_p[N_AXIS]; // сколько проехали в предыдущем периоде
		long tmp_pc[N_AXIS];// значение счетчика на начале движения
		char sts_c[N_AXIS + N_SA]; // статус 1 - канал в движении,  0 - нет
		char zn[N_AXIS][ZONES];	// текущие зоны для каждого канала
		char cnt_dvd[N_AXIS]; // счетчик доводок
		long cnt_impulse_prev[N_AXIS]; // предыдущие задание из кадра
		long cnt_impulse_cur[N_AXIS];// текущее задание из кадра
		long cnt_CNT_prev[N_AXIS]; // перемещение рассчетное, записыается текущая точка при старте потом приращается задание из кадра
		int8_t dir_exe[N_AXIS + N_SA]; // текущее направление в +(1)вправо или -(-1)влево (переменная определяется во всех режимах движения)
		short tmr_axis_pusk[N_AXIS]; //счет таймера на включение выходных сигналов на привод ПУСК+ или ПУСК- и ПУСКА
		short tmr_ref_pusk[N_AXIS];  //счет таймера на инициализацию реф меток
		uint8_t chs_vibor;// каналы у которых выставлен выход выбор оси
		long IJ[2]; // I,J для круговой интерполяции

		struct _v_luft{// ВЫБОРКА ЛЮФТА
			bool ON[N_AXIS];      //1 - установлена скорость выборки люфта
			long abs_cnt[N_AXIS]; // счетчики для определения перемещения во время выборки люфта
			long spd_sv[N_AXIS];  // сохраненная скорость во время выборки люфтом
			long set(uint8_t c,long spd);
			void process(int i);
			void init(int8_t e = N_AXIS);
		}v_luft;		

		char limit_sw[N_AXIS]; // произошел наезд на концевик, для поиска реф метки после наезда на концевик
		short lubric_tmr[N_AXIS]; //текущее время смазки
		
		struct _config_NC{
			unsigned long one_frame    : 1;
			unsigned long rezerv       : 30;
		}config_NC;
		
		struct choice_prg {
			uint32_t addres_prg;  // адрес выбранной программы
			short	 cnt; // кол -во кадров в выбранной программе
		}_choice_prg, // текущее
		cp_tmp; // временное хранения перед вызовом подпрограммы
		bool btn_zapusk_prg; // 1 - если нажата кнопка  выбора и запуска программы (без редактирования)  0 - нажата кнопка редактирования программы
		
		struct modals_cmd { // Модальные команды
			char G90; //0 - G90, 1 - G91
			char G0 = 1;  //0 - G0 , 1 - G1
			struct _H {
				int fr;  //номер стартового кадра
				char cnt;//кол - во повторений
			}H;
			SHPINDEL_DIR M3; // 0 - M5 , 1 - M3, 2 - M4
			uint8_t T_instr = 1; //выбранный инструмент в программе
			char G17 = 17; // 17 -XY  18 - ZX  19 - YZ
			char G40 = 40; //G40 - отмена коррекции радиуса ,G45 +, G46 -, ,G41 - коррекция на радиус, инструмент слева от детали, G42 - коррекция на радиус, инструмент справа от детали
			char G43 = 49; //G43 - коррекция на инструмент +,G44 - коррекция на инструмент -, G49 - отмена коррекции длины
			long F;// заданная скорость 0.0000
			long F_sv;//сохраненная заданная скорость на время движения 0.0000
			short Feed;//процент подач
		}_modals_cmd;

		long F_cur[N_AXIS + N_SA];//текущие скорости для каждой оси
		long F_li[N_AXIS + N_SA];//рассчитанная скорость для одновременного движения
		
		struct _ch_sw{ //аппаратные концевики
			char plus; // +
			char minus;// -
			bool if_sw_ON();
		}ch_sw;

		struct _ch_sw_prog{ //программные концевики
			char plus; // +
			char minus;// -
		}ch_sw_prog;

		bool SHPINDEL;		  //общий вход "Шпиндель вращается" 1-  шпиндель вращается 0 - нет
		bool INSTRUMENT_ZAJAT;//общий вход "Инструмент зажат"
		bool TORMOZ[N_AXIS + N_SA];  //осевой вход "Ось на тормозе"
		bool STANOK_GOTOV;    //общий вход "Готовность станка"
		bool STOP_PODACHA;    //общий вход "Стоп подача"
		bool STOP_SHPINDEL;   //общий вход "Стоп шпиндель"

		// если сюда записать 1 - выставиться выход Ручной режим  ,0 - выставиться выход Авто режим  
		// если в покое то 0 - ручной режим , если 1 - авто режим
		char SEToutRA;  
		struct _cms{
			bool on; // 1 - включен режим изменения вывода координат в движении 0 - выкл
			char cur_move_show[N_AXIS]; //режим CUR_COR(10)/REM_COR(11)/ZAD_CORR(12),
			char cur_fync; // 0 - режим смены вывода координат , 1 -  доп режим
		}cms;
		bool DBG_; //0 - отладка входов /выходов на главном экране
		char DIR_FIND_REF; // направление выбора поиска реф метки 0 - нет,  1- влево  2 - вправо

		bool upd_m_icons; // обновление иконок в движение только в конце позиционирования
		bool upd_sw_status; // обновление статуса в программном режиме после съезда с концевиков
		
		char trg_p[N_AXIS]; //защелка для программных концевиков+
		char trg_m[N_AXIS]; //защелка для программных концевиков-
		
		struct _memPause_releaseG91{
			int num; // номер кадра
			bool on; // пауза включена
		}mPR;

		void actions_exe(const unsigned char & page);
		void Init_exe(const unsigned char & page);
		void show_func_exe(const char & page);
		void change_exe(const unsigned char & page);
		void exit_exe(const unsigned char & page);
		void execute_exe(const unsigned char & page);
		void Set_drv_axis_inv();
		void Instr_corr(long &tmp,int i);
		void set_surf(CInit::AXIS_NAME axis0,long &tmp,int i);
		void Radius_Instr_corr(long &tmp,int i);
		void set_surf_radius(CInit::AXIS_NAME axis1,CInit::AXIS_NAME axis2,long &tmp,int i);
		//============обработка кнопок во время движения===============
		void put_moving_icons(const unsigned char & page);  // иконки во время движения
		void change_moving_icons(int s); 
		void change_moving_functions();
		void toggle_view();
		void set_view_prog(); // УСТАНОВКА УМЕНЬШЕННЫХ КООРДИНАТ + отображение программы
		void no_view_prog();
		void Feed_DOWN();
		void Feed_UP();
		bool passedZN(int8_t z);
		void upd_Spd_inMove();
		void pause_G91();
		void release_G91(long *ch_impulse, uint8_t chs);
		//=============================================================		
		//=============================позиционирование================
		void timers_c();
		void timers_c_10();
		void prog_sw_zone_c();// торможение перед программным ограничением
		void prog_sw_c();     // обнаружение программных ограничителей
		void overf_c();       // обнаружение переполнения круговой циклической оси
		void servo_c();
		void move_controller(int i);
		void G0G1(int i);
		void cnt_wait_afterG0G1_f(int i);
		void dovodka_f(int i);
		void vne_dopuska_f(int i);
		void positioning_is_over(int i);
		char wait_drive_lir541();
		void check_dir_pusk();
		void check_dir_pusk_();
		long get_STRT_spd(char G0,uint8_t c);
		long get_ZONE_spd(uint8_t c,uint8_t z);
		long get_STOP_spd(uint8_t c);
		long conv_c(long tmp,int i,char type = 0,char endless_circle = 0);
		bool lirCommand(long *ch_impulse, uint8_t chs,char G,long& F);
		void set_cnt_z_exe(long *ch_impulse, uint8_t chs);
		void check_cnt_z_exe(uint8_t chs);
		bool set_dir_exe(int8_t c);
		void calc_Linear_inter(char G,const long& F);
		int set_lirCommand(int i);
		void set_lir541dovodka(int c);
		bool in_STRT(uint8_t c);
		//=============================================================					
		long get_dopusk(int i);
		long get_mod_zadanie(int i);				
		long Spd_to_U(long outcmd,uint8_t c, char G0,bool zones = 0);	
		long getF_li(uint8_t c);
		long Ln4_form(long tmp,uint8_t c);				
		void init_Feed();			
		bool print_prog_sw_error(int n_str,unsigned char v,long data);
		void put_info_sw_error(long& data,unsigned char v,int y);
		bool ifStrch(int n_str);		
		//======================входа/выхода===========================
		void input_handler_IRQ();
		void input_handler();
		void LUBRICATE_out();
		void OTMENA_VIBOR_AXIS_input();
		void VIBOR_AXIS_input();
		void PUSK_AXIS_input();
		void TOLCHOK_AXIS_input();
		void OGRANICHITEL();
		void G0_TOLCHOK();
		void SHPINDEL_input();
		void SHPINDEL_Stop();
		void outPozitionirovanie();
		void outRychnoyAutorejim();
		void STANOK_GOTOV_input();
		void START_PROG();
		void toggleGOTOVNOSTbSPPU();
		void delay_PUSK(int c);
		void delay_PUSK_find_ref(int c);
		void delay_INIT_REF(int c);
		void debugOUT_INPUT(int y);
		void on_off_debugOUT_INPUT();
		void PUSK_axis(int c);
		void STOP_axis(int c);
		void VIBOR_axis(int c,bool set,bool PORTOUT = 0);
		void rst_VIBOR_axis(uint8_t chs,bool PORTOUT);
		void outP(char pin_out, bool set,bool PORTOUT = 0);
		void outP_all(int32_t p_out_,int32_t m_out_,int16_t U_out_a1,int16_t U_out_a2);
		void outP_all();
		void rst_outP();
		void outABCD(char outcmd,char *pin_out,char c,bool PORTOUT = 0);
		void outAnalog(long outcmd,char a,char polar,int8_t dir ,bool PORTOUT = 0);
		void outP_G0(char G0,bool PORTOUT = 0);
		bool outM3(bool CW,bool STOP,bool PORTOUT = 0);
		char outM3_delay(char res,bool M3);
		bool outM5_delay();	
		void set_timer_off_M5();
		inline bool unlockSPH();
		//=============================================================								
		//======================выход в 0==============================
		void lir541comRef(char reverse = 0,char mode = 0);
		bool if_vibor_ref();
		bool if_LimitPM(int8_t c);
		//=============================================================						
		//======================интерпретатор==========================
		uint8_t get_bin_from_cmd(long *cnt,char *frame,int sz_frame);
		bool get_cmd(char CMD, long& tmp, char* frame, int sz_frame,char &p);
		void get_F_from_cmd(long *cnt,char *frame,int sz_frame);
		void get_G_from_cmd(char *G,char *frame,int sz_frame);
		int get_M_from_cmd(char *M,char* frame, int sz_frame);
		int set_wait_M_input(int M);
		void set_M_output(int M,bool set);
		char M3M5M4_cmd(char M, m_state& state);
		bool get_L_from_cmd(char* frame, int sz_frame);
		bool get_E_from_cmd(char* frame, int sz_frame);
		void get_H_from_cmd(char* frame, int sz_frame);
		void get_T_from_cmd(char* frame, int sz_frame);
		bool get_G4xxdelay_from_cmd(char* frame, int sz_frame);
		bool tmr_frame();
		void searcher(short tpm_num);
		void rewind_comm();
		void if_pass_frm();
		bool last_frm();
		bool is_frame_move();    //!!!
		bool is_frame_move_prog();
		bool is_frame_move_prednabor();
		bool is_move_hand();
		bool if_TORMOZ(uint8_t chs);
		bool if_ENC(uint8_t chs);
		bool if_ABS(long *ch_impulse,uint8_t chs);
		bool if_CIRCLE(long *ch_impulse,uint8_t chs);			
		void reset_circle_hw_couters(int ch);
		void G90_circle(int ch,long *ch_impulse);
		bool is_single_axis(uint8_t chs);
		//=============================================================
		//========================MCX514===============================
#if (configUSE_mcx514 == 1)
		long LP_axis[N_AXIS]; // логическая позиция (задание) чипа Mcx514					
		void Set_Axis_Conf_mcx514(int8_t c);
		void outMcx514(int axis,const long& pulse,const long& F,const long&Accel,const long&InitSpd); //==RELATIVE DRIVING
		void outMcx514_cont(int axis,int8_t c,const long& F,const long&Accel,const long&InitSpd); 	//==CONTINIUS DRIVING
		void servo_c_mcx514();
		long acceleration(const unsigned char &ch);
		long Spd_to_Hz(long *lc_F,signed char ch,char apmlifire = 0);
		void linear_interpolation(uint8_t chs);
		void circle_interpolation(uint8_t chs,char G);
		long get_f_MD(int8_t c);
		long mul_LP(int8_t c,long long in);
		long div_LP(int8_t c,long long in);
		long mul_MAXO(int8_t c,long long in);
		long div_MAXO(int8_t c,long long in);
#endif		
		//==============================================================		

		void menu_stop_exe();                          //_key_STOP
		void choice_exe_up();                          //_key_UP
		void choice_exe_down();                        //_key_DOWN
		void menu_left_exe();						   //_key_LEFT
		void menu_right_exe();						   //_key_RIGHT
		void go_cmd_from_exe();                        //_key_BLANK4
		void delete_cmd_exe();					       //_key_C
		void menu_start_exe();						   //_key_START
		void go_edit_frame_exe(const char & ENTER);    
		void blank_right_EXE(const char & BLANK);      //_key_BLANK1 _key_BLANK2 _key_BLANK3 _key_BLANK4
		void menu_enter_exe();						   //_key_ENTER		                    		
		//==============================================================================================================================
		//==============================================РУЧНОЕ ПЕРЕМЕЩЕНИЕ(idHandMove)==================================================
		int8_t entry_mh;
		int8_t dir_mh;//выбор направления
		int8_t dir_mh_sv;//сохраненное выбор направления для обратного переключения
		int ch_to_move;//выбранный канал для движения 0x01- 0 канал, 0x02- 1 канал, 0x04- 2 канал, 0x08- 3 канал, 0x10- слепая ось,
		int invers_btn;//инверсная строка
		int8_t btn_hold_mh; //толчок
		int8_t btn_cont_mh; //движение до отмены
		int8_t st_Drive; // канал с заданием
		struct _extPULT{
			bool on; //1 - включен внешний пульт 0 - нет
			char G0; // G0 во время работы пульта
		}extPULT;
		bool stop_in_move; // 1 - произошла остановка по кнопке стоп в преднаборе
		bool HM_toF; // 1 - переключение из  режима "Ручное перемещение" в FUNC

		struct _MAXO{  //в стиле ООП
			long mul = 100;
			long prev[N_AXIS];
			bool on; //1 - включен маховик 0 - нет
			bool operator==(bool b);
			_MAXO& operator=(bool set);
			bool use();
			void sync();
			void process(int8_t c);
			bool hw_on(int8_t c);
			private:
				bool safety(int8_t c);
		}MAXO;

		bool ifG1();
		void Led_START_STOP();
		void actions_mh(const unsigned char & page);
		void set_axis_mh(const unsigned char & page);  
		void sym_invers(int s,char set,bool clr);
		void set_mh();
		void set_HAND_MOVE();
		void print_status_HAND_MOVE();	
		void change_pic_axis_mh(const unsigned char & page); //_key_BLANK1,_key_BLANK2,_key_BLANK3,_key_BLANK4
		void update_pic_axis_mh(const unsigned char & page,bool PORTOUT);		
		void stop_mh();                   //_key_STOP
		void F_mh();                      //_key_F
		void BLANK1_mh(const char& BLANK);//_key_BLANK1
		void BLANK2_mh(const char& BLANK);//_key_BLANK2
		void BLANK3_mh(const char& BLANK);//_key_BLANK3
		void set_RIGHT_dir_mh();          //_key_RIGHT
		void set_LEFT_dir_mh();           //_key_LEFT
		void move_axis_mh();              //_key_ENTER_PRESS  ТОЛЧОК
		void move_axis_mh_RELEASE();      //_key_ENTER_RELEASE
		void key_UP_mh();				  //_key_UP
		void key_DOWN_mh();               //_key_DOWN
		void tolchok_release();
		void move_axis_START();
		void move_axis_STOP(char sw_ch = 0xFF);
		char sw_s(char sw_ch);		
		bool INmove();   //!!!
		bool INmovePrednabor();
		bool INmoveContext();
		bool INContiniusmove();
		bool INMovingKeys();
		void OFF_D_LEFT_RIGHT();		
		void axis_drive_stop_lir(bool Prog = 0,char sw_ch = 0xFF);
		void uart_on_off(const char &set);
		bool Continius_drive_move(int axis,char G0,bool tolchok);		
		//==============================================================================================================================
#endif
#if (configUSE_mcx514 == 1)
		//=============================================МЕНЮ "НАСТРОЙКИ УПРАВЛЕНИЯ"=============================================			
		int8_t entry_cntr;
		int8_t menu_cntr_cnt = 1;
		void actions_cntr(const unsigned char & page);
		void actions_Cntrl_input(const unsigned char & page);
		void init_m_cntr(const unsigned char & page);
		void draw_m_cntr(const unsigned char & page);
		void draw_param_cntr(const unsigned char & page,int x,char align);
		void m_up_cntr();	   //_key_UP
		void m_down_cntr();    //_key_DOWN
		void m_enter_cntr();   //_key_ENTER
		void m_stop_cntr();    //_key_STOP
		void m_left_cntr();    //_key_LEFT
		void m_right_cntr();   //_key_RIGHT		
		//===============================================ПОДМЕНЮ "МАХОВИК"===================================================
		int8_t entry_cntr_hw;
		int8_t menu_cntr_cnt_hw = 1;		
		void actions_cntr_hw(const unsigned char & page);
		void actions_Cntrl_input_hw(const unsigned char & page);
		void init_m_cntr_hw(const unsigned char & page);
		void draw_m_cntr_hw(const unsigned char & page);
		void draw_param_cntr_hw(const unsigned char & page,int x,char align);
		void m_up_cntr_hw();	  //_key_UP
		void m_down_cntr_hw();    //_key_DOWN
		void m_enter_cntr_hw();   //_key_ENTER
		void m_stop_cntr_hw();    //_key_STOP
		void m_left_cntr_hw();    //_key_LEFT
		void m_right_cntr_hw();   //_key_RIGHT
		//===================================================================================================================
		//===================================================================================================================
#endif
		//===================================================================================================================
		struct _LH_CH_setup{
			unsigned char sound     : 1;
			unsigned char rezerv    : 7;
		}LH_CH_setup[2];
		void save_LH_CH_setup_to_eeprom(unsigned char sz = sizeof(LH_CH_setup));
		void load_LH_CH_setup_from_eeprom(unsigned char sz = sizeof(LH_CH_setup));
		//===============================================LINEAR_HOLES========================================================
#if (configUSE_Linear_holes == 1)
		int8_t type_lh = 1;                                  // тип
		int entry_lh;										 // 
		uint8_t menu_lh = 1;                                 // счетчик выбранного пункта
		int8_t primary_axis_lh;                              // главная ось
		int8_t secondary_axis_lh;							 // второстепенная ось

		void Init_LH(const unsigned char &page);
		void draw_menu_lh(const unsigned char &page);
		void draw_param_lh(const unsigned char &page);
		void menu_lh_up();
		void menu_lh_down();
		void action_Linear_holes();
		void change_param_lh_left_right(const char &r_l);
		void enter_to_input_loop_lh();                        // вход в режим ввода чисел
		void action_input_linear_input(const unsigned char &page);
		void menu_lh_blank(const char& BLANK);
		bool exit_lh_PUSK();
		void action_input_linear_PUSK(const unsigned char &page);
		void getcoord_inlinear_one();
		void drive_next_hole_lh();
		

		long primary_first_point_lh,secondary_first_point_lh;  // координаты первого отверстия
		unsigned int holes_lh;                                 // кол -во отверстий в ряду
		long interval_holes;								   // расстояние между отверстиями
		unsigned int rows_lh;								   // кол -во рядов
		long interval_rows;									   // расстояние между рядами
		double ang_lh;                                         // угол
		unsigned int count_holes_lh;                           // выбранное отверстие
		long x_old_lh;
		long y_old_lh;

		long interval_step;
		long row_step;
		bool dir_lh = 1;
		bool dir_lh_btn;
		bool horizon;

#endif
		//===================================================================================================================
		//===============================================CIRCLE_HOLES========================================================
#if (configUSE_Circle_holes == 1)
		int8_t type_ch;
		int entry;
		uint8_t menu_ch = 1;
		int8_t primary_axis_ch;
		int8_t secondary_axis_ch;
		
		void getcoord_incircle_all(int number_of_holes,int radius,int start_angel);
		void getcoord_incircle_one();
		void sound_holes();
		void action_input_radius();
		void action_input_radius_input(const unsigned char &page);
		void action_input_radius_PUSK(const unsigned char &page);
		void enter_to_input_loop_ch();      // вход в режим ввода чисел
		void Init_CH(const unsigned char &page);
		void check_axis_ch(int8_t & axis_ch);
		void draw_menu_ch(const unsigned char &page);
		void draw_param_ch(const unsigned char &page);
		void menu_ch_up();//UP
		void menu_ch_down();//DOWN
		void menu_ch_blank(const char& BLANK);
		bool exit_ch_PUSK();
		void change_param_ch_left_right(const char &r_l);  //LEFT RIGHT
	    
		long R;                                  // радиус мм
		double start_ang;                        // начальный угол
		double step_ang;                         // шаг угла
		double cur_ang;                          // текущий шаг
		unsigned int  n;                         // кол - во отверстий
		unsigned int count_holes;                // выбранное отверстие
		long primary_centr_ch,secondary_centr_ch;// центр
		long x_old;
		long y_old;

#endif
        //==================================================================================================================
		uint8_t change_axis_ch(int8_t &a,int8_t&b,const char znak);
		void change_axis_str(uint16_t &_str,const bool znak);

		//=================================================CALCULATOR=======================================================
		unsigned char txt_out_clc_res[BUF_CLC_RES];
		char calc_entry;
		int y;
		unsigned char mode_calc;
		char page_calc;
		void action_calc(char page);
		void calc_copy_result();
		void clc_shift_result();
		void show_mode(const char &page);
		void get_str_a_b_operator_calc(char buf[],char &operator_,char buf_a[],char buf_b[],bool &point);
		void calculate_integer_number(char buf_a[],char buf_b[],char &operator_,bool &point,unsigned char txt_out[]);
		void calculate_real_number(char buf_a[],char buf_b[],char &operator_,unsigned char txt_out[]);
		void calculate_mode_1(char *buf_a, char &operator_, unsigned char *txt_out);
		void calculate_mode_2(char *buf_a, char &operator_, unsigned char *txt_out,bool &point_);
		bool shift_str_left(unsigned char * str);
		
		void calculate_real_number_fixed(char buf_a[], char buf_b[], char & operator_, unsigned char txt_out[]);
		void put_dot(char * str);
		unsigned char count_before_dot(char * str);
		long long div_calc(long long& a, long long& b);
		long long mul_calc2(long long& a, long long& b);
		int count_addedzero_(long long& num);
		int count_addedzero(long long& num);
		int count_addedzero2(long long num,char action);

		void Init_calc(char & entry,int &y_coord,const char &page);
		void Init_new_string(char & entry,int &y_coord,const char &page);
		void get_nums_calc(char* buf,unsigned char* txt_out);
		int check_overflow_long(long long &result);
		void save_load_calc(const unsigned char& action);
		void calc_blank_input(char k_c);
		void cut_zero(unsigned char* txt_out);
		void real_to_longlong(signed long long &result64bit,double &result_d);
		void check_str_num(char buf_num[],unsigned char* txt_out);
		bool compare(const long long &a,const long long &b);
		void calc_cursor_up();
		void calc_cursor_down();
       //====================================================================================================================
	

	   //===============================================SNAKE================================================================
	   short dire[2] = { 0,1 };
#if( configUSE_SnakeGame == 1)
	   
	   #define X_SNAKE_MAP 28
	   #define Y_SNAKE_MAP 12
	   struct snakeBlock {
		   int x;
		   int y;
	   };

	   struct snake
	   {
		   int size;
		   struct snakeBlock block[51]; 
	   }snake1;
	   short cash = 1;
	   int speed;
	   char map[Y_SNAKE_MAP][X_SNAKE_MAP + 1] = {  "############################",
												   "#                          #",
												   "#                          #",
												   "#                          #",
												   "#           *              #",
												   "#                          #",
												   "#                          #",
												   "#                          #",
												   "#                          #",
												   "#                          #",
												   "#                          #",
												   "############################"};
	   void initsnakegame();
	   void snakeLirInit(int x, int y);
	   bool snakeMoveLir(short *);
	   bool checkloose(int x, int y);
	   void drawmap();
	   void draw_skbis(char obj);
	   bool checkmap(int x, int y);  
	   void exit_game();
#endif
	   bool sn_win;
	   void direup();
	   void diredown();
	   void direleft();
	   void direright();
	   //=====================================================================================================================

};

extern Menu Lir;
#endif /* MENULIR_H_ */

