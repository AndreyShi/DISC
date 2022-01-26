#ifndef __counter_h
#define __counter_h
//03.01.2018
//#include "programm.h"
//#include "io_altera.h"
#include "main.h"
#include "translate.h"
//#include "MenuLir.h"
class CLUFT{
public:
	enum LUFT_STATE{
		NO_LUFT,
		CHECK_RIGHT,
		CHECK_LEFT
	};
	LUFT_STATE fsm_luft;    //текущее направление люфта
	long approach;			//координата конца люфта
	long luft;				//положение внутри люфта
	long last_before_rev;   //координата до входа в люфт
	long gap;				//люфт в осевых параметрах
};
extern CLUFT mLuft[N_AXIS];

class CCNT_SENSOR {
public:
	enum COUNT_STATE{
		NO_COUNT,
		ACTIVE
	};
	enum DIRECTION_CNT: int8_t{
		NO_DIRECTION = 0,
		DIRECTION_CNT_LEFT = -1,
		DIRECTION_CNT_RIGHT = 1,
	};
    long hw_cnt_cur;
    long hw_cnt_prev;
    long rel;
    long abs;
	long abs_pure;	
	long delta;
	
    DIRECTION_CNT prev_dir;
	DIRECTION_CNT cur_dir;
    COUNT_STATE fsm_counter;
	
    bool increment_0_360_one_turn;
	long max_value;
	
	bool ovfl_abs; // переполнение кругового циклического счетчика
	bool ovfl_rel; // 
	bool error;
    long correction_simple,correction_complex,correction_right;   
    long corr_prev;	
	long delta_corr;
	DIRECTION_CNT get_dir();
	void ref_reset();		
};
extern CCNT_SENSOR mCounter[N_AXIS];

class  CCounters {
public:
	long run_luft_new(long ,const int &ch);
	void reset_counters_rel(char *k_c);
	bool to_do_correction_in_realtime_ch(const int &ch);
	void do_circle_counters(bool correction_is_made,uint8_t ch);
	long read_cnt(long *incoming,const int &ch);
};
extern  CCounters pult_counters;


#endif
