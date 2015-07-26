.NOCODES
;=====================================================================
; This is all the I/O Decision Stuff
;=====================================================================
.CODES



;====  Setup I/O Devices  ====
iniz_IOdevices
			lda #SERIAL			; Serial is default I/O
			sta STDIN
			lda #SCREEN
			sta STDOUT
			rts
			
			
;====  Decide which Output Device to use  ====
PutC
			pha					; Save char 
			lda STDOUT			; Get curr STDOUT dev#
			cmp #SERIAL			; Is it serial?
			bne isScreen_pc		; No, go check for screen
			pla					; restore char
			jsr term_putc			; send it out the serial port
			rts
isScreen_pc
			cmp #SCREEN			; Is it the screen?
			bne isBoth_pc			; No, go check for both
			pla					; restore char
			jsr scr_putc			; and send it to the screen
			rts
isBoth_pc
			cmp #BOTH			; Is it both??
			bne isErr_pc			; No, Huston we have a problem
			pla					; restore and save the char
			pha
			jsr term_putc			; send it to the serial port
			pla
			jsr scr_putc			; and then the screen, just 'cos we can!
			rts
isErr_pc							; unknown device - Should never get here!!
			pla					; take the char of the stack
			rts					; and exit - Error msg??
			
			
;====  Decide which input device to use  ====
;====  Currently only one device - the serial keyboard :(  ====
GetC
			lda STDIN			; Get the curr STDIN device #
			cmp #SERIAL
			bne isErr_gc			; Not serial!!!! How? Why?
			jsr term_getc
			rts
isErr_gc							; Err msg??
			rts
			
			
;====  Device specific Screen Clear  ====
Pr_ClrScr
			lda STDOUT
			cmp #SERIAL
			bne isScreen_pcs
			jsr term_clear
			rts
isScreen_pcs
			cmp #SCREEN
			bne isErr_pcs
			jsr scr_clear
			rts
isErr_pcs
			rts
			
