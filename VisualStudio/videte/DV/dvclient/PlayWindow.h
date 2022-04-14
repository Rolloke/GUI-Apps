#if !defined(AFX_PLAYWINDOW_H__6B5BBEDD_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_PLAYWINDOW_H__6B5BBEDD_9757_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlayWindow.h : header file
//


									 
#include "DisplayWindow.h"
#include "DirectCD.h"
class CCopyReadOnlyVersion;
class CCDRBurn;
class CPlayRealTime;
class CIPCMediaRecord;
/////////////////////////////////////////////////////////////////////////////
// CPlayWindow window

class CPlayWindow : public CDisplayWindow
{

public:
	enum PlayStatus
	{
		PS_STOP						=	1,
		PS_PLAY_BACK				=	2,
		PS_PLAY_FORWARD				=	3,
		PS_FAST_BACK				=	4,
		PS_FAST_FORWARD				=	5,
		PS_EJECT					=   7,
		PS_SKIP_BACK				= 	8,
		PS_SKIP_FORWARD				= 	9,
		PS_SINGLE_BACK				=  10,
		PS_SINGLE_FORWARD			=  11,
		PS_QUERY					=  12,
		PS_QUERY_FAILED				=  13,
		PS_DELETE_ALARM				=  14,
		PS_DELETE_SUSPECT			=  15,
		PS_DELETING					=  16,
		PS_DELETED					=  17,
		PS_ACCESS_DENIED			=  18,
		PS_SHOW_SUSPECT				=  48,
		PS_NAVIGATE_FAILED			=  49,
		PS_READ_FAILED				=  50,
		PS_QUERY_RECT				=  51,
	};

	enum RectPlayStatus  //playstati during rectangle search
	{
		PS_RECT_NO_RECT				=   0,
		PS_RECT_STOP				=	1,
		PS_RECT_PLAY_BACK			=	2,
		PS_RECT_PLAY_FORWARD		=	3,
		PS_RECT_SINGLE_BACK			=   4,
		PS_RECT_SINGLE_FORWARD		=   5,
		PS_RECT_QUERY_RECT			=   6,

	};
	DECLARE_DYNAMIC(CPlayWindow)
// Construction
public:
	CPlayWindow(CMainFrame* pParent, CServer* pServer, const CIPCArchivRecord& rec);
	virtual ~CPlayWindow();

// Attributes
public:
	virtual CSecID		GetID() const;
			WORD		GetNr() const;
	virtual CString		GetName() const;
	virtual CString		GetTitle(CDC* pDC);
	virtual void		GetFooter(CByteArray& Array);
	virtual CString		GetDefaultText();
	virtual COLORREF	GetOSDColor();
	virtual BOOL		IsShowable();
	virtual int			GetType() const { return DISPLAYWINDOW_PLAY;};
	inline 	PlayStatus	GetPlayStatus() const;
	inline  PlayStatus  GetLastPlayStatus() const;
		    BOOL		CanPlayStatus(PlayStatus playstatus);
			BOOL		CanChangeMode();
	
	inline	RectPlayStatus	GetRectPlayStatus() const;
	void					SetRectPlayStatus(RectPlayStatus rectplaystatus);

	inline	BOOL		IsMD() const;
	inline	BOOL		IsAlarm() const;
	inline	BOOL		IsBackup() const;
	
	inline  BOOL		IsDeleting() const;
	
			CSystemTime			GetStartTime();
			__int64				GetNrOfPictures();
			CIPCSequenceRecord*	GetFirstNotBackupSequence();


// Operations
public:
	virtual void InitialRequests();
			void RequestSequenceList(BOOL bWait);
	virtual BOOL Request(int iMax=-1);
	virtual BOOL SetMDValues(WORD wX, WORD wY, BOOL bForce);
			void InitRectQueryMembers();
			BOOL SetAllMDValues(WORD wX, WORD wY);		//nur bei m_bCanRectSearch == TRUE
			BOOL RemoveAllMDValues();					//nur bei m_bCanRectSearch == TRUE
		
// Operations from Panel
public:
			void SetPlayStatus(PlayStatus playstatus);
			BOOL StartDatabaseSearch(const CSystemTime &SearchTime);
			BOOL NavigateLastPicture(PlayStatus playstatus = PS_STOP);
			BOOL NavigateFirstPicture(PlayStatus playstatus = PS_STOP);

			//sets the QueryTime for RectQuery, started with QueryButton in Panel 
			//when rectangle is on image
			void SetQueryRectSystemTime(CSystemTime stQueryRect); 

			//wird benötigt, um evtl. disabled Buttons nach Verschieben oder Resizen des 
			//Rechtecks wieder zu enablen
			void SetNextQueryRectResult(BOOL bForward, BOOL bBack); 

			inline BOOL GetNextQueryRectResult(); //liefert die Stati, ob während der Rechtecksuche
												  //weiter forwärts oder rückwärts abgespielt werden kann
			BOOL GetAlarmStartandEndTime(CSystemTime& start,CSystemTime& end);

// indications from CIPCDatabaseDVClient
public:

			BOOL IndicateSequenceList(int iNumRecords, const CIPCSequenceRecord data[]);
			BOOL RecordNr(WORD wSequenceNr, DWORD dwRecordNr, DWORD dwNrOfRecords,
						  const CIPCPictureRecord &pict, int iNumFields,
						  const CIPCField fields[]);
			BOOL RecordNr(WORD wSequenceNr, DWORD dwRecordNr, DWORD dwNrOfRecords, const CIPCMediaSampleRecord &pict);
			BOOL IndicateNewSequence(const CIPCSequenceRecord& data,
									 WORD  wPrevSequenceNr,
									 DWORD dwNrOfRecords);
			BOOL IndicateDeleteSequence(WORD wSequenceNr);
			BOOL IndicateError(CIPCError error, int iErrorCode, const CString &sErrorMessage);
			BOOL IndicateQueryResult(WORD wSequenceNr, DWORD dwRecordNr);
			BOOL ConfirmQuery();
			BOOL ConfirmGetValue(const CString &sKey,const CString &sValue);
	
			BOOL CanMilliSeconds(); //liefert TRUE, wenn im Archiv nach Millisekunden gesucht werden kann

			CPlayRealTime*	GetPlayRealTime();

			BOOL ContainsAudio();
//TODO TKR für Sync Abspielen
/*
			void			SetSyncBigOne(BOOL bCanBigOne);
			BOOL			CanMakeSyncBigOne(); //kann beim SyncPlay in 1Plus ein kleines PlayWnd
												 //wieder als großes Fenster dargestellt werden?
*/
			
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayWindow)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);

	void ChangePlayStatus(PlayStatus playstatus);
	BOOL Navigate(WORD wSequenceNr, DWORD dwRecordNr, 
				  int nRecCount=0, DWORD dwCam = (DWORD)(-1-NAVIGATION_ALL));
	BOOL RequestRecord(WORD wSequenceNr, DWORD dwRecordNr, DWORD dwNewRecord, DWORD dwCam);
	BOOL Activate();

	BOOL OnEject();
	BOOL OnStop();
	BOOL OnPlayBack();
	BOOL OnPlayForward();
	BOOL OnFastBack();
	BOOL OnFastForward();
	BOOL OnSingleBack();
	BOOL OnSingleForward();
	BOOL OnSkipBack();
	BOOL OnSkipForward();
	BOOL OnDeleteCurrentAlarm();
	BOOL OnDeleteSuspect();
	BOOL OnDeleting();
	BOOL OnDeleted();
	BOOL OnAccessDenied();
	BOOL OnNavigateFailed();
	BOOL OnReadFailed();
	
	BOOL OnQueryFailed();
	BOOL OnQuery();
	BOOL OnQueryRect();		//rectangle query was started via QueryButton in panel

	BOOL OnCopyRO();
	BOOL OnCopyingRO();

	BOOL OnShowSuspect();

	BOOL CanBack();
	BOOL CanForward();
	BOOL CanNavigate();
	BOOL CanSendAudioSamples();
	BOOL CanSuspect();


	CIPCSequenceRecord* GetNextSequence(WORD wSequenceNr);
	CIPCSequenceRecord* GetPrevSequence(WORD wSequenceNr);

	void CalcPreAlarm(int iIndex,CWordArray& numbers);
	void CalcAlarm(int iIndex,CWordArray& numbers);
	__int64 CalcBackup(const CSystemTime& stStart,
				       const CSystemTime& stEnd,
					   CDWordArray& dwaSequences);
	

	void CalcBackupAlarm(CDWordArray& dwaSequences);
	void RequestDeleteSequences(CWordArray& wNumbers);
	WORD GetNextTrackSequence();
	WORD GetPrevTrackSequence();
	CSystemTime GetSequenceEndtime(WORD wSequenceNr);

	BOOL UpdatePanel();

	static CString NameOfEnum(PlayStatus ps);

	BOOL DecodeMpeg(CIPCPictureRecord pict, DWORD dwX, DWORD dwY);
//methods for rectangle database search
	void AddRectToDatabaseSearch(CIPCFields &fields);

	// Generated message map functions
protected:
	CString GetAudioCompressionType();
	//{{AFX_MSG(CPlayWindow)
	//}}AFX_MSG
	afx_msg long OnPlayRealTime(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	// data member
private:
	CIPCArchivRecord	m_ArchivRecord;
	CIPCSequenceRecords	m_Sequences;
	CEvent				m_evRequestSequenceList;

	BOOL		m_bCanMDTrack;
	BOOL		m_bSuspect;
	BOOL		m_bManipulated;
	BOOL        m_bGetPicture;
	BYTE		m_bBackupSymbol;

	PlayStatus	m_LastPlayStatusFromCmd;
	PlayStatus	m_LastPlayStatus;
	WORD		m_wLastSequenceRequest;
	DWORD		m_dwLastRecordRequest;
	DWORD		m_dwLastNewRecordRequest;
	DWORD		m_dwLastCamRequest;

	WORD				m_wCurrentSequence;
	DWORD				m_dwCurrentRecord;
	DWORD				m_dwCurrentAudioRecord;
	DWORD               m_dwRequestedRecord; // bei gemischten Archiven
	WORD                m_wRequestedSequence; // bei gemischten Archiven
	
	BOOL				m_bNavigateToLastRecord;
	DWORD				m_dwNavigateFailedStart;
	BOOL				m_bEjectAfterBackupRemoved;
	
	CWordArray			m_waSequencesToDelete;
	CCriticalSection    m_csSequencesToDelete;
	DWORD				m_dwStartDeleting;
	DWORD				m_dwStartDeleted;

	DWORD				m_dwStartQueryFailed;

	PlayStatus			m_PlayStatus;

	//indicates playstatus during rectsearch via buttons 
	RectPlayStatus		m_RectPlayStatus; 
	CString				m_sQuery;
	BOOL				m_bPlayNextRect;  //if TRUE, query is confirmed, go on with play rectangle records
	BOOL				m_bStopRectPlay;  //if TRUE, stop all rectangle plays
	
	//if FALSE, no next rectangle query results available, disable all forward buttons
	//is set to TRUE in "IndicateQuery" and set to FALSE in "ConfirmQuery"
	//variable is used in "CanForward" on rectangle search
	BOOL				m_bIsNextForwardQueryRectResult; 
	//if FALSE, no next rectangle query results available, disable all back buttons
	//is set to TRUE in "IndicateQuery" and set to FALSE in "ConfirmQuery"
	//variable is used in "CanBack" on rectangle search
	BOOL				m_bIsNextBackQueryRectResult;

	//time from query, started with QueryButton on panel when rectangle in on image
	CSystemTime			m_stQueryRect;

	int					m_iFFStepWidth;
	int					m_iFFPicts;

	BOOL				m_bCanMilliSeconds;  //gibt es im BackupArchiv (readonly) den Eintrag "MS"

	// für die Alarmzeitberechnung
	BOOL				m_bGetSequenceEndtime;
	CEvent				m_evGetSequenceEndtime;
	CSystemTime			m_stGetSequenceEndtime;
	
	// für das Echtzeit Apspielen von Datenbank Bildern
	CPlayRealTime*		m_pPlayRealTime;
	// für das Abspielen von gemischten Archiven mit Audio
	CIPCMediaSampleRecord *m_pAudioRecord;
	bool                m_bSetSyncPoint;
	bool                m_bDoPrefetch;
	bool                m_bContainsAudio;
	bool                m_bReInitAudio;
	CString             m_sAudioCompressionType;
	CPtrList            m_PictBuffer;

//TODO TKR für Sync Abspielen
/*
	//für das synchrone Abspielen mehrerer PlayWindows
	BOOL				m_bCanMakeSyncBigOne;
*/
	BOOL m_bCanTashaRectangleSearch;
};
/////////////////////////////////////////////////////////////////////////////
inline CPlayWindow::PlayStatus CPlayWindow::GetPlayStatus() const
{
	return m_PlayStatus;
}
/////////////////////////////////////////////////////////////////////////////
inline CPlayWindow::PlayStatus CPlayWindow::GetLastPlayStatus() const
{
	return m_LastPlayStatus;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CPlayWindow::IsMD() const
{
	if (m_ArchivRecord.IsBackup())
	{
		CAutoCritSec acs((CCriticalSection*)&m_Sequences.m_cs);
		if (m_Sequences.GetSize())
		{
			CIPCSequenceRecord* pSR = m_Sequences.GetAtFast(0);
			return pSR->IsRing();
		}
	}
	return m_ArchivRecord.IsRing(); 
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CPlayWindow::IsAlarm() const
{
//	BYTE bType =  HIBYTE(m_ArchivRecord.GetSubID());
	if (m_ArchivRecord.IsBackup())
	{
		CAutoCritSec acs((CCriticalSection*)&m_Sequences.m_cs);
		if (m_Sequences.GetSize())
		{
			CIPCSequenceRecord* pSR = m_Sequences.GetAtFast(0);
			return pSR->IsAlarm() || pSR->IsSafeRing() || pSR->IsSuspect();
		}
	}
	return m_ArchivRecord.IsAlarm();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CPlayWindow::IsBackup() const
{
	return m_ArchivRecord.IsBackup();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CPlayWindow::IsDeleting() const
{
	return    (PS_DELETE_ALARM <= m_PlayStatus) 
		   && (m_PlayStatus <= PS_DELETED);
}
/////////////////////////////////////////////////////////////////////////////
inline CPlayWindow::RectPlayStatus CPlayWindow::GetRectPlayStatus() const
{
	return m_RectPlayStatus;
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL CPlayWindow::GetNextQueryRectResult()
{
	return m_bIsNextForwardQueryRectResult && m_bIsNextBackQueryRectResult;
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYWINDOW_H__6B5BBEDD_9757_11D3_A870_004005A19028__INCLUDED_)
