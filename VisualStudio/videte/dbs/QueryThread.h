// QueryThread.h: interface for the CQueryThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYTHREAD_H__FF9C5E63_7D66_11D2_B4FA_004005A19028__INCLUDED_)
#define AFX_QUERYTHREAD_H__FF9C5E63_7D66_11D2_B4FA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Query.h"

class CQueryThread : public CWK_Thread  
{
public:
	CQueryThread(CQuery* pQuery);
	virtual ~CQueryThread();

	// attributes
public:
	inline CQuery* GetQuery() const;
	inline BOOL    IsCancelled() const;

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
			void Cancel();

	// implementation
protected:
	BOOL Query(CArchiv* pArchiv);
	BOOL WaitForResponses();

private:
	CQuery* m_pQuery;
	BOOL	m_bCancelled;
};
///////////////////////////////////////////////////
inline CQuery* CQueryThread::GetQuery() const
{
	return m_pQuery;
}
///////////////////////////////////////////////////
inline BOOL CQueryThread::IsCancelled() const
{
	return m_bCancelled;
}
///////////////////////////////////////////////////
typedef CQueryThread* CQueryThreadPtr;
WK_PTR_ARRAY_CS(CQueryThreadArray,CQueryThreadPtr,CQueryThreads)
///////////////////////////////////////////////////

#endif // !defined(AFX_QUERYTHREAD_H__FF9C5E63_7D66_11D2_B4FA_004005A19028__INCLUDED_)
