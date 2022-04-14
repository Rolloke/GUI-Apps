// ServerMonitorDlg.h : header file
//
#include "NetSpy.h"
#if !defined(AFX_SERVERMONITORDLG_H__59CEAFA7_15D1_11D4_AA00_004005A26A3B__INCLUDED_)
#define AFX_SERVERMONITORDLG_H__59CEAFA7_15D1_11D4_AA00_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "NetSpy.h"
#include "ClientConnection.h"
/////////////////////////////////////////////////////////////////////////////
// CServerMonitorDlg dialog
class CNetSpy;
class CClientConnection;
class CServerMonitorDlg : public CDialog
{
// Construction
public:
	CClientConnection* m_pClientConnection;
	CNetSpy* m_pNetSpy;
	CServerMonitorDlg(CWnd* pParent = NULL);	// standard constructor
	~CServerMonitorDlg();

// Dialog Data
	//{{AFX_DATA(CServerMonitorDlg)
	enum { IDD = IDD_SERVERMONITOR_DIALOG };
	CIPAddressCtrl	m_ctrlIP;
	CButton	m_SendButton;
	CEdit	m_cCommand;
	CEdit	m_cMessage;
	CEdit	m_cReceiveData;
	CButton	m_StopListenButton;
	CButton	m_Listening_Button;
	CEdit	m_SpyPort;
	int		m_iSpyPort;
	CString	m_eReceiveData;
	CString	m_sMessage;
	CString	m_sCommand;
	CString	m_sAddress;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerMonitorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CServerMonitorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangePort();
	afx_msg void OnListeningButton();
	afx_msg void OnStopListenButton();
	afx_msg void OnKillfocusData();
	afx_msg void OnUpdatePort();
	afx_msg void OnSendButton();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonDisconnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERMONITORDLG_H__59CEAFA7_15D1_11D4_AA00_004005A26A3B__INCLUDED_)
