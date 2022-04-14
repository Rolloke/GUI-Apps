				  /////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ScanChk.cpp $
// ARCHIVE:		$Archive: /DV/DVStarter/ScanChk.cpp $
// CHECKIN:		$Date: 28.05.03 16:23 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 28.05.03 16:12 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "DVStarter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
void DeleteFoundDirs(char c)
{
	WIN32_FIND_DATA fd;
	HANDLE hFound;
	CString sPattern;
	CString sTmp;
	CString sDir;

	//
	sPattern.Format(_T("%c:\\found.*"),c);
	hFound = FindFirstFile(sPattern, &fd);

	if (hFound != INVALID_HANDLE_VALUE)
	{
		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &fd)) 
		{
			sTmp = fd.cFileName; 
			if (sTmp == _T(".") || sTmp == _T("..") )
			{
				// akuelles und übergeordnetes Verzeichnis ignorieren
				continue;
			}
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sDir.Format(_T("%c:\\%s"),c,sTmp);

				WK_TRACE(_T("deleting scandisk dir %s\n"),sDir);

				if (DeleteDirRecursiv(sDir))
				{
					WK_TRACE(_T("successfull deleted scandisk dir %s\n"),sDir);
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot delete scandisk dir %s\n"),sDir);
				}
			}
		}
	}
	FindClose(hFound);
}
/////////////////////////////////////////////////////////////////////////////
void DeleteFile(char c, const CString &sFileName)
{
	WIN32_FIND_DATA fd;
	HANDLE hFound;
	CString sPattern;
	CString sTmp;
	CString sFile;
	BOOL bDeleted;						   

	//
	sPattern.Format(_T("%c:\\%s"),c,sFileName);
	hFound = FindFirstFile(sPattern, &fd);

	if (hFound != INVALID_HANDLE_VALUE)
	{
		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &fd)) 
		{
			sTmp = fd.cFileName; 
			if (sTmp == _T(".") || sTmp == _T("..") )
			{
				// akuelles und übergeordnetes Verzeichnis ignorieren
				continue;
			}
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				continue;
			}
			sFile.Format(_T("%c:\\%s"),c,sTmp);
			WK_TRACE(_T("deleting file %s\n"),sFile);
			bDeleted = TRUE;

			TRY
			{
				CFile::Remove(sFile);
			}
			CATCH(CFileException, e)
			{
				bDeleted = FALSE;
			}
			END_CATCH

			if (bDeleted)
			{
				WK_TRACE(_T("successfull deleted file %s\n"),sFile);
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot delete file %s\n"),sFile);
			}
		}
	}
	FindClose(hFound);
}

/////////////////////////////////////////////////////////////////////////////
void DeleteFiles(const CString& sDir, const CString &sPat)
{
	WIN32_FIND_DATA fd;
	HANDLE hFound;
	CString sPattern;
	CString sTmp;
	CString sFile;
	BOOL bDeleted;						   

	//
	sPattern.Format(_T("%s\\%s"),sDir,sPat);
	hFound = FindFirstFile(sPattern, &fd);

	if (hFound != INVALID_HANDLE_VALUE)
	{
		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &fd)) 
		{
			sTmp = fd.cFileName; 
			if (sTmp == _T(".") || sTmp == _T("..") )
			{
				// akuelles und übergeordnetes Verzeichnis ignorieren
				continue;
			}
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				continue;
			}
			sFile.Format(_T("%s\\%s"),sDir,sTmp);
			WK_TRACE(_T("deleting file %s\n"),sFile);
			bDeleted = TRUE;

			TRY
			{
				CFile::Remove(sFile);
			}
			CATCH(CFileException, e)
			{
				bDeleted = FALSE;
			}
			END_CATCH

			if (bDeleted)
			{
				WK_TRACE(_T("successfull deleted file %s\n"),sFile);
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot delete file %s\n"),sFile);
			}
		}
	}
	FindClose(hFound);
}

/////////////////////////////////////////////////////////////////////////////
void ScanDrive(char c)
{
	DeleteFile(c, _T("*.chk"));
	DeleteFile(c, _T("Bootex.txt"));
	DeleteFoundDirs(c);
}
/////////////////////////////////////////////////////////////////////////////
void CDVStarterApp::ScanChk()
{
	DWORD dwD = GetLogicalDrives();
	int iBit = 1;
	char c = _T('a');
	CString sRoot;

	for (iBit = 1; iBit!=0; iBit<<=1,c++)
	{
		if (dwD & iBit)
		{
			sRoot.Format(_T("%c:\\"),c);
			if (DRIVE_FIXED==GetDriveType(sRoot))
			{
				ScanDrive(c);
			}
		}
	}

	// Lösche alle *.tmp Dateien im Windowsdirectory
	DeleteFiles(GetTempDirectory(), _T("*.tmp"));
	
	// Lösche alle *.tmp Dateien im Tempdirectory	
	DeleteFiles(GetWindowsDirectory(), _T("*.tmp"));
}


