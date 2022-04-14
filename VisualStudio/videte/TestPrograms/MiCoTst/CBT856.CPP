/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CBT856.CPP $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CBT856.CPP $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "conio.h"
#include "MiCoDefs.h"
#include "MiCoReg.h"
#include "CBT856.h"
#include "micoini.h"

/////////////////////////////////////////////////////////////////////////////
CBT856::CBT856(WORD wIOBase) : CI2C(wIOBase, BT856_SLAVE_ADR) 
{
	m_csBT856.Lock();

	m_bOk = FALSE;

	// Basisklasse korrekt angelegt ?
	if (!CI2C::IsValid())
	{
		m_csBT856.Unlock();
		return;
	}

	m_wIOBase		 = wIOBase;
	m_wBeVideoFormat = MICO_PAL_CCIR;

	// Registerbuffer auf 0 initialisieren
	for (int nReg = 0; nReg < BT856_MAX_REG; nReg++){
 		m_byBT856Reg[nReg] = 0;
	}

	CBT856Reset();

	m_bOk = TRUE;

	m_csBT856.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
CBT856::~CBT856()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBT856::CheckIt()
{
	m_csBT856.Lock();

	BYTE byID = BT856In(BT856_GETID);

	if (byID != 0x60)
	{
		WK_TRACE(TRUE, "\tBT856\t\twrong ID (0x%02x)\n", (WORD)byID);
		m_csBT856.Unlock();
		return FALSE;
	}
	else
		WK_TRACE(TRUE, "\tBT856\t\tID-Code  test passed (ID=0x%02x)\n", (WORD)byID);
		
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
//			WK_TRACE(TRUE, "CBT856::SetBeVideoFormat\tPAL CCIR\n");
			byControl = (BYTE)SETBIT(byControl, 2);	 	
			byControl = (BYTE)CLRBIT(byControl, 0);	 	
	
			break;                           
		case MICO_NTSC_CCIR:
//			WK_TRACE(TRUE, "CBT856::SetBeVideoFormat\tNTSC CCIR\n");
			byControl = (BYTE)CLRBIT(byControl, 2);	 	
			byControl = (BYTE)CLRBIT(byControl, 0);	 	
	 		break;
	 	case MICO_PAL_SQUARE:
//			WK_TRACE(TRUE, "CBT856::SetBeVideoFormat\tPAL Square\n");
			byControl = (BYTE)SETBIT(byControl, 2);	 	
			byControl = (BYTE)SETBIT(byControl, 0);	 	
	
			break;                           
		case MICO_NTSC_SQUARE:
//			WK_TRACE(TRUE, "CBT856::SetBeVideoFormat\tNTSC Square\n");
			byControl = (BYTE)CLRBIT(byControl, 2);	 	
			byControl = (BYTE)SETBIT(byControl, 0);	 	
	 		break;
	 	default:
			WK_TRACE(TRUE, "CBT856::SetBeVideoFormat\tFalscher Parameter\n");
			m_csBT856.Unlock();
			return wOldBeVideoFormat;
	}

	BT856Out(BT856_MODE_CONTROL, byControl);
	m_wBeVideoFormat = wBeVideoFormat;

	m_csBT856.Unlock();

	return wOldBeVideoFormat;
}

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
BOOL CBT856::Init()
{
	m_csBT856.Lock();

	BYTE		byReg;
	int			nValue;
	WORD		wI;
		
	for (wI = 0; wI < BT856REGCOUNT; wI++)
	{  
       	byReg  = HIBYTE(BT856[wI]);                              
        nValue = LOBYTE(BT856[wI]);

		BT856Out(byReg, (BYTE)nValue);
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
	 	WK_TRACE(TRUE, "CBT856::CBT856Out\tRegisternummer außerhalb gültiger Grenzen\n");
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
	 	WK_TRACE(TRUE, "CBT856::CBT856Out\tRegisternummer außerhalb gültiger Grenzen\n");
		m_csBT856.Unlock();
		return 0xff;
	}

	// Der BT856 ist nicht readback-fähig. BT856_GETID ist als einzieges lesbar.
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

