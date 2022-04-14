// CIPCInputSaVicUnitMDAlarm.cpp: implementation of the CIPCInputSaVicUnitMDAlarm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SavicUnitDlg.h"
#include "CIPCInputSavicUnit.h"
#include "CIPCInputSavicUnitMDAlarm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCInputSaVicUnitMDAlarm::CIPCInputSaVicUnitMDAlarm(CSaVicUnitDlg* pMainWnd, CCodec *pCodec, LPCTSTR shmName)
	: CIPCInputSaVicUnit(pMainWnd, pCodec, shmName)
{
//	TRACE(_T("CIPCInputSaVicUnitMDAlarm::CIPCInputSaVicUnitMDAlarm\n"));
	m_dwAlarmState	= 0;
	m_dwEdgeMask	= 0;
	SetConnectRetryTime(25);
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputSaVicUnitMDAlarm::~CIPCInputSaVicUnitMDAlarm()
{
//	TRACE(_T("CIPCInputSaVicUnitMDAlarm::~CIPCInputSaVicUnitMDAlarm\n"));
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputSaVicUnitMDAlarm::IsValid()
{
	return CIPCInputSaVicUnit::IsValid();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnitMDAlarm::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE(_T("CIPCInputSaVicUnitMDAlarm::OnRequestSetEdge\tGroupID=%i\t0x%lx\n"), wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnitMDAlarm::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	m_dwEdgeMask = dwEdgeMask;

	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnitMDAlarm::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE(_T("CIPCInputSaVicUnitMDAlarm::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnitMDAlarm::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnitMDAlarm::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCInputSaVicUnitMDAlarm::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnitMDAlarm::OnRequestReset\tWrong wGroupID\n"));

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
void CIPCInputSaVicUnitMDAlarm::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputSaVicUnitMDAlarm::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnitMDAlarm::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	DoConfirmAlarmState(m_wGroupID, m_dwAlarmState ^ m_dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnitMDAlarm::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputSaVicUnitMDAlarm::OnResponseAlarmState   Not Yet \n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnitMDAlarm::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnitMDAlarm::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputSaVicUnitMDAlarm::OnResponseInputs   Not Yet\n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputSaVicUnitMDAlarm::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
/*void CIPCInputSaVicUnitMDAlarm::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die SaVic besitzt 16 MD-Alarmeingänge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE(_T("CIPCInputSaVicUnitMDAlarm::AlarmStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);


	m_dwAlarmState = dwNewState;
}
*/

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSaVicUnitMDAlarm::OnReceivedMotionAlarm(WORD wSource, CMDPoints& Points, BOOL bState)
{
	DWORD dwChangeMask = 1<<wSource;
	if (bState)
	{
		m_dwAlarmState = 1<<wSource;
	}
	else
	{
		m_dwAlarmState = 0;
	}

#if (0)
	CPoint Point = Points.GetPointAt(0);
	DoIndicateAlarmNr(CSecID(m_wGroupID, wSource), 
					  bState, 
					  MAKELONG(Point.x, Point.y), 
					  0);
#else
	if (GetIPCState() == CIPC_STATE_CONNECTED)
	{
		CIPCFields fields;
		if (bState)
		{
			CString sValueX, sValueY;
			CString sDBDNameX, sDBDNameY;

			for (int nI = 0; nI < min(Points.GetSize(), 5); nI++)
			{
				sValueX.Format(_T("%04hx"), Points.GetPointAt(nI).x);
				sValueY.Format(_T("%04hx"), Points.GetPointAt(nI).y);
				
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

				fields.Add(new CIPCField(sDBDNameX, sValueX, _T('C')));
				fields.Add(new CIPCField(sDBDNameY, sValueY, _T('C')));
			}
		}

		DoIndicateAlarmFieldsState(m_wGroupID,
			m_dwAlarmState,	// 1 high, 0 low
			dwChangeMask,	// 1 changed, 0 unchanged
			fields);
	}
#endif
}

