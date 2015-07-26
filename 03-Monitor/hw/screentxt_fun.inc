
;====================================================================
; DRIVER AND UTILITY FUNCTIONS FOR ATMEGA328 VIDEO OUTPUT
;====================================================================

;====  Write a chr, in A, to Port A in odd Handshake mode on Port B  ====
scr_putc
			pha					; save char
get_ACK_status
			jsr get_ACK			; Get ACK status
			cmp AVAIL_REG			; is it the same as AVAIL?
			bne get_ACK_status	; go check again until it is
			
			pla					; restore char
			sta VIA_ORA			; and send it out
;			jsr WAIT_6551			; wait a little wait - test to cure screen flickering
			jsr flip_AVAIL		; and Toggle the AVAIL pin
			rts
			
			
;====  Read ACK convert to "BOOL" and store status in ACK_REG  ====
get_ACK
			lda VIA_IRB			; Get ACK
			and #ACK_BIT
			cmp #ACK_BIT			; Is it High?
			bne send_ACK_lo		; No, then go set reg LOW
send_ACK_hi
			lda #HIGH			; Set ACK reg
			sta ACK_REG
			rts
send_ACK_lo
			lda #LOW				; Clear ACK reg
			sta ACK_REG
			rts
			
			
;==== Flip the AVAIL pin  ====
flip_AVAIL
			lda AVAIL_REG
			cmp #HIGH
			bne send_AVAIL_hi		; If LO go set it HI
send_AVAIL_lo
			lda VIA_ORB			; set it LO
			and #AVAIL_MASK
			sta VIA_ORB			; and store it
			lda #LOW
			sta AVAIL_REG
			rts
send_AVAIL_hi
			lda VIA_ORB
			ora #AVAIL_BIT		; set it HI
			sta VIA_ORB
			lda #HIGH
			sta AVAIL_REG
			rts


;====================================================================
; Display control functions
;====================================================================

; Clear the screen
scr_clear
		lda #CLEAR_SCREEN
		jsr scr_putc
		rts

; Blink the cursor
scr_cursorBlink
		lda #CUR_BLINKING
		jsr scr_putc
		rts

; use a solid cursor
scr_cursorBlinkOff
		lda #CUR_SOLID
		jsr scr_putc
		rts

; Change the cursor to a block
scr_cursorBlock
		lda #$DB
		jsr scr_cursorCustom
		rts

; Change the cursor to an underscore (default)
scr_cursorUnderscore
		lda #'_'
		jsr scr_cursorCustom
		rts

; no cursor
scr_cursorOff
		lda #CUR_SOLID
		jsr scr_putc
		lda #$00
		jsr scr_cursorCustom
		rts

; set any character as the cursor character, #$00 for off
scr_cursorCustom
		pha
		lda #CUR_SET
		jsr scr_putc
		pla
		jsr scr_putc
		rts

; set the font (use combinations of FONT_x
scr_setFont
		pha
		lda #SET_FONT
		jsr scr_putc
		pla
		jsr scr_putc
		rts

; Set the cursor location: COL = X, ROW = Y
scr_setCursor
		pha
		txa
		jsr scr_setColumn
		tya
		jsr scr_setRow
		pla
		rts

; Set location to a given column in X
scr_setColumn
		lda #SET_COLUMN
		jsr scr_putc
		txa
		jsr scr_putc
		rts

; Set location to a given row in Y
scr_setRow
		lda #SET_ROW
		jsr scr_putc
		tya
		jsr scr_putc
		rts

		
