// InitializeThread.h: interface for the CInitializeThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITIALIZETHREAD_H__2B0560F3_1862_11D5_99BD_004005A19028__INCLUDED_)
#define AFX_INITIALIZETHREAD_H__2B0560F3_1862_11D5_99BD_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInitializeThread : public CWK_Thread  
{
public:
	CInitializeThread();
	virtual ~CInitializeThread();

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);

};

#endif // !defined(AFX_INITIALIZETHREAD_H__2B0560F3_1862_11D5_99BD_004005A19028__INCLUDED_)
