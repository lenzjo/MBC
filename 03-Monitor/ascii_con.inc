; Control Codes
NULL				= $00
CUR_HOME			= $01
CUR_SET			= $02
CUR_BLINKING		= $03
CUR_SOLID		= $04
SET_PIXEL		= $05
CLEAR_PIXEL		= $06
BS				= $07
BELL				= $08
TAB				= $09
LF				= $0A
CLEAR_SCREEN		= $0C
CR				= $0D
SET_COLUMN		= $0E		; Set column 0 to 79 (2nd char is the column number)
SET_ROW			= $0F		; Set row 0 to 24 (2nd char is the row number)
DEL_BEFORE_LINE	= $10		; Delete to the start of the line
DEL_AFTER_LINE	= $11		; Delete to the end of the line
DEL_BEFORE_PAGE	= $12		; Delete to start of screen
DEL_AFTER_PAGE	= $13		; Delete to end of screen
SCROLL_UP		= $14
SCROLL_DOWN		= $15
SCROLL_LEFT		= $16
SCROLL_RIGHT		= $17
SET_FONT			= $18
QUIT				= $19		; CTRL-Y
NEXT_IS_CHAR		= $1A		; Treat next char as a character (to allow PC DOS char codes 1-31 to be displayed on screen)

ESC				= $1B		; ESC - reserved for ANSI sequences
CUR_RIGHT		= $1C
CUR_LEFT			= $1D
CUR_UP			= $1E
CUR_DOWN			= $1F

SPC				= $20

DEL				= $7F

;====  font options  ====
FONT_40_CHR		= $00
FONT_80_CHR		= $01
FONT_NORM		= $00
FONT_BOLD		= $02
FONT_SINGLE_HGT	= $00
FONT_DOUBLE_HGT	= $04
FONT_GFX			= $80

FONT_40_NORM_SNG	= $00
FONT_80_NORM_SNG	= $01
FONT_40_BOLD_SNG	= $02
FONT_80_BOLD_SNG	= $03
FONT_40_NORM_DBL	= $04
FONT_80_NORM_DBL	= $05
FONT_40_BOLD_DBL	= $06
FONT_80_BOLD_DBL	= $07
