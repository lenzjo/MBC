;==============================================================================
; Clear the Screen and home the cursor
;==============================================================================

screen_clear
			ldx ARG1				; Check if cmd help is wanted
			beq sclr_cont			; no, then do cmd
			lda CMDBUFF,x
			cmp #'?'				; is it help?
			beq csHelp			; Yes, go show it
			jmp Pr_Euparm			; No, Show err msg
csHelp
			jmp Pr_Khelp			; show cmd help msg
sclr_cont
			lda STDOUT
			cmp #SCREEN
			bne sclrIsTerminal
			jsr scr_clear
			jsr Pr_SysHdr
			rts
sclrIsTerminal
			cmp #SERIAL
			bne sclrIsBoth
			jsr term_clear
			jsr Pr_SysHdr
			rts
			
sclrIsBoth
			lda #SERIAL
			sta STDOUT
			jsr term_clear
			jsr Pr_SysHdr
			
			lda #SCREEN
			sta STDOUT
			jsr scr_clear
			jsr Pr_SysHdr
			
			lda #BOTH
			sta STDOUT
			rts