// Search.h: interface for the CSearch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCH_H)
#define AFX_SEARCH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StorageId.h"

class CIPCDatabaseStorage;
class CClient;

class CSearch  
{
	// construction/destruction
public:
	CSearch(DWORD dwUserID, 
		   DWORD dwClient, 
		   int iNumCollections,
		   const WORD Collections[],
		   int iNumFields,
		   const CIPCField fields[],
		   DWORD dwMaxQueryResults);
	virtual ~CSearch();

	// attributes
public:
	inline DWORD	GetClientID() const;
	inline DWORD	GetUserID() const;
	inline DWORD	GetMaxQueryResults() const;
	inline CString  GetExpression() const;
	inline BOOL		IsForward() const;
		   CCollection*	GetAndRemoveCollection();
		   BOOL		HasEnoughResponses();

	inline const CSystemTime& GetStartTime() const;
	inline const CSystemTime& GetEndTime() const;

	// operations
public:
	BOOL AddSearchResult(CTape* pTape, const CStorageId& id);
	void SendSearchResults(CClient* pClient);
	void IncreaseResponses();
	void Cancel();

	// implementation
protected:
	void SendSearchResult(CIPCDatabaseStorage* pCIPC, 
						  CTape* pTape, 
						  CStorageId* pDbsId);
	void DoConfirmQuery();
	int  GetIndicationDifferenz();

	// data member
private:
	CCollections m_Collections;
	CSystemTime  m_stStart;
	CSystemTime  m_stEnd;

	CString  m_sExpression;
	CString	 m_sSearch;
	DWORD	 m_dwClient;
	DWORD	 m_dwUserID;
	DWORD	 m_dwMaxQueryResults;
	DWORD	 m_dwSentQueryResults;

	int		 m_iResponses;
	int		 m_iLastIndicated;
	CStorageIds	 m_Results;

	BOOL	 m_bCancelled;
	BOOL	 m_bForward;
	BOOL	 m_bConfirmed;

	friend class CSearchThread;
};
/////////////////////////////////////////////////////////////////////////////
inline CString CSearch::GetExpression() const
{
	return m_sExpression;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CSearch::GetClientID() const
{
	return m_dwClient;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CSearch::GetUserID() const
{
	return m_dwUserID;
}
/////////////////////////////////////////////////////////////////////////////
inline const CSystemTime& CSearch::GetStartTime() const
{
	return m_stStart;
}
/////////////////////////////////////////////////////////////////////////////
inline const CSystemTime& CSearch::GetEndTime() const
{
	return m_stEnd;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSearch::IsForward() const
{
	return m_bForward;
}
////////////////////////////////////////////////////////////////////////////
inline DWORD CSearch::GetMaxQueryResults() const
{
	//m_dwMaxQueryResults übermittelt 2 Flgas
	//LO=1: MaxQueryResults, HI=1: SpecialQueryCancel
	WORD wMaxQueryResults = HIWORD(m_dwMaxQueryResults);
	return wMaxQueryResults;
	
	//return m_dwMaxQueryResults;
}
////////////////////////////////////////////////////////////////////////////
typedef CSearch* CSearchPtr;
WK_PTR_LIST_CS(CSearchList,CSearchPtr,CSearchs);
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_QUERY_H__FF9C5E64_7D66_11D2_B4FA_004005A19028__INCLUDED_)
