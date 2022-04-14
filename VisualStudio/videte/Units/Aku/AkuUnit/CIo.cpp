/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: CIo.cpp $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/CIo.cpp $
// CHECKIN:		$Date: 5.08.04 13:54 $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 5.08.04 13:54 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <winioctl.h>
#include "cio.h"
#include "AkuReg.h"
#include "DAAku.h"

CCriticalSection	CIo::m_csIo;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CIo::Input(WORD wPort)
{
	BYTE byVal = 0;

	m_csIo.Lock();

	byVal = (BYTE)DAReadPort8(wPort, FALSE);

	m_csIo.Unlock();

	return byVal;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
WORD CIo::Inputw(WORD wPort)
{
	WORD wVal = 0;

	m_csIo.Lock();

	wVal = (WORD)DAReadPort16(wPort, FALSE);

	m_csIo.Unlock();

	return wVal;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIo::Output(WORD wPort, BYTE byData)
{
	m_csIo.Lock();

	DAWritePort8(wPort, byData, FALSE);

	m_csIo.Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIo::Outputw(WORD wPort, WORD wData) 
{
	m_csIo.Lock();

	DAWritePort8(wPort, (BYTE)wData, FALSE);

	m_csIo.Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CIo::ReadFromLatch(WORD wPort)
{
	m_csIo.Lock();

	BYTE byVal = (BYTE)DAReadPort8(wPort, TRUE);

	m_csIo.Unlock();
	
	return byVal;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIo::WriteToLatch(WORD wPort, BYTE byData)
{
	m_csIo.Lock();

	DAWritePort8(wPort, byData, TRUE);

	m_csIo.Unlock();
}



