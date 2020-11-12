;cpu_s.asm
;Author: Straight Coder<simpleisrobust@gmail.com>
;Date: Sept 01, 2020

;********************************************************************************************************
;                                      PUBLIC FUNCTIONS
;********************************************************************************************************

	EXPORT  CPU_IntDis
	EXPORT  CPU_IntEn
		
	EXPORT  CPU_Nop

	EXPORT  CPU_SR_Save
	EXPORT  CPU_SR_Restore


;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

	AREA |.text|, CODE, READONLY, ALIGN=2
	THUMB
	REQUIRE8
	PRESERVE8

;********************************************************************************************************
;                                    FUNCTIONS
;              void  CPU_IntEn (void);
;********************************************************************************************************

CPU_IntDis
	CPSID   I
	BX      LR


CPU_IntEn
	CPSIE   I
	BX      LR

CPU_Nop
	NOP
	BX      LR

CPU_SR_Save
	MRS     R0, PRIMASK
	CPSID   I
	BX      LR

CPU_SR_Restore
	MSR     PRIMASK, R0
	BX      LR

	END

