#if !defined(AFX_NETSPY_H__59CEAFAF_15D1_11D4_AA00_004005A26A3B__INCLUDED_)
#define AFX_NETSPY_H__59CEAFAF_15D1_11D4_AA00_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetSpy.h : header file
//

#include"ServerMonitorDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNetSpy command target

class CNetSpy : public CSocket
{
// Attributes
public:

// Operations
public:
	CNetSpy(CServerMonitorDlg* pDlg);
	virtual ~CNetSpy();

// Overrides
public:
	CServerMonitorDlg* m_pDlg;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetSpy)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CNetSpy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETSPY_H__59CEAFAF_15D1_11D4_AA00_004005A26A3B__INCLUDED_)
