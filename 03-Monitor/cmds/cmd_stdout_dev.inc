;==============================================================
;  Change STDIO device - Screen, Terminal or Both
;==============================================================


change_stdio
			; Check that all args are present
			ldx ARG1				; Get first arg
			bne csIsHelp			; found
			jsr Pr_Emdev			; Print error msg
			rts
csIsHelp	
			lda CMDBUFF,x
			cmp #'?'				; want help?
			bne csdev			; No, go test for which dev
			jsr Pr_Ohelp			; show cmd help msg
			rts
csdev
			ora #$40				; change to lower case
			cmp #'s'				; Is it Screen?
			bne csIsTerm
			lda #SCREEN
			bra csExit
csIsTerm
			cmp #'t'				; Is it Terminal?
			bne csIsBoth
			lda #SERIAL
			bra csExit
			
csIsBoth
			cmp #'b'				; Is it Both?
			bne csErr
			lda #BOTH
csExit
			sta STDOUT			; Save valid device
			rts					; and exit
csErr
			jsr Pr_Euodev
			rts