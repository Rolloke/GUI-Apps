// MainFrm.h : Schnittstelle der Klasse CMainFrame
//
/////////////////////////////////////////////////////////////////////////////
#include "CaraFrameWnd.h"
#include "CaraToolBar.h"

#if !defined(AFX_MAINFRM_H__FA828EE8_A94A_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_MAINFRM_H__FA828EE8_A94A_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#define NO_OF_MAIN_INDICATORS 3

class CTreeDlg;

class CMainFrame : public CCaraFrameWnd
{
protected: // Nur aus Serialisierung erzeugen
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attribute
public:

// Operationen
public:
   bool SetMainPaneInfo();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CMainFrame();
	virtual CLabel* PasteSpecial(CPtrList*);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
	CCaraToolbar m_wndToolBar;
//   CDialogBar   m_wndMenuBar;
// Generierte Message-Map-Funktionen
protected:
	static int DataTree(CTreeDlg *, int);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   static UINT gm_nMainIndicators[NO_OF_MAIN_INDICATORS];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_MAINFRM_H__FA828EE8_A94A_11D2_9DB9_0000B458D891__INCLUDED_)
