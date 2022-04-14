/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CIo.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CIo.cpp $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cio.h"
#include "micoreg.h"

CCriticalSection			CIo::m_csIo;
BYTE						CIo::byLatchBuffer[LATCH_SIZE];
int							CIo::m_nIoInstCnt = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CIo::CIo()
{
	m_csIo.Lock();
	// Nur beim 1. Objekt das static-Array initialisieren
	if (m_nIoInstCnt == 0)
	{
		// LatchBuffer auf 0 initialisieren
		memset (&byLatchBuffer[0], 0x00, LATCH_SIZE);
	}
	
	// Z‰hlt die CIo-Objekte
	m_nIoInstCnt++;
	m_csIo.Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CIo::~CIo()
{
	m_csIo.Lock();
	m_nIoInstCnt--;

	m_csIo.Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CIo::Input(WORD wPort)
{
	m_csIo.Lock();

	if (wPort == 0)
	{
		m_csIo.Unlock();
		return 0;
	}
	
	BYTE byVal = (BYTE)_inp(wPort);

	m_csIo.Unlock();
	return byVal;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
WORD CIo::Inputw(WORD wPort)
{
	m_csIo.Lock();

	if (wPort == 0)
	{
		m_csIo.Unlock();
		return 0;
	}
	
	WORD wVal = (WORD)_inpw(wPort);

	m_csIo.Unlock();

	return wVal;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIo::Output(WORD wPort, BYTE byDatabyte)
{
	m_csIo.Lock();

	if (wPort != 0){
		_outp(wPort, byDatabyte);
	}

	m_csIo.Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIo::Outputw(WORD wPort, WORD wDataword) 
{
	m_csIo.Lock();

	if (wPort != 0){
		_outpw(wPort, wDataword);
	}

	m_csIo.Unlock();

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CIo::ReadFromLatch(WORD wPort)
{
	m_csIo.Lock();
	
	BYTE bRet = 0;
	if (wPort > LATCH_SIZE)
	{
		WK_TRACE(TRUE, "CIo::ReadFromLatch\tPortAdresse auﬂerhalb des Latchbuffers\n");
		bRet = 0xff;
	}

	bRet = byLatchBuffer[wPort]; 

	m_csIo.Unlock();
	
	return bRet;
} 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIo::WriteToLatch(WORD wPort, BYTE byDatabyte)
{
	
	m_csIo.Lock();

	if (wPort > LATCH_SIZE)
	{
		WK_TRACE(TRUE, "CIo::WriteToLatch\tPortAdresse auﬂerhalb des Latchbuffers\n");
		m_csIo.Unlock();
		return;
	}

	if (wPort != 0){
		Output(wPort, byDatabyte);
	}

	byLatchBuffer[wPort] = byDatabyte;	
	m_csIo.Unlock();
}