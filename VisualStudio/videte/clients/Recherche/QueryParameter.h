// QueryParameter.h: interface for the CQueryParameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYPARAMETER_H__FD687121_A931_11D2_B552_004005A19028__INCLUDED_)
#define AFX_QUERYPARAMETER_H__FD687121_A931_11D2_B552_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQueryParameter  
{
	// construction/destruction
public:
	CQueryParameter();
	virtual ~CQueryParameter();

	// attributes
public:
	inline BOOL  QueryAll() const;
	inline BOOL	 UseExactDate() const;
	inline BOOL	 UseExactTime() const;
	inline BOOL	 UseTimeFromTo() const;
	inline BOOL  UseRectToQuery() const;
	inline BOOL	 ShowPictures() const;
	inline BOOL	 CopyPictures() const;

	inline CIPCFields& GetFields();
	
	inline const CSystemTime& GetDateExact() const;
	inline const CSystemTime& GetTimeExact() const;
	inline const CSystemTime& GetPCDateEnd() const;
	inline const CSystemTime& GetTimeEnd() const;
	inline CRect GetRectToQuery() const;


	// operations
public:

	void SetAddField(const CIPCField& field);
	void DeleteFields();

	void SetAll(BOOL bAll);
	void SetExactDate(BOOL bExactDate);
	void SetExactTime(BOOL bExactTime);
	void SetTimeFromTo(BOOL bTimeFromTo);
	void SetShowPictures(BOOL bShowPictures);
	void SetCopyPictures(BOOL bCopyPictures);

	void SetExactDate(const CSystemTime& t);
	void SetExactTime(const CSystemTime& t);
	void SetPCDateEnd(const CSystemTime& t);
	void SetTimeEnd(const CSystemTime& t);
	void SetRectToQuery(CRect rect);



	void Load(const CString& sSection, CWK_Profile& wkp);
	void Save(const CString& sSection, CWK_Profile& wkp);

private:
	CIPCFields  m_Fields;

	CSystemTime m_stDateExact;

	CSystemTime	m_stTimeExact;
	CSystemTime m_stPCDateEnd;
	CSystemTime m_stTimeEnd;

	//enthält in m_rect die Eckpunkte des Rectangles, in dem gesucht werden soll
	//die Eckpunkte sind in Promille, bezüglich des zugrundeliegenden 
	//Bildes gespeichert
	CRect	m_RectToQuery; 

	BOOL	m_bExactDate;
	BOOL	m_bExactTime;
	BOOL	m_bTimeFromTo;

	BOOL	m_bAll;
	BOOL	m_bShowPictures;
	BOOL	m_bCopyPictures;
};
//////////////////////////////////////////////////////////////////////
inline CIPCFields& CQueryParameter::GetFields()
{
	return m_Fields;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CQueryParameter::QueryAll() const
{
	return m_bAll;
}
//////////////////////////////////////////////////////////////////////
inline const CSystemTime& CQueryParameter::GetDateExact() const
{
	return m_stDateExact;
}
//////////////////////////////////////////////////////////////////////
inline const CSystemTime& CQueryParameter::GetPCDateEnd() const
{
	return m_stPCDateEnd;
}
//////////////////////////////////////////////////////////////////////
inline const CSystemTime& CQueryParameter::GetTimeExact() const
{
	return m_stTimeExact;
}
//////////////////////////////////////////////////////////////////////
inline const CSystemTime& CQueryParameter::GetTimeEnd() const
{
	return m_stTimeEnd;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CQueryParameter::UseExactDate() const
{
	return m_bExactDate;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CQueryParameter::UseExactTime() const
{
	return m_bExactTime;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CQueryParameter::UseTimeFromTo() const
{
	return m_bTimeFromTo;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CQueryParameter::UseRectToQuery() const
{
	return !m_RectToQuery.IsRectNull();
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CQueryParameter::ShowPictures() const
{
	return m_bShowPictures;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CQueryParameter::CopyPictures() const
{
	return m_bCopyPictures;
}
//////////////////////////////////////////////////////////////////////
inline CRect CQueryParameter::GetRectToQuery() const
{
	return m_RectToQuery;
}

class CQueryArchives  
{
	// construction/destruction
public:
	CQueryArchives();
	virtual ~CQueryArchives();

	// attributes
public:
	inline const CWordArray& GetArchives() const;

	// operations
public:
	CString GetOldArchives();
	BOOL IsArchive(WORD wArchive);
	void AddArchive(WORD wArchive);
	void DeleteArchives();

private:
	CWordArray  m_Archives;
};
//////////////////////////////////////////////////////////////////////
inline const CWordArray& CQueryArchives::GetArchives() const
{
	return m_Archives;
}
#endif // !defined(AFX_QUERYPARAMETER_H__FD687121_A931_11D2_B552_004005A19028__INCLUDED_)
