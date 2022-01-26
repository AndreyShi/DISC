
/*
 * work_with_altera.s
 *
 * Created: 06.03.2018 14:37:55
 *  Author: Dmitry
 */
 #include <avr/io.h>
.global read_counter_step_0 
.global read_counter_step_1
.global read_counter_step_2  
read_counter_step_0:
;PORTR.OUTSET=0x01;
LDI     R18, 1
STS     2021, R18
;PORTE.OUT=LATCH_CNT R24
MOV     R18, R24
STS     1668, R18
; PORTE.DIR=0xff output
LDI     R18, 255
STS     1664, R18
;PORTR.OUTCLR=0x02;
LDI     R18, 2
STS     2022, R18
;PORTR.OUTSET=0x02;
LDI     R18, 2
STS     2021, R18
; PORTE.OUT=0x01;
LDI     R18, 1
STS     1668, R18
;PORTR.OUTCLR=0x02;
LDI     R18, 2
STS     2022, R18
;PORTE.OUT=h;
STS     1668, R22
;PORTR.OUTCLR=0x02;
LDI     R18, 2
STS     2022, R18
;PORTR.OUTSET=0x02;
LDI    R18, 2
STS    2021, R18
//------------------//очистка буфера PORTE 
// все ноги порта PORTE нужно поставить в 0 ,прежде чем считывать с него значение 
LDI     R18, 0
STS     1668, R18 
//------------------
;PORTE.DIR=0x00 input
LDI     R18, 0
STS     1664, R18
;PORTR.OUTCLR=0x01;
LDI     R18, 1
STS     2022, R18
;PORTR.OUTCLR=0x02;
LDI     R18, 2
STS     2022, R18
;__delay_cycles(4)
NOP
NOP
NOP
NOP
; buf[2] =PORTE.IN;
LDS     R24, 1672
;PORTR.OUTSET=0x01;
LDI     R18, 1
STS     2021, R18
;PORTE.DIR=0xff output
LDI     R18, 255
STS     1664, R18
RET
read_counter_step_1:
;PORTE.OUT=m;
STS     1668, R24
;PORTR.OUTCLR=0x02;
LDI     R18, 2
STS     2022, R18
;PORTR.OUTSET=0x02;
LDI    R18, 2
STS    2021, R18
//------------------//очистка буфера PORTE
// все ноги порта PORTE нужно поставить в 0 ,прежде чем считывать с него значение 
LDI     R18, 0
STS     1668, R18 
//------------------
;PORTE.DIR=0x00 input
LDI     R18, 0
STS     1664, R18
;PORTR.OUTCLR=0x01
LDI     R18, 1
STS     2022, R18
;PORTR.OUTCLR=0x02
LDI     R18, 2
STS     2022, R18
;__delay_cycles(4)
NOP
NOP
NOP
NOP
; buf[1] =PORTE.IN;
LDS     R24, 1672
;PORTR.OUTSET=0x01;
LDI     R18, 1
STS     2021, R18
;PORTE.DIR=0xff output
LDI     R18, 255
STS     1664, R18
RET
read_counter_step_2:
;PORTE.OUT=l;
STS     1668, R24
;PORTR.OUTCLR=0x02;
LDI     R18, 2
STS     2022, R18
;PORTR.OUTSET=0x02;
LDI    R18, 2
STS    2021, R18
//------------------//очистка буфера PORTE
// все ноги порта PORTE нужно поставить в 0 ,прежде чем считывать с него значение 
LDI     R18, 0
STS     1668, R18 
//------------------
;PORTE.DIR=0x00 input
LDI     R18, 0
STS     1664, R18
;PORTR.OUTCLR=0x01
LDI     R18, 1
STS     2022, R18
;PORTR.OUTCLR=0x02
LDI     R18, 2
STS     2022, R18
;__delay_cycles(4)
NOP
NOP
NOP
NOP
; buf[0] =PORTE.IN;
LDS     R24, 1672
;PORTR.OUTSET=0x01;
LDI     R18, 1
STS     2021, R18
;PORTE.DIR=0xff output
LDI     R18, 255
STS     1664, R18
RET