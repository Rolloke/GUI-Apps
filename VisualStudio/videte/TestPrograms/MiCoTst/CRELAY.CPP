/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CRELAY.CPP $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CRELAY.CPP $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "micotst.h"
#include "conio.h"
#include "MiCoReg.h"
#include "MiCoDefs.h"
#include "CIo.h"
#include "CRelay.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::CRelay
CRelay::CRelay(WORD wIOBase, WORD wExtCard, BOOL bIgnoreRelay4)
{
	m_wExtCard = wExtCard;
	m_wIOBase = wIOBase;
	m_bIgnoreRelay4 = bIgnoreRelay4;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::~CRelay
CRelay::~CRelay()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::SetRelay
void CRelay::SetRelay(BYTE byRelais)
{
	if (m_wExtCard == MICO_EXTCARD0)
	{
		if (m_bIgnoreRelay4)
		{
			BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_OFFSET);
			byReg &= 0xf8;
			byReg |= (byRelais & 0x07);
			m_IoAccess.Output(m_wIOBase + RELAY_OFFSET, byReg);
		}
		else
		{
			BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_OFFSET);
			byReg &= 0xf0;
			byReg |= (byRelais & 0x0f);
			m_IoAccess.Output(m_wIOBase + RELAY_OFFSET, byReg);
		}
	}
	else
	{
		BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_COVI_OFFSET);
		byReg &= 0xf0;
		byReg |= (byRelais & 0x0f);
		m_IoAccess.Output(m_wIOBase + RELAY_COVI_OFFSET, byReg);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::GetRelay
BYTE CRelay::GetRelay()
{
	if (m_wExtCard == MICO_EXTCARD0)
	{
		BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_OFFSET);
		return (byReg & 0x0f);
	}
	else
	{
		BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_COVI_OFFSET);
		return (byReg & 0x0f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::BeeperOn()
void CRelay::BeeperOn()
{
	BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_OFFSET);
	byReg = CLRBIT(byReg, RELAY3_BIT);
	m_IoAccess.Output(m_wIOBase + RELAY_OFFSET, byReg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::BeeperOff()
void CRelay::BeeperOff()
{
	BYTE byReg = m_IoAccess.Input(m_wIOBase + RELAY_OFFSET);
	byReg = SETBIT(byReg, RELAY3_BIT);
	m_IoAccess.Output(m_wIOBase + RELAY_OFFSET, byReg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::CheckIt()
void CRelay::CheckIt(WORD wNr)
{
	WK_TRACE(TRUE, "\tTesting Relay %u\n", wNr+1);
	
	for (WORD wI = 0; wI < 3; wI++)
	{
		SetRelay(1<<wNr);
		Sleep(100);
		SetRelay(0x00);
		Sleep(100);
	}	
}

