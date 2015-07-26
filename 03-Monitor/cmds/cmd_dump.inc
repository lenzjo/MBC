;====  Dump a block of memory to screen  ====
; d xxxx [zzzz]     - Dump memory to screen from address xxxx for an optional
;                     count of zzzz bytes, default 256.
;====
dump_memblk
			; Check that all args are present
			ldx ARG1				; Get first arg
			bne dmIsHelp			; found
			jsr Pr_Esaddrm		; Print error msg
			rts
dmIsHelp	
			lda CMDBUFF,x
			cmp #'?'				; want help?
			bne dmGetAddr			; No, go test if addr
			jsr Pr_Dhelp			; show cmd help msg
			rts
dmGetAddr
			jsr GetASCnum			; Get Start Address
			bcc dmGotAddr			; Is Valid
			jsr Pr_Esaddri		; Err msg
			rts
dmGotAddr
			CPYW(HEX_RES,SRC)		; Store addr
dmGetLength
			ldx ARG2				; Is length present?
			bne dmGetLen			; Yes, go get it
			lda SCRWID
			cmp #FORTYCOL
			bne dmgl80
			lda #$B0				; 40 col, show only $B0 bytes
			sta HEX_RES
			lda #$00
			sta HEX_RES+1
			bra dmgLen
dmgl80
			lda #$00				; No, set up default $0100
			sta HEX_RES
			lda #$01
			sta HEX_RES+1
			bra dmgLen
dmGetLen
			jsr GetASCnum			; Get Memory Length
			bcc dmgLen			; Is Valid
			jsr Pr_Emleni			; Err msg
			rts
			
			;==== Parms are good, let's do it!
dmgLen
			CPYW(HEX_RES,MEMLEN)	; store memory length

			lda SCRWID			; setup BYTECNT for screen width
			cmp #FORTYCOL
			bne dm80
			lda #$08				; 40 width = 8 bytes
			sta BYTECNT
			bra dmDoRow
dm80
			lda #$10				; 80 width = 16 bytes
			sta BYTECNT
dmDoRow
			ldy #$00				; bytes per line ctr

			jsr Pr_Address		; Print the addr '$xxxx'
			lda SCRWID
			cmp #FORTYCOL
			beq dmRowCon			; If small screen print only colon
			jsr Pr_SPC
			lda #':'
			jsr PutC
			jsr Pr_SPC
			bra dmPrByte
dmRowCon
			lda #':'
			jsr PutC
dmPrByte
			lda (SRC)			; get byte from memory
			jsr Pr_HexByte		; print it
			jsr Pr_SPC			; followed by a space

			lda (SRC)			; Now save the ASCII
			cmp #SPC
			bcs dmPrTest			; >= SPC
			lda #'.'				; Substitute a '.' for unprintable chars
			bra dmAscSave
dmPrTest
			cmp #$7F
			bcc dmAscSave			; it's printable
			lda #'.'				; Substitute a '.' for unprintable chars
dmAscSave
			sta ASCIIDUMP,y		; finally save it in buffer
			INCW(SRC)			; Inc the mem-ptr
			DECW(MEMLEN)			; Dec the mem ctr
			bne dmEOL			; Not 0, continue
			lda MEMLEN+1
			beq dmFinish			; It is 0, so go exit
dmEOL
			iny					; inc line byte ctr
			cpy BYTECNT
			bne dmPrByte			; go print another byte
			jsr Pr_AsciiDump		; and print the ASCIIDUMP
			jmp dmDoRow
dmFinish
			cpy BYTECNT
			beq dmLastADump		; at the end of line
dmFillGap
			iny
			cpy BYTECNT
			beq dmLastADump
			jsr Pr_SPC			; print 3 spaces for each 'missing' byte on the row
			jsr Pr_SPC
			jsr Pr_SPC
			lda #SPC				; put a SPC in the ASCIIDUMP string
			sta ASCIIDUMP,y
			bra dmFillGap
dmLastADump
			jsr Pr_AsciiDump
			rts

			;==== Print ASCII version of the last 16 bytes
Pr_AsciiDump
			lda #NULL			; NULL terminate the string
			sta ASCIIDUMP,y
			lda SCRWID
			cmp #FORTYCOL
			beq padCon			; Don't print the colon in 40 col mode
			lda #':'				; Put a separator after the bytes
			jsr PutC
padCon
			jsr Pr_SPC
			CPYMSPTR(ASCIIDUMP)		; Now print it
			jsr Pr_Str
			lda #CR
			jsr PutC
			lda #LF
			jsr PutC
			rts

Pr_Address
			lda #'$'				; Format is $xxxx
			jsr PutC
			lda SRC+1
			jsr Pr_HexByte
			lda SRC
			jsr Pr_HexByte
			rts
