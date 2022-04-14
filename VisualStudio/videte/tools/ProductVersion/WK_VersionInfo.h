
#ifndef _WK_VersionDB_H_
#define _WK_VersionDB_H_

#include <afxwin.h>

#include "wk.h"
#include "WK_Template.h"
#include "wkclasses\WK_Trace.h"


class CWK_VersionInfo
{
public:
	// construct from file 
	CWK_VersionInfo(const CString &sDir,const CString &sFileName,
		BOOL bIgnoreOS
		);
	// construct from attributes (.dat) file
	CWK_VersionInfo(
		const CString & sFileName,
		const CString & sVersionText,
		DWORD	dwFileSize,
		CTime	tFileTime
		);
	//
	inline const CString GetFileName() const;
	inline const CString &GetVersionText() const;
	inline DWORD GetFileSize() const;
	inline CTime GetFileTime() const;
	//
	inline BOOL IgnoreFile() const;
	//
	CString GetString() const;	// single line string representation
	//
	// returns TRUE if equal
	BOOL CompareWith(const CWK_VersionInfo &x,
			BOOL &bSameName,
			BOOL &bSameVersion,
			BOOL &bSameSize,
			BOOL &bSameTime,
			BOOL bIgnoreTime=FALSE
			) const;
private:
	void Init();
	void CheckOSVersion();
	//
	CString m_sDirName;	// not stored in .pvi
	CString m_sFileName;
	CString m_sVersionText;
	DWORD	m_dwFileSize;
	CTime	m_tFileTime;
	BOOL	m_bIgnore;
};

inline BOOL CWK_VersionInfo::IgnoreFile() const
{
	return m_bIgnore;
}

typedef CWK_VersionInfo* CWK_VersionInfoPtr;
WK_PTR_ARRAY_CS(CWK_VersionInfosPlain,CWK_VersionInfoPtr,CWK_VersionInfos);

class CWK_VersionDB : public CWK_VersionInfos
{
public:
	CWK_VersionDB();
	// creation
	BOOL ScanDirectory(DWORD dwBuildNumber, const CString &sDir, 
						BOOL bIgnoreOS
						);
	BOOL ReadFromFile(const CString &sDatabase);
	//
	BOOL WriteToFile(const CString &sDatabase);
	//
	inline DWORD GetBuildNumber() const;
	// NOT YET CStringArray GetAllInfos();
	// NOT YET CStringArray CalcDiffs(const CWK_VersionDB &oldDB);
	const CWK_VersionInfo *GetByName(const CString &sName) const;
	//
	inline const CString &GetFileName() const;	// only set in ReadFromFile
private:
	DWORD m_dwBuildNumber;
	CString m_sFileName;	// set in ReadFromFile
};


inline const CString CWK_VersionInfo::GetFileName() const
{
	CString s(m_sFileName);

	if (s.GetLength()) {
		if (s[0]=='%') {
			s=s.Mid(1);
		}
	}

	if (s.GetLength()) {
		if (s[0]=='!') {
			s=s.Mid(1);
		}
	}

	if (s.GetLength()) {
		if (s[0]=='$') {
			s=s.Mid(1);
		}
	}
	return s;
}
inline const CString &CWK_VersionInfo::GetVersionText() const
{
	return m_sVersionText;
}
inline DWORD CWK_VersionInfo::GetFileSize() const
{
	return m_dwFileSize;
}
inline CTime CWK_VersionInfo::GetFileTime() const
{
	return m_tFileTime;
}

inline DWORD CWK_VersionDB::GetBuildNumber() const
{
	return m_dwBuildNumber;
}

inline const CString &CWK_VersionDB::GetFileName() const	// only set in ReadFromFile
{
	return m_sFileName;
}
#endif
