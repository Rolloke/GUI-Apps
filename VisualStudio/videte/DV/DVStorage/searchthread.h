// SearchThread.h: interface for the CSearchThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SearchTHREAD_H__FF9C5E63_7D66_11D2_B4FA_004005A19028__INCLUDED_)
#define AFX_SearchTHREAD_H__FF9C5E63_7D66_11D2_B4FA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Search.h"

class CSearchThread : public CWK_Thread  
{
public:
	CSearchThread(CSearch* pSearch);
	virtual ~CSearchThread();

	// attributes
public:
	inline CSearch* GetSearch() const;
	inline BOOL    IsCancelled() const;

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
			void Cancel();

	// implementation
protected:
	BOOL Search(CCollection* pCollection);
	BOOL Search(CTape* pTape);
	BOOL WaitForResponses();

private:
	CSearch* m_pSearch;
	BOOL	m_bCancelled;
};
///////////////////////////////////////////////////
inline CSearch* CSearchThread::GetSearch() const
{
	return m_pSearch;
}
///////////////////////////////////////////////////
inline BOOL CSearchThread::IsCancelled() const
{
	return m_bCancelled;
}
///////////////////////////////////////////////////
typedef CSearchThread* CSearchThreadPtr;
WK_PTR_ARRAY_CS(CSearchThreadArray,CSearchThreadPtr,CSearchThreads);
///////////////////////////////////////////////////

#endif // !defined(AFX_SearchTHREAD_H__FF9C5E63_7D66_11D2_B4FA_004005A19028__INCLUDED_)
