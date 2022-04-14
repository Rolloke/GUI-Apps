/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: crelay.cpp $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/crelay.cpp $
// CHECKIN:		$Date: 27.02.04 9:53 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 27.02.04 9:02 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "conio.h"
#include "AkuReg.h"
#include "CAku.h"
#include "CIo.h"
#include "CRelay.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::CRelay
CRelay::CRelay(WORD wIOBase, WORD wAkuCard)
{
	m_wAkuCard = wAkuCard;
	m_wIOBase = wIOBase;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::~CRelay
CRelay::~CRelay()
{
	 WK_TRACE( _T("CRelay::~CRelay()\n"));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::SetRelay
void CRelay::SetRelay(BYTE byRelais)
{
	BYTE byReg = m_IoAccess.ReadFromLatch(m_wIOBase + RELAY_OFFSET);
	byReg &= 0x00;
	byReg |= (byRelais & 0xff);
	m_IoAccess.WriteToLatch(m_wIOBase + RELAY_OFFSET, byReg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRelay::GetRelay
BYTE CRelay::GetRelay()
{
	return m_IoAccess.ReadFromLatch(m_wIOBase + RELAY_OFFSET);
}

