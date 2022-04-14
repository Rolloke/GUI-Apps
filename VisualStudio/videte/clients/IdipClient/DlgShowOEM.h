// DlgShowOEM.h: interface for the CDlgShowOEM class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ViewDlg.h"
#include "afxwin.h"

// CDlgShowOEM dialog

class CDlgShowOEM : public CDlgInView
{
	DECLARE_DYNAMIC(CDlgShowOEM)

public:
	CDlgShowOEM(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgShowOEM();

// Dialog Data
	enum  { IDD = IDD_OEM_DLG };
protected:
	int GetLogoRect(CRect*prc);

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgShowOEM)
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL StretchElements();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CDlgShowOEM)
	afx_msg void OnBnClickedCloseDlgBar();
	afx_msg LRESULT OnUserMsg(WPARAM, LPARAM);
	afx_msg void OnBnClickedAppAbout();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CSkinButton m_btnClose;
	CImageList  m_Logo;	
	CSkinStatic m_txtDate;
	CSkinStatic m_txtTime;
	CFont		m_Font1;
	CFont		m_Font2;
	int			m_nImage;
};
