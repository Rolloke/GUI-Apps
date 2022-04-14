// InsertResource.cpp

#include "stdafx.h"

#include "..\Scanner.h"
#include "..\TranslateString.h"
#include "..\Parser.h"

#define CODEPAGE_SCAN    _T("code_page(%x")
#define DEFAULT_CODEPAGE _T("code_page(1252)")
#define CHS_CODEPAGE     _T("code_page(936)")
#define CSY_CODEPAGE     _T("code_page(1250)")
#define PLK_CODEPAGE     _T("code_page(1250)")
#define RUS_CODEPAGE     _T("code_page(1251)")
#define JPN_CODEPAGE     _T("code_page(932)")

/////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
	BOOL ReplaceLanguageDefines(const CString& sFile, CString sLang, int nSaveTextAs);
#else
	BOOL ReplaceLanguageDefines(const CString& sFile, CString sLang);
#endif

/////////////////////////////////////////////////////////////////////////////
void PrintCmdLineOptions()
{
#ifdef _UNICODE
		_ftprintf(stdout, _T("Aufruf: insertresource <OriginalResourceFile> <SprachAbkürzung> [/r] [/u /a /cXXX]\n"));
		_ftprintf(stdout, _T("Der Schalter /r entfernt evtl. enthaltene TODOs\n"));
		_ftprintf(stdout, _T("Der Schalter /u speichert die RC-Datei im Unicode-Format\n"));
		_ftprintf(stdout, _T("Der Schalter /a speichert die RC-Datei im Ansi-Format\n"));
		_ftprintf(stdout, _T("Der Schalter /c speichert die RC-Datei im MBCS-Format\n"));
		_ftprintf(stdout, _T("mit der CodePage XXX ab.\n"));
		_ftprintf(stdout, _T("Die RC-Datei wird ohne Schalter im MBCS-Format mit einer von der\n"));
		_ftprintf(stdout, _T("SprachAbkürzung abhängigen CodePage abgespeichert.\n"));
#else
		_ftprintf(stdout, _T("Aufruf: insertresource <OriginalResourceFile> <SprachAbkürzung> [/r]\n"));
		_ftprintf(stdout, _T("Der Schalter /r entfernt evtl. enthaltene TODOs\n"));
#endif
}
/////////////////////////////////////////////////////////////////////////////
// Command Line is
// insertresource OriginalResourceFile SprachAbkürzung
#ifdef _UNICODE
int wmain( int argc, wchar_t *argv[])
#else
int main( int argc, char *argv[] )
#endif
{
	if (argc < 3)
	{
		_ftprintf(stdout, _T("Keine Dateien und Sprache als Parameter angegeben\n"));
		PrintCmdLineOptions();
	}
	else
	{
		CTSArray m_TSArray;
		CString sFilenameRC = argv[1];
		CString sLng        = argv[2];

		CString sOptionalParameter;
		CStringArray saOptionalParameters;
		if (argc > 3)
		{
			sOptionalParameter = argv[3];
			saOptionalParameters.Add(sOptionalParameter);
		}
		if (argc > 4)
		{
			sOptionalParameter = argv[4];
			saOptionalParameters.Add(sOptionalParameter);
		}

		BOOL bIgnoreTODO = FALSE;
		BOOL bTolerant   = FALSE;

#ifdef _UNICODE
		wchar_t *pstr = DEFAULT_CODEPAGE;
		if      (sLng.CompareNoCase(_T("chs")) == 0)
		{
			pstr = CHS_CODEPAGE;
		}
		else if (sLng.CompareNoCase(_T("csy")) == 0)
		{
			pstr = CSY_CODEPAGE;
		}
		else if (sLng.CompareNoCase(_T("plk")) == 0)
		{
			pstr = PLK_CODEPAGE;
		}
		else if (sLng.CompareNoCase(_T("rus")) == 0)
		{
			pstr = RUS_CODEPAGE;
		}
		else if (sLng.CompareNoCase(_T("jpn")) == 0)
		{
			pstr = JPN_CODEPAGE;
		}
		else
		{
			// TODO! RKE: find codpage from abbreviation
		}
		int  nSaveTextAs = _ttoi(&pstr[10]);
#endif

		for (int i=0 ; i<saOptionalParameters.GetSize() ; i++)
		{
			sOptionalParameter = saOptionalParameters.GetAt(i);
			if (sOptionalParameter.CompareNoCase(_T("/r")) == 0)
			{
				bIgnoreTODO = TRUE;
				_ftprintf(stdout, _T("Ignore TODOs\n"));
			}
#ifdef _UNICODE
			else if (sOptionalParameter.CompareNoCase(_T("/u")) == 0)
			{
				nSaveTextAs = 1;
			}
			else if (sOptionalParameter.CompareNoCase(_T("/a")) == 0)
			{
				nSaveTextAs = 0;
			}
			else if (sOptionalParameter.CompareNoCase(_T("/t")) == 0)
			{
				bTolerant = TRUE;
			}
			else if (sOptionalParameter.Left(2).CompareNoCase(_T("/c")) == 0)
			{
				nSaveTextAs = _ttoi(sOptionalParameter.Mid(2));
			}
#endif
		}
		
#ifdef _UNICODE
		switch (nSaveTextAs)
		{
			case 0: _ftprintf(stdout, _T("Save as Ansi\n"));    break;
			case 1: _ftprintf(stdout, _T("Save as UNICODE\n")); break;
			default:_ftprintf(stdout, _T("Save as MBCS with Codepage %d\n"), nSaveTextAs);
		}
#endif

		int p = sFilenameRC.Find(_T(".rc"));
		if (p!=-1)
		{
			CString sFilenameLng = sFilenameRC.Left(p) + sLng + _T(".lng");
			if (sLng.CompareNoCase(_T("enuenu")) == 0)
			{
				sLng = _T("Enu");
			}
			CString sFilenameDest = sFilenameRC.Left(p) + sLng + _T(".rc");
			CScanner scanner(sFilenameRC, TRUE);
			scanner.SetTolerant(bTolerant);
			CFileStatus cfs;
			BOOL bCheckOut = FALSE;
			if (CFile::GetStatus(sFilenameDest, cfs))
			{
				// destination file exists
				if (cfs.m_attribute & CFile::readOnly)
				{
					bCheckOut = TRUE;
				}
			}
			if (!bCheckOut)
			{
				_ftprintf(stdout, _T("Lese %s\n"), sFilenameLng);
				int iRet = m_TSArray.Load(sFilenameLng);
				if (iRet == 1)
				{
					_ftprintf(stdout, _T("%i zu übersetzende Texte gefunden\n"), m_TSArray.GetSize());
					_ftprintf(stdout, _T("Scanne %s\n"), sFilenameRC);

					if (scanner.Scan())
					{
						_ftprintf(stdout, _T("Parse %s\n"), sFilenameRC);
						CParser parser(&scanner, &m_TSArray, sFilenameDest, TRUE);

						if (parser.Parse(bIgnoreTODO))
						{
							_ftprintf(stdout, _T("%i zu übersetzende Texte ersetzt\n"), m_TSArray.GetSize());
							_ftprintf(stdout, _T("Texte in Datei %s geschrieben\n"), sFilenameDest);
							// Die Sprachen-Defines muessen noch ersetzt werden
#ifdef _UNICODE
							if (ReplaceLanguageDefines(sFilenameDest, sLng, nSaveTextAs))
#else
							if (ReplaceLanguageDefines(sFilenameDest, sLng))
#endif
							{
								_ftprintf(stdout, _T("Sprachen-Defines in Datei %s ersetzt\n"), sFilenameDest);
							}
							else
							{
								_ftprintf(stdout, _T("%s: Fehler beim Ersetzen der Defines\n"), sFilenameDest);
							}
						}
					}
					else
					{
						if (scanner.GetTokens()->GetSize())
						{
							CToken* pToken = scanner.GetTokens()->GetAt(scanner.GetTokens()->GetSize()-1);
							_ftprintf(stdout, _T("%s(%i): Fehler beim Scannen\n"), sFilenameRC, pToken->GetLine());
							_ftprintf(stdout, _T("%s\n"), pToken->GetOriginal());
						}
						else
						{
							_ftprintf(stdout, _T("%s(0): Fehler beim Scannen\n"), sFilenameRC);
						}
					}
				}
				else if (iRet == -1)
				{
					_ftprintf(stdout, _T("FEHLER: Language Datei ist keine ANSI-Datei\n"));
				}
				else
				{
					_ftprintf(stdout, _T("Language Datei nicht gefunden oder fehlerhaft\n"));
				}
				m_TSArray.DeleteAll();
			}
			else
			{
				_ftprintf(stdout, _T("Resource-Datei bitte auschecken: %s!\n"), sFilenameDest);
			}
		}
		else
		{
			_ftprintf(stdout, _T("Der erste Parameter ist keine RC-Datei!\n"));
			PrintCmdLineOptions();
		}
	}
	
	_ftprintf(stdout, _T("InsertResource DONE\n"));
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
	BOOL ReplaceLanguageDefines(const CString& sFile, CString sLang, int nSaveTextAs)
#else
	BOOL ReplaceLanguageDefines(const CString& sFile, CString sLang)
#endif
{
	BOOL bReturn = TRUE;
	sLang.MakeUpper();

	CFileStatus fileStatus;
	
#ifdef _UNICODE
	CStdioFileU fileSource, fileDest;
	if (nSaveTextAs == 0)
	{
		fileDest.SetFileAnsi();
	}
	else if (nSaveTextAs > 1)
	{
		fileDest.SetFileMCBS((WORD)nSaveTextAs);
	}
	else
	{
		fileDest.SetFileUnicode();
	}
#else
	CStdioFile fileSource, fileDest;
#endif
	
	CString sFileBak = sFile + ".bak";
	if (fileSource.GetStatus(sFileBak, fileStatus))
	{
		fileSource.Remove(sFileBak);
	}
	TRY
	{
		fileSource.Rename(sFile, sFileBak);
		if (fileSource.Open(sFileBak, CFile::modeRead, NULL))
		{
			if (fileDest.Open(sFile, CFile::modeWrite|CFile::modeCreate, NULL))
			{
				CString sLine;
				while (fileSource.ReadString(sLine))
				{
					if (!sLine.IsEmpty())
					{
						if (sLang.CompareNoCase(_T("CHS")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_CHS"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED"));
							sLine.Replace(DEFAULT_CODEPAGE, CHS_CODEPAGE);
							sLine.Replace(_T("l.deu\\"), _T("l.chs\\"));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 4, 2"));
							sLine.Replace(_T("German (Germany)"), _T("Chinese (P.R.C.)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"040504b0\"\n"));
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x405\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("CSY")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_CSY"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_CZECH, SUBLANG_DEFAULT"));
							sLine.Replace(DEFAULT_CODEPAGE, CSY_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 5, 1"));
							sLine.Replace(_T("German (Germany)"), _T("Czech (Czech)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"040504b0\"\n"));
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x405\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("ENU")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_ENU"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_ENGLISH, SUBLANG_ENGLISH_US"));
//							sLine.Replace(DEFAULT_CODEPAGE, DEFAULT_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 9, 1"));
							sLine.Replace(_T("German (Germany)"), _T("English (U.S.)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"040904b0\"\n"));
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x409\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("ESP")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_ESP"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_SPANISH, SUBLANG_SPANISH_MODERN"));
//							sLine.Replace(DEFAULT_CODEPAGE, DEFAULT_CODEPAGE);
							sLine.Replace(_T("l.deu\\"), _T("l.esp\\"));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 10, 3"));
							sLine.Replace(_T("German (Germany)"), _T("Spanish (Modern)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"0c0a04b0\"\n"));
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0xc0a\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("FRA")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_FRA"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_FRENCH, SUBLANG_FRENCH"));
//							sLine.Replace(DEFAULT_CODEPAGE, DEFAULT_CODEPAGE);
							sLine.Replace(_T("l.deu\\"), _T("l.fra\\"));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 12, 1"));
							sLine.Replace(_T("German (Germany)"), _T("French (French)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"040c04b0\"\n"));
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x40c\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("ITA")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_ITA"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_ITALIAN, SUBLANG_ITALIAN"));
//							sLine.Replace(DEFAULT_CODEPAGE, DEFAULT_CODEPAGE);
							sLine.Replace(_T("l.deu\\"), _T("l.ita\\"));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 16, 1"));
							sLine.Replace(_T("German (Germany)"), _T("Italian (Italian)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"041004b0\"\n"));
								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x410\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("NLD")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_NLD"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_DUTCH, SUBLANG_DUTCH"));
//							sLine.Replace(DEFAULT_CODEPAGE, DEFAULT_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 19, 1"));
							sLine.Replace(_T("German (Germany)"), _T("Dutch (Dutch)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"041304b0\"\n"));
								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x413\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("NOR")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_NOR"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL"));
//							sLine.Replace(DEFAULT_CODEPAGE, DEFAULT_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 20, 1"));
							sLine.Replace(_T("German (Germany)"), _T("Norwegian (Norwegian)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"041404b0\"\n"));
								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x414\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("PLK")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_PLK"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_POLISH, SUBLANG_DEFAULT"));
							sLine.Replace(DEFAULT_CODEPAGE, PLK_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 21, 1"));
							sLine.Replace(_T("German (Germany)"), _T("Polish (Polish)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"041504b0\"\n"));
								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x415\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("PTG")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_PTG"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_PORTUGUESE, SUBLANG_DEFAULT"));
//							sLine.Replace(DEFAULT_CODEPAGE, DEFAULT_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 22, 1"));
							sLine.Replace(_T("German (Germany)"), _T("Portuguese (Portuguese)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"081604b0\"\n"));
								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x0816\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("RUS")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_RUS"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_RUSSIAN, SUBLANG_DEFAULT"));
							sLine.Replace(DEFAULT_CODEPAGE, RUS_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 25, 1"));
							sLine.Replace(_T("German (Germany)"), _T("Russian (Russian)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"081904b0\"\n"));
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x0819\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("JPN")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_JPN"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_JAPANESE, SUBLANG_DEFAULT"));
							sLine.Replace(DEFAULT_CODEPAGE, JPN_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 17, 1"));
							sLine.Replace(_T("German (Germany)"), _T("Japanese (Japan)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"081904b0\"\n"));
//								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x0819\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("SVE")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_SVE"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_SWEDISH, SUBLANG_SWEDISH"));
//							sLine.Replace(DEFAULT_CODEPAGE, DEFAULT_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 29, 1"));
							sLine.Replace(_T("German (Germany)"), _T("Swedish (Swedish)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"041d04b0\"\n"));
								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x41d\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else if (sLang.CompareNoCase(_T("WLS")) == 0)
						{
							sLine.Replace(_T("AFX_TARG_DEU"), _T("AFX_TARG_ENG"));
							sLine.Replace(_T("LANG_GERMAN, SUBLANG_GERMAN"), _T("LANG_ENGLISH, SUBLANG_ENGLISH_UK"));
//							sLine.Replace(DEFAULT_CODEPAGE, DEFAULT_CODEPAGE);
							sLine.Replace(_T("l.deu\\\\"), _T(""));
							sLine.Replace(_T("l.deu\\"), _T(""));
							sLine.Replace(_T("LANGUAGE 7, 1"), _T("LANGUAGE 9, 2"));
							sLine.Replace(_T("German (Germany)"), _T("English (UK)"));
/*							if (sLine.Find(_T("non-Microsoft Visual C++ edited resources")) != -1)
							{
								fileDest.WriteString(_T("#define WK_LANG_BLOCK \"080904b0\"\n"));
								fileDest.WriteString(_T("#define WK_LANG_BLOCK_NUM 0x809\n"));
							}
*/							// special OEMGUI.dll workaround, because of additional STR-file!
							sLine.Replace(_T("res\\ApplicationNames.str"), _T("res\\ApplicationNamesEnu.str"));
						}
						else
						{
							_ftprintf(stdout, _T("Fehler: Sprache %s unbekannt\n"), sLang);
							bReturn = FALSE;
						}
					}
					sLine += _T("\n");
					fileDest.WriteString(sLine);
				}
				fileDest.Close();
			}
			else
			{
				bReturn = FALSE;
			}
			fileSource.Close();
		}
		else
		{
			bReturn = FALSE;
		}
		fileSource.Remove(sFileBak);
	}
	CATCH( CFileException, e )
	{
		bReturn = FALSE;
		_ftprintf(stdout, _T("Fehler: Datei %s konnte nicht umbenannt werden\n"), sFile);
	}
	END_CATCH
	return bReturn;
}