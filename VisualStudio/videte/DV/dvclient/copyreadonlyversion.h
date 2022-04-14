// CopyReadOnlyVersion.h: interface for the CCopyReadOnlyVersion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COPYREAD_H__C391D021_C4BA_11D2_B584_004005A19028__INCLUDED_)
#define AFX_COPYREAD_H__C391D021_C4BA_11D2_B584_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCopyReadOnlyVersion : public CWK_Thread  
{
	// construction / destruction
public:
	CCopyReadOnlyVersion(const CString& sPath, 
						 int iOem, 
						 int nCameras,
						 const CString& sLanguage,
						 BOOL bOverWrite);
	virtual ~CCopyReadOnlyVersion();

public:
	// attributes
	BOOL		 IsAlreadyDone(CString sDestinationPath);
	DWORD		 GetBytesReadOnlySoftware();
	void		 GetReadOnlyFiles(CStringArray &saROFiles);

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);

	// implementation
private:
	BOOL CreateAutoRunInf();
	BOOL CopyFiles();

	// data member
private:
	CString			m_sPath;
	CStringArray 	m_saSourceFiles;
	CString			m_sAutoRunInf;
	CString			m_sSourceDir;
	CString			m_sWindowsSystemDir;
	CString			m_sWindowsDir;
	int				m_iOem;
	int				m_nCameras;
	CString			m_sLanguage;
	BOOL			m_bOverWrite;
};

#endif // !defined(AFX_COPYREADER_H__C391D021_C4BA_11D2_B584_004005A19028__INCLUDED_)
