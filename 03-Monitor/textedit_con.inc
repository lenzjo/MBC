
TXTBOUND		.BYTE NULL, "---", NULL		; Marker at start and end of text
NOH			.BYTE 24						; Number of CRT rows to be displayed
TXTSTART		= $1000						; Fixed Start of text.

CURSOR_RIGHT	= $1C
CURSOR_LEFT	= $1D
CURSOR_UP	= $1E
CURSOR_DOWN	= $1F

