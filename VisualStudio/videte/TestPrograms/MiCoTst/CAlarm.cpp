/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CAlarm.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CAlarm.cpp $
// CHECKIN:		$Date: 5.08.98 9:59 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 5.08.98 9:59 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////


#include "micotst.h"
#include "conio.h"
#include "MiCoDefs.h"
#include "MiCoReg.h"
#include "CIo.h"
#include "CAlarm.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAlarm::CAlarm
CAlarm::CAlarm(WORD wIOBase, WORD wExtCard)
{
	m_wIOBase 	= wIOBase;
	m_wExtCard  = wExtCard;
	m_byAck     = 0;
	m_byEdge    = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAlarm::~CAlarm
CAlarm::~CAlarm()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAlarm::CAlarm
BOOL CAlarm::IsValid()
{
	return (m_wIOBase != 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAlarm::GetCurrentState
BYTE CAlarm::GetCurrentState()
{
	if (m_wExtCard == MICO_EXTCARD0){
		return IoAccess.Input(m_wIOBase + ALARM_CUR_STATUS_OFFSET); 
	}
	else{
		return IoAccess.Input(m_wIOBase + ALARM_CUR_STATUS_COVI_OFFSET); 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAlarm::GetState
BYTE CAlarm::GetState()
{
	if (m_wExtCard == MICO_EXTCARD0){
		return IoAccess.Input(m_wIOBase + ALARM_STATUS_OFFSET);
	}
	else{
		return IoAccess.Input(m_wIOBase + ALARM_STATUS_COVI_OFFSET);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAlarm::SetEdge
void CAlarm::SetEdge(BYTE byEdge)
{
	if (m_wExtCard == MICO_EXTCARD0){
	    IoAccess.Output(m_wIOBase + ALARM_EDGE_SEL_OFFSET, byEdge);
	}
	else{
	    IoAccess.Output(m_wIOBase + ALARM_EDGE_SEL_COVI_OFFSET, byEdge);
	}

	m_byEdge = byEdge;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAlarm::SetAck
void CAlarm::SetAck(BYTE byAck)
{
	if (m_wExtCard == MICO_EXTCARD0){
	    IoAccess.Output(m_wIOBase + ALARM_ACK_OFFSET, byAck);
	}
	else{
	    IoAccess.Output(m_wIOBase + ALARM_ACK_COVI_OFFSET, byAck);
	}

	m_byAck = byAck;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAlarm::GetAck
BYTE CAlarm::GetAck()
{
	return m_byAck;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAlarm::GetEdge
BYTE CAlarm::GetEdge()
{
	return m_byEdge;
} 

//////////////////////////////////////////////////////////////////////////////////////
// CAlarm::CheckIt
BOOL CAlarm::CheckIt()
{
	BYTE   	byEdge	 = 0;

	for (WORD wJ = 0; wJ < 10; wJ++)
	{
		for (WORD wI = 0; wI < 8; wI++)
		{	
			SetAck(0x00); 
			SetAck(0xff);
			SetEdge(0x00);
	
			SetEdge(SETBIT(0, wI));

		    Sleep(50);
			if (GetState() != (1 << wI))
			{
				WK_TRACE(TRUE, "\tAlarmtest\tfailed\n");
				return FALSE;
			}
		}
	}
	
	WK_TRACE(TRUE, "\tAlarmtest\tpassed\n");

	return TRUE;
}

