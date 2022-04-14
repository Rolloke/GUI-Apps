#if !defined(AFX_REMOTEDIALOG_H__F2C3DF06_B43C_11D2_B565_004005A19028__INCLUDED_)
#define AFX_REMOTEDIALOG_H__F2C3DF06_B43C_11D2_B565_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RemoteDialog.h : header file
//
#include "RemoteThread.h"
/////////////////////////////////////////////////////////////////////////////
// CRemoteDialog dialog

class CRemoteDialog : public CWK_Dialog
{
// Construction
public:
	CRemoteDialog(CSecID hostID,
				  BOOL bReceiving,
				  CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRemoteDialog)
	enum { IDD = IDD_REMOTE };
	CButton	m_btnCancel;
	CButton	m_btnOK;
	CProgressCtrl	m_ctrlProgress;
	CString	m_sState;
	CString	m_sHost;
	//}}AFX_DATA

public:
	CIPCDrives& GetDrives();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRemoteDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CSecID		   m_HostID;
	BOOL		   m_bReceiving;
	CString		   m_sRegistryFile;
	CString		   m_sCoCoISAini;
	CString		   m_sCommand;
	UINT		   m_nRemoteCodePage;

private:
	CRemoteThread	m_RemoteThread;
	DWORD			m_dwCounter;
	DWORD			m_dwTimeout;	// in 1/10 Sekunden, wegen 100 ms - Timer
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTEDIALOG_H__F2C3DF06_B43C_11D2_B565_004005A19028__INCLUDED_)
