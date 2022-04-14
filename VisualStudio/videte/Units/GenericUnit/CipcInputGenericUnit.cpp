/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: CipcInputGenericUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CipcInputGenericUnit.cpp $
// CHECKIN:		$Date: 19.10.00 8:46 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 19.10.00 8:07 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenericUnitDlg.h"
#include "CIPCInputGenericUnit.h"

CIPCInputGenericUnit::CIPCInputGenericUnit(CGenericUnitDlg* pMainWnd, CCodec *pCodec, const char *shmName)
	: CIPCInput(shmName,FALSE)
{
//	TRACE("CIPCInputGenericUnit::CIPCInputGenericUnit\n");
	m_pCodec				= pCodec;
	m_pMainWnd			= pMainWnd;
	m_wGroupID			= SECID_NO_GROUPID;
	m_bOK				= TRUE;
	m_dwAlarmState		= 0L;					// Alarmstatus
	m_dwEdge			= 0xFFFFFFFF;
	m_dwAck				= 0xFFFFFFFF;

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputGenericUnit::~CIPCInputGenericUnit()
{
//	TRACE("CIPCInputGenericUnit::~CIPCInputGenericUnit\n");
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputGenericUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::OnRequestHardware(WORD /*wGroupID*/)
{
//	TRACE("CIPCInputGenericUnit::OnRequestHardware\tGroupID=%i\n", wGroupID);

	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE("CIPCInputGenericUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n", wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnit::OnRequestSetEdge\tWrong wGroupID\n");
		return;
	}

	SetAllAlarmEdge(dwEdgeMask);

	m_dwEdge = dwEdgeMask;
	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE("CIPCInputGenericUnit::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n",wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnit::OnRequestSetFree\tWrong wGroupID\n");
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE("CIPCInputGenericUnit::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnit::OnRequestReset\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pMainWnd)
		m_pMainWnd->RequestInputReset();

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE("CIPCInputGenericUnit::OnRequestAlarmState\tGroupID=%i\n", wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnit::OnRequestAlarmState\tWrong wGroupID\n");
		return;
	}

	DWORD dwAlarmMask = GetAllCurrentAlarmStatus();

	DoConfirmAlarmState(m_wGroupID, dwAlarmMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE("CIPCInputGenericUnit::OnResponseAlarmState   Not Yet \n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnit::OnResponseAlarmState\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::OnResponseInputs(WORD wGroupID)
{
//	TRACE("CIPCInputGenericUnit::OnResponseInputs   Not Yet\n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("IPCInputGenericUnit::OnResponseInputs\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die Generic besitzt 16 Alarmeingänge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE("CIPCInputGenericUnit::AlarmStateChanged (0x%x,0x%x)\n", dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputGenericUnit::GetAllCurrentAlarmStatus()
{
	DWORD dwAlarmMask = 0L;

	if (m_pCodec)
		dwAlarmMask = (DWORD)m_pCodec->GetCurrentAlarmState();

	return dwAlarmMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnit::SetAllAlarmEdge(DWORD dwEdgeMask)
{
	if (m_pCodec)
		m_pCodec->SetAlarmEdge((WORD)dwEdgeMask);

	m_dwEdge = dwEdgeMask;
	
	return;
}


//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputGenericUnit::GetAllAlarmEdge()
{
	DWORD dwEdgeMask = 0L;

	if (m_pCodec)
		dwEdgeMask = (DWORD)m_pCodec->GetAlarmEdge();
	return dwEdgeMask;
}

