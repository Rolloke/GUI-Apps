// ExtractRecource.cpp

#include "stdafx.h"

#include "..\Scanner.h"
#include "..\TranslateString.h"
#include "..\Parser.h"

// Command Line is
// extractresource OriginalResourceFile SprachAbkürzung
int main( int argc, char *argv[] )
{
	if (argc<3)
	{
		cout << "zu wenig Parameter angegeben" << endl << flush;
		cout << "Aufruf: extractresource <OriginalResourceFile> <SprachAbkürzung> [/r]" << endl << flush;
		cout << "Der Schalter /r überschreibt die SprachAbkürzung im OriginalResourceFile" << endl << flush;
	}
	else
	{
		CTSArray m_TSArray;
		CString sSource = argv[1];
		CString sLang = argv[2];
		CString sReplace = argv[3];
		CScanner scanner(sSource);
		CString sDest;
		CFileStatus cfs;
		BOOL bCheckOut = FALSE;
		int p;

		p = sSource.Find(".rc");
		if (p!=-1)
		{
			if (sReplace.CompareNoCase("/r") == 0)
			{
				sDest = sSource.Left(p-sLang.GetLength()) + sLang + ".lng";
			}
			else
			{
				sDest = sSource.Left(p) + sLang + ".lng";
			}

			if (CFile::GetStatus(sDest,cfs))
			{
				// destination file exists
				if (cfs.m_attribute & CFile::readOnly)
				{
					bCheckOut = TRUE;
				}
			}

			if (!bCheckOut)
			{
				if (m_TSArray.Load(sDest))
				{
					cout << "aktualisiere " << sDest << endl << flush;
				}

				cout << "scanne " << sSource << endl << flush;

				if (scanner.Scan())
				{
					cout << "parse" << endl << flush;
					CParser parser(&scanner,&m_TSArray,TRUE);
					if (parser.Parse())
					{
						m_TSArray.Save(sDest);
						cout << m_TSArray.GetSize() << " zu übersetzende Texte gefunden." << endl;
						cout << m_TSArray.GetNumOfWords() << " zu übersetzende Worte gefunden." << endl;
						cout << "Texte in Datei " << sDest << " geschrieben" << endl << flush;
					}
					else
					{
						cout << "Fehler beim Parsen" << endl << flush;
					}
				}
				else
				{
					if (scanner.GetTokens()->GetSize())
					{
						CToken* pToken = scanner.GetTokens()->GetAt(scanner.GetTokens()->GetSize()-1);
						cout << sSource << "(" << pToken->GetLine() << ")" << ": Fehler beim Scannen";
						cout << pToken->GetOriginal() << endl << flush;
					}
					else
					{
						cout << sSource << "(0)" << ": Fehler beim Scannen" << endl << flush;
					}
				}
			}
			else
			{
				cout << "please check out " << sDest << " !" << endl << flush;
			}
		}
		else
		{
			cout << "keine rc Datei als Parameter angegeben" << endl << flush;
			cout << "Aufruf: extractresource <OriginalResourceFile> <SprachAbkürzung> [/r]" << endl << flush;
		}
	}
	
	cout << "ExtractResource DONE" << endl << flush;
	return 0;
}