#if !defined(AFX_ABOUTAC_COM_H__F7940622_CA98_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ABOUTAC_COM_H__F7940622_CA98_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutAC_Com.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAboutAC_Com 

class CAboutAC_Com : public CDialog
{
// Konstruktion
public:
	HWND m_hHoverCtrl;
	CAboutAC_Com(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CAboutAC_Com)
	enum { IDD = IDD_ABOUTBOX };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CAboutAC_Com)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CAboutAC_Com)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   void    ClearHoverWnd();
   LRESULT OnMouseLeave(WPARAM, LPARAM);

   static BOOL CALLBACK EnumChildWndFunc(HWND, LPARAM);
   static LRESULT CALLBACK SubClassProc(HWND,UINT, WPARAM, LPARAM);

   HFONT m_hFont;
   BYTE  m_bTrackMouseEvent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_ABOUTAC_COM_H__F7940622_CA98_11D4_B6EC_0000B458D891__INCLUDED_
