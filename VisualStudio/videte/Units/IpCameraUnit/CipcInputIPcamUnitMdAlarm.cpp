/////////////////////////////////////////////////////////////////////////////
// PROJECT:		IPCameraUnit
// FILE:		$Workfile: CipcInputIPcamUnitMdAlarm.cpp $
// ARCHIVE:		$Archive: /Project/Units/IPCameraUnit/CipcInputIPcamUnitMdAlarm.cpp $
// CHECKIN:		$Date: 30.08.06 10:56 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 30.08.06 10:42 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
// Implementation of the CIPC MD input connection to the server
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPCameraUnit.h"
#include "IPCameraUnitDlg.h"

#include "CIPCInputIpCamUnitMDAlarm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCInputIpCamUnitMDAlarm::CIPCInputIpCamUnitMDAlarm(CIpCameraUnitDlg* pDlg, LPCTSTR shmName)
	: CIPCInputIpCamUnit(pDlg, shmName)
{
//	TRACE("CIPCInputIpCamUnitMDAlarm::CIPCInputIpCamUnitMDAlarm\n");
	m_dwEdge    	= 0;
	m_bMDinput		= TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputIpCamUnitMDAlarm::~CIPCInputIpCamUnitMDAlarm()
{
//	TRACE("CIPCInputIpCamUnitMDAlarm::~CIPCInputIpCamUnitMDAlarm\n");
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputIpCamUnitMDAlarm::IsValid()
{
	return CIPCInputIpCamUnit::IsValid();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnitMDAlarm::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputIpCamUnitMDAlarm::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}

	DoConfirmAlarmState(m_wGroupID, m_dwAlarmState ^ m_dwEdge);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnitMDAlarm::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputIpCamUnitMDAlarm::OnResponseAlarmState   Not Yet \n"));
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnitMDAlarm::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputIpCamUnitMDAlarm::OnResponseInputs   Not Yet\n"));
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnitMDAlarm::OnReceivedMotionAlarm(WORD wSource, BOOL bState)
{
	DWORD dwChangeMask = m_dwAlarmState;
	if (bState)
		m_dwAlarmState = SETBIT(m_dwAlarmState, wSource);
	else
		m_dwAlarmState = CLRBIT(m_dwAlarmState, wSource);
	dwChangeMask = (dwChangeMask ^ m_dwAlarmState);

	CIPCFields dummy;
	
	DoIndicateAlarmFieldsState(m_wGroupID,
							m_dwAlarmState,	// 1 high, 0 low
							dwChangeMask,	// 1 changed, 0 unchanged
							dummy);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnitMDAlarm::OnRequestSetFree(WORD wGroupID, DWORD dwMask)
{
	CIPCInputIpCamUnit::OnRequestSetFree(wGroupID, dwMask);
	DWORD i, dwFree = 1;
	for (i=0; i<MAX_CAMERAS; i++, dwFree<<=1)
	{
		// TODO! RKE: SetMotionDetection
//		m_pDlg->SetMotionDetection((WORD)i, m_dwFree & dwFree ? TRUE: FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputIpCamUnitMDAlarm::IsConnected()
{
	return (GetIPCState() == CIPC_STATE_CONNECTED) ? TRUE : FALSE;
}
