/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: CipcInputAkuUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/CipcInputAkuUnit.cpp $
// CHECKIN:		$Date: 27.02.04 9:53 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 27.02.04 9:02 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mmsystem.h"

#include "CIPCInputAkuUnit.h"
#include "CAku.h"

CIPCInputAkuUnit::CIPCInputAkuUnit(CAku *pAku, LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
//	TRACE("CIPCInputAkuUnit::CIPCInputAkuUnit\n");
	m_pAku				= pAku;
	m_wGroupID			= SECID_NO_GROUPID;
	m_bOK				= TRUE;
	m_dwHardwareState	= m_pAku->GetHardwareState();	// Hardwarekonfiguration
	m_dwAlarmState		= 0L;					// Alarmstatus
	m_dwEdge			= 0xFFFFFFFF;
	m_dwAck				= 0xFFFFFFFF;

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputAkuUnit::~CIPCInputAkuUnit()
{
//	TRACE("CIPCInputAkuUnit::~CIPCInputAkuUnit\n");
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputAkuUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::OnRequestHardware(WORD wGroupID)
{
//	TRACE("CIPCInputAkuUnit::OnRequestHardware\tGroupID=%i\n", wGroupID);

	DoConfirmHardware(m_wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE("CIPCInputAkuUnit::OnRequestSetEdge\tGroupID=%i\t0x%lx\n", wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCInputAkuUnit::OnRequestSetEdge\tWrong wGroupID\n"));
		return;
	}

	SetAllAlarmEdge(dwEdgeMask);

	m_dwEdge = dwEdgeMask;
	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE(_T("CIPCInputAkuUnit::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n"),wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCInputAkuUnit::OnRequestSetFree\tWrong wGroupID\n"));
		return;
	}

	SetAllAlarmAck(~dwOpenMask); // Alarme löschen
	SetAllAlarmAck(dwOpenMask); // und freigeben

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::OnRequestReset(WORD wGroupID)	
{
//	TRACE(_T("CIPCInputAkuUnit::OnRequestReset Not Yet\n"));

	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCInputAkuUnit::OnRequestReset\tWrong wGroupID\n"));
		return;
	}

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE(_T("CIPCInputAkuUnit::OnRequestAlarmState\tGroupID=%i\n"), wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCInputAkuUnit::OnRequestAlarmState\tWrong wGroupID\n"));
		return;
	}

	DWORD dwAlarmMask = GetAllCurrentAlarmStatus();

	DoConfirmAlarmState(m_wGroupID, dwAlarmMask);
}

#if 0
// OLD, OOPS source not removed, HEDU
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::OnRequestAlarmReset(WORD wGroupID, DWORD dwResetMask)
{
//	TRACE(_T("CIPCInputAkuUnit::OnRequestAlarmReset\tGroupID=%i\n"), wGroupID);

   	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCInputAkuUnit::OnRequestAlarmReset\tWrong wGroupID\n"));
		return;
	}

	// ??? Meine Interpretation von 'OnRequestAlarmReset'
   	DWORD dwAck		= GetAllAlarmAck();
	DWORD dwReset	= ~dwResetMask & dwAck;

	SetAllAlarmAck(dwReset); // Gewünschten Alarmeingänge löschen 
	SetAllAlarmAck(dwAck);	 // Und wieder die vorherige Ack-Maske setzen
	
	DoConfirmAlarmReset(m_wGroupID, dwResetMask);
}
#endif

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE(_T("CIPCInputAkuUnit::OnResponseAlarmState   Not Yet \n"));

   	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tCIPCInputAkuUnit::OnResponseAlarmState\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::OnResponseInputs(WORD wGroupID)
{
//	TRACE(_T("CIPCInputAkuUnit::OnResponseInputs   Not Yet\n"));

   	if (wGroupID != m_wGroupID)
	{
		 WK_TRACE( _T("ERROR:\tIPCInputAkuUnit::OnResponseInputs\tWrong wGroupID\n"));
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::AlarmStateChanged(WORD wExtCard, WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;
	switch (wExtCard)
	{
		case AKU_CARD0:
			dwNewState = dwNewState & 0xffffff00 | (DWORD)wAlarmState<<0;
			break;
		case AKU_CARD1:
			dwNewState = dwNewState & 0xffff00ff | (DWORD)wAlarmState<<8;
			break;
		case AKU_CARD2:
			dwNewState = dwNewState & 0xff00ffff | (DWORD)wAlarmState<<16;
			break;
		case AKU_CARD3:
			dwNewState = dwNewState & 0x00ffffff | (DWORD)wAlarmState<<24;
			break;
	}
		
	 WK_TRACE( _T("CIPCInputAkuUnit::AlarmStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputAkuUnit::GetAllCurrentAlarmStatus()
{
 	DWORD dwBit = 0;
	DWORD dwAlarmMask = 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAlarmMask |= (DWORD)m_pAku->GetCurrentAlarmState(AKU_CARD0)<<0;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAlarmMask |= (DWORD)m_pAku->GetCurrentAlarmState(AKU_CARD1)<<8;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAlarmMask |= (DWORD)m_pAku->GetCurrentAlarmState(AKU_CARD2)<<16;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAlarmMask |= (DWORD)m_pAku->GetCurrentAlarmState(AKU_CARD3)<<24;
	}

	return dwAlarmMask;
}


//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputAkuUnit::GetAllAlarmStatus()
{
 	DWORD dwBit = 0;
	DWORD dwAlarmMask = 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAlarmMask |= (DWORD)m_pAku->GetAlarmState(AKU_CARD0)<<0;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAlarmMask |= (DWORD)m_pAku->GetAlarmState(AKU_CARD1)<<8;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAlarmMask |= (DWORD)m_pAku->GetAlarmState(AKU_CARD2)<<16;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAlarmMask |= (DWORD)m_pAku->GetAlarmState(AKU_CARD3)<<24;
	}	
	return dwAlarmMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::SetAllAlarmEdge(DWORD dwEdgeMask)
{
 	DWORD dwBit = 0;	

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetAlarmEdge(AKU_CARD0, (BYTE)(dwEdgeMask>>0  & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetAlarmEdge(AKU_CARD1, (BYTE)(dwEdgeMask>>8  & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetAlarmEdge(AKU_CARD2, (BYTE)(dwEdgeMask>>16 & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetAlarmEdge(AKU_CARD3, (BYTE)(dwEdgeMask>>24 & 0x000000ff));
	}
	m_dwEdge = dwEdgeMask;
	return;
}


//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputAkuUnit::GetAllAlarmEdge()
{
 	DWORD dwBit = 0;
	DWORD dwEdgeMask = 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwEdgeMask |= (DWORD)m_pAku->GetAlarmEdge(AKU_CARD0)<<0;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwEdgeMask |= (DWORD)m_pAku->GetAlarmEdge(AKU_CARD1)<<8;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwEdgeMask |= (DWORD)m_pAku->GetAlarmEdge(AKU_CARD2)<<16;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwEdgeMask |= (DWORD)m_pAku->GetAlarmEdge(AKU_CARD3)<<24;
	}
	return dwEdgeMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputAkuUnit::SetAllAlarmAck(DWORD dwAckMask)
{
 	DWORD dwBit = 0;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetAlarmAck(AKU_CARD0, (BYTE)(dwAckMask>>0  & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetAlarmAck(AKU_CARD1, (BYTE)(dwAckMask>>8  & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetAlarmAck(AKU_CARD2, (BYTE)(dwAckMask>>16 & 0x000000ff));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			m_pAku->SetAlarmAck(AKU_CARD3, (BYTE)(dwAckMask>>24 & 0x000000ff));
	}
	m_dwAck = dwAckMask;
	return;
}


//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCInputAkuUnit::GetAllAlarmAck()
{
 	DWORD dwBit = 0;
	DWORD dwAckMask = 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAckMask |= (DWORD)m_pAku->GetAlarmAck(AKU_CARD0)<<0;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAckMask |= (DWORD)m_pAku->GetAlarmAck(AKU_CARD1)<<8;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAckMask |= (DWORD)m_pAku->GetAlarmAck(AKU_CARD2)<<16;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pAku)
			dwAckMask |= (DWORD)m_pAku->GetAlarmEdge(AKU_CARD3)<<24;
	}
	return dwAckMask;
}

