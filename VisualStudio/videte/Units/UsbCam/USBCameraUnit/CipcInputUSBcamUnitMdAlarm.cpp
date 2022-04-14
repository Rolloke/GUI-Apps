/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCameraUnit
// FILE:		$Workfile: CipcInputUSBcamUnitMdAlarm.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCameraUnit/CipcInputUSBcamUnitMdAlarm.cpp $
// CHECKIN:		$Date: 6.12.04 12:09 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 3.12.04 9:54 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBCameraUnitDlg.h"
#include "CIPCInputUSBcamUnit.h"
#include "CIPCInputUSBcamUnitMDAlarm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCInputUSBcamUnitMDAlarm::CIPCInputUSBcamUnitMDAlarm(CUSBCameraUnitDlg* pMainWnd, LPCTSTR shmName)
	: CIPCInputUSBcamUnit(pMainWnd, shmName)
{
//	TRACE("CIPCInputUSBcamUnitMDAlarm::CIPCInputUSBcamUnitMDAlarm\n");
	m_dwAlarmState	= 0;
	m_dwEdge    	= 0;
	SetConnectRetryTime(25);
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputUSBcamUnitMDAlarm::~CIPCInputUSBcamUnitMDAlarm()
{
//	TRACE("CIPCInputUSBcamUnitMDAlarm::~CIPCInputUSBcamUnitMDAlarm\n");
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputUSBcamUnitMDAlarm::IsValid()
{
	return CIPCInputUSBcamUnit::IsValid();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnitMDAlarm::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCInputUSBcamUnitMDAlarm::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputUSBcamUnitMDAlarm::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
	}
	else
	{	
		if (m_pMainWnd)
		{
			m_pMainWnd->PostMessage(WM_COMMAND, ID_REQUEST_RESET);
		}
		m_dwAlarmState = 0;
		DoConfirmReset(m_wGroupID);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnitMDAlarm::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputUSBcamUnitMDAlarm::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputUSBcamUnitMDAlarm::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	DoConfirmAlarmState(m_wGroupID, m_dwAlarmState ^ m_dwEdge);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnitMDAlarm::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputUSBcamUnitMDAlarm::OnResponseAlarmState   Not Yet \n"));
   if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputUSBcamUnitMDAlarm::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnitMDAlarm::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputUSBcamUnitMDAlarm::OnResponseInputs   Not Yet\n"));
   if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCInputUSBcamUnitMDAlarm::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnitMDAlarm::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die USBCam besitzt 16 MD-Alarmeingänge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE(_T("CIPCInputUSBcamUnitMDAlarm::AlarmStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnitMDAlarm::OnReceivedMotionAlarm(WORD wSource, CMDPoints& Points, BOOL bState)
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

void CIPCInputUSBcamUnitMDAlarm::OnRequestSetFree(WORD wGroupID, DWORD dwMask)
{
	CIPCInputUSBcamUnit::OnRequestSetFree(wGroupID, dwMask);
	DWORD i, dwFree = 1;
	for (i=0; i<MAX_CAMERAS; i++, dwFree<<=1)
	{
		m_pMainWnd->SetMotionDetection((WORD)i, m_dwFree & dwFree ? TRUE: FALSE);
	}
}

