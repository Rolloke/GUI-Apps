/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcInputUSBcamUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCamUnit/CipcInputUSBcamUnit.cpp $
// CHECKIN:		$Date: 17.11.03 14:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 28.04.03 12:41 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBCamUnitDlg.h"
#include "CIPCInputUSBcamUnit.h"

CIPCInputUSBcamUnit::CIPCInputUSBcamUnit(CUSBCamUnitDlg* pMainWnd, LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
//	TRACE("CIPCInputUSBcamUnit::CIPCInputUSBcamUnit\n");
	m_pMainWnd			= pMainWnd;
	m_wGroupID			= SECID_NO_GROUPID;
	m_bOK				   = TRUE;
	m_dwAlarmState		= 0L;					// Alarmstatus
	m_dwEdge			   = 0xFFFFFFFF;
	m_dwAck				= 0xFFFFFFFF;
	m_dwAlarmMask     = 0;

	SetConnectRetryTime(25);
	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputUSBcamUnit::~CIPCInputUSBcamUnit()
{
//	TRACE("CIPCInputUSBcamUnit::~CIPCInputUSBcamUnit\n");
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputUSBcamUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestHardware(WORD /*wGroupID*/)
{
//	TRACE("CIPCInputUSBcamUnit::OnRequestHardware\tGroupID=%i\n", wGroupID);

	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE("CIPCInputUSBcamUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n", wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputUSBcamUnit::OnRequestSetEdge\tWrong wGroupID\n");
		return;
	}

	SetAllAlarmEdge(dwEdgeMask);

	m_dwEdge = dwEdgeMask;
	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE("CIPCInputUSBcamUnit::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n",wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputUSBcamUnit::OnRequestSetFree\tWrong wGroupID\n");
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE("CIPCInputUSBcamUnit::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputUSBcamUnit::OnRequestReset\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pMainWnd)
		m_pMainWnd->RequestInputReset();

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE("CIPCInputUSBcamUnit::OnRequestAlarmState\tGroupID=%i\n", wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputUSBcamUnit::OnRequestAlarmState\tWrong wGroupID\n");
		return;
	}

	DWORD dwAlarmMask = GetAllCurrentAlarmStatus();

	DoConfirmAlarmState(m_wGroupID, dwAlarmMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE("CIPCInputUSBcamUnit::OnResponseAlarmState   Not Yet \n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputUSBcamUnit::OnResponseAlarmState\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::OnResponseInputs(WORD wGroupID)
{
//	TRACE("CIPCInputUSBcamUnit::OnResponseInputs   Not Yet\n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("IPCInputUSBCamUnit::OnResponseInputs\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die USBCam besitzt 16 Alarmeingänge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE("CIPCInputUSBcamUnit::AlarmStateChanged (0x%x,0x%x)\n", dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputUSBcamUnit::GetAllCurrentAlarmStatus()
{
	return m_dwAlarmMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputUSBcamUnit::SetAllAlarmEdge(DWORD dwEdgeMask)
{
	m_dwEdge = dwEdgeMask;
	
	return;
}


//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputUSBcamUnit::GetAllAlarmEdge()
{

	return m_dwEdge;
}

