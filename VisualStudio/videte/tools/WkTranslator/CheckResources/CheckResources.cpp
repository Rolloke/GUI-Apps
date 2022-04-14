// CheckResources.cpp : Defines the entry point for the console application.
//

//#pragma warning(push, 3)
#include "stdafx.h"
//#pragma warning(pop)

#include "CheckResources.h"
#include "..\Scanner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
CWinApp theApp;

/////////////////////////////////////////////////////////////////////////////
using namespace std;

/////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	UNREFERENCED_PARAMETER(envp);

	int nRetCode = 1;
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_ftprintf(stdout, _T("Fatal Error: MFC initialization failed\n"));
	}
	else
	{
		if (argc<2)
		{
			_ftprintf(stdout, _T("Zu wenig Parameter angegeben\n"));
			_ftprintf(stdout, _T("Aufruf: checkresources <Directory>\n"));
		}
		else
		{
			CString sDir = argv[1];
			int iBufferLen = MAX_PATH;
			CString sCurrentDir;
			/*DWORD dwDirLen = */
			GetCurrentDirectory(iBufferLen, sCurrentDir.GetBuffer(iBufferLen+1));
			
			sCurrentDir.ReleaseBuffer();
			if (SetCurrentDirectory(sDir))
			{
				CStringArray sRCFiles;
				int iFiles = SearchFiles(sRCFiles, sDir, "*.rc");
				if (iFiles > 1)
				{
					CString sRCFile;
					int iTokens = -1;
//					BOOL bOK = TRUE;
					for (int i=0 ; i<sRCFiles.GetSize() ; i++)
					{
						sRCFile = sRCFiles.GetAt(i);
						TRACE(_T("%s\n"), (LPCTSTR)sRCFile);
						CScanner scanner(sRCFiles.GetAt(i));
						if (scanner.Scan())
						{
							if (iTokens == -1)
							{
								iTokens = scanner.GetTokens()->GetSize();
							}
							else if (scanner.GetTokens()->GetSize() != iTokens)
							{
								_ftprintf(stdout, _T("ERROR %s: Fehler bei der Übersetzung\n"), sRCFile);
								break;
							}
						}
						else
						{
							if (scanner.GetTokens()->GetSize())
							{
								CToken* pToken = scanner.GetTokens()->GetAt(scanner.GetTokens()->GetSize()-1);
								_ftprintf(stdout, _T("ERROR %s(%d): Fehler beim Scannen\n"), sRCFile, pToken->GetLine());
								_ftprintf(stdout, _T("%s\n"), pToken->GetOriginal());
							}
							else
							{
								_ftprintf(stdout, _T("ERROR %s(0): Fehler beim Scannen\n"), sRCFile);
							}
							break;
						}
					}
				}
				else if (iFiles == 1)
				{
					_ftprintf(stdout, _T("Nur 1 RC-Datei im  Verzeichnis <%s> gefunden\n"),sDir);
				}
				else
				{
					_ftprintf(stdout, _T("Keine RC-Dateien im  Verzeichnis <%s> gefunden\n"), sDir);
				}
			}
			else
			{
				_ftprintf(stdout, _T("Konnte das Verzeichnis <%s> nicht öffnen\n"), sDir);
			}
		}
	}

	return nRetCode;
}
/////////////////////////////////////////////////////////////////////////////
int SearchFiles(
			   CStringArray &result,
			   const CString sPathname,
			   const CString &sPattern
			   )
{
	HANDLE hF;
	WIN32_FIND_DATA FindFileData;
	CString sSearch;

	int iNumFound=0;

	sSearch = sPathname;
	if (sSearch.GetLength()==0)
	{
		sSearch += ".\\";
	}
	else if (sSearch[sSearch.GetLength()-1]!='\\')
	{
		sSearch += '\\';
	}
	sSearch += sPattern;

	hF = FindFirstFile(LPCTSTR(sSearch), &FindFileData);
	if (hF != INVALID_HANDLE_VALUE)
	{
		if ( (wkstrcmp(FindFileData.cFileName, _T("."))!=0) &&
			  (wkstrcmp(FindFileData.cFileName, _T(".."))!=0))
		{
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				result.Add(FindFileData.cFileName);
				iNumFound++;
			}
		} 
		while (FindNextFile(hF,&FindFileData))
		{
			if ( (wkstrcmp(FindFileData.cFileName, _T("."))!=0) &&
				  (wkstrcmp(FindFileData.cFileName, _T(".."))!=0))
			{
				if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					result.Add(FindFileData.cFileName);
					iNumFound++;
				}
			}
		}
		FindClose(hF);
	}
	return iNumFound;
}
