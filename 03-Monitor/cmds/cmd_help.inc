.NOCODES
;=======================================================================================
;  HELP COMMAND
;=======================================================================================
.CODES

;====               "1234567890123456789012345678901234567890" = 40 cols
HD_HELP40	.BYTE "Available commands are:", CR, LF, LF, NULL
R_HELP40		.BYTE "r xxxx            - Read address xxxx", CR, LF, NULL
W_HELP40		.BYTE "w xxxx bb         - Write bb to xxxx", CR, LF, NULL
D_HELP40		.BYTE "d xxxx [zzzz]     - Dump from xxxx", CR, LF
			.BYTE "                    optional cnt of zzzz", CR, LF
			.BYTE "                    bytes, default 256", CR, LF, NULL
C_HELP40		.BYTE "c xxxx yyyy zzzz  - Cpy xxxx-yyyy > zzzz", CR, LF, NULL
F_HELP40		.BYTE "f xxxx yyyy bb    - Fill bb > xxxx-yyyy", CR, LF, NULL
K_HELP40		.BYTE "k                 - Klr scr & home curs", CR, LF, NULL
X_HELP40		.BYTE "x                 - Xchange 40/80 cols.", CR, LF, NULL
H_HELP40		.BYTE "h                 - Show help screen.", CR, LF, NULL
Z_HELP40		.BYTE "@ ?               - A cmd followed by ?", CR, LF
			.BYTE "                    shows help for that", CR, LF
			.BYTE "                    cmd only.", CR, LF, NULL
O_HELP40		.BYTE "o s|t             - Set output to screen", CR, LF
			.BYTE "                    terminal or both.", CR, LF, NULL
M_HELP40		.BYTE "m xxxx            - Memory edit starting", CR, LF
			.BYTE "                    at xxxx. use , and", CR, LF
			.BYTE "                    . to move cursor.", CR, LF, NULL
T_HELP40		.BYTE "t                 - Text Editor. V basic", CR, LF, NULL
			
;====               "12345678901234567890123456789012345678901234567890123456789012345678901234567890" = 80 cols
HD_HELP80	.BYTE "Available commands are:", CR, LF, LF, NULL
R_HELP80		.BYTE "r xxxx            - Read data at address xxxx", CR, LF, NULL
W_HELP80		.BYTE "w xxxx bb         - Write byte bb to address xxxx", CR, LF, NULL
D_HELP80		.BYTE "d xxxx [zzzz]     - Dump memory from addr xxxx for an optl", CR, LF
			.BYTE "                    count of zzzz bytes, default 256.", CR, LF, NULL
C_HELP80		.BYTE "c xxxx yyyy zzzz  - Copy bytes from xxxx-yyyy to zzzz.", CR, LF, NULL
F_HELP80		.BYTE "f xxxx yyyy bb    - Fill memory from xxxx to yyyy with bb.", CR, LF, NULL
K_HELP80		.BYTE "k                 - Klear screen and home cursor.", CR, LF, NULL
X_HELP80		.BYTE "x                 - Xchange screen mode between 40 and 80 col modes.", CR, LF, NULL
H_HELP80		.BYTE "h                 - Show the help screen.", CR, LF, NULL
Z_HELP80		.BYTE "@ ?               - Any cmd followed by a ? shows help for that cmd only.", CR, LF, NULL
O_HELP80		.BYTE "o s|t|b           - Set output to screen, terminal or both.", CR, LF, NULL
M_HELP80		.BYTE "m xxxx            - Memory edit starting at xxxx. Use , and . to move cursor", CR, LF, NULL
T_HELP80		.BYTE "t                 - Text Editor. Very basic.", CR, LF, NULL

;====  Print the Help Screen  ====
Pr_AllHelp
			ldx ARG1				; Check if cmd help is wanted
			beq pah_cont			; no, then do cmd
			lda CMDBUFF,x
			cmp #'?'				; is it help?
			beq pahHelp			; Yes, go show it
			jmp Pr_Euparm			; No, Show err msg
pahHelp
			jmp Pr_Hhelp			; show cmd help msg
pah_cont
			jsr Pr_HDRhelp
			jsr Pr_Rhelp
			jsr Pr_Whelp
			jsr Pr_Dhelp
			jsr Pr_Chelp
			jsr Pr_Fhelp
			jsr Pr_Khelp
			jsr Pr_Xhelp
			jsr Pr_Hhelp
			jsr Pr_Zhelp
			jsr Pr_Ohelp
			jsr Pr_Mhelp
			jsr Pr_Thelp
			rts
			
			
Pr_HDRhelp
			lda SCRWID
			cmp #FORTYCOL
			beq phdr40
			CPYMSPTR(HD_HELP80)
			bra phdr40pr
phdr40
			CPYMSPTR(HD_HELP40)
phdr40pr
			jsr Pr_Str
			rts
			
			
Pr_Rhelp
			lda SCRWID
			cmp #FORTYCOL
			beq prh40
			CPYMSPTR(R_HELP80)
			bra prh40pr
prh40
			CPYMSPTR(R_HELP40)
prh40pr
			jsr Pr_Str
			rts

			
Pr_Whelp
			lda SCRWID
			cmp #FORTYCOL
			beq prwh40
			CPYMSPTR(W_HELP80)
			bra prwh40pr
prwh40
			CPYMSPTR(W_HELP40)
prwh40pr
			jsr Pr_Str
			rts
			
			
Pr_Dhelp
			lda SCRWID
			cmp #FORTYCOL
			beq prdh40
			CPYMSPTR(D_HELP80)
			bra prdh40pr
prdh40
			CPYMSPTR(D_HELP40)
prdh40pr
			jsr Pr_Str
			rts
			
			
Pr_Chelp
			lda SCRWID
			cmp #FORTYCOL
			beq prch40
			CPYMSPTR(C_HELP80)
			bra prch40pr
prch40
			CPYMSPTR(C_HELP40)
prch40pr
			jsr Pr_Str
			rts
			
			
Pr_Fhelp
			lda SCRWID
			cmp #FORTYCOL
			beq prfh40
			CPYMSPTR(F_HELP80)
			bra prfh40pr
prfh40
			CPYMSPTR(F_HELP40)
prfh40pr
			jsr Pr_Str
			rts
			
			
Pr_Khelp
			lda SCRWID
			cmp #FORTYCOL
			beq prkh40
			CPYMSPTR(K_HELP80)
			bra prkh40pr
prkh40
			CPYMSPTR(K_HELP40)
prkh40pr
			jsr Pr_Str
			rts

			
Pr_Xhelp
			lda SCRWID
			cmp #FORTYCOL
			beq prxh40
			CPYMSPTR(X_HELP80)
			bra prxh40pr
prxh40
			CPYMSPTR(X_HELP40)
prxh40pr
			jsr Pr_Str
			rts
			
			
Pr_Hhelp
			lda SCRWID
			cmp #FORTYCOL
			beq prhh40
			CPYMSPTR(H_HELP80)
			bra prhh40pr
prhh40
			CPYMSPTR(H_HELP40)
prhh40pr
			jsr Pr_Str
			rts
			
			
Pr_Zhelp
			lda SCRWID
			cmp #FORTYCOL
			beq przh40
			CPYMSPTR(Z_HELP80)
			bra przh40pr
przh40
			CPYMSPTR(Z_HELP40)
przh40pr
			jsr Pr_Str
			rts
			

Pr_Ohelp
			lda SCRWID
			cmp #FORTYCOL
			beq proh40
			CPYMSPTR(O_HELP80)
			bra proh40pr
proh40
			CPYMSPTR(O_HELP40)
proh40pr
			jsr Pr_Str
			rts
			
			
Pr_Mhelp
			lda SCRWID
			cmp #FORTYCOL
			beq prmh40
			CPYMSPTR(M_HELP80)
			bra prmh40pr
prmh40
			CPYMSPTR(M_HELP40)
prmh40pr
			jsr Pr_Str
			rts
			
			
Pr_Thelp
			lda SCRWID
			cmp #FORTYCOL
			beq prth40
			CPYMSPTR(T_HELP80)
			bra prth40pr
prth40
			CPYMSPTR(T_HELP40)
prth40pr
			jsr Pr_Str
			rts
			
			
			