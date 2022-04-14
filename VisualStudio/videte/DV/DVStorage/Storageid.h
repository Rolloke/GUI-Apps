// StorageId.h: interface for the CStorageId class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGE_H)
#define AFX_STORAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStorageId  
{
	// construction/destruction
public:
	inline CStorageId();
	inline CStorageId(WORD wCollectionNr,WORD wTapeNr,DWORD dwImageNr);

	// attributes
public:
	inline WORD GetCollectionNr() const;
	inline WORD GetTapeNr() const;
	inline DWORD GetImageNr() const;

	// data member
private:
	WORD	m_wCollectionNr;
	WORD	m_wTapeNr;
	DWORD	m_dwImageNr;
};
/////////////////////////////////////////////////////////////////////////////
inline WORD CStorageId::GetCollectionNr() const
{
	return m_wCollectionNr;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CStorageId::GetTapeNr() const
{
	return m_wTapeNr;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CStorageId::GetImageNr() const
{
	return m_dwImageNr;
}
//////////////////////////////////////////////////////////////////////
inline CStorageId::CStorageId()
{
	m_wCollectionNr = 0;
	m_wTapeNr = 0;
	m_dwImageNr = 0;
}
//////////////////////////////////////////////////////////////////////
inline CStorageId::CStorageId(WORD wCollectionNr,WORD wTapeNr,DWORD dwImageNr)
{
	m_wCollectionNr = wCollectionNr;
	m_wTapeNr = wTapeNr;
	m_dwImageNr = dwImageNr;
}
/////////////////////////////////////////////////////////////////////////////
typedef CStorageId* CStorageIdPtr;
WK_PTR_ARRAY_CS(CStorageIdArray,CStorageIdPtr,CStorageIds);
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DBSID_H__E7140593_CA57_11D2_B58C_004005A19028__INCLUDED_)
