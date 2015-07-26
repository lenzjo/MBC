
; I/O starts at $8100. There are eight 32 byte blocks decoded
; ACIA - 6551
ACIA1		= $8100
ACIA_DATA	= ACIA1
ACIA_STAT	= ACIA1 + 1
ACIA_COM		= ACIA1 + 2
ACIA_CTRL	= ACIA1 + 3
