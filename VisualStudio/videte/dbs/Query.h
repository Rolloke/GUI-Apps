// Query.h: interface for the CQuery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERY_H__FF9C5E64_7D66_11D2_B4FA_004005A19028__INCLUDED_)
#define AFX_QUERY_H__FF9C5E64_7D66_11D2_B4FA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DBSId.h"


class CQueryThread;

class CQuery  
{
	// construction/destruction
public:
	CQuery(DWORD dwUserID, DWORD dwClient, const CString& sQuery);
	CQuery(DWORD dwUserID, 
		   DWORD dwClient, 
		   int iNumArchives,
		   const WORD archives[],
		   int iNumFields,
		   const CIPCField fields[],
		   DWORD dwMaxQueryResults);
	virtual ~CQuery();

	// attributes
public:
	inline DWORD	GetClientID() const;
	inline DWORD	GetUserID() const;
	inline CString  GetExpression() const;
		   CArchiv*	GetAndRemoveArchiv();
		   BOOL		HasEnoughResponses();
	inline const CSystemTime& GetStartTime() const;
	inline const CSystemTime& GetEndTime() const;
	inline BOOL		IsForward() const;
	inline BOOL		IsSpecialQueryCancel() const;

	// operations
public:
	BOOL AddQueryResult(CSequence* pSequence, const CDBSId& id);
	void SendQueryResults(CClient* pClient);
	void IncreaseResponses();
	void Cancel();

	CQueryThread* m_pQueryThread;

	// implementation
protected:
	void Analyze();
	CString TranslateComparison(const CString& sComparison);
	void SendQueryResult(CIPCDatabaseServer* pCIPC, 
						 CSequence* pSequence, 
						 CDBSId* pDbsId);
	void DoConfirmQuery();
	int  GetIndicationDifferenz();
		
	// data member
private:
	CArchivs m_Archives;
	CString  m_sExpression;
	CString	 m_sQuery;
	DWORD	 m_dwClient;
	DWORD	 m_dwUserID;
	DWORD	 m_dwMaxQueryResults;
	DWORD	 m_dwSentQueryResults;

	int		 m_iResponses;
	int		 m_iLastIndicated;
	CDBSIds	 m_Results;

	BOOL	 m_bCancelled;
	BOOL	 m_bForward;
	BOOL	 m_bSpecialQueryCancel;
	BOOL	 m_bConfirmed;

	CSystemTime  m_stStart;
	CSystemTime  m_stEnd;

	friend class CQueryThread;
};
/////////////////////////////////////////////////////////////////////////////
inline CString CQuery::GetExpression() const
{
	return m_sExpression;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CQuery::GetClientID() const
{
	return m_dwClient;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CQuery::GetUserID() const
{
	return m_dwUserID;
}
/////////////////////////////////////////////////////////////////////////////
inline const CSystemTime& CQuery::GetStartTime() const
{
	return m_stStart;
}
/////////////////////////////////////////////////////////////////////////////
inline const CSystemTime& CQuery::GetEndTime() const
{
	return m_stEnd;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CQuery::IsForward() const
{
	return m_bForward;
}
/////////////////////////////////////////////////////////////////////////////
//On RectanglePlay (Client PlayButtons PlayForward and PlayBack and Rectangle)
//the query will be stoppt automatically after MaxQueryResults (1 in this case) is reached.
//Normally we have only one query and after delivering the queryresult the query is over
//and the querythread will be destroyed.
//In case of rectangle play we have one query after the other and the querythread should
//be alive until nothing more result were found or the query will be stoppt from outside (Client)
inline BOOL CQuery::IsSpecialQueryCancel() const
{
	return m_bSpecialQueryCancel;
}
/////////////////////////////////////////////////////////////////////////////
typedef CQuery* CQueryPtr;
WK_PTR_LIST_CS(CQueryList,CQueryPtr,CQueries)
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_QUERY_H__FF9C5E64_7D66_11D2_B4FA_004005A19028__INCLUDED_)
