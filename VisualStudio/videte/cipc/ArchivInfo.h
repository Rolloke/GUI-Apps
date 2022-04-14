/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ArchivInfo.h $
// ARCHIVE:		$Archive: /Project/CIPC/ArchivInfo.h $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 20 $
// LAST CHANGE:	$Modtime: 19.01.06 21:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _ARCHIV_HEADER_H_
#define _ARCHIV_HEADER_H_

// 

#include "SecID.h"
#include "ArchivDefines.h"
/////////////////////////////////////////////////////////////////////
//  CArchivInfo | A class to describe properties
// of a database server picture archive. Used for communication
// between Systemverwaltung and database server
// All CArchivInfo are collected in CArchivInfoArray, which
// is serialized into registry path HKLM\Software\Dvrt\Archive
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CArchivInfo 
{
	//  construction / destruction:
public:
	//!ic! constructor
	CArchivInfo();
	//!ic! destructor
	virtual ~CArchivInfo();

protected:
	//!ic! protected constructor for CArchivInfoArray
	CArchivInfo(CSecID id);

	//  attributes:
public:
	// returns the CSecID of the archive
	inline CSecID		GetID() const;
	// returns the unique WORD size id of the archive
	inline WORD			GetArchivNr() const;
	// returns the name of the archive
	inline CString		GetName() const;
	// returns the defined maximum size of the archive in MB
	inline DWORD		GetSizeMB() const;
	// returns the nr of the pre alarm ring archive if any
	inline WORD			GetSafeRingFor() const;
	// returns the type of the archive alarm, pre alarm or ring
	inline ArchivType	GetType() const;
	// returns the root string i.e. d: of the archive if any, else empty string
	inline CString		GetFixedDrive() const;
	// returns the max storage time in hours
	inline DWORD		GetMaxStorageTime() const;
	// returns whether archive has dynamic size or not
	inline DWORD		GetDynamicSize() const;

	//  operations:
public:
		   void SetName(const CString& sName);
	inline void SetArchivNr(WORD wNr);
	inline void SetSizeMB(DWORD dwSizeMB);
	inline void SetTyp(ArchivType typ);
	inline void SetSafeRingFor(WORD wID);
	inline void SetFixedDrive(const CString& sRoot);
	inline void SetMaxStorageTime(DWORD dwMaxStorageTime);
	inline void	SetDynamicSize(BOOL bDynamicSize);

	BOOL Load(int i,CWK_Profile& wkProfile);
	void Save(int i,CWK_Profile& wkProfile);

	// implementation
private:
	void Init();

	// data
private:
	CSecID			m_ID;
	CString			m_sName;
	DWORD			m_dwSizeMB;
	WORD			m_wSafeRingFor;
	ArchivType		m_ArchivTyp;
	CString			m_sFixedDrive;
	DWORD			m_dwMaxStorageTime;
	BOOL			m_bDynamicSize;

	friend class CArchivInfoArray;
};
/////////////////////////////////////////////////////////////////////
//  CArchivInfoArray | A collection(array) of CArchivInfo pointers
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CArchivInfoArray : public CTypedPtrArray<CPtrArray,CArchivInfo*>
{
	//  construction / destruction:
public:
	// NYD
	virtual ~CArchivInfoArray();

	//  operations:
public:
	//!ic! NYD
	void	Load(CWK_Profile& wkProfile);
	//!ic! NYD
	void	Save(CWK_Profile& wkProfile);

	//!ic! NYD
	CArchivInfo*  AddArchivInfo();
	//!ic! NYD
	void		  DeleteArchivInfo(CArchivInfo* pArchivInfo);
	//!ic! NYD
	CArchivInfo*  GetArchivInfo(const CSecID& id);
	//!ic! NYD
	void		  DeleteAll();

	//!ic! NYD
	DWORD		  GetSizeMB();
	//!ic! NYD
	DWORD		  GetSizeMBForFixedDrive(const CString& sRoot);
	inline CArchivInfo*  GetAtFast(int nPos) const;

private:
	CWordArray m_DeletedIDs;
};
/////////////////////////////////////////////////////////////////////////////
inline CString CArchivInfo::GetName() const
{
	return (m_sName);
}
/////////////////////////////////////////////////////////////////////////////
inline WORD CArchivInfo::GetArchivNr() const
{
	return (m_ID.GetSubID());
}
/////////////////////////////////////////////////////////////////////////////
inline ArchivType CArchivInfo::GetType() const
{
	return m_ArchivTyp;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CArchivInfo::GetSizeMB() const
{
	return m_dwSizeMB;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CArchivInfo::GetID() const
{
	return (m_ID);
}
/////////////////////////////////////////////////////////////////////////////
inline WORD	CArchivInfo::GetSafeRingFor() const
{
	return m_wSafeRingFor;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CArchivInfo::GetFixedDrive() const
{
	return m_sFixedDrive;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CArchivInfo::GetMaxStorageTime() const
{
	return m_dwMaxStorageTime;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CArchivInfo::GetDynamicSize() const
{
	return m_bDynamicSize;
}
/////////////////////////////////////////////////////////////////////////////
inline void CArchivInfo::SetArchivNr(WORD wNr)
{
	m_ID.SetSubID(wNr);
}
/////////////////////////////////////////////////////////////////////////////
inline void CArchivInfo::SetSizeMB(DWORD dwSizeMB)
{
	m_dwSizeMB = dwSizeMB;
}
/////////////////////////////////////////////////////////////////////////////
inline void CArchivInfo::SetTyp(ArchivType typ)
{
	m_ArchivTyp = typ;
}
/////////////////////////////////////////////////////////////////////////////
inline void CArchivInfo::SetSafeRingFor(WORD wID)
{
	m_wSafeRingFor = wID;
}
/////////////////////////////////////////////////////////////////////////////
inline void CArchivInfo::SetFixedDrive(const CString& sRoot)
{
	m_sFixedDrive = sRoot;
}
/////////////////////////////////////////////////////////////////////////////
inline void CArchivInfo::SetMaxStorageTime(DWORD dwMaxStorageTime)
{
	m_dwMaxStorageTime = dwMaxStorageTime;
}
/////////////////////////////////////////////////////////////////////////////
inline void	CArchivInfo::SetDynamicSize(BOOL bDynamicSize)
{
	m_bDynamicSize = bDynamicSize;
}
/////////////////////////////////////////////////////////////////////////////
inline CArchivInfo*  CArchivInfoArray::GetAtFast(int nPos) const
{
	ASSERT(nPos >= 0 && nPos < m_nSize);
	return (CArchivInfo* ) m_pData[nPos];
}
/////////////////////////////////////////////////////////////////////////////
#endif

