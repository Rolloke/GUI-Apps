/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ascii.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ascii.h $
// CHECKIN:		$Date: 9.03.98 12:42 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 18.02.98 16:00 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#ifndef ASCII_HEADER
#define ASCII_HEADER

////////////////////////////////////////////////
// ASCII-Definitionen
#define ASCII_NUL		0x00
#define ASCII_SOH		0x01
#define ASCII_STX		0x02
#define ASCII_ETX		0x03
#define ASCII_EOT		0x04
#define ASCII_ENQ		0x05
#define ASCII_ACK		0x06
#define ASCII_BEL		0x07
#define ASCII_BS		0x08
#define ASCII_HT		0x09
#define ASCII_LF		0x0a
#define ASCII_VT		0x0b
#define ASCII_FF		0x0c
#define ASCII_CR		0x0d
#define ASCII_SO		0x0e
#define ASCII_SI		0x0f
#define ASCII_SLE		0x10
#define ASCII_CS1		0x11
#define ASCII_DC2		0x12
#define ASCII_DC3		0x13
#define ASCII_DC4		0x14
#define ASCII_NAK		0x15
#define ASCII_SYN		0x16
#define ASCII_ETB		0x17
#define ASCII_CAN		0x18
#define ASCII_EM		0x19
#define ASCII_SIB		0x1a
#define ASCII_ESC		0x1b
#define ASCII_FS		0x1c
#define ASCII_GS		0x1d
#define ASCII_RS		0x1e
#define ASCII_US		0x1f
#define	ASCII_SPACE		0x20
/*
#define	ASCII_		0x21
#define	ASCII_		0x22
#define	ASCII_		0x23
#define	ASCII_		0x24
#define	ASCII_		0x25
#define	ASCII_		0x26
#define	ASCII_		0x27
#define	ASCII_		0x28
#define	ASCII_		0x29
#define	ASCII_		0x2a
#define	ASCII_		0x2b
#define	ASCII_		0x2c
#define	ASCII_		0x2d
#define	ASCII_		0x2e
#define	ASCII_		0x2f
*/
#define	ASCII_0		0x30
#define	ASCII_1		0x31
#define	ASCII_2		0x32
#define	ASCII_3		0x33
#define	ASCII_4		0x34
#define	ASCII_5		0x35
#define	ASCII_6		0x36
#define	ASCII_7		0x37
#define	ASCII_8		0x38
#define	ASCII_9		0x39
/*
#define	ASCII_		0x3a
#define	ASCII_		0x3b
#define	ASCII_		0x3c
#define	ASCII_		0x3d
#define	ASCII_		0x3e
#define	ASCII_		0x3f
#define	ASCII_		0x40
*/
#define	ASCII_A		0x41
#define	ASCII_B		0x42
#define	ASCII_C		0x43
#define	ASCII_D		0x44
#define	ASCII_E		0x45
#define	ASCII_F		0x46
#define	ASCII_G		0x47
#define	ASCII_H		0x48
#define	ASCII_I		0x49
#define	ASCII_J		0x4a
#define	ASCII_K		0x4b
#define	ASCII_L		0x4c
#define	ASCII_M		0x4d
#define	ASCII_N		0x4e
#define	ASCII_O		0x4f
#define	ASCII_P		0x50
#define	ASCII_Q		0x51
#define	ASCII_R		0x52
#define	ASCII_S		0x53
#define	ASCII_T		0x54
#define	ASCII_U		0x55
#define	ASCII_V		0x56
#define	ASCII_W		0x57
#define	ASCII_X		0x58
#define	ASCII_Y		0x59
#define	ASCII_Z		0x5a
/*
#define	ASCII_		0x5b
#define	ASCII_		0x5c
#define	ASCII_		0x5d
#define	ASCII_		0x5e
#define	ASCII_		0x5f
#define	ASCII_		0x60
*/
#define	ASCII_a		0x61
#define	ASCII_b		0x62
#define	ASCII_c		0x63
#define	ASCII_d		0x64
#define	ASCII_e		0x65
#define	ASCII_f		0x66
#define	ASCII_g		0x67
#define	ASCII_h		0x68
#define	ASCII_i		0x69
#define	ASCII_j		0x6a
#define	ASCII_k		0x6b
#define	ASCII_l		0x6c
#define	ASCII_m		0x6d
#define	ASCII_n		0x6e
#define	ASCII_o		0x6f
#define	ASCII_p		0x70
#define	ASCII_q		0x71
#define	ASCII_r		0x72
#define	ASCII_s		0x73
#define	ASCII_t		0x74
#define	ASCII_u		0x75
#define	ASCII_v		0x76
#define	ASCII_w		0x77
#define	ASCII_x		0x78
#define	ASCII_y		0x79
#define	ASCII_z		0x7a
/*
#define	ASCII_		0x7b
#define	ASCII_		0x7c
#define	ASCII_		0x7d
#define	ASCII_		0x7e
*/
#define	ASCII_DEL	0x7f
/*
#define	ASCII_		0x80
#define	ASCII_		0x81
#define	ASCII_		0x82
#define	ASCII_		0x83
#define	ASCII_		0x84
#define	ASCII_		0x85
#define	ASCII_		0x86
#define	ASCII_		0x87
#define	ASCII_		0x88
#define	ASCII_		0x89
#define	ASCII_		0x8a
#define	ASCII_		0x8b
#define	ASCII_		0x8c
#define	ASCII_		0x8d
#define	ASCII_		0x8e
#define	ASCII_		0x8f
#define	ASCII_		0x90
#define	ASCII_		0x91
#define	ASCII_		0x92
#define	ASCII_		0x93
#define	ASCII_		0x94
#define	ASCII_		0x95
#define	ASCII_		0x96
#define	ASCII_		0x97
#define	ASCII_		0x98
#define	ASCII_		0x99
#define	ASCII_		0x9a
#define	ASCII_		0x9b
#define	ASCII_		0x9c
#define	ASCII_		0x9d
#define	ASCII_		0x9e
#define	ASCII_		0x9f
#define	ASCII_		0xa0
#define	ASCII_		0xa1
#define	ASCII_		0xa2
#define	ASCII_		0xa3
#define	ASCII_		0xa4
#define	ASCII_		0xa5
#define	ASCII_		0xa6
#define	ASCII_		0xa7
#define	ASCII_		0xa8
#define	ASCII_		0xa9
#define	ASCII_		0xaa
#define	ASCII_		0xab
#define	ASCII_		0xac
#define	ASCII_		0xad
#define	ASCII_		0xae
#define	ASCII_		0xaf
#define	ASCII_		0xb0
#define	ASCII_		0xb1
#define	ASCII_		0xb2
#define	ASCII_		0xb3
#define	ASCII_		0xb4
#define	ASCII_		0xb5
#define	ASCII_		0xb6
#define	ASCII_		0xb7
#define	ASCII_		0xb8
#define	ASCII_		0xb9
#define	ASCII_		0xba
#define	ASCII_		0xbb
#define	ASCII_		0xbc
#define	ASCII_		0xbd
#define	ASCII_		0xbe
#define	ASCII_		0xbf
#define	ASCII_		0xc0
#define	ASCII_		0xc1
#define	ASCII_		0xc2
#define	ASCII_		0xc3
#define	ASCII_		0xc4
#define	ASCII_		0xc5
#define	ASCII_		0xc6
#define	ASCII_		0xc7
#define	ASCII_		0xc8
#define	ASCII_		0xc9
#define	ASCII_		0xca
#define	ASCII_		0xcb
#define	ASCII_		0xcc
#define	ASCII_		0xcd
#define	ASCII_		0xce
#define	ASCII_		0xcf
#define	ASCII_		0xd0
#define	ASCII_		0xd1
#define	ASCII_		0xd2
#define	ASCII_		0xd3
#define	ASCII_		0xd4
#define	ASCII_		0xd5
#define	ASCII_		0xd6
#define	ASCII_		0xd7
#define	ASCII_		0xd8
#define	ASCII_		0xd9
#define	ASCII_		0xda
#define	ASCII_		0xdb
#define	ASCII_		0xdc
#define	ASCII_		0xdd
#define	ASCII_		0xde
#define	ASCII_		0xdf
#define	ASCII_		0xe0
#define	ASCII_		0xe1
#define	ASCII_		0xe2
#define	ASCII_		0xe3
#define	ASCII_		0xe4
#define	ASCII_		0xe5
#define	ASCII_		0xe6
#define	ASCII_		0xe7
#define	ASCII_		0xe8
#define	ASCII_		0xe9
#define	ASCII_		0xea
#define	ASCII_		0xeb
#define	ASCII_		0xec
#define	ASCII_		0xed
#define	ASCII_		0xee
#define	ASCII_		0xef
#define	ASCII_		0xf0
#define	ASCII_		0xf1
#define	ASCII_		0xf2
#define	ASCII_		0xf3
#define	ASCII_		0xf4
#define	ASCII_		0xf5
#define	ASCII_		0xf6
#define	ASCII_		0xf7
#define	ASCII_		0xf8
#define	ASCII_		0xf9
#define	ASCII_		0xfa
#define	ASCII_		0xfb
#define	ASCII_		0xfc
#define	ASCII_		0xfd
#define	ASCII_		0xfe
*/
#define	ASCII_255	0xff


BOOL GetAsciiSymbol(BYTE byAscii, CString& sSymbol, BOOL bCRLFAsString=TRUE);
	 // Setzt den ASCII-Code byAscii um in das druckbare Zeichen
	 // oder die Kurzbezeichnung in der Form <CR>, <ACK> etc. inklusive Anfuehrungszeichen
	 // PARAMETER
	 //    byAscii      : ASCII-Code, der umgesetzt werden soll
	 //    sSymbol      : Buffer, in dem das druckbare Zeichen oder die Kurzbezeichnung
	 //                   zurueckgeliefert wird
	 //    bCRLFAsString: TRUE : CR und LF werden als Kurzbezeichnung <CR> und <LF> zurueckgegeben
	 //                   FALSE: CR und LF werden als ESC-Zeichen \r und \n zurueckgegeben
	 // RETURN
	 //    TRUE : sSymbol ist gueltig
	 //    FALSE: byAscii ist auﬂerhalb des Wertebereiches, sSymbol ist nicht gueltig

#endif // ASCII_HEADER
