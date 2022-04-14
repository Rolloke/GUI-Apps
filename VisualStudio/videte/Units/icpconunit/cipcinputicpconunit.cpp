/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ICPCONUnit
// FILE:		$Workfile: cipcinputicpconunit.cpp $
// ARCHIVE:		$Archive: /Project/Units/ICPCONUnit/cipcinputicpconunit.cpp $
// CHECKIN:		$Date: 27.02.04 13:42 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 27.02.04 13:39 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIPCInputICPCONUnit.h"

#ifdef _TEST_TRACES
   #define TEST_TRACE WK_TRACE
#else
   #define TEST_TRACE //
#endif

CIPCInputICPCONUnit::CIPCInputICPCONUnit(CWnd* pMainWnd, LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::CIPCInputICPCONUnit\n"));
	m_pMainWnd		= pMainWnd;
	m_wGroupID		= SECID_NO_GROUPID;
	m_dwEdge		   = 0;
	m_dwMask			= 0;
	m_dwState		= 0;

	SetConnectRetryTime(25);
	StartThread();
	WK_TRACE(_T("CIPCInput(%s)\n"), shmName);
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputICPCONUnit::~CIPCInputICPCONUnit()
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::~CIPCInputICPCONUnit\n"));

	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputICPCONUnit::IsValid()
{
	return (m_wGroupID != SECID_NO_GROUPID) ? TRUE : FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::OnRequestDisconnect()
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::OnRequestDisconnect(%x)\n"), m_wGroupID);
	m_wGroupID = SECID_NO_GROUPID;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::OnRequestHardware(WORD wGroupID)
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::OnRequestHardware\tGroupID=%x\n"), wGroupID);

	int iErrorCode = 0;
	DoConfirmHardware(m_wGroupID, iErrorCode);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::OnRequestSetEdge\tGroupID=%x\t0x%08x\n"), wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputICPCONUnit::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	m_dwEdge = dwEdgeMask;

	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::OnRequestSetFree\tGroupID=%x\tOpenMask=0x%08x\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputICPCONUnit::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}
	m_dwMask = dwOpenMask;

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::OnRequestReset(WORD wGroupID)	
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::OnRequestReset\tGroupID=%x\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputICPCONUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pMainWnd)
	{
		m_pMainWnd->PostMessage(WM_USER, USER_MSG_REQUEST_RESET, NULL);
	}
	
	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::OnRequestAlarmState(WORD wGroupID)
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::OnRequestAlarmState\tGroupID=%x\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputICPCONUnit::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	DWORD dwAlarmMask = ((m_dwState ^ m_dwEdge) & m_dwMask);
	DoConfirmAlarmState(m_wGroupID, dwAlarmMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::OnResponseAlarmState(WORD wGroupID)
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::OnResponseAlarmState   Not Yet \n"));

   if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputICPCONUnit::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::OnResponseInputs(WORD wGroupID)
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::OnResponseInputs\n"));

   if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputICPCONUnit::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputICPCONUnit::SetAlarm(int nBit, bool bSet)
{
	TEST_TRACE(_T("CIPCInputICPCONUnit::SetAlarm   Not Yet\n"));
	DWORD dwChangeMask = 1 << nBit, dwInputMask = 0;
	if (bSet)
	{
		m_dwState |=  dwChangeMask;
	}
	else
	{
		m_dwState &= ~dwChangeMask;
	}

	if (m_wGroupID	!= SECID_NO_GROUPID)
	{
		if (m_dwMask & dwChangeMask)
		{
			dwInputMask = (m_dwState ^ m_dwEdge);
			DoIndicateAlarmState(m_wGroupID, dwInputMask, dwChangeMask);
		}
	}
}

int CIPCInputICPCONUnit::GetNoOfInputs()
{
	int i, nInputs = 0;
	DWORD dwMask = 1;
	for (i=0; i<32; i++, dwMask<<=1)
	{
		if (m_dwMask & dwMask) nInputs++;
	}
	return nInputs;
}

bool CIPCInputICPCONUnit::IsInputActive(int nInput)
{
	return (m_dwMask & (1<<nInput)) ? true : false;
}

WORD CIPCInputICPCONUnit::GetGroupID()
{
	return m_wGroupID;
}

BOOL CIPCInputICPCONUnit::GetState(int nInput)
{
	return (m_dwState & (1<<nInput)) ? true : false;
}
