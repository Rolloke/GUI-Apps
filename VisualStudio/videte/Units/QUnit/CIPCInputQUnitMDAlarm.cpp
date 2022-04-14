// CIPCInputQUnitMDAlarm.cpp: implementation of the CIPCInputQUnitMDAlarm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QUnitDlg.h"
#include "CIPCInputQUnit.h"
#include "CIPCInputQUnitMDAlarm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCInputQUnitMDAlarm::CIPCInputQUnitMDAlarm(CQUnitDlg* pMainWnd, CUDP *pUDP, LPCTSTR shmName)
	: CIPCInputQUnit(pMainWnd, pUDP, shmName)
{
//	TRACE(_T("CIPCInputQUnitMDAlarm::CIPCInputQUnitMDAlarm\n"));
	m_dwAlarmState	= 0;
	m_dwEdgeMask	= 0;
	SetConnectRetryTime(25);
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputQUnitMDAlarm::~CIPCInputQUnitMDAlarm()
{
//	TRACE(_T("CIPCInputQUnitMDAlarm::~CIPCInputQUnitMDAlarm\n"));
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputQUnitMDAlarm::IsValid()
{
	return CIPCInputQUnit::IsValid();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnitMDAlarm::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE(_T("CIPCInputQUnitMDAlarm::OnRequestSetEdge\tGroupID=%i\t0x%lx\n"), wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnitMDAlarm::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	m_dwEdgeMask = dwEdgeMask;

	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnitMDAlarm::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE(_T("CIPCInputQUnitMDAlarm::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnitMDAlarm::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnitMDAlarm::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCInputQUnitMDAlarm::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnitMDAlarm::OnRequestReset\tWrong wGroupID\n"));

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
void CIPCInputQUnitMDAlarm::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputQUnitMDAlarm::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnitMDAlarm::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	DoConfirmAlarmState(m_wGroupID, m_dwAlarmState ^ m_dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnitMDAlarm::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputQUnitMDAlarm::OnResponseAlarmState   Not Yet \n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnitMDAlarm::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnitMDAlarm::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputQUnitMDAlarm::OnResponseInputs   Not Yet\n"));

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputQUnitMDAlarm::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputQUnitMDAlarm::OnReceivedMotionAlarm(int nCamera, CMDPoints& Points, BOOL bState)
{
	DWORD dwChangeMask = 1<<nCamera;
	if (bState)
	{
		m_dwAlarmState = 1<<nCamera;
	}
	else
	{
		m_dwAlarmState = 0;
	}

#if (0)
	CPoint Point = Points.GetPointAt(0);
	DoIndicateAlarmNr(CSecID(m_wGroupID, nCamera), 
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

