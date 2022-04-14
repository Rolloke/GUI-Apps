#if !defined(AFX_TESTTHREAD_H__0D1F8B6F_217F_11D2_9FB5_00C095ECA33E__INCLUDED_)
#define AFX_TESTTHREAD_H__0D1F8B6F_217F_11D2_9FB5_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TestThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CTestThread thread

class CTestThread : public CWinThread
{
public:
	CTestThread();           // protected constructor used by dynamic creation
	virtual ~CTestThread();

	virtual int Run();

	BOOL m_bThreadRun;

	DWORD m_dwPauseTime;
	BOOL m_bDoTest;

	DECLARE_DYNCREATE(CTestThread)
protected:
// Attributes
public:

// Operations
public:
	void StartTest(DWORD dwTime);
	void StopTest();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTestThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTTHREAD_H__0D1F8B6F_217F_11D2_9FB5_00C095ECA33E__INCLUDED_)
