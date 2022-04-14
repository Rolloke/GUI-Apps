// VisionView.h : interface of the CVisionView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _VISIONVIEW_H
#define _VISIONVIEW_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "images.h"
#include "DisplayWindow.h"
#include "HtmlWindow.h"
#include "TextWindow.h"
#include "ReferenceWindow.h"

#define SDOW_DISPLAY	0x01
#define SDOW_REACTIVATE	0x02
#define SDOW_CLEARSTA	0x04

class CStateCmdUI: public CCmdUI
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetRadio( BOOL bOn = TRUE );
	BOOL m_bEnabled;
	int  m_nChecked;
};

class CExternalFrameDlg;
class CVisionView : public CView 
{
enum EnumViewMode
{
	VIEW_1x1,
	VIEW_1PLUS,
	VIEW_XxX
};

protected: // create from serialization only
	CVisionView();
	DECLARE_DYNCREATE(CVisionView)

// Attributes
public:
	CVisionDoc*       GetDocument();
	CDisplayWindow*   GetDisplayWindow(WORD wServerID, CSecID camID);
	CDisplayWindow*   GetCmdActiveDisplayWindow();
	CSmallWindow*     GetCmdActiveSmallWindow();
	CSmallWindow*     GetExclusiveSmallWindow();
	int               GetNrOfDisplayWindows();
	int               GetNrOfDisplayWindows(WORD wServerID);
	CHtmlWindow*      GetHtmlWindow(WORD wServerID, CSecID idInput);
	CHtmlWindow*      GetHtmlWindow(WORD wServerID, const CString& sURL);
	CTextWindow*      GetTextWindow(WORD wServerID, const CString& sText);
	CReferenceWindow* GetReferenceWindow(WORD wServerID, const CString& sCameraName);
	inline BOOL       Is1x1() const;
	inline BOOL       IsView1plus() const;
	int               GetRequestedFPS(WORD wServer);
	inline CSecID     Get1plusWindowID() const;
	BOOL              IsView1x1Allowed();

// Operations
public:
	void DeleteSmallWindow(WORD wServerID, CSecID id); 
	void SetSmallWindowView1(CSmallWindow* pDW);
	
	void SetCmdActiveSmallWindow(CSmallWindow* pSW);
	void SetBigSmallWindow(CSmallWindow* pSW);
	BOOL SetRequestedDisplayWindow(CDisplayWindow* pDW);

	void OnToggle();
	void SetStore(BOOL bOn);

	void TraceSmallWindows();
	void ShowMapWindow();

	BOOL HaveAllWindowsGotAPicture(WORD wServerID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisionView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVisionView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CVisionView)
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
	afx_msg void OnMove(int x, int y);
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
	afx_msg void OnUpdateViewTitleOfSmallwindows(CCmdUI* pCmdUI);
	//}}AFX_MSG
#ifdef _HTML_ELEMENTS
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
#endif
	DECLARE_MESSAGE_MAP()


	// Implementation
protected:
	BOOL CanEditHTMLWindow();
	void SetXWin(int i);
	void SetWindowSize(BOOL bFromOnSize=FALSE);
	void SetSmallWindowPositions(CSmallWindows& sws);
	void SetSmallWindowPositions1pX(CSmallWindows& sws);
	void SetSmallWindowPositionsXxX(CSmallWindows& sws, CWnd *pExternal=NULL);
	void AddDisplayWindow(CServer* pServer, const CIPCOutputRecord & rec); 
	void SingleDisplayWindow(CServer* pServer, const CIPCOutputRecord & rec); 
	void DeleteSmallWindows(WORD wServerID); 
	void DeactivateCameras(WORD wServerID); 
	void PictureData(WORD wServerID, const CIPCPictureRecord &pict,CSecID id);
	CHtmlWindow* AddHtmlWindow(CServer* pServer, CSecID id, const CString& sURL);
	void AddTextWindow(CServer* pServer, CSecID id, const CString& sMessage);
	void AddReferenceWindow(CServer* pServer, CSecID id, const CString& sCameraName);
	void AddSmallWindow(CSmallWindow* pSW, BOOL bIsAlarm = FALSE);
	void SortSmallWindowsByID(CSmallWindows& sw);
	void SortSmallWindowsByIDandTime(CSmallWindows& sw);
	void SortSmallWindowsByMonitor(CSmallWindows& sw, DWORD dwPrefer=0xffffffff);
	void CheckSmallWindows();
	void SetRequestedDisplayWindow(WORD wServerID, CSecID id);
	void Sequence();
	CDisplayWindow* GetNextSequenceDisplayWindow(WORD wServerID, CDisplayWindow* pCurrent);
	WORD			GetNextSequenceServer(WORD wServerID, int start);
	int  GetNrOfRequestingWindows();
	void OnDeleteWindow(CSmallWindow* pSW);
	void CheckAndSetActiveSmallWindow();
	void Cancel1PlusMode();
	void SetAppropriateQuadViewMode();
	BOOL OnUpdateViewXX(int);
	void SetWindowSizeAtResizing();
	void SetWindowSizeReplacing(int iMaxVisible);

#ifndef _DTS
	void SetDisplayOutputWindow(DWORD dwOptions);
	void ClearCocoWasSomeTimesActive(CSecID camID);
	void ClearDisplayOutputWindow();
#endif

	// data members
private:
	CImageList	m_Images;
	CBrush		m_KeyBrush;
	CBrush		m_CinemaBrush;
	CFont		m_TitleFont;
	int			m_iFontHeight;	
	
	CHtmlWindow*	m_pHostMapWindow;
	CSmallWindows	m_SmallWindows;		// alle Fenster, die echten Objekte
										// verweist auf Objekte in m_SmallWindows
	CSmallWindow*   m_pCmdActiveSmallWindow;
	CSmallWindow*   m_pBigSmallWindow;

	COLORREF	m_crKeyColor;
	int			m_iXWin;
	BOOL		m_b1to1;
	BOOL		m_bView1Plus;
	DWORD		m_dwIDCounter;
	UINT		m_uTimerID;
	DWORD		m_dwTimerCounter;
	BOOL     m_bSizing;
		
	friend class CExternalFrameDlg;
	friend class CHtmlWindow;
	friend class CSmallWindow;
	friend class CDisplayWindow;
	friend class CReferenceWindow;
	friend class CCocoWindow;
	friend class CMainFrame;
	friend class CVisionDoc;
};

#ifndef _DEBUG  // debug version in VisionView.cpp
inline CVisionDoc* CVisionView::GetDocument()
   { return (CVisionDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
inline BOOL	CVisionView::Is1x1() const
{
	return m_iXWin == 1;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CVisionView::IsView1plus() const
{
	return m_bView1Plus;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CVisionView::Get1plusWindowID() const
{
	if (m_pBigSmallWindow)
	{
		return m_pBigSmallWindow->GetID();
	}
	return CSecID();
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
