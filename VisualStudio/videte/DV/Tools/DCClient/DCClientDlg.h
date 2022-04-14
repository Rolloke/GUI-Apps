// DCClientDlg.h : header file
//

#if !defined(AFX_DCCLIENTDLG_H__B849B77E_9EFB_4A31_9719_7A499A1047C9__INCLUDED_)
#define AFX_DCCLIENTDLG_H__B849B77E_9EFB_4A31_9719_7A499A1047C9__INCLUDED_

#include "IPCDataCarrierClient.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDCClientDlg dialog

class CDCClientDlg : public CDialog
{
// Construction
public:
	CDCClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDCClientDlg)
	enum { IDD = IDD_DCCLIENT_DIALOG };
	CEdit	m_EditBrowse;
	CEdit	m_EdtMessages;
	CProgressCtrl	m_prgCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void OnIdle();
	
	//set the progress bar in programs window (gui)
	void SetProgress(DWORD dwPercent);
	void AppendMessage(CString sMessage);
	void DoDisconnect();	//delete m_pClient
	void StartBurning();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDCClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonDisconnect();
	afx_msg void OnButtonCdinfo();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonBurn();
	afx_msg void OnAbort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()




private:

	CIPCDataCarrierClient*	m_pClient;
	CString					m_sDirToBurn;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCCLIENTDLG_H__B849B77E_9EFB_4A31_9719_7A499A1047C9__INCLUDED_)
