#include "Strings.h"

/*==============================Венгерская нотация: названия  переменных  в которых хранятся строки, начинаются с s=============================*/
//=======================================================Строки для Индикации (базовые строки)==================================================
const char sLine[] PROGMEM = {" --------------------------------------"};
const char sPoint[]PROGMEM={"."};
const char sP2[][5] PROGMEM = {{"P2.0"},
	{"P2.1"},
	{"P2.2"},
	{"P2.3"},
	{"P2  "}};
const char sP3[][5] PROGMEM = {{"P3.0"},
	{"P3.1"},
	{"P3.2"},
	{"P3.3"},
	{"P3  "}};
const char sProbel10[] PROGMEM = "            ";
const char sProbel21[] PROGMEM = "                             ";
#if (configUSE_Lang == 0)//======================================start RUS======================================================================
const char sCM[] PROGMEM = "  КМ  ";
const char sZona[] PROGMEM = "Зона";
const char s_CheckAxisParam[] PROGMEM = "проверить осевые параметры!";
const char sSpd[] PROGMEM = "скорость";
const char sTme[] PROGMEM = " время  ";
const char sStack_left[] PROGMEM = " Cтэк осталось:";
const char sStack_used[] PROGMEM = " Cтэк использовано:";
const char sError[] PROGMEM = "ошибка";
const char sYES[] PROGMEM = "да ";
const char sNO[] PROGMEM = "нет";
const char sSchetchik[] PROGMEM = "счетчик:";
const char sPerepolnenie_sbros[] PROGMEM = "переполнение,сброс <C>";
const char sSchetchik_ne_v_diapozone[] PROGMEM = "счетчик не в диапазоне";
const char sOsb_ne_vishla_v_0[] PROGMEM = "ось не вышла в 0";
const char sVivesti_v_0_PUSK[] PROGMEM = "вывести в ноль - <СТАРТ>";
const char sBaudrate[][13]PROGMEM = {{"200 Кбит/c  "},
									 {"115,2 Кбит/c"},
								     {"9,6 Кбит/c  "}};
const char sOshibka_datchika_portP2[]PROGMEM = "ошибка датчика, cброс <C>  ";
const char sPlease_input_pass[] PROGMEM = "Введите пароль:";
const char sAccess_denided[] PROGMEM = "Нет доступа";
const char sAxisList[] PROGMEM =  " СПИСОК ОСЕЙ ПУЛЬТА" ;
const char sAxisListOffset[] PROGMEM = " СИСТЕМА КООРДИНАТ";
const char sAxisNoList[] PROGMEM =  " Нет назначенных осей" ;
const char sRadius[] PROGMEM = "радиус ";
const char sDiametr[] PROGMEM = "диаметр";
const char s01sek[] PROGMEM = "0.1 сек.";
const char s03sek[] PROGMEM = "0.3 сек.";
const char s06sek[] PROGMEM = "0.6 сек.";
const char sOFFSET_COORDINATE[] PROGMEM = " СМЕЩЕНИЯ КООРДИНАТ";
const char sFast[] PROGMEM = "быстро  ";
const char sSlow[] PROGMEM = "медленно";
const char sStrokaIndikacii[] PROGMEM = " СТРОКА ИНДИКАЦИИ";
const char sSbrosParam[] PROGMEM = " Сброс параметров:";
const char sSbrosParam01[] PROGMEM = " После третьего нажатия <СТАРТ>";
const char sSbrosParam02[] PROGMEM = " произойдет сброс параметров";
const char sSbrosParam03[] PROGMEM = " <СТОП> выход из меню";
const char sResetOffset[] PROGMEM = " Сброс смещений:";
const char sResetOffset01[] PROGMEM = " После нажатия <СТАРТ>";
const char sResetOffset02[] PROGMEM = " произойдет сброс смещений";
const char sResetOffset03[] PROGMEM = " <СТОП> выход из меню";
const char sCalculator[] PROGMEM = "КАЛЬКУЛЯТОР";
const char sPolniy[] PROGMEM = " полный";
const char sSegment[] PROGMEM = "сегмент";
const char sOpornaOsb[] PROGMEM = "опорная ось";
const char sReshetka[] PROGMEM = "решётка";
const char sSBRS[] PROGMEM = "СБРС";
const char sPusk[] PROGMEM = "ПУСК";
const char sSTOP[] PROGMEM = "СТОП";
const char sSled[] PROGMEM = "след";
const char sPred[] PROGMEM = "пред";
const char sDiag[][23] PROGMEM = {{" Общие параметры"},
								  {" Параметры осей"},
								  {" Смещения координат"},
							      {" Коррекция"},
								  {" Измерения"},
								  {" Настройки"},
								  {" Режим обмена с ПК"},
								  {" Режим отладки"},
								  {" Калькулятор"},
								  {" О программе"}};
const char sAxisOpt[][25] PROGMEM = {{" ПАРАМЕТРЫ ОСИ"},
									 {" Формат"},
									 {" Токарный режим"},
									 {" Направление"},
								     {" Коэффициент"},
									 {" Люфт"},
									 {" Контроль датчика"},
									 {" Сохранение счётчиков"},
									 {" Отображение скорости"},
									 {" Зона референтной метки"},
									 {" Внешнее обнуление"},
									 {" Интервал измер.скорости"},
									 {" Коррекция"},
									 {" Кодированные метки"}};
const char sPredel[]PROGMEM = "предел";
const char sAllpar[][29] PROGMEM = {{" ОБЩИЕ ПАРАМЕТРЫ"},
									{" Оси-Измерительные каналы"},
									{" Оси-экран"},
									{" СБРОС ПАРАМЕТРОВ"},
									{" СБРОС СМЕЩЕНИЙ"},
									{" ПЕРЕЗАГРУЗКА"}};
const char s00_DBG[] PROGMEM = " РЕЖИМ ОТЛАДКИ";
const char s01_DBG[] PROGMEM = " Датчики";
const char s02_DBG[] PROGMEM = " Питание и батарея";
const char s03_DBG[] PROGMEM = " Время";
const char s04_DBG[] PROGMEM = " Входы";
const char s05_DBG[] PROGMEM = " Выходы";
const char s06_DBG[] PROGMEM = " Аналоговый выход";
const char s07_DBG[] PROGMEM = " Аналоговый вход";

const char s00_ABOU[] PROGMEM = " ИЗГОТОВИТЕЛЬ:";
const char s01_ABOU[] PROGMEM = " Санкт-Петербург";
const char s02_ABOU[] PROGMEM = " ОАО СКБ ИС";
const char s03_ABOU[] PROGMEM = " www.skbis.ru";
const char s04_ABOU[] PROGMEM = " дата сборки:";
const char s05_ABOU[] PROGMEM = " сборка  :";
const char s06_ABOU[] PROGMEM = " версия прошивки:";

const char s00_ENCM[] PROGMEM = " ПРЕОБРАЗОВАТЕЛИ ПЕРЕМЕЩЕНИЯ";
const char s01_ENCM[] PROGMEM = "Канал Ось Задание     Счётчик  Люфт";
const char s02_ENCM[] PROGMEM = "Канал Ось Индикация   Счётчик  Люфт";
const char s03_ENCM[] PROGMEM = " ИЗМЕРЕНИЕ СКОРОСТИ";
const char s04_ENCM[] PROGMEM = "Канал Ось Частота(Гц) Реф-Реф";

const char s00_SUPM[] PROGMEM = " ИСТОЧНИК ПИТАНИЯ И БАТАРЕЯ ";
const char s01_SUPM[] PROGMEM = "Напряжение батареи:";
const char s02_SUPM[] PROGMEM = "Напряжение питания:";
const char sBatLow[] PROGMEM = " батарея разряжена";
const char sBatNoCntrl[] PROGMEM = " нет контроля батарейки";

const char s00_TIME[] PROGMEM = " ВРЕМЯ ";
const char s01_TIME[] PROGMEM = "Текущее время:";
const char s02_TIME[] PROGMEM = "День недели  :";
const char s03_TIME[] PROGMEM = "День и месяц :";
const char s04_TIME[] PROGMEM = "Год          :";

const char s00_VIIN[] PROGMEM = " ПРОСМОТР ВХОДОВ 1 - 24";
const char s01_VIIN[] PROGMEM = "- номер";
const char s02_VIIN[] PROGMEM = " ЗОНА РЕФЕРЕНТНОЙ МЕТКИ";
const char s03_VIIN[] PROGMEM = "Канал Ось Индикация   Вход-Реф";

const char s00_LINPK[] PROGMEM = " РЕЖИМ СВЯЗЬ ЧЕРЕЗ ПК ";
const char s01_LINPK[] PROGMEM = "тест:";
const char s02_LINPK[] PROGMEM = "чтение параметров из eeprom";
const char s03_LINPK[] PROGMEM = "прочитано байт:";
const char s04_LINPK[] PROGMEM = "запись параметров в eeprom ";
const char s05_LINPK[] PROGMEM = "записано  байт:";
const char s06_LINPK[] PROGMEM = "Ошибка,проверьте скорость подключения";
const char s07_LINPK[] PROGMEM = "Код ошибки:";
const char s08_LINPK[] PROGMEM = "Количество ошибок:";

const char sOFF[] PROGMEM = "Выключение через: ";

const char sSunday[] PROGMEM    = "воскресенье";
const char sMonday[] PROGMEM    = "понедельник";
const char sTuesday[] PROGMEM   = "вторник    ";
const char sWednesday[] PROGMEM = "среда      ";
const char sThursday[] PROGMEM  = "четверг    ";
const char sFriday[] PROGMEM    = "пятница    ";
const char sSaturday[] PROGMEM  = "суббота    ";

#if (configUSE_Circle_holes == 1)
const char sInpointRadius[][25] PROGMEM = {{" ОТВЕРСТИЯ НА ОКРУЖНОСТИ"},
										   {" Тип окружности"},
										   {" Кол-во отверстий"},
										   {" ----- Центр -----"},
										   {""},
										   {""},
										   {" -----------------"},
										   {" Радиус"},
										   {" Начальный угол"},
										   {" Шаг угла"},
										   {" Звук"}};
#endif
const char s00_lh[] PROGMEM =  " ОТВЕРСТИЯ НА ПРЯМОЙ";
const char s01_lh[] PROGMEM =  " Тип" ;
const char s02_lh[] PROGMEM =  " --1ое отверстие--" ;
const char s03_lh[] PROGMEM =  " Отвер. в ряду" ;
const char s04_lh[] PROGMEM =  " Интервал отвер." ;
const char s05_lh[] PROGMEM =  " Кол-во рядов" ;
const char s06_lh[] PROGMEM =  " Интервал рядов" ;
const char s07_lh[] PROGMEM =  " Угол" ;
const char s08_lh[] PROGMEM =  " Звук" ;
const char sCorr[][20] PROGMEM = {{" КОРРЕКЦИЯ"},
							      {" Снятие коррекции"},
								  {" Работа с таблицами"}};
const char sIzmer[][20] PROGMEM = {{" ИЗМЕРЕНИЯ"},
								   {" Индикатор"},
								   {" График"}};
const char sOption[][23] PROGMEM = {{" НАСТРОЙКИ"},
									{" Яркость экрана"},
									{" Отображение времени"},
									{" Скорость USB"},
									{" Звук"},
									{" Обновление координат"},
									{" Захват ошибки датчика"}};
const char sPultOcb[][13] PROGMEM = {{" ПУЛЬТ - ОСЬ"},
									 {" Р2.0  - "},
									 {" Р2.1  - "},
									 {" Р2.2  - "},
									 {" Р2.3  - "}};
const char sAxis_symbol[][13] PROGMEM = {{" X(линейная)"},
									     {" Y(линейная)"},
										 {" Z(линейная)"},
										 {" U(линейная)"},
										 {" V(линейная)"},
										 {" W(линейная)"},
										 {" P(линейная)"},
										 {" Q(линейная)"},
										 {" R(линейная)"},
										 {" A(круговая)"},
										 {" B(круговая)"},
										 {" C(круговая)"},
										 {" D(круговая)"},
										 {"    нет     "}};
const char  sAxis_screen[][6] PROGMEM = {{ "  X  " },
										 { "  Y  " },
										 { "  Z  " },
										 { "  U  " },
										 { "  V  " },
										 { "  W  " },
										 { "  P  " },
										 { "  Q  " },
									     { "  R  " },
										 { "  A  " },
										 { "  B  " },
										 { "  C  " },
										 { "  D  " },
										 { " нет " }};
#endif //========================================================end RUS========================================================================
#if (configUSE_Lang == 1)//======================================start ENG======================================================================
const char sCM[] PROGMEM = "  CM  ";
const char sZona[] PROGMEM = "Zone";
const char s_CheckAxisParam[] PROGMEM = "check axis parameters!     ";
const char sSpd[] PROGMEM = "speed   ";
const char sTme[] PROGMEM = " time  ";
const char sStack_left[] PROGMEM = " Stack left:   ";
const char sStack_used[] PROGMEM = " Stack used:       ";
const char sError[] PROGMEM = "error ";
const char sYES[] PROGMEM = "on ";
const char sNO[] PROGMEM =  "off";
const char sSchetchik[] PROGMEM = "counter:";
const char sPerepolnenie_sbros[] PROGMEM = "overflow    ,reset <C>";
const char sSchetchik_ne_v_diapozone[] PROGMEM = "counter out  of range  ";
const char sOsb_ne_vishla_v_0[] PROGMEM = "axis doesn`t have ref";
const char sVivesti_v_0_PUSK[] PROGMEM = "go to ref press - <START>";
const char sBaudrate[][13]PROGMEM = {{"200 Kbit/s  "},
{"115,2 Kbit/s"},
{"9,6 Kbit/s  "}};
const char sOshibka_datchika_portP2[]PROGMEM = "encoder fault , reset <C>  ";
const char sPlease_input_pass[] PROGMEM = "Enter password:";
const char sAccess_denided[] PROGMEM = "No access ";
const char sAxisList[] PROGMEM =  " PANEL AXIS LIST" ;
const char sAxisListOffset[] PROGMEM = " COORDINATE SYSTEM";
const char sAxisNoList[] PROGMEM =  " No axes assigned" ;
const char sRadius[] PROGMEM =  "radius  ";
const char sDiametr[] PROGMEM = "diameter";
const char s01sek[] PROGMEM = "0.1 sec.";
const char s03sek[] PROGMEM = "0.3 sec.";
const char s06sek[] PROGMEM = "0.6 sec.";
const char sOFFSET_COORDINATE[] PROGMEM = " OFFSETS SETUP";
const char sFast[] PROGMEM = "fast";
const char sSlow[] PROGMEM = "slow";
const char sStrokaIndikacii[] PROGMEM = " DISPLAY LINE";
const char sSbrosParam[] PROGMEM = " Clear settings:";
const char sSbrosParam01[] PROGMEM = " After the button <START> press three";
const char sSbrosParam02[] PROGMEM = " times, all parameters will be cleared";
const char sSbrosParam03[] PROGMEM = " <STOP> button - exit";
const char sResetOffset[] PROGMEM = " Clear offsets:";
const char sResetOffset01[] PROGMEM = " After the button <START> press";
const char sResetOffset02[] PROGMEM = " all offsets will be cleared";
const char sResetOffset03[] PROGMEM = " <STOP> button - exit";
const char sCalculator[] PROGMEM = "CALCULATOR";
const char sPolniy[] PROGMEM =  " full  ";
const char sSegment[] PROGMEM = "segment";
const char sOpornaOsb[] PROGMEM = "ref axis   ";
const char sReshetka[] PROGMEM = "array  ";
const char sSBRS[] PROGMEM = "CLER";
const char sPusk[] PROGMEM = " RUN";
const char sSTOP[] PROGMEM = "STOP";
const char sSled[] PROGMEM = "next";
const char sPred[] PROGMEM = "prev";
const char sDiag[][23] PROGMEM = {{" General setup"},
{" Axis setup"},
{" Offsets setup"},
{" Error compensation"},
{" Measurement"},
{" Settings"},
{" PC connection"},
{" Diagnostics"},
{" Calculator"},
{" About"}};
const char sAxisOpt[][28] PROGMEM = {{" AXIS SETUP"},
{" Display format"},
{" Turning mode"},
{" Count direction"},
{" Counter ratio"},
{" Backlash"},
{" Encoder fault"},
{" Save counters"},
{" Show feedrate"},
{" Reference mark zone"},
{" External nulling input"},
{" Speed measur. interval"},
{" Error compensation"},
{" Coded reference marks"}};
const char sPredel[]PROGMEM = "overflow";
const char sAllpar[][29] PROGMEM = {{" GENERAL SETUP"},
{" Axes-Encoder ports"},
{" Axes-Screen"},
{" FACTORY RESET"},
{" OFFSETS RESET"},
{" SOFT RESET"}};

const char s00_DBG[] PROGMEM = " DIAGNOSTICS  ";
const char s01_DBG[] PROGMEM = " Encoders";
const char s02_DBG[] PROGMEM = " Power and battery";
const char s03_DBG[] PROGMEM = " Time ";
const char s04_DBG[] PROGMEM = " Inputs";
const char s05_DBG[] PROGMEM = " Outputs";
const char s06_DBG[] PROGMEM = " Analog output";

const char s00_ABOU[] PROGMEM = " MANUFACTURER:";
const char s01_ABOU[] PROGMEM = " Saint-Petersburg, Russia";
const char s02_ABOU[] PROGMEM = " JSC SKBIS ";
const char s03_ABOU[] PROGMEM = " www.skbis.ru";
const char s04_ABOU[] PROGMEM = " Firm.  date:";
const char s05_ABOU[] PROGMEM = " Firmware:";
const char s06_ABOU[] PROGMEM = " Firm.   version:";

const char s00_ENCM[] PROGMEM = " ENCODERS                   ";
const char s01_ENCM[] PROGMEM = "Port Axis Задание     Counter  Backlash";
const char s02_ENCM[] PROGMEM = "Port Axis Display     Counter  Backlash";
const char s03_ENCM[] PROGMEM = " SPEED MEASUREMENT ";
const char s04_ENCM[] PROGMEM = "Port Axis Freq(Hz)    Ref2Ref";

const char s00_SUPM[] PROGMEM = " POWER SUPPLY & BATTERY     ";
const char s01_SUPM[] PROGMEM = "Battery level:     ";
const char s02_SUPM[] PROGMEM = "Power level:       ";
const char sBatLow[] PROGMEM = " battery low      ";
const char sBatNoCntrl[] PROGMEM = " no battery control    ";

const char s00_TIME[] PROGMEM = " TIME  ";
const char s01_TIME[] PROGMEM = "Current time :";
const char s02_TIME[] PROGMEM = "Week         :";
const char s03_TIME[] PROGMEM = "Day & Month  :";
const char s04_TIME[] PROGMEM = "Year         :";

const char s00_VIIN[] PROGMEM = " INPUTS DIAGNOSTICS 1 - 24";
const char s01_VIIN[] PROGMEM = "- num  ";
const char s02_VIIN[] PROGMEM = " REFERENCE MARK ZONE   ";
const char s03_VIIN[] PROGMEM = "Port Axis Display     Input2Ref";

const char s00_LINPK[] PROGMEM = " PC CONNECTION";
const char s01_LINPK[] PROGMEM = "test:";
const char s02_LINPK[] PROGMEM = "bytes read from EEPROM     ";
const char s03_LINPK[] PROGMEM = "bytes read    :";
const char s04_LINPK[] PROGMEM = "bytes write in EEPROM      ";
const char s05_LINPK[] PROGMEM = "bytes write   :";
const char s06_LINPK[] PROGMEM = "Error ,check speed connection        ";
const char s07_LINPK[] PROGMEM = "Error num :";
const char s08_LINPK[] PROGMEM = "Number of errors :";

const char sOFF[] PROGMEM = "Shut down in    : ";

const char sSunday[] PROGMEM    = "sunday     ";
const char sMonday[] PROGMEM    = "monday     ";
const char sTuesday[] PROGMEM   = "tuesday    ";
const char sWednesday[] PROGMEM = "wednesday  ";
const char sThursday[] PROGMEM  = "thursday   ";
const char sFriday[] PROGMEM    = "friday     ";
const char sSaturday[] PROGMEM  = "saturday   ";

#if (configUSE_Circle_holes == 1)
const char sInpointRadius[][25] PROGMEM = {{" CIRCLE PATTERNS        "},
{" Pattern type  "},
{" Number of holes "},
{" ----- Center-----"},
{""},
{""},
{" -----------------"},
{" Radius"},
{" Start angle   "},
{" Step angle"},
{" Beeper"}};
#endif
const char s00_lh[] PROGMEM =  " LINEAR PATTERNS    ";
const char s01_lh[] PROGMEM =  " Pattern type" ;
const char s02_lh[] PROGMEM =  " --1st hole-------" ;
const char s03_lh[] PROGMEM =  " Numbers of holes per row" ;
const char s04_lh[] PROGMEM =  " Hole spacing   " ;
const char s05_lh[] PROGMEM =  " Number of rows" ;
const char s06_lh[] PROGMEM =  " Rows spacing  " ;
const char s07_lh[] PROGMEM =  " Tilt angle" ;
const char s08_lh[] PROGMEM =  " Beeper" ;
const char sCorr[][20] PROGMEM = {{" КОРРЕКЦИЯ"},
{" Снятие коррекции"},
{" Работа с таблицами"}};
const char sIzmer[][20] PROGMEM = {{" ИЗМЕРЕНИЯ"},
{" Индикатор"},
{" График"}};
const char sOption[][24] PROGMEM = {{" SETTINGS"},
{" Screen brightness"},
{" Show clock"},
{" USB speed"},
{" Beep sound"},
{" Counters refresh speed"},
{" Encoders fault hold"}};
const char sPultOcb[][14] PROGMEM = {{" PANEL - AXIS"},
{" Р2.0  - "},
{" Р2.1  - "},
{" Р2.2  - "},
{" Р2.3  - "}};
const char sAxis_symbol[][13] PROGMEM = {{" X(linear)"},
{" Y(linear)"},
{" Z(linear)"},
{" U(linear)"},
{" V(linear)"},
{" W(linear)"},
{" P(linear)"},
{" Q(linear)"},
{" R(linear)"},
{" A(rotary)"},
{" B(rotary)"},
{" C(rotary)"},
{" D(rotary)"},
{"    off   "}};
const char  sAxis_screen[][6] PROGMEM = {{ "  X  " },
{ "  Y  " },
{ "  Z  " },
{ "  U  " },
{ "  V  " },
{ "  W  " },
{ "  P  " },
{ "  Q  " },
{ "  R  " },
{ "  A  " },
{ "  B  " },
{ "  C  " },
{ "  D  " },
{ " off " }};
//==ДОБАВИТЬ СТРОКИ СЮДА ДЛЯ ПЕРЕВОДА
#endif //========================================================end ENG========================================================================
//==============================================================================================================================================




//=======================================================Строки для системы управления==========================================================
const char sProg_limit[] PROGMEM = "Программный ограничитель   ";
const char sDiagCorrInstrument[] PROGMEM = { " Таблица инструментов" };
const char sG1header[] PROGMEM = { " ПОДАЧА G1" };
const char sG0header[] PROGMEM = { " УСКОРЕННЫЙ ХОД G0" };
const char sABCDheader[] PROGMEM = { "A  B  C  D" };
const char sFheader[] PROGMEM = { "F, мм/мин" };
const char sSheader[] PROGMEM = { "S, об/мин" };

const char sQuest[] PROGMEM = { "?" };
const char sZones[][11] PROGMEM = {{ "  Старт" },
								  { "  Зона  01" },
								  { "  Зона  02" },
								  { "  Зона  03" },
								  { "  Зона  04" },
								  { "  Стоп"}};
const char sAllparNCShell[][29] PROGMEM = {{" ОБЩИЕ ПАРАМЕТРЫ"},
{" Оси-Измерительные каналы"},
{" Оси-экран"},
{" Конфигурация входов"},
{" Конфигурация выходов"},
{" Вне допуска -"},
{" СБРОС ПАРАМЕТРОВ"},
{" СБРОС СМЕЩЕНИЙ"},
{" ПЕРЕЗАГРУЗКА"} };

const char sSpdZ[][19] PROGMEM = {
	{" ЗОНЫ СКОРОСТИ"},
	{" Подача G1"},
	{" Ускоренный ход G0"},
	{" Выход в РМ"},
	{" Выборка люфта"},
};

const char sPrgOrSubPrg[][14] PROGMEM ={
	{" Программа"},
	{" Подпрограмма"}
};

const char sMoverCntrl[][28] PROGMEM = {
	{" ПАРАМЕТРЫ ОСИ"},
	{" Задание на привод, имп/об"},
	{" Перемещение оси, мм/об"},
	{" Макс скорость, мм/мин"},
	{" Нач скорость, мм/мин"},
	{" Разгон/Тормоз ,сек"},
	{" Контроль положения"},
	{" Направление,Dir"},
	{" Сигнал"},
};
const char sHandWeelCntrl[][28] PROGMEM = {
	{" МАХОВИК ОСИ"},
	{" Разрешение, имп/об"},
	{" Перемещение оси, мм/об"},
	{" Коэфф усиления"},
};

const char  sAxis_symbol_sw[][13] PROGMEM = {
	{" X(слепая)  "},
	{" Y(слепая)  "},
	{" Z(слепая)  "},
	{" U(слепая)  "},
	{" V(слепая)  "},
	{" W(слепая)  "},
	{" P(слепая)  "},
	{" Q(слепая)  "},
	{" R(слепая)  "},
	{" A(слепая)  "},
	{" B(слепая)  "},
	{" C(слепая)  "},
	{" D(слепая)  "},
	{"    нет     "}
};
