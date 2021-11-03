// MainFrm.h : Schnittstelle der Klasse CMainFrame
//
/////////////////////////////////////////////////////////////////////////////
#include "..\Label.h"
#include "..\CaraFrameWnd.h"
#include "..\CaraToolBar.h"

#if !defined(AFX_MAINFRM_H__30FA9EA9_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_MAINFRM_H__30FA9EA9_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define NO_OF_MAIN_INDICATORS 4

class CMainFrame : public CCaraFrameWnd
{
protected: // Nur aus Serialisierung erzeugen
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
// Attribute
public:
   bool SetMainPaneInfo();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CCaraToolbar m_wndToolBar;
	CCaraToolbar m_wndPlotToolBar;
protected:  // Eingebundene Elemente der Steuerleiste

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewPlottoolbar();
	afx_msg void OnUpdateViewPlottoolbar(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnSysColorChange();
	afx_msg LRESULT OnNewCurve(WPARAM, LPARAM);
	afx_msg LRESULT OnEndThread(WPARAM, LPARAM);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClose(WPARAM, LPARAM);
	afx_msg LRESULT OnUserOptUpdate(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public :
       CWinThread     *m_pThread;
       CLabel         *m_pLabel;
       int             m_nPrecision;
       bool            m_bSigned;
       static UINT gm_nMainIndicators[NO_OF_MAIN_INDICATORS];
       afx_msg void OnHelpShortcuts();
       afx_msg void OnUpdateHelpShortcuts(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_MAINFRM_H__30FA9EA9_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
