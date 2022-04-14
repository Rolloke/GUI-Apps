/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ScanChk.cpp $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/ScanChk.cpp $
// CHECKIN:		$Date: 13.07.06 11:11 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 13.07.06 10:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "SecurityLauncher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
void DeleteChkFiles(char c)
{
	WIN32_FIND_DATA fd;
	HANDLE hFound;
	CString sPattern;
	CString sTmp;
	CString sFile;
	BOOL bDeleted;

	//
	sPattern.Format(_T("%c:\\*.chk"),c);
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
			WK_TRACE(_T("deleting scandisk file \n"),sFile);
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
				WK_TRACE(_T("successfull deleted scandisk file %s\n"),sFile);
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot delete scandisk file %s\n"),sFile);
			}
		}
	}
	FindClose(hFound);
}
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
void ScanDrive(char c)
{
	DeleteChkFiles(c);
	DeleteFoundDirs(c);
}
/////////////////////////////////////////////////////////////////////////////
void DeleteFiles(const CString& sDir, const CString &sPat, BOOL bDirectories = FALSE)
{
	WIN32_FIND_DATA fd;
	HANDLE hFound;
	CString sPattern;
	CString sTmp;
	CString sFile;
	BOOL bDeleted = TRUE;						   

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
				if (bDirectories)
				{
					sFile.Format(_T("%s\\%s"),sDir,sTmp);
					WK_TRACE(_T("deleting dir %s\n"),sFile);
					bDeleted = DeleteDirRecursiv(sFile);
				}
				else
				{
					bDeleted = FALSE;
				}
			}
			else
			{
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
			}
			if (bDeleted)
			{
				WK_TRACE(_T("successfull deleted %s\n"),sFile);
			}
		}
	}
	FindClose(hFound);
}
#define DELETE_TEMP_FILES		_T("DeleteTempFiles")
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::ScanChk()
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

	BOOL bDeleteTempfiles = theApp.GetIntValue(DELETE_TEMP_FILES, 1);
	if (bDeleteTempfiles)
	{
		// Lösche alle *.tmp Dateien im TEMP directory
		CString s = GetTempDirectory();
		if (s.GetLength()>3)
		{
			DeleteFiles(s, _T("*.*"),TRUE);
		}

		// Lösche alle *.tmp Dateien im Windows directory	
		DeleteFiles(GetWindowsDirectory(), _T("*.tmp"));
		// Lösche alle *.tmp Dateien im Windows\temp directory	
		s = GetWindowsDirectory()+_T("\\temp");
		if (s.GetLength()>3)
		{
			DeleteFiles(s, _T("*.*"),TRUE);
		}
	}
}
