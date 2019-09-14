// ----------------------------------------------------------------------
//   Blackfin BF537
//
//   Funktion zur Bedienung des IO-Expanders auf der EBE284
// ----------------------------------------------------------------------
//
//   Copyright (c) 2001-2011 ELAC-Nautik GmbH.
//
//   WEB		:	http://www.elac-nautik.com
//   Support	:	support@elac-nautik.com
//
//
//   Filename	:	BF1_IOExpander.c
//   Autor		:	M. Sieger
//
// ----------------------------------------------------------------------


#include <cdefBF537.h>
#include "..\ST30_globals.h"
#include "..\telegrams.h"
#include "BF1_locals.h"
#include "BF1_SPI.h"
#include "BF1_IOExpander.h"
#include "BF1_Modbus.h"
#include "BF1_Transmit.h"


// Initdata siehe Tabelle in 'IOExp_Init'
static unsigned short IOExp_Initdata[] = {	0x0000,		// D1 NOOP (zu Synchronisationszwecken nach Reset)
											0x0000,		// D2 NOOP (zu Synchronisationszwecken nach Reset)
											0x0401,		// D1 Disable Shutdown Mode
											0x0401,		// D2 Disable Shutdown Mode
											0x0000,		// D1 NOOP
											0x0625,		// D2 Set Transition Detect Mask (P29|P26|P24)
											0x0955,		// D1 Config P[ 7.. 4] = 0b0101.0101
											0x09FF,		// D2 Config P[ 7.. 4] = 0b1111.1111
											0x0AF5,		// D1 Config P[11.. 8] = 0b1001.0101
											0x0AFF,		// D2 Config P[11.. 8] = 0b1111.1111
											0x0BAA,		// D1 Config P[15..12] = 0b1010.1010
											0x0BFF,		// D2 Config P[15..12] = 0b1111.1111
											0x0C6E,		// D1 Config P[19..16] = 0b0110.1110
											0x0C7D,		// D2 Config P[19..16] = 0b0111.1101
											0x0D95,		// D1 Config P[23..20] = 0b1001.0101
											0x0D55,		// D2 Config P[23..20] = 0b0101.0101
											0x0EAA,		// D1 Config P[27..24] = 0b1010.1010
											0x0E67,		// D2 Config P[27..24] = 0b0110.0111
											0x0F6A,		// D1 Config P[31..28] = 0b0110.1010
											0x0F5D,		// D2 Config P[31..28] = 0b0101.1101
											0x4466,		// D1 Write  P[11.. 4] = 0b0110.0110
											0x4400,		// D2 Write  P[11.. 4] = 0b0000.0000
											0x4C00,		// D1 Write  P[19..12] = 0b0000.0000
											0x4C10,		// D2 Write  P[19..12] = 0b0001.0000
											0x5400,		// D1 Write  P[27..20] = 0b0000.0000
											0x5400,		// D2 Write  P[27..20] = 0b0000.0000
											0x5C00,		// D1 Write  P[31..28] = 0b0000.0000
											0x5C00		// D2 Write  P[31..28] = 0b0000.0000
										 };


										 


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
/*! \brief	Schreiben von zwei Datenworten für IO-Expander #2 und #1
 *
 * 	Da es sich um kaskadierte Expander handelt, muß SSEL5 manuell durch PF5
 *  nachgebildet werden (Rücksetzen des SSEL erst nach 32 Bits)
 *
 *	\param	D1_data	zu schreibendes Wort für Expander #2 (D1)
 *	\param	D2_data	zu schreibendes Wort für Expander #1 (D2)
 */
 
void IOExp_Write32 (unsigned short D1_data, unsigned short D2_data)
{
	
	*pSPI_CTL =   TDBR_CORE			// Start with write of TDBR
//				| CPOL				// rising edge
//				| CPHA				// clock phase
				| SIZE				// 16 Bit
				| MSTR				// Master
				| SPE;				// Enable
	ssync();
	
	*pPORTFIO_CLEAR = SSEL_IOEXP;	// SSEL5 (PF5) auf Low

	SPI_SendSingleWord(D1_data);	
	SPI_SendSingleWord(D2_data);	
	
	*pPORTFIO_SET = SSEL_IOEXP;		// SSEL5 (PF5) auf High

	*pSPI_CTL = 0;					// SPI abschalten
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
/*! \brief	Lesen der IO-Expander #1 bzw. #2
 *
 *	\param	cmd		Kommando für Lesebefehl
 *
 *	\result			gelesenes 8 Bit Datenwort
 */
 
unsigned char IOExp_Read (enum_IOExp_Command cmd)
{
	unsigned short	prog;
	unsigned short	D1_result;
	unsigned short	D2_result;
	
	// 1. Schritt: Schnittstelle aufsetzen
	prog = (cmd << 8) | 0x8000;	// MSB=cmd; Bit 8 wird rausgeschoben; 
								// Bit 15 gesetzt für RD
	
	*pSPI_CTL =   TDBR_CORE			// Start with write of TDBR
				| SIZE				// 16 Bit
				| MSTR				// Master
				| SPE;				// Enable
				
	ssync();
				
	*pPORTFIO_CLEAR = SSEL_IOEXP;	// SSEL5 (PF5) auf Low

	// 2. Schritt: Kommando einschreiben
	if (cmd >= IO_Enable_TxVoltage)				// Adressierung von D1
	{
		SPI_SendSingleWord(prog);	
		SPI_SendSingleWord(IOEXP_NOOP);	
	}
	else										// Adressierung von D2
	{
		SPI_SendSingleWord(IOEXP_NOOP);	
		SPI_SendSingleWord(prog);	
	}
	
	*pPORTFIO_SET = SSEL_IOEXP;		// SSEL5 (PF5) auf High
	ssync();
	
	// 3. Schritt: Ergebnisse abholen	
	*pPORTFIO_CLEAR = SSEL_IOEXP;	// SSEL5 (PF5) auf Low

	SPI_SendSingleWord(IOEXP_NOOP);	
	D1_result = *pSPI_RDBR;				// 1. Ergebnis auslesen
	
	SPI_SendSingleWord(IOEXP_NOOP);	
	D2_result = *pSPI_RDBR;				// 2. Ergebnis auslesen
	
	*pPORTFIO_SET = SSEL_IOEXP;		// SSEL5 (PF5) auf High
	ssync();

	*pSPI_CTL = 0;					// SPI abschalten
	
	// 3. Schritt: Ergebnis übergeben
	if (cmd >= IO_Enable_TxVoltage)			
		return (D1_result & 0x00FF);	// von Expander D1
	else
		return (D2_result & 0x00FF);	// von Expander D2

}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
/*! \brief	Schreiben eines Wertes in die IO-Expandern D1 bzw. D2
 *
 *	\param	cmd		Kommando für Schreibbefehl an Expander D1 bzw. D2
 *	\param	data	zu schreibendes 8 Bit Datenwort
 */
 
void IOExp_Write (enum_IOExp_Command cmd, unsigned char data)
{
	unsigned short	prog;
	
	prog = (cmd << 8) | data;		// MSB=cmd; LSB=state; Bit 8 wird rausgeschoben
	
	if (cmd >= IO_Enable_TxVoltage)			
		IOExp_Write32 (prog, IOEXP_NOOP);	// Bit an Expander D1
	else
		IOExp_Write32 (IOEXP_NOOP, prog);	// Bit an Expander D2
}


void IOExp_EnableTransitionDetection(void)
{
	IOExp_Write (IO_ConfigurationRegister, IOEXP_ENABLETRANSITION);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
/*! \brief	Initialisierung der beiden kaskadierten IO-Expander
 *  siehe hierzu auch SP 58 933 8001
 *  bei Inputs wird der Pullup-Zustand angegeben, 
 *  bei Outputs der Initwert
 *
 *	IO-Expander #1 (D2):
 *	P4		Modbus_Adr0			In		Pullup
 *	P5		Modbus_Adr1			In		Pullup
 *	P6		Modbus_Adr2			In		Pullup
 *	P7		Modbus_Adr3			In		Pullup
 *	P8		Modbus_Adr4			In		Pullup
 *	P9		Modbus_Adr5			In		Pullup
 *	P10		Modbus_Adr6			In		Pullup
 *	P11		Modbus_Adr7			In		Pullup
 *	P12		BF561_installed		In		Pullup
 *	P13		Modbus_Baudrate		In		Pullup
 *	P14		Frequenzkennung		In		Pullup
 *	P15		Nato_Conformity		In		Pullup
 *	P16		PLL_PowerDown		Out		1
 *	P17		Modbus_Interface1	In		Pullup
 *	P18		Modbus_Protocol		In		Pullup
 *	P19		TxPower_0dB			Out		0
 *	P20		TxPower_-6dB		Out		0
 *	P21		TxPower_-12dB		Out		0
 *	P22		TxPower_-18dB		Out		0
 *	P23		Dummylast			Out		0
 *	P24		USV_OK				In		Pullup
 *	P25		BF561_Reset			Out		0
 *	P26		USV_ERROR			In		no Pullup	PortChange
 *	P27		Status_Aktiviert	Out		0
 *	P28		Status_Fehler		Out		0
 *	P29		Status_Disable		Int		Pullup		PortChange
 *	P30		Status_Bereit		Out		0
 *	P31		PortChange_Output	Out		0
 
 *	IO-Expander #2 (D1):
 *	P4		Enable_TxVoltage	Out		0
 *	P5		Enable_Rx1			Out		1
 *	P6		Enable_Rx2			Out		1
 *	P7		Attenuator			Out		0
 *	P8		Enable_Simulator	Out		0
 *	P9		Inhibit_Rx1Test		Out		1
 *	P10		Inhibit_Rx2Test		Out		1
 *	P11		unused				In		no Pullup
 *	P12		Spare2_In			In		no Pullup
 *	P13		Test1				In		no Pullup
 *	P14		Test2				In		no Pullup
 *	P15		Test3				In		no Pullup
 *	P16		Test4				In		no Pullup
 *	P17		Modbus_Interface2	In		Pullup
 *	P18		unused				In		no Pullup
 *	P19		LED1				Out		0
 *	P20		LED2				Out		0
 *	P21		LED3				Out		0
 *	P22		LED4				Out		0
 *	P23		unused				In		no Pullup
 *	P24		unused				In		no Pullup
 *	P25		unused				In		no Pullup
 *	P26		unused				In		no Pullup
 *	P27		unused				In		no Pullup
 *	P28		unused				In		no Pullup
 *	P29		unused				In		no Pullup
 *	P30		unused				In		no Pullup
 *	P31		PortChange_Output	Out		0 			(unused)
 */
 
void IOExp_Init (void)
{
	int	i;
	
	for (i=0; i<(sizeof(IOExp_Initdata)/2); i+=2)
		IOExp_Write32 (IOExp_Initdata[i], IOExp_Initdata[i+1]);
}



