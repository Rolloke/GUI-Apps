#if !defined(AFX_MAINFRAME_H__460D7BE7_D2B5_4B84_B4D9_992F3EF1B7D9__INCLUDED_)
#define AFX_MAINFRAME_H__460D7BE7_D2B5_4B84_B4D9_992F3EF1B7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainFrame.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Rahmen CMainFrame 

class CMainFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CMainFrame)
protected:

// Attribute
public:
	CMainFrame();           // Dynamische Erstellung verwendet geschützten Konstruktor

// Operationen
public:
	int  m_nSpecial;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CMainFrame)
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CMainFrame();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnQueryEndSession();
	//}}AFX_MSG
   afx_msg LRESULT OnAppMessage1(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_MAINFRAME_H__460D7BE7_D2B5_4B84_B4D9_992F3EF1B7D9__INCLUDED_
