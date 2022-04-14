/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CipcInputTashaUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CipcInputTashaUnit.cpp $
// CHECKIN:		$Date: 6.12.01 11:40 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 6.12.01 11:28 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaUnitDlg.h"
#include "CIPCInputTashaUnit.h"
#include "CCodec.h"

CIPCInputTashaUnit::CIPCInputTashaUnit(CTashaUnitDlg* pMainWnd, CCodec *pCodec, const char *shmName)
	: CIPCInput(shmName,FALSE)
{
//	TRACE("CIPCInputTashaUnit::CIPCInputTashaUnit\n");
	m_pCodec			= pCodec;
	m_pMainWnd			= pMainWnd;
	m_wGroupID			= SECID_NO_GROUPID;
	m_bOK				= TRUE;
	m_dwAlarmState		= 0L;					// Alarmstatus
	m_dwEdge			= 0xFFFFFFFF;
	m_dwAck				= 0xFFFFFFFF;

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputTashaUnit::~CIPCInputTashaUnit()
{
//	TRACE("CIPCInputTashaUnit::~CIPCInputTashaUnit\n");
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
//	TRACE("CIPCInputTashaUnit::OnRequestHardware\tGroupID=%i\n", wGroupID);

	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE("CIPCInputTashaUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n", wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnit::OnRequestSetEdge\tWrong wGroupID\n");
		return;
	}

	if (m_pCodec)
		m_pCodec->DoRequestSetAlarmEdge(dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnConfirmSetEdge(DWORD dwAlarmEdgeMask)
{
//	TRACE("CIPCInputTashaUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n", wGroupID, dwEdgeMask);

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
//	TRACE("CIPCInputTashaUnit::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n",wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnit::OnRequestSetFree\tWrong wGroupID\n");
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE("CIPCInputTashaUnit::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnit::OnRequestReset\tWrong wGroupID\n");

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
	// TRACE("CIPCInputTashaUnit::OnRequestAlarmState\tGroupID=%i\n", wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnit::OnRequestAlarmState\tWrong wGroupID\n");
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
//	TRACE("CIPCInputTashaUnit::OnResponseAlarmState   Not Yet \n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnit::OnResponseAlarmState\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::OnResponseInputs(WORD wGroupID)
{
//	TRACE("CIPCInputTashaUnit::OnResponseInputs   Not Yet\n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("IPCInputTashaUnit::OnResponseInputs\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnit::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die Tasha besitzt 16 Alarmeingänge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE("CIPCInputTashaUnit::AlarmStateChanged (0x%x,0x%x)\n", dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}

