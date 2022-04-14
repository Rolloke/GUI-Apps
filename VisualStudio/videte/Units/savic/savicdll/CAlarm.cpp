/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CAlarm.cpp $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/CAlarm.cpp $
// CHECKIN:		$Date: 13.12.02 11:35 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 13.12.02 11:04 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CAlarm.h"
#include "CIoMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
		    
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CAlarm::CAlarm(DWORD dwIOBase, DWORD dwBoardVariation)
{
	m_pCIo  = new CIoMemory(dwIOBase);
	
	m_wEdge = 0;

	m_dwBoardVariation = dwBoardVariation;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CAlarm::~CAlarm()
{
	WK_DELETE(m_pCIo);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CAlarm::IsValid() const
{
	return (m_pCIo != NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
WORD CAlarm::GetState()
{
	WORD	wAlarmState1	= 0;
	WORD	wAlarmState2	= 0;
	WORD	wAlarmState		= 0;
	DWORD 	dwGPIOData		= 0;
	
	if (IsValid())
	{
		// Alarmeingänge 0....4 das erste mal einlesen
		dwGPIOData = m_pCIo->Inputdw(BT878_GPIO_DATA_CTL);
		if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN0_BIT))
			wAlarmState1 = SETBIT(wAlarmState1, 0);
		if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN1_BIT))
			wAlarmState1 = SETBIT(wAlarmState1, 1);
		if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN2_BIT))
			wAlarmState1 = SETBIT(wAlarmState1, 2);
		if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN3_BIT))
			wAlarmState1 = SETBIT(wAlarmState1, 3);
		if (TSTBIT(m_dwBoardVariation, VARIATION_ALARM4_PRESENT))
		{
			if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN4_BIT))
				wAlarmState1 = SETBIT(wAlarmState1, 4);
		}

		// Liegt mindestens ein Alarm an?
		if (wAlarmState1 != 0)
		{
			ClearLatch();

			// Zum Entprellen nach 90ms ein weiteres mal den Alarmeingang auslesen.
			Sleep(90);

			// Alarmeingänge 0....4 ein zweites mal einlesen
			dwGPIOData = m_pCIo->Inputdw(BT878_GPIO_DATA_CTL);
			if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN0_BIT))
				wAlarmState2 = SETBIT(wAlarmState2, 0);
			if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN1_BIT))
				wAlarmState2 = SETBIT(wAlarmState2, 1);
			if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN2_BIT))
				wAlarmState2 = SETBIT(wAlarmState2, 2);
			if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN3_BIT))
				wAlarmState2 = SETBIT(wAlarmState2, 3);
			if (TSTBIT(m_dwBoardVariation, VARIATION_ALARM4_PRESENT))
			{
				if (TSTBIT(dwGPIOData, BT878_GPIO_ALARM_IN4_BIT))
					wAlarmState2 = SETBIT(wAlarmState2, 4);
			}
			
			// Nur die Alarme sind gültig, die beidemale anlagen!
			wAlarmState = (wAlarmState1 & wAlarmState2);

			// Der Prototyp enthält einen Drehen
			if (TSTBIT(m_dwBoardVariation, VARIATION_ALARM_SWAP_BIT))
			{
				// Korrigiert einen Dreher im SaVic-Layout.
				// GPIOAlarmIn0 - Alarm3 
				// GPIOAlarmIn1 - Alarm2
				// GPIOAlarmIn2 - Alarm1
				// GPIOAlarmIn3 - Alarm0
				WORD wTmp = 0;
				if (TSTBIT(wAlarmState, 0))
					wTmp = SETBIT(wTmp, 3);
				if (TSTBIT(wAlarmState, 1))
					wTmp = SETBIT(wTmp, 2);
				if (TSTBIT(wAlarmState, 2))
					wTmp = SETBIT(wTmp, 1);
				if (TSTBIT(wAlarmState, 3))
					wTmp = SETBIT(wTmp, 0);
				wAlarmState = wTmp;
			}

			ClearLatch();	
		}
	}
	return wAlarmState ^ m_wEdge;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CAlarm::SetEdge(WORD wEdge)
{
	m_wEdge = wEdge;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
WORD CAlarm::GetEdge()	const
{
	return m_wEdge;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CAlarm::ClearLatch()
{
	if (IsValid())
	{
		// Lowpuls löscht das Latch wieder
		DWORD dwDPIOState = m_pCIo->Inputdw(BT878_GPIO_DATA_CTL);
		
		dwDPIOState = CLRBIT(dwDPIOState, BT878_GPIO_CLEAR_LATCH_BIT);
		m_pCIo->Outputdw(BT878_GPIO_DATA_CTL, dwDPIOState);
		
		Sleep(1);
		
		dwDPIOState = SETBIT(dwDPIOState, BT878_GPIO_CLEAR_LATCH_BIT);
		m_pCIo->Outputdw(BT878_GPIO_DATA_CTL, dwDPIOState);
	}
}
