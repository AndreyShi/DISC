
/*
 * work_with_display.s
 *
 * Created: 26.12.2017 16:16:52
 *  Author: Dmitry
 */
#include <avr/io.h>
.global write_c_asm
.global write_d_asm
.global a0_to_one_cs_to_zero_asm
.global cs_to_one_asm
.global write_data_asm
.global only_write_pulse_asm
;04.03.2018 
.global to_check_data_80000000
 write_c_asm:
;disable interrupt
 BCLR 7
 ;clr_A0()
 LDI R18,64
 STS 1542,R18
 ;clr_CS()
 LDI R18,32
 STS 1542,R18
 ;clr_WR()
 LDI R19,128
 STS 1542,R19
 ;PORTF_OUT=cmd
 STS 1700,R24
 ;set_WR()
 LDI R19,128
 STS 1541,R19
 ;set_CS()
 LDI R19,32
 STS 1541,R19
 ;enable interrupt
 BSET 7
 ; 
  RET
write_d_asm: 
;disable interrupt
 BCLR 7
 ;set_A0()
 LDI R18,64
 STS 1541,R18
 ;clr_CS()
 LDI R18,32
 STS 1542,R18
 ;clr_WR()
 LDI R19,128
 STS 1542,R19
 ;PORTF_OUT=data
 STS 1700,R24
 ;set_WR()
 LDI R19,128
 STS 1541,R19
 ;set_CS()
 LDI R19,32
 STS 1541,R19
 ;enable interrupt
 BSET 7
  RET  
a0_to_one_cs_to_zero_asm:
;disable interrupt
 BCLR 7
;set_A0()
 LDI R18,64
 STS 1541,R18
 ;clr_CS()
 LDI R18,32
 STS 1542,R18
 BSET 7
 RET
 cs_to_one_asm: 
 ;disable interrupt
 BCLR 7
 ;set_CS()
 LDI R19,32
 STS 1541,R19
 ;enable interrupt
 BSET 7
  RET 
  write_data_asm: 
  ;disable interrupt
 BCLR 7
  ;clr_WR()
 LDI R19,128
 STS 1542,R19
  ;PORTF_OUT=data
 STS 1700,R24
 ;set_WR()
 LDI R19,128
 STS 1541,R19
 ;enable interrupt
 BSET 7
  RET 
only_write_pulse_asm:
 BCLR 7
  ;clr_WR()
 LDI R19,128
 STS 1542,R19
  ;PORTF_OUT=data
 ;STS 1700,R24
 ;set_WR()
 LDI R19,128
 STS 1541,R19
 ;enable interrupt
 BSET 7
  RET 
to_check_data_80000000:
;byte order for long r25,r24,r23,r22
CPI R25,0x80
BRNE OK
CPI R24,0x00
BRNE OK
CPI R23,0x00
BRNE OK
CPI R22,0x00
BRNE OK
;it is 0x80000000
SER R24
;SER R25
RET
OK:
CLR R24
;CLR R25
RET
