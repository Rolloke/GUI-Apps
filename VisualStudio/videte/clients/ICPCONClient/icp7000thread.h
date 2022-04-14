#if !defined(AFX_ICP7000THREAD_H__89841E94_73A7_46CA_9E97_5AE3E6556AA8__INCLUDED_)
#define AFX_ICP7000THREAD_H__89841E94_73A7_46CA_9E97_5AE3E6556AA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ICP7000Thread.h : header file
//

#define WM_CHECK_SKINBUTTON (WM_USER + 1)


/////////////////////////////////////////////////////////////////////////////
// CICPI7000Module
#define ARRAY_SIZE 10
#ifdef _DEBUG

class CThreadDebug
{
public:
	CThreadDebug(LPCTSTR pszMsg)
	{
		TRACE("Begin(%s), ThreadID = %d\n", pszMsg, GetCurrentThreadId());
		m_pszMsg = pszMsg;
	}
	~CThreadDebug()
	{
		TRACE("End(%s), ThreadID = %d\n", m_pszMsg, GetCurrentThreadId());
	}
private:
	LPCTSTR m_pszMsg;
};

#endif

class CICPI7000Module
{
public:
	CICPI7000Module();
	~CICPI7000Module();
	int  AddRef() {return (++m_nReference);};
	int  Release();
	void SetComPort(WORD wPort)      { m_w7000[0]   = wPort; };
	void SetModuleAddress(WORD wAdr) { m_w7000[1]   = wAdr;  };
	bool SetModuleID(WORD wID)			{ m_w7000[2]   = wID;   };
	bool SetModuleID(const char*pszModuleID);
	void SetChecksum(WORD wCk)       { m_w7000[3]   = wCk;   };
	void SetTimeOut(WORD wTO)        { m_w7000[4]   = wTO;   };
	void SetBaudrate(DWORD dwBaud)   { m_dwBaudrate = dwBaud;}
	bool SetConfig(char*);
	void SetDObit(int, BOOL);
	void SetDOlevel(int, BOOL);
	void SetPollDO(bool );
	void SetDOstateChanged(bool );
	void SetDIbit(int, BOOL);
	void SetPollDI(bool);

	char  GetComPort()               { return (char)m_w7000[0];};
	WORD  GetModuleAddress()         { return m_w7000[1];  };
	WORD  GetModuleID()              { return m_w7000[2];  };
	WORD  GetChecksum()              { return m_w7000[3];  };
	WORD  GetTimeOut()               { return m_w7000[4];  };
	DWORD GetBaudrate()					{ return m_dwBaudrate;}
	bool  GetDObit(int);
	int   GetDOoffset()					{ return m_wDOoffset; };
	int	GetOutputs();
	bool  GetOutput(int);
	bool  HasOutputs()					{ return (m_wDOmask!=0) ? true : false;};
	bool  DoPollDO();
	bool  IsDOstateChanged();
	bool  GetDIbit(int);
	int   GetDIoffset()					{ return m_wDIoffset; };
	int	GetInputs();
	bool  GetInput(int);
	bool  HasInputs()						{ return (m_wDImask!=0) ? true : false;};
	bool  DoPollDI();
	bool  GetDOlevel(int);

	CString &GetItemID()             { return m_sItemID;  };
	CString &GetDIOmode()            { return m_sDIOmode;  };

	bool OpenPort();
	bool IsOpened();
	void ClosePort();
	WORD GetFailSeconds()            { return m_wFailSeconds; };

	WORD DigitalIn();
	WORD DigitalOut();
	WORD DigitalOutReadBack();

	WORD  m_wDIstate;
	WORD  m_wDOstate;
	UINT  m_nStartID;
private:
	CString m_sDIOmode;
	CString m_sItemID;
	WORD  m_w7000[ARRAY_SIZE];
	WORD  m_wDOoffset;
	WORD  m_wDIoffset;
	WORD  m_wDOmask;
	WORD  m_wDImask;
	WORD  m_wDOlevel;
	DWORD m_dwBaudrate;
	WORD  m_wFlags;
	WORD  m_wFailSeconds;
	int   m_nReference;
};
/////////////////////////////////////////////////////////////////////////////
// CICPI7000Thread thread
class CSwitchPanel;


class CICPI7000Thread : public CWinThread
{
friend class CSwitchPanel;

	DECLARE_DYNCREATE(CICPI7000Thread)
public:
	CICPI7000Thread();
	CICPI7000Thread(int, void*, CSwitchPanel*, bool);
	virtual ~CICPI7000Thread();

// Attributes
public:

// Operations
public:
	static void CreatePerformanceMonitor();
	static void DeletePerformanceMonitor();
	void StopThread();
	int  GetPort()           { return m_nPort;   };

	void SetPollState(BOOL);
	BOOL IncrementPollState();
	BOOL IsPolling();
	bool DoPoll()            { return m_bDoPoll;};

	void SetFailCount(int nCount)   { m_nFailcount = nCount; };
	WORD GetFailCount()             { return m_nFailcount;   };
	bool HasFailed();

	void AddModule(void*);
	bool ContainsModule(void*);

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
	LRESULT OnCheckSkinButton(UINT, CSkinButton*);
	LRESULT OnCommand(WORD wID, WORD wNotify, HWND);
	LRESULT OnTimer(UINT, UINT);

	CSwitchPanel *m_pSwitchPanel;
	int      m_nPort;
	CPtrList m_Modules;
	BOOL     m_bPolling;
	bool     m_bDoPoll;
	bool     m_bMultipleAccess;
	int      m_nFailcount;

	static CWK_PerfMon *gm_pPerformanceMonitor;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICP7000THREAD_H__89841E94_73A7_46CA_9E97_5AE3E6556AA8__INCLUDED_)
