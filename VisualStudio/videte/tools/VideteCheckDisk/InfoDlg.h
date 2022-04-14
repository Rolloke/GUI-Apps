#pragma once
#include "afxwin.h"


// CInfoDlg dialog

class CInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInfoDlg)

public:
	CInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoDlg();
	void Create(CWnd* pParent = NULL);
	void SetInfoText(LPCTSTR sInfo, int nTimeout);
	BOOL PendingMessages();

// Dialog Data
protected:
	enum { IDD = IDD_INFO };
	CStatic m_txtInfo;
	COLORREF m_colBkGnd;
	COLORREF m_colText;
	COLORREF m_colLight;
	COLORREF m_colShaddow;
	CBrush	 m_BkGndBrush;
	CFont	 m_FixedFont;
	UINT	 m_nTimer;
	CStringArray m_saMessages;
	CDWordArray	 m_dwaTimeouts;

protected:
	//{{AFX_VIRTUAL(CInfoDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

protected:
	// Generierte Funktionen für die Meldungstabellen
	//{{AFX_MSG(CInfoDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
