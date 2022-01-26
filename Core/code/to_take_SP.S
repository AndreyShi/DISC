
/*
 * to_take_SP.S
 *
 * Created: 13.09.2017 17:02:46
 *  Author: Dmitry
 */ 
#include <avr/io.h>
.global to_take_SP
.global to_fill_SP_memory
.global to_check_SP_memory
.global to_take_address
.global to_get_byte_from_RAM
.global return_after_real_calculate
.global restore_y_pair
 to_take_SP:
;it is low byte of SP
IN R24,0X3D
;it is high byte of SP
IN R25,0X3E
RET
 to_fill_SP_memory:
;it is low byte of SP
IN R24,0X3D
;it is high byte of SP
IN R25,0X3E
;start memory fill from here
LDI R26,0x00
LDI R27,0x20
CLR R18
loop: ST X+,R18
INC R18
; SP must be the same with X
CP R26,R24
BRNE loop
CP R27,R25
BRNE loop
RET
to_check_SP_memory:
;05.01.2018 from 0x2000 till 0x24f5 it takes 10176 cycles
;10176*31.25=318000 ns mistake takes 318*4=1272 mks
;start memory check from here
LDI R26,0x00
LDI R27,0x20
SER R18
loop_:INC R18
;stack is from 2000 till 27ff (2 kb) or
;from 2000 till 2bff (3 kb) or
;from 2000 till 2fff (4 kb)
;check for high byte of address.Bit 4 is 001?****(0x3000 is limit)
SBRC R27,4
RJMP error_
LD R19,X+
CP R18,R19
BREQ loop_
;debug
;RJMP loop_
//it will be first high address byte with mistake
LD R19,-X
MOV R25,R27
//low address byte
MOV R24,R26
RET
//it will be mistake <end of stack>
error_:LDI R24,0
LDI R25,0
RET
to_take_address:
RET
 to_get_byte_from_RAM:
 //low address byte
MOV R26,R24
//hign address byte
MOV R27,R25
LD R24,X+
 RET
restore_y_pair:
MOV R28,R24
MOV R29,R25
RET	