#if !defined(AFX_DlgConnecting_H__5AFF7523_0F73_11D4_BAC8_00400531137E__INCLUDED_)
#define AFX_DlgConnecting_H__5AFF7523_0F73_11D4_BAC8_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConnecting.h : header file
//

class CServer;

/////////////////////////////////////////////////////////////////////////////
// CDlgConnecting dialog
class CDlgConnecting : public CSkinDialog
{
	// Construction
public:
	CDlgConnecting(CServer* pServer,
					  const CString& sHost,
					  DWORD dwConnectTimeout,
					  CWnd* pParent = NULL);   // standard constructor

	// Operations
public:
	void Cancel();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConnecting)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConnecting)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// Dialog Data
	//{{AFX_DATA(CDlgConnecting)
	enum  { IDD = IDD_CONNECTING };
	CSkinProgress	m_ctrlProgress;
	CString	m_sHost;
	//}}AFX_DATA
private:
	WORD		m_wServerID;
	WORD		m_wDummy;
	DWORD		m_dwConnectTimeout;
	UINT		m_uTimerID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgConnecting_H__5AFF7523_0F73_11D4_BAC8_00400531137E__INCLUDED_)
