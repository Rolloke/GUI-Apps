/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RechercheView.h $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/RechercheView.h $
// CHECKIN:		$Date: 16.01.04 13:35 $
// VERSION:		$Revision: 38 $
// LAST CHANGE:	$Modtime: 16.01.04 13:26 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#if !defined(AFX_RECHERCHEVIEW_H__517E1F02_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_RECHERCHEVIEW_H__517E1F02_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DisplayWindow.h"
#include "DibWindow.h"
#include "HtmlWindow.h"
#include "BackupDialog.h"
#include "ObjectView.h"
#include "QueryDialog.h"
#include "AlarmListWindow.h"

class CRechercheView : public CView
{
protected: // create from serialization only
	CRechercheView();
	DECLARE_DYNCREATE(CRechercheView)

// Attributes
public:
	CRechercheDoc*	GetDocument();
	BOOL			Is1x1();
	BOOL		    UseSingleWindow();
	BOOL			CanPlayRealtime();

// Operations
public:
	void			SetSmallWindowView1(CSmallWindow* pDW);
	void			DeleteSmallWindow(WORD wServerID, CSecID id); 
	void			DeleteHtmlWindow(CHtmlWindow* pHW); 
	CDisplayWindow*	GetDisplayWindow(WORD wServerID, CSecID id);
	CDisplayWindow*	GetDisplayWindow(WORD wServerID, WORD wArchiv, WORD wSequence);
	CAlarmListWindow* GetAlarmListWindow(WORD wServerID, WORD wArchiv, WORD wSequence);
	CSmallWindow*	GetActiveSmallWindow();
	BOOL			ActivateSmallWindow(CSmallWindow* pSW);
	CBackupDialog*  GetBackupDialog();
	void			OnIdle();
	void			ShowMapWindow();
	void			CloseAllWindows();
	CSmallWindows*	GetSmallWindows();

	CDisplayWindow* GetOldestDisplayWindow(CSystemTime& t);
	CDisplayWindow* GetYoungestDisplayWindow(CSystemTime& t);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRechercheView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo); 
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRechercheView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void OnBackupClosed();

protected:
	void SetXWin(int i);
	void SetWindowSize();
	void AddDisplayWindow(CServer* pServer, CIPCArchivFileRecord* pFile); 
	void AddDisplayWindow(CServer* pServer, CIPCSequenceRecord* pSequence);
	void DeleteSmallWindows(WORD wServerID); 
	void DeleteSmallWindows(WORD wServerID, WORD wArchivID); 
	void AddSmallWindow(CSmallWindow* pSW);
	void SortSmallWindows();
	void CheckSmallWindowsFullScreen();

	void OnArchivBackup(CServer* pServer);
	void ShowHideRectangle();


// Generated message map functions
protected:
	//{{AFX_MSG(CRechercheView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnView1();
	afx_msg void OnUpdateView1(CCmdUI* pCmdUI);
	afx_msg void OnView22();
	afx_msg void OnUpdateView22(CCmdUI* pCmdUI);
	afx_msg void OnView33();
	afx_msg void OnUpdateView33(CCmdUI* pCmdUI);
	afx_msg void OnViewOriginal();
	afx_msg void OnUpdateViewOriginal(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnViewToggle();
	afx_msg void OnSmallClose();
	afx_msg void OnUpdateSmallClose(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenDisk();
	afx_msg void OnSmallCloseAll();
	afx_msg void OnUpdateSmallCloseAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpenDisk(CCmdUI* pCmdUI);
	afx_msg void OnSequenceOver();
	afx_msg void OnUpdateSequenceOver(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CBackupDialog*	m_pBackupDialog;
private:
	CHtmlWindow*	m_pMapWindow;
	CSmallWindows   m_SmallWindows;
	CSmallWindow*   m_pActiveSmallWindow;

	int			m_iXWin;
	BOOL		m_b1to1;
	BOOL		m_bSequenceOver;
	CString		m_sOpenDirectory;

	friend class CSmallWindow;
	friend class CDisplayWindow;
	friend class CMainFrame;
	friend class CRechercheDoc;
};

#ifndef _DEBUG  // debug version in RechercheView.cpp
inline CRechercheDoc* CRechercheView::GetDocument()
   { return (CRechercheDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECHERCHEVIEW_H__517E1F02_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
