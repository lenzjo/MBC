;==== Error Messages  ====
;====               "1234567890123456789012345678901234567890" = 40 cols
;====               "12345678901234567890123456789012345678901234567890123456789012345678901234567890" = 80 cols
ERR_LEADER	.BYTE "Err: ", NULL
ERR_GARBAGE	.BYTE "Huh?? Do not understand... Try again.", CR, LF, NULL
ERR_CMD		.BYTE "Cmd not coded yet...", CR, LF, NULL
ERR_SAM		.BYTE "Src Addr is missing.", CR, LF, NULL
ERR_SAI		.BYTE "Src Addr is invalid.", CR, LF, NULL
ERR_DAM		.BYTE "Data byte is missing.", CR, LF, NULL
ERR_DAI		.BYTE "Data byte is invalid.", CR, LF, NULL
ERR_MLI		.BYTE "Invalid Memory length.", CR, LF, NULL
ERR_EOD		.BYTE "Unknown Output Device (s|t|b).", CR, LF, NULL
ERR_MOD		.BYTE "Output Device is missing (s|t|b).", CR, LF, NULL
ERR_EUP		.BYTE "Invalid parameter.", CR, LF, NULL
ERR_SAR		.BYTE "Cannot alter rom!", CR, LF, NULL
ERR_FAM		.BYTE "From Addr is missing.", CR, LF, NULL
ERR_FAI		.BYTE "From Addr is invalid.", CR, LF, NULL
ERR_UAM		.BYTE "Until Addr is missing.", CR, LF, NULL
ERR_UAI		.BYTE "Until Addr is invalid.", CR, LF, NULL
ERR_TAM		.BYTE "To Addr is missing.", CR, LF, NULL
ERR_TAI		.BYTE "To Addr is invalid.", CR, LF, NULL

;====  Print Functions  ====

Pr_ErrMsg
			pha
			phy
			jsr Pr_Err_lead		; Print 'Err: '
			ply
			pla
			jsr Pr_Str			; then the actual error msg
			rts
				
Pr_Err_lead
			CPYMSPTR(ERR_LEADER)
			jsr Pr_Str
			rts
			
Pr_Encmd		; Print Cmd Err msg
			CPYMSPTR(ERR_GARBAGE)
			jsr Pr_ErrMsg
			rts

Pr_Evcmd		; Print Cmd Err msg
			CPYMSPTR(ERR_CMD)
			jsr Pr_ErrMsg
			rts
			
Pr_Esaddrm	; Err: Missing SRC addr
			CPYMSPTR(ERR_SAM)
			jsr Pr_ErrMsg
			rts
			
Pr_Esaddri	; Err: invalid SRC addr
			CPYMSPTR(ERR_SAI)
			jsr Pr_ErrMsg
			rts
			
Pr_Edatai
			CPYMSPTR(ERR_DAI)
			jsr Pr_ErrMsg
			rts
			
Pr_Edatam
			CPYMSPTR(ERR_DAM)
			jsr Pr_ErrMsg
			rts

Pr_Emleni
			CPYMSPTR(ERR_MLI)
			jsr Pr_ErrMsg
			rts
			
Pr_Euodev
			CPYMSPTR(ERR_EOD)
			jsr Pr_ErrMsg
			rts

Pr_Emdev
			CPYMSPTR(ERR_MOD)
			jsr Pr_ErrMsg
			rts
			
Pr_Euparm
			lda #<ERR_EUP
			ldy #>ERR_EUP
			jsr Pr_ErrMsg
			rts
			
Pr_Esaddrr
			lda #<ERR_SAR
			ldy #>ERR_SAR
			jsr Pr_ErrMsg
			rts
			
Pr_Efaddrm	; Err: Missing SRC addr
			CPYMSPTR(ERR_FAM)
			jsr Pr_ErrMsg
			rts
			
Pr_Efaddri	; Err: invalid SRC addr
			CPYMSPTR(ERR_FAI)
			jsr Pr_ErrMsg
			rts
			
Pr_Euaddrm	; Err: Missing SRC addr
			CPYMSPTR(ERR_UAM)
			jsr Pr_ErrMsg
			rts
			
Pr_Euaddri	; Err: invalid SRC addr
			CPYMSPTR(ERR_UAI)
			jsr Pr_ErrMsg
			rts
			
Pr_Etaddrm	; Err: Missing SRC addr
			CPYMSPTR(ERR_TAM)
			jsr Pr_ErrMsg
			rts
			
Pr_Etaddri	; Err: invalid SRC addr
			CPYMSPTR(ERR_TAI)
			jsr Pr_ErrMsg
			rts
			
			
			