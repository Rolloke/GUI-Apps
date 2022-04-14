// CopyReadOnlyVersion.h: interface for the CopyReadOnlyVersionIdip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COPYREAD_H__C391D021_C4BA_11D2_B584_004005A19028__INCLUDED_)
#define AFX_COPYREAD_H__C391D021_C4BA_11D2_B584_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCopyReadOnlyVersion
{
	// construction / destruction
public:
	CCopyReadOnlyVersion(CString sDestDir);
	virtual ~CCopyReadOnlyVersion();

public:
	// attributes
	BOOL IsAlreadyDone();

	// operations
public:
	BOOL Run(BOOL bCopy);

	// implementation
private:
	BOOL CreateOemIni();
	BOOL CreateAutoRunInf();
	BOOL CopyFiles();
	BOOL DeleteFiles();

	// data member
private:
	CString			m_sDestinationDir;
	CStringArray 	m_saSourceFiles;

	CString			m_sOemIni;
	CString			m_sAutoRunInf;

	CString			m_sSourceDir;
	CString			m_sWindowsSystemDir;
	CString			m_sWindowsDir;
};

#endif // !defined(AFX_COPYREADER_H__C391D021_C4BA_11D2_B584_004005A19028__INCLUDED_)
