/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: displaywindow.h $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/displaywindow.h $
// CHECKIN:		$Date: 30.03.04 15:22 $
// VERSION:		$Revision: 92 $
// LAST CHANGE:	$Modtime: 30.03.04 15:10 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _DISPLAYWINDOW_H
#define _DISPLAYWINDOW_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DisplayWindow.h : header file
//

#include "ImageWindow.h"
#include "DibData.h"
#include "QueryRectTracker.h"
#include "PlayRealTime.h"

#define DW_CHECK_PLAY_STATUS    10

/////////////////////////////////////////////////////////////////////////////
// CDisplayWindow window
class CIPCAudioRecherche;
class CDisplayWindow : public CImageWindow
{
	// Construction
public:
	CDisplayWindow(CServer* pServer, CIPCArchivFileRecord* pFile);
	CDisplayWindow(CServer* pServer, const CIPCSequenceRecord& s);

	// Attributes
public:
	virtual	CSecID	GetID();
	inline 	int		GetCurrentRecordNr() const;
	inline  WORD	GetSequenceNr() const;
	inline  WORD	GetArchivNr() const;
	inline  CRect	GetRectToQuery() const;
		    BOOL	GetTimeStamp(CSystemTime& t) const;

	inline CRect	GetZoom() const;
	inline BOOL		ContainsAudio() const;
	CString         GetArchiveName() const { return m_sArchivName; };
	

	// Operations
public:
	// window creation
	virtual BOOL Create(const RECT& rect, CRechercheView* pParentWnd);

	// picture data
    BOOL PictureData(const CIPCArchivFileRecord &data, const CIPCPictureRecord &pict);
    BOOL PictureData(DWORD dwRecordNr,
					 DWORD dwNrOfRecords,
					 const CIPCPictureRecord &pict,
					 int iNumFields,
					  const CIPCField fields[]);
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
	virtual	void PopupMenu(CPoint pt);
	virtual	CSize GetPictureSize();
	virtual	void  Set1to1(BOOL b1to1);
			BOOL  Skip(BOOL bPlayOverSequenceBorder, int i=1);
	virtual BOOL  PreDestroyWindow(CRechercheView* pRechercheView);
	void  UpdateAudioState(CIPCAudioRecherche*);
	void	SetSyncPlayStatus(PlayStatus ps);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayWindow)
	protected:
	//}}AFX_VIRTUAL

	// Overrides
public:
	virtual	BOOL IsDisplayWindow();
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
	virtual int		GetNrDibs();
	virtual CString	GetName();
	virtual void	OnIdle();


protected:
	virtual CString GetTitleText(CDC* pDC);
	virtual void OnSetWindowSize();
	void DrawRectToQuery(CDC* pDC);
	void DecreasePictureRequests();
	BOOL CanPlayBack();
	BOOL CanPlayForward();
	DWORD GetRequestBaseRecord() const;

// Implementation
public:
	BOOL CanShowImageInOriginalSize();
	BOOL ScrollDisplayWindow(CPoint point = CPoint(0,0));
	void ZoomDisplayWindow(const CPoint &point);

	virtual ~CDisplayWindow();

	//functions for realtime playback
	PlayStatus		GetPlayStatus();
	CPlayRealTime*	GetPlayRealTime();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDisplayWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChangeVideo();
	afx_msg void OnChangeAudio();
	afx_msg void OnRedrawVideo();
	afx_msg void OnUpdateDialogs();
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
	afx_msg void OnFileDisk();
	afx_msg void OnFileHtml();
	afx_msg void OnFileJpg();
	afx_msg void OnFileJpx();
	afx_msg void OnDisplay1();
	afx_msg void OnUpdateDisplay1(CCmdUI* pCmdUI);
	afx_msg void OnDisplay4();
	afx_msg void OnUpdateDisplay4(CCmdUI* pCmdUI);
	afx_msg void OnDisplay9();
	afx_msg void OnUpdateDisplay9(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditComment();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateFileDib(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileDisk(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileHtml(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileJpg(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileJpx(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDisplayZoomOut();
	afx_msg void OnUpdateDisplayZoomOut(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNavigateSkipForward();
	afx_msg void OnUpdateNavigateSkipForward(CCmdUI* pCmdUI);
	afx_msg void OnNavigateSkipBack();
	afx_msg void OnUpdateNavigateSkipBack(CCmdUI* pCmdUI);
	afx_msg void OnNavigateFastBack();
	afx_msg void OnUpdateNavigateFastBack(CCmdUI* pCmdUI);
	afx_msg void OnNavigateFastForward();
	afx_msg void OnUpdateNavigateFastForward(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	BOOL OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnDecodeVideo();
	afx_msg long OnPlayRealTime(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	// Implementation
protected:
			void Init(CServer* pServer);
	virtual void OnDraw(CDC* pDC);
			void OnDrawDIBs(CDC* pDC, CRect rect);
	virtual int  PrintPicture(CDC* pDC,CRect rect, BOOL bUseDim = FALSE);
	virtual	void PrintDIBs(CDC* pDC,CRect rect,int iSpace);
			void DrawFace(CDC* pDC);
			void DrawCross(CDC* pDC, WORD wX, WORD wY);
			void DrawCrosses(CDC* pDC, WORD wX, WORD wY, COLORREF col);

			
			void CheckPlayStatus();
	virtual UINT GetToolBarID();

	// saving operations
			BOOL SaveJPX(const CString& sFileName);
			BOOL SaveDIB(const CString& sFileName);
			BOOL SaveJPG(const CString& sFileName);
			BOOL SaveRAW(const CString& sFileName);
			BOOL SaveHTML(const CString& sHTMLName);
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
	BOOL               m_bIFrame;

	// buffers for software decoding
	CIPCPictureRecord*      m_pPictureRecord;
	CIPCMediaSampleRecord*  m_pAudioRecord;
	CCriticalSection        m_CS;

#ifdef TEST_MIXED_ARCHIVE_CUEUING
	CIPCPictureRecords       m_PictureCue;
	CIPCMediaSampleRecordsEx m_AudioCue;
	UINT                     m_nCueTimer;
#endif

	// my decoders
	// mico
	CRect			m_rcZoom;	// Rechteckiger Zoomausschnitt
	CJpeg			m_Jpeg;

	// coco
	CH263			m_H263;
#ifndef _DTS
	// pt
	CPTDecoder		m_PTDecoder;
#endif

	// for fast forward/review
	int            m_iPictureRequests;
	PlayStatus     m_PlayStatus;
	BOOL           m_bUseOneWindow;
	BOOL           m_bNavigateLast;
	BOOL           m_bSetSyncPoint;
	BOOL           m_bGetPicture;
	BOOL           m_bContainsAudio;	
	BOOL           m_bContainsVideo;
	BOOL           m_bAudioBusy;
	

	CDibData*		m_pDibDataTooltip;
	CDibDatas		m_DibDatas;
	int				m_nDibData;

	// for check if image can be shown in original size
	BOOL	m_bImageOriginalSize;

	// zoomcounter, max. 3 
	int		m_iCountZooms;

	//members for rectangle search
	CWordArray	m_waAllMD_X; 
	CWordArray	m_waAllMD_Y; 
	WORD		m_wMD_X;		
	WORD		m_wMD_Y;
	
	friend class CRechercheView;
	friend class CMainFrame;

private:

	//für das Echtzeit Apspielen von Datenbank Bildern
	CPlayRealTime*	  m_pPlayRealTime;
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::IsDisplayWindow()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CDisplayWindow::GetSequenceNr() const
{
	return m_SequenceRecord.GetSequenceNr();
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CDisplayWindow::GetArchivNr() const
{
	return m_SequenceRecord.GetArchiveNr();
}
/////////////////////////////////////////////////////////////////////////////
inline CRect CDisplayWindow::GetRectToQuery() const
{
	if(m_pQueryRectTracker)
	{
		return m_pQueryRectTracker->m_rect;
	}

	CRect rect(0,0,0,0);
	return rect;
}
/////////////////////////////////////////////////////////////////////////////
inline CRect CDisplayWindow::GetZoom() const
{
	return m_rcZoom;
}
/////////////////////////////////////////////////////////////////////////////
inline int	CDisplayWindow::GetCurrentRecordNr() const
{
	return m_dwCurrentRecord; 
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CDisplayWindow::ContainsAudio() const
{
	return m_bContainsAudio; 
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
