.NOCODES
;=====================================================================
; assemble with tasm -65 -c  -b -f00 test_01_loop.asm test_01_loop.bin
;=====================================================================
; PURPOSE: To test the memory-I/O decoding for eeprom
;=====================================================================
.CODES

.NOCODES
;=====================================================================
; Memory Map Constants
;=====================================================================
.CODES

; 28256 EEPROM - 31K
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
			
DOLOOP
			nop
			jmp DOLOOP		; do nothing forever

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