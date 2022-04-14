/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CBT829.CPP $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CBT829.CPP $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "conio.h"
#include "MiCoDefs.h"
#include "MiCoReg.h"
#include "CBT829.h"
#include "MiCOini.h"

CCriticalSection	CBT829::m_csBT829;
int					CBT829::m_nBT829InstCnt = 0;

/////////////////////////////////////////////////////////////////////////////
CBT829::CBT829(WORD wIOBase, WORD wFeID) : CI2C(wIOBase, BT829_SLAVE_ADR) 
{
	m_csBT829.Lock();

	// Die MiCo besitzt zwei Frontends, deshalb die ID
	m_wFeID = wFeID;

	// Basisklasse korrekt angelegt ?
	if (!CI2C::IsValid())
	{
		m_csBT829.Unlock();
		return;
	}

	m_wFeVideoFormat	= MICO_PAL_SQUARE;
	m_wFeSourceType		= MICO_FBAS;
	m_wFeBrightness		= (MICO_MAX_BRIGHTNESS	- MICO_MIN_BRIGHTNESS)	/ 2;
	m_wFeContrast		= (MICO_MAX_CONTRAST	- MICO_MIN_CONTRAST)	/ 2;
	m_wFeSaturation		= (MICO_MAX_SATURATION	- MICO_MIN_SATURATION)	/ 2;
	m_wFeHue			= (MICO_MAX_HUE			- MICO_MIN_HUE)			/ 2;
	m_wFeHScale			= (MICO_MAX_HSCALE		- MICO_MIN_HSCALE)		/ 2;
	m_wFeVScale			= (MICO_MAX_VSCALE		- MICO_MIN_VSCALE)		/ 2;
	m_wFeFilter			= MICO_NOTCH_FILTER_ON;
	m_wIOBase			= wIOBase;

	// Zählt die Anzahl der CBT829-Objekte
	m_nBT829InstCnt++;

	// Registerbuffer auf 0 initialisieren
	for (int nReg = 0; nReg < BT829_MAX_REG; nReg++)
	{
	 	for (int nMode = 0; nMode < 4; nMode++){
	 		m_byBT829Reg[nReg][nMode] = 0;
		}
	}

	CBT829Reset();

	m_bOk = TRUE;

	m_csBT829.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
CBT829::~CBT829()
{
}
//////////////////////////////////////////////////////////////////////////////////////
void CBT829::CBT829Reset()
{
	m_csBT829.Lock();
	
	// Beim zweiten Frontend darf kein Reset durchgeführt werden, da der Reset
	// beide Frondends zurücksetzt.

	if (m_nBT829InstCnt > 1)
	{
		m_csBT829.Unlock();
		return;
	}
    
	BYTE byReset;

	// I2C-Bus Controller reseten...
	byReset = m_Io.ReadFromLatch(m_wIOBase | RESET_OFFSET);

	byReset = (BYTE)CLRBIT(byReset, PARAM_RESET_BT829_BIT);

	m_Io.WriteToLatch(m_wIOBase | RESET_OFFSET, byReset);

	Sleep(10);

	byReset = (BYTE)SETBIT(byReset, PARAM_RESET_BT829_BIT);

	m_Io.WriteToLatch(m_wIOBase | RESET_OFFSET, byReset);

	Sleep(10);

	m_csBT829.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT829::CheckIt()
{
	m_csBT829.Lock();

	BYTE byID = BT829In(BT829_IDCODE);
	if ((byID & 0xf0) != 0xe0)	// ID des BT829 = 0xe0
	{
		WK_TRACE(TRUE, "\tBT829 (%u)\twrong ID-Code (0x%02x)", m_wFeID+1, (WORD)byID);
		m_csBT829.Unlock();
    	return FALSE;
    }
    else
		WK_TRACE(TRUE, "\tBT829 (%u)\tID-Code  test passed\n", m_wFeID+1);
		
	for (WORD wReg = 0; wReg <= 0x1a; wReg++)
	{
		if ((wReg != BT829_IDCODE) && (wReg != BT829_STATUS) && (wReg != BT829_TEST))
		{
			for (WORD wVal = 0; wVal <= 255; wVal++)
			{
				BT829Out(wReg, wVal);
				if (BT829In(wReg) != wVal)
				{
					WK_TRACE(TRUE, "\tBT829 (%u)\tReadback test failed\n", m_wFeID+1);
					m_csBT829.Unlock();
				    return FALSE;
				}
			}
		}
	}
	WK_TRACE(TRUE, "\tBT829 (%u)\tReadback test passed\n", m_wFeID+1);
		
	m_csBT829.Unlock();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT829::Init()
{
	m_csBT829.Lock();

	BYTE		byReg;
	WORD		wMode;
    WORD		wI;
    
	for (wI = 0; wI < BT829REGCOUNT; wI++)
	{  
		wMode = MICO_NTSC_SQUARE;
		byReg = HIBYTE(BT829_NTSC_SQUARE[wI]);
		m_byBT829Reg[byReg][wMode] = LOBYTE(BT829_NTSC_SQUARE[wI]);
		
		wMode = MICO_PAL_SQUARE;
		byReg = HIBYTE(BT829_PAL_SQUARE[wI]);
		m_byBT829Reg[byReg][wMode] = LOBYTE(BT829_PAL_SQUARE[wI]);
	
		wMode = MICO_NTSC_CCIR;
		byReg = HIBYTE(BT829_NTSC_CCIR[wI]);
		m_byBT829Reg[byReg][wMode] = LOBYTE(BT829_NTSC_CCIR[wI]);

		wMode = MICO_PAL_CCIR;
		byReg = HIBYTE(BT829_PAL_CCIR[wI]);
		m_byBT829Reg[byReg][wMode] = LOBYTE(BT829_PAL_CCIR[wI]);
		BT829Out(byReg, LOBYTE(BT829_PAL_CCIR[wI]));
	}

	m_csBT829.Unlock();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT829::IsValid()
{
	return ((m_bOk == TRUE) && (CI2C::IsValid()));
}


//////////////////////////////////////////////////////////////////////////
BOOL CBT829::SetFeVideoFormat(WORD wFeVideoFormat)
{
	m_csBT829.Lock();

	BYTE byMode;
	
    switch (wFeVideoFormat)
    {
    	case MICO_NTSC_SQUARE:
//			WK_TRACE(TRUE, "CBT829::SetFeVideoFormat\tNTSC Square\n");
			byMode = MICO_NTSC_SQUARE;
			break;

    	case MICO_PAL_SQUARE:
//			WK_TRACE(TRUE, "CBT829::SetFeVideoFormat\tPAL Square\n");
			byMode = MICO_PAL_SQUARE;
			break;

		case MICO_NTSC_CCIR:
//			WK_TRACE(TRUE, "CBT829::SetFeVideoFormat\tNTSC CCIR\n");
			byMode = MICO_NTSC_CCIR;
    		break;

		case MICO_PAL_CCIR:
//			WK_TRACE(TRUE, "CBT829::SetFeVideoFormat\tPAL CCIR\n");
			byMode = MICO_PAL_CCIR;
    		break;
    	default:
			WK_TRACE(TRUE, "CBT829::SetFeVideoFormat\tFalscher Parameter\n");
			m_csBT829.Unlock();
			return FALSE;
    }

	BT829Out(BT829_IFORM, 		m_byBT829Reg[BT829_IFORM][byMode]);
	BT829Out(BT829_CROP,  		m_byBT829Reg[BT829_CROP][byMode]);
	BT829Out(BT829_VACTIVE_LO,	m_byBT829Reg[BT829_VACTIVE_LO][byMode]);
	BT829Out(BT829_HDELAY_LO,	m_byBT829Reg[BT829_HDELAY_LO][byMode]);
	BT829Out(BT829_HACTIVE_LO,	m_byBT829Reg[BT829_HACTIVE_LO][byMode]);
	BT829Out(BT829_ADELAY, 		m_byBT829Reg[BT829_ADELAY][byMode]);
	BT829Out(BT829_BDELAY, 		m_byBT829Reg[BT829_BDELAY][byMode]);
	BT829Out(BT829_HSCALE_LO, 	m_byBT829Reg[BT829_HSCALE_LO][byMode]);
	BT829Out(BT829_HSCALE_HI, 	m_byBT829Reg[BT829_HSCALE_HI][byMode]);
	BT829Out(BT829_VSCALE_LO, 	m_byBT829Reg[BT829_VSCALE_LO][byMode]);
	BT829Out(BT829_VSCALE_HI, 	m_byBT829Reg[BT829_VSCALE_HI][byMode]);

	m_wFeVideoFormat = wFeVideoFormat;
	
	m_csBT829.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT829::GetFeVideoFormat()
{
	return m_wFeVideoFormat;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT829::SetFeSourceType(WORD wFeSourceType, WORD wFeFilter)
{
	m_csBT829.Lock();

	BYTE byControl;

	byControl = BT829In(BT829_CONTROL);

    switch (wFeSourceType)
    {
    	case MICO_FBAS:
//			WK_TRACE(TRUE, "CBT829::SetFeSourceType\tFBAS\n");
	 		byControl = (BYTE)CLRBIT(byControl, CONTROL_COMP_BIT);
			byControl = (BYTE)SETBIT(byControl, CONTROL_LDEC_BIT);		// Decfilter ein 	
			if ((wFeFilter == MICO_NOTCH_FILTER_AUTO) || (wFeFilter == MICO_NOTCH_FILTER_ON)){
				byControl = (BYTE)CLRBIT(byControl, CONTROL_LNOTCH_BIT);// Notchfilter ein		 	
			}
			else{
				byControl = (BYTE)SETBIT(byControl, CONTROL_LNOTCH_BIT);// Notchfilter aus		 	
			}
			break;

    	case MICO_SVHS:
//			WK_TRACE(TRUE, "CBT829::SetFeSourceType\tSVHS\n");
	 		byControl = (BYTE)SETBIT(byControl, CONTROL_COMP_BIT);
			byControl = (BYTE)SETBIT(byControl, CONTROL_LDEC_BIT);		// Decfilter ein 	
			if ((wFeFilter == MICO_NOTCH_FILTER_AUTO) || (wFeFilter == MICO_NOTCH_FILTER_OFF)){
				byControl = (BYTE)SETBIT(byControl, CONTROL_LNOTCH_BIT);// Notchfilter aus	 	
			}
			else{
				byControl = (BYTE)CLRBIT(byControl, CONTROL_LNOTCH_BIT);// Notchfilter ein	 	
			}
			break;

    	default:
			WK_TRACE(TRUE, "CBT829::SetFeSourceType\tFalscher Parameter (%u)\n", wFeSourceType);
			m_csBT829.Unlock();
			return FALSE;
    }

	BT829Out(BT829_CONTROL, byControl);

	m_wFeSourceType = wFeSourceType;

	m_csBT829.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT829::GetFeSourceType()
{
	return m_wFeSourceType;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT829::SetFeBrightness(WORD wFeBrightness)
{
	m_csBT829.Lock();
	//	WK_TRACE(TRUE, "CBT829::SetFeBrightness\t%u", wFeBrightness);

	BT829Out(BT829_BRIGHT, (BYTE)((short int)wFeBrightness - 128));

	m_wFeBrightness = wFeBrightness;

	m_csBT829.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT829::GetFeBrightness()
{
	return m_wFeBrightness;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT829::SetFeContrast(WORD wFeContrast)
{
	m_csBT829.Lock();

	BYTE byControl;
	
	byControl = BT829In(BT829_CONTROL);

	BT829Out(BT829_CONTRAST_LO, (BYTE)wFeContrast);

	if (wFeContrast > 0xff){
		byControl = (BYTE)SETBIT(byControl, CONTROL_CON_MSB_BIT);
	}
	else{
		byControl = (BYTE)CLRBIT(byControl, CONTROL_CON_MSB_BIT);
	}

//	WK_TRACE(TRUE, "CBT829::SetFeContrast\t%u", wFeContrast);

	BT829Out(BT829_CONTROL, byControl);

	m_wFeContrast = wFeContrast;

	m_csBT829.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT829::GetFeContrast()
{
	return m_wFeContrast;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT829::SetFeSaturation(WORD wFeSaturation)
{
	m_csBT829.Lock();

	BYTE  byControl;

	byControl = BT829In(BT829_CONTROL);

	BT829Out(BT829_SAT_U_LO, (BYTE)wFeSaturation);
	BT829Out(BT829_SAT_V_LO, (BYTE)wFeSaturation);

	if (wFeSaturation > 0xff)
	{
		byControl = (BYTE)SETBIT(byControl, CONTROL_SAT_V_MSB_BIT);
		byControl = (BYTE)SETBIT(byControl, CONTROL_SAT_U_MSB_BIT);
	}
	else
	{
		byControl = (BYTE)CLRBIT(byControl, CONTROL_SAT_V_MSB_BIT);
		byControl = (BYTE)CLRBIT(byControl, CONTROL_SAT_U_MSB_BIT);
    }
    
//	WK_TRACE(TRUE, "CBT829::SetFeSaturation\t%u", wFeSaturation);

	BT829Out(BT829_CONTROL, byControl);

	m_wFeSaturation = wFeSaturation;

	m_csBT829.Unlock();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT829::IsVideoPresent()
{
	m_csBT829.Lock();

	BOOL bRet = TRUE;

	bRet = TSTBIT(BT829In(BT829_STATUS), STATUS_PRES_BIT);

	m_csBT829.Unlock();

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT829::GetFeSaturation()
{
	return m_wFeSaturation;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBT829::SetFeHue(WORD wFeHue)
{
	m_csBT829.Lock();

	//	WK_TRACE(TRUE, "CBT829::SetFeHue\t%u", wFeHue);

	BT829Out(BT829_HUE, (BYTE)((short int)wFeHue - 128));

	m_wFeHue = wFeHue;

	m_csBT829.Unlock();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
WORD CBT829::GetFeHue()
{
	return m_wFeHue;//OOPS inline ??
}

/////////////////////////////////////////////////////////////////////////////
WORD CBT829::SetFeHScale(WORD wFeHScale)
{
	m_csBT829.Lock();

	WORD wOldFeHScale = GetFeHScale();

//	WK_TRACE(TRUE, "CBT829::SetFeHScale\tFeHale=%u\n", wFeHScale);

	BT829Out(BT829_HSCALE_LO, LOBYTE(wFeHScale));
	BT829Out(BT829_HSCALE_HI, HIBYTE(wFeHScale));

	m_wFeHScale = wFeHScale;
	
	m_csBT829.Unlock();

	return wOldFeHScale; 
}

/////////////////////////////////////////////////////////////////////////////
WORD CBT829::GetFeHScale()
{
	return m_wFeHScale;//OOPS inline ??
}

/////////////////////////////////////////////////////////////////////////////
WORD CBT829::SetFeVScale(WORD wFeVScale)
{
	m_csBT829.Lock();

	WORD wOldFeVScale = GetFeVScale();

//	WK_TRACE(TRUE, "CBT829::SetFeVScale\tVScale=%u\n", wFeVScale);

	BT829Out(BT829_VSCALE_LO, LOBYTE(wFeVScale));
	BT829Out(BT829_VSCALE_HI, HIBYTE(wFeVScale));
	
	m_wFeVScale = wFeVScale;
	
	m_csBT829.Unlock();

	return wOldFeVScale; 
}

/////////////////////////////////////////////////////////////////////////////
WORD CBT829::GetFeVScale()
{
	return m_wFeVScale;//OOPS inline ??
}

/////////////////////////////////////////////////////////////////////////////
WORD CBT829::SetFeFilter(WORD wFeFilter)
{
	m_csBT829.Lock();

	WORD wOldFeFilter = wFeFilter;
	BYTE byControl;
	
	byControl = BT829In(BT829_CONTROL);

	if (wFeFilter & MICO_LNOTCH)
	{
//		WK_TRACE(TRUE, "CBT829::SetFeFilter\tLNotch On\n");
		byControl = (BYTE)SETBIT(byControl, CONTROL_LNOTCH_BIT);	 	
    }
	else
	{
//		WK_TRACE(TRUE, "CBT829::SetFeFilter\tSetFeFilter\tLNotch Off\n");
		byControl = (BYTE)CLRBIT(byControl, CONTROL_LNOTCH_BIT);	 	
    }

	if (wFeFilter & MICO_LDEC)
	{
//		WK_TRACE(TRUE, "CBT829::SetFeFilter\tSetFeFilter\tLDec On\n");
		byControl = (BYTE)SETBIT(byControl, CONTROL_LDEC_BIT);	 	
    }
	else
	{
//		WK_TRACE(TRUE, "CBT829::SetFeFilter\tSetFeFilter\tLDec Off\n");
		byControl = (BYTE)CLRBIT(byControl, CONTROL_LDEC_BIT);	 	
    }
	
	BT829Out(BT829_CONTROL, byControl);
	
	m_wFeFilter = wFeFilter;

	m_csBT829.Unlock();

	return wOldFeFilter;
}

/////////////////////////////////////////////////////////////////////////////
WORD CBT829::GetFeFilter()
{
	return m_wFeFilter; //OOPS inline ??
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT829::BT829Out(BYTE bySubAddress, BYTE byData)
{
	m_csBT829.Lock();

	BYTE byVal = m_Io.ReadFromLatch(m_wIOBase | CSVINDN_OFFSET);

	// Frontend auswählen
	if (m_wFeID == MICO_FE0){
		byVal = CLRBIT(byVal, SELECT_I2C_BIT);
	}
	else if (m_wFeID == MICO_FE1){
		byVal = SETBIT(byVal, SELECT_I2C_BIT);
	}

	m_Io.WriteToLatch(m_wIOBase | CSVINDN_OFFSET, byVal);

	BOOL bRet = WriteToI2C(bySubAddress, byData);

	m_csBT829.Unlock();

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CBT829::BT829In(BYTE bySubAddress)
{
	m_csBT829.Lock();

	BYTE byVal = m_Io.ReadFromLatch(m_wIOBase | CSVINDN_OFFSET);

	// Frontend auswählen
	if (m_wFeID == MICO_FE0){
		byVal = CLRBIT(byVal, SELECT_I2C_BIT);
	}
	else if (m_wFeID == MICO_FE1){
		byVal = SETBIT(byVal, SELECT_I2C_BIT);
	}

	m_Io.WriteToLatch(m_wIOBase | CSVINDN_OFFSET, byVal);

	BYTE byRet = ReadFromI2C(bySubAddress);

	m_csBT829.Unlock();

	return byRet;
}

