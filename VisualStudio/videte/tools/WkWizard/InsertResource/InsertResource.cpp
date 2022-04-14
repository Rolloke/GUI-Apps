// InsertResource.cpp

#include "stdafx.h"

#include "..\Scanner.h"
#include "..\TranslateString.h"
#include "..\Parser.h"

BOOL ReplaceLanguageDefines(const CString& sFile, CString sLang);

// Command Line is
// insertresource OriginalResourceFile SprachAbkürzung
int main( int argc, char *argv[] )
{
	if (argc!=3)
	{
		cout << "keine Dateien und Sprache als Parameter angegeben" << endl << flush;
		cout << "Aufruf: insertresource <OriginalResourceFile> <SprachAbkürzung>" << endl << flush;
	}
	else
	{
		CTSArray m_TSArray;
		CString sFilenameRC = argv[1];
		CString sLng = argv[2];

		int p = sFilenameRC.Find(".rc");
		if (p!=-1)
		{
			CString sFilenameLng = sFilenameRC.Left(p) + sLng + ".lng";
			if (sLng.CompareNoCase("enuenu") == 0)
			{
				sLng = "Enu";
			}
			CString sFilenameDest = sFilenameRC.Left(p) + sLng + ".rc";
			CScanner scanner(sFilenameRC, TRUE);
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
				cout << "lese " << sFilenameLng << endl << flush;
				if (m_TSArray.Load(sFilenameLng))
				{
					cout << m_TSArray.GetSize() << " zu übersetzende Texte gefunden." << endl << flush;
					cout << "scanne " << sFilenameRC << endl << flush;

					if (scanner.Scan())
					{
						cout << "parse " << sFilenameRC << endl << flush;
						CParser parser(&scanner, &m_TSArray, sFilenameDest, sLng, TRUE);

						if (parser.Parse())
						{
							cout << m_TSArray.GetSize() << " zu übersetzende Texte ersetzt." << endl << flush;
							cout << "Texte in Datei " << sFilenameDest <<" geschrieben" << endl << flush;
							// Die Sprachen-Defines muessen noch ersetzt werden
							if (ReplaceLanguageDefines(sFilenameDest, sLng))
							{
								cout << "Sprachen-Defines in Datei " << sFilenameDest <<" ersetzt" << endl << flush;
							}
							else
							{
								cout << sFilenameDest << ": Fehler beim Ersetzen der Defines" << endl << flush;
							}
						}
					}
					else
					{
						if (scanner.GetTokens()->GetSize())
						{
							CToken* pToken = scanner.GetTokens()->GetAt(scanner.GetTokens()->GetSize()-1);
							cout << sFilenameRC << "(" << pToken->GetLine() << ")" << ": Fehler beim Scannen";
							cout << pToken->GetOriginal() << endl << flush;
						}
						else
						{
							cout << sFilenameRC << "(0)" << ": Fehler beim Scannen" << endl << flush;
						}
					}
				}
				else
				{
					cout << "language Datei nicht gefunden oder fehlerhaft" << endl << flush;
				}
				m_TSArray.DeleteAll();
			}
			else
			{
				cout << "please check out " << sFilenameDest << " !" << endl << flush;
			}
		}
		else
		{
			cout << "Der erste Parameter ist keine rc Datei!" << endl << flush;
			cout << "Aufruf: insertresource <OriginalResourceFile> <SprachAbkürzung>" << endl << flush;
		}
	}
	
	cout << "InsertResource DONE" << endl << flush;
	return 0;
}

BOOL ReplaceLanguageDefines(const CString& sFile, CString sLang)
{
	BOOL bReturn = TRUE;
	sLang.MakeUpper();

	CFileStatus fileStatus;
	CStdioFile fileSource, fileDest;
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
						if (sLang.CompareNoCase("CSY") == 0)
						{
							sLine.Replace("AFX_TARG_DEU", "AFX_TARG_CSY");
							sLine.Replace("LANG_GERMAN, SUBLANG_GERMAN", "LANG_CZECH, SUBLANG_DEFAULT");
							sLine.Replace("code_page(1252)", "code_page(1252)");
							sLine.Replace("l.deu\\", "");
							sLine.Replace("LANGUAGE 7, 1", "LANGUAGE 5, 1");
							sLine.Replace("German (Germany)", "Czech (Czech)");
							if (sLine.Find("#include \"res\\") != -1)
							{
								fileDest.WriteString("#define WK_LANG_BLOCK \"040504b0\"\n");
								fileDest.WriteString("#define WK_LANG_BLOCK_NUM 0x405\n" );
							}
						}
						else if (sLang.CompareNoCase("ENU") == 0)
						{
							sLine.Replace("AFX_TARG_DEU", "AFX_TARG_ENU");
							sLine.Replace("LANG_GERMAN, SUBLANG_GERMAN", "LANG_ENGLISH, SUBLANG_ENGLISH_US");
							sLine.Replace("code_page(1252)", "code_page(1252)");
							sLine.Replace("l.deu\\", "");
							sLine.Replace("LANGUAGE 7, 1", "LANGUAGE 9, 1");
							sLine.Replace("German (Germany)", "English (U.S.)");
							if (sLine.Find("#include \"res\\") != -1)
							{
								fileDest.WriteString("#define WK_LANG_BLOCK \"040904b0\"\n");
								fileDest.WriteString("#define WK_LANG_BLOCK_NUM 0x409\n" );
							}
						}
						else if (sLang.CompareNoCase("ESP") == 0)
						{
							sLine.Replace("AFX_TARG_DEU", "AFX_TARG_ESP");
							sLine.Replace("LANG_GERMAN, SUBLANG_GERMAN", "LANG_SPANISH, SUBLANG_SPANISH_MODERN");
							sLine.Replace("code_page(1252)", "code_page(1252)");
							sLine.Replace("l.deu\\", "l.esp\\");
							sLine.Replace("LANGUAGE 7, 1", "LANGUAGE 10, 3");
							sLine.Replace("German (Germany)", "Spanish (Modern)");
							if (sLine.Find("#include \"res\\") != -1)
							{
								fileDest.WriteString("#define WK_LANG_BLOCK \"0c0a04b0\"\n");
								fileDest.WriteString("#define WK_LANG_BLOCK_NUM 0xc0a\n" );
							}
						}
						else if (sLang.CompareNoCase("FRA") == 0)
						{
							sLine.Replace("AFX_TARG_DEU", "AFX_TARG_FRA");
							sLine.Replace("LANG_GERMAN, SUBLANG_GERMAN", "LANG_FRENCH, SUBLANG_FRENCH");
							sLine.Replace("code_page(1252)", "code_page(1252)");
							sLine.Replace("l.deu\\", "l.fra\\");
							sLine.Replace("LANGUAGE 7, 1", "LANGUAGE 12, 1");
							sLine.Replace("German (Germany)", "French (French)");
							if (sLine.Find("#include \"res\\") != -1)
							{
								fileDest.WriteString("#define WK_LANG_BLOCK \"040c04b0\"\n");
								fileDest.WriteString("#define WK_LANG_BLOCK_NUM 0x40c\n" );
							}
						}
						else if (sLang.CompareNoCase("ITA") == 0)
						{
							sLine.Replace("AFX_TARG_DEU", "AFX_TARG_ITA");
							sLine.Replace("LANG_GERMAN, SUBLANG_GERMAN", "LANG_ITALIAN, SUBLANG_ITALIAN");
							sLine.Replace("code_page(1252)", "code_page(1252)");
							sLine.Replace("l.deu\\", "l.ita\\");
							sLine.Replace("LANGUAGE 7, 1", "LANGUAGE 16, 1");
							sLine.Replace("German (Germany)", "Italian (Italian)");
							if (sLine.Find("#include \"res\\") != -1)
							{
								fileDest.WriteString("#define WK_LANG_BLOCK \"041004b0\"\n");
								fileDest.WriteString("#define WK_LANG_BLOCK_NUM 0x410\n" );
							}
						}
						else if (sLang.CompareNoCase("NLD") == 0)
						{
							sLine.Replace("AFX_TARG_DEU", "AFX_TARG_NLD");
							sLine.Replace("LANG_GERMAN, SUBLANG_GERMAN", "LANG_DUTCH, SUBLANG_DUTCH");
							sLine.Replace("code_page(1252)", "code_page(1252)");
							sLine.Replace("l.deu\\", "");
							sLine.Replace("LANGUAGE 7, 1", "LANGUAGE 19, 1");
							sLine.Replace("German (Germany)", "Dutch (Dutch)");
							if (sLine.Find("#include \"res\\") != -1)
							{
								fileDest.WriteString("#define WK_LANG_BLOCK \"041304b0\"\n");
								fileDest.WriteString("#define WK_LANG_BLOCK_NUM 0x413\n" );
							}
						}
						else if (sLang.CompareNoCase("PLK") == 0)
						{
							sLine.Replace("AFX_TARG_DEU", "AFX_TARG_PLK");
							sLine.Replace("LANG_GERMAN, SUBLANG_GERMAN", "LANG_POLISH, SUBLANG_DEFAULT");
							sLine.Replace("code_page(1252)", "code_page(1252)");
							sLine.Replace("l.deu\\", "");
							sLine.Replace("LANGUAGE 7, 1", "LANGUAGE 21, 1");
							sLine.Replace("German (Germany)", "Polish (Polish)");
							if (sLine.Find("#include \"res\\") != -1)
							{
								fileDest.WriteString("#define WK_LANG_BLOCK \"041504b0\"\n");
								fileDest.WriteString("#define WK_LANG_BLOCK_NUM 0x415\n" );
							}
						}
						else if (sLang.CompareNoCase("SVE") == 0)
						{
							sLine.Replace("AFX_TARG_DEU", "AFX_TARG_SVE");
							sLine.Replace("LANG_GERMAN, SUBLANG_GERMAN", "LANG_SWEDISH, SUBLANG_SWEDISH");
							sLine.Replace("code_page(1252)", "code_page(1252)");
							sLine.Replace("l.deu\\", "");
							sLine.Replace("LANGUAGE 7, 1", "LANGUAGE 29, 1");
							sLine.Replace("German (Germany)", "Swedish (Swedish)");
							if (sLine.Find("#include \"res\\") != -1)
							{
								fileDest.WriteString("#define WK_LANG_BLOCK \"041d04b0\"\n");
								fileDest.WriteString("#define WK_LANG_BLOCK_NUM 0x41d\n" );
							}
						}
						else
						{
							cout << "Fehler: Sprache " << sLang << " unbekannt" << endl << flush;
							bReturn = FALSE;
						}
					}
					sLine += "\n";
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
		cout << "Fehler: Datei " << sFile << " konnte nicht umbenannt werden" << endl << flush;
	}
	END_CATCH
	return bReturn;
}