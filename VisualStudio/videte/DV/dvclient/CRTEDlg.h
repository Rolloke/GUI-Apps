#if !defined(AFX_CRTEDLG_H__8C3C4A53_F990_11D3_8E80_004005A11E32__INCLUDED_)
#define AFX_CRTEDLG_H__8C3C4A53_F990_11D3_8E80_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CRTEDlg.h : header file
//
#include "wkclasses\WK_RuntimeErrors.h"

#define RTE_FILENAME  _T("rterrors.dat")
#define RTE_CONFIRMED _T("/RTE Confirmed/")			  

/////////////////////////////////////////////////////////////////////////////
// CRTEDlg dialog

class CRTEDlg : public CTransparentDialog
{
// Construction
public:
	virtual  ~CRTEDlg();
	CRTEDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRTEDlg)
	enum { IDD = IDD_RTE };
	CDisplay	m_ctrlTimestampDisplay;
	CDisplay	m_ctrlConfirmDisplay;
	CSkinButton	m_ctrlConfirm;
	CSkinButton	m_ctrlOK;
	CSkinButton	m_ctrlUp;
	CSkinButton	m_ctrlDown;
	CSkinButton	m_ctrlDel;
	CSkinButton	m_ctrlDelAll;
	CSkinButton	m_ctrlPrint;
	CDisplay	m_ctrlDisplay;
	CFrame		m_ctrlBorder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTEDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ShowRTErrors();

	// Generated message map functions
	//{{AFX_MSG(CRTEDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRteDel();
	afx_msg void OnRteDown();
	afx_msg void OnRteUp();
	afx_msg void OnRteConfirm();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCloseDlg();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRteDelAll();
	afx_msg void OnRtePrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSkinButtons		m_RTEButtons;
	CImageList			m_ilUp;
	CImageList			m_ilDown;
	CImageList			m_ilOK;
	CImageList			m_ilDel;
	CImageList			m_ilDelAll;
	CImageList			m_ilConfirm;
	CImageList			m_ilPrint;

	COLORREF			m_BaseColor;
	CWK_RunTimeErrors	m_RTErrors;
	CWnd*				m_pPanel;

	int					m_nStartPos;
	CString				m_sConfirm;
	HCURSOR				m_hCursor;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRTEDLG_H__8C3C4A53_F990_11D3_8E80_004005A11E32__INCLUDED_)
