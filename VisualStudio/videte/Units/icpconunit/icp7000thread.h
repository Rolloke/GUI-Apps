#if !defined(AFX_ICP7000THREAD_H__89841E94_73A7_46CA_9E97_5AE3E6556AA8__INCLUDED_)
#define AFX_ICP7000THREAD_H__89841E94_73A7_46CA_9E97_5AE3E6556AA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ICP7000Thread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CICPI7000Module
#define ARRAY_SIZE 10
#ifdef _DEBUG

class CThreadDebug
{
public:
	CThreadDebug(const char*pszMsg)
	{
		TRACE("Begin(%s), ThreadID = %d\n", pszMsg, GetCurrentThreadId());
		m_pszMsg = pszMsg;
	}
	~CThreadDebug()
	{
		TRACE("End(%s), ThreadID = %d\n", m_pszMsg, GetCurrentThreadId());
	}
private:
	const char *m_pszMsg;
};

#endif

/////////////////////////////////////////////////////////////////////////////
// CICPI7000Thread thread
class CICPCONUnitDlg;

class CICPI7000Thread : public CWinThread
{
	friend class CICPCONUnitDlg;

	DECLARE_DYNCREATE(CICPI7000Thread)
public:
	CICPI7000Thread();
	CICPI7000Thread(int, void*, CICPCONUnitDlg*, bool);
	virtual ~CICPI7000Thread();

// Attributes
public:

// Operations
public:
	int  GetPort()           { return m_nPort;   };

	void SetPollState(BOOL);
	BOOL IncrementPollState();
	BOOL IsPolling();
	bool DoPoll()            { return m_bDoPoll;};

	void SetFailCount(int nCount)   { m_nFailcount = nCount; };
	WORD GetFailCount()             { return (WORD)m_nFailcount;   };
	bool HasFailed();

	void AddModule(void*);
	bool ContainsModule(void*);

	void StopThread(bool);
	void SuspendThread();
	void ResumeThread();
	static void CreatePerformanceMonitor();
	static void DeletePerformanceMonitor();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CICPI7000Thread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CICPI7000Thread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	LRESULT OnCommand(WORD wID, WORD wNotify, HWND);
	LRESULT OnTimer(UINT, UINT);

	CICPCONUnitDlg *m_pDialog;
	int      m_nPort;
	CPtrList m_Modules;
	BOOL     m_bPolling;
	bool     m_bDoPoll;
	bool     m_bMultipleAccess;
	int      m_nFailcount;
	DWORD    m_dwSuspendCount;
	static CWK_PerfMon *gm_pPerformanceMonitor;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICP7000THREAD_H__89841E94_73A7_46CA_9E97_5AE3E6556AA8__INCLUDED_)
