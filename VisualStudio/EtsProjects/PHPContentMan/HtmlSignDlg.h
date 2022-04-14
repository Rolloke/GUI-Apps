#if !defined(AFX_HTMLSIGNDLG_H__8ACE9D00_F2A0_458F_9DB4_03BA036561EB__INCLUDED_)
#define AFX_HTMLSIGNDLG_H__8ACE9D00_F2A0_458F_9DB4_03BA036561EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HtmlSignDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CHtmlSignDlg 

class CHtmlSignDlg : public CDialog
{
// Konstruktion
public:
	CHtmlSignDlg(CWnd* pParent = NULL);   // Standardkonstruktor

	virtual ~CHtmlSignDlg();   // StandardDestruktor
// Dialogfelddaten
	//{{AFX_DATA(CHtmlSignDlg)
	enum { IDD = IDD_HTML_SIGN_DLG };
	CComboBox	m_cCommandList;
	//}}AFX_DATA

   bool m_bCloseFrame;
   bool m_bTopMost;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CHtmlSignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HWND m_hWndPasteTo;
	HICON m_hIcon;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CHtmlSignDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBtnCopyHtmlCode();
  	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeHtmlPasteCommand();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	//}}AFX_MSG
   afx_msg LRESULT OnAppMessage1(WPARAM, LPARAM);
   afx_msg LRESULT OnAppMessage2(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
private:
	int m_nOldHeight;

   CFont    m_System;
	CFont    m_Symbol;
   int      m_nCommandID;

   static BOOL    CALLBACK DeleteChildren(HWND, LPARAM);
   static LRESULT CALLBACK BtnWndProc(HWND, UINT, WPARAM, LPARAM);

   static HWND gm_hwndMouseLeave;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_HTMLSIGNDLG_H__8ACE9D00_F2A0_458F_9DB4_03BA036561EB__INCLUDED_
