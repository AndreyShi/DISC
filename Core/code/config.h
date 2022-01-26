 /*
 * config.h
 *
 * Created: 07.09.2018 8:09:51
 *  Author: Andrew
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_
#include <avr/pgmspace.h>
#define F_CPU 32000000UL  // 32 MHz
#include <util/delay.h>
//установленная конфигурация показана в меню "О программе" напротив пункта "сборка:"
//в виде 32ух битного десятичного числа, содержащее в себе биты настроек представленных ниже
//самый младший бит это параметр configUSE_menuItem_Correction_and_Izmerenia и так далее
//установка битов в функции menu_2_ABOU() MenuLir.h
//УПР - настройки используемые в системе управления
//добавление новых конфигураций в конец
#define configUSE_Lang                              0  // 0 - русский ,1 - английский
#define configUSE_SnakeGame                         0  // 2 356 bytes in flash and 1 020 bytes in static RAM ,по кнопке Enter находясь в меню "О Программе"
#define configUSE_menuItem_Correction_and_Izmerenia 0  // 26 198 bytes пункты меню коррекции и измерения 1 - есть  0 - нет
#define configUSE_Circle_holes                      0  // 7 118 bytes функция для рассчета координат отверстий на окружности takes   кнопка 4(|V) 
#define configUSE_Linear_holes                      0  // функция для рассчета отверстий на прямой ,кнопка || 7044 bytes
#define configUSE_Calc                              1  // 9 086 bytes функция калькулятор takes about in flash кнопка | (x) и в меню 
#define configUSE_INCH                              1  // использование формата дюймов
#define configUSE_Large_Digit                       1  // 2 112 bytes отображение large чисел во 2ой строке и в 4ой при условии ,если 1 строка оси == нет или 3 строка оси == нет
#define configUSE_REZERV                            0  // резервная конфигурация 
#define configUSE_NO_input                          0  // 0 - входа есть , 1 - входов нет
#define configUSE_ABS_Encoder                       0  // 0 - инкрементные, 1 - абсолютные преобразователи перемещения 

#define configUSE_Cycle1ms                          0  //УПР// цикл опроса датчиков: 1 - 1 мс , 0 - 10мс (стабильный) 
#define configUSE_NCshell                           1  //УПР// оболочка , 1 - индикация + управление, 0 - индикация
#define configUSE_LIR541                            1  //УПР// 1 - ЛИР541
#define configUSE_Shadow_axis                       1  //УПР// 1 - включена доп. ось без датчика, 0 - нет
#define configUSE_PLC		                        1  //УПР// 1 - связь с контроллером включена(6860 bytes), 0 - нет
#define configUSE_mcx514                            0  //УПР// 1 - ЛИР542 
#define configUSE_Conf01                            1  //УПР// 1 - ошибки датчика не приходят на порты МК и REF0 ->PA3 , REN0-> PB1  (только c PLC ЛИР541)
#define configUSE_ModBus_DRV                        0  //УПР// 1 - используется связь с приводом через ModBus, 0 - нет
#define configUSE_Alt_ENCerr                        1  //УПР// 1 - ошибки датчика через Альтеру(только c PLC ЛИР541)

#define configUSE_ExtFormats                        0  // форматы 0.000000 и 0°00'00.0"
#define configUSE_max7370                           1  // если 1 то для клавиатуры используется max7370 , если 0 то max7349
#define configUSE_RGB_disp                          0  // использование цветного дисплея
#define configUSE_debug_ABS_G53                     1  // вывод абсолютных координат + G53 под осями в режимах Отв на окр и Отв на прям

#define Sleep(x) _delay_ms(x)

#define MAX_contr 50  //максимальная контрастность
#define MIN_contr 1   //минимальная контрастность

#define MAX_INSTR 10  //кол -во инструмента

const char *const p_DATE = __DATE__;
const char *const p_TIME = __TIME__;

#define usb200000 1
#define usb115200 2
#define usb9600  3


/*Диагностика коррекции на главном экране:
	если NO_ERROR или TABLE_IS_ENDED_PLUS_ или TABLE_IS_ENDED_MINES_ то 'K' ,
    если NO_REF то вместо 'K' будет пробел
	если сообщения другие будет стоять номер сообщения из списка ниже.
	Что бы сбросить сообщение/ошибку можно войти в меню диагностика или меню выхода в реф или нажать 'C'


0   NO_ERROR                          нет ошибки (будет стоять 'К')
1   NO_FLASH_ANSWER                   память не отвечает
2   TIMEOUT_WHEN_HEADER_IS_READ       превышено время чтение заголовка памяти
3   NO_TABLE_PLUS_NO_TABLE_MINES_     нет таблицы + и нет таблицы -
4   TABLE_IS_ENDED_PLUS_              таблица + закончена (будет стоять 'К')
5   TABLE_IS_ENDED_MINES_			  таблица - закончена (будет стоять 'К')
6   INTERVAL_IS_ZERO_                 интервал между точками равен 0
7   SIGN_PLUS_WRONG_                  знак + ошибка
8   SIGN_MINES_WRONG_				  знак - ошибка
9   DELTA_BELOW_ZERO_                 дельта меньше нуля
10  RESULT_IS_UNREAL_                 результат не реальный
11  MULT_WRONG_                       умножение на кооэффициент ошибка
12  NO_FLASH_ANSWER_COMMON            память нет ответа команды
13  NO_REF						      ось не вышла в ноль (будет стоять ' ')
*/
#endif /* CONFIG_H_ */