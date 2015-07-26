.NOCODES
;===================================================================================
; TOGGLE SCREEN WIDTH BETWEEN 40 AND 80 COLUMN
;===================================================================================
.CODES

toggle_scrwid
			ldx ARG1				; Check if cmd help is wanted
			beq tgs_cont			; no, then do cmd
			lda CMDBUFF,x
			cmp #'?'				; is it help?
			bne tgs_cont			; No, go do cmd
			jsr Pr_Xhelp			; show cmd help msg
			rts
tgs_cont
			lda SCRWID
			cmp #FORTYCOL
			beq tgsw80			; If it's 40 col go toggle to 80 col
			jsr scr_40col
			jsr Pr_SysHdr			; and print header
			rts
tgsw80
			jsr scr_80col
			jsr Pr_SysHdr			; and print header
			rts
			
			
scr_40col
			jsr scr_clear
			lda #FONT_40_NORM_SNG	; set to 40 col mode
			jsr scr_setFont		; by changing the font
			lda #FORTYCOL
			sta SCRWID			; Update scrwid reg
			rts
			
scr_80col
			jsr scr_clear
			lda #FONT_80_NORM_SNG	; set to 40 col mode
			jsr scr_setFont		; by changing the font
			lda #EIGHTYCOL
			sta SCRWID			; Update scrwid reg
			rts