#if !defined(AFX_SERVICEDIALOG_H__685C0802_5696_11D1_93E4_00C095ECA33E__INCLUDED_)
#define AFX_SERVICEDIALOG_H__685C0802_5696_11D1_93E4_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ServiceDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServiceDialog dialog

class CServiceDialog : public CSkinDialog
{
// Construction
public:
	CServiceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CServiceDialog)
	enum eID{ IDD = IDD_SERVICE };
	CSkinStatic	m_txtNoSuccess;
	CSkinStatic	m_txtSuccess;
	CSkinStatic	m_ToDoTxt;
	CSkinStatic	m_DoneTxt;
	CString	m_sService;
	//}}AFX_DATA
	BOOL	m_bDone;
	BOOL	m_bShowSuccess;
	BOOL	m_bSuccess;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServiceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CServiceDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVICEDIALOG_H__685C0802_5696_11D1_93E4_00C095ECA33E__INCLUDED_)
