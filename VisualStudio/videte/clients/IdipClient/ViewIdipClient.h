// ViewIdipClient.h : interface of the CViewIdipClient class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _VIEW_IDIP_CLIENT_H
#define _VIEW_IDIP_CLIENT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "images.h"
#include "WndSmall.h"

class CWndSmall;
class CWndSmallArray;
class CWndText;
class CWndHTML;
class CWndLive;
class CWndPlay;
class CWndReference;
class CWndAlarmList;
class CDlgBackup;

#define SDOW_DISPLAY	0x01
#define SDOW_REACTIVATE	0x02
#define SDOW_CLEARSTA	0x04
/////////////////////////////////////////////////////////////////////////////
// CPictureData
/////////////////////////////////////////////////////////////////////////////
#define LIVE_PICTURE	1
#define PLAY_PICTURE	2

class CPictureData
{
public:
	CPictureData()
	{
		m_pPict = NULL;
		m_nType = 0;
	}
	virtual ~CPictureData()
	{
		WK_DELETE(m_pPict);
	}
	CIPCPictureRecord*m_pPict;
	int		m_nType;
	WORD	m_wServer;
};

class CLivePictureData : public CPictureData
{
public:
	CLivePictureData(const CIPCPictureRecord &pict, DWORD dwX, DWORD dwY, WORD wServer, CSecID idCam, CSecID idArchive)
	{
		m_pPict = new CIPCPictureRecord(pict);
		m_nType = LIVE_PICTURE;
		m_dwX = dwX;
		m_dwY = dwY;
		m_wServer = wServer;
		m_idCam = idCam;
		m_idArchive = idArchive;
	}
	DWORD	m_dwX, m_dwY;
	CSecID	m_idCam;
	CSecID  m_idArchive;
};

class CPlayPictureData : public CPictureData
{
public:
	CPlayPictureData(DWORD dwRecordNr, DWORD dwNrOfRecords, const CIPCPictureRecord &pict, const CIPCFields& fields, BOOL bShowQueryResults, WORD wServer, WORD wArchiveNr, WORD wSequenceNr)
	{
		m_pPict = new CIPCPictureRecord(pict);
		m_Fields = fields;
		m_nType = PLAY_PICTURE;
		m_dwRecordNr = dwRecordNr;
		m_dwNrOfRecords = dwNrOfRecords;
		m_bShowQueryResults = bShowQueryResults;
		m_wServer = wServer;
		m_wArchiveNr = wArchiveNr;
		m_wSequenceNr = wSequenceNr;
	}

	CIPCFields m_Fields;
	DWORD m_dwRecordNr;
	DWORD m_dwNrOfRecords;
	BOOL  m_bShowQueryResults;
	WORD  m_wArchiveNr;
	WORD  m_wSequenceNr;
};

typedef CPictureData* CPictureDataPtr;
WK_PTR_LIST_CS(CPictureDataList, CPictureDataPtr, CPictureDataListCS);

/////////////////////////////////////////////////////////////////////////////
// CPictureBufferThread
/////////////////////////////////////////////////////////////////////////////
class CPictureBufferThread : public CWK_Thread
{
public:
	CPictureBufferThread(const CString sName, LPVOID lpContext=NULL);
	virtual ~CPictureBufferThread();
	BOOL PostThreadMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	void LivePictureData(const CIPCPictureRecord &pict, DWORD dwX, DWORD dwY, WORD wServer, CSecID idCam, CSecID idArchive);
	void PlayPictureData(DWORD dwRecordNr, DWORD dwNrOfRecords, const CIPCPictureRecord &pict, const CIPCFields& fields, BOOL  bShowQueryResults, WORD wServer, WORD wArchiveNr, WORD wSequenceNr);

protected:
	virtual	BOOL Run(LPVOID lpContext);
	LRESULT OnPictureData(WPARAM, LPARAM);

private:
	DWORD m_dwThreadID;
	CPictureDataListCS m_Pictures;
};

class CDlgExternalFrame;
class CViewIdipClient : public CView 
{
	friend class CViewPrintPreview;
enum EnumViewMode
{
	VIEW_1x1,
	VIEW_1PLUS,
	VIEW_XxX
};

protected: // create from serialization only
	CViewIdipClient();
	DECLARE_DYNCREATE(CViewIdipClient)

// Attributes
public:
	CIdipClientDoc*	GetDocument();
	CWndSmallArray*	GetSmallWindows();
	CWndLive*		GetWndLive(WORD wServerID, CSecID camID);
	CWndLive*		GetWndLiveCmdActive();
	CWndSmall*		GetWndSmallCmdActive();
	CWndSmall*		GetWndSmallExclusive();
	int				GetNrOfLiveWindows();
	int				GetNrOfLiveWindows(WORD wServerID);
	int				GetNrOfWindows(WORD wServerID, EnumWndSmallType nType);
	CWndHTML*		GetWndHtml(WORD wServerID, CSecID idInput);
	CWndHTML*		GetWndHtml(WORD wServerID, const CString& sURL);
	CWndText*		GetWndText(WORD wServerID, const CString& sText);
	CWndReference*	GetWndReference(WORD wServerID, const CString& sCameraName);
	BOOL			Is1x1() const;
	inline BOOL		IsView1plus() const;
	int				GetRequestedFPS(WORD wServer, EnumWndSmallType wst=WST_ALL);
	void			CalculateRequestedFPS(CServer*pServer);
	inline CSecID	Get1plusWindowID() const;
	inline BOOL		IsWndSmallBig(CWndSmall*pWnd) const;
	inline BOOL		IsWndSmallCmdActive(CWndSmall*pWnd) const;
	BOOL			IsView1x1Allowed();
	BOOL			DoFullScreenOrView1PlusAtAlarm();
	// Recherche
	BOOL		    UseSingleWindow();
	BOOL			CanPlayRealtime();
	CWndPlay*		GetWndPlay(WORD wServerID, CSecID id);
	CWndPlay*		GetWndPlay(WORD wServerID, WORD wArchiv, WORD wSequence);
	CWndAlarmList*	GetWndAlarmList(WORD wServerID, WORD wArchiv, WORD wSequence);
	CWndPlay*		GetWndPlayOldest(CSystemTime& t);
	CWndPlay*		GetWndPlayYoungest(CSystemTime& t);
	CDlgBackup*		GetDlgBackup();
	void			SetView1(); //is only used in 1-mode and switching cams via numblock
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewIdipClient)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL
public:
	virtual ~CViewIdipClient();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Implementation
// Generated message map functions
protected:
	//{{AFX_MSG(CViewIdipClient)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnView1();
	afx_msg void OnView22();
	afx_msg void OnView33();
	afx_msg void OnView44();
	afx_msg void OnUpdateView1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateView22(CCmdUI* pCmdUI);
	afx_msg void OnUpdateView33(CCmdUI* pCmdUI);
	afx_msg void OnUpdateView44(CCmdUI* pCmdUI);
	afx_msg void OnVideoFein();
	afx_msg void OnUpdateVideoFein(CCmdUI* pCmdUI);
	afx_msg void OnVideoGrob();
	afx_msg void OnUpdateVideoGrob(CCmdUI* pCmdUI);
	afx_msg void OnVideoCsb();
	afx_msg void OnUpdateVideoCsb(CCmdUI* pCmdUI);
	afx_msg void OnVideoSave();
	afx_msg void OnUpdateVideoSave(CCmdUI* pCmdUI);
	afx_msg void OnViewOriginal();
	afx_msg void OnUpdateViewOriginal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSequenze(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnView1Plus();
	afx_msg void OnUpdateView1Plus(CCmdUI* pCmdUI);
	afx_msg void OnViewMonitor();
	afx_msg void OnViewTitleOfSmallwindows();
	afx_msg void OnViewTitleInPicture();
	afx_msg void OnUpdateViewTitleOfSmallwindows(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTitleInPicture(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSmallClose(CCmdUI *pCmdUI);
	afx_msg void OnFileOpenDisk();
	afx_msg void OnUpdateFileOpenDisk(CCmdUI *pCmdUI);
	afx_msg void OnCloseWndSmallAll();
	afx_msg void OnUpdateCloseWndSmallAll(CCmdUI *pCmdUI);
	afx_msg void OnCloseWndSmallAllLive();
	afx_msg void OnUpdateCloseWndSmallAllLive(CCmdUI *pCmdUI);
	afx_msg void OnCloseWndSmallAllPlay();
	afx_msg void OnUpdateCloseWndSmallAllPlay(CCmdUI *pCmdUI);
	afx_msg void OnSequenceOver();
	afx_msg void OnUpdateSequenceOver(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNavigateRange(CCmdUI *pCmdUI);
	afx_msg void OnNavigateRange(UINT uID);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFilePrintPreview();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);	// splitten und in UpdateAllViewsAsync aufteilen
	afx_msg void OnEditHtmlPage();
	afx_msg void OnUpdateEditHtmlPage(CCmdUI* pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnEditCreateHtmlCameraPage();
	afx_msg void OnUpdateEditCreateHtmlCameraPage(CCmdUI* pCmdUI);
	afx_msg void OnEditCreateHtmlHostPage();
	afx_msg void OnUpdateEditCreateHtmlHostPage(CCmdUI* pCmdUI);
	afx_msg void OnEditHtmlMapProperties();
	afx_msg void OnUpdateEditHtmlMapProperties(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
public:
	void OnIdle();
	void DeleteWndSmall(WORD wServerID, CSecID id); 
	void SetWndSmallView1(CWndSmall* pDW);
	void SetWndSmallCmdActive(CWndSmall* pSW);
	void SetWndSmallBig(CWndSmall* pSW);
	BOOL SetWndLiveRequested(CWndLive* pDW);
	BOOL FindNewCmdActiveWndSmall(CRuntimeClass*pRTC=NULL);
	void OnToggle();
	void SetStore(BOOL bOn);
	void TraceSmallWindows();
	void ShowMapWindow();
	BOOL HaveAllWindowsGotAPicture(WORD wServerID);
	void RedrawSmallWindows();
	void LockSmallWindows(LPCTSTR sFkt=NULL);
	void UnlockSmallWindows();
	void OnTab(BOOL bNext);
	void LockSmallWindowResizing(BOOL bLock);

	// Recherche
	void OnBackupClosed();
	void CloseWndPlayAll();
	void CloseWndAll();
	void ShowHideRectangle();
	void OpenFile();
	void LivePictureData(WORD wServerID, const CIPCPictureRecord &pict, CSecID id, DWORD dwMDX, DWORD dwMDY, CSecID idArchive);
	void PlayPictureData(DWORD dwRecordNr, DWORD dwNrOfRecords, const CIPCPictureRecord &pict, CWndPlay*pPW, const CIPCFields& fields, BOOL bShowQueryResults, WORD wServer, WORD wArchiveNr, WORD wSequenceNr);

	void ConfirmQuery(DWORD dwUserID,
					  WORD wServerNr,
                      WORD wArchivNr,
					  WORD wSequenceNr, 
					  DWORD dwRecordNr,
					  int iNumFields,
					  const CIPCField fields[]);

protected:
	BOOL		CanEditHTMLWindow();
	void		SetXWin(int i);
	void		SetWindowSize(BOOL bFromOnSize=FALSE);
	void		SetSmallWindowPositions(CWndSmallArray& sws);
	void		SetSmallWindowPositions1pX(CWndSmallArray& sws, CWnd *pExternal=NULL);
	void		SetSmallWindowPositionsXxX(CWndSmallArray& sws, CWnd *pExternal=NULL);
	void		AddWndLive(CServer* pServer, const CIPCOutputRecord & rec); 
	void		SingleWndLive(CServer* pServer, const CIPCOutputRecord & rec); 
	void		CloseWndSmallAll(WORD wServerID); 
	void		DeactivateCameras(WORD wServerID); 
	CWndHTML*	AddHtmlWindow(CServer* pServer, CSecID id, const CString& sURL);
	void		AddTextWindow(CServer* pServer, CSecID id, const CString& sMessage);
	void		AddReferenceWindow(CServer* pServer, CSecID id, const CString& sCameraName);
	void		AddWndSmall(CWndSmall* pSW, BOOL bIsAlarm = FALSE);
	void		SortSmallWindowsByID(CWndSmallArray& sw);
	void		SortSmallWindowsByIDandTime(CWndSmallArray& sw);
	void		SortSmallWindowsByMonitor(CWndSmallArray& sw, DWORD dwPrefer=0xffffffff);
	void		CheckSmallWindows();
	void		SetWndLiveRequested(WORD wServerID, CSecID id);
	void		Sequence();
	CWndLive*	GetNextSequenceWndLive(WORD wServerID, CWndLive* pCurrent);
	WORD		GetNextSequenceServer(WORD wServerID, int start);
	int			GetNrOfRequestingWindows();
	void		OnDeleteWindow(CWndSmall* pSW);
	void        CheckAndSetActiveSmallWindow();
	void		Cancel1PlusMode();
	void		SetAppropriateQuadViewMode();
	BOOL		OnUpdateViewXX(int);
	void		SetWindowSizeAtResizing();
	void		SetWindowSizeReplacing(int iMaxVisible);
	void		CloseWndSmallAllOfType(EnumWndSmallType type);
	int			HasWndSmallOfType(EnumWndSmallType type);
	void		OnUpdateSyncDlgsOnAllWndPlay();

	//Recherche
	void AddWndPlay(CServer* pServer, CIPCSequenceRecord* pSequence);
	void CloseWndPlayAllOfArchive(WORD wServerID, WORD wArchivID); 
	void CheckSmallWindowsFullScreen();
	void OnArchivBackup(CServer* pServer);
	void OnSearchResult();

	// data members
public:
	CDlgBackup*	m_pDlgBackup;
	BOOL		m_b1to1;
private:
	CImageList	m_Images;
	CBrush		m_KeyBrush;
	CBrush		m_CinemaBrush;
	CFont		m_TitleFont;
	int			m_iFontHeight;	
	
	CWndHTML*	m_pHostMapWindow;
	CWndSmallArray	m_SmallWindows;		// alle Fenster, die echten Objekte
										// verweist auf Objekte in m_SmallWindows
	CWndSmall*   m_pWndSmallCmdActive;
	CWndSmall*   m_pWndSmallBig;

	COLORREF	m_crKeyColor;
	int			m_iXWin;
	BOOL		m_bView1Plus;
	DWORD		m_dwIDCounter;
	DWORD		m_dwTimerCounter;
	BOOL		m_bSizing;
	//Recherche
	BOOL		m_bSequenceOver;
	BOOL		m_bSequenceRealtime;
	BOOL		m_bUncheckNavigateForward;
	BOOL		m_bUncheckNavigateFastForward;
	BOOL		m_bUncheckNavigateBack;
	BOOL		m_bUncheckNavigateFastBack; //wenn während des Schließens eines WndPlay dieses noch
									   //abgespielt wird, dann das Abspielen erst stoppen, weil sonst
									   //der Abspielbutton gedrückt bleibt.

	BOOL		m_bPreviewPrinting;
	CPictureBufferThread*m_pPictureBufferThread;
	CQueryResultListCS m_QueryResultsToShow;
	BOOL		m_bLockSmallWindowResize;

	friend class CDlgExternalFrame;
	friend class CWndImageRecherche;
	friend class CWndHTML;
	friend class CWndSmall;
	friend class CWndLive;
	friend class CWndImage;
	friend class CWndPlay;
	friend class CWndReference;
	friend class CWndCoco;
	friend class CMainFrame;
	friend class CIdipClientDoc;
public:
};

#ifndef _DEBUG  // debug version in IdipClientView.cpp
inline CIdipClientDoc* CViewIdipClient::GetDocument()
   { return (CIdipClientDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
inline BOOL	CViewIdipClient::IsView1plus() const
{
	return m_bView1Plus;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CViewIdipClient::Get1plusWindowID() const
{
	if (m_pWndSmallBig)
	{
		return m_pWndSmallBig->GetID();
	}
	return CSecID();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CViewIdipClient::IsWndSmallCmdActive(CWndSmall*pWnd) const
{
	return (m_pWndSmallCmdActive == pWnd) ? TRUE : FALSE;
}

inline BOOL CViewIdipClient::IsWndSmallBig(CWndSmall*pWnd) const
{
	return (m_pWndSmallBig == pWnd) ? TRUE : FALSE;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
