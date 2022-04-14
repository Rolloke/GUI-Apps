#if !defined(AFX_NONMODALDLG_H__EBAB0640_045D_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_NONMODALDLG_H__EBAB0640_045D_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NonModalDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CNonModalDlg 
#define INVALIDATE_VIEW 1

class CNonModalDlg : public CDialog
{
// Konstruktion
public:
	bool IsActive();
	CNonModalDlg(UINT nID, CWnd* pParent);   // Standardkonstruktor
	void MiniMize();
	void Restore();
//	void DDX_TextVar(CDataExchange*, int, int, double&);

// Dialogfelddaten
	//{{AFX_DATA(CNonModalDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA

   DWORD    m_dwChanges;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CNonModalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
   LRESULT OnHelp(WPARAM, LPARAM);
   bool    OnApplyNow();
   void    OnNcLButtonDblClk(UINT, CPoint);
   void    OnSysCommand(UINT, LPARAM);
	void    SetChanged(DWORD, DWORD);

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CNonModalDlg)
	virtual void    OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_NONMODALDLG_H__EBAB0640_045D_11D3_B6EC_0000B458D891__INCLUDED_
