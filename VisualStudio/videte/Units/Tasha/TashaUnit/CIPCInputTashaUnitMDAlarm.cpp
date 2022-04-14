/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CIPCInputTashaUnitMDAlarm.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CIPCInputTashaUnitMDAlarm.cpp $
// CHECKIN:		$Date: 24.01.05 14:46 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 24.01.05 13:48 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaUnitDlg.h"
#include "CIPCInputTashaUnit.h"
#include "CIPCInputTashaUnitMDAlarm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCInputTashaUnitMDAlarm::CIPCInputTashaUnitMDAlarm(CTashaUnitDlg* pMainWnd, CCodec *pCodec, LPCTSTR shmName)
	: CIPCInputTashaUnit(pMainWnd, pCodec, shmName)
{
//	TRACE(_T("CIPCInputTashaUnitMDAlarm::CIPCInputTashaUnitMDAlarm\n"));
	m_dwAlarmState	= 0;
	m_dwEdgeMask	= 0;
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputTashaUnitMDAlarm::~CIPCInputTashaUnitMDAlarm()
{
//	TRACE(_T("CIPCInputTashaUnitMDAlarm::~CIPCInputTashaUnitMDAlarm\n"));
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputTashaUnitMDAlarm::IsValid()
{
	return CIPCInputTashaUnit::IsValid();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
	TRACE(_T("CIPCInputTashaUnitMDAlarm::OnRequestSetEdge\tGroupID=%i\t0x%lx\n"), wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnitMDAlarm::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	m_dwEdgeMask = dwEdgeMask;

	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE(_T("CIPCInputTashaUnitMDAlarm::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnitMDAlarm::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCInputTashaUnitMDAlarm::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnitMDAlarm::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
	}
	else
	{	
		m_dwAlarmState = 0;
		DoConfirmReset(m_wGroupID);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputTashaUnitMDAlarm::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnitMDAlarm::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	DoConfirmAlarmState(m_wGroupID, m_dwAlarmState ^ m_dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputTashaUnitMDAlarm::OnResponseAlarmState   Not Yet \n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnitMDAlarm::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputTashaUnitMDAlarm::OnResponseInputs   Not Yet\n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputTashaUnitMDAlarm::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die Tasha besitzt 16 MD-Alarmeingänge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE(_T("CIPCInputTashaUnitMDAlarm::AlarmStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);

	if (this->GetIPCState() == CIPC_STATE_CONNECTED)
		DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);
	else
		TRACE(_T("CIPCInputTashaUnitMDAlarm::AlarmStateChanged...CIPC not connected\n"));

	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnReceivedMotionAlarm(DATA_PACKET* pPacket, BOOL bState)
{
	if (!pPacket)
		return;

	DWORD dwChangeMask = 1<<pPacket->wSource;
	if (bState)
	{
		m_dwAlarmState = 1<<pPacket->wSource;
	}
	else
	{
		m_dwAlarmState = 0;
	}

	CIPCFields fields;
	if (bState)
	{
		CString sValueX, sValueY;
		CString sDBDNameX, sDBDNameY;

		for (int nI = 0; nI < 5; nI++)
		{
			if ((pPacket->Point[nI].cx != 0) && (pPacket->Point[nI].cy != 0))
			{
				sValueX.Format(_T("%04hx"), pPacket->Point[nI].cx);
				sValueY.Format(_T("%04hx"), pPacket->Point[nI].cy);
				
				// Das 1. Element erhält aus kompatibilitätsgründen keine Nummer
				// Der Name ist (DB)atabase (D)ata
				// DBP ist das idip Format, für DTS wird in
				// DVProcess das DBP_ durch DVD_ (D)igital (V)ideo (D)ata ersetzt
				if (nI == 0)
				{
					sDBDNameX = _T("DBD_MD_X");
					sDBDNameY = _T("DBD_MD_Y");
				}
				else // > 1
				{
					sDBDNameX.Format(_T("DBD_MD_X%01d"), nI+1);
					sDBDNameY.Format(_T("DBD_MD_Y%01d"), nI+1);
				}

				fields.Add(new CIPCField(sDBDNameX, sValueX, 'C'));
				fields.Add(new CIPCField(sDBDNameY, sValueY, 'C'));
			}
		}
	}
	if (this->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		DoIndicateAlarmFieldsState(m_wGroupID,
								m_dwAlarmState,	// 1 high, 0 low
								dwChangeMask,	// 1 changed, 0 unchanged
								fields);
	}
	else
		TRACE(_T("CIPCInputTashaUnitMDAlarm::OnReceivedMotionAlarm...CIPC not connected\n"));
}

