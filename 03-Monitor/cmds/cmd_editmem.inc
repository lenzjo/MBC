.NOCODES
;============================================================================
;====  Rudimentary memory edit function  ====
;============================================================================
.CODES

EDCOL		= $06				; Left-most curr pos

edit_mem
			ldx ARG1				; Get first arg
			bne emIsHelp			; found
			jsr Pr_Esaddrm		; Print error msg
			rts
emIsHelp	
			lda CMDBUFF,x
			cmp #'?'				; want help?
			bne emGetAddr			; No, go test if addr
			jsr Pr_Mhelp			; show cmd help msg
			rts
emGetAddr
			jsr GetASCnum			; Get Start Address
			bcc emGotAddr			; Is Valid
			jsr Pr_Esaddri		; Err msg
			rts
emGotAddr
			lda HEX_RES+1			; Test for ram/rom
			cmp #$80				; rom starts at $8000 
			bcc emInRam			; We're in ram so continue
			jsr Pr_Esaddrr		; In rom, show err
			rts					; and exit.
			
emInRam
			CPYW(HEX_RES,SRC)		; Setup start addr.
			CPYW(SRC,TEMP)		; and save it
			
			lda SCRWID			; setup BYTECNT for screen width
			cmp #FORTYCOL
			bne em80
			lda #$08				; 40 width = 8 bytes
			sta BYTECNT
			jsr Pr_Crlf			; make some vertical space
			jsr Pr_Crlf			; so the line is clearer
			bra emDoRow
em80
			lda #$10				; 80 width = 16 bytes
			sta BYTECNT
			jsr Pr_Crlf			; make some vertical space
			jsr Pr_Crlf			; so the line is clearer
			
emDoRow
			ldx #$00				; X = curpos
			jsr scr_setColumn		; Put cursor at 1st column
			jsr Pr_Address		; Print the addr '$xxxx'
			lda #':'				; then a colon
			jsr PutC
			ldy #$00				; bytes per line ctr
emPrByte
			lda (SRC),y			; get byte from memory
			jsr Pr_HexByte		; print it
			jsr Pr_SPC			; followed by a space
			iny					; inc byte ctr
			cpy BYTECNT			; have we hit max for this scr width?
			bne emPrByte
			
			ldx #EDCOL+1
			jsr scr_setColumn		; move cursor to right of 1st byte
			ldy #$00				; iniz byte ptr
			
emLoop
			jsr GetC				; get a chr from input
			cmp #ESC				; Want to quit edit?
			bne emIsLeft
emExit							; Exit Edit mode
			jsr Pr_Crlf
			jsr Pr_Crlf
			rts
emIsLeft
			cmp #','				; Is it cur left?
			bne emIsRight
emCurLeft

			cpy #$00				; At 1st char?
			bne emDoLeft			; not yet, so go move cursor
			
			lda SRC+1			; Are we at the start of Zero Page
			cmp #$00
			bne emGoLeft			; No, so continue
			lda SRC
			cmp #$00+BYTECNT
			bne emGoLeft			; No, so continue
			
			lda #$7F				; Yes, so go to top of ram
			sta SRC+1
			jmp emDoRow			; Yes, go loop
emGoLeft
			sec					; Subtract BYTECNT bytes
			lda SRC				; from the addr to move backwards
			sbc BYTECNT
			sta SRC
			lda SRC+1
			sbc #$00
			sta SRC+1
			jmp emDoRow			; and go display the new row
emDoLeft
			dex					; move cur left 3 columns
			dex
			dex
			jsr scr_setColumn
			dey					; dec the mem ptr
			jmp emLoop
			
emIsRight
			cmp #'.'				; Is it cur right?
			bne emIsNum
emCurRight
			iny					; INC byte ctr
			cpy BYTECNT			; At the end of the row?
			bne emDoRight
			
			lda SRC+1			; Last page of ram?
			cmp #$7F
			bne emGoRight			; Not yet, so go to next page
			lda SRC
			cmp #$FF-BYTECNT
			bne emGoRight			; Not yet, so go to next page
			stz SRC+1			; Yes, so cycle round to Zero page
			jmp emDoRow			; and go display the new row
emGoRight
			clc					; add BYTECNT bytes to SRC
			lda SRC
			adc BYTECNT
			sta SRC
			lda SRC+1
			adc #$00
			sta SRC+1
			jmp emDoRow			; and go display the new row
emDoRight
			inx					; move cur right 3 columns
			inx
			inx
			jsr scr_setColumn
			jmp emLoop
emIsNum
			jsr GetAscNybl		; Is it a Hex Number?
			bcs emLoop			; No, go get another char
			pha					; save char
			lda (SRC),y			; get the byte
			asl a				; move lower nybble to upper pos
			asl a
			asl a
			asl a
			sta BDATA			; temp save
			pla
			ora BDATA			; add the new lower nybble
			sta (SRC),y			; and save back to memory
			; Now display updated byte
			phx					; save curpos
;			dex					
			dex
			jsr scr_setColumn		; Move to start of byte
			lda (SRC),y			; Get the updated byte
			jsr Pr_HexByte		; and print it
			
			plx
			jsr scr_setColumn		; restore curpos
			jmp emLoop

		
		
			

			
			
			
			
			
			
			
			
			
			
			
			
			
			
			