#if !defined(AFX_SOCKETCONNECTION_H__0372CAF8_1C21_11D4_AA0D_004005A26A3B__INCLUDED_)
#define AFX_SOCKETCONNECTION_H__0372CAF8_1C21_11D4_AA0D_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SocketConnection.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CSocketConnection thread

class CSocketConnection : public CWinThread
{
	DECLARE_DYNCREATE(CSocketConnection)
protected:
	CSocketConnection();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketConnection)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSocketConnection();

	// Generated message map functions
	//{{AFX_MSG(CSocketConnection)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_hSocketHandle;
	CConnection m_sSocket;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKETCONNECTION_H__0372CAF8_1C21_11D4_AA0D_004005A26A3B__INCLUDED_)
