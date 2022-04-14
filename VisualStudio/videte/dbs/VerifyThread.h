// VerifyThread.h: interface for the CVerifyThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERIFYTHREAD_H__C2958A13_3876_4E0B_AD3A_23AF43C86476__INCLUDED_)
#define AFX_VERIFYTHREAD_H__C2958A13_3876_4E0B_AD3A_23AF43C86476__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVerifyThread : public CWK_Thread  
{
public:
	CVerifyThread();
	virtual ~CVerifyThread();

	// attributes
public:

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
			void Cancel();
			void Verify();

private:
	BOOL m_bCancel;
	BOOL  m_bVerify;
	WORD  m_wArchive;
	WORD  m_wSequence;
};

#endif // !defined(AFX_VERIFYTHREAD_H__C2958A13_3876_4E0B_AD3A_23AF43C86476__INCLUDED_)
