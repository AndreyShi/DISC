#include "error_log.h" 
/* 
добавляет ошибку в журнал ,ошибки со статусом SINGLE если она уже есть в журнале то добавление игнорируется
text_ - указатель на flash с текстом ошибки 
id_ - статус ошибки SINGLE - добавлена один раз RESET_MSG - сброшена
*/
void error_log::addmsg(const char * text_,char type_msg, unsigned short number_)
{
	char buf[40] = { 0 };
	if(type_msg == SINGLE || type_msg == RESET_MSG)
	{
		for (int i = MAX_ERROR - 1; i >= 0; i--)
		{
			if (msg[i].text == NULL)continue;
			strcpy_P(buf, msg[i].text);
			if (!strcmp_P(buf, text_) && msg[i].status != RESET_MSG)
			{
				if (type_msg == RESET_MSG)
				{
					msg[i].status = RESET_MSG;
					change_msg_status_in_eeprom(i);
				}
				return;
			}			
		}
	}
	if (type_msg == RESET_MSG) return;

	if (counter >= MAX_ERROR) {
		counter = MAX_ERROR - 1;
	}

	for (int i = counter - 1; i >= 0; i--)
	{
		msg[i + 1].number = msg[i].number;
		msg[i + 1].status = msg[i].status;
		msg[i + 1].text = msg[i].text;
		memcpy(msg[i + 1].e_time, msg[i].e_time, sizeof(msg[i].e_time));
	}
	msg[0].number = number_;
	msg[0].status = SINGLE;
	msg[0].text = text_;
	set_e_time(msg[0].e_time, l_k_s.real_time_to_see);

	counter++;
	save_msgs_to_eeprom();
}
/*
функция записывает из SYSTEMTIME_BCD в ERRORTIME
SYSTEMTIME указатель на буфер со временем системы
ERRORTIME указатель на буфер время ошибки
*/
void error_log::set_e_time(ERRORTIME * e_time_bcd, SYSTEMTIME_BCD * time_bcd)
{
	for (int i = 0; i < 6; i++)
	{
		e_time_bcd[i] = time_bcd[i];
	}
}

void error_log::time_string_with_seconds_and_date(unsigned char * time_bcd, char * time_ascii)
{
	// BCD format to ASCII для вывода на экран
	char tmp[2] = { 0 };
	l_k_s.convertBCDtoASCII(time_bcd[2], tmp);
	time_ascii[0] = tmp[0];
	time_ascii[1] = tmp[1];
	l_k_s.convertBCDtoASCII(time_bcd[1], tmp);
	time_ascii[2] = ':';
	time_ascii[3] = tmp[0];
	time_ascii[4] = tmp[1];
	l_k_s.convertBCDtoASCII(time_bcd[0], tmp);
	time_ascii[5] = ':';
	time_ascii[6] = tmp[0];
	time_ascii[7] = tmp[1];
	time_ascii[8] = ' ';
	l_k_s.convertBCDtoASCII(time_bcd[3] & 0x3f, tmp);
	time_ascii[9] = tmp[0];
	time_ascii[10] = tmp[1];
	time_ascii[11] = '.';
	l_k_s.convertBCDtoASCII(time_bcd[4] & 0x1f, tmp);
	time_ascii[12] = tmp[0];
	time_ascii[13] = tmp[1];
	time_ascii[14] = '\0';
}

void error_log::save_msgs_to_eeprom()
{
	unsigned char buf[MSG_LOG_SZ] = { 0 };


	for (int i = 0; i < counter; i++)
	{
		int j = 0;
		buf[j] = msg[i].number >> 8;
		j++;
		buf[j] |= (unsigned char)msg[i].number;
		j++;
		for (; j < 7; j++)
		{
			buf[j] = msg[i].e_time[j - 2];
		}
		buf[j] = msg[i].status;

		flash_prg.write_block_to_eeprom_address_int(LOG_EEPROM + (i * MSG_LOG_SZ), buf, sizeof(buf));
	}
}

void error_log::change_msg_status_in_eeprom(const short &i)
{
	flash_prg.write_block_to_eeprom_address_int(LOG_EEPROM + ((i * MSG_LOG_SZ) + 7), &msg[i].status, 1);
}

void error_log::reset_msgs_in_eeprom()
{
	unsigned char buf[MSG_LOG_SZ] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
	for (int i = 0; i < MAX_ERROR * MSG_LOG_SZ; i+=8)
	{
		flash_prg.write_block_to_eeprom_address_int(LOG_EEPROM + i, buf, sizeof(buf));
	}
}

/*
выводит ошибки на дисплей с задержкой DELAY циклов .
page - номер страницы памяти дисплея
*/
void error_log::out_ondisplay(const char &page)
{
#define DELAY  800
	
	counter_display_run++;

	if (counter_display_run < DELAY)
	{
		INVERT_CHAR()
		display.print_flash_local(PSTR("ЖУРНАЛ ОШИБОК:"),page, 0, 0);
		NORMAL_CHAR()
		display.print_flash_local(PSTR("страница"), page, 15, 224);
		display.put_char_local_independent((e_log.page + 1) + 0x30, page, 24, 224);
		display.put_char_local_independent('/', page, 25, 224);
		display.put_char_local_independent('2', page, 26, 224);
		display.print_flash_local(PSTR("сбросить <C>"), page, 0, 224);	
		
		if(reset)
		{
			if(counter_display_run == 1)display.put_char_local_independent('.',page,0,16);	
			if(counter_display_run == 260)display.put_char_local_independent('.',page,1,16);
			if(counter_display_run == 532){display.put_char_local_independent('.',page,2,16);reset = 0;}
		}
		return;
	}

	if (counter_display_run > DELAY)
	{
		counter_display_run = 0; //DELAY
		return;
	}

	if (counter == 0)
	{
		display.print_flash_local(PSTR("журнал пуст"), page, 0, 16);
		return;
	}
	int j = 0;
	for (int i = 0; i < counter; i++)
	{
		if ((i >= 0 && i < 6) && e_log.page == 1) {
			if(counter == 0)
				display.print_flash_local(PSTR("журнал пуст"), page, 0, 16);
			else
				display.print_flash_local(PSTR("           "), page, 0, 16);
			j = 0;
			continue;
		}
		if ((i > 5 && i < MAX_ERROR ) && e_log.page == 0) {
			j = 0;
			continue;
		}

		display.print_flash_local(PSTR("номер:"), page, 0, (j + 1) * 16);
		display.print_debug_one_local(msg[i].number, page, 6, (j + 1) * 16);

		char time_ascii[15] = { 0 };
		time_string_with_seconds_and_date(msg[i].e_time, time_ascii);
		display.print_ram_local(time_ascii, page, 26, (j + 1) * 16);

		display.print_debug_one_local(msg[i].status, page, 41, (j + 1) * 16);

		display.print_flash_local(msg[i].text, page, 0, (j + 2) * 16);
		j += 2;
	}

}
/*
обнуляет все ошибки, очищает экран
*/
void error_log::reset_log()
{
	for (int i = 0; i < MAX_ERROR; i++)
	{
		msg[i].text = NULL;
		msg[i].status = 0;
		msg[i].number = 0;
 		memset(msg[i].e_time,0,sizeof(msg[i].e_time));
	}
	counter = 0;
	reset = 1;
	counter_display_run = 0;
	display.clear_rectangle(0, 1, 320, 25, 0x00, 2);
	reset_msgs_in_eeprom();
}

void error_log::set_counter_delay()
{
	counter_display_run = 0;
}

void error_log::set_page()
{
	if (page == 0)
	{
		page = 1;
	}
	else
	{
		page = 0;
	}
	display.clear_rectangle(0, 1, 320, 25, 0x00, 2);
}

void error_log::load_msgs_from_eeprom()
{
	bool error;

	unsigned short num[MAX_ERROR] = { 0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF };
	unsigned char  time_BCD[MAX_ERROR][5] = { 0 };
	unsigned char  status[MAX_ERROR] = { 0 };

	for (int i = 0; i < MAX_ERROR; i++)
	{
		int j = 0;
		num[i] = flash_prg.read_one_byte_eeprom_address_int(LOG_EEPROM + (i* MSG_LOG_SZ) + j, &error);
		num[i] = num[i] << 8;
		j++;
		num[i] |= flash_prg.read_one_byte_eeprom_address_int(LOG_EEPROM + (i* MSG_LOG_SZ) + j, &error);

		if (num[i] > ERRORS || num[i] == 0 || num[i] == 0xFFFF)
		continue;
		j++;
		for (; j < 7; j++)
		{
			time_BCD[i][j - 2] = flash_prg.read_one_byte_eeprom_address_int(LOG_EEPROM + (i* MSG_LOG_SZ) + j, &error);
		}

		status[i] = flash_prg.read_one_byte_eeprom_address_int(LOG_EEPROM + (i* MSG_LOG_SZ) + j, &error);

		msg[i].number = num[i];
		msg[i].status = status[i];
		memcpy(msg[i].e_time, time_BCD[i], sizeof(time_BCD[i]));

		switch (msg[i].number)
		{
			case 1:
			msg[i].text = sOshibka_datchika_portP2;
			break;
			case 2:
			msg[i].text = sOshibka_datchika_portP2;
			break;
			case 3:
			msg[i].text = sOshibka_datchika_portP2;
			break;
			case 4:
			msg[i].text = sOshibka_datchika_portP2;
			break;
			default:
				msg[i].text = PSTR("неизвестная ошибка");
			break;
		}
		counter++;
	}
}

void error_log::out_on_screen_service(short x, short y, unsigned char page)
{
	static unsigned char trigger;

	if (msg[0].status == SINGLE && counter != 0)
	{
		if (~trigger & 1)
		{
			display.print_flash_local(msg[0].text, page, x, y);
			trigger |= 1;
		}
	}
	else
	{
		if (trigger & 1)
		{
			display.print_symbols_local(' ', 27, page, x, y);
			trigger &= ~(1 << 0);
			trigger &= ~(1 << 1);
		}
		if ((~trigger & 2) && counter != 0)
		{
			display.print_flash_local(PSTR("смотри журнал"), page, x, y);
			trigger |= 2;
		}
		if ((trigger & 2) && counter == 0)
		{
			trigger &= ~(1 << 1);
		}
	}
}
