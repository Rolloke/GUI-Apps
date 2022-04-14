/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CZoran.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CZoran.cpp $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include <dos.h>
#include "stdafx.h"
#include "CIo.h"
#include "MiCoDefs.h"
#include "MiCoReg.h"
#include "CZoran.h"
#include "h55_tbls.h"

void (interrupt far *g_pOldInt)(void);  /* Save old kbd handler */
void (interrupt far *g_pNewInt)(void);  /* Points to keybrd routine */
void interrupt far IRQHandler(void);
DWORD	g_dwIRQCounter;			// Zählt die auflaufenden Videointerrupts.
WORD	g_wIOBase;              // Basisadresse
WORD	g_wIRQ;					// Interruptnummer
WORD	g_wPicCntrl;			// PIC Controll Register
WORD	g_wPicMask;             // PIC Mask Register

int g_nIntLookup[] = {
						0x08,  // IRQ 0
						0x09,  // IRQ 1
						0x0A,  // IRQ 2
						0x0B,  // IRQ 3
						0x0C,  // IRQ 4
						0x0D,  // IRQ 5
						0x0E,  // IRQ 6
						0x0F,  // IRQ 7
						0x70,  // IRQ 8
						0x71,  // IRQ 9
						0x72,  // IRQ 10
						0x73,  // IRQ 11
						0x74,  // IRQ 12
						0x75,  // IRQ 13
						0x77,  // IRQ 14
						0x78,  // IRQ 15
					};	

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::CZoran
CZoran::CZoran(WORD wIOBase)
{
	m_bOK								= FALSE;
	m_pzCoder							= NULL;
	m_pZIo								= NULL;
	m_nEncoderState						= MICO_ENCODER_OFF;
	m_nDecoderState						= MICO_DECODER_OFF;
	m_wFormat							= MICO_ENCODER_HIGH;
	m_wFramerate						= 12;
	m_dwBPF								= 30000;
	m_wYBackgnd							= 0x7ff;
	m_wUVBackgnd						= 0x7ff;
	m_wFeVideoFormat					= MICO_PAL_CCIR;
    m_pBuffer                           = NULL;
    
    m_wIOBase							= wIOBase;
	g_wIOBase 							= wIOBase;
	m_wFeHStart[MICO_NTSC_SQUARE]		= 0;
	m_wFeHEnd[MICO_NTSC_SQUARE]			= 0;
	m_wFeHTotal[MICO_NTSC_SQUARE]		= 0;
	m_wFeHSStart[MICO_NTSC_SQUARE]		= 0;
	m_wFeHSEnd[MICO_NTSC_SQUARE]		= 0;
	m_wFeBlankStart[MICO_NTSC_SQUARE]	= 0;
	m_wFeVStart[MICO_NTSC_SQUARE]		= 0;
	m_wFeVEnd[MICO_NTSC_SQUARE]			= 0;
	m_wFeVTotal[MICO_NTSC_SQUARE]		= 0;

	m_wFeHStart[MICO_PAL_SQUARE]		= 0;
	m_wFeHEnd[MICO_PAL_SQUARE]			= 0;
	m_wFeHTotal[MICO_PAL_SQUARE]		= 0;
	m_wFeHSStart[MICO_PAL_SQUARE]		= 0;
	m_wFeHSEnd[MICO_PAL_SQUARE]			= 0;
	m_wFeBlankStart[MICO_PAL_SQUARE]	= 0;
	m_wFeVStart[MICO_PAL_SQUARE]		= 0;
	m_wFeVEnd[MICO_PAL_SQUARE]			= 0;
	m_wFeVTotal[MICO_PAL_SQUARE]		= 0;
	m_wFeNMCU[MICO_PAL_SQUARE]			= 0;

	m_wFeHStart[MICO_NTSC_CCIR]			= 0;
	m_wFeHEnd[MICO_NTSC_CCIR]			= 0;
	m_wFeHTotal[MICO_NTSC_CCIR]			= 0;
	m_wFeHSStart[MICO_NTSC_CCIR]		= 0;
	m_wFeHSEnd[MICO_NTSC_CCIR]			= 0;
	m_wFeBlankStart[MICO_NTSC_CCIR]		= 0;
	m_wFeVStart[MICO_NTSC_CCIR]			= 0;
	m_wFeVEnd[MICO_NTSC_CCIR]			= 0;
	m_wFeVTotal[MICO_NTSC_CCIR]			= 0;
	m_wFeNMCU[MICO_NTSC_CCIR]			= 0;

	m_wFeHStart[MICO_PAL_CCIR]			= 0;
	m_wFeHEnd[MICO_PAL_CCIR]			= 0;
	m_wFeHTotal[MICO_PAL_CCIR]			= 0;
	m_wFeHSStart[MICO_PAL_CCIR]			= 0;
	m_wFeHSEnd[MICO_PAL_CCIR]			= 0;
	m_wFeBlankStart[MICO_PAL_CCIR]		= 0;
	m_wFeVStart[MICO_PAL_CCIR]			= 0;
	m_wFeVEnd[MICO_PAL_CCIR]			= 0;
	m_wFeVTotal[MICO_PAL_CCIR]			= 0;
	m_wFeNMCU[MICO_PAL_CCIR]			= 0;
	
	// Backend mit den gleichen Einstellungen wie das Frontend konfigurieren
	m_wBeVideoFormat= m_wFeVideoFormat;
	m_wBeHStart		= m_wFeHStart[m_wBeVideoFormat];
	m_wBeHEnd		= m_wFeHEnd[m_wBeVideoFormat];
	m_wBeHTotal		= m_wFeHTotal[m_wBeVideoFormat];
	m_wBeHSStart	= m_wFeHSStart[m_wBeVideoFormat];
	m_wBeHSEnd		= m_wFeHSEnd[m_wBeVideoFormat];
	m_wBeBlankStart	= m_wFeBlankStart[m_wBeVideoFormat];
	m_wBeVStart		= m_wFeVStart[m_wBeVideoFormat];
	m_wBeVEnd		= m_wFeVEnd[m_wBeVideoFormat];
	m_wBeVTotal		= m_wFeVTotal[m_wBeVideoFormat]; 
	m_wBeNMCU		= m_wFeNMCU[m_wBeVideoFormat];
  
	m_dwTCVData		= 20000L*8L;
	m_dwTCVNet 		= JPEG_BUFFER_SIZE*8-15;
	m_wSF			= 1*256;
	m_dwDecodedFrames = 0;
	m_dwEncodedFrames = 0;
		
	m_pBuffer = new WORD[32000];
	if (!m_pBuffer)
	{
	 	WK_TRACE(TRUE, "cant allocate memory\n");
	 	return;
	}
	
	
	m_bOK = TRUE;

	// Zoran reset
	Reset();
	DisableBrdIRQ();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::~CZoran
CZoran::~CZoran()
{                    
	delete m_pBuffer;
	m_pBuffer = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::IsValid
BOOL CZoran::IsValid()
{
	return m_bOK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::Reset
BOOL CZoran::Reset()
{
    BYTE byReset = 0;

	// I2C-Bus Controller reseten...
	byReset = m_IO.ReadFromLatch(m_wIOBase | RESET_OFFSET);
	byReset = (BYTE)CLRBIT(byReset, PARAM_RESET_ZR);
	m_IO.WriteToLatch(m_wIOBase | RESET_OFFSET, byReset);
	Sleep(10);

	byReset = (BYTE)SETBIT(byReset, PARAM_RESET_ZR);

	m_IO.WriteToLatch(m_wIOBase | RESET_OFFSET, byReset);

	Sleep(10);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::Open
BOOL CZoran::Open()
{
	// Mico Identifier Register auslesen
	m_byMiCoID = m_IO.Input(m_wIOBase | MICO_ID_OFFSET);

	m_wFeHStart[MICO_NTSC_CCIR]			= 8;
	m_wFeHEnd[MICO_NTSC_CCIR]			= 712;
	m_wFeHTotal[MICO_NTSC_CCIR]			= 858;
	m_wFeHSStart[MICO_NTSC_CCIR]		= 736;
	m_wFeHSEnd[MICO_NTSC_CCIR]			= 784;
	m_wFeBlankStart[MICO_NTSC_CCIR]		= 720;
	m_wFeVStart[MICO_NTSC_CCIR]			= 14;
	m_wFeVEnd[MICO_NTSC_CCIR]			= 254;
	m_wFeVTotal[MICO_NTSC_CCIR]			= 525;
	m_wFeNMCU[MICO_NTSC_CCIR]			= 43;

	m_wFeHStart[MICO_PAL_CCIR]			= 8;
	m_wFeHEnd[MICO_PAL_CCIR]			= 712;
	m_wFeHTotal[MICO_PAL_CCIR]			= 864;
	m_wFeHSStart[MICO_PAL_CCIR]			= 732;
	m_wFeHSEnd[MICO_PAL_CCIR]			= 784;
	m_wFeBlankStart[MICO_PAL_CCIR]		= 720;
	m_wFeVStart[MICO_PAL_CCIR]			= 17;
	m_wFeVEnd[MICO_PAL_CCIR]			= 297;
	m_wFeVTotal[MICO_PAL_CCIR]			= 625;
	m_wFeNMCU[MICO_PAL_CCIR]			= 43;

	// 58 MHz Quarz
	if (m_byMiCoID == 0x22)
	{
		m_wFeHStart[MICO_NTSC_SQUARE]		= 0;
		m_wFeHEnd[MICO_NTSC_SQUARE]			= 640;
		m_wFeHTotal[MICO_NTSC_SQUARE]		= 780;
		m_wFeHSStart[MICO_NTSC_SQUARE]		= 658;
		m_wFeHSEnd[MICO_NTSC_SQUARE]		= 698;
		m_wFeBlankStart[MICO_NTSC_SQUARE]	= 640;
		m_wFeVStart[MICO_NTSC_SQUARE]		= 14;
		m_wFeVEnd[MICO_NTSC_SQUARE]			= 254; //525;
		m_wFeVTotal[MICO_NTSC_SQUARE]		= 525; //39;
		m_wFeNMCU[MICO_NTSC_SQUARE]			= 39;

		m_wFeHStart[MICO_PAL_SQUARE]		= 8;
		m_wFeHEnd[MICO_PAL_SQUARE]			= 744;
		m_wFeHTotal[MICO_PAL_SQUARE]		= 928;
		m_wFeHSStart[MICO_PAL_SQUARE]		= 766;
		m_wFeHSEnd[MICO_PAL_SQUARE]			= 820;
		m_wFeBlankStart[MICO_PAL_SQUARE]	= 752;
		m_wFeVStart[MICO_PAL_SQUARE]		= 16;
		m_wFeVEnd[MICO_PAL_SQUARE]			= 304;
		m_wFeVTotal[MICO_PAL_SQUARE]		= 625;
		m_wFeNMCU[MICO_PAL_SQUARE]			= 45;
	}
	else if (m_byMiCoID == 0x30)	// 59MHz Quarz
	{
		m_wFeHStart[MICO_NTSC_SQUARE]		= 0;
		m_wFeHEnd[MICO_NTSC_SQUARE]			= 640;
		m_wFeHTotal[MICO_NTSC_SQUARE]		= 780;
		m_wFeHSStart[MICO_NTSC_SQUARE]		= 658;
		m_wFeHSEnd[MICO_NTSC_SQUARE]		= 698;
		m_wFeBlankStart[MICO_NTSC_SQUARE]	= 640;
		m_wFeVStart[MICO_NTSC_SQUARE]		= 14;
		m_wFeVEnd[MICO_NTSC_SQUARE]			= 254;
		m_wFeVTotal[MICO_NTSC_SQUARE]		= 525;
		m_wFeNMCU[MICO_NTSC_SQUARE]			= 39;

		m_wFeHStart[MICO_PAL_SQUARE]		= 0;
		m_wFeHEnd[MICO_PAL_SQUARE]			= 768;
		m_wFeHTotal[MICO_PAL_SQUARE]		= 944;
		m_wFeHSStart[MICO_PAL_SQUARE]		= 782;
		m_wFeHSEnd[MICO_PAL_SQUARE]			= 838;
		m_wFeBlankStart[MICO_PAL_SQUARE]	= 768;
		m_wFeVStart[MICO_PAL_SQUARE]		= 16;
		m_wFeVEnd[MICO_PAL_SQUARE]			= 304;
		m_wFeVTotal[MICO_PAL_SQUARE]		= 625;
		m_wFeNMCU[MICO_PAL_SQUARE]			= 47;
	}
	
	// Backend mit den gleichen Einstellungen wie das Frontend konfigurieren
	m_wBeVideoFormat= m_wFeVideoFormat;
	m_wBeHStart		= m_wFeHStart[m_wBeVideoFormat];
	m_wBeHEnd		= m_wFeHEnd[m_wBeVideoFormat];
	m_wBeHTotal		= m_wFeHTotal[m_wBeVideoFormat];
	m_wBeHSStart	= m_wFeHSStart[m_wBeVideoFormat];
	m_wBeHSEnd		= m_wFeHSEnd[m_wBeVideoFormat];
	m_wBeBlankStart	= m_wFeBlankStart[m_wBeVideoFormat];
	m_wBeVStart		= m_wFeVStart[m_wBeVideoFormat];
	m_wBeVEnd		= m_wFeVEnd[m_wBeVideoFormat];
	m_wBeVTotal		= m_wFeVTotal[m_wBeVideoFormat]; 
	m_wBeNMCU		= m_wFeNMCU[m_wBeVideoFormat];

	// Zoran reset
	Reset();

	// Board-Interrupts sperren
	DisableBrdIRQ();

	// Dies sind die original Zoran-Klassen
	m_pZIo		= new ZoranIo(m_wIOBase);
	m_pzCoder	= new Z050_Coder(m_pZIo);

	// Interrupts freigeben
	EnableBrdIRQ();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::Close
BOOL CZoran::Close()
{
	// Interrupts sperren
	DisableBrdIRQ();

	delete m_pzCoder;
	m_pzCoder = NULL;
	
	delete m_pZIo;
	m_pZIo = NULL;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::SetInputSource
void CZoran::SetInputSource(WORD wFrontEnd, WORD wExtCard, WORD wSource, BYTE byYPort, BYTE byCPort, WORD wFormat, DWORD dwBPF)
{
	DWORD	dwTimeOut = GetTickCount();
	BYTE	byCSVDIN;
	BYTE	byCSVIAN;
	BOOL	bChanges = FALSE;

//	WK_TRACE(TRUE, "CZoran::SetInputSource\tExtCard=%u\tSource%u\n", wExtCard, wSource);
	// Soll Megrafrontend benutzt werden ?
	if (wExtCard == MICO_EXTCARD_MEGRA)
	{
		// Megra über Latch aktivieren
		byCSVDIN = ChipSelectVideoInDigitalIn();
		byCSVDIN = (BYTE)SETBIT(byCSVDIN, SELECT_MEGRA_BIT);	
		ChipSelectVideoInDigitalOut(byCSVDIN);
		WK_TRACE(TRUE, "CZoran::SetInputSource\tSetExtCard\tMegra");
	}
	else
	{
		// Megra über Latch deaktivieren
		byCSVDIN = ChipSelectVideoInDigitalIn();
		byCSVDIN = (BYTE)CLRBIT(byCSVDIN, SELECT_MEGRA_BIT);	
		ChipSelectVideoInDigitalOut(byCSVDIN);
        
		// Gewünschte Videoquelle auswählen
		byCSVIAN = ChipSelectVideoInAnalogIn(wFrontEnd, wExtCard) & 0xc0;
		byCSVIAN = byCSVIAN | byYPort << 3 | byCPort;		
		ChipSelectVideoInAnalogOut(wFrontEnd, wExtCard, byCSVIAN);

		// Auf gewünschten CoVi schalten
		byCSVIAN = ChipSelectVideoInAnalogIn(wFrontEnd, MICO_EXTCARD0) & 0x3f;
		byCSVIAN = byCSVIAN | wExtCard << 6; 					
		ChipSelectVideoInAnalogOut(wFrontEnd, MICO_EXTCARD0, byCSVIAN);

		// Frontend wählen
		if (wFrontEnd == MICO_FE0)
			byCSVDIN = CLRBIT(byCSVDIN, SELECT_VD_BIT);
		else if (wFrontEnd == MICO_FE1)
			byCSVDIN = SETBIT(byCSVDIN, SELECT_VD_BIT);

		// Wenn das Bildformat oder die Bildgröße (in Bytes) geändert werden
		// soll während der Encoder läuft, so wird dieser zunächst gestoppt und
		// die Parameter geändert, anschließend wird auf die neue kamera geschaltet
		// und der Encoder wieder gestartet.
		if (GetEncoderState() == MICO_ENCODER_ON)
		{
			bChanges = TRUE;
	 		StopEncoding();
		}
		SetFormat(wFormat);
		SetBPF(dwBPF);

		ChipSelectVideoInDigitalOut(byCSVDIN);

		// Encoder wieder starten. 
		if (bChanges)
			StartEncoding();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::SetFeVideoFormat
void CZoran::SetFeVideoFormat(WORD wFormat)
{
	switch (wFormat)
	{
		case MICO_PAL_SQUARE:
//			WK_TRACE(TRUE, "CZoran::SetFeVideoFormat\tPAL_SQUARE\n");
			break;					  
		
		case MICO_NTSC_SQUARE:
//			WK_TRACE(TRUE, "CZoran::SetFeVideoFormat\tNTSC_SQUARE\n");
			break;

		case MICO_PAL_CCIR:
//			WK_TRACE(TRUE, "CZoran::SetFeVideoFormat\tPAL_CCIR\n");
			break;
	
		case MICO_NTSC_CCIR:
//			WK_TRACE(TRUE, "CZoran::SetFeVideoFormat\tNTSC_CCIR\n");
			break;

		default:
			WK_TRACE(TRUE, "CZoran::SetFeVideoFormat\tUnbekanntes Videoformat:%u\n", wFormat);
			return;
	}

	m_wFeVideoFormat	= wFormat;

	if (GetEncoderState() == MICO_ENCODER_ON)
	{
		StopEncoding();
		StartEncoding();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::SetBeVideoFormat
void CZoran::SetBeVideoFormat(WORD wFormat)
{

	switch (wFormat)
	{
		case MICO_PAL_SQUARE:
//			WK_TRACE(TRUE, "CZoran::SetBeVideoFormat\tPAL_SQUARE\n");
			break;					  
		
		case MICO_NTSC_SQUARE:
//			WK_TRACE(TRUE, "CZoran::SetBeVideoFormat\tNTSC_SQUARE\n");
			break;

		case MICO_PAL_CCIR:
//			WK_TRACE(TRUE, "CZoran::SetBeVideoFormat\tPAL_CCIR\n");
			break;
	
		case MICO_NTSC_CCIR:
//			WK_TRACE(TRUE, "CZoran::SetBeVideoFormat\tNTSC_CCIR\n");
			break;

		default:
			WK_TRACE(TRUE, "CZoran::SetBeVideoFormat\tUnbekanntes Videoformat:%u\n", wFormat);
			return;
	}


	m_wBeHTotal		= m_wFeHTotal[wFormat];
	m_wBeVTotal		= m_wFeVTotal[wFormat]; 
	m_wBeBlankStart	= m_wFeBlankStart[wFormat];
	m_wBeHStart		= m_wFeHStart[wFormat];
	m_wBeHEnd		= m_wFeHEnd[wFormat];
	m_wBeHSStart	= m_wFeHSStart[wFormat];
	m_wBeHSEnd		= m_wFeHSEnd[wFormat];
	m_wBeVStart		= m_wFeVStart[wFormat];
	m_wBeVEnd		= m_wFeVEnd[wFormat];

	m_wBeVideoFormat	= wFormat;
	m_wBeNMCU			= (m_wBeHEnd - m_wBeHStart) / 16 - 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::SetFormat
void CZoran::SetFormat(WORD wFormat)
{

	if (wFormat == MICO_ENCODER_HIGH)
	{
//		WK_TRACE(TRUE, "CZoran::SetFormat High\n");
	}
	else if (wFormat == MICO_ENCODER_LOW)
	{
//		WK_TRACE(TRUE, "CZoran::SetFormat Low\n");
	}
	else
	{
		WK_TRACE(TRUE, "CZoran::SetFormat\tUnbekanntes Bildformat\n");
		return;
	}

	m_wFormat = wFormat;

	if (GetEncoderState() == MICO_ENCODER_ON)
	{
		StopEncoding();
		StartEncoding();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::SetFramerate
void CZoran::SetFramerate(WORD wFramerate)
{
	m_wFramerate = wFramerate;
	if (wFramerate == 0)
	{
		WK_TRACE(TRUE, "Framerate=0\n");
		return;
	}
	
	WK_TRACE(TRUE, "CZoran::SetFramerate\t(Framerate=%d)\n", wFramerate);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::SetBPF
void CZoran::SetBPF(DWORD dwBPF)
{
	m_dwBPF			= dwBPF;	// BYTE PER FRAME
	m_dwTCVData		= 8*dwBPF;	// BITS PER FRAME

//	WK_TRACE(TRUE, "CZoran::SetBPF\t(Bytes per Frame=%d)\n", dwBPF);

	if (GetEncoderState() == MICO_ENCODER_ON)
	{
		StopEncoding();
		StartEncoding();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::StartEncoding
BOOL CZoran::StartEncoding()
{
//	WK_TRACE(TRUE, "CZoran::StartEncoding\n");
	m_dwEncodedFrames = 0;

	ZR36055InitForEncoding();
	ZR36050InitForEncoding();
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::StopEncoding
BOOL CZoran::StopEncoding()
{
	BYTE byCom1		= 0x00;
	
//	WK_TRACE(TRUE, "CZoran::StopEncoding\n");

	// Commandregister 1 initialisieren (COM1 = 0x03)
	byCom1 = m_IO.Input(m_wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_ACTIVE_BIT);		// Inactive			
	byCom1 = CLRBIT(byCom1, COM1_CLK_EN_BIT);		// Disable ZR36050 clock
	byCom1 = CLRBIT(byCom1, COM1_INT_EN_BIT);		// Disable ZR36050 interrupts
	byCom1 = SETBIT(byCom1, COM1_CLK_EN_BIT);		// Disable ZR36050 clock
	byCom1 = SETBIT(byCom1, COM1_INT_EN_BIT);		// Disable ZR36050 interrupts
 	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1);

	m_nEncoderState = MICO_ENCODER_OFF;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::StartDecoding
BOOL CZoran::StartDecoding()
{
//	WK_TRACE(TRUE, "CZoran::StartDecoding\n");
	m_dwDecodedFrames = 0;

	ZR36055InitForDecoding();
	ZR36050InitForDecoding();

	m_nDecoderState = MICO_DECODER_ON;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::StopDecoding
BOOL CZoran::StopDecoding()
{
	BYTE byCom0		= 0x00;
	BYTE byCom1		= 0x00;

//	WK_TRACE(TRUE, "CZoran::StopDecoding\n");

	// Commandregister 1 initialisieren (COM1 = 0x03)
	byCom1 = m_IO.Input(m_wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_ACTIVE_BIT);		// Inactive			
	byCom1 = CLRBIT(byCom1, COM1_CLK_EN_BIT);		// Disable ZR36050 clock
	byCom1 = CLRBIT(byCom1, COM1_INT_EN_BIT);		// Disable ZR36050 interrupts
 	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1);

	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = CLRBIT(byCom0, COM0_EXP_BIT);			// Compression
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);

	m_nDecoderState = MICO_DECODER_OFF;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::GetEncoderState
int  CZoran::GetEncoderState()
{
 	return m_nEncoderState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::GetDecoderState
int	 CZoran::GetDecoderState()
{
 	return m_nDecoderState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::WriteData
DWORD CZoran::WriteData(LPCSTR pData, DWORD dwDataLen)
{
	BYTE  byCom0	= 0;
	BYTE  byFCNT	= 0;
	BYTE  byLen0	= 0;
	BYTE  byLen1	= 0;
	BYTE  byStatus0	= 0;
	DWORD  dwTimeOut= 0;

	m_dwDecodedFrames++;

	WORD  wLen 		= dwDataLen / 2;	// 16 Bit Zugriff
	WORD  wCBData 	= m_wIOBase | ZR_CB_DATA_OFFSET;
	WORD* pWord 	= (WORD*)pData;

	if (m_dwDecodedFrames <= 2)
	{
		for (DWORD dwI = 0; dwI < wLen; dwI++)
			m_IO.Outputw(wCBData, pWord[dwI]);	

		// Setze nächste Codebuffer page number
		byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
		if (TSTBIT(byCom0, STATUS0_PG0_BIT))
			byCom0 = CLRBIT(byCom0, STATUS0_PG0_BIT);
		else
			byCom0 = SETBIT(byCom0, STATUS0_PG0_BIT);
		m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
	}
	else
	{
		// Wait until Ready
		dwTimeOut = GetTickCount();
		while (!TSTBIT(m_IO.Input(m_wIOBase | ZR_STATUS0_OFFSET), STATUS0_RDY_BIT))
		{
			if (GetTickCount() > dwTimeOut + 1000)
			{
				WK_TRACE(TRUE, "CZoran::WriteData\tTimeOut\n");
				return 0L;
			}
		}
		
		// Read Status
		byStatus0	= m_IO.Input(m_wIOBase | ZR_STATUS0_OFFSET);
		byFCNT		= m_IO.Input(m_wIOBase | ZR_FCNT_OFFSET);
		byLen0		= m_IO.Input(m_wIOBase | ZR_LEN0_OFFSET);
		byLen1		= m_IO.Input(m_wIOBase | ZR_LEN1_OFFSET);

		// Daten in den Codebuffer schreiben
		for (WORD wI = 0; wI < wLen; wI++)
			m_IO.Outputw(wCBData, pWord[wI]);	

		// Setze nächste Codebuffer page number
		byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
		if (TSTBIT(byCom0, STATUS0_PG0_BIT))
			byCom0 = CLRBIT(byCom0, STATUS0_PG0_BIT);
		else
			byCom0 = SETBIT(byCom0, STATUS0_PG0_BIT);
		m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);

		// Read Status
		byStatus0	= m_IO.Input(m_wIOBase | ZR_STATUS0_OFFSET);
		byFCNT		= m_IO.Input(m_wIOBase | ZR_FCNT_OFFSET);
		byLen0		= m_IO.Input(m_wIOBase | ZR_LEN0_OFFSET);
		byLen1		= m_IO.Input(m_wIOBase | ZR_LEN1_OFFSET);

	}
	return dwDataLen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::EnableBrdIRQ
void CZoran::EnableBrdIRQ()
{
	BYTE byStatus = m_IO.Input(m_wIOBase | MICO_IER_OFFSET);
	byStatus = SETBIT(byStatus, VI_IE_BIT);		// Video-IRQ freigeben
	byStatus = SETBIT(byStatus, ZR_IE_BIT);
	byStatus = CLRBIT(byStatus, VID_AES_BIT);  	// VSync bei steigende Flanke
	m_IO.Output(m_wIOBase | MICO_IER_OFFSET, byStatus);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::DisableBrdIRQ
void CZoran::DisableBrdIRQ()
{
	BYTE byStatus = m_IO.Input(m_wIOBase | MICO_IER_OFFSET);
	byStatus = CLRBIT(byStatus, VI_IE_BIT);
	byStatus = CLRBIT(byStatus, ZR_IE_BIT);
	m_IO.Output(m_wIOBase | MICO_IER_OFFSET, byStatus);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CZoran::SetIRQNumber
void CZoran::SetIRQNumber(WORD wIRQ)
{
	BYTE byTbl[16] = {0,0,0,0,0,1,0,0,0,0,2,3,4,0,5,6}; // IRQ 5, 10, 11, 12, 14, 15
	BYTE byStatus;
		
	if (wIRQ > 15)
	{
		WK_TRACE(TRUE, "CZoran::SetIRQNumber\t Falsche IRQ (%u)\n", wIRQ);
		return;
	}

	byStatus = m_IO.Input(m_wIOBase + MICO_IER_OFFSET);
	byStatus &= 0xf8;
	m_IO.Output(m_wIOBase + MICO_IER_OFFSET, byStatus | byTbl[wIRQ]);
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ChipSelectVideoInAnalogOut(WORD wFrontEnd, WORD wExtCard, BYTE byData)
{
	if (wExtCard == MICO_EXTCARD0)
	{
		if (wFrontEnd == MICO_FE0)
		{
			m_IO.WriteToLatch(m_wIOBase | CSVINAN1_OFFSET, byData);
		}
		else if (wFrontEnd == MICO_FE1)
		{
			m_IO.WriteToLatch(m_wIOBase | CSVINAN2_OFFSET, byData);
		}
	}
	else
	{
		m_IO.WriteToLatch(m_wIOBase | CSVINAN_COVI_OFFSET, byData);
	}
}

/////////////////////////////////////////////////////////////////////////////
BYTE CZoran::ChipSelectVideoInAnalogIn(WORD wFrontEnd, WORD wExtCard)
{
	if (wExtCard == MICO_EXTCARD0)
	{
		if (wFrontEnd == MICO_FE0)
			return m_IO.ReadFromLatch(m_wIOBase | CSVINAN1_OFFSET);
		else if (wFrontEnd == MICO_FE1)
			return m_IO.ReadFromLatch(m_wIOBase | CSVINAN2_OFFSET);
		else
			return 0;
	}
	else
		return m_IO.ReadFromLatch(m_wIOBase | CSVINAN_COVI_OFFSET);
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ChipSelectVideoInDigitalOut(BYTE byData)
{
	m_IO.WriteToLatch(m_wIOBase | CSVINDN_OFFSET, byData);
}

/////////////////////////////////////////////////////////////////////////////
BYTE CZoran::ChipSelectVideoInDigitalIn()
{
	return m_IO.ReadFromLatch(m_wIOBase | CSVINDN_OFFSET);
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ZR36055InitForEncoding()
{
	BYTE byCom0	 = 0;
	BYTE byCom1	 = 0;
	BYTE byCom2	 = 0;

	// Commandregister 0 initialisieren (COM0 = 0xd0)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			// Codebufferpage 0
	byCom0 = CLRBIT(byCom0, COM0_PCG1_BIT);			// Codebufferpage 0
	byCom0 = CLRBIT(byCom0, COM0_STILL_BIT);		// Contininuous compression
	byCom0 = CLRBIT(byCom0, COM0_EXP_BIT);			// Compression
	byCom0 = SETBIT(byCom0, COM0_VID_422_BIT);		// Irrelevant for Compression
	byCom0 = CLRBIT(byCom0, COM0_GRESET_BIT);		// Reset ZR36055
	byCom0 = SETBIT(byCom0, COM0_FFI_BIT);			// Irrelevant for Compression
	byCom0 = SETBIT(byCom0, COM0_FAST_BIT);			// OOPS? FAST-Clock 
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);

	// Commandregister 1 initialisieren (COM1 = 0x13)
	byCom1 = m_IO.Input(m_wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_ACTIVE_BIT);		// Inactive			
	byCom1 = SETBIT(byCom1, COM1_CB_SIZE_BIT);		// 256KByte VRAM
	byCom1 = CLRBIT(byCom1, COM1_VER_DEC_BIT);		// Irrelevant for Compression

	if (m_wFormat == MICO_ENCODER_HIGH)
		byCom1 = CLRBIT(byCom1, COM1_HOR_DEC_BIT);	// No horizontal decimation		
	else if (m_wFormat == MICO_ENCODER_LOW)
		byCom1 = SETBIT(byCom1, COM1_HOR_DEC_BIT);	// Horizontal decimation		

	byCom1 = SETBIT(byCom1, COM1_SNC_SRC_BIT);		// Internal Syncronization
	byCom1 = CLRBIT(byCom1, COM1_CLK_EN_BIT);		// Disable ZR36050 clock
	byCom1 = CLRBIT(byCom1, COM1_INT_EN_BIT);		// Disable ZR36050 interrupts
	
	if ((m_wFeVideoFormat == MICO_PAL_SQUARE) || (m_wFeVideoFormat == MICO_PAL_CCIR))
		byCom1 = CLRBIT(byCom1, COM1_VID_FMT_BIT);	// PAL Videoformat	
  	else if ((m_wFeVideoFormat == MICO_NTSC_SQUARE) || (m_wFeVideoFormat == MICO_NTSC_CCIR))
		byCom1 = SETBIT(byCom1, COM1_VID_FMT_BIT);	// NTSC Videoformat;	
 	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1);

	// Set operationg mode (COM0 = 0xd3)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_PCG0_BIT);			// Codebufferpage 3
	byCom0 = SETBIT(byCom0, COM0_PCG1_BIT);			// Codebufferpage 3
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
	
	// Set operationg control (COM1 = 0x03)
	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1);	

	//  (COM2 = 0x80)
	byCom2 = m_IO.Input(m_wIOBase | ZR_COM2_OFFSET);
	byCom2 = CLRBIT(byCom2, COM2_ID0_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID1_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID2_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID3_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID4_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID5_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_VID_FILT_BIT);		// No Video filter
	byCom2 = SETBIT(byCom2, COM2_STRP_SIZE_BIT);	// 32 KByte SRAM
	m_IO.Output(m_wIOBase | ZR_COM2_OFFSET, byCom2);

	// Set HSTART, HEND, HTOTAL, VSTART, VEND, VTOTAL and NMCU
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x0000 | m_wFeHStart[m_wFeVideoFormat]);	// Set HSTART
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x1000 | m_wFeHEnd[m_wFeVideoFormat]-1);		// Set HEND
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x2000 | m_wFeHTotal[m_wFeVideoFormat]);	// Set HTOTAL
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x8000 | m_wFeVStart[m_wFeVideoFormat]);	// Set VSTART
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x9000 | m_wFeVEnd[m_wFeVideoFormat]-1);		// Set VEND
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0xa000 | m_wFeVTotal[m_wFeVideoFormat]);	// Set VTOTAL

	
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x3000 | m_wFeHSStart[m_wFeVideoFormat]);		// Set HSSTART
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x4000 | m_wFeHSEnd[m_wFeVideoFormat]);		// Set HSEND

	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0xc000 | m_wFeNMCU[m_wFeVideoFormat]);		// Set NMCU

	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x5000 | m_wFeBlankStart[m_wFeVideoFormat]);		// Set HSEND

	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0xe000 | m_wYBackgnd); // Set YBackgnd
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0xe800 | m_wUVBackgnd);// Set UVBackgnd

	// Enable ZR36050 clock (COM1 = 0x23)
	byCom1 = m_IO.Input(m_wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_CLK_EN_BIT);		
	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1);		

	Sleep(1);	// Wait minimal 0.5 ms

	// Deactivate GRESET (COM0 = 0xf3)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_GRESET_BIT);
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
}

void CZoran::ZR36050InitForEncoding()
{
	BYTE 	byCom1		= 0x00;
	WORD	wSamplesPerLine;
	WORD	wNumberOfLines;
	WORD	wMBCV;
	char 	strSpace[64];

	memset(strSpace, 0x20, 64);
	wSamplesPerLine = m_wFeHEnd[m_wFeVideoFormat] - m_wFeHStart[m_wFeVideoFormat]; 

	// Horizontale Dezimierung ein ?
	if (m_wFormat == MICO_ENCODER_LOW)
		wSamplesPerLine /= 2;

	wNumberOfLines	= m_wFeVEnd[m_wFeVideoFormat] - m_wFeVStart[m_wFeVideoFormat];
	wMBCV 			= (WORD)(8L * 2L * 256L * m_dwBPF / ((DWORD)wSamplesPerLine * (DWORD)wNumberOfLines));

	Z050_CODER_CONFIG Cfg;
	Cfg.Width  = wSamplesPerLine;
	Cfg.Height = wNumberOfLines;
	Cfg.HSampRatio[0] = 2;
	Cfg.VSampRatio[0] = 1;
	Cfg.HSampRatio[1] = 1;
	Cfg.VSampRatio[1] = 1;
	Cfg.HSampRatio[2] = 1;
	Cfg.VSampRatio[2] = 1;
	Cfg.TotalCodeVolume = m_dwBPF;
	Cfg.MaxBlockCodeVolume = wMBCV;
	Cfg.ScaleFactor = 1*0x100;
	Cfg.DRI = 1;
	Cfg.QTable = (char FAR *)&DefaultQTables[0];
	Cfg.HuffTable = (char FAR *)&DefaultHuffTables[0];
	Cfg.APPString = strSpace;
	Cfg.COMString = strSpace;
	if (m_pzCoder)
		m_pzCoder->CoderConfigureCompression((LP_Z050_CODER_CONFIG)&Cfg);
  	
	ReadFromZR36050(0);	// Hardware GO

	// Get Status
	m_IO.Input(m_wIOBase | ZR_FCNT_OFFSET);
	m_IO.Input(m_wIOBase | ZR_STATUS0_OFFSET);
	m_IO.Input(m_wIOBase | ZR_LEN0_OFFSET);
	m_IO.Input(m_wIOBase | ZR_LEN1_OFFSET);

	// Activate
	byCom1 = m_IO.Input(m_wIOBase | ZR_COM1_OFFSET);
	byCom1 = CLRBIT(byCom1, COM1_ACTIVE_BIT);	// Activate
	byCom1 = SETBIT(byCom1, COM1_INT_EN_BIT);	// Enable Interrupt
	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1); 
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ZR36055InitForDecoding()
{
	BYTE byCom0;
	BYTE byCom1;
	BYTE byCom2;

	// Commandregister 0 initialisieren (COM0 = 0xd0)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			// Codebufferpage 0
	byCom0 = CLRBIT(byCom0, COM0_PCG1_BIT);			// Codebufferpage 0
	byCom0 = CLRBIT(byCom0, COM0_STILL_BIT);		// Contininuous expansion
	byCom0 = CLRBIT(byCom0, COM0_EXP_BIT);			// Compression
	byCom0 = SETBIT(byCom0, COM0_VID_422_BIT);		// YUV422
	byCom0 = CLRBIT(byCom0, COM0_GRESET_BIT);		// Reset ZR36055
	byCom0 = SETBIT(byCom0, COM0_FFI_BIT);			// First expanded field is field I
	byCom0 = SETBIT(byCom0, COM0_FAST_BIT);			// OOPS? FAST-Clock 
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);

	// Commandregister 1 initialisieren (COM1 = 0x03)
	byCom1 = m_IO.Input(m_wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_ACTIVE_BIT);		// Inactive			
	byCom1 = SETBIT(byCom1, COM1_CB_SIZE_BIT);		// 256KByte VRAM

	// OOPS Ich weiß nicht genau ob die Dezimierung benötigt wird
	byCom1 = CLRBIT(byCom1, COM1_VER_DEC_BIT);		// No vert. decimation
	byCom1 = CLRBIT(byCom1, COM1_HOR_DEC_BIT);		// No hor. decimation or interpolation		
	byCom1 = CLRBIT(byCom1, COM1_SNC_SRC_BIT);		// OOPS? External Syncronization
	byCom1 = CLRBIT(byCom1, COM1_CLK_EN_BIT);		// Disable ZR36050 clock
	byCom1 = CLRBIT(byCom1, COM1_INT_EN_BIT);		// Disable ZR36050 interrupts
	if ((m_wBeVideoFormat == MICO_PAL_SQUARE) || (m_wBeVideoFormat == MICO_PAL_CCIR))
		byCom1 = CLRBIT(byCom1, COM1_VID_FMT_BIT);	// PAL Videoformat	
  	else if ((m_wBeVideoFormat == MICO_NTSC_SQUARE) || (m_wBeVideoFormat == MICO_NTSC_CCIR))
		byCom1 = SETBIT(byCom1, COM1_VID_FMT_BIT);	// NTSC Videoformat;	
 	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1);

	// Set operationg mode (COM0 = 0xd8)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_EXP_BIT);			// Expansion
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);

	// Set operationg control (COM1 = 0x17) (COM2 = 0x80)
	byCom1 = m_IO.Input(m_wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_VER_DEC_BIT);		// Vert. decimation or interpolation		
	byCom1 = SETBIT(byCom1, COM1_SNC_SRC_BIT);		// Internal Synchronisation
	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1);	
	
	byCom2 = m_IO.Input(m_wIOBase | ZR_COM2_OFFSET);
	byCom2 = SETBIT(byCom2, COM2_ID0_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID1_BIT);			// ZR36055 ID
	byCom2 = SETBIT(byCom2, COM2_ID2_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID3_BIT);			// ZR36055 ID
	byCom2 = SETBIT(byCom2, COM2_ID4_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID5_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_VID_FILT_BIT);		// No Video filter
	byCom2 = SETBIT(byCom2, COM2_STRP_SIZE_BIT);	// 32 KByte SRAM
	m_IO.Output(m_wIOBase | ZR_COM2_OFFSET, byCom2);

	// Set HSTART, HEND, HTOTAL, VSTART, VEND, VTOTAL and NMCU
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x0000 | m_wBeHStart);		// Set HSTART
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x1000 | m_wBeHEnd-1);			// Set HEND
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x2000 | m_wBeHTotal);		// Set HTOTAL
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x3000 | m_wBeHSStart);		// Set HSSTART
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x4000 | m_wBeHSEnd);		// Set HSEND
	
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x8000 | m_wBeVStart);		// Set VSTART
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x9000 | m_wBeVEnd-1);			// Set VEND
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0xa000 | m_wBeVTotal);		// Set VTOTAL
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0xc000 | m_wBeNMCU);			// Set NMCU
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0x5000 | m_wBeBlankStart);	// Set BLANKSTART

	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0xe000 | m_wYBackgnd);		// Set YBackgnd
	m_IO.Outputw(m_wIOBase | ZR_SIZE_OFFSET, 0xe000 | m_wUVBackgnd);	// Set UVBackgnd

	// Enable ZR36050 clock (COM1 = 0x23)
	byCom1 = m_IO.Input(m_wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_CLK_EN_BIT);		
	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1);		

	Sleep(1);	// Wait minimal 0.5 ms

	// Deactivate GRESET (COM0 = 0xf3)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_GRESET_BIT);
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CZoran::ZR36050InitForDecoding()
{
	BYTE	byCom1			= 0x00;
	Z050_CODER_CONFIG Cfg;
	Cfg.HuffTable = (char FAR *)&DefaultHuffTables[0];
	if (m_pzCoder)
		m_pzCoder->CoderConfigureExpansion((LP_Z050_CODER_CONFIG)&Cfg);

	ReadFromZR36050(0x0000);				// Go...(Siehe S. 22)

	// Get Status
	m_IO.Input(m_wIOBase | ZR_FCNT_OFFSET);
	m_IO.Input(m_wIOBase | ZR_LEN0_OFFSET);
	m_IO.Input(m_wIOBase | ZR_LEN1_OFFSET);

	// Activate
	byCom1 = m_IO.Input(m_wIOBase | ZR_COM1_OFFSET);
	byCom1 = CLRBIT(byCom1, COM1_ACTIVE_BIT);	// Activate
	m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, byCom1); 
}

//////////////////////////////////////////////////////////////////////////////////////
inline void CZoran::WriteToZR36050(WORD wAdr, BYTE byValue)
{
	_asm cli
	m_IO.Outputw(m_wIOBase | ZR_ADDRESS_OFFSET, wAdr);	// Write Adress
	m_IO.Output(m_wIOBase  | ZR_DATA_OFFSET,	byValue);	// Write Data
	_asm sti
}

//////////////////////////////////////////////////////////////////////////////////////
inline BYTE CZoran::ReadFromZR36050(WORD wAdr)
{
	_asm cli
	m_IO.Outputw(m_wIOBase | ZR_ADDRESS_OFFSET, wAdr);	// Write Adress
	BYTE byVal = m_IO.Input(m_wIOBase | ZR_DATA_OFFSET);		// Read Data
	_asm sti

	return byVal;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::TestingEncoder()
{
	WORD	wIRR;
	WORD	wIER;
	DWORD 	dwTC;
	DWORD	dwCalcTime = 5000L;
	BYTE	byStatus0;
	BYTE	byFCNT;
	BYTE	byLen0;
	BYTE	byLen1;
	BYTE	byCom0;
	BYTE	byBufferPage;
	DWORD	dwUnvalidData 	= 0;
	
	EnableBrdIRQ();
	StartEncoding();
    StopEncoding();
	StartEncoding();
    
	///////////////////////////////////////////////////
	// Test Stufe 1: Encoding ohne zu speichern 
	dwTC  	= GetTickCount();
	wIER 	= m_IO.Input(m_wIOBase | MICO_IER_OFFSET);
	do
	{
		wIRR = m_IO.Input(m_wIOBase | MICO_IRR_OFFSET);
		
		if (TSTBIT(wIRR, ZR_IR_BIT) && (TSTBIT(wIER, ZR_IE_BIT)))
		{
			byStatus0= m_IO.Input(m_wIOBase | ZR_STATUS0_OFFSET);
			byFCNT	 = m_IO.Input(m_wIOBase | ZR_FCNT_OFFSET);
			byLen0	 = m_IO.Input(m_wIOBase | ZR_LEN0_OFFSET);
			byLen1	 = m_IO.Input(m_wIOBase | ZR_LEN1_OFFSET);

			// Pagenummer aus dem Status0-Register selektieren
			byBufferPage = byStatus0 & 0x03;
		
			// Eine neue Pagenummer in COM0 selektieren.
			byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET) & 0xfc;
			byCom0 |= byBufferPage;
			m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
 
		 	// Invalid Data
			if (TSTBIT(byStatus0, STATUS0_FNV_BIT))
			{
				dwUnvalidData++;
            	continue;
            }

			m_dwEncodedFrames++; 
			WK_TRACE(FALSE, "\r\tFrame:%lu", m_dwEncodedFrames);		 		
		}	
		else
		{
			byStatus0= m_IO.Input(m_wIOBase | ZR_STATUS0_OFFSET);
			byFCNT	 = m_IO.Input(m_wIOBase | ZR_FCNT_OFFSET);
			byLen0	 = m_IO.Input(m_wIOBase | ZR_LEN0_OFFSET);
			byLen1	 = m_IO.Input(m_wIOBase | ZR_LEN1_OFFSET);
		}
	}
	while(GetTickCount() < dwTC + dwCalcTime);

    DWORD	dwF = ((dwCalcTime / 2L) + 1000L * m_dwEncodedFrames) / dwCalcTime;
    
	// Zoran-frequenz kleiner 30Hz?	                                       
	if ((dwF < 30) || (dwF > 60) || (dwUnvalidData > 5))
	{
		WK_TRACE(TRUE, "\n\tZoran Encoder\ttest failed (f=%luFps, unvalid Data=%lu)\n", dwF, dwUnvalidData);
		return FALSE;
    }
	else
		WK_TRACE(TRUE, "\n\tZoran Encoder\ttest passed (f=%luFps, unvalid Data=%lu)\n", dwF, dwUnvalidData);

    StopEncoding();
	DisableBrdIRQ();
    
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::Recording()
{
	WORD	wIRR;
	WORD	wIER;
	DWORD 	dwTC;
	DWORD	dwCalcTime = 5000L;
	BYTE	byStatus0;
	BYTE	byFCNT;
	BYTE	byLen0;
	BYTE	byLen1;
	BYTE	byCom0;
	WORD	wLen;
	WORD	wCBData;
	BYTE	byBufferPage;
	DWORD	dwUnvalidData 	= 0;
	FILE 	*stream			= NULL;
	char	sFileName[128];
	
	EnableBrdIRQ();
	StartEncoding();
    StopEncoding();
	StartEncoding();
    
	///////////////////////////////////////////////////
	// Test Stufe 2: Encoding ohne zu speichern 
	m_dwEncodedFrames 	= 0L;
	dwUnvalidData		= 0L;
	dwTC  	= GetTickCount();
	wIER 	= m_IO.Input(m_wIOBase | MICO_IER_OFFSET);
	do
	{
		wIRR = m_IO.Input(m_wIOBase | MICO_IRR_OFFSET);
		
		if (TSTBIT(wIRR, ZR_IR_BIT) && (TSTBIT(wIER, ZR_IE_BIT)))
		{
			byStatus0= m_IO.Input(m_wIOBase | ZR_STATUS0_OFFSET);
			byFCNT	 = m_IO.Input(m_wIOBase | ZR_FCNT_OFFSET);
			byLen0	 = m_IO.Input(m_wIOBase | ZR_LEN0_OFFSET);
			byLen1	 = m_IO.Input(m_wIOBase | ZR_LEN1_OFFSET);
			wLen	 = MAKEWORD(byLen0, byLen1);

			// Länge soll gerade sein
			wLen = ((wLen + 1) & 0xfffe);	
		
		    // This section prevents reads of lengthes ending with 511 or 512 chip bug.
			if (((wLen & 0xff) == 0xfe) || ((wLen & 0xff) == 0xfd))
				wLen += 8;

			// Pagenummer aus dem Status0-Register selektieren
			byBufferPage = byStatus0 & 0x03;
		
			// Eine neue Pagenummer in COM0 selektieren.
			byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET) & 0xfc;
			byCom0 |= byBufferPage;
			m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
        
            // Ist die Filegröße ok?
        	if (wLen <= (WORD)((float)m_dwBPF * 0.8))
        		continue;
        
		 	// Invalid Data
			if (TSTBIT(byStatus0, STATUS0_FNV_BIT))
			{
				dwUnvalidData++;
            	continue;
            }
           	wCBData = m_wIOBase | ZR_CB_DATA_OFFSET;

			// Es wird nur ein Halbbild verwertet
			if (TSTBIT(wIRR, FIELD_BIT))
			{
				// Daten auslesen...
				for (WORD wI = 0; wI < wLen / 2; wI++)
					m_pBuffer[wI] = m_IO.Inputw(wCBData);
				
				//...und wegspeichern
				sprintf(sFileName,"tmp\\%lu.jpg\0", m_dwEncodedFrames++); 
				stream = fopen(sFileName, "wb");
				if (stream)
				{
					fwrite(m_pBuffer, 1, wLen, stream);
		   			fclose( stream );
					WK_TRACE(FALSE, "\r\tFrame:%lu", m_dwEncodedFrames);		 		
				}
			}
		}	
		else
		{
			byStatus0= m_IO.Input(m_wIOBase | ZR_STATUS0_OFFSET);
			byFCNT	 = m_IO.Input(m_wIOBase | ZR_FCNT_OFFSET);
			byLen0	 = m_IO.Input(m_wIOBase | ZR_LEN0_OFFSET);
			byLen1	 = m_IO.Input(m_wIOBase | ZR_LEN1_OFFSET);
		}
	}
	while(GetTickCount() < dwTC + dwCalcTime);
    WK_TRACE(FALSE, "\n");

    StopEncoding();
	DisableBrdIRQ();
    
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::Playing()
{
	char	sFileName[128];
	FILE* 	stream		= NULL;
	DWORD  	dwLen		= 0;
	DWORD	dwCnt		= 0;	
	BOOL	bRet		= TRUE;
			
	StartDecoding();
	
	//...und wegspeichern
	sprintf(sFileName,"tmp\\%lu.jpg\0", dwCnt); 
	for (dwCnt = 0; dwCnt <= m_dwEncodedFrames; dwCnt++)
	{
		if (!(stream = fopen(sFileName, "rb")))
			break;
		
		dwLen = fread(m_pBuffer, 1, 65535, stream);
		
		if (WriteData((LPCSTR)m_pBuffer, dwLen) == 0L)
		{
			bRet = FALSE;
			fclose(stream);
			break;		
		}
		fclose(stream);
		WK_TRACE(FALSE, "\r\tFrame:%lu", dwCnt);		 		
		sprintf(sFileName,"tmp\\%lu.jpg\0", dwCnt); 
		WaitUntilKeyStroke();
	}
    WK_TRACE(FALSE, "\n");
	
	WaitUntilKeyStroke();

	StopDecoding();
    
    if (!bRet)
		WK_TRACE(TRUE, "\tZoran Decoder\ttest failed\n");
	else
		WK_TRACE(TRUE, "\tZoran Decoder\ttest passed\n");
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::PollingVideoIRQ()
{
	WORD	wIRR;
	WORD	wIER;
	DWORD 	dwTC  = GetTickCount();
	DWORD	dwCalcTime = 2000L;
	
	EnableBrdIRQ();


	g_dwIRQCounter = 0;

	do
	{
		wIRR = m_IO.Input(m_wIOBase | MICO_IRR_OFFSET);
		wIER = m_IO.Input(m_wIOBase | MICO_IER_OFFSET);
		
		if ((TSTBIT(wIRR, VI_IR_BIT)) && (TSTBIT(wIER, VI_IE_BIT)))
		{
			g_dwIRQCounter++;		 		
		}	
	}
	while(GetTickCount() < dwTC + dwCalcTime);
	
	DisableBrdIRQ();
	                                       
    DWORD	dwF = ((dwCalcTime / 2L) + 1000L * g_dwIRQCounter) / dwCalcTime;

	// VYsnc-frequenz in gültigen Grenzen	                                       
	if ((dwF  < 40) || (dwF > 60))
	{
		WK_TRACE(TRUE, "\tVideo Interrupt\tfailed (f=%luHz)\n", dwF);
		return FALSE;
    }
	else
		WK_TRACE(TRUE, "\tVideo Interrupt\tpassed (f=%luHz)\n", dwF);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::TestingIRQ(const WORD *wIRQArray, WORD wIRQCount)
{
	DWORD	dwCalcTime = 2000L;
	BOOL	bRet		= TRUE;
	WORD	wI;	

	for (wI = 0; wI < wIRQCount; wI++)
	{                 
		WORD wIRQ = wIRQArray[wI];
		g_dwIRQCounter = 0L;
				
		WK_TRACE(FALSE, "\nTesting Video Interrupt %u...\n", wIRQ);
		
		_asm cli
		DisableBrdIRQ();		// Interrupts auf der MiCo sperren
		SetIRQNumber(wIRQ);		// Interruptnummer auf der MiCo setzen
		InstallISR(wIRQ); 		// ISR einbinden
		EnableBrdIRQ();			// Interrupts auf der MiCo freigeben
	    _asm sti
	    
		Sleep(dwCalcTime);
	
	    DWORD	dwF = ((dwCalcTime / 2L) + 1000L * g_dwIRQCounter) / dwCalcTime;
	
		// VYsnc-frequenz in gültigen Grenzen	                                       
		if ((dwF  < 40) || (dwF > 60))
		{
			WK_TRACE(TRUE, "\tVideo Interrupt %u\ttest failed (f=%luHz)\n", wIRQ, dwF);
			bRet = FALSE;
	    }
		else
			WK_TRACE(TRUE, "\tVideo Interrupt %u\ttest passed (f=%luHz)\n", wIRQ, dwF);

		_asm cli
		DisableBrdIRQ();		// Interrupts auf der MiCo sperren
		RemoveISR(wIRQ);    	// ISR entfernen.
	    _asm sti
	}
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::TestingZR36055()
{
	WORD  wI;
		
	// Überprüfe ID-Code
	WORD wID = m_IO.Input(m_wIOBase | ZR_COM2_OFFSET) & 0x3f;
	
	if (wID != 0x15)
		WK_TRACE(TRUE, "\tZR36055 \twrong ID-Code (0x%x)\n", wID);
	else
		WK_TRACE(TRUE, "\tZR36055 \tID-Code test passed\n");
			 
	
	// Überprüfe Schreib/Lese-Zugriff auf COM0
	for (wI = 0; wI <= 0xff; wI++)
	{
		m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, wI);
		if (m_IO.Input(m_wIOBase | ZR_COM0_OFFSET) != wI)
		{
			WK_TRACE(TRUE, "\tZR36055 \tReadback test failed\n");
			return FALSE;
		}
	}
	
	// Überprüfe Schreib/Lese-Zugriff auf COM1
	for (wI = 0; wI <= 0xff; wI++)
	{
		m_IO.Output(m_wIOBase | ZR_COM1_OFFSET, wI);
		if (m_IO.Input(m_wIOBase | ZR_COM1_OFFSET) != wI)
		{
			WK_TRACE(TRUE, "\tZR36055 \tReadback test failed\n");
			return FALSE;
		}
	}
	
	// Überprüfe Schreib/Lese-Zugriff auf COM2
	for (wI = 0; wI <= 0xff; wI++)
	{
		m_IO.Output(m_wIOBase | ZR_COM2_OFFSET, wI);
		if ((m_IO.Input(m_wIOBase | ZR_COM2_OFFSET) & 0xc0) != (wI & 0xc0))
		{
			WK_TRACE(TRUE, "\tZR36055 \tReadback test failed\n");
			return FALSE;
		}
	}   
	
	WK_TRACE(TRUE, "\tZR36055 \tReadback test passed\n");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::TestingInternalMemory()
{
	WORD wI;
	WORD wVal;
		
	// Überprüfe Schreib/Lesezugriff auf internen Speicher (Bitmuster: 0101010b)
	for (wI = 2; wI <= 0x3ff; wI++)
	{
		if ((wI != 0x2e) && (wI != 0x2f) && (wI != 0x30))
		{
			WriteToZR36050(wI, 0x55);
		}
	}
			
	Sleep(1000);
	
	for (wI = 2; wI <= 0x3ff; wI++)
	{
		if ((wI != 0x2e) && (wI != 0x2f) && (wI != 0x30))
		{
            wVal = ReadFromZR36050(wI);
			if (wVal != 0x55)
			{
				WK_TRACE(TRUE, "\tInternal memory\tReadback test 1 failed (Offset=0x%x, Value=0x%x)\n", wI, wVal); 		
				return FALSE;
			}	
		}
	}
	WK_TRACE(TRUE, "\tInternal memory\tReadback test 1 passed\n"); 		

	// Überprüfe Schreib/Lesezugriff auf internen Speicher (Bitmuster: 10101010b)
	for (wI = 2; wI <= 0x3ff; wI++)
	{
		if ((wI != 0x2e) && (wI != 0x2f) && (wI != 0x30))
		{
			WriteToZR36050(wI, 0xaa);
		}
	}
	
	Sleep(1000);
			
	for (wI = 2; wI <= 0x3ff; wI++)
	{
		if ((wI != 0x2e) && (wI != 0x2f) && (wI != 0x30))
		{
            wVal = ReadFromZR36050(wI);
			if (wVal != 0xaa)
			{
				WK_TRACE(TRUE, "\tInternal memory\tReadback test 2 failed (Offset=0x%x, Value=0x%x)\n", wI, wVal);	
				return FALSE;
			}	
		}
	}
	WK_TRACE(TRUE, "\tInternal memory\tReadback test 2 passed\n"); 		

	// Überprüfe Schreib/Lesezugriff auf internen Speicher (Bitmuster: wI)
	for (wI = 2; wI <= 0x3ff; wI++)
	{
		if ((wI != 0x2e) && (wI != 0x2f) && (wI != 0x30))
		{
			WriteToZR36050(wI, wI);
		}
	}
	
	Sleep(1000);

	for (wI = 2; wI <= 0x3ff; wI++)
	{
		if ((wI != 0x2e) && (wI != 0x2f) && (wI != 0x30))
		{
            wVal = ReadFromZR36050(wI);
			if (wVal != (wI & 0xff))
			{
				WK_TRACE(TRUE, "\tInternal memory\tReadback test 3 failed (Offset=0x%x, Value=0x%x)\n", wI, wVal);		
				return FALSE;
			}	
		}
	}
	WK_TRACE(TRUE, "\tInternal memory\tReadback test 3 passed\n"); 		

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::TestingCodebuffer()
{
	WORD	wRnd	= 0;
	WORD	wVal	= 0;
	BYTE	byCom0	= 0;
	DWORD	dwI		= 0;
		
	StartDecoding();
	
	// Schreibe Daten in Page 0 des Codebuffers (128KByte)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_EXP_BIT);        	// Expansion
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			// Codebufferpage 1
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
	for (dwI = 0; dwI < 128L * 1024L; dwI++)
	{
		m_IO.Outputw(m_wIOBase | ZR_CB_DATA_OFFSET, 0x55); 
	}
	
	Sleep(1000);
	
	// Lese Daten aus Page 0 des Codebuffers (64KByte)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = CLRBIT(byCom0, COM0_EXP_BIT);        	 // Compression
	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
	for (dwI = 0; dwI < 32L * 1024L; dwI++)
	{
		wVal = m_IO.Inputw(m_wIOBase | ZR_CB_DATA_OFFSET);
		if (wVal != 0x55)
		{
			WK_TRACE(TRUE, "\tCodebuffer\tMemorytest 1 failed at adress 0x%lx 0x%x\n", dwI, wVal);
			return FALSE;
		}				 
	}
	WK_TRACE(TRUE, "\tCodebuffer\tMemorytest 1 passed\n");

	// Schreibe Daten in Page 0 des Codebuffers (128KByte)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_EXP_BIT);        	// Expansion
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			// Codebufferpage 1
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
	for (dwI = 0; dwI < 128L * 1024L; dwI++)
	{
		m_IO.Outputw(m_wIOBase | ZR_CB_DATA_OFFSET, 0xaa); 
	}

	Sleep(1000);
		
	// Lese Daten aus Page 0 des Codebuffers (64KByte)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = CLRBIT(byCom0, COM0_EXP_BIT);        	 // Compression
	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
	for (dwI = 0; dwI < 32L * 1024L; dwI++)
	{
		wVal = m_IO.Inputw(m_wIOBase | ZR_CB_DATA_OFFSET);
		if (wVal != 0xaa)
		{
			WK_TRACE(TRUE, "\tCodebuffer\tMemorytest 2 failed at adress 0x%lx 0x%x\n", dwI, wVal);
			return FALSE;
		}				 
	}
	WK_TRACE(TRUE, "\tCodebuffer\tMemorytest 2 passed\n");

	// Schreibe Daten in Page 0 des Codebuffers (128KByte)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_EXP_BIT);        	// Expansion
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			// Codebufferpage 1
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			
  	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
	for (dwI = 0; dwI < 128L * 1024L; dwI++)
	{
		m_IO.Outputw(m_wIOBase | ZR_CB_DATA_OFFSET, dwI); 
	}
	
	Sleep(1000);
	
	// Lese Daten aus Page 0 des Codebuffers (64KByte)
	byCom0 = m_IO.Input(m_wIOBase | ZR_COM0_OFFSET);
	byCom0 = CLRBIT(byCom0, COM0_EXP_BIT);        	 // Compression
	m_IO.Output(m_wIOBase | ZR_COM0_OFFSET, byCom0);
	for (dwI = 0; dwI < 32L * 1024L; dwI++)
	{
		wVal = m_IO.Inputw(m_wIOBase | ZR_CB_DATA_OFFSET);
		if (wVal != dwI)
		{
			WK_TRACE(TRUE, "\tCodebuffer\tMemorytest failed at adress 0x%lx 0x%x\n", dwI, wVal);
			return FALSE;
		}				 
	}

	WK_TRACE(TRUE, "\tCodebuffer\tMemorytest 3 passed\n");

	return TRUE;
} 

//////////////////////////////////////////////////////////////////////////////////////
void CZoran::InstallISR(WORD wIrq)
{
    g_wIRQ = wIrq;
    
	// PIC-cntrl Register
	g_wPicCntrl = (wIrq <= 8) ? (0x20) : (0xA0);

	// PIC-mask Register	
	g_wPicMask  = (wIrq <= 8) ? (0x21) : (0xA1);

	_disable();

	// Bisherigen Iterruptvektor sichern
    g_pOldInt = _dos_getvect(g_nIntLookup[wIrq]);

	// Neuen Interruptvektor setzen
    g_pNewInt = IRQHandler;
	_dos_setvect(g_nIntLookup[wIrq], g_pNewInt );
    
    // Interrupt freigeben.
//	_outp(g_wPicMask,_inp(g_wPicMask) && (!(1 << (wIrq&7))) );
	_outp(g_wPicMask,_inp(g_wPicMask) & (~(1 << (wIrq&7))) );
  	
  	_enable();
}

//////////////////////////////////////////////////////////////////////////////////////
void CZoran::RemoveISR(WORD wIrq)
{
	_disable();

	// Interrupt sperren
//  	_outp(g_wPicMask,_inp(g_wPicMask) | (1 >> (wIrq&7)) );
  	_outp(g_wPicMask,_inp(g_wPicMask) | (1 << (wIrq&7)) );
  
	// Alten Interruptvektor wieder setzen.
	_dos_setvect(g_nIntLookup[wIrq], g_pOldInt );
    g_pOldInt = NULL;
    g_pNewInt = NULL;
    
	_enable();
}

//////////////////////////////////////////////////////////////////////////////////////
void interrupt far IRQHandler(void)
{
	_asm cli
	
	WORD wIRR = _inp(g_wIOBase | MICO_IRR_OFFSET);
	WORD wIER = _inp(g_wIOBase | MICO_IER_OFFSET);
	
	if ((TSTBIT(wIRR, VI_IR_BIT)) && (TSTBIT(wIER, VI_IE_BIT)))
	{
		g_dwIRQCounter++;
	}	

	// Interrupt quittieren	(EOI)
	_outp(0xA0, 0x20);
	_outp(0x20, 0x20);

	_asm sti
}

