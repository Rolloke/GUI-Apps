/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ConnectionDlg.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/ConnectionDlg.h $
// CHECKIN:		$Date: 16.01.98 9:57 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 5.12.97 13:21 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_CONNECTIONDLG_H__08F7FC62_656F_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_CONNECTIONDLG_H__08F7FC62_656F_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CHostArray;

/////////////////////////////////////////////////////////////////////////////
// CConnectionDlg dialog
class CConnectionDlg : public CDialog
{
// Construction
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CConnectionDlg(CWnd* pParent = NULL);   // standard constructor
	~CConnectionDlg();

// Dialog Data
	//{{AFX_DATA(CConnectionDlg)
	enum { IDD = IDD_CONNECTION };
	CEdit	m_editPassword;
	CEdit	m_editName;
	CListBox	m_lbHosts;
	CString	m_sName;
	CString	m_sPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CString	m_sTelefon;
	CString m_sHost;
protected:
	CToolTipCtrl	m_ToolTip;
	CStringArray	m_sTTArray;
private:
	CHostArray* m_pHostArray;
	CString		m_sTTEditName;

	// Generated message map functions
	//{{AFX_MSG(CConnectionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListboxHost();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIONDLG_H__08F7FC62_656F_11D1_9F29_0000C036AC0D__INCLUDED_)
