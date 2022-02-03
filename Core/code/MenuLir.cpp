/*
 * MenuLir.cpp
 *
 * Created: 12.01.2018 11:46:51
 *  Author: Andrew
 */ 
 #include "MenuLir.h"
 #include "stdlib.h"
 #include "programm.h"
 #include <stdio.h>
 #include "uart_flash_PD6_7.h"
 #include "io_altera.h"
 #include "keyfunc.h"
 #include <math.h>
 #include "screen_service.h"
 #include <limits.h>
 #include <errno.h>
 #include "twitest.h"
 #include <avr/wdt.h>


 #define INIT_CALC       0
 #define INIT_NEW_STRING 1
 #define INPUT_LOOP      2
 #define FINISH_INPUT    3


 extern "C"{
 // функция находится в файле to_take_SP.S
 // понадобилась при умножении больших чисел
 // например при умножении 9999 9999.9999 * 1000 ,
 // указатель стека уходил за границу т.е указатель был выше дна SP>0x24ff(не путать с переполнением стека)
 // функция восстанавливает Y pair который искажается в результате перемножения больших чисел,
 // с помощью  y_par, затем из Y pair адрес записывается в SP
	void restore_y_pair(int y_par);
 }
 /*
 функция мигания курсора , интервал мигания задается с помощью переменной cursorrunner
 смещение курсора с помощью переменной cursor_pos
 in unsigned int x_ - координата x курсора
 in unsigned int y_ - координата y курсора
 char page - страница памяти экрана (0 - 3)
 */
 void Menu::flashing_cursor(unsigned int x_, unsigned char y_,char page)
 {
	char MAX_length = 39;
	if (curItem->id == idCmd || curItem->id == idEditPrgInput)
		MAX_length = 29;

     cli();
	 unsigned char count = cursorrunner;
	 sei();
	 
//========================================КУРСОР на вторую строку============================================
if ((x_ + cursor_pos) > MAX_length && !tgrc) //переключение курсора вниз
{
#if (configUSE_NCshell == 1)
	if(!UPD_2nd){
		display.put_char_local_independent(result[1], page, x_ + cursor_pos - 1, y_);
		result[1] = '\0';
	}
#endif
	y_cur_offset += 16;
	x_cur_offset = (MAX_length);
	tgrc = 1;
	return;
}

if ((x_ + cursor_pos) == MAX_length && tgrc) //переключение курсора вверх
{
	display.put_char_local_independent(' ', page, x_ + cursor_pos - x_cur_offset + 1, y_ + y_cur_offset);
	y_cur_offset = 0;
	x_cur_offset = 0;
	tgrc = 0;
	clr_cursor = false;
}
//============================================================================================================

	 if(count >= 100)
	 {    
	     cli();
	     cursorrunner = 0;
		 count = 0;
		 sei();
		 display.put_char_local_independent(bufASCIIstr[cursor_pos]/*' '*/,page,x_ + cursor_pos - x_cur_offset,y_+ y_cur_offset);
	 }
	 else
	 {
		if(count>=50)
			display.put_char_local_independent(bufASCIIstr[cursor_pos]/*' '*/,page,x_ + cursor_pos - x_cur_offset,y_+ y_cur_offset);
		else
			display.put_char_local_independent('#',page,x_ + cursor_pos - x_cur_offset,y_+ y_cur_offset);
	 }
	 	
	 if (cursor_pos > 0 && cursor_pos != 26){
		 if(curItem->id == idDiagInput||curItem->id == idCoordOffsetInput)
			display.put_char_local_independent('*',page,(x_-1) + cursor_pos - x_cur_offset,y_+ y_cur_offset);//значение перед курсором
		 else
			display.put_char_local_independent(bufASCIIstr[cursor_pos - 1]/*result[1]*/,page,(x_-1) + cursor_pos - x_cur_offset,y_+ y_cur_offset);//значение перед курсором
	 }
		
	 //add
	 if (cursor_pos < inputnum)
		display.put_char_local_independent(bufASCIIstr[cursor_pos + 1], page, (x_ + 1) + cursor_pos - x_cur_offset, y_ + y_cur_offset);

	 if(clr_cursor){
		display.put_char_local_independent(' ',page,x_ + cursor_pos+1- x_cur_offset,y_+ y_cur_offset);
		clr_cursor = false;
	 }	 

	 if(Update_input_str)
	 {
		display.print_ram_local((char*)bufASCIIstr,page,x_,y_);
		Update_input_str = false;
	 }
#if (configUSE_NCshell == 1)
	 upd_2nd_str(page,x_,y_);
#endif
 }


void Menu::print_alert_mes()
{
	if(Lir.alert == Menu::INSTRUMENT_NE_ZAJAT)
		Lir.status(PSTR("ИНСТРУМЕНТ НЕ ЗАЖАТ!"),0,0,0,1);
	else if(Lir.alert == Menu::SHPINDEL_NE_VRASHAETSIA)
		Lir.status(PSTR("ШПИНДЕЛЬ НЕ ВРАЩАЕТСЯ!"),0,0,0,1);
	else if(Lir.alert == Menu::SHPINDEL_NE_OSTANOVLEN)
		Lir.status(PSTR("ШПИНДЕЛЬ НЕ ОСТАНОВЛЕН!"),0,0,0,1);
	else if(Lir.alert == Menu::OCb_NA_TORMOZE)
		Lir.status(PSTR("ОСЬ НА ТОРМОЗЕ!"),0,0,0,1);
	else if(Lir.alert == Menu::OCb_VNE_DOPUSKA)
		Lir.status(PSTR("ОСЬ ВНЕ ДОПУСКА!"),0,0,0,1);
	else if(Lir.alert == Menu::MOVE_ERROR)
		Lir.status(PSTR("ОШИБКА СЧЕТЧИКА!        "),0,0,0,1);
	else if(Lir.alert == Menu::DREIF_ERROR)
		Lir.status(PSTR("ОШИБКА ДРЕЙФА!          "),0,0,0,1);
	else if(Lir.alert == Menu::STANOK_NE_GOTOV)
		Lir.status(PSTR("СТАНОК НЕ ГОТОВ!        "),0,0,0,1);
	else if(Lir.alert == Menu::PLC_ERROR)
		Lir.status(PSTR("КОНТРОЛЛЕР НЕ ОТВЕЧАЕТ!"),0,0,0,1);
	else if(Lir.alert == Menu::CLEAR_ALERT)
		Lir.status(PSTR(""),0,0,0,2);
}

unsigned short Menu::crc(unsigned char *data, unsigned short length)
 {
	 uint16_t maska = 0;
	 uint16_t crc = ~0;
	 uint8_t buf;

	 while (length--) {
		 buf = *data;
		 for (int j = 0; j < 8; j++) {
			 maska = (1 & crc) ^ (1 & buf);
			 buf >>= 1; crc >>= 1;
			 if (maska) crc ^= 0xa001;
		 }
		 data++;
	 }
	 return (crc >> 8) | (crc << 8);
 }
/*
функция добавляет в массив data значение crc16 по указателю p
*/
void Menu::append_crc(uint16_t crc16,unsigned char *data,int p)
{
	memcpy(&data[p + 1] ,(uint8_t*)&crc16, 1);
	memcpy(&data[p],((uint8_t*)&crc16) + 1 ,1);
}
/*
функция сравнивает crc16 с crc из массива по указателю p
*/
bool Menu::check_crc(uint16_t crc16, unsigned char* data, int p)
{
	uint16_t crc16_;
	memcpy((uint8_t*)&crc16_, &data[p], 1);
	memcpy(((uint8_t*)&crc16_) + 1, &data[p + 1], 1);
	if(crc16_ == crc16)
		return false;
	else
		return true;
}

/*
вывод сообщения в статусную строку
const char *str - указатель на строку во Flash
int delay - задержка для всплывающего сообщения
unsigned char page - страница памяти экрана
bool rst_buf_mem - сброс памяти RAM
char A -  1 - установка аварийного сообщения , 2 - сброс
*/
void Menu::status(const char *str,int delay /*= 0*/,unsigned char page /*= 0*/,bool rst_buf_mem/*= 0*/,char A/*= 0*/)
{
	#define SZ_STATUS 25
	static char buf_mem[SZ_STATUS];
	char tmp[SZ_STATUS];
	if (str == NULL)
	return;
	if(rst_buf_mem)
		memset(buf_mem,'\0',SZ_STATUS);

	if(A == 1 && alert){
		if(init.time_metre>=100)// для отображения ыключение через
			return;
		display.print_flash_local(str,page,1,16 * 14);
		return;
	}
	else if(A == 2){
		alert = NO_ALERT;
		goto l;
	}

		if (delay <= 0) {
			if(!strncmp_P(buf_mem,str,SZ_STATUS - 1))
				return;
			strncpy_P(buf_mem, str,SZ_STATUS); //сохранение в память ОЗУ
			l:
			display.strncpy_lir(tmp,buf_mem,SZ_STATUS);
			tmp[SZ_STATUS - 1] = '\0';
			add_filler((uint8_t*)tmp, SZ_STATUS, SZ_STATUS, ' ', 1);
			display.print_ram_local(tmp, page, 1, 16 * 14);
		}
		else {
			char buf[SZ_STATUS];
			strncpy_P(buf, str,SZ_STATUS);
			buf[SZ_STATUS - 1] = '\0';
			add_filler((uint8_t*)buf, SZ_STATUS, SZ_STATUS, ' ', 1);
			display.print_ram_local(buf, page, 1, 16 * 14);
			while (delay--){
				Sleep(1);
				wdt_reset();
			}
			add_filler((uint8_t*)buf_mem, SZ_STATUS, SZ_STATUS, ' ', 1);
			display.print_ram_local(buf_mem, page, 1, 16 * 14);
		}
	//}
}
/*
вывод сообщения в статусную строку вторая часть , начиная с x 25.
если в цикле то повторного вывода не будет
*/
bool Menu::status2(const char *str,int delay /*= 0*/,unsigned char page /*= 0*/)
{
	#define SZ_STATUS2 6
	static char buf_mem[SZ_STATUS2];
	if (str == NULL)
		return 0;
	if(!strncmp_P(buf_mem,str,SZ_STATUS2 - 1))
		return 0;
	//else
	//	USART_DEBUG(0x11);
	strncpy_P(buf_mem, str,SZ_STATUS2); //сохранение в память ОЗУ

	char tmp[SZ_STATUS2];
	display.strncpy_lir(tmp,buf_mem,SZ_STATUS2);
	tmp[SZ_STATUS2 - 1] = '\0';
	add_filler((uint8_t*)tmp, SZ_STATUS2, SZ_STATUS2, ' ', 1);
	display.print_ram_local(tmp, page, 25, 16 * 14);	
	return 1;
}

void Menu::status_RAZRABOTKA()
{
	status(PSTR("в разработке"),200,0);
}
/*
сброс ошибок по кнопке "С"
return 1 - был сброс 0 - небыло
*/
bool Menu::reset_error()
{
	bool reset = 0;
	int y_k;
	for(unsigned char n=0;n<N_AXIS;n++){
		display.encoder_fault[n] = false;
		if(altera.error_corr_real_time[n] != CIOAltera::ERROR_CORR_REAL_TIME::CANT_BE_DONE){		
			altera.error_corr_real_time[n] = CIOAltera::ERROR_CORR_REAL_TIME::NO_ERROR;
			if(n == 0)y_k = 41;
			if(n == 1)y_k = 96;
			if(n == 2)y_k = 158;
			if(n == 3)y_k = 208;
			display.put_char_local_independent(' ',0,0,y_k);
			display.put_char_local_independent(' ',0,1,y_k);
			reset = 1;
		}
	}

	updENC_error();

	if(atdc.alarm==true){
		char tmp[25];
		for(unsigned char n=0;n<24;n++)	
			tmp[n]=0x20;
		tmp[24]=0;
		display.print_ram_local(tmp,0,0,208);
		display.print_ram_local(tmp,0,0,224);
		atdc.alarm=false;
		reset = 1;
	}
	if(flash_prg.reset_axis_errors())
		reset = 1;
		
	return reset;
}

void Menu::updENC_error()
{
	for(int place = 1 ; place < 5;place++)
		screen.encoder_fault_[place] = 0;
}

//-------------------------
/*
функция извлекает из строчного массива точку(если есть) и конвертирует в бинарное число long data ,затем
выравнивает бинарное число относительно положения точки и формата , добавляя в конец нули
out long & data            - указатель на бинарное число результат
in  int format             - формат приходящих данных в unsigned char * string
in  unsigned char * string - указатель на входящую строку
*/
int Menu::ConvertASCIIlinear_circle_tolong(long & data, int format, unsigned char * string)
{
	int pp = 0;
	int n = 0;
	//int mp = 0;
	int num = 1;
	int step = 0;
	long bin;
	bool point = false;
	char n_after_dot = 0;
	//int num;
	for (pp = 0; pp < 15; pp++)
	{
		if (string[pp] == '.')break;
	}
	//== если точки нет до добавляем точку 09.04.2019 ввод в мм ,вместо мкм
	if(pp == 15){
		signed char end_str = display.strnlen_lir((char*)string, 25);
		if(end_str < 15){
		string[end_str] = '.';
		pp = end_str;
		}
	}
	//==
	if (pp == 15 ) { 
		pp = 0;
		/* n_after_dot = 0; */
		point = false; 
	}
	else
	{
		n = display.strnlen_lir((char*)string, 25);
		n_after_dot = n - pp - 1;

		cutting_str((char*)string, n,n_after_dot, format);

		point = true;
		for (int z = 0; z < n - pp; z++)
		{
			string[pp + z] = string[pp + z + 1];
		}
	}
	
	//bin = atol((const char*)string);
	bin = strtol_lir(string);//strtol((const char*)string,0,10);

	if (format == CPrg::FORMAT::LINEAR_1 || format == CPrg::FORMAT::CIRCLE_1) step = 2;    // *1
	if (format == CPrg::FORMAT::LINEAR_2 || format == CPrg::FORMAT::CIRCLE_2) step = 3;    // *10
	if (format == CPrg::FORMAT::LINEAR_3 || format == CPrg::FORMAT::CIRCLE_3) step = 4;    // *100
	if (format == CPrg::FORMAT::LINEAR_4 || format == CPrg::FORMAT::CIRCLE_4 || format == CPrg::FORMAT::LINEAR_INCH_4) step = 5;    // *1000
	
	if (point == true)
	{
		n = n - 1;
		if (pp == 0)
			n = n + 1;
		else
			step = step + (pp - 1);

		if (step > n)
		{
			for (int c = 0; c < step - n; c++) // возведение в степень
			{
				num *= 10;
			}
		}
	}
	long long check;
	check = (long long)bin * num;

	if(format == CPrg::FORMAT::LINEAR_INCH_4)
		check = check *2;

	if(check_overflow_long(check)){
		if (check>=100000000)
			data=99999999;
		if (check<=-100000000)
			data=-99999999;
		return 0;
	}

	data = bin * num;

	if(format == CPrg::FORMAT::LINEAR_INCH_4)
		if(coeff.mult(255,&data,1,254) != CDouble::OK){
			if(data > 0)
				data = MAX_LONG_PLUS;
			if(data < 0)
				data = MAX_LONG_MINES;
			}

	return 1;
}

/*
функция извлекает из строчного массива точку(если есть) и конвертирует в бинарное число long long data ,затем
выравнивает бинарное число относительно положения точки и формата , добавляя в конец нули
*/
int Menu::ConvertASCIIlinear_circle_tolonglong(long long&data, int format, char *str)
{
	int pp = 0;  // pp - point position
	int n = 0;
	int n_after_dot;
	int num = 1;
	int step = 0;
	long long bin = 0;
	bool point = false;

	for (pp = 0; pp < 22; pp++)
	{
		if (str[pp] == '.')break;
	}

	if (pp == 22) { pp = 0; n_after_dot = 0; point = false; }
	else
	{
		n = display.strnlen_lir(str, 22);
		n_after_dot = n - pp - 1;

		cutting_str(str, n, n_after_dot, format);

		point = true;

		for (int z = 0; z < n - pp; z++)
		{
			str[pp + z] = str[pp + z + 1];
		}
	}

	bin = strtoll_lir((unsigned char*)str);

	if (format == CPrg::LINEAR_1 || format == CPrg::CIRCLE_1) step = 2;    // *1
	if (format == CPrg::LINEAR_2 || format == CPrg::CIRCLE_2) step = 3;    // *10
	if (format == CPrg::LINEAR_3 || format == CPrg::CIRCLE_3) step = 4;    // *100
	if (format == CPrg::LINEAR_4 || format == CPrg::CIRCLE_4 || format == CPrg::LINEAR_INCH_4) step = 5;    // *1000

	if (point == true)
	{
		n = n - 1;
		if (pp == 0)
		n = n + 1;
		else
		step = step + (pp - 1);

		if (step > n)
		{
			for (int c = 0; c < step - n; c++) // возведение в степень
			{
				num *= 10;
			}
		}
	}

	data = bin * num;
	return 0;
}
/*
перед выводом на экран функция отбрасывает незначащие нули после точки в конце строки
*/
void Menu::cutting_str(char *input_str,int&n_sym, const char&n_after_dot,const int& f)
{ 
	char f_n_after_dot;
	if (f == CPrg::FORMAT::LINEAR_1 || f == CPrg::FORMAT::CIRCLE_1) f_n_after_dot = 1;    // *1
	if (f == CPrg::FORMAT::LINEAR_2 || f == CPrg::FORMAT::CIRCLE_2) f_n_after_dot = 2;    // *10
	if (f == CPrg::FORMAT::LINEAR_3 || f == CPrg::FORMAT::CIRCLE_3) f_n_after_dot = 3;    // *100
	if (f == CPrg::FORMAT::LINEAR_4 || f == CPrg::FORMAT::CIRCLE_4 || f == CPrg::FORMAT::LINEAR_INCH_4) f_n_after_dot = 4;    // *1000


	if (n_after_dot > f_n_after_dot)
	{
		for (int i = 0; i < n_sym; i++)
		{
			if (input_str[i] == '.')
			{
				input_str[i + f_n_after_dot + 1] = '\0';
				int razn = n_after_dot - f_n_after_dot;
				n_sym = n_sym - razn;
				return;
			}
		}
	}
}
/*
in unsigned char size_txt_string -  размер строки в байтах  ,один символ  - один байт
in char *buf - указатель на строку с символами
in unsigned char y - координата по y
*/
void Menu::put_large_string(unsigned char size_txt_string,char *buf,unsigned char y)
{
#define DIGIT_WIDTH  24
#define MINUS_WIDTH  20

	int x_coord = 264;
	unsigned char sym_ = 0;
	unsigned char width = DIGIT_WIDTH;
	
	for(int i = 0 ;i < size_txt_string;i++)
		{
			if(buf[i] >= '0' && buf[i] <= '9') {sym_ = buf[i] - 0x30;width = DIGIT_WIDTH;}
			if(buf[i] == '-') {sym_ = 0x0A;width = MINUS_WIDTH;}
			if(buf[i] == '.') {sym_ = 0x0C;width = 12;}
			if(buf[i] == '°') {sym_ = 0x0D;width = 11;if(i == 0)x_coord = 274;}
			if(buf[i] == 0x01) {sym_ = 0x0E;width = 12;if(i == 0)x_coord = 274;} // '
			if(buf[i] == 0x02) {sym_ = 0x0F;width = 11;if(i == 0)x_coord = 275;} // "
			if(buf[i] == ':') {sym_ = 0x10;width = 12;}                          // :

			x_coord-=width;

			display.put_large_symbol(sym_,0,width,x_coord,y);			
		}

		if(x_coord < 40)
			x_coord  = 40;

		display.clear_rectangle(40,y,x_coord - 40,8,0x00,0);
}
/*
in unsigned char size_txt_string -  размер строки в байтах  ,один символ  - один байт
in char *buf - указатель на строку с символами
in unsigned char y - координата по y
*/
void Menu::put_big_string(unsigned char size_txt_string,char *buf,unsigned char y)
{
	/*
	unsigned char width - ширина символа ,смотреть в файле со шрифтом
	*/

	int x_coord = 264;
	unsigned char sym_ = 0;
	unsigned char width = DIGIT_WIDTH;	

		for(int i = 0 ;i < size_txt_string;i++)
		{
			if(buf[i] >= '0' && buf[i] <= '9') {sym_ = buf[i] - 0x30;width = DIGIT_WIDTH;}
			if(buf[i] == '-') {sym_ = 0x0A;width = MINUS_WIDTH + 4;}
			if(buf[i] == '.') {sym_ = 0x0C;width = 8;}
			if(buf[i] == '°') {sym_ = 0x0D;width = 8;if(i == 0)x_coord = 272;}
			if(buf[i] == 0x01) {sym_ = 0x0E;width = 8;if(i == 0)x_coord = 272;} // '
			if(buf[i] == 0x02) {sym_ = 0x0F;width = 8;if(i == 0)x_coord = 272;} // "
			if(buf[i] == ':') {sym_ = 0x10;width = 8;}                          // :

			x_coord-=width;

			display.put_big_symbol(sym_,0,width,x_coord,y);			
		}

		if(x_coord < 40)
			x_coord  = 40;

		display.clear_rectangle(40,y,x_coord - 40,5,0x00,0);
}

void Menu::strings_out(uint8_t place,uint8_t string_size,char *reverse_order)
{
	//display.form_and_write_5_string_every_232_bit_long(string_size,reverse_order,7); // 1.160ms 0.000  - -99999.000 1.600ms
	switch(place){
		case 1:		
			put_big_string(string_size,reverse_order,0); // 0.000 (0.880ms) - -99999.000 (1.440ms) 
			break;

		case 2:
		if(Str[0] == no_axis && configUSE_Large_Digit == 1 && display.position_to_see[0] != CDisp::TIME){
			put_large_string(string_size,reverse_order,4);// 0.000 (1.440ms) - -99999.000 (2.320ms)
			}else{
			put_big_string(string_size,reverse_order,7);
			}
			break;

		case 3:
		if(Str[1] == no_axis && configUSE_Large_Digit == 1 && display.position_to_see[1] != CDisp::TIME){
			put_large_string(string_size,reverse_order,11);// - 2.520 ms
			}else{
			put_big_string(string_size,reverse_order,14);
			}
			break;

		case 4:
		if(Str[2] == no_axis && configUSE_Large_Digit == 1 && display.position_to_see[2] != CDisp::TIME){
			put_large_string(string_size,reverse_order,18);
			}else{
			put_big_string(string_size,reverse_order,21);
			}
			break;
	}
}
/*
функция для преобразования счетчика радиус/диаметр
*/
void Menu::l_d(long &data,int8_t c,char action){
	if(flash_prg.axis_cfg[c].r_d == CPrg::R_D::DIAMETER && flash_prg.axis_cfg[c].type == CPrg::TYPE::LINEAR){
		if (action == in_mem) 
			data /= 2;
		if (action == on_display) 
			data *= 2;
	}
}

/*
out long & data            - указатель на бинарное число
in  int format             - формат приходящих данных в unsigned char *input_str
in  unsigned char *input_str - указатель на строку
*/
void Menu::ConvertASCIIcircle_secundo_tolong_new(long &data,int format,unsigned char *input_str)
{
#define STR_SZ 10
	char str_gradus[STR_SZ] = { 0 };
	char str_min[STR_SZ] = { 0 };
	char str_sec[STR_SZ] = { 0 };
	char str_bez_znaka[STR_SZ] = { 0 }; //5old

	//===если формат секунды/минуты и если там стоит точка то это будет расцениваться как градус
	if (format == CPrg::FORMAT::CIRCLE_MINUTO || format == CPrg::FORMAT::CIRCLE_SECUNDO){
		for(int i = 0 ; input_str[i] != '\0'; i++){
			if(input_str[i] == '.'){
				input_str[i] = '°';
				break;
			}
		}
	} 
	//===
	get_grdusminsec((char*)input_str, str_gradus,STR_SZ, '°');    // '.'  ==   '°'
	get_grdusminsec((char*)input_str, str_min,STR_SZ, 0x01);      // '\'' ==   0x01
	get_grdusminsec((char*)input_str, str_sec,STR_SZ, 0x02);      // '\"' ==   0x02
	get_grdusminsec((char*)input_str, str_bez_znaka,STR_SZ, '\0');

	long grd=0;
	long min=0;
	long sec=0;
	long bez_znaka = 0;

	grd = strtol_lir((unsigned char*)str_gradus);
	min = strtol_lir((unsigned char*)str_min);
	sec = strtol_lir((unsigned char*)str_sec);
	bez_znaka = strtol_lir((unsigned char*)str_bez_znaka);

	if (!grd) {
		if (!min)
		grd = bez_znaka;
		else if (sec);
		else
		sec = bez_znaka;
	}
	else {
		if (!min)
		min = bez_znaka;
		else {
			if (!sec)
			sec = bez_znaka;
		}
	}

	if (format == CPrg::FORMAT::CIRCLE_MINUTO) {
		if(!grd)
			grd = bez_znaka;	 
		}

	grd = 3600 * grd;
	min = 60 * min;

	data = labs(grd) + labs(min) + labs(sec) ;/*+ bez_znaka;*/

	if (format == CPrg::FORMAT::CIRCLE_MINUTO)
	data = data / 60;

	if (grd < 0 || min < 0 || sec < 0)
		data *= -1;
}

void Menu::get_grdusminsec(char * input_str, char * out_str,const unsigned char& size, const unsigned char znak)
{
	int i = 0;
	for (int l = 0; l < size - 1; l++)
	{
		out_str[l] = '0';
	}

	while (input_str[i] != '\0')
	{
		n:
		if (input_str[i] == (char)znak)
		{
			for (int j = i - 1, k = size - 2; k != 0; j--, k--)
			{
				if (j < 0 )  break;
				if (input_str[j] == '°')  break; // '.'  ==   '°'
				if (input_str[j] == 0x01) break; // '\'' ==   0x01
				if (input_str[j] == 0x02) break; // '\"' ==   0x02
				out_str[k] = input_str[j];
			}
			//==сдвиг влево
			while (out_str[0] == '0')
			{
				for(int i = 0; out_str[i] !='\0'; i++)
				out_str[i] = out_str[i + 1];
			}
			//==
			return;
		}
		i++;
		if (input_str[i] == '\0' && znak == '\0') goto n;
	}
}
    
long Menu::strtol_lir(unsigned char * str)
{
	//функция конвертирует байтовый массив символов со значениями с 0x30 по 0x39
	//в бинарное знаковое число типа long, незначющие нули вначале строки отбрасываются
	//в мк xmega long 4 байта  в  пк 4 байт
	signed char minus = 0;
	if (str[0] == '-') minus = 1;

	int c = display.strnlen_lir((char*)str) - minus;
	if (c > 10 || (c == 10 && str[minus] > '2'))
	{
		overflow:
		errorno = integer_overflow;
		if (minus) return -2147483647L ;//- 1; -2147483647L becouse print_debug_one_local uses 0 instead -2147483648L
		else
		return 2147483647L;
	}
	long res = 0;
	long res_prev = 0;
	long k = 1;

	for (int i = 1; i < c; i++)
	{
		k *= 10;
	}
	for (int i = minus; str[i] != '\0'; i++)
	{
		res = res + (str[i] - 0x30) * k;
		if (res_prev > res ) // переполнение разрядной сетки
			goto overflow;
		res_prev = res;
		k /= 10;
	}

	if (minus)
		res = -res;

	return res;
}
int Menu::strtoi_lir(unsigned char * str)
{
	//функция конвертирует байтовый массив символов со значениями с 0x30 по 0x39
	//в бинарное знаковое число типа int, незначющие нули вначале строки отбрасываются
	//в мк xmega  int  2 байта    в  пк 4 байта
	signed char minus = 0;
	if (str[0] == '-') minus = 1;

	int c = display.strnlen_lir((char*)str) - minus;
	if (c > 5 || (c == 5 && str[minus] > '3'))
	{
		overflow:
		errorno = integer_overflow;
		if (minus) return -32768;
		else
		return 32767;
	}
	short res = 0;
	short res_prev = 0;
	short k = 1;

	for (short i = 1; i < c; i++)
	{
		k *= 10;
	}

	for (short i = minus; str[i] != '\0'; i++)
	{
		res = res + (str[i] - 0x30) * k;
		if(res_prev > res) // переполнение разрядной сетки
			goto overflow;
		res_prev = res;
		k /= 10;
	}

	if(minus)
		res = -res;

	return res;
}

long long Menu::strtoll_lir(unsigned char * str)
{
	//функция конвертирует байтовый массив символов со значениями с 0x30 по 0x39
	//в бинарное знаковое число типа long long, незначющие нули вначале строки отбрасываются
	//в мк xmega long long 8 байт в пк 8 байт
	char minus = 0;
	if (str[0] == '-') minus = 1;

	int c = display.strnlen_lir((char*)str) - minus;
	if (c > 19)
	{
		overflow:
		errorno = errors::integer_overflow;
		if (minus) return MINLONGLONG;
		else
		return MAXLONGLONG;
	}
	long long res = 0;
	long long res_prev = 0;
	long long k = 1;

	for (int i = 1; i < c; i++)
	{
		k *= 10;
	}

	for (int i = minus; str[i] != '\0'; i++)
	{
		res = res + (str[i] - 0x30) * k;
		if(res_prev > res) // переполнение разрядной сетки
			goto overflow;
		res_prev = res;
		k /= 10;
	}
	if (minus) 
		res = -res;
	return res;
}

int Menu::lltoa_lir(long long data, unsigned char * str_c)
{
	//функция конвертирует из бинарного знакового long long data в строку
	//что бы не было переполнений строчный массив *str_c должен быть минимум из 20 ого байта
	//в мк long long 8 байт в пк 8 байт
	unsigned char str[20] = { 0 };
	//unsigned char str_c[21] = { 0 };
	//long long data = MINLONGLONG; //-9223372036854775808;
	

	if (data == MINLONGLONG) data++;

	int k = 0;
	unsigned char tmp = 0;
	if (data < 0) {
		data = -data;
		tmp = 0xff;
	}
	do {
		str[k++] = data % 10 + 0x30;
		data /= 10;
	} while (data);

	if (tmp) {
		str[k] = '-';
	}
	else
	k--;

	int i = 0;
	for (int j = k; j >= 0; i++, j--)
	{
		str_c[i] = str[j];
	}
	str_c[i] = '\0';
	return 0;
}

 void Menu::init_cursor_pos(const int &inputnum_ /*= 11*/)
 {
	 cursor_pos = 0;
	 result[1] = 0;
	 result[3] = 0;
	 x_cur_offset = 0;
	 y_cur_offset = 0;
	 tgrc = 0;
	 CurFormatM = 0;
	 key_point_counter = 0;
	 ResultForInputNum = 0;
	 inputnum = inputnum_; // максимальное кол - во вводимых символов

	 memset(bufASCIIstr,0,IN_BUF_SZ);
	 memset(buf_for_reset,0,4);	
 }

void Menu::init_cursor_after_input(int id,int x,int y,int count)
{
	cursor_pos = 0;
	result[1] = 0;
	result[3] = 0;
	x_cur_offset = 0;
	y_cur_offset = 0;

	//display.print_flash_local(probel10,0,x,y);
	display.print_symbols_local(0x20,count,0,x,y);
	memset(bufASCIIstr,0,IN_BUF_SZ);
	SET_C(id);
	Lir.ResultForInputNum = 0;
}



/*
return true - если на канал назначена ось в ПУЛЬТ ОСЬ и эта ось в ОСИ ЭКРАН
*/
bool Menu::ch_use(const int ch)
{
	if(P2[ch] != no_axis){
		for(int s = 0 ; s < N_STR;s++){
			if(P2[ch] == Str[s])
				return 1;
		}
	}
	return 0;
}

void Menu::change_axis_name(const int ch)
{
	if (ch >= N_AXIS + N_SA)
        return;

	P2[ch] = incdec_axis(direction,P2[ch]);
    if(P2[ch] == CInit::NO_AXIS)
        return;

    for (int c = 0; c < N_AXIS + N_SA; c++) {
        if (ch == c)
            continue;
        if (P2[ch] == P2[c]){
			P2[ch] = incdec_axis(direction,P2[ch]);
            if(P2[ch] == CInit::NO_AXIS)
				return;
			else
				c = -1;
		}
    }
}
/*
const char dir - направление влево вправо;
int axis - буква оси
*/
int Menu::incdec_axis(const char dir,signed char axis) {
    if (dir){
        if (axis >= CInit::NO_AXIS)
            axis = CInit::AXIS_X;
        else
            axis++;
    } else {
        if (axis <= CInit::AXIS_X)
            axis = CInit::NO_AXIS;
        else
            axis--;
    }
	return axis;
}

/*
установка текущего контекста
*/
void Menu::SET_C(int id)
{
	cli();
	const_cast<context*>(curItem)->id = id;
	sei();
}


void Menu::UpdateMenuEncoders()
{
      //redraws the current menu again
	  display.clear_ram_null();
      INVERT_CHAR()
	  display.print_flash_local(s00_ENCM,0,0,0);
	  NORMAL_CHAR()
	  #if (configUSE_mcx514 == 1)
	  display.print_flash_local(s01_ENCM,0,1,16);
	  #else
	  display.print_flash_local(s02_ENCM,0,1,16);
	  #endif
	  for(int c = 0 ; c < N_AXIS;c++){
		display.print_flash_local(sP2[c],0,1,16 * (2 + c));
		display.print_flash_local(sAxis_screen[P2[c]],0,6,16 * (2 + c));
	  }

	  INVERT_CHAR()
	  display.print_flash_local(s03_ENCM,0,0,112);
	  NORMAL_CHAR()
	  display.print_flash_local(s04_ENCM,0,1,128);
	  for(int c = 0 ; c < N_AXIS;c++){
		  display.print_flash_local(sP2[c],0,1,16 * (9 + c));
		  display.print_flash_local(sAxis_screen[P2[c]],0,6,16 * (9 + c));
	  }
}

void Menu::menu_stop_ENCM()
{
	if(curItem->id == idMenuEncoders){
		SET_C(idDebugMode);
	}
}

void Menu::UpdateMenuSupply()
{
	//redraws the current menu again
	display.clear_ram_null();
	INVERT_CHAR()
	display.print_flash_local(s00_SUPM,0,0,0);
	NORMAL_CHAR()
	display.print_flash_local(s01_SUPM,0,1,16);
	display.print_flash_local(s02_SUPM,0,1,32);
}

void Menu::menu_stop_SUPM()
{
	if(curItem->id == idSupplyandBatt){
		SET_C(idDebugMode);
	}	
}

void Menu::UpdateMenuTime()
{
	//redraws the current menu again
	display.clear_ram_null();
	INVERT_CHAR()
	display.print_flash_local(s00_TIME,0,0,0);
	NORMAL_CHAR()
	display.print_flash_local(s01_TIME,0,1,16);
	display.print_flash_local(s02_TIME,0,1,32);
	display.print_flash_local(s03_TIME,0,1,48);
	display.print_flash_local(s04_TIME,0,1,64);
}

void Menu::menu_stop_TIME()
{
	if(curItem->id == idTime){
		SET_C(idDebugMode);
	}
}

void Menu::InitMenuViewInputs()
{
	//redraws the current menu again
	display.clear_ram_null();
	
	#if (configUSE_PLC == 0)
		INVERT_CHAR()
		display.print_flash_local(s00_VIIN,0,0,0);
		display.print_flash_local(PSTR(" P1 "),0,0,32);
		NORMAL_CHAR()
		display.print_flash_local(s01_VIIN,0,31,16 * 3);
		draw_Zone_ref_metki_static(4);		
	#else
		INVERT_CHAR()
		display.print_flash_local(PSTR(" ПРОСМОТР ВХОДОВ/МАРКЕРОВ"),0,0,0);
		NORMAL_CHAR()
		display.print_flash_local(PSTR("- номер"),0,31,16 * 7);
		draw_Zone_ref_metki_static(8);
	#endif
}
/*
в этой функции выводится все то что изменяется
*/
void Menu::draw_menu_VIIN()
{
	if (Lir.curItem->id == idViewInputs)
	{
		for(int i = 0; i < 24;i++){
			if(Lir.menu_cnt_VIIN == i && menu_cnt_up_down_VIIN == 0)
				INVERT_CHAR()
			else
				NORMAL_CHAR()
			char offset = 0;
			if(i >= 16)
				offset = 1;
			print_bit(init.p_in,i,8 + offset ,16 * 2);
			}
		NORMAL_CHAR()
		#if (configUSE_PLC == 0)
			draw_Zone_ref_metki(6);	
			Lir.print_long_format(menu_cnt_VIIN + 1,CPrg::FORMAT_END,0,29,16 *3,RIGHT_ALIGN,1);
		#else
			const char *menu_[4];
			menu_[0] = PSTR("    <P1");
			menu_[1] = PSTR("m32-m63");
			menu_[2] = PSTR("m64-m79");
			menu_[3] = nullptr;

			for (uint8_t str = 0; menu_[str] != nullptr; str++) {
				if (str == menu_cnt_up_down_VIIN)
					INVERT_CHAR()
				else
					NORMAL_CHAR()
				display.print_flash_local(menu_[str], 0, 0, 32 + ((16 * str) * 2));
			}
			NORMAL_CHAR()

			//==m32 - m63
				for(int i = 0 ; i < 32;i++){
					if (menu_cnt_VIIN == i && menu_cnt_up_down_VIIN == 1)
						INVERT_CHAR()
					else
						NORMAL_CHAR()
					print_bit(init.m_in,i,8,32 + ((16 * 1) * 2));
					NORMAL_CHAR()
				}
			//==
			//==m64 - m79
				for(int i = 0 ; i < 16;i++){
					if (menu_cnt_VIIN == i && menu_cnt_up_down_VIIN == 2)
						INVERT_CHAR()
					else
						NORMAL_CHAR()
					print_bit(uart_485.pack_Tx.m64_m79,i,8,32 + ((16 * 2) * 2));
					NORMAL_CHAR()
				}
			//==

			draw_Zone_ref_metki(10);
			int tmp = menu_cnt_VIIN;
			char tmp_c = 'P';
			if(menu_cnt_up_down_VIIN > 0)
				tmp_c = 'm';

			if(menu_cnt_up_down_VIIN == 2)
				tmp = menu_cnt_VIIN + 63;
			else if(menu_cnt_up_down_VIIN == 1)
				tmp = menu_cnt_VIIN + 31;
			print_long_format(tmp + 1, CPrg::FORMAT_END, 0, 29, 16 * 7, RIGHT_ALIGN, 1);
			display.put_char_local_independent(tmp_c,0,27,16 * 7);
		#endif	
	}
}
/*
int y - смещение номер строки по y
*/
void Menu::draw_Zone_ref_metki(int y)
{
	long data;
	for(int  c = 0 ;  c <N_AXIS; c ++){
		if (Lir.P2[c] != no_axis)
		{
			data = mCounter[c].rel;
			coeff.mult(c,&data);
			if(screen.measure_chan[c]){
				if(display.blinker&0x04)
				INVERT_CHAR()
				else
				NORMAL_CHAR()
			}
			Lir.print_long_format(data,Lir.get_format_from_ch(c),0,11,16 * (c + y),LEFT_ALIGN,0,11); //Индикация
			NORMAL_CHAR()
			if(!(flash_prg.axis_cfg[c].ref_pulse_zone & YESNO_P_bm )){
				data = ref_.counter_measure[c];
				if(data<0)
				data=-data;
				
				coeff.mult(c,&data);
				Lir.print_long_format(data,Lir.get_format_from_ch(c),0,23,16 * (c + y),LEFT_ALIGN,0,11);
			}
			if(ref_.measurement_is_ended){
				//MsgKey.to_stop_and_to_see_measurement_result();
				MsgKey.all_ref_interrupts_must_be_disabled=true;
				if(screen.measure_chan[c])
				screen.measure_chan[c]=false;
				signed char s = Lir.get_str_from_ch(c);
				if(s != N_STR){
					screen.measure_string[s] = false;
				}
				MsgKey.mode_ref_submode_measure=false;
			}
		}
	}
}

void Menu::draw_Zone_ref_metki_static(int y)
{
	INVERT_CHAR()
	display.print_flash_local(s02_VIIN,0,0,16 * (y + 0));
	NORMAL_CHAR()
	display.print_flash_local(s03_VIIN,0,1,16 * (y + 1));

	for(int c = 0 ; c < N_AXIS;c++){
		display.print_flash_local(sP2[c],0,1,16 * (y + 2 + c));
		display.print_flash_local(sAxis_screen[P2[c]],0,6,16 * (y + 2 + c));
		if(flash_prg.axis_cfg[c].ref_pulse_zone & YESNO_P_bm)
			display.print_flash_local(sNO,0,23,16 * (y + 2 + c));
	}
}

/*
uint32_t data - данные
uint8_t BIT_n - позиция бита в data
int x - координата по x
int y - координата по y
*/
void Menu::print_bit(uint32_t data,uint8_t BIT_n,int x ,int y)
{
	if(data & plb2l(BIT_n))
		display.put_char_local_independent('1', 0, x + BIT_n, y);
	else
		display.put_char_local_independent('0', 0, x + BIT_n, y);
}

void Menu::menu_left_VIIN()
{
	if(Lir.curItem->id == idViewInputs)
	{
		menu_cnt_VIIN--;
		if (menu_cnt_VIIN < 0 && !menu_cnt_up_down_VIIN)
			menu_cnt_VIIN = 23;
		else if(menu_cnt_VIIN < 0 && menu_cnt_up_down_VIIN == 1)
			menu_cnt_VIIN = 31;
		else if(menu_cnt_VIIN < 0 && menu_cnt_up_down_VIIN == 2)
			menu_cnt_VIIN = 15;
	}
}

void Menu::menu_right_VIIN()
{
	if(Lir.curItem->id == idViewInputs)
	{
		menu_cnt_VIIN++;
		if (menu_cnt_VIIN > 23 && !menu_cnt_up_down_VIIN)
			menu_cnt_VIIN = 0;
		else if(menu_cnt_VIIN > 31 && menu_cnt_up_down_VIIN == 1)
			menu_cnt_VIIN = 0;
		else if(menu_cnt_VIIN > 15 && menu_cnt_up_down_VIIN == 2)
			menu_cnt_VIIN = 0;
	}
}

void Menu::menu_up_VIIN()
{
	#if (configUSE_PLC == 0)
	return;
	#endif
	if (curItem->id == idViewInputs) {
		if(menu_cnt_up_down_VIIN != 0 ){
			menu_cnt_up_down_VIIN--;
			if(menu_cnt_up_down_VIIN == 0 && menu_cnt_VIIN > 23)
				menu_cnt_VIIN = 23;
		}
	}
}

void Menu::menu_down_VIIN()
{
	#if (configUSE_PLC == 0)
	return;
	#endif
	if (curItem->id == idViewInputs) {
		if(menu_cnt_up_down_VIIN < 2){
			menu_cnt_up_down_VIIN++;
			if(menu_cnt_up_down_VIIN == 2 && menu_cnt_VIIN > 15)
			menu_cnt_VIIN = 15;
		}
	}
}

void Menu::menu_enter_VIIN()
{
#if (configUSE_PLC == 0)
return;
#endif
	if(curItem->id == idViewInputs){
		if(menu_cnt_up_down_VIIN == 2){
			cli();
			long m64_m79;
			memcpy(&m64_m79,&uart_485.pack_Tx.m64_m79,sizeof(short));
			m64_m79 ^= plb2l(menu_cnt_VIIN);
			memcpy(&uart_485.pack_Tx.m64_m79,&m64_m79,sizeof(short));
			sei();
		}	
	}
}

void Menu::menu_stop_VIIN()
{
	if(curItem->id == idViewInputs){
		SET_C(idDebugMode);
	}
}

/*
char Pin    - номер входа биты 0 - 5 , 6 - инверсия входа !, 7 - да/нет
bool RISE   - 1 - срабатывание по фронту - включению
bool FALL   - 1 - срабатывание по спаду - выключению
return char - 1 - фронт(вкл) ,0 - спад(выкл) ,2(NO_P) - вход не работает
если RISE 1 и FALL 1 то вход работает по уровню , (если оба 0 то тоже но не будет выделения фронта и спада)
*/
char Menu::inP(char Pin,bool RISE /*= false*/,bool FALL /*= false*/)
{
	static uint32_t p_in_mem;  //работаем с входами пульта
	static uint32_t m_in_mem;  //работаем с маркерами
	uint32_t* mem_ref = &p_in_mem;
	char res = NO_P;

	if(Pin & YESNO_P_bm)
		return res;

	const uint32_t BIT_IN = plb2l(Pin & DATA_P_bm);
	cli();
	uint32_t p_in;
	if(Pin & P_OR_m_bm)
		p_in = (uint32_t)init.p_in; //вход P
	else{
		p_in = init.m_in; // вход m
		mem_ref = &m_in_mem;
	}
	sei();
	if(Pin & INVERSE_P_bm){
		if(p_in & BIT_IN)// сравниваем номер параметра и номер входа приходящий
			res = 1;
		else
			res = 0;
	}
	else{
		if(!(p_in & BIT_IN))
			res = 1;
		else
			res = 0;
	}

	if(RISE || FALL){
		if(res){
			if(!((*mem_ref) & BIT_IN)) // фронт
				(*mem_ref) |= BIT_IN;
			else
				res = HV_P; // держится 1
			if(!RISE)
				res = NO_P;
		}
		else{
			if((*mem_ref) & BIT_IN) // спад
				(*mem_ref) &= ~BIT_IN;
			else
				res = LV_P;// держится 0
			if(!FALL)
				res = NO_P;
		}
	}
	return res;
}
/*
int x_place_axis - координата оси по х , место где высвечивается буква оси
char only_axis - 1 -показ только буквы оси , 0 - буквы + разъем
char type - тип разъема 0 - входной для датчиков  , 1 - выходной задание
*/
int Menu::show_axis_above_menu2(int x_place_axis,char only_axis/*= 0*/ ,char type /*= 0*/)
{
	for(int c = 0 ; c < N_AXIS + N_SA;c++){
		if(flash_prg.ch == c){
			display.print_flash_local(sAxis_screen[P2[c]], 0, x_place_axis, 0);
			if(!only_axis){
				if (!type)
					display.print_flash_local(sP2[c], 0, x_place_axis + 5, 0);
				else
					display.print_flash_local(sP3[c], 0, x_place_axis + 5, 0);
			}
			return 0;
		}
	}

	display.print_flash_local(PSTR("ошибка"), 0, x_place_axis + 5, 0);
	return -1;   
}

bool Menu::strncmp_lir(const char *str1, const char *str2, int maxcount)
{
	for (int x = 0; x < maxcount; x++){
		if (str1[x] != str2[x]) 
			return false;
	}
	return true;
}

 
void Menu::Menu_actions()
{
		switch(Lir.curItem->id)
		{
		#if (configUSE_NCshell == 1)
			case idG0:
			actions_m_G1_G0(0);
			break;
			case idG1:
			actions_m_G1_G0(0);
			break;	
			case idG1Input:
			case idG0Input:
			actions_G1_G0_input_pr(0);
			break;		
			case idChpa:
			actions_CHPA(0);
			break;
			case idSpdZ:
			actions_SPDZ(0);
			break;
			case idCntrl:
			actions_CNTRL(0);
			break;
			case idCntrlInput:
			actions_CNTRL_input_pr(0);
			break;
			case idZoneBrake:
			actions_ZNBRK(0);
			break;
			case idZoneBrakeInput:
			actions_ZNBRK_input_pr(0);
			break;
			case idVixodPM:
			actions_VIXPM(0);
			break;
			case idVixodPMInput:
			actions_VIXPM_input_pr(0);
			break;
			case idViborkaLuft:
			actions_VIBLUF(0);
			break;
			case idViborkaLuftInput:
			actions_VIBLUF_input_pr(0);
			break;
			case idInAxis:
			actions_INAXI(0);
			break;
			case idOutAxis:
			actions_OUTAXI(0);
			break;
			case idLubric:
			actions_LUBR(0);
			break;
			case idLubricInput:
			actions_LUBR_input(0);
			break;
			case idDiscZ:
			actions_DISCZ(0);
			break;
			case idAnalZ:
			actions_ANALZ(0);
			break;
			case idAnalZInput:
			actions_ANALZ_input_pr(0);
			break;
			case idViewOutputs:
			actions_DOUT(0);
			break;
			case idViewOutputsInput:
			actions_DOUT_input(0);
			break;
			case idInGen:
			actions_INGEN(0);
			break;
			case idFInGen:
			actions_FINGE(0);
			break;
			case idSInGen:
			actions_SINGE(0);
			break;
			case idMInGen:
			actions_MINGE(0);
			break;
			case idOutGen:
			actions_OUTGEN(0);
			break;
			case idFOutGen:
			actions_FOUTGE(0);
			break;
			case idSOutGen:
			actions_SOUTGE(0);
			break;
			case idMOutGen:
			actions_MOUTGE(0);
			break;
			
		#endif
		#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
			case idCorr:
			actions_CORR(0);
			break;
			case idIzmer:
			actions_IZMER(0);
			break;
		#endif
			case idHand_input:
			actions_input_HNDI();
			break;
			//==Смещения координат
			case idCoordOffset:
			actions_OFFSN(0);
			break;
			case idCoordOffsetInput:
			actions_input_OFFSN(0);
			break;
			//case idAxisListOffset:
			//actions_FSET(0);
			//break;
			case idAxisListOffsetInput:
			actions_input_FSET(0);
			break;
			//==
			case idDebugMode:
				actions_DBG(0);
				break;
			case idAxislist:
				actions_AXILIST(0);
				break;
			case idOption:
				actions_OPT(0);
				break;
			case idOptionInput:
				actions_input_OPT(0);
				break;
			case idScreenOcb:
				actions_m_da(0);
				break;
			case idPultOcb:
				actions_m_pa(0);
				break;
			case idAllp:
				actions_m_gs(0);
				break;
			case idBegin:
				actions_m_BEGN(0);
				break;
			case idHand:
				actions_hand(0);
				break;
			case idDiag:
				actions_m_DIAG(0);
				break;
			case idDiagInput:
				actions_m_DIAG_input(0);
				break;
			case idInputIndicOpt:
				actions_Cntrl_input_pr(0);
				break;
			case idIndicOpt:
				actions_cntr_pr(0);
				break;
		#if (configUSE_ABS_Encoder == 1)
			case idOABS:
				actions_OABS(0);
				break;
		#endif
		#if (configUSE_mcx514 == 1)			 
			case idInputCntrlOpt:
				actions_Cntrl_input(0);
			break;
			case idHandWeelOpt:
				actions_cntr_hw(0);
			break;
			case idInputHandWeelOpt:
				actions_Cntrl_input_hw(0);
			break;
			case idCntrlopt:
				actions_cntr(0);
			break;
		#endif
		#if (configUSE_NCshell == 1)
			case idAnout:
				actions_ANOUT();
			break;
			case idAnin:
				actions_ANIN();
			break;
			case idAninInput:
				actions_input_ANIN();
			break;
			case idAnoutInput:
				actions_input_ANOUT();
			break;
			case idHandMove:
				actions_mh(0);
			break;
			case idExecmd:				
				actions_exe(0);
			break;
			case idCmd:
				actions_cmd(0);
			break;
			case idPrgOrSubPrg:
				actions_Prg_SubPrg(0);
			break;
			case idListProg:
				actions_LISPRG(0);
			break;
			case idListSubProg:
				actions_LISSUB(0);
			break;
			case idEditPrg:
				actions_EDITPR(0);
			break;
			case idEditPrgInput:
				actions_EDITPR_input(0);
			break;
			case idInstrC:
				actions_INSTRC(0);
			break;
			case idInstrCInput:
				actions_input_INSTRC(0);
			break;
		#endif
			case idCalcfrom_menu:
			#if (configUSE_Calc == 1)
				//InitCalcFrommenu();
				SET_C(idCalc);
				display.turn_on_page(1);
				display.page_for_buttons = 1;
				Lir.save_load_calc(2);
				Lir.Calcfrommenu = 1;
			#else
				SET_C(idDiag);
			#endif

			break;

			case idCalc:
			#if (configUSE_Calc == 1)
				action_calc(1);
			#else
				SET_C(idDiag);
			#endif
			break;
		#if (configUSE_Linear_holes == 1)
			case idInpointLinear:
				action_Linear_holes();
			break;
			case idInpointLinearInput:
				action_input_linear_input(0);
			break;
			case idInpointLinearPUSK:
				action_input_linear_PUSK(0);
			break;
		#endif
		#if (configUSE_Circle_holes == 1)
		    case idInpointRadius:			
				action_input_radius();
			break;
			case idInpointRadiusInput:
				action_input_radius_input(0);
			break;
			case idInpointRadiusPUSK:
				action_input_radius_PUSK(0);
			break;
		#endif					
#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
			case idSeeCorr:
				actions_SEECOR(0);
				break;
			case idGetCorr:
				actions_GTCOR(0);
				break;
			case idIndikator:
				actions_INDI(0);
			break;
			case idGraph:
				actions_GRPH(0);
			break; 
#endif					
			default:
			break;
		}
}
/*
const int id  - текущий контекст
int8_t &cnt - текущий счетчик указателя
const uint8_t SZ - текущий размер меню
m_state &entry - текущее состояние меню
*/
void Menu::menu_up(const int id,int8_t &cnt,const uint8_t SZ,m_state &entry)
{
	if (curItem->id == id) {
		cnt--;
		if (cnt < 1)
			cnt = SZ;
		entry = UPDATE;
	}
}
/*
const int id  - текущий контекст
int8_t &cnt - текущий счетчик указателя
const uint8_t SZ - текущий размер меню
m_state &entry - текущее состояние меню
*/
void Menu::menu_down(const int id,int8_t &cnt,const uint8_t SZ,m_state &entry)
{
	if (curItem->id == id) {
		cnt++;
		if (cnt > SZ)
			cnt = 1;
		entry = UPDATE;
	}
}
/*
пропустить пункт меню
bool updown - 0 вверх 1- вниз
int8_t &cnt - счетчик
int num - номер пропускаемой строки
*/
void Menu::skip_menu_item(bool updown,int8_t &cnt,int num)
{
	if(updown){
		if(cnt == num)
			cnt++;
	}
	else{
		if(cnt == num)
			cnt--;
	}
}

void Menu::key_start_actions(char &k_c)
{
#if (configUSE_NCshell == 1)
		menu_start_exe();	
		move_axis_START();
		menu_START_DEALLP();
#endif
#if (configUSE_mcx514 == 1)
#if (configUSE_Linear_holes == 1)
	if(curItem->id == idInpointLinearPUSK)
		Lir.drive_next_hole_lh();
#endif
#endif
		if(Lir.Activation_Reset_Parametrs(k_c))
		{
			if(Lir.curItem->id == idResetParametrs)
			{
				if((Lir.ResetParametrsinRAM()) && (flash_prg.reset_eeprom_param()))
					Lir.showActionResetparam2();
				else
					display.print_flash_local(PSTR("Ошибка при сбросе осевых параметров"),0,1,100);
			}
			if(Lir.curItem->id == idResetOffset)
			{
				if(flash_prg.G53_and_G5x_to_clear())
					Lir.showActionResetOffset();
				else
					display.print_flash_local(PSTR("Ошибка при сбросе смещений"),0,1,100);
			}
		}
		
}

void Menu::key_stop_actions()
{
#if (configUSE_Linear_holes == 1)
	if(exit_lh_PUSK())
		return;
	if(exit_ch_PUSK())
		return;	
#endif		
}


void Menu::take_buttons(char& k_c)
{
#if (configUSE_menuItem_Correction_and_Izmerenia == 1)		
		if(curItem->id == idGetCorr)
			flash_prg.to_take_buttons_code_for_correction(k_c);        // Снятие коррекции
		else if(curItem->id == idSeeCorr)
			flash_prg.to_take_buttons_code_and_to_see_correction(k_c); // Работа с таблицами
		else if(curItem->id == idIndikator)
			flash_prg.to_take_buttons_code_for_new_technical_task(k_c);
		else if(curItem->id == idGraph)
			command_GRPH(k_c); 
#endif
}

void Menu::gotoStart()
{
	delay_ms(1750);
	SET_C(idBegin);
	MsgKey.mode =TMsgKey::MODE::BEGIN;
	l_k_s.OFF_LED(_led_MODE_DIAGNOSTICS);
}


void Menu::GoToMenu(int id)
{
   SET_C(id);
   if(id == idBegin)
		entry_BEGN = INIT;
}
/*
отрисовка меню
const char** menu_ - массив указателей на указатели ,обязательно должен заканчиваться нулевым указателем
uint8_t cnt - текущее выделение пункта меню
const unsigned char& page - страница памяти дисплея
*/
void Menu::Draw_menu(const char** menu_, uint8_t cnt,const unsigned char& page)
{
	for (uint8_t str = 0; menu_[str] != nullptr; str++) {
		if (str == cnt)
			INVERT_CHAR()
		else
			NORMAL_CHAR()
		display.print_flash_local(menu_[str], page, 0, 16 * str);
	}
	NORMAL_CHAR()
}

void Menu::LirSnake()
{
#if (configUSE_SnakeGame == 1)
   switch(curItem->id)
   {
       case SnakeInit:
       display.clearram=true;
       display.clear_ram_1();
       initsnakegame();
	   SET_C(SnakeLoop);
       break;

       case SnakeLoop:
       if (cursorrunner == speed)// game speed
       {
	       if (!snakeMoveLir(dire))
	       {
		       if(cash!=50){
			       display.printf_flash2(PSTR("игра закончена"), 0, 7, 16*5);
				   sn_win = 0;
				   }
		       else{		
					display.printf_flash2(PSTR("победа!"), 0, 10, 16*5);
					sn_win = 1;
				   }
				   				  
		       delay_ms(2000);
			   SET_C(SnakeInit);
	       }
	       cursorrunner = 0;
       }
       //count++;
       break;
    default:
    break;
   }
#else
switch(curItem->id)
{
	case SnakeInit:
		SET_C(idAbout);
	break;
	default:
	break;
}
#endif
}





void Menu::UpdateTypeAxis(int ch)
{

	int axename = (int)P2[ch];

	if (axename >= 0 && axename < 9)
	flash_prg.axis_cfg[ch].type = CPrg::TYPE::LINEAR;

	if (axename > 8 && axename < 13)
	flash_prg.axis_cfg[ch].type = CPrg::TYPE::CIRCLE;
}

void Menu::UpdateStr()
{
	for(int i = 0 ;  i < N_STR; i++){
		int j = 0;
		for(; j < N_AXIS;j++){
			if(Str[i] ==  P2[j])
			break;
		}
		if(j == 4)
		Str[i] = no_axis;
	}
}

void Menu::UpdateFormatAxis(int ch)
{
	int axename = (int)P2[ch];

	if (axename >= 0 && axename < 9)
	{
		switch (flash_prg.axis_cfg[ch].format)
		{
			case CPrg::FORMAT::CIRCLE_4:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::LINEAR_4;
			break;
			case CPrg::FORMAT::CIRCLE_3:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::LINEAR_3;
			break;
			case CPrg::FORMAT::CIRCLE_2:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::LINEAR_2;
			break;
			case CPrg::FORMAT::CIRCLE_1:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::LINEAR_1;
			break;
			case CPrg::FORMAT::CIRCLE_SECUNDO:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::LINEAR_3;
			break;
			default:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::LINEAR_4;
			break;
		}
	}

	if (axename > 8 && axename < 13)
	{
		switch (flash_prg.axis_cfg[ch].format)
		{
			case CPrg::FORMAT::LINEAR_INCH_4:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::CIRCLE_4;
			break;
			case CPrg::FORMAT::LINEAR_4:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::CIRCLE_4;
			break;
			case CPrg::FORMAT::LINEAR_3:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::CIRCLE_3;
			break;
			case CPrg::FORMAT::LINEAR_2:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::CIRCLE_2;
			break;
			case CPrg::FORMAT::LINEAR_1:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::CIRCLE_1;
			break;
			default:
			flash_prg.axis_cfg[ch].format = CPrg::FORMAT::CIRCLE_4;
			break;
		}
	}

}

void Menu::UpdateFormatAxis2(int ch)
{
	int axename = (int)P2[ch];

	if (axename >= 0 && axename < 9)
		flash_prg.axis_cfg[ch].format = CPrg::FORMAT::LINEAR_3;

	if (axename > 8 && axename < 13)
		flash_prg.axis_cfg[ch].format = CPrg::FORMAT::CIRCLE_3;
}

void Menu::WritetoEepromAxisOptions(int ch)
{

}


 void Menu::number_key(char &k_c)
 {  

	 switch(curItem->id)
	 {   
		 case idCoordOffsetInput:   //ввод пароля для G53
		 case idAxisListOffsetInput://ввод смещений
		 case idOptionInput:       // ввод для настроек
		 case idDiagInput:         // ввод для диагностики
		 case idInpointRadiusInput:// ввод для отверстий на окружности
		 case idInpointLinearInput:// ввод для отверстий на прямой
		 case idCalc:              // ввод для калькулятора
		 case idHand_input:        // ввод для преднабора
		 case idGetCorr_input:     // ввод для меню Снятие коррекции
		 case idWorkCorr_Input:    // ввод для работы с таблицами коррекций
		 case idCmd:               // ввод для командной строки
		 case idInputCntrlOpt:     // ввод для настроек управления осями
		 case idInputIndicOpt:     // ввод для настроек индикации
		 case idInputHandWeelOpt:  // ввод для настроек маховика
		 case idCntrlInput:        // ввод для настроек управление осями лир541
		 case idZoneBrakeInput:    // ввод для Зон торможения
		 case idEditPrgInput:      // ввод для Редактора Программ/Подпрограмм
		 case idInstrCInput:       // ввод для Инструмента
		 case idAnalZInput:        // ввод для Аналоговое задание
		 case idG0Input:		   // ввод для Скорости G0
		 case idG1Input:		   // ввод для Скорости G1
		 case idVixodPMInput:      // ввод для Выход в PM
		 case idViborkaLuftInput:  // ввод для Выборка Люфта
		 case idViewOutputsInput:  // ввод для Выходы
		 case idAnoutInput:        // ввод для Аналоговых входов
		 case idAninInput:         // ввод для Аналоговых выходов
		 case idLubricInput:       // ввод для Смазки

		if(cursor_pos<inputnum )
		{
			if(trigger_Point_min_sec && k_c != _key_POINT)
					trigger_Point_min_sec = 0;

			switch(k_c){
				case _key_0:  result[1] = '0';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_1:  result[1] = '1';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_2:  result[1] = '2';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_3:  result[1] = '3';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_4:  result[1] = '4';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_5:  result[1] = '5';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_6:  result[1] = '6';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_7:  result[1] = '7';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_8:  result[1] = '8';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_9:  result[1] = '9';bufASCIIstr[cursor_pos++] = result[1];break;
				case _key_UP:
				if(curItem->id == idWorkCorr_Input){
					Lir.WorkCorr_entry = PREV_POINT;
					ResultForInputNum = input::ok;
				}
				break;
				case _key_DOWN:
				if(curItem->id == idWorkCorr_Input){
					ResultForInputNum = input::ok;
				}
				break;
				case _key_X:
				case _key_Y:
				case _key_Z:
					break;
				case _key_BLANK1: 			
				case _key_BLANK2:
				case _key_BLANK3:
				case _key_BLANK4:
					calc_blank_input(k_c);
					break;
				case _key_PLUS_MINUS:
					if (curItem->id == idCalc){
						Negative_input();
						return;
					}
					if(curItem->id == idHand_input){
						if((CurFormatM>=CPrg::FORMAT::LINEAR_3 && CurFormatM <=CPrg::FORMAT::LINEAR_1 )||
						CurFormatM == CPrg::FORMAT::LINEAR_INCH_4 || CurFormatM == CPrg::FORMAT::LINEAR_4)
						{
							if(result[1] != '-')//подряд не вводим
								result[1] = '-';
							else
								return;
							bufASCIIstr[cursor_pos] = result[1];
							cursor_pos++;
						}
					}else{
						if(result[1] != '-')//подряд не вводим
							result[1] = '-';
						else
							return;
						bufASCIIstr[cursor_pos] = result[1];
						cursor_pos++;
					}
				break;
				case _key_POINT: 
				      if(CurFormatM>=CPrg::FORMAT::LINEAR_3 && CurFormatM <=CPrg::FORMAT::CIRCLE_4 ){
						if(result[1] != '.')//подряд не вводим
							result[1] = '.';
						else
							return;
						}
				      else if(CurFormatM==CPrg::FORMAT::CIRCLE_SECUNDO || CurFormatM==CPrg::FORMAT::CIRCLE_MINUTO)
						 {
							if(Key_Point_min_sec())		//result[1] = '°';
								return;												
						 }
				      
					  bufASCIIstr[cursor_pos] = result[1];
					  cursor_pos++;
				break;

				case _key_C :   // стирание
				case _key_ENTER:
				case _key_STOP:
				case _key_LEFT:
				case _key_RIGHT:
					key_C_ENTER_STOP(k_c);
					break;
				default:break;
			}
			if(curItem->id == idDiagInput||curItem->id == idCoordOffsetInput) {
				result[1] = '*';// for pass
			} 
			
		}
		else
		{
			switch(k_c){
				case _key_C :
				case _key_ENTER:
				case _key_STOP:
				case _key_LEFT:
				case _key_RIGHT:
					key_C_ENTER_STOP(k_c);
					break;
				default:break;
			}
		}
		break;

		default:
		break;
	  }		 
 }


void Menu::key_C_ENTER_STOP(const unsigned char & k_c)
{
	switch(k_c){
 		case _key_LEFT:
 		if (cursor_pos > 0)
 		cursor_pos--;
 		break;
 		case _key_RIGHT:
 		{
 			uint16_t n = display.strnlen_lir((const char*)bufASCIIstr, IN_BUF_SZ);
 			if (cursor_pos < n)
			cursor_pos++;
 		}
 		break;
		case _key_C :
			{
			uint16_t n = display.strnlen_lir((const char*)bufASCIIstr, IN_BUF_SZ);
			if (n != cursor_pos)
				break;
			}
			if (cursor_pos > 0)
				cursor_pos--; // дикрементируем курсор т е сдвигаем влево
			bufASCIIstr[cursor_pos] = 0;  // удаляем символ в буфере(Важно! в этом буфере хранится нужная нам строка)
			if (cursor_pos > 0)
				result[1] = bufASCIIstr[cursor_pos-1]; // выводим символ перед курсором который стоит перед удаленным символом
			clr_cursor = true;
			break;
		case _key_ENTER:
			ResultForInputNum = input::ok;
			if (curItem->id == idCalc) 
				calc_entry = FINISH_INPUT;
			break;
		case _key_STOP:
			if(curItem->id != idCalc)
				ResultForInputNum = input::cancel_;
			break;
		default:break;
	}
}

bool Menu::Activation_Reset_Parametrs(char &_k_c)
{
    switch(curItem->id)
	{
	    case idResetOffset:
	    case idResetParametrs:
		     switch(_k_c)
		     {
			     case _key_START:
			     buf_for_reset[cursor_pos]= cursor_pos + 1 + 0x30;
				 cursor_pos++;
			     break;
			     default:
			     break;
		     }
		break;

	    default:
		break;
	}

	if(curItem->id == idResetParametrs)
		display.print_ram_local((char*)buf_for_reset,0,1,64);

	if (buf_for_reset[0]== '1' && buf_for_reset[1]== '2' && buf_for_reset[2]== '3' && curItem->id == idResetParametrs)
	{		
		buf_for_reset[0] = 0;
		buf_for_reset[1] = 0;
		buf_for_reset[2] = 0;
		buf_for_reset[3] = 0;
		cursor_pos=0;
		return true;
	}
	if (buf_for_reset[0]== '1' && curItem->id == idResetOffset)
	{
		buf_for_reset[0] = 0;
		buf_for_reset[1] = 0;
		buf_for_reset[2] = 0;
		buf_for_reset[3] = 0;
		cursor_pos=0;
		return true;
	}
	return false;
}

 void Menu::strcpylir(char*_Dest, char const* _Source)
 {
  // string copying
   int index;
   index = 0;
   while (_Source[index] != '\0')
   {
	   _Dest[index] = _Source[index];
	   index++;
   }
   _Dest[index] = 0x00;
 }
 

bool Menu::checkInputStr(char * str)
{
	int n;
	n = display.strnlen_lir(str, IN_BUF_SZ);

	if (n == 0)return true;    // пустой ввод

	int point = 0;
	int min = 0;
	int sec = 0;
	int gradus = 0;
	int minus = 0;
	for (int i = 0; i < n; i++)
	{
		if (str[i] == '.')  point++;
		if (str[i] == 0x01) min++;   // lir 0x01
		if (str[i] == 0x02)  sec++;   // lir 0x02
		if (str[i] == '°')  gradus++;
		if (str[i] == '-')  minus++;
	}

	if (point > 1)  return true;
	if (min > 1)    return true;
	if (sec > 1)    return true;
	if (gradus > 1) return true;
	if (minus > 1) return true;

	if(minus == 1)
	{
	  if(str[0] != '-') return true;
	}

	return false;
} 
 
void Menu::ChangeSpeedOnScreen()
{
	int menucount = menu_cnt_da - 1;
	if (menucount >3) 
		return;
	if(curItem->id == idScreenOcb){

		uint8_t plus_str = 0;
		uint8_t result_str = 0;

		for(int i = 0 ; i < N_STR;i++){
			if(display.position_to_see[i] == CDisp::PLUS && Str[i] != no_axis)
				plus_str++;
			if(display.position_to_see[i] == CDisp::RESULT && Str[i] != no_axis)
				result_str++;
		}
	
		if(Str[menucount] != no_axis)
		{
			if(display.position_to_see[menucount] == CDisp::COORDINATE)
				display.position_to_see[menucount] = CDisp::SPEED;
			else if(display.position_to_see[menucount] == CDisp::SPEED && plus_str < 2)
				display.position_to_see[menucount] = CDisp::PLUS; 
			else if(display.position_to_see[menucount] == CDisp::SPEED && result_str < 1)
				display.position_to_see[menucount] = CDisp::RESULT;
			else if(display.position_to_see[menucount] == CDisp::PLUS && result_str < 1)
				display.position_to_see[menucount] = CDisp::RESULT; 
			else
				display.position_to_see[menucount] = CDisp::COORDINATE;
		}
		else{
			if(display.position_to_see[menucount] == CDisp::COORDINATE)
				display.position_to_see[menucount] = CDisp::TIME;
			else if(display.position_to_see[menucount] == CDisp::TIME)
				display.position_to_see[menucount] = CDisp::COORDINATE;
			else
				display.position_to_see[menucount] = CDisp::COORDINATE;
		}	
	}
}
/*
функция вызывается после изменения отображаемой оси в строке
*/
void Menu::UpdateSpeedOnScreen()
{
	for(int x= 0 ;x<N_STR;x++)//if(curItem == &mScreenOcb[menucount + 1] && buf[menucount] != no_axis)
	{
		if(Str[x] == no_axis){
			if(display.position_to_see[x] == CDisp::SPEED || 
				display.position_to_see[x] == CDisp::PLUS ||
				display.position_to_see[x] == CDisp::RESULT)
				{
					display.position_to_see[x] = CDisp::COORDINATE;
					display.print_flash_local(PSTR("        "),0,16,(x+1)*16);
				}
			else if(display.position_to_see[x] == CDisp::TIME)	
					display.print_flash_local(sTme,0,16,(x+1)*16);	
			else if(display.position_to_see[x] == CDisp::COORDINATE)
					display.print_flash_local(PSTR("        "),0,16,(x+1)*16);
			else
				display.print_flash_local(PSTR("   ?    "),0,16,(x+1)*16);
		}
		else{
			if(display.position_to_see[x] == CDisp::COORDINATE)
				display.print_flash_local(PSTR("        "),0,16,(x+1)*16);
			else if(display.position_to_see[x] == CDisp::SPEED)
				display.print_flash_local(sSpd,0,16,(x+1)*16);
			else if(display.position_to_see[x] == CDisp::PLUS)
				display.print_flash_local(PSTR("   +    "),0,16,(x+1)*16);
			else if(display.position_to_see[x] == CDisp::RESULT)
				display.print_flash_local(PSTR("   =    "),0,16,(x+1)*16);
			else if(display.position_to_see[x] == CDisp::TIME)
			{
				display.position_to_see[x] = CDisp::COORDINATE;
				display.print_flash_local(PSTR("        "),0,16,(x+1)*16);
			}
			else
				display.print_flash_local(PSTR("   ?    "),0,16,(x+1)*16);				
		}
	}
	/*for(int x= 0 ;x<N_STR;x++)//if(curItem == &mScreenOcb[menucount + 1] && buf[menucount] != no_axis)
	{
		if(display.position_to_see[x] == CDisp::COORDINATE || Str[x] == no_axis)
		{
			if (display.position_to_see[x] == CDisp::SPEED && Str[x] == no_axis)
				display.position_to_see[x] = CDisp::COORDINATE;

			display.print_flash_local(PSTR("        "),0,16,(x+1)*16);
		}
		else
		{
			display.print_flash_local(PSTR("скорость"),0,16,(x+1)*16);
		}
	} */
}

void Menu::ChangeSettings(unsigned char &b)
{

   // if (menucount == 2)
   // {
	    if (b == usb9600)
		    b = usb115200;
	    else if (b == usb115200)
		    b = usb200000;
		else if (b == usb200000)
			b = usb9600;

	    //curItem = &mOption[3];
   // }
}

 void Menu::ShowResetParmetrs()
 {
   display.clear_ram_null();
   cursor_pos=0;
   display.print_flash_local(sSbrosParam,0,0,0);
   display.print_flash_local(sSbrosParam01,0,0,20);
   display.print_flash_local(sSbrosParam02,0,0,35);
   display.print_flash_local(sSbrosParam03,0,0,50);
   memset(buf_for_reset,0x00,4);
 }
 
 
void Menu::ShowResetOffset()
{
	display.clear_ram_null();
	cursor_pos=0;
	display.print_flash_local(sResetOffset,0,0,0);
	display.print_flash_local(sResetOffset01,0,0,20);
	display.print_flash_local(sResetOffset02,0,0,35);
	display.print_flash_local(sResetOffset03,0,0,50);
}

 bool Menu::gotomenuDiagnostika()
 {
     if (curItem->id == idBegin || curItem->id == idHand || curItem->id == idRef){
		GoToMenu(idDiag);		
		return true;
     }
	 else{
		if(exitmenuDiagnostika(idBegin))
			return false;
		else
			return true;
	 }
 }
 
 
bool Menu::gotoModeHand()
{
    if(curItem->id == idHand)
    {
	    GoToMenu(idBegin);		
	    return false;
    }
    else{
	    if (curItem->id == idBegin || curItem->id == idRef)
	    {
		    GoToMenu(idHand);			
		    return true;
	    }
	    else
	    {
		    if(exitmenuDiagnostika(idHand))			
				return true;
		    else			
				return false;
	    }
    }
}

bool Menu::gotoModeRef()
{
    if(curItem->id == idRef){
	    GoToMenu(idBegin);		
		return false;
    }
    else{
	    if (curItem->id == idBegin || curItem->id == idHand){
		    GoToMenu(idRef);
		    return true;
	    }
	    else{
		    if(exitmenuDiagnostika(idRef))
			    return true;
		    else
			    return false;
			}
		}
}

bool Menu::gotoModeRef_from_button()
{
	l_k_s.OFF_LED(_led_MODE_HAND);
	l_k_s.OFF_LED(_led_MODE_DIAGNOSTICS);
	l_k_s.ON_LED(_led_REF);
	screen.to_clear_place_for_virtual_buttons(4);
	screen.to_clear_string_connectors(4);
	MsgKey.follower_ref = TMsgKey::PURE_REF;
	MsgKey.to_look_for_submode_in_ref_mode();
	status(PSTR(""));
	return true;
}

 bool Menu::exitmenuDiagnostika(int id)
 {
	if(curItem->id == idDiag){
		entry_DIAG = INIT;
		mOFFSNPassG53Ok = false;
		GoToMenu(id);
		//==сброс всех выходов в ноль
		#if (configUSE_NCshell == 1)
		Lir.rst_outP();
		#endif
		//==
		return true;
	}
	else
		return false;
 }



void Menu::check_pointer()
{
   if(curItem->id == 0  || curItem == nullptr)
   {
      display.clear_ram_null();
	  display.print_flash_local(PSTR("check_pointer(): указатель пуст!" ),0,0,48);
	  display.print_flash_local(PSTR("адрес объекта:" ),0,0,64);
	  display.print_debug_one_local((long)curItem,0,18,64);
	  Lir.gotoStart();
   }
}

void Menu::show_interval_izmer_speed(int ch,int x_offset,int y_offset,char align)
{
	if(flash_prg.axis_cfg[ch].interval_to_measure_speed == CPrg::RANGE::INTERVAL0_1)
		display.print_flash_local(s01sek,0,x_offset,y_offset,align); 
	if(flash_prg.axis_cfg[ch].interval_to_measure_speed == CPrg::RANGE::INTERVAL0_3)
		display.print_flash_local(s03sek,0,x_offset,y_offset,align); 
	if(flash_prg.axis_cfg[ch].interval_to_measure_speed == CPrg::RANGE::INTERVAL0_6)
		display.print_flash_local(s06sek,0,x_offset,y_offset,align); 
}

bool Menu::find_same_symbol(uint16_t *symbols,int cur)
{
	for(int i = 0;i<N_AXIS;i++)
	{
		if(cur == i)continue;
		if(symbols[cur] == symbols[i])
		{
			if(display.position_to_see[i] == CDisp::SPEED)
				return true;
		}
	}
	return false;
}

int Menu::find_same_symbol_and_return_str(CInit::AXIS_NAME symbols[],int cur)
{
	for(int i = 0;i<N_AXIS;i++)
	{
		if(cur == i)continue;
		if(symbols[cur] == symbols[i])
		{
			if(display.position_to_see[i] == CDisp::SPEED)
			return i;
		}
	}
	return -1;
}

int Menu::GetCountSpeedStr(uint8_t *param)
{
	int counter = 0 ;
	for(int i=0;i<N_AXIS;i++)
	{
		if(param[i] == 0)counter++;
	}
	return counter;
}

bool Menu::see_error_in_rectangle(unsigned char speed_error_memory[],unsigned char speed_error[],int i)
{
	//if(speed_error_memory[i] == speed_error[i]) return false;
	
	int y_pos_rect,y_pos_txt;
	if( i == 0 ) {y_pos_rect = 0;y_pos_txt = 8;}
	if( i == 1 ) {y_pos_rect = 7;y_pos_txt = 64;}
	if( i == 2 ) {y_pos_rect = 14;y_pos_txt = 120;}
	if( i == 3 ) {y_pos_rect = 21;y_pos_txt = 176;}	
	switch(speed_error[i]){
		
		case 1:
		display.clear_rectangle(40,y_pos_rect,232,5,0x00,0);
		display.print_flash_local(PSTR("ошибка скорости: "),0,5,y_pos_txt);
		display.put_char_local_independent('1',0,22,y_pos_txt);
		speed_error_memory[i] = speed_error[i];
		Update_big_digit[i] = true;
		return true;
		break;

		case 2:
		display.clear_rectangle(40,y_pos_rect,232,5,0x00,0);
		display.print_flash_local(PSTR("ошибка скорости: "),0,5,y_pos_txt);
		display.put_char_local_independent('2',0,22,y_pos_txt);
		speed_error_memory[i] = speed_error[i];
		Update_big_digit[i] = true;
		return true;
		break;

		case 3:
		display.clear_rectangle(40,y_pos_rect,232,5,0x00,0);
		display.print_flash_local(PSTR("ошибка скорости: "),0,5,y_pos_txt);
		display.put_char_local_independent('3',0,22,y_pos_txt);
		speed_error_memory[i] = speed_error[i];
		Update_big_digit[i] = true;
		return true;
		break;

		case 5:
		display.clear_rectangle(40,y_pos_rect,232,5,0x00,0);
		display.print_flash_local(PSTR("ошибка скорости: умножение"),0,5,y_pos_txt);
		speed_error_memory[i] = speed_error[i];
		Update_big_digit[i] = true;
		return true;
		break;

		case 6:
		display.clear_rectangle(40,y_pos_rect,232,5,0x00,0);
		display.print_flash_local(PSTR("ошибка скорости: предел"),0,5,y_pos_txt);
		speed_error_memory[i] = speed_error[i];
		Update_big_digit[i] = true;
		return true;
		break;

		default:
		speed_error_memory[i] = speed_error[i];
		Update_big_digit[i] = false;
		break;
		}
return false;
}



/*
функция для перехода между модами
const int& in   - мод в которой входим
const int& from - мод из которого выходим
char clear      - очистка всего экрана перед переходом
*/
void Menu::goto_mode(const int& in, const int& from,const char &clear)
{			
	if(in == TMsgKey::MODE::HAND && from == TMsgKey::MODE::INPUT_PARAM){
		if(curItem->id == idHandMove){
			return;
		}
		if(curItem->id == idCmd){
			#if (configUSE_NCshell == 1)
			exit_cmd(0);	
			#endif
			return;
		}
		#if (configUSE_NCshell == 1)
		if(curItem->id == idExecmd){						
			//exit_exe(0);
			return;
		}
		#endif
		if(Lir.exitmenuDiagnostika(idHand)){			
			if(clear)
				display.clear_ram(0x00,0);	
			#if (configUSE_NCshell == 0)	
			MsgKey.tracer_next = MsgKey.tracer_real = TMsgKey::FUNC;
			#else				
			MsgKey.tracer_next = MsgKey.tracer_real;
			#endif
			MsgKey.to_look_for_submode_of_operation_inside_HAND();
			MsgKey.submode_change_offset_inside_HAND();
			Lir.init_hand(0);
		}
		else
			return;
	}	

	MsgKey.mode = (TMsgKey::MODE)in;
	MsgKey.old_mode = (TMsgKey::MODE)from;
}



/*
функция конвертирует из строчного массива в бинарное число с учетом формата

in unsigned char*input_str - указатель на входящую строку
const unsigned char& ch    - канал

return value
 long  - результат преобразования
*/
long Menu::String_to_bin(unsigned char*input_str,const unsigned char& ch)
{
	int f = flash_prg.axis_cfg[ch].format;
	long bin_counter = 0;
	if((f >= CPrg::FORMAT::LINEAR_3 && f<=CPrg::FORMAT::CIRCLE_4) || f == CPrg::FORMAT::LINEAR_INCH_4){
		ConvertASCIIlinear_circle_tolong(bin_counter,f,input_str);
		l_d(bin_counter,ch,in_mem);
	}
	if(f == CPrg::FORMAT::CIRCLE_SECUNDO || f == CPrg::FORMAT::CIRCLE_MINUTO){
		ConvertASCIIcircle_secundo_tolong_new(bin_counter,f,input_str);
	}
	return bin_counter;
}

char Menu::Key_Point_min_sec()
{
// если мы хотим на одну кнопку  три знака  '°'1)точка  '''2)минуты  '"'3)секунды

	int sym_pos = 0;

	if (!trigger_Point_min_sec)
	{
		cursor_pos++;
		trigger_Point_min_sec = 1;
	}

	if (trigger_Point_min_sec)
	{
		if (cursor_pos > 0)
			sym_pos = cursor_pos - 1;
	}
	//=== в формате CIRCLE_MINUTO исключаем секунды
	if(CurFormatM == CPrg::FORMAT::CIRCLE_MINUTO && key_point_counter == 2)
		key_point_counter = 0;
	//==

	switch (key_point_counter) {
	case 0:
		result[1] = '°';
		key_point_counter = 1;
		break;
	case 1:
		result[1] = 0x01;
		key_point_counter = 2;
		break;
	case 2:
		result[1] = 0x02;
		key_point_counter = 0;
		break;
	default:
		break;
	}
	bufASCIIstr[sym_pos] = result[1];
	return 1;
}

void Menu::Negative_input()
{
	#define NEGATIVE 0
	#define NO_NEGATIVE 1
	static char action;

	if (!(result[1] >= '0' && result[1] <= '9')) return;
	///ставим минус
	if (action == NEGATIVE)
	{
		set_negative:
		int i = cursor_pos;
		for (; i >= 0; i--)
		{
			bufASCIIstr[i + 1] = bufASCIIstr[i];

			if (bufASCIIstr[i] == '+' || bufASCIIstr[i] == '-' || bufASCIIstr[i] == '*' || bufASCIIstr[i] == '/')
			{
				bufASCIIstr[i + 1] = '±';
				break;
			}

			if (i == 0)bufASCIIstr[i] = '±';
		}
		cursor_pos++;
		action = NO_NEGATIVE;
		Update_input_str = true;
		return;
		//выводим строку
		//выходим из функции
	}

	///// убираем минус
	//находим минус
	if (action == NO_NEGATIVE)
	{
		unsigned int  j = 0;
		for (; j < cursor_pos; j++)
		{
			if (bufASCIIstr[j] == 0xB1)
			break;
		}
		if (j == cursor_pos) goto set_negative;  // если знак отрицания не найден, то переходим на его установку

		for (int i = cursor_pos; i >= 0; i--)  // проверяем есть ли переменная справа от знака, если есть то переходим на установку знака
		{
			if ((bufASCIIstr[i] == '+' || bufASCIIstr[i] == '-' || bufASCIIstr[i] == '*' || bufASCIIstr[i] == '/') && bufASCIIstr[i+1] != 0xB1)
			goto set_negative;
			if ((bufASCIIstr[i] == '+' || bufASCIIstr[i] == '-' || bufASCIIstr[i] == '*' || bufASCIIstr[i] == '/') && bufASCIIstr[i + 1] == 0xB1)
			{
				j = i + 1;
			}
		}

		for (unsigned int i = j; i <= cursor_pos; i++)
		{
			bufASCIIstr[i] = bufASCIIstr[i + 1];
		}
		cursor_pos--;
		//выводим строку
		//выходим из функции
		action = NEGATIVE;
		Update_input_str = true;
		return;
	}
}


unsigned char Menu::find_grd_min_sec()
{
	for(int i = 0 ;i<IN_BUF_SZ;i++)
	{
		if(bufASCIIstr[i] == 0x01) return 2;
		if(bufASCIIstr[i] == 0x02) return 3;
		if(bufASCIIstr[i] == '°') return 1;
	}

	return 0;
}
/*
 in out long &counter  - счетчик дискрет
*/
void Menu::checklimit_for_rectangle_Output(long &counter)
{
	if (counter>=100000000)
		counter=99999999;
	if (counter<=-100000000) 
		counter=-99999999;
}

void Menu::change_sound()
{
	sound_and_other ^=0x01;
}

void Menu::change_speed_out_disp()
{
	sound_and_other ^=0x02;
}

void Menu::change_Zaxvat_oshibki_datchika()
{
	sound_and_other ^=0x04;
}

void Menu::change_Extern_PLC()
{
	sound_and_other ^= 0x08;
#if (configUSE_PLC == 1)
	cli();
	lock_error_PLC = false;	
	uart_485.crc_errors_Rx = 0;
	uart_485.crc_errors_Tx = 0;
	sei();
#endif
}

void Menu::change_Erase_Prednabor()
{
	sound_and_other ^= 0x10;
}

bool Menu::get_Extern_PLC()
{
	if(sound_and_other & 0x08)
		return false; // нет ПЛК
	else
		return true;  // есть ПЛК
}

bool Menu::get_Erase_Prednabor()
{
	if(sound_and_other & 0x10)
		return false; // нет Очистки
	else
		return true;  // есть Очистка
}

bool Menu::get_Disp_upd()
{
	if(sound_and_other & 0x02)
		return 1;
	else
		return 0;
}

/*
возведение в степень числа 2
сдвиг в пределах 2 байт, на 1 mks меньше
для получения бита
максимальная stepb 15
из 4 в 00000000 00010000
*/
uint16_t Menu::plb2(uint8_t stepb)
{
	uint16_t tmp = 0;
	//stepb = stepb&15;
	tmp = 1 << stepb;
	return tmp;
	/*uint16_t tmp = 1; //тоже самое
	while(stepb--)
	tmp *= 2;
	return tmp;	*/
}
/*
возведение в степень числа 2
сдвиг в пределах 4 байт, на 1 mks больше
максимальная stepb 31
из 4 в 00000000 00000000 00000000 00010000
*/
uint32_t Menu::plb2l(uint8_t stepb)
{
	uint32_t tmp = 0;
	//stepb = stepb&31;
	tmp = 1UL << stepb;
	return tmp;
}
/*
возведение в степень числа 2
сдвиг в пределах 8 байт
максимальная stepb 63
из 4 в 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00010000
*/
uint64_t Menu::plb2ll(uint8_t stepb)
{
	uint64_t tmp = 0;
	//stepb = stepb&63;
	tmp = 1ULL << stepb;
	return tmp;
}

/*
установка канала для абсолютного энкодера
*/
void Menu::set_ch_to_ABS(int ch)
{
	//установить абсолютную координату
	//сделать не возможным поиск реф метки
	//сделать не возможным расстояние от рефметки до рефметки
	//установить разрядность датчика в альтере
	ref_.pRef[ch] = 1;
	init.ref_mode[ch] = 1;
	abs_ENC[ch] = 1;
	if(flash_prg.axis_cfg2[ch].bit_enc > 0 && flash_prg.axis_cfg2[ch].bit_enc < 33)
		altera.set_ch_abs_bit(ch,flash_prg.axis_cfg2[ch].bit_enc);

}
/*
функция для оброботки ошибок преобразователей перемещения
*/
void Menu::control_encoders()
{
	display.encoder_fault_[0] = init.input_D4_inc_f0();
	display.encoder_fault_[1] = init.input_B1_inc_f1();
	display.encoder_fault_[2] = init.input_A3_inc_f2();
	display.encoder_fault_[3] = init.input_A2_inc_f3();  	
	
	#if (configUSE_Alt_ENCerr  == 1)
	for(int c = 0; c  < N_AXIS;c++){
		display.encoder_fault_[c] = MsgKey.er & plb2(c);
	} 
	#endif 

	#if (configUSE_ABS_Encoder == 1)
	for(int c = 0 ; c < N_AXIS;c++){
		if(abs_ENC[c]){
			long abs_err = Lir.plb2ll(flash_prg.axis_cfg2[c].bit_enc) - 1;
			if(altera.counter[c] == abs_err) // 16777215 - означает ошибка для 24 Bit
				display.encoder_fault_[c] = true;
			else
				display.encoder_fault_[c] = false;
		}
	}
	#endif
	
	for(int c = 0 ; c < N_AXIS;c++){
		if(!(Lir.sound_and_other&0x04))
			display.encoder_fault[c] |= display.encoder_fault_[c]; //Захват ошибки датчика
		else
			display.encoder_fault[c] = display.encoder_fault_[c];
	}

	#if (configUSE_NCshell == 1)
		//ошибка срабатывает при параметре контроле датчика
		for(int c = 0; c < N_AXIS;c++){
			if(ch_use(c) && flash_prg.is_encoder_fault(c)){
				if(is_frame_move_prog())
					move_axis_STOP();
				else
					move_axis_STOP(plb2(c));
			}
		}
	#endif
}



/*
функция возвращает максимальное значение для циклической оси (без учета коэффициента)
*/
long Menu::get_max_circle(int ch)
{
	switch(flash_prg.axis_cfg[ch].format){
		case CPrg::FORMAT::CIRCLE_3:
		return 360000L;
		break;
		case CPrg::FORMAT::CIRCLE_2:
		return 36000L;
		break;
		case CPrg::FORMAT::CIRCLE_1:
		return 3600L;
		break;
		case CPrg::FORMAT::CIRCLE_4:
		return 3600000L;
		break;
		case CPrg::FORMAT::CIRCLE_SECUNDO:
		return 1296000L;
		break;
		case CPrg::FORMAT::CIRCLE_SECUNDO_1:
		return 12960000L;
		break;
		case CPrg::FORMAT::CIRCLE_MINUTO:
		return 21600L;
		break;
		default:
		return 1;
	}
}
/*
return true - если выбрана хоть одна ось с аналоговым или импульсным управлением
*/
bool Menu::is_Analog()
{
	#if (configUSE_NCshell == 1)
	for(int c = 0 ; c < N_AXIS + N_SA; c++){
		if(P2[c] != no_axis && 
		(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG || flash_prg._OUTAxis[c].typeZ == CPrg::IMPULSE))
			return  true;
		}
	#endif
	return false;
}
/*
обновление скорости на экране
*/
void Menu::upd_F_display()
{
	#if (configUSE_PLC == 1)
	if(is_Analog()){
		if(MsgKey.mode != TMsgKey::FIND_REF)
			Lir.UPD_show_Instr = 1;
	}
	#endif
}

void Menu::beep()
{
	//if(Lir.beep_cnt > 0)
	//Lir.beep_cnt--;
	//if(Lir.beep_cnt == 1){
	//	off_sound_max7370();
	//	Lir.beep_cnt = 0;
	//}
}

void Menu::beep_on()
{
	if(!(Lir.sound_and_other & 0x01)){
	//	on_sound_max7370();
	//	Lir.beep_cnt = 6;
		peak_sound_max7370(50);
		}
}
/*
преобразование счетчика для индикации
bool ref - 1 - работаем с абсолютным счетчиком  ,0 - с относительным
long cnt - счетчик
uint8_t c - канал
*/
long Menu::conv_c_i(long cnt,uint8_t c, bool ref)
{
	#if (configUSE_NCshell == 1)
		if(flash_prg._OUTAxis[c].typeZ == CPrg::IMPULSE){
			#if (configUSE_mcx514 == 1)
			cnt = mul_LP(c,cnt);
			#endif
		}
		else
			coeff.mult(c,&cnt);
	#else
	coeff.mult(c,&cnt);
	#endif
	
	if(is_corr(c)){	// ЕСТЬ КОРРЕКЦИЯ
		if(ref){
			if(flash_prg.axis_cfg[c].is_coded_ref == CPrg::YESM){
				//смещение код меток и G53 применяются перед таблицей (смещение таблицы для преобразователей с КМ)
				if(MsgKey.mode == TMsgKey::HAND || MsgKey.mode == TMsgKey::INPUT_PARAM){
					if(MsgKey.offset_number > 0)
						cnt -= display.offset_g5x[MsgKey.offset_number - 1][c];
				}		
			} else
				  set_offset(cnt,c);
		} else
			  cnt -= R_off[c];
	} else { // БЕЗ КОРРЕКЦИИ
		if(ref){
			if(flash_prg.axis_cfg[c].is_coded_ref == CPrg::YESM)
				cnt += Lir.size_2r2[c];	
			set_offset(cnt,c);
		}
		else if(is_rel_coded_ref(c)){ //ОТНОСИТЕЛЬНАЯ С КМ
			cnt	+= size_2r2[c]; //смещение кодированных меток
			cnt -= display.offset_g53[c]; // смещение G53
			cnt -= R_off[c];
			}
		}

	l_d(cnt,c,on_display);
	
	if(flash_prg.axis_cfg[c].format == CPrg::FORMAT::LINEAR_INCH_4)
		coeff.div(255,&cnt,1,254,1);// делим
	
	if(flash_prg.axis_cfg[c].type == CPrg::CIRCLE){
		flash_prg.endless_circle(&cnt,c);
	}

	#if (configUSE_NCshell == 1)
	Instr_corr(cnt,c);
	#endif

	return cnt;
}
/*
установка смещений
*/
void Menu::set_offset(long &cnt,uint8_t c)
{
	if(MsgKey.mode == TMsgKey::HAND || MsgKey.mode == TMsgKey::INPUT_PARAM){
		if(MsgKey.offset_number == 0)
			cnt -= display.offset_g53[c]; // смещение G53
		else
			cnt = cnt - display.offset_g53[c] - display.offset_g5x[MsgKey.offset_number - 1][c];
	}
	else
		cnt -= display.offset_g53[c]; // смещение G53
}

/*
преобразование счетчика для коррекции
смещение для таблицы появилось с кодированными линейками т. к. у них нет нуля
*/
long Menu::conv_c_i_c(long counter,uint8_t ch, bool ref)
{
	if(ref){ // смещение таблицы
		coeff.mult(ch,&counter);
		if(flash_prg.axis_cfg[ch].is_coded_ref == CPrg::YESM){
			counter	+= Lir.size_2r2[ch]; //смещение кодированных меток
			counter -= display.offset_g53[ch]; // смещение только G53
		}

		flash_prg.endless_circle(&counter,ch);

		coeff.div(ch,&counter);
	}

	return counter;
}


long Menu::get_spd(uint8_t c)
{
	cli();
	long speed_0 = display.speed_100[c];
	sei();
	#if (configUSE_Cycle1ms == 1)
	speed_0 *= 10;
	#endif

	if(screen.interval_for_speed[c] == 30)
		speed_0 *= 2;
	else if(screen.interval_for_speed[c] == 10)
		speed_0 *= 6;

	if(flash_prg.axis_cfg[c].type == CPrg::LINEAR){
		switch(flash_prg.axis_cfg[c].format){
			case CPrg::FORMAT::LINEAR_3:break;
			case CPrg::FORMAT::LINEAR_2:speed_0 *= 10;break;
			case CPrg::FORMAT::LINEAR_1:speed_0 *= 100;break;
			case CPrg::FORMAT::LINEAR_4:break;
			case CPrg::FORMAT::LINEAR_INCH_4:break;
		}
	}
	else if (flash_prg.axis_cfg[c].type == CPrg::CIRCLE){
		switch (flash_prg.axis_cfg[c].format){
			case CPrg::FORMAT::CIRCLE_3:speed_0 = (speed_0*10)/36;break;
			case CPrg::FORMAT::CIRCLE_2:speed_0 = (speed_0*10)/36;break;
			case CPrg::FORMAT::CIRCLE_1:speed_0 = (speed_0*10)/36;break;
			case CPrg::FORMAT::CIRCLE_4:speed_0 = (speed_0*10)/36;break;
			case CPrg::FORMAT::CIRCLE_SECUNDO:speed_0 = (speed_0*100)/1296;break;
			case CPrg::FORMAT::CIRCLE_MINUTO:speed_0 = (speed_0*100)/216;break;
		}
	}
	#if (configUSE_mcx514 == 1)
	if(flash_prg._OUTAxis[c].typeZ == CPrg::IMPULSE)
		speed_0 = mul_LP(c,speed_0);
	else
		coeff.mult(c,&speed_0);
	#else
		coeff.mult(c,&speed_0);
	#endif

	return speed_0;
}

/*
инициализация формата для отображения параметров
*/
int Menu::iForm(uint8_t c)
{
	if(c >= N_AXIS)
		return CPrg::LINEAR_4;
	if(flash_prg.axis_cfg[c].type == CPrg::LINEAR){
		return CPrg::LINEAR_4;
	}else{
		if(flash_prg.axis_cfg[c].format == CPrg::CIRCLE_1 ||
		   flash_prg.axis_cfg[c].format == CPrg::CIRCLE_2 ||
		   flash_prg.axis_cfg[c].format == CPrg::CIRCLE_3 ||
		   flash_prg.axis_cfg[c].format == CPrg::CIRCLE_4){
				return CPrg::CIRCLE_4;
		   }else if (flash_prg.axis_cfg[c].format == CPrg::CIRCLE_SECUNDO ||
					 flash_prg.axis_cfg[c].format == CPrg::CIRCLE_MINUTO){
				return CPrg::CIRCLE_SECUNDO;
		   }else{
				return flash_prg.axis_cfg[c].format;
		   }			
		}
}
/*
преобразование формата параметра для использования со счетчиком
*/
long Menu::cForm(long tmp,uint8_t c)
{
	if(c >= N_AXIS)
		return tmp;
	if(flash_prg.axis_cfg[c].format == CPrg::LINEAR_1 || flash_prg.axis_cfg[c].format == CPrg::CIRCLE_1){
			return tmp/1000;
	}else if(flash_prg.axis_cfg[c].format == CPrg::LINEAR_2 || flash_prg.axis_cfg[c].format == CPrg::CIRCLE_2){
			return tmp/100;
	}else if(flash_prg.axis_cfg[c].format == CPrg::LINEAR_3 || flash_prg.axis_cfg[c].format == CPrg::CIRCLE_3){
			return tmp/10;
	}else if(flash_prg.axis_cfg[c].format == CPrg::LINEAR_4 || flash_prg.axis_cfg[c].format == CPrg::CIRCLE_4){
			return tmp;
	}else if(flash_prg.axis_cfg[c].format == CPrg::CIRCLE_MINUTO){
			return tmp/60;
	}else{
		return tmp;
	}
}


uint8_t Menu::hex2bcd(uint8_t x)
{
	uint8_t y;
	y = (x / 10) << 4;
	y = y | (x % 10);
	return y;
}

long Menu::hex2bcd_l(long in) {
	long b;
	long a_ = in;
	uint8_t* b_ = (uint8_t*)&b;

	if (in < 0){
		a_ = -a_;
		a_ -= 1;
		}

	for (int i = 0; i < 4; i++) {
		b_[i] = in < 0 ? hex2bcd(99 - (a_ % 100)) : hex2bcd(a_ % 100);
		a_ /= 100;
	}
	return b;
}

/*
return
true -  на каком то канале имеется ошибка
false - ошибок нет
*/
bool Menu::Encs_fault()
{
	for(int c = 0; c < N_AXIS; c++){
		if(ch_use(c) && flash_prg.is_encoder_fault(c))
			return true;
	}
	return false;	
}
/*
uint8_t c - канал
return true  - если есть метка на канале , включен параметр "Кодированные метки" и режим отображения Относительный
*/
bool Menu::is_rel_coded_ref(uint8_t c)
{
	#if (configUSE_NCshell == 1)
	return false;// нужно тестировать
	#endif
	if(init.mRef_ext[c] == 0 && ref_.pRef[c] && flash_prg.axis_cfg[c].is_coded_ref == CPrg::YESM)
		return true;
	else
		return false;
}

void Menu::init_hand(const unsigned char& page)
{
	l_k_s.ON_LED(_led_MODE_HAND);
	l_k_s.OFF_LED(_led_MODE_DIAGNOSTICS);
	l_k_s.OFF_LED(_led_REF);

	for(int s = 0 ; s < N_STR ;s++){
		if(strfunc_hand == s)// по быстрому переходу не затираем выделяемую ось
			continue;
		if(Str[s] == no_axis)
			continue;
		display.put_axis_shot(0,s * 7,Str[s]);
		}
	#if (configUSE_NCshell == 1)	
	 if(MsgKey.tracer_real == TMsgKey::DRIVE_CONTROL && _resz_prg_v.SZ == 0)
		display.clear_rectangle(X_BUTTON_PLACE,Y_FOUR,48,5,0x00,0);
		
		if(block_str[3])
			block_str[3] = false; // разблокируем

		if(MsgKey.tracer_real == TMsgKey::DRIVE_CONTROL)
			show_cmd(page, 0, 1);

		if(extPULT.on)
			outP_G0(extPULT.G0,1);
		else
			outP_G0(_modals_cmd.G0,1);

		//Lir.init_hand_CMD_CTRL(0);  // блокируем
		cms.on = false;
	#endif
	screen.to_clear_place_for_virtual_buttons(4); //display.clear_ram_null();
	MsgKey.to_put_icons_in_mode_HAND(MsgKey.tracer_real);//show_func_blank_hand(page);
	
	entry_hand = IDLE;
	#if (configUSE_NCshell == 1)
	if(init_hand_CMD_CTRL(0) == 0){// перемещена сюда из за entry_hand = IDLE;
			if(HM_toF && MsgKey.tracer_real == TMsgKey::DRIVE_CONTROL){//переходим в ручное перемещение оси Lir.strfunc_hand
				MsgKey.key = convStoKey(strfunc_hand);
				dir_mh = dir_mh_sv;
				if(dir_mh == DIR_MH_RIGHT){
					l_k_s.ON_LED(_led_RIGHT);
					l_k_s.OFF_LED(_led_LEFT);
				} else if(dir_mh == DIR_MH_LEFT){
					l_k_s.OFF_LED(_led_RIGHT);
					l_k_s.ON_LED(_led_LEFT);
				}
				set_mh();			
			}
		};  
	#endif
}

int Menu::init_hand_CMD_CTRL(const unsigned char& page)
{
	#if (configUSE_NCshell == 1)
	if(MsgKey.tracer_next != TMsgKey::CMD_CNTRL)  //если входим в режим CMD_CNTRL то начинаем инициализацию CMD_CNTRL
		return 0;
	if(!Lir.init_resz_prg_v()) // если внизу нет места
		Lir.block_str[3] = true;// блокируем вывод 4ой строки
		
	show_Instr(page,1);	
		
	for(int i = 0 ; i < 3 ; i++){
		display.print_symbols_local(' ',30,page,0, 16 * (11 + i));
	}	
	display.print_symbols_local(' ',30,page,0, (16 * 10 ) + 10);//стираем место для 4ой оси
	display.print_symbols_local(' ',3,page,30, (16 * 10 ));//очищаем место над уголком
	
	func_cmd = 0;
	screen.update_icons_hand_mode();
	//==переход в режим ввода сразу после исполнения кадра
	if(if_input_used_cmd == 0){ //если не было ввода кадра
		if(Lir.stop_in_move == 0){ //если не было нажатия кнопки стоп во время перемещения
			if(get_Erase_Prednabor()){ //если включен параметр "Обновление преднабора"
				if(error_on_hand == 0){ //если не было ошибок при пуске кадра
					if(alert == NO_ALERT){ //если нет ошибок
						if(!ch_sw.plus && !ch_sw.minus && !ch_sw_prog.minus && !ch_sw_prog.plus) {//если нет ошибки ограничений
							if(!Encs_fault()){ // если нет ошибок датчика
								memset(frame_cmd[0],'\0',FRAME_SZ);//очистка буфера ввода
								MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);//== сразу переходим в режим ввода
								Lir.menu_BLANK2_hand();
							}
						}						
					}
				}
			}
		}
		else
			Lir.stop_in_move = 0;
	}
	else
		if_input_used_cmd = 0;
	//==	
	int cnt = display.strnlen_lir(frame_cmd[MAX_FRAME - 1] + 4);
	if (cnt == 0){
		status(PSTR("Преднабор"), 0, page);
	}
	else {
		if (cnt + 4 <= STR_FRAME_SZ_DISPLAY)
			out_1nd_frame(frame_cmd[MAX_FRAME - 1] + 4, page, 4, 16 * 12, 16 * 12, 16 * 14);
		else
			out_2nd_frame(frame_cmd[MAX_FRAME - 1] + 4, page, 4, 16 * 12, 16 * 12, 16 * 14);
		status(PSTR("Нажмите СТАРТ"), 0, page,1);//status(PSTR("Преднабор"),0,page,1);
	}
	#endif
	return 1;
}

void Menu::exit_hand_CMD_CTRL(const unsigned char& page)
{	
	if(MsgKey.tracer_next != TMsgKey::CMD_CNTRL)
		return;
	#if (configUSE_NCshell == 1)
	if(Lir.block_str[3])
		Lir.block_str[3] = false;
	
	if(Str[N_AXIS - 1] != no_axis)
		display.put_char_local_independent(' ',page,4,16 * 11);//очистка символа рядом со знаком оси
	//==Очистка ShowInstr
	for(int i = 0 ; i < 4 ; i++){
		display.print_symbols_local(' ',10,page,30, 16 * (11 + i));
	}
	//==
	//==Очистка поля команды + очистка 4ой  уменьшенной оси
	display.print_symbols_local(' ',30,page,0, (16 * 10 ) + 10);
	for(int i = 0 ; i < 3 ; i++){
		display.print_symbols_local(' ',30,page,0, 16 * (11 + i));
	}
	//==
	
	memset(frame_cmd[0],'\0',FRAME_SZ);//очистка буфера ввода
	Lir.show_cmd(0,0,1);
	Lir.status(PSTR(""));
	rst_VIBOR_axis(0,1);
	MsgKey.tracer_next = TMsgKey::DRIVE_CONTROL;
	screen.update_icons_hand_mode();
	#endif
}

void Menu::actions_hand(const unsigned char& page)
{	
	#if (configUSE_NCshell == 1)
	if(INmovePrednabor() && upd_m_icons){
		put_moving_icons(page);	 //0.748 ms 
		upd_m_icons = false;
	}
	#endif
	
	if (entry_hand == INIT)
		init_hand(page);
	if (entry_hand == EXECUTE_EXE){
		execute_hand();
		#if (configUSE_NCshell == 1)
		first_frame = false;
		#endif
	}
	#if (configUSE_NCshell == 1)
	if (entry_hand == DELAY_M5M3M4) {
		if (cnt_M3M4 || cnt_M5)
			return;
		Lir.entry_hand = INIT;
			return;
	}	
	if(entry_hand == DELAY_WAIT_SHPINDEL){
		if(Lir.cnt_wait_shpindel || cnt_wait_shpindel_off){
			if(Lir.cnt_wait_shpindel)
				Lir.status(PSTR("Разгон шпинделя"));
			return;
			}		
		//new
		if(cnt_M3M4 || cnt_M5){
			entry_exe = DELAY_M5M3M4;
			return;
			}
		//
		if(Lir.alert == Menu::SHPINDEL_NE_VRASHAETSIA || Lir.alert == Menu::SHPINDEL_NE_OSTANOVLEN){
			Lir.entry_hand = INIT;
			return;
			}
		else {
			Lir.entry_hand = INIT;
			return;
		}
	}
	#endif
	if (entry_hand == INIT_WORK_EXE)
		initWork_hand(page);
	#if (configUSE_NCshell == 1)
	if (entry_hand == WORKING_EXE){		
		if(!ch_sw.plus && !ch_sw.minus && !ch_sw_prog.minus && !ch_sw_prog.plus)
			status(PSTR("В работе"));
	}
	#endif
}

void Menu::execute_hand()
{
	#if (configUSE_NCshell == 1)
	long ch_impulse[N_AXIS] = { 0 };
	//long channel_f[N_AXIS]= {0};
	long F = 0;
	char G = 0;
	char M = 0;
	char FR[FRAME_SZ];
	uint8_t chs; // выбранные каналы

	display.strncpy_lir(FR,frame_cmd[MAX_FRAME - 1] + 4,FRAME_SZ);
	int cnt = display.strnlen_lir(FR,FRAME_SZ);
	if(!cnt){
		entry_hand = INIT;
		return;
	}
	chs = get_bin_from_cmd(ch_impulse, FR, FRAME_SZ);
	//==
	get_T_from_cmd(FR, FRAME_SZ);
	get_F_from_cmd(&F, FR, FRAME_SZ);
	get_G_from_cmd(&G, FR, FRAME_SZ);
	get_M_from_cmd(&M,FR, FRAME_SZ);
	if (M == 3 || M == 4 || M == 5){
	    char res = M3M5M4_cmd(M,entry_hand);
		if (res == 1 || res == 2){ //команды M3 M4 M5 только одни в строке
			if(res == 2)
				error_on_hand = 1;
			else
				error_on_hand = 0;
			return;
		}
		else if(res == 0)
			error_on_hand = 1;
	}

	if(!lirCommand(ch_impulse,chs,G,F)){
		entry_hand = INIT;
		error_on_hand = 1;
		return;
	}							

	error_on_hand = 0;
	entry_hand = INIT_WORK_EXE;
	#endif
}

void Menu::initWork_hand(const unsigned char & page)
{
	#if (configUSE_NCshell == 1)
	status(PSTR("В работе"));
	show_Instr(page,1);
	put_moving_icons(page);
	entry_hand = WORKING_EXE; // начинает работать код в прерывании
	entry_exe = INIT;
	#endif
}

void Menu::blank_right_input_hand(const char& BLANK)
{
	if (curItem->id == idHand) {
		switch (BLANK)
		{
			case _key_BLANK1:
			
			break;
			case _key_BLANK2:
			
			break;
			case _key_BLANK3:
			menu_BLANK3_hand();			
			break;
			case _key_BLANK4:
			
			break;
			default:
			return;
		}
	}
}

void Menu::menu_enter_hand()
{
	//if (curItem->id == idHand) {
		entry_hand = INIT;
		SET_C(idExecmd);
#if (configUSE_NCshell == 1)
		mPR.on = false;
#endif
	//}
}

void Menu::menu_D_hand()
{
	//if (curItem->id == idHand) {
		entry_hand = INIT;
		SET_C(idDiag);
	//}
}

void Menu::menu_R_hand()
{
	//if (curItem->id == idHand) {
		entry_hand = INIT;
		SET_C(idRef);
		#if (configUSE_NCshell == 1)
		show_cmd(0,0,0);
		#endif
	//}
}
void Menu::menu_BLANK2_hand()
{
	entry_hand = INIT;
	SET_C(idCmd);
}

void Menu::menu_BLANK3_hand()
{
	entry_hand = INIT;
	SET_C(idPrgOrSubPrg);
}


void Menu::menu_START_hand()
{
	//if (curItem->id == idHand)
	#if (configUSE_NCshell == 1)
	if(extPULT.on)
		return;
	cms.cur_fync = 0;
	entry_hand = EXECUTE_EXE;
	first_frame = true;
	#endif
}

void Menu::invert_axis_in_modeFUNC(char key)
{
	for(int s = 0; s < N_STR;s++){
		if(Lir.Str[s] == no_axis)
			continue;
		if(display.position_to_see[s] != CDisp::COORDINATE)
			continue;
		if(key == _key_BLANK1_L && s == 0){
			Lir.strfunc_hand = s;
			INVERT_CHAR()
		}
		else if(key == _key_BLANK2_L && s == 1){
			Lir.strfunc_hand = s;
			INVERT_CHAR()
		}
		else if(key == _key_BLANK3_L && s == 2){
			Lir.strfunc_hand = s;
			INVERT_CHAR()
		}
		else if(key == _key_BLANK4_L && s == 3){
			Lir.strfunc_hand = s;
			INVERT_CHAR()
		}

		display.put_axis_shot(0,s * 7,Lir.Str[s]);
		NORMAL_CHAR();
	}
	NORMAL_CHAR();
}
/*
преобразование номера строки в нажатую кнопку
*/
char Menu::convStoKey(int s)
{
	if(s == 0){
		return _key_BLANK1_L;
	}
	else if(s == 1){
		return _key_BLANK2_L;
	}
	else if(s == 2){
		return _key_BLANK3_L;
	}
	else if(s == 3){
		return _key_BLANK4_L;
	}
	else 
		return EMPTY_KEY;
}

/*
проверка есть ли на кнопке ось
*/
bool Menu::is_str_mh(char key)
{
	if(key == _key_BLANK1_L && Lir.Str[0] != no_axis)
		return true;
	else if(key == _key_BLANK2_L && Lir.Str[1] != no_axis)
		return true;
	else if(key == _key_BLANK3_L && Lir.Str[2] != no_axis)
		return true;
	else if(key == _key_BLANK4_L && Lir.Str[3] != no_axis)
		return true;
	else
		return false;
}
/*
return true - вывод больших осей на экране и вывод букв осей
*/
bool Menu::is_BIG_Out_Axis()
{
	if(MsgKey.mode != TMsgKey::MODE::INPUT_PARAM || 
	curItem->id == idInpointRadiusPUSK ||
	curItem->id == idInpointLinearPUSK || 
	curItem->id == idCmd || 
	curItem->id == idHandMove|| 
	curItem->id == idExecmd || 
	curItem->id == idHand_input)
		return 1;
	else
		return 0;
}

void Menu::actions_input_HNDI()
{
	hand_input();
}

void Menu::hand_input()
{
	flashing_cursor(5,208);
	if(ResultForInputNum == input::ok)
	{		
		if(checkInputStr((char*)bufASCIIstr) == 0){ //если нет ошибок
			memset(display.go_to_blink_string,0,N_STR);
			get_bin_from_str_for_hand_input(MsgKey.what_channel_is_chosen_for_input);			
		}
		else //если есть ошибки
			return; 			 

		MsgKey.SET_M(TMsgKey::MODE::BEGIN);
		SET_C(idBegin);
		init_cursor_pos();
		display.print_symbols_local(' ',20,0,5,208); //очистка
		memset(display.go_to_blink_string,0,N_STR);
	}

	if(ResultForInputNum == input::cancel_)
	{
		MsgKey.SET_M(TMsgKey::MODE::BEGIN);
		SET_C(idBegin);
		init_cursor_pos();
		display.print_symbols_local(' ',20,0,5,208); //очистка
		memset(display.go_to_blink_string,0,N_STR);	
	}
}

void Menu::get_bin_from_str_for_hand_input(const unsigned char& ch)
{
	long bin_counter = String_to_bin((uint8_t*)bufASCIIstr,ch);
	if(bin_counter > MAX_LONG_PLUS)
		bin_counter = MAX_LONG_PLUS;
	if(bin_counter < MAX_LONG_MINES)
		bin_counter = MAX_LONG_MINES;
	set_counter_rel(ch,bin_counter);
}

void Menu::set_counter_rel(const unsigned char& c,long &in)
{
	if(is_corr(c) || is_rel_coded_ref(c)){
		cli();
		long cnt = screen.channel_counter[c];
		sei();
		cnt = conv_c_i(cnt,c,0);
		R_off[c] += cnt - in;
		cli();
		//0.240ms == 240 mks
		l_k_s.write_to_realtime((unsigned char *)&R_off[c],80 + (c * sizeof(long)),sizeof(long));//80 == 0x50		
		sei();
	}else{
		coeff.div(c,&in);
		cli();
		mCounter[c].rel = in;
		sei();
	}
}

#if (configUSE_NCshell == 1)

void Menu::actions_DOUT(const unsigned char& page)
{
	if (entry_DOUT == INIT)
		init_menu_DOUT(page);
	if (entry_DOUT == UPDATE)
		draw_menu_DOUT(page);
	if (entry_DOUT == IDLE){		

	}
	return;
}

void Menu::actions_DOUT_input(const unsigned char& page)
{	
	
	/*int x_, y_;
	int8_t cnt = 0;

	if(menu_cnt_up_down_DOUT == 3)
		cnt = 11;
	else if(menu_cnt_up_down_DOUT == 4)
		cnt = 12;
	
	x_ = 23;
	y_ = cnt * 16;

	flashing_cursor(x_, y_, page);
	if (menu.ResultForInputNum == input::ok) {
		long tmp = 0;
		if (!display.strnlen_lir((const char*)bufASCIIstr))
			goto skip_pr;
		ConvertASCIIlinear_circle_tolong(tmp,CPrg::LINEAR_3,bufASCIIstr);
		if(menu_cnt_up_down_DOUT == 3 || menu_cnt_up_down_DOUT == 4){
			if(tmp > 10000L)
				tmp = 10000;
			else if(tmp < -10000L)
				tmp = -10000;
			U_out[menu_cnt_up_down_DOUT - 3] = (int16_t)tmp; //вычислить указатель
			cli();
			uart_485.send_U(U_out);
			sei();
		}
		skip_pr:
		display.print_flash_local(probel10, page, x_, y_);
		init_cursor_pos();
		SET_C(idViewOutputs);
		entry_DOUT = UPDATE;//exit_G1_G0_input_pr(page,x_,y_);		
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(probel10, page, x_, y_);
		init_cursor_pos();
		SET_C(idViewOutputs);
		entry_DOUT = UPDATE;//exit_G1_G0_input_pr(page,x_,y_);
	}
	
	if (entry_DOUT == UPDATE)
		draw_menu_DOUT(page);  */
}

void Menu::init_menu_DOUT(const unsigned char& page)
{
	display.clear_ram_null();
	INVERT_CHAR()
	
	#if (configUSE_PLC == 1)
		display.print_flash_local(PSTR(" ТЕСТ ВЫХОДОВ/МАРКЕРОВ"), 0, 0, 0);
		NORMAL_CHAR()
		display.print_flash_local(PSTR("- номер"), 0, 26, 16 * 8);
		display.print_flash_local(PSTR("<ВВОД> - вкл/выкл"), 0, 16, 16 * 9);
		display.print_flash_local(PSTR(" Ошибки связи:"), 0, 0, 16 * 13);
	#else
		display.print_flash_local(PSTR(" ТЕСТ ВЫХОДОВ "), 0, 0, 0);
		display.print_flash_local(PSTR(" P1 "), 0, 0, 32);
		NORMAL_CHAR()
		display.print_flash_local(PSTR("- номер выхода"), 0, 26, 16 * 3);
		display.print_flash_local(PSTR("<ВВОД> - вкл/выкл выход"), 0, 16, 16 * 4);
	#endif
	
	
	entry_DOUT = UPDATE;
}

void Menu::draw_menu_DOUT(const unsigned char& page)
{
	#if (configUSE_PLC == 1)
		const char *menu[6] ;

		menu[0] = PSTR("    >P1");
		menu[1] = PSTR(" m0-m31");
		menu[2] = PSTR("m80-m95");
		//menu[3] = PSTR(" Напряжение а1:");
		//menu[4] = PSTR(" Напряжение а2:");
		menu[3] = nullptr;

		for (uint8_t str = 0; menu[str] != nullptr; str++) {
			if (str == menu_cnt_up_down_DOUT)
				INVERT_CHAR()
			else
				NORMAL_CHAR()
			int offset;
			if(str >= 0 && str <= 2)
				offset = 32 + ((16 * str) * 2);
			else
				offset = 176 + (16 * (str - 3));
			
			display.print_flash_local(menu[str], page, 0,offset);
		}
		NORMAL_CHAR()

	//маркеры m0-m31
		for(int i = 0 ; i < 32;i++){
			if (menu_cnt_DOUT == i && menu_cnt_up_down_DOUT == 1)
				INVERT_CHAR()
			else
				NORMAL_CHAR()				
			print_bit(m_out,i,8,32 + ((16 * 1) * 2));
			NORMAL_CHAR()
		}
	//
	//маркеры m80-m95
		for(int i = 0 ; i < 16;i++){  
			if (menu_cnt_DOUT == i && menu_cnt_up_down_DOUT == 2)
			INVERT_CHAR()
			else
			NORMAL_CHAR()			
			print_bit(mem_m80_m95,i,8,32 + ((16 * 2) * 2));
			NORMAL_CHAR()
		}
	//
	
	//Ошибки связи
	long t_ = uart_485.erros_cnt_disp;
	if(uart_485.crc_errors_Tx + uart_485.crc_errors_Rx)
		t_ += uart_485.crc_errors_Tx + uart_485.crc_errors_Rx;

	print_long_format(t_, CPrg::FORMAT_END, 0, 15, 16 * 13, LEFT_ALIGN,0,11);
	//
	#endif
	
	for (int i = 0; i < 24;i++) {
		if (menu_cnt_DOUT == i && !menu_cnt_up_down_DOUT)
			INVERT_CHAR()
		else
			NORMAL_CHAR()
		char offset = 0;
		if(i >= 16)
			offset = 1;
		print_bit(p_out,i,8 + offset,16 * 2);
		NORMAL_CHAR()
	}
	#if (configUSE_PLC == 1)
		int tmp = menu_cnt_DOUT;
		char tmp_c = 'P';
		if(menu_cnt_up_down_DOUT == 2)
			 tmp = menu_cnt_DOUT + 79;
		else if(menu_cnt_up_down_DOUT == 1)
			 tmp--;
		if (menu_cnt_up_down_DOUT > 0)
			tmp_c = 'm';
		if(menu_cnt_up_down_DOUT == 0 || menu_cnt_up_down_DOUT == 1 || menu_cnt_up_down_DOUT == 2){
			print_long_format(tmp + 1, CPrg::FORMAT_END, 0, 24, 16 * 8, RIGHT_ALIGN, 1);
			display.put_char_local_independent(tmp_c,0,22,16 * 8);
		}
		else
			display.print_symbols_local(' ',3,0,22,16 * 8);
	#else
		print_long_format(menu_cnt_DOUT + 1, CPrg::FORMAT_END, 0, 24, 16 * 3, RIGHT_ALIGN, 1);
	#endif
	entry_DOUT = IDLE;
}

void Menu::menu_up_DOUT()
{
#if (configUSE_PLC == 0)
return;
#endif
	if (curItem->id == idViewOutputs) {	
		if(menu_cnt_up_down_DOUT != 0 ){
			menu_cnt_up_down_DOUT--;
			if(menu_cnt_up_down_DOUT == 0 && menu_cnt_DOUT > 23)
				menu_cnt_DOUT = 23;
			entry_DOUT = UPDATE;
			}
	}
}

void Menu::menu_down_DOUT()
{
#if (configUSE_PLC == 0)
return;
#endif
	if (curItem->id == idViewOutputs) {
		if(menu_cnt_up_down_DOUT < 2){
			menu_cnt_up_down_DOUT++;
			if(menu_cnt_up_down_DOUT == 2 && menu_cnt_DOUT > 15)
				menu_cnt_DOUT = 15;
			entry_DOUT = UPDATE;
			}
	}
}

void Menu::menu_left_DOUT()
{
	if (curItem->id == idViewOutputs) {
		if(menu_cnt_up_down_DOUT == 0 || menu_cnt_up_down_DOUT == 1 || menu_cnt_up_down_DOUT == 2)
			menu_cnt_DOUT--;
		if (menu_cnt_DOUT < 0 && !menu_cnt_up_down_DOUT)
			menu_cnt_DOUT = 23;
		else if(menu_cnt_DOUT < 0 && menu_cnt_up_down_DOUT == 1)
			menu_cnt_DOUT = 31;
		else if(menu_cnt_DOUT < 0 && menu_cnt_up_down_DOUT == 2)
			menu_cnt_DOUT = 15;
		entry_DOUT = UPDATE;
	}
}

void Menu::menu_right_DOUT()
{
	if (curItem->id == idViewOutputs) {
		if(menu_cnt_up_down_DOUT == 0 || menu_cnt_up_down_DOUT == 1 || menu_cnt_up_down_DOUT == 2)
			menu_cnt_DOUT++;
		if (menu_cnt_DOUT > 23 && !menu_cnt_up_down_DOUT)
			menu_cnt_DOUT = 0;
		else if(menu_cnt_DOUT > 31 && menu_cnt_up_down_DOUT == 1)
			menu_cnt_DOUT = 0;
		else if(menu_cnt_DOUT > 15 && menu_cnt_up_down_DOUT == 2)
			menu_cnt_DOUT = 0;
		entry_DOUT = UPDATE;
	}
}

void Menu::menu_enter_DOUT()
{
	if (curItem->id == idViewOutputs) {
		if(menu_cnt_up_down_DOUT == 0){//P1
			p_out ^= plb2l(menu_cnt_DOUT);
			entry_DOUT = UPDATE;
			cli();
			altera.to_send_outputs((unsigned char *)&p_out); //14,5 mks
			sei();
		}
		else if(menu_cnt_up_down_DOUT == 1){//m0-m31
			cli();
			m_out ^= plb2l(menu_cnt_DOUT);
			uart_485.send_m0_m31((unsigned short *)&m_out);			
			sei();
			entry_DOUT = UPDATE;
		}
	}
}

void Menu::menu_stop_DOUT()
{
	if (curItem->id == idViewOutputs) {
		entry_DOUT = INIT;
		SET_C(idDebugMode);
	}
}

void Menu::menu_C_DOUT()
{
	if (curItem->id == idViewOutputs) {
		uart_485.erros_cnt_disp = 0;
		entry_DOUT = UPDATE;
	}
}

void Menu::actions_INGEN(const unsigned char& page)
{
	if (entry_INGEN == INIT)
		init_menu_INGEN(page);
	if (entry_INGEN == UPDATE)
		draw_menu_INGEN(page);
	if (entry_INGEN == IDLE)
	return;
}

void Menu::init_menu_INGEN(const unsigned char& page)
{
	display.clear_ram_null();
	entry_INGEN = UPDATE;
}

void Menu::draw_menu_INGEN(const unsigned char& page)
{
	const char* menu_table[5];
	//==конструктор меню
	menu_table[0] = PSTR(" КОНФИГУРАЦИЯ ВХОДОВ");
	menu_table[1] = PSTR(" Общие функции");
	menu_table[2] = PSTR(" Шпиндель");
	menu_table[3] = PSTR(" М-функции входов");
	menu_table[4] = nullptr;
	//==
	//==размер меню
	menu_INGEN_SZ = 3;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_INGEN, page);
	//==
	entry_INGEN = IDLE;
}

void Menu::menu_up_INGEN()
{
	if (curItem->id == idInGen) {
		menu_cnt_INGEN--;
		if (menu_cnt_INGEN < 1)
		menu_cnt_INGEN = menu_INGEN_SZ;
		entry_INGEN = UPDATE;
	}
}

void Menu::menu_down_INGEN()
{
	if (curItem->id == idInGen) {
		menu_cnt_INGEN++;
		if (menu_cnt_INGEN > menu_INGEN_SZ)
		menu_cnt_INGEN = 1;
		entry_INGEN = UPDATE;
	}
}

void Menu::menu_enter_INGEN()
{
	if (curItem->id == idInGen) {
		if (menu_cnt_INGEN == 1) {
			entry_INGEN = INIT;
			SET_C(idFInGen);
		}
		else if (menu_cnt_INGEN == 2) {
			entry_INGEN = INIT;
			SET_C(idSInGen);
		}
		else if (menu_cnt_INGEN == 3) {
			entry_INGEN = INIT;
			SET_C(idMInGen);
		}
	}
}

void Menu::menu_stop_INGEN()
{
	if (curItem->id == idInGen) {
		entry_INGEN = INIT;		
		SET_C(idAllp);
	}
}

void Menu::actions_FINGE(const unsigned char& page)
{
	if (entry_FINGE == INIT)
	init_menu_FINGE(page);
	if (entry_FINGE == UPDATE) {
		draw_menu_FINGE(page);
		draw_param_FINGE(page, 25, LEFT_ALIGN);
	}
	if (entry_FINGE == IDLE)
	return;
}

void Menu::init_menu_FINGE(const unsigned char& page)
{
	display.clear_ram_null();
	entry_FINGE = UPDATE;
}

void Menu::draw_menu_FINGE(const unsigned char& page)
{
	const char* menu_table[10];
	//==конструктор меню
	menu_table[0] = PSTR(" ОБЩИЕ ФУНКЦИИ ВХОДОВ");
	menu_table[1] = PSTR(" Готовность станка");
	menu_table[2] = PSTR(" Ускоренный ход G0");
	menu_table[3] = PSTR(" Стоп подачи");
	menu_table[4] = PSTR(" Смена кадра");
	menu_table[5] = PSTR(" Пауза");
	menu_table[6] = PSTR(" Старт программы");
	menu_table[7] = sLine;
	menu_table[8] = PSTR(" Внешний пульт");
	menu_table[9] = nullptr;
	//==
	//==размер меню
	menu_FINGE_SZ = 8;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_FINGE, page);
	//==
	entry_FINGE = IDLE;
}

void Menu::draw_param_FINGE(const unsigned char& page, int x, char align)
{
	char* p = &flash_prg._INGen.gotov_stanok;
	for (int i = 0; i < 7; i++) {
		if (i == 6)
			i++;
		flash_prg.show_P(*p, IN_P, x, 16 * (i + 1), align);
		p++;
	}
}


void Menu::menu_up_FINGE()
{
	if (curItem->id == idFInGen) {
		menu_cnt_FINGE--;						
		skip_menu_item(0,menu_cnt_FINGE,7);
		//skip_menu_item(0,menu_cnt_FINGE,6);
		skip_menu_item(0,menu_cnt_FINGE,5);
		//skip_menu_item(0,menu_cnt_FINGE,4);
		if (menu_cnt_FINGE < 1)
		menu_cnt_FINGE = menu_FINGE_SZ;
		entry_FINGE = UPDATE;
	}
}

void Menu::menu_down_FINGE()
{
	if (curItem->id == idFInGen) {
		menu_cnt_FINGE++;
		//skip_menu_item(1,menu_cnt_FINGE,4);
		skip_menu_item(1,menu_cnt_FINGE,5);
		//skip_menu_item(1,menu_cnt_FINGE,6);
		skip_menu_item(1,menu_cnt_FINGE,7);
		if (menu_cnt_FINGE > menu_FINGE_SZ)
		menu_cnt_FINGE = 1;		
		entry_FINGE = UPDATE;
	}
}

void Menu::menu_left_FINGE()
{
	if (curItem->id == idFInGen) {
		if (menu_cnt_FINGE >= 1  && menu_cnt_FINGE <= 6) {
			char* p = &flash_prg._INGen.gotov_stanok;
			p = p + (menu_cnt_FINGE - 1);
			flash_prg.changeP(*p, IN_P, 0);
			entry_FINGE = UPDATE;
		}
		if (menu_cnt_FINGE == 8) {
			flash_prg.changeP(flash_prg._INGen.vneshnee_ypravlenie, IN_P, 0);
			entry_FINGE = UPDATE;
		}
	}
}

void Menu::menu_right_FINGE()
{
	if (curItem->id == idFInGen) {
		if (menu_cnt_FINGE >= 1 && menu_cnt_FINGE <= 6) {
			char* p = &flash_prg._INGen.gotov_stanok;
			p = p + (menu_cnt_FINGE - 1);
			flash_prg.changeP(*p, IN_P, 1);
			entry_FINGE = UPDATE;
		}
		if (menu_cnt_FINGE == 8) {
			flash_prg.changeP(flash_prg._INGen.vneshnee_ypravlenie, IN_P, 1);
			entry_FINGE = UPDATE;
		}
	}
}

void Menu::menu_F_FINGE()
{
	if (curItem->id == idFInGen) {
		if (menu_cnt_FINGE >= 1 && menu_cnt_FINGE <= 6) {
			char* p = &flash_prg._INGen.gotov_stanok;
			p = p + (menu_cnt_FINGE - 1);
			flash_prg.change_MP(*p,IN_P);
			entry_FINGE = UPDATE;
		}
		if (menu_cnt_FINGE == 8) {
			flash_prg.change_MP(flash_prg._INGen.vneshnee_ypravlenie,IN_P);
			entry_FINGE = UPDATE;
		}
	}
}

void Menu::menu_enter_FINGE()
{
	if (curItem->id == idFInGen) {
		if (menu_cnt_FINGE >= 1 && menu_cnt_FINGE <= 6) {
			char* p = &flash_prg._INGen.gotov_stanok;
			p = p + (menu_cnt_FINGE - 1);
			flash_prg.change_inv_P(*p);
			entry_FINGE = UPDATE;
		}
		if (menu_cnt_FINGE == 8) {
			flash_prg.change_inv_P(flash_prg._INGen.vneshnee_ypravlenie);
			entry_FINGE = UPDATE;
		}
	}
}

void Menu::menu_stop_FINGE()
{
	if (curItem->id == idFInGen) {
		entry_FINGE = INIT;
		flash_prg.write_eeprom(700,sizeof(flash_prg._INGen),&flash_prg._INGen);
		SET_C(idInGen);
	}
}

void Menu::actions_SINGE(const unsigned char& page)
{
	if (entry_SINGE == INIT)
	init_menu_SINGE(page);
	if (entry_SINGE == UPDATE) {
		draw_menu_SINGE(page);
		draw_param_SINGE(page, 25, LEFT_ALIGN);
	}
	if (entry_SINGE == IDLE)
	return;
}

void Menu::init_menu_SINGE(const unsigned char& page)
{
	display.clear_ram_null();
	entry_SINGE = UPDATE;
}

void Menu::draw_menu_SINGE(const unsigned char& page)
{
	const char* menu_table[10];
	//==конструктор меню
	menu_table[0] = PSTR(" ВХОДЫ УПРАВЛЕНИЯ ШПИНДЕЛЕМ");
	menu_table[1] = PSTR(" По часовой     М3");
	menu_table[2] = PSTR(" Против часовой М4");
	menu_table[3] = PSTR(" Стоп           М5");
	menu_table[4] = PSTR(" Толчок+");
	menu_table[5] = PSTR(" Толчок-");
	menu_table[6] = sLine;
	menu_table[7] = PSTR(" Шпиндель вращается");
	menu_table[8] = PSTR(" Инструмент зажат");
	menu_table[9] = nullptr;
	//==
	//==размер меню
	menu_SINGE_SZ = 8;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_SINGE, page);
	//==
	entry_SINGE = IDLE;
}

void Menu::draw_param_SINGE(const unsigned char& page, int x, char align)
{
	char* p = &flash_prg._INGen.po_chasovoy_M3;
	for (int i = 0; i < 8; i++) {
		if (i == 5)continue;//line
		flash_prg.show_P(*p, IN_P, x, 16 * (i + 1), align);
		p++;
	}
}

void Menu::menu_up_SINGE()
{
	if (curItem->id == idSInGen) {
		menu_cnt_SINGE--;
		if (menu_cnt_SINGE < 1)
		menu_cnt_SINGE = menu_SINGE_SZ;
		if (menu_cnt_SINGE == 6)
		menu_cnt_SINGE--;
		entry_SINGE = UPDATE;
	}
}

void Menu::menu_down_SINGE()
{
	if (curItem->id == idSInGen) {
		menu_cnt_SINGE++;
		if (menu_cnt_SINGE > menu_SINGE_SZ)
		menu_cnt_SINGE = 1;
		if (menu_cnt_SINGE == 6)
		menu_cnt_SINGE++;
		entry_SINGE = UPDATE;
	}
}

void Menu::menu_left_SINGE()
{
	if (curItem->id == idSInGen) {
		if (menu_cnt_SINGE >= 1 && menu_cnt_SINGE <= 8) {
			char* p = &flash_prg._INGen.po_chasovoy_M3;
			char tmp = menu_cnt_SINGE;
			if (tmp > 6)
			tmp--;
			p = p + (tmp - 1);
			flash_prg.changeP(*p, IN_P, 0);
			entry_SINGE = UPDATE;
		}
	}
}

void Menu::menu_right_SINGE()
{
	if (curItem->id == idSInGen) {
		if (menu_cnt_SINGE >= 1 && menu_cnt_SINGE <= 8) {
			char* p = &flash_prg._INGen.po_chasovoy_M3;
			char tmp = menu_cnt_SINGE;
			if (tmp > 6)
			tmp--;
			p = p + (tmp - 1);
			flash_prg.changeP(*p, IN_P, 1);
			entry_SINGE = UPDATE;
		}
	}
}

void Menu::menu_F_SINGE()
{
	if (curItem->id == idSInGen) {
		if (menu_cnt_SINGE >= 1 && menu_cnt_SINGE <= 8) {
			char* p = &flash_prg._INGen.po_chasovoy_M3;
			char tmp = menu_cnt_SINGE;
			if (tmp > 6)
			tmp--;
			p = p + (tmp - 1);
			flash_prg.change_MP(*p,IN_P);
			entry_SINGE = UPDATE;
		}
	}
}

void Menu::menu_enter_SINGE()
{
	if (curItem->id == idSInGen) {
		if (menu_cnt_SINGE >= 1 && menu_cnt_SINGE <= 8) {
			char* p = &flash_prg._INGen.po_chasovoy_M3;
			char tmp = menu_cnt_SINGE;
			if (tmp > 6)
			tmp--;
			p = p + (tmp - 1);
			flash_prg.change_inv_P(*p);
			entry_SINGE = UPDATE;
		}
	}
}

void Menu::menu_stop_SINGE()
{
	if (curItem->id == idSInGen) {
		entry_SINGE = INIT;
		flash_prg.write_eeprom(700,sizeof(flash_prg._INGen),&flash_prg._INGen);// Входы общие
		/*сброс переменной , так как при установке шпиндель вращается "нет" SHPINDEL == 1,
		 а потом если установить снова вход , то все равно будет SHPINDEL == 1, аналогично с INSTRUMENT_ZAJAT*/
		 if(flash_prg._INGen.shpindel_vrashaetsia & INVERSE_P_bm)
			SHPINDEL = 0; 
		 else
			SHPINDEL = 1; 

		if(flash_prg._INGen.instrum_zajat & INVERSE_P_bm)
			INSTRUMENT_ZAJAT = 0;
		else
			INSTRUMENT_ZAJAT = 1;


		SET_C(idInGen);
	}
}

void Menu::actions_MINGE(const unsigned char& page)
{
	if (entry_MINGE == INIT)
	init_menu_MINGE(page);
	if (entry_MINGE == UPDATE) {
		draw_menu_MINGE(page);
		draw_param_MINGE(page, 25, LEFT_ALIGN);
	}
	if (entry_MINGE == IDLE)
	return;
}

void Menu::init_menu_MINGE(const unsigned char& page)
{
	display.clear_ram_null();
	entry_MINGE = UPDATE;
}

void Menu::draw_menu_MINGE(const unsigned char& page)
{
	const char* menu_table[10];
	//==конструктор меню
	menu_table[0] = PSTR(" М-ФУНКЦИИ ВХОДОВ");
	menu_table[1] = PSTR(" М102");
	menu_table[2] = PSTR(" М112");
	menu_table[3] = PSTR(" М122");
	menu_table[4] = PSTR(" М132");
	menu_table[5] = PSTR(" М142");
	menu_table[6] = PSTR(" М152");
	menu_table[7] = PSTR(" М162");
	menu_table[8] = PSTR(" М172");
	menu_table[9] = nullptr;
	//==
	//==размер меню
	menu_MINGE_SZ = 8;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_MINGE, page);
	//==
	entry_MINGE = IDLE;
}

void Menu::draw_param_MINGE(const unsigned char& page, int x, char align)
{
	char* p = &flash_prg._INGen2.M102;
	for (int i = 0; i < 8; i++) {
		flash_prg.show_P(*p, IN_P, x, 16 * (i + 1), align);
		p++;
	}
}

void Menu::menu_up_MINGE()
{
	if (curItem->id == idMInGen) {
		menu_cnt_MINGE--;
		if (menu_cnt_MINGE < 1)
		menu_cnt_MINGE = menu_MINGE_SZ;
		entry_MINGE = UPDATE;
	}
}

void Menu::menu_down_MINGE()
{
	if (curItem->id == idMInGen) {
		menu_cnt_MINGE++;
		if (menu_cnt_MINGE > menu_MINGE_SZ)
		menu_cnt_MINGE = 1;
		entry_MINGE = UPDATE;
	}
}

void Menu::menu_left_MINGE()
{
	if (curItem->id == idMInGen) {
		char* p = &flash_prg._INGen2.M102;
		p = p + (menu_cnt_MINGE - 1);
		flash_prg.changeP(*p, IN_P, 0);
		entry_MINGE = UPDATE;
	}
}

void Menu::menu_right_MINGE()
{
	if (curItem->id == idMInGen) {
		char* p = &flash_prg._INGen2.M102;
		p = p + (menu_cnt_MINGE - 1);
		flash_prg.changeP(*p, IN_P, 1);
		entry_MINGE = UPDATE;
	}
}

void Menu::menu_F_MINGE()
{
	if (curItem->id == idMInGen) {
		char* p = &flash_prg._INGen2.M102;
		p = p + (menu_cnt_MINGE - 1);
		flash_prg.change_MP(*p,IN_P);
		entry_MINGE = UPDATE;
	}
}

void Menu::menu_enter_MINGE()
{
	if (curItem->id == idMInGen) {
		char* p = &flash_prg._INGen2.M102;
		p = p + (menu_cnt_MINGE - 1);
		flash_prg.change_inv_P(*p);
		entry_MINGE = UPDATE;
	}
}

void Menu::menu_stop_MINGE()
{
	if (curItem->id == idMInGen) {
		entry_MINGE = INIT;
		flash_prg.write_eeprom(1180,sizeof(flash_prg._INGen2),&flash_prg._INGen2);
		SET_C(idInGen);
	}
}

void Menu::actions_OUTGEN(const unsigned char& page)
{
	if (entry_OUTGEN == INIT)
	init_menu_OUTGEN(page);
	if (entry_OUTGEN == UPDATE)
	draw_menu_OUTGEN(page);
	if (entry_OUTGEN == IDLE)
	return;
}

void Menu::init_menu_OUTGEN(const unsigned char& page)
{
	display.clear_ram_null();
	entry_OUTGEN = UPDATE;
}

void Menu::draw_menu_OUTGEN(const unsigned char& page)
{
	const char* menu_table[5];
	//==конструктор меню
	menu_table[0] = PSTR(" КОНФИГУРАЦИЯ ВЫХОДОВ");
	menu_table[1] = PSTR(" Общие функции");
	menu_table[2] = PSTR(" Шпиндель");
	menu_table[3] = PSTR(" М-функции выходов");
	menu_table[4] = nullptr;
	//==
	//==размер меню
	menu_OUTGEN_SZ = 3;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_OUTGEN, page);
	//==
	entry_OUTGEN = IDLE;
}

void Menu::menu_up_OUTGEN()
{
	if (curItem->id == idOutGen) {
		menu_cnt_OUTGEN--;
		if (menu_cnt_OUTGEN < 1)
		menu_cnt_OUTGEN = menu_OUTGEN_SZ;
		entry_OUTGEN = UPDATE;
	}
}

void Menu::menu_down_OUTGEN()
{
	if (curItem->id == idOutGen) {
		menu_cnt_OUTGEN++;
		if (menu_cnt_OUTGEN > menu_OUTGEN_SZ)
		menu_cnt_OUTGEN = 1;
		entry_OUTGEN = UPDATE;
	}
}

void Menu::menu_enter_OUTGEN()
{
	if (curItem->id == idOutGen) {
		if (menu_cnt_OUTGEN == 1) {
			entry_OUTGEN = INIT;
			SET_C(idFOutGen);
		}
		else if (menu_cnt_OUTGEN == 2) {
			entry_OUTGEN = INIT;
			SET_C(idSOutGen);
		}
		else if (menu_cnt_OUTGEN == 3) {
			entry_OUTGEN = INIT;
			SET_C(idMOutGen);
		}
	}
}

void Menu::menu_stop_OUTGEN()
{
	if (curItem->id == idOutGen) {
		entry_OUTGEN = INIT;
		SET_C(idAllp);
	}
}

void Menu::actions_FOUTGE(const unsigned char& page)
{
	if (entry_FOUTGE == INIT)
	init_menu_FOUTGE(page);
	if (entry_FOUTGE == UPDATE) {
		draw_menu_FOUTGE(page);
		draw_param_FOUTGE(page, 25, LEFT_ALIGN);
	}
	if (entry_FOUTGE == IDLE)
	return;
}

void Menu::init_menu_FOUTGE(const unsigned char& page)
{
	display.clear_ram_null();
	entry_FOUTGE = UPDATE;
}

void Menu::draw_menu_FOUTGE(const unsigned char& page)
{
	const char* menu_table[10];
	//==конструктор меню
	menu_table[0] = PSTR(" ОБЩИЕ ФУНКЦИИ ВЫХОДОВ");
	menu_table[1] = PSTR(" Ускоренный ход  G0");
	menu_table[2] = PSTR(" Подача          G1");
	menu_table[3] = PSTR(" Технол. останов M0");
	menu_table[4] = PSTR(" Конец программы М2");
	menu_table[5] = PSTR(" Ручной режим");
	menu_table[6] = PSTR(" Автоматический режим");
	menu_table[7] = PSTR(" Позиционирование");
	menu_table[8] = PSTR(" Готовность УЦИПУ");
	menu_table[9] = nullptr;
	//==
	//==размер меню
	menu_FOUTGE_SZ = 8;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_FOUTGE, page);
	//==
	entry_FOUTGE = IDLE;
}

void Menu::draw_param_FOUTGE(const unsigned char& page, int x, char align)
{
	char* p = &flash_prg._OUTGen.yskorenniy_xod_G0;
	char* tmp;
	for (int i = 0; i < 8; i++) {
		tmp = p + i;
		flash_prg.show_P(*tmp, OUT_P, x, 16 * (i + 1), align);		
	}	
}

void Menu::menu_up_FOUTGE()
{
	if (curItem->id == idFOutGen) {
		menu_cnt_FOUTGE--;
		//skip_menu_item(0,menu_cnt_FOUTGE,4);
		//skip_menu_item(0,menu_cnt_FOUTGE,3);		
		if (menu_cnt_FOUTGE < 1)
		menu_cnt_FOUTGE = menu_FOUTGE_SZ;
		entry_FOUTGE = UPDATE;
	}
}

void Menu::menu_down_FOUTGE()
{
	if (curItem->id == idFOutGen) {
		menu_cnt_FOUTGE++;
		//skip_menu_item(1,menu_cnt_FOUTGE,3);
		//skip_menu_item(1,menu_cnt_FOUTGE,4);
		if (menu_cnt_FOUTGE > menu_FOUTGE_SZ)
		menu_cnt_FOUTGE = 1;
		entry_FOUTGE = UPDATE;
	}
}

void Menu::menu_left_FOUTGE()
{
	if (curItem->id == idFOutGen) {
		if (menu_cnt_FOUTGE >= 1 && menu_cnt_FOUTGE <= 8) {
			char* p = &flash_prg._OUTGen.yskorenniy_xod_G0;
			p = p + (menu_cnt_FOUTGE - 1);
			flash_prg.changeP(*p, OUT_P, 0,1);
			entry_FOUTGE = UPDATE;
		}
	}
}

void Menu::menu_right_FOUTGE()
{
	if (curItem->id == idFOutGen) {
		if (menu_cnt_FOUTGE >= 1 && menu_cnt_FOUTGE <= 8) {
			char* p = &flash_prg._OUTGen.yskorenniy_xod_G0;
			p = p + (menu_cnt_FOUTGE - 1);
			flash_prg.changeP(*p, OUT_P, 1,1);
			entry_FOUTGE = UPDATE;
		}
	}
}

void Menu::menu_F_FOUTGE()
{
	if (curItem->id == idFOutGen) {
		if (menu_cnt_FOUTGE >= 1 && menu_cnt_FOUTGE <= 8) {
			char* p = &flash_prg._OUTGen.yskorenniy_xod_G0;
			p = p + (menu_cnt_FOUTGE - 1);
			flash_prg.change_MP(*p,OUT_P);
			entry_FOUTGE = UPDATE;
		}
	} 
}

void Menu::menu_enter_FOUTGE()
{
	/*if (curItem->id == idFOutGen) {
		if (menu_cnt_FOUTGE >= 1 && menu_cnt_FOUTGE <= 7) {
			char* p = &flash_prg._OUTGen.yskorenniy_xod_G0;
			p = p + (menu_cnt_FOUTGE - 1);
			flash_prg.change_inv_P(*p);
			entry_FOUTGE = UPDATE;
		}
	} */
}

void Menu::menu_stop_FOUTGE()
{
	if (curItem->id == idFOutGen) {
		entry_FOUTGE = INIT;
		flash_prg.write_eeprom(732,sizeof(flash_prg._OUTGen),&flash_prg._OUTGen);//save_Allp_param6_to_eeprom();
		SET_C(idOutGen);
	}
}

void Menu::actions_SOUTGE(const unsigned char& page)
{
	if (entry_SOUTGE == INIT)
	init_menu_SOUTGE(page);
	if (entry_SOUTGE == UPDATE) {
		draw_menu_SOUTGE(page);
		draw_param_SOUTGE(page, 25, LEFT_ALIGN);
	}
	if (entry_SOUTGE == IDLE)
	return;
}

void Menu::init_menu_SOUTGE(const unsigned char& page)
{
	display.clear_ram_null();
	entry_SOUTGE = UPDATE;
}

void Menu::draw_menu_SOUTGE(const unsigned char& page)
{
	const char* menu_table[9];
	//==конструктор меню
	menu_table[0] = PSTR(" ВЫХОДЫ УПРАВЛЕНИЯ ШПИНДЕЛЕМ");
	menu_table[1] = PSTR(" По часовой     М3");
	menu_table[2] = PSTR(" Против часовой М4");
	menu_table[3] = PSTR(" Стоп           М5");
	menu_table[4] = PSTR(" Толчок");
	menu_table[5] = sLine;
	menu_table[6] = PSTR(" M3,M4 через М5");
	menu_table[7] = PSTR(" Время для М5, сек");
	menu_table[8] = nullptr;
	//==
	//==размер меню
	menu_SOUTGE_SZ = 7;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_SOUTGE, page);
	//==
	entry_SOUTGE = IDLE;
}

void Menu::draw_param_SOUTGE(const unsigned char& page, int x, char align)
{
	char* p = &flash_prg._OUTGen.po_chasovoy_M3;
	const char *pf;
	for (int i = 0; i < 4; i++) {
		flash_prg.show_P(*p, OUT_P, x, 16 * (i + 1), align);
		p++;
	}

	//==M3 M4 через М5
	if (!flash_prg._OUTGen.M3_M4_cherez_M5)
		pf = sYES;
	else
		pf = sNO;
	display.print_flash_local(pf, 0, x, 16 * 6, align);
	//==

	//==Время для М5, сек
	if(flash_prg._OUTGen.pause_M5 == 0)
		display.print_flash_local(PSTR("Постоянно"), 0, x, 16 * 7, align);
	else
		print_long_format(flash_prg._OUTGen.pause_M5, CPrg::FORMAT::LINEAR_1, page, x, 16 * 7, align, 0,12);
	//==
}

void Menu::menu_up_SOUTGE()
{
	if (curItem->id == idSOutGen) {
		menu_cnt_SOUTGE--;
		if (menu_cnt_SOUTGE < 1)
		menu_cnt_SOUTGE = menu_SOUTGE_SZ;
		if (menu_cnt_SOUTGE == 5)
		menu_cnt_SOUTGE--;
		entry_SOUTGE = UPDATE;
	}
}

void Menu::menu_down_SOUTGE()
{
	if (curItem->id == idSOutGen) {
		menu_cnt_SOUTGE++;
		if (menu_cnt_SOUTGE > menu_SOUTGE_SZ)
		menu_cnt_SOUTGE = 1;
		if (menu_cnt_SOUTGE == 5)
		menu_cnt_SOUTGE++;
		entry_SOUTGE = UPDATE;
	}
}

void Menu::menu_left_SOUTGE()
{
	if (curItem->id == idSOutGen) {
		if (menu_cnt_SOUTGE >= 1 && menu_cnt_SOUTGE <= 4) {
			char* p = &flash_prg._OUTGen.po_chasovoy_M3;
			char tmp = menu_cnt_SOUTGE;
			p = p + (tmp - 1);
			flash_prg.changeP(*p, OUT_P, 0,1);
			entry_SOUTGE = UPDATE;
		}
		
		if (menu_cnt_SOUTGE == 6) {
			flash_prg._OUTGen.M3_M4_cherez_M5 ^= 0x01;
			entry_SOUTGE = UPDATE;
		}
		if (menu_cnt_SOUTGE == 7) {
			flash_prg._OUTGen.pause_M5--;
			if (flash_prg._OUTGen.pause_M5 < 0)
			flash_prg._OUTGen.pause_M5 = 100;
			entry_SOUTGE = UPDATE;
		}
	}
}

void Menu::menu_right_SOUTGE()
{
	if (curItem->id == idSOutGen) {
		if (menu_cnt_SOUTGE >= 1 && menu_cnt_SOUTGE <= 4) {
			char* p = &flash_prg._OUTGen.po_chasovoy_M3;
			char tmp = menu_cnt_SOUTGE;
			p = p + (tmp - 1);
			flash_prg.changeP(*p, OUT_P, 1,1);
			entry_SOUTGE = UPDATE;
		}

		if (menu_cnt_SOUTGE == 6) {
			flash_prg._OUTGen.M3_M4_cherez_M5 ^= 0x01;
			entry_SOUTGE = UPDATE;
		}
		if (menu_cnt_SOUTGE == 7) {
			flash_prg._OUTGen.pause_M5++;
			if (flash_prg._OUTGen.pause_M5 > 100)
			flash_prg._OUTGen.pause_M5 = 0;
			entry_SOUTGE = UPDATE;
		}
	}
}

void Menu::menu_F_SOUTGE()
{
	if (curItem->id == idSOutGen) {
		if (menu_cnt_SOUTGE >= 1 && menu_cnt_SOUTGE <= 4) {
			char* p = &flash_prg._OUTGen.po_chasovoy_M3;
			char tmp = menu_cnt_SOUTGE;
			p = p + (tmp - 1);
			flash_prg.change_MP(*p,OUT_P);
			entry_SOUTGE = UPDATE;
		}
	}
}

void Menu::menu_enter_SOUTGE()
{
	/*if (curItem->id == idSOutGen) {
		if (menu_cnt_SOUTGE >= 1 && menu_cnt_SOUTGE <= 3) {
			char* p = &flash_prg._OUTGen.po_chasovoy_M3;
			char tmp = menu_cnt_SOUTGE;
			p = p + (tmp - 1);
			flash_prg.change_inv_P(*p);
			entry_SOUTGE = UPDATE;
		}
	} */
}

void Menu::menu_stop_SOUTGE()
{
	if (curItem->id == idSOutGen) {
		entry_SOUTGE = INIT;
		flash_prg.write_eeprom(732,sizeof(flash_prg._OUTGen),&flash_prg._OUTGen);//save_Allp_param6_to_eeprom();
		SET_C(idOutGen);
	}
}

void Menu::actions_MOUTGE(const unsigned char& page)
{
	if (entry_MOUTGE == INIT)
	init_menu_MOUTGE(page);
	if (entry_MOUTGE == UPDATE) {
		draw_menu_MOUTGE(page);
		draw_param_MOUTGE(page, 25, LEFT_ALIGN);
	}
	if (entry_MOUTGE == IDLE)
	return;
}

void Menu::init_menu_MOUTGE(const unsigned char& page)
{
	display.clear_ram_null();
	entry_MOUTGE = UPDATE;
}

void Menu::draw_menu_MOUTGE(const unsigned char& page)
{
	const char* menu_table[10];
	//==конструктор меню
	menu_table[0] = PSTR(" М-ФУНКЦИИ ВЫХОДОВ");
	menu_table[1] = PSTR(" М100-выкл М101-вкл");
	menu_table[2] = PSTR(" М110-выкл М111-вкл");
	menu_table[3] = PSTR(" М120-выкл М121-вкл");
	menu_table[4] = PSTR(" М130-выкл М131-вкл");
	menu_table[5] = PSTR(" М140-выкл М141-вкл");
	menu_table[6] = PSTR(" М150-выкл М151-вкл");
	menu_table[7] = PSTR(" М160-выкл М161-вкл");
	menu_table[8] = PSTR(" М170-выкл М171-вкл");
	menu_table[9] = nullptr;
	//==
	//==размер меню
	menu_MOUTGE_SZ = 8;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_MOUTGE, page);
	//==
	entry_MOUTGE = IDLE;
}

void Menu::draw_param_MOUTGE(const unsigned char& page, int x, char align)
{
	char* p = &flash_prg._OUTGen2.M100off_M101on;
	for (int i = 0; i < 8; i++) {
		flash_prg.show_P(*p, OUT_P, x, 16 * (i + 1), align);
		p++;
	}
}

void Menu::menu_up_MOUTGE()
{
	if (curItem->id == idMOutGen) {
		menu_cnt_MOUTGE--;
		if (menu_cnt_MOUTGE < 1)
		menu_cnt_MOUTGE = menu_MOUTGE_SZ;
		entry_MOUTGE = UPDATE;
	}
}

void Menu::menu_down_MOUTGE()
{
	if (curItem->id == idMOutGen) {
		menu_cnt_MOUTGE++;
		if (menu_cnt_MOUTGE > menu_MOUTGE_SZ)
		menu_cnt_MOUTGE = 1;
		entry_MOUTGE = UPDATE;
	}
}

void Menu::menu_left_MOUTGE()
{
	if (curItem->id == idMOutGen) {
		char* p = &flash_prg._OUTGen2.M100off_M101on;
		p = p + (menu_cnt_MOUTGE - 1);
		flash_prg.changeP(*p, OUT_P, 0,1);
		entry_MOUTGE = UPDATE;
	}
}

void Menu::menu_right_MOUTGE()
{
	if (curItem->id == idMOutGen) {
		char* p = &flash_prg._OUTGen2.M100off_M101on;
		p = p + (menu_cnt_MOUTGE - 1);
		flash_prg.changeP(*p, OUT_P, 1,1);
		entry_MOUTGE = UPDATE;
	}
}

void Menu::menu_F_MOUTGE()
{
	if (curItem->id == idMOutGen) {
		char* p = &flash_prg._OUTGen2.M100off_M101on;
		p = p + (menu_cnt_MOUTGE - 1);
		flash_prg.change_MP(*p,OUT_P);
		entry_MOUTGE = UPDATE;
	} 
}

void Menu::menu_enter_MOUTGE()
{
	/*if (curItem->id == idMOutGen) {
		char* p = &flash_prg._OUTGen.M100off_M101on;
		p = p + (menu_cnt_MOUTGE - 1);
		flash_prg.change_inv_P(*p);
		entry_MOUTGE = UPDATE;
	} */
}

void Menu::menu_stop_MOUTGE()
{
	if (curItem->id == idMOutGen) {
		entry_MOUTGE = INIT;
		flash_prg.write_eeprom(1148,sizeof(flash_prg._OUTGen2),&flash_prg._OUTGen2);
		SET_C(idOutGen);
	}
}

void Menu::actions_m_G1_G0(const unsigned char & page)
{
	if (entry_G1_G0 == INIT)
		init_menu_G1_G0(page);
	if (entry_G1_G0 == UPDATE) {
		draw_menu_G1_G0(page);
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET)
			draw_param_G1_G0_Discret(page);
		else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			draw_param_G1_G0_Analog(page);
	}
	if (entry_G1_G0 == IDLE)
		return;
}

void Menu::actions_G1_G0_input_pr(const unsigned char& page)
{
	int x_, y_;
	long *p = nullptr;
	int8_t cnt = 0;

	if (!(menu_cnt_G0 >= 1 && menu_cnt_G0 <= 6))
		return;

	if (curItem->id == idG0Input) {
		p = &flash_prg._Allp_param5[flash_prg.ch].start_G0;
		cnt = menu_cnt_G0;
	}
	else if (curItem->id == idG1Input) {
		p = &flash_prg._Allp_param4[flash_prg.ch].start_G1;
		cnt = menu_cnt_G1;
	}

	x_ = 15;
	y_ = cnt * 16;	

	flashing_cursor(x_, y_, page);
	if (menu.ResultForInputNum == input::ok) {
		long tmp = 0;		
		if (!display.strnlen_lir((const char*)bufASCIIstr))
			goto skip_pr;
		ConvertASCIIlinear_circle_tolong(tmp,gFi(flash_prg.ch),bufASCIIstr);		
		if(cnt == 1){ //ввод зоны СТАРТ
			if (curItem->id == idG0Input) {
				if(tmp > flash_prg._OUTAxis2[flash_prg.ch].MaxSpdG0){
					status(PSTR("> макс. скрости"),300);
					goto skip_pr;
					}
			}
			else if (curItem->id == idG1Input) {
				if(tmp > flash_prg._OUTAxis2[flash_prg.ch].MaxSpdG1){
					status(PSTR("> макс. скрости"),300);
					goto skip_pr;
					}
			} 
		}  
		
		p[cnt - 1] = tmp; //вычислить указатель p
		skip_pr:
		exit_G1_G0_input_pr(page,x_,y_);
		
	}
	if (menu.ResultForInputNum == input::cancel_) {
		exit_G1_G0_input_pr(page,x_,y_);
	}
}

void Menu::exit_G1_G0_input_pr(const unsigned char& page,int x_,int y_)
{
	display.print_flash_local(sProbel10, page, x_, y_);
	if (curItem->id == idG0Input)
		SET_C(idG0);
	else if(curItem->id == idG1Input)
		SET_C(idG1);
	entry_G1_G0 = UPDATE;
	init_cursor_pos();
}

void Menu::init_menu_G1_G0(const unsigned char & page)
{
	display.clear_ram_null();
	entry_G1_G0 = UPDATE;
}

void Menu::draw_menu_G1_G0(const unsigned char & page)
{
	const char *menu_table[8] = { 0 };
	uint8_t i = 0;
	uint8_t j = 0;
	//==конструктор меню
	if (curItem->id == idG0)
		menu_table[i++] = sG0header;
	else if (curItem->id == idG1)
		menu_table[i++] = sG1header;

	while (i < 7) {
		menu_table[i++] = sZones[j++];
	}
	//==
	//==размер меню
	menu_G1_G0_SZ = i - 1;
	//==
	//==вывод на экран
	uint8_t cnt_p = 0;
	if (curItem->id == idG0)
		cnt_p = menu_cnt_G0;
	else if (curItem->id == idG1)
		cnt_p = menu_cnt_G1;

	Draw_menu(menu_table, cnt_p,page);
	const char *p_s = sError;
	if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET)
		p_s = sABCDheader;
	else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG){
		if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::LINEAR)
			p_s = sFheader;
		else
			p_s = sSheader;
	}
	display.print_flash_local(p_s, page, 29, 0);
	//==
	entry_G1_G0 = IDLE;
}

void Menu::draw_param_G1_G0_Discret(const unsigned char & page)
{
	uint8_t tmp = 0;
	char* p;
	if (curItem->id == idG0) {		
		for (int i = 0; i < 6; i++) {
			p = &flash_prg._Allp_param[flash_prg.ch].start_G0;
			p = p + i;
			tmp = *p;
			for (int j = 0; j < 4; j++) {
				if( i == menu_cnt_G0 - 1 && j == menu_cnt_G0h)
					INVERT_CHAR()
				else
					NORMAL_CHAR()
				if (tmp & 8)
					display.put_char_local_independent('1', page, 29 + (j * 3), 16 * (i + 1));
				else
					display.put_char_local_independent('0', page, 29 + (j * 3), 16 * (i + 1));
				tmp <<= 1;
				NORMAL_CHAR()
			}
		}
	}
	else if (curItem->id == idG1) {
		for (int i = 0; i < 6; i++) {		
			p = &flash_prg._Allp_param[flash_prg.ch].start_G1;
			p = p + i;
			tmp = *p;
			for (int j = 0; j < 4; j++) {
				if (i == menu_cnt_G1 - 1 && j == menu_cnt_G1h)
					INVERT_CHAR()
				else
					NORMAL_CHAR()
				if (tmp & 8)
					display.put_char_local_independent('1', page, 29 + (j * 3), 16 * (i + 1));
				else
					display.put_char_local_independent('0', page, 29 + (j * 3), 16 * (i + 1));
				tmp <<= 1;
				NORMAL_CHAR()
			}
		}
	}
}

void Menu::draw_param_G1_G0_Analog(const unsigned char & page)
{
	long *p = nullptr;
	int f = gFi(flash_prg.ch);
	if (curItem->id == idG0) {
		p = &flash_prg._Allp_param5[flash_prg.ch].start_G0;
		}
	else if (curItem->id == idG1) {
		p = &flash_prg._Allp_param4[flash_prg.ch].start_G1;
		}

	if(!p)
		return;

	for (int i = 0; i < 6; i++) {
		print_long_format(*p,f,page,29,16 * (i + 1),0,0,11,1);
		p++;
	}
}

void Menu::menu_up_G1_G0()
{
	if (curItem->id == idG0) {
		menu_cnt_G0--;
		if (menu_cnt_G0 <= 0)
		menu_cnt_G0 = menu_G1_G0_SZ;
		entry_G1_G0 = UPDATE;
	}
	else if(curItem->id == idG1) {
		menu_cnt_G1--;
		if (menu_cnt_G1 <= 0)
		menu_cnt_G1 = menu_G1_G0_SZ;
		entry_G1_G0 = UPDATE;
	}
}

void Menu::menu_down_G1_G0()
{
	if (curItem->id == idG0) {
		menu_cnt_G0++;
		if (menu_cnt_G0 > menu_G1_G0_SZ)
		menu_cnt_G0 = 1;
		entry_G1_G0 = UPDATE;
	}
	else if (curItem->id == idG1) {
		menu_cnt_G1++;
		if (menu_cnt_G1 > menu_G1_G0_SZ)
		menu_cnt_G1 = 1;
		entry_G1_G0 = UPDATE;
	}
}

void Menu::menu_left_G1_G0()
{
	if (curItem->id == idG0) {
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			return;
		menu_cnt_G0h--;
		if (menu_cnt_G0h < 0)
		menu_cnt_G0h = 3;
		entry_G1_G0 = UPDATE;
	}
	else if (curItem->id == idG1) {
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			return;
		menu_cnt_G1h--;
		if (menu_cnt_G1h < 0)
		menu_cnt_G1h = 3;
		entry_G1_G0 = UPDATE;
	}
}

void Menu::menu_right_G1_G0()
{
	if (curItem->id == idG0) {
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			return;
		menu_cnt_G0h++;
		if (menu_cnt_G0h > 3)
		menu_cnt_G0h = 0;
		entry_G1_G0 = UPDATE;
	}
	else if (curItem->id == idG1) {
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			return;
		menu_cnt_G1h++;
		if (menu_cnt_G1h > 3)
		menu_cnt_G1h = 0;
		entry_G1_G0 = UPDATE;
	}
}

void Menu::menu_enter_G1_G0()
{
	if (curItem->id == idG0) {		
		if (menu_cnt_G0 >= 1 && menu_cnt_G0 <= 6) {
			if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET){
				char* tmp;
				tmp = &flash_prg._Allp_param[flash_prg.ch].start_G0;
				tmp = tmp + (menu_cnt_G0 - 1);
				*tmp ^= plb2(3 - menu_cnt_G0h);
				entry_G1_G0 = UPDATE;
			}
			else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG){
				init_cursor_pos();
				CurFormatM = CPrg::LINEAR_3;
				SET_C(idG0Input);
				return;
			}
		}					
	}
	else if (curItem->id == idG1) {
		if (menu_cnt_G1 >= 1 && menu_cnt_G1 <= 6) {
			if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET){
				char* tmp;
				tmp = &flash_prg._Allp_param[flash_prg.ch].start_G1;
				tmp = tmp + (menu_cnt_G1 - 1);
				*tmp ^= plb2(3 - menu_cnt_G1h);
				entry_G1_G0 = UPDATE;
			}
			else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG){
				init_cursor_pos();
				CurFormatM = CPrg::LINEAR_3;
				SET_C(idG1Input);
				return;
			}
		}		
	}
}

void Menu::menu_1_G1_G0() //key_1
{
	if (curItem->id == idG0) {
		if (menu_cnt_G0 >= 1 && menu_cnt_G0 <= 6) {
			char* tmp = &flash_prg._Allp_param[flash_prg.ch].start_G0;
			tmp = tmp + (menu_cnt_G0 - 1);
			*tmp |= plb2(3 - menu_cnt_G0h);
		}
		entry_G1_G0 = UPDATE;
	}
	else if (curItem->id == idG1) {
		if (menu_cnt_G1 >= 1 && menu_cnt_G1 <= 6) {
			char* tmp = &flash_prg._Allp_param[flash_prg.ch].start_G1;
			tmp = tmp + (menu_cnt_G1 - 1);
			*tmp |= plb2(3 - menu_cnt_G1h);
		}
		entry_G1_G0 = UPDATE;
	}
}

void Menu::menu_0_G1_G0()//key_0
{
	if (curItem->id == idG0) {
		if (menu_cnt_G0 >= 1 && menu_cnt_G0 <= 6) {
			char* tmp = &flash_prg._Allp_param[flash_prg.ch].start_G0;
			tmp = tmp + (menu_cnt_G0 - 1);
			*tmp &= ~plb2(3 - menu_cnt_G0h);
		}
		entry_G1_G0 = UPDATE;
	}
	else if (curItem->id == idG1) {
		if (menu_cnt_G1 >= 1 && menu_cnt_G1 <= 6) {
			char* tmp = &flash_prg._Allp_param[flash_prg.ch].start_G1;
			tmp = tmp + (menu_cnt_G1 - 1);
			*tmp &= ~plb2(3 - menu_cnt_G1h);
		}
		entry_G1_G0 = UPDATE;
	}
}

void Menu::menu_stop_G1_G0()
{
	if (curItem->id == idG0 || curItem->id == idG1) {
		entry_G1_G0 = INIT;
		//сохранить в память EEPROM
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET){
			flash_prg.write_eeprom(188,sizeof(flash_prg._Allp_param[0]) * N_AXIS,&flash_prg._Allp_param);//save_Allp_param_to_eeprom();
			flash_prg._Allp_param[flash_prg.ch].start_LUFT = flash_prg._Allp_param[flash_prg.ch].spd_G1[menu_cnt_VIBLUFh];
			}
		else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG){
			if (curItem->id == idG1)
				flash_prg.write_eeprom(1373,sizeof(flash_prg._Allp_param4),&flash_prg._Allp_param4);
			else if (curItem->id == idG0)
				flash_prg.write_eeprom(1493,sizeof(flash_prg._Allp_param5),&flash_prg._Allp_param5);
			}
		SET_C(idSpdZ);
	}
}
/*
получение формата для ввода/вывода аналоговой скорости
*/
int Menu::gFi(uint8_t c)
{
	/*if(c == SW_AXIS0)
		return CPrg::LINEAR_3;
	if(flash_prg.axis_cfg[c].type == CPrg::LINEAR)
		return flash_prg.axis_cfg[c].format;
	else
		return CPrg::LINEAR_3; //для круговой оси об/мин  */
	return CPrg::LINEAR_4;
}


void Menu::actions_VIXPM(const unsigned char& page)
{
	if (entry_VIXPM == INIT)
		init_menu_VIXPM(page);
	if (entry_VIXPM == UPDATE) {
		draw_menu_VIXPM(page);
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET)
			draw_param_VIXPM_Discret(page);
		else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			draw_param_VIXPM_Analog(page);
	}
	if (entry_VIXPM == IDLE)
		return;
}

void Menu::actions_VIXPM_input_pr(const unsigned char& page)
{
	int x_, y_;
	long *p = nullptr;
	int8_t cnt = 0;

	if (!(menu_cnt_VIXPM >= 1 && menu_cnt_VIXPM <= 4))
		return;

	p = &flash_prg._Allp_param6[flash_prg.ch].start_PM;
	cnt = menu_cnt_VIXPM;

	x_ = 15;
	y_ = cnt * 16;

	flashing_cursor(x_, y_, page);
	if (menu.ResultForInputNum == input::ok) {
		long tmp = 0;
		if (!display.strnlen_lir((const char*)bufASCIIstr))
			goto skip_pr;
		ConvertASCIIlinear_circle_tolong(tmp,gFi(flash_prg.ch),bufASCIIstr);
		p[cnt - 1] = tmp; //вычислить указатель p
		skip_pr:
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idVixodPM);
		entry_VIXPM = UPDATE;
		init_cursor_pos();		
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idVixodPM);
		entry_VIXPM = UPDATE;
		init_cursor_pos();
	}
}

void Menu::init_menu_VIXPM(const unsigned char& page)
{
	display.clear_ram_null();
	entry_VIXPM = UPDATE;
}

void Menu::draw_menu_VIXPM(const unsigned char& page)
{
	const char* menu_table[6];
	uint8_t i = 0;
	//==конструктор меню
	menu_table[i++] = PSTR(" ВЫХОД В PM");

	menu_table[i++] = PSTR(" Старт");
	menu_table[i++] = PSTR(" Ограничитель");
	menu_table[i++] = PSTR(" Зона PM");
	menu_table[i++] = PSTR(" Стоп");
	menu_table[i] = nullptr;
	//==
	//==размер меню
	menu_VIXPM_SZ = i - 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_VIXPM,page);
	const char *p_s = sError;
	if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET)
		p_s = sABCDheader;
	else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG){
		if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::LINEAR)
			p_s = sFheader;
		else
			p_s = sSheader;
	}
	display.print_flash_local(p_s, page, 29, 0);
	//==
	entry_VIXPM = IDLE;
}

void Menu::draw_param_VIXPM_Discret(const unsigned char& page)
{
	int ch = flash_prg.ch;
	for (int i = 0; i < 4; i++) {
		uint8_t tmp = 0;
		if (i == 0)
			tmp = flash_prg._Allp_param[ch].start_PM;
		else if (i == 1)
			tmp = flash_prg._Allp_param[ch].limit_PM;
		else if (i == 2)
			tmp = flash_prg._Allp_param[ch].zone_PM;
		else if (i == 3)
			tmp = flash_prg._Allp_param[ch].stop_PM;

		for (int j = 0; j < 4; j++) {
			if (i == menu_cnt_VIXPM - 1 && j == menu_cnt_VIXPMh)
				INVERT_CHAR()
			else
				NORMAL_CHAR()
			if (tmp & 8)
				display.put_char_local_independent('1', page, 29 + (j * 3), 16 * (i + 1));
			else
				display.put_char_local_independent('0', page, 29 + (j * 3), 16 * (i + 1));
			tmp <<= 1;
			NORMAL_CHAR()
		}
	}
}

void Menu::draw_param_VIXPM_Analog(const unsigned char& page)
{
	long *p = &flash_prg._Allp_param6[flash_prg.ch].start_PM;
	int f = gFi(flash_prg.ch);
	if(!p)
		return;

	for (int i = 0; i < 4; i++) {
		print_long_format(*p,f,page,29,16 * (i + 1),0,0,11,1);
		p++;
	}
}

void Menu::menu_up_VIXPM()
{
	if (curItem->id == idVixodPM) {
		menu_cnt_VIXPM--;
		if (menu_cnt_VIXPM <= 0)
		menu_cnt_VIXPM = menu_VIXPM_SZ;
		entry_VIXPM = UPDATE;
	}
}

void Menu::menu_down_VIXPM()
{
	if (curItem->id == idVixodPM) {
		menu_cnt_VIXPM++;
		if (menu_cnt_VIXPM > menu_VIXPM_SZ)
		menu_cnt_VIXPM = 1;
		entry_VIXPM = UPDATE;
	}
}

void Menu::menu_left_VIXPM()
{
	if (curItem->id == idVixodPM) {
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			return;
		menu_cnt_VIXPMh--;
		if (menu_cnt_VIXPMh < 0)
		menu_cnt_VIXPMh = 3;
		entry_VIXPM = UPDATE;
	}
}

void Menu::menu_right_VIXPM()
{
	if (curItem->id == idVixodPM) {
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			return;
		menu_cnt_VIXPMh++;
		if (menu_cnt_VIXPMh > 3)
		menu_cnt_VIXPMh = 0;
		entry_VIXPM = UPDATE;
	}
}

void Menu::menu_enter_VIXPM()
{	
	if (curItem->id == idVixodPM) {	
		int ch = flash_prg.ch;	
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET){
			if (menu_cnt_VIXPM == 1)
				flash_prg._Allp_param[ch].start_PM ^= plb2(3 - menu_cnt_VIXPMh);
			else if (menu_cnt_VIXPM == 2) 
				flash_prg._Allp_param[ch].limit_PM ^= plb2(3 - menu_cnt_VIXPMh);
			else if (menu_cnt_VIXPM == 3)
				flash_prg._Allp_param[ch].zone_PM ^= plb2(3 - menu_cnt_VIXPMh);
			else if (menu_cnt_VIXPM == 4)
				flash_prg._Allp_param[ch].stop_PM ^= plb2(3 - menu_cnt_VIXPMh);
			entry_VIXPM = UPDATE;
		}else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG){
			init_cursor_pos();
			CurFormatM = CPrg::LINEAR_3;
			SET_C(idVixodPMInput);
			return;
		}
	}
}

void Menu::menu_stop_VIXPM()
{
	if (curItem->id == idVixodPM) {
		//сохранить в память EEPROM
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET)
			flash_prg.write_eeprom(188,sizeof(flash_prg._Allp_param[0]) * N_AXIS,&flash_prg._Allp_param);//save_Allp_param_to_eeprom();
		else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			flash_prg.write_eeprom(1613,sizeof(flash_prg._Allp_param6),&flash_prg._Allp_param6);
		entry_VIXPM = INIT;
		SET_C(idSpdZ);
	}
}

void Menu::actions_VIBLUF(const unsigned char& page)
{
	if (entry_VIBLUF == INIT)
		init_menu_VIBLUF(page);
	if (entry_VIBLUF == UPDATE) {
		draw_menu_VIBLUF(page);
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET)
			draw_param_VIBLUF_Discret(page);
		else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			draw_param_VIBLUF_Analog(page);
	}
	if (entry_VIXPM == IDLE)
	return;
}

void Menu::actions_VIBLUF_input_pr(const unsigned char& page)
{
	int x_, y_;
	long *p = nullptr;
	int8_t cnt = 0;

	if (!(menu_cnt_VIBLUF >= 1 && menu_cnt_VIBLUF <= 1))
		return;

	p = &flash_prg._Allp_param6[flash_prg.ch].start_LUFT;
	cnt = menu_cnt_VIBLUF;

	x_ = 15;
	y_ = cnt * 16;

	flashing_cursor(x_, y_, page);
	if (menu.ResultForInputNum == input::ok) {
		long tmp = 0;
		if (!display.strnlen_lir((const char*)bufASCIIstr))
			goto skip_pr;
		ConvertASCIIlinear_circle_tolong(tmp,gFi(flash_prg.ch),bufASCIIstr);
		p[cnt - 1] = tmp; //вычислить указатель p
		skip_pr:
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idViborkaLuft);
		entry_VIBLUF = UPDATE;
		init_cursor_pos();
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idViborkaLuft);
		entry_VIBLUF = UPDATE;
		init_cursor_pos();
	}
}

void Menu::init_menu_VIBLUF(const unsigned char& page)
{
	display.clear_ram_null();
	entry_VIBLUF = UPDATE;
}

void Menu::draw_menu_VIBLUF(const unsigned char& page)
{
	const char* menu_table[3];
	uint8_t i = 0;
	//==конструктор меню
	menu_table[i++] = PSTR(" ВЫБОРКА ЛЮФТА");
	menu_table[i++] = PSTR(" Старт");
	menu_table[i] = nullptr;
	//==
	//==размер меню
	menu_VIBLUF_SZ = i - 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_VIBLUF,page);
	const char *p_s = sError;
	if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET)
		p_s = sABCDheader;
	else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG){
		if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::LINEAR)
			p_s = sFheader;
		else
			p_s = sSheader;
	}
	display.print_flash_local(p_s, page, 29, 0);
	//==
	entry_VIBLUF = IDLE;
}

void Menu::draw_param_VIBLUF_Discret(const unsigned char& page)
{
	int ch = flash_prg.ch;
	uint8_t tmp = flash_prg._Allp_param[ch].start_LUFT;
	int  i = 0;

	int z = 0;
	for(; z < ZONES - 1;z++){
		if(flash_prg._Allp_param[ch].spd_G1[z] == tmp)
			break;
	}
	if(z < ZONES - 1){
		menu_cnt_VIBLUFh = z;
		display.print_flash_local(&sZones[menu_cnt_VIBLUFh + 1][1],page,19,16 * 1); 
	}
	else{
		display.print_flash_local(PSTR("   ?   "),page,19,16 * 1); 
	}

	for (int j = 0; j < 4; j++) {
		//if (i == menu_cnt_VIBLUF - 1 && j == menu_cnt_VIBLUFh)
		//	INVERT_CHAR()
		//else
		//	NORMAL_CHAR()
		if (tmp & 8)
			display.put_char_local_independent('1', page, 29 + (j * 3), 16 * (i + 1));
		else
			display.put_char_local_independent('0', page, 29 + (j * 3), 16 * (i + 1));
		tmp <<= 1;
		NORMAL_CHAR()
	}  
}

void Menu::draw_param_VIBLUF_Analog(const unsigned char& page)
{	
	print_long_format(flash_prg._Allp_param6[flash_prg.ch].start_LUFT,gFi(flash_prg.ch),page,29,16 * (0 + 1),0,0,11,1);	
}

void Menu::menu_up_VIBLUF()
{

}

void Menu::menu_down_VIBLUF()
{

}

void Menu::menu_left_VIBLUF()
{
	if (curItem->id == idViborkaLuft) {
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET){
			menu_cnt_VIBLUFh--;
			if (menu_cnt_VIBLUFh < 0)
				menu_cnt_VIBLUFh = 3;

			flash_prg._Allp_param[flash_prg.ch].start_LUFT = flash_prg._Allp_param[flash_prg.ch].spd_G1[menu_cnt_VIBLUFh];

			entry_VIBLUF = UPDATE;
		}		
	}
}

void Menu::menu_right_VIBLUF()
{
	if (curItem->id == idViborkaLuft) {
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET){
			menu_cnt_VIBLUFh++;
			if (menu_cnt_VIBLUFh > 3)
				menu_cnt_VIBLUFh = 0;

			flash_prg._Allp_param[flash_prg.ch].start_LUFT = flash_prg._Allp_param[flash_prg.ch].spd_G1[menu_cnt_VIBLUFh];

			entry_VIBLUF = UPDATE;
		}		
	}
}

void Menu::menu_enter_VIBLUF()
{
	if (curItem->id == idViborkaLuft) {
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET){
			//if (menu_cnt_VIBLUF == 1) 
			//	flash_prg._Allp_param[flash_prg.ch].start_LUFT ^= plb2(3 - menu_cnt_VIBLUFh);
			entry_VIBLUF = UPDATE;
		}else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG){
			init_cursor_pos();
			CurFormatM = CPrg::LINEAR_3;
			SET_C(idViborkaLuftInput);
			return;
		}
	}
}

void Menu::menu_stop_VIBLUF()
{
	if (curItem->id == idViborkaLuft) {
		//сохранить в память EEPROM
		if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::DISCRET)
			flash_prg.write_eeprom(188,sizeof(flash_prg._Allp_param[0]) * N_AXIS,&flash_prg._Allp_param);
		else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::ANALOG)
			flash_prg.write_eeprom(1613,sizeof(flash_prg._Allp_param6),&flash_prg._Allp_param6);
		entry_VIBLUF = INIT;
		SET_C(idSpdZ);
	}
}



void Menu::actions_CHPA(const unsigned char& page)
{
	if (entry_CHPA == INIT)
	init_menu_CHPA(page);
	if (entry_CHPA == UPDATE)
	draw_menu_CHPA(page);
	if (entry_CHPA == IDLE)
	return;
}

void Menu::init_menu_CHPA(const unsigned char& page)
{
	display.clear_ram_null();
	show_axis_above_menu2(13);
	entry_CHPA = UPDATE;
}

void Menu::draw_menu_CHPA(const unsigned char& page)
{
	const char* menu_table[8];
	//==конструктор меню
	menu_table[0] =  PSTR(" ПАРАМЕТРЫ ОСИ");
	menu_table[1] =  PSTR(" Отображение");
	menu_table[2] =  PSTR(" Управление");
	menu_table[3] =  PSTR(" Конфигурация входов");
	menu_table[4] =  PSTR(" Конфигурация выходов");
	menu_table[5] =  PSTR(" Зоны торможения");
	menu_table[6] =  PSTR(" Зоны скорости");
	menu_table[7] = nullptr;
	//==
	//==размер меню
	menu_CHPA_SZ = 6;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_CHPA,page);
	//==
	entry_CHPA = IDLE;
}

void Menu::menu_up_CHPA()
{
	if (curItem->id == idChpa) {
		menu_cnt_CHPA--;
		if (menu_cnt_CHPA < 1)
		menu_cnt_CHPA = menu_CHPA_SZ;
		entry_CHPA = UPDATE;
	}
}

void Menu::menu_down_CHPA()
{
	if (curItem->id == idChpa) {
		menu_cnt_CHPA++;
		if (menu_cnt_CHPA > menu_CHPA_SZ)
		menu_cnt_CHPA = 1;
		entry_CHPA = UPDATE;
	}
}

void Menu::menu_enter_CHPA()
{
	if (curItem->id == idChpa) {
		//==Отображение
		if (menu_cnt_CHPA == 1) {
			if(flash_prg.ch == SW_AXIS0)
				return;
			SET_C(idIndicOpt);
			entry_CHPA = INIT;
			return;
		}
		//==
		//==Управление
		if (menu_cnt_CHPA == 2) {
			if(flash_prg.ch == SW_AXIS0)
				return;
			SET_C(idCntrl);
			entry_CHPA = INIT;
			return;
		}
		//==
		//==Конфигурация входов
		if (menu_cnt_CHPA == 3) {
			SET_C(idInAxis);
			entry_CHPA = INIT;
			return;
		}
		//==
		//==Конфигурация выходов
		if (menu_cnt_CHPA == 4) {
			SET_C(idOutAxis);
			entry_CHPA = INIT;
			return;
		}
		//==
		//==Зоны торможения
		if (menu_cnt_CHPA == 5) {
			if(flash_prg.ch == SW_AXIS0)
				return;
			else if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::IMPULSE)
				return;
			SET_C(idZoneBrake);
			entry_CHPA = INIT;
			return;
		}
		//==
		//==Зоны скорости
		if (menu_cnt_CHPA == 6) {
			if(flash_prg._OUTAxis[flash_prg.ch].typeZ == CPrg::IMPULSE)
				return;
			SET_C(idSpdZ);
			entry_CHPA = INIT;
			return;
		}
		//==
	}
}

void Menu::menu_stop_CHPA()
{
	if (curItem->id == idChpa) {
		SET_C(idAxislist);
		entry_CHPA = INIT;
	}
}

void Menu::actions_SPDZ(const unsigned char& page)
{
	if (entry_SPDZ == INIT)
	init_menu_SPDZ(page);
	if (entry_SPDZ == UPDATE)
	draw_menu_SPDZ(page);
	if (entry_SPDZ == IDLE)
	return;
}

void Menu::init_menu_SPDZ(const unsigned char& page)
{
	display.clear_ram_null();
	show_axis_above_menu2(13);
	entry_SPDZ = UPDATE;
}

void Menu::draw_menu_SPDZ(const unsigned char& page)
{
	const char* menu_table[6] = { 0 };
	uint8_t i = 0;
	//==конструктор меню
	for (uint8_t j = 0; j < 5;) {
		menu_table[i++] = sSpdZ[j++];
	}
	//==
	//==размер меню
	menu_SPDZ_SZ = i - 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_SPDZ,page);
	//==
	entry_SPDZ = IDLE;
}

void Menu::menu_up_SPDZ()
{
	if (curItem->id == idSpdZ) {
		menu_cnt_SPDZ--;
		if (menu_cnt_SPDZ < 1)
		menu_cnt_SPDZ = menu_SPDZ_SZ;
		entry_SPDZ = UPDATE;
	}
}

void Menu::menu_down_SPDZ()
{
	if (curItem->id == idSpdZ) {
		menu_cnt_SPDZ++;
		if (menu_cnt_SPDZ > menu_SPDZ_SZ)
		menu_cnt_SPDZ = 1;
		entry_SPDZ = UPDATE;
	}
}

void Menu::menu_enter_SPDZ()
{
	if (curItem->id == idSpdZ) {
		//==Подача G1
		if (menu_cnt_SPDZ == 1) {
			SET_C(idG1);
			entry_SPDZ = INIT;
			return;
		}
		//==
		//==Ускоренный ход G0
		if (menu_cnt_SPDZ == 2) {
			SET_C(idG0);
			entry_SPDZ = INIT;
			return;
		}
		//==
		//==Выход в PM
		if (menu_cnt_SPDZ == 3) {
			if(flash_prg.ch == SW_AXIS0)
				return;
			SET_C(idVixodPM);
			entry_SPDZ = INIT;
			return;
		}
		//==
		//==Выборка Люфта
		if (menu_cnt_SPDZ == 4) {
			if(flash_prg.ch == SW_AXIS0)
				return;
			SET_C(idViborkaLuft);
			entry_SPDZ = INIT;
			return;
		}
		//==
	}
}

void Menu::menu_stop_SPDZ()
{
	if (curItem->id == idSpdZ) {
		#if (configUSE_Shadow_axis == 1)
		flash_prg.write_eeprom(1116,sizeof(flash_prg._Allp_param[SW_AXIS0]),&flash_prg._Allp_param[SW_AXIS0]);
		#endif
		SET_C(idChpa);
		entry_SPDZ = INIT;
	}
}

void Menu::actions_CNTRL(const unsigned char& page)
{
	if (entry_CNTRL == INIT)
	init_menu_CNTRL(page);
	if (entry_CNTRL == UPDATE) {
		draw_menu_CNTRL(page);
		draw_param_CNTRL(page, 25, LEFT_ALIGN);
	}
	if (entry_CNTRL == IDLE)
	return;
}

void Menu::actions_CNTRL_input_pr(const unsigned char& page)
{
	int x_, y_;
	x_ = 15;
	y_ = menu_cnt_CNTRL * 16;

	flashing_cursor(x_, y_, page);
	if (menu.ResultForInputNum == input::ok) {
		int ch = flash_prg.ch;
		long tmp = 0;
		if (!display.strnlen_lir((const char*)bufASCIIstr))
		goto skip_pr;

		if (menu_cnt_CNTRL == 2) {
			//==Допуск==
			flash_prg._Allp_param2[ch].dopusk = String_to_bin(bufASCIIstr, ch);
			if(flash_prg._Allp_param2[ch].dopusk < 0)
				flash_prg._Allp_param2[ch].dopusk = -flash_prg._Allp_param2[ch].dopusk;
			//==
		}
		else if (menu_cnt_CNTRL == 3) {
			//==Ограничение +==
			tmp = String_to_bin(bufASCIIstr, ch);
			if(!tmp || tmp > flash_prg._Allp_param2[ch].limit_minus)
				flash_prg._Allp_param2[ch].limit_plus = tmp;
			//==
		}
		else if (menu_cnt_CNTRL == 4) {
			//==Ограничение -==
			tmp = String_to_bin(bufASCIIstr, ch);
			if(!tmp || tmp < flash_prg._Allp_param2[ch].limit_plus){
				tmp = circle_endless_inout(tmp,ch,1);
				flash_prg._Allp_param2[ch].limit_minus = tmp;
				}
			//==
		}
		skip_pr:
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idCntrl);
		entry_CNTRL = UPDATE;
		init_cursor_pos();
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idCntrl);
		entry_CNTRL = UPDATE;
		init_cursor_pos();
	}
}

void Menu::init_menu_CNTRL(const unsigned char& page)
{
	display.clear_ram_null();
	show_axis_above_menu2(13,0,0);
	entry_CNTRL = UPDATE;
}

void Menu::draw_menu_CNTRL(const unsigned char& page)
{
	const char* menu_table[10];
	menu_table[0] = PSTR(" ПАРАМЕТРЫ ОСИ");
	menu_table[1] = PSTR(" Направление реф.");
	menu_table[2] = PSTR(" Допуск");
	menu_table[3] = PSTR(" Ограничение +");
	menu_table[4] = PSTR(" Ограничение -");
	menu_table[5] = PSTR(" Доводка");
	menu_table[6] = PSTR(" Выборка люфта");
    menu_table[7] = PSTR(" Задержка на Пуск");
	menu_table[8] = PSTR(" В позиции");
	menu_table[9] = nullptr;
	menu_CNTRL_SZ = 8;

	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_CNTRL,page);
	//==
	entry_CNTRL = IDLE;
}

void Menu::draw_param_CNTRL(const unsigned char& page, int x, char align)
{
	int ch = flash_prg.ch;
	const char *p = nullptr;
	//==Направление реф
	if (flash_prg._Allp_param2[ch].ref_dir == 1)
		p = PSTR("+    ");
	else if(flash_prg._Allp_param2[ch].ref_dir == 0)
		p = PSTR("-    ");
	else if(flash_prg._Allp_param2[ch].ref_dir >= 2)
		p = PSTR("выбор");
	display.print_flash_local(p, page, x, 16 * 1, align);
	//==

	//==Допуск
	Lir.print_long_format(flash_prg._Allp_param2[ch].dopusk, flash_prg.axis_cfg[ch].format, page, x, 16 * 2, align, 0, 15);
	//==

	//==Ограничение +
	Lir.print_long_format(flash_prg._Allp_param2[ch].limit_plus, flash_prg.axis_cfg[ch].format, page, x, 16 * 3, align, 0, 15);
	//==

	//==Ограничение -
	long tmp = flash_prg._Allp_param2[ch].limit_minus;
	tmp = circle_endless_inout(tmp,ch,0);
	Lir.print_long_format(tmp, flash_prg.axis_cfg[ch].format, page, x, 16 * 4, align, 0, 15);
	//==

	//==Доводка
	if (!flash_prg._Allp_param2[ch].dovodka)
		p = sYES;
	else
		p = sNO;
	display.print_flash_local(p, page, x, 16 * 5, align);
	//==

	//==Выборка люфта
	if (!flash_prg._Allp_param2[ch].viborka_lufta)
		p = sYES;
	else
		p = sNO;
	display.print_flash_local(p, page, x, 16 * 6, align);
	//==

	//==Задержка на пуск
	if(flash_prg._Allp_param2[ch].delay_VIBOR_PUSK > 0){
		Lir.print_long_format(flash_prg._Allp_param2[ch].delay_VIBOR_PUSK, CPrg::FORMAT::LINEAR_3, page, x, 16 * 7, align, 0, 3,1);
		display.print_flash_local(PSTR("сек"), page, x + 5, 16 * 7, align);
	}
	else{
		display.print_flash_local(sNO, page, x, 16 * 7, align);
		display.print_flash_local(PSTR("   "), page, x + 5, 16 * 7, align);
	}
	//==
	//== В позиции
	if(flash_prg._Allp_param2[ch].v_Pozition > 0){
		Lir.print_long_format(flash_prg._Allp_param2[ch].v_Pozition/10, CPrg::FORMAT::LINEAR_2, page, x, 16 * 8, align, 0, 3);
		display.print_flash_local(PSTR("сек"), page, x + 5, 16 * 8, align);
	}
	else{
		flash_prg._Allp_param2[ch].v_Pozition = 50;
		Lir.print_long_format(flash_prg._Allp_param2[ch].v_Pozition/10, CPrg::FORMAT::LINEAR_2, page, x, 16 * 8, align, 0, 3);
		display.print_flash_local(PSTR("сек"), page, x + 5, 16 * 8, align);
		}
	//==
}

void Menu::menu_up_CNTRL()
{
	if (curItem->id == idCntrl) {
		menu_cnt_CNTRL--;		
		if (menu_cnt_CNTRL < 1)
		menu_cnt_CNTRL = menu_CNTRL_SZ;
		//skip_menu_item(0,menu_cnt_CNTRL,6);
		entry_CNTRL = UPDATE;
	}
}

void Menu::menu_down_CNTRL()
{
	if (curItem->id == idCntrl) {
		menu_cnt_CNTRL++;
		//skip_menu_item(1,menu_cnt_CNTRL,6);
		if (menu_cnt_CNTRL > menu_CNTRL_SZ)
		menu_cnt_CNTRL = 1;
		entry_CNTRL = UPDATE;
	}
}

void Menu::menu_enter_CNTRL()
{
	if (curItem->id == idCntrl) {
		int ch = flash_prg.ch;
		if (menu_cnt_CNTRL == 2) {
			init_cursor_pos();
			CurFormatM = flash_prg.axis_cfg[ch].format;
			SET_C(idCntrlInput);
			return;
		}
		if (menu_cnt_CNTRL == 3) {
			init_cursor_pos();
			CurFormatM = flash_prg.axis_cfg[ch].format;
			SET_C(idCntrlInput);
			return;
		}
		if (menu_cnt_CNTRL == 4) {
			init_cursor_pos();
			CurFormatM = flash_prg.axis_cfg[ch].format;
			SET_C(idCntrlInput);
			return;
		}
	}
}

void Menu::menu_left_CNTRL()
{
	if (curItem->id == idCntrl) {
		int ch = flash_prg.ch;
		if (menu_cnt_CNTRL == 1) {
			flash_prg._Allp_param2[ch].ref_dir++;
			if(flash_prg._Allp_param2[ch].ref_dir > 2)
				flash_prg._Allp_param2[ch].ref_dir = 0;
			entry_CNTRL = UPDATE;
			return;
		}
		if (menu_cnt_CNTRL == 5) {
			flash_prg._Allp_param2[ch].dovodka ^= 1;
			entry_CNTRL = UPDATE;
			return;
		}
		if (menu_cnt_CNTRL == 6) {
			flash_prg._Allp_param2[ch].viborka_lufta ^= 1;
			entry_CNTRL = UPDATE;
			return;
		}
		if (menu_cnt_CNTRL == 7) {
			if(flash_prg._Allp_param2[ch].delay_VIBOR_PUSK > 0)
				flash_prg._Allp_param2[ch].delay_VIBOR_PUSK -= 100; 
			else
				flash_prg._Allp_param2[ch].delay_VIBOR_PUSK = 900; // 0.9 сек
			entry_CNTRL = UPDATE;
			return;
		}
		if (menu_cnt_CNTRL == 8) {
			if(flash_prg._Allp_param2[ch].v_Pozition >= 100)
				flash_prg._Allp_param2[ch].v_Pozition -= 50;
			else
				flash_prg._Allp_param2[ch].v_Pozition = 1000; // 0.9 сек
			entry_CNTRL = UPDATE;
			return;
		}
	}
}

void Menu::menu_right_CNTRL()
{
	if (curItem->id == idCntrl) {
		int ch = flash_prg.ch;
		if (menu_cnt_CNTRL == 1) {
			flash_prg._Allp_param2[ch].ref_dir++;
			if(flash_prg._Allp_param2[ch].ref_dir > 2)
			flash_prg._Allp_param2[ch].ref_dir = 0;
			entry_CNTRL = UPDATE;
			return;
		}
		if (menu_cnt_CNTRL == 5) {
			flash_prg._Allp_param2[ch].dovodka ^= 1;
			entry_CNTRL = UPDATE;
			return;
		}
		if (menu_cnt_CNTRL == 6) {
			flash_prg._Allp_param2[ch].viborka_lufta ^= 1;
			entry_CNTRL = UPDATE;
			return;
		}
		if (menu_cnt_CNTRL == 7) {
			if(flash_prg._Allp_param2[ch].delay_VIBOR_PUSK > 0){
				if(flash_prg._Allp_param2[ch].delay_VIBOR_PUSK < 900)
					flash_prg._Allp_param2[ch].delay_VIBOR_PUSK += 100;
				else
					flash_prg._Allp_param2[ch].delay_VIBOR_PUSK = 0; // нет
			}
			else
				flash_prg._Allp_param2[ch].delay_VIBOR_PUSK = 100; 
			entry_CNTRL = UPDATE;
			return;
		}
		if (menu_cnt_CNTRL == 8) {
			if(flash_prg._Allp_param2[ch].v_Pozition > 0){
				if(flash_prg._Allp_param2[ch].v_Pozition < 1000)
					flash_prg._Allp_param2[ch].v_Pozition += 50;
				else
					flash_prg._Allp_param2[ch].v_Pozition = 50; // нет
			}
			else
				flash_prg._Allp_param2[ch].v_Pozition = 100;
			entry_CNTRL = UPDATE;
			return;
		}
	}
}

void Menu::menu_stop_CNTRL()
{
	if (curItem->id == idCntrl) {
		//сброс установок программных ограничений
		uint8_t c = flash_prg.ch;
		trg_p[c] = 0;
		trg_m[c] = 0;
		ch_sw_prog.plus &= ~plb2(c);
		ch_sw_prog.minus &= ~plb2(c);
		//save param
		flash_prg.write_eeprom(316,sizeof(flash_prg._Allp_param2),&flash_prg._Allp_param2);// Параметры оси - Управление
		SET_C(idChpa);
		entry_CNTRL = INIT;
	}
}

/*
long tmp - данные
int c - канал
bool in  - 1 - функция для преобразования из -350 в -10 , для ввода параметров 0 - -10 в -350 , для вывода на экран в параметрах
*/
long Menu::circle_endless_inout(long tmp,int c,bool in)
{
	long max_value;
	if(flash_prg.axis_cfg[c].type == CPrg::LINEAR)
		return tmp;

	max_value = get_max_circle(c);
	//выходим если ограничение больше либо равно нулю
	if(tmp >= 0)
		return tmp;
	//==перенос минуса на максимальное значение
	max_value *=-1;
	tmp *=-1;
	//==
	if(in) // если ввод
		return max_value + tmp;
	else   // если вывод
		return tmp + max_value;
	//==
}

void Menu::actions_ZNBRK(const unsigned char& page)
{
	if (entry_ZNBRK == INIT)
	init_menu_ZNBRK(page);
	if (entry_ZNBRK == UPDATE) {
		draw_menu_ZNBRK(page);
		draw_param_ZNBRK(page, 25, LEFT_ALIGN);
	}
	if (entry_ZNBRK == IDLE)
	return;
}

void Menu::actions_ZNBRK_input_pr(const unsigned char& page)
{
	int x_, y_;
	x_ = 15;
	y_ = menu_cnt_ZNBRK * 16;

	flashing_cursor(x_, y_, page);
	if (menu.ResultForInputNum == input::ok) {
		int ch = flash_prg.ch;
		long tmp = 0;
		if (!display.strnlen_lir((const char*)bufASCIIstr))
		goto skip_pr;
		tmp = String_to_bin(bufASCIIstr, ch);
		//coeff.div(ch,&tmp);
		flash_prg._Allp_param3[ch].zone[menu_cnt_ZNBRK - 1] = tmp;
		skip_pr:
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idZoneBrake);
		entry_ZNBRK = UPDATE;
		init_cursor_pos();
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idZoneBrake);
		entry_ZNBRK = UPDATE;
		init_cursor_pos();
	}
}

void Menu::init_menu_ZNBRK(const unsigned char& page)
{
	display.clear_ram_null();
	show_axis_above_menu2(19,1);
	entry_ZNBRK = UPDATE;
}

void Menu::draw_menu_ZNBRK(const unsigned char& page)
{
	const char* menu_table[7];

	menu_table[0] = PSTR(" ЗОНЫ ТОРМОЖЕНИЯ ОСИ");
	menu_table[1] = &sZones[1][1];//PSTR(" Зона 01");
	menu_table[2] = &sZones[2][1];//PSTR(" Зона 02");
	menu_table[3] = &sZones[3][1];//PSTR(" Зона 03");
	menu_table[4] = &sZones[4][1];//PSTR(" Зона 04");
	menu_table[5] = PSTR(" Стоп   ");
	menu_table[6] = nullptr;

	menu_ZNBRK_SZ = 5;

	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_ZNBRK,page);
	//==
	entry_ZNBRK = IDLE;
}

void Menu::draw_param_ZNBRK(const unsigned char& page, int x, char align)
{
	int ch = flash_prg.ch;
	for(int s = 0 ; s < ZONES;s++){
		Lir.print_long_format(flash_prg._Allp_param3[ch].zone[s], flash_prg.axis_cfg[ch].format, page, x, 16 * (1 + s), align, 0, 15);
	}
}

void Menu::menu_up_ZNBRK()
{
	if (curItem->id == idZoneBrake) {
		menu_cnt_ZNBRK--;
		if (menu_cnt_ZNBRK < 1)
			menu_cnt_ZNBRK = menu_ZNBRK_SZ;
		entry_ZNBRK = UPDATE;
	}
}

void Menu::menu_down_ZNBRK()
{
	if (curItem->id == idZoneBrake) {
		menu_cnt_ZNBRK++;
		if (menu_cnt_ZNBRK > menu_ZNBRK_SZ)
			menu_cnt_ZNBRK = 1;
		entry_ZNBRK = UPDATE;
	}
}

void Menu::menu_enter_ZNBRK()
{
	if (curItem->id == idZoneBrake) {
		int ch = flash_prg.ch;
		if (menu_cnt_ZNBRK > 0 && menu_cnt_ZNBRK <  6) {
			init_cursor_pos();
			CurFormatM = flash_prg.axis_cfg[ch].format;
			SET_C(idZoneBrakeInput);
			return;
		}
	}
}

void Menu::menu_stop_ZNBRK()
{
	if (curItem->id == idZoneBrake) {
		//save param
		//сохранить в память EEPROM
		flash_prg.write_eeprom(764,sizeof(flash_prg._Allp_param3),&flash_prg._Allp_param3);// Зоны торможения
		SET_C(idChpa);
		entry_ZNBRK = INIT;
	}
}

void Menu::actions_INAXI(const unsigned char& page)
{
	if (entry_INAXI == INIT)
	init_menu_INAXI(page);
	if (entry_INAXI == UPDATE) {
		draw_menu_INAXI(page);
		draw_param_INAXI(page, 25, LEFT_ALIGN);
	}
	if (entry_INAXI == IDLE)
	return;
}

void Menu::init_menu_INAXI(const unsigned char& page)
{
	display.clear_ram_null();
	show_axis_above_menu2(23, 1);
	entry_INAXI = UPDATE;
}

void Menu::draw_menu_INAXI(const unsigned char& page)
{
	const char* menu_table[11];

	menu_table[0] = PSTR(" КОНФИГУРАЦИЯ ВХОДОВ ОСИ");
	menu_table[1] = PSTR(" Ось на тормозе");
	menu_table[2] = PSTR(" Выбор оси");
	menu_table[3] = PSTR(" Отмена выбора оси");
	//menu_table[4] = PSTR(" Внешнее обнуление");
	menu_table[4] = PSTR(" Подача пуск+");
	menu_table[5] = PSTR(" Подача пуск-");
	menu_table[6] = PSTR(" Толчок+");
	menu_table[7] = PSTR(" Толчок-");
	menu_table[8] = PSTR(" Ограничитель+");
	menu_table[9] = PSTR(" Ограничитель-");
	//menu_table[11] = PSTR(" Зона референтной метки");
	menu_table[10] = nullptr;

	menu_INAXI_SZ = 9;

	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_INAXI,page);
	//==
	entry_INAXI = IDLE;
}

void Menu::draw_param_INAXI(const unsigned char& page, int x, char align)
{
	int ch = flash_prg.ch;
	char* p;
	p = &flash_prg._INAxis[ch].axis_on_brake;
	for (int i = 0; i < menu_INAXI_SZ; i++) {
		flash_prg.show_P(*p, IN_P, x, 16 * (i + 1), align);
		p++;
	}
}

void Menu::menu_up_INAXI()
{
	if (curItem->id == idInAxis) {
		menu_cnt_INAXI--;
		if (menu_cnt_INAXI <= 0) {
			menu_cnt_INAXI = menu_INAXI_SZ;
		}
		entry_INAXI = UPDATE;
	}
}

void Menu::menu_down_INAXI()
{
	if (curItem->id == idInAxis) {
		menu_cnt_INAXI++;
		if (menu_cnt_INAXI > menu_INAXI_SZ) {
			menu_cnt_INAXI = 1;
		}
		entry_INAXI = UPDATE;
	}
}

void Menu::menu_left_INAXI()
{
	if (curItem->id == idInAxis) {
		int ch = flash_prg.ch;
		char* p; 
		p = &flash_prg._INAxis[ch].axis_on_brake;

		p = p + (menu_cnt_INAXI - 1);
		flash_prg.changeP(*p, IN_P, 0);
		entry_INAXI = UPDATE;
	}
}

void Menu::menu_right_INAXI()
{
	if (curItem->id == idInAxis) {
		int ch = flash_prg.ch;
		char* p;
		p = &flash_prg._INAxis[ch].axis_on_brake;
		p = p + (menu_cnt_INAXI - 1);
		flash_prg.changeP(*p, IN_P, 1);
		entry_INAXI = UPDATE;
	}
}

void Menu::menu_F_INAXI()
{
	if (curItem->id == idInAxis) {
		int ch = flash_prg.ch;
		char* p;
		p = &flash_prg._INAxis[ch].axis_on_brake;

		p = p + (menu_cnt_INAXI - 1);
		flash_prg.change_MP(*p,IN_P);
		entry_INAXI = UPDATE;
	}
}

void Menu::menu_enter_INAXI()
{
	if (curItem->id == idInAxis) {
		int ch = flash_prg.ch;
		char* p;
		p = &flash_prg._INAxis[ch].axis_on_brake;

		p = p + (menu_cnt_INAXI - 1);
		flash_prg.change_inv_P(*p);
		entry_INAXI = UPDATE;
	}
}

void Menu::menu_stop_INAXI()
{
	if (curItem->id == idInAxis) {
		//save parametrs
		//save_Allp_param3_to_eeprom();
		flash_prg.write_eeprom(444,sizeof(flash_prg._INAxis[0]) * N_AXIS,&flash_prg._INAxis);// Входы осевые
		#if (configUSE_Shadow_axis == 1)
		flash_prg.write_eeprom(1052,sizeof(flash_prg._INAxis[SW_AXIS0]),&flash_prg._INAxis[SW_AXIS0]);
		#endif
		entry_INAXI = INIT;
		SET_C(idChpa);
	}
}

void Menu::actions_OUTAXI(const unsigned char& page)
{
	if (entry_OUTAXI == INIT)
	init_menu_OUTAXI(page);
	if (entry_OUTAXI == UPDATE) {
		draw_menu_OUTAXI(page);
		draw_param_OUTAXI(page, 25, LEFT_ALIGN);
	}
	if (entry_OUTAXI == IDLE)
	return;
}

void Menu::init_menu_OUTAXI(const unsigned char& page)
{
	display.clear_ram_null();
	show_axis_above_menu2(24, 1);
	entry_OUTAXI = UPDATE;
}

void Menu::draw_menu_OUTAXI(const unsigned char& page)
{
	const char* menu_table[12];

	menu_table[0] = PSTR(" КОНФИГУРАЦИЯ ВЫХОДОВ ОСИ");
	menu_table[1] = PSTR(" Выбор оси");
	menu_table[2] = PSTR(" Пуск+");
	menu_table[3] = PSTR(" Пуск-");
	menu_table[4] = PSTR(" Пуск");
	menu_table[5] = PSTR(" Смазка");
	menu_table[6] = sLine;
	menu_table[7] = PSTR(" Тип задания");//menu_table[7] = PSTR(" Выход A");
	menu_table[8] = nullptr;//menu_table[8] = PSTR(" Выход B");
	//menu_table[9] = PSTR(" Выход C");
	//menu_table[10] = PSTR(" Выход D");
	//menu_table[11] = nullptr;

	menu_OUTAXI_SZ = 7;

	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_OUTAXI,page);
	//==
	if(menu_cnt_OUTAXI == 7 || (menu_cnt_OUTAXI == 5  && !(flash_prg._OUTAxis[flash_prg.ch].Smazka & YESNO_P_bm)))
		display.print_flash_local(PSTR(" <ВВОД> настройка"),page,0,16 * 14);
	else
		display.print_symbols_local(' ',20,0,0,16 * 14);

	entry_OUTAXI = IDLE;
}

void Menu::draw_param_OUTAXI(const unsigned char& page, int x, char align)
{
	int ch = flash_prg.ch;
	char* p;
	const char *p_s = nullptr;
		p = &flash_prg._OUTAxis[ch].vibor_axis;
	for (int i = 0; i < menu_OUTAXI_SZ; i++) {
		if (i >= 5) continue;//skip line
		flash_prg.show_P(*p, OUT_P, x, 16 * (i + 1), align);
		p++;
	}

	//==Тип задания
	if(flash_prg._OUTAxis[ch].typeZ == CPrg::ANALOG)
		p_s = PSTR("аналоговый");
	else if(flash_prg._OUTAxis[ch].typeZ == CPrg::DISCRET)
		p_s = PSTR("дискретный");
	else if(flash_prg._OUTAxis[ch].typeZ == CPrg::IMPULSE)
		p_s = PSTR("импульсный");
	else if(flash_prg._OUTAxis[ch].typeZ == CPrg::MODBUS)
		p_s = PSTR("modbus    ");

	display.print_flash_local(p_s,page,x,16 * 7);
	//==
}

void Menu::menu_up_OUTAXI()
{
	if (curItem->id == idOutAxis) {
		menu_cnt_OUTAXI--;
		skip_menu_item(0,menu_cnt_OUTAXI,6);
		//skip_menu_item(0,menu_cnt_OUTAXI,5);				
		if (menu_cnt_OUTAXI <= 0) {
			menu_cnt_OUTAXI = menu_OUTAXI_SZ;
		}
		entry_OUTAXI = UPDATE;
	}
}

void Menu::menu_down_OUTAXI()
{
	if (curItem->id == idOutAxis) {
		menu_cnt_OUTAXI++;
		//skip_menu_item(1,menu_cnt_OUTAXI,5);
		skip_menu_item(1,menu_cnt_OUTAXI,6);
		if (menu_cnt_OUTAXI > menu_OUTAXI_SZ) {
			menu_cnt_OUTAXI = 1;
		}
		entry_OUTAXI = UPDATE;
	}
}

void Menu::menu_left_OUTAXI()
{
	if (curItem->id == idOutAxis) {
		int ch = flash_prg.ch;
		if(menu_cnt_OUTAXI >= 1 && menu_cnt_OUTAXI <=5){
			char* p;
			p = &flash_prg._OUTAxis[ch].vibor_axis;
			p = p + (menu_cnt_OUTAXI - 1);
			if (menu_cnt_OUTAXI > 5)
				p--;
			flash_prg.changeP(*p, OUT_P, 0,1);
		}
		else if(menu_cnt_OUTAXI == 7){ // Тип задания
			flash_prg._OUTAxis[ch].typeZ--;
			if(flash_prg._OUTAxis[ch].typeZ < -1){
				flash_prg._OUTAxis[ch].typeZ = CPrg::ANALOG;
				#if (configUSE_mcx514 == 1)
					flash_prg._OUTAxis[ch].typeZ = CPrg::IMPULSE;
				#endif
				#if (configUSE_ModBus_DRV == 1)
					flash_prg._OUTAxis[ch].typeZ = CPrg::MODBUS;
				#endif
			}
		}
		entry_OUTAXI = UPDATE;
	}
}

void Menu::menu_right_OUTAXI()
{
	if (curItem->id == idOutAxis) {
		int ch = flash_prg.ch;
		if(menu_cnt_OUTAXI >= 1 && menu_cnt_OUTAXI <=5){
			char* p;
			p = &flash_prg._OUTAxis[ch].vibor_axis;
			p = p + (menu_cnt_OUTAXI - 1);
			if (menu_cnt_OUTAXI > 5)
				p--;
			flash_prg.changeP(*p, OUT_P, 1,1);
		}
		else if(menu_cnt_OUTAXI == 7){ // Тип задания
			flash_prg._OUTAxis[ch].typeZ++;
			CPrg::TYPEZ tmp = CPrg::ANALOG;
			#if (configUSE_mcx514 == 1)
				tmp = CPrg::IMPULSE;
			#endif
			#if (configUSE_ModBus_DRV == 1)
				tmp = CPrg::MODBUS;
			#endif
			if(flash_prg._OUTAxis[ch].typeZ > tmp)
				flash_prg._OUTAxis[ch].typeZ = CPrg::DISCRET;
			}
		entry_OUTAXI = UPDATE;
	}
}

void Menu::menu_F_OUTAXI()
{
	if (curItem->id == idOutAxis) {
		int ch = flash_prg.ch;
		if(menu_cnt_OUTAXI >= 1 && menu_cnt_OUTAXI <=5){
			char* p;
			p = &flash_prg._OUTAxis[ch].vibor_axis;
			p = p + (menu_cnt_OUTAXI - 1);
			if (menu_cnt_OUTAXI > 5)
				p--;
			flash_prg.change_MP(*p,OUT_P);
			entry_OUTAXI = UPDATE;
		}
	}
}

void Menu::menu_enter_OUTAXI()
{
	if (curItem->id == idOutAxis) {
		int ch = flash_prg.ch;
		if(menu_cnt_OUTAXI == 5 && !(flash_prg._OUTAxis[ch].Smazka & YESNO_P_bm)){
			if(ch < N_AXIS){ // только для осей с датчиком движения
				SET_C(idLubric);
				entry_OUTAXI = INIT;
			}			
		}
		if(menu_cnt_OUTAXI == 7){ // Тип задания
			if(flash_prg._OUTAxis[ch].typeZ == CPrg::DISCRET)
				SET_C(idDiscZ);
			else if(flash_prg._OUTAxis[ch].typeZ == CPrg::ANALOG)
				SET_C(idAnalZ);
			else if(flash_prg._OUTAxis[ch].typeZ == CPrg::IMPULSE)
				SET_C(idCntrlopt);
			#if (configUSE_ModBus_DRV == 1)
			else if(flash_prg._OUTAxis[ch].typeZ == CPrg::MODBUS)
				SET_C(idModbZ);
			#endif
			entry_OUTAXI = INIT;
		}
	}  
}

void Menu::menu_stop_OUTAXI()
{
	if (curItem->id == idOutAxis) {
		//save parametrs
		flash_prg.write_eeprom(572,sizeof(flash_prg._OUTAxis[0]) * N_AXIS,&flash_prg._OUTAxis);// Выходы осевые
		#if (configUSE_Shadow_axis == 1)
		flash_prg.write_eeprom(1084,sizeof(flash_prg._OUTAxis[SW_AXIS0]),&flash_prg._OUTAxis[SW_AXIS0]);
		#endif
		entry_OUTAXI = INIT;
		SET_C(idChpa);
	}
}

void Menu::actions_LUBR(const unsigned char& page)
{
	if (entry_LUBR == INIT)
		init_menu_LUBR(page);
	if (entry_LUBR == UPDATE) {
		draw_menu_LUBR(page);
		draw_param_LUBR(page, 25, LEFT_ALIGN);
	}
	if (entry_LUBR == IDLE)
	return;
}

void Menu::actions_LUBR_input(const unsigned char& page)
{
	int x_, y_;
	
	x_ = 9;
	y_ = 1 * 16;

	flashing_cursor(x_, y_, page);
	if (menu.ResultForInputNum == input::ok) {
		long tmp = 0;
		int f = iForm(flash_prg.ch);//flash_prg.axis_cfg[flash_prg.ch].format;
		if (!display.strnlen_lir((const char*)bufASCIIstr))
			goto skip_pr;
		if(f == CPrg::FORMAT::CIRCLE_SECUNDO || f == CPrg::FORMAT::CIRCLE_MINUTO)
			ConvertASCIIcircle_secundo_tolong_new(tmp,f,bufASCIIstr);
		else
			ConvertASCIIlinear_circle_tolong(tmp,f,bufASCIIstr);
		if(tmp < 0)
			tmp = -tmp;
		flash_prg._OUTAxis[flash_prg.ch].dist_lubr = tmp;
		skip_pr:
		display.print_flash_local(sProbel10, page, x_, y_);
		init_cursor_pos();
		SET_C(idLubric);
		entry_LUBR = UPDATE;	
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel10, page, x_, y_);
		init_cursor_pos();
		SET_C(idLubric);
		entry_LUBR = UPDATE;
	}
}

void Menu::init_menu_LUBR(const unsigned char& page)
{
	display.clear_ram_null();
	show_axis_above_menu2(12, 1);
	entry_LUBR = UPDATE;
}

void Menu::draw_menu_LUBR(const unsigned char& page)
{
	const char* menu_table[4];

	menu_table[0] = PSTR(" СМАЗКА ОСИ");
	menu_table[1] = PSTR(" Период");
	menu_table[2] = PSTR(" Длительность, сек");
	menu_table[3] = nullptr;

	menu_LUBR_SZ = 2;
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_LUBR,page);
	//==
	entry_LUBR = IDLE;
}

void Menu::draw_param_LUBR(const unsigned char& page, int x, char align)
{
	//==Период смазки
	print_long_format(flash_prg._OUTAxis[flash_prg.ch].dist_lubr,iForm(flash_prg.ch),page,x,16 * 1,align,0,11);
	//==

	//==Длительность
	print_long_format(flash_prg._OUTAxis[flash_prg.ch].time_lubr,CPrg::FORMAT_END,page,x,16 * 2,align,0,11);
	//==
}

void Menu::menu_up_LUBR()
{
	if(curItem->id == idLubric){
		menu_cnt_LUBR--;
		if(menu_cnt_LUBR < 1)
			menu_cnt_LUBR = menu_LUBR_SZ;
		entry_LUBR = UPDATE;
	}
}

void Menu::menu_down_LUBR()
{
	if(curItem->id == idLubric){
		menu_cnt_LUBR++;
		if(menu_cnt_LUBR > menu_LUBR_SZ)
			menu_cnt_LUBR = 1;
		entry_LUBR = UPDATE;
	}
}

void Menu::menu_left_LUBR()
{
	if(curItem->id == idLubric){
		if(menu_cnt_LUBR == 2) { //длительность
			flash_prg._OUTAxis[flash_prg.ch].time_lubr--;
			if(flash_prg._OUTAxis[flash_prg.ch].time_lubr < 1)
				flash_prg._OUTAxis[flash_prg.ch].time_lubr = 10;
		}
		entry_LUBR = UPDATE;
	}
}

void Menu::menu_right_LUBR()
{
	if(curItem->id == idLubric){
		if(menu_cnt_LUBR == 2) { //длительность
			flash_prg._OUTAxis[flash_prg.ch].time_lubr++;
			if(flash_prg._OUTAxis[flash_prg.ch].time_lubr > 10)
				flash_prg._OUTAxis[flash_prg.ch].time_lubr = 1;
		}
		entry_LUBR = UPDATE;
	}
}

void Menu::menu_enter_LUBR()
{
	if(curItem->id == idLubric){
		if(menu_cnt_LUBR == 1){
			CurFormatM = flash_prg.axis_cfg[flash_prg.ch].format;
			SET_C(idLubricInput);
		}		
	}
}

void Menu::menu_stop_LUBR()
{
	if(curItem->id == idLubric){
		SET_C(idOutAxis);
		entry_LUBR = INIT;
	}
}

void Menu::actions_DISCZ(const unsigned char& page)
{
	if (entry_DISCZ == INIT)
		init_menu_DISCZ(page);
	if (entry_DISCZ == UPDATE) {
		draw_menu_DISCZ(page);
		draw_param_DISCZ(page, 25, LEFT_ALIGN);
	}
	if (entry_DISCZ == IDLE)
		return;
}

void Menu::init_menu_DISCZ(const unsigned char& page)
{
	display.clear_ram_null();
	show_axis_above_menu2(24, 1);
	entry_DISCZ = UPDATE;
}

void Menu::draw_menu_DISCZ(const unsigned char& page)
{
	const char* menu_table[6];

	menu_table[0] = PSTR(" ДИСКРЕТНОЕ ЗАДАНИЕ ОСИ");
	menu_table[1] = PSTR(" Выход A");
	menu_table[2] = PSTR(" Выход B");
	menu_table[3] = PSTR(" Выход C");
	menu_table[4] = PSTR(" Выход D");
	menu_table[5] = nullptr;

	menu_DISCZ_SZ = 4;

	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_DISCZ,page);
	//==
	entry_DISCZ = IDLE;
}

void Menu::draw_param_DISCZ(const unsigned char& page, int x, char align)
{
	int ch = flash_prg.ch;
	char* p;
	p = &flash_prg._OUTAxis[ch].Out_A;
	for (int i = 0; i < menu_DISCZ_SZ; i++) {
		flash_prg.show_P(*p, OUT_P, x, 16 * (i + 1), align);
		p++;
	}
}

void Menu::menu_up_DISCZ()
{
	if (curItem->id == idDiscZ) {
		menu_cnt_DISCZ--;
		if (menu_cnt_DISCZ <= 0) {
			menu_cnt_DISCZ = menu_DISCZ_SZ;
		}
		entry_DISCZ = UPDATE;
	}
}

void Menu::menu_down_DISCZ()
{
	if (curItem->id == idDiscZ) {
		menu_cnt_DISCZ++;
		if (menu_cnt_DISCZ > menu_DISCZ_SZ) {
			menu_cnt_DISCZ = 1;
		}
		entry_DISCZ = UPDATE;
	}
}

void Menu::menu_left_DISCZ()
{
	if (curItem->id == idDiscZ) {
		int ch = flash_prg.ch;
		char* p = &flash_prg._OUTAxis[ch].Out_A;
		p = p + (menu_cnt_DISCZ - 1);
		flash_prg.changeP(*p, OUT_P, 0,1);
		entry_DISCZ = UPDATE;
	}
}

void Menu::menu_right_DISCZ()
{
	if (curItem->id == idDiscZ) {
		int ch = flash_prg.ch;
		char* p = &flash_prg._OUTAxis[ch].Out_A;
		p = p + (menu_cnt_DISCZ - 1);
		flash_prg.changeP(*p, OUT_P, 1,1);
		entry_DISCZ = UPDATE;
	}
}

void Menu::menu_F_DISCZ()
{
	if (curItem->id == idDiscZ) {
		int ch = flash_prg.ch;
		char* p = &flash_prg._OUTAxis[ch].Out_A;
		p = p + (menu_cnt_DISCZ - 1);
		flash_prg.change_MP(*p,OUT_P);
		entry_DISCZ = UPDATE;
	}
}

void Menu::menu_enter_DISCZ()
{
	//инверсия не используется
}

void Menu::menu_stop_DISCZ()
{
	if (curItem->id == idDiscZ) {
		//save parametrs
		flash_prg.write_eeprom(572,sizeof(flash_prg._OUTAxis[0]) * N_AXIS,&flash_prg._OUTAxis);// Выходы осевые
		#if (configUSE_Shadow_axis == 1)
		flash_prg.write_eeprom(1084,sizeof(flash_prg._OUTAxis[SW_AXIS0]),&flash_prg._OUTAxis[SW_AXIS0]);
		#endif
		entry_DISCZ = INIT;
		SET_C(idOutAxis);
	}
}

void Menu::actions_ANALZ(const unsigned char& page)
{
	if (entry_ANALZ == INIT)
		init_menu_ANALZ(page);
	if (entry_ANALZ == UPDATE) {
		draw_menu_ANALZ(page);
		draw_param_ANALZ(page, 28, LEFT_ALIGN);
	}
	if (entry_ANALZ == IDLE)
	return;
}

void Menu::actions_ANALZ_input_pr(const unsigned char& page)
{
	int x_, y_;
	x_ = 20;
	y_ = menu_cnt_ANALZ * 16;

	flashing_cursor(x_, y_, page);
	if (menu.ResultForInputNum == input::ok) {
		int ch = flash_prg.ch;
		long tmp = 0;
		int f = gFi(ch);
		if (!display.strnlen_lir((const char*)bufASCIIstr))		
			goto skip_pr;
		
		if(menu_cnt_ANALZ  == 4 || menu_cnt_ANALZ  == 6)//для напряжения
			f = CPrg::LINEAR_3;

		ConvertASCIIlinear_circle_tolong(tmp,f,bufASCIIstr);
		//coeff.div(ch,&tmp);
		if (menu_cnt_ANALZ == 3) {
			//==Макс скорость G1==
			flash_prg._OUTAxis2[ch].MaxSpdG1 = tmp;
			//==
		}
		else if (menu_cnt_ANALZ == 4) {
			//==Макс напряжение G1==
			flash_prg._OUTAxis2[ch].MaxUG1 = tmp;
			//==
		}
		else if (menu_cnt_ANALZ == 5) {
			//==Макс скорость G0==
			flash_prg._OUTAxis2[ch].MaxSpdG0 = tmp;
			//==
		}
		else if (menu_cnt_ANALZ == 6) {
			//==Макс напряжение G0==
			flash_prg._OUTAxis2[ch].MaxUG0 = tmp;
			//==
		}

		skip_pr:
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idAnalZ);
		entry_ANALZ = UPDATE;
		init_cursor_pos();
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idAnalZ);
		entry_ANALZ = UPDATE;
		init_cursor_pos();
	}
}

void Menu::init_menu_ANALZ(const unsigned char& page)
{
	display.clear_ram_null();
	show_axis_above_menu2(24, 1);
	entry_ANALZ = UPDATE;
}

void Menu::draw_menu_ANALZ(const unsigned char& page)
{
	const char* menu_table[8];

	menu_table[0] = PSTR(" АНАЛОГОВОЕ ЗАДАНИЕ ОСИ");
	menu_table[1] = PSTR(" Аналоговый выход");
	menu_table[2] = PSTR(" Полярность");
	if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::LINEAR)
		menu_table[3] = PSTR(" Макс. скорость G1,мм/мин");
	else
		menu_table[3] = PSTR(" Макс. скорость G1,об/мин");

	menu_table[4] = PSTR(" Макс. напряжение G1,В");

	if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::LINEAR)
		menu_table[5] = PSTR(" Макс. скорость G0,мм/мин");
	else
		menu_table[5] = PSTR(" Макс. скорость G0,об/мин");
	menu_table[6] = PSTR(" Макс. напряжение G0,В");
	menu_table[7] = nullptr;

	menu_ANALZ_SZ = 6;

	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_ANALZ,page);	
	//==
	entry_ANALZ = IDLE;
}

void Menu::draw_param_ANALZ(const unsigned char& page, int x, char align)
{
	int ch = flash_prg.ch;
	const char *p = sError;
	int f = gFi(ch);
		
	//==Аналоговый выход
	if(flash_prg._OUTAxis2[ch].AnalOut == CPrg::NO_AOUT)
			p = sNO;//нет
	else if(flash_prg._OUTAxis2[ch].AnalOut == CPrg::A1)
			p = PSTR(">a1 ");//a1
	else if(flash_prg._OUTAxis2[ch].AnalOut == CPrg::A2)
			p = PSTR(">a2 ");//a2
	display.print_flash_local(p, page, x, 16 * 1, align);
	//==
	p = sError;
	//==Полярность
	if(flash_prg._OUTAxis2[ch].Polar == CPrg::PLUS_pl)
			p = PSTR("+  ");//+
	else if(flash_prg._OUTAxis2[ch].Polar == CPrg::MINUS_pl)
			p = PSTR("-  ");//-
	else if(flash_prg._OUTAxis2[ch].Polar == CPrg::PL_MN_pl)
			p = PSTR("+/-");//+/-
	else if(flash_prg._OUTAxis2[ch].Polar == CPrg::MN_PL_pl)
			p = PSTR("-/+");//-/+
	display.print_flash_local(p, page, x, 16 * 2, align);
	//==

	//==Макс. скорость G1
	print_long_format(flash_prg._OUTAxis2[ch].MaxSpdG1,f,page,x,16 * 3,align,0,10,1);
	//==

	//==Макс. напряжение G1
	print_long_format(flash_prg._OUTAxis2[ch].MaxUG1,CPrg::LINEAR_3,page,x,16 * 4,align,0,10,1);
	//==

	//==Макс. скорость G0
	print_long_format(flash_prg._OUTAxis2[ch].MaxSpdG0,f,page,x,16 * 5,align,0,10,1);
	//==

	//==Макс. напряжение G0
	print_long_format(flash_prg._OUTAxis2[ch].MaxUG0,CPrg::LINEAR_3,page,x,16 * 6,align,0,10,1);
	//==

}

void Menu::menu_up_ANALZ()
{
	if (curItem->id == idAnalZ) {
		menu_cnt_ANALZ--;
		if (menu_cnt_ANALZ <= 0) {
			menu_cnt_ANALZ = menu_ANALZ_SZ;
		}
		entry_ANALZ = UPDATE;
	}
}

void Menu::menu_down_ANALZ()
{
	if (curItem->id == idAnalZ) {
		menu_cnt_ANALZ++;
		if (menu_cnt_ANALZ > menu_ANALZ_SZ) {
			menu_cnt_ANALZ = 1;
		}
		entry_ANALZ = UPDATE;
	}
}

void Menu::menu_left_ANALZ()
{
	if (curItem->id == idAnalZ) {
		int ch = flash_prg.ch;
		if(menu_cnt_ANALZ == 1){
			flash_prg._OUTAxis2[ch].AnalOut--;
			if(flash_prg._OUTAxis2[ch].AnalOut < -1)
				flash_prg._OUTAxis2[ch].AnalOut = CPrg::A2;				
		}
		else if(menu_cnt_ANALZ == 2){
			flash_prg._OUTAxis2[ch].Polar--;
			if(flash_prg._OUTAxis2[ch].Polar < -1)
				flash_prg._OUTAxis2[ch].Polar = CPrg::MN_PL_pl;
		}
		entry_ANALZ = UPDATE;
	}
}

void Menu::menu_right_ANALZ()
{
	if (curItem->id == idAnalZ) {
		int ch = flash_prg.ch;
		if(menu_cnt_ANALZ == 1){
			flash_prg._OUTAxis2[ch].AnalOut++;
			if(flash_prg._OUTAxis2[ch].AnalOut > CPrg::A2)
				flash_prg._OUTAxis2[ch].AnalOut = CPrg::NO_AOUT;
		}
		else if(menu_cnt_ANALZ == 2){
			flash_prg._OUTAxis2[ch].Polar++;
			if(flash_prg._OUTAxis2[ch].Polar > CPrg::MN_PL_pl)
				flash_prg._OUTAxis2[ch].Polar = CPrg::PLUS_pl;
		}
		entry_ANALZ = UPDATE;
	}
}

void Menu::menu_enter_ANALZ()
{
	//для перехода в режим ввода
	if (curItem->id == idAnalZ) {
		if (menu_cnt_ANALZ > 2 && menu_cnt_ANALZ < 7) {
			init_cursor_pos();
			CurFormatM = CPrg::LINEAR_3;
			SET_C(idAnalZInput);
			return;
		}
	}
}

void Menu::menu_stop_ANALZ()
{
	if (curItem->id == idAnalZ) {
		//save parametrs
		flash_prg.write_eeprom(1213,sizeof(flash_prg._OUTAxis2),&flash_prg._OUTAxis2);
		entry_ANALZ = INIT;
		SET_C(idOutAxis);
	}
}

void Menu::actions_MODBZ(const unsigned char& page)
{

}

void Menu::actions_MODBZ_input_pr(const unsigned char& page)
{

}

void Menu::init_menu_MODBZ(const unsigned char& page)
{

}

void Menu::draw_menu_MODBZ(const unsigned char& page)
{

}

void Menu::draw_param_MODBZ(const unsigned char& page, int x, char align)
{

}

void Menu::menu_up_MODBZ()
{
	if(curItem->id == idModbZ){

	}
}

void Menu::menu_down_MODBZ()
{
	if(curItem->id == idModbZ){

	}
}

void Menu::menu_left_MODBZ()
{
	if(curItem->id == idModbZ){

	}
}

void Menu::menu_right_MODBZ()
{
	if(curItem->id == idModbZ){

	}
}

void Menu::menu_enter_MODBZ()
{
	if(curItem->id == idModbZ){

	}
}

void Menu::menu_stop_MODBZ()
{
	if(curItem->id == idModbZ){

	}
}

#endif

void Menu::actions_CORR(const unsigned char & page)
{
	if (entry_CORR == INIT)
	init_menu_CORR(page);
	if (entry_CORR == UPDATE)
	draw_menu_CORR(page);
	if (entry_CORR == IDLE)
	return;
}

void Menu::init_menu_CORR(const unsigned char & page)
{
	display.clear_ram_null();
	entry_CORR = UPDATE;
}

void Menu::draw_menu_CORR(const unsigned char & page)
{
	const char *menu_table[4] = { 0 };
	uint8_t i = 0;
	//==конструктор меню
	for (uint8_t j = 0; j < 3;) {
		menu_table[i++] = sCorr[j++];
	}
	//==
	//==размер меню
	menu_CORR_SZ = i - 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_CORR,page);
	//==
	entry_CORR = IDLE;
}

void Menu::menu_up_CORR()
{
	if (curItem->id == idCorr) {
		menu_cnt_CORR--;
		if (menu_cnt_CORR <= 0)
		menu_cnt_CORR = menu_CORR_SZ;
		entry_CORR = UPDATE;
	}
}

void Menu::menu_down_CORR()
{
	if (curItem->id == idCorr) {
		menu_cnt_CORR++;
		if (menu_cnt_CORR > menu_CORR_SZ)
		menu_cnt_CORR = 1;
		entry_CORR = UPDATE;
	}
}

void Menu::menu_enter_CORR()
{
	if (curItem->id == idCorr) {
		int8_t place = 1;
		entry_CORR = INIT;
		//==Снятие коррекции
		if (menu_cnt_CORR == place++) {
			SET_C(idGetCorr);
			return;
		}
		//==
		//== Работа с таблицами
		if (menu_cnt_CORR == place++) {
			SET_C(idSeeCorr);
			return;
		}
		//==
	}
}

void Menu::menu_stop_CORR()
{
	if (curItem->id == idCorr) {
		entry_CORR = INIT;
		SET_C(idDiag);
	}
}

void Menu::actions_GTCOR(const unsigned char & page)
{
	if (entry_GTCOR == INIT)
		init_menu_GTCOR(page);
	if (entry_GTCOR == UPDATE)
		entry_GTCOR = IDLE;
	if (entry_GTCOR == IDLE)
	return;
}

void Menu::init_menu_GTCOR(const unsigned char & page)
{
	//02.08.2018
	cli();
	flash_prg.to_take_table=true;
	//ref_.init_PA0_int_ref_1(true);
	ref_.follower=CPorta_ref::CORRECTION::SET_MODE_CORRECTION;
	sei();
	//-----------------------------
	
	display.clear_ram_null();
	//display.print_flash_local(PSTR("RefMode"),0,0,0);
	
	display.print_flash_local(PSTR(" Размер интервалов L:"),0,0,0);
	display.print_flash_local(PSTR(" Кол-во интервалов N:"),0,0,16);
	
	//display.print_flash_local(PSTR(" Коэффициент     :"),0,0,32);
	display.print_flash_local(PSTR(" Дискрет в интервале:"),0,0,32);
	//display.print_debug_one_local_with_dot(coeff.new_coeff_0.binary,coeff.new_coeff_0.n_after_dot,0,18,32);
	
	
	display.print_flash_local(PSTR("ЭТАЛОН  XP2.0"),0,10,48);
	char str[8]= {0};
	if(P2[0] != no_axis){
		memcpy_P(str,&sAxis_screen[P2[0]][2],1);
		display.print_ram_local(str,0,24,48);
	}
	else
		display.print_flash_local(PSTR("нет"),0,24,48);

	display.print_flash_local(PSTR(" Счётчик         :"),0,0,64);
	display.print_flash_local(PSTR(" Номер интервала :"),0,0,80);
	display.print_flash_local(PSTR(" Эталон значение :"),0,0,96);
	
	display.print_flash_local(PSTR("РАБОЧИЙ XP2.1"),0,10,112);
	if(P2[1] != no_axis){
		memcpy_P(str,&sAxis_screen[P2[1]][2],1);
		display.print_ram_local(str,0,24,112);
	}
	else
		display.print_flash_local(PSTR("нет"),0,24,112);

	display.print_flash_local(PSTR(" Счётчик         :"),0,0,128);
	display.print_flash_local(PSTR(" Индикация       :"),0,0,144);
	display.print_flash_local(PSTR(" Значение люфта  :"),0,0,160);
	
	
	display.print_flash_local(PSTR(" Скорость за 10 мс  :"),0,0,192);
	display.print_flash_local(PSTR(" Ошибки при работе  :"),0,0,208);
	

	display.put_button_(X_BUTTON_PLACE + 1,Y_ONE,_pic_L_CORR);

	display.put_button_(X_BUTTON_PLACE + 1,Y_TWO,_pic_N_CORR);

	//display.print_flash_local(PSTR(" N"),0,35,128);
	display.put_button_(X_BUTTON_PLACE + 1,Y_THREE);
	display.print_flash_local(sPusk,0,35,126);

	display.put_button_(X_BUTTON_PLACE + 1,Y_FOUR);
	display.print_flash_local(sSTOP,0,35,176);
	entry_GTCOR = IDLE;
}

void Menu::actions_SEECOR(const unsigned char & page)
{
	if (entry_SEECOR == INIT)
		init_menu_SEECOR(page);
	if (entry_SEECOR == UPDATE)
		entry_SEECOR = IDLE;
	if (entry_SEECOR == IDLE)
	return;
}

void Menu::init_menu_SEECOR(const unsigned char & page)
{
	display.clear_ram_null();
	
	//cnt_StrCh = 0;
	Lir.update_table_corr = 1;
	//02.08.2018
	cli();
	ref_.follower=CPorta_ref::CORRECTION::SET_MODE_READOUT;
	flash_prg.to_take_table=false;
	sei();
	Str_Channel[0][1] = '.';
	Str_Channel[1][1] = '.';
	Str_Channel[2][1] = '.';
	Str_Channel[3][1] = '.';

	Str_Channel[0][2] = '0';
	Str_Channel[1][2] = '1';
	Str_Channel[2][2] = '2';
	Str_Channel[3][2] = '3';

	if(P2[0] != no_axis)
		memcpy_P(Str_Channel[0],&sAxis_screen[P2[0]][2],1);
	if(P2[1] != no_axis)
		memcpy_P(Str_Channel[1],&sAxis_screen[P2[1]][2],1);
	if(P2[2] != no_axis)
		memcpy_P(Str_Channel[2],&sAxis_screen[P2[2]][2],1);
	if(P2[3] != no_axis)
		memcpy_P(Str_Channel[3],&sAxis_screen[P2[3]][2],1);

	//35 64
	display.print_ram_local(Str_Channel[cnt_StrCh],0,36,64);
	
	display.put_button_(X_BUTTON_PLACE + 1,Y_ONE);
	display.put_button_(X_BUTTON_PLACE + 1,Y_TWO);
	display.put_button_(X_BUTTON_PLACE + 1,Y_THREE);
	display.put_button_(X_BUTTON_PLACE + 1,Y_FOUR);

	display.print_flash_local(PSTR("СИНХ"), 0, 35, 12);
	display.print_flash_local(PSTR("СЧИТ"), 0, 35, 126);
	display.print_flash_local(PSTR("СОХР"), 0, 35, 178);
	//-----------------------------
	display.print_flash_local(PSTR("РАБОЧИЙ"),0,4,0); //0
	if(flash_prg.t_save)
		display.put_char_local_independent('*',0,16,0);
	display.print_flash_local(PSTR("ЭТАЛОН"),0,17,0); //12
	display.print_flash_local(PSTR("ДЕЛЬТА"),0,27,0); //24
	display.print_flash_local(PSTR("Точки  :"),0,0,16);
	display.print_flash_local(PSTR("Размер интервалов:"),0,0,16*8);
	display.print_flash_local(PSTR("Кол-во интервалов:"),0,0,16*9);
	display.print_flash_local(PSTR("    Рабочий X.1:"),0,0,176);
	display.print_flash_local(PSTR("    Эталон  X.0:"),0,0,192);
	display.print_flash_local(PSTR("    Дельта     :"),0,0,208);
	display.print_flash_local(PSTR("    Коррек. X.1:"),0,0,224);
	//display.print_flash_local(PSTR("Код ошибки     :"),0,0,208);
	entry_SEECOR = IDLE;
}

/*
return 1 - если коррекция работает в таблице,'K' под буквой оси
return 0 - коррекция не работает
*/
bool Menu::is_corr_table(uint8_t c)
{
	#if (configUSE_menuItem_Correction_and_Izmerenia == 0)
		return 0;
	#endif

	if(altera.error_corr_real_time[c] == CIOAltera::ERROR_CORR_REAL_TIME::NO_ERROR)
		return 1;
	else
		return 0;
}
/*
return 1 - если коррекция включена в настройках и есть реф метка и находимся в РУЧНОМ (HAND) или ГЛАВНОМ (BEGIN) экране
return 0 - коррекция не работает
*/
bool Menu::is_corr(uint8_t c)
{
	#if (configUSE_menuItem_Correction_and_Izmerenia == 0)
		return 0;
	#endif

	if(flash_prg.axis_cfg[c].is_correction == CPrg::YES &&
	  ref_.pRef[c] && 
	  (MsgKey.mode == TMsgKey::HAND || MsgKey.mode == TMsgKey::BEGIN  || (MsgKey.mode == TMsgKey::INPUT_PARAM && curItem->id == idHand_input)))
		return 1;
	else
		return 0;
}

void Menu::init_menu_LINPK(const unsigned char & page)
{
	if(!uart_usb.mode_rs_232_usb)
	{
		display.clear_ram_null();
		memset(uart_usb.data,0,272);	
		uart_usb.real_mode_usb();
		timer1MSEKOFF();
	}
}

void Menu::menu_1_LINKPK()
{
#if (configUSE_PLC == 1)
	if(curItem->id == idLinkPK){
		if(uart_usb.follower == CUART_usb::START_LINK_USB){
			uart_usb.SET_U(CUART_usb::BRIDGE_RS485_PK);
			display.print_flash_local(PSTR("мост"),0,21,0);
			uart_485.setSpd115_2kHz();
		}else if(uart_usb.follower == CUART_usb::BRIDGE_RS485_PK){
			uart_usb.SET_U(CUART_usb::START_LINK_USB);
			display.print_flash_local(PSTR("    "),0,21,0);
			uart_485.setSpd250kHz();
		}
	}
#endif
}

void Menu::menu_2_LINKPK()
{
#if (configUSE_PLC == 1)
if(curItem->id == idLinkPK){
	/*static char t;
	if(uart_usb.follower == CUART_usb::BRIDGE_RS485_PK){
		if(t == 0){
			uint8_t buf[] = {0x02,0x06,0x09,0x00,0x00,0x01,0x00,0x00}; //ServoOn
			uint16_t crc_ = crc(buf,sizeof(buf) - 2);
			buf[6] = *(((uint8_t*)&crc_) + 1);
			buf[7] = (uint8_t)crc_;
			init.adm2687_transmitter();
			cli();
			uart_485.TransmitBlock(buf,sizeof(buf));
			sei();
			t = 1;
		}else if(t == 1){
			uint8_t buf[] = {0x02,0x06,0x09,0x00,0x00,0x00,0x00,0x00}; //ServoOff
			uint16_t crc_ = crc(buf,sizeof(buf) - 2);
			buf[6] = *(((uint8_t*)&crc_) + 1);
			buf[7] = (uint8_t)crc_;
			init.adm2687_transmitter();
			cli();
			uart_485.TransmitBlock(buf,sizeof(buf));
			sei();
			t = 0;
		}
	}  */
}
#endif	
}

void Menu::menu_stop_LINPK()
{
	if(curItem->id == idLinkPK){
		to_copy_parameters_from_eeprom_to_ram();
		if(uart_usb.mode_rs_232_usb)
			uart_usb.disable_mode_usb();
		SET_C(idDiag);
		uart_usb.SET_U(CUART_usb::START_LINK_USB);
		uart_485.setSpd250kHz();
		timer1MSEKON();
	}
}
/*
таймер 1 мсек MED в то время как 10msek LO  метки HI
*/
void Menu::timer1MSEKON()
{
	#if (configUSE_Cycle1ms == 1)
	cli();
	TCC0.INTCTRLA=TC_OVFINTLVL_MED_gc;// 1ms таймер в
	sei();
	#endif
}

void Menu::timer1MSEKOFF()
{
	#if (configUSE_Cycle1ms == 1)
	cli();
	TCC0.INTCTRLA=TC_OVFINTLVL_OFF_gc;  // 1ms таймер в off
	sei();
	#endif
}

void Menu::init_menu_ABOU(const unsigned char & page)
{
	display.clear_ram_null();
	display.show_about_programm();
}

void Menu::menu_2_ABOU()
{
	if(curItem->id == idAbout){
		long pack = 0;
		pack |= (long)configUSE_menuItem_Correction_and_Izmerenia << 0;
		pack |= (long)configUSE_Circle_holes << 1;                  
		pack |= (long)configUSE_Linear_holes << 2;                    
		pack |= (long)configUSE_Calc << 3;  
		                           
		pack |= (long)configUSE_INCH << 4;                             
		pack |= (long)configUSE_Large_Digit << 5;                      
		pack |= (long)configUSE_REZERV  << 6;                       
		pack |= (long)configUSE_NO_input << 7; 
		                        
		pack |= (long)configUSE_ABS_Encoder  << 8;                    
		pack |= (long)configUSE_Cycle1ms << 9;                         
		pack |= (long)configUSE_NCshell  << 10;                         
		pack |= (long)configUSE_LIR541  << 11;  
		                        
		pack |= (long)configUSE_Shadow_axis << 12;                      
		pack |= (long)configUSE_PLC << 13;		                        
		pack |= (long)configUSE_mcx514  << 14;                          
		pack |= (long)configUSE_Conf01 << 15;  
		                          
		pack |= (long)configUSE_ModBus_DRV << 16;                      
		pack |= (long)configUSE_Alt_ENCerr << 17;                       
		pack |= (long)configUSE_ExtFormats << 18;                       
		pack |= (long)configUSE_max7370 << 19; 
		                         
		pack |= (long)configUSE_RGB_disp  << 20;                     
		pack |= (long)configUSE_debug_ABS_G53 << 21;

		display.print_debug_one_local(pack,0,11,16 * 5);
	}
}

void Menu::menu_stop_ABOU()
{
	if(curItem->id == idAbout){
		SET_C(idDiag);
	}
}

void Menu::menu_enter_ABOU()
{
	if(curItem->id == idAbout){
		SET_C(SnakeInit);
	}
}


void Menu::actions_input_FSET(const unsigned char & page)
{
	int x_, y_;
	x_ = 26;
	y_ = menu_cnt_FSET * 16 + (menu_cnt_OFFSN - 1) * 16;

	flashing_cursor(x_,y_,page);
	if (menu.ResultForInputNum == input::ok) {
		if (display.strnlen_lir((const char*)bufASCIIstr)) {
			uint8_t ch = get_ch_FSET();
			long offset = String_to_bin(bufASCIIstr,ch);
			flash_prg.to_check_offsets(ch, &offset);
			if (menu_cnt_OFFSN == 1)
				display.offset_g53[ch] = offset;
			else
				display.offset_g5x[menu_cnt_OFFSN - 2][ch] = offset;
			entry_OFFSN = UPDATE;
		}
		display.print_flash_local(sProbel21, 0, x_, y_);
		//display.print_flash_local(probel21, 0, 0, 96);
		//display.print_flash_local(probel21, 0, 0, 112);
		init_cursor_pos();
		SET_C(idCoordOffset);//SET_C(idAxisListOffset);
		
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel21, 0, x_, y_);
		//display.print_flash_local(probel21, 0, 0, 96);
		//display.print_flash_local(probel21, 0, 0, 112);
		init_cursor_pos();
		SET_C(idCoordOffset);//SET_C(idAxisListOffset);
		//entry_FSET = UPDATE;
	}
}



uint8_t Menu::get_ch_FSET()
{
	for (uint8_t j = 0, i = 0; j < N_AXIS; j++) {
		if (P2[j] != no_axis) {
			if (i == menu_cnt_FSET - 1)
				return j;
			else
				i++;
		}
	}
	return N_AXIS;
}

void Menu::actions_ANOUT()
{
	if (entry_ANOUT == INIT)
		init_menu_ANOUT();
	if (entry_ANOUT == UPDATE) {
		draw_menu_ANOUT();
		draw_param_ANOUT(25,LEFT_ALIGN);
	}
	if (entry_ANOUT == IDLE){
		debug_counters_ANOUT();
	}
	return;
}

void Menu::debug_counters_ANOUT(){
	#if (configUSE_PLC == 1)
	for(int c = 0 ; c < N_AXIS; c++){
		if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG && P2[c] != no_axis){
			char str_a[5] = {0};
			str_a[0] = pgm_read_byte_near(sAxis_screen[P2[c]] + 2);
			str_a[1] = '>';
			str_a[2] = 'a';
			if(flash_prg._OUTAxis2[c].AnalOut == CPrg::A1){
				str_a[3] = '1';		
			}else if(flash_prg._OUTAxis2[c].AnalOut == CPrg::A2){
				str_a[3] = '2';
			}
			else
				continue;

			display.print_ram_local(str_a,0,1, 16 * (c + 10));

			if(display.encoder_fault_[c])//Счетчик Altera
				display.print_flash_local(sError,0,6,16 *(c + 10));
			else
				display.print_debug_one_local_shot(altera.counter[c],0,6,16 *(c + 10),10);
		}
	}
	#endif
}

void Menu::actions_input_ANOUT()
{
	#if (configUSE_PLC == 1)		
	int x_, y_;	
	x_ = 16;
	y_ = menu_cnt_ANOUT * 16;

	flashing_cursor(x_, y_, 0);
	debug_counters_ANOUT();
	if (menu.ResultForInputNum == input::ok) {
		long tmp = 0;
		int f = CPrg::LINEAR_3;
		if(menu_cnt_ANOUT == 1 || menu_cnt_ANOUT == 2)
			f = CPrg::LINEAR_4;
		if (!display.strnlen_lir((const char*)bufASCIIstr))
			goto skip_pr;
		ConvertASCIIlinear_circle_tolong(tmp,f,bufASCIIstr);
		if(menu_cnt_ANOUT == 1 || menu_cnt_ANOUT == 2){
			if(tmp < 0){
				//tmp = (tmp - (tmp % 10)) + ((((tmp % 10) - 1) / 3 ) * 3);
				int tm2 = ((((tmp % 10) - 1) / 3) * 3);
				tm2 = tm2 < 8 ? tm2 : 10;
				tmp = (tmp - (tmp % 10)) + tm2;
				}else{
				//tmp = (tmp - (tmp % 10)) + ((((tmp % 10) + 1) / 3 ) * 3);
				int tm2 = ((((tmp % 10) + 1) / 3) * 3);
				tm2 = tm2 < 8 ? tm2 : 10;
				tmp = (tmp - (tmp % 10)) + tm2;
				}

			if(tmp > 6959L)
				tmp = 6959;
			else if(tmp < -6959L)
				tmp = -6959;

			cli();
			flash_prg._Allp_param7.U_off_a[menu_cnt_ANOUT - 1] = tmp; //вычислить указатель
			uart_485.send_U(U_out);
			sei();
		}
		else if(menu_cnt_ANOUT == 4 || menu_cnt_ANOUT == 5){
			if(tmp > 10000L)
				tmp = 10000;
			else if(tmp < -10000L)
				tmp = -10000;
			U_out[menu_cnt_ANOUT - 4] = (int16_t)tmp; //вычислить указатель
			cli();
			uart_485.send_U(U_out);
			sei();
		}
		skip_pr:
		display.print_flash_local(sProbel10, 0, x_, y_);
		init_cursor_pos();
		SET_C(idAnout);
		entry_ANOUT = UPDATE;//exit_G1_G0_input_pr(page,x_,y_);
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel10, 0, x_, y_);
		init_cursor_pos();
		SET_C(idAnout);
		entry_ANOUT = UPDATE;//exit_G1_G0_input_pr(page,x_,y_);
	}
	#endif
}

void Menu::init_menu_ANOUT()
{
	display.clear_ram_null();
	entry_ANOUT = UPDATE;
}

void Menu::draw_menu_ANOUT()
{
		const char *menu_table[10] = { 0 };
		menu_table[0] = PSTR(" АНАЛОГОВЫЕ ВЫХОДЫ >");
		menu_table[1] = PSTR(" Смещение >a1");
		menu_table[2] = PSTR(" Смещение >a2");
		menu_table[3] = sLine;
		menu_table[4] = PSTR(" Напряжение >а1");
		menu_table[5] = PSTR(" Напряжение >а2");
		menu_table[6] = nullptr;
		menu_ANOUT_SZ = 5;
		//==вывод на экран
		Draw_menu(menu_table, menu_cnt_ANOUT,0);
		//==
		entry_ANOUT = IDLE;
}

void Menu::draw_param_ANOUT(char x,char align)
{
	#if (configUSE_PLC == 1)
	//==Смещение a1
	print_long_format(flash_prg._Allp_param7.U_off_a[0],CPrg::FORMAT::LINEAR_4,0,x,16 * 1,align,0,15);
	//==
	
	//==Смещение a2
	print_long_format(flash_prg._Allp_param7.U_off_a[1],CPrg::FORMAT::LINEAR_4,0,x,16 * 2,align,0,15);
	//==
	
	//==Напряжение а1
	print_long_format(U_out[0], CPrg::LINEAR_3, 0, x, 16 * 4, align,0,15);
	//==
	
	//==Напряжение а2
	print_long_format(U_out[1], CPrg::LINEAR_3, 0, x, 16 * 5, align,0,15);
	//==

	#endif
}

void Menu::menu_up_ANOUT()
{
	if(curItem->id == idAnout){
		menu_cnt_ANOUT--;
		//skip_menu_item(0,menu_cnt_ANOUT,6);
		skip_menu_item(0,menu_cnt_ANOUT,3);
		if(menu_cnt_ANOUT < 1)
			menu_cnt_ANOUT = menu_ANOUT_SZ;
		entry_ANOUT = UPDATE;
	}
}

void Menu::menu_down_ANOUT()
{
	if(curItem->id == idAnout){
		menu_cnt_ANOUT++;
		skip_menu_item(1,menu_cnt_ANOUT,3);
		//skip_menu_item(1,menu_cnt_ANOUT,6);		
		if(menu_cnt_ANOUT > menu_ANOUT_SZ)
			menu_cnt_ANOUT = 1;
		entry_ANOUT = UPDATE;
	}
}

void Menu::menu_enter_ANOUT()
{
	if(curItem->id == idAnout){
		if(menu_cnt_ANOUT == 1  || menu_cnt_ANOUT == 2 || menu_cnt_ANOUT == 4  || menu_cnt_ANOUT == 5){
			SET_C(idAnoutInput);
		}
	}
}

void Menu::menu_stop_ANOUT()
{
	if(curItem->id == idAnout){
		#if (configUSE_PLC == 1)
		flash_prg.write_eeprom(1713,sizeof(flash_prg._Allp_param7),&flash_prg._Allp_param7);
		#endif
		entry_ANOUT = INIT;
		SET_C(idDebugMode);
	}
}

void Menu::actions_ANIN()
{
	if (entry_ANIN == INIT)
		init_menu_ANIN();
	if (entry_ANIN == UPDATE) {
		draw_menu_ANIN();
		draw_param_ANIN(25,LEFT_ALIGN);
	}
	if (entry_ANIN == IDLE){
		debug_counters_ANIN();
	}
	return;
}

void Menu::debug_counters_ANIN()
{
	static uint16_t disp_slow;

	disp_slow++;
	if(disp_slow % 1200)
		return;
	//==Код напряжения а1
	ADCA.CH0.MUXCTRL = 0x60;
	ADCA.CH0.CTRL = 0x81;
	while(!(ADCA.CH0.INTFLAGS & 0x01));
	ADCA.CH0.INTFLAGS = 0x01;  
	print_long_format((int16_t)ADCA.CH0.RES < 0 ? 0 : (int16_t)ADCA.CH0.RES,CPrg::FORMAT::FORMAT_END,0,25,16 * 1,0,0,15);
	//==
	
	//==Код напряжения а2
	ADCA.CH0.MUXCTRL = 0x68;
	ADCA.CH0.CTRL = 0x81;
	while(!(ADCA.CH0.INTFLAGS & 0x01));
	ADCA.CH0.INTFLAGS = 0x01;  
	print_long_format((int16_t)ADCA.CH0.RES < 0 ? 0 : (int16_t)ADCA.CH0.RES,CPrg::FORMAT::FORMAT_END,0,25,16 * 2,0,0,15);
	//==
}

void Menu::actions_input_ANIN()
{
	#if (configUSE_PLC == 1)
	int x_, y_;
	x_ = 16;
	y_ = menu_cnt_ANIN * 16;

	flashing_cursor(x_, y_, 0);
	debug_counters_ANIN();
	if (menu.ResultForInputNum == input::ok) {
		long tmp = 0;
		int f = CPrg::LINEAR_3;
		if (!display.strnlen_lir((const char*)bufASCIIstr))
			goto skip_pr;
		ConvertASCIIlinear_circle_tolong(tmp,f,bufASCIIstr);
		if(menu_cnt_ANIN == 5){ //==Минимальный Код
			tmp/=1000;//из CPrg::LINEAR_3 в код
			if(tmp < 0)
			tmp = 0;
			else if(tmp > flash_prg._Allp_param7.MAX_code)
			goto skip_pr;
			flash_prg._Allp_param7.min_code = tmp;
		}else if(menu_cnt_ANIN == 6){ //== Максимальный Код
			tmp/=1000;//из CPrg::LINEAR_3 в код
			if(tmp < flash_prg._Allp_param7.min_code)
			goto skip_pr;
			else if(tmp > 2047)
			tmp = 2047;
			flash_prg._Allp_param7.MAX_code = tmp;			
		}else if(menu_cnt_ANIN == 7){ // ==Минимальный % подач
			tmp/=1000;//из CPrg::LINEAR_3 в %
			if(tmp < 0)
			tmp = 0;
			else if(tmp > flash_prg._Allp_param7.MAX_percent_feed)
			goto skip_pr;
			flash_prg._Allp_param7.min_percent_feed = tmp;
			_modals_cmd.Feed = flash_prg._Allp_param7.min_percent_feed;
		}
		else if(menu_cnt_ANIN == 8){ // ==Максимальный % подач
			tmp/=1000;//из CPrg::LINEAR_3 в %
			if(tmp < flash_prg._Allp_param7.min_percent_feed)
			goto skip_pr;
			else if(tmp > 200)
			tmp = 200;
			flash_prg._Allp_param7.MAX_percent_feed = tmp;
		}
		skip_pr:
		display.print_flash_local(sProbel10, 0, x_, y_);
		init_cursor_pos();
		SET_C(idAnin);
		entry_ANIN = UPDATE;//exit_G1_G0_input_pr(page,x_,y_);
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel10, 0, x_, y_);
		init_cursor_pos();
		SET_C(idAnin);
		entry_ANIN = UPDATE;//exit_G1_G0_input_pr(page,x_,y_);
	}
	#endif
}

void Menu::init_menu_ANIN()
{
	display.clear_ram_null();
	entry_ANIN = UPDATE;
}

void Menu::draw_menu_ANIN()
{
	const char *menu_table[10] = { 0 };
	menu_table[0] = PSTR(" АНАЛОГОВЫЕ ВХОДЫ <");
	menu_table[1] = PSTR(" Код напряжения <a1");
	menu_table[2] = PSTR(" Код напряжения <a2");
	menu_table[3] = sLine;
	menu_table[4] = PSTR(" КОРРЕКТОР ПОДАЧИ <а1");
	menu_table[5] = PSTR(" Миним. код");
	menu_table[6] = PSTR(" Макс. код");
	menu_table[7] = PSTR(" Миним. % подачи");
	menu_table[8] = PSTR(" Макс.  % подачи");
	menu_table[9] = nullptr;
	menu_ANIN_SZ = 8;
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_ANIN,0);
	//==
	entry_ANIN = IDLE;
}

void Menu::draw_param_ANIN(char x,char align)
{
    //==КОРРЕКТОР ПОДАЧ а1
	if(flash_prg._Allp_param7.korr_podach == 1)
		display.print_flash_local(PSTR("цифровой  "), 0, x, 16 * 4);
	else
		display.print_flash_local(PSTR("аналоговый"), 0, x, 16 * 4);
	//==

	//==Минимальный код
	print_long_format(flash_prg._Allp_param7.min_code,CPrg::FORMAT::FORMAT_END,0,x,16 * 5,align,0,15);
	//==

	//==Максимальный код
	print_long_format(flash_prg._Allp_param7.MAX_code,CPrg::FORMAT::FORMAT_END,0,x,16 * 6,align,0,15);
	//==

	//==Мин % подачи
	print_long_format(flash_prg._Allp_param7.min_percent_feed,CPrg::FORMAT::FORMAT_END,0,x,16 * 7,align,0,15);
	//==
	
	//==Макс % подачи
	print_long_format(flash_prg._Allp_param7.MAX_percent_feed,CPrg::FORMAT::FORMAT_END,0,x,16 * 8,align,0,15);
	//==
}

void Menu::menu_up_ANIN()
{
	if(curItem->id == idAnin){
		menu_cnt_ANIN--;
		if(menu_cnt_ANIN < 4)
			menu_cnt_ANIN = menu_ANIN_SZ;
		entry_ANIN = UPDATE;
	}
}

void Menu::menu_down_ANIN()
{
	if(curItem->id == idAnin){
		menu_cnt_ANIN++;
		if(menu_cnt_ANIN > menu_ANIN_SZ)
			menu_cnt_ANIN = 4;
		entry_ANIN = UPDATE;
	}
}

void Menu::menu_enter_ANIN()
{
	if(curItem->id == idAnin){
		if(menu_cnt_ANIN == 5 || menu_cnt_ANIN == 6 || menu_cnt_ANIN == 7 || menu_cnt_ANIN == 8){
			SET_C(idAninInput);
		}
	}
}

void Menu::menu_stop_ANIN()
{
	if(curItem->id == idAnin){
		#if (configUSE_PLC == 1)
		flash_prg.write_eeprom(1713,sizeof(flash_prg._Allp_param7),&flash_prg._Allp_param7);
		#endif
		entry_ANIN = INIT;
		SET_C(idDebugMode);
	}
}

void Menu::menu_left_ANIN()
{
	if(curItem->id == idAnin){
		if(menu_cnt_ANIN == 4){
			flash_prg._Allp_param7.korr_podach ^= 1;
			entry_ANIN = UPDATE;
		}
	}
}

void Menu::menu_right_ANIN()
{
	if(curItem->id == idAnin){
		if(menu_cnt_ANIN == 4){
			flash_prg._Allp_param7.korr_podach ^= 1;
			entry_ANIN = UPDATE;
		}
	}
}

void Menu::Analog_Corr_handler_ANIN()
{
	static short m_Feed;
	static uint16_t disp_slow;
	disp_slow++;
	if(disp_slow % 1200)
	return;
	if(MsgKey.mode == TMsgKey::HAND || 
	(MsgKey.mode == TMsgKey::INPUT_PARAM && (curItem->id == idCmd ||curItem->id == idHandMove||curItem->id == idExecmd ||curItem->id == idHand_input)))
	{
		if(flash_prg._Allp_param7.korr_podach == 1)
			return;

		int16_t tmp = 0;
		cli();
		ADCA.CH0.MUXCTRL = 0x60;// <a1
		ADCA.CH0.CTRL = 0x81;
		while(!(ADCA.CH0.INTFLAGS & 0x01));
		ADCA.CH0.INTFLAGS = 0x01;
		tmp = (int16_t)ADCA.CH0.RES;
		sei();
		if(tmp < flash_prg._Allp_param7.min_code)
			tmp = flash_prg._Allp_param7.min_code;
		else if(tmp > flash_prg._Allp_param7.MAX_code)
			tmp = flash_prg._Allp_param7.MAX_code;

		short denom = flash_prg._Allp_param7.MAX_code - flash_prg._Allp_param7.min_code;
		if(denom <= 0)
			denom = 1;

		_modals_cmd.Feed = ((long)tmp * flash_prg._Allp_param7.MAX_percent_feed) / (denom);

		if(_modals_cmd.Feed < flash_prg._Allp_param7.min_percent_feed)
			_modals_cmd.Feed = flash_prg._Allp_param7.min_percent_feed;
		else if(_modals_cmd.Feed > flash_prg._Allp_param7.MAX_percent_feed)
			_modals_cmd.Feed = flash_prg._Allp_param7.MAX_percent_feed;

		if(_modals_cmd.Feed > 200)
			_modals_cmd.Feed = 200;

			//if(MsgKey.mode != TMsgKey::FIND_REF)
		if(m_Feed != _modals_cmd.Feed){				
			UPD_show_Instr = 1;				
			upd_Spd_inMove();
			m_Feed = _modals_cmd.Feed;
		}
	}
}

void Menu::actions_OFFSN(const unsigned char & page)
{
	if (entry_OFFSN == INIT)
		init_menu_OFFSN(page);
	if (entry_OFFSN == UPDATE)
		draw_menu_OFFSN(page);
	if (entry_OFFSN == IDLE)
	return;
}

void Menu::actions_input_OFFSN(const unsigned char & page)
{
	int x_, y_;
	x_ = 12;

	x_ = strlen_P(sPlease_input_pass) + 1;
	y_ = 16 * 13;

	flashing_cursor(x_, y_, page);
	if (menu_cnt_OFFSN == 1) {
		if (cursor_pos > 4) {
			display.print_flash_local(sAccess_denided, page, x_, y_);
			Sleep(500);
			display.print_flash_local(sProbel10, page, x_, y_);
			init_cursor_pos();
		}
	}

	if (menu.ResultForInputNum == input::ok) {
		if (bufASCIIstr[0] == '5' && bufASCIIstr[1] == '3' && bufASCIIstr[2] == '\0' && bufASCIIstr[3] == '\0' && bufASCIIstr[4] == '\0')
		{
			mOFFSNPassG53Ok = true;
			//entry_OFFSN = INIT;
			//SET_C(idAxisListOffset);
			if(menu_column_OFFSN == 0){
				menu_column_OFFSN = 1;
				entry_OFFSN = UPDATE;
			}
			SET_C(idCoordOffset);
			display.print_flash_local(sProbel21, page, 0, y_);
		}
		else {
			display.print_flash_local(sAccess_denided, page, x_, y_);
			Sleep(500);
			display.print_flash_local(sProbel10, page, x_, y_);
		}
		init_cursor_pos();
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel21, page, 0, y_);
		SET_C(idCoordOffset);
		init_cursor_pos();
	}
}

void Menu::init_menu_OFFSN(const unsigned char & page)
{
	display.clear_ram_null();
	entry_OFFSN = UPDATE;
}

void Menu::draw_menu_OFFSN(const unsigned char & page)
{
const char*menu_table[9];
menu_table[0] = sOFFSET_COORDINATE;
menu_table[1] = PSTR("   G53   ");
menu_table[2] = PSTR("   G54   ");
menu_table[3] = PSTR("   G55   ");
menu_table[4] = PSTR("   G56   ");
menu_table[5] = PSTR("   G57   ");
menu_table[6] = PSTR("   G58   ");
//menu_table[7] = sCM;
menu_table[7] = nullptr;
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_OFFSN,page);
	//==
	predprosmotr_offset(page,9,16 * menu_cnt_OFFSN);
	entry_OFFSN = IDLE;
}

void Menu::predprosmotr_offset(const unsigned char& page,int x,int y)
{
	long offset;
	for(int str = 1 ; str < 10;str++){
		if(str == menu_cnt_OFFSN)
			continue;
		if(str == menu_cnt_OFFSN + 1 && menu_FSET_SZ >= 2)
			continue;
		if(str == menu_cnt_OFFSN + 2 && menu_FSET_SZ >= 3)
			continue;
		if(str == menu_cnt_OFFSN + 3 && menu_FSET_SZ >= 4)
			continue;
		display.print_symbols_local(' ',22,page,x,str * 16);
	}
	for (int c = 0,s = 0; c < N_AXIS; c++) {
		if(menu_column_OFFSN == 1){
			if(s == menu_cnt_FSET - 1)
				INVERT_CHAR()
			else
				NORMAL_CHAR()
		}
		if (P2[c] != no_axis){
			display.print_flash_local(sAxis_screen[P2[c]] + 1,page,x,y + (16 * s));
			if (menu_cnt_OFFSN == 1)
				offset = display.offset_g53[c];
			else
				offset = display.offset_g5x[menu_cnt_OFFSN - 2][c];
			l_d(offset,c,on_display);
			print_long_format(offset, get_format_from_ch(c), 0, x + 5 + 11 - 1, y + (16 * s), RIGHT_ALIGN, 0, 11); //11
			s++;
		}
		menu_FSET_SZ = s;
		NORMAL_CHAR()
	}
	if(!menu_FSET_SZ)
		display.print_flash_local(sAxisNoList,page,x,y);

	if (menu_cnt_FSET > menu_FSET_SZ)
		menu_cnt_FSET = menu_FSET_SZ;

	if (!menu_cnt_FSET)
		menu_cnt_FSET = 1;

}

void Menu::menu_up_OFFSN()
{
	if (curItem->id == idCoordOffset) {
		if(menu_column_OFFSN == 0){
		menu_cnt_OFFSN--;
		if (menu_cnt_OFFSN <= 0)
			menu_cnt_OFFSN = 6;
		entry_OFFSN = UPDATE;
		}
		else if(menu_column_OFFSN == 1){
			menu_cnt_FSET--;
			if (menu_cnt_FSET <= 0)
			menu_cnt_FSET = menu_FSET_SZ;
			entry_OFFSN = UPDATE;
		}
	}
}

void Menu::menu_down_OFFSN()
{
	if (curItem->id == idCoordOffset) {
		if(menu_column_OFFSN == 0){
		menu_cnt_OFFSN++;
		if (menu_cnt_OFFSN > 6)
		menu_cnt_OFFSN = 1;
		entry_OFFSN = UPDATE;
		}
		else if(menu_column_OFFSN == 1){
			menu_cnt_FSET++;
			if (menu_cnt_FSET > menu_FSET_SZ)
			menu_cnt_FSET = 1;
			entry_OFFSN = UPDATE;
		}
	}
}

void Menu::menu_enter_OFFSN()
{
	if (curItem->id == idCoordOffset) {
		//entry_OFFSN = INIT;
		//==G53==
		if(!menu_FSET_SZ)
			return;

		if (menu_cnt_OFFSN == 1) {
			if (!mOFFSNPassG53Ok) {
				entry_OFFSN = IDLE;
				display.print_flash_local(sPlease_input_pass, 0, 1, 16 * 13);
				init_cursor_pos();
				SET_C(idCoordOffsetInput);
			}
			else {
				if(menu_column_OFFSN == 0){//переход на соседнюю колонку
					menu_column_OFFSN = 1;
					entry_OFFSN = UPDATE;
					}
				else if(menu_column_OFFSN == 1){
					uint8_t ch = get_ch_FSET();
					if (ch >= 0 && ch < N_AXIS) {
						CurFormatM = flash_prg.axis_cfg[ch].format;
						SET_C(idAxisListOffsetInput);//вход в режим редактирования
						}
					}
				//SET_C(idAxisListOffset);
			}
			return;
		}
		//==
		//==G54,G55,G56,G57,G58
		else if(menu_cnt_OFFSN > 1 && menu_cnt_OFFSN < 7){
			if(menu_column_OFFSN == 0){
				menu_column_OFFSN = 1;
				entry_OFFSN = UPDATE;
				}
			else if(menu_column_OFFSN == 1){
				uint8_t ch = get_ch_FSET();
				if (ch >= 0 && ch < N_AXIS) {
					CurFormatM = flash_prg.axis_cfg[ch].format;
					SET_C(idAxisListOffsetInput);//вход в режим редактирования
				}
			}
			//SET_C(idAxisListOffset);
			//MB(menu_cnt_OFFSN);
			return;
		}
		//==
		//==КМ
		else if(menu_cnt_OFFSN == 8){
			return;
		}
		//==
		
	}
}

void Menu::menu_stop_OFFSN()
{
	if (curItem->id == idCoordOffset) {
		if(menu_column_OFFSN == 0){
		entry_OFFSN = INIT;
		flash_prg.to_copy_G53_and_G5x_from_RAM_to_EEPROM();
		SET_C(idDiag);
		}
		else if(menu_column_OFFSN == 1){
			menu_column_OFFSN = 0;
			entry_OFFSN = UPDATE;
		}
	}
}

void Menu::actions_INSTRC(const unsigned char & page)
{
	if (entry_INSTRC == INIT)
		init_menu_INSTRC(page);
	if (entry_INSTRC == UPDATE)
		draw_menu_INSTRC(page);
	if (entry_INSTRC == IDLE)
	return;
}

void Menu::actions_input_INSTRC(const unsigned char & page)
{
	int x_, y_;
	x_ = 27;
	y_ = 16 + (16 * menu_cnt_INSTRC) + (16 * menu_cnt_INSTRC_rl);

	flashing_cursor(x_,y_,page);
	if (menu.ResultForInputNum == input::ok) {
		if (display.strnlen_lir((const char*)bufASCIIstr)) {
			long tmp = 0;
			Lir.ConvertASCIIlinear_circle_tolong(tmp,CPrg::FORMAT::LINEAR_4,bufASCIIstr);
			if(tmp < 0)
				tmp = -tmp;

			*(&T_instr[menu_cnt_INSTRC].r + menu_cnt_INSTRC_rl) = tmp;
			entry_INSTRC = UPDATE;
		}
		display.print_flash_local(sProbel21, 0, x_, y_);
		init_cursor_pos();
		SET_C(idInstrC);		
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel21, 0, x_, y_);
		init_cursor_pos();
		SET_C(idInstrC);
	}
}

void Menu::init_menu_INSTRC(const unsigned char & page)
{
	display.clear_ram_null();
	display.print_flash_local(PSTR(" ИНСТРУМЕНТЫ"),page,0,0);
	entry_INSTRC = UPDATE;
}

void Menu::draw_menu_INSTRC(const unsigned char & page)
{
	for(int T_num = 0; T_num < MAX_INSTR; T_num++){
		if(menu_cnt_INSTRC == T_num){
			show_cur_instr(menu_cnt_INSTRC);
			INVERT_CHAR()
			}
		else
			NORMAL_CHAR()
		
		display.print_symbols_local(' ',3,0,0,16 * (T_num + 1));
		display.put_char_local_independent('T',page,3,16 * (T_num + 1));
		Lir.print_long_format(T_num + 1,CPrg::FORMAT_END,page,5,16 * (T_num + 1),RIGHT_ALIGN,1);
		display.print_symbols_local(' ',3,0,6,16 * (T_num + 1));
		NORMAL_CHAR()
	}
	entry_INSTRC = IDLE;
}

void Menu::show_cur_instr(int y)
{
	for(int str = 1; str <= MAX_INSTR + 1;str++){
		if(str == menu_cnt_INSTRC + 1 || str == menu_cnt_INSTRC + 2)
			continue;
		display.print_symbols_local(' ',17,0,9,str * 16);
		}

	const char *menu_table[2] = { 0 };

	menu_table[0] = PSTR("радиус ");
	menu_table[1] = PSTR(" длина ");
	//str_lr 0 - радиус
	//str_lr 1 - длина

	for(int str_lr = 0 ; str_lr < 2;str_lr++){
		if(menu_column_INSTRC == 1 ){
			if(str_lr == menu_cnt_INSTRC_rl)
				INVERT_CHAR()
			else
				NORMAL_CHAR()
		}
		display.print_flash_local(menu_table[str_lr],0,9,16 * (y + (str_lr + 1)));
		Lir.print_long_format(*(&T_instr[y].r + str_lr),CPrg::FORMAT::LINEAR_4,0,25,16 * (y + (str_lr + 1)),RIGHT_ALIGN,0,10);
		NORMAL_CHAR()
	}  
}

void Menu::menu_up_INSTRC()
{
	if(curItem->id == idInstrC){
		if(menu_column_INSTRC == 0){
		menu_cnt_INSTRC--;
		if(menu_cnt_INSTRC < 0)
			menu_cnt_INSTRC = MAX_INSTR - 1;
		entry_INSTRC = UPDATE;
		}
		else if(menu_column_INSTRC == 1){
			if(menu_cnt_INSTRC_rl > 0){
			menu_cnt_INSTRC_rl--;
			entry_INSTRC = UPDATE;
			}
		}
		
	}
}

void Menu::menu_down_INSTRC()
{
	if(curItem->id == idInstrC){
		if(menu_column_INSTRC == 0){
		menu_cnt_INSTRC++;
		if(menu_cnt_INSTRC > MAX_INSTR - 1)
			menu_cnt_INSTRC = 0;
		entry_INSTRC = UPDATE;
		}
		else if(menu_column_INSTRC == 1){
			if(menu_cnt_INSTRC_rl < 1){
			menu_cnt_INSTRC_rl++;
			entry_INSTRC = UPDATE;
			}
		}
		
	}
}

void Menu::menu_left_INSTRC()
{
	if(curItem->id == idInstrC){
		entry_INSTRC = UPDATE;
	}
}

void Menu::menu_right_INSTRC()
{
	if(curItem->id == idInstrC){
		entry_INSTRC = UPDATE;
	}
}

void Menu::menu_enter_INSTRC()
{
	if(curItem->id == idInstrC){
		if(menu_column_INSTRC == 0){
			menu_column_INSTRC = 1;
			entry_INSTRC = UPDATE;
			}
		else if(menu_column_INSTRC == 1)
			SET_C(idInstrCInput);//входим в режим редактирования
	}
}

void Menu::menu_stop_INSTRC()
{
	if(curItem->id == idInstrC){
		if(menu_column_INSTRC == 0){
		//save_Allp_param9_to_eeprom();
		flash_prg.write_eeprom(924,sizeof(Lir.T_instr[0]) * MAX_INSTR,&Lir.T_instr);
		entry_INSTRC = INIT;
		SET_C(idDiag);
		}
		else if(menu_column_INSTRC == 1){
			menu_column_INSTRC = 0;
			entry_INSTRC = UPDATE;
		}
	}
}

void Menu::actions_IZMER(const unsigned char & page)
{
	if (entry_IZMER == INIT)
	init_menu_IZMER(page);
	if (entry_IZMER == UPDATE)
	draw_menu_IZMER(page);
	if (entry_IZMER == IDLE)
	return;
}

void Menu::init_menu_IZMER(const unsigned char & page)
{
	display.clear_ram_null();
	entry_IZMER = UPDATE;
}

void Menu::draw_menu_IZMER(const unsigned char & page)
{
	const char *menu_table[4] = { 0 };
	uint8_t i = 0;
	//==конструктор меню
	for (uint8_t j = 0; j < 3;) {
		menu_table[i++] = sIzmer[j++];
	}
	//==
	//==размер меню
	menu_IZMER_SZ = i - 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_IZMER,page);
	//==
	entry_IZMER = IDLE;
}

void Menu::menu_up_IZMER()
{
	if (curItem->id == idIzmer) {
		menu_cnt_IZMER--;
		if (menu_cnt_IZMER <= 0)
		menu_cnt_IZMER = menu_IZMER_SZ;
		entry_IZMER = UPDATE;
	}
}

void Menu::menu_down_IZMER()
{
	if (curItem->id == idIzmer) {
		menu_cnt_IZMER++;
		if (menu_cnt_IZMER > menu_IZMER_SZ)
		menu_cnt_IZMER = 1;
		entry_IZMER = UPDATE;
	}
}

void Menu::menu_enter_IZMER()
{
	if (curItem->id == idIzmer) {
		entry_IZMER = INIT;
		int8_t place = 1;
		//==Индикатор
		if (menu_cnt_IZMER == place++) {
			//MB("Индикатор");
			SET_C(idIndikator);			
			return;
		}
		//==
		//== График
		if (menu_cnt_IZMER == place++) {
			//MB("График");
			SET_C(idGraph);	
			return;
		}
		//==
	}
}

void Menu::menu_stop_IZMER()
{
	if (curItem->id == idIzmer) {
		entry_IZMER = INIT;
		SET_C(idDiag);
	}
}

void Menu::actions_INDI(const unsigned char & page)
{
	if (entry_INDI == INIT)
		init_menu_INDI(page);
	if (entry_INDI == UPDATE)
		entry_INDI = IDLE;
	if (entry_INDI == IDLE)
		return;
}

void Menu::init_menu_INDI(const unsigned char & page)
{
	display.clear_ram_null();
	flash_prg.mode_graph=false;
	flash_prg.clear_place_for_graph=true;
	entry_INDI = UPDATE;
	
}

void Menu::actions_GRPH(const unsigned char & page)
{
	if (entry_GRPH == INIT)
		init_menu_GRPH(page);
	if (entry_GRPH == UPDATE)
		entry_GRPH = IDLE;
	if (entry_GRPH == IDLE)
		return;
}

void Menu::init_menu_GRPH(const unsigned char & page)
{
	display.clear_ram_null();
	//display.print_flash_local(PSTR("График"),0,0,0);
	flash_prg.debug++;
	flash_prg.mode_graph=true;
	flash_prg.clear_place_for_graph=true;
	flash_prg.start_point_for_graph=true;
	entry_GRPH = UPDATE;
}

void Menu::command_GRPH(char key)
{
	switch(key){
		case _key_STOP:
			entry_GRPH = INIT;
			SET_C(idIzmer);
			break;
		default:
			break;
	}
}

void Menu::menu_stop_REPA()
{
	if(curItem->id == idResetParametrs){
		SET_C(idAllp);
	}
}

void Menu::menu_stop_REOF()
{
	if(curItem->id == idResetOffset){
		SET_C(idAllp);
	}
}

void Menu::actions_DBG(const unsigned char & page)
{
	if (entry_DBG == INIT)
	init_menu_DBG(page);
	if (entry_DBG == UPDATE)
	draw_menu_DBG(page);
	if (entry_DBG == IDLE)
	return;
}

void Menu::init_menu_DBG(const unsigned char & page)
{
	display.clear_ram_null();
	entry_DBG = UPDATE;
	#if (configUSE_NCshell == 1)
	//==сброс выхода готовность УЦИПУ при входе в режим отладка
	block_SPPU_OUT = true;
	Lir.outP(flash_prg._OUTGen.gotovnostbUCIPU,0,1); 	
	//==
	#endif
}

void Menu::draw_menu_DBG(const unsigned char & page)
{
	const char *menu_table[9] = { 0 };
	menu_table[0] = s00_DBG;
	menu_table[1] = s01_DBG;
	menu_table[2] = s02_DBG;
	menu_table[3] = s03_DBG;
	menu_table[4] = s04_DBG;
	menu_table[5] = nullptr;
	menu_DBG_SZ = 4;
#if (configUSE_NCshell == 1)
	menu_table[5] = s05_DBG;
	menu_table[6] = nullptr;
	menu_DBG_SZ = 5;
#endif
#if (configUSE_PLC == 1)
	menu_table[6] = s06_DBG;
	menu_table[7] = s07_DBG;
	menu_table[8] = nullptr;
	menu_DBG_SZ = 7;
#endif

	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_DBG,page);
	//==
	entry_DBG = IDLE;
}

void Menu::menu_up_DBG()
{
	if (curItem->id == idDebugMode) {
		menu_cnt_DBG--;
		if (menu_cnt_DBG < 1)
		menu_cnt_DBG = menu_DBG_SZ;
		entry_DBG = UPDATE;
	}
}

void Menu::menu_down_DBG()
{
	if (curItem->id == idDebugMode) {
		menu_cnt_DBG++;
		if (menu_cnt_DBG > menu_DBG_SZ)
		menu_cnt_DBG = 1;
		entry_DBG = UPDATE;
	}
}

void Menu::menu_enter_DBG()
{
	if (curItem->id == idDebugMode) {
		//uint8_t place = 1;
		entry_DBG = INIT;
		//==Датчики ==
		if (menu_cnt_DBG == 1) {
			UpdateMenuEncoders();
			SET_C(idMenuEncoders);
			return;
		}
		//==
		//==Питание и батарея ==
		if (menu_cnt_DBG == 2) {
			SET_C(idSupplyandBatt);
			UpdateMenuSupply();
			return;
		}
		//==
		//==Время ==
		if (menu_cnt_DBG == 3) {
			SET_C(idTime);
			UpdateMenuTime();
			return;
		}
		//==
		//==Входы ==
		if (menu_cnt_DBG == 4) {
			if(configUSE_NO_input == 0){
				SET_C(idViewInputs);
				InitMenuViewInputs();
			}
			return;
		}
		//==

		//==Выходы ==
		if (menu_cnt_DBG == 5) {
			#if (configUSE_NCshell == 1)
			entry_DOUT = INIT; // доп инициализация, была ошибка переменная не равна INIT при входе в меню idViewOutputs
			#endif
			SET_C(idViewOutputs);
			return;
		}
		//==
		#if (configUSE_PLC == 1)
		//==Аналоговый выход
		if (menu_cnt_DBG == 6) {
			SET_C(idAnout);
			return;
		}
		if (menu_cnt_DBG == 7) {
			SET_C(idAnin);
			return;
		}
		//==
		#endif
	}
}

void Menu::menu_stop_DBG()
{
	if (curItem->id == idDebugMode) {
		block_SPPU_OUT = false;
		entry_DBG = INIT;
		SET_C(idDiag);
	}
}

void Menu::actions_AXILIST(const unsigned char & page)
{
	if (entry_AXILIST == INIT)
	init_menu_AXILIST(page);
	if (entry_AXILIST == UPDATE)
	draw_menu_AXILIST(page);
	if (entry_AXILIST == IDLE)
	return;
}

void Menu::init_menu_AXILIST(const unsigned char & page)
{
	display.clear_ram_null();
	entry_AXILIST = UPDATE;
}

void Menu::draw_menu_AXILIST(const unsigned char & page)
{
	const char *menu_table[7] = { 0 };
	uint8_t i = 0;
	//==конструктор меню
	menu_table[i++] = sAxisList; //заголовок

	for (uint8_t j = 0; j < N_AXIS + N_SA;j++) {
		if (P2[j] != no_axis){
			if(j == SW_AXIS0)
				menu_table[i++] = sAxis_symbol_sw[P2[j]];
			else
				menu_table[i++] = sAxis_symbol[P2[j]];
			}
	}
	if (i == 1)
		menu_table[i] = sAxisNoList;
	//==
	//==размер меню
	menu_AXILIST_SZ = i - 1;
	if (menu_cnt_AXILIST > menu_AXILIST_SZ)
	menu_cnt_AXILIST = menu_AXILIST_SZ;

	if (!menu_cnt_AXILIST && i != 1)
	menu_cnt_AXILIST = 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_AXILIST,page);
	//==
	entry_AXILIST = IDLE;
}

uint8_t Menu::getChfromAxisList(const int8_t& str)
{
	for (uint8_t j = 0,i = 0; j < N_AXIS + N_SA; j++) {
		if (P2[j] != no_axis) {
			if (i == str - 1)
			return j;
			i++;
		}
	}
	return int8_t(N_AXIS + N_SA);
}

void Menu::menu_up_AXILIST()
{
	if (curItem->id == idAxislist) {
		menu_cnt_AXILIST--;
		if (menu_cnt_AXILIST <= 0)
		menu_cnt_AXILIST = menu_AXILIST_SZ;
		entry_AXILIST = UPDATE;
	}
}

void Menu::menu_down_AXILIST()
{
	if (curItem->id == idAxislist) {
		menu_cnt_AXILIST++;
		if (menu_cnt_AXILIST > menu_AXILIST_SZ)
		menu_cnt_AXILIST = 1;
		entry_AXILIST = UPDATE;
	}
}

void Menu::menu_enter_AXILIST()
{
	if (curItem->id == idAxislist) {
		uint8_t place = 1;
		//==Строка 1
		if (menu_cnt_AXILIST == place++) {
			flash_prg.ch = getChfromAxisList(menu_cnt_AXILIST);
			entry_AXILIST = INIT;			
			if(configUSE_NCshell == 1)
				SET_C(idChpa);	
			else	
				SET_C(idIndicOpt);
			return;
		}
		//==
		//==Строка 2
		if (menu_cnt_AXILIST == place++) {
			flash_prg.ch = getChfromAxisList(menu_cnt_AXILIST);
			entry_AXILIST = INIT;
			if(configUSE_NCshell == 1)
				SET_C(idChpa);
			else
				SET_C(idIndicOpt);
			return;
		}
		//==
		//==Строка 3
		if (menu_cnt_AXILIST == place++) {
			flash_prg.ch = getChfromAxisList(menu_cnt_AXILIST);
			entry_AXILIST = INIT;
			if(configUSE_NCshell == 1)
				SET_C(idChpa);
			else
				SET_C(idIndicOpt);
			return;
		}
		//==
		//==Строка 4
		if (menu_cnt_AXILIST == place++) {
			flash_prg.ch = getChfromAxisList(menu_cnt_AXILIST);
			entry_AXILIST = INIT;
			if(configUSE_NCshell == 1)
				SET_C(idChpa);
			else
				SET_C(idIndicOpt);
			return;
		}
		//==
		//==Строка 5  // слепая ось
		if (menu_cnt_AXILIST == place++) {
			flash_prg.ch = getChfromAxisList(menu_cnt_AXILIST);
			entry_AXILIST = INIT;
			if(configUSE_NCshell == 1)
				SET_C(idChpa);
			else
				SET_C(idIndicOpt);
			return;
		}
		//==
	}
}

void Menu::menu_stop_AXILIST()
{
	if (curItem->id == idAxislist) {
		entry_AXILIST = INIT;
		SET_C(idDiag);
	}
}

void Menu::actions_OPT(const unsigned char & page)
{
	if (entry_OPT == INIT)
	init_menu_OPT(page);
	if (entry_OPT == UPDATE) {
		draw_menu_OPT(page);
		draw_param_OPT(page,27,LEFT_ALIGN);
	}
	if (entry_OPT == IDLE)
	return;
}

void Menu::actions_input_OPT(const unsigned char & page)
{
	int x_, y_;
	x_ = 12;

	x_ = strlen_P(sOption[menu_cnt_OPT]) + 1;
	y_ = menu_cnt_OPT * 16;

	flashing_cursor(x_, y_, page);
	if (menu.ResultForInputNum == input::ok) {
		uint8_t place = 1;
		if (!display.strnlen_lir((const char*)bufASCIIstr))
		goto skip_pr;

		if (menu_cnt_OPT == place++) {
			//==Яркость экрана==
			int n = strtoi_lir(bufASCIIstr);//atoi((char*)buf_str);
			if (n == 999) {  // сброс по watchdog
				cli();
				while (1);
			}
			n = (MAX_contr + 1) - n;
			if ((n >= MIN_contr) && (n <= MAX_contr))
			init.contrast_value = n;
			else {
				if (n < MIN_contr)
				init.contrast_value = MIN_contr;
				else
				init.contrast_value = MAX_contr;
			}
			//==
		}
		skip_pr:
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idOption);
		entry_OPT = UPDATE;
		init_cursor_pos();
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idOption);
		entry_OPT = UPDATE;
		init_cursor_pos();
	}
}

void Menu::init_menu_OPT(const unsigned char & page)
{
	display.clear_ram_null();
	entry_OPT = UPDATE;
}

void Menu::draw_menu_OPT(const unsigned char & page)
{
	const char *menu_table[10] = { 0 };
	uint8_t i = 0;
	//==конструктор меню
	for (uint8_t j = 0; j < 7;) {
		menu_table[i++] = sOption[j++];
	}
	//==
	#if (configUSE_PLC == 1)
	menu_table[i++] = PSTR(" Внешний ПЛК");
	#endif
	#if (configUSE_NCshell == 1)
	menu_table[i++] = PSTR(" Обновление преднабора");
	#endif
	//==размер меню
	menu_OPT_SZ = i - 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_OPT,page);
	//==
	entry_OPT = IDLE;
}

void Menu::draw_param_OPT(const unsigned char & page, int x, char align)
{
	const char* p;
	uint8_t place = 1;
	//==Яркость экрана
	int n = (MAX_contr + 1) - init.contrast_value;
	print_long_format(n, CPrg::FORMAT_END, page, x, 16 * place++, align, 0, 5); // значение контрастности
	//==
	//==Отображение времени
	if (init.to_see_time == 1)
		p = sYES;
	else
		p = sNO;
	display.print_flash_local(p, 0, x, 16 * place++, align);
	//==
	//==Скорость USB
	display.print_flash_local(sBaudrate[uart_usb.baudrate - 1], 0, x, 16 * place++, align);// почему  -1?
	//==
	//==Звук
	if (sound_and_other & 0x01)
		p = sNO;
	else
		p = sYES;
	display.print_flash_local(p, 0, x, 16 * place++, align);
	//==
	//==Обновление координат
	if (get_Disp_upd())
		 p = sFast;//PSTR("быстро  ");
	else
		 p = sSlow;//PSTR("медленно");
	display.print_flash_local(p, 0, x, 16 * place++, align);
	//==
	//==Захват ошибки датчика
	if (sound_and_other & 0x04)
		p = sNO;
	else
		p = sYES;
	display.print_flash_local(p, 0, x, 16 * place++, align);
	//==

	
	#if (configUSE_PLC == 1)
	//==Внешний ПЛК
	if(sound_and_other & 0x08)
		p = sNO;
	else
		p = sYES;
	display.print_flash_local(p, 0, x, 16 * place++, align);
	//==
	#endif
	#if (configUSE_NCshell == 1)
	//==Обновление преднабора
	if(sound_and_other & 0x10)
		p = sNO;
	else
		p = sYES;
	display.print_flash_local(p, 0, x, 16 * place++, align);
	//==
	#endif

}

void Menu::menu_up_OPT()
{
	if (curItem->id == idOption) {
		menu_cnt_OPT--;
		if (menu_cnt_OPT <= 0)
		menu_cnt_OPT = menu_OPT_SZ;
		entry_OPT = UPDATE;
	}
}

void Menu::menu_down_OPT()
{
	if (curItem->id == idOption) {
		menu_cnt_OPT++;
		if (menu_cnt_OPT > menu_OPT_SZ)
		menu_cnt_OPT = 1;
		entry_OPT = UPDATE;
	}
}

void Menu::menu_enter_OPT()
{
	if (curItem->id == idOption) {
		uint8_t place = 1;
		//==Яркость экрана
		if (menu_cnt_OPT == place++) {
			init_cursor_pos(3);
			SET_C(idOptionInput);
			return;
		}
		//==
	}
}

void Menu::menu_stop_OPT()
{
	if (curItem->id == idOption) {
		entry_OPT = INIT;
		//сохранить настройки в EEPROM
		flash_prg.to_save_contrast_to_EEPROM(&init.contrast_value);                  //176
		flash_prg.to_save_to_see_time_to_EEPROM(&init.to_see_time);                   //177
		flash_prg.to_save_Bdrate_toEEPROM(&uart_usb.baudrate);                        //178
		flash_prg.to_save_Rejim_zaxvata_ref_metok_toEEPROM(&MsgKey.submode_mode_REF); //179 (параметр не нужный)
		flash_prg.to_save_sound();	//185										  
		SET_C(idDiag);
	}
}

void Menu::menu_left_OPT()
{
	if (curItem->id == idOption) {
		uint8_t place = 1;
		//==Яркость экрана
		if (menu_cnt_OPT == place++) {
			init.contrast_value++;
			if (init.contrast_value > MAX_contr)
			init.contrast_value = MIN_contr;
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Отображение времени
		if (menu_cnt_OPT == place++) {
			if (init.to_see_time == 1)
				init.to_see_time = 0;
			else
				init.to_see_time = 1;
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Скорость USB
		if (menu_cnt_OPT == place++) {
			ChangeSettings(uart_usb.baudrate);
			uart_usb.init_uart1_portD();
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Звук
		if (menu_cnt_OPT == place++) {
			change_sound();
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Обновление координат
		if (menu_cnt_OPT == place++) {
			change_speed_out_disp();
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Захват ошибки датчика
		if (menu_cnt_OPT == place++) {
			change_Zaxvat_oshibki_datchika();
			entry_OPT = UPDATE;
			return;
		}
		//==		
		#if (configUSE_PLC == 1)
		//== Внешний ПЛК
		if (menu_cnt_OPT == place++) {
			change_Extern_PLC();			
			entry_OPT = UPDATE;
			return;
		}
		#endif
		//==
		//==Очистка Преднабора
		if (menu_cnt_OPT == place++) {
			change_Erase_Prednabor();
			entry_OPT = UPDATE;
			return;
		}
		//==
		
	}
}

void Menu::menu_right_OPT()
{
	if (curItem->id == idOption) {
		uint8_t place = 1;
		//==Яркость экрана
		if (menu_cnt_OPT == place++) {
			init.contrast_value--;
			if (init.contrast_value < MIN_contr)
			init.contrast_value = MAX_contr;
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Отображение времени
		if (menu_cnt_OPT == place++) {
			if (init.to_see_time == 1)
			init.to_see_time = 0;
			else
			init.to_see_time = 1;
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Скорость USB
		if (menu_cnt_OPT == place++) {
			ChangeSettings(uart_usb.baudrate);
			uart_usb.init_uart1_portD();
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Звук
		if (menu_cnt_OPT == place++) {
			change_sound();
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Обновление координат
		if (menu_cnt_OPT == place++) {
			change_speed_out_disp();
			entry_OPT = UPDATE;
			return;
		}
		//==
		//==Захват ошибки датчика
		if (menu_cnt_OPT == place++) {
			change_Zaxvat_oshibki_datchika();
			entry_OPT = UPDATE;
			return;
		}
		//==
		
		#if (configUSE_PLC == 1)
		//== Внешний ПЛК
		if (menu_cnt_OPT == place++) {
			change_Extern_PLC();
			entry_OPT = UPDATE;
			return;
		}
		#endif
		//==
		//==Очистка Преднабора
		if (menu_cnt_OPT == place++) {
			change_Erase_Prednabor();
			entry_OPT = UPDATE;
			return;
		}
		//==
		
		
	}
}

void Menu::actions_m_da(const unsigned char & page)
{
	if (entry_da == INIT)
	init_menu_da(page);
	if (entry_da == UPDATE) {
		draw_menu_da(page);
		draw_param_da(page);
	}
	if (entry_da == IDLE)
	return;
}

void Menu::init_menu_da(const unsigned char & page)
{
	display.clear_ram_null();
	entry_da = UPDATE;
}

void Menu::draw_menu_da(const unsigned char & page)
{
	const char *menu_table[6] = { 0 };
	//==конструктор меню
	menu_table[0] = sStrokaIndikacii;	
	menu_table[1] = PSTR("  1  ");	
	menu_table[2] = PSTR("  2  ");	
	menu_table[3] = PSTR("  3  ");	
	menu_table[4] = PSTR("  4  ");	
	menu_table[5] = nullptr;
	//==
	//==размер меню
	menu_da_SZ = 4;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_da,page);
	//==
	entry_da = IDLE;
}

void Menu::draw_param_da(const unsigned char & page)
{
	//uint8_t place = 1;
	for (int axis = 0; axis < N_AXIS; axis++)
	display.print_flash_local(sAxis_screen[Str[axis]], page, 10, 16 * (axis + 1));
	UpdateSpeedOnScreen();
	entry_da = IDLE;
}

void Menu::menu_up_da()
{
	if (curItem->id == idScreenOcb) {
		menu_cnt_da--;
		if (menu_cnt_da <= 0)
		menu_cnt_da = menu_da_SZ;
		entry_da = UPDATE;
	}
}

void Menu::menu_down_da()
{
	if (curItem->id == idScreenOcb) {
		menu_cnt_da++;
		if (menu_cnt_da > menu_da_SZ)
		menu_cnt_da = 1;
		entry_da = UPDATE;
	}
}

void Menu::menu_left_da()
{
	if (curItem->id == idScreenOcb) {
		direction = false;
		change_axisD();
		entry_da = UPDATE;
	}
}

void Menu::menu_right_da()
{
	if (curItem->id == idScreenOcb) {
		direction = true;
		change_axisD();
		entry_da = UPDATE;
	}
}

void Menu::menu_enter_da()
{
	if (curItem->id == idScreenOcb) {
		ChangeSpeedOnScreen();
		entry_da = UPDATE;
	}
}

void Menu::menu_stop_da()
{
	if (curItem->id == idScreenOcb) {
		//for (int i = 0; i < N_AXIS; i++)
		//	flash_prg.to_save_axis_param_to_EEPROM(i, (unsigned char*)&flash_prg.axis_cfg[i]);

		//flash_prg.to_write_to_eeprom_axis_is_chosen(&Lir.P2[0], &Lir.P2[1], &Lir.P2[2], &Lir.P2[3]);
		flash_prg.to_write_to_eeprom_axis_to_screen(&Lir.Str[0], &Lir.Str[1], &Lir.Str[2], &Lir.Str[3]);
		//flash_prg.to_save_type_of_axis(&Lir.P2[0], &Lir.P2[1], &Lir.P2[2], &Lir.P2[3]);
		flash_prg.to_save_speed_screen_param_to_EEPROM(reinterpret_cast<uint8_t*>(display.position_to_see)); 
		Lir.SET_C(idAllp);
		entry_da = INIT;
	}
}

void Menu::change_axisD()
{
	//==1
	if (menu_cnt_da == 1)
	change_axis_str(Str[0], direction);
	//==2
	if (menu_cnt_da == 2)
	change_axis_str(Str[1], direction);
	//==3
	if (menu_cnt_da == 3)
	change_axis_str(Str[2], direction);
	//==4
	if (menu_cnt_da == 4)
	change_axis_str(Str[3], direction);
}

void Menu::actions_m_pa(const unsigned char & page)
{
	if (entry_pa == INIT)
		init_menu_pa(page);
	if (entry_pa == UPDATE) {
		draw_menu_pa(page);
		draw_param_pa(page);
	}
	if (entry_pa == IDLE)
	return;
}

void Menu::init_menu_pa(const unsigned char & page)
{
	display.clear_ram_null();
	entry_pa = UPDATE;
}

void Menu::draw_menu_pa(const unsigned char & page)
{
	const char *menu_table[7] = { 0 };
	uint8_t i = 0;
	//==конструктор меню
	for (uint8_t j = 0; j < 5;) {
		menu_table[i++] = sPultOcb[j++];
	}
	//==
	#if (configUSE_Shadow_axis == 1)
	menu_table[i++] = PSTR(" Р2    - ");
	#endif
	//==размер меню
	menu_pa_SZ = i - 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_pa,page);
	//==
	entry_pa = IDLE;
}

void Menu::draw_param_pa(const unsigned char & page)
{
	//uint8_t place = 1;
	const char *pM_str;
	for(int axis = 0 ; axis < N_AXIS + N_SA;axis++){
			if(axis == SW_AXIS0)
				pM_str = sAxis_symbol_sw[P2[axis]];
			else
				pM_str = sAxis_symbol[P2[axis]];
			display.print_flash_local(pM_str, page, 20, 16 * (axis + 1));		
		}
	entry_pa = IDLE;
}

void Menu::menu_up_pa()
{
	if (curItem->id == idPultOcb) {
		menu_cnt_pa--;
		if (menu_cnt_pa <= 0)
		menu_cnt_pa = menu_pa_SZ;
		entry_pa = UPDATE;
	}
}

void Menu::menu_down_pa()
{
	if (curItem->id == idPultOcb) {
		menu_cnt_pa++;
		if (menu_cnt_pa > menu_pa_SZ)
		menu_cnt_pa = 1;
		entry_pa = UPDATE;
	}
}

void Menu::menu_left_pa()
{
	if (curItem->id == idPultOcb) {
	direction = false;
	change_axisP();
	entry_pa = UPDATE;
	}
}

void Menu::menu_right_pa()
{
	if (curItem->id == idPultOcb) {
	direction = true;
	change_axisP();
	entry_pa = UPDATE;
	}
}

void Menu::menu_stop_pa()
{
	if (curItem->id == idPultOcb) {	
		UpdateStr();
		flash_prg.to_write_to_eeprom_axis_to_screen(&Lir.Str[0], &Lir.Str[1], &Lir.Str[2], &Lir.Str[3]);
		flash_prg.to_save_type_of_axis(&Lir.P2[0], &Lir.P2[1], &Lir.P2[2], &Lir.P2[3]);
		flash_prg.to_set_var_for_angle_axis();
		for (int i = 0; i < N_AXIS; i++)
			flash_prg.to_save_axis_param_to_EEPROM(i, (unsigned char*)&flash_prg.axis_cfg[i]);
		flash_prg.to_write_to_eeprom_axis_is_chosen(&Lir.P2[0], &Lir.P2[1], &Lir.P2[2], &Lir.P2[3]);	
		
		//==сохраняем выбор скрытой оси
		#if (configUSE_Shadow_axis == 1)
		//save_Allp_param12_to_eeprom();
		flash_prg.write_eeprom(1212,sizeof(char),&P2[SW_AXIS0]);
		#endif
		//==
		Lir.SET_C(idAllp);
		entry_pa = INIT;
	}
}

void Menu::change_axisP()
{
	if (menu_cnt_pa >= 1 && menu_cnt_pa <= N_AXIS + N_SA){
		change_axis_name(menu_cnt_pa - 1);
		UpdateTypeAxis(menu_cnt_pa - 1);
		UpdateFormatAxis2(menu_cnt_pa - 1);	
	}
}

void Menu::actions_m_gs(const unsigned char & page)
{
	if (entry_gs == INIT)
	init_menu_gs(page);
	if (entry_gs == UPDATE){
		draw_menu_gs(page);
		draw_param_gs(page);
	}
	if (entry_gs == IDLE)
	return;
}

void Menu::init_menu_gs(const unsigned char & page)
{
	display.clear_ram_null();
	entry_gs = UPDATE;
}

void Menu::draw_menu_gs(const unsigned char & page)
{
#if (configUSE_NCshell == 0)
	const char *menu_table[8] = { 0 };
	uint8_t i = 0;
	//==конструктор меню
	for (uint8_t j = 0; j < 6;) {
		if (i == 3) {
			menu_table[i++] = sLine;
			continue;
		}
		menu_table[i++] = sAllpar[j++];
	}
	//==
	//==размер меню
	menu_gs_SZ = i - 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_gs,page);
	//==
	entry_gs = IDLE;
#endif

#if (configUSE_NCshell == 1)
	const char *menu_table[13] = { 0 };
	uint8_t i = 0;
	uint8_t j = 0;
	//==конструктор меню
	menu_table[i++] = sAllparNCShell[j++]; // ставим заголовок

	while (i < 12) {
		if (i == 3 || i == 6 || i == 8) {
			menu_table[i++] = sLine;
			continue;
		}
		menu_table[i++] = sAllparNCShell[j++];		
	}
	//==
	//==размер меню
	menu_gs_SZ = i - 1;
	//==
	//==вывод на экран
	Draw_menu(menu_table, menu_cnt_gs,page);
	//==
	entry_gs = IDLE;
#endif
}

void Menu::draw_param_gs(const unsigned char& page)
{
	#if (configUSE_NCshell == 1)
	const char *p;
	if (flash_prg._Allp_param2[0].VNE_DOPUSKA == 1 &&
	flash_prg._Allp_param2[0].VNE_DOPUSKA == 1 &&
	flash_prg._Allp_param2[0].VNE_DOPUSKA == 1 &&
	flash_prg._Allp_param2[0].VNE_DOPUSKA == 1)
	{
		p = PSTR("продолжить");
	}
	else
		p = PSTR("стоп      ");
	display.print_flash_local(p, 0, 15, 16 * 7);
	#endif
}

void Menu::menu_up_gs()
{
	#if (configUSE_NCshell == 0)
	if (curItem->id == idAllp) {
		menu_cnt_gs--;
		if(menu_cnt_gs == 3)
		menu_cnt_gs--;
		if (menu_cnt_gs <= 0)
		menu_cnt_gs = menu_gs_SZ;
		entry_gs = UPDATE;
	}
	#endif
	#if (configUSE_NCshell == 1)
	if (curItem->id == idAllp) {
		menu_cnt_gs--;
		if (menu_cnt_gs == 3 || menu_cnt_gs == 6 || menu_cnt_gs == 8)
		menu_cnt_gs--;
		if (menu_cnt_gs <= 0)
		menu_cnt_gs = menu_gs_SZ;
		
		entry_gs = UPDATE;
	}
	#endif
}

void Menu::menu_down_gs()
{
	#if (configUSE_NCshell == 0)
	if (curItem->id == idAllp) {
		menu_cnt_gs++;
		if (menu_cnt_gs == 3)
		menu_cnt_gs++;
		if (menu_cnt_gs > menu_gs_SZ)
		menu_cnt_gs = 1;
		entry_gs = UPDATE;
	}
	#endif
	#if (configUSE_NCshell == 1)
	if (curItem->id == idAllp) {
		menu_cnt_gs++;
		if (menu_cnt_gs == 3 || menu_cnt_gs == 6 || menu_cnt_gs == 8)
		menu_cnt_gs++;
		if (menu_cnt_gs > menu_gs_SZ)
		menu_cnt_gs = 1;
		
		entry_gs = UPDATE;
	}
	#endif
}

void Menu::menu_enter_gs()
{
	#if (configUSE_NCshell == 0)
	if (curItem->id == idAllp) {
		entry_gs = INIT;
		//==Оси-Измерительные каналы
		if (menu_cnt_gs == 1) {
			//MB("Оси - Измерительные каналы");
			SET_C(idPultOcb);
			return;
		}
		//==
		//==Оси-экран
		if (menu_cnt_gs == 2) {
			//MB("Оси - экран");
			SET_C(idScreenOcb);
			return;
		}
		//==
		//==СБРОС ПАРАМЕТРОВ
		if (menu_cnt_gs == 4) {
			//MB("СБРОС ПАРАМЕТРОВ");
			SET_C(idResetParametrs);
			ShowResetParmetrs();
			return;
		}
		//==
		//==СБРОС СМЕЩЕНИЙ
		if (menu_cnt_gs == 5) {
			//MB("СБРОС СМЕЩЕНИЙ");
			SET_C(idResetOffset);
			ShowResetOffset();
			return;
		}
		//==
		//==СБРОС
		if (menu_cnt_gs == 6) {
			//MB("СБРОС");
			// Softreset
			CCP = 0xD8; //Разрешение записи в защищённый регистр
			RST.CTRL = 0x01;
			return;
		}
		//==
	}
	#endif
	#if (configUSE_NCshell == 1)
	if (curItem->id == idAllp) {		
		//==Оси-Измерительные каналы
		if (menu_cnt_gs == 1) {
			//MB("Оси - Измерительные каналы");
			SET_C(idPultOcb);
			entry_gs = INIT;
			return;
		}
		//==
		//==Оси-экран
		if (menu_cnt_gs == 2) {
			//MB("Оси - экран");
			SET_C(idScreenOcb);
			entry_gs = INIT;
			return;
		}
		//==
		//==Конфигурация входов
		if (menu_cnt_gs == 4) {
			//MB("Конфигурация входов");
			//status_RAZRABOTKA();
			SET_C(idInGen);
			entry_gs = INIT;
			return;
		}
		//==
		//==Конфигурация выходов
		if (menu_cnt_gs == 5) {
			//MB("Конфигурация выходов");
			entry_gs = INIT;
			SET_C(idOutGen);
			return;
		}
		//==
		//==Вне допуска - продолжить
		if (menu_cnt_gs == 7) {
			for (int i = 0; i < N_AXIS; i++)
				flash_prg._Allp_param2[i].VNE_DOPUSKA ^= 1;
			entry_gs = UPDATE;
			return;
		}
		//==
		//==СБРОС ПАРАМЕТРОВ
		if (menu_cnt_gs == 9) {
			//MB("СБРОС ПАРАМЕТРОВ");
			SET_C(idResetParametrs);
			ShowResetParmetrs();
			entry_gs = INIT;
			return;
		}
		//==
		//==СБРОС СМЕЩЕНИЙ
		if (menu_cnt_gs == 10) {
			//MB("СБРОС СМЕЩЕНИЙ");
			SET_C(idResetOffset);
			ShowResetOffset();
			entry_gs = INIT;
			return;
		}
		//==
		//==СБРОС
		if (menu_cnt_gs == 11) {
			//MB("СБРОС");
			CCP = 0xD8; //Разрешение записи в защищённый регистр
			RST.CTRL = 0x01;
			return;
		}
		//==
	}
	#endif
}

void Menu::menu_stop_gs()
{
	if (curItem->id == idAllp) {
		entry_gs = INIT;
		#if (configUSE_NCshell == 1)
		flash_prg.write_eeprom(316,sizeof(flash_prg._Allp_param2),&flash_prg._Allp_param2);// Параметры оси - Управление
		#endif
		SET_C(idDiag);
	}
}
#if (configUSE_NCshell == 1)
void Menu::menu_left_gs()
{
	if (curItem->id == idAllp) {
		//==Вне допуска - продолжить
		if (menu_cnt_gs == 7) {
			for(int i = 0 ; i < N_AXIS;i++)
			flash_prg._Allp_param2[i].VNE_DOPUSKA ^= 1;
			entry_gs = UPDATE;
			return;
		}
		//==
	}
}

void Menu::menu_right_gs()
{
	if (curItem->id == idAllp) {
		//==Вне допуска - продолжить
		if (menu_cnt_gs == 7) {
			for (int i = 0; i < N_AXIS; i++)
			flash_prg._Allp_param2[i].VNE_DOPUSKA ^= 1;
			entry_gs = UPDATE;
			return;
		}
		//==
	}
}
#endif
void Menu::actions_m_BEGN(const unsigned char & page)
{
	if(entry_BEGN == INIT)
		init_menu_BEGN(page);
	else if(entry_BEGN == UPDATE)
		draw_menu_BEGN(page);
	else if(entry_BEGN == IDLE)
		return;
}

void Menu::init_menu_BEGN(const unsigned char & page)
{
	display.clear_ram_null(page);
	
	mOFFSNPassG53Ok = false;
	MsgKey.common_go_to_mode_begin();
	cli();
	ref_.disable_PA0_int_1_corr();
	ref_.disable_PB5_int_0();
	ref_.disable_PA1_int_2();
	ref_.disable_PB3_int_3();
	sei();
	entry_BEGN = UPDATE;
}

void Menu::draw_menu_BEGN(const unsigned char & page)
{
	entry_BEGN = IDLE;
}

void Menu::actions_m_DIAG(const unsigned char & page)
{
	if(entry_DIAG == INIT)
		init_menu_DIAG(page);
	else if(entry_DIAG == UPDATE)
		draw_menu_DIAG(page);
	else if(entry_DIAG == IDLE)
		return;
}


void Menu::actions_m_DIAG_input(const unsigned char & page)
{
	int x_, y_;
	x_ = 12;

	x_ = strlen_P(sPlease_input_pass) + 1;
	y_ = 16 * 13;

	flashing_cursor(x_, y_, page);
	if (menu_cnt_DIAG == 0 || menu_cnt_DIAG == 1) {
		if (cursor_pos > 4) {
			if (bufASCIIstr[0] == '1' && bufASCIIstr[1] == '6' && bufASCIIstr[2] == '3' && bufASCIIstr[3] == '8' && bufASCIIstr[4] == '4')
			{
				mDiagPassOk = true;
				if (menu_cnt_DIAG == 0) {
					//==Общие параметры==
					entry_DIAG = INIT;
					SET_C(idAllp);				
					//==
				}
				else if (menu_cnt_DIAG == 1) {
					//==Параметры осей==
					entry_DIAG = INIT;
					SET_C(idAxislist);
					//==
				}
			}
			else {
				display.print_flash_local(sAccess_denided, page, x_, y_);
				Sleep(500);
			}
			display.print_flash_local(sProbel10, page, x_, y_);
			init_cursor_pos();
		}
	}

	if (menu.ResultForInputNum == input::ok) {
		display.print_flash_local(sAccess_denided, page, x_, y_);
		Sleep(500);
		display.print_flash_local(sProbel10, page, x_, y_);
		init_cursor_pos();
	}
	if (menu.ResultForInputNum == input::cancel_) {
		display.print_flash_local(sProbel21, page, 0, y_);
		SET_C(idDiag);
		init_cursor_pos();
	}
}

void Menu::init_menu_DIAG(const unsigned char & page)
{
	display.clear_ram_null();
	strfunc_hand = N_STR;//сброс переменной
	updENC_error();
	entry_DIAG = UPDATE;
}

void Menu::draw_menu_DIAG(const unsigned char & page)
{
	const char *menu_table[14] = { 0 };
	#if (configUSE_NCshell == 1) 
		#if (configUSE_menuItem_Correction_and_Izmerenia == 1) // УПР + КОРРЕКЦИЯ
		menu_table[0] = sDiag[0];
		menu_table[1] = sDiag[1];
		menu_table[2] = sLine;
		menu_table[3] = sDiag[2];
		menu_table[4] = sDiagCorrInstrument;
		menu_table[5] =	sDiag[3];
		menu_table[6] =	sDiag[4];
		menu_table[7] = sDiag[5];
		menu_table[8] = sLine;
		menu_table[9] = sDiag[6];
		menu_table[10] = sDiag[7];
		menu_table[11] = sDiag[8];
		menu_table[12] = sDiag[9];
		menu_DIAG_SZ = 12;
		#else // УПР
		menu_table[0] = sDiag[0];
		menu_table[1] = sDiag[1];
		menu_table[2] = sLine;
		menu_table[3] = sDiag[2];
		menu_table[4] = sDiagCorrInstrument;
		menu_table[5] = sDiag[5];
		menu_table[6] = sLine;
		menu_table[7] = sDiag[6];
		menu_table[8] = sDiag[7];
		menu_table[9] = sDiag[8];
		menu_table[10] = sDiag[9];
		menu_DIAG_SZ = 10;
		#endif
	#else
		#if (configUSE_menuItem_Correction_and_Izmerenia == 1) //ИНДИКАЦИЯ + КОРРЕКЦИЯ
			menu_table[0] = sDiag[0];
			menu_table[1] = sDiag[1];
			menu_table[2] = sDiag[2];
			menu_table[3] = sDiag[3];
			menu_table[4] = sDiag[4];
			menu_table[5] = sDiag[5];
			menu_table[6] = sLine;
			menu_table[7] = sDiag[6];
			menu_table[8] = sDiag[7];
			menu_table[9] = sDiag[8];
			menu_table[10] = sDiag[9];
			menu_DIAG_SZ = 10;
		#else //ИНДИКАЦИЯ
			menu_table[0] = sDiag[0];
			menu_table[1] = sDiag[1];
			menu_table[2] = sDiag[2];
			menu_table[3] = sDiag[5];
			menu_table[4] = sLine;
			menu_table[5] = sDiag[6];
			menu_table[6] = sDiag[7];
			menu_table[7] = sDiag[8];
			menu_table[8] = sDiag[9];
			menu_DIAG_SZ = 8;
		#endif
	#endif
	//== вывод на экран готового меню
	Draw_menu(menu_table, menu_cnt_DIAG,page);
	//==
	entry_DIAG = IDLE;
}

void Menu::menu_up_DIAG()
{
	if (curItem->id == idDiag) {
		menu_cnt_DIAG--;
		#if (configUSE_NCshell == 1)
			#if (configUSE_menuItem_Correction_and_Izmerenia == 1) // УПР + КОРРЕКЦИЯ
			skip_menu_item(0,menu_cnt_DIAG,8);
			skip_menu_item(0,menu_cnt_DIAG,2);
			#else // УПР
			skip_menu_item(0,menu_cnt_DIAG,6);
			skip_menu_item(0,menu_cnt_DIAG,2);
			#endif
		#else
			#if (configUSE_menuItem_Correction_and_Izmerenia == 1) //ИНДИКАЦИЯ + КОРРЕКЦИЯ
				skip_menu_item(0,menu_cnt_DIAG,6);
			#else //ИНДИКАЦИЯ
				skip_menu_item(0,menu_cnt_DIAG,4);
			#endif
		#endif
		if (menu_cnt_DIAG < 0)
			menu_cnt_DIAG = menu_DIAG_SZ;
		entry_DIAG = UPDATE;
	}
}

void Menu::menu_down_DIAG()
{
	if (curItem->id == idDiag) {
		menu_cnt_DIAG++;
		#if (configUSE_NCshell == 1) 
			#if (configUSE_menuItem_Correction_and_Izmerenia == 1) // УПР + КОРРЕКЦИЯ
			skip_menu_item(1,menu_cnt_DIAG,2);
			skip_menu_item(1,menu_cnt_DIAG,8);
			#else // УПР
			skip_menu_item(1,menu_cnt_DIAG,2);
			skip_menu_item(1,menu_cnt_DIAG,6);
			#endif
		#else
			#if (configUSE_menuItem_Correction_and_Izmerenia == 1) //ИНДИКАЦИЯ + КОРРЕКЦИЯ
				skip_menu_item(1,menu_cnt_DIAG,6);
			#else //ИНДИКАЦИЯ
				skip_menu_item(1,menu_cnt_DIAG,4);
			#endif
		#endif
		if (menu_cnt_DIAG > menu_DIAG_SZ)
			menu_cnt_DIAG = 0;
		entry_DIAG = UPDATE;
	}
}

void Menu::menu_enter_DIAG()
{		 
	if (curItem->id == idDiag) {
		uint8_t place = 0;		
		//==Общие параметры==
		if (menu_cnt_DIAG == place++) {
			if (!mDiagPassOk) {
				entry_DIAG = IDLE;
				display.print_flash_local(sPlease_input_pass, 0, 1, 16 * 13);
				init_cursor_pos();
				SET_C(idDiagInput);
				entry_DIAG = INIT;
			}
			else {
				SET_C(idAllp);
				entry_DIAG = INIT;
			}
			return;
		}
		//==
		//==Параметры осей==
		if (menu_cnt_DIAG == place++) {
			if (!mDiagPassOk) {
				entry_DIAG = IDLE;
				display.print_flash_local(sPlease_input_pass, 0, 1, 16 * 13);
				init_cursor_pos();
				SET_C(idDiagInput);
				entry_DIAG = INIT;
			}
			else {
				SET_C(idAxislist);
				entry_DIAG = INIT;
			}
			return;
		}
		//==
		#if (configUSE_NCshell == 1)
		place++; // пропуск line
		#endif
		//== Смещения координат
		if (menu_cnt_DIAG == place++) {
			SET_C(idCoordOffset);
			entry_DIAG = INIT;
			return;
		}
		//==
		#if (configUSE_NCshell == 1)
		//== Коррекция на инструмент
		if (menu_cnt_DIAG == place++) {
			SET_C(idInstrC);
			entry_DIAG = INIT;
			return;
		}
		//==
		#endif
		#if (configUSE_menuItem_Correction_and_Izmerenia == 1)
		//== Коррекция
		if (menu_cnt_DIAG == place++) {
			SET_C(idCorr);
			entry_DIAG = INIT;
			return;
		}
		//==

		//== Измерения
		if (menu_cnt_DIAG == place++) {
			SET_C(idIzmer);
			entry_DIAG = INIT;
			return;
		}
		//==
		#endif
		//== Настройки
		if (menu_cnt_DIAG == place++) {
			SET_C(idOption);
			entry_DIAG = INIT;
			return;
		}
		//==
		place++; // пропуск line
		//== Режим обмена с ПК
		if (menu_cnt_DIAG == place++) {
			entry_DIAG = INIT;
			SET_C(idLinkPK);
			init_menu_LINPK(0);
			return;
		}
		//==

		//== Режим отладки
		if (menu_cnt_DIAG == place++) {
			SET_C(idDebugMode);
			entry_DIAG = INIT;
			return;
		}
		//==

		//== Калькулятор
		if (menu_cnt_DIAG == place++) {
			SET_C(idCalcfrom_menu);
			entry_DIAG = INIT;
			return;
		}
		//==

		//== О программе
		if (menu_cnt_DIAG == place++) {
			entry_DIAG = INIT;
			SET_C(idAbout);
			init_menu_ABOU(0);			
			return;
		}
		//==
	}
}

void Menu::menu_stop_DIAG()
{
	if(curItem->id == idDiag){
		entry_DIAG = INIT;		
		entry_BEGN = INIT;
		#if (configUSE_NCshell == 1)
		//==сброс всех выходов в ноль
		rst_outP();
		//==
		#endif
		SET_C(idBegin);
		}
}


void Menu::actions_cntr_pr(const unsigned char & page)
{
	if(entry_cntr_pr == INIT)
		init_m_cntr_pr(page);
	if(entry_cntr_pr == UPDATE){
		draw_m_cntr_pr(page);
		draw_param_cntr_pr(page,25,LEFT_ALIGN);  // 38 Right Align
	}
	if(entry_cntr_pr == IDLE)
		return;
}

void Menu::actions_Cntrl_input_pr(const unsigned char & page)
{
	int x_,y_;
	x_ = 12;
	
	if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::TYPE::CIRCLE)
		x_ = strlen_P(sAxisOpt[menu_cntr_cnt_pr + 1]) + 1;
	else
		x_ = strlen_P(sAxisOpt[menu_cntr_cnt_pr]) + 1;

	y_ = menu_cntr_cnt_pr  * 16;

	flashing_cursor(x_,y_,page);
	if (menu.ResultForInputNum == input::ok){
		uint8_t place = 4;
		if(!display.strnlen_lir((const char*)bufASCIIstr))
			goto skip_pr;
		
		if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::TYPE::CIRCLE)
			place--;
			if(menu_cntr_cnt_pr == place++){
				//==Коэффициент==
				coeff.translate_((char*)bufASCIIstr,IN_BUF_SZ);
				coeff.set_axis_coeff(flash_prg.ch,coeff.data,coeff.n_after_dot);
				//coeff.new_coeff_[flash_prg.ch] = CDouble::COEFF((char*)bufASCIIstr,IN_BUF_SZ);
				flash_prg.check_coeff(flash_prg.ch);
				flash_prg.check_luft(flash_prg.ch,CPrg::GAP::NO_SAVE_GAP_TO_EEPROM);
				//==
			}
			if(menu_cntr_cnt_pr == place++){
				//==Люфт==
				long gap = String_to_bin(bufASCIIstr,flash_prg.ch);
				flash_prg.luft_limit_after_input(flash_prg.ch,&gap);
				coeff.answer = coeff.div(flash_prg.ch,&gap);

				if(coeff.answer == CDouble::MULT_OR_DIV_ANSWER::OK)
					coeff.set_axis_gap(flash_prg.ch,gap);
				else
					coeff.set_axis_gap(flash_prg.ch,0);

				flash_prg.axis_cfg[flash_prg.ch].gap=coeff.new_gap[flash_prg.ch].binary;
				//==
			}
		skip_pr:
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idIndicOpt);
		entry_cntr_pr = UPDATE;
		init_cursor_pos();
	}
	if (menu.ResultForInputNum == input::cancel_){
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idIndicOpt);
		entry_cntr_pr = UPDATE;
		init_cursor_pos();
	}
}

void Menu::init_m_cntr_pr(const unsigned char & page)
{
	display.clear_ram_null();
	show_axis_above_menu2(13);
	entry_cntr_pr = UPDATE;
}
/*
создание меню с переменным размером строк во времени выполнения
*/
void Menu::draw_m_cntr_pr(const unsigned char & page)
{	
	const char *menu_table[15] = {0};
	uint8_t i = 0;
	//==создаем меню
	for (uint8_t j = 0; j < 14; i++,j++) {
		if (i == 2 && flash_prg.axis_cfg[flash_prg.ch].type == CPrg::TYPE::CIRCLE)
			j++;
		menu_table[i] = sAxisOpt[j];
		if(j == 13 && abs_ENC[flash_prg.ch])
			menu_table[i] = PSTR(" Настройки датчика");
	}
	//==
	//== вычисляем размер меню
	menu_cntr_SZ = i - 1;
	if (menu_cntr_cnt_pr > menu_cntr_SZ)
		menu_cntr_cnt_pr = menu_cntr_SZ;
	//==
	//== выводим меню на экран
	Draw_menu(menu_table, menu_cntr_cnt_pr,page);
	//==
	entry_cntr_pr = IDLE;
}

void Menu::draw_param_cntr_pr(const unsigned char & page,int x,char align)
{
	uint8_t place = 1;
	const char *p;
	//==Формат==
	print_long_format(0,get_format_from_ch(flash_prg.ch),page,x,16 * place++,align,0,10);
	//==

	if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::TYPE::LINEAR){
		//==Токарный режим==
		if (flash_prg.axis_cfg[flash_prg.ch].r_d==CPrg::RADIUS)
			p = sRadius;//PSTR("радиус ");
		else
			p = sDiametr;//PSTR("диаметр");
		display.print_flash_local(p,0,x,16 * place++,align);
		//==
	}
	//==Направление==
	if (flash_prg.axis_cfg[flash_prg.ch].direction==CPrg::PLUS)
		p = PSTR("+");
	else 
		p = PSTR("-");
	display.print_flash_local(p,0,x,16 * place++,align);
	//==

	//==Коэффициент==
	display.print_coeff(coeff.new_coeff_[flash_prg.ch].binary,
	coeff.new_coeff_[flash_prg.ch].n_after_dot,0,x,16 * place++,align,11);
	//==

	//==Люфт==
	long tmp=flash_prg.axis_cfg[flash_prg.ch].gap;	
	coeff.mult(flash_prg.ch,&tmp);
	Lir.print_long_format(tmp,flash_prg.axis_cfg[flash_prg.ch].format,0,x,16 * place++,align,0,15);

	//==

	//==Контроль датчика==
	if (flash_prg.axis_cfg[flash_prg.ch].encoder_fault == CPrg::YES_FAULT)
		p = sYES;
	else 
		p = sNO;
	display.print_flash_local(p,0,x,16 * place++,align);
	//==

	//==Сохранение счетчиков==
	if (flash_prg.axis_cfg[flash_prg.ch].save == CPrg::NO_SAVE)
		p = sNO;
	else 
		p = sYES;
	display.print_flash_local(p,0,x,16 * place++,align);
	//==

	//==Отображение скорости==
	if (flash_prg.axis_cfg[flash_prg.ch].readout_speed == CPrg::YES_SPEED)
		 p = sYES;
	else
		 p = sNO;
	display.print_flash_local(p,0,x,16 * place++,align);
	//==

	//==Зона референтной метки==
	flash_prg.show_P(flash_prg.axis_cfg[flash_prg.ch].ref_pulse_zone,IN_P, x, 16 * place++, align); 
	//==

	//==Внешнее обнуление==
	flash_prg.show_P(flash_prg.axis_cfg[flash_prg.ch].extern_to_zero,IN_P, x, 16 * place++, align);
	//==

	//==Интервал измер.скорости==
	show_interval_izmer_speed(flash_prg.ch,x,16 * place++,align);
	//==

	//==Коррекция==
	if(flash_prg.axis_cfg[flash_prg.ch].is_correction == CPrg::YES)
		p = sYES;
	else
		p = sNO;
	display.print_flash_local(p,0,x,16 * place++,align);
	//==

	//==Кодированные метки==
	if(!abs_ENC[flash_prg.ch]){
		if(flash_prg.axis_cfg[flash_prg.ch].is_coded_ref == CPrg::YESM)
			p = sYES;
		else
			p = sNO;
		display.print_flash_local(p,0,x,16 * place++,align);
		}
	//==

}

void Menu::m_up_cntr_pr()
{
	if(curItem->id == idIndicOpt){
		menu_cntr_cnt_pr--;
		if(menu_cntr_cnt_pr <= 0)
			menu_cntr_cnt_pr = menu_cntr_SZ;
		entry_cntr_pr = UPDATE;
	}
}

void Menu::m_down_cntr_pr()
{
	if(curItem->id == idIndicOpt){
		menu_cntr_cnt_pr++;
		if(menu_cntr_cnt_pr > menu_cntr_SZ)
			menu_cntr_cnt_pr = 1;
		entry_cntr_pr = UPDATE;
	}
}

void Menu::m_F_cntr_pr()
{
	if(curItem->id == idIndicOpt){
		uint8_t place = 4;
		if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::TYPE::CIRCLE)
		place--;
		place++;
		place++;
		place++;
		place++;
		place++;
		//==Зона референтной метки==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_MP(flash_prg.axis_cfg[flash_prg.ch].ref_pulse_zone,IN_P);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Внешнее обнуление==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_MP(flash_prg.axis_cfg[flash_prg.ch].extern_to_zero,IN_P);
			entry_cntr_pr = UPDATE;
		}
	}
}

void Menu::m_enter_cntr_pr()
{
	if(curItem->id == idIndicOpt){
		uint8_t place = 4;
		if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::TYPE::CIRCLE)
			place--;
		//==Коэффициент==
		if(menu_cntr_cnt_pr == place++){ 
			init_cursor_pos();
			SET_C(idInputIndicOpt);
			return;
		}
		//==
		//==Люфт==
		if(menu_cntr_cnt_pr == place++){ 
			init_cursor_pos();
			CurFormatM = flash_prg.axis_cfg[flash_prg.ch].format;			
			SET_C(idInputIndicOpt);
			return;
		}
		//==
		place++;
		place++;
		place++;
		//==Зона референтной метки==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_inv_P(flash_prg.axis_cfg[flash_prg.ch].ref_pulse_zone);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Внешнее обнуление==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_inv_P(flash_prg.axis_cfg[flash_prg.ch].extern_to_zero);
			entry_cntr_pr = UPDATE;
		}
		//==
		place++;
		place++;
		#if (configUSE_ABS_Encoder == 1)
		//== Настройки датчика
		if(abs_ENC[flash_prg.ch]){
			if(menu_cntr_cnt_pr == place++){
				//ставим контекст меню
				SET_C(idOABS);
				entry_cntr_pr = INIT;
			}
		}
		//==
		#endif

	}
}

void Menu::m_stop_cntr_pr()
{
	if(curItem->id == idIndicOpt){
		//==сохранение в eeprom==
		flash_prg.to_save_axis_param_to_EEPROM(flash_prg.ch,(unsigned char*)&flash_prg.axis_cfg[flash_prg.ch]);
		flash_prg.to_save_coeff_to_EEPROM(flash_prg.ch,(unsigned char*)&coeff.new_coeff_[flash_prg.ch]);
		//==
		flash_prg.to_save_luft_to_RAM(flash_prg.ch,coeff.new_gap[flash_prg.ch].binary);		
		flash_prg.to_set_var_for_angle_axis();
		screen.to_convert_interval_for_speed_measure_to_digit();
		
		if(configUSE_NCshell == 1)
			SET_C(idChpa);
		else
			SET_C(idAxislist);
		entry_cntr_pr = INIT;
	}
}

void Menu::m_left_cntr_pr()
{
	if(curItem->id == idIndicOpt){
		uint8_t place = 1;
		//==Формат==

		if(menu_cntr_cnt_pr == place++){ 
			flash_prg.change_format_axis(&flash_prg.ch,0);
			flash_prg.check_luft(flash_prg.ch,CPrg::GAP::NO_SAVE_GAP_TO_EEPROM);
			entry_cntr_pr = UPDATE;
		}
		//==
		if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::TYPE::LINEAR){
			//==Токарный режим==
			if(menu_cntr_cnt_pr == place++){
				flash_prg.change_mode_axis(&flash_prg.ch);
				entry_cntr_pr = UPDATE;
			}
			//==
		}
		
		//==Направление==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_direction_axis(&flash_prg.ch);
			entry_cntr_pr = UPDATE;
		}
		//==
		place++;
		place++;
		//==Контроль датчика==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_encoder_fault_axis(&flash_prg.ch);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Сохранение счетчиков==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_save_axis(&flash_prg.ch);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Отображение скорости==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_readout_speed_axis(&flash_prg.ch);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Зона референтной метки==
		if(menu_cntr_cnt_pr == place++){
			if(configUSE_NO_input == 0)
			flash_prg.changeP(flash_prg.axis_cfg[flash_prg.ch].ref_pulse_zone,IN_P, 0);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Внешнее обнуление==
		if(menu_cntr_cnt_pr == place++){
			if(configUSE_NO_input == 0)
			flash_prg.changeP(flash_prg.axis_cfg[flash_prg.ch].extern_to_zero,IN_P, 0);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Интервал измер.скорости==
		if(menu_cntr_cnt_pr == place++){
			switch (flash_prg.axis_cfg[flash_prg.ch].interval_to_measure_speed)
			{
				case CPrg::RANGE::INTERVAL0_6:
					flash_prg.axis_cfg[flash_prg.ch].interval_to_measure_speed = CPrg::RANGE::INTERVAL0_3;
					break;
				case CPrg::RANGE::INTERVAL0_3:
					flash_prg.axis_cfg[flash_prg.ch].interval_to_measure_speed = CPrg::RANGE::INTERVAL0_1;
					break;
				case CPrg::RANGE::INTERVAL0_1:
					flash_prg.axis_cfg[flash_prg.ch].interval_to_measure_speed = CPrg::RANGE::INTERVAL0_6;
					break;
				default:
				break;
			}
		entry_cntr_pr = UPDATE;
		}
		//==

		//==Коррекция==
		if(menu_cntr_cnt_pr == place++){
			if(flash_prg.axis_cfg[flash_prg.ch].is_correction == CPrg::YES){
				flash_prg.axis_cfg[flash_prg.ch].is_correction = CPrg::NO;
				//во время переключений относительный счетчик остается со сдвинутой координатой
				//по причине смещения таблицы до коррекции в ней относительный счетчик такой же как и абсолютный
				//если мы отключаем коррекцию то относительный счетчик становиться отдельным
				//восстанавливаем координату
				mCounter[flash_prg.ch].rel = mCounter[flash_prg.ch].abs_pure;
				}
			else{
				flash_prg.axis_cfg[flash_prg.ch].is_correction = CPrg::YES;
				#if (configUSE_menuItem_Correction_and_Izmerenia == 0)
				flash_prg.axis_cfg[flash_prg.ch].is_correction = CPrg::NO;
				#endif
			}
		entry_cntr_pr = UPDATE;
		}
		//==

		//==Кодированные метки==
		if(menu_cntr_cnt_pr == place++){
			if(flash_prg.axis_cfg[flash_prg.ch].is_coded_ref == CPrg::YESM)
				flash_prg.axis_cfg[flash_prg.ch].is_coded_ref = CPrg::NOM;
			else
				flash_prg.axis_cfg[flash_prg.ch].is_coded_ref = CPrg::YESM;
		entry_cntr_pr = UPDATE;
		}
		//==
	}
}

void Menu::m_right_cntr_pr()
{
	if(curItem->id == idIndicOpt){
		uint8_t place = 1;
		//==Формат==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_format_axis(&flash_prg.ch,1);
			flash_prg.check_luft(flash_prg.ch,CPrg::GAP::NO_SAVE_GAP_TO_EEPROM);
			entry_cntr_pr = UPDATE;
		}
		//==
		if(flash_prg.axis_cfg[flash_prg.ch].type == CPrg::TYPE::LINEAR){
			//==Токарный режим==
			if(menu_cntr_cnt_pr == place++){
				flash_prg.change_mode_axis(&flash_prg.ch);
				entry_cntr_pr = UPDATE;
			}
			//==
		}

		//==Направление==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_direction_axis(&flash_prg.ch);
			entry_cntr_pr = UPDATE;
		}
		//==
		place++;
		place++;
		//==Контроль датчика==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_encoder_fault_axis(&flash_prg.ch);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Сохранение счетчиков==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_save_axis(&flash_prg.ch);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Отображение скорости==
		if(menu_cntr_cnt_pr == place++){
			flash_prg.change_readout_speed_axis(&flash_prg.ch);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Зона референтной метки==
		if(menu_cntr_cnt_pr == place++){
			if(configUSE_NO_input == 0)
			flash_prg.changeP(flash_prg.axis_cfg[flash_prg.ch].ref_pulse_zone,IN_P,1);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Внешнее обнуление==
		if(menu_cntr_cnt_pr == place++){
			if(configUSE_NO_input == 0)
			flash_prg.changeP(flash_prg.axis_cfg[flash_prg.ch].extern_to_zero,IN_P, 1);
			entry_cntr_pr = UPDATE;
		}
		//==

		//==Интервал измер.скорости==
		if(menu_cntr_cnt_pr == place++){
			switch (flash_prg.axis_cfg[flash_prg.ch].interval_to_measure_speed)
			{
				case CPrg::RANGE::INTERVAL0_6:
				flash_prg.axis_cfg[flash_prg.ch].interval_to_measure_speed = CPrg::RANGE::INTERVAL0_1;
				break;
				case CPrg::RANGE::INTERVAL0_1:
				flash_prg.axis_cfg[flash_prg.ch].interval_to_measure_speed = CPrg::RANGE::INTERVAL0_3;
				break;
				case CPrg::RANGE::INTERVAL0_3:
				flash_prg.axis_cfg[flash_prg.ch].interval_to_measure_speed = CPrg::RANGE::INTERVAL0_6;
				break;
				default:
				break;
			}
		entry_cntr_pr = UPDATE;
		}
		//==

		//==Коррекция==
		if(menu_cntr_cnt_pr == place++){
			if(flash_prg.axis_cfg[flash_prg.ch].is_correction == CPrg::YES)
				{
					flash_prg.axis_cfg[flash_prg.ch].is_correction = CPrg::NO;
					//во время переключений относительный счетчик остается со сдвинутой координатой
					//по причине смещения таблицы до коррекции в ней относительный счетчик такой же как и абсолютный
					//если мы отключаем коррекцию то относительный счетчик становиться отдельным
					//восстанавливаем координату
					mCounter[flash_prg.ch].rel = mCounter[flash_prg.ch].abs_pure;
				}
			else{
				flash_prg.axis_cfg[flash_prg.ch].is_correction = CPrg::YES;
				#if (configUSE_menuItem_Correction_and_Izmerenia == 0)
				flash_prg.axis_cfg[flash_prg.ch].is_correction = CPrg::NO;
				#endif
			}
		entry_cntr_pr = UPDATE;
		}
		//==

		//==Кодированные метки==
		if(menu_cntr_cnt_pr == place++){
			if(flash_prg.axis_cfg[flash_prg.ch].is_coded_ref == CPrg::YESM)
				flash_prg.axis_cfg[flash_prg.ch].is_coded_ref = CPrg::NOM;
			else
				flash_prg.axis_cfg[flash_prg.ch].is_coded_ref = CPrg::YESM;
		entry_cntr_pr = UPDATE;
		}
		//==
	}
}


void Menu::actions_OABS(const unsigned char & page)
{
	if(entry_OABS == INIT)
		init_OABS(page);
	if(entry_OABS == UPDATE){
		draw_menu_OABS(page);
		draw_param_OABS(page,25,LEFT_ALIGN);  // 38 Right Align
	}
	if(entry_OABS == IDLE){
		int i = 0;
		for (; i < 8;i++) {
			if(altera.counter[flash_prg.ch] & plb2l(i))
				display.put_char_local_independent('1', 0, 8 + (i - 0), 16 * 5);
			else
				display.put_char_local_independent('0', 0, 8 + (i - 0), 16 * 5);
		}
		for (; i < 16;i++) {
			if(altera.counter[flash_prg.ch] & plb2l(i))
				display.put_char_local_independent('1', 0, 8 + (i - 8), 16 * 6);
			else
				display.put_char_local_independent('0', 0, 8 + (i - 8), 16 * 6);
		}
		for (; i < 24;i++) {
			if(altera.counter[flash_prg.ch] & plb2l(i))
				display.put_char_local_independent('1', 0, 8 + (i - 16), 16 * 7);
			else
				display.put_char_local_independent('0', 0, 8 + (i - 16), 16 * 7);
		}
		for (; i < 32;i++) {
			if(altera.counter[flash_prg.ch] & plb2l(i))
				display.put_char_local_independent('1', 0, 8 + (i - 24), 16 * 8);
			else
				display.put_char_local_independent('0', 0, 8 + (i - 24), 16 * 8);
		}
	return;
	}
}

void Menu::actions_OABS_input(const unsigned char & page)
{

}

void Menu::init_OABS(const unsigned char & page)
{
	display.clear_ram_null();
	show_axis_above_menu2(17);
	entry_OABS = UPDATE;

	display.print_flash_local(PSTR("  0..7"),page,1,16 * 5);
	display.print_flash_local(PSTR(" 8..15"),page,1,16 * 6);
	display.print_flash_local(PSTR("16..23"),page,1,16 * 7);
	display.print_flash_local(PSTR("24..31"),page,1,16 * 8);
}

void Menu::draw_menu_OABS(const unsigned char & page)
{
	const char *menu_table[15] = {0};
	//==создаем меню
	menu_table[0] = PSTR(" НАСТРОЙКИ ДАТЧИКА");
	menu_table[1] = PSTR(" Кол-во бит");
	menu_table[2] = PSTR(" Бит ошибки");
	menu_table[3] = PSTR(" Частота");
	//==
	//== вычисляем размер меню
	menu_OABS_SZ = 1;
	if (menu_OABS_cnt > menu_OABS_SZ)
	menu_OABS_cnt = menu_OABS_SZ;
	//==
	//== выводим меню на экран
	Draw_menu(menu_table, menu_OABS_cnt,page);
	//==
	entry_OABS = IDLE;
}

void Menu::draw_param_OABS(const unsigned char & page,int x,char align)
{
	//== Кол - во бит
	Lir.print_long_format(flash_prg.axis_cfg2[flash_prg.ch].bit_enc,CPrg::FORMAT_END,page,x,16 * 1,align,0,2);
	//==
	//==Бит ошибки

	//==
	//== Частота опроса
	//if(flash_prg.axis_cfg2[flash_prg.ch].HZ_enc == 0xFF)
		display.print_flash_local(PSTR("250kHz"),page,x,16 * 3,align);
	//==
}

void Menu::m_up_OABS()
{
	if(curItem->id == idOABS){
		menu_OABS_cnt--;
		if(menu_OABS_cnt <= 0)
		menu_OABS_cnt = menu_OABS_SZ;
		entry_OABS = UPDATE;
	}
}

void Menu::m_down_OABS()
{
	if(curItem->id == idOABS){
		menu_OABS_cnt++;
		if(menu_OABS_cnt > menu_OABS_SZ)
		menu_OABS_cnt = 1;
		entry_OABS = UPDATE;
	}
}

void Menu::m_stop_OABS()
{
	if(curItem->id == idOABS){
		entry_OABS = INIT;
		flash_prg.write_eeprom(796,sizeof(flash_prg.axis_cfg2),&flash_prg.axis_cfg2);
		SET_C(idIndicOpt);
	}
}

void Menu::m_left_OABS()
{
	if(curItem->id == idOABS){
		if(menu_OABS_cnt == 1){
			flash_prg.axis_cfg2[flash_prg.ch].bit_enc--;
			if(flash_prg.axis_cfg2[flash_prg.ch].bit_enc <= 0 || flash_prg.axis_cfg2[flash_prg.ch].bit_enc == 255)
				flash_prg.axis_cfg2[flash_prg.ch].bit_enc = 32;
			entry_OABS = UPDATE;

			altera.set_ch_abs_bit(flash_prg.ch,flash_prg.axis_cfg2[flash_prg.ch].bit_enc);
		}
	}
}

void Menu::m_right_OABS()
{
	if(curItem->id == idOABS){
		if(menu_OABS_cnt == 1){
			flash_prg.axis_cfg2[flash_prg.ch].bit_enc++;
			if(flash_prg.axis_cfg2[flash_prg.ch].bit_enc > 32)
				flash_prg.axis_cfg2[flash_prg.ch].bit_enc = 1;
			entry_OABS = UPDATE;

			altera.set_ch_abs_bit(flash_prg.ch,flash_prg.axis_cfg2[flash_prg.ch].bit_enc);
		}
	}
}

/*
in long data                 - бинарные данные (дискреты)
in int format                - формат отображения (0.0,0.00,0.000,0.0000,0.0°,0.00°,0.000°,0.0000°,0.0000",0°00',0°00'00",...)
in const unsigned char& page - страница памяти в экране
in unsigned char x           - координата x на экране
in unsigned char y           - координата y на экране
in char align                - выравнивание текста с  0 -левого края или с  1 - правого края
in char zero                 - кол-во добавляемых незначащих нулей , только если format == CPrg::FORMAT::FORMAT_END
in char clear                - размер строки очищения перед выводом текста
in char cut_zero_r           - обрезаем незначащие нули после точки 0 - нет 1 - да
*/
void Menu::print_long_format(long data,int format,const unsigned char& page,unsigned char x,unsigned char y,
							 char align,char zero,char clear,char cut_zero_r)
{ 
#define SZ_STR 15
	unsigned char txt_out[SZ_STR] = {0};

	if(format == CPrg::FORMAT::LINEAR_INCH_4)
		coeff.div(255,&data,1,254,1);// делим

	display.itoa_quick_lir(&data,txt_out);

	if((format>=CPrg::FORMAT::LINEAR_3 && format<=CPrg::FORMAT::CIRCLE_4) ||
		format==CPrg::FORMAT::LINEAR_INCH_4 ||
		format==CPrg::FORMAT::LINEAR_6){
			display.formatting2((char*)txt_out,format);
			if(cut_zero_r)
				cut_zero(txt_out);				
		}

	if( format == CPrg::FORMAT::CIRCLE_SECUNDO || format == CPrg::FORMAT::CIRCLE_MINUTO || 
		format == CPrg::FORMAT::CIRCLE_SECUNDO_1)
		Lir.convert_long_to_secundo2(data,txt_out,format);

	//== добавляем незначащие нули
	if( format == CPrg::FORMAT::FORMAT_END && zero)
		Lir.add_filler(txt_out,SZ_STR,zero,'0');	
	//===
	
	if(align == LEFT_ALIGN){
		if(clear)
			Lir.add_filler(txt_out,SZ_STR,clear,' ',1);
		display.print_ram_local((char*)txt_out,page,x,y);
	}

	if(align == RIGHT_ALIGN){
		if(clear)
			Lir.add_filler(txt_out,SZ_STR,clear,' ',0);
		display.print_ram_local_RA((char*)txt_out,page,x,y);
	}
		
}

unsigned short Menu::crc16(unsigned char *data, unsigned short length)
{
	unsigned short maska = 0;
	unsigned short crc = ~0;
	unsigned char buf;

	while (length--) {
		buf = *data;
		for (int j = 0; j < 8; j++) {
			maska = (1 & crc) ^ (1 & buf);
			buf >>= 1; crc >>= 1;
			if (maska) crc ^= 0xa001;
		}
		data++;
	}
	//(crc >> 8) | (crc << 8); big endian
	return crc; //little endian
}

/*
const long &Xi - расстояние между двумя метками
int &ch - текущий выбранный канал
*/
void Menu::psevdo_abs_disp(long &Xinput,unsigned int &ch)
{
	long Xo = 0;
	
	cli();
	long Xi = Xinput; //расстояние между метками
	sei();
	
	if(!Xi)
		return;

	if(ch >= N_AXIS)
		return;
	//только линейные датчики для линейных осей
	//использование круговых датчиков с км на линеных осях не допускается
		if(Xi >= 8388608){
			Xi = 16777217 - Xi;
			Xi = -Xi;
		}
		else
			Xi++;

		int f = 1000;
		//==
		long k0 = 10000;
		long k1 = 10020;
		coeff.div(ch,&k0);
		coeff.div(ch,&k1);

		if (Xi > 0 && Xi < k0){   //< + алгоритм 1
			Xo = (k0 - Xi);
			coeff.mult(ch,&Xo);
			Xo *= f;
			}
		else if (Xi < -k0){ //> - алгоритм 1
			Xo = (-k1 - Xi);
			coeff.mult(ch,&Xo);
			Xo *= f;
			}
		else if (Xi > k0){  // < + алгоритм 2
			long Xo_0,Xo_1;
			Xo_0 = (Xi - k0);
			coeff.mult(ch,&Xo_0);
			Xo_1 = Xo_0 * f;
			Xo = Xo_0 + Xo_1 - 10000;
			}
		else if (Xi > -k0 && Xi < 0){ // > - алгоритм 2
			long Xo_0,Xo_1;
			Xo_0 = (Xi + k0);
			coeff.mult(ch,&Xo_0);
			Xo_1 = Xo_0 * f;
			Xo = Xo_0 + Xo_1 - 10000;
			}

		//==
		//==коэффициент 1,  10000
		/*long K = 1;
		if (Xi > 0 && Xi < 10000)
			Xo = (10000 / K - Xi) * K * f;
		else if (Xi < -10000)
			Xo = (-10020 / K - Xi) * K * f;
		else if (Xi > 10000)
			Xo = (Xi - 10000 / K) * K + (Xi - 10000 / K) * K * f - 10000;
		else if (Xi > -10000 && Xi < 0)
			Xo = (Xi + 10000 / K) * K + (Xi + 10000 / K) * K * f - 10000; */
		//coeff.mult(ch,&Xo);

	if(flash_prg.axis_cfg[ch].direction == CPrg::DIRECTION::MINES)
		Xo = -Xo;

	size_2r2[ch] = Xo;
	//display.offset_g53[ch]= Xo; //для теста
	//upd_ref_cnt[ch] = 1;
	ch = N_AXIS;

	Xinput = 0;
	return;
}

void Menu::read_cnt_simple(long incoming,const int &ch)
{
	//mCounter[ch].hw_cnt_prev = mCounter[ch].hw_cnt_cur;
	//mCounter[ch].hw_cnt_cur=*incoming;
	static long cur;
	static long prev;
	long delta;

	//mCounter[ch].delta=mCounter[ch].hw_cnt_cur-mCounter[ch].hw_cnt_prev;
	if(incoming >= 8388608){
		incoming = 16777218 - incoming;
		incoming = -incoming;
	}
	prev = cur;
	cur = incoming;
	delta = cur - prev;


	mCounter[ch].rel += delta ;
	//if(mem_2r){
	//mCounter[ch].rel = mem_2r;
	//mem_2r = 0;
	//}
	//mCounter[ch].rel += mem_2r;
	//mCounter[ch].abs = delta + mCounter[ch].abs;
	//mCounter[ch].abs_pure = incoming;
}

/*
функция возвращает номер канала на строке
*/
char Menu::get_ch_from_str(int s)
{
	for(int c = 0 ; c < N_AXIS;c++){
		if(P2[c] == Str[s])
			return c;
	}
	return N_AXIS;
}
/*
функция возвращает номер строки по каналу
*/
char Menu::get_str_from_ch(int ch)
{
	if(ch >N_AXIS)
		return N_STR;

	 unsigned int tmp = P2[ch];

	 for(unsigned int str = 0 ; str < N_STR;str++){
		if(tmp == Str[str])
			return str;
	 }

	 return N_STR;
}

/*
in  const int &AXIS - номер оси
out char            - формат оси
*/
char Menu::get_format(const int &AXIS)
{
	int channel = 0;

	if((unsigned int)AXIS == P2[0])
		channel = 0;
	if((unsigned int)AXIS == P2[1])
		channel = 1;
	if((unsigned int)AXIS == P2[2])
		channel = 2;
	if((unsigned int)AXIS == P2[3])
		channel = 3;

		return (char)flash_prg.axis_cfg[channel].format;
}
/*
Функция возвращает формат по коду ASCII символа оси
*/
char Menu::get_format_from_ASCII(const char &sym_fr)
{
	//==для I берется формат 1 оси в строке для J 2ая ось
	static char axiscmd[2];
	static signed char cnt;

	if(sym_fr == 'I' || sym_fr == 'J'){
		if(sym_fr == 'I')
			return get_format(axiscmd[0]);//const_cast<char&>(sym_fr) = axiscmd[0];
		else if(sym_fr == 'J')
			return get_format(axiscmd[1]);//const_cast<char&>(sym_fr) = axiscmd[1];
	}
	else{
		axiscmd[cnt++]= sym_fr;
		if(cnt > 1)
			cnt = 0;
	}
	//==
	for(int i = 0; i< no_axis;i++){
		if( sym_fr == pgm_read_byte_near(sAxis_screen[i] + 2)){
			return get_format(i);
		}
	}
	return 0;
}
/*
Функция возвращает канал по коду ASCII символа оси
*/
char Menu::get_channel_from_ASCII(const char &sym_fr)
{
	for(int i = 0; i< no_axis;i++){
		if( sym_fr == pgm_read_byte_near(sAxis_screen[i] + 2)){
			return get_channel(i);
		}
	}
	return N_AXIS;
}
/*
Функция возвращает ASCII символа оси по номеру канала
*/
char Menu::get_ASCII_from_channel(int ch)
{
	return pgm_read_byte_near(sAxis_screen[P2[ch]] + 2);
}

/*
in  const int &channel - канал
out char               - формат канала
*/
char Menu::get_format_from_ch(const int &channel)
{
	return (char)flash_prg.axis_cfg[channel].format;
}
/*
in  const int &AXIS - номер оси (X - D)
out char            - канал оси
*/
char Menu::get_channel(const int &AXIS)
{
	int channel = N_AXIS;

	if((unsigned int)AXIS == no_axis)
		return (char)channel;

	for(int i = 0 ; i < N_AXIS;i++){
		if((unsigned int)AXIS == P2[i])
			channel = i;
	}

	return (char)channel;
}

int Menu::get_ch_mcx514(char ch)
{
	if(ch == 0)
		return MCX514_AXIS_X;
	else if(ch == 1)
		return MCX514_AXIS_Y;
	else if(ch == 2)
		return MCX514_AXIS_Z;
	else if(ch == 3)
		return MCX514_AXIS_U;
	else if(ch == 4 && configUSE_Shadow_axis == 1)
		return MCX514_AXIS_V;
	else 
		return MCX514_AXIS_NONE;
}

char Menu::get_ch_fromaxis_mcx514(int axis, char ch)
{
	if((axis & MCX514_AXIS_X) && (ch == 0))
		return 0;
	else if((axis & MCX514_AXIS_Y) && (ch == 1))
		return 1;
	else if((axis & MCX514_AXIS_Z) && (ch == 2))
		return 2;
	else if((axis & MCX514_AXIS_U) && (ch == 3))
		return 3;
	else
		return N_AXIS;
}


void Menu::get_counters_rel(long* counter)
{
	cli();
	counter[0] = mCounter[0].rel;
	counter[1] = mCounter[1].rel;
	counter[2] = mCounter[2].rel;
	counter[3] = mCounter[3].rel;
	sei();
}

void Menu::set_counters_rel(const long* counter)
{
	cli();
	mCounter[0].rel = counter[0];
	mCounter[1].rel = counter[1];
	mCounter[2].rel = counter[2];
	mCounter[3].rel = counter[3];
	sei();
}

void Menu::reset_counters_rel()
{
	cli();
	mCounter[3].rel=0;
	mCounter[2].rel=0;
	mCounter[1].rel=0;
	mCounter[0].rel=0;
	sei();
}

/*
функция добавляет нули в строку слева
unsigned char *str   - указатель на буфер строки
const short sz_str   - размер буфера с строкой
const short cnt_zero - кол- во добавляемых незначащих нулей слева
*/
void Menu::add_zero_l(unsigned char *str,const short sz_str,const short cnt_zero)
{
	char n = mystr.strnlen_lir((char*)str);
	if (n) {
		if (cnt_zero >= n && ((cnt_zero + n) < sz_str)) {
			for (int i = n - 1; i < cnt_zero; ) {
				str[i + 1] = str[i];
				str[i] = '0';
				if (i)
					i--;
				else
					i = n++;
			}
		}
	}
}
/*
функция заполняет строчный массив размером sz_str добавляя к концу строки заполнитель
out unsigned char *str     - указатель на буфер строки
in  const short sz_str     - размер буфера со строкой
in  const short cnt        - кол- во заполнителя
in  char filler            - заполнитель
in  const char dir         - направление заполнения (по умолчанию 0 - влево)
*/
void Menu::add_filler(unsigned char * str, const short sz_str, const short cnt, const char filler,const char dir)
{
	signed char n = mystr.strnlen_lir((char*)str);
	if (!n) {
		for (int i = 0; i < cnt + 1; i++) {
			str[i] = filler;
			if (i == (sz_str - 1)) {
				str[i] = '\0';
				break;
			}
		}
		return;
	}

	if (dir) {
		if (cnt >= n ) {
			for (int i = n - 1; i < cnt; ) {
				if (n + 1 >= sz_str)
					break;
				str[i + 1] = filler;
				i = n++;
			}
			str[n] = '\0';
		}
	}
	else {
		if (cnt >= n ) {
			for (int i = n - 1; i < cnt; ) {
				if (n + 1 >= sz_str)
					break;
				str[i + 1] = str[i];
				str[i] = filler;
				if (i)
					i--;
				else
					i = n++;
			}
			str[n] = '\0';
		}
	}
}
/*
функция конвертирует из бинарного числа в строку с форматами 0°00'00",0°00',0°00'00.0"
in  unsigned long data - бинарное число (счетчик,...)
out unsigned char *str - указатель на строку с результатом
in  int format         - текущий формат(CIRCLE_MINUTO,CIRCLE_SECUNDO_1,CIRCLE_SECUNDO)
*/
void Menu::convert_long_to_secundo2(long data,unsigned char *str,int format)
{
	unsigned long grad;// = 0;
	unsigned long min;// = 0;
	unsigned long sec;// = 0;
	unsigned long tmp;//= 0;

	unsigned long mul = 1;
	char zero_sec = 1;
	char minus  = 0;

	unsigned char grad_str[10] = { 0 };
	unsigned char min_str[10] = { 0 };
	unsigned char sec_str[10] = { 0 };

	//format = CPrg::FORMAT::CIRCLE_SECUNDO;
	if (format == CPrg::FORMAT::CIRCLE_SECUNDO_1) {
		zero_sec = 2;
		mul = 10; //10
	}
	if (format == CPrg::FORMAT::CIRCLE_MINUTO)
		data = data * 60;

	if (data < 0) {
		data = -data;
		minus = 1;
	}

	//это не нужно так как сразу сюда приходит 1295999 или 12959999
	//data %= ((unsigned long)1296000 * mul);

	//if (data < 0)
	//	data = (1296000 * mul) + data;
    //===

	grad = data / (3600 * mul);
	tmp = grad * (3600 * mul);
	sec = data - tmp;
	min = sec / (60 * mul);
	tmp = min * (60 * mul);
	sec = sec - tmp;

	lltoa_lir(grad, grad_str);

	lltoa_lir(min, min_str);
	add_zero_l(min_str, 10, 1);

	lltoa_lir(sec, sec_str);
	add_zero_l(sec_str, 10, zero_sec);

	if (format == CPrg::FORMAT::CIRCLE_SECUNDO_1) {
		display.add_point(sec_str, 10, 3);
	}
	
	int n = display.strnlen_lir((const char*)grad_str);
		grad_str[n] = '°';
	n = display.strnlen_lir((const char*)min_str);
		min_str[n] = 1;//0x01'\'';
	n = display.strnlen_lir((const char*)sec_str);
		sec_str[n] = 2;//0x02'\"';

	if(minus)
		display.strncatsym_lir((char*)str, 15, '-');

	display.strncat_lir_offset((char*)str, 15, (char*)grad_str, minus);

	display.strncat_lir_offset((char*)str, 15, (char*)min_str, display.strnlen_lir((const char*)str));

	if (format != CPrg::FORMAT::CIRCLE_MINUTO)
		display.strncat_lir_offset((char*)str, 15, (char*)sec_str, display.strnlen_lir((const char*)str));
}
/*
 Внешнее обнуление - функция обнуляет относительный счетчик по входу
 in int data - считанные входа
*/
void Menu::extern_go_to_zero()
{
//от2.480 до 5.6 mks
	for(int c = 0; c < N_AXIS; c++){
		if(Lir.inP(flash_prg.axis_cfg[c].extern_to_zero) == 1)
			mCounter[c].rel = 0;
	}
}
/*
 Поиск реф метки по входу зоны РФ
 in int data - считанные входа
*/
void Menu::zonePM_switch_toref()
{
	char res;
	for(int c = 0; c < N_AXIS; c++){
		res = Lir.inP(flash_prg.axis_cfg[c].ref_pulse_zone);
		if(res == 0 || res == 1)
			start_measure_sw_to_ref(c,res);
	}
}
/*
включение поиска рефмтеки в режими от концевика
*/
void Menu::zone_limit_switch_toref()
{
#if (configUSE_NCshell == 1)
	for(int c = 0 ; c < N_AXIS;c++){
		if(Lir.inP(flash_prg.axis_cfg[c].ref_pulse_zone) != NO_P)// если есть зона реф метки то поиск метки в этом режиме не включается
			continue;
		if(display.ifSearchRef(c) && sts_c[c] && if_LimitPM(c)){						
			if(limit_sw[c]){
				if(ch_sw.plus & plb2(c) || ch_sw.minus & plb2(c)){ //включение только после съезда с концевика
					ref_.altera_off_int(c);
					ref_.ref_ref_count = 0;
				}
				else
					ref_.altera_on_int(c);
				}
			else{				
				ref_.altera_off_int(c);
				ref_.ref_ref_count = 0;
			}
		}
	}
#endif
}

/*
int ch - канал
bool res - вход TRUE/FALSE
если вход TRUE то говорим альтере что она может принимать реф метку
*/
void Menu::start_measure_sw_to_ref(int ch,bool res)
{	
		true_input[ch] = res;
//=========Поиск реф меток в зоне концевика=========================================
	if(Lir.ref_mode[ch] == SIMPLE_RM){
		if(ref_.enable_measure_int[ch] == 0){   
			if(res){
				ref_.altera_on_int(ch);
				#if (configUSE_NCshell == 1)
				if(sts_c[ch]){
					if(flash_prg._OUTAxis[ch].typeZ == CPrg::DISCRET)
						outABCD(flash_prg._Allp_param[ch].zone_PM, &flash_prg._OUTAxis[ch].Out_A,ch,1);
					else if(flash_prg._OUTAxis[ch].typeZ == CPrg::ANALOG)
						outAnalog(Spd_to_U(flash_prg._Allp_param6[ch].zone_PM,ch,_modals_cmd.G0),flash_prg._OUTAxis2[ch].AnalOut,flash_prg._OUTAxis2[ch].Polar,dir_exe[ch],1);
				}
				#endif
				}
			else{
				ref_.altera_off_int(ch);
				ref_.ref_ref_count = 0;
				#if (configUSE_NCshell == 1)
				if(sts_c[ch]){
					if(flash_prg._OUTAxis[ch].typeZ == CPrg::DISCRET)
						outABCD(flash_prg._Allp_param[ch].start_PM, &flash_prg._OUTAxis[ch].Out_A,ch,1);
					else if(flash_prg._OUTAxis[ch].typeZ == CPrg::ANALOG)
						outAnalog(Spd_to_U(flash_prg._Allp_param6[ch].start_PM,ch,_modals_cmd.G0),flash_prg._OUTAxis2[ch].AnalOut,
							flash_prg._OUTAxis2[ch].Polar,dir_exe[ch],1);
				}
				#endif
				}			
			return;
		}
	}
//==================================================================================

//=========Поиск реф меток для замера расстояния от концевика до реф метки==========
	if(Lir.ref_mode[ch] == DISTANCE_TO_SWITCH_RM){
			if(res && !trig_input[ch]){
				ref_.altera_on_int(ch);
				trig_input[ch] = 1;
				mCounter[ch].rel = 0;
			}

			if(!res){
				trig_input[ch] = 0;
				ref_.altera_off_int(ch);
			}
			// если
			if(ref_.ref_mark_measure[ch] && res){
				ref_.enable_measure_int[ch]=ref_.ref_mark_measure[ch]=false;
				ref_.measurement_is_ended=true;				
			}	
		}	
}
/*
функция для перехода выбранных осей в относительные координаты
const char &prim_axis - ось №1 которую надо перевести в относительную координату
const char &second_axis - ось №2 которую надо перевести в относительную координату
*/
void Menu::goto_Relative_coord(const char &prim_axis,const char &second_axis)
{
	unsigned char n_channel;
	n_channel = get_channel(prim_axis);
	
	if(n_channel<N_AXIS) {
		if(init.ref_mode[n_channel]==true)
		{
			init.ref_mode[n_channel]=false;
			ref_.ref_change_mode[n_channel]=false;
		}
	}

	n_channel = get_channel(second_axis);

	if(n_channel<N_AXIS) {
		if(init.ref_mode[n_channel]==true)
		{
			init.ref_mode[n_channel]=false;
			ref_.ref_change_mode[n_channel]=false;
		}
	}
}
/*
функция для обработки нажатий кнопок _key_BLANK1..4  в меню "ПРЕОБРАЗОВАТЕЛИ ПЕРЕМЕЩЕНИЯ"
int const char& BLANK - код нажатой кнопки
*/
void Menu::ref_to_ref_inEncoderMenu(const char& BLANK)
{
	if (Lir.curItem->id == idMenuEncoders)
	{					
		if(BLANK == _key_BLANK1 && P2[0] != no_axis && !Lir.abs_ENC[0]){
			MsgKey.to_count_pulses_from_ref_to_ref(0);
			}
		else if(BLANK == _key_BLANK2 && P2[1] != no_axis && !Lir.abs_ENC[1]){
			MsgKey.to_count_pulses_from_ref_to_ref(1);
			}
		else if(BLANK == _key_BLANK3 && P2[2] != no_axis && !Lir.abs_ENC[2]){
			MsgKey.to_count_pulses_from_ref_to_ref(2);
			}
		else if(BLANK == _key_BLANK4 && P2[3] != no_axis && !Lir.abs_ENC[3]){
			MsgKey.to_count_pulses_from_ref_to_ref(3);
			}
	}
}
/*
функция для обработки нажатий кнопок _key_BLANK1..4  в меню "ПРОСМОТР ВХОДОВ"
int const char& BLANK - код нажатой кнопки
*/
void Menu::input_to_ref_inViewinputsMenu(const char& BLANK)
{
	if (Lir.curItem->id == idViewInputs)
	{
		if(BLANK == _key_BLANK1 && P2[0] != no_axis && !(flash_prg.axis_cfg[0].ref_pulse_zone & YESNO_P_bm)){
			MsgKey.to_measure_distance_from_ref_to_switch(0);
		}
		else if(BLANK == _key_BLANK2 && P2[1] != no_axis && !(flash_prg.axis_cfg[1].ref_pulse_zone & YESNO_P_bm)){
			MsgKey.to_measure_distance_from_ref_to_switch(1);
		}
		else if(BLANK == _key_BLANK3 && P2[2] != no_axis && !(flash_prg.axis_cfg[2].ref_pulse_zone & YESNO_P_bm)){
			MsgKey.to_measure_distance_from_ref_to_switch(2);
		}
		else if(BLANK == _key_BLANK4 && P2[3] != no_axis && !(flash_prg.axis_cfg[3].ref_pulse_zone & YESNO_P_bm)){
			MsgKey.to_measure_distance_from_ref_to_switch(3);
		}
	} 
}
/*
const int &f  - кол -во нулей после точки после умножения
const long &a - множитель
const long &b - множитель
*/
long Menu::mul_di(const int &f,const long &a,const long &b)
{
	long result = a * b;

	if(f == CPrg::FORMAT::LINEAR_4 || f == CPrg::FORMAT::CIRCLE_4)
		result/=10000;
	else if(f == CPrg::FORMAT::LINEAR_3 || f == CPrg::FORMAT::CIRCLE_3)
		result/=1000;
	else if(f == CPrg::FORMAT::LINEAR_2 || f == CPrg::FORMAT::CIRCLE_2)
		result/=100;
	else if(f == CPrg::FORMAT::LINEAR_1 || f == CPrg::FORMAT::CIRCLE_1)
		result/=10;

	return result;
}
/*
const int &f_DeNominator - кол -во нулей после точки у делителя
const long &Numerator    - делимое	(числитель)
const long &DeNominator  - делитель (знаменатель)
*/
long Menu::div_di(const int &f_DeNominator,const long &Numerator,const long &DeNominator)
{
	long a = Numerator;
	if(f_DeNominator == CPrg::FORMAT::LINEAR_4 || f_DeNominator == CPrg::FORMAT::CIRCLE_4)
		a*=10000;
	else if(f_DeNominator == CPrg::FORMAT::LINEAR_3 || f_DeNominator == CPrg::FORMAT::CIRCLE_3)
		a*=1000;
	else if(f_DeNominator == CPrg::FORMAT::LINEAR_2 || f_DeNominator == CPrg::FORMAT::CIRCLE_2)
		a*=100;
	else if(f_DeNominator == CPrg::FORMAT::LINEAR_1 || f_DeNominator == CPrg::FORMAT::CIRCLE_1)
		a*=10;

	return a/DeNominator;
}

long Menu::check_max_k_divider(long result)
{
	if(flash_prg.axis_cfg[1].type == CPrg::CIRCLE){
		long res;
		if(flash_prg.axis_cfg[1].format == CPrg::FORMAT::CIRCLE_4 ||
		flash_prg.axis_cfg[1].format == CPrg::FORMAT::CIRCLE_3 ||
		flash_prg.axis_cfg[1].format == CPrg::FORMAT::CIRCLE_2 ||
		flash_prg.axis_cfg[1].format == CPrg::FORMAT::CIRCLE_1){
			res  = Lir.div_di(flash_prg.axis_cfg[1].format, 360, 1);			
		}
		else if(flash_prg.axis_cfg[1].format == CPrg::FORMAT::CIRCLE_SECUNDO){
			res = 1296000; // 360 *60 *60
		}
		else if(flash_prg.axis_cfg[1].format == CPrg::FORMAT::CIRCLE_MINUTO){
			res = 21600; // 360 *60
		}
		coeff.div(1, &res);

		if(ref_.set_point){  // если есть кол - во интервалов то считаем max
			if((ref_.set_point * result) > res){
				return res/ref_.set_point;
			}
		}
		else
			return result;
	}

	return result;
}

long Menu::check_max_set_point(long divider, long result,uint8_t ch)
{
	if(flash_prg.axis_cfg[ch].type == CPrg::CIRCLE){
		long res;
		res = get_max_circle(ch);
		coeff.div(ch, &res);

		if(divider){  // если есть размер интервалов то считаем max
			if((divider * result) > res){
				return res/divider;
			}
		}
		else
			return result;
	}else if(flash_prg.axis_cfg[ch].type == CPrg::LINEAR){
		long long tmp = 0;
		long cnt = 0;
		do{
			if(!result)
				break;
			tmp+=divider;
			if(tmp > MAX_INTERVAL_PLUS || tmp < MAX_INTERVAL_MINUS)
				return cnt;
			else
				cnt++;
		}
		while (--result);
		result = cnt;
	}

	return result;
}

long Menu::abs_lir(const long &in)
{
	if(in <0)
		return (in * -1);
	else 
		return in;
}

#if (configUSE_NCshell == 1)

void Menu::show_small_axis(int x,int y)
{	
	uint8_t c;
	
	if(!small_axis_view){		
		if(MsgKey.tracer_next == TMsgKey::CMD_CNTRL && Str[N_STR - 1] != no_axis){
			c =	get_ch_from_str(N_STR - 1);
			if(c != N_AXIS){			
				char tmp = pgm_read_byte_near(sAxis_screen[Str[N_STR - 1]] + 2);
				
				int c  = get_channel(Str[N_STR - 1]);
				if(st_Drive & plb2(c))
					INVERT_CHAR()
				else
					NORMAL_CHAR()
				if(Str[N_STR - 1] != no_axis){
					display.put_char_local_independent(tmp,0,1,17 * 10); // буква оси
				}
				NORMAL_CHAR()
				
				if(init.mRef_ext[c])
					tmp = 'a'; // код символа а
				else
					tmp = 'o'; // код символа о
				display.put_char_local_independent('(',0,2,17 * 10);
				display.put_char_local_independent(tmp,0,3,17 * 10);
				display.put_char_local_independent(')',0,4,17 * 10);
				
				if(flash_prg.is_encoder_fault(c)){
					display.print_flash_local(sError,0,16,17*10);
				}
				else{
					const long CNT = conv_c(screen.channel_counter[c],c);
					print_long_format(CNT,flash_prg.axis_cfg[c].format,0,16,17 * 10,1,0,11);
				}
			}
		} 
		return;
	}
	
	// отображение 4 оси  в преднаборе
	for(int s = 0; s < N_STR ;s++){
		if(Str[s] == no_axis)
			continue;
		c =	get_ch_from_str(s);
		if(c == N_AXIS)
			continue;
		//display.print_flash_local(sAxis_screen[Str[s]],0,x + 0,y + (16 * s));
		if(st_Drive & plb2(c))
			INVERT_CHAR()
		else
			NORMAL_CHAR()
		char tmp = pgm_read_byte_near(sAxis_screen[Str[s]] + 2);
		if(Str[s] != no_axis){
			display.put_char_local_independent(tmp,0,x + 1,y + (16 * s));
		}
		NORMAL_CHAR()
		
		if(init.mRef_ext[c])
			tmp = 'a'; // код символа а
		else
			tmp = 'o'; // код символа о
		display.put_char_local_independent('(',0,x + 2,y + (16 * s));
		display.put_char_local_independent(tmp,0,x + 3,y + (16 * s));
		display.put_char_local_independent(')',0,x + 4,y + (16 * s));				
		
		if(flash_prg.is_encoder_fault(c)){
			display.print_flash_local(sError,0,11 + x,y + (16 * s));
		}else{
			const long CNT = conv_c(screen.channel_counter[c],c);
			print_long_format(CNT,flash_prg.axis_cfg[c].format,0,x + 16,y + (16 * s),1,0,11);
		}
		
		if(flash_prg.axis_cfg[c].readout_speed == CPrg::READOUT_SPEED::NO_SPEED){
			display.print_flash_local(PSTR("=>"),0,x + 19,y + (16 * s));		
			print_long_format(cnt_z[c],flash_prg.axis_cfg[c].format,0,x + 21,y + (16 * s),0,0,11);
		}
	}
	display.print_symbols_local('-',30,0,0,y + (16 * 4));
}

/*
функция определяет кол -во свободных координатных строк снизу
return ch - кол во строку начиная снизу
*/
char Menu::init_resz_prg_v()
{
	const int N_STR_ = N_STR;	
	char ch;

	if(block_str[N_STR_ - 1] || (Str[N_STR_ - 1] == no_axis && display.position_to_see[N_STR_ - 1] == CDisp::COORDINATE)) {
		_resz_prg_v.ALIGN = 1;
		_resz_prg_v.SZ = 3;
		_resz_prg_v.y_offset = 10;
		ch = 1;
		if (block_str[N_STR_ - 2] || (Str[N_STR_ - 2] == no_axis && display.position_to_see[N_STR_ - 2] == CDisp::COORDINATE)) {
			_resz_prg_v.ALIGN = 2;
			_resz_prg_v.SZ = 7;
			_resz_prg_v.y_offset = 6;
			ch = 2;
		}
	}
	else {
		_resz_prg_v.ALIGN = 0;
		_resz_prg_v.SZ = 0;
		_resz_prg_v.y_offset = 0;
		ch = 0;
	}

	if (ch != ch_mem) {
		entry_cnt_EDITPR = 0;    //счетчик курсора
		entry_scroll_EDITPR = 0; //счетчик сдвига
		ch_mem = ch;
	}	
	
	return ch;
}

//==================================================РЕДАКТОР ПРОГРАММ(для копирования с пк)==============================================
void Menu::actions_EDITPR(const unsigned char& page)
{
	if (entry_EDITPR == INIT)
	init_menu_EDITPR(page);
	if (entry_EDITPR == UPDATE)
	draw_prg_EDITPR(page, 1,1);
	if (entry_EDITPR == ADD_FRAME) { //добавление кадра
		SET_C(idEditPrgInput);
		type_edit_input = 0;
		init_input_EDITPR(page);
		entry_EDITPR = IDLE;
	}
	if (entry_EDITPR == REP_FRAME) { //замена кадра
		if (_header_prg.cnt > 0) {
			SET_C(idEditPrgInput);
			type_edit_input = 1;
			init_rep_EDITPR(page);
		}
		else
		status(PSTR("нет кадров"), 300, page);
		entry_EDITPR = IDLE;
	}
	if (entry_EDITPR == INS_FRAME) { //вставка кадра
		if (_header_prg.cnt > 0) {
			SET_C(idEditPrgInput);
			type_edit_input = 2;
			init_ins_EDITPR(page);
		}
		else
		status(PSTR("нет кадров"), 300, page);
		entry_EDITPR = IDLE;
	}
	if (entry_EDITPR == EDT_FRAME) { //изменить кадра
		if (_header_prg.cnt > 0) {
			SET_C(idEditPrgInput);
			type_edit_input = 3;
			init_edit_EDITPR(page);
		}
		else
		status(PSTR("нет кадров"), 300, page);
		entry_EDITPR = IDLE;
	}
	if (entry_EDITPR == COM_FRAME) { //закомментировать кадр
		if (_header_prg.cnt > 0)
			com_frame_EDITPR(get_addres_prg());
		else
			status(PSTR("нет кадров"), 300, page);
		entry_EDITPR = UPDATE;
	}
	if (entry_EDITPR == FID_FRAME) { //поиск кадра
		if (_header_prg.cnt > 0) {
			func_cmd = 0;
			trigger_func_cmd = 0;
			SET_C(idEditPrgInput);
			type_edit_input = 4;
			init_find_EDITPR(page);
		}
		else
		status(PSTR("нет кадров"), 300, page);
		entry_EDITPR = IDLE;
	}
	if (entry_EDITPR == DEL_FRAME)
	delete_frm_EDITPR();        //удалить кадр
	if (entry_EDITPR == IDLE)
	;
}

void Menu::actions_EDITPR_input(const unsigned char& page)
{
	update_func_cmd(page);
	flashing_cursor(4, 16 * 12, page);
	detect_format();
	if (menu.ResultForInputNum == input::ok) {
		if (((type_edit_input == 0 || type_edit_input == 2) && _header_prg.cnt < Max_Frame) ||
		 (type_edit_input == 1 || type_edit_input == 4 || type_edit_input == 3)) {
			if (type_edit_input == 0) {	    // 0 - добавление кадра
				add_frame_EDITPR();
				draw_prg_EDITPR(page, 0,1);
				show_current_frame_input(page, _header_prg.cnt);
			}
			else if (type_edit_input == 1 || type_edit_input == 3) {// 1 - замена кадра или 3 - изменить кадр
				repl_frame_EDITPR();
				exit_input_EDITPR();
				return;
			}
			else if (type_edit_input == 2) {// 2 - вставка кадра
				ins_frame_EDITPR();
				scroll_dw(entry_cnt_EDITPR, entry_scroll_EDITPR, 2, _header_prg.cnt, 11);
				draw_prg_EDITPR(page, 1,1);
				show_current_frame_input(page, entry_cnt_EDITPR);
			}
			else if (type_edit_input == 4) {// 4 - поиск кадра
				find_frame_EDITPR();
				draw_prg_EDITPR(page, 1,1);
				init_cursor_pos(3);
				clr_info(page);
				ResultForInputNum = input::input_idle;
				return;
			}
		}
		else {
			if (PrgorSubPrg == 0)
				status(PSTR("макс 500 кадров"), 500, page);
			else if (PrgorSubPrg == 1)
				status(PSTR("макс 100 кадров"), 500, page);
		}
		
		init_cursor_pos(54);
	}
	if (menu.ResultForInputNum == input::cancel_) {
		exit_input_EDITPR();
	}
}

void Menu::exit_input_EDITPR()
{
	status(PSTR(""));
	clr_info(0);
	init_cursor_pos();
	entry_EDITPR = UPDATE;
	SET_C(idEditPrg);
}

void Menu::init_input_EDITPR(const unsigned char& page)
{
	init_cursor_pos(54);
	//screen.to_clear_place_for_virtual_buttons(4);
	show_func_cmd(page);
	int tmp = (_header_prg.cnt + 1) - entry_cnt_EDITPR ;
	for (int i = 0; i < tmp; i++) {
		scroll_dw(entry_cnt_EDITPR, entry_scroll_EDITPR, 2, _header_prg.cnt, 11);
		draw_prg_EDITPR(page, 0,0);
		//Sleep(1000);
	}
	draw_prg_EDITPR(page, 0, 1);
	show_current_frame_input(page, _header_prg.cnt);
	status(PSTR("Ввод кадра"), 0, page);
}

void Menu::init_rep_EDITPR(const unsigned char& page)
{
	init_cursor_pos(54);
	show_func_cmd(page);
	show_current_frame_input(page, entry_cnt_EDITPR);
	status(PSTR("Замена кадра"), 0, page);
}

void Menu::init_ins_EDITPR(const unsigned char& page)
{
	init_cursor_pos(54);
	show_func_cmd(page);
	show_current_frame_input(page, entry_cnt_EDITPR);
	status(PSTR("Вставка кадра"), 0, page);
}

void Menu::init_edit_EDITPR(const unsigned char& page)
{
	uint32_t addres = 0;
	uint32_t addres_prg = 0;
	char buf[FRAME_FLASH_SZ];

	init_cursor_pos(54);
	show_func_cmd(page);
	show_current_frame_input(page, entry_cnt_EDITPR);

	addres = get_addres_prg();
	addres_prg = addres + (entry_cnt_EDITPR * FRAME_FLASH_SZ);

	myflash.Read_flash_at45(addres_prg, (uint8_t*)buf, FRAME_FLASH_SZ);
	display.strcpylir((char*)bufASCIIstr, buf + 4);
	int n_tmp = display.strnlen_lir(buf, FRAME_FLASH_SZ);
	cursor_pos = n_tmp - 4;
	result[1] = buf[n_tmp - 1];
	//Update_input_str = true;
	UPD_2nd = true;
	type_edit_input = 3;
	status(PSTR("Изменить кадр"), 0, page);
}

void Menu::init_find_EDITPR(const unsigned char& page)
{
	init_cursor_pos(3);
	clr_info(page);
	status(PSTR("Поиск кадра"), 0, page);
}

void Menu::clr_info(const unsigned char& page)
{
	display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY + 1, page, 0, 16 * 12);
	display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY + 1, page, 0, 16 * 13);
}

void Menu::upd_2nd_str(const unsigned char& page,int x_,int y_)
{
	if (UPD_2nd) {
		int i = 0;
		bool str1OUT = true;
		for (; bufASCIIstr[i] != '\0'; i++) {
			if (i < STR_FRAME_SZ_DISPLAY - 4)
			display.put_char_local_independent(bufASCIIstr[i], page, x_++, y_);
			else {
				if (str1OUT) {
					y_ += 16;
					x_ = 1;
					str1OUT = false;
				}
				display.put_char_local_independent(bufASCIIstr[i], page, x_++, y_);
			}
		}
		UPD_2nd = false;
	}
}

void Menu::add_frame_EDITPR()
{
	uint32_t addres = 0;
	uint32_t addres_prg = 0;
	//====если в последнем кадре только частота то сразу ее записываем
	//exe_F_only(bufASCIIstr, IN_BUF_SZ);
	//===
	set_blank_cmd((char*)bufASCIIstr, IN_BUF_SZ);
	set_XYZIJ_format_cmd((char*)bufASCIIstr, IN_BUF_SZ);
	_header_prg.cnt++;//увеличение _header_prg.cnt на один
	set_num_str(bufASCIIstr, IN_BUF_SZ, _header_prg.cnt);
	
	addres = get_addres_prg();
	addres_prg = addres + (_header_prg.cnt - 1) * FRAME_FLASH_SZ;
	
	char buf[FRAME_FLASH_SZ];
	memcpy(buf,bufASCIIstr,IN_BUF_SZ);
	append_crc(Lir.crc((uint8_t*)buf, display.strnlen_lir((const char*)buf,FRAME_FLASH_SZ - 2)),(uint8_t*)buf,FRAME_FLASH_SZ - 2);
	
	myflash.Write_flash_at45(addres_prg, (uint8_t*)buf, FRAME_FLASH_SZ);
	//==
	//обновление заголовка во флешь
	myflash.Write_flash_at45(addres - HEADER_PRG_SZ, (uint8_t*)&_header_prg, sizeof(_header_prg));
	//if (_header_prg.cnt > 11) {
	const int DW_ALIGN = 2;
	entry_cnt_EDITPR = _header_prg.cnt - 1 - DW_ALIGN;
	//int tmp = (_header_prg.cnt + 1) - entry_cnt_EDITPR;
	if (entry_cnt_EDITPR < 0)
		entry_cnt_EDITPR = 0;
	for (int i = 0; i < 2; i++) {
		scroll_dw(entry_cnt_EDITPR, entry_scroll_EDITPR, DW_ALIGN, _header_prg.cnt, 11);
		draw_prg_EDITPR(0, 0, 0);
	}
	//}
}

void Menu::repl_frame_EDITPR()
{
	uint32_t addres = 0;
	uint32_t addres_prg = 0;
	//====если в последнем кадре только частота то сразу ее записываем в gl_F
	//exe_F_only(bufASCIIstr, IN_BUF_SZ);
	//===
	set_blank_cmd((char*)bufASCIIstr, IN_BUF_SZ);
	set_XYZIJ_format_cmd((char*)bufASCIIstr, IN_BUF_SZ);
	set_num_str(bufASCIIstr, IN_BUF_SZ, entry_cnt_EDITPR + 1);

	addres = get_addres_prg();
	addres_prg = addres + (entry_cnt_EDITPR * FRAME_FLASH_SZ);
	
	char buf[FRAME_FLASH_SZ];
	memcpy(buf,bufASCIIstr,IN_BUF_SZ);
	append_crc(Lir.crc((uint8_t*)buf, display.strnlen_lir((const char*)buf,FRAME_FLASH_SZ - 2)),(uint8_t*)buf,FRAME_FLASH_SZ - 2);

	myflash.Write_flash_at45(addres_prg, (uint8_t*)buf, FRAME_FLASH_SZ);
}

void Menu::ins_frame_EDITPR()
{
	uint32_t addres = 0;
	uint32_t addres_prg = 0;
	char buf[FRAME_FLASH_SZ];

	addres = get_addres_prg();

	if (_header_prg.cnt > 0) {
		for (int i = _header_prg.cnt; i - 1 >= entry_cnt_EDITPR; i--) {
			addres_prg = addres + (i - 1) * FRAME_FLASH_SZ;
			myflash.Read_flash_at45(addres_prg, (uint8_t*)buf, FRAME_FLASH_SZ);
			IncDec_frm_number(buf, 1);//увеличение номера кадра на один
			addres_prg = addres + (i * FRAME_FLASH_SZ);
			append_crc(Lir.crc((uint8_t*)buf, display.strnlen_lir((const char*)buf,FRAME_FLASH_SZ - 2)),(uint8_t*)buf,FRAME_FLASH_SZ - 2);
			myflash.Write_flash_at45(addres_prg, (uint8_t*)&buf, FRAME_FLASH_SZ);
		}
		repl_frame_EDITPR();
		_header_prg.cnt++;
		//обновление заголовка во флешь
		myflash.Write_flash_at45(addres - HEADER_PRG_SZ, (uint8_t*)&_header_prg, sizeof(_header_prg));
	}
}

void Menu::com_frame_EDITPR(uint32_t addres)
{
	uint32_t addres_prg = 0;
	char buf[FRAME_FLASH_SZ];
	addres_prg = addres + (entry_cnt_EDITPR * FRAME_FLASH_SZ);

	myflash.Read_flash_at45(addres_prg, (uint8_t*)buf, FRAME_FLASH_SZ);
	if (buf[3] == '/')
		buf[3] = ':';
	else if (buf[3] == ':')
		buf[3] = '/';
	append_crc(Lir.crc((uint8_t*)buf, display.strnlen_lir((const char*)buf,FRAME_FLASH_SZ - 2)),(uint8_t*)buf,FRAME_FLASH_SZ - 2);
	myflash.Write_flash_at45(addres_prg, (uint8_t*)&buf, FRAME_FLASH_SZ);
}

void Menu::find_frame_EDITPR()
{
	char number[4] = {0};
	int tpm_num;
	//==получаем номер из введенного
	for (int i = 0; i < 3; i++) {
		if (bufASCIIstr[i] >= '0' && bufASCIIstr[i] <= '9')
		number[i] = bufASCIIstr[i];
		else
		break;
	}
	tpm_num = strtol_lir((uint8_t*)&number);
	//==
	if (tpm_num >= 1 && tpm_num <= _header_prg.cnt) {
		if (tpm_num > entry_cnt_EDITPR + 1) {
			while (tpm_num != entry_cnt_EDITPR + 1) {
				scroll_dw(entry_cnt_EDITPR, entry_scroll_EDITPR, 2, _header_prg.cnt, 11);
				draw_prg_EDITPR(0, 0,0);
			}
		}
		else if (tpm_num < entry_cnt_EDITPR + 1) {
			while (tpm_num != entry_cnt_EDITPR + 1) {
				scroll_up(entry_cnt_EDITPR, entry_scroll_EDITPR, 2, _header_prg.cnt,11);
				draw_prg_EDITPR(0, 0,0);
			}
		}
	}
	else
	status(PSTR("не найден"), 200);
}
/*
очистка всей программы заключается в сбросе заголовка программы
очистка текста заголовка и кол- ва кадров на 0
*/
void Menu::erase_all_EDITPR()
{
	uint32_t addres = get_addres_prg();
	
	//memcpy(&save_header_prg,&_header_prg,sizeof(_header_prg));
	
	_header_prg.cnt = 0;
	memset(_header_prg.name,'\0',sizeof(_header_prg.name));
	_header_prg.sign = 0xAAAA;
	//обновление заголовка во флешь
	myflash.Write_flash_at45(addres - HEADER_PRG_SZ, (uint8_t*)&_header_prg, sizeof(_header_prg));
	entry_cnt_EDITPR = 0;    //счетчик курсора
	entry_scroll_EDITPR = 0;
	
	display.clear_ram_null();
	display.print_flash_local(PSTR("%заголовок программы"), 0, 0, 0);
	
	entry_EDITPR = UPDATE;
}

void Menu::choice_frame_EDITPR()
{		
	_choice_prg.addres_prg = get_addres_prg();
	_choice_prg.cnt = _header_prg.cnt;

	//==обнуляем индекс выбранного кадра ,скроллинг и кол -во вызванных подпрограмм
	entry_scroll_EDITPR = 0;
	entry_cnt_EDITPR = 0;
	call_sub = 0;
	//==
	//==обнуляем установленный кадровый таймер
	timer_frm = 0;
	timer_sv = 0;
	//==

	Lir.SEToutRA = 0; //устанавливаем выход Авто режим
	SET_C(idExecmd);
	display.clear_ram_null();
	entry_EDITPR = INIT;
}

void Menu::update_prg_stop_EDITPR()
{
	if(_choice_prg.addres_prg == get_addres_prg()){
		_choice_prg.cnt = _header_prg.cnt;
		//==обнуляем установленный кадровый таймер
		timer_frm = 0;
		timer_sv = 0;
		//==
	}
}

uint32_t Menu::get_addres_prg()
{
	uint32_t tmp = 0;
	if (PrgorSubPrg == 0)
	tmp = OFFSET_PRG_DEST + (menu_LISPRG * PRG_SZ) + HEADER_PRG_SZ;
	else if (PrgorSubPrg == 1)
	tmp = OFFSET_PRG_DEST + (MAX_PRG * PRG_SZ) + (menu_LISSUB * SUBPRG_SZ) + HEADER_PRG_SZ;
	return tmp;
}

/*
char PrgorSub - программа 0 или подпрограмма 1
int8_t num - номер
*/
uint32_t Menu::get_addres_prg_cur(char PrgorSub, int8_t num)
{
	uint32_t tmp = 0;
	if (PrgorSub == 0)
		tmp = OFFSET_PRG_DEST + (num * PRG_SZ) + HEADER_PRG_SZ;
	else if (PrgorSub == 1)
		tmp = OFFSET_PRG_DEST + (MAX_PRG * PRG_SZ) + (num * SUBPRG_SZ) + HEADER_PRG_SZ;
	return tmp;
}

void Menu::init_menu_EDITPR(const unsigned char& page)
{
	//получение адреса на основании рассчета выбранной программы 1- 10/подпрограммы 1 -10:
	//==ПРОГРАММА
	//адрес программы: OFFSET_PRG_DEST + (numPrg * PRG_SZ)
	//адрес кадра: OFFSET_PRG_DEST + (numPrg * PRG_SZ + HEADER_PRG_SZ) + (numFrame * FRAME_SZ)
	//==ПОДПРОГРАММА
	//адрес подпрограммы: OFFSET_PRG_DEST + (MAX_PRG * PRG_SZ) + (subPrg * SUBPRG_SZ)
	//адрес кадра: OFFSET_PRG_DEST + (MAX_PRG * PRG_SZ) + (subPrg * SUBPRG_SZ + HEADER_PRG_SZ) + (numFrame * FRAME_SZ)
	display.clear_ram_null();
	
	uint32_t addres = 0;
	if (PrgorSubPrg == 0) {
		addres = OFFSET_PRG_DEST + (menu_LISPRG * PRG_SZ);
		Max_Frame = 500;
	}
	else if (PrgorSubPrg == 1) {
		addres = OFFSET_PRG_DEST + (MAX_PRG * PRG_SZ) + (menu_LISSUB * SUBPRG_SZ);
		Max_Frame = 100;
	}

	myflash.Read_flash_at45(addres, (uint8_t*)&_header_prg, sizeof(_header_prg));
	if (_header_prg.sign != (short)SIGN_PRG) {
		memset(_header_prg.name, '\0', 36);
		_header_prg.sign = (short)SIGN_PRG;
		_header_prg.cnt = 0;
		myflash.Write_flash_at45(addres, (uint8_t*)&_header_prg, sizeof(_header_prg));
	}
	if (_header_prg.name[0] == '\0')
		display.print_flash_local(PSTR("%заголовок программы"), page, 0, 0);
	else
		display.print_ram_local(_header_prg.name, page, 0, 0);
	if (_header_prg.cnt == 0)   // если кадров ноль входим в режим ввода
	entry_EDITPR = ADD_FRAME;
	else
	entry_EDITPR = UPDATE;
}
/*
const unsigned char& page - страница памяти экрана
char inv - 1 - использовать выделение строки , 0 -нет
bool OnDisp - вывод на экран строк кадра 1 - да ,0 - нет
*/
void Menu::draw_prg_EDITPR(const unsigned char& page,char inv, bool OnDisp)
{
	//отрисовка кадров программы
	uint32_t addres = get_addres_prg();
	draw_prg(11, _header_prg.cnt, entry_scroll_EDITPR, entry_cnt_EDITPR,inv,page,0, addres, OnDisp);
	//==
	if (curItem->id == idEditPrg) {
		show_info_prg_EDITPR(page);
		show_func_blank_EDITPR(page);
	}
	entry_EDITPR = IDLE;
}
/*
char sz_disp_out - кол -во кадров выводимых на экран за раз
short max_prg - максимальное количесво кадров в программе
int scroll - текущий скроллинг
int index - текущий указатель
char inv - использовать инверсию или нет
char page - страница памяти экрана
int offsety - смещение выводимых кадров по y
uint32_t addres - адрес программы во flash памяти
bool OnDisp - вывод на экран 1 - да ,0 - нет
*/
void Menu::draw_prg(char sz_disp_out, short max_prg, int scroll, int index, char inv, char page, int offsety, uint32_t addres,bool OnDisp)
{
	if(!sz_disp_out)
		return;
	if (!max_prg) {
		display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY, page, 0, 16 * (0 + 1 + offsety));
		display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY, page, 0, 16 * (1 + 1 + offsety));
		return;
	}
		
	uint32_t addres_prg = 0;
	char buf[FRAME_FLASH_SZ];
	bool mr = false;
	bool dir = true;  //вниз
	cnt_2ndstr = 0;
	int tmpoff = offsety;
	int str_inv = 0;

	for (; str_inv < sz_disp_out; str_inv++) {
		if (index == str_inv + scroll) {
			break;
		}
	}
	//USART_DEBUG(scroll);
	for (int str = str_inv;;) {		
		if (max_prg >= (str + scroll + 1)) {
			addres_prg = addres + ((str + scroll) * FRAME_FLASH_SZ);
			myflash.address_flash_test = addres_prg;
			myflash.Read_flash_at45(addres_prg, (uint8_t*)buf, FRAME_FLASH_SZ);
			if (!dir) {
				if (display.strnlen_lir(buf) > STR_FRAME_SZ_DISPLAY)
				offsety--;
			}			
		}
		else {
			if (OnDisp) {
				display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY, page, 0, 16 * (str + 1 + offsety));//очистка
				for (int i = str;; i++) {
					if ((16 * (i + 2 + offsety) > 16 * (tmpoff + 1)) && (16 * (i + 2 + offsety) < 16 * (tmpoff + 1 + sz_disp_out)))
						display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY, page, 0, 16 * (i + 2 + offsety));//очистка
					else
						break;
				}				
			}
			//USART_DEBUG(0);
			goto r;//continue;
		}
		if ((index == str + scroll) && inv) {
			INVERT_CHAR();
			mr = true;
		}
		else {
			NORMAL_CHAR();
		}
		if (OnDisp)
		out_num_frame(buf, page, 0, 16 * (str + 1 + offsety), 16 * (tmpoff + 1), 16 * (tmpoff + 1 + sz_disp_out));
		NORMAL_CHAR();
		if (display.strnlen_lir(buf) <= STR_FRAME_SZ_DISPLAY) {
			if (OnDisp)
			out_1nd_frame(buf + 4, page, 4, 16 * (str + 1 + offsety),16 * (tmpoff + 1), 16 * (tmpoff + 1 + sz_disp_out));
		}
		else {
			if (OnDisp)
			out_2nd_frame(buf + 4, page, 4, 16 * (str + 1 + offsety), 16 * (tmpoff + 1), 16 * (tmpoff + 1 + sz_disp_out));
			if (dir) {
				offsety++;
				cnt_2ndstr++;
			}
		}
		if (dir) {// переход к следущей строке
			str++;
			if (!(str + cnt_2ndstr < sz_disp_out)){
				r:
				str = str_inv;
				offsety = tmpoff;
				dir = false;
				if (!str)
					break;
				else
					str--;
			}
		}
		else {
			str--;
			if (str < 0)
			break;
			else if(str + offsety < 0)
			break;
		}
	}
	if (!mr && max_prg && inv)
	status(PSTR("ошибка 1"), 200, page);

}

void Menu::delete_frm_EDITPR()
{
	uint32_t addres = 0;
	uint32_t addres_prg = 0;
	char buf[FRAME_FLASH_SZ];
	addres = get_addres_prg();

	if (_header_prg.cnt > 0) {
		addres_prg = addres + entry_cnt_EDITPR * FRAME_FLASH_SZ;
		memset(buf, 0xFF, FRAME_FLASH_SZ);
		myflash.Write_flash_at45(addres_prg, (uint8_t*)&buf, FRAME_FLASH_SZ);
		if(entry_cnt_EDITPR == _header_prg.cnt - 1){
			if (entry_cnt_EDITPR > 0)
				entry_cnt_EDITPR--;
		}
		else {
			int i = 0;
			int tmp = entry_cnt_EDITPR + 1;
			for (; i < _header_prg.cnt - 1; i++) {
				if (tmp == _header_prg.cnt)
				break;
				myflash.Read_flash_at45(addres_prg + (i * FRAME_FLASH_SZ) + FRAME_FLASH_SZ, (uint8_t*)buf, FRAME_FLASH_SZ);
				IncDec_frm_number(buf, 0);//уменьшение номера кадра на один
				append_crc(Lir.crc((uint8_t*)buf, display.strnlen_lir((const char*)buf,FRAME_FLASH_SZ - 2)),(uint8_t*)buf,FRAME_FLASH_SZ - 2);//20us				
				myflash.Write_flash_at45(addres_prg + (i * FRAME_FLASH_SZ), (uint8_t*)&buf, FRAME_FLASH_SZ);
				tmp++;	
				//при 500 кадрах обновление занимает около 3ех секунд
			}
			memset(buf, 0xFF, FRAME_FLASH_SZ);
			myflash.Write_flash_at45(addres_prg + (i * FRAME_FLASH_SZ), (uint8_t*)&buf, FRAME_FLASH_SZ);
		}
		_header_prg.cnt--;
		//обновление заголовка во флешь
		myflash.Write_flash_at45(addres - HEADER_PRG_SZ, (uint8_t*)&_header_prg, sizeof(_header_prg));
		
		//========смещение при удалении
		int tmp = entry_cnt_EDITPR;
		scroll_up(entry_cnt_EDITPR, entry_scroll_EDITPR, 2, _header_prg.cnt, 11);
		if(tmp != entry_cnt_EDITPR)
		entry_cnt_EDITPR++;
		//if (entry_scroll_EDITPR)
		//	entry_scroll_EDITPR--;
		entry_EDITPR = UPDATE;
		//========
	}
}
/*
char* frm - буфер с номером
char incDec - увеличение или уменьшение: 0 - уменьшение ,1 - увеличение
*/
void Menu::IncDec_frm_number(char* frm, char incDec)
{
	char buf[4] = { 0 };
	long res;
	for (int i = 0; i < 3; i++)
	buf[i] = frm[i];
	res = strtol_lir((uint8_t*)&buf);
	if (res > 500)
	status(PSTR("res > 500"), 200);
	if (incDec == 0)
	res--;
	else
	res++;
	if (res > 999)
	res = 999;
	if (res < 0)
	res = 0;
	display.itoa_quick_lir(&res, (uint8_t*)&buf);
	add_filler((uint8_t*)&buf, 4, 2, '0');

	for (int i = 0; i < 3; i++)
	frm[i] = buf[i];
}

void Menu::menu_up_EDITPR()
{
	//вверх по программме
	if (curItem->id == idEditPrg) {
		scroll_up(entry_cnt_EDITPR, entry_scroll_EDITPR, 2, _header_prg.cnt, 11);
		entry_EDITPR = UPDATE;
	}
}
/*
int& cur - номер текущего кадра
int& scroll - текущий скроллинг
const int UP_ALIGN - отступ сверху
const int mn_sz - размер программы(кадры)
const int SZ - размер вывода(кадры)
*/
void Menu::scroll_up(int& cur, int& scroll,int UP_ALIGN, const int mn_sz, const int SZ)
{
	cur--;
	if (cur < 0) {
		cur = 0;
		scroll = 0;
		return;
	}
	if (cur - UP_ALIGN == scroll - 1) {
		if (scroll)
		scroll--;
	}
}
/*
int& cur - номер текущего кадра
int& scroll - текущий скроллинг
const int DW_ALIGN - отступ снизу
const int mn_sz - размер программы(кадры)
const int SZ - размер вывода(кадры)
cnt_2ndstr - кол -во двухстрочных кадров в текущем выводе
*/
void Menu::scroll_dw(int& cur, int& scroll,int DW_ALIGN, const int mn_sz, int SZ)
{
	cur++;
	if (cur > mn_sz - 1) {
		cur = mn_sz - 1;
		if (cur < 0)
		cur = 0;
		return;
	}
	int tmp = mn_sz - 1;
//=== затычка на время выполнения кадров чтобы потом переключится в отображение кадров
if(!SZ && !DW_ALIGN){
	DW_ALIGN = 2;
	SZ = 7;
}
//==
	int sc_prev = scroll;
	if (tmp + cnt_2ndstr >= SZ) {
		tmp = cur - SZ;
		if (tmp + DW_ALIGN >= scroll) {
			scroll = tmp + 1 + DW_ALIGN;
			tmp = mn_sz - 1 - DW_ALIGN;
			if (cur > tmp) {
				scroll = scroll - (cur - tmp);
				if (cnt_2ndstr)
				scroll++;
				else if (cur == mn_sz - 1)
				scroll = sc_prev;
			}
		}
	}
}

void Menu::menu_down_EDITPR()
{
	//вниз по программме
	if (curItem->id == idEditPrg) {
		scroll_dw(entry_cnt_EDITPR, entry_scroll_EDITPR, 2, _header_prg.cnt, 11);
		entry_EDITPR = UPDATE;
	}
}

void Menu::menu_left_EDITPR()
{
	if (curItem->id == idEditPrg) {
		page_up_EDITPR();
	}
}

void Menu::menu_right_EDITPR()
{
	if (curItem->id == idEditPrg) {
		page_down_EDITPR();
	}
}

void Menu::page_up_EDITPR()
{
	for(int i = 0 ; i < 11;i++)
		scroll_up(entry_cnt_EDITPR, entry_scroll_EDITPR, 2, _header_prg.cnt, 11);
	entry_EDITPR = UPDATE;
}

void Menu::page_down_EDITPR()
{
	for (int i = 0; i < 11; i++)
		scroll_dw(entry_cnt_EDITPR, entry_scroll_EDITPR, 2, _header_prg.cnt, 11);
	entry_EDITPR = UPDATE;
}

void Menu::menu_enter_EDITPR()
{
	if (curItem->id == idEditPrg) {
		//редактирование текущего кадра
		entry_EDITPR = EDT_FRAME;
	}
}

void Menu::menu_stop_EDITPR()
{
	if (curItem->id == idEditPrg) {
		//выход в меню список Программы или Подпрограммы
		update_prg_stop_EDITPR();
		if (PrgorSubPrg == 0) {
			entry_EDITPR = INIT;
			SET_C(idListProg);
		}
		else if (PrgorSubPrg == 1) {
			entry_EDITPR = INIT;
			SET_C(idListSubProg);
		}
	}
}

void Menu::menu_C_EDITPR()
{
	if (curItem->id == idEditPrg) {
		entry_EDITPR = DEL_FRAME;
	}
}

void Menu::menu_F_EDITPR()
{
	if (curItem->id == idEditPrg) {
		func_blanc_EDITPR++;
		if (func_blanc_EDITPR > 1)
		func_blanc_EDITPR = 0;
		show_func_blank_EDITPR(0);
	}
}

void Menu::blank_right_input_EDITPR(const char& BLANK)
{
	//обработка кнопок ИНФО ВВОД ЗАМЕ ВСТА //
	if (curItem->id == idEditPrg) {
		switch (BLANK)
		{
			case _key_BLANK1:
			if (func_blanc_EDITPR == 0) {
				if(_header_prg.cnt == 0)
					break;
				choice_frame_EDITPR();//"ПУСК"
			}
			else if (func_blanc_EDITPR == 1) {
				entry_EDITPR = COM_FRAME;
			}
			break;
			case _key_BLANK2:
			if (func_blanc_EDITPR == 0) {
				entry_EDITPR = INS_FRAME; //"ВСТА"
			}
			else if (func_blanc_EDITPR == 1) {
				menu_init_DEALLP();//erase_all_EDITPR();       //"ОЧИС"
			}
			break;
			case _key_BLANK3:
			if (func_blanc_EDITPR == 0) {
				entry_EDITPR = REP_FRAME; //"ЗАМЕ"
			}
			else if (func_blanc_EDITPR == 1) {
				page_down_EDITPR();
			}
			break;
			case _key_BLANK4:
			if (func_blanc_EDITPR == 0) {				
				entry_EDITPR = ADD_FRAME;//"ВВОД"
			}
			else if (func_blanc_EDITPR == 1) {				
				entry_EDITPR = FID_FRAME; //"КАДР"
			}
			break;
			default:
			return;
		}
	}
}

void Menu::show_func_blank_EDITPR(const char& page)
{
	display.put_button_(X_BUTTON_PLACE, Y_ONE);
	display.put_button_(X_BUTTON_PLACE, Y_TWO);
	display.put_button_(X_BUTTON_PLACE, Y_THREE);
	display.put_button_(X_BUTTON_PLACE, Y_FOUR);

	if (func_blanc_EDITPR == 0)
	{
		display.print_flash_local(sPusk, page, 35, 12);
		display.print_flash_local(PSTR("ВСТА"), page, 35, 70);
		display.print_flash_local(PSTR("ЗАМЕ"), page, 35, 126);
		display.print_flash_local(PSTR("ВВОД"), page, 35, 178);
	}
	if (func_blanc_EDITPR == 1)
	{
		display.print_flash_local(PSTR(" // "), page, 35, 12);
		display.print_flash_local(PSTR("ОЧИС"/*"-10 "*/), page, 35, 70);
		display.print_flash_local(PSTR("+10 "), page, 35, 126);
		display.print_flash_local(PSTR("КАДР"), page, 35, 178);
	}
}

void Menu::show_info_prg_EDITPR(const char& page)
{
	if (PrgorSubPrg == 0) {
		display.print_flash_local(sPrgOrSubPrg[0], page, 0, 16 * 12);
		print_long_format(menu_LISPRG + 1, CPrg::FORMAT::FORMAT_END, page, 12, 16 * 12, 1, 1, 2);
		display.print_flash_local(PSTR("/500"), page, 19, 16 * 13);
	}
	else if (PrgorSubPrg == 1) {
		display.print_flash_local(sPrgOrSubPrg[1], page, 0, 16 * 12);
		print_long_format(menu_LISSUB + 1, CPrg::FORMAT::FORMAT_END, page, 15, 16 * 12, 1, 1, 2);
		display.print_flash_local(PSTR("/100"), page, 19, 16 * 13);
	}

	display.print_flash_local(PSTR("Занято кадров:"), page, 1, 16 * 13);
	print_long_format(_header_prg.cnt, CPrg::FORMAT::FORMAT_END, page, 18, 16 * 13, 1, 2,3);
}

//===================================================конец РЕДАКТОР ПРОГРАММ(для копирования с пк)=============================================

void Menu::out_1nd_frame(char* frame_cmd, unsigned char page, int x, int y,const int y_max_up, const int y_max_down)
{
	if (y >= y_max_up && y < y_max_down) {
		display.print_ram_local(frame_cmd, page, x, y);
		//==очистка строк для scroll меню
		int8_t cnt = display.strnlen_lir(frame_cmd);
		display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY - (cnt + 4), page, cnt + 4, y);
		//==
	}
}

void Menu::out_2nd_frame(char* frame_cmd, unsigned char page, int x, int y, const int y_max_up, const int y_max_down)
{
	char buf[16] = { 0 };
	char offset_bf = 0;
	char offset_disp = 0;
	bool OUT_1str = true;
	char sz_cmd = 0;
	int SZ = display.strnlen_lir(frame_cmd, 100);
	//cuttter_2nd = 1;
	if (SZ) {
		while (1) {
			sz_cmd = get_one_cmd(frame_cmd + offset_bf, buf, 16);
			if (buf[0] == '\0') {
				if (y >= y_max_up && y < y_max_down)
				display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY - (offset_disp + 1), page, offset_disp, y);//очистка
				break;
			}
			offset_bf += sz_cmd;
			if (offset_bf > STR_FRAME_SZ_DISPLAY - 3 && OUT_1str) {
				if (y >= y_max_up && y < y_max_down)
				display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY - (offset_disp + 4), page, offset_disp + 3, y);//очистка
				y += 16;
				x = 1;
				offset_disp = 0;
				OUT_1str = false;
				if (y >= y_max_up && y < y_max_down)
				display.put_char_local_independent(' ', page, 0, y);
			}
			if (y >= y_max_up && y < y_max_down) {
				display.print_ram_local(buf, page, x + offset_disp, y);
			}
			//else
			//	cuttter_2nd = 0;
			offset_disp += sz_cmd;
			memset(buf, '\0', 16);
		}
		//if(OUT_2str)
		//	cnt_2ndstr++;
	}
}

void Menu::out_num_frame(char* frame_cmd, unsigned char page, int x, int y, const int y_max_up, const int y_max_down)
{
	if (y >= y_max_up && y < y_max_down) {
		for (int x = 0; x < 4; x++)
		display.put_char_local_independent(frame_cmd[x], page, x, y);
	}
}

void Menu::menu_init_DEALLP()
{
	char tmp = ' ';
	for(int j = 0 ; j < 4 ;j++){		
		for(int i = 0 ; i < 30;i++){
			display.put_char_local_independent(tmp,0,i,16 * (3 + j));
		}
	}
	display.print_flash_local(PSTR("Удалить всю программу?"),0,7,16 * 4);
	display.print_flash_local(PSTR("ПУСК-ДА, СТОП-НЕТ"),0,9,16 * 5);
	SET_C(idDeallp);
}

void Menu::menu_START_DEALLP()
{
	if(curItem->id == idDeallp){
		erase_all_EDITPR();
		SET_C(idEditPrg);
	}
}

void Menu::menu_STOP_DEALLP()
{
	if(curItem->id == idDeallp){
		entry_EDITPR = UPDATE;
		SET_C(idEditPrg);
	}
}

void Menu::actions_Prg_SubPrg(const unsigned char & page)
{
	if(entry_Prg_SubPrg == m_state::INIT)
	init_menu_Prg_SubPrg(page);
	else if(entry_Prg_SubPrg == m_state::UPDATE)
	draw_menu_Prg_SubPrg(page);
	else if(entry_Prg_SubPrg == m_state::IDLE)
	;
	else
	;
}

void Menu::init_menu_Prg_SubPrg(const unsigned char & page)
{
	display.clear_ram_null(page);
	entry_Prg_SubPrg = m_state::UPDATE;
}

void Menu::draw_menu_Prg_SubPrg(const unsigned char & page)
{
	const char*menu_table[3];
	menu_table[0] = sPrgOrSubPrg[0];
	menu_table[1] = sPrgOrSubPrg[1];
	menu_table[2] = nullptr;
	Draw_menu(menu_table, menu_Prg_SubPrg,page);
	entry_Prg_SubPrg = m_state::IDLE;
}

void Menu::menu_up_Prg_SubPrg()
{
	if(curItem->id == idPrgOrSubPrg){
		menu_Prg_SubPrg--;
		if(menu_Prg_SubPrg < 0)
		menu_Prg_SubPrg = 1;
		entry_Prg_SubPrg = m_state::UPDATE;
	}
}

void Menu::menu_down_Prg_SubPrg()
{
	if(curItem->id == idPrgOrSubPrg){
		menu_Prg_SubPrg++;
		if(menu_Prg_SubPrg > 1)
		menu_Prg_SubPrg = 0;
		entry_Prg_SubPrg = m_state::UPDATE;
	}
}

void Menu::menu_enter_Prg_SubPrg()
{
	if(curItem->id == idPrgOrSubPrg){
		if(menu_Prg_SubPrg == 0){
			SET_C(idListProg);
			entry_Prg_SubPrg = m_state::INIT;
		}
		else if(menu_Prg_SubPrg == 1){
			SET_C(idListSubProg);
			entry_Prg_SubPrg = m_state::INIT;
		}
	}
}

void Menu::menu_stop_Prg_SubPrg()
{
	if(curItem->id == idPrgOrSubPrg){
		display.print_flash_local(sProbel21,0,0,0);
		display.print_flash_local(sProbel21,0,0,16);
		display.print_flash_local(sProbel10,0,30,16 *14);		
		entry_cnt_EDITPR=0;    //счетчик курсора
		entry_scroll_EDITPR=0; //счетчик сдвига
		call_sub = 0;
		//Lir.show_cmd(0,0);
		MsgKey.SET_M(TMsgKey::MODE::HAND);
		MsgKey.to_put_icons_in_mode_HAND(MsgKey.tracer_real);
		SET_C(idHand);
		entry_Prg_SubPrg = INIT;
		func_cmd = 0;//обнуление текущего набора команд для ввода
	}
}

void Menu::show_comm_in_list(char PrgorSubPrg,int i,int8_t scroll,int x)
{
	uint32_t addres = 0;
	header_prg head;

	addres = get_addres_prg_cur(PrgorSubPrg,i) - HEADER_PRG_SZ;// рассчет заголовка адреса программы
	myflash.Read_flash_at45(addres, (uint8_t*)&head, sizeof(head));// чтение буфера по адресу
	if (head.sign == (short)SIGN_PRG && head.name[0] != '\0'){ // вывод заголовка
		display.print_ram_local(head.name,0,x,16 * (i - scroll));
		int cnt = display.strnlen_lir(head.name);
		display.print_symbols_local(' ',40 - (x + cnt),0,x + cnt,16 * (i - scroll));
	}
	else
		display.print_symbols_local(' ',40 - x,0,x,16 * (i - scroll));
}

bool Menu::check_prg(char PrgorSubPrg)
{
	uint32_t addres;
	header_prg head;
	int num_prg;

	if(!PrgorSubPrg)
		num_prg = menu_LISPRG;
	else
		num_prg = menu_LISSUB;

	addres = get_addres_prg_cur(PrgorSubPrg,num_prg) - HEADER_PRG_SZ;// рассчет заголовка адреса программы
	myflash.Read_flash_at45(addres, (uint8_t*)&head, sizeof(head));// чтение буфера по адресу

	if(head.sign == (short)SIGN_PRG && head.cnt != 0){
		_header_prg.cnt = head.cnt;
		return 1;
		}
	else
		return 0;
}

void Menu::actions_LISPRG(const unsigned char & page)
{
	if(entry_LISPRG == INIT)
	init_menu_LISPRG(page);
	else if(entry_LISPRG == UPDATE)
	draw_menu_LISPRG(page);
	else if(entry_LISPRG == IDLE)
	;
	else
	;
}

void Menu::init_menu_LISPRG(const unsigned char & page)
{
	display.clear_ram_null();
	entry_LISPRG = UPDATE;
}

void Menu::draw_menu_LISPRG(const unsigned char & page)
{
	menu_LISPRG_SZ = MAX_PRG - 1;
	for(uint8_t i = menu_scroll_LISPRG; i < menu_scroll_LISPRG + 14;i++)
	{
		if(menu_LISPRG == i)
			INVERT_CHAR()
		else
			NORMAL_CHAR()
		display.print_flash_local(sPrgOrSubPrg[0],page,0,16 * (i - menu_scroll_LISPRG));
		print_long_format(i + 1,CPrg::FORMAT::FORMAT_END,page,12,16 * (i - menu_scroll_LISPRG),1,1,2);
		NORMAL_CHAR()
		show_comm_in_list(0,i,menu_scroll_LISPRG,14);				
	}
	if(menu_scroll_LISPRG == 6)
		display.put_char_local_independent(' ',page,0,16 * 14);
	else 
		display.put_char_local_independent(0x1F,page,0,16 * 14);
	
	entry_LISPRG = IDLE;
}

void Menu::menu_up_LISPRG()
{
	if(curItem->id == idListProg){
		menu_LISPRG--;
		if(menu_LISPRG < 13 && menu_scroll_LISPRG)
			menu_scroll_LISPRG--;
		if(menu_LISPRG < 0){
			menu_LISPRG = menu_LISPRG_SZ;
			menu_scroll_LISPRG = 6;
		}
		entry_LISPRG = UPDATE;
	}
}

void Menu::menu_down_LISPRG()
{
	if(curItem->id == idListProg){
		menu_LISPRG++;
		if(menu_LISPRG > 7 && menu_LISPRG < 14)
			menu_scroll_LISPRG++;
		if(menu_LISPRG > menu_LISPRG_SZ){
			menu_LISPRG = 0;
			menu_scroll_LISPRG = 0;
			}
		entry_LISPRG = UPDATE;
	}
}

void Menu::menu_left_LISPRG()
{
	if(curItem->id == idListProg){
		if(menu_LISPRG == 0)
			return;
		for(int i = 0 ; i < 13;i++){
			menu_up_LISPRG();
			if(menu_LISPRG == 0)
				break;
		}
	}
}

void Menu::menu_right_LISPRG()
{
	if(curItem->id == idListProg){
		if(menu_LISPRG == menu_LISPRG_SZ)
			return;
		for(int i = 0 ; i < 13;i++){
			menu_down_LISPRG();
			if(menu_LISPRG == menu_LISPRG_SZ)
				break;
			}
	}
}

void Menu::menu_enter_LISPRG()
{
	if(curItem->id == idListProg){
		//запускаем редактор программ
		if(!myflash.check_spi_at45()){
			if(Lir.btn_zapusk_prg && check_prg(0)){
				//запускаем программу сразу на отработку если там есть кадры
				PrgorSubPrg = 0;
				choice_frame_EDITPR();
				entry_LISPRG = INIT;
			}else{					
					entry_LISPRG = INIT;
					PrgorSubPrg = 0;
					entry_cnt_EDITPR = 0;    //счетчик курсора
					entry_scroll_EDITPR = 0; //счетчик сдвига
					SET_C(idEditPrg);
				}
		}			
		else
			status(PSTR("ошибка памяти"),200,0);
	}
}

void Menu::menu_stop_LISPRG()
{
	if(curItem->id == idListProg){
		SET_C(idPrgOrSubPrg);
		entry_LISPRG = INIT;
	}
}

void Menu::actions_LISSUB(const unsigned char & page)
{
	if(entry_LISSUB == INIT)
	init_menu_LISSUB(page);
	else if(entry_LISSUB == UPDATE)
	draw_menu_LISSUB(page);
	else if(entry_LISSUB == IDLE)
	;
	else
	;
}

void Menu::init_menu_LISSUB(const unsigned char & page)
{
	display.clear_ram_null();
	entry_LISSUB = UPDATE;
}

void Menu::draw_menu_LISSUB(const unsigned char & page)
{
	menu_LISSUB_SZ = MAX_SUBPRG - 1;
	for(uint8_t i = menu_scroll_LISSUB; i < menu_scroll_LISSUB + 14;i++)
	{
		if(menu_LISSUB == i)
			INVERT_CHAR()
		else
			NORMAL_CHAR()
		display.print_flash_local(sPrgOrSubPrg[1],page,0,16 * (i - menu_scroll_LISSUB));
		print_long_format(i + 1,CPrg::FORMAT::FORMAT_END,page,15,16 * (i - menu_scroll_LISSUB),1,1,2);
		NORMAL_CHAR()
		show_comm_in_list(1,i,menu_scroll_LISSUB,17);
	}
	NORMAL_CHAR()
	if(menu_scroll_LISSUB == 36)
		display.put_char_local_independent(' ',page,0,16 * 14);
	else
		display.put_char_local_independent(0x1F,page,0,16 * 14); 
	entry_LISSUB = IDLE;
}

void Menu::menu_up_LISSUB()
{
	if(curItem->id == idListSubProg){
		menu_LISSUB--;
		if(menu_LISSUB < 43 && menu_scroll_LISSUB)
			menu_scroll_LISSUB--;
		if(menu_LISSUB < 0){
			menu_LISSUB = menu_LISSUB_SZ;
			menu_scroll_LISSUB = 36;
		}
		entry_LISSUB = UPDATE;
	}
}

void Menu::menu_down_LISSUB()
{
	if(curItem->id == idListSubProg){
		menu_LISSUB++;
		if(menu_LISSUB > 7 && menu_LISSUB < 44)
			menu_scroll_LISSUB++;
		if(menu_LISSUB > menu_LISSUB_SZ){
			menu_LISSUB = 0;
			menu_scroll_LISSUB = 0;
		}
		entry_LISSUB = UPDATE;
	}
}

void Menu::menu_left_LISSUB()
{
	if(curItem->id == idListSubProg){
		if(menu_LISSUB == 0)
		return;
		for(int i = 0 ; i < 13;i++){
			menu_up_LISSUB();
			if(menu_LISSUB == 0)
			break;
		}
	}
}

void Menu::menu_right_LISSUB()
{
	if(curItem->id == idListSubProg){
		if(menu_LISSUB == menu_LISSUB_SZ)
		return;
		for(int i = 0 ; i < 13;i++){
			menu_down_LISSUB();
			if(menu_LISSUB == menu_LISSUB_SZ)
			break;
		}
	}
}

void Menu::menu_enter_LISSUB()
{
	if(curItem->id == idListSubProg){
		//запускаем редактор программ
		if(!myflash.check_spi_at45()){
			if(Lir.btn_zapusk_prg && check_prg(1)){
				//запускаем программу сразу на отработку если там есть кадры
				PrgorSubPrg = 1;
				choice_frame_EDITPR();
				entry_LISSUB = INIT;
			}
			else{
			entry_LISSUB = INIT;
			PrgorSubPrg = 1;
			entry_cnt_EDITPR = 0;    //счетчик курсора
			entry_scroll_EDITPR = 0; //счетчик сдвига
			SET_C(idEditPrg);
			}
		}
		else
			status(PSTR("ошибка памяти"),200,0);
	}
}

void Menu::menu_stop_LISSUB()
{
	if(curItem->id == idListSubProg){
		SET_C(idPrgOrSubPrg);
		entry_LISSUB = INIT;
	}
}

void Menu::show_cmd(const unsigned char & page,bool show, bool showInstr,bool inv)
{
	init_resz_prg_v();
	if (show) {
		draw_prg(_resz_prg_v.SZ, _choice_prg.cnt, entry_scroll_EDITPR, entry_cnt_EDITPR, inv, page, _resz_prg_v.y_offset, _choice_prg.addres_prg, 1);
	}
	else {
		for (int y = _resz_prg_v.y_offset + 1; y < _resz_prg_v.y_offset + _resz_prg_v.SZ + 1; y++)
			display.print_symbols_local(' ', 29, page, 0, y * 16);
		display.print_symbols_local(' ',10, page, 30, 16 * 14);
	}

	if(showInstr)
		show_Instr(page,_resz_prg_v.SZ);
	else{
		if (_resz_prg_v.SZ){//проверка если на экране 4ыре строки
			for (int y = 11; y < 15; y++)
				display.print_symbols_local(' ', 10, page, 30, y * 16);
		}
		status(PSTR(""));
	}	
}
/*
const unsigned char & page - страница памяти экрана
int8_t SZ - свободное место  0 - места нет (4 строки)
*/
void Menu::show_Instr(const unsigned char & page,int8_t SZ)
{
	MsgKey.submode_change_offset_inside_HAND();
	if (!SZ)//проверка если на экране 4ыре строки
	return;

	display.print_symbols_local(' ',6,page,34,10 * 16);//6 символов над рамкой

	for (int y = 11; y < 15; y++)
	display.put_char_local_independent((char)0x81, page, 30, y * 16);//вертикальная черта
	//==H
	display.put_char_local_independent('H', page, 33, 11 * 16);
	print_long_format(_modals_cmd.H.cnt, CPrg::FORMAT_END, page, 35, 11 * 16,1,1);
	//==
	//==M3
	if(_modals_cmd.M3 == spSTOP)
		display.print_flash_local(PSTR("M5"), page, 31, 11 * 16);
	else if(_modals_cmd.M3 == spCW)
		display.print_flash_local(PSTR("M3"), page, 31, 11 * 16);
	else if(_modals_cmd.M3 == spCCW)
		display.print_flash_local(PSTR("M4"), page, 31, 11 * 16);
	//==
	//==G90
	if (_modals_cmd.G90 == 0)
		display.print_flash_local(PSTR(" G90"), page, 36, 11 * 16);
	else if (_modals_cmd.G90 == 1)
		display.print_flash_local(PSTR(" G91"), page, 36, 11 * 16);
	//==
	//==G17 - G19
	const char* p = nullptr;
	if(_modals_cmd.G17 == 17)
		p = PSTR("G17T");
	else if(_modals_cmd.G17 == 18)
		p = PSTR("G18T");	
	else if(_modals_cmd.G17 == 19)
		p = PSTR("G19T");	
	display.print_flash_local(p, page, 31, 12 * 16);
	p = nullptr;
	print_long_format(_modals_cmd.T_instr, CPrg::FORMAT_END, page, 35, 12 * 16,0,0,1);
	//==
	//==G53 - G58
	display.put_char_local_independent('G',page,37,12 * 16);
	print_long_format(MsgKey.offset_number + 53, CPrg::FORMAT_END, page, 38, 12 * 16);
	//==
	//G40 G45 G46	
	if(_modals_cmd.G40 == 40)
		p = PSTR("G40");
	else if(_modals_cmd.G40 == 45)
		p = PSTR("G45");
	else if(_modals_cmd.G40 == 46)
		p = PSTR("G46");
	display.print_flash_local(p, page, 31, 13 * 16);
	p = nullptr;
	//G43 G44 G49
	if(_modals_cmd.G43 == 43)
		p = PSTR("G43 ");
	else if(_modals_cmd.G43 == 44)
		p = PSTR("G44 ");
	else if(_modals_cmd.G43 == 49)
		p = PSTR("G49 ");
	display.print_flash_local(p, page, 34, 13 * 16);
	p = nullptr;
	//==G0
	char tmp;
	if(extPULT.on)
		tmp = extPULT.G0;
	else
		tmp = _modals_cmd.G0;
	if (tmp == 0)
		display.print_flash_local(PSTR("G0"), page, 38, 13 * 16);
	else if (tmp == 1)
		display.print_flash_local(PSTR("G1"), page, 38, 13 * 16);
	//==
	//==корректор и F
	if (is_Analog()){
		if(_modals_cmd.F >= 0){
			print_long_format(_modals_cmd.Feed, CPrg::FORMAT_END, page, 33, 14 * 16,RIGHT_ALIGN,0,2);
			display.put_char_local_independent('%',page,34,14 * 16);
			put_F();
			if(_modals_cmd.F < 990000)
				print_long_format(_modals_cmd.F / 100, CPrg::LINEAR_2, page, 39, 14 * 16,RIGHT_ALIGN,0,3,1); //будет вылезать за экран
			else
				print_long_format(_modals_cmd.F / 10000, CPrg::FORMAT_END, page, 39, 14 * 16,RIGHT_ALIGN,0,3,1); //будет вылезать за экран
		}else{
			display.print_symbols_local(' ',9,0,31,16 * 14);
		}	
	}
}

void Menu::put_F(){

	char tmp = 'F';
	for(int c = 0; c < N_AXIS + N_SA;c++){
		if(sts_c[c] || ch_to_move & plb2(c)){			
			if(flash_prg.axis_cfg[c].type == CPrg::CIRCLE)
				tmp = 'S';	
			break;						
		}
	}
	display.put_char_local_independent(tmp,0,35,14 * 16);
}

int Menu::get_one_cmd(char *frm,char *one_cmd, int MAX_SZ)
{
	for (int i = 0; i < MAX_SZ; i++) {
		one_cmd[i] = frm[i];
		if (frm[i] == ' ' || frm[i] == '\0')
		return i + 1;
	}
	return MAX_SZ;
}

void Menu::show_current_frame_input(const unsigned char & page,long cnt)
{
	//==выводит номер вводимого кадра
	//if (cnt_frame > MAX_FRAME)
	//	cnt_frame = MAX_FRAME;

	uint8_t tmp_buf[5] = { 0 };
	cnt++;
	display.itoa_quick_lir(&cnt, tmp_buf);
	add_filler(tmp_buf, 5, 2, '0');
	tmp_buf[3] = ':';
	tmp_buf[4] = '\0';
	display.print_ram_local((char*)tmp_buf, page, 0, 16 * 12);
	//==
	//display.print_flash_local(probel21, page, 4, 16 * 13);
	display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY - 4, page, 4, 16 * 12);
	display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY, page, 0, 16 * 13);
}

void Menu::clr_cmd(const unsigned char & page)
{
	for(int i = 0; i < MAX_FRAME ; i++){
		display.print_flash_local(sProbel21,page,0,16 * (10 + i));
	}
}
/*
функция добавление кадра по нажатию ENTER в конце ввода
*/
void Menu::add_frame()
{
	/*for (int i = 1; i  < MAX_FRAME ; i++) {
		memset(frame_cmd[i - 1], '\0', FRAME_SZ);
		display.strcpylir(frame_cmd[i - 1], frame_cmd[i]);
		memset(frame_cmd[i], '\0', FRAME_SZ);
	} */
	memset(frame_cmd[MAX_FRAME - 1], '\0', FRAME_SZ);
	//====если в последнем кадре только частота то сразу ее записываем в gl_F
	exe_F_only(bufASCIIstr, IN_BUF_SZ);
	//===
	set_blank_cmd((char*)bufASCIIstr, IN_BUF_SZ);
	set_XYZIJ_format_cmd((char*)bufASCIIstr, IN_BUF_SZ);	
	//cnt_frame = 1;
	set_num_str(bufASCIIstr, IN_BUF_SZ,0);
		
	display.strncpy_lir(frame_cmd[MAX_FRAME - 1], (const char*)bufASCIIstr, FRAME_SZ); //копируем полностью строку вместо
}

void Menu::add_edit_frame()
{
	//====если в последнем кадре только частота то сразу ее записываем в gl_F
	exe_F_only(bufASCIIstr, IN_BUF_SZ);
	//===
	set_blank_cmd((char*)bufASCIIstr, IN_BUF_SZ);
	set_XYZIJ_format_cmd((char*)bufASCIIstr, IN_BUF_SZ);
	
	display.strncpy_lir(frame_cmd[frame_CURSOR_exe] + 4, (const char*)bufASCIIstr + 1, FRAME_SZ);
}

/*
функция ищет в строке 'F' и если найден то записывает ее в gl_F и показывает на экране + сохранение в EEPROM
*/
void Menu::exe_F_only(unsigned char *str,const int& sz)
{
	for(int i = 0; i <sz ; i++){
		if(str[i] == 'F'){
			long tmp = 0;
			get_F_from_cmd(&tmp,(char *)str,sz);
			#if (configUSE_mcx514 == 1)
			//save_gl_F();
			#endif
			show_Instr(0,_resz_prg_v.SZ);
			return;
		}			
	}	
}

void Menu::set_blank_cmd(char *frame,int sz_frame)
{
	for (int i = 0; frame[i] != '\0'; i++)
	{
		if (!(frame[i] >= '0' && frame[i] <= '9')
		&& frame[i] != '.' && frame[i] != '-'
		&& frame[i] != '°' && frame[i] != 0x01 && frame[i] != 0x02)
		{
			//==если пробелы есть, то пропуск
			if (frame[i] == ' ' || frame[i - 1] == ' ')
			continue;
			//==
			display.insert_sym((unsigned char*)frame, sz_frame, display.strnlen_lir(frame, sz_frame) - i, ' ');
		}
	}
}

void Menu::set_XYZIJ_format_cmd(char *frame,int sz_frame)
{
	for (int i = 0; frame[i] != '\0'; i++)
	{
		if (frame[i] == pgm_read_byte_near(sAxis_screen[Str[0]] + 2) || 
		    frame[i] == pgm_read_byte_near(sAxis_screen[Str[1]] + 2) || 
			frame[i] == pgm_read_byte_near(sAxis_screen[Str[2]] + 2) || 
			frame[i] == pgm_read_byte_near(sAxis_screen[Str[3]] + 2) ||
			frame[i] == 'I' || frame[i] == 'J')
		{
			char sym_fr = frame[i];
			for (int f = i; f < sz_frame; f++)
			{
				if (frame[f] == '\0') {
					char buf[IN_BUF_SZ] = { 0 };					
					int dif = to_format_cmd(buf, frame, i, get_format_from_ASCII(sym_fr));
					int st = i;

					if (display.strnlen_lir(buf , IN_BUF_SZ) + i > FRAME_SZ - 4) {
						char e = 3;
						while (e--)
						frame[i--] = '\0';
						break;
					}

					//add
					if (dif < 0) {
						for (int d = 0; d > dif; d--) {
							int k = display.strnlen_lir(buf, IN_BUF_SZ);
							for (; frame[k + i] != '\0'; k++)
							{
								frame[k+i] = frame[k + i + 1];
							}
							f--;
						}
					}
					//
					for (int d = 0; d < dif; d++) {
						int k = f;
						for (; k != i - 1; k--)
						{
							frame[k + 1] = frame[k];
						}
						frame[k + 1] = ' ';
						i++;
						f++;
					}
					for (int v = 0; buf[v] != '\0'; st++,v++)
					{
						if (frame[st] == '\0')
						break;
						frame[st] = buf[v];
					}
					break;
				}
			}
		}
	}
	
	//==обрезка кадра если кадр вышел за границы
		/*for(int i = 0 ; i < FRAME_SZ; i++){
			if(frame[i] == '\0' && i >=50){
				while(frame[i] != ' '){
					frame[i--] = '\0';
				}
				break;
			}
			else
				break;
		}*/
	//==	
}


void Menu::set_num_str(uint8_t * frame, int sz_frame, const short& cnt)
{
	uint8_t tmp_buf[5];
	long tmp_cnt = cnt;
	display.itoa_quick_lir(&tmp_cnt, tmp_buf);
	add_filler(tmp_buf, 5, 2, '0');
	tmp_buf[3] = ':';
	tmp_buf[4] = '\0';

	for(int i = 0; i < 4;i++)
		display.insert_sym(frame, sz_frame, display.strnlen_lir((char*)frame, sz_frame) - i, tmp_buf[i]);

	display.delete_sym(frame, sz_frame, ' ');
}

int Menu::to_format_cmd(char * buf, char * frame, int & i,int format)
{
	i++;
	int k = 0;
	int a,b;
	bool minus = false;
	for (int j = i;; j++, k++)
	{
		if (frame[j + 1] == '-') {
			j++;
			minus = true;
		}
		if (frame[j] == ' ' || frame[j] == '\0') {
			buf[k] = '\0';
			break;
		}
		buf[k] = frame[j];
	}
	a = mystr.strnlen_lir(buf,IN_BUF_SZ);
	long data = 0;

	if (format == CPrg::FORMAT::CIRCLE_SECUNDO || format == CPrg::FORMAT::CIRCLE_MINUTO) {
		ConvertASCIIcircle_secundo_tolong_new(data, format, (unsigned char*)buf);
		convert_long_to_secundo2(data, (unsigned char*)buf, format);
		//uart_usb.TransmitBlock((unsigned char*)buf,30);
	}
	else {
		ConvertASCIIlinear_circle_tolong(data, format, (unsigned char*)buf);  //bin<< str
		//if(format == CPrg::FORMAT::LINEAR_INCH_4)
		//	coeff.div(255,&data,1,254);// делим
		mystr.itoa_quick_lir(&data, (unsigned char*)buf);
		mystr.formatting2(buf, format); //bin >> str
	}
	
	b = mystr.strnlen_lir(buf,IN_BUF_SZ);

	int dif = 0;

	if (minus)
		dif = (b - a) - 1;
	else
		dif = b - a;

	return dif;
}

void Menu::set_num_frames()
{
	for (int i = 0; i < MAX_FRAME; i++) {
		if (frame_cmd[i][0] != '\0') {
			for (int j = 0; i < MAX_FRAME; j++,i++) {
				uint8_t tmp_buf[4];
				long tmp_cnt = j+1;
				display.itoa_quick_lir(&tmp_cnt, tmp_buf);
				add_filler(tmp_buf, 5, 2, '0');
				tmp_buf[3] = '\0';

				for (int a = 0; a < 3; a++)
				frame_cmd[i][a] = tmp_buf[a];
			}
			break;
		}
	}
}

void Menu::actions_cmd(const unsigned char & page)
{
	if (entry_cmd == INIT) {
		init_cmd(page);
	}

	if (entry_cmd == IDLE) {
		input_cmd(page);
	}
	/*if (entry_cmd == INIT_EDIT_CMD) {
		init_cursor_pos(54);
		//скопировать выделенную строку кадра в bufferASCII ,установить смещение курсора,Update_input_str = true
		display.strcpylir((char*)bufASCIIstr, frame_cmd[frame_CURSOR_exe] + 4);
		int n_tmp = display.strnlen_lir(frame_cmd[frame_CURSOR_exe]);
		cursor_pos = n_tmp - 4;
		result[1] = frame_cmd[frame_CURSOR_exe][n_tmp - 1];
		Update_input_str = true;
		//==
		show_func_cmd(page);
		entry_cmd = EDIT_CMD; 
	}
	if (entry_cmd == EDIT_CMD) {
		//edit_input_cmd(page);
	}*/
}

void Menu::init_cmd(const unsigned char &page)
{
	init_cursor_pos(54);
	show_func_cmd_3btn(page);
	
	for(int i = 0 ; i < 3 ; i++){		
		display.print_symbols_local(' ',30,page,0, 16 * (11 + i));
	}

	/*int cnt = display.strnlen_lir(frame_cmd[MAX_FRAME - 1] + 4);
	if(cnt){
		for(int i = 0; i < STR_FRAME_SZ_DISPLAY - 4;i++){
			if(frame_cmd[MAX_FRAME - 1][4 + i] == '\0'){
				display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY - (4 + i), page,4 + i , 16 * 11);
				break;
			}
			display.put_char_local_independent(frame_cmd[MAX_FRAME - 1][4 + i],page,4 + i,16 * 11);
		}
	} */

	
	status(PSTR("Преднабор"), 0, page);
	entry_cmd = IDLE;
}

void Menu::input_cmd(const unsigned char &page)
{
	update_func_cmd_3btn(page);
	flashing_cursor(4, 16 * 12, page);

	//взять букву , найти по букве канал, записать формат в форматM
	// для определения текущего формата для ввода можно здесь парсить строку (букву оси)
	detect_format();	

	if (ResultForInputNum == input::ok) {
		//если ничего не введено то
		if(bufASCIIstr[0] == '\0'){
			init_cursor_pos();
			return;
		}
		
		add_frame();
		//Lir.show_current_frame_input(page, cnt_frame);
		//display.print_flash_local(probel21, page, 1, 16 * 14);
		MsgKey.SET_M(TMsgKey::MODE::HAND);
		SET_C(idHand);
		init_cursor_pos();
		entry_cmd = INIT;
		mPR.on = false;
		if_input_used_cmd = 1;		
	}
	if (ResultForInputNum == input::cancel_) {
		exit_cmd(page);	//выходим из режима ввода
		if(frame_cmd[MAX_FRAME - 1][4] == '\0') //выходим из преднабора
			Lir.exit_hand_CMD_CTRL(0);
		else
			if_input_used_cmd = 1;		//остаемся в преднаборе
	}
}

void Menu::edit_input_cmd(const unsigned char &page)
{
	update_func_cmd(page);
	// здесь вычислить позицию курсора
	int x, y;
	x = 4;
	for (int i = MAX_FRAME - 4 - scroll_exe, j = 0; j < 4; i++, j++) {
		if (i == frame_CURSOR_exe)
		y = 16 * (10 + j);
	}
	flashing_cursor(x, y, page);
	if (menu.ResultForInputNum == input::ok) {
		//display.print_flash_local(probel21, page, 1, 16 * 14);
		//Lir.goto_mode(TMsgKey::MODE::HAND,TMsgKey::MODE::INPUT_PARAM,0);
		//menu.curItem = &menu.mHand;
		add_edit_frame();
		SET_C(idExecmd);
		init_cursor_pos();
		entry_cmd = INIT;
		entry_exe = UPDATE;
	}
	if (menu.ResultForInputNum == input::cancel_) {
		//display.print_flash_local(probel21, page, 1, 16 * 14);
		//Lir.goto_mode(TMsgKey::MODE::HAND, TMsgKey::MODE::INPUT_PARAM, 0);
		//menu.curItem = &menu.mHand;
		SET_C(idExecmd);
		init_cursor_pos();
		entry_cmd = INIT;
		entry_exe = UPDATE;
	}
}

/*
функция отрисовывает кнопки напротив _key_BLANK1,_key_BLANK2,_key_BLANK3,_key_BLANK4
в режиме ввода командной строки
*/
void Menu::show_func_cmd(const char & page)
{
	display.put_button_(X_BUTTON_PLACE, Y_ONE);
	display.put_button_(X_BUTTON_PLACE, Y_TWO);
	display.put_button_(X_BUTTON_PLACE, Y_THREE);
	display.put_button_(X_BUTTON_PLACE, Y_FOUR);

	if (func_cmd == 0)
	{
		display.print_flash_local(PSTR(" M  "), page, 35, 12);
		display.print_flash_local(PSTR(" G  "), page, 35, 70);
		display.print_flash_local(PSTR(" H  "), page, 35, 126);
		display.print_flash_local(PSTR(" F  "), page, 35, 178);
	}
	if (func_cmd == 1)
	{
		display.print_flash_local(PSTR(" L  "), page, 35, 12);
		display.print_flash_local(PSTR(" T  "), page, 35, 70);
		display.print_flash_local(PSTR(" E  "), page, 35, 126);
		display.print_flash_local(PSTR(" -  "), page, 35, 178);
	}
	if (func_cmd == 2)
	{
		display.print_flash_local(PSTR(" -  "), page, 35, 12);
		display.print_flash_local(PSTR(" -  "), page, 35, 70);
		display.print_flash_local(PSTR(" -  "), page, 35, 126);
		display.print_flash_local(PSTR(" -  "), page, 35, 178);
	}
	//if(entry_cmd != EDIT_CMD)
	//	entry_cmd = IDLE;
}

void Menu::show_func_cmd_3btn(const char& page)
{
	display.put_button_(X_BUTTON_PLACE, Y_ONE);
	display.put_button_(X_BUTTON_PLACE, Y_TWO);
	display.put_button_(X_BUTTON_PLACE, Y_THREE);
	//func_cmd = 0;
	if (func_cmd == 0)
	{
		display.print_flash_local(PSTR(" G  "), page, 35, 12);
		display.print_flash_local(PSTR(" M  "), page, 35, 70);
		display.print_flash_local(PSTR(" T  "), page, 35, 126);
	}
	if (func_cmd == 1)
	{
		display.print_flash_local(PSTR(" F  "), page, 35, 12);
		display.print_flash_local(PSTR(" I  "), page, 35, 70);
		display.print_flash_local(PSTR(" J  "), page, 35, 126);
	}
	if (func_cmd == 2)
	{
		display.print_flash_local(PSTR(" -  "), page, 35, 12);
		display.print_flash_local(PSTR(" -  "), page, 35, 70);
		display.print_flash_local(PSTR(" -  "), page, 35, 126);
	}
	if (curItem->id == idCmd) {
		//if (entry_cmd != EDIT_CMD)
		entry_cmd = IDLE;
	}
}

/*
функция обновляет рисунок кнопки по нажатию  кнопки 'F'
*/
void Menu::update_func_cmd(const char & page)
{
	if ( func_cmd == 0 && trigger_func_cmd == 0) {
		show_func_cmd(page);
		trigger_func_cmd = 1;
	}
	if ( func_cmd == 1 && trigger_func_cmd == 1) {
		show_func_cmd(page);
		trigger_func_cmd = 0; // только два набора функций
	}
	if ( func_cmd == 2 && trigger_func_cmd == 2) {
		show_func_cmd(page);
		trigger_func_cmd = 0;
	}
}

void Menu::update_func_cmd_3btn(const char& page)
{
	static unsigned char trigger;

	if (func_cmd == 0 && trigger == 0) {
		show_func_cmd_3btn(page);
		trigger = 1;
	}
	if (func_cmd == 1 && trigger == 1) {
		show_func_cmd_3btn(page);
		trigger = 0;
	}
	/*if (func_cmd == 2 && trigger == 2) {
		show_func_cmd_3btn(page);
		trigger = 0;
	} */
}

void Menu::exit_cmd(const char & page)
{
	display.print_symbols_local(' ', STR_FRAME_SZ_DISPLAY, page, 0, 16 * 13);
	//for (int i = 0; i < 13; i++)
	//	display.put_char_local_independent(' ', page, 1 + i, 16 * 14);
	MsgKey.SET_M(TMsgKey::MODE::HAND);
	SET_C(idHand);
	init_cursor_pos();
	entry_cmd = INIT;
}

/*
функция обрабатывает нажатие кнопок _key_BLANK1_L _key_BLANK2_L _key_BLANK3_L _key_BLANK4_L
в режиме ввода кадра (curItem->id == idCmd)
*/
void Menu::blank_left_input_cmd(const char& BLANK)
{
	if(curItem->id == idCmd || curItem->id == idEditPrgInput){

		if(BLANK == _key_BLANK1_L){
			if(Str[0] != no_axis){
				menu.sym = pgm_read_byte_near(sAxis_screen[Str[0]] + 2);
				menu.bufASCIIstr[menu.cursor_pos] = menu.sym;
				menu.cursor_pos++;
			}
			return;
		}
		if(BLANK == _key_BLANK2_L){
			if(Str[1] != no_axis){
				menu.sym = pgm_read_byte_near(sAxis_screen[Str[1]] + 2);
				menu.bufASCIIstr[menu.cursor_pos] = menu.sym;
				menu.cursor_pos++;
			}
			return;
		}
		if(BLANK == _key_BLANK3_L){
			if(Str[2] != no_axis){
				menu.sym = pgm_read_byte_near(sAxis_screen[Str[2]] + 2);
				menu.bufASCIIstr[menu.cursor_pos] = menu.sym;
				menu.cursor_pos++;
			}
			return;
		}
		if(BLANK == _key_BLANK4_L){
			if(Str[3] != no_axis){
				menu.sym = pgm_read_byte_near(sAxis_screen[Str[3]] + 2);
				menu.bufASCIIstr[menu.cursor_pos] = menu.sym;
				menu.cursor_pos++;
			}
			return;
		}
	}
}

void Menu::blank_right_input_cmd(const char& BLANK)
{
	if (curItem->id == idEditPrgInput)
	{
		switch (BLANK)
		{
			case _key_BLANK1:
			if (func_cmd == 0)menu.sym = 'M';
			else if (func_cmd == 1)menu.sym = 'L';
			else if (func_cmd == 2) return;
			break;
			case _key_BLANK2:
			if (func_cmd == 0)menu.sym = 'G';
			else if (func_cmd == 1)menu.sym = 'T';
			else if (func_cmd == 2) return;
			break;
			case _key_BLANK3:
			if (func_cmd == 0)menu.sym = 'H';
			else if (func_cmd == 1)menu.sym = 'E';
			else if (func_cmd == 2) return;
			break;
			case _key_BLANK4:
			if (func_cmd == 0){
				char tmp = 'F';
				if(if_Type_Circ())
					tmp = 'S';
				menu.sym = tmp;
			}
			else if (func_cmd == 1)	return;
			else if (func_cmd == 2) return;
			break;
			default:
			return;
		}

		menu.bufASCIIstr[menu.cursor_pos] = menu.sym;
		menu.cursor_pos++;
	}
	else if(menu.curItem->id == idCmd ){
		switch (BLANK)
		{
			case _key_BLANK1:
			if(func_cmd == 0)menu.sym = 'G';
			else if (func_cmd == 1)menu.sym = 'F';
			else if(func_cmd == 2) return;
			break;
			case _key_BLANK2:
			if (func_cmd == 0)menu.sym = 'M';
			else if (func_cmd == 1)menu.sym = 'I';
			else if (func_cmd == 2) return;
			break;
			case _key_BLANK3:
			if (func_cmd == 0)menu.sym = 'T';
			else if (func_cmd == 1)menu.sym = 'J';
			else if (func_cmd == 2) return;
			break;
			case _key_BLANK4:
			if (func_cmd == 0)return;
			else if (func_cmd == 1)	return;
			else if (func_cmd == 2) return;
			break;
			default:
			return;
		}

		menu.bufASCIIstr[menu.cursor_pos] = menu.sym;
		menu.cursor_pos++;
	}
}

/*
функция вызывается по нажатию кнопки 'F'
*/
void Menu::change_func_cmd_F()
{		
	func_cmd++;
	if(func_cmd > 1)
		func_cmd = 0;
		
	//if(Lir.curItem->id == idCmd){
	//	func_cmd = 0;		
	//}
}

void Menu::key_F4_cmd()
{
	if(menu.curItem->id == idCmd){
		char tmp = 'F';
		if(if_Type_Circ())
			tmp = 'S';

		menu.sym = tmp;
		menu.bufASCIIstr[menu.cursor_pos++] = menu.sym;
	}
}

void Menu::detect_format()
{
	for(int i = cursor_pos - 1 ; i >= 0; i--){
		if(bufASCIIstr[i] >= 'A' && bufASCIIstr[i] <= 'Z'){
			CurFormatM = get_format_from_ASCII(bufASCIIstr[i]);
			break;
		}
	}
}
/*
возвращает 1 если последняя ось круговая
*/
bool Menu::if_Type_Circ()
{
	for(int i = cursor_pos - 1 ; i >= 0; i--){
		if(bufASCIIstr[i] >= 'A' && bufASCIIstr[i] <= 'Z'){
			if(bufASCIIstr[i] >= 'A' && bufASCIIstr[i] <= 'D')
				return 1;
			else
				return 0;
		}
	}

	return 0;
}

bool Menu::_ch_sw::if_sw_ON()
{
	if(plus || minus)
		return 1;
	else 
		return 0;
}

void Menu::actions_exe(const unsigned char & page)
{
	if(INmove() && upd_m_icons){
		put_moving_icons(page);	 //0.748 ms 
		upd_m_icons = false;
		}	

	if (entry_exe == INIT)
		Init_exe(page);

	if (entry_exe == UPDATE_BLANK_FUNC)
		show_func_exe(page);

	if (entry_exe == IDLE)
		return;

	if (entry_exe == UPDATE) {
		show_func_exe(page);
		change_exe(page);
		entry_exe = IDLE;	
	}
	if (entry_exe == AUTO_EXECUTE_EXE) {
		//задержка между кадрами от 4 до 8,5 msek
		if(!(flash_prg._INGen.smena_kadra & YESNO_P_bm)){
			if(!inP(flash_prg._INGen.smena_kadra))
				return;
			}
		choice_exe_down();
		change_exe(page);
		entry_exe = AUTO_EXECUTE_EXE;
		execute_exe(page);
	}

	if (entry_exe == EXECUTE_EXE) {
		if(if_ENC(-1)){
			entry_exe = INIT;
			return;
		}
		status(PSTR("В работе"));
		execute_exe(page);
		first_frame = false;
	}

	if (entry_exe == EXIT_EXE || entry_exe == INIT_WORK_EXE)
		exit_exe(page);
	if (entry_exe == DELAY_FRM) {
		if (tmr_frame()) {
			if (config_NC.one_frame)
				entry_exe = ONE_FRM_DOWN;
			else
				entry_exe = AUTO_EXECUTE_EXE;
		}
	}

	if (entry_exe == DELAY_M5M3M4) {
		if (cnt_M3M4 || cnt_M5)
			return;
		//if(SHPINDEL){
			Lir.if_pass_frm();
			return;
		//	}
	}

	if(entry_exe == DELAY_WAIT_SHPINDEL){
		if(cnt_wait_shpindel || cnt_wait_shpindel_off){
			if(cnt_wait_shpindel)
				Lir.status(PSTR("Разгон шпинделя"));
			return;
			}
			//new
		if(cnt_M3M4 || cnt_M5){
			entry_exe = DELAY_M5M3M4;
			return;
			}
			//
		if(alert == Menu::SHPINDEL_NE_VRASHAETSIA || alert == Menu::SHPINDEL_NE_OSTANOVLEN)
			entry_exe = INIT;
		else {
			status(PSTR("В работе"));
			if_pass_frm();
			return;
		}			
	}

	if(entry_exe == DELAY_INPUT_M){
		char res = inP(*p_M102_gl,1,1);
		if(res == 1){					
			if (config_NC.one_frame)
				entry_exe = ONE_FRM_DOWN;
			else
				entry_exe = AUTO_EXECUTE_EXE;
			p_M102_gl = nullptr;
			}
	}
	
	if (entry_exe == ONE_FRM_DOWN) {
		choice_exe_down();
		if (config_NC.one_frame) {
			rewind_comm();
		}		
		status(PSTR("Нажмите СТАРТ"), 0, page);
	}
		
}

void Menu::Init_exe(const unsigned char & page)
{
	//if (!scroll_exe && (MAX_FRAME - frame_CURSOR_exe > 3))
	//frame_CURSOR_exe = MAX_FRAME - 1;
	//USART_DEBUG(entry_hand);
	init_resz_prg_v();
	if(_choice_prg.cnt)
		status(PSTR("Нажмите СТАРТ"),0,page,1);
	else
		{
			for(int i = 0; i < 3;i++)
				display.print_symbols_local(' ',STR_FRAME_SZ_DISPLAY,page,0,16 * (11 + i));
			status(PSTR(""),0,page);
		}
	if(set_view)
		set_view_prog();	
	entry_exe = UPDATE;
}

void Menu::show_func_exe(const char & page)
{
	if (func_exe == 0)
	{
		display.put_button_(X_BUTTON_PLACE,Y_ONE,_pic_SHOW_PROG);//display.put_button_(X_BUTTON_PLACE,Y_ONE,_pic_HAND);
		display.put_button_(X_BUTTON_PLACE,Y_TWO,_pic_COMMENT);
		if(config_NC.one_frame == 1)
			display.put_button_(X_BUTTON_PLACE,Y_THREE,_pic_FR_ON);
		else	
			display.put_button_(X_BUTTON_PLACE,Y_THREE,_pic_FR_OFF);
		if(!_resz_prg_v.SZ){
			//display.put_button_(X_BUTTON_PLACE,Y_FOUR);
			//display.print_flash_local(PSTR("ВИД "), page, 35, 178);
			//display.put_button_(X_BUTTON_PLACE,Y_FOUR,_pic_SHOW_PROG);
			}
	}	
	entry_exe = IDLE;
	cms.on = false;
}

void Menu::change_exe(const unsigned char & page)
{
	draw_prg(_resz_prg_v.SZ, _choice_prg.cnt, entry_scroll_EDITPR, entry_cnt_EDITPR, 1, page, _resz_prg_v.y_offset, _choice_prg.addres_prg,1);
	show_Instr(page,_resz_prg_v.SZ);	
}

void Menu::exit_exe(const unsigned char & page)
{						
	if(entry_exe == EXIT_EXE){
		outP(flash_prg._OUTGen.texnolog_stop_M0,0,1);//сбросить выход М0
		outP(flash_prg._OUTGen.konec_program_M2,0,1);
		MsgKey.SET_M(TMsgKey::MODE::HAND);
		SET_C(idHand);
		show_cmd(page,0,1);//сделать как в exe
		status(PSTR(""));
		rst_VIBOR_axis(0,1);
		MsgKey.to_put_icons_in_mode_HAND(MsgKey.tracer_real);
		entry_exe = INIT;
		mPR.on = false;
	}
	else if(entry_exe == INIT_WORK_EXE){
		outP(flash_prg._OUTGen.texnolog_stop_M0,0,1);//сбросить выход М0
		outP(flash_prg._OUTGen.konec_program_M2,0,1);
		show_Instr(page,_resz_prg_v.SZ);
		if(set_view == 1){
			if(cms.cur_fync == 0)
				cms.cur_fync = 1;//функции вида, покадровое
		}		
		put_moving_icons(page);
		entry_hand = INIT;
		entry_exe = WORKING_EXE;
	}					
}

/*
функция выполняет выбранную строку кадра  - ПОСТПРОЦЕССОР или ИНТЕРПРЕТАТОР для языка G code
*/
void Menu::execute_exe(const unsigned char & page)
{
	long ch_impulse[N_AXIS] = { 0 };
	long F = 0;
	char G = 0;
	char M = 0;
	char FR[FRAME_FLASH_SZ];
	uint32_t addres_prg;
	uint8_t chs; // выбранные каналы

	addres_prg = _choice_prg.addres_prg + (entry_cnt_EDITPR * FRAME_FLASH_SZ);
	myflash.Read_flash_at45(addres_prg, (uint8_t*)FR, FRAME_FLASH_SZ);//1.380 ms - цикл не постоянен во время смены кадра
	
	if((FR[FRAME_FLASH_SZ - 2] == 0xFF && FR[FRAME_FLASH_SZ - 1] == 0xFF) || (FR[FRAME_FLASH_SZ - 2] == 0x00 && FR[FRAME_FLASH_SZ - 1] == 0x00 )){
		;//для совместимости с версиями без crc в кадрах ,пропуск проверки
	}
	else{
		if(check_crc(crc16((uint8_t*)FR,display.strnlen_lir((const char*)FR,FRAME_FLASH_SZ - 2)),(uint8_t*)FR,FRAME_FLASH_SZ - 2)){
			status(PSTR("Ошибка кадра!"),300);
			entry_exe = INIT;
			return;
		}
	}
	//== последний ли кадр?
	last_frm();
	//==
	//==пропуск кадра с комментарием
	if (FR[3] == '/') {
		if_pass_frm();
		timer_frm = 0; // ставим таймер в ноль если кадр закоментирован
		return;
	}
	//==
	get_T_from_cmd(FR + 4, FRAME_SZ - 4);
	//== установка задержки после выполнения кадра
	if (get_G4xxdelay_from_cmd(FR + 4, FRAME_SZ - 4)) {
		entry_exe = DELAY_FRM;
		return;
	}
	//==
	//==вызов подпрограммы
	if (get_L_from_cmd(FR + 4, FRAME_SZ - 4)) {
		change_exe(page);
		if (!config_NC.one_frame)
		entry_exe = EXECUTE_EXE;
		return;
	}
	//==
	//==безусловный переход
	if (get_E_from_cmd(FR + 4, FRAME_SZ - 4)) {
		if (!config_NC.one_frame) {
			change_exe(page);
			entry_exe = EXECUTE_EXE;
		}
		else
			entry_exe = INIT;
		return;
	}
	//==
	chs = get_bin_from_cmd(ch_impulse, FR + 4, FRAME_SZ - 4);
	//==задержка только после кадра с движением
	if(!chs)
	timer_frm = 0; // ставим таймер в ноль если в кадре нет движений
	//==
	get_F_from_cmd(&F, FR + 4, FRAME_SZ - 4);
	get_G_from_cmd(&G, FR + 4, FRAME_SZ - 4);
	if(get_M_from_cmd(&M, FR + 4, FRAME_SZ - 4))
		return;
	if (M3M5M4_cmd(M,entry_exe)) //команды M3 M4 M5 только одни в строке
		return;
	get_H_from_cmd(FR + 4, FRAME_SZ - 4);

	if(!lirCommand(ch_impulse, chs,G,F)){
		entry_exe = INIT;
		return;
	}

	entry_exe = INIT_WORK_EXE;
}

void Menu::put_moving_icons(const unsigned char & page)
{
	if(cms.cur_fync == 0){
		for(int s = 0; s < N_STR;s++){
			int ch = get_channel(Str[s]);
			if(s == 3 && _resz_prg_v.SZ)
				break;
			if(s == 3 && MsgKey.tracer_next == TMsgKey::CMD_CNTRL)
				break;
			bool clr = true;									
			for(int c = 0; c < N_AXIS;c++){
				if(ch == c && sts_c[c]){				
					display.put_button_(X_BUTTON_PLACE,s * 7,cms.cur_move_show[c]);
					clr = false;
					}				
			}
			if(clr)
				display.clear_button_(X_BUTTON_PLACE,s * 7);
		}	
	}
	else if (cms.cur_fync == 1){
		for(int s = 0; s < N_STR;s++){
			if(s == 3 && _resz_prg_v.SZ)
				break;
			if(s == 3 && MsgKey.tracer_next == TMsgKey::CMD_CNTRL)
				break;
			if(s == 0){
				 display.put_button_(X_BUTTON_PLACE,Y_ONE,_pic_SHOW_PROG);
			}
			else if(s == 3){
				//display.put_button_(X_BUTTON_PLACE,Y_FOUR);
				//display.print_flash_local(PSTR("ВИД "), page, 35, 178);
				// display.put_button_(X_BUTTON_PLACE,Y_FOUR,_pic_SHOW_PROG);
				}
			else if(s == 2){
				//==отключение включение покадрового режима во время движения
				if(config_NC.one_frame == 1)
					display.put_button_(X_BUTTON_PLACE,Y_THREE,_pic_FR_ON);
				else
					display.put_button_(X_BUTTON_PLACE,Y_THREE,_pic_FR_OFF);
				//==
			}
			else
				display.clear_button_(X_BUTTON_PLACE,s * 7);
		}		
	}		
	cms.on = true;
}

void Menu::change_moving_icons(int s)
{
	bool update = false;
	if(cms.cur_fync == 0){
		for(int s_ = 0 ; s_ < N_STR;s_++){
			int c = get_channel(Str[s_]);
			if(c >= N_AXIS)
				continue;
			//if(!sts_c[c])
			//	continue;
			cms.cur_move_show[c]++;
			if(cms.cur_move_show[c] > 12)
				cms.cur_move_show[c] = 10;
		}
		update = true;
	}else if(cms.cur_fync == 1){
		if(s == 0){ // показываем программы расширенно
			if(Lir.curItem->id == idExecmd){
				toggle_view();					
				change_exe(0);
			}
		}
		else if(s == 2){ // третий бланк справа
			config_NC.one_frame ^= 1;
			update = true;
			}
	}
	if(update);
		put_moving_icons(0);
}
/*
переключение функций вида, покадровое/функции перемещения
*/
void Menu::change_moving_functions()
{
	if(set_view)
		return;
	if(INmovePrednabor())
		return;
		
	if(!cms.cur_fync)
		cms.cur_fync = 1;//функции вида, покадровое
	else
		cms.cur_fync = 0;//функции перемещения

	put_moving_icons(0);
}
/*
переключение УМЕНЬШЕННЫХ КООРДИНАТ + отображение программы/большие координаты
*/
void Menu::toggle_view()
{
	if(block_str[2] && block_str[3]){
		no_view_prog();
		set_view = 0;
	}
	else{
		set_view_prog();
		set_view = 1;
	}
	
	if(Str[3] == no_axis && display.position_to_see[3] == CDisp::COORDINATE)
		set_view = 0;
}
/*
установка УМЕНЬШЕННЫХ КООРДИНАТ + отображение программы
*/
void Menu::set_view_prog()
{
	/*
	пока только переключение если программы не видно вообще, для других  случаев
	когда программа видна в три строки нужно писать код инициализации смещения курсора при
	переключениии вида из 3 строк в 7 строк
	*/
	if(Str[3] == no_axis && display.position_to_see[3] == CDisp::COORDINATE){
	 init_resz_prg_v();
	 return;
	}

	block_str[0] = 1;
	block_str[1] = 1;
	block_str[2] = 1;
	block_str[3] = 1;
	small_axis_view = 1;
	
	_resz_prg_v.ALIGN = 2;
	_resz_prg_v.SZ = 7;
	_resz_prg_v.y_offset = 6;
	ch_mem = 2;
	for(int s = 0 ; s < 14;s++){
		display.print_symbols_local(' ',s < 7 ? 34 : 4,0,s < 7 ? 0 : 30,16 * (0 + s));
	} 	
	display.print_symbols_local(' ',6,0,34,16 * 10);
}
/*
снятие Уменьшенных координат
*/
void Menu::no_view_prog()
{
	if(!block_str[2] && !block_str[3])
		return;	
	block_str[0] = 0;
	block_str[1] = 0;
	block_str[2] = 0;
	block_str[3] = 0;			
	small_axis_view = 0;
	_resz_prg_v.ALIGN = 0;
	_resz_prg_v.SZ = 0;
	_resz_prg_v.y_offset = 0;		
	for(int s = 0 ; s < 14;s++){
		display.print_symbols_local(' ',s >= 11 ? 40 : 34 ,0,0,16 * (0 + s)); 
	}
	if(is_Analog())
		display.print_symbols_local(' ',10,0,30,16 * 14);
	else
		display.put_char_local_independent(' ',0,30,16*14);
	display.put_char_local_independent(' ',0,34,16*11);
	
	//init_resz_prg_v();
}


/*
переход между кадрами для программного режима
*/
void Menu::if_pass_frm()
{
	if (config_NC.one_frame || last_frame) {
		if (last_frame && call_sub) {
			//==восстановление программы
			_choice_prg.addres_prg = cp_tmp.addres_prg;
			_choice_prg.cnt = cp_tmp.cnt;
			entry_cnt_EDITPR = ecEDITPR_tmp;
			entry_scroll_EDITPR = esEDITPR_tmp;
			//==
			call_sub--;			
			if (timer_sv)
				entry_exe = DELAY_FRM;
			else {
				if(last_frm())
					entry_exe = INIT;
				else if(config_NC.one_frame)
					entry_exe = ONE_FRM_DOWN;
				else
					entry_exe = AUTO_EXECUTE_EXE;
			}			
		}
		else if(last_frame)
			entry_exe = INIT;
		else if (config_NC.one_frame) {
			if (timer_sv)
				entry_exe = DELAY_FRM;
			else
				entry_exe = ONE_FRM_DOWN;
		}
	}
	else {
		if(timer_sv)
			entry_exe = DELAY_FRM;
		else
			entry_exe = AUTO_EXECUTE_EXE;
	}
}
/*
return 1 - кадр последний
*/
bool Menu::last_frm()
{
	if (entry_cnt_EDITPR + 1 == _choice_prg.cnt) {
		last_frame = true;
		//==для замыкания
		//entry_cnt_EDITPR = 0;
		//entry_scroll_EDITPR=0;
	}
	else
		last_frame = false;

	return last_frame;
}

/*
функция перематывает вниз если кадр закоментирован
*/
void Menu::rewind_comm()
{
	char buf[5] = { 0 };
	while (1) {
		uint32_t addres_prg = _choice_prg.addres_prg + (entry_cnt_EDITPR * FRAME_FLASH_SZ);
		myflash.Read_flash_at45(addres_prg, (uint8_t*)buf, 4);
		if (buf[3] == '/') {
			if (last_frm()) {
				if_pass_frm();
				break;
			}
			choice_exe_down();
		}
		else
			break;
	}
}
/*
Инверсия символов строк
*/
void Menu::Set_drv_axis_inv()
{
	if(Lir.curItem->id == idExecmd || Lir.entry_hand == Menu::WORKING_EXE){			
		for(int s = 0 ; s < N_STR ; s++){
			if(block_str[s])
				continue;
			int c  = get_channel(Str[s]);
			if(st_Drive & plb2(c))
				INVERT_CHAR()
			else
				NORMAL_CHAR()
			if(Str[s] != no_axis)
				display.put_axis_shot(0,s * 7,Str[s]);
			NORMAL_CHAR()
		}		
	}
}
/*
посик номера кадра в программе
*/
void Menu::searcher(short tpm_num)
{
	if (tpm_num > entry_cnt_EDITPR + 1) {
		while (tpm_num != entry_cnt_EDITPR + 1) {
			scroll_dw(entry_cnt_EDITPR, entry_scroll_EDITPR, _resz_prg_v.ALIGN, _choice_prg.cnt, _resz_prg_v.SZ);
			draw_prg(_resz_prg_v.SZ, _choice_prg.cnt, entry_scroll_EDITPR,
			entry_cnt_EDITPR, 0, 0, _resz_prg_v.y_offset, _choice_prg.addres_prg, 0);
		}
	}
	else if (tpm_num < entry_cnt_EDITPR + 1) {
		while (tpm_num != entry_cnt_EDITPR + 1) {
			scroll_up(entry_cnt_EDITPR, entry_scroll_EDITPR, _resz_prg_v.ALIGN, _choice_prg.cnt, _resz_prg_v.SZ);
			draw_prg(_resz_prg_v.SZ, _choice_prg.cnt, entry_scroll_EDITPR,
			entry_cnt_EDITPR, 0, 0, _resz_prg_v.y_offset, _choice_prg.addres_prg, 0);
		}
	}
}


#if (configUSE_NCshell == 1)
/*
функция обработки таймерных счетчиков с учетом 1 ms
*/
void Menu::timers_c()
{
	
	
}
/*
функция обработки таймерных счетчиков с учетом 10 ms
*/
void Menu::timers_c_10()
{
	cnt_tf++;// таймер для G4.
	//== задержка для переключения выбора оси
	if(cnt_galet > 1)
		cnt_galet--;
	//==
	//==M3M4 через M5
	if(cnt_M3M4 > 1)
		cnt_M3M4--;
	else if(cnt_M3M4 == 1){
		cnt_M3M4 = 0;
		outM3(M3_mem,0,1);
	}
	//==
	//==задержка на M5
	if (cnt_M5 > 1)
		cnt_M5--;
	else if (cnt_M5 == 1) {
		cnt_M5 = 0;
		outP(flash_prg._OUTGen.stop_M5, 0,1);
		if(SHPINDEL && inP(flash_prg._INGen.shpindel_vrashaetsia) != NO_P){
			alert = Menu::SHPINDEL_NE_OSTANOVLEN;
			cnt_wait_shpindel_off = 0;
			if(lock_error_SNO)
			alert = Menu::NO_ALERT;
		}
	}
	//==
	//==ждем включения шпинделя
	if(cnt_wait_shpindel > 1){
		cnt_wait_shpindel--;
		if(SHPINDEL)
			cnt_wait_shpindel = 0;
	}
	else if(cnt_wait_shpindel == 1){
		alert = Menu::SHPINDEL_NE_VRASHAETSIA;
		cnt_wait_shpindel = 0;
		outM5_delay();
	}
	//==
	//==ждем отключения шпинделя
	if(cnt_wait_shpindel_off > 1){
		cnt_wait_shpindel_off--;
		if(!SHPINDEL || inP(flash_prg._INGen.shpindel_vrashaetsia) == NO_P)
			cnt_wait_shpindel_off = 0;
	}
	else if(cnt_wait_shpindel_off == 1){
		alert = Menu::SHPINDEL_NE_OSTANOVLEN;
		cnt_wait_shpindel_off = 0;
		if(lock_error_SNO)
			alert = Menu::NO_ALERT;
	}
	//==
	
	for(int c = 0 ; c < N_AXIS;c++){
		if(lubric_tmr[c] > 0)
			lubric_tmr[c]--;
	}

	for(int c = 0 ; c < N_AXIS; c++)
		delay_PUSK_find_ref(c);
		
	for(int c = 0; c < N_AXIS; c++)
		delay_INIT_REF(c);		
}

/*
Торможение перед программным ограничением
*/
void Menu::prog_sw_zone_c()
{
	//return;
	if(!INContiniusmove()) //только в ручных перемещениях
		return;

	for(int c = 0 ; c < N_AXIS;c++){
		if(ref_.pRef[c] && sts_c[c]){
			const long CNT = conv_c(mCounter[c].abs,c,1,1);
			long ZN = flash_prg._Allp_param3[c].zone[0] * dir_exe[c];//получаем направление
			//умножаем зону на dir_exe
			if(dir_exe[c] > 0){ // едем вправо 1 (+)
				if(flash_prg._Allp_param2[c].limit_plus){
					if(CNT >= flash_prg._Allp_param2[c].limit_plus - ZN){
						if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
							outABCD(flash_prg._Allp_param[c].spd_G1[0], &flash_prg._OUTAxis[c].Out_A,c,1);
						else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
							outAnalog(Spd_to_U(flash_prg._Allp_param4[c].spdG1[0],c,_modals_cmd.G0),flash_prg._OUTAxis2[c].AnalOut,flash_prg._OUTAxis2[c].Polar,dir_exe[c],1);
					}
				}					
			}
			else if(dir_exe[c] < 0){// едем влево -1 (-)
				if(flash_prg._Allp_param2[c].limit_minus){
					if(CNT <= flash_prg._Allp_param2[c].limit_minus - ZN){
						if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
							outABCD(flash_prg._Allp_param[c].spd_G1[0], &flash_prg._OUTAxis[c].Out_A,c,1);
						else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
							outAnalog(Spd_to_U(flash_prg._Allp_param4[c].spdG1[0],c,_modals_cmd.G0),flash_prg._OUTAxis2[c].AnalOut,flash_prg._OUTAxis2[c].Polar,dir_exe[c],1);
					}
				}
			}
		}
	} 
}

/*
Срабатывание программных ограничений
*/
void Menu::prog_sw_c()
{	
	bool pl = false;
	bool ml = false;

	for(int c = 0; c < N_AXIS;c++){	
		char CH = plb2(c);
		if(ref_.pRef[c]){
			const long CNT = conv_c(mCounter[c].abs,c,1,1);
			
			if(flash_prg._Allp_param2[c].limit_plus){ //+вправо
				if(CNT >= flash_prg._Allp_param2[c].limit_plus && trg_p[c] == 0){
					ch_sw_prog.plus |= CH;
					trg_p[c] = 1;					
					pl = true;				
				}
				else if(CNT < flash_prg._Allp_param2[c].limit_plus && trg_p[c] == 1){
					ch_sw_prog.plus &= ~CH;
					trg_p[c] = 0;
				}
				//==добавка для лево вправо
				else if(CNT > flash_prg._Allp_param2[c].limit_minus && trg_m[c] == 1){
					ch_sw_prog.minus &= ~CH;
					trg_m[c] = 0;
				}										
			}
			if (flash_prg._Allp_param2[c].limit_minus){ //-влево
				if(CNT <= flash_prg._Allp_param2[c].limit_minus && trg_m[c] == 0){
					ch_sw_prog.minus |= CH;
					trg_m[c] = 1;
					ml = true;
				}
				else if(CNT > flash_prg._Allp_param2[c].limit_minus && trg_m[c] == 1){
					ch_sw_prog.minus &= ~CH;
					trg_m[c] = 0;
				}
				//==добавка для лево вправо
				else if(CNT < flash_prg._Allp_param2[c].limit_plus && trg_p[c] == 1){
					ch_sw_prog.plus &= ~CH;
					trg_p[c] = 0;
				}
			}
		}
		else{ // сброс ошибок если нет метки
			trg_p[c] = 0;
			trg_m[c] = 0;
			ch_sw_prog.plus &= ~CH;
			ch_sw_prog.minus &= ~CH;
		}
	}
	if(pl)
		move_axis_STOP(ch_sw_prog.plus);
	if(ml)
		move_axis_STOP(ch_sw_prog.minus);
}
/*
остановка по переполнению счетчика
*/
void Menu::overf_c()
{
	for(int c = 0; c < N_AXIS;c++){
		if((init.mRef_ext[c] && mCounter[c].ovfl_abs) || (!init.mRef_ext[c] && mCounter[c].ovfl_rel) || mCounter[c].error){
			move_axis_STOP(0xFF);
			mCounter[c].error = true;
		}
	}
}
/*
серво цикл для перемещений с контролем зон торможений по датчику
*/
void Menu::servo_c()
{		
	#if (configUSE_mcx514 == 1)
		return;
	#endif
	//==обработка перемещений установочных (из кадра)
	if(entry_exe == WORKING_EXE || entry_hand == WORKING_EXE){
		for(int i = 0 ; i < N_AXIS;i++){
			if(!sts_c[i])
				continue;
			delay_PUSK(i);			
			move_controller(i);
			v_luft.process(i);					
			G0G1(i);			
		}			
	}
	//==
	st_Drive = wait_drive_lir541();
	Led_START_STOP();
	if(entry_exe == WORKING_EXE && !st_Drive && alert == OCb_VNE_DOPUSKA)
		entry_exe = INIT;//останавливаем дальнейшую отработку кадров
	else if(entry_exe == WORKING_EXE && !st_Drive)//возвращаемся в меню пуска команды
		if_pass_frm();
	else if(entry_hand == WORKING_EXE && !st_Drive)	
		entry_hand = INIT;
	//==обработка ручного перемещения
	if(curItem->id == idHandMove){
		MAXO.process(0);		
		for(int c = 0;c < N_AXIS; c++){
			if(sts_c[c])
				v_luft.process(c);
			}
		}
	//==
	//== обработка перемещения поиска реф
	for(int c = 0 ;c <N_AXIS;c++){
		if(display.ifSearchRef(c) && sts_c[c])
			v_luft.process(c);  
	}
	//==
}

/*
Проверка направления заданного и направления датчика движения
при условии что истек таймер
timer[0][N_AXIS] - таймер для MOVE_ERROR
timer[1][N_AXIS] - таймер для DREIF_ERROR LEFT
timer[2][N_AXIS] - таймер для DREIF_ERROR RIGHT
DIRECTION_CNT cur_dir_prev[N_AXIS] - переменная для сброса таймера при переходах между NO_DIRECTION,DIRECTION_CNT_LEFT и DIRECTION_CNT_RIGHT
*/
void Menu::check_dir_pusk()
{
	#if (configUSE_mcx514 == 1)
	return;
	#endif
	static uint8_t timer[3][N_AXIS];
	static CCNT_SENSOR::DIRECTION_CNT cur_dir_prev[N_AXIS];
	for(int c = 0; c < N_AXIS;c++){
		if(sts_c[c] && dir_exe[c] && tmr_axis_pusk[c] == 0){
			CCNT_SENSOR::DIRECTION_CNT cur_dir = mCounter[c].get_dir(); // определение направления аппаратно
			//==
			//USART_DEBUG(timer[0][c]);
			if(cur_dir == CCNT_SENSOR::NO_DIRECTION){
				if(cur_dir != cur_dir_prev[c])
					timer[0][c] = 0;
				cur_dir_prev[c] = cur_dir;
				if(dir_exe[c] == 1 || dir_exe[c] == -1){
					if(timer[0][c] > 250){ //ждем 2,5 секунды переменная char макс 2,55 сек
						//некорректно работает ошибка MOVE_ERROR, датчик долго стоит при смене направления и если есть еще люфт
						//alert = MOVE_ERROR;//ошибка движения
						//move_axis_STOP();
						timer[0][c] = 0;
					}
					else if(timer[0][c] > 200){						
						timer[1][c] = 0;
						timer[2][c] = 0;
					}
					timer[0][c]++;
				}				
			}						
			else if(cur_dir == CCNT_SENSOR::DIRECTION_CNT_LEFT){
				if(cur_dir != cur_dir_prev[c]){
					timer[0][c] = 0;
					//if(cur_dir_prev[c] == CCNT_SENSOR::DIRECTION_CNT_RIGHT)
						timer[2][c] = 0;// не копим ошибку при смене направления
					}
				cur_dir_prev[c] = cur_dir;
				if(dir_exe[c] > 0){
					if(timer[1][c] > 100){ //ждем 1 сек
						alert = DREIF_ERROR;//ошибка дрейфа
						move_axis_STOP();
						timer[1][c] = 0;
					}
					timer[1][c]++;
				}			
			}
			else if(cur_dir == CCNT_SENSOR::DIRECTION_CNT_RIGHT){
				if(cur_dir != cur_dir_prev[c]){
					timer[0][c] = 0;
					//if(cur_dir_prev[c] == CCNT_SENSOR::DIRECTION_CNT_LEFT)
						timer[1][c] = 0;// не копим ошибку при смене направления
					}
				cur_dir_prev[c] = cur_dir;
				if(dir_exe[c] < 0){
					if(timer[2][c] > 100){ //ждем 1 сек
						alert = DREIF_ERROR;//ошибка дрейфа
						move_axis_STOP();
						timer[2][c] = 0;
					}
					timer[2][c]++;
				}
			}  			
		}
		else{
			timer[0][c] = 0;
			timer[1][c] = 0;
			timer[2][c] = 0;
		}
	}
}
/*
Проверка направления заданного и направления датчика движения
при условии что прошли определенный путь
*/
void Menu::check_dir_pusk_()
{
	static char t[N_AXIS]; 
	static long CNT[N_AXIS];
	
	for(int c = 0; c < N_AXIS;c++){
		if(dir_exe[c] && sts_c[c]){
			if(t[c]== 0){
				CNT[c] = conv_c(screen.channel_counter[c],c,0,1);			
				t[c] = 1;
			}		
			const long CNT_cur = conv_c(screen.channel_counter[c],c,0,1);
			unsigned char buf[4];
			buf[0] = '0';buf[1] = '.';buf[2] = '1';buf[3] = '\0';
			long dopusk = String_to_bin(buf,c);
			
			if(dir_exe[c] > 0){
				if(CNT_cur < (CNT[c] - dopusk)){
					alert = DREIF_ERROR;//ошибка дрейфа
					move_axis_STOP();
				}
			}		
			if(dir_exe[c] < 0){
				if(CNT_cur > (CNT[c] + dopusk)){
					alert = DREIF_ERROR;//ошибка дрейфа
					move_axis_STOP();
				}
			}						
		}
		else{
			t[c] = 0;
			CNT[c] = 0;
		}
	}
}

/*
функция возвращает скорость СТАРТ и записывает значение в F скорость СТАРТ,
если скорость СТАРТ меньше скорости F
*/
long Menu::get_STRT_spd(char G0,uint8_t c)
{
	long SPD = 0; //может хранится как spdbit(char) и spdlong(long)
	if(G0 == 0){//G0
		if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
			SPD = flash_prg._Allp_param[c].start_G0;
		else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
			SPD = flash_prg._Allp_param5[c].start_G0;
	}else{ //G1
		if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
			SPD = flash_prg._Allp_param[c].start_G1;
		else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
			SPD = flash_prg._Allp_param4[c].start_G1;
	}	
	_modals_cmd.F_sv = _modals_cmd.F;
	if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG){
		if(is_frame_move() && in_STRT(c)){ //меняем F только когда в зоне СТАРТ и движение из кадра
			if((_modals_cmd.F > SPD) || (G0 == 0 && _modals_cmd.F < SPD))
				_modals_cmd.F = SPD;
		}
	}

	SPD = v_luft.set(c,SPD);// переопределение скорости если включена Выборка люфта
	return SPD;		
}
/*
uint8_t c - канал
uint8_t z - текущая зона индексы 0 - 3 зоны:1 2 3 4,  индекс 4 - зона стоп
*/
long Menu::get_ZONE_spd(uint8_t c,uint8_t z)
{
	long SPD = 0; //может хранится как spdbit(char) и spdlong(long)
	if(_modals_cmd.G0 == 0){//G0
		if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
			SPD = flash_prg._Allp_param[c].spd_G0[z];
		else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
			SPD = flash_prg._Allp_param5[c].spdG0[z];
	}else{ //G1
		if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
			SPD = flash_prg._Allp_param[c].spd_G1[z];
		else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
			SPD = flash_prg._Allp_param4[c].spdG1[z];
	}

	return SPD;
}

long Menu::get_STOP_spd(uint8_t c)
{
	long SPD = 0;	
	if(display.ifSearchRef(c)){
		if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
			SPD = flash_prg._Allp_param[c].stop_PM;
		else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
			SPD = flash_prg._Allp_param6[c].stop_PM;
	}
	else if(_modals_cmd.G0 == 0){
		if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
			SPD = flash_prg._Allp_param[c].spd_G0[4];
		else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
			SPD = flash_prg._Allp_param5[c].spdG0[4];
	}
	else {
		if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
			SPD = flash_prg._Allp_param[c].spd_G1[4];
		else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
			SPD = flash_prg._Allp_param4[c].spdG1[4];
	}

	return SPD;
}

/*
функция для преобразования счетчика
long tmp - счетчик
int i - канал
char type - тип счетчика 0 (инкрементный или абсолютный в зависимости что на экране)
 1 - только абсолютный(используется для программных концевиков) программные концевики рассчитываются только с G53
 G54 55 56 57 58 не участвуют в расчете
char no_endless_circle - 0 - преобразует счетчик в бесконечный круговой
*/
long Menu::conv_c(long tmp,int i,char type,char no_endless_circle)
{
	if(flash_prg._OUTAxis[i].typeZ == CPrg::IMPULSE){
		#if (configUSE_mcx514 == 1)
		tmp = mul_LP(i,tmp);
		#endif
	}
	else
		coeff.mult(i,&tmp);	
	
	if((init.mRef_ext[i] && type == 0) || (ref_.pRef[i] && type == 1)){
		if(flash_prg.axis_cfg[i].is_coded_ref == CPrg::YESM)
			tmp	+= size_2r2[i];
		
		if(MsgKey.offset_number == 0 || type == 1)// type для программных концевиков только в зоне G53
			tmp -= display.offset_g53[i];
		else
			tmp = tmp - display.offset_g53[i] - display.offset_g5x[MsgKey.offset_number - 1][i];
	}
	else if(is_rel_coded_ref(i)){ //ОТНОСИТЕЛЬНАЯ С КМ
		tmp	+= size_2r2[i]; //смещение кодированных меток
		tmp -= display.offset_g53[i]; // смещение G53
		tmp -= R_off[i];
		}

	l_d(tmp,i,on_display);

	if(flash_prg.axis_cfg[i].format == CPrg::FORMAT::LINEAR_INCH_4)
		coeff.div(255,&tmp,1,254,1);// делим
		
	if(!no_endless_circle){
		flash_prg.endless_circle(&tmp,i);
		}
	Instr_corr(tmp,i);		 
	return tmp;
}
/*
выбор плоскости для корректировки координаты на длину инструмента
*/
void Menu::Instr_corr(long &tmp,int i)
{
	if(_modals_cmd.G17 == 17)
		set_surf(CInit::AXIS_NAME::AXIS_Z,tmp,i);	
	else if(_modals_cmd.G17 == 18)
		set_surf(CInit::AXIS_NAME::AXIS_Y,tmp,i);
	else if(_modals_cmd.G17 == 19)
		set_surf(CInit::AXIS_NAME::AXIS_X,tmp,i);		
}
/*
корректировка координаты на длину инструмента
*/
void Menu::set_surf(CInit::AXIS_NAME axis0,long &tmp,int i)
{
	if(_modals_cmd.G43 == 44 || _modals_cmd.G43 == 43){ //длина инструмента
		if(P2[i] == axis0 && _modals_cmd.T_instr){ //ось с инструментом (к относительной координате или абс?)
			long instr = Ln4_form(T_instr[_modals_cmd.T_instr - 1].l,i);
			if(_modals_cmd.G43 == 43)
				instr = -instr;
			tmp = tmp - instr; 
		}
	}
}
/*
выбор плоскости для корректировки задания на радиус инструмента
*/
void Menu::Radius_Instr_corr(long &tmp,int i)
{
	if(_modals_cmd.G17 == 17)
		set_surf_radius(CInit::AXIS_NAME::AXIS_X,CInit::AXIS_NAME::AXIS_Y,tmp,i);
	else if(_modals_cmd.G17 == 18)
		set_surf_radius(CInit::AXIS_NAME::AXIS_X,CInit::AXIS_NAME::AXIS_Z,tmp,i);
	else if(_modals_cmd.G17 == 19)
		set_surf_radius(CInit::AXIS_NAME::AXIS_Y,CInit::AXIS_NAME::AXIS_Z,tmp,i);
}
/*
корректировка задания на радиус инструмента
*/
void Menu::set_surf_radius(CInit::AXIS_NAME axis1,CInit::AXIS_NAME axis2,long &tmp,int i)
{
	if(_modals_cmd.G40 == 45 || _modals_cmd.G40 == 46){ // радиус инструмента
		if((P2[i] == axis1 || P2[i] == axis2 ) && _modals_cmd.T_instr){ //ось с инструментом (к относительной координате или абс?)
			long instr = Ln4_form(T_instr[_modals_cmd.T_instr - 1].r,i);
			//coeff.mult(i,&instr);
			if(_modals_cmd.G40 == 46) // G46 прибавляем  G45 вычитаем
				instr = -instr;
			long abs_cnt_z = get_mod_zadanie(i);//берем модуль задания
			abs_cnt_z = abs_cnt_z - instr;
			
			if(cnt_z[i] < 0)  //if(cnt_z[i] < 1)
				abs_cnt_z = -abs_cnt_z;
			
			tmp = abs_cnt_z;			
		}
	}
}
/*
работа таймера на ПУСК канала, длительность берется из параметра "Задержка на пуск"
*/
void Menu::delay_PUSK(int c)
{
	if(tmr_axis_pusk[c] > 0){
		tmr_axis_pusk[c]--;
		//USART_DEBUG(0);
	}
	if(tmr_axis_pusk[c] == 1){
		tmr_axis_pusk[c] = 0;
		PUSK_axis(c);
		outP_all();
	}
}
/*
работа таймера на ПУСК канала в режиме поиска реф, длительность берется из параметра "Задержка на пуск"
*/
void Menu::delay_PUSK_find_ref(int c)
{
	if(display.ifSearchRef(c) && sts_c[c]){ // находимся в движении и в поиске ref
		if(tmr_axis_pusk[c] > 0){
			tmr_axis_pusk[c] -= 10;  //работаем в 10 msek таймере  вычитаем 0.01 сек
			//USART_DEBUG(0);
		}
		if(tmr_axis_pusk[c] == 10){
			tmr_axis_pusk[c] = 0;
			PUSK_axis(c);
			outP_all();
		}
	}
}
/*
работа таймера для инициализации реф метки, не параметризирован ,
всегда устанавливается 1 сек
*/
void Menu::delay_INIT_REF(int c)
{			
	if(display.ifSearchRef(c) && sts_c[c]){
		if(tmr_ref_pusk[c] > 0){
			tmr_ref_pusk[c] -= 10; // вычитаем 0.01 сек
			//USART_DEBUG(0);
		}
		
		if(tmr_ref_pusk[c] == 10){
			tmr_ref_pusk[c] = 0;
			MsgKey.enable_int[c] = 1;
		}
	}
}
/*
установка выхода ПУСК канала
*/
void Menu::PUSK_axis(int c)
{
	outP(flash_prg._OUTAxis[c].Pusk,1);//выставить выход ПУСК
	if (dir_exe[c] == 1)
		outP(flash_prg._OUTAxis[c].Pusk_plus,1);//выставить выход ПУСК +
	else if(dir_exe[c] == -1)
		outP(flash_prg._OUTAxis[c].Pusk_minus,1);//выставить выход ПУСК -
}
/*
сброс выходов ПУСК канала
*/
void Menu::STOP_axis(int c)
{
	outP(flash_prg._OUTAxis[c].Pusk,0);        //сброс выход ПУСК
	if (dir_exe[c] == 1)
		outP(flash_prg._OUTAxis[c].Pusk_plus,0);//сброс выход ПУСК +
	else if(dir_exe[c] == -1)
		outP(flash_prg._OUTAxis[c].Pusk_minus,0);//сброс выход ПУСК -
		
	dir_exe[c] = 0;
	F_cur[c] = 0;
	F_li[c] = 0;
}

/*
int c - канал
bool set - 0 - сброс , 1 - установка
*/
void Menu::VIBOR_axis(int c,bool set,bool PORTOUT)
{
	if(set){
		outP(flash_prg._OUTAxis[c].vibor_axis, 1,PORTOUT); //выставить выход Выбор Оси
		chs_vibor |= plb2(c);
	}
	else{
		outP(flash_prg._OUTAxis[c].vibor_axis, 0,PORTOUT); //сбросить выход Выбор Оси
		chs_vibor &= ~plb2(c);
	}
}
/*
сброс выхода выбора оси, uint8_t chs - канал который не сбрасывать, если 0
то сбросить все каналы
*/
void Menu::rst_VIBOR_axis(uint8_t chs,bool PORTOUT)
{		
	for(int c = 0; c < N_AXIS + N_SA; c++){
		if(!(chs & chs_vibor & plb2(c)))
			VIBOR_axis(c,0,PORTOUT);
	}
}

/*
входной блок контроллера движения
*/
void Menu::move_controller(int c)
{
	const long CNT = conv_c(screen.channel_counter[c],c,0,1);
	cnt_s[c] = CNT - tmp_pc[c];
	if(cnt_p[c] != cnt_s[c]){
		cnt_z[c] -= cnt_s[c] - cnt_p[c];
		cnt_p[c] = cnt_s[c];
	}
}
/*
выходной блок контроллера движения
int i - номер канала
*/
void Menu::G0G1(int i)
{		
	if(v_luft.ON[i] == 1)// находимся в выборке люфта
		return;
	bool OUT = false;
	cnt_wait_afterG0G1_f(i);

	long tmp = cnt_z[i] * dir_exe[i];	

	for(int z = 0 ; z < ZONES;z++){
		if (flash_prg._Allp_param3[i].zone[z] >= tmp && !zn[i][z] && sts_c[i]) {
			zn[i][z] = 1;
			if(flash_prg._OUTAxis[i].typeZ == CPrg::DISCRET)
				outABCD(get_ZONE_spd(i,z), &flash_prg._OUTAxis[i].Out_A,i);
			else if(flash_prg._OUTAxis[i].typeZ == CPrg::ANALOG)
				outAnalog(Spd_to_U(get_ZONE_spd(i,z),i,_modals_cmd.G0,1),flash_prg._OUTAxis2[i].AnalOut,flash_prg._OUTAxis2[i].Polar,dir_exe[i]);
			if(z == 4){ //зона стоп
				//outP(flash_prg._OUTAxis[i].vibor_axis, 0); //сброс выход Выбор Оси
				STOP_axis(i);				
				if(flash_prg._Allp_param2[i].v_Pozition < 50)
					flash_prg._Allp_param2[i].v_Pozition = 50;
				cnt_wait_afterG0G1[i] = flash_prg._Allp_param2[i].v_Pozition;				
				}
			OUT = true;
		}
	}
	if(OUT)
		outP_all();
}
/*
работа таймера ,длительность которого задается параметром "В Позиции"
*/
void Menu::cnt_wait_afterG0G1_f(int i)
{
	if(zn[i][4] == 1 && sts_c[i] && cnt_wait_afterG0G1[i] > 0){
		//USART_DEBUG(0x77);
		if(cnt_wait_afterG0G1[i] == 1){			
			cnt_wait_afterG0G1[i] = 0;
			//==ДОВОДКА
			dovodka_f(i);
			//==
			}
		else{
			cnt_wait_afterG0G1[i]--;
		}
	}
}
/*
алгоритм доводки
*/
void Menu::dovodka_f(int i)
{
	long dopusk;
	long abs_cnt_z;
	dopusk = get_dopusk(i);
	abs_cnt_z = get_mod_zadanie(i);

	if(abs_cnt_z > dopusk){
		if(flash_prg._Allp_param2[i].dovodka == 0) // если доводка включена
		{
			if(cnt_dvd[i] == 1){
				vne_dopuska_f(i);
				cnt_dvd[i] = 0;
				return;
				}
			cnt_s[i] = 0;
			cnt_p[i] = 0;
			memset(zn[i], 0, ZONES);
			tmp_pc[i] = conv_c(screen.channel_counter[i],i,0,1);
			set_lir541dovodka(i);// ставим выходы
			if(cnt_dvd[i] == 0)
				cnt_dvd[i] = 10;//ставим счетчик на кол- во доводок
			else
				cnt_dvd[i]--;
		}
		else
			vne_dopuska_f(i);
	}
	else
		positioning_is_over(i);
}
/*
функция возвращает допуск канала,
если допуск меньше зоны стоп (zone[4]) ,то допуск равен зоне стоп
*/
long Menu::get_dopusk(int i)
{
	if(flash_prg._Allp_param2[i].dopusk < flash_prg._Allp_param3[i].zone[4])
		return flash_prg._Allp_param3[i].zone[4];
	else
		return flash_prg._Allp_param2[i].dopusk;
}
/*
функция возвращает модуль задания канала
*/
long Menu::get_mod_zadanie(int i)
{
	if(cnt_z[i] < 0) 
		return cnt_z[i] * -1;
	else
		return cnt_z[i];
}

void Menu::vne_dopuska_f(int i)
{
	if(!flash_prg._Allp_param2[i].VNE_DOPUSKA){ // если Вне допуска - СТОП
		alert = OCb_VNE_DOPUSKA;
		move_axis_STOP();
		upd_m_icons = true;
	}
	else
		positioning_is_over(i);
}
/*
Позиционирование оси закончено успешно
*/
void Menu::positioning_is_over(int i)
{
	//выбор оси теперь сбрасывается при чтении сл кадра в функции rst_VIBOR_axis() ,если в нем нет текущей оси
	//outP(flash_prg._OUTAxis[i].vibor_axis, 0,1); 
	sts_c[i] = 0;
	upd_m_icons = true;
}

/*
если задание равно нулю возвращает ноль,
возвращает 1 если задание не равно нулю
*/
char Menu::wait_drive_lir541()
{	
	char tmp = 0;
	for(int i = 0; i< N_AXIS;i++){
		if(sts_c[i])			
			tmp |=plb2(i);//return 1;
	}
	return tmp;
}
/*
Выставление Дискретной скорости через выход A B C D
char spdbit - 4ex битная команда скорости
char *pin_assigment - номера пинов
char c - канал (не используется)
bool PORTOUT - 1 - выдать в порт ,0 - не выдавать в порт
*/
void Menu::outABCD(char spdbit,char *pin_assigment,char c,bool PORTOUT /*= 0*/)
{
	for (int i = 0; i < 4; i++) {//ABCD
		if(!(pin_assigment[i] & YESNO_P_bm)){
			int32_t BIT_p = plb2l(pin_assigment[i] & DATA_P_bm);
			if (spdbit & plb2(3 - i)){//ABCD				
				if(pin_assigment[i] & P_OR_m_bm)
					p_out |= BIT_p;
				else
					m_out |= BIT_p;
				}
			else{
				if(pin_assigment[i] & P_OR_m_bm)
					p_out &= ~BIT_p;
				else
					m_out &= ~BIT_p;
				}
			}
	}
	if(PORTOUT){	
		cli();
		altera.to_send_outputs((unsigned char*)&p_out);
		sei();
		cli();
		uart_485.send_m0_m31((unsigned short*)&m_out);
		sei();			
	}
}
/*
Выставление напряжения через выход a1 или a2
long U      - напряжение для ЦАП
char a      - выход a1 или a2
char polar  - полярность выхода
int8_t dir  - направление 1 в + или -1 в -
bool PORTOUT - 1 - выдать в порт ,0 - не выдавать в порт
*/
void Menu::outAnalog(long U,char a,char polar,int8_t dir ,bool PORTOUT /*= 0*/)
{
	#if (configUSE_PLC == 0)
		return ;
	#endif
	if(a == CPrg::NO_AOUT)
		return;

	if(polar == CPrg::PLUS_pl){
		if(U < 0)
			U *= -1;
	}
	else if(polar == CPrg::MINUS_pl){
		if(U > 0)
			U *= -1; 
	}
	else if(polar == CPrg::PL_MN_pl){
		U *= dir;
	}
	else if(polar == CPrg::MN_PL_pl){
		U *= dir * -1;
	}

	if(a == CPrg::A1)
		U_out[0] = (int16_t)U;
	else if(a == CPrg::A2)
		U_out[1] = (int16_t)U;
		
	if(PORTOUT){
		cli();
		uart_485.send_U(U_out);//выставить напряжение контроллера с ЦАП
		sei();
	}
}
/*
преобразование F(мм/мин) или S(об/мин) -> U для ЦАП
Spd - скорость в мм/мин для линейной оси формат 0.0000,
скорость в об/мин для круговой оси формат 0.0000
uint8_t c - канал
char G0 - 0 - G0  1 - G1
bool zones -  1 - установка скорости зон торможения 0 - любая другая установка скорости
*/
long Menu::Spd_to_U(long Spd,uint8_t c, char G0,bool zones)
{
#if (configUSE_PLC == 0)
	return 0;
#endif
	long U_max; //максимальное напряжение
	long Spd_max;// максимальная скорость мм/мин или об/мин
	if(G0 == 0){ // G0
		U_max = flash_prg._OUTAxis2[c].MaxUG0;
		Spd_max = flash_prg._OUTAxis2[c].MaxSpdG0;
	}
	else{ // G1
		U_max = flash_prg._OUTAxis2[c].MaxUG1;
		Spd_max = flash_prg._OUTAxis2[c].MaxSpdG1;
	}	
	long F = getF_li(c);//_modals_cmd.F формат один 0.0000
		
	if(G0 == 1 && is_frame_move()){				
		if(Spd > F) // фильтруем скорость если она больше _modals_cmd.F ,то ставиться _modals_cmd.F
			Spd = F;			
	}
	long long _Spd = Spd;
	if(zones == 1 && F_cur[c]){
		if(_Spd > ((long long)F_cur[c] * _modals_cmd.Feed) / 100)// фильтр зоны
			_Spd = ((long long)F_cur[c] * _modals_cmd.Feed) / 100; //если зона больше корректора
		else
			; //если зона меньше или равна корректору
	}else{
		F_cur[c] = _Spd; //сохранение скорости для изменения величины в движении
		if(v_luft.ON[c] == 0) //корректор не действует на скорость люфта
			_Spd = (_Spd * _modals_cmd.Feed) / 100;	//умножение на процент Feed
		}	
			
	//фильтруем скорость на максимальное в параметрах
	if(_Spd > Spd_max)
		_Spd = Spd_max;				
	// (формат скорости(0.0000) * формат напряжения(0.000) / формат скорости(0.0000))
	_Spd = (_Spd * U_max) /Spd_max;  // преобразование в U
	
	if(_Spd < 0)
		_Spd = -_Spd;		
	return (long)_Spd;
}

/*
return 1 - если выполнение кадра преднабор или программно (начинаем движение в кадре или движемся в кадре )
*/
bool Menu::is_frame_move()
{
	if(is_frame_move_prog() || is_frame_move_prednabor())
		return 1;
	else
		return 0;
}
/*
return 1 - если выполнение кадра (начинаем движение в кадре или движемся в кадре ) в программе
*/
bool Menu::is_frame_move_prog()
{
	if(entry_exe == EXECUTE_EXE || entry_exe == WORKING_EXE || entry_exe == AUTO_EXECUTE_EXE)
		return 1;
	else
		return 0;
}

/*
return 1 - если выполнение кадра (начинаем движение в кадре или движемся в кадре ) в преднаборе
*/
bool Menu::is_frame_move_prednabor()
{
	if(entry_hand == EXECUTE_EXE || entry_hand == WORKING_EXE)	
		return 1;
	else
		return 0;
}
/*
return 1 - если начинаем движение или движемся в ручную
*/
bool Menu::is_move_hand()
{
	if(curItem->id == idHandMove)
		return true;
	else
		return false;
}

/*
функция для получения скорости для сравнения во время рассчета из Скорости в напряжение
*/
long Menu::getF_li(uint8_t c)
{
	if(F_li[c] && is_frame_move())
		return F_li[c];
	else
		return _modals_cmd.F;
}


/*
преобразование переменной tmp с форматом 0.0000 в переменную с форматом
текущего канала uint8_t c, только линейный
*/
long Menu::Ln4_form(long tmp,uint8_t c)
{	
	if(c == SW_AXIS0)
		return tmp;
	
	if(flash_prg.axis_cfg[c].format == CPrg::LINEAR_1)
		tmp /= 1000;
	else if(flash_prg.axis_cfg[c].format == CPrg::LINEAR_2)
		tmp /= 100;
	else if(flash_prg.axis_cfg[c].format == CPrg::LINEAR_3)
		tmp /= 10;

	return tmp;
}

/*
char pin_out - номер пина
bool set - 1- установка, 0- сброс
bool PORTOUT - 1 - выдать в порт ,0 - не выдавать в порт
*/
void Menu::outP(char pin_out, bool set,bool PORTOUT /*= 0*/)
{
	if(!(pin_out & YESNO_P_bm)){
		int32_t BIT_p = plb2l(pin_out & DATA_P_bm);
		if(set){
			if(pin_out & P_OR_m_bm)
				p_out |= BIT_p;
			else
				m_out |= BIT_p;
			}
		else{
			if(pin_out & P_OR_m_bm)
				p_out &= ~BIT_p;
			else
				m_out &= ~BIT_p;
			}
		if(PORTOUT){
			cli();
			altera.to_send_outputs((unsigned char*)&p_out);
			uart_485.send_m0_m31((unsigned short*)&m_out);
			sei();
		}
	}
}
/*
установка всех выходов разом
int32_t p_out_ - выходы пульта
int32_t m_out_ - выходные маркеры внешнего контроллера
int16_t U_out_a1 - выход1 ЦАП внешнего контроллера
int16_t U_out_a2 - выход2 ЦАП внешнего контроллера
*/
void Menu::outP_all(int32_t p_out_,int32_t m_out_,int16_t U_out_a1,int16_t U_out_a2)
{		
		cli();
		p_out = p_out_;
		m_out =	m_out_;
		U_out[0] = U_out_a1;
		U_out[1] = U_out_a2;
		altera.to_send_outputs((unsigned char*)&p_out);
		uart_485.send_m0_m31((unsigned short*)&m_out);
		uart_485.send_U(U_out);//выставить напряжение контроллера с ЦАП
		sei();
}
/*
установка всех выходов разом в порт
*/
void Menu::outP_all()
{
	cli();
	altera.to_send_outputs((unsigned char*)&p_out);//выставить выходы пульта
	uart_485.send_m0_m31((unsigned short*)&m_out);// выставить маркеры
	uart_485.send_U(U_out);//выставить напряжение контроллера с ЦАП
	sei();
}
/*
сброс выходов системы
*/
void Menu::rst_outP()
{
	mem_m80_m95 = 0;
	outP_all(0,0,0,0);
	outM5_delay();
}



void Menu::debugOUT_INPUT(int y)
{
	if(!DBG_)
		return;
	/*debug inputs*/
	for (int i = 0; i < 16;i++) {
		print_bit(init.p_in,i,0,16 * y);
	}
	
	//print_long_format(cnt_z[1],flash_prg.axis_cfg[1].format,0,0,16 * y,0,0,11);
	/*debug outputs*/
	INVERT_CHAR()
	for (int i = 0; i < 24;i++) {
		print_bit(Lir.p_out,i,16,16 * y);
	}
	NORMAL_CHAR()
}

void Menu::on_off_debugOUT_INPUT()
{
	
	//if(MsgKey.mode == TMsgKey::MODE::BEGIN || 
	//MsgKey.mode == TMsgKey::MODE::FIND_REF || 
	//MsgKey.mode == TMsgKey::MODE::HAND)
	//{
		if(curItem->id == idAbout){
			if(MsgKey.key == _key_1){
				DBG_ ^= 1;
			}
		}
		/*if(MsgKey.key == _key_1 && !DBG_OUT_IN){
			display.print_symbols_local(' ',40,0,0,16 * 10);
		}  */
	//}

}

void Menu::upd_Spd_inMove()
{
	if(!is_Analog())
		return;
	if(passedZN(0))
		return;
	for(int c = 0; c < N_AXIS + N_SA;c++){
		if(sts_c[c]){
			if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
				outAnalog(Spd_to_U(F_cur[c],c,_modals_cmd.G0),flash_prg._OUTAxis2[c].AnalOut,flash_prg._OUTAxis2[c].Polar,dir_exe[c],1);
			else if(flash_prg._OUTAxis[c].typeZ == CPrg::IMPULSE){
				#if (configUSE_mcx514 == 1)
				SetSpeed(plb2(c), Spd_to_Hz(&_modals_cmd.F,c));	// Drive speed ps F
				#endif
				}
			}			
		}	
}
/*
инициализация процента подач
*/
void Menu::init_Feed()
{	
	if(flash_prg._Allp_param7.MAX_percent_feed > 100)
		Lir._modals_cmd.Feed = 100;
	else
		Lir._modals_cmd.Feed = flash_prg._Allp_param7.MAX_percent_feed;
}

/*
функция для показа сообщения о наезде на программный ограничитель вместо координат
*/
bool Menu::print_prog_sw_error(int n_str,unsigned char v,long data)
{
	return 0;// отключили
	//if (clear_rec==1)
	//display.clear_rectangle(40,n_str * 7,232,5,0x00,0);
	if(ifStrch(n_str)){
		put_info_sw_error(data,v,n_str * 56);
		return 1;
	}
	return 0;
}

void Menu::put_info_sw_error(long &data,unsigned char v,int y)
{
	const long CNT = conv_c(mCounter[v].abs,v,1);
	reset_circle_hw_couters(v);
	display.print_flash_local(sProg_limit,0,6,y);

	if(plb2(v) & ch_sw_prog.plus)
		display.put_char_local_independent('+',0,30,y);
	else if(plb2(v) & ch_sw_prog.minus)
		display.put_char_local_independent('-',0,30,y);

	display.print_flash_local(PSTR("абс."),0,6,y + 16);
	display.print_flash_local(sSchetchik,0,10,y + 16);
	display.print_symbols_local(' ',1,0,18,y + 16);
	Lir.print_long_format(CNT,flash_prg.axis_cfg[v].format,0,29 + 3,y + 16,1,0,17);
}
/*
есть ли ошибка программного концевика на оси
return 1 - да
return 0 - нет
*/
bool Menu::ifStrch(int n_str)
{
	int ch = get_ch_from_str(n_str);

	if(plb2(ch) & ch_sw_prog.plus)
		return 1;
	else if(plb2(ch) & ch_sw_prog.minus)
		return 1;
	else
		return 0;
}
/*
определяет установлена ли для канала параметр скорости Ограничитель
int8_t c - канал
*/
bool Menu::if_LimitPM(int8_t c)
{
	if(c >= N_AXIS)
		return 0;
	if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET){
		if(flash_prg._Allp_param[c].limit_PM & 0xF)
			return 1;
		else
			return 0;
	}
	else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG){
		if(flash_prg._Allp_param6[c].limit_PM > 0 )
			return 1;
		else
			return 0;
	}
	return 0;
}

void Menu::input_handler_IRQ()
{
	char sig = NO_P;
		SHPINDEL_Stop();
		STANOK_GOTOV_input();
		START_PROG();
	//== ось на ТОРМОЗЕ
		char brk_t = 0; // каналы для остановки
		for(int c = 0; c < N_AXIS + N_SA;c++){
			if(P2[c] == no_axis)
				continue;
			sig = inP(flash_prg._INAxis[c].axis_on_brake,1,1);
			if(sig == 1){
				TORMOZ[c] = 1;
				brk_t |= plb2(c);
				if(INmove())
					alert = OCb_NA_TORMOZE;
				}
			else if(sig == 0)
				TORMOZ[c] = 0;
			else if(sig == NO_P)
				TORMOZ[c] = 0;
		}
		if(brk_t)
			move_axis_STOP();// остановка всех каналов или нет?
	//==
	//==ИНСТРУМЕНТ
	sig = inP(flash_prg._INGen.instrum_zajat,1,1);
	if(sig == 1)
		INSTRUMENT_ZAJAT = 1;
	else if(sig == 0){
		INSTRUMENT_ZAJAT = 0;
		if(_modals_cmd.M3 == spCW || _modals_cmd.M3 == spCCW){
			alert = INSTRUMENT_NE_ZAJAT;
			outM5_delay();
		}		
		if(ifG1()){
			if(INmove())			
				move_axis_STOP();		
			}
		}
	else if (sig == NO_P)
		INSTRUMENT_ZAJAT = 1;
	//==ШПИНДЕЛЬ
	sig = inP(flash_prg._INGen.shpindel_vrashaetsia,1,1);
	if(sig == 1)
		SHPINDEL = 1;
	else if(sig == 0){
		SHPINDEL = 0;	
		if(cnt_wait_shpindel_off)// если задержка на отключение (команда M5)
			return;							
		Lir.outM5_delay();				
		if(ifG1()){
			if(INmove()){
				move_axis_STOP();				
				alert = SHPINDEL_NE_VRASHAETSIA;
				}
			}
	}else if (sig == NO_P)
		SHPINDEL = 1;	
	//==ВНЕШНЕЕ УПРАВЛЕНИЕ
	sig = Lir.inP(flash_prg._INGen.vneshnee_ypravlenie,1,1);
	if(sig == 1)
		extPULT.on = 1;
	else if(sig == 0)
		extPULT.on = 0;
	else if(sig == NO_P)
		extPULT.on = 0;
	//==
	//==Стоп Подача
	sig = Lir.inP(flash_prg._INGen.stop_podacha,1,1);
	if(sig == 1 || sig == HV_P){		
		STOP_PODACHA = 1;
		move_axis_STOP();
		l_k_s.OFF_LED(_led_RIGHT);
		l_k_s.OFF_LED(_led_LEFT);
		//USART_DEBUG(sig);
		}
	else if(sig == 0){
		if(STOP_PODACHA)
		STOP_PODACHA = 0;
		//USART_DEBUG(sig);
	}
	else if(sig == NO_P){
		if(STOP_PODACHA)
		STOP_PODACHA = 0;
		//USART_DEBUG(sig);
	}
	//==
	//== Ограничитель+
	for(int c = 0 ; c <N_AXIS + N_SA;c++){
		if(P2[c] == no_axis){
			ch_sw.plus &= ~plb2(c);
			continue;
			}
		sig = Lir.inP(flash_prg._INAxis[c].Ogranichitel_plus,1,1);		
		if(sig == 1){
			ch_sw.plus |= plb2(c);
			bool moved = sts_c[c];//запись в переменную для поиска реф
			move_axis_STOP(sw_s(ch_sw.plus));
			OFF_D_LEFT_RIGHT();			
			if(limit_sw[c] == 0 && display.ifSearchRef(c) && if_LimitPM(c) && moved){
				MsgKey.set_out_ref_mode = 0;
				lir541comRef(1,1);
				}
			}
		else if(sig == 0 || sig == NO_P){
			ch_sw.plus &= ~plb2(c);
			}
	}
	//== Ограничитель-
	for(int c = 0 ; c <N_AXIS + N_SA;c++){		
		if(P2[c] == no_axis){
			ch_sw.minus &= ~plb2(c);
			continue;
			}
		sig = Lir.inP(flash_prg._INAxis[c].Ogranichitel_minus,1,1);
		if(sig == 1){
			ch_sw.minus |= plb2(c);
			bool moved = sts_c[c];//запись в переменную для поиска реф
			move_axis_STOP(sw_s(ch_sw.minus));
			OFF_D_LEFT_RIGHT();
			if(limit_sw[c] == 0 && display.ifSearchRef(c) && if_LimitPM(c) && moved){
				MsgKey.set_out_ref_mode = 0;
				lir541comRef(2,1);
				}
			}
		else if(sig == 0 || sig == NO_P){
			ch_sw.minus &= ~plb2(c);
			}		
	}
	//==
	//пример кода срабатывания по фронту (тактовая кнопка)
	/*res = Lir.inP(flash_prg._INGen.vneshnee_ypravlenie,1);
	if(res == 1)
		extPULT ^= 1;*/

	//пример кода срабатывания по уровню (кнопка с залипанием)
	/*res = Lir.inP(flash_prg._INGen.vneshnee_ypravlenie,1,1);
	if(res == 1)
		extPULT = 1;
	else if(res == 0)
		extPULT = 0;*/
}

void Menu::input_handler()
{			
	if(INmoveContext() && extPULT.on)//==ВНЕШНЕЕ УПРАВЛЕНИЕ
	{ 
		if(Lir.status2(PSTR("ПУЛЬТ"))){
			MAXO = 0;
			extPULT.G0 = 1;
			outP_G0(extPULT.G0,1);
			show_Instr(0,_resz_prg_v.SZ);			
			}
		//для нормальной работы этих входов , сигнал ВНЕШНЕЕ УПРАВЛЕНИЕ должен полностью покрывать их по времени
		if(!Lir.alert){			
			VIBOR_AXIS_input();
			OTMENA_VIBOR_AXIS_input();
			PUSK_AXIS_input();
			TOLCHOK_AXIS_input();
			G0_TOLCHOK();
			SHPINDEL_input();
		}
	}else{
		if(Lir.status2(PSTR(""))){
			if(!extPULT.on){
				outP_G0(_modals_cmd.G0,1);
				show_Instr(0,_resz_prg_v.SZ);
				}
			//сбрасываем все ,только если выбрано больше одной оси
			if(!(invers_btn == 0 || invers_btn == 1 || invers_btn == 2 || invers_btn == 4 || invers_btn == 8 || invers_btn == 16)){
					update_pic_axis_mh(0,1);	
					axis_drive_stop_lir(0);
				}		
			}
		}		
	//==	
	if(INmoveContext() || MsgKey.mode == TMsgKey::MODE::HAND || MsgKey.mode == TMsgKey::MODE::BEGIN || MsgKey.mode == TMsgKey::MODE::FIND_REF)
	{		
		//==Ограничитель +/-
		OGRANICHITEL();								
	}

	outPozitionirovanie();
	outRychnoyAutorejim();
	LUBRICATE_out();
}
/*
Смазка ,только для осей с датчиком движения
*/
void Menu::LUBRICATE_out()
{
	static long CNT[N_AXIS];
	static long CNT_target[N_AXIS];
	static long CNT_passed[N_AXIS];
	static long CNT_pass_prev[N_AXIS];
	
	for(int c = 0 ; c < N_AXIS;c++){
		if(!(flash_prg._OUTAxis[c].Smazka & YESNO_P_bm)){
			//включение смазки начало движения
			if(sts_c[c] == 1 && lubric_tmr[c] == -1){ 
				outP(flash_prg._OUTAxis[c].Smazka,1,1);
				if(flash_prg._OUTAxis[c].time_lubr > 0 && flash_prg._OUTAxis[c].time_lubr < 11)
					lubric_tmr[c] = flash_prg._OUTAxis[c].time_lubr * 100;
				else
					lubric_tmr[c] = 100; //100 * 10 мсек таймер = 1 секунда  (по умолчанию)
				continue;
			}
			//отключение смазки во время импульса смазки и отключение при включении питания
			if(sts_c[c] == 0 && lubric_tmr[c] >= 0){
				outP(flash_prg._OUTAxis[c].Smazka,0,1);
				lubric_tmr[c] = -1;
				continue;
			}
			//отключение смазки во время счета расстояния -2
			if(sts_c[c] == 0 && lubric_tmr[c] == -2 && !is_frame_move()){
				outP(flash_prg._OUTAxis[c].Smazka,0,1);
				lubric_tmr[c] = -1;
				continue;
			}
			//отключение смазки в движении	и начало отсчета расстояния
			if(sts_c[c] == 1 && lubric_tmr[c] == 0){  			
				outP(flash_prg._OUTAxis[c].Smazka,0,1);
				lubric_tmr[c] = -2;
				CNT[c] = conv_c(screen.channel_counter[c],c,0,1);//сохраняем координату для вычисления пройденного пути
				if(flash_prg._OUTAxis[c].dist_lubr > 0)
					CNT_target[c] = cForm(flash_prg._OUTAxis[c].dist_lubr,c);
				else
					CNT_target[c] = cForm(10000,c);//(параметр) 10000 10 мм при K = 1и формате 0.000
				CNT_passed[c] = 0;
				CNT_pass_prev[c] = 0;
				continue;
			}
			// счетчик расстояния
			if(sts_c[c] == 1 && lubric_tmr[c] == -2){ 
				long passing = conv_c(screen.channel_counter[c],c,0,1);
				CNT_passed[c] = passing - CNT[c];
				
				if(CNT_pass_prev[c] != CNT_passed[c]){
					if(CNT_passed[c] >= CNT_pass_prev[c])
						CNT_target[c] -= CNT_passed[c] - CNT_pass_prev[c];
					else
						CNT_target[c] -= CNT_pass_prev[c] - CNT_passed[c];
					CNT_pass_prev[c] = CNT_passed[c];
				}

				if(CNT_target[c] <= 0){
					lubric_tmr[c] = -1;
				}  
			}			
		}		
	}
}

void Menu::OTMENA_VIBOR_AXIS_input()
{
	char res = 0;
	for(int c  = 0 ; c < N_AXIS + N_SA;c++){
		if(P2[c] == no_axis)
		continue;
		res = Lir.inP(flash_prg._INAxis[c].otmena_vibor_axis,1,1);
		if(res == 1){
			move_axis_STOP(plb2(c));
			if(!ch_to_move)
				stop_mh();
			char s = get_str_from_ch(c);
			#if (configUSE_Shadow_axis == 1)						
			sym_invers(s,0,0);	
			#else
			if(s != N_STR)
				sym_invers(s,0,0);	
			#endif												
		}
	}
}

void Menu::VIBOR_AXIS_input()
{
	char res;
	bool reset_vibor = 0;
	//=============================Выбор оси==============================================
	for(int c  = 0 ; c < N_AXIS + N_SA;c++){
		if(P2[c] == no_axis)
			continue;
		res = Lir.inP(flash_prg._INAxis[c].vibor_axis,1,1);
		if(c == SW_AXIS0 ){
			if(res == 1){
				if(invers_btn == AXIS5_bit)// если уже выбрана ось
					continue;
				else{
					reset_vibor = 1;
					invers_btn = 0;
				}
			}
			//else if(res == 0)
			//	USART_DEBUG(0);
		}
		//== фильтр спада
		if(res == 0){
			int sum = 0;
			for(int c_ = 0 ; c_ < N_AXIS + N_SA;c_++){		
				if(P2[c_] == no_axis)
					continue;		
				if(Lir.inP(flash_prg._INAxis[c_].vibor_axis) == 1)
					sum += 1;
			}
			if(!sum) // блокирует отключение оси
				res = NO_P;
		}

		//== задержка на включение оси
		if(res == 1 && invers_btn ) {  //если есть выбранная ось , то следущий выбор с задержкой
			cli();
			//init.set_B6();
			cnt_galet = 30;// 300 ms задержка ,при 10 мсек 30 при 1 мсек 300
			axis_d = c;
			sei();
			res = NO_P;
		}
		else if (cnt_galet && res == 0 && c == axis_d) {
			//res = NO_P;
			cnt_galet = 0;
		}
		else if (cnt_galet == 1 && res == LV_P) {
			res = 1;
			c = axis_d;
			cnt_galet = 0;
			//== логика для галетного переключателя
			int sum = 0;
			for(int c_ = 0 ; c_ < N_AXIS + N_SA;c_++){
				if(P2[c_] == no_axis)
					continue;
				if(Lir.inP(flash_prg._INAxis[c_].vibor_axis) == 1)
					sum += 1;
				}
			if(sum < 2 )
				reset_vibor = 1;
			else
				reset_vibor = 0;
		}	
			
		if(res == 0 || res == 1){
			set_HAND_MOVE();
			sym_invers(get_str_from_ch(c),res,reset_vibor);
			print_status_HAND_MOVE();			
			//USART_DEBUG(ch_to_move);			
		}
	}
	//================================================================
}

void Menu::PUSK_AXIS_input()
{
	char res;
	//=============================Пуск оси==============================================
	for(int c  = 0 ; c < N_AXIS + N_SA;c++){	
		if(P2[c] == no_axis)
			continue;
		if(vibor_axis_ON(c)){ 
			if(!(ch_to_move & plb2(c)))
				continue;
		}

		res = Lir.inP(flash_prg._INAxis[c].Pusk_plus,1,1);
		if(res == 1){
			dir_mh = DIR_MH_RIGHT;
			//USART_DEBUG(dir_mh)
			}
		else
			dir_mh = NO_DIR_MH;

		if(dir_mh == NO_DIR_MH){
			res = Lir.inP(flash_prg._INAxis[c].Pusk_minus,1,1);
			if(res == 1){
				dir_mh = DIR_MH_LEFT;
				//USART_DEBUG(dir_mh)
				}
			else
				dir_mh = NO_DIR_MH;
			}
		if(res == 1){
			if(dir_mh && !btn_cont_mh && !btn_hold_mh){
				//включение диодов
				if(dir_mh == DIR_MH_RIGHT){
					l_k_s.ON_LED(_led_RIGHT);
					l_k_s.OFF_LED(_led_LEFT);
					}else{
						l_k_s.OFF_LED(_led_RIGHT);
						l_k_s.ON_LED(_led_LEFT);
					}
				//if(ch_to_move != AXIS5_bit) // на подаче с внешнего пульта всегда включается G1,(кроме слепой оси )
					extPULT.G0 = 1;

				if(vibor_axis_OFF(c)){ //если нет входа выбора оси
					if(!(ch_to_move & plb2(c))){ //ось не выбрана
						set_HAND_MOVE();
						sym_invers(get_str_from_ch(c),1,1);
						print_status_HAND_MOVE();
					}
				}

				if(Continius_drive_move(ch_to_move,extPULT.G0,0))
					btn_cont_mh = 1;// БЕЗРАЗМЕРНЫЕ ПЕРЕМЕЩЕНИЯ
				show_Instr(0,_resz_prg_v.SZ);
			}
		}
	}
}

void Menu::TOLCHOK_AXIS_input()
{
	char res;
//=============================Толчок оси==============================================
	for(int c  = 0 ; c < N_AXIS + N_SA;c++){	
		if(P2[c] == no_axis)
			continue;	
		if(vibor_axis_ON(c)){
			if(!(ch_to_move & plb2(c)))
				continue;
		}
		res = Lir.inP(flash_prg._INAxis[c].Tolchok_plus,1,1);
		if(res == 1)
			dir_mh = DIR_MH_RIGHT;
		else if (res == 0){
			Lir.tolchok_release();
			l_k_s.OFF_LED(_led_RIGHT);
			}
		else
			dir_mh = NO_DIR_MH;

		if(dir_mh == NO_DIR_MH){
			res = Lir.inP(flash_prg._INAxis[c].Tolchok_minus,1,1);
			if(res == 1)
				dir_mh = DIR_MH_LEFT;
			else if (res == 0){
				Lir.tolchok_release();
				l_k_s.OFF_LED(_led_LEFT);
				}
			else
				dir_mh = NO_DIR_MH;
			}
		if(res == 1){
			if(dir_mh && !btn_hold_mh && !btn_cont_mh){
				//включение диодов
				if(dir_mh == DIR_MH_RIGHT){
					l_k_s.ON_LED(_led_RIGHT);
					l_k_s.OFF_LED(_led_LEFT);
				}else{
					l_k_s.OFF_LED(_led_RIGHT);
					l_k_s.ON_LED(_led_LEFT);
				}
				if(vibor_axis_OFF(c)){ //если нет входа выбора оси
					if(!(ch_to_move & plb2(c))){ //ось не выбрана
						set_HAND_MOVE();
						sym_invers(get_str_from_ch(c),1,1);
						print_status_HAND_MOVE();
					}
				}
				if(Continius_drive_move(ch_to_move,extPULT.G0,1))
					btn_hold_mh = 1; //ТОЛЧОК
			}
		}
	}
}

void Menu::OGRANICHITEL()
{
	if(Lir.alert == Menu::STANOK_NE_GOTOV)
		return;
	if(init.time_metre>=100)// для отображения выключение через
		return;
	char sw_str[21];//+X+Y+Z+W+U-X-Y-Z-W-U
	memset(sw_str,' ',21);	
	int off = 0;
	if(ch_sw.plus || ch_sw.minus || ch_sw_prog.plus || ch_sw_prog.minus){		
		if(ch_sw.plus || ch_sw.minus){ //приоретет для аппаратных концевиков
			Lir.status(PSTR("ОГРАНИЧИТЕЛЬ"));
			for(int c = 0; c < N_AXIS + N_SA;c++){
				if(ch_sw.plus & plb2(c)){
					sw_str[off] = '+';
					sw_str[off + 1] = get_ASCII_from_channel(c);
					off+=2;
				}
			}
			for(int c = 0; c < N_AXIS + N_SA;c++){
				if(ch_sw.minus & plb2(c)){
					sw_str[off] = '-';
					sw_str[off + 1] = get_ASCII_from_channel(c);
					off+=2;
				}
			}
		}
		else if(ch_sw_prog.plus || ch_sw_prog.minus){
			Lir.status(PSTR("ограничение")); // Программные концевики без слепой оси
			for(int c = 0; c < N_AXIS;c++){
				if(ch_sw_prog.plus & plb2(c)){
					sw_str[off] = '+';
					sw_str[off + 1] = get_ASCII_from_channel(c);
					off+=2;
				}
			}
			for(int c = 0; c < N_AXIS;c++){
				if(ch_sw_prog.minus & plb2(c)){
					sw_str[off] = '-';
					sw_str[off + 1] = get_ASCII_from_channel(c);
					off+=2;
				}
			}
		}		
		sw_str[13 - 1] = '\0';//ограничение по выводу c соседней надписью "ПУЛЬТ"
		display.print_ram_local(sw_str,0,13,16*14);
		//==блок для активации режима обновления статуса
		if(Lir.curItem->id == idExecmd)
			upd_sw_status = true;
		else if(Lir.curItem->id == idHand && MsgKey.tracer_real == TMsgKey::CMD_CNTRL)
			upd_sw_status = true;
		//==
	}
	else{
		if(Lir.curItem->id == idHandMove)
			print_status_HAND_MOVE();
		else if(Lir.curItem->id == idExecmd){
			if(upd_sw_status){
				if(INmove())
					status(PSTR("В работе"));//доделать
				else
					status(PSTR("Нажмите СТАРТ"));
				upd_sw_status = false;
			}
		}
		else if(Lir.curItem->id == idHand && MsgKey.tracer_real == TMsgKey::CMD_CNTRL){
			if(upd_sw_status){
				if(INmove())
					status(PSTR("В работе"));//доделать
				else{
					//status(PSTR("Преднабор"));
					if(display.strnlen_lir(frame_cmd[MAX_FRAME - 1] + 4))// проверить когда пустая команда
						status(PSTR("Нажмите СТАРТ"));
					else
						status(PSTR("Преднабор"));
					}
				upd_sw_status = false;
			}
		}
		else{
			if(MsgKey.tracer_next != TMsgKey::CMD_CNTRL && 			
				!display.strnlen_lir(frame_cmd[MAX_FRAME - 1] + 4) && 
				Lir.curItem->id != idExecmd && 
				!display.ifSearchRefFINDREF())
				Lir.status(PSTR(""));
			else{				
				if(MsgKey.mode == TMsgKey::MODE::FIND_REF){
					if(display.ifSearchRefFINDREF())
						Lir.status(PSTR("Нажмите СТАРТ"));
					else
						Lir.status(PSTR(""));
					}
				}
			}
		}
}

void Menu::G0_TOLCHOK()
{
	if(btn_cont_mh)
		return;
	if(SEToutRA)
		return;
		
	char res;
	res = Lir.inP(flash_prg._INGen.yskorenniy_xod_G0,1,1);
	if(res == 1){
		extPULT.G0 = 0;		
		//выставить выходы G0 start на ходу только при толчке
		if(btn_hold_mh){
			for(int c = 0 ; c < N_AXIS + N_SA;c++){
				if (ch_to_move & plb2(c)){
					if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
						outABCD(flash_prg._Allp_param[c].start_G0, &flash_prg._OUTAxis[c].Out_A,c);
					else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
						outAnalog(Spd_to_U(flash_prg._Allp_param5[c].start_G0,c,extPULT.G0),flash_prg._OUTAxis2[c].AnalOut,flash_prg._OUTAxis2[c].Polar,dir_exe[c]);
				}
			}			
		}
		outP_G0(extPULT.G0);		
		outP_all();
		show_Instr(0,_resz_prg_v.SZ);
	}
	else if(res == 0){
		extPULT.G0 = 1;				
		//выставить выходы G1 start на ходу только при толчке
		if(btn_hold_mh){
			for(int c = 0 ; c < N_AXIS + N_SA;c++){
				if (ch_to_move & plb2(c)){
					if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
						Lir.outABCD(flash_prg._Allp_param[c].start_G1, &flash_prg._OUTAxis[c].Out_A,c);
					else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
						outAnalog(Spd_to_U(flash_prg._Allp_param4[c].start_G1,c,extPULT.G0),flash_prg._OUTAxis2[c].AnalOut,flash_prg._OUTAxis2[c].Polar,dir_exe[c]);
				}
			}			
		}
		outP_G0(extPULT.G0);
		outP_all();
		show_Instr(0,_resz_prg_v.SZ);
	}
}

void Menu::SHPINDEL_input()
{	
	//M3 M4 M5 на вход
	char res = NO_P;
	//res = inP(flash_prg._INGen.stop_M5,1,1);
	//if(res == 1)
	//	outM5_delay();		
			
	res = inP(flash_prg._INGen.po_chasovoy_M3,1,1);
	if(res == 1)
		M3_mem = 1;
	else {
		res = inP(flash_prg._INGen.protiv_chasovoy_M4,1,1);
		if(res == 1)
			M3_mem = 0;
	}
	outM3_delay(res,M3_mem);		
	

	//===Толчок ШПИНДЕЛЯ+/-
	res = inP(flash_prg._INGen.tolchok_plus,1,1);
	if(res == 1){
		if(outM3_delay(res,1) == 1);
			outP(flash_prg._OUTGen.Tolchok,1,1);
	}
	else if(res == 0){
		outM5_delay();
		outP(flash_prg._OUTGen.Tolchok,0,1);	
	}
	if(res == 0 || res == LV_P){
		res = inP(flash_prg._INGen.tolchok_minus,1,1);
		if(res == 1){
			if(outM3_delay(res,0) == 1);
				outP(flash_prg._OUTGen.Tolchok,1,1);
		}
		else if(res == 0){
			outM5_delay();
			outP(flash_prg._OUTGen.Tolchok,0,1);
		}
	}
	//===
}

void Menu::SHPINDEL_Stop()
{
	char res = NO_P;
	res = inP(flash_prg._INGen.stop_M5,1,1);
	if(res == 1 || res == HV_P){
		STOP_SHPINDEL = 1;
		outM5_delay();
		//USART_DEBUG(STOP_SHPINDEL);
	}
	else if(res == 0){
		if(STOP_SHPINDEL){
			STOP_SHPINDEL = 0;
			//USART_DEBUG(STOP_SHPINDEL);
		}
	}
	else if(res == NO_P){
		if(STOP_SHPINDEL){
			STOP_SHPINDEL = 0;
			//USART_DEBUG(STOP_SHPINDEL);
		}
	}
}

void Menu::outPozitionirovanie()
{
	static char i;

		if(st_Drive && i){
			outP(flash_prg._OUTGen.pozitionirovanie,1,1);
			i = 0;
		}
		else if (st_Drive == 0 && i == 0){
			outP(flash_prg._OUTGen.pozitionirovanie,0,1);
			i = 1;
			}
}

void Menu::outRychnoyAutorejim()
{
	if((curItem->id != idExecmd && curItem->id != idCmd && curItem->id != idHandMove && MsgKey.mode != TMsgKey::MODE::HAND && SEToutRA == 0 )|| 
	   (SEToutRA == 0 && MsgKey.mode == TMsgKey::MODE::HAND && MsgKey.tracer_next == TMsgKey::CMD_CNTRL)|| 
	   (SEToutRA == 0 && curItem->id == idCmd))
	   {
		outP(flash_prg._OUTGen.rychnoy_rejim,0);
		outP(flash_prg._OUTGen.auto_rejim,0);
		outP_all();
		SEToutRA = 1;
		//USART_DEBUG(2);
	   }
	else if(SEToutRA == 1 && MsgKey.mode == TMsgKey::MODE::HAND &&  MsgKey.tracer_next != TMsgKey::CMD_CNTRL){
		outP(flash_prg._OUTGen.rychnoy_rejim,1);
		outP(flash_prg._OUTGen.auto_rejim,0);
		outP_all();
		SEToutRA = 0;
		//USART_DEBUG(1);
	}
	else if(curItem->id == idExecmd && SEToutRA == 0){
		outP(flash_prg._OUTGen.rychnoy_rejim,0);
		outP(flash_prg._OUTGen.auto_rejim,1);
		outP_all();
		SEToutRA = 1;
		//USART_DEBUG(0);
	}	
}

void Menu::STANOK_GOTOV_input()
{
	char sig;

	sig = inP(flash_prg._INGen.gotov_stanok);// если уровень, обнуление переменной не требуется
	if(sig == 1){
		if(!STANOK_GOTOV){
			Lir.alert = Menu::CLEAR_ALERT;
			STANOK_GOTOV = 1;
		}			
	}
	else if(sig == 0){
		STANOK_GOTOV = 0;
//		if(INmove()){
			alert = STANOK_NE_GOTOV;
			move_axis_STOP();	
			outM5_delay();
		//	}
		}
	else if (sig == NO_P){
		if(!STANOK_GOTOV){
			Lir.alert = Menu::CLEAR_ALERT;
			STANOK_GOTOV = 1;
		}
	}
}

void Menu::START_PROG()
{
	char sig;
	sig = inP(flash_prg._INGen.start_prog,1,1);
	if(sig == 1){
		if(entry_exe == IDLE)
			menu_start_exe();
	}
}

void Menu::toggleGOTOVNOSTbSPPU()
{
	if(block_SPPU_OUT)
		return;
	
	static char toggle;
	
	if(toggle >= 10){
		Lir.outP(flash_prg._OUTGen.gotovnostbUCIPU,1,1);
		if(toggle >= 20)
			toggle = 0;
	}
	else
		Lir.outP(flash_prg._OUTGen.gotovnostbUCIPU,0,1);

	toggle++;
}

void Menu::outP_G0(char G0,bool PORTOUT /*= 0*/)
{
	if(G0 == 0){
		outP(flash_prg._OUTGen.yskorenniy_xod_G0,1);
		outP(flash_prg._OUTGen.podacha_G1,0);
	}
	else if(G0 == 1){
		outP(flash_prg._OUTGen.yskorenniy_xod_G0,0);
		outP(flash_prg._OUTGen.podacha_G1,1);
	}
	if(PORTOUT)
		outP_all();
}
/*
bool M3 - 1  по часовой (CW)M3  0- против часовой (CCW)M4
bool STOP - остановка
*/
bool Menu::outM3(bool M3,bool STOP /*= 0*/,bool PORTOUT /*= 0*/)
{
	if(STOP){
		outP(flash_prg._OUTGen.po_chasovoy_M3,0);
		outP(flash_prg._OUTGen.protiv_chasovoy_M4,0);
		outP(flash_prg._OUTGen.stop_M5,1);
		_modals_cmd.M3 = spSTOP;
		//здесь запускаем таймер на выключение шпинделя
		//если SHPINDEL не false по истечению таймера то АВАРИЯ
		cnt_wait_shpindel_off = 500; // 5 секунд при 1 мсек 5000  при 10 мсек 500
		lock_error_SNO = false; //снимаем блокировку ошибки
		//==переключаем контекст,если контекст программный
		if(entry_exe == Menu::DELAY_M5M3M4 || entry_exe == Menu::EXECUTE_EXE || entry_exe == Menu::AUTO_EXECUTE_EXE)
			entry_exe = Menu::DELAY_WAIT_SHPINDEL;
		else if(entry_hand == Menu::DELAY_M5M3M4 || entry_hand == Menu::EXECUTE_EXE)
			entry_hand = Menu::DELAY_WAIT_SHPINDEL;
		//==
	}
	else
		{	
			if(STOP_SHPINDEL)	
				return 0;
			if(!STANOK_GOTOV)
				return 0;
			if(!INSTRUMENT_ZAJAT)
				return 0;				
			if(M3){ // по часовой
				outP(flash_prg._OUTGen.po_chasovoy_M3,1);
				outP(flash_prg._OUTGen.protiv_chasovoy_M4,0);
				outP(flash_prg._OUTGen.stop_M5,0);
				_modals_cmd.M3 = spCW;
			}
			else{ // против часовой
				outP(flash_prg._OUTGen.po_chasovoy_M3,0);
				outP(flash_prg._OUTGen.protiv_chasovoy_M4,1);
				outP(flash_prg._OUTGen.stop_M5,0);
				_modals_cmd.M3 = spCCW;
			}
		//здесь запускаем таймер на включение шпинделя
		//если SHPINDEL не true по истечению таймера то АВАРИЯ
		cnt_wait_shpindel = 500; // 5 секунд при 10 мсек 500 при 1 мсек 5000
		//==переключаем контекст,если контекст программный
		if(entry_exe == Menu::DELAY_M5M3M4 || entry_exe == Menu::EXECUTE_EXE || entry_exe == Menu::AUTO_EXECUTE_EXE)
			entry_exe = Menu::DELAY_WAIT_SHPINDEL;
		else if(entry_hand == Menu::DELAY_M5M3M4 || entry_hand == Menu::EXECUTE_EXE)
			entry_hand = Menu::DELAY_WAIT_SHPINDEL;
		//==
		}
	if(PORTOUT)
		outP_all();
	if(curItem->id != idDiag)
		UPD_show_Instr = 1;
	return 1;
}
/*
char res - входящий сигнал true/false
in bool M3 - новая команда 1 - M3 , 0 - M4
_modals_cmd.M3 - предыдущая команда
*/
char Menu::outM3_delay(char res,bool M3)
{
	if(res != 1)
		return 0;

	if(STOP_SHPINDEL)
		return 0;
	
	if(!INSTRUMENT_ZAJAT){
		outM5_delay();
		status(PSTR("ИНСТРУМЕНТ НЕ ЗАЖАТ!"),300);
		return 2;
	}
	
	if(!STANOK_GOTOV){
		outM5_delay();
		//status(PSTR("СТАНОК НЕ ГОТОВ!"),300);
		return 2;
	}

	//==защита от повторного включения
	if(M3 == 1 && _modals_cmd.M3 == spCW)
		return 0;
	if(M3 == 0 && _modals_cmd.M3 == spCCW)
		return 0;
	//==

	if(flash_prg._OUTGen.M3_M4_cherez_M5 == 0 && (_modals_cmd.M3 == spCW || _modals_cmd.M3 == spCCW)){		
		if(flash_prg._OUTGen.pause_M5 == 0){
			status(PSTR("ВКЛЮЧИТЬ M5!"),600);
			return 2;
		}
		else if(cnt_M3M4 == 0){
			outM3(0,1,1);  //включение M5
			cli();
			cnt_M3M4 = flash_prg._OUTGen.pause_M5 * (10 - 0);
			sei();
			if(cnt_M3M4 == 0)
				cnt_M3M4 = 200;			
		}
	}
	else
		outM3(M3,0,1);
	return 1;
}

bool Menu::outM5_delay()
{	
	if(_modals_cmd.M3 == spSTOP){
		if(cnt_M3M4){ // если во время паузы выходим из  программы
			cnt_M3M4 = 0;
			set_timer_off_M5();
			}
		return 0;
	}
		
	cnt_wait_shpindel = 0;
	cnt_wait_shpindel_off = 0;
	cnt_M3M4 = 0;	
	
	outM3(0, 1, 1);
	set_timer_off_M5();
	return 1;
}

void Menu::set_timer_off_M5()
{
	if (flash_prg._OUTGen.pause_M5) { // пауза на ожидание отключения шпинделя
		cli();
		if (!cnt_M5)
			cnt_M5 = flash_prg._OUTGen.pause_M5 * (10 - 0);//1 msek период 100 -0 коррекция , 10 мсек (10 - 0)
		sei();
	}
}

/*
функция разблокирует движение по G1 если шпиндель не включен
*/
bool Menu::unlockSPH()
{
	if(flash_prg._INGen.shpindel_vrashaetsia & YESNO_P_bm)
	   return false;
	else
	   return true;
}



/*
long *ch_impulse - задание
uint8_t chs - выбранные каналы 0x01 - 0 канал, 0x02 - 1 канал, 0x04 - 2 канал, 0x08 - 3 канал
*/
bool Menu::lirCommand(long *ch_impulse, uint8_t chs,char G,long& F)
{		
	if(chs)
		rst_VIBOR_axis(chs,0);	
			
	release_G91(ch_impulse,chs);
	memset(cnt_s, 0x00, sizeof(long) * N_AXIS);
	memset(cnt_z, 0x00, sizeof(long) * N_AXIS);
	memset(cnt_p, 0x00, sizeof(long) * N_AXIS);
	memset(tmp_pc, 0x00, sizeof(long) * N_AXIS);
	memset(sts_c, 0x00, sizeof(char) * (N_AXIS + N_SA));	
	memset(cnt_dvd, 0x00, sizeof(char) * N_AXIS);
	memset(cnt_impulse_cur,0x00,sizeof(long) * N_AXIS);
	memset(cnt_wait_afterG0G1,0x00,sizeof(int) * N_AXIS);
	memset(zn, 0x00,N_AXIS * ZONES);
	memset(F_li,0x00,sizeof(long) * (N_AXIS + N_SA));
	v_luft.init();
	if(first_frame){ //обнуление для перемещений в G91
		memset(cnt_CNT_prev, 0x00, sizeof(long) * N_AXIS);
		memset(cnt_impulse_prev, 0x00, sizeof(long) * N_AXIS);		
		}

	/*if(!chs){
		if(_modals_cmd.G0 == 0 || _modals_cmd.G0 == 1)
			outP_G0(_modals_cmd.G0,1);
	} */
	outP_G0(_modals_cmd.G0,1);
	
	if(STOP_PODACHA && chs){
		status(PSTR("СТОП ПОДАЧА!"),300);
		return 0;
	}
	
	#if (configUSE_PLC == 1)
	if(Lir.lock_error_PLC){
		status(PSTR("КОНТРОЛЛЕР НЕ ОТВЕЧАЕТ!"),300);
		return 0;
	}
	#endif
	if(STANOK_GOTOV == 0 && chs){
		status(PSTR("СТАНОК НЕ ГОТОВ!"),300);
		return 0;
	}
	if(INSTRUMENT_ZAJAT == 0 && _modals_cmd.G0 == 1 && chs){
		status(PSTR("ИНСТРУМЕНТ НЕ ЗАЖАТ!"),300);
		return 0;
	}
	if((SHPINDEL == 0 || (unlockSPH() && _modals_cmd.M3 == spSTOP)) && _modals_cmd.G0 == 1 && chs){
		status(PSTR("ШПИНДЕЛЬ НЕ ВРАЩАЕТСЯ!"), 300);
		return 0;
	}
	if(if_ENC(chs))
		return 0;
	if(if_TORMOZ(chs))
		return 0;
	if(if_CIRCLE(ch_impulse,chs))
		return 0 ;
	if(if_ABS(ch_impulse,chs))
		return 0;	

	if(ch_sw.if_sw_ON())
		return 0;

	set_cnt_z_exe(ch_impulse,chs);
	check_cnt_z_exe(chs);

	if(is_single_axis(chs)){
		for (int c = 0; c < N_AXIS; c++) {
			if(sts_c[c])
				set_lirCommand(c);
		}
	}else{
		#if (configUSE_mcx514 == 1)
		if(G == 2 || G == 3)
			circle_interpolation(chs,G);
		else
			linear_interpolation(chs);
		#endif
		calc_Linear_inter(G,F);//рассчет F для линейной интерполяции (движение по двум осям) ЛИР541
	}
		
	outP_all();
	return 1;    
}
/*
рассчет задания
*/
void Menu::set_cnt_z_exe(long *ch_impulse, uint8_t chs)
{
	for (int i = 0; i < N_AXIS; i++) {
		if (chs & plb2(i)) {
			cli();
			const long CNT = conv_c(screen.channel_counter[i],i,0,1);
			sei();
			tmp_pc[i] = CNT;
			cnt_impulse_cur[i] = ch_impulse[i];
			if(_modals_cmd.G90 == 0){
				cnt_z[i] = ch_impulse[i] - CNT;//задание из кадра - текущее G90

				if(cnt_impulse_prev[i] == 0 && first_frame)
					cnt_CNT_prev[i] = CNT;
				else
					cnt_CNT_prev[i] += cnt_impulse_prev[i];
				cnt_impulse_prev[i] = ch_impulse[i] - cnt_CNT_prev[i];
			}else if(_modals_cmd.G90 == 1){
				//cnt_z[i] = ch_impulse[i];//задание из кадра G91 (приращения)
				//=========приращения G91 с вычитанием ошибки
				if(cnt_impulse_prev[i] == 0 && first_frame){
					cnt_z[i] = ch_impulse[i];
					cnt_CNT_prev[i] = CNT;
				}
				else
					cnt_z[i] = ch_impulse[i] - ((CNT - cnt_CNT_prev[i]) - cnt_impulse_prev[i]);
				
				cnt_CNT_prev[i] += cnt_impulse_prev[i];
				cnt_impulse_prev[i] = ch_impulse[i];
				//===========
			}
			Radius_Instr_corr(cnt_z[i],i);
		}
	}
}
/*
проверка задания
*/
void Menu::check_cnt_z_exe(uint8_t chs)
{
	for (int c = 0; c < N_AXIS; c++) {
		if (chs & plb2(c)) {
			if(set_dir_exe(c))//==если в выбранном направление включен ограничитель или задание равно нулю то пропуск
				continue;
		#if (configUSE_mcx514 == 0)
			if(get_mod_zadanie(c) <= get_dopusk(c))	//==если задание в пределах допуска то пропуск
				continue;
		#endif
			sts_c[c] = 1;
		}
	}
}

/*
установка направления и проверка если в выбранном направление включен
ограничитель или задание равно нулю то return 1 если иначе return 0
*/
bool Menu::set_dir_exe(int8_t c)
{
	if (cnt_z[c] > 0){
		if(!(ch_sw.plus & plb2(c)) && !(ch_sw_prog.plus & plb2(c)))
			dir_exe[c] = 1;
		else
			return 1;
	}
	else if (cnt_z[c] < 0){
		if(!(ch_sw.minus & plb2(c)) && !(ch_sw_prog.minus & plb2(c)))
			dir_exe[c] = -1;
		else
			return 1;
	}
	else{ // равно нулю
	#if (configUSE_mcx514 == 1)
		return 0;
	#endif
		return 1;
	}
	return 0;
}

/*
рассчет скоростей для движения по двум осям
*/
void Menu::calc_Linear_inter(char G,const long& F){
	
	//if(F == 0)
	//	return;
	long data[2];
	memset(data,0x00,sizeof(long) * 2);
	
	for(int c = 0 ,s = 0; c < N_AXIS; c++){
		if(cnt_z[c] && sts_c[c] && flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG){
			data[s] = cnt_z[c];
			s++;
			if(s == 2)
				break;
		}			
	}
	if(data[0] == 0 || data[1] == 0)
		return;

	float a = atan((float)data[1]/data[0]); // Y/X
	if(a < 0)
		a = -a;
	for(int c = 0 , s = 0; c < N_AXIS;c++){
		if(cnt_z[c] && sts_c[c] && s == 0){
			F_li[c] = _modals_cmd.F * cos(a);
			s++;
		} else if(cnt_z[c] && sts_c[c] && s == 1){
			F_li[c] = _modals_cmd.F * sin(a);
			break;
		}
	}
	for(int c = 0 ;c < N_AXIS; c++){
		if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG){
			outAnalog(Spd_to_U(F_li[c],c,_modals_cmd.G0),
				flash_prg._OUTAxis2[c].AnalOut,
				flash_prg._OUTAxis2[c].Polar,
				dir_exe[c]);
			}		
		}	
}

int Menu::set_lirCommand(int i)
{		
	if(flash_prg._OUTAxis[i].typeZ == CPrg::DISCRET)
		outABCD(get_STRT_spd(_modals_cmd.G0,i), &flash_prg._OUTAxis[i].Out_A,i);
	else if(flash_prg._OUTAxis[i].typeZ == CPrg::ANALOG)
		outAnalog(Spd_to_U(get_STRT_spd(_modals_cmd.G0,i),i,_modals_cmd.G0),flash_prg._OUTAxis2[i].AnalOut,flash_prg._OUTAxis2[i].Polar,dir_exe[i]);
	else if(flash_prg._OUTAxis[i].typeZ == CPrg::IMPULSE){
		#if (configUSE_mcx514 == 1)
		outMcx514(plb2(i),div_LP(i,cnt_z[i]),Spd_to_Hz(&_modals_cmd.F,i),acceleration(i),Spd_to_Hz(&flash_prg.axis_cfg_nc[i].InitSpd,i));
		#endif
	}
	
	if(flash_prg._Allp_param2[i].delay_VIBOR_PUSK > 0 && !(chs_vibor & plb2(i)))
		tmr_axis_pusk[i] = flash_prg._Allp_param2[i].delay_VIBOR_PUSK;
	else
		PUSK_axis(i);
	VIBOR_axis(i,1);
	return 1;
}
/*
функция для сброса счетчика, если ось круговая циклическая
*/
void Menu::reset_circle_hw_couters(int ch)
{
	if(flash_prg.axis_cfg[ch].type == CPrg::TYPE::LINEAR)
		return;
	if(first_frame == 0) // если кадр не первый то не обнуляем
		return;
	cli();
	long tmp = screen.channel_counter[ch];
	coeff.mult(ch,&tmp);
	tmp %= get_max_circle(ch);
	coeff.div(ch,&tmp);
	if(init.mRef_ext[ch]){
		mCounter[ch].abs = tmp;	
		mCounter[ch].abs_pure = tmp;
	}
	else{
		mCounter[ch].rel = tmp;
	}

	screen.channel_counter[ch] = tmp;
	sei();
}
/*
функция для вычисления задания в G90 для круговой оси (циклической)
алгоритм поиска меньшего пути
*/
void Menu::G90_circle(int ch,long *ch_impulse)
{
	if(flash_prg.axis_cfg[ch].type == CPrg::TYPE::LINEAR)  // если канал линейный ,выходим
		return;
	if(_modals_cmd.G90 != 0) // если не G90 ,выходим
		return;	
	//==на время теста
	struct _test{
		long zd;
		long CNT;
		long ch_impulse;
		long max;
	}t;
	//memset(&t,0,sizeof(t));
	//==
	t.max = get_max_circle(ch);
	t.ch_impulse = ch_impulse[ch];

	if(t.ch_impulse < 0 )  // игнорируем  - в задании на G90
		t.ch_impulse *= -1;
 
	t.CNT =	conv_c(screen.channel_counter[ch],ch,0,1);

	t.zd = t.ch_impulse - t.CNT;

	if(t.zd > (t.max / 2)){
		t.zd = t.ch_impulse - t.max;
		if(t.zd - t.CNT > (t.max / 2)){
			t.zd = t.max - (t.zd - t.CNT);
			t.zd = t.CNT - t.zd;
			}
		t.ch_impulse = t.zd;
	}
	else if(t.zd < (-t.max /2)){
		t.zd = t.max + t.zd;
		t.ch_impulse = t.CNT + t.zd;
	}
	
	ch_impulse[ch] = t.ch_impulse;
}
/*
если ось одна в кадре return 1
если больше return 0
*/
bool Menu::is_single_axis(uint8_t chs)
{
	int8_t cnt_axis = 0;
	for(int c = 0 ; c < N_AXIS; c++){
		if(chs & plb2(c))
			cnt_axis++;
	}

	if(cnt_axis < 2)
		return 1;
	else
		return 0;
}

/*
функция для активации режима паузы в G91 по кнопке стоп
что бы при следующем запуске начать с товоже места
*/
void Menu::pause_G91()
{
	if(_modals_cmd.G90 == 0 || btn_cont_mh || btn_hold_mh){
		mPR.on = false;
		return;
	}

	if(!sts_c[0] && !sts_c[1] && !sts_c[2] && !sts_c[3]){
		mPR.on = false;
		return;
	}

	mPR.on = true; 
	mPR.num = entry_cnt_EDITPR;
}
/*
функция для возобновления перемещения на котором остановились
*/
void Menu::release_G91(long *ch_impulse, uint8_t chs)
{
	if(mPR.on == 0)
		return;	
	if(entry_cnt_EDITPR != mPR.num){  //защита №1 если выполянемый кадр не равен кадру  на котором поставили паузу, возобновления не происходит
		mPR.on = false;
		return;
	}
	if(first_frame == 0){//защита №2 если кадр не первый
		mPR.on = false;
		return;
	}
	
	for (int c = 0; c < N_AXIS; c++) {
		if (chs & plb2(c)) {
			if(cnt_impulse_cur[c] == ch_impulse[c]){ //защита №3 если предыдущее задание не равно текущему
				move_controller(c);
				ch_impulse[c] = cnt_z[c];
			}
		}
	}

	mPR.on = false;
	return;
}


/*
выбор направления движения в режиме доводка
*/
void Menu::set_lir541dovodka(int c)
{
	if (cnt_z[c] > 0) // установка направления
	{
		if(!(ch_sw.plus & plb2(c)) && !(ch_sw_prog.plus & plb2(c)))
			dir_exe[c] = 1;
		else{
			positioning_is_over(c);
			return;
		}
	}
	else if (cnt_z[c] < 0){
		if(!(ch_sw.minus & plb2(c)) && !(ch_sw_prog.minus & plb2(c)))
			dir_exe[c] = -1;
		else{
			positioning_is_over(c);
			return;
		}
	}
	else{
		positioning_is_over(c);
		return;
		}
	PUSK_axis(c);
	outP_all();
}

/*
функция возвращает false если выбран поиск рефметки и в параметрах канала стоит "выбор"
*/
bool Menu::if_vibor_ref()
{
	for(int c = 0 ; c < N_AXIS;c++){
		if(display.ifSearchRef(c)){
			if(flash_prg._Allp_param2[c].ref_dir >= 2)
				return false;
		}
	}
	return true;
}
/*
return true - если мы находимся до зон 01-04-стоп, т е в зоне СТАРТ
return false - если находимся в зонах 01 - 04 или стоп
*/
bool Menu::in_STRT(uint8_t c)
{
	if(cnt_z[c] * dir_exe[c] > flash_prg._Allp_param3[c].zone[0])
		return 1;
	else
		return 0;
}

/*
Команда на движение для поиска реф метки
char reverse - направление поиска реф метки 0 1 в +  ,2 в -
char mode - режим 0 -обычный ,1 - от концевика
*/
void Menu::lir541comRef(char reverse,char mode_)
{
	bool GO_REF = false;
	memset(cnt_z, 0x00, sizeof(long) * N_AXIS);	
	for(int c = 0; c < N_AXIS; c++){
		if(display.ifSearchRef(c)){
			v_luft.init(c);
			char DIR = flash_prg._Allp_param2[c].ref_dir;
			if(reverse){
				if(DIR >= 2){ //если выбор 2 или 3
					if(reverse == 1)
						DIR = 0;
					else if(reverse == 2)
						DIR = 1;
				}
				else
					DIR ^= 1;
				if(mode_){
					limit_sw[c] = 1;
					}
				else
					limit_sw[c] = 0;//инициализация переменной
				}
			else{
					limit_sw[c] = 0;//инициализация переменной
				}
			if(DIR){ //+
				if(!(ch_sw.plus & plb2(c)))
					dir_exe[c] = 1;
				else
					continue;
			}
			else{ //-
				if(!(ch_sw.minus & plb2(c)))
					dir_exe[c] = -1;
				else
					continue;
			}						
			if(mode_){
				if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
					outABCD(v_luft.set(c,flash_prg._Allp_param[c].limit_PM), &flash_prg._OUTAxis[c].Out_A,c);
				else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
					outAnalog(Spd_to_U(v_luft.set(c,flash_prg._Allp_param6[c].limit_PM),c,_modals_cmd.G0),flash_prg._OUTAxis2[c].AnalOut,flash_prg._OUTAxis2[c].Polar,dir_exe[c]);
			}
			else{
				if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)				
					outABCD(v_luft.set(c,flash_prg._Allp_param[c].start_PM), &flash_prg._OUTAxis[c].Out_A,c);
				else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
					outAnalog(Spd_to_U(v_luft.set(c,flash_prg._Allp_param6[c].start_PM),c,_modals_cmd.G0),flash_prg._OUTAxis2[c].AnalOut,flash_prg._OUTAxis2[c].Polar,dir_exe[c]);
			}
			
			outP(flash_prg._OUTAxis[c].vibor_axis, 1); //выставить выход Выбор Оси
			if(flash_prg._Allp_param2[c].delay_VIBOR_PUSK > 0)
				tmr_axis_pusk[c] = flash_prg._Allp_param2[c].delay_VIBOR_PUSK;
			else
				PUSK_axis(c);
			if(dir_exe[c] == 1){	
				l_k_s.ON_LED(_led_RIGHT);
				l_k_s.OFF_LED(_led_LEFT);
			}
			else if(dir_exe[c] == -1){
				l_k_s.OFF_LED(_led_RIGHT);
				l_k_s.ON_LED(_led_LEFT);
			}
			if(mode_ == 0 && MsgKey.mode_ref_submode_ref_ref == 0){ //активация режима поиска реф меток только для РМ
				if(flash_prg._Allp_param2[c].delay_VIBOR_PUSK > 0)
					tmr_ref_pusk[c] = flash_prg._Allp_param2[c].delay_VIBOR_PUSK;
				else
					tmr_ref_pusk[c] = 0;
				tmr_ref_pusk[c] += 1000; // добавляем 1 сек
			}
			sts_c[c] = 1;
			GO_REF = true;
		}
	}
	if(GO_REF)
		outP_all();
}
/*
uint8_t chs - выбранные каналы
*/
bool Menu::if_TORMOZ(uint8_t chs)
{
	for(int c = 0; c < N_AXIS ;c++){
		if(TORMOZ[c] && (chs & plb2(c))){
			status(PSTR("ОСЬ НА ТОРМОЗЕ!"),300);
			return true;
		}
	}
	return false;
}

bool Menu::if_ENC(uint8_t chs)
{
	for(int c = 0; c < N_AXIS ;c++){
		if(is_frame_move_prednabor() || is_move_hand()){
			if(ch_use(c) && flash_prg.is_encoder_fault(c) && (chs & plb2(c))){
				status(PSTR("ОШИБКА ДАТЧИКА!"),300);
				return true;
			}
		}
		else if(is_frame_move_prog()){
			if(ch_use(c) && flash_prg.is_encoder_fault(c)){
				status(PSTR("ОШИБКА ДАТЧИКА!"),300);
				return true;
			}
		}		
	}
	return false;
}

/*проверка если канал абсолютный ,сверить заданное расстояние с программным ограничителем
long *ch_impulse - массив с заданием
uint8_t chs - выбранные каналы
return true  - если ошибка
*/
bool Menu::if_ABS(long *ch_impulse,uint8_t chs)
{
	long tmp = 0;
	for(int c = 0; c < N_AXIS ;c++){
		if(ref_.pRef[c] && (chs & plb2(c))){
			cli();
			const long CNT_tst = conv_c(mCounter[c].abs,c,1,1); // счетчик только G53
			const long CNT_tst0 = conv_c(mCounter[c].abs,c,0,1);// счетчик G53 - G5x
			sei();	
			if (MsgKey.offset_number > 0 && MsgKey.offset_number < 6){	
				if(_modals_cmd.G90 == 0)						
					tmp = ch_impulse[c] - CNT_tst0;
				else if(_modals_cmd.G90 == 1)
					tmp = ch_impulse[c];
				tmp = tmp + CNT_tst;
			}
			else{	
				if(_modals_cmd.G90 == 0)
					tmp = ch_impulse[c];
				else if(_modals_cmd.G90 == 1)
					tmp = CNT_tst + ch_impulse[c];
			}			

			if(flash_prg._Allp_param2[c].limit_plus){//проверяем не выходит ли за ограничитель +
				if(tmp > flash_prg._Allp_param2[c].limit_plus){
					status(PSTR("Задание > ограничения+"),300);
					return true;
					}
			}
			if(flash_prg._Allp_param2[c].limit_minus){//или -
				if(tmp < flash_prg._Allp_param2[c].limit_minus){
					status(PSTR("Задание < ограничения-"),300);
					return true;
					}
			}			
		}
	}
	return false;
}
/*
функция производит ряд преобразований если счетчик циклический круговой
*/
bool Menu::if_CIRCLE(long *ch_impulse,uint8_t chs)
{
	//==если ось циклическая круговая то сбрасываем счетчики
	for(int c = 0 ; c < N_AXIS; c++){
		if (chs & plb2(c)) {
			reset_circle_hw_couters(c);
			G90_circle(c,ch_impulse);
		}
	}
	//==
	return 0;
}

#endif
/*
long *cnt - указатель на массив задания
char *frame - ASCII кадр
int sz_frame - размер кадра
return uint8_t  - каналы для движения  , 0 - в кадре нет движения
*/
uint8_t Menu::get_bin_from_cmd(long *cnt,char *frame,int sz_frame)
{
	uint8_t chs = 0;
	for (int i = 0; frame[i] != '\0'; i++)
	{
		if (frame[i] == pgm_read_byte_near(sAxis_screen[Str[0]] + 2) ||
			frame[i] == pgm_read_byte_near(sAxis_screen[Str[1]] + 2) ||
			frame[i] == pgm_read_byte_near(sAxis_screen[Str[2]] + 2) ||
			frame[i] == pgm_read_byte_near(sAxis_screen[Str[3]] + 2) ||
			frame[i] == 'I' || frame[i] == 'J')
		{
			char sym_fr = frame[i];
			for (int f = i; f < sz_frame; f++)
			{
				if (frame[f] == '\0') {
					char buf[22] = { 0 };
					int f = get_format_from_ASCII(sym_fr);
					to_format_cmd(buf, frame, i, f);
					uint8_t n = get_channel_from_ASCII(sym_fr);
					if (n >= N_AXIS){							
						if(sym_fr == 'I' || sym_fr == 'J'){
							long * p;
							if(sym_fr == 'I')
								p = &IJ[0];
							else //(sym_fr == 'J')
								p = &IJ[1];
							ConvertASCIIlinear_circle_tolong(*p,f, (unsigned char*)buf);
						}		
						break;
					}
					chs |= plb2(n);					
					if (f == CPrg::FORMAT::CIRCLE_SECUNDO || f == CPrg::FORMAT::CIRCLE_MINUTO)
						ConvertASCIIcircle_secundo_tolong_new(cnt[n],f, (unsigned char*)buf);
					else
						ConvertASCIIlinear_circle_tolong(cnt[n],f, (unsigned char*)buf);
					break;
				}
			}
		}
	}

	return chs; //выбранные каналы
}
/*
функция для извлечения номера команды в tmp
in  char CMD     - разделитель
out long& tmp    - полученные данные
in  char* frame  - ASCII кадр
int sz_frame     - размер ASCII кадра
char p           - текущее смещение в строке (указатель)
*/
bool Menu::get_cmd(char CMD, long& tmp, char* frame, int sz_frame,char &p)
{
	for (int i = p; frame[i] != '\0'; i++)
	{
		if (i >= sz_frame - 1)
		return false;
		if (frame[i] == CMD) {
			char buf[10] = { 0 };
			for (int j = 0; j < 10; j++) {
				if (i + j + 1 >= sz_frame) {
					return false;
				}
				if (frame[i + j + 1] == ' ' || frame[i + j + 1] == '\0') {
					buf[j] = '\0';
					tmp = strtol_lir((unsigned char*)buf);
					p = i + j + 1;
					return true;
				}
				buf[j] = frame[i + j + 1];
			}
		}
	}
	return false;
}

void Menu::get_F_from_cmd(long *F,char *frame,int sz_frame)
{
	for (int i = 0; frame[i] != '\0'; i++)
	{
		if(frame[i] == 'F' || frame[i] == 'S'){
			char buf[15] = {0};
			for(int j = 0; j<15;j++){
				if(i + j + 1 >= sz_frame){
					return;
					}
				if(frame[i + j + 1] == ' ' || frame[i + j + 1] == '\0'){
					buf[j] = '\0';
					//*F = strtol_lir((unsigned char*)buf);
					ConvertASCIIlinear_circle_tolong(*F, CPrg::FORMAT::LINEAR_4, (unsigned char *)buf);
					if(*F < 0)
						*F = -*F;
					if(*F > 99999999L)
						*F = 99990000L;
					_modals_cmd.F = *F;
					return;
				}				
				buf[j] = frame[i + j + 1];
			}
		}
	}
}

void Menu::get_G_from_cmd(char *G,char *frame,int sz_frame)
{
	long tmp;
	char p = 0;
	while (get_cmd('G', tmp, frame, sz_frame,p)) {
		*G = tmp;
		if(*G == 0)
			_modals_cmd.G0 = 0;
		else if(*G == 1 ||*G == 2 || *G == 3)
			_modals_cmd.G0 = 1;
		else if(*G == 90)
			_modals_cmd.G90 = 0;
		else if(*G == 91)
			_modals_cmd.G90 = 1;
		else if (*G >= 53 && *G <= 58)
			MsgKey.offset_number = *G - 53;
		else if(*G >= 17 && *G <= 19)
			_modals_cmd.G17 = *G;
		else if(*G == 40 || *G == 45 || *G == 46)
			_modals_cmd.G40 = *G;
		else if(*G == 43 || *G == 44 || *G == 49)
			_modals_cmd.G43 = *G;
	}
}

int Menu::get_M_from_cmd(char *M,char* frame, int sz_frame)
{
	long tmp;
	char p = 0;
	while (get_cmd('M', tmp, frame, sz_frame,p)) {
		*M = (char)tmp;
		//========================================обработка M функций====================================================
		if (tmp == 0){ // M00			
			if(curItem->id == idExecmd){
				if(first_frame == 0){
					//last_frame = true;
					outP(flash_prg._OUTGen.texnolog_stop_M0,1,1);//выставить выход М0
					outM5_delay();
					if (config_NC.one_frame)
						entry_exe = ONE_FRM_DOWN;
					else	
						entry_exe = INIT;
					return 1;
				}
			}									
		}
		else if (tmp == 1) // M01
			last_frame = true;
		else if (tmp == 2){ //M02			
			if(curItem->id == idExecmd){
				//last_frame = true;
				outP(flash_prg._OUTGen.konec_program_M2,1,1);
				outM5_delay();
				if (config_NC.one_frame)
					entry_exe = ONE_FRM_DOWN;
				else
					entry_exe = INIT;
				return 1;
				}
			}
		else if (tmp == 20) { //M20
			if(curItem->id == idExecmd){
				if (_modals_cmd.H.cnt) {
					_modals_cmd.H.cnt--;
					if(_modals_cmd.H.cnt){
						searcher(_modals_cmd.H.fr + 1);
						change_exe(0);
						last_frame = false;
						}
					}
				}
			}
		else if (tmp == 30){ // M30
			if(curItem->id == idExecmd){
				while (entry_cnt_EDITPR) {
					scroll_up(entry_cnt_EDITPR, entry_scroll_EDITPR, _resz_prg_v.ALIGN, _choice_prg.cnt, _resz_prg_v.SZ);
				}
				outM5_delay();
				entry_exe = INIT;
				return 1;
				}
			}
			//== M функции ожидающие входа
		else if(tmp == 102 || tmp == 112 || tmp == 122 ||
				tmp == 132 || tmp == 142 || tmp == 152 ||
				tmp == 162 || tmp == 172){
				if(set_wait_M_input(tmp))
					return 1;
				}
			//== M функции сбрасывающие выход
		else if(tmp == 100 || tmp == 110 || tmp == 120 ||
				tmp == 130 || tmp == 140 || tmp == 150 ||
				tmp == 160 || tmp == 170)
				set_M_output(tmp,0);
			//== M функция устанавливающая выход
		else if(tmp == 101 || tmp == 111 || tmp == 121 ||
				tmp == 131 || tmp == 141 || tmp == 151 ||
				tmp == 161 || tmp == 171)
				set_M_output(tmp,1);
			//==============================================================================================================
		}
	return 0;
}

int Menu::set_wait_M_input(int M)
{
	char *p_M = &flash_prg._INGen2.M102;
	int M_tmp = 102;
	char res;
	for(int i = 0;  i < 8; i++){
		if(M == M_tmp){
			res = inP(*p_M);
			if(res == 0 ){
			//записываем в глобальный указатель адрес входа и ждем
			p_M102_gl = p_M;
			entry_exe = DELAY_INPUT_M;			
			return 1;
			}		
		}
		p_M++;
		M_tmp+=10;
	}
	p_M102_gl = nullptr;
	return 0;
}

void Menu::set_M_output(int M,bool set)
{
	char *p_M = &flash_prg._OUTGen2.M100off_M101on;
	int M_tmp = 100;

	if(set)
		M_tmp = 101;

	for(int i = 0;  i < 8; i++){
		if(M == M_tmp){
			outP(*p_M,set,1);
			return;
			}
		p_M++;
		M_tmp += 10;
	}
}
/*
return 1 - команда выполнена успешно
return 2 - команда не выполнена
return 0 - команда не выполнена
*/
char Menu::M3M5M4_cmd(char M, m_state& state)
{
	char res;
	if (M == 3 || M == 4 || M == 5) {
		if (M == 5) {
			outM5_delay();			
			if(state != DELAY_WAIT_SHPINDEL)
				state = DELAY_M5M3M4;
			return 1;
		}
		else if (M == 3) {
			M3_mem = 1;
			res = outM3_delay(1, M3_mem);
			if(res == 1){
				if(state != DELAY_WAIT_SHPINDEL)
					state = DELAY_M5M3M4;
				return 1;
				}
			else if(res == 0)
				return 0;
			else if(res == 2){
				state = INIT;
				return 2;
			}
		}
		else if (M == 4) {
			M3_mem = 0;
			res = outM3_delay(1, M3_mem);
			if(res == 1){
				if(state != DELAY_WAIT_SHPINDEL)
					state = DELAY_M5M3M4;
				return 1;
				}
			else if(res == 0)
				return 0;
			else if(res == 2){
				state = INIT;
				return 2;
			}
		}
	}
	return 0;
}

bool Menu::get_L_from_cmd(char* frame, int sz_frame)
{
	if (call_sub)
	return false;

	char n_sub_prg;
	uint32_t addres_tmp;
	long tmp;
	char p = 0;
	if(!get_cmd('L', tmp, frame, sz_frame,p))
		return false;
	n_sub_prg = (char)tmp;
	if (n_sub_prg > 0) {
		addres_tmp = get_addres_prg_cur(1, n_sub_prg - 1);
		addres_tmp -= HEADER_PRG_SZ;
		myflash.Read_flash_at45(addres_tmp, (uint8_t*)&_header_prg, sizeof(_header_prg));
		if (_header_prg.sign == (short)SIGN_PRG) {
			if (_header_prg.cnt == 0)
				return false;
			//==сохранение текущего во  временный,выгрузить после завершения подпрограммы
			cp_tmp.addres_prg = _choice_prg.addres_prg;
			cp_tmp.cnt = _choice_prg.cnt;
			ecEDITPR_tmp = entry_cnt_EDITPR;
			esEDITPR_tmp = entry_scroll_EDITPR;
			//==
			_choice_prg.addres_prg = addres_tmp + HEADER_PRG_SZ;
			_choice_prg.cnt = _header_prg.cnt;
			entry_cnt_EDITPR = 0;
			entry_scroll_EDITPR = 0;
			call_sub++;
			return true;
		}
		else
			return false;
	}

	return false;
}

bool Menu::get_E_from_cmd(char* frame, int sz_frame)
{
	long tpm_num;
	char p = 0;
	if(get_cmd('E', tpm_num, frame, sz_frame,p)){
		if (tpm_num > 0 && tpm_num <= _choice_prg.cnt) {
			searcher(tpm_num);
			return 1;
		}
	}
	return 0;
}

void Menu::get_H_from_cmd(char* frame, int sz_frame)
{
	long tmp;
	char p = 0;
	if(get_cmd('H', tmp, frame, sz_frame,p)){
		if (tmp > 99)
			tmp = 99;
		else if (tmp < 0)
			tmp = 0;
		_modals_cmd.H.cnt = tmp;
		_modals_cmd.H.fr = entry_cnt_EDITPR;
		return;
	}
}

void Menu::get_T_from_cmd(char* frame, int sz_frame)
{
	//проверка назначено ли три оси
	//проверка есть ли ось Z (ось Z ось с инструментом)
	//ищем букву T в строке
	//_modals_cmd.T_instr инициализируем переменную
	long tmp;
	char p = 0;
	if(get_cmd('T', tmp, frame, sz_frame,p)){
		if (tmp > 10)
			return;
		else if (tmp < 0)
			return;
		else if (tmp == 0)
			return;
		_modals_cmd.T_instr = tmp;
			return;
	}
}

bool Menu::get_G4xxdelay_from_cmd(char* frame, int sz_frame)
{
	char n = display.strnlen_lir(frame, sz_frame);
	uint16_t tmp;
	if (n < 4)
	return 0;

	for (int i = 0; frame[i] != '\0'; i++) {
		if (frame[i] == 'G' && frame[i + 1] == '4' && frame[i + 2] == '.') {
			tmp = (uint16_t)strtoi_lir((uint8_t*)frame + i + 3);
			if (tmp > 10)
				tmp = 10;
			if (tmp >= 0) {
				//timer_frm = tmp * (1000 - 100); // 1 msek period
				timer_frm = tmp * (100 - 10); // 10 msek period
				timer_sv = timer_frm;
				timer_frm++;//установка флага начала отсчета
				return 1;
			}
			else
				return 0;
		}
	}
	return 0;
}
/*
обработка таймера G4.
return true - таймер истек
return false - таймер тикает
*/
bool Menu::tmr_frame()
{
	//if (config_NC.one_frame)  если не нужна задержка в покадровом выполнении
	//	return true;
	if (timer_sv) {
		if (timer_frm) {
			if (timer_sv == timer_frm - 1) {//начало отсчета
				cli();
				cnt_tf = 0;
				sei();
				timer_frm--;//снятие флага начала отсчета
				return false;
			}
			else { //считаем
				if (timer_frm < cnt_tf) {
					timer_frm = timer_sv + 1;//+ 1 - установка флага начала отсчета
					return true;
				}
				else {
					timer_frm -= cnt_tf;
					cli();
					cnt_tf = 0;
					sei();
					return false;
				}
			}
		}
		else {
			timer_frm = timer_sv + 1;//+ 1 - установка флага начала отсчета
			return true;
		}
	}

	return true;
}

#if (configUSE_mcx514 == 1)

void Menu::Set_Axis_Conf_mcx514(int8_t c)
{
	int opt = 0;
	if (flash_prg.axis_cfg_nc[c].sig_type == CPrg::STEP_DIR)
		opt |= 0x0008; // step dir инверсия
	else if (flash_prg.axis_cfg_nc[c].sig_type == CPrg::Quad_A_B)
		opt |= 0x0010; // quad
	
	if(flash_prg.axis_cfg_nc[c].dir == CPrg::DIRECTION::MINES){
		opt |= 0x0040;
		//opt |= 0x0800; // инверсия энкодера
	}
	cli();
	WriteReg3(plb2(c), opt);
	sei();
}

/*
функция для Relative position driving
int axis           - канал MCX514_AXIS_X,MCX514_AXIS_Y,MCX514_AXIS_U,MCX514_AXIS_Z
const long& pulse  - кол - во импульсов (задание)
const long& F      - частота импульсов (скорость) Гц
const long&Accel   - ускорение/замедление
const long&InitSpd - начальная частота (скорость) Гц
*/
void Menu::outMcx514(int axis,const long& pulse,const long& F,const long&Accel,const long&InitSpd)
{
	if(F == 0)
	return;
	if(pulse == 0)
	return;
	cli();
	SetStartSpd(axis, InitSpd);// Initial speed 1pps
	SetSpeed(axis, F);	// Drive speed 10ps
	SetAcc(axis, Accel);	// Acceleration (maximum in specification)
	SetPulse(axis, pulse);	// Drive pulse number 500
	ExeDRVRL(axis);		// Relative position driving
	sei();
}

/*
функция для Continuously driving
int axis           - канал MCX514_AXIS_X,MCX514_AXIS_Y,MCX514_AXIS_U,MCX514_AXIS_Z
const long& F      - частота импульсов (скорость) Гц
const long&Accel   - ускорение/замедление
const long&InitSpd - начальная частота (скорость) Гц
*/

void Menu::outMcx514_cont(int axis,int8_t c,const long& F,const long& Accel,const long& InitSpd)
{
	if(F == 0)
		return;
	cli();
	SetStartSpd(axis, InitSpd);	// Initial speed pps InitSpd
	SetSpeed(axis, F);	// Drive speed ps F
	SetAcc(axis, Accel);	// Acceleration (maximum in specification)Accel
	//SetJerk(axis, 1000000);	// Jerk 1pps/sec2

	if(dir_exe[c] == 1)
		ExeDRVVP(axis);
	else if(dir_exe[c] == -1)
		ExeDRVVM(axis);
	sei();
}

void Menu::servo_c_mcx514()
{	
	if(entry_exe == WORKING_EXE || entry_hand == WORKING_EXE){
			for(int c = 0 ; c < N_AXIS;c++){
				if(!sts_c[c])
					continue;
				delay_PUSK(c);
				move_controller(c);
				if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET || flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
					G0G1(c);
			}
	}	
		for(int c = 0; c < N_AXIS; c++){
			if(P2[c] == no_axis)
				continue;
			if(st_Drive & plb2(c))
				continue;
			MAXO.process(c);			
		}		
	st_Drive = waitdrive_rtc(MCX514_AXIS_ALL);
	if(entry_exe == WORKING_EXE && !st_Drive && alert == OCb_VNE_DOPUSKA)
		entry_exe = INIT;//останавливаем дальнейшую отработку кадров
	else if(entry_exe == WORKING_EXE && !st_Drive)//возвращаемся в меню пуска команды
		if_pass_frm();
	else if(entry_hand == WORKING_EXE && !st_Drive)
		entry_hand = INIT;

	
}

/*
Трапецивидное симметричное ускорение/торможение  = (F(drive speed) - InitSpeed)/время(секунды),
т к время можно задать меньше секунды  1< ,то применена функция деления div_di
*/
long Menu::acceleration(const unsigned char &ch)
{
	return div_di(CPrg::FORMAT::LINEAR_1,Spd_to_Hz(&_modals_cmd.F,ch) - Spd_to_Hz(&flash_prg.axis_cfg_nc[ch].InitSpd,ch),flash_prg.axis_cfg_nc[ch].t);
}

/*
функция для преобразования частоты из F(мм/мин) или S(об/мин) в Hz(герц)
формат скорости в 0.0000
in long *F_ - частота в мм/мин
in signed char ch - канал

return (long)F - частота в герцах для привода
*/
long Menu::Spd_to_Hz(long *F_,signed char ch,char apmlifire/* = 0*/)
{
	long long F = *F_ ;
	F = (F * _modals_cmd.Feed) / 100;	//умножение на процент Feed
		
	if(_modals_cmd.G0 == 0)
		F = flash_prg.axis_cfg_nc[ch].MaxSpd;
	else{
		if(F == 0)
			return 1;// скорость 1 Hz при  0 Hz зависает , если G1 и скорость равна 0
		}

	if(F > flash_prg.axis_cfg_nc[ch].MaxSpd)
		F = flash_prg.axis_cfg_nc[ch].MaxSpd;
	
	F = (F * flash_prg.axis_cfg_nc[ch].imp_oborot )/flash_prg.axis_cfg_nc[ch].move_oborot;
	F = F/60;

	if(apmlifire)
		F = mul_di(CPrg::FORMAT::LINEAR_2,F,flash_prg.axis_cfg_nc2[ch].F_amplifire);

	return (long)F;
}

void Menu::linear_interpolation(uint8_t chs)
{
	char main_axis = N_AXIS;
	int8_t cnt_axis = 0;

	for(int c = 0 ; c < N_AXIS;c++){
		if(sts_c[c] && flash_prg._OUTAxis[c].typeZ == CPrg::IMPULSE)
			cnt_axis++;
	}

	if(cnt_axis < 2)
		return;
	//==вычисляем главную ось
	for(int c = 0 ; c <N_AXIS;c++){
		if(sts_c[c]){
			main_axis = c;
			break;
		}
	}
	//==
	WriteReg6(chs);
	WriteReg0(MCX514_CMD2A_IPM);

	SetStartSpd(chs, Spd_to_Hz(&_modals_cmd.F,main_axis));	// Initial speed 1pps
	SetSpeed(chs, Spd_to_Hz(&_modals_cmd.F,main_axis));// Drive speed 10ps

	for(int c = 0; c < N_AXIS; c++){
		if(sts_c[c])
			SetPulse(plb2(c), div_LP(c,cnt_z[c]));
	}
	if(cnt_axis == 2)
		ExeLHK2();
	else if (cnt_axis == 3)
		ExeLHK3();
	else if (cnt_axis == 4)
		ExeLHK4();
}

void Menu::circle_interpolation(uint8_t chs,char G)
{
	char main_axis = N_AXIS;
	int8_t cnt_axis = 0;

	for(int c = 0 ; c < N_AXIS;c++){
		if(sts_c[c] && flash_prg._OUTAxis[c].typeZ == CPrg::IMPULSE)
			cnt_axis++;
	}

	if(cnt_axis != 2) //only 2 axis
		return;
	//==вычисляем главную ось
	for(int c = 0 ; c <N_AXIS;c++){
		if(sts_c[c]){
			main_axis = c;
			break;
		}
	}
	//==

	WriteReg6(0x40 | chs);
	WriteReg0(MCX514_CMD2A_IPM);

	SetStartSpd(chs, Spd_to_Hz(&_modals_cmd.F,main_axis));	// Initial speed 1pps
	SetSpeed(chs, Spd_to_Hz(&_modals_cmd.F,main_axis));// Drive speed 10ps

	for(int c = 0; c < N_AXIS; c++){
		if(sts_c[c])
			SetPulse(plb2(c), div_LP(c,cnt_z[c]));//Set Finish Point
	}
	
	for(int c = 0,ij = 0; c < N_AXIS;c++){
		if(sts_c[c] && ij < 2)
			SetCCP(plb2(c), div_LP(c,IJ[ij++])); //Set Central Point: I,j
	}

	if(G == 2)
		ExeCHKCW();
	else if(G == 3)
		ExeCHKCCW();
}

long Menu::get_f_MD(int8_t c)
{
	long f;
	if (flash_prg.axis_cfg[c].format == CPrg::FORMAT::LINEAR_1)
		f = 1000;
	else if (flash_prg.axis_cfg[c].format == CPrg::FORMAT::LINEAR_2)
		f = 100;
	else if (flash_prg.axis_cfg[c].format == CPrg::FORMAT::LINEAR_3)
		f = 10;
	else
		f = 1;
	return f;
}
/*
умножение логического счетчика mcx514 для вывода на дисплей
*/
long Menu::mul_LP(int8_t c,long long in)
{
	long f  = get_f_MD(c);
	return (in * (flash_prg.axis_cfg_nc[c].move_oborot /f)) / flash_prg.axis_cfg_nc[c].imp_oborot;
}
/*
деление логического счетчика mcx514 для записи в mcx514
*/
long Menu::div_LP(int8_t c,long long in)
{
	long f = get_f_MD(c);	
	return (in * flash_prg.axis_cfg_nc[c].imp_oborot ) / (flash_prg.axis_cfg_nc[c].move_oborot /f);
}

long Menu::mul_MAXO(int8_t c,long long in)
{
	long f = get_f_MD(c);	
	long tmp_hw_ob = ((flash_prg.axis_cfg_nc2[c].hw_oborot_mm /f) * MAXO.mul) / 100;
	return (in * tmp_hw_ob) / flash_prg.axis_cfg_nc2[c].hw_oborot_imp;
	//in = (in * maxo_mul) / 100;//умножение на процент
	//return (in * (flash_prg.axis_cfg_nc2[c].hw_oborot_mm /f)) / flash_prg.axis_cfg_nc2[c].hw_oborot_imp;
}

long Menu::div_MAXO(int8_t c,long long in)
{
	long f = get_f_MD(c);
	long tmp_hw_ob = ((flash_prg.axis_cfg_nc2[c].hw_oborot_mm /f) * MAXO.mul) / 100;
	return (in * flash_prg.axis_cfg_nc2[c].hw_oborot_imp) / tmp_hw_ob;
	//in = (in * 100 ) / maxo_mul;//деление на процент
	//return (in * flash_prg.axis_cfg_nc2[c].hw_oborot_imp) / (flash_prg.axis_cfg_nc2[c].hw_oborot_mm /f);
}

#endif


void Menu::menu_stop_exe()
{
	if(curItem->id == idExecmd ){
		no_view_prog();
		entry_exe = EXIT_EXE;
	}
}

void Menu::choice_exe_up()
{
	if (curItem->id == idExecmd) {
		scroll_up(entry_cnt_EDITPR, entry_scroll_EDITPR, _resz_prg_v.ALIGN, _header_prg.cnt, _resz_prg_v.SZ);
		entry_exe = UPDATE;
	}
}

void Menu::choice_exe_down()
{
	if (curItem->id == idExecmd) {		
		scroll_dw(entry_cnt_EDITPR, entry_scroll_EDITPR, _resz_prg_v.ALIGN, _choice_prg.cnt, _resz_prg_v.SZ);
		entry_exe = UPDATE;
	}
}

void Menu::menu_left_exe()
{
	//добавить переход вверх +11
	if (curItem->id == idExecmd) {	
		for(int i = 0 ; i < 11;i++)
			scroll_up(entry_cnt_EDITPR, entry_scroll_EDITPR, _resz_prg_v.ALIGN, _header_prg.cnt, _resz_prg_v.SZ);
	entry_exe = UPDATE;
	}
}

void Menu::menu_right_exe()
{
	//добавить переход вниз -11
	if (curItem->id == idExecmd) {
		for(int i = 0 ; i < 11;i++)
			scroll_dw(entry_cnt_EDITPR, entry_scroll_EDITPR, _resz_prg_v.ALIGN, _choice_prg.cnt, _resz_prg_v.SZ);
	entry_exe = UPDATE;
	}
}

/*
функция вызывается по нажатию кнопки _key_BLANK4,
*/
void Menu::go_cmd_from_exe()
{

}

void Menu::delete_cmd_exe()
{
	if (curItem->id == idExecmd) {
		Lir.reset_error();		
		/*if (cnt_frame) {
			for (int i = frame_CURSOR_exe; i >= 0; i--) {
				if(i == 0 )
					memset(frame_cmd[i],'\0', FRAME_SZ);
				else
					memcpy(frame_cmd[i], frame_cmd[i - 1], FRAME_SZ);
			}
			cnt_frame--;
			if (cnt_frame < MAX_FRAME - frame_CURSOR_exe) {
				if(cnt_frame)
				frame_CURSOR_exe++;
				if (scroll_exe)
				scroll_exe--;
			}
			set_num_frames();
			entry_exe = CHANGE_EXE;
			
		}
		if (cnt_frame)
			status(PSTR("Нажмите СТАРТ"));
		else
			status(PSTR("             "));*/
	}
}

void Menu::menu_start_exe()
{
	if (Lir.curItem->id == idExecmd) {
		if(extPULT.on)
			return;
		first_frame = true;
		entry_exe = EXECUTE_EXE;
	}
}

void Menu::go_edit_frame_exe(const char & BLANK)
{
	if (BLANK == _key_ENTER) {
		if (curItem->id == idExecmd) {
			/*	if (cnt_frame && frame_cmd[frame_CURSOR_exe][3] != '/' &&
				display.strnlen_lir(frame_cmd[frame_CURSOR_exe]) < STR_FRAME_SZ_DISPLAY){ //отключение редактирования двухстрочного кадра, доделать
				curItem = &mCmd;
				entry_exe = INIT_EXE;
				entry_cmd = INIT_EDIT_CMD; 
			}*/
		}
	}
}

void Menu::blank_right_EXE(const char & BLANK)
{
	if (curItem->id == idExecmd) {
		switch (BLANK)
		{
			case _key_BLANK1:// переключение вида
			if (func_exe == 0){				
			//menu_stop_exe();				
				toggle_view();
				change_exe(0);
				entry_exe = UPDATE_BLANK_FUNC;
			}
			break;
			case _key_BLANK2:
			if (func_exe == 0) {
				if (_choice_prg.cnt > 0)
					com_frame_EDITPR(_choice_prg.addres_prg);
				else
					status(PSTR("нет кадров"), 300, 0);
				entry_exe = UPDATE;
			}
			break;
			case _key_BLANK3:	
			if (func_exe == 0) {
				config_NC.one_frame ^= 1;
				entry_exe = UPDATE_BLANK_FUNC;
			}		
			break;
			case _key_BLANK4: 
			break;
			default:
			return;
		}
	}
}

//функции для изменения Feed работают в разных режимах
void Menu::Feed_DOWN()
{
	if(!is_Analog())
		return;
	if(flash_prg._Allp_param7.korr_podach == 0)
		return;
	if(passedZN(0))
		return;	
	
	if(l_k_s.lock_key == PROCESSING_HOLDDOWN)
		_modals_cmd.Feed -= 10;
	else
		_modals_cmd.Feed--;
	if(_modals_cmd.Feed < flash_prg._Allp_param7.min_percent_feed)
		_modals_cmd.Feed = flash_prg._Allp_param7.min_percent_feed;
	if(MsgKey.mode != TMsgKey::FIND_REF)
		UPD_show_Instr = 1;
}
//функции для изменения Feed работают в разных режимах
void Menu::Feed_UP()
{
	if(!is_Analog())
		return;
	if(flash_prg._Allp_param7.korr_podach == 0)
		return;
	if(passedZN(0))
		return;

	if(l_k_s.lock_key == PROCESSING_HOLDDOWN)
		_modals_cmd.Feed += 10;
	else
		_modals_cmd.Feed++;
	if(_modals_cmd.Feed > flash_prg._Allp_param7.MAX_percent_feed)
		_modals_cmd.Feed = flash_prg._Allp_param7.MAX_percent_feed;
	if(MsgKey.mode != TMsgKey::FIND_REF)
		UPD_show_Instr = 1;
}
/*
char z - ,
0 - 1 зона,
1 - 2 зона,
2 - 3 зона,
3 - 4 зона,
4 - зона СТОП
return true - если есть движение по каналу c датчиками и пройдена зона
*/
bool Menu::passedZN(int8_t z)
{
	for(int c = 0; c < N_AXIS;c++){
		if(sts_c[c] && zn[c][z])
			return 1;
	}
	return 0;
}





bool Menu::ifG1()
{
	if(extPULT.on){
		if(extPULT.G0 == 1)
			return 1;
	}
	else if(_modals_cmd.G0 == 1)
			return 1;
	return 0;
}


void Menu::Led_START_STOP()
{
	if (Lir.st_Drive){
		l_k_s.ON_LED(_led_START);
		l_k_s.OFF_LED(_led_STOP);
	}
	if (Lir.st_Drive == 0){
		l_k_s.ON_LED(_led_STOP);
		l_k_s.OFF_LED(_led_START);		
	}
}

void Menu::actions_mh(const unsigned char & page)
{
	if(entry_mh == INIT_MH){
		return;//set_axis_mh(page);
		}
	if(entry_mh == IDLE_MH)
		return;

}
/*
функция вызывается по нажатию кнопки выбора оси L_BLANK1, L_BLANK2,L_BLANK3,L_BLANK4
для перемещения оси в ручную
*/
void Menu::set_axis_mh(const unsigned char & page)
{	
	if(MsgKey.key == _key_BLANK1_L)		
		sym_invers(0,1,1);			
	else if(MsgKey.key == _key_BLANK2_L)
		sym_invers(1,1,1);
	else if(MsgKey.key == _key_BLANK3_L)
		sym_invers(2,1,1);
	else if(MsgKey.key == _key_BLANK4_L)
		sym_invers(3,1,1);

	if(!ch_to_move)
		stop_mh();
}
/*
int s - номер строки
char set - 0 - сброс ,1 - установка , 2 - сброс или установка
bool clr - очистка всех строк и выходов
*/
void Menu::sym_invers(int s,char set,bool clr)
{	
	if(clr)
		update_pic_axis_mh(0,0);
	
	//==поправка для слепой оси
	int ch;
	if(s == SW_AXIS0)	
		ch = 4;
	else
	    ch = get_channel(Lir.Str[s]); //нет проверки на отсутствие канала
	//==	
	if(invers_btn & plb2(s) && (set == 0 || set == 2)){
		NORMAL_CHAR();
		ch_to_move &= ~get_ch_mcx514(ch);
		invers_btn &= ~plb2(s);
		outP(flash_prg._OUTAxis[ch].vibor_axis, 0,0); //сбросить выход Выбор Оси
	}
	else if(set == 1 || set == 2){
		INVERT_CHAR();
		ch_to_move |= get_ch_mcx514(ch);
		invers_btn |= plb2(s);
		outP(flash_prg._OUTAxis[ch].vibor_axis, 1,0); //выставить выход Выбор Оси
		//==обновление скорости при выборе оси
		if(flash_prg._OUTAxis[ch].typeZ == CPrg::ANALOG){
			if(_modals_cmd.G0 == 0)
				_modals_cmd.F = flash_prg._Allp_param5[ch].start_G0;
			else
				_modals_cmd.F = flash_prg._Allp_param4[ch].start_G1;
			UPD_show_Instr = 1;		
		}
		else if(flash_prg._OUTAxis[ch].typeZ == CPrg::DISCRET){
			_modals_cmd.F = -1;
			UPD_show_Instr = 1;	
		}
		else if(flash_prg._OUTAxis[ch].typeZ == CPrg::IMPULSE){
		#if (configUSE_mcx514 == 1)
			_modals_cmd.F = flash_prg.axis_cfg_nc[ch].MaxSpd;
			UPD_show_Instr = 1;	
		#endif
		}		
	}
	if(s < 4)
		display.put_axis_shot(0,s * 7,Lir.Str[s]);	
	NORMAL_CHAR();
	if(s < 4){
		for(int s_ = 0; s_ < N_STR;s_++){
			if((Str[s_] == no_axis) || (invers_btn & plb2(s_)))
				continue;
			display.put_axis_shot(0,s_ * 7,Str[s_]);
		}
	}
	outP_all();
	entry_mh = IDLE_MH;
}

void Menu::set_mh()
{
	if(is_str_mh(MsgKey.key)){
		set_HAND_MOVE();
		change_pic_axis_mh(0);
		print_status_HAND_MOVE();		
		HM_toF = 0;
		dir_mh_sv = NO_DIR_MH;
		display.put_button_with_pic(X_BUTTON_PLACE,Y_ONE,_pic_HANDWEEL_OFF);
	}
}
/*
установка контекста Ручного перемещения
*/
void Menu::set_HAND_MOVE()
{
	MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);
	entry_hand = Menu::INIT;
	SET_C(idHandMove);
}

void Menu::print_status_HAND_MOVE()
{
	if(ch_to_move){
		status(PSTR("Ручное перемещение"));
		#if (configUSE_Shadow_axis == 1)
		if(ch_to_move & 0x10) // слепая ось
		display.put_char_local_independent(get_ASCII_from_channel(4),0,20,16 * 14);
		else
		display.put_char_local_independent(' ',0,20,16 * 14);
		#endif
	}
	else
		status(PSTR(""));
}

/*
функция вызывается по нажатию кнопок выбора оси _key_BLANK1,_key_BLANK2,_key_BLANK3,_key_BLANK4
*/
void Menu::change_pic_axis_mh(const unsigned char & page)
{
	if(curItem->id == idHandMove){
		if(extPULT.on)
			return;
		if(is_str_mh(MsgKey.key))
			set_axis_mh(page);		
	}
}

/*
сброс инверсного выделения оси
*/
void Menu::update_pic_axis_mh(const unsigned char & page,bool PORTOUT)
{
	//== сброс всего
	ch_to_move = 0;
	invers_btn = 0;//memset(invers_btn,0,N_STR);
	for(int c = 0 ; c < N_AXIS + N_SA;c++)
		outP(flash_prg._OUTAxis[c].vibor_axis, 0,PORTOUT); //сбросить выход Выбор Оси
	
	for(int s = 0; s < N_STR;s++){
		if(Str[s] == no_axis)
			continue;
		display.put_axis_shot(0,s * 7,Str[s]);		
	}	
	//==	
}


void Menu::stop_mh()
{
	if(curItem->id == idHandMove){
		MsgKey.SET_M(TMsgKey::MODE::HAND);
		SET_C(idHand);
		MAXO = 0;
		ch_to_move = 0;
		invers_btn = 0;//memset(invers_btn,0,N_STR);
		entry_mh = INIT_MH;
		dir_mh = NO_DIR_MH;
		//dir_mh_sv = NO_DIR_MH;
		btn_hold_mh = 0; //толчок
		btn_cont_mh = 0; //движение до отмены
		l_k_s.OFF_LED(_led_LEFT);
		l_k_s.OFF_LED(_led_RIGHT);
		Lir.status(PSTR(""));
		if (_modals_cmd.F < 0)
			_modals_cmd.F = _modals_cmd.F_sv;
		for(int i = 0 ; i < N_AXIS + N_SA;i++)
			outP(flash_prg._OUTAxis[i].vibor_axis, 0); //сбросить выход Выбор Оси
		outP_all();
		HM_toF = 0;		
	}
}
/*
переход из "Ручного перемещения" в режим FUNC
*/
void Menu::F_mh()
{
	//return;//(нужно протестировать)
	if(curItem->id == idHandMove){
		if(!INmove()){//если нет движения
			//сохраняем инвертированную ось
			for(int s = 0; s < N_STR;s++){
				if(invers_btn & plb2(s))
					strfunc_hand = s;
			}
			dir_mh_sv = dir_mh;
			stop_mh();//выход из режима ручное перемещение
			screen.update_icons_hand_mode();//входим в режим FUNC с инвертированной сохраненной осью
			invert_axis_in_modeFUNC(convStoKey(strfunc_hand));
			HM_toF = 1;			
		}
	}
}
/*
включение режима маховик Маховика
*/
void Menu::BLANK1_mh(const char& BLANK)
{
	if(curItem->id == idHandMove && BLANK == _key_BLANK1){
		if(!INmove()){//если нет движения
			if(MAXO == 0)				
				MAXO = 1;				
			else
				MAXO = 0;		
		}
	}
}

/*
переход из "Ручного перемещения" в режим Преднабора
*/
void Menu::BLANK2_mh(const char& BLANK)
{
	if(curItem->id == idHandMove && BLANK == _key_BLANK2){
		if(!INmove()){//если нет движения
			stop_mh();//выход из режима ручное перемещение
			MsgKey.tracer_next = TMsgKey::CMD_CNTRL;
			screen.update_icons_hand_mode();//входим в режим CMD_CNTRL
			invert_axis_in_modeFUNC(EMPTY_KEY);	//сброс инверсии осей
			Lir.init_hand_CMD_CTRL(0);//инициализация CMD_CTRL
			//==переходим в режим ввода
			MsgKey.SET_M(TMsgKey::MODE::INPUT_PARAM);
			Lir.menu_BLANK2_hand();
			//==
		}
	}
}

void Menu::BLANK3_mh(const char& BLANK)
{
	if(curItem->id == idHandMove && BLANK == _key_BLANK3){
		if(!INmove()){//если нет движения
			if(MAXO == 1){
				MAXO.mul /= 10;
				if(MAXO.mul == 0)
					MAXO.mul = 100;
				//print_long_format(maxo_mul,CPrg::FORMAT_END,0,36,16 * 8,0,0,2);
				MAXO.sync(); 
				if(MAXO.mul == 1)	
					display.put_button_with_pic(X_BUTTON_PLACE,Y_THREE,_pic_HWx1);	
				else if (MAXO.mul == 10)	
					display.put_button_with_pic(X_BUTTON_PLACE,Y_THREE,_pic_HWx10);
				else if (MAXO.mul == 100)
					display.put_button_with_pic(X_BUTTON_PLACE,Y_THREE,_pic_HWx100);	
			}	
		}
	}
}

void Menu::set_RIGHT_dir_mh()
{
	if(curItem->id == idHandMove){
		MAXO = 0;
		if(extPULT.on)
			return;		
		dir_mh = DIR_MH_RIGHT;
		l_k_s.ON_LED(_led_RIGHT);
		l_k_s.OFF_LED(_led_LEFT);
	}
}

void Menu::set_LEFT_dir_mh()
{
	if(curItem->id == idHandMove){
		MAXO = 0;
		if(extPULT.on)
			return;
		dir_mh = DIR_MH_LEFT;
		l_k_s.ON_LED(_led_LEFT);
		l_k_s.OFF_LED(_led_RIGHT);
	}
}

void Menu::move_axis_mh()
{
	if(curItem->id == idHandMove){
		if(extPULT.on)
			return;
		if(dir_mh && !btn_hold_mh && !btn_cont_mh){	
			if(Continius_drive_move(ch_to_move,_modals_cmd.G0,1))
				btn_hold_mh = 1; //ТОЛЧОК
		}
	}
}

void Menu::move_axis_mh_RELEASE()
{
	if(curItem->id == idHandMove){
		if(extPULT.on)
			return;
		tolchok_release();
	}
}

void Menu::key_UP_mh()
{
	if(curItem->id == idHandMove){
		Feed_UP();
	}
}

void Menu::key_DOWN_mh()
{
	if(curItem->id == idHandMove){
		Feed_DOWN();
	}
}

void Menu::tolchok_release()
{
	if(btn_hold_mh){		
		#if (configUSE_NCshell == 1)
		axis_drive_stop_lir();
		#endif
		#if (configUSE_max7370 == 0)
		set_auto_repeat_max7349(0x95);
		#endif
		btn_hold_mh = 0;
		upd_F_display();
	}
}

/*
начать продолжительное перемещения
*/
void Menu::move_axis_START()
{
	if(curItem->id == idHandMove){	
		if(extPULT.on)
			return;	
		if(dir_mh && !btn_cont_mh && !btn_hold_mh){
			if(Continius_drive_move(ch_to_move,_modals_cmd.G0,0))			
				btn_cont_mh = 1; 
		}		
	}
}
/*
остановить перемещение
возвращаемся в меню пуска команды
char sw_ch - каналы для стопа , по умолчанию все 0XFF
нужна ли при этом остановка шпинделя?
*/
void Menu::move_axis_STOP(char sw_ch)
{		
	if(INmove())
	{			
		#if (configUSE_NCshell == 1)		//outABCD() выставить выходы остановки
		bool rst = 0;
		if(btn_cont_mh || btn_hold_mh)
			rst = 0;
		else 
			rst = 1;
		axis_drive_stop_lir(rst,sw_ch);			
		#endif
		#if (configUSE_Shadow_axis == 0)
		if((btn_cont_mh || btn_hold_mh) && !sts_c[0] && !sts_c[1] && !sts_c[2] && !sts_c[3])
		#else
		if((btn_cont_mh || btn_hold_mh) && !sts_c[0] && !sts_c[1] && !sts_c[2] && !sts_c[3] && !sts_c[4])
		#endif
		{
			btn_cont_mh = 0;
			btn_hold_mh = 0;
		}else{
			//выход в инит если все оси не двигаются
			for(int c = 0 ; c < N_AXIS + N_SA; c++){
				if(sts_c[c])
					return;
			}	
			//
			entry_exe = INIT;
			entry_hand = INIT;
			}						
	}
}
/*
если мы находимся в процессе движения return 1
*/
bool Menu::INmove()
{
	if (entry_exe == WORKING_EXE || entry_exe == AUTO_EXECUTE_EXE || entry_exe == DELAY_FRM || entry_hand == WORKING_EXE ||
		btn_cont_mh || btn_hold_mh || entry_exe == DELAY_M5M3M4 || entry_hand == DELAY_M5M3M4 ||
		entry_exe == DELAY_WAIT_SHPINDEL || entry_hand == DELAY_WAIT_SHPINDEL || display.ifSearchRefAUTO()|| entry_exe == DELAY_INPUT_M)
		return true;
	else
		return false;
}
/*
если двигаемся только в режиме преднабора
*/
bool Menu::INmovePrednabor()
{
	if (entry_hand == WORKING_EXE || entry_hand == DELAY_M5M3M4 || entry_hand == DELAY_WAIT_SHPINDEL)
		return true;
	else
		return false;
}

/*
если мы находимся в контексте движения return 1
*/
bool Menu::INmoveContext()
{
	if((MsgKey.mode == TMsgKey::MODE::HAND && (MsgKey.tracer_real == TMsgKey::DRIVE_CONTROL /*|| MsgKey.tracer_real == TMsgKey::CMD_CNTRL*/)) ||
	(MsgKey.mode == TMsgKey::MODE::INPUT_PARAM && (Lir.curItem->id == idHandMove ||Lir.curItem->id == idExecmd)))
		return true;
	else
		return false;
}

bool Menu::INContiniusmove()
{
	if(btn_cont_mh || btn_hold_mh )
		return 1;
	else 
		return 0;
}

bool Menu::INMovingKeys()
{
	if(MsgKey.key == _key_STOP){
		//==пауза
		pause_G91();
		//==
		if(INmovePrednabor())
			stop_in_move = 1;
		else
			stop_in_move = 0;
		move_axis_STOP();
		outM5_delay();
		return 1;
		}
	if(MsgKey.key == _key_DOWN){
		Feed_DOWN();
		upd_Spd_inMove();
		}
	else if(MsgKey.key == _key_UP){
		Feed_UP();
		upd_Spd_inMove();
		}

	if(display.ifSearchRefAUTO())// во время поиска реф метки блокировка нажатий функций кнопок
		return 1;	

	if(INContiniusmove())// во время безразмерных передвижений блокировка нажатий функций кнопок
		return 1;
	if(MsgKey.key == _key_BLANK1)
		change_moving_icons(0);
	else if(MsgKey.key == _key_BLANK2)
		change_moving_icons(1);
	else if(MsgKey.key == _key_BLANK3)
		change_moving_icons(2);
	else if(MsgKey.key == _key_BLANK4)
		change_moving_icons(3);
	else if(MsgKey.key == _key_F)
		change_moving_functions();
	return 1;
}

/*
отключение светодиодов направление только если выбор направления безразмерно перемещения dir_mh равен 0
*/
void Menu::OFF_D_LEFT_RIGHT()
{
	if(dir_mh == NO_DIR_MH){
		l_k_s.OFF_LED(_led_RIGHT);
		l_k_s.OFF_LED(_led_LEFT);
	}
}

#if (configUSE_NCshell == 1)
/*
bool rst_vibor - если 1 то снятие выхода ВЫБОР ОСИ
char sw_ch - каналы для стопа, по умолчанию 0xFF (все каналы)
*/
void Menu::axis_drive_stop_lir(bool rst_vibor,char sw_ch)
{	
	OFF_D_LEFT_RIGHT();
	if(is_frame_move())
		_modals_cmd.F = _modals_cmd.F_sv;	
	for(int i = 0; i < N_AXIS + N_SA;i++){
		if(sts_c[i] && (sw_ch & plb2(i))){
			if(display.ifSearchRef(i)){
				rst_vibor = 1;				
				MsgKey.set_out_ref_mode = 1;			
				}
			
			if(flash_prg._OUTAxis[i].typeZ == CPrg::DISCRET)
				outABCD(get_STOP_spd(i), &flash_prg._OUTAxis[i].Out_A,i);
			else if(flash_prg._OUTAxis[i].typeZ == CPrg::ANALOG)
				outAnalog(Spd_to_U(get_STOP_spd(i),i,_modals_cmd.G0),flash_prg._OUTAxis2[i].AnalOut,flash_prg._OUTAxis2[i].Polar,dir_exe[i]);
			else if(flash_prg._OUTAxis[i].typeZ == CPrg::IMPULSE){
				#if (configUSE_mcx514 == 1)
				ExeDRVFBRK(plb2(i));
				#endif
			}
			
			if(rst_vibor)
				VIBOR_axis(i,0); //сброс выхода "Выбор Оси"
			STOP_axis(i);
			outP_all();
			sts_c[i] = 0;
			F_li[i] = 0;			
			if(i < N_AXIS){
				tmr_axis_pusk[i] = 0;
				tmr_ref_pusk[i] = 0;
			}
		}
	}
}
#endif
/*
Функция отключает или включает прерывания по UART
*/
void Menu::uart_on_off(const char &set)
{
	
}
/*
int axis - выбранные каналы для движения
char G0 - подача - 1(G1) , ускоренное 0(G0)
bool tolchok - 1 - готовимся к толчку , 0- готовимся к перемещению до отмены
*/
bool Menu::Continius_drive_move(int axis,char G0,bool tolchok)
{
//upd_F_display();
mPR.on = false;
memset(zn, 0x00,N_AXIS * ZONES);
memset(F_li,0x00,sizeof(long) * (N_AXIS + N_SA));
memset(cnt_z, 0x00, sizeof(long) * N_AXIS);
v_luft.init();
#if (configUSE_max7370 == 0)
	set_auto_repeat_max7349(0x90);
#endif	

#if (configUSE_NCshell == 1)
	if(if_ENC(axis))
		return 0;

	if(STOP_PODACHA && axis){
		status(PSTR("СТОП ПОДАЧА!"),300);
		return 0;
	}

	#if (configUSE_PLC == 1)
	if(Lir.lock_error_PLC){
		status(PSTR("КОНТРОЛЛЕР НЕ ОТВЕЧАЕТ!"),300);
		return 0;
	}
	#endif
	if(INSTRUMENT_ZAJAT == 0 && G0 == 1 && !tolchok){
		status(PSTR("ИНСТРУМЕНТ НЕ ЗАЖАТ!"),300);
		return 0;
	}
	if((SHPINDEL == 0 || (unlockSPH() && _modals_cmd.M3 == spSTOP)) && G0 == 1 && !tolchok){
		status(PSTR("ШПИНДЕЛЬ НЕ ВРАЩАЕТСЯ!"), 300);
		return 0;
	}
	//USART_DEBUG(SHPINDEL | G0 << 1 | tolchok << 2);
	outP_G0(G0);
	for (int i = 0; i < N_AXIS + N_SA; i++) {
		char ch = plb2(i);
		if (axis & ch) {	
			dir_exe[i] = 0;
			if(TORMOZ[i]){
				status(PSTR("ОСЬ НА ТОРМОЗЕ!"),300);
				continue;
			}

			if(dir_mh == DIR_MH_RIGHT && !(ch_sw.plus & ch) && !(ch_sw_prog.plus & ch))
				dir_exe[i] = 1;
			if(dir_mh == DIR_MH_LEFT && !(ch_sw.minus & ch) && !(ch_sw_prog.minus & ch))
				dir_exe[i] = -1;

			if(!dir_exe[i])
				continue;
			reset_circle_hw_couters(i);			
			sts_c[i] = 1;						
			if(flash_prg._OUTAxis[i].typeZ == CPrg::DISCRET)		
				outABCD(get_STRT_spd(G0,i), &flash_prg._OUTAxis[i].Out_A,i);			
			else if(flash_prg._OUTAxis[i].typeZ == CPrg::ANALOG)
				outAnalog(Spd_to_U(get_STRT_spd(G0,i),i,G0),flash_prg._OUTAxis2[i].AnalOut,flash_prg._OUTAxis2[i].Polar,dir_exe[i]);
			else if(flash_prg._OUTAxis[i].typeZ == CPrg::IMPULSE){
				#if (configUSE_mcx514 == 1)
				outMcx514_cont(ch,i,Spd_to_Hz(&_modals_cmd.F,i),acceleration(i),Spd_to_Hz(&flash_prg.axis_cfg_nc[i].InitSpd,i));
				#endif
				}
			//outP(flash_prg._OUTAxis[i].vibor_axis, 1); //выбор оси устанавливается заранее
			PUSK_axis(i);						
			outP_all();
		}
	}
	#if (configUSE_Shadow_axis == 0)
		if(sts_c[0] == 0 && sts_c[1] == 0 && sts_c[2] == 0 && sts_c[3] == 0)
			return 0;
	#else
		if(sts_c[0] == 0 && sts_c[1] == 0 && sts_c[2] == 0 && sts_c[3] == 0 && sts_c[4] == 0)
			return 0;
	#endif
		else 
			return 1;
#endif
}


/*
функция для преоразования остановки одной оси в остановку всех осей
*/
char Menu::sw_s(char sw_ch)
{
	if(is_frame_move())
		return 0xFF;
	else
		return sw_ch;
}

#endif

#if (configUSE_mcx514 == 1)

void Menu::actions_cntr(const unsigned char & page)
{
	if(entry_cntr == INIT_CNTR)
		init_m_cntr(page);
	if(entry_cntr == UPDATE_CNTR){
		draw_m_cntr(page);
		draw_param_cntr(page,38,RIGHT_ALIGN);
		}
	if(entry_cntr == IDLE_CNTR)
		return;
}

void Menu::actions_Cntrl_input(const unsigned char & page)
{
	int x_,y_;
	x_ = 12;
	y_ = menu_cntr_cnt * 16;

	x_ = strlen_P(sMoverCntrl[menu_cntr_cnt]) + 1;
	flashing_cursor(x_,y_,page);
	if (menu.ResultForInputNum == input::ok){

		if(display.strnlen_lir((const char*)bufASCIIstr)){	
			//==Задание на привод, имп/об==
			if(menu_cntr_cnt == 1){
				flash_prg.axis_cfg_nc[flash_prg.ch].imp_oborot = strtol_lir(bufASCIIstr);
			}
			//==
			//==Перемещение оси, мм/об
			if(menu_cntr_cnt == 2){
				long data;
				ConvertASCIIlinear_circle_tolong(data,CPrg::LINEAR_4,bufASCIIstr);
				flash_prg.axis_cfg_nc[flash_prg.ch].move_oborot = data;
			}
			//==
			//==Макс скорость, мм/мин
			if(menu_cntr_cnt == 3){
				long data;
				ConvertASCIIlinear_circle_tolong(data,CPrg::LINEAR_4,bufASCIIstr);
				flash_prg.axis_cfg_nc[flash_prg.ch].MaxSpd = data;
				if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::HANDWEEL){
					flash_prg.axis_cfg_nc[flash_prg.ch].InitSpd = flash_prg.axis_cfg_nc[flash_prg.ch].MaxSpd;
					flash_prg.axis_cfg_nc[flash_prg.ch].t = 0;
				}
			}
			//==
			//==Нач скорость, мм/мин
			if(menu_cntr_cnt == 4 && flash_prg.axis_cfg_nc[flash_prg.ch].datchik != CPrg::DATCHIK::HANDWEEL){
				long data;
				ConvertASCIIlinear_circle_tolong(data,CPrg::LINEAR_4,bufASCIIstr);
				flash_prg.axis_cfg_nc[flash_prg.ch].InitSpd = data;
			}
			//==
			//== Разгон/Тормоз ,сек
			if(menu_cntr_cnt == 5 && flash_prg.axis_cfg_nc[flash_prg.ch].datchik != CPrg::DATCHIK::HANDWEEL){
				long data;
				ConvertASCIIlinear_circle_tolong(data,CPrg::LINEAR_1,bufASCIIstr);
				flash_prg.axis_cfg_nc[flash_prg.ch].t = data;
			}
			//==
		}
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idCntrlopt);
		entry_cntr = UPDATE_CNTR;
		init_cursor_pos();
		
	}
	if (menu.ResultForInputNum == input::cancel_){
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idCntrlopt);
		entry_cntr = UPDATE_CNTR;
		init_cursor_pos();
	}
}

void Menu::init_m_cntr(const unsigned char & page)
{
	display.clear_ram_null();
	show_axis_above_menu2(13,0,1);
	entry_cntr = UPDATE_CNTR;	
}

void Menu::draw_m_cntr(const unsigned char & page)
{
	for(int i = 0; i < 9 ; i++){
		if(i == menu_cntr_cnt)
			INVERT_CHAR()
		else
			NORMAL_CHAR()
		display.print_flash_local(sMoverCntrl[i],page,0,16 * i); 
	}
	NORMAL_CHAR()

	if(menu_cntr_cnt == 6){
		if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::HANDWEEL)
			display.print_flash_local(PSTR("<ВВОД> настройка"),page,0,16 * 14);
		else
			display.print_symbols_local(' ',20,0,0,16 * 14);//display.print_flash_local(probel21,page,0,16 * 14);
	}
	else
		display.print_symbols_local(' ',20,0,0,16 * 14);//display.print_flash_local(probel21,page,0,16 * 14);


	entry_cntr = IDLE_CNTR;
}

void Menu::draw_param_cntr(const unsigned char & page,int x, char align)
{
	//==Задание на привод, имп/об===
	print_long_format(flash_prg.axis_cfg_nc[flash_prg.ch].imp_oborot,CPrg::FORMAT_END,page,x,16 * 1,align,0,10);
	//==

	//==Перемещение оси, мм/об==
	print_long_format(flash_prg.axis_cfg_nc[flash_prg.ch].move_oborot,CPrg::LINEAR_4,page,x,16 * 2,align,0,10);
	//==

	//==Макс скорость, мм/мин==
	print_long_format(flash_prg.axis_cfg_nc[flash_prg.ch].MaxSpd,CPrg::LINEAR_4,page,x,16 * 3,align,0,10,1);
	//==

	//==Нач скорость, мм/мин==
	if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::HANDWEEL)
		display.print_flash_local(PSTR("       нет"),page,x,16*4,align);
	else
		print_long_format(flash_prg.axis_cfg_nc[flash_prg.ch].InitSpd,CPrg::LINEAR_4,page,x,16 * 4,align,0,10,1);
	//==

	//==Разгон/Тормоз ,сек==
	if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::HANDWEEL)
		display.print_flash_local(PSTR("       нет"),page,x,16*5,align);
	else
		print_long_format(flash_prg.axis_cfg_nc[flash_prg.ch].t,CPrg::LINEAR_1,page,x,16 * 5,align,0,10);
	//==

	//==Контроль положения=====
	if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::NO_DAT)
		display.print_flash_local(PSTR("    нет"),page,x,16*6,align);
	else if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::ENCODER)
		display.print_flash_local(PSTR(" датчик"),page,x,16*6,align);
	else if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::HANDWEEL)
		display.print_flash_local(PSTR("маховик"),page,x,16*6,align);
	//==
	//==Направление,DIR==
	const char *p;
	if (flash_prg.axis_cfg_nc[flash_prg.ch].dir == CPrg::PLUS)
		p = PSTR("+");
	else
		p = PSTR("-");
	display.print_flash_local(p,0,x,16 * 7,align);
	//==

	//==Сигнал==
	if (flash_prg.axis_cfg_nc[flash_prg.ch].sig_type == CPrg::STEP_DIR)
		p = PSTR("Step/Dir");
	else if (flash_prg.axis_cfg_nc[flash_prg.ch].sig_type == CPrg::Quad_A_B)
		p = PSTR("    Quad");
	display.print_flash_local(p,0,x,16 * 8,align);
	//==
	
}

void Menu::m_up_cntr()
{
	if(curItem->id == idCntrlopt){
		menu_cntr_cnt--;
		if(menu_cntr_cnt <= 0)
			menu_cntr_cnt = 8;
		entry_cntr = UPDATE_CNTR;
	}
}

void Menu::m_down_cntr()
{
	if(curItem->id == idCntrlopt){
		menu_cntr_cnt++;
		if(menu_cntr_cnt > 8)
			menu_cntr_cnt = 1;
		entry_cntr = UPDATE_CNTR;
	}
}

void Menu::m_enter_cntr()
{
	if(curItem->id == idCntrlopt){
		if(menu_cntr_cnt == 6){
			if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::HANDWEEL){				
				SET_C(idHandWeelOpt);
				entry_cntr = INIT_CNTR;
			}
		}
		else{
			init_cursor_pos();
			SET_C(idInputCntrlOpt);
		}
	}
}

void Menu::m_stop_cntr()
{
	if(curItem->id == idCntrlopt){
		flash_prg.write_eeprom(1937,sizeof(flash_prg.axis_cfg_nc),&flash_prg.axis_cfg_nc);
		for(int c = 0; c < N_AXIS;c++)
			Set_Axis_Conf_mcx514(c);
		SET_C(idOutAxis);
		entry_cntr = INIT_CNTR;
	}
}

void Menu::m_left_cntr()
{
	if(curItem->id == idCntrlopt){
		if(menu_cntr_cnt == 6){
			flash_prg.axis_cfg_nc[flash_prg.ch].datchik--;
			if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::END)
				flash_prg.axis_cfg_nc[flash_prg.ch].datchik = CPrg::DATCHIK::NO_DAT;

			if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::HANDWEEL){
				//axis_cfg_nc2[flash_prg.which_channel_is_chosen_for_edit].OperSpd = axis_cfg_nc2[flash_prg.which_channel_is_chosen_for_edit].MaxSpd;
				flash_prg.axis_cfg_nc[flash_prg.ch].InitSpd = flash_prg.axis_cfg_nc[flash_prg.ch].MaxSpd;
				flash_prg.axis_cfg_nc[flash_prg.ch].t = 0;
			}
			
			entry_cntr = UPDATE_CNTR;
		}
		else if(menu_cntr_cnt == 7){
			flash_prg.axis_cfg_nc[flash_prg.ch].dir ^= 1;
			entry_cntr = UPDATE_CNTR;
		}
		else if(menu_cntr_cnt == 8){
			flash_prg.axis_cfg_nc[flash_prg.ch].sig_type--;
			if(flash_prg.axis_cfg_nc[flash_prg.ch].sig_type < CPrg::STEP_DIR)
				flash_prg.axis_cfg_nc[flash_prg.ch].sig_type = CPrg::Quad_A_B;
			entry_cntr = UPDATE_CNTR;
		}
	}
}

void Menu::m_right_cntr()
{
	if(curItem->id == idCntrlopt){
		if(menu_cntr_cnt == 6){
			flash_prg.axis_cfg_nc[flash_prg.ch].datchik++;
			if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::END)
				flash_prg.axis_cfg_nc[flash_prg.ch].datchik = CPrg::DATCHIK::ENCODER;

			if(flash_prg.axis_cfg_nc[flash_prg.ch].datchik == CPrg::DATCHIK::HANDWEEL){
				//axis_cfg_nc2[flash_prg.which_channel_is_chosen_for_edit].OperSpd = axis_cfg_nc2[flash_prg.which_channel_is_chosen_for_edit].MaxSpd;
				flash_prg.axis_cfg_nc[flash_prg.ch].InitSpd = flash_prg.axis_cfg_nc[flash_prg.ch].MaxSpd;
				flash_prg.axis_cfg_nc[flash_prg.ch].t = 0;
			}

			entry_cntr = UPDATE_CNTR;
		}
		else if(menu_cntr_cnt == 7){
			flash_prg.axis_cfg_nc[flash_prg.ch].dir ^= 1;
			entry_cntr = UPDATE_CNTR;
		}
		else if(menu_cntr_cnt == 8){
			flash_prg.axis_cfg_nc[flash_prg.ch].sig_type++;
			if(flash_prg.axis_cfg_nc[flash_prg.ch].sig_type > CPrg::Quad_A_B)
				flash_prg.axis_cfg_nc[flash_prg.ch].sig_type = CPrg::STEP_DIR;
			entry_cntr = UPDATE_CNTR;
		}
	}
}


void Menu::actions_cntr_hw(const unsigned char & page)
{
	if(entry_cntr_hw == INIT_CNTR)
		init_m_cntr_hw(page);
	if(entry_cntr_hw == UPDATE_CNTR){
		draw_m_cntr_hw(page);
		draw_param_cntr_hw(page,38,RIGHT_ALIGN);
	}
	if(entry_cntr_hw == IDLE_CNTR)
		return;
}

void Menu::actions_Cntrl_input_hw(const unsigned char & page)
{
	int x_,y_;
	x_ = 12;
	y_ = menu_cntr_cnt_hw * 16;

	x_ = strlen_P(sHandWeelCntrl[menu_cntr_cnt_hw]) + 1;
	flashing_cursor(x_,y_,page);
	if (menu.ResultForInputNum == input::ok){
		if(display.strnlen_lir((const char*)bufASCIIstr)){
			if(menu_cntr_cnt_hw == 1){
				flash_prg.axis_cfg_nc2[flash_prg.ch].hw_oborot_imp = strtol_lir(bufASCIIstr);
			}
			if(menu_cntr_cnt_hw == 2){
				long data;
				ConvertASCIIlinear_circle_tolong(data,CPrg::LINEAR_4,bufASCIIstr);
				flash_prg.axis_cfg_nc2[flash_prg.ch].hw_oborot_mm = data;
			}
			if(menu_cntr_cnt_hw == 3){
				long data;
				ConvertASCIIlinear_circle_tolong(data,CPrg::LINEAR_2,bufASCIIstr);
				flash_prg.axis_cfg_nc2[flash_prg.ch].F_amplifire = data;
			}
		}
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idHandWeelOpt);
		entry_cntr_hw = UPDATE_CNTR;
		init_cursor_pos();
		
	}
	if (menu.ResultForInputNum == input::cancel_){
		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idHandWeelOpt);
		entry_cntr_hw = UPDATE_CNTR;
		init_cursor_pos();
	}
}

void Menu::init_m_cntr_hw(const unsigned char & page)
{
	display.clear_ram_null();
	show_axis_above_menu2(13);
	entry_cntr_hw = UPDATE_CNTR;
}

void Menu::draw_m_cntr_hw(const unsigned char & page)
{
	for(int i = 0; i < 4 ; i++){
		if(i == menu_cntr_cnt_hw)
			INVERT_CHAR()
		else
			NORMAL_CHAR()

		display.print_flash_local(sHandWeelCntrl[i],page,0,16 * i);
	}
	NORMAL_CHAR()
	entry_cntr_hw = IDLE_CNTR;
}

void Menu::draw_param_cntr_hw(const unsigned char & page,int x,char align)
{
	//==Разрешение, имп/об==
	Lir.print_long_format(flash_prg.axis_cfg_nc2[flash_prg.ch].hw_oborot_imp,
	CPrg::FORMAT_END,page,x,16*1,align,0,10);
	//==

	//==Перемещение оси, мм/об ==
	Lir.print_long_format(flash_prg.axis_cfg_nc2[flash_prg.ch].hw_oborot_mm,
	CPrg::LINEAR_4,page,x,16 * 2,align,0,10);
	//==

	//==Коэфф усиления ==
	Lir.print_long_format(flash_prg.axis_cfg_nc2[flash_prg.ch].F_amplifire,
	CPrg::LINEAR_2,page,x,16 * 3,align,0,10);
	//==
}

void Menu::m_up_cntr_hw()
{
	if(curItem->id == idHandWeelOpt){
		menu_cntr_cnt_hw--;
		if(menu_cntr_cnt_hw <= 0)
			menu_cntr_cnt_hw = 3;
		entry_cntr_hw = UPDATE_CNTR;
	}
}

void Menu::m_down_cntr_hw()
{
	if(curItem->id == idHandWeelOpt){
		menu_cntr_cnt_hw++;
		if(menu_cntr_cnt_hw > 3)
			menu_cntr_cnt_hw = 1;
		entry_cntr_hw = UPDATE_CNTR;
	}
}

void Menu::m_enter_cntr_hw()
{
	if(curItem->id == idHandWeelOpt){
		init_cursor_pos();
		SET_C(idInputHandWeelOpt);
	}
}

void Menu::m_stop_cntr_hw()
{
	if(curItem->id == idHandWeelOpt){
		flash_prg.write_eeprom(2069,sizeof(flash_prg.axis_cfg_nc2),&flash_prg.axis_cfg_nc2);//сохранения параметров нету
		SET_C(idCntrlopt);
		entry_cntr_hw = INIT_CNTR;
	}
}

#endif
void Menu::load_LH_CH_setup_from_eeprom(unsigned char sz /*= sizeof(LH_CH_setup)*/)
{
	int start_EEPROM = 186;
	bool error = false;
	unsigned char *p = reinterpret_cast<unsigned char*>(LH_CH_setup);
	for(int i = 0 ; i < sz; i ++){
		p[i] = flash_prg.read_one_byte_eeprom_address_int(start_EEPROM + i,&error);
	}
}

void Menu::save_LH_CH_setup_to_eeprom(unsigned char sz /*= sizeof(LH_CH_setup)*/)
{
	int start_EEPROM = 186;
	for(int i = 0 ; i < 2; i++){
		flash_prg.write_block_to_eeprom_address_int(start_EEPROM + (i * sizeof(LH_CH_setup[i])),reinterpret_cast<unsigned char*>(&LH_CH_setup[i]),sizeof(LH_CH_setup[i]));
	}
}
//#pragma GCC pop_options
#if (configUSE_Linear_holes == 1)


void Menu::Init_LH(const unsigned char &page)
{
	display.clear_ram_null();
	init_cursor_pos();
	//menu_lh = 1;
	check_axis_ch(primary_axis_lh);
	check_axis_ch(secondary_axis_lh);

	draw_menu_lh(page);
	draw_param_lh(page);

	display.put_button_(X_BUTTON_PLACE + 2,Y_FOUR);
	display.print_flash_local(sPusk, page, 35, 178);

	entry_lh = IDLE_CH;
}

void Menu::draw_menu_lh(const unsigned char &page)
{
	const char *menu_table[12];
	menu_table[0] = s00_lh;
	menu_table[1] = s01_lh;
	menu_table[2] = s02_lh;
	menu_table[3] = nullptr;
	menu_table[4] = nullptr;
	menu_table[5] = PSTR(" -----------------");
	menu_table[6] = s03_lh;
	menu_table[7] = s04_lh;
	menu_table[8] = s05_lh;
	menu_table[9] = s06_lh;
	menu_table[10] = s07_lh;
	menu_table[11] = s08_lh;

	for(uint8_t i = 0; i < 12;i++)
	{
		if( i == menu_lh || i == 0)
			INVERT_CHAR()
		else
			NORMAL_CHAR()

		if( i == 3 ){
			display.print_flash_local(sAxis_screen[primary_axis_lh],page,1,48);
			display.print_symbols_local(' ',12,page,6,48);

			if(primary_axis_lh != no_axis)
				display.put_char_local_independent(0x80,page,2,48);
			else
				display.put_char_local_independent(0x80,page,1,48);

			if(primary_axis_lh != no_axis)
				Lir.print_long_format(primary_first_point_lh,get_format(primary_axis_lh),page,17,48,RIGHT_ALIGN);
		}
		else if( i == 4 ){
			display.print_flash_local(sAxis_screen[secondary_axis_lh],page,1,64);
			display.print_symbols_local(' ',12,page,6,64);
			if(secondary_axis_lh != no_axis)
				Lir.print_long_format(secondary_first_point_lh,get_format(secondary_axis_lh),page,17,64,RIGHT_ALIGN);
		}
		else
			display.print_flash_local(menu_table[i],page,0,i * 16);
	}
	NORMAL_CHAR()
	display.put_char_local_independent(0x80,page,2,16*14);
	display.print_flash_local(sOpornaOsb,page,3,16*14);
	entry_lh = IDLE_CH;
}

void Menu::draw_param_lh(const unsigned char &page)
{
	const char *p;
	//==Тип==
	if(type_lh)
		display.print_flash_local(sReshetka,page,32,16,RIGHT_ALIGN);
	else
		display.print_flash_local(PSTR("  рамка"),page,32,16,RIGHT_ALIGN);
	//==

	//==1ое отверстие==
	//==Оси==
	//Оси и 1ое отверстие рисуются в draw_menu
	//

	//==Отверстий в ряду==
	Lir.print_long_format(holes_lh,CPrg::FORMAT::FORMAT_END,page,32,16 * 6,RIGHT_ALIGN,1);
	//==

	// ==Интервал отверстий==
	Lir.print_long_format(interval_holes,CPrg::FORMAT::LINEAR_4,page,32,16 * 7,RIGHT_ALIGN);
	//==

	// ==Рядов ===
	Lir.print_long_format(rows_lh,CPrg::FORMAT::FORMAT_END,page,32,16 * 8,RIGHT_ALIGN,1);
	//==

	// ==Интервал рядов ===
	Lir.print_long_format(interval_rows,CPrg::FORMAT::LINEAR_4,page,32,16 * 9,RIGHT_ALIGN);
	//==

	// ==Угол==
	long tmp;
	tmp = ang_lh * 10000;
	Lir.print_long_format(tmp,CPrg::FORMAT::CIRCLE_4,page,33,16 * 10,RIGHT_ALIGN);
	tmp = 0;
	//==

	//==Звук==
	if(!LH_CH_setup[1].sound)
		p = sYES;//display.print_flash_local(PSTR(" да"),page,32,16 * 11,RIGHT_ALIGN);
	else
		p = sNO;//display.print_flash_local(PSTR("нет"),page,32,16 * 11,RIGHT_ALIGN);
	display.print_flash_local(p,page,32,16 * 11,RIGHT_ALIGN);
	//==
	
	entry_lh = IDLE_CH;
}

void Menu::menu_lh_up()
{
	if(curItem->id == idInpointLinear)
	{
		menu_lh--;
		if(menu_lh == 2 || menu_lh == 5)
		menu_lh--;

		if(menu_lh == 0 )
		menu_lh  = 12 - 1;

		entry_lh = CHANGE_FOCUS_MENU_CH;
	}
}

void Menu::menu_lh_down()
{
	if(curItem->id == idInpointLinear)
	{
		menu_lh++;

		if(menu_lh == 2 || menu_lh == 5)
		menu_lh++;

		if(menu_lh == 12 )
		menu_lh = 1;

		entry_lh = CHANGE_FOCUS_MENU_CH;
	}
}

void Menu::action_Linear_holes()
{
	if(entry_lh == INIT_CH){
		Init_LH(0);
	}

	if(entry_lh == CHANGE_FOCUS_MENU_CH){
		draw_menu_lh(0);
	}
	if(entry_lh == CHANGE_PARAM_CH){
		draw_param_lh(0);
	}

	if(entry_lh == IDLE_CH){
		return;
	}
}
/*
функция вызывается по нажатию кнопки либо влево(_key_LEFT) либо вправо (_key_RIGHT)
const char &r_l - переменная определяющая направление '-'<- или ->'+'
*/
void Menu::change_param_lh_left_right(const char &r_l)
{
	if(curItem->id == idInpointLinear)
	{
		if(menu_lh == 1){
			if(type_lh)
				type_lh = 1;
			else
				type_lh = 1;
			entry_lh = CHANGE_PARAM_CH;
		}

		if(menu_lh == 3){
			change_axis_ch(primary_axis_lh,secondary_axis_lh,r_l);	
			flash_prg.to_save_primary_axis_lh_to_EEPROM((uint8_t*)&primary_axis_lh);		
			entry_lh = CHANGE_FOCUS_MENU_CH;
		}

		if(menu_lh == 4){
			change_axis_ch(secondary_axis_lh,primary_axis_lh,r_l);
			flash_prg.to_save_secondary_axis_lh_to_EEPROM((uint8_t*)&secondary_axis_lh);
			entry_lh = CHANGE_FOCUS_MENU_CH;
		}
		if(menu_lh == 11){
			if(!LH_CH_setup[1].sound)
				LH_CH_setup[1].sound = 1;
			else
				LH_CH_setup[1].sound = 0;
			save_LH_CH_setup_to_eeprom();
			entry_lh = CHANGE_PARAM_CH;
		}
	}
}

void Menu::enter_to_input_loop_lh()
{
	if(curItem->id == idInpointLinear)
	{
		if(menu_lh == 3 && primary_axis_lh != CInit::NO_AXIS)  // 1ое отверстие главная ось
			SET_C(idInpointLinearInput);
		if(menu_lh == 4 && secondary_axis_lh != CInit::NO_AXIS) // 1ое отверстие вторичная ось
			SET_C(idInpointLinearInput);
		if(menu_lh == 6) //Отверстий в ряду
			SET_C(idInpointLinearInput);
		if(menu_lh == 7) //Интервал отверстий
			SET_C(idInpointLinearInput);
		if(menu_lh == 8) //Угол
			SET_C(idInpointLinearInput);
		if(menu_lh == 9) //Рядов
			SET_C(idInpointLinearInput);
		if(menu_lh == 10) //Интервал Рядов
			SET_C(idInpointLinearInput);
	}
}

void Menu::action_input_linear_input(const unsigned char &page)
{
	static unsigned char x_,y_;

	if(menu_lh == 3)
		x_ = 21;
	if(menu_lh == 4)
		x_ = 21;
	if(menu_lh == 6)
		x_ = 17;
	if(menu_lh == 7)
		x_ = 17;
	if(menu_lh == 8)
		x_ = 17;
	if(menu_lh == 9)
		x_ = 17;
	if(menu_lh == 10)
		x_ = 17;

	y_ = menu_lh * 16;
	
	Lir.flashing_cursor(x_, y_, page);

	if (menu.ResultForInputNum == input::ok || menu.ResultForInputNum == input::cancel_ )
	{
		if (menu.ResultForInputNum == input::ok && (mystr.strnlen_lir((char*)menu.bufASCIIstr, IN_BUF_SZ) > 0))
		{
			long result;
			if(menu_lh == 3){
				primary_first_point_lh = String_to_bin(menu.bufASCIIstr,get_channel(primary_axis_lh));
			}
			if(menu_lh == 4){
				secondary_first_point_lh = String_to_bin(menu.bufASCIIstr,get_channel(secondary_axis_lh));
			}
			if(menu_lh == 6){
				result = strtol_lir(menu.bufASCIIstr);
				if(result < 0)
					result *= -1;
				
				if(result > 99)result = 99;
				if(result < 1) result = 1;
				holes_lh = (int)result;
			}
			if(menu_lh == 7){
				Lir.ConvertASCIIlinear_circle_tolong(result,CPrg::FORMAT::LINEAR_4,menu.bufASCIIstr);
				interval_holes = result;
			}
			if(menu_lh == 10){
				ang_lh = atof((char*)menu.bufASCIIstr);

				if(ang_lh >= 0)
					ang_lh +=0.00005;
				else
					ang_lh -=0.00005;

				if(ang_lh > 279)
					ang_lh = 279;
				if(ang_lh < -279)
					ang_lh = -279;
			}
			if(menu_lh == 8){
				result = strtol_lir(menu.bufASCIIstr);
				if(result < 0)
					result *= -1;
				
				if(result > 99)result = 99;
				if(result < 1) result = 1;
				rows_lh = (int)result;
			}
			if(menu_lh == 9){
				Lir.ConvertASCIIlinear_circle_tolong(result,CPrg::FORMAT::LINEAR_4,menu.bufASCIIstr);
				interval_rows = result;
			}
		}



		if(menu_lh == 3)
			entry_lh = CHANGE_FOCUS_MENU_CH;
		if(menu_lh == 4)
			entry_lh = CHANGE_FOCUS_MENU_CH;
		if(menu_lh == 6)
			entry_lh = CHANGE_PARAM_CH;
		if(menu_lh == 7)
			entry_lh = CHANGE_PARAM_CH;
		if(menu_lh == 8)
			entry_lh = CHANGE_PARAM_CH;
		if(menu_lh == 9)
			entry_lh = CHANGE_PARAM_CH;
		if(menu_lh == 10)
			entry_lh = CHANGE_PARAM_CH;

		display.print_flash_local(sProbel10, page, x_, y_);
		SET_C(idInpointLinear);
		init_cursor_pos();
	}
}

void Menu::menu_lh_blank(const char& BLANK)
{
	if(curItem->id == idInpointLinear)
	{
		if(BLANK == _key_BLANK4)
		{
			if(primary_axis_lh != no_axis && secondary_axis_lh != no_axis && holes_lh > 0 && rows_lh > 0){

				if(holes_lh > 1 && !interval_holes)
						return;
				if(rows_lh > 1 && !interval_rows)
						return;				
				SET_C(idInpointLinearPUSK);
				entry_lh = PUSK_CH;
			}
			return;
		}
	}

	if(curItem->id == idInpointLinearPUSK){

		if(BLANK == _key_BLANK2){
			count_holes_lh++;
			if(count_holes_lh > (holes_lh * rows_lh))
			count_holes_lh = 1;

			dir_lh_btn = 0;
			entry_lh = LOAD_COORD_HOLE_CH;
			return;
		}
		if(BLANK == _key_BLANK3){
			count_holes_lh--;
			if(count_holes_lh == 0)
			count_holes_lh = holes_lh * rows_lh;

			dir_lh_btn = 1;
			entry_lh = LOAD_COORD_HOLE_CH;
			return;
		}
		if(BLANK == _key_BLANK4)
		{
			exit_lh_PUSK();
			//curItem = &mInpointLinear;
			//entry_lh = INIT_CH;
			return;
		}
	}
}

bool Menu::exit_lh_PUSK()
{
	if(curItem->id == idInpointLinearPUSK){
		SET_C(idInpointLinear);
		entry_lh = INIT_CH;
		return 1;
	}
	return 0;
}

void Menu::action_input_linear_PUSK(const unsigned char &page)
{
	if(entry_lh == PUSK_CH)
	{
		display.clear_ram_null(page);
		display.put_button_(X_BUTTON_PLACE + 1,Y_ONE);
		display.put_button_(X_BUTTON_PLACE + 1,Y_TWO);
		display.put_button_(X_BUTTON_PLACE + 1,Y_THREE);
		display.put_button_(X_BUTTON_PLACE + 1,Y_FOUR);

		display.print_flash_local(sSled, page, 35, 70);
		display.print_flash_local(sPred, page, 35, 126);
		display.print_flash_local(sSTOP, page, 35, 178);

		count_holes_lh = 1;
		x_old_lh = 0;
		y_old_lh = 0;
		reset_counters_rel();
		goto_Relative_coord(primary_axis_lh,secondary_axis_lh);
		entry_lh = LOAD_COORD_HOLE_CH;		
	}

	if(entry_lh == LOAD_COORD_HOLE_CH){
		print_long_format(count_holes_lh,CPrg::FORMAT::FORMAT_END,page,38,12,RIGHT_ALIGN,3);// выбранное отверстие
		getcoord_inlinear_one();
		entry_lh = IDLE_CH;
	}

	if(entry_lh == IDLE_CH){
		if(!LH_CH_setup[1].sound)
			sound_holes();
		return;
	}
}

void Menu::getcoord_inlinear_one()
{
		long x;                    // координата x;
		long y;                    // координата y;
		long counter[N_AXIS] = {0};

		//=====какие каналы
		int ch1 = get_channel(primary_axis_lh);
		int ch2 = get_channel(secondary_axis_lh);

		get_counters_rel(counter);
		coeff.mult(ch1,&counter[ch1]);
		coeff.mult(ch2,&counter[ch2]);
		//===

		int channel_1 = 1;
		int channel_2 = 1;

		if (flash_prg.axis_cfg[ch1].format == CPrg::FORMAT::LINEAR_1 )      {channel_1 = 1000;}
		if (flash_prg.axis_cfg[ch1].format == CPrg::FORMAT::LINEAR_2 )      {channel_1 = 100;}
		if (flash_prg.axis_cfg[ch1].format == CPrg::FORMAT::LINEAR_3 )      {channel_1 = 10;}

		if (flash_prg.axis_cfg[ch2].format == CPrg::FORMAT::LINEAR_1 )      {channel_2 = 1000;}
		if (flash_prg.axis_cfg[ch2].format == CPrg::FORMAT::LINEAR_2 )      {channel_2 = 100;}
		if (flash_prg.axis_cfg[ch2].format == CPrg::FORMAT::LINEAR_3 )      {channel_2 = 10;}
		
//===логика
		bool t;
		if (dir_lh_btn)
			t = count_holes_lh % holes_lh;
		if (!dir_lh_btn)
			t = (count_holes_lh - 1) % holes_lh;

		if ((count_holes_lh == (holes_lh + !dir_lh_btn))  || (count_holes_lh == (holes_lh * rows_lh)) || !t)
		{
			if (count_holes_lh == (holes_lh + !dir_lh_btn) && rows_lh > 1) {
				horizon = 0;
				if (!dir_lh_btn)
					row_step += interval_rows;
				else
					row_step -= interval_rows;
				dir_lh = !dir_lh;
			}
			else if (count_holes_lh == (holes_lh * rows_lh)) {
				horizon = 0;
				row_step = interval_rows * (rows_lh - 1);

				if (rows_lh & 1 )
				interval_step = interval_holes * (holes_lh - 1);
				else {
					interval_step = 0;
					dir_lh = !dir_lh;
				}
			}
			else if(!t && count_holes_lh != 1 ) {
				if (dir_lh_btn) {
					horizon = 0;
					row_step -= interval_rows;
					dir_lh = !dir_lh;
				}
				if (!dir_lh_btn) {
					horizon = 0;
					row_step += interval_rows;
					dir_lh = !dir_lh;
				}
			}

		}
		else
		horizon = 1;


		if (count_holes_lh > 1 && horizon) {

			if (dir_lh) {
				if (!dir_lh_btn)
				interval_step += interval_holes;
				else
				interval_step -= interval_holes;
			}
			if (!dir_lh) {
				if (!dir_lh_btn)
				interval_step -= interval_holes;
				else
				interval_step += interval_holes;
			}
		}

		if (count_holes_lh == 1) {
			interval_step = 0;
			row_step = 0;
			if (!dir_lh)
			dir_lh = !dir_lh;
		}


		x = interval_step/channel_1;
		y = row_step/channel_2;
		
		if (ang_lh) {
			long x_res,y_res;
			x_res = lround((x * cos(M_PI*ang_lh / 180)) - y * sin(M_PI*ang_lh / 180));
			y_res = lround((x * sin(M_PI*ang_lh / 180)) + y * cos(M_PI*ang_lh / 180));
			x = x_res;
			y = y_res;
		}

		//if(flash_prg.axis_cfg[ch1].direction == CPrg::DIRECTION::PLUS)
			counter[ch1] = -x + x_old_lh + counter[ch1] - primary_first_point_lh;
		//if(flash_prg.axis_cfg[ch1].direction == CPrg::DIRECTION::MINES)
		//	counter[ch1] = x - x_old_lh + counter[ch1] + primary_first_point_lh;

		//if(flash_prg.axis_cfg[ch2].direction == CPrg::DIRECTION::PLUS)
			counter[ch2] = -y + y_old_lh + counter[ch2] - secondary_first_point_lh;
		//if(flash_prg.axis_cfg[ch2].direction == CPrg::DIRECTION::MINES)
		//	counter[ch2] = y - y_old_lh + counter[ch2] + secondary_first_point_lh;

		x_old_lh = x + primary_first_point_lh;
		y_old_lh = y + secondary_first_point_lh;

		coeff.div(ch1,&counter[ch1]);
		coeff.div(ch2,&counter[ch2]);
		set_counters_rel(counter);
}

void Menu::drive_next_hole_lh()
{
	int ch1 = Lir.get_channel(Lir.primary_axis_lh);
	int ch2 = Lir.get_channel(Lir.secondary_axis_lh);

	SetStartSpd(MCX514_AXIS_U, 10);	// Initial speed 1pps
	SetSpeed(MCX514_AXIS_U, 200);	// Drive speed 10ps
	SetAcc(MCX514_AXIS_U, 1000000);	// Acceleration (maximum in specification)
	SetJerk(MCX514_AXIS_U, 1000000);	// Jerk 1pps/sec2
	SetPulse(MCX514_AXIS_U, mCounter[ch1].rel);	// Drive pulse number 500
	SetLp(MCX514_AXIS_U, 0);		// Logical position counter  Clear
	//WriteReg3(MCX514_AXIS_U, 0x0004);	// Specifies S-curve acceleration/deceleration driving
	ExeDRVRL(MCX514_AXIS_U);		// Relative position driving
	//ExeDRVVM(MCX514_AXIS_U);	        // Continuous position driving minus
	
	//delay_ms(200);

	SetStartSpd(MCX514_AXIS_Z, 10);	// Initial speed 1pps
	SetSpeed(MCX514_AXIS_Z, 200);	// Drive speed 10ps
	SetAcc(MCX514_AXIS_Z, 1000000);	// Acceleration (maximum in specification)
	SetJerk(MCX514_AXIS_Z, 1000000);	// Jerk 1pps/sec2
	SetPulse(MCX514_AXIS_Z, mCounter[ch2].rel);	// Drive pulse number 500
	SetLp(MCX514_AXIS_Z, 0);		// Logical position counter  Clear
	//WriteReg3(MCX514_AXIS_U, 0x0004);	// Specifies S-curve acceleration/deceleration driving
	ExeDRVRL(MCX514_AXIS_Z);		// Relative position driving
	//ExeDRVVM(MCX514_AXIS_Z);	        // Continuous position driving minus
	
	//delay_ms(200);
}

#endif
#if (configUSE_Circle_holes == 1)

void Menu::getcoord_incircle_all(int number_of_holes,int radius,int start_angel)
{
	int x[10];              // координата x;
	int y[10];              // координата y;
	int R = radius;         // радиус мм
	int ang = start_angel;  // угол
	int  n;                 // кол - во отверстий

	n = 360 / number_of_holes;

	for (int i = 0; i < number_of_holes; i++)
	{
		x[i] = R * cos(M_PI*ang / 180) * 10000;
		y[i] = R * sin(M_PI*ang / 180) * 10000;
		ang = ang + n;
	}
	
	display.print_debug_one_local(x[0],0,10,16);
	display.print_debug_one_local(y[0],0,10,32);
}

void Menu::getcoord_incircle_one()
{		//n,R,ang,count_holes
	long x;                    // координата x;
	long y;                    // координата y;
	long counter[N_AXIS] = {0};

	//=====какие каналы
	int ch1 = get_channel(primary_axis_ch);
	int ch2 = get_channel(secondary_axis_ch);

	get_counters_rel(counter);
	coeff.mult(ch1,&counter[ch1]);
	coeff.mult(ch2,&counter[ch2]);
	//===
	int channel_1 = 1;
	int channel_2 = 1;

	if (flash_prg.axis_cfg[ch1].format == CPrg::FORMAT::LINEAR_1 )      {channel_1 = 1000;}
	if (flash_prg.axis_cfg[ch1].format == CPrg::FORMAT::LINEAR_2 )      {channel_1 = 100;}
	if (flash_prg.axis_cfg[ch1].format == CPrg::FORMAT::LINEAR_3 )      {channel_1 = 10;}
	if (flash_prg.axis_cfg[ch1].format == CPrg::FORMAT::LINEAR_4 )	    {channel_1 = 1;}
	if (flash_prg.axis_cfg[ch1].format == CPrg::FORMAT::LINEAR_INCH_4 ) {channel_1 = 1;}

	if (flash_prg.axis_cfg[ch2].format == CPrg::FORMAT::LINEAR_1 )      {channel_2 = 1000;}
	if (flash_prg.axis_cfg[ch2].format == CPrg::FORMAT::LINEAR_2 )      {channel_2 = 100;}
	if (flash_prg.axis_cfg[ch2].format == CPrg::FORMAT::LINEAR_3 )      {channel_2 = 10;}
	if (flash_prg.axis_cfg[ch2].format == CPrg::FORMAT::LINEAR_4 )	    {channel_2 = 1;}
	if (flash_prg.axis_cfg[ch2].format == CPrg::FORMAT::LINEAR_INCH_4 ) {channel_2 = 1;}
	
	cur_ang = start_ang + ( step_ang * (count_holes - 1));

	x = lround(cos(M_PI*cur_ang / 180) * (R / channel_1));
	y = lround(sin(M_PI*cur_ang / 180) * (R / channel_2));

	//if(flash_prg.axis_cfg[ch1].direction == CPrg::DIRECTION::PLUS)
		counter[ch1] = -x + x_old + counter[ch1] - primary_centr_ch;
	//if(flash_prg.axis_cfg[ch1].direction == CPrg::DIRECTION::MINES)
		//counter[ch1] = x - x_old + counter[ch1] + primary_centr_ch;

	//if(flash_prg.axis_cfg[ch2].direction == CPrg::DIRECTION::PLUS)
		counter[ch2] = -y + y_old + counter[ch2] - secondary_centr_ch;
	//if(flash_prg.axis_cfg[ch2].direction == CPrg::DIRECTION::MINES)
		//counter[ch2] = y - y_old + counter[ch2] + secondary_centr_ch;

	x_old = x + primary_centr_ch;
	y_old = y + secondary_centr_ch;

	coeff.div(ch1,&counter[ch1]);
	coeff.div(ch2,&counter[ch2]);
	set_counters_rel(counter);

}

void Menu::sound_holes()
{
	static bool lock;
	//==какие каналы
	int8_t ch1 = get_channel(primary_axis_ch);
	int8_t ch2 = get_channel(secondary_axis_ch);
	//==
	if(ch1 < N_AXIS && ch2 < N_AXIS){
		if(mCounter[ch1].rel == 0 && mCounter[ch2].rel == 0){
			if(!lock){
				peak_sound_max7370(50);
				lock = 1;
			}
		}
		else
			lock = 0;
	}
}

void Menu::action_input_radius()
{		
	if(entry == INIT_CH){
	Init_CH(0);
	}

	if(entry == CHANGE_FOCUS_MENU_CH){
	draw_menu_ch(0);
	}
	if(entry == CHANGE_PARAM_CH){
	draw_param_ch(0);
	}

	if(entry == IDLE_CH){
		return;
	}
}

void Menu::action_input_radius_input(const unsigned char &page)
{
	static unsigned char x_,y_;

	if(menu_ch == 2)
	{
		x_ = 20;
		y_ = 32;
	}
	if(menu_ch == 4)
	{
		x_ = 21;
		y_ = 64;
	}
	if(menu_ch == 5)
	{
		x_ = 21;
		y_ = 80;
	}
	if(menu_ch == 7)
	{
		x_ = 11;
		y_ = 112;
	}
	if(menu_ch == 8)
	{
		x_ = 16;
		y_ = 128;
	}
	if(menu_ch == 9)
	{
		x_ = 11;
		y_ = 144;
	}

		Lir.flashing_cursor(x_, y_, page);

	if (menu.ResultForInputNum == input::ok)
	{
		display.put_char_local_independent(' ',page,x_,y_);	

		if (mystr.strnlen_lir((char*)menu.bufASCIIstr, IN_BUF_SZ) > 0)
		{	
			display.print_flash_local(sProbel21, page, x_, y_);
			long result;			
				if(menu_ch == 2){
						result = strtol_lir(menu.bufASCIIstr);
						if(result < 0)
							result *= -1;
						
						if(result > 99)result = 99;
						if(result < 1) result = 1;
						n = (int)result;
						step_ang = ((float)360 / n) + 0.00005;

						if(type_ch) // если выбран сегмент ,то автоматического рассчета шага угла нету
							step_ang = 0;

						entry = CHANGE_PARAM_CH;
						}
				if(menu_ch == 4){
					result = String_to_bin(menu.bufASCIIstr,get_channel(primary_axis_ch));
					primary_centr_ch = result;
					entry = CHANGE_FOCUS_MENU_CH;
				}
				if(menu_ch == 5){
					result = String_to_bin(menu.bufASCIIstr,get_channel(secondary_axis_ch));
					secondary_centr_ch = result;
					entry = CHANGE_FOCUS_MENU_CH;
				}
				if(menu_ch == 7 ){
					Lir.ConvertASCIIlinear_circle_tolong(result,CPrg::FORMAT::LINEAR_4,menu.bufASCIIstr);
					R = result;
					if(R < 0)
						R *= -1;
					entry = CHANGE_PARAM_CH;
				}
				if(menu_ch == 8 ){
					start_ang = atof((char*)menu.bufASCIIstr);

					if(start_ang >= 0)
						start_ang +=0.00005;
					else
						start_ang -=0.00005;

					if(start_ang > 279) 
						start_ang = 279;
					if(start_ang < -279)
						start_ang = -279;
					entry = CHANGE_PARAM_CH;
				}
				if(menu_ch == 9 ){
					step_ang = atof((char*)menu.bufASCIIstr);

					if(step_ang >= 0)
						step_ang +=0.00005;
					else
						step_ang -=0.00005;

					if(step_ang > 279)
						step_ang = 279;
					if(step_ang < -279)
						step_ang = -279;
					entry = CHANGE_PARAM_CH;
				}		
		}
		SET_C(idInpointRadius);
		init_cursor_pos();
		return;
	}
	if (menu.ResultForInputNum == input::cancel_)
	{
		display.print_flash_local(sProbel21, page, x_, y_);
		SET_C(idInpointRadius);
		init_cursor_pos();
		if(menu_ch == 2)entry = CHANGE_PARAM_CH;
		if(menu_ch == 4)entry = CHANGE_FOCUS_MENU_CH;
		if(menu_ch == 5)entry = CHANGE_FOCUS_MENU_CH;
		if(menu_ch == 7)entry = CHANGE_PARAM_CH;
		if(menu_ch == 8)entry = CHANGE_PARAM_CH;
		if(menu_ch == 9)entry = CHANGE_PARAM_CH;
		return;
	}
	return;
}

void Menu::action_input_radius_PUSK(const unsigned char &page)
{

	if(entry == PUSK_CH)
	{
		display.clear_ram_null(page);
		display.put_button_(X_BUTTON_PLACE + 1,Y_ONE);
		display.put_button_(X_BUTTON_PLACE + 1,Y_TWO);
		display.put_button_(X_BUTTON_PLACE + 1,Y_THREE);
		display.put_button_(X_BUTTON_PLACE + 1,Y_FOUR);

		display.print_flash_local(sSled, page, 35, 70);
		display.print_flash_local(sPred, page, 35, 126);
		display.print_flash_local(sSTOP, page, 35, 178);

		count_holes = 1;
		x_old = 0;
		y_old = 0;
		reset_counters_rel();
		goto_Relative_coord(primary_axis_ch,secondary_axis_ch);
		entry = LOAD_COORD_HOLE_CH;
	}

	if(entry == LOAD_COORD_HOLE_CH){
		print_long_format(count_holes,CPrg::FORMAT::FORMAT_END,page,37,12,RIGHT_ALIGN,1);// выбранное отверстие
		getcoord_incircle_one();
		entry = IDLE_CH;
	}

	if(entry == IDLE_CH){
		if(!LH_CH_setup[0].sound)
			sound_holes();
		return;
	}

}

void Menu::enter_to_input_loop_ch()
{
	if(curItem->id == idInpointRadius)
	{
		if(menu_ch == 2)  // кол-во отверстий
			SET_C(idInpointRadiusInput);
		if(menu_ch == 4 && primary_axis_ch != no_axis)  // центр главная ось
			SET_C(idInpointRadiusInput);
		if(menu_ch == 5 && secondary_axis_ch != no_axis) // центр вторичная ось
			SET_C(idInpointRadiusInput);
		if(menu_ch == 7) //радиус
			SET_C(idInpointRadiusInput);
		if(menu_ch == 8) //начальный угол
			SET_C(idInpointRadiusInput);
		if(menu_ch == 9 && type_ch == 1) //начальный угол
			SET_C(idInpointRadiusInput);
	}
}



void Menu::Init_CH(const unsigned char &page)
{
	display.clear_ram_null();
	init_cursor_pos();
	//menu_ch = 1;
	check_axis_ch(primary_axis_ch);
	check_axis_ch(secondary_axis_ch);

	draw_menu_ch(page); 
	draw_param_ch(page);

	display.put_button_(X_BUTTON_PLACE + 1,Y_FOUR);
	display.print_flash_local(sPusk, page, 35, 178);

	entry = IDLE_CH;
}

void Menu::check_axis_ch(int8_t & axis_ch)
{
	if((unsigned char)axis_ch == P2[0] || (unsigned char)axis_ch == P2[1] || (unsigned char)axis_ch == P2[2] || (unsigned char)axis_ch == P2[3]){
		if(axis_ch > 8)
		axis_ch = no_axis;
	}
	else
		axis_ch = no_axis;
}

void Menu::draw_menu_ch(const unsigned char &page)
{
	for(uint8_t i = 0; i < 11;i++)
	{		        			
			if( i == menu_ch || i == 0)
			{				
				INVERT_CHAR()
			}
			else
				NORMAL_CHAR()

		if( i == 4 ){
			display.print_flash_local(sAxis_screen[primary_axis_ch],page,1,64);
			display.print_symbols_local(' ',12,page,6,64);

			if(primary_axis_ch != no_axis)
				display.put_char_local_independent(0x80,page,2,64);
			else
				display.put_char_local_independent(0x80,page,1,64);
				
			if(primary_axis_ch != no_axis)			
				Lir.print_long_format(primary_centr_ch,get_format(primary_axis_ch),page,17,64,RIGHT_ALIGN);			
		}
		else if( i == 5 ){
			display.print_flash_local(sAxis_screen[secondary_axis_ch],page,1,80);
			display.print_symbols_local(' ',12,page,6,80);
			if(secondary_axis_ch != no_axis)
				Lir.print_long_format(secondary_centr_ch,get_format(secondary_axis_ch),page,17,80,RIGHT_ALIGN);
		}			
		else
			display.print_flash_local(sInpointRadius[i],page,0,i * 16);
	}
	NORMAL_CHAR()
	display.put_char_local_independent(0x80,page,2,16*14);
	display.print_flash_local(sOpornaOsb,page,3,16*14);
	entry = IDLE_CH;
}

void Menu::draw_param_ch(const unsigned char &page)
{
	const char *p;
	//==тип окружности==
	if(type_ch)
		display.print_flash_local(sSegment,page,32,16,RIGHT_ALIGN);
	else
		display.print_flash_local(sPolniy,page,32,16,RIGHT_ALIGN);
	//==

	//==кол-во отверстий==
	Lir.print_long_format(n,CPrg::FORMAT::FORMAT_END,page,32,32,RIGHT_ALIGN,1);
	//==


	//==Центр==
		//==Оси==
		//Оси и центр рисуются в draw_menu
	//

	// ==Радиус==
	long tmp = R;
	Lir.print_long_format(tmp,CPrg::FORMAT::LINEAR_4,page,32,112,RIGHT_ALIGN);
	tmp = 0;
	//==

	// ==Начальный угол==
	tmp = start_ang * 10000;
	Lir.print_long_format(tmp,CPrg::FORMAT::CIRCLE_4,page,33,128,RIGHT_ALIGN);
	tmp = 0;
	//==

	// ==Шаг угла ===
	if(n != 1)		
	tmp = step_ang * 10000;	
	display.print_symbols_local(' ',10,page,20,144);

	Lir.print_long_format(tmp,CPrg::FORMAT::CIRCLE_4,page,33,144,RIGHT_ALIGN);
	tmp = 0;
	//==

	//==Звук==
	if(!LH_CH_setup[0].sound)
		p = sYES;
	else
		p = sNO;
	display.print_flash_local(p,page,32,16 * 10,RIGHT_ALIGN);
	//==

	entry = IDLE_CH;
}

void Menu::menu_ch_up()
{
	if(curItem->id == idInpointRadius)
	{
		menu_ch--;
		if(menu_ch == 3 || menu_ch == 6)
		menu_ch--;

		if(menu_ch == 0 )
		menu_ch  = 11 - 1;

		entry = CHANGE_FOCUS_MENU_CH;
	}	
}

void Menu::menu_ch_down()
{
	if(curItem->id == idInpointRadius)
	{
		menu_ch++;

		if(menu_ch == 3 || menu_ch == 6)
		menu_ch++;

		if(menu_ch == 11 )
		menu_ch = 1;

		entry = CHANGE_FOCUS_MENU_CH;
	}	
}

void Menu::menu_ch_blank(const char& BLANK)
{
	if(curItem->id == idInpointRadius)
	{
		if(BLANK == _key_BLANK4)
		{
			if(primary_axis_ch != no_axis && secondary_axis_ch != no_axis && n > 1 && R != 0){
			SET_C(idInpointRadiusPUSK);
			entry = PUSK_CH;
			}
			return;
		}
	}

	if(curItem->id == idInpointRadiusPUSK){

		if(BLANK == _key_BLANK2){
			count_holes++;
			if(count_holes > n)
				count_holes = 1;

			entry = LOAD_COORD_HOLE_CH;
			return;
		}
		if(BLANK == _key_BLANK3){
			count_holes--;
			if(count_holes == 0)
				count_holes = n;

			entry = LOAD_COORD_HOLE_CH;
			return;
		}
		if(BLANK == _key_BLANK4)
		{
			exit_ch_PUSK();
			return;
		}
	}
}

bool Menu::exit_ch_PUSK()
{
	if(curItem->id == idInpointRadiusPUSK){
		SET_C(idInpointRadius);
		entry = INIT_CH;
		return 1;
	}
	return 0;
}

void Menu::change_param_ch_left_right(const char &r_l)
{
	if(curItem->id == idInpointRadius)
	{
		if(menu_ch == 1){
			if(type_ch)
				type_ch = 0;  // полная окружность
			else
				type_ch = 1;  // сегмент
			entry = CHANGE_PARAM_CH;
		}

		if(menu_ch == 4){
			change_axis_ch(primary_axis_ch,secondary_axis_ch,r_l);
			flash_prg.to_save_primary_axis_ch_to_EEPROM((uint8_t*)&primary_axis_ch);
			entry = CHANGE_FOCUS_MENU_CH;
		}

		if(menu_ch == 5){
			change_axis_ch(secondary_axis_ch,primary_axis_ch,r_l);
			flash_prg.to_save_secondary_axis_ch_to_EEPROM((uint8_t*)&secondary_axis_ch);
			entry = CHANGE_FOCUS_MENU_CH;
		}
		if(menu_ch == 10){
			if(!LH_CH_setup[0].sound)
				LH_CH_setup[0].sound = 1;
			else
				LH_CH_setup[0].sound = 0;
			save_LH_CH_setup_to_eeprom();
			entry = CHANGE_PARAM_CH;
		}
	}
}
#endif
/*
int8_t &a - переменная с номером оси 1 изменяемая
int8_t &b - переменная с номером оси 2 не изменяемая
const char znak - направление изменения
*/
uint8_t Menu::change_axis_ch(int8_t &a,int8_t&b,const char znak)
{
	do{
		if (a == CInit::NO_AXIS){
			if(znak == '-')
			a = CInit::AXIS_A;
			if(znak == '+')
			a = -1;
		}

		if(znak == '-'){
			a--;
			if (a == b)
			a--;
		}
		if(znak == '+'){
			a++;
			if (a == b)
			a++;
		}
		
		if (a == -1 || a == CInit::AXIS_A) {
			a = CInit::NO_AXIS;
			return 0;
		}

	}while(a != (int16_t)P2[0] && a != (int16_t)P2[1] && a != (int16_t)P2[2] && a != (int16_t)P2[3]);

	return 1;
}

/*
uint16_t &str - переменная с номером оси изменяемая
const char znak - направление изменения
*/
void Menu::change_axis_str(uint16_t &_str,const bool znak)
{
	int8_t a;
	a = (int8_t)_str;

	if(a < 0)
		a = 0;

	do{
		if (a == CInit::NO_AXIS){
			if(znak)
				a = -1;
		}

		if(!znak){
			a--;
		}
		if(znak){
			a++;
		}
		
		if (a == -1 || a == CInit::NO_AXIS) {
			a = CInit::NO_AXIS;
			_str = (uint16_t)a;
			return;
		}

	}while(a != (int16_t)P2[0] && a != (int16_t)P2[1] && a != (int16_t)P2[2] && a != (int16_t)P2[3]);

	_str = (uint16_t)a;
	return;
}



#if (configUSE_Calc == 1)
void Menu::action_calc(char page)
{
static unsigned char trigger_c;

	if( calc_entry == INIT_CALC)
	{
		Init_calc(calc_entry,y,page);
	}
	if( calc_entry == INIT_NEW_STRING)
	{
		Init_new_string(calc_entry,y,page);
	}
	if( calc_entry == INPUT_LOOP)
	{
		flashing_cursor(0,y,page);
		if ( mode_calc == 0 && trigger_c == 0) {
			show_mode(page);
			trigger_c = 1;
		}
		if ( mode_calc == 1 && trigger_c == 1) {
			show_mode(page);
			trigger_c = 2;
		}
		if ( mode_calc == 2 && trigger_c == 2) {
			show_mode(page);
			trigger_c = 0;
		}
	}
	if( calc_entry == FINISH_INPUT)
	{		
		memset(txt_out_clc_res,0,BUF_CLC_RES);
		get_nums_calc((char*)bufASCIIstr,txt_out_clc_res);
		memset(bufASCIIstr,0,IN_BUF_SZ);
		char c = display.strnlen_lir((char*)txt_out_clc_res,BUF_CLC_RES);
		display.put_char_local_independent(' ',page,cursor_pos,y);
		char tmp = 'K';
		#if (configUSE_Lang == 1)
		tmp = 'C';
		#endif
		display.put_char_local_independent(tmp,page,0,0);
		if (mode_calc == 0) {				
			if(c >=12) {
				if(y != 224 ){
					y = 16 + y;
					cursor_pos = 0;
				}
			}
			if(c > 0){
				display.put_char_local_independent('=',page,cursor_pos+1,y);
				display.print_ram_local((char*)txt_out_clc_res,page,cursor_pos+3,y);
			}
		}
		else{
			if(c > 0){
				display.print_symbols_local(' ',32,page,0,y);
				display.print_ram_local((char*)txt_out_clc_res, page, 0, y);
				}
		}
		calc_entry = INIT_NEW_STRING;
	} 
}
	
/*
функция копирует последний результат и выводит на текущую строку
*/
void Menu::calc_copy_result()
{
	if (Lir.curItem->id == idCalc){
		uint16_t n = display.strnlen_lir((const char*)bufASCIIstr, IN_BUF_SZ);
		if (n != cursor_pos)
		 return;
		clc_shift_result();
		if(txt_out_clc_res[0] == '-')
			txt_out_clc_res[0] = '±';

		display.strncat_lir((char*)bufASCIIstr,IN_BUF_SZ,(char*)txt_out_clc_res);
		cursor_pos = display.strnlen_lir((const char*)bufASCIIstr);

		if(cursor_pos > 0)
		result[1] = bufASCIIstr[cursor_pos - 1];

		Update_input_str = 1;

		memset(txt_out_clc_res,0,BUF_CLC_RES);
	
	}
}
/*
функция смещает буфер результата в начало строки если находит знак '='
*/
void Menu::clc_shift_result()
{
	for(int i = 0 ; i<BUF_CLC_RES; i++){
		if (txt_out_clc_res[i] == '=') {
			for (int j = 0; (i + j + 2) < BUF_CLC_RES ; j++) {
				txt_out_clc_res[j] = txt_out_clc_res[j + i + 2];
				if (txt_out_clc_res[j + i + 2] == '\0')
					return;
			}
		}
	}
}

void Menu::show_mode(const char &page)
{
	if (mode_calc == 0)
	{
		display.print_flash_local(PSTR(" + "), page, 35, 12);
		display.print_flash_local(PSTR(" - "), page, 35, 70);
		display.print_flash_local(PSTR(" * "), page, 35, 126);
		display.print_flash_local(PSTR(" /  "), page, 35, 178);
	}
	if (mode_calc == 1)
	{
		display.print_flash_local(PSTR("sin"), page, 35, 12);
		display.print_flash_local(PSTR("cos"), page, 35, 70);
		display.print_flash_local(PSTR("tan"), page, 35, 126);
		display.print_flash_local(PSTR("    "), page, 35, 178);
	}
	if (mode_calc == 2)
	{
		display.print_flash_local(PSTR("1/x"), page, 35, 12);
		display.print_flash_local(PSTR(" x "), page, 35, 70);
		display.put_char_local_independent(0xB2,page,37,70);
		display.print_flash_local(PSTR("   "), page, 35, 126);
		display.put_char_local_independent(0xB3,page,36,126);
		display.put_char_local_independent(0xB4,page,37,126);
		display.print_flash_local(sSBRS, page, 35, 178);
	}
}

void Menu::Init_calc(char & calc_entry,int &y_coord,const char &page)
{
	page_calc = page;
	display.clear_ram_null(page);
	init_cursor_pos();
	display.print_flash_local(sCalculator,page,0,0);

	display.put_button_(X_BUTTON_PLACE,Y_ONE);
	display.put_button_(X_BUTTON_PLACE,Y_TWO);
	display.put_button_(X_BUTTON_PLACE,Y_THREE);
	display.put_button_(X_BUTTON_PLACE,Y_FOUR);

	show_mode(page);
	calc_entry = INIT_NEW_STRING;
	y_coord=0;
}

void Menu::Init_new_string(char & calc_entry,int &y_coord,const char &page)
{
	// перед переходом на новую строку проверка и инициализация новой строки
	#define max_string 224
	#define next_string 16

	if(y_coord >= 208) //max_string
	{
		y_coord = 0;
	}
	if(y_coord == 0)
		y_coord+=next_string;
	else
		y_coord+=next_string * 2;

	display.print_symbols_local(' ',32,page,0,y_coord);

	init_cursor_pos();
	calc_entry = INPUT_LOOP;
}
/*
функция анализирует входящую строку находя оператор и члены
вычисляет результат и выводит результат в *txt_out
*/
void Menu::get_nums_calc(char* buf,unsigned char* txt_out)
{

char buf_a[22] = {0};
char buf_b[22] = {0};
char operator_ = 0;
bool point = false;

		get_str_a_b_operator_calc(buf,operator_,buf_a,buf_b,point);

		if (mode_calc == 1)// sin cos tan sqrt
		{
		    if(operator_ == '/') return;
			n:
			calculate_mode_1(buf_a, operator_, txt_out);
			return;
		}
		if (mode_calc == 2)//1/x sqr
		{
			if(operator_ == '*') 
			{
				operator_ = '/';
				goto n;
			}

			calculate_mode_2(buf_a, operator_, txt_out,point);
			return;
		}
		
		if(buf_b[0] == 0 || operator_ == 0|| buf_a[0] == 0) 
		{
			//txt_out[0]='?';
			//txt_out[1]='\0';
			return;
		}

		if(point == false)
		{
			calculate_integer_number(buf_a,buf_b,operator_,point,txt_out);
		}

		if(point == true )
		{					
			calculate_real_number_fixed(buf_a,buf_b,operator_,txt_out);
		} 
}

void Menu::get_str_a_b_operator_calc(char buf[],char &operator_,char buf_a[],char buf_b[],bool &point)
{
	bool a = true;
	signed char b_inc = 0;
	for(int i = 0;buf[i] !=0 ;i++)
	{
		switch(buf[i])
		{
			case '+':
			operator_ = buf[i];
			a = false;
			break;
			case '-':
			operator_ = buf[i];
			a = false;
			break;
			case '*':
			operator_ = buf[i];
			a = false;
			break;
			case '/':
			operator_ = buf[i];
			a = false;
			break;
			case '±':
			if(a == true)
			buf_a[i] = '-';
			else
			{
				buf_b[b_inc] = '-';
				b_inc++;
			}
			break;
			case '.':
			point = true;
			if(a == true)
			buf_a[i] = '.';
			else
			{
				buf_b[b_inc] = '.';
				b_inc++;
			}
			break;
			default:
			if(a == true)
			buf_a[i] = buf[i];
			else
			{
				buf_b[b_inc] = buf[i];
				b_inc++;
			}
			break;
		}
	}
}

void Menu::calculate_integer_number(char buf_a[],char buf_b[],char &operator_,bool &point,unsigned char txt_out[])
{
		signed long long a_num64bit = 0;
		signed long long b_num64bit = 0;
		signed long long result64bit = 0;
		//signed long result = 0;

		a_num64bit = strtoll_lir((unsigned char*)buf_a);
		b_num64bit = strtoll_lir((unsigned char*)buf_b);
		if(errorno == errors::integer_overflow)
		{
			errorno = no_error;
			strcpy_P((char*)txt_out,sPredel);		
			return;
		}
		unsigned char a_count = display.strnlen_lir(buf_a);
		unsigned char b_count = display.strnlen_lir(buf_b);
		if(buf_a[0] == '-')a_count--;
		if(buf_b[0] == '-')b_count--;
		unsigned char sym_ = a_count + b_count;

 		if(a_count >18 || b_count >18)
 		{
 			strcpy_P((char*)txt_out,sPredel);
 			return;
 		}
		switch(operator_)
		{
			case'+':
			result64bit = a_num64bit + b_num64bit;
			break;
			case'-':
			result64bit = a_num64bit - b_num64bit;
			break;
			case'*':
				if (a_num64bit == 0 || b_num64bit == 0)
				{
					result64bit = 0;
					break;
				}
				if (sym_ > 18) { 
					strcpy_P((char*)txt_out,sPredel);
					return; 
				}
				result64bit = a_num64bit * b_num64bit;
			break;
			case'/':
			if (b_num64bit == 0)
			{
				strcpy_P((char*)txt_out,sError);
				return;
			}
			if (a_num64bit == 0)
			{
				result64bit = 0;
				break;
			}			
			if(a_num64bit % b_num64bit != 0)
			{
				point  = true;
				return;
			}
			result64bit = a_num64bit / b_num64bit;
			break;
			default:
			return;
			break;
		}
		if(SREG & 0x8)
		{
			strcpy_P((char*)txt_out,sPredel);
			return;
		}

		lltoa_lir(result64bit,txt_out);
}

void Menu::calculate_real_number(char buf_a[],char buf_b[],char &operator_,unsigned char txt_out[])
{
		double a_num,b_num,result_d;
		signed long long result64bit = 0;
		signed long result = 0;
		
		a_num = strtod((const char*)buf_a,0);
		b_num = strtod((const char*)buf_b,0);
		switch(operator_)
		{
			case'+':
			result_d = a_num + b_num;
			break;
			case'-':
			result_d = a_num - b_num;
			break;
			case'*':
			result_d = a_num * b_num;
			break;
			case'/':
			result_d = a_num / b_num;
			break;
			default:
			result_d = a_num;
			break;
		}
		//dtostrf(result_d,7, 3, (char*)txt_out);	lround(R * cos(M_PI*cur_ang / 180) * channel_1);result64bit  = lround(result_d) * 10000;
		//real_to_longlong(result64bit,result_d);
		//result64bit  =  ceil(result_d *10000);
		//result64bit  =  (result_d + 0.00005)*10000; // 0.0000
		//result64bit  =  (result_d + 0.005)*100;  // 0.00
		//result64bit  =  lround(result_d*10000);
		result64bit  =  result_d*10000;
		//dtostrf(result_d,5, 4, (char*)txt_out);
		//return;
		if(check_overflow_long(result64bit))
		{
			strcpy_P((char*)txt_out,PSTR("предел"));
			return;
		}

		result = static_cast<long>(result64bit);

		display.itoa_quick_lir(&result,txt_out);
		display.formatting2((char*)txt_out,CPrg::FORMAT::LINEAR_4);
		cut_zero(txt_out);
}

void Menu::calculate_mode_1(char *buf_a, char &operator_, unsigned char *txt_out)
{
	signed long long i_num64bit;

	float f_res = 0,f_num;

	f_num = atof(buf_a);

	unsigned char buf[22] = { 0 };
	//-----this is not need for avr becouse atof works differently on PC and avr
	/*display.strncpy_lir((char*)buf, buf_a,sizeof(buf));
	if (shift_str_left(buf))
	{
		i_num64bit = strtoll_lir(buf);
		char a = display.strnlen_lir((char*)buf);

		int del = 1;
		for (int i = 0; i < a; i++)
		{
			del *= 10;
		}
		f_num += (float)i_num64bit / del;
	}
	memset(buf, 0, sizeof(buf)); */
	if (operator_ == '/')
	{
		if (f_num < 0)
		{
			strcpy_P((char*)txt_out, PSTR("ошибка ввода"));
			return;
		}
	}
	if (operator_ == '*')
	{
		if (f_num >= 90 && f_num <= 90.09)
		{
			strcpy_P((char*)txt_out, PSTR("ошибка ввода"));
			return;
		}
	}
	
	switch (operator_)
	{
		case'+':
		f_res = sin(M_PI*f_num / 180);
		buf[0] = 's';
		buf[1] = 'i';
		buf[2] = 'n';
		buf[3] = '(';
		break;
		case'-':
		f_res = cos(M_PI*f_num / 180);
		buf[0] = 'c';
		buf[1] = 'o';
		buf[2] = 's';
		buf[3] = '(';
		break;
		case'*':
		f_res = tan(M_PI*f_num / 180);
		buf[0] = 't';
		buf[1] = 'a';
		buf[2] = 'n';
		buf[3] = '(';
		break;
		case'/':
		f_res = sqrt(f_num);
		buf[0] = 0xB3;//'s';
		buf[1] = 0xB4;//'q';
		//buf[2] = 'r';
		//buf[3] = 't';
		//buf[4] = '(';
		break;
		default:
			return;
		break;
	}

	if (f_res > 0)
		i_num64bit = (f_res + 0.00005) * 10000;
	else
		i_num64bit = (f_res - 0.00005) * 10000;


	lltoa_lir(i_num64bit, txt_out);
	display.formatting2((char*)txt_out, CPrg::LINEAR_4);
	cut_zero((unsigned char*)txt_out);

	display.strncat_lir((char*)buf, sizeof(buf), buf_a);
	if(operator_ != '/'){
	display.strncatsym_lir((char*)buf, sizeof(buf), '°');
	display.strncatsym_lir((char*)buf, sizeof(buf), ')');
	}
	display.strncatsym_lir((char*)buf, sizeof(buf), ' ');
	display.strncatsym_lir((char*)buf, sizeof(buf), '=');
	display.strncatsym_lir((char*)buf, sizeof(buf), ' ');
	display.strncat_lir((char*)buf, sizeof(buf),(char*)txt_out);

	display.strncpy_lir((char*)txt_out, (const char*)buf,30);
}

void Menu::calculate_mode_2(char *buf_a, char &operator_, unsigned char *txt_out,bool &point_)
{
	//strcpy_P((char*)txt_out, PSTR("в разработке"));
	//return;
	char buf_b[22] = { 0 };
	char buf_a_[22] = { 0 };
	char buf[30] = { 0 };
	char operator_2 = 0;
	switch (operator_)
	{
		case'+':
			buf_b[0] = '1';
			buf_b[1] = '\0';
			operator_2 = '/';

			buf[0] = '1';
			buf[1] = '/';
			mystr.strncpy_lir(buf_a_, buf_a, sizeof(buf_a_));
			calculate_real_number_fixed(buf_b, buf_a, operator_2, txt_out);
		break;
		case'-':
// 			buf[0] = 's';
// 			buf[1] = 'q';
// 			buf[2] = 'r';
// 			buf[3] = '(';
			mystr.strncpy_lir(buf_a_, buf_a, sizeof(buf_a_));
			mystr.strncpy_lir(buf_b, buf_a, sizeof(buf_b));
			operator_2 = '*';
			calculate_real_number_fixed(buf_b, buf_a, operator_2, txt_out);
		break;
		case'*':

		return;
		break;
		case'/':
		for(int i = 16 ;i <=224; i +=16)
			display.print_symbols_local(' ',32,page_calc,0,i);
		init_cursor_pos();
		y = 0;
		return;
		break;
		default:
		return;
		break;
	}

	mystr.strncat_lir((char*)buf, sizeof(buf), buf_a_);
	if (operator_ == '-')
	{
		mystr.strncatsym_lir((char*)buf, sizeof(buf), 0xB2); //')'
	}
	mystr.strncatsym_lir((char*)buf, sizeof(buf), ' ');
	mystr.strncatsym_lir((char*)buf, sizeof(buf), '=');
	mystr.strncatsym_lir((char*)buf, sizeof(buf), ' ');
	mystr.strncat_lir((char*)buf, sizeof(buf), (char*)txt_out);

	mystr.strncpy_lir((char*)txt_out, (const char*)buf, 30);
	return;
}

bool Menu::shift_str_left(unsigned char * str)
{
	#define SREZ 4
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == '.' && str[i + 1] != '\0')
		{
			int j = 0;
			for (; str[i] != '\0'; i++, j++)
			{
				str[j] = str[i + 1];
				if (j == SREZ)
				break;
			}
			str[j] = '\0';
			return 1;
		}
	}
	return 0;
}

void Menu::calculate_real_number_fixed(char buf_a[], char buf_b[], char & operator_, unsigned char txt_out[])
{
	static int y_pair;
	y_pair = SPL | (SPH<<8);  //9239
	long long a = 0;
	long long b = 0;
	long long result64bit = 0;

	put_dot(buf_a);
	put_dot(buf_b);
	
	if((count_before_dot(buf_a)>=15) || (count_before_dot(buf_b)>=15))
	{
		strcpy_P((char*)txt_out,sPredel);
		return;
	}


	ConvertASCIIlinear_circle_tolonglong(a, CPrg::LINEAR_4,buf_a);
	ConvertASCIIlinear_circle_tolonglong(b, CPrg::LINEAR_4,buf_b);
	lltoa_lir(a,(unsigned char*)buf_a);
	lltoa_lir(b,(unsigned char*)buf_b);
	unsigned char a_count = display.strnlen_lir(buf_a,21);
	unsigned char b_count = display.strnlen_lir(buf_b,21);	
	if(buf_a[0] == '-')a_count--;
	if(buf_b[0] == '-')b_count--;
	unsigned char sym_ = a_count + b_count;

	switch (operator_)
	{
		case'+':
			result64bit = a + b;
		break;
		case'-':
			result64bit = a - b;
		break;
		case'*':
			if (a == 0 || b == 0)
			{
				result64bit = 0;
				break;
			}
			if(sym_ >21)
			{
				strcpy_P((char*)txt_out,sPredel);
				return;
			}
			result64bit = mul_calc2(a,b);
			if(result64bit == 0)
			{
				strcpy_P((char*)txt_out,sPredel);
				return;
			}

		break;
		
		case'/':
			if (b == 0)
			{
				strcpy_P((char*)txt_out,sError);
				return;
			}
			if (a == 0)
			{
				result64bit = 0;
				break;
			}
			if(a_count >16 || b_count >16)
			{
				strcpy_P((char*)txt_out,sPredel);
				return;
			}		
			result64bit = div_calc(a,b);
			if(result64bit == 0)
			{
				strcpy_P((char*)txt_out,sPredel);
				return;
			}
		break;

		default:
		result64bit = a;
		break;
	}
	if(SREG & 0x8)
	{
		strcpy_P((char*)txt_out,sPredel);
		return;
	}

	lltoa_lir(result64bit, txt_out);
	display.formatting2((char*)txt_out, CPrg::LINEAR_4);
	cut_zero(txt_out);	
	restore_y_pair(y_pair);
	return;
}

void Menu::put_dot(char * str)
{
	int c = display.strnlen_lir(str, 20);

	for (int i = c; i >= 0; i--)
	{
		if (str[i] == '.') {
			return;
		}
	}
	str[c] = '.';
	str[c + 1] = '\0';
}

unsigned char Menu::count_before_dot(char * str)
{
	// считает кол-во разрядов до точки в строке
	unsigned char i = 0 ;
	for(;str[i] !='\0';i++)
	{
		if(str[i] == '.')break;
	}
	return i;
}

long long Menu::div_calc(long long& a_, long long& b)
{
		int b_zero = count_addedzero2(b,1);//count_addedzero_(b);
		int dif = 0;

		if (b_zero == 0) dif = 4;
		if (b_zero == 1) dif = 3;
		if (b_zero == 2) dif = 2;
		if (b_zero == 3) dif = 1;
		if (b_zero == 4) dif = 0;

		//long long a_ = a;
		long long res_;

		for (int i = 0; i < dif; i++)
		{
			a_ *= 10;
		}
		//unsigned char txt[21]= {0};
		//lltoa_lir(a_,txt);
		//display.print_ram_local((char*)txt,1,0,224);
		//if(a_>= 999999900000000 && b_zero<=1) return 0;

		int divb = count_addedzero2(b,2);//count_addedzero(b);
#define rounding 10
		if (a_ > MAXLONGLONG /10) return 0;

		res_ = a_*rounding / (b / divb);
		if (res_ < 0)res_ = (res_ - 5)/ rounding; //округление
		if (res_ >= 0)res_ = (res_ + 5) / rounding; //округление
		return res_;
}

long long Menu::mul_calc2(long long& a_,long  long& b_)
{
	int b_zero = count_addedzero2(b_,1);
	int a_zero = count_addedzero2(a_,1);

	long long res_;
	long rounding_ = 10000;
	if (b_zero > a_zero)
	{
		for (int i = 0; i < b_zero; i++)
		{
			b_ /= 10;
			rounding_ /= 10;
		}
	}
	else
	{
		for (int i = 0; i < a_zero; i++)
		{
			a_ /= 10;
			rounding_ /= 10;
		}
	}
	res_ = (a_ *  b_) *10;
	if (res_ < 0) res_ = ((res_ / rounding_) - 5) / 10; //округление
	if (res_ >= 0)res_ = ((res_ / rounding_) + 5) / 10; //округление
	return res_;
}


int Menu::count_addedzero_(long long & num)
{
// функция возвращает кол во нулей в конце числа
	unsigned char str[21] = {0};
	//display.itoa_quick_lir(&num, str);
	lltoa_lir(num,str);

	int c = display.strnlen_lir((char*)str, 21) - 1;
	int counter = 0;
	for (int i = c; i >= 0; i--)
	{
		if (counter == 4) break;
		if (str[i] == '0') counter++;
		else
		break;
	}

	return counter;
}

int Menu::count_addedzero(long long & num)
{
// функция возвращает число для усечения нулей
	unsigned char str[21]= {0};
	//display.itoa_quick_lir((long*)&num, str);
	lltoa_lir(num,str);

	int c = display.strnlen_lir((char*)str,21) - 1;
	int counter = 0;
	for (int i = c; i >= 0; i--)
	{
		if (counter == 4) break;
		if (str[i] == '0') counter++;
		else
		break;
	}

	if (counter == 4) return 10000;
	if (counter == 3) return 1000;
	if (counter == 2) return 100;
	if (counter == 1) return 10;

	return 1;
}

int Menu::count_addedzero2(long long data,char action)
{
	unsigned char byte;
	if (data < 0) {
		data = -data;
	}
	int i = 0;
	for (;i < 4; i++)
	{
		byte = data % 10 + 0x30;
		if (byte != '0') break;
		data /= 10;
	}
	if (action == 1) 
		return  i;
	else
	{
		if (i == 4) return 10000;
		if (i == 3) return 1000;
		if (i == 2) return 100;
		if (i == 1) return 10;
		if (i == 0) return 1;
	}
	return 1;
}

void Menu::real_to_longlong(signed long long &result64bit,double &result_d)
{
		double truncated=0;/*,roundedFraction*/;
		double fraction =0;// modf(result_d, &truncated);
		//modf((0.00005 + fraction)*10000, &roundedFraction);
		fraction = 0.00005 + fraction;
		//fraction = trunc(fraction *10000)/10000;
		result64bit =  (truncated + fraction) *10000;
}


void Menu::check_str_num(char buf_num[],unsigned char* txt_out)
{
	//int c = display.strnlen_lir(buf_num);
	

}

bool Menu::compare(const long long &a,const long long &b)
{
#define max_c 999999999999999999LL
#define min_c 9LL
		if(a > max_c && b >min_c) return 1;
		if(a > max_c/10 && b >min_c) return 1;
		if(a > max_c/100 && b >min_c*11) return 1;
		if(a > max_c/1000 && b >min_c*111) return 1;
		if(a > max_c/10000 && b >min_c*1111) return 1;
		if(a > max_c/100000 && b >min_c*11111) return 1;
		if(a > max_c/1000000 && b >min_c*111111) return 1;
		if(a > max_c/10000000 && b >min_c*1111111) return 1;
		if(a > max_c/100000000 && b >min_c*11111111) return 1;
		if(a > max_c/1000000000 && b >min_c*111111111) return 1;


// 		long long k = 1;
// 		long long j = 100000000000000000;
// 		for(int i = 0;i <10;i++)
// 		{
// 			if(a > max_c /k  && b > max_c/j) return 1;
// 			k = k*10;
// 			if( i >=1) j = j/10;
// 		}

	return 0;
}

void Menu::calc_cursor_down()
{
 	display.put_char_local_independent(' ',page_calc,cursor_pos,y);
	if( y < 224){		
		y += 16;		
	}
	else
		y = 16;
	init_cursor_pos();
	display.print_symbols_local(' ',32,page_calc,0,y);
}

void Menu::calc_cursor_up()
{
	display.put_char_local_independent(' ',page_calc,cursor_pos,y);
	if( y > 16){		
		y -= 16;		
	}
	else
		y = 224;
	init_cursor_pos();
	display.print_symbols_local(' ',32,page_calc,0,y);
}


#endif
void Menu::cut_zero(unsigned char* txt_out)
{
	int c = display.strnlen_lir((char*)txt_out,22) - 1;

	for(int i = c;i>=0;i--)
	{
		if (txt_out[i] != '0' && txt_out[i] != '.' && txt_out[i + 1] != '0')
		return;

		if(txt_out[i] != '0' && txt_out[i + 1] == '0')
		{
			if(txt_out[i] == '.')
				txt_out[i] = '\0';
			else
				txt_out[i + 1] = '\0';
			return;
		}
	}
	return;
}
int Menu::check_overflow_long(long long &result)
{
	#define OVERFLOW_LONG 1

	if(result > LONG_MAX || result < LONG_MIN)
	return OVERFLOW_LONG; // overflow
	else
	return 0; // no overflow
}

void Menu::calc_blank_input(char k_c)
{
#if (configUSE_Calc == 1)
	if (curItem->id == idCalc)
	{																	//для кнопки очистки экрана
		if (!(result[1] >= '0' && result[1] <= '9') &&  !(result[1] == '\0' && k_c == _key_BLANK4 && mode_calc == 2))
					return;
		switch (k_c)
		{
			case _key_BLANK1:
			result[1] = '+';
			bufASCIIstr[cursor_pos] = result[1];
			cursor_pos++;
			break;
			case _key_BLANK2:
			result[1] = '-';
			bufASCIIstr[cursor_pos] = result[1];
			cursor_pos++;
			break;
			case _key_BLANK3:
			result[1] = '*';
			bufASCIIstr[cursor_pos] = result[1];
			cursor_pos++;
			break;
			case _key_BLANK4:
			result[1] = '/';
			bufASCIIstr[cursor_pos] = result[1];
			cursor_pos++;
			break;
		}
		if (Lir.mode_calc == 1 )
		{
			Lir.calc_entry = FINISH_INPUT;
		}
		if (Lir.mode_calc == 2)
		{
			Lir.calc_entry = FINISH_INPUT;
		}
	}
#endif
}

void Menu::save_load_calc(const unsigned char& action)
{
	// сохранение и восстановление набираемой строки , текущего положения курсора и символа перед курсором
	#define save 1
	#define load 2

	static unsigned char bufASCIIstr_[IN_BUF_SZ];
	static unsigned char cursor_pos_;
	static unsigned char sym_;

	if(action == save)
	{
		memset(bufASCIIstr_,0,IN_BUF_SZ);
		display.strncpy_lir((char*)bufASCIIstr_,(char*)bufASCIIstr,IN_BUF_SZ);
		cursor_pos_ = cursor_pos;
		sym_ = result[1];
		init_cursor_pos();
	}
	if(action == load)
	{
		memset(bufASCIIstr,0,IN_BUF_SZ);
		display.strncpy_lir((char*)bufASCIIstr,(char*)bufASCIIstr_,IN_BUF_SZ);
		cursor_pos = cursor_pos_;
		result[1] = sym_;
		inputnum = 21;
	}
}
#if (configUSE_SnakeGame == 1)
void Menu::draw_skbis(char obj)
{
	/*map[12][28 + 1] =                 {  "############################",
										   "#                          #",
										   "#  ** *  * ****  *   *  ** #",
										   "# *   * *  *     *  ** *   #",
										   "# *   **   ***   * * * *   #",
										   "# *   * *  *  *  **  * *   #",
										   "#  ** *  * ***   *   *  ** #",
										   "#                          #",
										   "#                          #",
										   "#                          #",
										   "#                          #",
										   "############################"};*/
int n = 2;

	map[n][3]= obj;
	map[n][4]= obj;
	map[n][6]= obj;
	map[n][9]= obj;
	map[n][11]= obj;
	map[n][12]= obj;
	map[n][13]= obj;
	map[n][14]= obj;
	map[n][17]= obj;
	map[n][21]= obj;
	map[n][24]= obj;
	map[n][25]= obj;

n = 3;
	map[n][2]= obj;
	map[n][6]= obj;
	map[n][8]= obj;
	map[n][11]= obj;
	map[n][17]= obj;
	map[n][20]= obj;
	map[n][21]= obj;
	map[n][23]= obj;

n = 4;
	map[n][2]= obj;
	map[n][6]= obj;
	map[n][7]= obj;
	map[n][11]= obj;
	map[n][12]= obj;
	map[n][13]= obj;
	map[n][17]= obj;
	map[n][19]= obj;
	map[n][21]= obj;
	map[n][23]= obj;

n = 5;
	map[n][2]= obj;
	map[n][6]= obj;
	map[n][8]= obj;
	map[n][11]= obj;
	map[n][14]= obj;
	map[n][17]= obj;
	map[n][18]= obj;
	map[n][21]= obj;
	map[n][23]= obj;

n = 6;
	map[n][3]= obj;
	map[n][4]= obj;
	map[n][6]= obj;
	map[n][9]= obj;
	map[n][11]= obj;
	map[n][12]= obj;
	map[n][13]= obj;
	map[n][17]= obj;
	map[n][21]= obj;
	map[n][24]= obj;
	map[n][25]= obj;
}

void Menu::exit_game()
{
	if(curItem->id == SnakeLoop){
		init_menu_ABOU(0);
		SET_C(idAbout);
	}
}
#endif
void Menu::direup()
{
    if (dire[1] == 0)
    {
	    dire[1] = -1;
	    dire[0] = 0;
    }
}

void Menu::diredown()
{
    if (dire[1] == 0)
    {
	    dire[1] = 1;
	    dire[0] = 0;
    }
}

void Menu::direleft()
{
    if (dire[0] == 0)
    {
	    dire[1] = 0;
	    dire[0] = -1;
    }
}

void Menu::direright()
{
    if (dire[0] == 0)
    {
	    dire[1] = 0;
	    dire[0] = 1;
    }
}
 bool Menu::ResetParametrsinRAM()
 {    
     P2[0]=0;P2[1]= 1;P2[2] = no_axis;P2[3] = no_axis;
	 Str[0] = 0;Str[1] = 1;Str[2] = no_axis;Str[3] = no_axis;

	 for(int x = 0;x<N_AXIS;x++)
		display.position_to_see[x] = CDisp::COORDINATE;

	 return true;
 }
 
 
 void Menu::showActionResetparam()
 {
     display.print_flash_local(PSTR("Сброс параметров"),0,1,100);
     delay_ms(333);
     display.print_flash_local(sPoint,0,18,100);
     delay_ms(333);
     display.print_flash_local(sPoint,0,19,100);
     delay_ms(333);
     display.print_flash_local(sPoint,0,20,100);
     delay_ms(333);
     display.print_flash_local(PSTR("ok"),0,21,100);
     delay_ms(100);
     display.print_flash_local(sProbel21,0,1,100);
 }
 
 
 void Menu::showActionResetparam2()
 {
    GoToMenu(idAllp);
 }
 
 
void Menu::showActionResetOffset()
{
	GoToMenu(idAllp);
}

Menu::_MAXO& Menu::_MAXO::operator=(bool set)
{
	if(set == on)
		return *this;

	if(set == 1){
		if(!use())
			set = 0;
	}
	on = set;
	if(on){
		Lir.dir_mh = NO_DIR_MH;
		//dir_mh_sv = NO_DIR_MH;
		Lir.btn_hold_mh = 0; //толчок
		Lir.btn_cont_mh = 0; //движение до отмены
		l_k_s.OFF_LED(_led_LEFT);
		l_k_s.OFF_LED(_led_RIGHT);
		display.put_button_with_pic(X_BUTTON_PLACE,Y_ONE,_pic_HANDWEEL_ON);

		for(int c = 0; c < N_AXIS;c++){
			if(Lir.ch_to_move & Lir.plb2(c)){
				Lir.PUSK_axis(c);
				Lir.sts_c[c] = 1;
			}
		}	
		
		sync();//==синхронизация
		if(mul == 1)
			display.put_button_with_pic(X_BUTTON_PLACE,Y_THREE,_pic_HWx1);
		else if (mul == 10)
			display.put_button_with_pic(X_BUTTON_PLACE,Y_THREE,_pic_HWx10);
		else if (mul == 100)
			display.put_button_with_pic(X_BUTTON_PLACE,Y_THREE,_pic_HWx100);
	}else{
		display.put_button_with_pic(X_BUTTON_PLACE,Y_ONE,_pic_HANDWEEL_OFF);
		display.put_button_with_pic(X_BUTTON_PLACE,Y_THREE,_pic_PROG);
		Lir.axis_drive_stop_lir(0,Lir.ch_to_move);
	}
	
	return *this;
}

bool Menu::_MAXO::operator==(bool b)
 {
	if(on == b)
		return 1;
	else
		return 0;
 }
 /*
 использует ли выбранная ось маховик?
 return true - да
 return false - нет
 */
bool Menu::_MAXO::use()
{
	for(int c = 0 ; c < N_AXIS;c++){
		if(Lir.ch_to_move & Lir.plb2(c)){
			if(hw_on(c))
				return true;
		}
	}
	return false;
}
/*
синхронизация логического счетчика(LP) со счетчиком маховика(RP)
*/
void Menu::_MAXO::sync()
{
	for(int c = 0 ; c < N_AXIS;c++){
		if(Lir.ch_to_move & Lir.plb2(c)){
			if(hw_on(c)){
				prev[c] = 0;
				altera.counter[c] = 0;
				cli();
				#if (configUSE_mcx514 == 1)
				SetRp(Lir.plb2(c),0); //скопировать координату из LP в RP
				#endif
				sei();
				}
			}
		}
	return;
}

void Menu::_MAXO::process(int8_t c)
{
#if (configUSE_mcx514 == 1)
	if(hw_on(c)){
		long tmp_cur_pos = Lir.mul_MAXO(c,altera.counter[c]);
		if(on == 1){
			Lir.outMcx514(Lir.plb2(c),
						  Lir.div_LP(c,tmp_cur_pos - prev[c]) ,
						  Lir.Spd_to_Hz(&Lir._modals_cmd.F,c,1),
						  Lir.acceleration(c),
						  Lir.Spd_to_Hz(&flash_prg.axis_cfg_nc[c].InitSpd,c));
			}			
		prev[c] = tmp_cur_pos;
	}
#else
	if(on == 0)
		return;
	for(int c = 0; c < N_AXIS;c++){
		if(Lir.ch_to_move & Lir.plb2(c)){
			char ch = Lir.plb2(c);
			if(hw_on(c)){
				long spd = Lir.get_spd(0);
				int8_t dir = spd < 0 ? -1:1;
				if(dir == DIR_MH_RIGHT && !(Lir.ch_sw.plus & ch) && !(Lir.ch_sw_prog.plus & ch)){dir = 1;}
				else if(dir == DIR_MH_LEFT && !(Lir.ch_sw.minus & ch) && !(Lir.ch_sw_prog.minus & ch)){dir = -1;}
				else {spd = 0;} //Остановка

				if(safety(c))
					spd = 0;

				if(spd != prev[c]){
					Lir.outAnalog(Lir.Spd_to_U(labs(((long long)spd * 1000 * mul)/100),c,Lir._modals_cmd.G0,0),
									flash_prg._OUTAxis2[c].AnalOut,
									flash_prg._OUTAxis2[c].Polar,
									dir,1);
					prev[c] = spd;					
					}
				}
			}	
		}  
#endif
}
/*
включен ли маховик в параметрах
*/
bool Menu::_MAXO::hw_on(int8_t c)
{
	#if (configUSE_mcx514 == 1)
	if(flash_prg._OUTAxis[c].typeZ == CPrg::IMPULSE && flash_prg.axis_cfg_nc[c].datchik == CPrg::HANDWEEL)
		return true;
	#else
	if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
		return true;
	#endif
	return false;
}
/*
return true блокировка движения
*/
bool Menu::_MAXO::safety(int8_t c)
{
	if(Lir.lock_error_PLC)
		return true;
	else if(Lir.TORMOZ[c])
		return true;
	else if(flash_prg.is_encoder_fault(c))
		return true;
	else if(Lir.STOP_PODACHA)
		return true;
	else
		return false;
}

/*
uint8_t c - канал
long spd - скорость которая переопределяется
*/
long Menu::_v_luft::set(uint8_t c,long spd)
{
	if(c < N_AXIS){
		if(flash_prg._Allp_param2[c].viborka_lufta == 0){ //есть выборка люфта
			spd_sv[c] = spd;
			if(flash_prg._OUTAxis[c].typeZ == CPrg::DISCRET)
				spd = flash_prg._Allp_param[c].start_LUFT;
			else if(flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG)
				spd = flash_prg._Allp_param6[c].start_LUFT;
			ON[c] = 1;
			cli();
			abs_cnt[c] = mCounter[c].abs_pure;// сохраняем текущее положение
			sei();

			if(Lir.cnt_z[c] && flash_prg._OUTAxis[c].typeZ == CPrg::ANALOG){ //если есть задание то мы в преднаборе или программном режиме
				int z = 0;
				for(;z < ZONES - 1;z++){
					if(flash_prg._Allp_param4[c].spdG1[z] <= spd)
						break;		
					}
				for(;z < ZONES - 1;z++){
					if (flash_prg._Allp_param3[c].zone[z] >= (Lir.cnt_z[c] * Lir.dir_exe[c])){
						if(spd > flash_prg._Allp_param4[c].spdG1[z])
							spd = flash_prg._Allp_param4[c].spdG1[z];							
						}
					}
				}
			}
		}
	return spd;
}

void Menu::_v_luft::process(int i)
{
	if(i >= N_AXIS)
		return; // для слепой оси исключение
	if(ON[i] == 0)
		return;

	if(labs(abs_cnt[i] - mCounter[i].abs) > 30){
		ON[i] = 0;
		abs_cnt[i] = 0;
		if(flash_prg._OUTAxis[i].typeZ == CPrg::DISCRET)
			Lir.outABCD(spd_sv[i], &flash_prg._OUTAxis[i].Out_A,i);
		else if(flash_prg._OUTAxis[i].typeZ == CPrg::ANALOG)
			Lir.outAnalog(Lir.Spd_to_U(spd_sv[i],i,Lir._modals_cmd.G0),flash_prg._OUTAxis2[i].AnalOut,flash_prg._OUTAxis2[i].Polar,Lir.dir_exe[i]);
		else if(flash_prg._OUTAxis[i].typeZ == CPrg::IMPULSE){
			#if (configUSE_mcx514 == 1)
			Lir.outMcx514(Lir.plb2(i),Lir.div_LP(i,cnt_z[i]),Lir.Spd_to_Hz(&_modals_cmd.F,i),Lir.acceleration(i),Lir.Spd_to_Hz(&flash_prg.axis_cfg_nc[i].InitSpd,i));
			#endif
		}
		spd_sv[i] = 0;
		Lir.outP_all();
	}
}

void Menu::_v_luft::init(int8_t e)
{
	if(e == N_AXIS){
		memset(ON     ,0,sizeof(bool) * N_AXIS);
		memset(abs_cnt,0,sizeof(long) * N_AXIS);
	}else{
		ON[e] = 0;
		abs_cnt[e] = 0;
	}
}
