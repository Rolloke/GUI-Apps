// Conversion.cpp: implementation of the CConversion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Conversion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConversion::CConversion(const CString& sDatabaseDir,
						 WORD wArchivNr,
						 WORD wSequenceNr, 
						 DWORD dwSize,
						 const CSystemTime& dbfTime,
						 BOOL bWasSafeRing,
						 const CString& sDbf)
{
	m_sDatabaseDir = sDatabaseDir;
	m_wArchivNr = wArchivNr;
	m_wSequenceNr = wSequenceNr; 
	m_dwSize = dwSize;
	m_dbfTime = dbfTime;
	m_bWasSafeRing = bWasSafeRing;
	m_sDbf = sDbf;
}
//////////////////////////////////////////////////////////////////////
CConversion::~CConversion()
{

}
//////////////////////////////////////////////////////////////////////
CString CConversion::GetDbfPath() const
{
	CString sDbfPath;
	sDbfPath.Format(_T("%s\\archiv%d\\%08d\\%s"),GetDatabaseDir(),m_wArchivNr,
					GetSequenceNr(),GetDbf());
	return sDbfPath;
}
//////////////////////////////////////////////////////////////////////
BOOL CConversion::DeleteFiles()
{
	CString sDbfPath,sDbxPath,toRemove;
	BOOL bRet = TRUE;

	sDbfPath = GetDbfPath();

	if (!DeleteFile(sDbfPath))
	{
		WK_TRACE_ERROR(_T("cannot delete old format file %s, %s\n"),sDbfPath,GetLastErrorString());
		bRet = FALSE;
	}

	sDbxPath = sDbfPath;
	sDbxPath.SetAt(sDbxPath.GetLength()-1,'x');
	if (DoesFileExist(sDbxPath))
	{
		if (!DeleteFile(sDbxPath))
		{
			WK_TRACE_ERROR(_T("cannot delete old format file %s, %s\n"),sDbxPath,GetLastErrorString());
			bRet = FALSE;
		}
	}

	int p;
	p = sDbfPath.ReverseFind('\\');
	toRemove = sDbfPath.Left(p);
	if (!RemoveDirectory(toRemove))
	{
		WK_TRACE_ERROR(_T("cannot remove old format sequence directory %s, %s\n"),
			toRemove,GetLastErrorString());
		bRet = FALSE;
	}

	p = toRemove.ReverseFind('\\');
	toRemove = toRemove.Left(p);
	RemoveDirectory(toRemove); // only try, may fail

	p = toRemove.ReverseFind('\\');
	toRemove = toRemove.Left(p);
	RemoveDirectory(toRemove); // only try, may fail

	return bRet;
}
