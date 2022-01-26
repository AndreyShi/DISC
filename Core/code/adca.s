
/*
 * adca.s
 *
 * Created: 15.06.2017 14:31:11
 *  Author: Андрей
 */ 

 
//#include <ioavr.h>
#include <avr/io.h>
;08.10.2018
.global get_adcal_calibration_low
.global get_adcal_calibration_high
;22.06.2016 
.global  delay_1_ms
.global  delay_mks
.global  to_count_ones_in_byte
get_adcal_calibration_low:
  LDI R30,0x20
  LDI R31,0x00
;NVM_CMD = 0x01CA	//< Command  NVM_CMD is kept into this adress
  LDI R18,0x02
  STS 0x01ca,r18
  LPM R0,Z
  MOV R24,R0
  LDI R18,0x00
  STS 0x01ca,r18
;  CLR R25
;  COM R25
  RET
get_adcal_calibration_high:
  LDI R30,0x21
  LDI R31,0x00
;NVM_CMD = 0x01CA	//< Command  NVM_CMD is kept into this adress
  LDI R18,0x02
  STS 0x01ca,r18
  LPM R0,Z
  MOV R24,R0
  LDI R18,0x00
  STS 0x01ca,r18
;  CLR R25
;  COM R25
  RET
;if one cycle is 32.5 ns (32 mgh)
delay_1_ms:
  CLR R18
  LDI R19,62
loop_0:  INC R18
  SBRS R18,7  
  RJMP loop_0
  CLR R18
  DEC R19
  BRNE loop_0
  LDI R19,17
loop_2:  DEC R19
  BRNE loop_2
  RET
 ;-------------------
delay_mks:
//25.09.2017 R25-high byte,R24-low byte
//check,if both are 0 or mines exit(loop_8).
//if R25 is not 0 go to loop_3
loop_7:  TST R25
  BRNE loop_3
  TST R24
  BREQ loop_8
 ; SBRC R25,7
 ; RJMP loop_8
  NOP
  NOP
  CPI R24,0x01
  BREQ loop_4
loop_5:LDI R18,4
       NOP
	   NOP
	   NOP
	   NOP
	   NOP
loop_1:DEC R18
       NOP
	   BRNE loop_1
       DEC R24
       BRNE loop_7
       RET
;  DEC R25
;  BRNE loop_1
loop_3:SER R20
loop_10:LDI R19,0
       LDI R18,0x01
	   SUB R24,R18
	   SBC R25,R19
	   TST R20
	   BRNE loop_11
	   LDI R20,5
loop_13:DEC R20
	   TST R20
	   BRNE loop_13
	   NOP
loop_12:TST R25
	   BRNE loop_9
	   TST R24
	   BRNE loop_10 
       RET 
;------------------
loop_9:RJMP loop_10
;------------------
loop_4:LDI R18,4
loop_6:DEC R18
	   BRNE loop_6
	   RET 
;-----------------
loop_8: RET
;-----------------
loop_11: CLR R20
       NOP
	   NOP
	   NOP
       RJMP loop_12
;--------------------
 to_count_ones_in_byte: 
  CLR R18
  SBRC R24,0 
  INC R18
  SBRC R24,1 
  INC R18
  SBRC R24,2 
  INC R18 
  SBRC R24,3 
  INC R18  
  SBRC R24,4 
  INC R18 
  SBRC R24,5 
  INC R18 
  SBRC R24,6 
  INC R18
  SBRC R24,7 
  INC R18 
  MOV R24,R18  
  RET      
   