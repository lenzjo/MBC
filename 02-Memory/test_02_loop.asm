.NOCODES
;=====================================================================
; assemble with tasm -65 -c  -b -f00 test_02_loop.asm test_02_loop.bin
;=====================================================================
; PURPOSE: To test the memory-I/O decoding for ram and eeprom
;=====================================================================
.CODES

.NOCODES
;=====================================================================
; Memory Map Constants
;=====================================================================
.CODES

; 62256 RAM - 32K
RAMBASE		= $0000
RAMLEN		= $7FFF

RAMTEST1	= $1000		; Arbitrary location for ram test
RAMTEST2	= $2000		; Arbitrary location for ram test
RAMTEST3	= $3000		; Arbitrary location for ram test
RAMTEST4	= $4000		; Arbitrary location for ram test


; 28256 EEPROM - 32K
ROMBASE     = $8000


.NOCODES
;=====================================================================
; UNUSED ROM: $8000-$83FF is used by the I/O
;=====================================================================
.CODES
			
			*= ROMBASE
			
			.FILL $400, $EA	; Fill the first k with NOPs
			
			
RST_VEC		jmp MAIN

NMI_VEC		rti

IRQ_VEC		rti

.NOCODES
;=====================================================================
; Cold reset
;=====================================================================
.CODES

MAIN
			ldx #$FF		; Iniz SP to $01FF
			txs
			lda #$00		; Iniz regs to zero
			tax
			tay
			cld				; Clear decimal mode
			clc				; Clear the carry
			cli				; Enable interrupts

; Copy SUBLOOP, SUBLOOP+5 to various random locations in ram
			ldx #00
COPYBYTE
			lda SUBLOOP,x	; Get the next byte
			sta RAMTEST1,x	; Copy it
			sta RAMTEST2,x	; Copy it
			sta RAMTEST3,x	; Copy it
			sta RAMTEST4,x	; Copy it
			inx
			cpx #06
			bne COPYBYTE
			
; SUBLOOP copied so continue
DOLOOP
			jsr RAMTEST1	; can we run in ram?
			jsr RAMTEST2	; can we run in ram again?
			jsr SUBLOOP
			jsr RAMTEST3	; can we run in ram?
			jsr SUBLOOP
			jsr RAMTEST4	; can we run in ram?
			jmp DOLOOP		; do silly stuff forever

; Relocatable code to be copied to ram
SUBLOOP
			ldx #03			; small value chosen for single step mode
SLLOOP		dex
			bne SLLOOP
			rts
			
			
.NOCODES
;=====================================================================
; Set the interupt vectors for 65C02 system 
;=====================================================================
.CODES

		*=  $FFFA 
.word	NMI_VEC  			; NMI_VECTOR   : $FFFA  (NMI) 
.word	RST_VEC            	; RESET_VECTOR : $FFFC  (Reset) 
.word	IRQ_VEC  			; IRQ_VECTOR   : $FFFE  (IRQ) 


		.END