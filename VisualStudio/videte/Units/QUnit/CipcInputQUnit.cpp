/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CipcInputQUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CipcInputQUnit.cpp $
// CHECKIN:		$Date: 1.12.05 16:09 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 1.12.05 14:53 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QUnitDlg.h"
#include "CIPCInputQUnit.h"

CIPCInputQUnit::CIPCInputQUnit(CQUnitDlg* pMainWnd, CUDP *pUDP, LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
//	TRACE(_T("CIPCInputQUnit::CIPCInputQUnit\n"));
	m_pUDP				= pUDP;
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
CIPCInputQUnit::~CIPCInputQUnit()
{
//	TRACE(_T("CIPCInputQUnit::~CIPCInputQUnit\n"));
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputQUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnit::OnRequestHardware(WORD /*wGroupID*/)
{
//	TRACE(_T("CIPCInputQUnit::OnRequestHardware\tGroupID=%i\n"), wGroupID);

	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE(_T("CIPCInputQUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n"), wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnit::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	if (m_pUDP)
		m_pUDP->DoRequestSetAlarmEdge((WORD)dwEdgeMask);
	
	m_dwEdge = dwEdgeMask;
	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE(_T("CIPCInputQUnit::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnit::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCInputQUnit::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	// TODO: Wie soll der Serverreset behandelt werden?
//	if (m_pMainWnd)
//		m_pMainWnd->RequestInputReset();

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnit::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputQUnit::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnit::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	DWORD dwAlarmMask = 0L;

	// Derzeit auf 32 Alarme begrenzt.
	if (m_pUDP)
		dwAlarmMask = (DWORD)m_pUDP->GetCurrentAlarmState();

	DoConfirmAlarmState(m_wGroupID, dwAlarmMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnit::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputQUnit::OnResponseAlarmState   Not Yet \n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnit::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnit::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputQUnit::OnResponseInputs   Not Yet\n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("IPCInputQUnit::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnit::AlarmStateChanged(DWORD dwAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	dwNewState = dwAlarmState;
	
	WK_TRACE(_T("CIPCInputQUnit::AlarmStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputQUnit::GetAlarmEdge()
{
	DWORD dwEdgeMask = 0L;

	if (m_pUDP)
		dwEdgeMask = m_pUDP->DoRequestGetAlarmEdge();

	return dwEdgeMask;
}

