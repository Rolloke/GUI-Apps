// SearchThread.h : header file
//
#if !defined(AFX_SEARCHTHREAD_H__A46575B3_8E71_11D2_B050_004005A1D890__INCLUDED_)
#define AFX_SEARCHTHREAD_H__A46575B3_8E71_11D2_B050_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
class CLogCountDlg;

/////////////////////////////////////////////////////////////////////////////
// CSearchThread thread
class CSearchThread : public CWK_Thread
{
public:
	CSearchThread(CLogCountDlg* pDlg,
				  const CString& sSearchPattern,
				  const CString& sErrorPattern);
	virtual ~CSearchThread();

	// attributes
public:

	// overrides
public:
	virtual	BOOL Run(LPVOID lpContext);


	// implementation
public:
	BOOL		StopSearchThread();
protected:
	void		Search();
	BOOL		ReadLine();
	BOOL		DecompressLGZFile(const CString& sName, const CString& sNew);
	CSystemTime	ExtractDateTimeFromFileName(const CString& sFileName);
	CSystemTime	ExtractDateTimeFromLine(const CString& sLine);
	BOOL		DeleteTempFile(const CString& sTempFileName);

protected:
	CLogCountDlg*	m_pDlg;
	CString			m_sSearchPattern;
	CString			m_sErrorPattern;
//	CMemFile*		m_pMemFile;
//	CStdioFile*		m_pStdioFile;
	CString			m_sLogFile;
	int				m_iCurrentIndex;
	BOOL			m_bSearch;
	DWORD			m_dwPatternCounter;
	DWORD			m_dwErrorCounter;
	CString			m_sCurrentLine;
	CString			m_sLastLineWithEntry;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHTHREAD_H__A46575B3_8E71_11D2_B050_004005A1D890__INCLUDED_)
