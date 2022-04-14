// FileOperations.cpp: implementation of the CFileOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wktranslator.h"
#include "FileOperations.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
void Translator_SplitPathAll(const CString sFullPath,
		  CString &sDrive,
		  CString &sPath, 
		  CString &sFilename,
		  CString &sExt) 
{
   _TCHAR drive[_MAX_DRIVE];
   _TCHAR dir[_MAX_DIR];
   _TCHAR fname[_MAX_FNAME];
   _TCHAR ext[_MAX_EXT];


   _tsplitpath( sFullPath, drive, dir, fname, ext );

   sDrive = drive;
   sPath = dir;
   sFilename = fname;
   sExt = ext;
#if 0
   WK_TRACE( _T("Path extracted with _splitpath:\n") );
   WK_TRACE( _T("  Drive: %s\n"), drive );
   WK_TRACE( _T("  Dir: %s\n"), dir );
   WK_TRACE( _T("  Filename: %s\n"), fname );
   WK_TRACE( _T("  Ext: %s\n"), ext );
#endif
}
/////////////////////////////////////////////////////////////////////////////
void Translator_SplitPath(const CString &sFullPath,
		  CString &sResultPath, 
		  CString &sResultFilename
		  )
{
	CString sDrive;
	CString sPath;
	CString sFilename;
	CString sExt;
	Translator_SplitPathAll(sFullPath,sDrive,sPath,sFilename,sExt);

	sResultPath = sDrive + sPath;
	sResultFilename = sFilename ;
	if (sExt.GetLength()) {
		sResultFilename +=  sExt;
	}
}
//////////////////////////////////////////////////////////////////////
int Translator_SearchFiles(
						   CStringArray &result,
						   const CString sPathname,
						   const CString &sPattern,
						   BOOL bIncludeDirectories /*=FALSE*/
						  )
{
	HANDLE hF;
	WIN32_FIND_DATA FindFileData;
	CString sSearch;

	int iNumFound=0;

	sSearch = sPathname;
	if (sSearch.GetLength()==0)
	{
		sSearch += _T(".\\");
	}
	else if (sSearch[sSearch.GetLength()-1]!='\\')
	{
		sSearch += '\\';
	}
	sSearch += sPattern;

	hF = FindFirstFile((LPCTSTR)sSearch,&FindFileData);
	if (hF != INVALID_HANDLE_VALUE)
	{
#ifdef _UNICODE
		if ( (wcscmp(FindFileData.cFileName, _T("." ))!=0) &&
			  (wcscmp(FindFileData.cFileName, _T(".."))!=0))
#else
		if ( (strcmp(FindFileData.cFileName, _T("." ))!=0) &&
			  (strcmp(FindFileData.cFileName, _T(".."))!=0))
#endif
		{
			if (bIncludeDirectories
				|| (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0
				)
			{
				result.Add(FindFileData.cFileName);
				iNumFound++;
			}
		} 
		while (FindNextFile(hF,&FindFileData))
		{
#ifdef _UNICODE
			if ( (wcscmp(FindFileData.cFileName, _T("." ))!=0) &&
				  (wcscmp(FindFileData.cFileName, _T(".."))!=0))
#else
			if ( (strcmp(FindFileData.cFileName, _T("." ))!=0) &&
				  (strcmp(FindFileData.cFileName, _T(".."))!=0))
#endif
			{
				if (bIncludeDirectories
					|| (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0
				)
				{
					result.Add(FindFileData.cFileName);
					iNumFound++;
				}
			}
		}
		FindClose(hF);
	}
	else
	{
#if 0
		// NOT YET error or pattern not matched
		WK_TRACE(_T("read directory failed %s, %s\n"),
				  (const char*)sPathname,(const char*)GetLastErrorString());
#endif
	}

	return iNumFound;
}
