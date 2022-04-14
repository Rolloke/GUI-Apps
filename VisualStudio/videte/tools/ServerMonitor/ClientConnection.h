#if !defined(AFX_CLIENTCONNECTION_H__59CEAFB0_15D1_11D4_AA00_004005A26A3B__INCLUDED_)
#define AFX_CLIENTCONNECTION_H__59CEAFB0_15D1_11D4_AA00_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientConnection.h : header file
//
#include "ServerMonitorDlg.h"

class CServerMonitorDlg;
/////////////////////////////////////////////////////////////////////////////
// CClientConnection command target

class CClientConnection : public CSocket
{
// Attributes
public:

// Operations
public:
	CClientConnection(CServerMonitorDlg* pDlg);
	virtual ~CClientConnection();

// Overrides
public:
	CString m_sReceiveData;
	CServerMonitorDlg* m_pDlg;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientConnection)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientConnection)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTCONNECTION_H__59CEAFB0_15D1_11D4_AA00_004005A26A3B__INCLUDED_)
