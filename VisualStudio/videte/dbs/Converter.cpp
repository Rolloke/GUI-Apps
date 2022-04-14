// Converter.cpp: implementation of the CConverter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "Converter.h"

#include "Archiv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConverter::CConverter(const CString& sDatabaseDir)
{
	// i.e. d:\database
	m_sDatabaseDir = sDatabaseDir;
}
//////////////////////////////////////////////////////////////////////
CConverter::~CConverter()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CConverter::DeleteForeignDir(const CString& sForeignDir)
{
	WK_TRACE(_T("CONVERT 3.x -> 4.0 foreign or empty dir found %s\n"),sForeignDir);
	if (DeleteDirRecursiv(sForeignDir))
	{
		WK_TRACE(_T("CONVERT 3.x -> 4.0 foreign or empty dir deleted %s\n"),sForeignDir);
		return TRUE;
	}
	else
	{
		WK_TRACE(_T("CONVERT 3.x -> 4.0 foreign or empty dir not deleted %s, %s\n"),
			sForeignDir,GetLastErrorString());
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CConverter::DeleteForeignFile(const CString& sForeignFile)
{
	WK_TRACE(_T("CONVERT 3.x -> 4.0 foreign file found %s\n"),sForeignFile);
	if (DeleteFile(sForeignFile))
	{
		WK_TRACE(_T("CONVERT 3.x -> 4.0 foreign file deleted %s\n"),sForeignFile);
		return TRUE;
	}
	else
	{
		WK_TRACE(_T("CONVERT 3.x -> 4.0 foreign file not deleted %s, %s\n"),
			sForeignFile,GetLastErrorString());
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CConverter::Convert()
{
	CString sSearch;

	sSearch = m_sDatabaseDir + "\\*.*";

	CString sTmp;
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;
	BOOL bRet = TRUE;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(sSearch, &found);
	
	if (hFound != (HANDLE)INVALID_HANDLE_VALUE)
	{
		for (BOOL bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			sTmp = found.cFileName; 
			sTmp.MakeLower();
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (sTmp == "." || sTmp == ".." )
				{
					// akuelles und übergeordnetes Verzeichnis ignorieren
					continue;
				}
				DWORD dwArchivNr=0;
				if (1 == _stscanf(sTmp,_T("archiv%hu"),&dwArchivNr))
				{
					bRet &= ConvertArchivDir((WORD)dwArchivNr);
				}
				else
				{
					bRet &= DeleteForeignDir(m_sDatabaseDir + '\\' + sTmp);
				}
			}
			else
			{
				bRet &= DeleteForeignFile(m_sDatabaseDir + '\\' + sTmp);
			}
		}
	}
	FindClose(hFound);

	if (!RemoveDirectory(m_sDatabaseDir))
	{
		WK_TRACE(_T("cannot remove old format database directory %s, %s\n"),
			m_sDatabaseDir,GetLastErrorString());
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CConverter::ConvertArchivDir(WORD wArchivNr)
{
	CString sSearch;
	CString sDir;

	sDir.Format(_T("%s\\archiv%d"),m_sDatabaseDir,wArchivNr);
	WK_TRACE(_T("CONVERT 3.x -> 4.0 archiv dir %s\n"),sDir);
	sSearch = sDir + _T("\\*.*");

	CString sTmp;
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;
	BOOL bRet = TRUE;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(sSearch, &found);
	
	if (hFound != (HANDLE)INVALID_HANDLE_VALUE)
	{
		for (BOOL bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			sTmp = found.cFileName; 
			sTmp.MakeLower();
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (sTmp == _T(".") || sTmp == _T("..") )
				{
					// akuelles und übergeordnetes Verzeichnis ignorieren
					continue;
				}
				// correct dir i.e. d:\database\archiv1\00000
				DWORD dwSequenceNr=0;
				if (1 == _stscanf(sTmp, _T("%08hu"), &dwSequenceNr))
				{
					if (ConvertSequenceDir(wArchivNr,(WORD)dwSequenceNr))
					{
						bRet &= TRUE;
					}
					else
					{
						bRet &= DeleteForeignDir(sDir + '\\' + sTmp);
					}
				}
				else
				{
					bRet &= DeleteForeignDir(sDir + '\\' + sTmp);
				}
			}
			else
			{
				bRet &= DeleteForeignFile(sDir + '\\' + sTmp);
			}
		}
	}

	FindClose(hFound);

	if (!RemoveDirectory(sDir))
	{
		WK_TRACE(_T("cannot remove old format database directory %s, %s\n"),
			sDir,GetLastErrorString());
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CConverter::ConvertSequenceDir(WORD wArchivNr, WORD wSequenceNr)
{
	CString sDir;

	sDir.Format(_T("%s\\archiv%d\\%08d"),m_sDatabaseDir,wArchivNr,wSequenceNr);
	WK_TRACE(_T("CONVERT 3.x -> 4.0 sequence dir %s\n"),sDir);

	// we must find a 00000001.dbf, and 00000001.dbx, or
	// 00000002.dbf, and 00000002.dbx for safe ring
	// and convert
	// d:\database\archiv1\00000001\00000001.dbf and
	// d:\database\archiv1\00000001\00000001.dbx
	// to
	// d:\dbs\arch0001\00000001.dbf
	// d:\dbs\arch0001\00000001.dbt
	// d:\dbs\arch0001\00000001.mdx

	WIN32_FIND_DATA	found;
	HANDLE			hFound = NULL;
	CString			sPattern;
	CString			sTmp;

	CSystemTime dbfTime;
	DWORD   dwDbfSize = 0;
	DWORD   dwDbxSize = 0;
	CString sDbf,sDbx;
	BOOL	bExist_dbf = FALSE;
	BOOL	bExist_dbx = FALSE;
	BOOL	bWasSafeRing = FALSE;
	BOOL	bRet = TRUE;


	sPattern = sDir + "\\*.*";
	hFound = FindFirstFile(sPattern, &found);
	if (hFound != (HANDLE)INVALID_HANDLE_VALUE)
	{
		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			sTmp = found.cFileName; 
			sTmp.MakeLower();

			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (sTmp == "." || sTmp == ".." )
				{
					// akuelles und übergeordnetes Verzeichnis ignorieren
					continue;
				}
				else
				{
					bRet &= DeleteForeignDir(sDir + '\\' + sTmp);
					continue;
				}
			}

			if (sTmp == "00000001.dbf")
			{
				bExist_dbf = TRUE;
				dwDbfSize += found.nFileSizeLow;
				sDbf = sTmp;
				FileTimeToSystemTime(&found.ftCreationTime,&dbfTime);
			}
			else if (sTmp == "00000001.dbx")
			{
				bExist_dbx = TRUE;
				dwDbxSize += found.nFileSizeLow;
				sDbx = sTmp;
			}
			else if (sTmp == "00000002.dbf")
			{
				bWasSafeRing = TRUE;
				bExist_dbf = TRUE;
				dwDbfSize += found.nFileSizeLow;
				sDbf = sTmp;
				FileTimeToSystemTime(&found.ftCreationTime,&dbfTime);
			}
			else if (sTmp == "00000002.dbx")
			{
				bExist_dbx = TRUE;
				dwDbxSize += found.nFileSizeLow;
				sDbx = sTmp;
			}
			else
			{
				// foreign file
				bRet &= DeleteForeignFile(sDir + '\\' + sTmp);
			}
		}
	}
	FindClose(hFound);

	if (bExist_dbf && bExist_dbx)
	{
		// now convert the two files
		bRet &= ConvertSequence(wArchivNr,wSequenceNr,
								dwDbfSize+dwDbxSize,
								dbfTime,
								bWasSafeRing,
								sDbf);
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CConverter::ConvertSequence(WORD wArchivNr, 
								 WORD wSequenceNr, 
								 DWORD dwSize,
								 const CSystemTime& dbfTime,
								 BOOL bWasSafeRing,
								 const CString& sDbf)
{
	BOOL bRet = TRUE;
	CArchiv* pArchiv;

	pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);

	if (pArchiv)
	{
		theApp.m_Conversions.Add(new CConversion(m_sDatabaseDir,wArchivNr,wSequenceNr,dwSize,dbfTime,bWasSafeRing,sDbf));
		bRet = TRUE;
	}
	else
	{
		WK_TRACE(_T("did't find matching archiv %d \n"),wArchivNr);
		bRet = FALSE;
	}
	return bRet;
}
