// SequenceHashEntry.h: interface for the CSequenceHashEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCEHASHENTRY_H__4FC543F3_2749_11D5_99D3_004005A19028__INCLUDED_)
#define AFX_SEQUENCEHASHENTRY_H__4FC543F3_2749_11D5_99D3_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSequenceHashEntry : public CObject
{
public:
	CSequenceHashEntry();
	CSequenceHashEntry(WORD wArchiveNr, WORD wSequenceNr, DWORD dwRecords,CTime tFirst, CTime tLast);
	virtual ~CSequenceHashEntry();

	// Attributes
public:
	inline DWORD GetKey() const;
	inline WORD  GetArchiveNr() const;
	inline WORD  GetSequenceNr() const;
	inline DWORD GetRecords() const;
	inline CTime GetFirst() const;
	inline CTime GetLast() const;

	// Operations
public:
	DECLARE_SERIAL(CSequenceHashEntry);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSequenceHashEntry)
	public:
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

private:
	WORD		m_wArchiveNr;
	WORD		m_wSequenceNr;
	DWORD		m_dwRecords;
	CTime		m_tFirst;
	CTime		m_tLast;
};
///////////////////////////////////////////////////////////////////
inline DWORD CSequenceHashEntry::GetKey() const
{
	return MAKELONG(m_wArchiveNr,m_wSequenceNr);
}
///////////////////////////////////////////////////////////////////
inline WORD  CSequenceHashEntry::GetArchiveNr() const
{
	return m_wArchiveNr;
}
inline WORD  CSequenceHashEntry::GetSequenceNr() const
{
	return m_wSequenceNr;
}
///////////////////////////////////////////////////////////////////
inline DWORD CSequenceHashEntry::GetRecords() const
{
	return m_dwRecords;
}
///////////////////////////////////////////////////////////////////
inline CTime CSequenceHashEntry::GetFirst() const
{
	return m_tFirst;
}
///////////////////////////////////////////////////////////////////
inline CTime CSequenceHashEntry::GetLast() const
{
	return m_tLast;
}

typedef CMap<DWORD,DWORD,CSequenceHashEntry*,CSequenceHashEntry*&> CMapDWordToSequenceHashEntry;

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
///////////////////////////////////////////////////////////////////
typedef CMap<DWORD,DWORD,CVDBSequenceHashEntry*,CVDBSequenceHashEntry*&> CVDBMapDWordToSequenceHashEntryBase;
///////////////////////////////////////////////////////////////////
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

	BOOL AssignEntry(const CString& sArchiveNr,
					 const CString& sSequenceNr,
					 const CString& sNrRecords,
					 const CString& sFirst,
					 const CString& sLast,
					 const CString&	sName);

	BOOL AssignField(const char *name, const CString& sValue);

	// data member
private:
	Code4		m_CodeBase;
	Field4info  m_Fields;
	Data4		m_Data;
	CString		m_sPathname;
	CCriticalSection m_cs;
};

#endif // !defined(AFX_SEQUENCEHASHENTRY_H__4FC543F3_2749_11D5_99D3_004005A19028__INCLUDED_)
