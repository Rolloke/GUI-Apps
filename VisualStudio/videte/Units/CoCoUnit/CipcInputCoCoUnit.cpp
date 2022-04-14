/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: CipcInputCoCoUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/CipcInputCoCoUnit.cpp $
// CHECKIN:		$Date: 5.08.98 9:43 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 5.08.98 9:43 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CIPCInputCoCoUnit.h"
#include "CCoCo.h"

CIPCInputCoCoUnit::CIPCInputCoCoUnit(CCoCo *pCoCo, BYTE byCoCoID,
										   const char *shmName)
	: CIPCInput(shmName,FALSE)
{
	m_pCoCo				= pCoCo;
	m_byCoCoID			= byCoCoID;
	m_wGroupID			= SECID_NO_GROUPID;
	m_bOK				= TRUE;
	m_dwHardwareState	= m_pCoCo->GetState();	// Hardwarekonfiguration
	m_dwAlarmState		= 0L;					// Alarmstatus
	m_dwEdge			= 0xFFFFFFFF;
	m_dwAck				= 0xFFFFFFFF;

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputCoCoUnit::~CIPCInputCoCoUnit()
{
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputCoCoUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::OnRequestHardware(WORD wGroupID)
{
//	TRACE("CIPCInputCoCoUnit::OnRequestHardware\tGroupID=%i\n", wGroupID);

	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE("CIPCInputCoCoUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n", wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("OnRequestSetEdge\tWrong wGroupID\n");
		return;
	}

	SetAllAlarmEdge(dwEdgeMask);

	m_dwEdge = dwEdgeMask;
	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE("CIPCInputCoCoUnit::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n",wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("OnRequestSetFree\tWrong wGroupID\n");
		return;
	}

	SetAllAlarmAck(~dwOpenMask); // Alarme löschen
	SetAllAlarmAck(dwOpenMask); // und freigeben

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::OnRequestReset(WORD wGroupID)	
{
//	TRACE("CIPCInputCoCoUnit::OnRequestReset Not Yet\n");

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("OnRequestReset\tWrong wGroupID\n");
		return;
	}

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE("CIPCInputCoCoUnit::OnRequestAlarmState\tGroupID=%i\n", wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("OnRequestAlarmState\tWrong wGroupID\n");
		return;
	}

	DWORD dwAlarmMask = GetAllCurrentAlarmStatus();

	DoConfirmAlarmState(m_wGroupID, dwAlarmMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE("CIPCInputCoCoUnit::OnResponseAlarmState   Not Yet \n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("OnResponseAlarmState\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::OnResponseInputs(WORD wGroupID)
{
//	TRACE("CIPCInputCoCoUnit::OnResponseInputs   Not Yet\n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("OnResponseInputs\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::AlarmStateChanged(WORD wExtCard, WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;
	switch (wExtCard)
	{
		case COCO_EXTCARD0:
			dwNewState = dwNewState & 0xffffff00 | (DWORD)wAlarmState<<0;
			break;
		case COCO_EXTCARD1:
			dwNewState = dwNewState & 0xffff00ff | (DWORD)wAlarmState<<8;
			break;
		case COCO_EXTCARD2:
			dwNewState = dwNewState & 0xff00ffff | (DWORD)wAlarmState<<16;
			break;
		case COCO_EXTCARD3:
			dwNewState = dwNewState & 0x00ffffff | (DWORD)wAlarmState<<24;
			break;
	}
		
//	WK_TRACE("CIPCInputCoCoUnit::AlarmStateChanged (0x%x,0x%x)\n", dwNewState, m_dwAlarmState);
//	WK_TRACE("CIPCInputCoCoUnit::AlarmStateChanged (0x%x,0x%x)\n", dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputCoCoUnit::GetAllCurrentAlarmStatus()
{
 	DWORD dwBit = m_byCoCoID * 4;	// m_byCoCoID = 0...3
	DWORD dwAlarmMask = 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAlarmMask |= (DWORD)m_pCoCo->GetCurrentAlarmStatus(m_byCoCoID, COCO_EXTCARD0)<<0;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAlarmMask |= (DWORD)m_pCoCo->GetCurrentAlarmStatus(m_byCoCoID, COCO_EXTCARD1)<<8;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAlarmMask |= (DWORD)m_pCoCo->GetCurrentAlarmStatus(m_byCoCoID, COCO_EXTCARD2)<<16;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAlarmMask |= (DWORD)m_pCoCo->GetCurrentAlarmStatus(m_byCoCoID, COCO_EXTCARD3)<<24;
	}

	return dwAlarmMask;
}


//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputCoCoUnit::GetAllAlarmStatus()
{
 	DWORD dwBit = m_byCoCoID * 4;	// m_byCoCoID = 0...3
	DWORD dwAlarmMask = 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAlarmMask |= (DWORD)m_pCoCo->GetAlarmStatus(m_byCoCoID, COCO_EXTCARD0)<<0;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAlarmMask |= (DWORD)m_pCoCo->GetAlarmStatus(m_byCoCoID, COCO_EXTCARD1)<<8;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAlarmMask |= (DWORD)m_pCoCo->GetAlarmStatus(m_byCoCoID, COCO_EXTCARD2)<<16;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAlarmMask |= (DWORD)m_pCoCo->GetAlarmStatus(m_byCoCoID, COCO_EXTCARD3)<<24;
	}	
	return dwAlarmMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::SetAllAlarmEdge(DWORD dwEdgeMask)
{
 	DWORD dwBit = m_byCoCoID * 4;	// m_byCoCoID = 0...3

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetAlarmEdge(m_byCoCoID, COCO_EXTCARD0, (WORD)(dwEdgeMask>>0  & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetAlarmEdge(m_byCoCoID, COCO_EXTCARD1, (WORD)(dwEdgeMask>>8  & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetAlarmEdge(m_byCoCoID, COCO_EXTCARD2, (WORD)(dwEdgeMask>>16 & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetAlarmEdge(m_byCoCoID, COCO_EXTCARD3, (WORD)(dwEdgeMask>>24 & 0x000000ff));
	}
	m_dwEdge = dwEdgeMask;
	return;
}


//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputCoCoUnit::GetAllAlarmEdge()
{
 	DWORD dwBit = m_byCoCoID * 4;	// m_byCoCoID = 0...3
	DWORD dwEdgeMask = 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwEdgeMask |= (DWORD)m_pCoCo->GetAlarmEdge(m_byCoCoID, COCO_EXTCARD0)<<0;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwEdgeMask |= (DWORD)m_pCoCo->GetAlarmEdge(m_byCoCoID, COCO_EXTCARD1)<<8;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwEdgeMask |= (DWORD)m_pCoCo->GetAlarmEdge(m_byCoCoID, COCO_EXTCARD2)<<16;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwEdgeMask |= (DWORD)m_pCoCo->GetAlarmEdge(m_byCoCoID, COCO_EXTCARD3)<<24;
	}
	return dwEdgeMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputCoCoUnit::SetAllAlarmAck(DWORD dwAckMask)
{
 	DWORD dwBit = m_byCoCoID * 4;	// m_byCoCoID = 0...3

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetAlarmAck(m_byCoCoID, COCO_EXTCARD0, (WORD)(dwAckMask>>0  & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetAlarmAck(m_byCoCoID, COCO_EXTCARD1, (WORD)(dwAckMask>>8  & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetAlarmAck(m_byCoCoID, COCO_EXTCARD2, (WORD)(dwAckMask>>16 & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetAlarmAck(m_byCoCoID, COCO_EXTCARD3, (WORD)(dwAckMask>>24 & 0x000000ff));
	}
	m_dwAck = dwAckMask;
	return;
}


//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputCoCoUnit::GetAllAlarmAck()
{
 	DWORD dwBit = m_byCoCoID * 4;	// m_byCoCoID = 0...3
	DWORD dwAckMask = 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAckMask |= (DWORD)m_pCoCo->GetAlarmAck(m_byCoCoID, COCO_EXTCARD0)<<0;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAckMask |= (DWORD)m_pCoCo->GetAlarmAck(m_byCoCoID, COCO_EXTCARD1)<<8;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAckMask |= (DWORD)m_pCoCo->GetAlarmAck(m_byCoCoID, COCO_EXTCARD2)<<16;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwAckMask |= (DWORD)m_pCoCo->GetAlarmEdge(m_byCoCoID, COCO_EXTCARD3)<<24;
	}
	return dwAckMask;
}

