/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: Altera.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/Altera.h $
// CHECKIN:		$Date: 25.08.04 15:00 $
// VERSION:		$Revision: 28 $
// LAST CHANGE:	$Modtime: 25.08.04 13:55 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __ALTERA_H__
#define __ALTERA_H__

#include "..\Include\SpiDefs.h" 

#define SPI1_ALTERA_BAUDRATE	10000000 	//10 MHz

#define pALTERA_REG     ((volatile WORD*)0x20000000)
#define pALTERA_REG_END ((volatile WORD*)0x20000020)

#define PF0 	0x0001
#define PF1 	0x0002
#define PF2 	0x0004
#define PF3 	0x0008
#define PF4 	0x0010
#define PF5 	0x0020
#define PF6 	0x0040
#define PF7 	0x0080
#define PF8 	0x0100
#define PF9 	0x0200
#define PF10 	0x0400
#define PF11 	0x0800
#define PF12 	0x1000
#define PF13 	0x2000
#define PF14 	0x4000
#define PF15 	0x8000

//Altera Porogrammierung
#define CONF_DONE	PF13
#define NCONFIG 	PF12
#define NSTATUS		PF11

#define DCOMI 		0x00000001;

#ifdef PROTO
// Registerdefinitionen
#define ALT_ALARM_IN			0x00
#define ALT_ALARM_OUT			0x01
#define ALT_PC_LEDS				0x04
#define ALT_RESET_BF533			0x05
#define ALT_SLAVE_SELECT		0x08
#define ALT_CS					0x0D
#define ALT_TERMINATION			0x0F
#define ALT_CONTROL				0x10
#define ALT_ENABLE				0x11
#define ALT_PC_KEYS				0x13
#define ALT_ADAPTER_SELECT		0x14
#define ALT_VARIATION			0x16

// Bitdefinitionen des ALT_PC_LEDS-Registers (0x08)
#define ALT_PC_LED1_F1_BIT		0x00
#define ALT_PC_LED1_F2_BIT		0x01
#define ALT_PC_LED2_F1_BIT		0x02
#define ALT_PC_LED2_F2_BIT		0x03

// Bitdefinitionen des ALT_CS-Registers (0x1A)
#define SPI_CROSSPOINT_CS_BIT	0x00

// Bitdefinitionen des ALT_CONTROL-Registers (0x20)
#define ALT_CROSSPOINT_SHUTDOWN_BIT	0x00
#define ALT_SWITSH_POWER_BUTTON_BIT	0x02
#define ALT_SWITCH_RESET_BUTTON_BIT	0x03

// Bitdefinitionen des ALT_ENABLE-Registers (0x22)
#define ALT_24MHZ_EN_BIT		0x00
#define ALT_27MHZ_A_EN_BIT		0x01
#define ALT_27MHZ_B_EN_BIT		0x02
#define ALT_EN_EP_BOOT_BIT		0x03
#define ALT_EN_BF_BOOT_BIT		0x04
#define ALT_VIDEO_ENABLE_BIT	0x05
#define ALT_JTAG_EN_BIT			0x06

#else

// Registerdefinitionen
#define ALT_ALARM_OUT			0x00
#define ALT_PC_LEDS				0x01
#define ALT_RESET_BF533			0x02
#define ALT_SLAVE_SELECT		0x03
#define ALT_BF533_GPIO_2_X		0x04
#define ALT_BF533_GPIO_3_X		0x04
#define ALT_TERMINATION			0x05
#define ALT_CTRL				0x06
#define ALT_ALARM_IN			0x07
#define ALT_PC_KEYS_ADAPT_SEL	0x08
#define ALT_STATUS_TEST			0x09
#define ALT_BF533_GPIO_0_X		0x0A
#define ALT_BF533_TMR1_X		0x0A
#define ALT_VARIATION			0x0B

// Bitdefinitionen des ALT_PC_LEDS-Registers (0x01)
#define ALT_PC_LED1_F1_BIT					8
#define ALT_PC_LED2_F1_BIT					10

// Bitdefinitionen des ALT_CTRL-Registers (0x06)
#define ALT_CTRL_CROSSPOINT_SHUTDOWN_BIT	0
#define ALT_CTRL_CROSSPOINT_CS_BIT			1
#define ALT_CTRL_SWITSH_POWER_BUTTON_BIT	2
#define ALT_CTRL_SWITCH_RESET_BUTTON_BIT	3
#define ALT_CTRL_BYPASS_BIT					4
#define ALT_CTRL_NMI_BIT					5
#define ALT_CTRL_SPI_BF535_HOLD_BIT			6
#define ALT_CTRL_SPI_BF535_WP_BIT			7	
#define ALT_CTRL_24MHZ_EN_BIT				8
#define ALT_CTRL_27MHZ_A_EN_BIT				9
#define ALT_CTRL_27MHZ_B_EN_BIT				10
#define ALT_CTRL_EN_BF_BOOT_BIT				11
#define ALT_CTRL_VIDEO_ENABLE_BIT			12
#define ALT_CTRL_JTAG_EN_BIT				13

#endif
	
void AlteraInit();
BOOL IsAlteraValid();

BOOL writeAlteraReg(BYTE byReg, WORD wVal);
BOOL readAlteraReg(BYTE byReg, WORD& wVal);

BOOL progAlteraInit();
BOOL progAlteraStart();
BOOL progAlteraTX(BYTE* pScr, DWORD dwProgLength);
BOOL CreateDescriptorList(BYTE* pSrc, DWORD dwLen);
BOOL WaitUntilAlteraTransferCompleted(int nTimeOut);

BOOL InitAlteraClockTimer(int nFreq);
void StartAlteraClockTimer();
void StopAlteraClockTimer();

#endif //__ALTERA_H__

