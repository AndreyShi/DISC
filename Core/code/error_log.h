
 #ifndef ERROR_LOG_H
 #define ERROR_LOG_H

#include "display.h"
#include "led_key_sound.h"
#include "MenuLir.h"

typedef unsigned char SYSTEMTIME_BCD;
typedef unsigned char ERRORTIME;

/*
  MAX_ERROR максимальное кол-во сообщение видимых на экране
*/ 
#define MAX_ERROR 12  
/*
  SINGLE добавление ошибки только один раз, 
  следущее добавление такой же ошибки возможно только после очистки журнала ошибок <C>
*/
#define SINGLE    0 
/*
  MULTIPLE добавление ошибки каждый цикл программы, каждый цикл добавляется одна и таже ошибка в журнал
*/ 
#define MULTIPLE  1   
/*
  RESET_MSG сделать ошибку не активной
*/
#define RESET_MSG  2 
/*
 LOG_EEPROM стартовый адрес для хранения ошибок в EEPROM
*/
#define LOG_EEPROM  2048
/*
 MSG_LOG_SZ размер в байтах одного сообщения лежащего в EEPROM
*/
#define MSG_LOG_SZ 8
class error_log 
{
private:
	struct e_msg
	{
		unsigned char  status;     // статус ошибки
		unsigned short number;     // номер ошибки
		const char *text;          // текст ошибки
		unsigned char e_time[6];   // время ошибки BCD format
	};
	unsigned char counter;                             // счетчик сообщений
	unsigned short counter_display_run;                // счетчик display_run
	e_msg msg[MAX_ERROR];                              // сообщения
	char reset; // флаг сброса
	char page;  // текущая страница
	void set_e_time(ERRORTIME *e_time_, SYSTEMTIME_BCD *time_p);// установка времени сообщения

	void time_string_with_seconds_and_date(unsigned char * incoming, char * output);// BCD format to ASCII для вывода на экран
	void save_msgs_to_eeprom();// сохраняет все текущее сообщения в eeprom
	void change_msg_status_in_eeprom(const short &i); // меняет статус сообщения в eeprom
	void reset_msgs_in_eeprom(); //очищает область памяти eeprom

public:
	void addmsg(const char * text_, char type_msg, unsigned short number); // добавление сообщения
	void out_ondisplay(const char &page = 0);                                   // показать error_log
	void reset_log();                                                           // сбросить журнал ошибок
	void set_counter_delay();
	void set_page();// переключение страницы ошибок
	void load_msgs_from_eeprom();// загружает сообщения из eeprom
	void out_on_screen_service(short x, short y, unsigned char page);      // вывод на главный экран одной ошибки
};

extern error_log e_log;
#define ERRORS 4
/* НОМЕРА ОШИБОК number
начало
01 ошибка датчика.разъём:P2.0
02 ошибка датчика.разъём:P2.1
03 ошибка датчика.разъём:P2.2
04 ошибка датчика.разъём:P2.3
конец
*/
#endif