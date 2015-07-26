;====  PCF8584 Constants  ====

;====  Control/Status register S1  ====
;==== S1 Write-Only Constants  ====
CTRL_PIN		= %10000000
CTRL_ESO		= %01000000
CTRL_ES1		= %00100000
CTRL_ES2		= %00010000
CTRL_ENI		= %00001000
CTRL_STA		= %00000100
CTRL_STO		= %00000010
CTRL_ACK		= %00000001

;====  S1 Read-Only Constants ====
ST_PIN		= %10000000
ST_0			= %01000000
ST_STS		= %00100000
ST_BER		= %00010000
ST_AD0LRB	= %00001000
ST_AAS		= %00000100
ST_LAB		= %00000010
ST_BB		= %00000001

;==== Exit codes  ====
I2C_OK			= 0
I2C_NOCTRL		= -1			; No controller hardware found
I2C_ABORTSEND		= -2			; Problem sending all data bytes
I2C_ABORTREAD		= -3			; Problem reading all data bytes