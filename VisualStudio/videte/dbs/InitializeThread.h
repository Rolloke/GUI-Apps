// InitializeThread.h: interface for the CInitializeThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITIALIZETHREAD_H__73D434F3_1924_11D5_99C1_004005A19028__INCLUDED_)
#define AFX_INITIALIZETHREAD_H__73D434F3_1924_11D5_99C1_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////////
// class CInitializeThread
class CInitializeThread : public CWK_Thread  
{
	// construction/destruction
public:
	CInitializeThread(BOOL bInit);
	virtual ~CInitializeThread();

public:
	inline BOOL GetInit() const;

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);


private:
	BOOL	m_bInit;
};
/////////////////////////////////////////////////////////////////////////////////
inline BOOL CInitializeThread::GetInit() const
{
	return m_bInit;
}

/////////////////////////////////////////////////////////////////////////////////
// class CCheckDriveThread
class CCheckDriveThread : public CWK_Thread  
{
	// construction/destruction
public:
	CCheckDriveThread();
	virtual ~CCheckDriveThread();

public:
	void DoCheck();

	// operations
protected:
	virtual	BOOL Run(LPVOID lpContext);
	virtual void AutoDelete();

private:
	BOOL m_bCheck;
};

#endif // !defined(AFX_INITIALIZETHREAD_H__73D434F3_1924_11D5_99C1_004005A19028__INCLUDED_)
