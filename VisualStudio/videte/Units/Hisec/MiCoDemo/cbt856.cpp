#include "stdafx.h"
#include "MiCoDefs.h"
#include "MiCoReg.h"
#include "CBT856.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
CBT856::CBT856(WORD wIOBase) : CI2C(wIOBase, BT856_SLAVE_ADR) 
{
	m_csBT856.Lock();

	// Basisklasse korrekt angelegt ?
	if (!CI2C::IsValid())
	{
		m_csBT856.Unlock();
		return;
	}

	m_wIOBase		 = wIOBase;
	m_wBeVideoFormat = MICO_PAL_CCIR;

	// Latch auf 0 initialisieren
	for (int nReg = 0; nReg < BT856_MAX_REG; nReg++){
 		m_byBT856Reg[nReg] = 0;
	}
	
	m_bOk = TRUE;

	m_csBT856.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT856::CheckIt()
{
	m_csBT856.Lock();

	BYTE byID = BT856In(BT856_GETID);

	// VendorID korrekt?
	if (byID != 0x60)
	{
		CString sError;
		sError.LoadString(IDS_RTE_BT856_WRONG_ID);
		sError.Format(sError, (WORD)byID);

		WK_TRACE_ERROR(sError);

		m_csBT856.Unlock();
		return FALSE;
	}
	
	m_csBT856.Unlock();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
WORD CBT856::SetBeVideoFormat(WORD wBeVideoFormat)
{
	m_csBT856.Lock();

	WORD wOldBeVideoFormat = GetBeVideoFormat();
	BYTE byControl;

	byControl = BT856In(BT856_MODE_CONTROL);
    
	switch (wBeVideoFormat)
	{
	 	case MICO_PAL_CCIR:
			byControl = (BYTE)SETBIT(byControl, 2);	 	
			byControl = (BYTE)CLRBIT(byControl, 0);	 	
			break;                           

		case MICO_NTSC_CCIR:
			byControl = (BYTE)CLRBIT(byControl, 2);	 	
			byControl = (BYTE)CLRBIT(byControl, 0);	 	
	 		break;
	 	case MICO_PAL_SQUARE:
			byControl = (BYTE)SETBIT(byControl, 2);	 	
			byControl = (BYTE)SETBIT(byControl, 0);	 	
			break;                           

		case MICO_NTSC_SQUARE:
			byControl = (BYTE)CLRBIT(byControl, 2);	 	
			byControl = (BYTE)SETBIT(byControl, 0);	 	
	 		break;

		default:
			WK_TRACE_WARNING("CBT856::SetBeVideoFormat\tFalscher Parameter\n");
			m_csBT856.Unlock();
			return wOldBeVideoFormat;
	}

	BT856Out(BT856_MODE_CONTROL, byControl);
	m_wBeVideoFormat = wBeVideoFormat;

	m_csBT856.Unlock();

	return wOldBeVideoFormat;
}

/////////////////////////////////////////////////////////////////////////////
WORD CBT856::GetBeVideoFormat()
{
	return m_wBeVideoFormat;
}

//////////////////////////////////////////////////////////////////////////////////////
void CBT856::CBT856Reset()
{
	m_csBT856.Lock();

    BYTE byReset;

	// I2C-Bus Controller reseten...
	byReset = m_Io.ReadFromLatch(m_wIOBase | RESET_OFFSET);

	byReset = (BYTE)CLRBIT(byReset, PARAM_RESET_BT856_BIT);

	m_Io.WriteToLatch(m_wIOBase | RESET_OFFSET, byReset);

	Sleep(10);
	
	byReset = (BYTE)SETBIT(byReset, PARAM_RESET_BT856_BIT);

	m_Io.WriteToLatch(m_wIOBase | RESET_OFFSET, byReset);

	Sleep(10);

	m_csBT856.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT856::Init(const CString &sFileName, const CString &sSection)
{
	m_csBT856.Lock();

	char		szString[SIZE];
	BYTE		byReg, byMinIndex, byMaxIndex;
	int			nValue;
	
	CBT856Reset();
	
	// Überprüft ob der Zugriff auf das MiCoFrontend (BT829) funktioniert
	if (!CheckIt())
		return FALSE;
	else
		WK_TRACE("BT856\tNo error detected\n");
	
	// Initialisierung aller BT856-Register...
 	byMinIndex = (BYTE)GetPrivateProfileInt(sSection, "MinIndex", 0, sFileName);
 	byMaxIndex = (BYTE)GetPrivateProfileInt(sSection, "MaxIndex", 0, sFileName);
        
    // Ini-Datei warscheinlich nicht da
	if ((byMinIndex == 0) && (byMaxIndex == 0))
	{
		WK_TRACE_WARNING("CBT856::Init\tInidatei nicht gefunden ?\n");
		m_csBT856.Unlock();
		return FALSE;
	}
			
	for (byReg = byMinIndex; byReg <= byMaxIndex; byReg++)
	{  
		wsprintf(szString,"Reg%02X", (WORD)byReg);

        nValue = GetPrivateProfileInt("BT856", szString, -1, sFileName);

		if (nValue != -1)
		{
			BT856Out(byReg, (BYTE)nValue);
		}
	}

   	m_csBT856.Unlock();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT856::IsValid()
{
	return ((m_bOk == TRUE) && (CI2C::IsValid()));
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT856::BT856Out(BYTE bySubAddress, BYTE byData)
{
	m_csBT856.Lock();

	BOOL bRet = FALSE;

	if (bySubAddress > BT856_MAX_REG)
	{
	 	WK_TRACE_ERROR("CBT856::CBT856Out\tRegisternummer außerhalb gültiger Grenzen\n");
	 	bRet = FALSE;
	}

	// Da der BT856 nicht readback-fähig ist, werden die Daten extern gespeichert
	m_byBT856Reg[bySubAddress] = byData;

	bRet = WriteToI2C(bySubAddress, byData);

	m_csBT856.Unlock();

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CBT856::BT856In(BYTE bySubAddress)
{
	m_csBT856.Lock();

	BYTE byRet = 0;

	if (bySubAddress > BT856_MAX_REG)
	{
	 	WK_TRACE_ERROR("CBT856::CBT856Out\tRegisternummer außerhalb gültiger Grenzen\n");
		m_csBT856.Unlock();
		return 0xff;
	}

	// Der BT856 ist nicht readback-fähig. BT856_GETID ist als einziges lesbar.
	if (bySubAddress ==	BT856_GETID)
	{
		byRet = ReadFromI2C(bySubAddress);                  
	}
	else
	{
	 	byRet = m_byBT856Reg[bySubAddress];
	}

	m_csBT856.Unlock();

	return byRet;
}

