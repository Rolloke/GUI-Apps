// QueryResult.h: interface for the CQueryResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYRESULT_H__D8EA41A3_AAC0_11D2_B555_004005A19028__INCLUDED_)
#define AFX_QUERYRESULT_H__D8EA41A3_AAC0_11D2_B555_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQueryResult  
{
public:
	CQueryResult(DWORD dwUserID,
				 WORD wServer,
				 WORD wArchive, 
				 WORD wSequenceNr,
				 DWORD dwRecordNr,
				 int iNumFields,
				 const CIPCField fields[]);
	CQueryResult(WORD wServer, const CIPCPictureRecord &pictData);
	virtual ~CQueryResult();

public:
	inline DWORD GetUserID() const;
	inline WORD	 GetServer() const;
	inline WORD	 GetArchiveNr() const;
	inline WORD	 GetSequenceNr() const;
	inline DWORD GetRecordNr() const;
	inline const CIPCFields& GetFields() const;

		   BOOL  IsEqual(WORD wServer,WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr);

private:
	DWORD m_dwUserID;
	WORD  m_wServer;
	WORD  m_wArchiveNr;
	WORD  m_wSequenceNr;
	DWORD m_dwRecordNr;
	CIPCFields m_Fields;
};
//////////////////////////////////////////////////////////////////////
inline DWORD CQueryResult::GetUserID() const
{
	return m_dwUserID;
}
//////////////////////////////////////////////////////////////////////
inline WORD	CQueryResult::GetServer() const
{
	return m_wServer;
}
//////////////////////////////////////////////////////////////////////
inline WORD	CQueryResult::GetArchiveNr() const
{
	return m_wArchiveNr;
}
//////////////////////////////////////////////////////////////////////
inline WORD	CQueryResult::GetSequenceNr() const
{
	return m_wSequenceNr;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CQueryResult::GetRecordNr() const
{
	return m_dwRecordNr;
}
//////////////////////////////////////////////////////////////////////
inline const CIPCFields& CQueryResult::GetFields() const
{
	return m_Fields;
}
//////////////////////////////////////////////////////////////////////
typedef CQueryResult* CQueryResultPtr;
WK_PTR_LIST_CS(CQueryResultList,CQueryResultPtr,CQueryResultListCS);
WK_PTR_ARRAY_CS(CQueryResultArray,CQueryResultPtr,CQueryResultArrayCS);
//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_QUERYRESULT_H__D8EA41A3_AAC0_11D2_B555_004005A19028__INCLUDED_)
