#if !defined(AFX_CONNECTINGDIALOG_H__A09BCE94_0B8A_11D4_BAC7_00400531137E__INCLUDED_)
#define AFX_CONNECTINGDIALOG_H__A09BCE94_0B8A_11D4_BAC7_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectingDialog.h : header file
//

class CServer;

/////////////////////////////////////////////////////////////////////////////
// CConnectingDialog dialog
class CConnectingDialog : public CWK_Dialog
{
// Construction
public:
	CConnectingDialog(CServer* pServer,
					  const CString& sHost,
					  DWORD dwConnectTimeout,
					  CWnd* pParent = NULL);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectingDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectingDialog)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// Dialog Data
	//{{AFX_DATA(CConnectingDialog)
	enum { IDD = IDD_CONNECTING };
	CProgressCtrl	m_ctrlProgress;
	CString	m_sHost;
	//}}AFX_DATA
private:
	CServer*	m_pServer;
	DWORD		m_dwConnectTimeout;
	UINT		m_uTimerID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTINGDIALOG_H__A09BCE94_0B8A_11D4_BAC7_00400531137E__INCLUDED_)
