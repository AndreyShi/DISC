/*
 * Strings.h
 *
 * Created: 10.07.2017 9:46:05
 *  Author: Андрей
 */ 


#ifndef STRINGS_H_
#define STRINGS_H_
#include "config.h"
#include <avr/pgmspace.h>

/*==============================Венгерская нотация: названия  переменных  в которых хранятся строки, начинаются с s=============================*/
//=======================================================Строки для Индикации (базовые строки)==================================================
extern const char sLine[];
extern const char sPoint[];
extern const char sP2[][5];
extern const char sP3[][5];
extern const char sProbel10[];
extern const char sProbel21[];
#if (configUSE_Lang == 0)//======================================start RUS======================================================================
extern const char sCM[];
extern const char sZona[];
extern const char s_CheckAxisParam[];
extern const char sSpd[];
extern const char sTme[];
extern const char sStack_left[];
extern const char sStack_used[];
extern const char sError[];
extern const char sYES[];
extern const char sNO[];
extern const char sSchetchik[];
extern const char sPerepolnenie_sbros[];
extern const char sSchetchik_ne_v_diapozone[];
extern const char sOsb_ne_vishla_v_0[];
extern const char sVivesti_v_0_PUSK[];
extern const char sBaudrate[][13];
extern const char sOshibka_datchika_portP2[];
extern const char sPlease_input_pass[];
extern const char sAccess_denided[];
extern const char sAxisList[];
extern const char sAxisListOffset[];
extern const char sAxisNoList[];
extern const char sRadius[];
extern const char sDiametr[];
extern const char s01sek[];
extern const char s03sek[];
extern const char s06sek[];
extern const char sOFFSET_COORDINATE[];
extern const char sFast[];
extern const char sSlow[];
extern const char sStrokaIndikacii[];
extern const char sSbrosParam[];
extern const char sSbrosParam01[];
extern const char sSbrosParam02[];
extern const char sSbrosParam03[];
extern const char sResetOffset[];
extern const char sResetOffset01[];
extern const char sResetOffset02[];
extern const char sResetOffset03[];
extern const char sCalculator[];
extern const char sPolniy[];
extern const char sSegment[];
extern const char sOpornaOsb[];
extern const char sReshetka[];
extern const char sSBRS[];
extern const char sPusk[];
extern const char sSTOP[];
extern const char sSled[];
extern const char sPred[];
extern const char sDiag[][23];
extern const char sAxisOpt[][25];
extern const char sPredel[];
extern const char sAllpar[][29];
extern const char s00_DBG[];
extern const char s01_DBG[];
extern const char s02_DBG[];
extern const char s03_DBG[];
extern const char s04_DBG[];
extern const char s05_DBG[];
extern const char s06_DBG[];
extern const char s07_DBG[];

extern const char s00_ABOU[];
extern const char s01_ABOU[];
extern const char s02_ABOU[];
extern const char s03_ABOU[];
extern const char s04_ABOU[];
extern const char s05_ABOU[];
extern const char s06_ABOU[];

extern const char s00_ENCM[];
extern const char s01_ENCM[];
extern const char s02_ENCM[];
extern const char s03_ENCM[];
extern const char s04_ENCM[];

extern const char s00_SUPM[];
extern const char s01_SUPM[];
extern const char s02_SUPM[];
extern const char sBatLow[];
extern const char sBatNoCntrl[];

extern const char s00_TIME[];
extern const char s01_TIME[];
extern const char s02_TIME[];
extern const char s03_TIME[];
extern const char s04_TIME[];

extern const char s00_VIIN[];
extern const char s01_VIIN[];
extern const char s02_VIIN[];
extern const char s03_VIIN[];

extern const char s00_LINPK[];
extern const char s01_LINPK[];
extern const char s02_LINPK[];
extern const char s03_LINPK[];
extern const char s04_LINPK[];
extern const char s05_LINPK[];
extern const char s06_LINPK[];
extern const char s07_LINPK[];
extern const char s08_LINPK[];

extern const char sOFF[];

extern const char sSunday[];
extern const char sMonday[];
extern const char sTuesday[];
extern const char sWednesday[];
extern const char sThursday[];
extern const char sFriday[];
extern const char sSaturday[];

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
extern const char s00_lh[];
extern const char s01_lh[];
extern const char s02_lh[];
extern const char s03_lh[];
extern const char s04_lh[];
extern const char s05_lh[];
extern const char s06_lh[];
extern const char s07_lh[];
extern const char s08_lh[];
extern const char sCorr[][20];
extern const char sIzmer[][20];
extern const char sOption[][23];
extern const char sPultOcb[][13];
extern const char sAxis_symbol[][13];
extern const char  sAxis_screen[][6];
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
extern const char sProg_limit[];
extern const char sDiagCorrInstrument[];
extern const char sG1header[];
extern const char sG0header[];
extern const char sABCDheader[];
extern const char sFheader[];
extern const char sSheader[];

extern const char sQuest[];
extern const char sZones[][11];
extern const char sAllparNCShell[][29];
extern const char sSpdZ[][19];
extern const char sPrgOrSubPrg[][14];
extern const char sMoverCntrl[][28];
extern const char sHandWeelCntrl[][28];

extern const char  sAxis_symbol_sw[][13];

//=================================================================================================================================================	
#endif /* STRINGS_H_ */
