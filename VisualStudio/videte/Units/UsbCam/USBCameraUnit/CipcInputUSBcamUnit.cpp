/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCameraUnit
// FILE:		$Workfile: CipcInputUSBcamUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCameraUnit/CipcInputUSBcamUnit.cpp $
// CHECKIN:		$Date: 6.12.04 12:09 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 3.12.04 9:54 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBCameraUnitDlg.h"
#include "CIPCInputUSBcamUnit.h"

CIPCInputUSBcamUnit::CIPCInputUSBcamUnit(CUSBCameraUnitDlg* pMainWnd, LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
//	TRACE("CIPCInputUSBcamUnit::CIPCInputUSBcamUnit\n");
	m_pMainWnd			= pMainWnd;
	m_wGroupID			= SECID_NO_GROUPID;
	m_bOK				   = TRUE;
	m_dwAlarmState		= 0L;					// Alarmstatus
	m_dwEdge			   = 0xFFFFFFFF;
	m_dwAlarmMask     = 0;
	m_bReset = FALSE;

	SetConnectRetryTime(25);
	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputUSBcamUnit::~CIPCInputUSBcamUnit()
{
//	TRACE("CIPCInputUSBcamUnit::~CIPCInputUSBcamUnit\n");
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputUSBcamUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestHardware(WORD /*wGroupID*/)
{
//	TRACE("CIPCInputUSBcamUnit::OnRequestHardware\tGroupID=%i\n", wGroupID);

	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE(_T("CIPCInputUSBcamUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n"), wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputUSBcamUnit::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	m_dwEdge = dwEdgeMask;

	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE(_T("CIPCInputUSBcamUnit::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputUSBcamUnit::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}
	
	m_dwFree = dwOpenMask;

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCInputUSBcamUnit::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputUSBcamUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	m_pMainWnd->InitResetTimer(); 
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputUSBcamUnit::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputUSBcamUnit::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	DWORD dwAlarmMask = GetAllCurrentAlarmStatus();

	DoConfirmAlarmState(m_wGroupID, dwAlarmMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputUSBcamUnit::OnResponseAlarmState   Not Yet \n"));

   if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputUSBcamUnit::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputUSBcamUnit::OnResponseInputs   Not Yet\n"));

   if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("IPCInputUSBCamUnit::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::AlarmStateChanged(WORD wCam, bool bSet)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die USBCam besitzt MAX_CAMERAS Alarmeingänge
	dwNewState = dwNewState & ~((1<<MAX_CAMERAS)-1);
	if (bSet)
	{
		dwNewState |= ((DWORD)1 << wCam);
	}
	else
	{
		dwNewState &= ~((DWORD)1 << wCam);
	}
	//	WK_TRACE(_T("CIPCInputUSBcamUnit::AlarmStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputUSBcamUnit::GetAllCurrentAlarmStatus()
{
	return m_dwAlarmMask;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCInputUSBcamUnit::IsAlarmActive(int nCam)
{
	if (IsBetween(nCam, 0, MAX_CAMERAS-1))
	{
		return (m_dwFree & (1 << nCam)) ? TRUE : FALSE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestDisconnect()
{
	m_wGroupID = SECID_NO_GROUPID;
	m_bReset = false;
}
//////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::ConfirmReset()
{
	m_bReset = false;
	DoConfirmReset(m_wGroupID);
}
