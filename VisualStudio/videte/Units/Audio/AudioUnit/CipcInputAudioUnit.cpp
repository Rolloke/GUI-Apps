/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AudioUnit
// FILE:		$Workfile: CipcInputAudioUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Audio/AudioUnit/CipcInputAudioUnit.cpp $
// CHECKIN:		$Date: 3.12.04 15:00 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 2.12.04 15:32 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CipcInputAudioUnit.h"
#include "AudioUnitDlg.h"

#ifdef _TEST_TRACES
   #define TEST_TRACE WK_TRACE
#else
   #define TEST_TRACE //
#endif


CIPCInputAudioUnit::CIPCInputAudioUnit(CAudioUnitDlg* pDlg, LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
	TEST_TRACE(_T("CIPCInputAudioUnit::CIPCInputAudioUnit\n"));

	m_pDlg = pDlg;
	m_wGroupID		= SECID_NO_GROUPID;

	m_dwEdgeMask   = 0;
	m_dwAlarm      = 0;
	m_dwFree       = 0;
	m_bReset	   = FALSE;
	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputAudioUnit::~CIPCInputAudioUnit()
{
	TEST_TRACE(_T("CIPCInputAudioUnit::~CIPCInputAudioUnit\n"));

	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::OnRequestSetGroupID(WORD wGroupID)
{
	TEST_TRACE(_T("CIPCInputAudioUnit::OnRequestSetGroupID\tGroupID=%x\n"), wGroupID);
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::OnRequestDisconnect()
{
	TEST_TRACE(_T("CIPCInputAudioUnit::OnRequestDisconnect(%x)\n"), m_wGroupID);
	m_wGroupID = SECID_NO_GROUPID;
	m_bReset = false;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::OnRequestHardware(WORD wGroupID)
{
	TEST_TRACE(_T("CIPCInputAudioUnit::OnRequestHardware\tGroupID=%x\n"), wGroupID);

	int iErrorCode = 0;
	if (m_pDlg->m_sCapture.IsEmpty())
	{
		iErrorCode = CIPC_ERROR_NO_HARDWARE;
	}
		
	DoConfirmHardware(m_wGroupID, iErrorCode);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
	TEST_TRACE(_T("CIPCInputAudioUnit::OnRequestSetEdge\tGroupID=%x\t0x%08x\n"), wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputAudioUnit::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	m_dwEdgeMask = dwEdgeMask;
	
	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
	WK_TRACE(_T("CIPCInputAudioUnit::OnRequestSetFree\tGroupID=%x\tOpenMask=0x%08x\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputAudioUnit::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}
	if (m_dwFree != dwOpenMask)
	{
/*
		if (IsAudioDetectorActive())
		{
			SetAudioAlarm(FALSE); 
		}
*/
		m_dwFree = dwOpenMask;
		m_pDlg->UpdateAudioDetector(IsAudioDetectorActive());
	}
	
	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCInputAudioUnit::OnRequestReset\tGroupID=%x\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputAudioUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}
	
	m_bReset = true;
	m_pDlg->InitResetTimer(); 
}
//////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::ConfirmReset()
{
	m_bReset = false;
	DoConfirmReset(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::OnRequestAlarmState(WORD wGroupID)
{
	TEST_TRACE(_T("CIPCInputAudioUnit::OnRequestAlarmState\tGroupID=%x\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputAudioUnit::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}
	DWORD dwMask = m_dwAlarm;
	
	dwMask ^= m_dwEdgeMask;

	DoConfirmAlarmState(m_wGroupID, dwMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::OnResponseAlarmState(WORD wGroupID)
{
	TEST_TRACE(_T("CIPCInputAudioUnit::OnResponseAlarmState Not Yet \n"));

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputAudioUnit::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::OnResponseInputs(WORD wGroupID)
{
	TEST_TRACE(_T("CIPCInputAudioUnit::OnResponseInputs Not Yet\n"));

   if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputAudioUnit::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAudioUnit::SetAudioAlarm(BOOL bAlarm)
{
	if (m_wGroupID	!= SECID_NO_GROUPID)
	{
		DWORD dwMask = bAlarm;
		DWORD dwCangeMask = dwMask ^ m_dwAlarm;
		m_dwAlarm = dwMask;
		if (m_dwFree & 0x00000001)
		{
			dwMask ^= m_dwEdgeMask;
		}
		DoIndicateAlarmState(m_wGroupID, dwMask, dwCangeMask);
	}
}
