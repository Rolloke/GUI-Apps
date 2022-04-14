/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Crelay.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/Crelay.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CRelay.h"
#include "CIoMemory.h"
#include "CAlarm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CRelay::CRelay(DWORD dwIOBase)
{
	m_dwIOBase = dwIOBase;
	m_pCIo = new CIoMemory(dwIOBase);
}

//////////////////////////////////////////////////////////////////////////////////////
CRelay::~CRelay()
{
	WK_DELETE(m_pCIo);
}

//////////////////////////////////////////////////////////////////////
BOOL CRelay::IsValid() const
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRelay::SetRelay(BOOL bSet) const
{
	if (IsValid())
	{
		DWORD dwVal = m_pCIo->Inputdw(BT878_GPIO_DATA_CTL);
		if (bSet)
			dwVal = SETBIT(dwVal, BT878_GPIO_RELAY_BIT);
		else
			dwVal = CLRBIT(dwVal, BT878_GPIO_RELAY_BIT);

		m_pCIo->Outputdw(BT878_GPIO_DATA_CTL, dwVal);
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CRelay::GetRelay() const
{	
	if (!IsValid())
		return FALSE;

	return TSTBIT(m_pCIo->Inputdw(BT878_GPIO_DATA_CTL), BT878_GPIO_RELAY_BIT);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRelay::BeeperOn() const
{
	SetRelay(FALSE);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRelay::BeeperOff() const
{
	SetRelay(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRelay::Diagnostic(DWORD dwBoardVariations) const
{
	BOOL bResult = FALSE;
	DWORD dwSleep = 250;

	ML_TRACE_DIAGNOSTIC(_T("\nTesting Relays (Acoustical).\n"));
	for (int nJ = 0; nJ <= 4; nJ++)
	{
		for (int nI = 0; nI < 4; nI++)
		{
			ML_TRACE_DIAGNOSTIC(_T("@+."));
			SetRelay(FALSE);
			Sleep(dwSleep);
			SetRelay(TRUE);
			Sleep(dwSleep);
		}

		SetRelay(FALSE);
		Sleep(dwSleep/2);
		SetRelay(TRUE);
		Sleep(dwSleep/2);
		SetRelay(FALSE);
		Sleep(dwSleep/2);

		for (nI = 0; nI < 4; nI++)
		{
			ML_TRACE_DIAGNOSTIC(_T("@+."));
			SetRelay(TRUE);
			Sleep(dwSleep);
			SetRelay(FALSE);
			Sleep(dwSleep);
		}

		SetRelay(TRUE);
		Sleep(dwSleep/2);
		SetRelay(FALSE);
		Sleep(dwSleep/2);
		SetRelay(TRUE);
		Sleep(dwSleep/2);
		
		dwSleep /= 2;
	}

	if (AfxMessageBox(_T("Arbeitet das Relay ordnungsgemäß?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		bResult = TRUE;
		ML_TRACE_DIAGNOSTIC(_T("@-Testing Relays: passed\n"));
	}
	else
	{
		bResult = FALSE;
		ML_TRACE_DIAGNOSTIC(_T("@-Testing Relays: failed\n"));
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRelay::Diagnostic2(DWORD dwBoardVariations) const
{
	BOOL bErr = FALSE;
	WORD wAlarmState = 0;

	CAlarm Alarm(m_dwIOBase, dwBoardVariations);

	ML_TRACE_DIAGNOSTIC(_T("\nTesting Relays.\n"));
	SetRelay(FALSE);
	Alarm.ClearLatch();
	Sleep(100);

	for (int nJ = 0; nJ < 12; nJ++)
	{
		ML_TRACE_DIAGNOSTIC(_T("@+."));
		
		Alarm.ClearLatch();
		SetRelay(TRUE);
		Sleep(100);

		WORD wAlarm = Alarm.GetState();
		if (!TSTBIT(wAlarm, 0))
			bErr |= TRUE;
		
		Alarm.ClearLatch();
		SetRelay(FALSE);
		Sleep(100);

		wAlarm = Alarm.GetState();
		if (TSTBIT(wAlarm, 0))
			bErr |= TRUE;
	}
	
	if (!bErr)
		ML_TRACE_DIAGNOSTIC(_T("@-Testing Relays: passed\n"));
	else
		ML_TRACE_DIAGNOSTIC(_T("@-Testing Relays: failed\n"));

	return !bErr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRelay::ClearLatch() const
{
	// Lowpuls löscht das Latch wieder
	DWORD dwDPIOState = m_pCIo->Inputdw(BT878_GPIO_DATA_CTL);
	
	dwDPIOState = CLRBIT(dwDPIOState, BT878_GPIO_CLEAR_LATCH_BIT);
	m_pCIo->Outputdw(BT878_GPIO_DATA_CTL, dwDPIOState);
	
	Sleep(1);
	
	dwDPIOState = SETBIT(dwDPIOState, BT878_GPIO_CLEAR_LATCH_BIT);
	m_pCIo->Outputdw(BT878_GPIO_DATA_CTL, dwDPIOState);

}
