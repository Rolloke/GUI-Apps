/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Cbt878.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/Cbt878.cpp $
// CHECKIN:		$Date: 20.06.03 12:00 $
// VERSION:		$Revision: 56 $
// LAST CHANGE:	$Modtime: 20.06.03 11:45 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "CBT878.h"
#include "CCodec.h"
#include "SavicProfile.h"
#include "CVideoInlay.h"
#include "CMotionDetection.h"
#include "CIoMemory.h"
#include "WK_Timer.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
CBT878::CBT878(CCodec* const pCodec, DWORD dwIOBase, const CString &sAppIniName)
{
	m_csBT878.Lock();

	// Initialisierung der Member
	m_wVideoFormat		= VFMT_INVALID;
	m_pCodec			= pCodec;
	m_sAppIniName		= sAppIniName;
	m_pVideoInlay		= NULL;
	m_pMotionDetection	= NULL;
	m_wFeVideoFormat	= VFMT_PAL_CCIR;

	m_pCIo = new CIoMemory(dwIOBase);

	// Registerbuffer auf 0 initialisieren
	for (int nReg = 0; nReg < BT878_MAX_REG; nReg++)
	{
	 	for (int nMode = 0; nMode < 2; nMode++){
	 		m_dwBT878Reg[nReg][nMode] = 0;
		}
	}

	m_csBT878.Unlock();
}

//////////////////////////////////////////////////////////////////////
CBT878::~CBT878()
{

	ClrGPIOPin(BT878_GPIO_POWERSWITCH_BIT);	// Kontrolle des Powerbuttons wieder an PC
	ClrGPIOPin(BT878_GPIO_RESETSWITCH_BIT); // Kontrolle des Resetbuttons wieder an PC

	ClrGPIOPin(BT878_GPIO_WATCHDOG_ACTIVE_BIT); // Watchdog sperren
	
	WK_DELETE(m_pVideoInlay);
	WK_DELETE(m_pMotionDetection);
	WK_DELETE(m_pCIo);
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::IsValid() const
{
	return (m_pCIo!=NULL);
}

//////////////////////////////////////////////////////////////////////////////////////
void CBT878::CBT878Reset()
{
	m_csBT878.Lock();
	
	BT878Out(BT878_SRESET, 0x00);

	m_csBT878.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::CheckIt()
{
	m_csBT878.Lock();

	BYTE byTmp  = 0;
	BOOL bOk	= TRUE;

	// 1. Schreib/Lese-test der BT878 Register
	BT878Out(BT878_BRIGHT, 0x55);
	BT878In(BT878_BRIGHT, byTmp);
	if (byTmp != 0x55)
	{
		OnError(ERR_BT878_READBACK_FAILED, (WORD)byTmp, 0);
		bOk  = FALSE;
	}

	// 2. Schreib/Lese-test der BT878 Register
	BT878Out(BT878_BRIGHT, 0x88);
	BT878In(BT878_BRIGHT, byTmp);
	if (byTmp != 0x88)
	{
		OnError(ERR_BT878_READBACK_FAILED, (WORD)byTmp, 0);
		bOk  = FALSE;
	}

	m_csBT878.Unlock();
	return bOk;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::Diagnostic()
{
	m_csBT878.Lock();

	BYTE byID		= 0; 
	BYTE byTmp		= 0;
	BOOL bVendorID	= TRUE;
	BOOL bReadBack	= TRUE;

	CBT878Reset();

	Sleep(1000);

	ML_TRACE_DIAGNOSTIC(_T("Readbacktest of BT878 ...\n"));

	WORD wTestPattern[] = {0x00, 0xff, 0x55, 0xaa};

	// Schreib/Lese-test der BT878 Register
	for (WORD wReg = 0; wReg <= 0x1a; wReg++)
	{
		if (wReg != BT878_DSTATUS)
		{
			for (int nI = 0; nI < 4; nI++)
			{
				BT878Out(wReg, wTestPattern[nI]);
				BT878In(wReg, byTmp);
				if (byTmp != wTestPattern[nI])
				{
					bReadBack = FALSE;
					break;
				}
			}
		}
		ML_TRACE_DIAGNOSTIC(_T("@+."));
	}

	if (bReadBack)
		ML_TRACE_DIAGNOSTIC(_T("@-BT878 Readbacktest: passed\n"));
	else
		ML_TRACE_DIAGNOSTIC(_T("@-BT878 Readbacktest: failed\n"));

	m_csBT878.Unlock();

	return (bVendorID && bReadBack);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::Init(const CString &sFileName, const CString &sSection)
{
	BYTE byLow	= 0;
	BYTE byHigh	= 0;

	m_csBT878.Lock();
	CString sDefaultReg[BT878_MAX_REG+1] = {_T("")};

	// NTSC-CCIR(720x480)  PAL-CCIR(720x576)
	sDefaultReg[BT878_DSTATUS]		=_T("0x00 0x00");	// DSTATUS
	sDefaultReg[BT878_IFORM]		=_T("0x58 0x58");	// IFORM
	sDefaultReg[BT878_TDEC]			=_T("0x00 0x00");	// TDEC
	sDefaultReg[BT878_E_CROP]		=_T("0x12 0x23");	// CROP
	sDefaultReg[BT878_O_CROP]		=_T("0x12 0x23");	// CROP
	sDefaultReg[BT878_E_VDELAY_LO]	=_T("0x1F 0x1F");	// VDELAY_LO
	sDefaultReg[BT878_O_VDELAY_LO]	=_T("0x1F 0x1F");	// VDELAY_LO
	sDefaultReg[BT878_E_VACTIVE_LO]	=_T("0xE0 0x40");	// VACTIVE_LO
	sDefaultReg[BT878_O_VACTIVE_LO]	=_T("0xE0 0x40");	// VACTIVE_LO
	sDefaultReg[BT878_E_HDELAY_LO]	=_T("0x82 0xA0");	// HDELAY_LO
	sDefaultReg[BT878_O_HDELAY_LO]	=_T("0x82 0xA0");	// HDELAY_LO
	sDefaultReg[BT878_E_HACTIVE_LO]	=_T("0xD0 0x00");	// HACTIVE_LO
	sDefaultReg[BT878_O_HACTIVE_LO]	=_T("0xD0 0x00");	// HACTIVE_LO
	sDefaultReg[BT878_E_HSCALE_HI]	=_T("0x00 0x03");	// HSCALE_HI
	sDefaultReg[BT878_O_HSCALE_HI]	=_T("0x00 0x03");	// HSCALE_HI
	sDefaultReg[BT878_E_HSCALE_LO]	=_T("0x92 0x3C");	// HSCALE_LO
	sDefaultReg[BT878_O_HSCALE_LO]	=_T("0x92 0x3C");	// HSCALE_LO
	sDefaultReg[BT878_BRIGHT]		=_T("0x00 0x00");	// BRIGHT
	sDefaultReg[BT878_E_CONTROL]	=_T("0x23 0x23");	// CONTROL
	sDefaultReg[BT878_O_CONTROL]	=_T("0x23 0x23");	// CONTROL
	sDefaultReg[BT878_CONTRAST_LO]	=_T("0xD8 0xD8");	// CONTRAST_LO
	sDefaultReg[BT878_SAT_U_LO]		=_T("0xFE 0xFE");	// SAT_U_LO
	sDefaultReg[BT878_SAT_V_LO]		=_T("0xB4 0xB4");	// SAT_V_LO
	sDefaultReg[BT878_HUE]			=_T("0x00 0x00");	// HUE

	sDefaultReg[BT878_E_SCLOOP]		=_T("0x40 0x40");	// SCLOOP
	sDefaultReg[BT878_O_SCLOOP]		=_T("0x40 0x40");	// SCLOOP
	sDefaultReg[BT878_E_SCLOOP]		=_T("0x00 0x00");	// SCLOOP (CAGC disabled)
	sDefaultReg[BT878_O_SCLOOP]		=_T("0x00 0x00");	// SCLOOP (CAGC disabled)

	sDefaultReg[BT878_WC_UP]		=_T("0xCF 0xCF");	// WC_UP
	sDefaultReg[BT878_OFORM]		=_T("0x80 0x80");	// OFORM

//	sDefaultReg[BT878_E_VSCALE_HI]	=_T("0x00 0x00");	// VSCALE_HI
//	sDefaultReg[BT878_O_VSCALE_HI]	=_T("0x00 0x00");	// VSCALE_HI
	sDefaultReg[BT878_E_VSCALE_HI]	=_T("0x40 0x40");	// VSCALE_HI
	sDefaultReg[BT878_O_VSCALE_HI]	=_T("0x40 0x40");	// VSCALE_HI
	
	sDefaultReg[BT878_E_VSCALE_LO]	=_T("0x00 0x00");	// VSCALE_LO
	sDefaultReg[BT878_O_VSCALE_LO]	=_T("0x00 0x00");	// VSCALE_LO
	sDefaultReg[BT878_ADELAY]		=_T("0x70 0x7F");	// ADELAY
	sDefaultReg[BT878_BDELAY]		=_T("0x5D 0x72");	// BDELAY

	sDefaultReg[BT878_ADC]			=_T("0x80 0x80");	// ADC	(Haupauge setzt es auf 0x82????)
//	sDefaultReg[BT878_ADC]			=_T("0x80 0x82");	// ADC	(Haupauge setzt es auf 0x82????)

	sDefaultReg[BT878_E_VTC]		=_T("0x60 0x00");	// VTC
	sDefaultReg[BT878_O_VTC]		=_T("0x60 0x00");	// VTC
	sDefaultReg[BT878_WC_DN]		=_T("0xBF 0x7F");	// WC_DN
	sDefaultReg[BT878_TGLB]			=_T("0xF7 0xF7");   // (Default bei Haupauge)
	sDefaultReg[BT878_VTOTAL_LO]	=_T("0x00 0x00");	// VTOTAL_LO LOBYTE(525-1 625-1)
	sDefaultReg[BT878_VTOTAL_HI]	=_T("0x00 0x00");	// VTOTAL_HI HIBYTE(525-1 625-1)
	sDefaultReg[BT878_COLOR_FMT]	=_T("0x00 0x00");	// COLOR_FMT (Even=RGB32 Odd=RGB32)
	sDefaultReg[BT878_COLOR_CTL]	=_T("0x10 0x10");	// COLOR_CTL
	sDefaultReg[BT878_CAP_CTL]		=_T("0x03 0x03");	// CAP_CTL
	sDefaultReg[BT878_VBI_PACK_SIZE]=_T("0x90 0x90");	// VBI_PACK_SIZE (Default bei Haupauge)
	sDefaultReg[BT878_VBI_PACK_DEL]	=_T("0x39 0x39");	// VBI_PACK_DEL	 (Default bei Haupauge)
	sDefaultReg[BT878_FCAP]			=_T("0x00 0x00");	// FCAP
	sDefaultReg[BT878_PLL_F_LO]		=_T("0x00 0x00");	// PLL_F_LO
	sDefaultReg[BT878_PLL_F_HI]		=_T("0x00 0x00");	// PLL_F_HI
	sDefaultReg[BT878_PLL_XCI]		=_T("0x00 0x00");	// PLL_XCI
	sDefaultReg[BT878_DVSIF]		=_T("0x00 0x00");	// DVSIF
	sDefaultReg[BT878_GPIO_DMA_CTL]	=_T("0x00 0x00");	// GPIO_DMA_CTL
	sDefaultReg[BT878_I2C_DATA_CTL]	=_T("0x03 0x03");	// I2C_DATA_CTL
	sDefaultReg[BT878_GPIO_OUT_EN]	=_T("0x00 0x00");	// GPIO_OUT_EN
	sDefaultReg[BT878_RISC_COUNT]	=_T("0x00 0x00");	// RISC_COUNT
	sDefaultReg[BT878_INT_MASK]		=_T("0x00 0x00");	// BT878_INT_MASK
	sDefaultReg[BT878_GPIO_DATA_CTL]=_T("0x00 0x00");	// GPIO_DATA_CTL

	_TCHAR   szFormat[BUFF_SIZE];
	_TCHAR	*pszNextWord;
	WORD		wReg	= 0;
	WORD		wMinIndex, wMaxIndex;
	WORD		wMode	= 0;

	CBT878Reset();

	// Überprüft ob der Zugriff auf den Videodecoder (BT878) funktioniert
	if (!CheckIt())
	{
		m_csBT878.Unlock();
		return FALSE;
	}
	else
		ML_TRACE(_T("BT878 \tNo error detected\n"));

// Die Initialisierungswerte werden nicht mehr aus der Registry genommen.
#if (1)
	// Initialisierung aller BT878-Register...
 	wMinIndex = BT878_MIN_REG;
 	wMaxIndex = BT878_MAX_REG;
								 
	for (wReg = wMinIndex; wReg <= wMaxIndex; wReg+=4)
	{  
		wsprintf(szFormat,_T("Reg%03X"), wReg);

		if (!sDefaultReg[wReg].IsEmpty())
		{
			pszNextWord = sDefaultReg[wReg].GetBuffer(sDefaultReg[wReg].GetLength());
			// NTSC (720x480), PAL (720x576)
			for (wMode = VFMT_NTSC_CCIR; wMode <= VFMT_PAL_CCIR; wMode++)
			{					  
				m_dwBT878Reg[wReg][wMode] = (BYTE)_tcstoul(pszNextWord, &pszNextWord, 0);
				if (wMode == VFMT_PAL_CCIR)
					BT878Out(wReg, m_dwBT878Reg[wReg][wMode]);
			}
			sDefaultReg[wReg].ReleaseBuffer();
		}
	}
#else
	char		szString[BUFF_SIZE];

	// Initialisierung aller BT878-Register...
 	wMinIndex = (BYTE)ReadConfigurationInt(sSection, _T("MinIndex"), BT878_MIN_REG, sFileName);
 	wMaxIndex = (BYTE)ReadConfigurationInt(sSection, _T("MaxIndex"), BT878_MAX_REG, sFileName);
		
	for (wReg = wMinIndex; wReg <= wMaxIndex; wReg+=4)
	{  
		wsprintf(szFormat,_T("Reg%02X"), (WORD)wReg);

		if (ReadConfigurationString(sSection, szFormat, sDefaultReg[wReg], szString, BUFF_SIZE, sFileName))
		{
			pszNextWord = &szString[0];

			// NTSC (720x480), PAL (720x576)
			for (wMode = VFMT_NTSC_CCIR; wMode <= VFMT_PAL_CCIR; wMode++)
			{					  
				m_dwBT878Reg[wReg][wMode] = (BYTE)strtoul(pszNextWord, &pszNextWord, 0);
				if (wMode == VFMT_PAL_CCIR){
					BT878Out(wReg, m_dwBT878Reg[wReg][wMode]);
				}
			}					
		}
	}
#endif

	DWORD dwVal = 0;
	if (BT878In(BT878_GPIO_DMA_CTL, dwVal))
	{
		dwVal = CLRBIT(dwVal, BT878_GPIO_DMA_CTL_GPIOMODE0_BIT);	// GPIO-Mode normal
		dwVal = CLRBIT(dwVal, BT878_GPIO_DMA_CTL_GPIOMODE1_BIT);	//	"
		BT878Out(BT878_GPIO_DMA_CTL, dwVal);
	}

	//GPIO-Input/Output Configuration
	dwVal = 0;
	dwVal = CLRBIT(dwVal, BT878_GPIO_ALARM_IN0_BIT);
	dwVal = CLRBIT(dwVal, BT878_GPIO_ALARM_IN1_BIT);
	dwVal = CLRBIT(dwVal, BT878_GPIO_ALARM_IN2_BIT);
	dwVal = CLRBIT(dwVal, BT878_GPIO_ALARM_IN3_BIT);
	dwVal = CLRBIT(dwVal, BT878_GPIO_ALARM_IN4_BIT);
	
	dwVal = SETBIT(dwVal, BT878_GPIO_RELAY_BIT);
	
	dwVal = SETBIT(dwVal, BT878_GPIO_POWERSWITCH_BIT);
	dwVal = CLRBIT(dwVal, BT878_GPIO_POWEROFF_BIT);
	
	dwVal = CLRBIT(dwVal, BT878_GPIO_RESET_BIT);
	dwVal = SETBIT(dwVal, BT878_GPIO_RESETSWITCH_BIT);
	
	dwVal = SETBIT(dwVal, BT878_GPIO_LED1_BIT);
	dwVal = SETBIT(dwVal, BT878_GPIO_LED2_BIT);
	
	dwVal = CLRBIT(dwVal, BT878_GPIO_VS_ACTIVE_BIT);
	dwVal = SETBIT(dwVal, BT878_GPIO_CLEAR_LATCH_BIT);
	
	dwVal = SETBIT(dwVal, BT878_GPIO_VS_MUX_A0_BIT);
	dwVal = SETBIT(dwVal, BT878_GPIO_VS_MUX_A1_BIT);
	
	dwVal = SETBIT(dwVal, BT878_GPIO_AA1_MUX_A0_BIT);
	dwVal = SETBIT(dwVal, BT878_GPIO_AA1_MUX_A1_BIT);
	
	dwVal = SETBIT(dwVal, BT878_GPIO_AA2_MUX_A0_BIT);
	dwVal = SETBIT(dwVal, BT878_GPIO_AA2_MUX_A1_BIT);
	
	dwVal = SETBIT(dwVal, BT878_GPIO_WATCHDOG_ACTIVE_BIT);
	
	BT878Out(BT878_GPIO_OUT_EN, dwVal);
	
	SetGPIOPin(BT878_GPIO_POWERSWITCH_BIT);		// Abfrage des Powerbuttons einschalten
	SetGPIOPin(BT878_GPIO_RESETSWITCH_BIT);		// Abfrage des Resetbuttons einschalten
	SetGPIOPin(BT878_GPIO_WATCHDOG_ACTIVE_BIT); // Watchdog freigeben
	SetGPIOPin(BT878_GPIO_CLEAR_LATCH_BIT);		// Clearlatch auf high
 
	// PLL Reference Multiplier
	BT878Out(BT878_TGCTRL,   0x00);
	BT878Out(BT878_PLL_F_LO, 0xf9); // LOBYTE(m_wPllFreq));
	BT878Out(BT878_PLL_F_HI, 0xdc); // HIBYTE(m_wPllFreq));
	BT878Out(BT878_PLL_XCI,  0x8e); // 0<<7 | 0<<6 | 6);

	BT878In(BT878_DSTATUS, dwVal);

	InitPLL();

	m_csBT878.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void CBT878::InitPLL()
{
	DWORD dwVal = 0;

	// PLL Reference Multiplier
	BT878Out(BT878_TGCTRL,   0x00);
	BT878Out(BT878_PLL_F_LO, 0xf9); // LOBYTE(m_wPllFreq));
	BT878Out(BT878_PLL_F_HI, 0xdc); // HIBYTE(m_wPllFreq));
	BT878Out(BT878_PLL_XCI,  0x8e); // 0<<7 | 0<<6 | 6);

	BT878In(BT878_DSTATUS, dwVal);
/*	DWORD dwTC = GetTickCount();
	do
	{
		if ((GetTickCount() - dwTC) > 500)
		{
			TRACE(_T("Pll out of lock\n"));
			break;
		}
		dwVal = CLRBIT(dwVal, BT878_DSTATUS_PLOCK_BIT);
		BT878Out(BT878_DSTATUS, dwVal);
		BT878In(BT878_DSTATUS, dwVal);
		TRACE(_T("Waiting 0x%x\n"), dwVal);
	}
	while (TSTBIT(dwVal, BT878_DSTATUS_PLOCK_BIT));
*/	
	Sleep(2000);
	BT878Out(BT878_TGCTRL,0x08);
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetGPIOPin(WORD wBit)
{
	DWORD dwVal = 0;
	BOOL  bResult = FALSE;
	if (BT878In(BT878_GPIO_DATA_CTL, dwVal))
	{
		dwVal = SETBIT(dwVal, wBit);
		bResult = BT878Out(BT878_GPIO_DATA_CTL, dwVal);
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT878::ClrGPIOPin(WORD wBit)
{
	DWORD dwVal = 0;
	BOOL  bResult = FALSE;
	if (BT878In(BT878_GPIO_DATA_CTL, dwVal))
	{
		dwVal = CLRBIT(dwVal, wBit);
		bResult = BT878Out(BT878_GPIO_DATA_CTL, dwVal);
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT878::ReadGPIOPin(WORD wBit)
{
	DWORD dwVal = 0;
	BOOL  bResult = FALSE;
	if (BT878In(BT878_GPIO_DATA_CTL, dwVal))
		bResult = TSTBIT(dwVal, wBit);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetVideoFormat(WORD wVideoFormat)
{
	m_csBT878.Lock();

	BYTE byMode =3;
	
    switch (wVideoFormat)
    {
  		case VFMT_NTSC_CCIR:
			byMode = 0;
    		break;
		case VFMT_PAL_CCIR:
			byMode = 1;
    		break;
    	default:
			ML_TRACE_ERROR(_T("CBT878::SetVideoFormat\tFalscher Parameter\n"));
			m_csBT878.Unlock();
			return FALSE;
    }

	// Diese Einstellungen sind vom Videoformat abhängig und müssen neu gesetzt werden
	BT878Out(BT878_IFORM, 			m_dwBT878Reg[BT878_IFORM][byMode]);
	
	BT878Out(BT878_E_CROP, 			m_dwBT878Reg[BT878_E_CROP][byMode]);
	BT878Out(BT878_O_CROP, 			m_dwBT878Reg[BT878_O_CROP][byMode]);

	BT878Out(BT878_E_VACTIVE_LO,	m_dwBT878Reg[BT878_E_VACTIVE_LO][byMode]);
	BT878Out(BT878_O_VACTIVE_LO,	m_dwBT878Reg[BT878_O_VACTIVE_LO][byMode]);
	
	BT878Out(BT878_E_HDELAY_LO,		m_dwBT878Reg[BT878_E_HDELAY_LO][byMode]);
	BT878Out(BT878_O_HDELAY_LO,		m_dwBT878Reg[BT878_O_HDELAY_LO][byMode]);
	
	BT878Out(BT878_E_HACTIVE_LO,	m_dwBT878Reg[BT878_E_HACTIVE_LO][byMode]);
	BT878Out(BT878_O_HACTIVE_LO,	m_dwBT878Reg[BT878_O_HACTIVE_LO][byMode]);
	
	BT878Out(BT878_ADELAY,	 		m_dwBT878Reg[BT878_ADELAY][byMode]);
	BT878Out(BT878_BDELAY, 			m_dwBT878Reg[BT878_BDELAY][byMode]);
	
	BT878Out(BT878_E_HSCALE_LO, 	m_dwBT878Reg[BT878_E_HSCALE_LO][byMode]);
	BT878Out(BT878_O_HSCALE_HI, 	m_dwBT878Reg[BT878_O_HSCALE_HI][byMode]);
	
	BT878Out(BT878_E_VSCALE_LO, 	m_dwBT878Reg[BT878_E_VSCALE_LO][byMode]);
	BT878Out(BT878_O_VSCALE_HI, 	m_dwBT878Reg[BT878_O_VSCALE_HI][byMode]);

	BT878Out(BT878_VTOTAL_LO, 		m_dwBT878Reg[BT878_VTOTAL_LO][byMode]);
	BT878Out(BT878_VTOTAL_HI, 		m_dwBT878Reg[BT878_VTOTAL_HI][byMode]);

	m_wVideoFormat = wVideoFormat;

	m_csBT878.Unlock();

	return TRUE;
}
	    
//////////////////////////////////////////////////////////////////////////
WORD CBT878::GetVideoFormat() const
{
	return m_wVideoFormat;
}


//////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetBrightness(WORD wBrightness)
{
	BOOL bResult	= FALSE;

	m_csBT878.Lock();
	bResult = BT878Out(BT878_BRIGHT, (BYTE)((short int)wBrightness - 128));
	m_csBT878.Unlock();

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT878::GetBrightness() const
{
	BOOL bResult	= FALSE;
	WORD wBrightness = 0;

	bResult = BT878In(BT878_BRIGHT, wBrightness);

	return (short int)wBrightness + 128;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetContrast(WORD wContrast)
{
	m_csBT878.Lock();
	
	BOOL bResult	= FALSE;
	BYTE byControl	= 0;

	if (BT878Out(BT878_CONTRAST_LO, (BYTE)wContrast))
	{
		if (BT878In(BT878_E_CONTROL, byControl))
		{
			if (wContrast > 0xff)
			{
				byControl = (BYTE)SETBIT(byControl, BT878_CONTROL_CON_MSB_BIT);
			}
			else
			{
				byControl = (BYTE)CLRBIT(byControl, BT878_CONTROL_CON_MSB_BIT);
			}

			bResult = BT878Out(BT878_E_CONTROL, byControl) && BT878Out(BT878_O_CONTROL, byControl);
		}
	}

	m_csBT878.Unlock();

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT878::GetContrast() const
{
	WORD wContrast		= 0;
	BYTE byControl		= 0;
	BYTE byContrastLo	= 0;

	if (BT878In(BT878_CONTRAST_LO, byContrastLo))
	{
		if (BT878In(BT878_E_CONTROL, byControl))
		{  
			if (TSTBIT(byControl, BT878_CONTROL_CON_MSB_BIT))
				wContrast = MAKEWORD(byContrastLo, 1);
			else
				wContrast = MAKEWORD(byContrastLo, 0);
		}
	}
	return wContrast;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetSaturation(WORD wSaturation)
{
	m_csBT878.Lock();
	WORD wSaturationU = wSaturation;
	WORD wSaturationV = (WORD)((float)wSaturation / 1.41111);

	BOOL bResult = FALSE;
	BYTE byControl = 0;

	if (BT878Out(BT878_SAT_U_LO, (BYTE)wSaturationU))
	{
		if (BT878In(BT878_E_CONTROL, byControl))
		{
			if (wSaturationU > 0xff)
			{
				byControl = (BYTE)SETBIT(byControl, BT878_CONTROL_SAT_U_MSB_BIT);
			}
			else
			{
				byControl = (BYTE)CLRBIT(byControl, BT878_CONTROL_SAT_U_MSB_BIT);
			}

			bResult = BT878Out(BT878_E_CONTROL, byControl) && BT878Out(BT878_O_CONTROL, byControl);
		}
	}

	if (BT878Out(BT878_SAT_V_LO, (BYTE)wSaturationV))
	{
		if (BT878In(BT878_E_CONTROL, byControl))
		{
			if (wSaturationV > 0xff)
			{
				byControl = (BYTE)SETBIT(byControl, BT878_CONTROL_SAT_V_MSB_BIT);
			}
			else
			{
				byControl = (BYTE)CLRBIT(byControl, BT878_CONTROL_SAT_V_MSB_BIT);
			}

			bResult = BT878Out(BT878_E_CONTROL, byControl) && BT878Out(BT878_O_CONTROL, byControl);
		}
	}

	m_csBT878.Unlock();
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT878::GetSaturation() const
{
	WORD wSaturation	= 0;
	BYTE byControl		= 0;
	BYTE bySaturationLo	= 0;

	if (BT878In(BT878_SAT_U_LO, bySaturationLo))
	{
		if (BT878In(BT878_E_CONTROL, byControl))
		{  
			if (TSTBIT(byControl, BT878_CONTROL_SAT_U_MSB_BIT))
				wSaturation = MAKEWORD(bySaturationLo, 1);
			else
				wSaturation = MAKEWORD(bySaturationLo, 0);
		}
	}

	return wSaturation;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetHue(WORD wHue)
{
	BOOL bResult = FALSE;

	m_csBT878.Lock();

	bResult = BT878Out(BT878_HUE, (BYTE)((short int)wHue - 128));

	m_csBT878.Unlock();
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT878::GetHue() const
{
	WORD wHue = 0;

	BT878In(BT878_HUE, wHue);

	return (short int) wHue+128;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetFilter(WORD wFilter)
{
	m_csBT878.Lock();
	
	BOOL bResult = FALSE;
	BYTE byControl = 0;

	if (BT878In(BT878_E_CONTROL, byControl))
	{
		if (wFilter & LNOTCH_FILTER)
		{
			byControl = (BYTE)CLRBIT(byControl, BT878_CONTROL_LNOTCH_BIT);	 	
		}
		else
		{
			byControl = (BYTE)SETBIT(byControl, BT878_CONTROL_LNOTCH_BIT);	 	
		}

		if (wFilter & LDEC_FILTER)
		{
			byControl = (BYTE)CLRBIT(byControl, BT878_CONTROL_LDEC_BIT);	 	
		}
		else
		{
			byControl = (BYTE)SETBIT(byControl, BT878_CONTROL_LDEC_BIT);	 	
		}
		
		if (BT878Out(BT878_E_CONTROL, byControl) && BT878Out(BT878_O_CONTROL, byControl))
			bResult = TRUE;
	}

	m_csBT878.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
WORD CBT878::GetFilter() const
{
	WORD wFilter	= 0;
	BYTE byControl	= 0;

	if (BT878In(BT878_E_CONTROL, byControl))
	{
		if (!TSTBIT(byControl, BT878_CONTROL_LNOTCH_BIT))
			wFilter |= LNOTCH_FILTER;
		if (!TSTBIT(byControl, BT878_CONTROL_LDEC_BIT))
			wFilter |= LDEC_FILTER;
	}
	return wFilter; 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetHScaleE(WORD wHScaleE)
{
	m_csBT878.Lock();

	BOOL bResult = FALSE;

	if (BT878Out(BT878_E_HSCALE_LO, LOBYTE(wHScaleE)))
		bResult = BT878Out(BT878_E_HSCALE_HI, HIBYTE(wHScaleE));
	
	m_csBT878.Unlock();

	return bResult; 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::GetHScaleE(WORD& wHScaleE) const
{
	BYTE	byLow=0;
	BYTE	byHigh=0;
	BOOL	bResult = FALSE;
	
	if (BT878In(BT878_E_HSCALE_LO, byLow))
	{
		if (BT878In(BT878_E_HSCALE_HI, byHigh))
		{
			wHScaleE = MAKEWORD(byLow, byHigh);
			bResult = TRUE;
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetVScaleE(WORD wVScaleE)
{
	m_csBT878.Lock();

	BOOL bResult = FALSE;

	if (BT878Out(BT878_E_VSCALE_LO, LOBYTE(wVScaleE)))
		bResult = BT878Out(BT878_E_VSCALE_HI, HIBYTE(wVScaleE));
	
	m_csBT878.Unlock();

	return bResult; 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::GetVScaleE(WORD& wVScaleE) const
{
	BYTE byLow		= 0;
	BYTE byHigh		= 0;
	BOOL bResult	= FALSE;

	if (BT878In(BT878_E_VSCALE_LO, byLow))
	{
		if (BT878In(BT878_E_VSCALE_HI, byHigh))
		{
			wVScaleE = MAKEWORD(byLow, byHigh);
			bResult = TRUE;
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetHScaleO(WORD wHScaleO)
{
	m_csBT878.Lock();

	BOOL bResult = FALSE;

	if (BT878Out(BT878_O_HSCALE_LO, LOBYTE(wHScaleO)))
		bResult = BT878Out(BT878_O_HSCALE_HI, HIBYTE(wHScaleO));
	
	m_csBT878.Unlock();

	return bResult; 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::GetHScaleO(WORD& wHScaleO) const
{
	BYTE	byLow=0;
	BYTE	byHigh=0;
	BOOL	bResult = FALSE;
	
	if (BT878In(BT878_O_HSCALE_LO, byLow))
	{
		if (BT878In(BT878_O_HSCALE_HI, byHigh))
		{
			wHScaleO = MAKEWORD(byLow, byHigh);
			bResult = TRUE;
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetVScaleO(WORD wVScaleO)
{
	m_csBT878.Lock();

	BOOL bResult = FALSE;

	if (BT878Out(BT878_O_VSCALE_LO, LOBYTE(wVScaleO)))
		bResult = BT878Out(BT878_O_VSCALE_HI, HIBYTE(wVScaleO));
	
	m_csBT878.Unlock();

	return bResult; 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::GetVScaleO(WORD& wVScaleO) const
{
	BYTE byLow		= 0;
	BYTE byHigh		= 0;
	BOOL bResult	= FALSE;

	if (BT878In(BT878_O_VSCALE_LO, byLow))
	{
		if (BT878In(BT878_O_VSCALE_HI, byHigh))
		{
			wVScaleO = MAKEWORD(byLow, byHigh);
			bResult = TRUE;
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetHActiveE(WORD wHActive)
{
	m_csBT878.Lock();

	BOOL bResult = FALSE;

	if (BT878Out(BT878_E_HACTIVE_LO, LOBYTE(wHActive)))
	{
		BYTE byVal = 0;
		if (BT878In(BT878_E_CROP, byVal))
		{		
			byVal &= 0xfC;
			byVal |= (HIBYTE(wHActive) & 0x03);
			BT878Out(BT878_E_CROP, byVal);
		}
	}

	m_csBT878.Unlock();
	
	return bResult; 
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetHActiveO(WORD wHActive)
{
	m_csBT878.Lock();

	BOOL bResult = FALSE;

	if (BT878Out(BT878_O_HACTIVE_LO, LOBYTE(wHActive)))
	{
		BYTE byVal = 0;
		if (BT878In(BT878_O_CROP, byVal))
		{		
			byVal &= 0xfC;
			byVal |= (HIBYTE(wHActive) & 0x03);
			BT878Out(BT878_O_CROP, byVal);
		}
	}

	m_csBT878.Unlock();
	
	return bResult; 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetHDelayE(WORD wHDelay)
{
	m_csBT878.Lock();

	BOOL bResult = FALSE;

	if (BT878Out(BT878_E_HDELAY_LO, LOBYTE(wHDelay)))
	{
		BYTE byVal = 0;
		if (BT878In(BT878_E_CROP, byVal))
		{		
			byVal &= 0xf3;
			byVal |= (HIBYTE(wHDelay) & 0x03)<<2;
			BT878Out(BT878_E_CROP, byVal);
		}
	}
	
	m_csBT878.Unlock();

	return bResult; 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetHDelayO(WORD wHDelay)
{
	m_csBT878.Lock();

	BOOL bResult = FALSE;

	if (BT878Out(BT878_O_HDELAY_LO, LOBYTE(wHDelay)))
	{
		BYTE byVal = 0;
		if (BT878In(BT878_O_CROP, byVal))
		{		
			byVal &= 0xf3;
			byVal |= (HIBYTE(wHDelay) & 0x03)<<2;
			BT878Out(BT878_O_CROP, byVal);
		}
	}

	m_csBT878.Unlock();

	return bResult; 
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::GetHActiveE(WORD& wHActive)
{
	BYTE byLow		= 0;
	BYTE byHigh		= 0;
	BOOL bResult = FALSE;

	m_csBT878.Lock();

	if (BT878In(BT878_E_HACTIVE_LO, byLow))
	{
		if (BT878In(BT878_E_CROP, byHigh))
		{	
			wHActive = MAKEWORD(byLow, byHigh & 0x03);
			bResult = TRUE;
		}
	}

	m_csBT878.Unlock();
	
	return bResult; 
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::GetHActiveO(WORD& wHActive)
{
	BYTE byLow		= 0;
	BYTE byHigh		= 0;
	BOOL bResult = FALSE;

	m_csBT878.Lock();

	if (BT878In(BT878_O_HACTIVE_LO, byLow))
	{
		if (BT878In(BT878_O_CROP, byHigh))
		{	
			wHActive = MAKEWORD(byLow, byHigh & 0x03);
			bResult = TRUE;
		}
	}

	m_csBT878.Unlock();
	
	return bResult; 
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::GetHDelayE(WORD& wHDelay)
{
	BYTE byLow		= 0;
	BYTE byHigh		= 0;
	BOOL bResult = FALSE;

	m_csBT878.Lock();

	if (BT878In(BT878_E_HDELAY_LO, byLow))
	{
		if (BT878In(BT878_E_CROP, byHigh))
		{	
			wHDelay = MAKEWORD(byLow, (byHigh>>2) & 0x03);
			bResult = TRUE;
		}
	}

	m_csBT878.Unlock();
	
	return bResult; 
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::GetHDelayO(WORD& wHDelay)
{
	BYTE byLow		= 0;
	BYTE byHigh		= 0;
	BOOL bResult = FALSE;

	m_csBT878.Lock();

	if (BT878In(BT878_O_HDELAY_LO, byLow))
	{
		if (BT878In(BT878_E_CROP, byHigh))
		{	
			wHDelay = MAKEWORD(byLow, (byHigh>>2) & 0x03);
			bResult = TRUE;
		}
	}

	m_csBT878.Unlock();
	
	return bResult; 
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT878::IsVideoPresent(BOOL& bPresent)
{
	m_csBT878.Lock();
	BYTE byVal = 0;
	BOOL bResult = FALSE;
	
	// 3ms warten, damit nach einer Umschaltung ein stabiler Wert gelesen werden kann
	WK_Sleep(4);
	if (BT878In(BT878_DSTATUS, byVal))
	{
		bPresent = TSTBIT(byVal, BT878_DSTATUS_PRES_BIT);
//		TRACE(_T("DStatus=0x%02x Present=%d\n"), byVal, bPresent);
		bResult = TRUE;
	}
	m_csBT878.Unlock();

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::DetectVideoFormat(WORD& wFormat)
{
	BOOL	bResult = FALSE;
	BYTE	byVal = 0;

	// The Videodecoder needs 32 fields to detect videonorm. (12000ms = 60 fields)
	Sleep(2000);
	
	m_csBT878.Lock();
	if (BT878In(BT878_DSTATUS, byVal))
	{
		if (TSTBIT(byVal, BT878_DSTATUS_NUML_BIT))
			wFormat = VFMT_PAL_CCIR;
		else
			wFormat = VFMT_NTSC_CCIR;
		bResult = TRUE;
	}
	m_csBT878.Unlock();

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::BT878Out(WORD wAddress, DWORD dwData)
{
	m_csBT878.Lock();

	m_pCIo->Outputdw(wAddress, dwData);

	m_csBT878.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::BT878In(WORD wAddress, DWORD& dwData) const
{
	dwData = m_pCIo->Inputdw(wAddress);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::BT878In(WORD wAddress, WORD& wData) const
{
	wData = m_pCIo->Inputdw(wAddress);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CBT878::BT878In(WORD wAddress, BYTE& byData) const
{
	byData = m_pCIo->Inputdw(wAddress);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
void CBT878::OnError(WORD wErr, WORD wParam1, WORD /*wParam2*/)	const
{ 		
	switch (wErr)
	{
		case ERR_BT878_WRONG_ID:
			{
				CString sError, sMsg;
				sMsg.LoadString(IDS_RTE_BT878_WRONG_ID);
				sError.Format(sMsg, wParam1);
				if (m_pCodec)
					m_pCodec->OnReceivedRunTimeError(REL_CANNOT_RUN, RTE_HARDWARE, sError);
				break;
			}
		case ERR_BT878_READBACK_FAILED:
			{
				CString sError, sMsg;
				sMsg.LoadString(IDS_RTE_BT878_READ_BACK);
				sError.Format(sMsg);
				if (m_pCodec)
					m_pCodec->OnReceivedRunTimeError(REL_CANNOT_RUN, RTE_HARDWARE, sError);
				break;
			}
		default:
			ML_TRACE_ERROR(_T("Unknown BT878-Error\n"));
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CBT878::EnableAGC(BOOL bEnable)
{
	BOOL bResult= FALSE;
	BYTE byReg = 0;

	if (BT878In(BT878_ADC, byReg))
	{
		if (bEnable)
			byReg = CLRBIT(byReg, BT878_ADC_AGC_EN_BIT);
		else
			byReg = SETBIT(byReg, BT878_ADC_AGC_EN_BIT);
		bResult = BT878Out(BT878_ADC, byReg);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::VideoInlayInit()
{
	// VideoInlay-Klasse anlegen
	if (!m_pVideoInlay)
	{
		m_pVideoInlay	= new CVideoInlay(this);
		if (!m_pVideoInlay)
			return FALSE;
	}

	m_pVideoInlay->Init(m_pCodec);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::VideoInlaySetOutputWindow(const CRect &rc) const
{								
	if (!m_pVideoInlay)
		return FALSE;

	m_pVideoInlay->SetOutputWindow(rc);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::VideoInlayActivate() const
{
	if (!m_pVideoInlay)
		return FALSE;

	m_pVideoInlay->Activate();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::VideoInlayDeactivate() const
{
	if (m_pVideoInlay)
		m_pVideoInlay->Deactivate();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::MotionDetectionInit()
{
	// MotionDetection-Klasse anlegen
	if (!m_pMotionDetection)
	{
		m_pMotionDetection	= new CMotionDetection(this, m_sAppIniName);
		if (!m_pMotionDetection)
			return FALSE;
	}

	m_pMotionDetection->Init(m_pCodec);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::MotionCheck(CAPTURE_STRUCT& Capture, CMDPoints &Points) const
{
	if (!m_pMotionDetection || !Capture.Buffer.pLinAddr)
		return FALSE;

	return m_pMotionDetection->MotionCheck(Capture, Points);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::MotionDetectionConfiguration(WORD wSource)
{
	if (!m_pMotionDetection)
		return FALSE;

	return	m_pMotionDetection->OpenConfigDlg(wSource);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CBT878::GetMask(WORD wSource, CIPCActivityMask& mask)
{
	if (!m_pMotionDetection)
		return FALSE;

	return m_pMotionDetection->GetMask(wSource, mask);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetMask(WORD wSource, const CIPCActivityMask& mask)
{
	if (!m_pMotionDetection)
		return FALSE;

	return m_pMotionDetection->SetMask(wSource, mask);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::EnableDIBIndication(WORD wSource)
{
	if (!m_pMotionDetection)
		return FALSE;
	
	return m_pMotionDetection->EnableDIBIndication(wSource);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::DisableDIBIndication(WORD wSource)
{
	if (!m_pMotionDetection)
		return FALSE;
	
	return m_pMotionDetection->DisableDIBIndication(wSource);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::ReLoadMDConfiguration()
{								
	if (!m_pMotionDetection)
		return FALSE;

	// Alle MD-Parameter erneut laden
	for (WORD wSubID = 0; wSubID < MAX_CAMERAS; wSubID++)
		m_pMotionDetection->LoadParameter(wSubID);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::ActivateMotionDetection(WORD wSource)
{
	if (!m_pMotionDetection)
		return FALSE;

	return	m_pMotionDetection->ActivateMotionDetection(wSource);
}


/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::DeactivateMotionDetection(WORD wSource)
{
	if (!m_pMotionDetection)
		return FALSE;

	return	m_pMotionDetection->DeactivateMotionDetection(wSource);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetMDMaskSensitivity(WORD wSource, const CString &sLevel)
{
	if (!m_pMotionDetection)
		return FALSE;

	return m_pMotionDetection->SetMDMaskSensitivity(wSource, sLevel);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetMDAlarmSensitivity(WORD wSource, const CString &sLevel)
{
	if (!m_pMotionDetection)
		return FALSE;

	return m_pMotionDetection->SetMDAlarmSensitivity(wSource, sLevel);
}

/////////////////////////////////////////////////////////////////////////////
CString CBT878::GetMDMaskSensitivity(WORD wSource)
{
	CString sLevel = _T("Unknown");

	if (m_pMotionDetection)
	{
		sLevel = m_pMotionDetection->GetMDMaskSensitivity(wSource);
	}

	return sLevel;
}

/////////////////////////////////////////////////////////////////////////////
CString CBT878::GetMDAlarmSensitivity(WORD wSource)
{
	CString sLevel = _T("Unknown");

	if (m_pMotionDetection)
	{
		sLevel = m_pMotionDetection->GetMDAlarmSensitivity(wSource);
	}

	return sLevel;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetVideoSource(SOURCE_SELECT_STRUCT &SrcSel)
{
	BOOL bResult	= FALSE;
	BYTE byIForm	= 0;
	BYTE byMuxSel	= 0;

	m_csBT878.Lock();

	switch (SrcSel.wPortYC)
	{
		case VIDEO_SOURCE0:
			byMuxSel = 2;
			break;
		case VIDEO_SOURCE1:
			byMuxSel = 3;
			break;
		case VIDEO_SOURCE2:
			byMuxSel = 1;
			break;
		case VIDEO_SOURCE3:
			byMuxSel = 0;
			break;
		default:
			ML_TRACE_WARNING(_T("SaVic Driver: Unknown Camport (%d)\n"), SrcSel.wPortYC);
	}

	if (BT878In(BT878_IFORM, byIForm))
	{
		byIForm &= 0x9f;	// 0x9f = 10011111b (Bit 5 und 6 löschen)
		byIForm |= (byMuxSel<<5);
		bResult = BT878Out(BT878_IFORM, byIForm);
	}

	m_csBT878.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::GetVideoSource(WORD& wSource)
{
	BOOL bResult = FALSE;
	BYTE byIForm	 = 0;

	if (BT878In(BT878_IFORM, byIForm))
	{
		byIForm &= 0x60;	// 0x60 = 01100000b	 (Alle Bits bis auf Bit 5 und 6 löschen)
		wSource = byIForm>>5;
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::SetCameraToAnalogOut(WORD wSource, WORD wPort)
{
	BOOL	bResult = FALSE;
	DWORD	dwValue	= 0;

	m_csBT878.Lock();

	if (BT878In(BT878_GPIO_DATA_CTL, dwValue))
	{
		if (wPort == VS_MUX)	// Syncdetection
		{
			dwValue = (dwValue & 0xffff3fff) | (wSource<<14);
			bResult = BT878Out(BT878_GPIO_DATA_CTL, dwValue);
		}
		else if (wPort == AA1_MUX)	// 1. Analoger Ausgang
		{
			dwValue = (dwValue & 0xfffcffff) | (wSource<<16);
			bResult = BT878Out(BT878_GPIO_DATA_CTL, dwValue);
		}
		else if (wPort == AA2_MUX)	// 2. Analoger Ausgang
		{
			dwValue = (dwValue & 0xfff3ffff) | (wSource<<18);
			bResult = BT878Out(BT878_GPIO_DATA_CTL, dwValue);
		}
		else
			ML_TRACE_WARNING(_T("SetCameraToAnalogOut unknown Port (%d)\n"), wPort);
	}

	m_csBT878.Unlock();
	
	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::EncoderStart()
{
	BOOL bResult = FALSE;
	WORD wReg = 0;
	BYTE byStatus = 0;
	BOOL b0, b1 = FALSE;

	m_csBT878.Lock();

	// Der Start des DMA-Transfer muß synchron zum Bildsignal erfolgen, deshalb wird
	// hier zunächst auf den Beginn des nächsten Bildes gewartet und dann der DMA-Transfer
	// aktiviert.
	if (BT878In(BT878_GPIO_DMA_CTL, wReg))
	{
		wReg = SETBIT(wReg, BT878_GPIO_DMA_CTL_FIFO_ENABLE_BIT);
		wReg = SETBIT(wReg, BT878_GPIO_DMA_CTL_RISC_ENABLE_BIT);
	
		if (BT878In(BT878_DSTATUS, byStatus))
		{
			b0 = TSTBIT(byStatus, BT878_DSTATUS_FIELD_BIT);
			DWORD dwTC = GetTickCount();
			do
			{
				if (BT878In(BT878_DSTATUS, byStatus))
					b1 = TSTBIT(byStatus, BT878_DSTATUS_FIELD_BIT);
			}
			while((b0 == b1) && GetTimeSpan(dwTC) < 250);
			
			if (b0 != b1)
				bResult = BT878Out(BT878_GPIO_DMA_CTL, wReg);
			else
				ML_TRACE(_T("EnableRisc waiting for even field timeout\n"));
		}
	}

	m_csBT878.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::EncoderStop()
{
	BOOL bResult = FALSE;

	m_csBT878.Lock();
	
	WORD wReg = 0;
	if (BT878In(BT878_GPIO_DMA_CTL, wReg))
	{
		wReg = CLRBIT(wReg, BT878_GPIO_DMA_CTL_FIFO_ENABLE_BIT);
		wReg = CLRBIT(wReg, BT878_GPIO_DMA_CTL_RISC_ENABLE_BIT);
		if (BT878Out(BT878_GPIO_DMA_CTL, wReg))
			bResult = TRUE;
	}

	m_csBT878.Unlock();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT878::RestartEncoder()
{
	BOOL bResult = FALSE;

	if (EncoderStop())
	{
		InitPLL();
		bResult = EncoderStart();
	}

	return bResult;
}
