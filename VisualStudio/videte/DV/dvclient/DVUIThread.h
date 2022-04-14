#if !defined(AFX_DVUITHREAD_H__C82F81DB_55C0_4AA3_8577_CD45C48FD1CD__INCLUDED_)
#define AFX_DVUITHREAD_H__C82F81DB_55C0_4AA3_8577_CD45C48FD1CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DVUIThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CDVUIThread thread

class CDVUIThread : public CWinThread
{
	DECLARE_DYNAMIC(CDVUIThread)
public:
	CDVUIThread();           // protected constructor used by dynamic creation
	virtual ~CDVUIThread();

// Attributes
public:
	CWnd *GetPanel() { return m_pMainWnd; };
// Operations
public:
	static UINT WinExecW(LPCWSTR lpCmdLine, UINT uCmdShow);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVUIThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CDVUIThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	void HandleKeyStroke(char vKey);

private:
	BOOL IsTempPassword(const CString& sPassword);
		
	CString	m_sInput;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVUITHREAD_H__C82F81DB_55C0_4AA3_8577_CD45C48FD1CD__INCLUDED_)
