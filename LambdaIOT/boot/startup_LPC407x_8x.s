;/*****************************************************************************
; * @file:    startup_LPC407x_8x.s
; * @purpose: CMSIS Cortex-M4 Core Device Startup File
; *           for the NXP LPC407x_8x Device Series 
; * @version: V1.20
; * @date:    16. January 2012
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------ 
; *
; * Copyright (C) 2012 ARM Limited. All rights reserved.
; * ARM Limited (ARM) is supplying this software for use with Cortex-M4
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; *****************************************************************************/

;	IMPORT TIMER2_IRQHandler
	IMPORT SysTick_Handler
	IMPORT RTC_IRQHandler
;	IMPORT PendSV_Handler

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00001000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000400

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors

__Vectors		DCD		__initial_sp              ; Top of Stack
				DCD		Reset_Handler             ; Reset Handler
				DCD		NMI_Handler               ; NMI Handler
				DCD		HardFault_Handler         ; Hard Fault Handler
				DCD		MemManage_Handler         ; MPU Fault Handler
				DCD		BusFault_Handler          ; Bus Fault Handler
				DCD		UsageFault_Handler        ; Usage Fault Handler
			;	DCD		0xEFFFF5D6                ; Reserved- vector sum
				DCD		0xEFFFF39E                ; Reserved- vector sum
				DCD		0                         ; Reserved
				DCD		0                         ; Reserved
				DCD		0                         ; Reserved
				DCD		SVC_Handler               ; SVCall Handler
				DCD		DebugMon_Handler          ; Debug Monitor Handler
				DCD		0                         ; Reserved
				DCD		PendSV_Handler      	  ; PendSV Handler
				DCD		SysTick_Handler     	  ; SysTick Handler
				
				; External Interrupts
				DCD		WDT_IRQHandler            ; 16: Watchdog Timer
				DCD		TIMER0_IRQHandler         ; 17: Timer0
				DCD		TIMER1_IRQHandler         ; 18: Timer1
				DCD		TIMER2_IRQHandler         ; 19: Timer2
				DCD		TIMER3_IRQHandler         ; 20: Timer3
				DCD		UART0_IRQHandler          ; 21: UART0
				DCD		UART1_IRQHandler          ; 22: UART1
				DCD		UART2_IRQHandler          ; 23: UART2
				DCD		UART3_IRQHandler          ; 24: UART3
				DCD		PWM1_IRQHandler           ; 25: PWM1
				DCD		I2C0_IRQHandler           ; 26: I2C0
				DCD		I2C1_IRQHandler           ; 27: I2C1
				DCD		I2C2_IRQHandler           ; 28: I2C2
				DCD		0						  ; 29: reserved, not for SPIFI anymore
				DCD		SSP0_IRQHandler           ; 30: SSP0
				DCD		SSP1_IRQHandler           ; 31: SSP1
				DCD		PLL0_IRQHandler           ; 32: PLL0 Lock (Main PLL)
				DCD		RTC_IRQHandler            ; 33: Real Time Clock
				DCD		EINT0_IRQHandler          ; 34: External Interrupt 0
				DCD		EINT1_IRQHandler          ; 35: External Interrupt 1
				DCD		EINT2_IRQHandler          ; 36: External Interrupt 2
				DCD		EINT3_IRQHandler          ; 37: External Interrupt 3
				DCD		ADC_IRQHandler            ; 38: A/D Converter
				DCD		BOD_IRQHandler            ; 39: Brown-Out Detect
				DCD		USB_IRQHandler            ; 40: USB
				DCD		CAN_IRQHandler            ; 41: CAN
				DCD		DMA_IRQHandler            ; 42: General Purpose DMA
				DCD		I2S_IRQHandler            ; 43: I2S
				DCD		ENET_IRQHandler           ; 44: Ethernet
				DCD		MCI_IRQHandler            ; 45: SD/MMC card I/F
				DCD		MCPWM_IRQHandler          ; 46: Motor Control PWM
				DCD		QEI_IRQHandler            ; 47: Quadrature Encoder Interface
				DCD		PLL1_IRQHandler           ; 48: PLL1 Lock (USB PLL)
				DCD		USBActivity_IRQHandler    ; 49: USB Activity interrupt to wakeup
				DCD		CANActivity_IRQHandler    ; 50: CAN Activity interrupt to wakeup
				DCD		UART4_IRQHandler          ; 51: UART4
				DCD		SSP2_IRQHandler           ; 52: SSP2
				DCD		LCD_IRQHandler            ; 53: LCD
				DCD		GPIO_IRQHandler           ; 54: GPIO
				DCD		PWM0_IRQHandler           ; 55: PWM0
				DCD		EEPROM_IRQHandler         ; 56: EEPROM
				
                AREA    |.ARM.__at_0x02FC|, CODE, READONLY
				IF :DEF:EN_CRP
					DCD     0x87654321
				ELSE
					DCD     0xFFFFFFFF
				ENDIF

;#if CRP_LEVEL == 0	    (void *)0xFFFFFFFF
;#if CRP_LEVEL == 1	    (void *)0x12345678
;#if CRP_LEVEL == 2	    (void *)0x87654321
;#if CRP_LEVEL == 3	    (void *)0x43218765
				
;				IF      :LNOT::DEF:NO_CRP
;				AREA    |.ARM.__at_0x02FC|, CODE, READONLY
;CRP_Key			DCD     0xFFFFFFFF
;				ENDIF
				
				
				AREA    |.text|, CODE, READONLY
				
; Reset Handler
INITSDRAM		EQU 	1
	
Reset_Handler	PROC
				EXPORT  Reset_Handler             [WEAK]
				IMPORT  SystemInit
				IMPORT  __main
					
				IF (INITSDRAM == 1)
;Configure for DATA LINES PIN 0..31
				LDR    	R0,=0x00000001
				LDR    	R1,=0x4002C180
				LDR    	R2,=0x4002C200
INITDATALINE								
				STR     R0,[R1,#0X00]
				ADD  	R1,R1,#4
				CMP     R1,R2
				BNE     INITDATALINE

;Configure for ADRESS LINS PIN 0..21
				LDR    	R0,=0x00000001
				LDR    	R1,=0x4002C200
				LDR    	R2,=0x4002C258
INITADDRLINE				
				STR     R0,[R1,#0X00]
				ADD  	R1,#4
				CMP     R1,R2
				BNE     INITADDRLINE

				;//EMC CONTROL SIGNAL PIN
				LDR    R0,=0x00000001	
				LDR    R1,=0x4002C260	;P4.24 - EMC_OE
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001	
				LDR    R1,=0x4002C264	;P4.25 - EMC_WE
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001	
				LDR    R1,=0x4002C278	;P4.30 - EMC_CS0
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001	
				LDR    R1,=0x4002C27C	;P4.31 - EMC_CS1
				STR            R0,[R1,#0X00]

				LDR    R0,=0x00000001
				LDR    R1,=0x4002C140	;P2.16 - EMC_CAS
				STR            R0,[R1,#0X00]

				LDR    R0,=0x00000001
				LDR    R1,=0x4002C144	;P2.17 - EMC_RAS
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x4002C148	;P2.18 - EMC_CLK[0]
				STR            R0,[R1,#0X00]

				LDR    R0,=0x00000001
				LDR    R1,=0x4002C150	;P2.20 - EMC_DYCS0
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x4002C160	;P2.24 - EMC_CKE0
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x4002C170	;P2.28 - EMC_DQM0
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x4002C174	;P2.29 - EMC_DQM1
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x4002C178	;P2.30 - EMC_DQM2
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x4002C17C	;P2.31 - EMC_DQM3
				STR            R0,[R1,#0X00]

;Initial for SYSCLK
				LDR    R0,=0x00000020
				LDR    R1,=0x400FC1A0	;SCS
				STR            R0,[R1,#0X00]
				
Wait_SCSReady
				LDR     R2, [R1]
				ANDS    R2, R2, #0X40
				BEQ		Wait_SCSReady
				LDR    R0,=0x00000001
				LDR    R1,=0x400FC10C	;CLKSRCSEL
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000009
				LDR    R1,=0x400FC084	;PLL0CFG
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x400FC080	;PLL0CON
				STR            R0,[R1,#0X00]
				LDR    R0,=0x000000AA
				LDR    R1,=0x400FC08C	;PLL0FEED
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000055
				LDR    R1,=0x400FC08C	;PLL0FEED
				STR            R0,[R1,#0X00]

				LDR	   	R1,=0x400FC088
WaitPLOCK0		
				LDR     R2, [R1]
				ANDS    R2, R2, #0X400
				BEQ		WaitPLOCK0
				
				LDR    R0,=0x00000023
				LDR    R1,=0x400FC0A4	;PLL1CFG
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x400FC0A0	;PLL1CON
				STR            R0,[R1,#0X00]
				LDR    R0,=0x000000AA
				LDR    R1,=0x400FC0AC	;PLL1FEED
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000055
				LDR    R1,=0x400FC0AC	;PLL1FEED
				STR            R0,[R1,#0X00]

				LDR	   	R1,=0x400FC0A8
WaitPLOCK1				
				LDR     R2, [R1]
				ANDS    R2, R2, #0X400
				BEQ		WaitPLOCK1

				LDR    R0,=0x00000101
				LDR    R1,=0x400FC104	;CCLKSEL
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000201
				LDR    R1,=0x400FC108	;USBCLKSEL
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x400FC100	;EMCCLKSEL
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000002
				LDR    R1,=0x400FC1B4	;SPIFICLKSEL
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000002
				LDR    R1,=0x400FC1A8	;PCLKSEL
				STR            R0,[R1,#0X00]
				LDR    R0,=0x83E0CF5E
				LDR    R1,=0x400FC0C4	;PCONP // Power On EMC
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000100
				LDR    R1,=0x400FC1C8	;CLKOUTCFG
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000003
				LDR    R1,=0x400FC1B0	;PBOOST
				STR            R0,[R1,#0X00]
				LDR    R0,=0x0000503A
				LDR    R1,=0x400FC000	;FLASHCFG
				STR            R0,[R1,#0X00]
;Initial for EMC&SDRAM
				LDR    R0,=0x00000001
				LDR    R1,=0x2009C000	;EMCCONTROL // Enable EMC 
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00081818
				LDR    R1,=0x400FC1DC	;EMCDLYCTL
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000000
				LDR    R1,=0x2009C008	;EMCCONFIG
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000003
				LDR    R1,=0x2009C020	;DYNAMICCONTROL
				STR            R0,[R1,#0X00]
				
				LDR    R0,=0x0000027D
				LDR    R1,=0x2009C024	;DYNAMICREFRESH
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000001
				LDR    R1,=0x2009C028	;DYNAMICREADCONFIG
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000003
				LDR    R1,=0x2009C030	;DYNAMICRP
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000004
				LDR    R1,=0x2009C034	;DYNAMICRAS
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000004
				LDR    R1,=0x2009C038	;DYNAMICSREX
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000006
				LDR    R1,=0x2009C03C	;DYNAMICAPR
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000006
				LDR    R1,=0x2009C040	;DYNAMICDAL
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000003
				LDR    R1,=0x2009C044	;DYNAMICWR
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000006
				LDR    R1,=0x2009C048	;DYNAMICRC
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000006
				LDR    R1,=0x2009C04C	;DYNAMICRFC
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000004
				LDR    R1,=0x2009C050	;DYNAMICXSR
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000003
				LDR    R1,=0x2009C054	;DYNAMICRRD
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000002
				LDR    R1,=0x2009C058	;DYNAMICMRD
				STR            R0,[R1,#0X00]
				LDR    R0,=0x00000201
				LDR    R1,=0x2009C104	;DYNAMICCASRAS0
				STR            R0,[R1,#0X00]
				LDR    	R0,=0x00084480
				LDR    	R1,=0x2009C100	;DYNAMICCONFIG0
				STR            R0,[R1,#0X00]

				LDR    	R0,=0x00000183
				LDR    	R1,=0x2009C020	;DYNAMICCONTROL for nop command
				STR            R0,[R1,#0X00]
				LDR     R6, =100000*52                 ; 100000us at 120 MHz
delay100_1          
				SUBS    R6, R6, #1
                BNE     delay100_1
				
				LDR    	R0,=0x00000103
				LDR    	R1,=0x2009C020	;DYNAMICCONTROL for Issue Pre-charge command
				STR            R0,[R1,#0X00]
				
				LDR     R6, =1000*52                 ; 1000us at 120 MHz
delay100_2          
				SUBS    R6, R6, #1
                BNE     delay100_2

				LDR    	R0,=0x00000083
				LDR    	R1,=0x2009C020	;DYNAMICCONTROL for mode command
				STR            R0,[R1,#0X00]

				LDR		R0, =0xA0000000
				ORR		R0,R0,#0x44000
				LDR		R1,[R0]
				
				LDR    	R0,=0x00000003
				LDR    	R1,=0x2009C020	;DYNAMICCONTROL for noamal command
				STR            R0,[R1,#0X00]
				LDR    	R0,=0x00084280
				LDR    	R1,=0x2009C100	;DYNAMICCONFIG0
				STR            R0,[R1,#0X00]
				ENDIF
				LDR     R0, =SystemInit
				BLX     R0
				
                IF {FPU} != "SoftVFP"
                                                ; Enable Floating Point Support at reset for FPU
                LDR.W   R0, =0xE000ED88         ; Load address of CPACR register
                LDR     R1, [R0]                ; Read value at CPACR
                ORR     R1,  R1, #(0xF <<20)    ; Set bits 20-23 to enable CP10 and CP11 coprocessors
                                                ; Write back the modified CPACR value
                STR     R1, [R0]                ; Wait for store to complete
                DSB
                
                                                ; Disable automatic FP register content
                                                ; Disable lazy context switch
                LDR.W   R0, =0xE000EF34         ; Load address to FPCCR register
                LDR     R1, [R0]
                AND     R1,  R1, #(0x3FFFFFFF)  ; Clear the LSPEN and ASPEN bits
                STR     R1, [R0]
                ISB                             ; Reset pipeline now the FPU is enabled
                ENDIF
					
				LDR     R0, =__main
				BX      R0
				ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler		PROC
				EXPORT  NMI_Handler               [WEAK]
				B       .
				ENDP
HardFault_Handler\
                PROC
				;EXPORT  HardFault_Handler         [WEAK]
				;B       .
				IMPORT hard_fault_handler_c
				TST LR, #4  	   ; Test EXC_RETURN number in LR bit 2
				ITE EQ  		   ; if zero (equal) then
				MRSEQ R0, MSP  	   ; Main Stack was used, put MSP in R0
				MRSNE R0, PSP  	   ; else, Process Stack was used, put PSP
				B hard_fault_handler_c ; call the printout routine 
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
				PROC
				EXPORT  UsageFault_Handler        [WEAK]
				B       .
				ENDP
SVC_Handler		PROC
				EXPORT  SVC_Handler               [WEAK]
				B       .
				ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler	PROC
				EXPORT  PendSV_Handler      [WEAK]
				B       .
				ENDP

Default_Handler PROC
				
				EXPORT  WDT_IRQHandler            [WEAK]
				EXPORT  TIMER0_IRQHandler         [WEAK]
				EXPORT  TIMER1_IRQHandler         [WEAK]
				EXPORT  TIMER2_IRQHandler         [WEAK]
				EXPORT  TIMER3_IRQHandler         [WEAK]
				EXPORT  UART0_IRQHandler          [WEAK]
				EXPORT  UART1_IRQHandler          [WEAK]
				EXPORT  UART2_IRQHandler          [WEAK]
				EXPORT  UART3_IRQHandler          [WEAK]
				EXPORT  PWM1_IRQHandler           [WEAK]
				EXPORT  I2C0_IRQHandler           [WEAK]
				EXPORT  I2C1_IRQHandler           [WEAK]
				EXPORT  I2C2_IRQHandler           [WEAK]
				;EXPORT  SPIFI_IRQHandler          [WEAK]
				EXPORT  SSP0_IRQHandler           [WEAK]
				EXPORT  SSP1_IRQHandler           [WEAK]
				EXPORT  PLL0_IRQHandler           [WEAK]
				;EXPORT  RTC_IRQHandler            [WEAK]
				EXPORT  EINT0_IRQHandler          [WEAK]
				EXPORT  EINT1_IRQHandler          [WEAK]
				EXPORT  EINT2_IRQHandler          [WEAK]
				EXPORT  EINT3_IRQHandler          [WEAK]
				EXPORT  ADC_IRQHandler            [WEAK]
				EXPORT  BOD_IRQHandler            [WEAK]
				EXPORT  USB_IRQHandler            [WEAK]
				EXPORT  CAN_IRQHandler            [WEAK]
				EXPORT  DMA_IRQHandler            [WEAK]
				EXPORT  I2S_IRQHandler            [WEAK]
				EXPORT  ENET_IRQHandler           [WEAK]
				EXPORT  MCI_IRQHandler            [WEAK]
				EXPORT  MCPWM_IRQHandler          [WEAK]
				EXPORT  QEI_IRQHandler            [WEAK]
				EXPORT  PLL1_IRQHandler           [WEAK]
				EXPORT  USBActivity_IRQHandler    [WEAK]
				EXPORT  CANActivity_IRQHandler    [WEAK]
				EXPORT  UART4_IRQHandler          [WEAK]
				EXPORT  SSP2_IRQHandler           [WEAK]
				EXPORT  LCD_IRQHandler            [WEAK]
				EXPORT  GPIO_IRQHandler           [WEAK]
				EXPORT  PWM0_IRQHandler           [WEAK]
				EXPORT  EEPROM_IRQHandler         [WEAK]

WDT_IRQHandler
TIMER0_IRQHandler
TIMER1_IRQHandler
TIMER2_IRQHandler
TIMER3_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
UART3_IRQHandler
PWM1_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
I2C2_IRQHandler
;SPIFI_IRQHandler	;not used            
SSP0_IRQHandler
SSP1_IRQHandler
PLL0_IRQHandler
;RTC_IRQHandler
EINT0_IRQHandler
EINT1_IRQHandler
EINT2_IRQHandler
EINT3_IRQHandler
ADC_IRQHandler
BOD_IRQHandler
USB_IRQHandler
CAN_IRQHandler
DMA_IRQHandler
I2S_IRQHandler
ENET_IRQHandler
MCI_IRQHandler          
MCPWM_IRQHandler
QEI_IRQHandler
PLL1_IRQHandler
USBActivity_IRQHandler
CANActivity_IRQHandler
UART4_IRQHandler
SSP2_IRQHandler
LCD_IRQHandler
GPIO_IRQHandler
PWM0_IRQHandler
EEPROM_IRQHandler
					
				B       .
				
				ENDP
				
				
				ALIGN
					

; User Initial Stack & Heap
				
				IF      :DEF:__MICROLIB
				
				EXPORT  __initial_sp
				EXPORT  __heap_base
				EXPORT  __heap_limit
				
				ELSE
				
				IMPORT  __use_two_region_memory
				EXPORT  __user_initial_stackheap
__user_initial_stackheap

				LDR     R0, =  Heap_Mem
				LDR     R1, =(Stack_Mem + Stack_Size)
				LDR     R2, = (Heap_Mem +  Heap_Size)
				LDR     R3, = Stack_Mem
				BX      LR
				
				ALIGN
				
				ENDIF
				
				
				END
