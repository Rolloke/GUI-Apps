// PlayRealTime.cpp: implementation of the CPlayRealTime class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlayRealTime.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayRealTime::CPlayRealTime()
: CWK_Thread(_T("PlayRealTime"),NULL)
{
	m_hDisplayWindow			= NULL;
	m_dwExtraTimeToWait			= 0;
	m_dwTick					= 0;
	m_pRealTimePicture			= NULL;
	m_pRealTimeFields			= NULL;
	m_bRealTimePictureDateTime	= FALSE;
	m_bExtraTimeToWait			= FALSE;
	m_dwShowTime				= 0;
	m_dwCountPics				= 0;

	//test
	m_dwTestSummeAllDiffs = 0;
	m_dwAllLastShowTimes = 0;
	m_dwTickTestBereich = 0;
	m_dwWholeCountPics = 0;
	m_dwWholeLastShowTimes = 0;

	StartThread();
}

CPlayRealTime::~CPlayRealTime()
{
 	StopThread();
	if(m_pRealTimeFields)
	{
		WK_DELETE(m_pRealTimeFields);
	}
}


//////////////////////////////////////////////////////////////////////
BOOL CPlayRealTime::Run(LPVOID lpContext)
{
	if(m_bExtraTimeToWait)
	{
		Wait();
	}

	return TRUE; // no longer run
}

//////////////////////////////////////////////////////////////////////
BOOL CPlayRealTime::StopThread()
{
	CWK_Thread::StopThread();
	m_bRealTimePictureDateTime = FALSE;
	if(m_pRealTimePicture)
	{
		WK_DELETE(m_pRealTimePicture);
		TRACE(_T("#### m_pRealTimePicture deleted\n"));
	}

	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CPlayRealTime::Wait()
{
	while(GetTimeSpan(m_dwTick) < m_dwExtraTimeToWait)
	{
		Sleep(10);
	}
	PostMessage(m_hDisplayWindow, WM_PLAY_REALTIME, 0, 0);
	m_bExtraTimeToWait = FALSE;
}

//////////////////////////////////////////////////////////////////////
BOOL CPlayRealTime::CheckRealTimePicture(WORD wArchivNr, 
										 WORD wSequenceNr, 
										 DWORD dwRecordNr,
										 DWORD dwNrOfRecords, 
										 const CIPCPictureRecord &pict, 
										 int iNumFields,
										 const CIPCField fields[],
										 HWND hDisplayWnd)
{

//TODO TKR evtl. später noch einbauen: zurzeit ist Realtime nur durch "bremsen" des Abspielens erreichbar
	//gibt es aber später bis zu 25 Bilder/Sek. müssen vielleicht ein paar Bilder übersprungen werden
	//um Echtzeit zu erreiochen
	BOOL bPlayNextRecord = TRUE;
	DWORD dwExtraTimeToWait = 500;
	DWORD dwLastShowTime = 0;
	DWORD dwLastExtraTimeToWait = 0;

	if(m_bRealTimePictureDateTime)
	{
		dwLastShowTime = GetTimeSpan(GetShowTime());
		dwLastExtraTimeToWait = GetExtraTimeToWait();
	}

//	TRACE(_T("--------------- LastShowTime: %d  LastExtraTime: %d\n"), dwLastShowTime, dwLastExtraTimeToWait);
	if(dwLastShowTime > dwLastExtraTimeToWait)
	{
		dwLastShowTime = dwLastShowTime - dwLastExtraTimeToWait;
	}

	//Addiere die letzte Showtime zu den vorherigen
	SetLastShowTimesCount(dwLastShowTime);

	//starte Wartezyklus
	SetShowTime(GetTickCount());

	if(!m_bRealTimePictureDateTime)
	{
		CString sFirst;
		sFirst.Format(_T("%s:%d"), pict.GetTimeStamp().GetDateTime(), pict.GetTimeStamp().GetMilliseconds());
//		TRACE(_T("*********** erstes Realtime Bild gesetzt: %s \n"), sFirst);
		SetRealTimePictureDateTime(pict.GetTimeStamp());
		SetHandleDisplayWindow(hDisplayWnd);

		//init members
		m_dwCountPics = 0;
		m_dwTestSummeAllDiffs = 0;
		m_dwAllLastShowTimes = 0;
		m_dwTickTestBereich = GetTickCount();
	}
	else
	{
		CSystemTime stLastPicture = GetRealTimePictureDateTime();
		CSystemTime stCurrPicture = pict.GetTimeStamp();
		CString sDateTimeAlt, sDateTimeNeu;
		sDateTimeAlt.Format(_T("%s:%d"), stLastPicture.GetDateTime(), stLastPicture.GetMilliseconds());
		sDateTimeNeu.Format(_T("%s:%d"), stCurrPicture.GetDateTime(), stCurrPicture.GetMilliseconds());
//		TRACE(_T("### Bild alt: %s, neu: %s\n"), sDateTimeAlt, sDateTimeNeu);
		
		//Differenz in Millisekunden berechnen
		DWORD dwDiffInMS = GetDiffInMilliSec(stLastPicture, stCurrPicture);
		
//		TRACE(_T("### Differenz: %d  LastShowTime: %d\n"), dwDiffInMS, dwLastShowTime);
		m_dwCountPics ++;

		if(m_dwCountPics == 100)
		{
//			TRACE(_T("### durchschn. Anzeigezeit pro Bild: %d bei %d Bildern\n"), (GetLastShowTimesCount()+dwLastShowTime)/m_dwCountPics, m_dwCountPics);
			m_dwWholeCountPics += m_dwCountPics;
			m_dwCountPics = 0;
			m_dwWholeLastShowTimes += m_dwAllLastShowTimes;
			m_dwAllLastShowTimes = 0;
		}
		
		m_bExtraTimeToWait = TRUE;
		
		if(dwDiffInMS > 1000)
		{
			//warte 500 millisekunden und melde dann nächstes Bild 
			TRACE(_T("**** TKR warte nur 500ms bei Abstand > 1000 ms\n"));
			bPlayNextRecord = FALSE;

			SaveRealTimePicture(wArchivNr, wSequenceNr, dwRecordNr,
								dwNrOfRecords, pict, iNumFields, fields);

			SetExtraTimeToWait(dwExtraTimeToWait);
			m_dwTestSummeAllDiffs += (500 + dwLastShowTime);

		}
		else if(dwDiffInMS > dwLastShowTime)
		{
			m_dwTestSummeAllDiffs += dwDiffInMS;
			dwExtraTimeToWait = dwDiffInMS - dwLastShowTime;
			TRACE(_T("##### warte ... %d - %d = %d\n"), dwDiffInMS, dwLastShowTime, dwExtraTimeToWait);
		
			//warte die Echtzeit ab
			bPlayNextRecord = FALSE;

			SaveRealTimePicture(wArchivNr, wSequenceNr, dwRecordNr,
								dwNrOfRecords, pict, iNumFields, fields);

			SetExtraTimeToWait(dwExtraTimeToWait);

		}
		else
		{
			//zeige Bild sofort an
			m_dwTestSummeAllDiffs += dwDiffInMS;
			m_bExtraTimeToWait = FALSE;
			dwExtraTimeToWait = 0;
			SetExtraTimeToWait(dwExtraTimeToWait);
//			TRACE(_T("##### TKR Bild direkt abspielen, nicht warten ... \n"));
		}
	
		SetRealTimePictureDateTime(pict.GetTimeStamp());
	}
	
	return bPlayNextRecord;
}
//////////////////////////////////////////////////////////////////////
void CPlayRealTime::SaveRealTimePicture(WORD wArchivNr, 
									    WORD wSequenceNr, 
									    DWORD dwRecordNr,
									    DWORD dwNrOfRecords, 
									    const CIPCPictureRecord &pict,
									    int iNumFields,
									    const CIPCField fields[])
{
	m_wRealTimeArchivNr		= wArchivNr;
	m_wRealTimeSequenceNr	= wSequenceNr;
	m_dwRealTimeRecordNr	= dwRecordNr;
	m_dwRealTimeNrOfRecords	= dwNrOfRecords;
	m_iRealTimeNumFields	= iNumFields;

	if(m_pRealTimePicture)
	{
		WK_DELETE(m_pRealTimePicture);
	}

	m_pRealTimePicture		= new CIPCPictureRecord(pict);

	CIPCFields AllFields;
	AllFields.FromArray(iNumFields, fields);

	if(m_pRealTimeFields)
	{
		WK_DELETE(m_pRealTimeFields);
	}
	
	m_pRealTimeFields = new CIPCFields(AllFields);


}

//////////////////////////////////////////////////////////////////////
void CPlayRealTime::GetRealTimePicture(WORD &wArchivNr,
									   WORD &wSequenceNr, 
									   DWORD &dwRecordNr,
									   DWORD &dwNrOfRecords, 
									   const CIPCPictureRecord* (&ppict),
									   int &iNumFields,
									   const CIPCFields *(&fields))
{
	wArchivNr		= m_wRealTimeArchivNr;
	wSequenceNr		= m_wRealTimeSequenceNr;
	dwRecordNr		= m_dwRealTimeRecordNr;
	dwNrOfRecords	= m_dwRealTimeNrOfRecords;
	ppict			= m_pRealTimePicture;
	iNumFields		= m_iRealTimeNumFields;
	fields			= m_pRealTimeFields;
}

//////////////////////////////////////////////////////////////////////
void CPlayRealTime::SetHandleDisplayWindow(HWND hDisplayWindow)
{
	m_hDisplayWindow = hDisplayWindow;
}
//////////////////////////////////////////////////////////////////////
void CPlayRealTime::SetRealTimePictureDateTime(CSystemTime stPictDateTime)
{
	m_stRealTimePictureDateTime = stPictDateTime;
	
	m_bRealTimePictureDateTime = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CPlayRealTime::SetRealTimePictureDateTime(BOOL bRealTime)
{
	if(m_dwCountPics > 0)
	{
		TRACE(_T("### Summe Bild-Abstände:	%d ms\n"), m_dwTestSummeAllDiffs);
		TRACE(_T("### Gesamt Laufzeit:		%d ms\n"), GetTimeSpan(m_dwTickTestBereich));
		TRACE(_T("### durchschn. Anzeigezeit pro Bild: %d ms bei %d Bildern\n"), 
			(m_dwWholeLastShowTimes+GetLastShowTimesCount())/(m_dwWholeCountPics+m_dwCountPics), 
			m_dwWholeCountPics+m_dwCountPics);
	}

	m_bRealTimePictureDateTime = bRealTime;
}
//////////////////////////////////////////////////////////////////////
CSystemTime CPlayRealTime::GetRealTimePictureDateTime()
{
	return m_stRealTimePictureDateTime;
}
//////////////////////////////////////////////////////////////////////
BOOL CPlayRealTime::IsRealTimePictureDateTime()
{
	return m_bRealTimePictureDateTime;
}
//////////////////////////////////////////////////////////////////////
void CPlayRealTime::SetExtraTimeToWait(DWORD dwExtraTimeToWait)
{
	m_dwExtraTimeToWait = dwExtraTimeToWait;
	m_dwTick = GetTickCount();
}

//////////////////////////////////////////////////////////////////////
DWORD CPlayRealTime::GetExtraTimeToWait()
{
	return m_dwExtraTimeToWait;
}

//////////////////////////////////////////////////////////////////////
DWORD CPlayRealTime::GetDiffInMilliSec(CSystemTime &stLastPicture, CSystemTime &stCurrPicture)
{
	DWORD dwDiffInMS = 0;

	//Abstand zwischen letztem und aktuellem Bild in Millisekunden berechnen
	FILETIME ftDiff;
	LARGE_INTEGER liDiff;
	CSystemTime stDiff;
	if(stLastPicture < stCurrPicture)
	{
		//play forward
		stDiff = stCurrPicture - stLastPicture;
	}
	else
	{
		//play back
		stDiff = stLastPicture - stCurrPicture;
	}


	//den jeweils berechneten Abstand in Sekunden bestimmen
	SystemTimeToFileTime(&stDiff, &ftDiff);
	
	liDiff.HighPart	= ftDiff.dwHighDateTime;
	liDiff.LowPart	= ftDiff.dwLowDateTime;

	//Ergebnis von Zeitschritt des LARGE_INTEGER 100 Nanosekunden
	//in Zeitschritt 1 MilliSekunde umrechnen
	liDiff.QuadPart	 = liDiff.QuadPart/(10*1000);
		
	dwDiffInMS = (DWORD)liDiff.QuadPart;
	
	return dwDiffInMS;
}

//////////////////////////////////////////////////////////////////////
void CPlayRealTime::SetLastShowTimesCount(DWORD dwLastShowTime)
{
	m_dwAllLastShowTimes += dwLastShowTime;

	if(dwLastShowTime == 0)
	{
		m_dwAllLastShowTimes = 0;
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CPlayRealTime::GetLastShowTimesCount()
{
	return m_dwAllLastShowTimes;
}
/////////////////////////////////////////////////////////////////////////////
void CPlayRealTime::SetShowTime(DWORD dwTick)
{
	m_dwShowTime = dwTick;
}
//////////////////////////////////////////////////////////////////////
DWORD CPlayRealTime::GetShowTime()
{
	return m_dwShowTime;
}
