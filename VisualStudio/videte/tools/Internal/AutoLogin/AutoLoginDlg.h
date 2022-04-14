/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AutoLogin
// FILE:		$Workfile: AutoLoginDlg.h $
// ARCHIVE:		$Archive: /Project/Tools/Internal/AutoLogin/AutoLoginDlg.h $
// CHECKIN:		$Date: 10.08.98 15:03 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 10.08.98 14:47 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOLOGINDLG_H__9B505288_3038_11D2_BC01_00A024D29804__INCLUDED_)
#define AFX_AUTOLOGINDLG_H__9B505288_3038_11D2_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CAutoLoginDlg dialog

class CAutoLoginDlg : public CDialog
{
// Construction
public:
	 ~CAutoLoginDlg();
	CAutoLoginDlg(CWnd* pParent = NULL);	// standard constructor
	void ClimbWindow(CWnd* pWnd);
// Dialog Data
	//{{AFX_DATA(CAutoLoginDlg)
	enum { IDD = IDD_AUTOLOGIN_DIALOG };
	CString	m_sName;
	CString	m_sPassword;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAutoLoginDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOLOGINDLG_H__9B505288_3038_11D2_BC01_00A024D29804__INCLUDED_)
