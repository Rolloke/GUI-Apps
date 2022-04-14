/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: CipcInputSavicUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/CipcInputSavicUnit.cpp $
// CHECKIN:		$Date: 20.06.03 13:15 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 20.06.03 12:22 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SavicUnitDlg.h"
#include "CIPCInputSavicUnit.h"

CIPCInputSaVicUnit::CIPCInputSaVicUnit(CSaVicUnitDlg* pMainWnd, CCodec *pCodec, LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
//	TRACE(_T("CIPCInputSaVicUnit::CIPCInputSaVicUnit\n"));
	m_pCodec				= pCodec;
	m_pMainWnd			= pMainWnd;
	m_wGroupID			= SECID_NO_GROUPID;
	m_bOK				= TRUE;
	m_dwAlarmState		= 0L;					// Alarmstatus
	m_dwEdge			= 0xFFFFFFFF;
	m_dwAck				= 0xFFFFFFFF;

	SetConnectRetryTime(25);
	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputSaVicUnit::~CIPCInputSaVicUnit()
{
//	TRACE(_T("CIPCInputSaVicUnit::~CIPCInputSaVicUnit\n"));
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputSaVicUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::OnRequestHardware(WORD /*wGroupID*/)
{
//	TRACE(_T("CIPCInputSaVicUnit::OnRequestHardware\tGroupID=%i\n"), wGroupID);

	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE(_T("CIPCInputSaVicUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n"), wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnit::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	SetAllAlarmEdge(dwEdgeMask);

	m_dwEdge = dwEdgeMask;
	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE(_T("CIPCInputSaVicUnit::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnit::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCInputSaVicUnit::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pMainWnd)
		m_pMainWnd->RequestInputReset();

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputSaVicUnit::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnit::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	DWORD dwAlarmMask = GetAllCurrentAlarmStatus();

	DoConfirmAlarmState(m_wGroupID, dwAlarmMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputSaVicUnit::OnResponseAlarmState   Not Yet \n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnit::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputSaVicUnit::OnResponseInputs   Not Yet\n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("IPCInputSaVicUnit::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die SaVic besitzt 16 Alarmeingänge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE(_T("CIPCInputSaVicUnit::AlarmStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputSaVicUnit::GetAllCurrentAlarmStatus()
{
	DWORD dwAlarmMask = 0L;

	if (m_pCodec)
		dwAlarmMask = (DWORD)m_pCodec->GetCurrentAlarmState();

	return dwAlarmMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnit::SetAllAlarmEdge(DWORD dwEdgeMask)
{
	if (m_pCodec)
		m_pCodec->SetAlarmEdge((WORD)dwEdgeMask);
	
	m_dwEdge = dwEdgeMask;
	
	return;
}


//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputSaVicUnit::GetAllAlarmEdge()
{
	DWORD dwEdgeMask = 0L;

	if (m_pCodec)
		dwEdgeMask = (DWORD)m_pCodec->GetAlarmEdge();

	return dwEdgeMask;
}

