/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CipcInputTashaUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CipcInputTashaUnit.cpp $
// CHECKIN:		$Date: 28.10.04 12:53 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 28.10.04 12:24 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaUnitDlg.h"
#include "CIPCInputTashaUnit.h"

CIPCInputTashaUnit::CIPCInputTashaUnit(CTashaUnitDlg* pMainWnd, CCodec *pCodec, LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
//	TRACE(_T("CIPCInputTashaUnit::CIPCInputTashaUnit\n"));
	m_pCodec				= pCodec;
	m_pMainWnd				= pMainWnd;
	m_wGroupID				= SECID_NO_GROUPID;
	m_bOK					= TRUE;
	m_dwAlarmState			= 0L;					// Alarmstatus
	m_dwAdapterSelectState	= 0L;					// CableSelect Status
	m_dwPCKeysState			= 0L;					// PC-Tasten Status
	m_dwEdge				= 0xFFFFFFFF;
	m_dwAck					= 0xFFFFFFFF;

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputTashaUnit::~CIPCInputTashaUnit()
{
//	TRACE(_T("CIPCInputTashaUnit::~CIPCInputTashaUnit\n"));
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputTashaUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnRequestHardware(WORD /*wGroupID*/)
{
//	TRACE(_T("CIPCInputTashaUnit::OnRequestHardware\tGroupID=%i\n"), wGroupID);

	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE(_T("CIPCInputTashaUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n"), wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnit::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	if (m_pCodec)
		m_pCodec->DoRequestSetAlarmEdge(dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnConfirmSetEdge(DWORD dwAlarmEdgeMask)
{
//	TRACE(_T("CIPCInputTashaUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n"), wGroupID, dwEdgeMask);

	m_dwEdge = dwAlarmEdgeMask;
	DoConfirmEdge(m_wGroupID, dwAlarmEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnConfirmGetEdge(DWORD dwAlarmEdgeMask)
{
	DoConfirmEdge(m_wGroupID, dwAlarmEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE(_T("CIPCInputTashaUnit::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnit::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCInputTashaUnit::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

//	if (m_pMainWnd)
//		m_pMainWnd->RequestInputReset();

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputTashaUnit::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnit::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	if (m_pCodec)
		m_pCodec->DoRequestGetAlarmState();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnConfirmAlarmState(DWORD dwAlarmStateMask)
{
	DoConfirmAlarmState(m_wGroupID, dwAlarmStateMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputTashaUnit::OnResponseAlarmState   Not Yet \n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnit::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputTashaUnit::OnResponseInputs   Not Yet\n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("IPCInputTashaUnit::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die Tasha besitzt 16 Alarmeing‰nge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE(_T("CIPCInputTashaUnit::AlarmStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);
	if (this->GetIPCState() == CIPC_STATE_CONNECTED)
		DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);
	else
		TRACE(_T("CIPCOutputTashaUnit::AlarmStateChanged...CIPC not connected\n"));

	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::AdapterSelectStateChanged(WORD wAdapterSelectState)
{
	DWORD dwNewState = m_dwAdapterSelectState;

	// Die Tasha besitzt 16 Alarmeing‰nge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAdapterSelectState; 

	//	WK_TRACE(_T("CIPCInputTashaUnit::AdapterSelectStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);

	// TODO Die Indication muﬂ noch definiert werden
//	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAdapterSelectState);

	m_dwAdapterSelectState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::PCKeysStateChanged(WORD wPCKeysState)
{
	DWORD dwNewState = m_dwPCKeysState;

	// Die Tasha besitzt 16 Alarmeing‰nge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wPCKeysState; 

	//	WK_TRACE(_T("CIPCInputTashaUnit::PCKeysStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwPCKeysState);

	// TODO Die Indication muﬂ noch definiert werden
//	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwPCKeysState);

	m_dwPCKeysState = dwNewState;
}

