/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: CAku.cpp $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/CAku.cpp $
// CHECKIN:		$Date: 5.08.04 13:54 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 5.08.04 13:54 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mmsystem.h"
#include "CIpc.h"
#include "AkuReg.h"
#include "CAku.h"
#include "CRelay.h"
#include "CAlarm.h"
#include "Ini2Reg.h"

// Buffersize
#define SIZE 255		

UINT AlarmPollThread(LPVOID pData)
{
	CAku*	pAku	 = (CAku*)pData;  // Pointer auf's Aku-Objekt

	if (!pAku)
	{
		 WK_TRACE( _T("ERROR:\tAlarmPollThread\tpAku = NULL\n"));
		return 0;
	}

	// Warte bis MainProzeß initialisiert ist.
	while (!pAku->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pAku->m_bRun)
	{
		Sleep(100);
		pAku->PollAlarm();
	}

	 WK_TRACE( _T("ExitAlarmPollThread\n"));
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::CAku
CAku::CAku(HWND hWnd, LPCSTR lpszAppIniName)
{
	m_csAku.Lock();

	// Default-Initialisierung
	m_bOK					= FALSE;
	m_bRun					= FALSE;
	m_hWndClient			= hWnd;
	m_wLastError			= AKU_ERR_NO_ERRROR;
	m_dwHardwareState		= 0;
	m_pAlarmPollThread		= NULL;

	// Ohne INI-Datei geht gar nichts
	if (!lpszAppIniName)
	{
	  	m_csAku.Unlock();
		return;
	}

	// Pfad und Name der INI-Datei sichern
	strcpy(m_szAppIniName, lpszAppIniName);

	// Basisadressen der Aku-Boards 
	m_Config.wIOBase[AKU_CARD0] = (WORD)MyGetPrivateProfileInt(_T("System"), _T("AkuIOBase0"), 0x00, m_szAppIniName);
	m_Config.wIOBase[AKU_CARD1] = (WORD)MyGetPrivateProfileInt(_T("System"), _T("AkuIOBase1"), 0x00, m_szAppIniName);
	m_Config.wIOBase[AKU_CARD2] = (WORD)MyGetPrivateProfileInt(_T("System"), _T("AkuIOBase2"), 0x00, m_szAppIniName);
	m_Config.wIOBase[AKU_CARD3] = (WORD)MyGetPrivateProfileInt(_T("System"), _T("AkuIOBase3"), 0x00, m_szAppIniName);
	m_Config.hWnd		= hWnd;

	// Hardwarestatusbits setzen
	if (m_Config.wIOBase[AKU_CARD0] != 0)
		m_dwHardwareState = (WORD)SETBIT(m_dwHardwareState, 0);
	if (m_Config.wIOBase[AKU_CARD1] != 0)
		m_dwHardwareState = (WORD)SETBIT(m_dwHardwareState, 1);
	if (m_Config.wIOBase[AKU_CARD2] != 0)
		m_dwHardwareState = (WORD)SETBIT(m_dwHardwareState, 2);
	if (m_Config.wIOBase[AKU_CARD3] != 0)
		m_dwHardwareState = (WORD)SETBIT(m_dwHardwareState, 3);

	// Jede Erweiterungskarte bekommt ein Relayobjekt
	m_pRelay[AKU_CARD0] = (CRelay*)new CRelay(m_Config.wIOBase[AKU_CARD0], AKU_CARD0);
	m_pRelay[AKU_CARD1] = (CRelay*)new CRelay(m_Config.wIOBase[AKU_CARD1], AKU_CARD1);
	m_pRelay[AKU_CARD2] = (CRelay*)new CRelay(m_Config.wIOBase[AKU_CARD2], AKU_CARD2);
	m_pRelay[AKU_CARD3] = (CRelay*)new CRelay(m_Config.wIOBase[AKU_CARD3], AKU_CARD3);
	
	// Jede Erweiterungskarte bekommt ein Alarmobjekt
	m_pAlarm[AKU_CARD0] = (CAlarm*)new CAlarm(m_Config.wIOBase[AKU_CARD0], AKU_CARD0);
	m_pAlarm[AKU_CARD1] = (CAlarm*)new CAlarm(m_Config.wIOBase[AKU_CARD1], AKU_CARD1);
	m_pAlarm[AKU_CARD2] = (CAlarm*)new CAlarm(m_Config.wIOBase[AKU_CARD2], AKU_CARD2);
	m_pAlarm[AKU_CARD3] = (CAlarm*)new CAlarm(m_Config.wIOBase[AKU_CARD3], AKU_CARD3);  

	m_pAlarmPollThread = AfxBeginThread(AlarmPollThread, this);
	m_pAlarmPollThread->m_bAutoDelete = FALSE;
	m_pAlarmPollThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);

	m_bOK = TRUE;  
	m_bRun= TRUE;

	m_csAku.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
// CAku::~CAku
CAku::~CAku()
{
	 WK_TRACE( _T("CAku::~CAku()\n"));

	SaveConfig();

	// AlarmPollThread beenden.
	m_bRun = FALSE;

	// Warte bis 'AlarmPollThread' beendet ist.
	if (m_pAlarmPollThread)
		WaitForSingleObject(m_pAlarmPollThread->m_hThread, 5000);

	WK_DELETE(m_pAlarmPollThread); //Autodelete = FALSE;

	for (WORD wI = AKU_CARD0; wI <= AKU_CARD3; wI++)
	{
		WK_DELETE(m_pRelay[wI]);
		WK_DELETE(m_pAlarm[wI]);
	}

	m_bOK  = FALSE;
}
	  
/////////////////////////////////////////////////////////////////////////////
// CAku::IsValid
BOOL CAku::IsValid()
{
	return m_bOK;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::SaveConfig
BOOL CAku::SaveConfig()
{
	if (!IsValid())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::GetVersion
WORD CAku::GetVersion()
{
	return AKU_VERSION;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::GetLastError
WORD CAku::GetLastError()
{
	m_csAku.Lock();

	WORD wTemp = m_wLastError;
	m_wLastError = AKU_ERR_NO_ERRROR;

	m_csAku.Unlock();

	return wTemp;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::CheckHardware
BOOL CAku::CheckHardware()
{
	return TRUE;
}

#if (0)
/////////////////////////////////////////////////////////////////////////////
// CAku::PollAlarm
BOOL CAku::PollAlarm()
{
	BYTE byAck					= 0;
	BYTE byFree					= 0;
	BYTE byChange				= 0;
	BYTE byCurrentAlarmState	= 0;
	BYTE byAlarmState			= 0;
	BOOL bRetFlag				= FALSE;


 	for (WORD wAkuCard = AKU_CARD0; wAkuCard <= AKU_CARD3; wAkuCard++)
	{
		// Karte vorhanden
		if (!TSTBIT(GetHardwareState(), wAkuCard))
			continue;
	
		// CurrentAlarmstatusregister lesen.
		byCurrentAlarmState = m_pAlarm[wAkuCard]->GetCurrentState();
		
		//byAlarmState = m_pAlarm[wAkuCard]->GetState() & m_pAlarm[wAkuCard]->GetAck(); 

		if ((byCurrentAlarmState != m_byAlarmState[wAkuCard]) | (byAlarmState != 0))
		{
			byAck 		= m_pAlarm[wAkuCard]->GetAck();  	
			byChange 	= byCurrentAlarmState ^ m_byAlarmState[wAkuCard];
			byFree		= ~byChange & byAck;

			m_byAlarmState[wAkuCard] = byCurrentAlarmState;
												
			// Alarm löschen
			m_pAlarm[wAkuCard]->SetAck(byFree);
			// Alarm freigeben
			m_pAlarm[wAkuCard]->SetAck(byFree);

			PostMessage(m_hWndClient, WM_AKU_ALARM, wAkuCard, byCurrentAlarmState);
			bRetFlag = TRUE;
		}
	}

	return bRetFlag;
}
#else
/////////////////////////////////////////////////////////////////////////////
BOOL CAku::PollAlarm()
{
	m_csAku.Lock();

	BYTE byAck					= 0;
	BYTE byFree					= 0;
	BYTE byChange				= 0;
	BYTE byCurrentAlarmState	= 0;
	BOOL  bRetFlag				= FALSE;
	DWORD dwStartTime;

	for (WORD wAkuCard = AKU_CARD0; wAkuCard <= AKU_CARD3; wAkuCard++)
	{
		// Karte vorhanden
		if (!TSTBIT(GetHardwareState(), wAkuCard))
			continue;

		// CurrentAlarmstatusregister lesen.
		byCurrentAlarmState = GetCurrentAlarmState(wAkuCard);
		
		if (byCurrentAlarmState != m_byAlarmState[wAkuCard])
		{
			byAck 		= GetAlarmAck(wAkuCard);  	
			byChange 	= byCurrentAlarmState ^ m_byAlarmState[wAkuCard];
			byFree		= ~byChange & byAck;

			m_byAlarmState[wAkuCard] = byCurrentAlarmState;
												
			// Alarm löschen
			SetAlarmAck(wAkuCard, byAck); 
			// Alarm freigeben
			SetAlarmAck(wAkuCard, byFree); 

   			// Test ob das Signal über einen längeren Zeitpunkt nicht ändert.
			dwStartTime = GetTickCount();
			while (GetTickCount() < dwStartTime + 50)
            {
				// CurrentAlarmstatusregister lesen und 'aufundieren'.
				byCurrentAlarmState = byCurrentAlarmState & GetCurrentAlarmState(wAkuCard);
				if (byCurrentAlarmState == 0)
					break;
				Sleep(0);
			}             

			PostMessage(m_hWndClient, WM_AKU_ALARM, wAkuCard, byCurrentAlarmState);
			bRetFlag = TRUE;
		}
	}
	
	m_csAku.Unlock();

	return bRetFlag;
}


#endif
/////////////////////////////////////////////////////////////////////////////
// CAku::GetState
DWORD CAku::GetHardwareState()
{
	return m_dwHardwareState;
}


/////////////////////////////////////////////////////////////////////////////
// CAku::GetCurrentAlarmState()
BYTE CAku::GetCurrentAlarmState(WORD wAkuCard)
{
	m_csAku.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wAkuCard))
		byRet = m_pAlarm[wAkuCard]->GetCurrentState();

	m_csAku.Unlock();

	return byRet;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::GetAlarmState()
BYTE CAku::GetAlarmState(WORD wAkuCard)
{
	m_csAku.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wAkuCard))
		byRet = m_pAlarm[wAkuCard]->GetState(); 

	m_csAku.Unlock();

	return byRet;

}

/////////////////////////////////////////////////////////////////////////////
// CAku::SetAlarmEdge()
BYTE CAku::SetAlarmEdge(WORD wAkuCard, BYTE byEdge)
{
	m_csAku.Lock();

	BYTE byOldEdge = 0;

	if (CheckExtCard(wAkuCard))
	{
		byOldEdge = GetAlarmEdge(wAkuCard);
		m_pAlarm[wAkuCard]->SetEdge(byEdge);
	}
	
	m_csAku.Unlock();

	return byOldEdge;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::SetAlarmAck()
BYTE CAku::SetAlarmAck(WORD wAkuCard, BYTE byAck)
{
 	m_csAku.Lock();

	BYTE byOldAck = 0; 

	if (CheckExtCard(wAkuCard))
	{
		byOldAck = GetAlarmAck(wAkuCard); 
		m_pAlarm[wAkuCard]->SetAck(byAck);
	}

	m_csAku.Unlock();

	return byOldAck;

}

/////////////////////////////////////////////////////////////////////////////
// CAku::GetAlarmEdge()
BYTE CAku::GetAlarmEdge(WORD wAkuCard)
{
 	m_csAku.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wAkuCard))
		byRet = m_pAlarm[wAkuCard]->GetEdge();

	m_csAku.Unlock();

	return byRet;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::GetAlarmAck()
BYTE CAku::GetAlarmAck(WORD wAkuCard)
{
 	m_csAku.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wAkuCard))
		byRet = m_pAlarm[wAkuCard]->GetAck();

	m_csAku.Unlock();

	return byRet;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::GetRelais()
BYTE CAku::GetRelais(WORD wAkuCard)
{
 	m_csAku.Lock();

	BYTE byRet = 0;

	if (CheckExtCard(wAkuCard))
		byRet = m_pRelay[wAkuCard]->GetRelay();
	
	m_csAku.Unlock();

	return byRet;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::SetRelais()
BYTE CAku::SetRelais(WORD wAkuCard, BYTE byRelais)
{
 	m_csAku.Lock();

	BYTE byOldRelais = 0; 

	if (CheckExtCard(wAkuCard))
	{
		byOldRelais = GetRelais(wAkuCard); 
		m_pRelay[wAkuCard]->SetRelay(byRelais);
	}

  	m_csAku.Unlock();

	return byOldRelais;
}

/////////////////////////////////////////////////////////////////////////////
// CAku::CheckExtCard
BOOL CAku::CheckExtCard(WORD wAkuCard)
{
	switch(wAkuCard)
	{
		case AKU_CARD0: 
		case AKU_CARD1: 
		case AKU_CARD2: 
		case AKU_CARD3:
			return TRUE;
		default:
			m_wLastError = AKU_ERR_WRONG_EXT_CARD;
			return FALSE;
	}		
} 
 
/*********************************************************************************/
/* Aufruf:    BOOL CheckRange(LPWORD lpwValue, long lMin, long lMax);            */
/* Parameter: lpwValue = Pointer auf eine WORD-Variable, dessen Inhalt auf      */
/*                        Grenzen untersucht wird.                               */
/*            lMin      = Kleinstwert der zugelassen ist                         */
/*            lMax      = Größtwert der zugelassen ist                           */
/* Rückgabe:  TRUE -> Der untersuchte Wert liegt innerhalb der angegebenen       */
/*                    Grenzen, ansonsten FALSE                                   */
/* Zweck:     Testet ob der Varableninhalt auf den 'lpwValue' zeigt innerhalb    */
/*            der Grenzen 'lMin' und 'lMax' liegt. Ist er kleiner als 'lMin',    */
/*            so wird er auf 'lMin' zurückgesetzt, ist er hingegen größer als    */
/*            'lMax', so wird er auf 'lMax' gesetzt.                             */
/* Bemerkung: Liegt der Variableninhalt außerhalb der Grenzen, so wird ein       */
/*            Fehler vom Typ VDE_RANGE erzeugt.                                  */
/*********************************************************************************/ 
BOOL CAku::CheckRange(LPWORD lpwValue, long lMin, long lMax)
{
	if (!lpwValue)
	{
		 WK_TRACE( _T("ERROR:\tCAku::CheckRange\tlpwValue = Null\n"));
		return FALSE;
	}

	if (*lpwValue < (WORD)lMin)
	{
		m_wLastError = AKU_ERR_PARM_OUT_OF_RANGE;
		*lpwValue = (WORD)lMin;
		return FALSE;
	}
	else if (*lpwValue > (WORD)lMax)
	{
		m_wLastError = AKU_ERR_PARM_OUT_OF_RANGE;
		*lpwValue = (WORD)lMax;
		return FALSE;
	
	}
	return TRUE;
}

