#include "counter.h"
#include "io_altera.h"
#include "programm.h"
#include "config.h"
#include "MenuLir.h"
#pragma GCC push_options
#pragma GCC optimize ("O0")

long CCounters::run_luft_new(long delta_luft,const int &ch)
{
	mLuft[ch].luft += delta_luft;
	if(mLuft[ch].fsm_luft == CLUFT::CHECK_LEFT){
		if(mLuft[ch].luft >= mLuft[ch].last_before_rev){ //выход из люфта если достигли значения входа в люфт
			delta_luft=mLuft[ch].luft-mLuft[ch].last_before_rev;
			mLuft[ch].fsm_luft = CLUFT::NO_LUFT;
			return delta_luft;
		}
		else if (mLuft[ch].luft<=mLuft[ch].approach){ //выход из люфта если достигли значения выхода из люфта
			delta_luft=mLuft[ch].luft-mLuft[ch].approach;
			mLuft[ch].fsm_luft = CLUFT::NO_LUFT;
			return delta_luft;
		}
	}
	//here is CHECK_RIGHT
	if(mLuft[ch].fsm_luft == CLUFT::CHECK_RIGHT){
		if(mLuft[ch].luft <= mLuft[ch].last_before_rev){//выход из люфта если достигли значения входа в люфт
			delta_luft=mLuft[ch].luft-mLuft[ch].last_before_rev;
			mLuft[ch].fsm_luft = CLUFT::NO_LUFT;
			return delta_luft;
		}
		else if (mLuft[ch].luft>=mLuft[ch].approach){//выход из люфта если достигли значения выхода из люфта
			delta_luft=mLuft[ch].luft-mLuft[ch].approach;
			mLuft[ch].fsm_luft = CLUFT::NO_LUFT;
			return delta_luft;
		}
	}
	return delta_luft;
}
#pragma GCC pop_options

//15.03.2018 to reset counters...
void  CCounters::reset_counters_rel(char *k_c){

if(Lir.curItem->id == idMenuEncoders || Lir.curItem->id == idViewInputs){
		if(*k_c == _key_BLANK4_L)
		{
				cli();
				mCounter[3].rel=0;
				sei();
		}
		else if (*k_c == _key_BLANK3_L)
		{
				cli();
				mCounter[2].rel=0;
				sei();
		}
		else if(*k_c == _key_BLANK2_L)
		{
				cli();
				mCounter[1].rel=0;
				sei();
		}
		else if(*k_c == _key_BLANK1_L)
		{
				cli();
				mCounter[0].rel=0;
				sei();
		}
	}
}
/*
in const int &ch - канал
*/
bool CCounters::to_do_correction_in_realtime_ch(const int &ch)
{
	if(configUSE_menuItem_Correction_and_Izmerenia == 0)
		return false;
#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
	if(flash_prg.axis_cfg[ch].is_correction == CPrg::CORR::NO){
		altera.error_corr_real_time[ch] = CIOAltera::ERROR_CORR_REAL_TIME::CORR_OFF;
		return false;
		}
	if(!altera.correction_can_be_done_in_real_time[ch]){
		altera.error_corr_real_time[ch] = CIOAltera::ERROR_CORR_REAL_TIME::CANT_BE_DONE;
		return false;
		}
	/*if(!(altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::NO_ERROR||
	     altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_PLUS_||
	     altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_MINES_||
	     altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_MINES_R_||
	     altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::TABLE_IS_ENDED_PLUS_L_||
	     altera.error_corr_real_time[ch] == CIOAltera::ERROR_CORR_REAL_TIME::DIFFER_COEFF))
		 return false;  */
	//------------------------------------------------------------------------------------------------

	if(!flash_prg.to_do_correction_in_realtime_ch(ch)){
		//altera.error_corr_real_time[ch]=CIOAltera::ERROR_CORR_REAL_TIME::SOME_ERROR;
		return false;
	}

	return true;
#endif
}

void CCounters::do_circle_counters(bool correction_is_made,uint8_t ch)
{	
	if(mCounter[ch].abs_pure > mCounter[ch].max_value){
		mCounter[ch].abs_pure = mCounter[ch].abs_pure - mCounter[ch].max_value;
		mCounter[ch].ovfl_abs = true;
		}
	else if(mCounter[ch].abs_pure < -mCounter[ch].max_value){
		mCounter[ch].abs_pure = mCounter[ch].abs_pure + mCounter[ch].max_value;
		mCounter[ch].ovfl_abs = true;
		}	
		
	#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
	if(mCounter[ch].increment_0_360_one_turn){
		if(mCounter[ch].abs_pure<0)
			mCounter[ch].abs_pure=mCounter[ch].abs_pure + mCounter[ch].max_value;
	}
	#endif

	if(correction_is_made == false){
		mCounter[ch].abs = mCounter[ch].abs_pure;//?
	}

	if(mCounter[ch].abs > mCounter[ch].max_value)
		mCounter[ch].abs = mCounter[ch].abs - mCounter[ch].max_value;
	else if(mCounter[ch].abs < -mCounter[ch].max_value)
		mCounter[ch].abs = mCounter[ch].abs + mCounter[ch].max_value;

	mCounter[ch].rel +=  mCounter[ch].delta;//?

	if(mCounter[ch].rel > mCounter[ch].max_value){
		mCounter[ch].rel = mCounter[ch].rel - mCounter[ch].max_value;
		mCounter[ch].ovfl_rel = true;
		}
	else if(mCounter[ch].rel < -mCounter[ch].max_value){
		mCounter[ch].rel = mCounter[ch].rel + mCounter[ch].max_value;
		mCounter[ch].ovfl_rel = true;
		}
		
	#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
	if(mCounter[ch].increment_0_360_one_turn){
		if(mCounter[ch].rel<0)
		mCounter[ch].rel = mCounter[ch].rel + mCounter[ch].max_value;
	}
	#endif


	//для коррекции нужны координаты с преобразованием

	/*if(mCounter[ch].abs_pure>mCounter[ch].max_value)
		mCounter[ch].abs_pure=mCounter[ch].abs_pure-mCounter[ch].max_value;
	else if(mCounter[ch].abs_pure<-mCounter[ch].max_value)
		mCounter[ch].abs_pure=mCounter[ch].abs_pure+mCounter[ch].max_value;
	else if(mCounter[ch].abs_pure < 0)
		mCounter[ch].abs_pure=mCounter[ch].max_value+mCounter[ch].abs_pure;

	
	long a,b,c;
	bool mines;
	mCounter[ch].rel +=  mCounter[ch].delta;
	mines=false;
	a=mCounter[ch].rel;
	if (a<0){
		a=-a;
		mines=true;
	}
	b=a/mCounter[ch].max_value;
	c=mCounter[ch].max_value*b;
	a=a-c;
	if (mines==true)
		mCounter[ch].rel=mCounter[ch].max_value-a;
	else
		mCounter[ch].rel=a;

	if(correction_is_made==false){
		mCounter[ch].abs=mCounter[ch].abs_pure;
	}
	mines=false;
	a=mCounter[ch].abs;
	if (a<0){
		a=-a;
		mines=true;
	}
	b=a/mCounter[ch].max_value;
	c=mCounter[ch].max_value*b;
	a=a-c;
	if (mines==true)
		mCounter[ch].abs=mCounter[ch].max_value-a;
	else
		mCounter[ch].abs=a;    */
}

long CCounters::read_cnt(long *incoming,const int &ch)
{
	//01.08.2017 altera 24 bit
	//22.11.2017 with or without backlash if linear it takes 28-31 mks.
	//if angle with or without backlash it takes 71-73 mks.
	#if (configUSE_ABS_Encoder == 1)
	if(Lir.abs_ENC[ch]){
		long abs_err =  Lir.plb2ll(flash_prg.axis_cfg2[ch].bit_enc) - 1; //для 24 бит  16777215 - означает ошибка
		if(mCounter[ch].hw_cnt_prev == abs_err && *incoming != abs_err)
			mCounter[ch].hw_cnt_cur = 0;
		if(*incoming == abs_err)
			return 0;
	}
	#endif
	mCounter[ch].hw_cnt_prev = mCounter[ch].hw_cnt_cur;
	mCounter[ch].hw_cnt_cur = *incoming;

	//mCounter[ch].delta=mCounter[ch].hw_cnt_cur-mCounter[ch].hw_cnt_prev;
	bool from3_4_to_0_prev,from0_to_1_4_prev;
	bool from3_4_to_0_cur,from0_to_1_4_cur;
	long cur,prev;
	//-------------------------------------------------------------
	if(mCounter[ch].hw_cnt_prev>=12582911 && mCounter[ch].hw_cnt_prev<=16777215)
		from3_4_to_0_prev=true;
	else
		from3_4_to_0_prev=false;

	if(mCounter[ch].hw_cnt_prev>=0 && mCounter[ch].hw_cnt_prev<=4194303)
		from0_to_1_4_prev=true;
	else
		from0_to_1_4_prev=false;
	//-------------------------------------------------------------
	if(mCounter[ch].hw_cnt_cur>=12582911 && mCounter[ch].hw_cnt_cur<=16777215)
		from3_4_to_0_cur=true;
	else
		from3_4_to_0_cur=false;
	if(mCounter[ch].hw_cnt_cur>=0 && mCounter[ch].hw_cnt_cur<=4194303)
		from0_to_1_4_cur=true;
	else
		from0_to_1_4_cur=false;
	//-----------------------------------------------------------
	// signed long delta;
	cur=mCounter[ch].hw_cnt_cur;
	prev=mCounter[ch].hw_cnt_prev;
	if(from0_to_1_4_cur==true && from3_4_to_0_prev==true) {
		cur = cur + 16777216;
	}
	else if (from3_4_to_0_cur==true && from0_to_1_4_prev==true) {
		prev = prev + 16777216;
	}

	mCounter[ch].delta = cur - prev; //если дельта меньше 0 то едем влево , если больше нуля то вправо
	//==new
	if(flash_prg.axis_cfg[ch].direction == CPrg::DIRECTION::MINES)
		mCounter[ch].delta = -mCounter[ch].delta;
	//==
	if(mCounter[ch].cur_dir == CCNT_SENSOR::NO_DIRECTION){
		if (mCounter[ch].delta < 0) mCounter[ch].cur_dir=CCNT_SENSOR::DIRECTION_CNT_LEFT;
		if (mCounter[ch].delta > 0) mCounter[ch].cur_dir=CCNT_SENSOR::DIRECTION_CNT_RIGHT;
			mCounter[ch].prev_dir = mCounter[ch].cur_dir;
			mLuft[ch].fsm_luft = CLUFT::NO_LUFT;
		} else {
		mCounter[ch].prev_dir = mCounter[ch].cur_dir;
		if (mCounter[ch].delta < 0) mCounter[ch].cur_dir=CCNT_SENSOR::DIRECTION_CNT_LEFT;
		if (mCounter[ch].delta > 0) mCounter[ch].cur_dir=CCNT_SENSOR::DIRECTION_CNT_RIGHT;
		if(mCounter[ch].prev_dir != mCounter[ch].cur_dir){//counter0.rev_ind++;
			if(mLuft[ch].fsm_luft == CLUFT::NO_LUFT){
				mLuft[ch].luft = mLuft[ch].last_before_rev = mCounter[ch].rel;
				mLuft[ch].luft = mLuft[ch].luft + mCounter[ch].delta;
				if (mCounter[ch].delta > 0){
					if(mLuft[ch].gap != 0){
						mLuft[ch].approach=mLuft[ch].gap + mLuft[ch].last_before_rev;
						mLuft[ch].fsm_luft = CLUFT::CHECK_RIGHT;
						return mCounter[ch].hw_cnt_cur;
					}
				} else {
					if(mLuft[ch].gap != 0){
						mLuft[ch].approach=mLuft[ch].last_before_rev - mLuft[ch].gap;
						mLuft[ch].fsm_luft = CLUFT::CHECK_LEFT;
						return mCounter[ch].hw_cnt_cur;
					}
				}
			}
		}
	}
	if(mLuft[ch].fsm_luft != CLUFT::NO_LUFT){
		mCounter[ch].delta = run_luft_new(mCounter[ch].delta,ch);
	}
	//--------------------------------------------------------------
	if(mLuft[ch].fsm_luft == CLUFT::NO_LUFT){
		mCounter[ch].abs_pure += mCounter[ch].delta;
		bool correction_is_made = false;
		if (mCounter[ch].increment_0_360_one_turn)
			do_circle_counters(correction_is_made,ch);
		correction_is_made = to_do_correction_in_realtime_ch(ch);	
		//if(correction_is_made) отключил чтоб не моргала скорость
		//	mCounter[ch].delta = mCounter[ch].delta_corr; // приращения и в относительной тоже
		if (mCounter[ch].increment_0_360_one_turn == 0){
			if(Lir.is_corr(ch) == 0){   //correction_is_made == 0
				mCounter[ch].abs  = mCounter[ch].abs_pure;
				mCounter[ch].rel += mCounter[ch].delta; //new
			}
		}		
	}
	if (mCounter[ch].rel > MAX_LONG_PLUS || mCounter[ch].rel < MAX_LONG_MINES) {
		mCounter[ch].rel=0;
		mCounter[ch].error=true;
	}
	if (mCounter[ch].abs > MAX_LONG_PLUS || mCounter[ch].abs < MAX_LONG_MINES) {
		mCounter[ch].abs=0;
		mCounter[ch].error=true;
		mCounter[ch].abs_pure=0;
	}
	return mCounter[ch].hw_cnt_cur;
}








/*
определяет направление счета
при не равномерном движении или медленном движении DIRECTION_CNT_RIGHT/DIRECTION_CNT_LEFT
будет черодоваться с NO_DIRECTION, возникает проблема определения : датчик стоит или медленно двигается?
*/
CCNT_SENSOR::DIRECTION_CNT CCNT_SENSOR::get_dir()
{
	if(delta > 0)
		return DIRECTION_CNT_RIGHT;
	else if(delta < 0)
		return DIRECTION_CNT_LEFT;
	else
		return NO_DIRECTION;
}
/*
обнуление счетчиков по приходу реф метки
обнуляются abs и ref только одновременно
*/
void CCNT_SENSOR::ref_reset()
{
	rel = 0;
	abs = 0;
	hw_cnt_cur = 0;
	abs_pure = 0;
}
