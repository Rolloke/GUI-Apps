// VDBSequenceHashEntry.h: interface for the CVDBSequenceHashEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VDBSEQUENCEHASHENTRY_H__38669A2E_9866_4D9E_99CB_8C42376CBAA0__INCLUDED_)
#define AFX_VDBSEQUENCEHASHENTRY_H__38669A2E_9866_4D9E_99CB_8C42376CBAA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVDBSequenceHashEntry : public CObject  
{
public:
	CVDBSequenceHashEntry(WORD wArchiveNr, 
						  WORD wSequenceNr, 
						  DWORD dwRecords, 
						  const CSystemTime& tFirst, 
						  const CSystemTime& tLast,
						  const CString& sName);
	CVDBSequenceHashEntry(const CVDBSequenceHashEntry& source);
	virtual ~CVDBSequenceHashEntry();

	// Attributes
public:
	inline DWORD GetKey() const;
	inline WORD  GetArchiveNr() const;
	inline WORD  GetSequenceNr() const;
	inline DWORD GetRecords() const;
	inline const CSystemTime& GetFirst() const;
	inline const CSystemTime& GetLast() const;
	inline const CString&	  GetName() const;

private:
	WORD		m_wArchiveNr;
	WORD		m_wSequenceNr;
	DWORD		m_dwRecords;
	CSystemTime	m_stFirst;
	CSystemTime	m_stLast;
	CString		m_sName;
};
///////////////////////////////////////////////////////////////////
inline DWORD CVDBSequenceHashEntry::GetKey() const
{
	return MAKELONG(m_wArchiveNr,m_wSequenceNr);
}
///////////////////////////////////////////////////////////////////
inline WORD  CVDBSequenceHashEntry::GetArchiveNr() const
{
	return m_wArchiveNr;
}
inline WORD  CVDBSequenceHashEntry::GetSequenceNr() const
{
	return m_wSequenceNr;
}
///////////////////////////////////////////////////////////////////
inline DWORD CVDBSequenceHashEntry::GetRecords() const
{
	return m_dwRecords;
}
///////////////////////////////////////////////////////////////////
inline const CSystemTime& CVDBSequenceHashEntry::GetFirst() const
{
	return m_stFirst;
}
///////////////////////////////////////////////////////////////////
inline const CSystemTime& CVDBSequenceHashEntry::GetLast() const
{
	return m_stLast;
}
///////////////////////////////////////////////////////////////////
inline const CString& CVDBSequenceHashEntry::GetName() const
{
	return m_sName;
}

typedef CMap<DWORD,DWORD,CVDBSequenceHashEntry*,CVDBSequenceHashEntry*&> CVDBMapDWordToSequenceHashEntryBase;

class CVDBSequenceMap : public CVDBMapDWordToSequenceHashEntryBase
{
	// construction/destruction
public:
	CVDBSequenceMap();
	virtual ~CVDBSequenceMap();

	// operations
public:
	CVDBSequenceHashEntry* GetSequenceHashEntry(WORD wArchivNr,
												WORD wSequenceNr) const;
	void SetSequenceHashEntry(WORD wArchivNr,
							  WORD wSequenceNr,
							  DWORD dwRecords,
							  const CSystemTime& tFirst,
							  const CSystemTime& tLast,
							  const CString& sName);
	void DeleteSequenceHashEntry(WORD wArchivNr,
			 					 WORD wSequenceNr);
	void DeleteAll();
	void Load(const CString& sPathname);
	void Save(const CString& sPathname);

	BOOL Create(const CString& sPathname);
	void Close();
	void Flush();
	void Delete();
	void AppendMap(const CVDBSequenceMap& map);

protected:
	void AppendEntry(WORD wArchivNr,
					 WORD wSequenceNr,
					 DWORD dwRecords,
					 const CSystemTime& tFirst,
					 const CSystemTime& tLast,
					 const CString& sName);
	BOOL GotoEntry(WORD wArchivNr,
				   WORD wSequenceNr);

	void SetEntry(WORD wArchivNr,
				 WORD wSequenceNr,
				 DWORD dwRecords,
				 const CSystemTime& tFirst,
				 const CSystemTime& tLast,
				 const CString& sName);

	BOOL AssignField(const char *name, const CString& sValue);
	BOOL AssignEntry(WORD wArchivNr,
					 WORD wSequenceNr,
					 DWORD dwNumRecords,
					 const CString& sFirst,
					 const CString& sLast,
					 const CString&	sName);

	// data member
private:
	Code4		m_CodeBase;
	Field4info  m_Fields;
	Data4		m_Data;
	CString		m_sPathname;
	CCriticalSection m_cs;
};

#endif // !defined(AFX_VDBSEQUENCEHASHENTRY_H__38669A2E_9866_4D9E_99CB_8C42376CBAA0__INCLUDED_)
