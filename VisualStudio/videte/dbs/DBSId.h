// DBSId.h: interface for the CDBSId class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBSID_H__E7140593_CA57_11D2_B58C_004005A19028__INCLUDED_)
#define AFX_DBSID_H__E7140593_CA57_11D2_B58C_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDBSId  
{
	// construction/destruction
public:
	CDBSId();
	CDBSId(WORD	wArchiveNr,WORD	wSequenceNr,DWORD dwRecordNr);
	virtual ~CDBSId();

	// attributes
public:
	inline WORD GetArchiveNr() const;
	inline WORD GetSequenceNr() const;
	inline DWORD GetRecordNr() const;

	// data member
private:
	WORD	m_wArchiveNr;
	WORD	m_wSequenceNr;
	DWORD	m_dwRecordNr;
};
/////////////////////////////////////////////////////////////////////////////
inline WORD CDBSId::GetArchiveNr() const
{
	return m_wArchiveNr;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CDBSId::GetSequenceNr() const
{
	return m_wSequenceNr;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CDBSId::GetRecordNr() const
{
	return m_dwRecordNr;
}
/////////////////////////////////////////////////////////////////////////////
typedef CDBSId* CDBSIdPtr;
WK_PTR_ARRAY_CS(CDBSIdArray,CDBSIdPtr,CDBSIds)
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DBSID_H__E7140593_CA57_11D2_B58C_004005A19028__INCLUDED_)
