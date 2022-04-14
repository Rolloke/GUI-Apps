#include "stdafx.h"
#include "MiCoDefs.h"
#include "MiCoReg.h"
#include "CAlarm.h"
		    
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CAlarm::CAlarm(WORD wIOBase, WORD wExtCard)
{
	m_wIOBase 	= wIOBase;		// Basisadresse des Boards
	m_wExtCard  = wExtCard;		// Nummer des Boards (0=MiCo, 1..3=CoVi0..CoVi8) 
	m_byAck     = 0;
	m_byEdge    = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CAlarm::IsValid()
{
	return (m_wIOBase != 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CAlarm::GetCurrentState()
{
	if (m_wExtCard == MICO_EXTCARD0){
		return m_IoAccess.Input(m_wIOBase + ALARM_CUR_STATUS_OFFSET);		// MiCo-Board 
	}
	else{
		return m_IoAccess.Input(m_wIOBase + ALARM_CUR_STATUS_COVI_OFFSET);	// CoVi-Board 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CAlarm::GetState()
{
	if (m_wExtCard == MICO_EXTCARD0){
		return m_IoAccess.Input(m_wIOBase + ALARM_STATUS_OFFSET);			// MiCo-Board
	}
	else{
		return m_IoAccess.Input(m_wIOBase + ALARM_STATUS_COVI_OFFSET);		// CoVi-Board 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CAlarm::SetEdge(BYTE byEdge)
{
	if (m_wExtCard == MICO_EXTCARD0){
	    m_IoAccess.Output(m_wIOBase + ALARM_EDGE_SEL_OFFSET, byEdge);		// MiCo-Board
	}
	else{
	    m_IoAccess.Output(m_wIOBase + ALARM_EDGE_SEL_COVI_OFFSET, byEdge);	// CoVi-Board 
	}

	m_byEdge = byEdge;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CAlarm::SetAck(BYTE byAck)
{
	if (m_wExtCard == MICO_EXTCARD0){
	    m_IoAccess.Output(m_wIOBase + ALARM_ACK_OFFSET, byAck);				// MiCo-Board

	}
	else{
	    m_IoAccess.Output(m_wIOBase + ALARM_ACK_COVI_OFFSET, byAck);		// CoVi-Board 
	}

	m_byAck = byAck;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CAlarm::GetAck()
{
	return m_byAck;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CAlarm::GetEdge()
{
	return m_byEdge;
}

//////////////////////////////////////////////////////////////////////////////////////
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
				WK_TRACE("\tAlarmtest\tfailed\n");
				return FALSE;
			}
		}
	}
	
	WK_TRACE("\tAlarmtest\tpassed\n");

	return TRUE;
}
