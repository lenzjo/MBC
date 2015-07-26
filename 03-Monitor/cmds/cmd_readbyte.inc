
;==== Print data at an addr  ====
read_byte
			; Check that all args are present
			ldx ARG1				; Get first arg
			bne rbIsHelp			; found
			jsr Pr_Esaddrm		; Print error msg
			rts
rbIsHelp
			lda CMDBUFF,x
			cmp #'?'				; want help?
			bne rbGetAddr			; No, go get addr
			jsr Pr_Rhelp			; show cmd help msg
			rts
			
			; Args present so get 'em
rbGetAddr
			ldx ARG1				; get line ptr
			jsr GetASCnum			; Get addr to read from
			bcc rbgAddr			; Is Valid
			jsr Pr_Esaddri		; Err msg
			rts
rbgAddr
			lda HEX_RES+1
			sta SRC+1
			lda HEX_RES
			sta SRC
rbShow
			lda (SRC)			; 'Peek' at the Addr
			jsr Pr_HexByte		; and print it
			jsr Pr_Crlf
			rts