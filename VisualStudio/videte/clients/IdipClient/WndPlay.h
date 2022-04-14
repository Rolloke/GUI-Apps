/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WndPlay.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/WndPlay.h $
// CHECKIN:		$Date: 5.05.06 13:28 $
// VERSION:		$Revision: 36 $
// LAST CHANGE:	$Modtime: 5.05.06 11:52 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef _WndPlay_H
#define _WndPlay_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WndImageRecherche.h"
#include "DibData.h"
#include "QueryRectTracker.h"
#include "PlayRealTime.h"

#define DW_CHECK_PLAY_STATUS    10
#define DW_INVALIDATE_RECT    11

#define RECTSEARCHENDREACHED  10

/////////////////////////////////////////////////////////////////////////////
// CWndPlay window
class CIPCAudioIdipClient;
class CWndPlay : public CWndImageRecherche
{
	DECLARE_DYNAMIC(CWndPlay)
	// Construction
public:
	CWndPlay(CServer* pServer, const CIPCSequenceRecord& s);

	// Attributes
public:
	virtual	CSecID	GetID();
	inline 	int		GetCurrentRecordNr() const;
	inline  WORD	GetSequenceNr() const;
	inline  WORD	GetArchivNr() const;
	inline  CRect	GetRectToQuery() const;
	inline  BOOL    IsInRectQuery() const;
		    BOOL	GetTimeStamp(CSystemTime& t) const;

	inline CRect	GetZoom() const;
	inline BOOL		ContainsAudio() const;
		   CString  GetArchiveName() const { return m_sArchivName; };
		   BOOL		IsQueryRectEmpty() const;
		   void		SetCanRectPlayForward(BOOL bValue);
		   void		SetCanRectPlayBack(BOOL bValue);
		   void		SetNewSequence(CIPCSequenceRecord* pSR);
	

	// Operations
public:
	// window creation
	virtual BOOL Create(const RECT& rect, CViewIdipClient* pParentWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// picture data
    BOOL PictureData(DWORD dwRecordNr,
					 DWORD dwNrOfRecords,
					 const CIPCPictureRecord &pict,
					 const CIPCFields& fields,
					 BOOL  bShowQueryResults);
    BOOL AudioData(DWORD dwRecordNr,
				   DWORD dwNrOfRecords,
				   const CIPCMediaSampleRecord &media,
				   int iNumFields,
				   const CIPCField fields[]);
	void CameraNames(int iNumFields,
					 const CIPCField fields[]);
	void NoPictureFoundForCamera(CSecID id);
	void RecordNrOutOfRange(CSecID id);
	void CannotReadPictureForCamera();
	void ConfirmRectQuery(DWORD dwUserID,
						WORD wArchivNr,
						WORD wSequenceNr, 
						DWORD dwRecordNr,
						int iNumFields,
						const CIPCField fields[]);
	void ConfirmQuery();

	virtual	void PopupMenu(CPoint pt);
	virtual	CSize GetPictureSize();
	virtual	void  Set1to1(BOOL b1to1);
			BOOL  Skip(BOOL bPlayOverSequenceBorder, int i=1);
	virtual BOOL  PreDestroyWindow(CViewIdipClient* pViewIdipClient);
	void  UpdateAudioState(CIPCAudioIdipClient*);
	void	SetSyncPlayStatus(PlayStatus ps);
	void	SetPlayStatus(PlayStatus psNew);
	void	NavigateEnd(); //calls OnNavigateEnd from outside of this class

// Overrides
	// ClassWizard generated virtual function overrides
	protected:

	// Overrides
public:
	virtual	BOOL	CanPrint();
	virtual	BOOL	IsWndPlay();
	virtual CString FormatComment();
	virtual CString FormatCommentHtml();
	virtual int     GetCameraNr();
	virtual CString GetDate();
	virtual CString GetTime();
	virtual CString GetCompression();
	virtual CString FormatPicture(BOOL bTabbed = FALSE);
	virtual int		GetRecNo();
	virtual int		GetRecCount();
			BOOL	CanNavigate() const;
			BOOL	Navigate(const CIPCSequenceRecord& s,
							 int nRecNr); 
	virtual BOOL	Navigate(int nRecNr,
							 int nRecCount=0); 
	BOOL			CheckForSequenceBorder(int nRecCount, int nRecNr, DWORD dwCamID);
	virtual int		GetNrDibs();
	virtual CString	GetName();
	virtual void	OnIdle();


protected:
	virtual	CString	GetTitleText(CDC* pDC);
	virtual	void	OnSetWindowSize();
			void	DrawRectToQuery(CDC* pDC);
			void	DecreasePictureRequests();
			BOOL	CanPlayBack();
			BOOL	CanPlayForward();
			BOOL	CanRectPlayForward();
			BOOL	CanRectPlayBack();
			void	ResetTracker();
			void	AddRectToDatabaseSearch(CIPCFields &fields);


// Implementation
public:
	BOOL ScrollWndPlay(CPoint point = CPoint(0,0));
	void ZoomWndPlay(const CPoint &point);

	virtual ~CWndPlay();

	//functions for realtime playback
	PlayStatus		GetPlayStatus();
	RectPlayStatus		GetRectPlayStatus();
	CPlayRealTime*	GetPlayRealTime();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndPlay)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnChangeVideo(WPARAM, LPARAM);
	afx_msg LRESULT OnChangeAudio(WPARAM, LPARAM);
	afx_msg LRESULT OnRedrawVideo(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdateDialogs(WPARAM, LPARAM);
	afx_msg void OnDestroy();
	afx_msg void OnNavigatePos1();
	afx_msg void OnUpdateNavigatePos1(CCmdUI* pCmdUI);
	afx_msg void OnNavigateEnd();
	afx_msg void OnUpdateNavigateEnd(CCmdUI* pCmdUI);
	afx_msg void OnNavigateNext();
	afx_msg void OnUpdateNavigateNext(CCmdUI* pCmdUI);
	afx_msg void OnNavigatePrev();
	afx_msg void OnUpdateNavigatePrev(CCmdUI* pCmdUI);
	afx_msg void OnNavigateForward();
	afx_msg void OnUpdateNavigateForward(CCmdUI* pCmdUI);
	afx_msg void OnNavigateReview();
	afx_msg void OnUpdateNavigateReview(CCmdUI* pCmdUI);
	afx_msg void OnNavigateStop();
	afx_msg void OnUpdateNavigateStop(CCmdUI* pCmdUI);
	afx_msg void OnViewOriginal();
	afx_msg void OnUpdateViewOriginal(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnFileDib();
	afx_msg void OnFileHtml();
	afx_msg void OnFileJpg();
	afx_msg void OnDisplay1();
	afx_msg void OnUpdateDisplay1(CCmdUI* pCmdUI);
	afx_msg void OnDisplay4();
	afx_msg void OnUpdateDisplay4(CCmdUI* pCmdUI);
	afx_msg void OnDisplay9();
	afx_msg void OnUpdateDisplay9(CCmdUI* pCmdUI);
	afx_msg void OnEditComment();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateFileDib(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileDisk(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileHtml(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileJpg(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDisplayZoomOut();
	afx_msg void OnUpdateDisplayZoomOut(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnNavigateSkipForward();
	afx_msg void OnUpdateNavigateSkipForward(CCmdUI* pCmdUI);
	afx_msg void OnNavigateSkipBack();
	afx_msg void OnUpdateNavigateSkipBack(CCmdUI* pCmdUI);
	afx_msg void OnNavigateFastBack();
	afx_msg void OnUpdateNavigateFastBack(CCmdUI* pCmdUI);
	afx_msg void OnNavigateFastForward();
	afx_msg void OnUpdateNavigateFastForward(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateSmallContext(CCmdUI* pCmdUI);
	afx_msg LRESULT OnDecodeVideo(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg LRESULT OnPlayRealTime(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateEditComment(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Implementation
protected:
			void Init(CServer* pServer);
	virtual void OnDraw(CDC* pDC);
			void OnDrawDIBs(CDC* pDC, CRect rect);
	virtual int  PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim = FALSE);
	virtual	void PrintDIBs(CDC* pDC,CRect rect,int iSpace);
			void DrawFace(CDC* pDC);
			void DrawCrosses(CDC* pDC, WORD wX, WORD wY, COLORREF col);

			BOOL IsNextSequence(); //check, if there is a next sequence 
								   //of current archiv available
			void CheckPlayStatus();
	virtual UINT GetToolBarID();

	// saving operations
		BOOL SaveDIB(const CString& sFileName);
		BOOL SaveJPG(const CString& sFileName);
		BOOL SaveRAW(const CString& sFileName);
		BOOL SaveHTML(const CString& sHTMLName);
		CString GetSaveFilename(UINT nIDFilterText, LPCTSTR sExt);
		void ConfirmSaving(BOOL bOK, const CString& sFileName);
		CString BuildExtraData();

	// dib queue
		void EnsureDibQueueNotEmpty();
		CDibData* HittestDibDataToolTip(CPoint pt);

	//for rectangle search
	void DoDrawAllCrosses(CDC* pDC);
	void DrawAllCrosses(CDC* pDC, WORD wX, WORD wY, COLORREF col, BOOL bLines = FALSE);
	BOOL SetMDValues(WORD wX, WORD wY);
	BOOL SetAllMDValues(WORD wX, WORD wY);	
	BOOL RemoveAllMDValues();
	BOOL AllMDsOutsideRect(int iAllMDs, CRect rcRectangle);
	BOOL IsAlarm();
	void KillQueryRectTimer();
	void SetIsInRectQuery(BOOL bIsInRectQuery);
	BOOL IsRectQueryWithQueryDlg(); 

	// data member
protected:
	// navigation info
	CIPCSequenceRecord m_SequenceRecord;
	DWORD              m_dwCurrentRecord;
	DWORD              m_dwRequestedRecord;

	CString            m_sArchivName;
	CString            m_sComment;
	CString            m_sError;
	CString            m_sLastCompressionType;

	BOOL               m_bWasSomeTimesActive;
	BOOL			   m_bCorrectRequestRecordNr;

#ifdef TEST_MIXED_ARCHIVE_CUEUING
	CIPCPictureRecords       m_PictureCue;
	CIPCMediaSampleRecordsEx m_AudioCue;
	UINT                     m_nCueTimer;
#endif


	// for fast forward/review
	int            m_iPictureRequests;
	PlayStatus     m_PlayStatus;
	RectPlayStatus m_RectPlayStatus; //wird zum disablen der vorwärts/rückwärts Einzelschrittbutton
									 //benötigt, wenn eine Rechtecksuche kein weiteres Ergebnis
									 //mehr liefert
	BOOL           m_bUseOneWindow;
	BOOL           m_bNavigateLast;
	BOOL           m_bSetSyncPoint;
	BOOL           m_bGetPicture;
	BOOL           m_bContainsAudio;	
	BOOL           m_bContainsVideo;
	BOOL           m_bAudioBusy;
	BOOL           m_bReinitDecoder;
	

	CDibData*		m_pDibDataTooltip;
	CDibDatas		m_DibDatas;
	int				m_nDibData;

	// zoomcounter, max. 3 
	int		m_iCountZooms;

	//members for rectangle search
	CWordArray	m_waAllMD_X; 
	CWordArray	m_waAllMD_Y; 
	WORD		m_wMD_X;		
	WORD		m_wMD_Y;
	BOOL		m_bCanRectPlayForward;
	BOOL		m_bCanRectPlayBack;
	CToolTipCtrl		m_ToolTip;
	
	friend class CViewIdipClient;
	friend class CMainFrame;

private:
	//für das Echtzeit Apspielen von Datenbank Bildern
	CPlayRealTime*	    m_pPlayRealTime;
	CQueryRectTracker*	m_pQueryRectTracker; 
	BOOL				m_bIsInRectQuery;
	int					m_iRedrawRectWithText; //if rechtangle search takes too long, 
											   //redraw rectangle with text
public:
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CWndPlay::IsWndPlay()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CWndPlay::GetSequenceNr() const
{
	return m_SequenceRecord.GetSequenceNr();
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CWndPlay::GetArchivNr() const
{
	return m_SequenceRecord.GetArchiveNr();
}
/////////////////////////////////////////////////////////////////////////////
inline CRect CWndPlay::GetRectToQuery() const
{
	if(m_pQueryRectTracker)
	{
		return m_pQueryRectTracker->m_rect;
	}

	CRect rect(0,0,0,0);
	return rect;
}
/////////////////////////////////////////////////////////////////////////////
inline CRect CWndPlay::GetZoom() const
{
	return m_rcZoom;
}
/////////////////////////////////////////////////////////////////////////////
inline int	CWndPlay::GetCurrentRecordNr() const
{
	return m_dwCurrentRecord; 
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CWndPlay::ContainsAudio() const
{
	return m_bContainsAudio; 
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CWndPlay::IsInRectQuery() const
{
	return m_bIsInRectQuery;
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
