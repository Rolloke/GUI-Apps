// PlayRealTime.h: interface for the CPlayRealTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYREALTIME_H__6DAECF43_A81A_4516_AFA6_D9F68B918717__INCLUDED_)
#define AFX_PLAYREALTIME_H__6DAECF43_A81A_4516_AFA6_D9F68B918717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPlayRealTime : public CWK_Thread  
{
public:
	CPlayRealTime();
	virtual ~CPlayRealTime();
	virtual	BOOL		Run(LPVOID lpContext);
	virtual BOOL		StopThread();

	//wird im "CIPCDatabaseDVRecherche::OnConfirmRecordNr" aufgerufen
	//prüft, ob und wie lange bis zum Anzeigen des nächsten 
	//Bildes gewartet werden muss
	BOOL				CheckRealTimePicture(WORD wArchivNr, 
											 WORD wSequenceNr, 
											 DWORD dwRecordNr,
											 DWORD dwNrOfRecords, 
											 const CIPCPictureRecord &pict, 
											 int iNumFields,
											 const CIPCField fields[],
											 HWND hPlayWnd);

	//save all picture data 
	void				SaveRealTimePicture(WORD wArchivNr, 
										   WORD wSequenceNr, 
										   DWORD dwRecordNr,
										   DWORD dwNrOfRecords,
										   const CIPCPictureRecord &pict,
										   int iNumFields, 
										   const CIPCField fields[]);
	//get all picture data
	void				GetRealTimePicture(WORD &wArchivNr, 
										   WORD &wSequenceNr, 
										   DWORD &dwRecordNr,
										   DWORD &dwNrOfRecords,
										   const CIPCPictureRecord *(&ppict),
										   int &iNumFields, 
										   CIPCFields &fields);

	//set timestamp of a picture
	void				SetRealTimePictureDateTime(CSystemTime stDateTime);
	
	//
	void				SetRealTimePictureDateTime(BOOL bRealTime);

	//TRUE, if a timestamp for a picture was set
	//FALSE means, realtime play was not yet startet
	BOOL				IsRealTimePictureDateTime();

	//extra time to wait
	void				SetExtraTimeToWait(DWORD dwExtraTimeToWait);

	//TEST TODO TKR, kann wieder raus, dient zur Zeitmessung
	void	SetLastShowTimesCount(DWORD dwLastShowTime); //Addiert alle showtimes für Endauswertung	

protected:
private:
	//functions
	void				Wait();
	DWORD				GetDiffInMilliSec(CSystemTime &stLastSyncPicture, CSystemTime &stCurrSyncPicture);

private:
	//members

	//time to wait before show next picture
	DWORD	m_dwExtraTimeToWait;
	CCriticalSection m_cs;

	//members which keeps the picture data
	WORD						m_wRealTimeArchivNr;
	WORD						m_wRealTimeSequenceNr; 
	DWORD						m_dwRealTimeRecordNr;
	DWORD						m_dwRealTimeNrOfRecords;
	int							m_iRealTimeNumFields;
	const CIPCPictureRecord*	m_pRealTimePicture;
	CIPCFields					m_RealTimeFields;

	volatile HWND				m_hDisplayWindow;				//handle to displaywindow
	CSystemTime					m_stRealTimePictureDateTime;	//timestamp of realtime picture
	BOOL						m_bRealTimePictureDateTime;		//is timestamp set
	DWORD						m_dwTick;						//time counter for extra waiting time
	BOOL						m_bExtraTimeToWait;				//if true, Run() waits for extratime
	DWORD						m_dwShowTime;					//whole showtime for one picture
	DWORD						m_dwCountPics;					//counts played realtime pictures

	//TEST TODO TKR, kann wieder raus, dient zur Zeitmessung
	DWORD		m_dwTestSummeAllDiffs;
	DWORD		m_dwAllLastShowTimes;
	DWORD		m_dwTickTestBereich;
	DWORD		m_dwWholeLastShowTimes;
	DWORD		m_dwWholeCountPics;
};

#endif // !defined(AFX_PLAYREALTIME_H__6DAECF43_A81A_4516_AFA6_D9F68B918717__INCLUDED_)

