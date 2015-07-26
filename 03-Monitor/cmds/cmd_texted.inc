.NOCODES
;==========================================================================
;====  Text Editor: in Alpha stage
;==========================================================================
.CODES


ED_NEWQ		.BYTE "New (Y/N) ? ", NULL
ED_QUITQ		.BYTE "Quit? Really? (Y/N) ", NULL

TXTBEG		= $1000				; Permanent start of Text
LINEBUFF		= $300				; temp storage for input line

TXTEND		= $80				; Ptr to end of text
TXTCURR		= $82				; Ptr to curr line of text
CURRLINE		= $84
COLPOS		= $86				; row/col pos
ROWPOS		= $88


SCRPOS		= $8A

text_edit
			ldx ARG1				; Check if cmd help is wanted
			beq te_cont			; no, then do cmd
			lda CMDBUFF,x
			cmp #'?'				; is it help?
			beq teHelp			; Yes, go show it
			jmp Pr_Euparm			; No, Show err msg
teHelp
			jmp Pr_Thelp			; show cmd help msg
			
			;====  Start of Text Editor  ====
te_cont
			jsr blank_screen		; clr scr & home cur to bot left
			jsr Pr_EdNew			; Show 'new file' question
te_NQloop
			jsr get_yn			; can only get a 'y' or 'n'
			pha
			jsr PutC				; so print it
			pla
			cmp #'n'				
			bne te_IsY			; Not no, must be yes
			jsr PutC
			bra te_wemkr
te_IsY
			CPYA2P(TXTBEG,TXTEND)	; Make start and end the same for a new file
te_wemkr
			CPYA2P(TXTBEG,TXTCURR)	; and point curr to start of text
te_show_loop
			jsr blank_screen
			jsr show_text
			jsr home_cursor
			
			lda #':'				; Print the input prompt
			jsr PutC
			
			stz SCRPOS
			stz SCRPOS+1
			
			ldy #$00				; iniz chr ptr
			lda #NULL
			sta (TXTEND),y		; and end marker for text 'file'
			
edit_loop
			jsr GetC				; get a chr
			cmp #SPC
			bcc te_IsESC			; control char found, go do it
			jmp te_IsChar			; if it's printable, go do it
te_IsESC
			cmp #ESC				; Wanna quit?
			bne te_IsCR
			jsr home_cursor
			jsr Pr_EdQuit			; Really wanna quit?
			jsr get_yn			; can only get a 'y' or 'n'
			pha
			jsr PutC				; so print it
			pla
			cmp #'n'
			bne te_ynexit			; if yes, then go exit editor
			bra te_show_loop
te_ynexit
			jsr Pr_Crlf
			jsr Pr_Crlf
			rts
te_IsCR
			cmp #CR				; Is it a CR?
			bne te_IsBS			; No, go check for backspace
			lda #NULL
			sta LINEBUFF,y
			jsr insert_line
			jmp te_show_loop
te_IsBS
			cmp #BS				; Is it a backspace?
			bne te_UpArrow		; No, go check for up arrow
			cpy #$00				; at 1st chr pos
			bne te_doBS			; We have chrs, go kill the last one
			jmp edit_loop			; Yes, so nothing to kill, go get another chr
te_doBS
			dey					; dec line ptr
			lda #BS
			jsr PutC				; go back a chr
			jsr Pr_SPC			; remove by printing a SPC
			lda #BS				; and move the cursor back again!
			jsr PutC
			jmp edit_loop
te_UpArrow
			cmp #SCROLL_UP		; Is it an up arrow? (ctrl-T)
			bne te_DownArrow		; No, go check for down arrow
			CPTR_AT_START?		; are we already at the 1st line?
			beq te_UA_ex			; yes so exit and go get another
			jsr get_prev_ptr		; No, so get prev ptr
			jmp te_show_loop		; and reprint screen
te_UA_ex
			jmp edit_loop
te_DownArrow
			cmp #SCROLL_DOWN		; Is it down arrow?  (ctrl-U)
			bne te_IsChar			; No, go check for chr
			CPTR_AT_END?			; already at the end?
			beq te_DA_ex			; yes so exit back to the edit loop
			jsr get_next_ptr		; no, so get ptr for next line
			jmp te_show_loop		; and reprint screen
te_DA_ex
			jmp edit_loop
te_IsChar						; ascii char so save it
			sta LINEBUFF,y		; A regular chr, so store in buffer
			jsr PutC				; and show it on screen
			iny					; inc line ptr
			jmp edit_loop			; go get another
			rts
			

;====  Insert line of text at TXTCURR  ====
insert_line
			CPYW(TXTCURR,TEMP)	; dup ptr
			CPTR_AT_END?			; Is curr == end
			beq il_InsertLine		;   Yes then go append new line
			CPYW(TEMP,ADFROM)		; prepare to move block up
			CPYW(TXTEND,ADUNTIL)	; to make space for new line
			iny
			ADDY(TEMP)
			CPYW(TEMP,ADTO)
			jsr copy_block_main	; and copy it
il_InsertLine
			ldy #$00				; iniz chr ptr
			INCW(TXTCURR)			; Skip over the NULL
il_cpyloop
			lda LINEBUFF,y		; get a chr from the buffer
			beq il_eol			;   If a NULL then exit to clean up
			sta (TXTCURR),y		; else put it into the document
			iny					; and inc the chr ptr
			bra il_cpyloop		; and go get the next one
il_eol
			sta (TXTCURR),y		; insert the NULL
			DECW(TXTCURR)			; restore the ptr
			iny
			ADDY(TXTCURR)			; Update the ptrs
			ADDY(TXTEND)
			rts

			
show_curr
			sta TEMP
			phx
			phy
			lda ROWPOS
			pha
			lda COLPOS
			pha
			
			ldx SCRPOS
			ldy SCRPOS+1
			jsr scr_setCursor
			inc SCRPOS+1
			
			lda TEMP
			jsr PutC
			lda #'='
			jsr PutC
			lda TXTCURR+1
			jsr Pr_HexByte
			lda TXTCURR
			jsr Pr_HexByte
			
			pla
			sta COLPOS
			pla
			sta ROWPOS
			ply
			plx
			rts

			
;==== Redraw the screen showing any text  ====
show_text
;			rts
			SAVE_PTRS
			CPYW(TXTCURR, CURRLINE)
st_ploop
			CPTR_AT_START?		; have we reached the start of text yet?
			beq st_at_end			; Yes, go check end
			CPTR_AT_END?			; Not at start, are we at the end?
			beq st_do_prev		; Yes, then go get prev ptr
			lda ROWPOS			; top row - exit
			beq st_exit
			jsr showCurr_line		; otherwise print a line
st_do_prev
			jsr get_prev_ptr		; get the prev ptr
			bra st_ploop			; and loop
st_at_end
			CPTR_AT_END?			; Start == end, means no text to print!
			beq st_exit			; So exit
			lda ROWPOS			; top row - exit
			beq st_exit
			jsr showCurr_line		; otherwise print the first line
st_exit
			RESTORE_PTRS			; and exit
			rts
			
			
;====  Show line at TXTCURR  ====
showCurr_line
			jsr curs_leftcol
			jsr curs_up
			ISCURRLINE?
			bne scl_spc
			lda #'>'
			bra scl_prln
scl_spc
			lda #SPC
scl_prln
			jsr PutC
			ldy #$01				; Ignore Starting NULL
scl_loop
			lda (TXTCURR),y		; get a chr
			beq scl_exit			; Go exit if a NULL
			jsr PutC				; otherwise print it
			iny
			bra scl_loop			; do it all again...
scl_exit
			rts

			
;====  Point TXTCURR to start of prev line  ====
get_prev_ptr
			ldy #$00
gpp_eolloop
			DECW(TXTCURR)			; go to previous char
			lda (TXTCURR),y		; get it
			bne gpp_eolloop		; keep going until a NULL is found
			rts
			
			
;==== Point TXTCURR to start of next line  ====
get_next_ptr
			ldy #$00
gnp_eolloop
			INCW(TXTCURR)
			lda (TXTCURR),y
			bne gnp_eolloop
			rts
			
;====  Blank screen and home cursor bottom left  ====
blank_screen
			jsr scr_clear
			jsr home_cursor
			rts
			
			
;==== move cursor to bot left
home_cursor
			phy
			phx
			ldx #0
			stx COLPOS
			ldy #24
			sty ROWPOS
			jsr scr_setCursor
			plx
			ply
			rts
			
			
;==== Print New File question  ====
Pr_EdNew
			CPYMSPTR(ED_NEWQ)
			jsr Pr_Str
			rts
			
			
;==== Print Really Quit question  ====
Pr_EdQuit
			CPYMSPTR(ED_QUITQ)
			jsr Pr_Str
			rts
			
			
;====  Get Yes/No answer  ====
get_yn
			jsr GetC				; and wait for answer
			beq get_yn			; NULL, try again
			ora #$40				; convert to lower case
			cmp #'y'
			bne gy_IsNo			; Not y, so go try n
			rts					; Is y so exit
gy_IsNo
			cmp #'n'
			bne get_yn			; Not n either so try again
			rts					; Is n so exit
			
			
;==== Move cursor up one row
curs_up
			dec ROWPOS			; dec reg
			phy					; save Y
			ldy ROWPOS
			jsr scr_setRow		; Move cursor up one row
			ply					; restore Y
			rts
			
			
;==== Move cursor to left col  ====
curs_leftcol
			phx					; save X
			ldx #$00
			sta COLPOS			; update reg
			jsr scr_setColumn		; move cursor
			plx					; restore X
			rts
			
			