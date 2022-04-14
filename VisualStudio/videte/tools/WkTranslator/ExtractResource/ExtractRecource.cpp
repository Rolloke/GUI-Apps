// ExtractRecource.cpp

#include "stdafx.h"

#include "..\Scanner.h"
#include "..\TranslateString.h"
#include "..\Parser.h"

// Command Line is
// extractresource OriginalResourceFile SprachAbkürzung
#ifdef _UNICODE
int wmain( int argc, wchar_t *argv[])
#else
int main( int argc, char *argv[] )
#endif
{
	if (argc<3)
	{
		_ftprintf(stdout, _T("Zu wenig Parameter angegeben\n"));
		_ftprintf(stdout, _T("Aufruf: extractresource <OriginalResourceFile> <SprachAbkuerzung> [/r]\n"));
		_ftprintf(stdout, _T("Der Schalter /r ueberschreibt die SprachAbkuerzung im OriginalResourceFile\n"));
	}
	else
	{
		CTSArray m_TSArray;
		CTSArray* pTSArrayEnu = NULL;
/*
#ifdef _UNICODE
		CString sSource, sLang, sReplace;
		int    nLen;
		LPTSTR str;
		if (argv[1])
		{
			nLen = strlen(argv[1]);
			str  = sSource.GetBufferSetLength(nLen);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[1], nLen, str, nLen);
		}
		if (argv[2])
		{
			nLen = strlen(argv[2]);
			str  = sLang.GetBufferSetLength(nLen);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[2], nLen, str, nLen);
		}
		if (argv[3])
		{
			nLen = strlen(argv[3]);
			str  = sReplace.GetBufferSetLength(nLen);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[3], nLen, str, nLen);
		}
#else
*/
		CString sSource  = argv[1];
		CString sLang    = argv[2];
		CString sReplace = argv[3];
//#endif
		CScanner scanner(sSource);
		if (argc>3)
		{
			CString sArg4 = argv[4];
			if (sArg4.CompareNoCase(_T("/t")) == 0)
			{
				scanner.SetTolerant(TRUE);
			}
		}

		CString sDest;
		CFileStatus cfs;
		BOOL bCheckOut = FALSE;
		int p;

		sSource.MakeLower();
		p = sSource.Find(_T(".rc"));
		if (p!=-1)
		{
			CString sSourceBase;
			if (sReplace.CompareNoCase(_T("/r")) == 0)
			{
				sSourceBase = sSource.Left(p-sLang.GetLength());
			}
			else
			{
				sSourceBase = sSource.Left(p);
			}

			sDest.Format(_T("%s%s.lng"), sSourceBase, sLang);

			if (CFile::GetStatus(sDest, cfs))
			{
				// destination file exists
				if (cfs.m_attribute & CFile::readOnly)
				{
					bCheckOut = TRUE;
				}
			}

			if (!bCheckOut)
			{
				// load Enu.lng file or Enu.lngold file for additional info (Length and comment)
				CString sEnuLngFile = sSourceBase + _T("Enu") + _T(".lng");
				CFileStatus fileStatus;
				BOOL bFound = CFile::GetStatus(sEnuLngFile, fileStatus);
				if (bFound == FALSE)
				{
					sEnuLngFile += _T("old");
					bFound = CFile::GetStatus(sEnuLngFile, fileStatus);
				}
				if (bFound)
				{
					pTSArrayEnu = new CTSArray();
					if (pTSArrayEnu)
					{
						int iRet = pTSArrayEnu->Load(sEnuLngFile);
						switch (iRet)
						{
						case 1:
							break; // ok
						case 0:
							_ftprintf(stdout, _T("ERROR: Datei %s nicht geladen, keine zusätzlichen Informationen verfügbar\n"), sEnuLngFile);
							break;
						case -1:
							_ftprintf(stdout, _T("FEHLER: Datei %s ist keine ANSI-Datei\n"), sEnuLngFile);
							exit(-1);
						}
					}
				}
				else
				{
					_ftprintf(stdout, _T("Datei %s nicht gefunden, keine zusätzlichen Informationen verfügbar\n"), sEnuLngFile);
				}

				// load the real LNG file
				int iRet = m_TSArray.Load(sDest);
				switch (iRet)
				{
				case 1:
					_ftprintf(stdout, _T("Aktualisiere %s %d Eintraege\n"), sDest,m_TSArray.GetSize());
					break; // ok
				case 0:
					_ftprintf(stdout, _T("Datei %s nicht gefunden, wird erzeugt\n"), sDest);
					break;
				case -1:
					_ftprintf(stdout, _T("FEHLER: Datei %s ist keine ANSI-Datei\n"), sDest);
					exit(-1);
					break;
				}

				_ftprintf(stdout, _T("Scanne %s\n"), sSource);

				if (scanner.Scan())
				{
					_ftprintf(stdout, _T("Parse\n"));
					CParser parser(&scanner, &m_TSArray, pTSArrayEnu, TRUE);
					if (parser.Parse())
					{
						m_TSArray.Save(sDest);
						_ftprintf(stdout, _T("%i zu übersetzende Texte gefunden\n"), m_TSArray.GetSize());
						_ftprintf(stdout, _T("%i zu übersetzende Worte gefunden\n"), m_TSArray.GetNumOfWords());
						_ftprintf(stdout, _T("Texte in Datei %s geschrieben\n"), sDest);
					}
					else
					{
						_ftprintf(stdout, _T("Fehler beim Parsen\n"));
					}
				}
				else
				{
					if (scanner.GetTokens()->GetSize())
					{
						CToken* pToken = scanner.GetTokens()->GetAt(scanner.GetTokens()->GetSize()-1);
						_ftprintf(stdout, _T("%s (%i): Fehler beim Scannen\n"), sSource, pToken->GetLine());
						_ftprintf(stdout, _T("%s\n"), pToken->GetOriginal());
					}
					else
					{
						_ftprintf(stdout, _T("%s (0): Fehler beim Scannen\n"), sSource);
					}
				}
				if (pTSArrayEnu)
				{
					delete pTSArrayEnu;
					pTSArrayEnu = NULL;
				}
			}
			else
			{
				_ftprintf(stdout, _T("Please check out %s!\n"), sDest);
			}
		}
		else
		{
			_ftprintf(stdout, _T("Keine RC Datei als Parameter angegeben\n"));
			_ftprintf(stdout, _T("Aufruf: extractresource <OriginalResourceFile> <SprachAbkürzung> [/r]\n"));
		}
	}
	
	_ftprintf(stdout, _T("ExtractResource DONE\n"));
	return 0;
}