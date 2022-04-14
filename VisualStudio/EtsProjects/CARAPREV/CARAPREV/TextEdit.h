#if !defined(AFX_TEXTEDIT_H__89A592E0_D896_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_TEXTEDIT_H__89A592E0_D896_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextEdit.h : Header-Datei
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTextEdit 
class CTextLabel;

class AFX_EXT_CLASS CTextEdit : public CDialog
{
// Konstruktion
public:
	CTextEdit(CWnd* pParent = NULL);   // Standardkonstruktor

	void SetTextLabel(CTextLabel *);
// Dialogfelddaten
	//{{AFX_DATA(CTextEdit)
	enum { IDD = IDD_TEXTDLG };
	CEdit	m_cTextEdit;
	//}}AFX_DATA
   float m_fRatio;
	BOOL	m_bCalcRect;
   bool  m_bAutoDelete;
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTextEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	CTextLabel *m_pTextLabel;
   CFont       m_Font;
   POINT       m_ptOk;
   SIZE        m_szButton;
   SIZE        m_szMin;
   POINT       m_ptCalcRect;
   SIZE        m_szCalcRect;
   int         m_nTextY;
   LRESULT OnGetminMaxInfo(WPARAM, LPARAM);
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTextEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTextCalcrect();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TEXTEDIT_H__89A592E0_D896_11D3_B6EC_0000B458D891__INCLUDED_
