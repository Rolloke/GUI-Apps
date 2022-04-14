// WkTranslatorDoc.cpp : implementation of the CWkTranslatorDoc class
//

#include "stdafx.h"
#include "WkTranslator.h"

#include "WkTranslatorDoc.h"

#include "TranslateDialog.h"

#include <WKClasses/WK_Ascii.h>

#include <process.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWkTranslatorApp theApp;

#define IMPORT_CLOSSARY 1
#define IMPORT_CLOSSARY_GER 2
#define ADDGLOSSARYFROMTXTFILE 3

/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorDoc
IMPLEMENT_DYNCREATE(CWkTranslatorDoc, CDocument)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWkTranslatorDoc, CDocument)
	//{{AFX_MSG_MAP(CWkTranslatorDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_GLOSSARY_IMPORT, OnGlossaryImport)
	ON_COMMAND(ID_GLOSSARY_IMPORT_DEU, OnGlossaryImportDeu)
	ON_COMMAND(ID_GLOSSARY_EXPORT, OnGlossaryExport)
	ON_COMMAND(ID_GLOSSARY_EXPORT_DEU, OnGlossaryExportDeu)
	ON_COMMAND(ID_FILE_SAVE_AS_GLOSSARY, OnFileSaveAsGlossary)
	ON_COMMAND(ID_FILE_SAVE_TEXT_ONLY, OnFileSaveTextOnly)
	ON_COMMAND(ID_ENABLE_EDIT, OnEnableEdit)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_EDIT, OnUpdateEnableEdit)
	ON_COMMAND(ID_ENABLE_EDIT_ACC, OnEnableEditAcc)
	ON_COMMAND(ID_FILE_SAVE_AS_GLOSSARY_DEU, OnFileSaveAsGlossaryDeu)
	ON_COMMAND(ID_FILE_SAVE_ORIGINAL_LEN_COMMENT, OnFileSaveOriginalLengthComment)
	ON_COMMAND(ID_GLOSSARY_IMPORT_TXT_FILE, OnGlossaryImportTxtFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorDoc construction/destruction
CWkTranslatorDoc::CWkTranslatorDoc()
{
	// TODO: add one-time construction code here
	m_bEnableEdit = FALSE;
	m_hThread	= NULL;
	m_nWhat     = 0;
	m_nThreadID = 0;
}
/////////////////////////////////////////////////////////////////////////////
CWkTranslatorDoc::~CWkTranslatorDoc()
{
}
/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorDoc serialization
void CWkTranslatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorDoc diagnostics
#ifdef _DEBUG
void CWkTranslatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorDoc commands
BOOL CWkTranslatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_TSArrayArray.DeleteAll();
	m_mapAutomatic.RemoveAll();
	UpdateAllViews(NULL, UPDATE_NEW_EMPTY_DOC, NULL);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWkTranslatorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
//	Yet only a single LNG file can be opened
//	if (m_TSArrayArray.Load(lpszPathName, FALSE))
	CTSArray* pTSArray = new CTSArray();
	int iRet = pTSArray->Load(lpszPathName);
	if (iRet == 1)
	{
		m_TSArrayArray.Add(pTSArray);
//		if (m_mapAutomatic.GetCount())
		{
			if (m_TSArrayArray.AutomaticTranslate(m_mapAutomatic))
			{
				SetModifiedFlag(TRUE);
			}
		}
		UpdateAllViews(NULL, UPDATE_OPENED_DOC, (CObject*)&m_TSArrayArray);
		return TRUE;
	}
	else
	{
		delete pTSArray;
		CString sMsg;
		if (iRet == 0)
		{
			sMsg.Format(_T("FEHLER:\n%s\nnicht gefunden\n"), lpszPathName);
		}
		else if (iRet == -1)
		{
			sMsg.Format(_T("FEHLER:\n%s\nist keine ANSI-Datei\n"), lpszPathName);
		}
		else
		{
			sMsg.Format(_T("FEHLER:\n%s\nnicht geöffnet\n"), lpszPathName);
		}
		AfxMessageBox(sMsg, MB_OK|MB_ICONSTOP);
		m_TSArrayArray.DeleteAll();
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWkTranslatorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// first check data for safety purposes
	if (!m_TSArrayArray.CheckData())
	{
		if (AfxMessageBox(IDP_CHECK_TRANSLATION, MB_YESNO|MB_ICONSTOP) == IDYES)
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_CHECK_TRANSLATION_ALL, NULL);
			return FALSE;
		}
	}

	CString sDirName = lpszPathName;
	int p;

	p = sDirName.ReverseFind('\\');
	if (p==-1)
	{
		return FALSE;
	}
	sDirName = sDirName.Left(p);
	if (m_TSArrayArray.Save(sDirName))
	{
		SetModifiedFlag(FALSE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::EditTranslateString(int i, int j, const CString& sNew)
{
	CTranslateString* pTS;
	CTSArray* pTSArray;

	if ((i>=0) && (i<m_TSArrayArray.GetSize()))
	{
		pTSArray = m_TSArrayArray.GetAt(i);
		if ((j>=0) && (j<pTSArray->GetSize()))
		{
			pTS = pTSArray->GetAt(j);
			if (pTS)
			{
				BOOL bUpdateViews = FALSE;
				if (sNew != pTS->GetTranslate())
				{
					SetModifiedFlag();
					pTS->SetTranslate(sNew);
					m_mapAutomatic.SetAt(pTS->GetText(), sNew);
					bUpdateViews = TRUE;

					CheckAndReplaceMultipleOccurences(pTS, sNew, i, j);
				}
				if (bUpdateViews)
				{
					UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)i,(WORD)j), (CObject*)pTS);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWkTranslatorDoc::EditTranslateString(int i, int j, BOOL bCheckData)
{
	CTranslateString* pTS;
	CTSArray* pTSArray;

	if ((i>=0) && (i<m_TSArrayArray.GetSize()))
	{
		pTSArray = m_TSArrayArray.GetAt(i);
		if ((j>=0) && (j<pTSArray->GetSize()))
		{
			pTS = pTSArray->GetAt(j);
			if (pTS)
			{
				CString sSuggestion = pTS->GetSuggestion();
/*
				if (m_mapAutomatic.GetCount())
				{
					if (m_mapAutomatic.Lookup(pTS->GetText(), sSuggestion) == FALSE)
					{
						CString sText = pTS->GetText();
						int iIndex = sText.Find('&');
						if (iIndex != -1)
						{
							sText = sText.Left(iIndex)+sText.Mid(iIndex+1);
							if (m_mapAutomatic.Lookup(sText, sSuggestion) == FALSE)
							{
								if (sText.Right(3) == _T("..."))
								{
									sText = sText.Left(sText.GetLength()-3);
									m_mapAutomatic.Lookup(sText, sSuggestion);
								}
							}
						}
					}
				}
*/
				CTranslateDialog dlg(pTS->GetText(),
									 sSuggestion,
									 pTS->GetTranslate(),
									 pTS->GetMaxLength(),
									 pTS->GetComment(),
									 m_bEnableEdit,
									 bCheckData,
									 AfxGetMainWnd()
									 );

				dlg.m_sResourceTypeName.LoadString((int)pTS->GetRSType() + IDS_RST_DEFAULT);
				dlg.m_sResourceTypeName += _T(" / ") + pTS->GetFullID();
				// TODO! RKE: - Menüitems Popupmenü zuordnen
				//			  - Dialogtext Dialogen zuordnen

				if (IDOK==dlg.DoModal())
				{
					BOOL bUpdateViews = FALSE;
					CString sTranslation = dlg.GetTranslation();
					if (sTranslation != pTS->GetTranslate())
					{
						SetModifiedFlag();
						pTS->SetTranslate(sTranslation);
						m_mapAutomatic.SetAt(dlg.GetOriginal(), sTranslation);
						bUpdateViews = TRUE;

						CheckAndReplaceMultipleOccurences(pTS, sTranslation, i, j);
					}
					UINT	uMaxLength = dlg.GetMaxLength();
					if (uMaxLength != pTS->GetMaxLength())
					{
						SetModifiedFlag();
						pTS->SetMaxLength(uMaxLength);
						bUpdateViews = TRUE;
					}
					CString sComment = dlg.GetComment();
					if (sComment != pTS->GetComment())
					{
						SetModifiedFlag();
						pTS->SetComment(sComment);
						bUpdateViews = TRUE;
					}
					if (bUpdateViews)
					{
						UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)i,(WORD)j),(CObject*)pTS);
					}
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWkTranslatorDoc::RequestCARMO(BOOL &bRequested)
{
	if (!bRequested)
	{
		bRequested = TRUE;
		if (AfxMessageBox(IDS_CHECK_AND_REPLACE_MULTIPLE_OCCURRENCES, MB_YESNO|MB_ICONQUESTION)==IDYES)
		{
			return TRUE;
		}
		else
		{
			bRequested = -1;
			return FALSE;
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::CheckAndReplaceMultipleOccurences(CTranslateString* pTS, CString sTranslation, int i, int j)
{
	CTSArray* pTSArray;
	CTranslateString* pTS2;
	CString sText1, sText2;
	BOOL bRequested = FALSE;
	for (int k=0;k<m_TSArrayArray.GetSize();k++)
	{
		pTSArray = m_TSArrayArray.GetAt(k);
		for (int l=0;l<pTSArray->GetSize();l++)
		{
			if ((j!=l) || (i!=k))
			{
				pTS2 = pTSArray->GetAt(l);
				sText1 = pTS->GetText();
				sText2 = pTS2->GetText();
				if (sText2 == sText1 
					&& RequestCARMO(bRequested) )
				{
//					WK_TRACE(_T("found same text at %d,%d\n"),k,l);
					pTS2->SetTranslate(sTranslation);
					UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)k,(WORD)l),(CObject*)pTS2);
				}
				// First only has "..." behind
				else if (   (sText1.Right(3) == _T("..."))
					     && (sText1.Left(sText1.GetLength()-3) == sText2)
						 && RequestCARMO(bRequested) )
				{
//					WK_TRACE(_T("found same text without '...' at %d,%d\n"),k,l);
					pTS2->SetTranslate(sTranslation.Left(sTranslation.GetLength()-3));
					UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)k,(WORD)l),(CObject*)pTS2);
				}
				// Second only has "..." behind
				else if (   (sText2.Right(3) == _T("..."))
					     && (sText2.Left(sText2.GetLength()-3) == sText1)
						 && RequestCARMO(bRequested) )
				{
//					WK_TRACE(_T("found same text with '...' at %d,%d\n"),k,l);
					pTS2->SetTranslate(sTranslation + _T("..."));
					UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)k,(WORD)l),(CObject*)pTS2);
				}
				// First only has ":" behind
				else if (   (sText1.Right(1) == _T(":"))
					     && (sText1.Left(sText1.GetLength()-1) == sText2)
						 && RequestCARMO(bRequested) )
				{
//					WK_TRACE(_T("found same text without ':' at %d,%d\n"),k,l);
					pTS2->SetTranslate(sTranslation.Left(sTranslation.GetLength()-1));
					UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)k,(WORD)l),(CObject*)pTS2);
				}
				// Second only has ":" behind
				else if (   (sText2.Right(1) == _T(":"))
					     && (sText2.Left(sText2.GetLength()-1) == sText1)
						 && RequestCARMO(bRequested) )
				{
//					WK_TRACE(_T("found same text with ':' at %d,%d\n"),k,l);
					pTS2->SetTranslate(sTranslation + _T(":"));
					UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)k,(WORD)l),(CObject*)pTS2);
				}
				// First only has ">" in front
				else if (   (sText1.Left(1) == _T(">"))
					     && (sText1.Mid(1) == sText2)
						 && RequestCARMO(bRequested) )
				{
//					WK_TRACE(_T("found same text without '>' at %d,%d\n"),k,l);
					pTS2->SetTranslate(sTranslation.Mid(1));
					UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)k,(WORD)l),(CObject*)pTS2);
				}
				// Second only has ">" in front
				else if (   (sText2.Left(1) == _T(">"))
					     && (sText2.Mid(1) == sText1)
						 && RequestCARMO(bRequested) )
				{
//					WK_TRACE(_T("found same text with '>' at %d,%d\n"),k,l);
					pTS2->SetTranslate(_T(">") + sTranslation);
					UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)k,(WORD)l),(CObject*)pTS2);
				}
				// First only has "&" in front
				else if (   (sText1.Left(1) == _T("&"))
					     && (sText1.Mid(1) == sText2)
						 && RequestCARMO(bRequested) )
				{
//					WK_TRACE(_T("found same text without '&' at %d,%d\n"),k,l);
					int iIndex = sTranslation.Find('&');
					if (iIndex != -1)
					{
						pTS2->SetTranslate(sTranslation.Left(iIndex) + sTranslation.Mid(iIndex+1));
						UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)k,(WORD)l),(CObject*)pTS2);
					}
				}
				// Second only has "&" in front
				else if (   (sText2.Left(1) == _T("&"))
					     && (sText2.Mid(1) == sText1)
						 && RequestCARMO(bRequested) )
				{
//					WK_TRACE(_T("found same text with '&' at %d,%d\n"),k,l);
					pTS2->SetTranslate(_T("&") + sTranslation);
					UpdateAllViews(NULL, UPDATE_TRANSLATION+MAKELONG((WORD)k,(WORD)l),(CObject*)pTS2);
				}
				if (bRequested == -1)
				{
					return ;
				}
			}
//			Sleep(0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsModified());
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnGlossaryImport() 
{
	theApp.OnGlossarySelectCodepage();
	if (m_hThread == NULL)
	{
		m_nWhat = IMPORT_CLOSSARY;
		m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &m_nThreadID);
		m_dlgInfo.DoModal();
	}
//	ImportGlossary(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnGlossaryImportDeu() 
{
	theApp.OnGlossarySelectCodepage();
	if (m_hThread == NULL)
	{
		m_nWhat = IMPORT_CLOSSARY_GER;
		m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &m_nThreadID);
		m_dlgInfo.DoModal();
	}
//	ImportGlossary(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::ImportGlossary(BOOL bGerman)
{
	CFileDialog dlgFile(TRUE,_T("*.csv"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV Files(*.csv)|*.csv"),AfxGetMainWnd());

	CString sFileNames;
	const int iSize = 10000;
	_TCHAR chBuffer[iSize] ={0};

	dlgFile.m_ofn.Flags		|= OFN_ALLOWMULTISELECT;
	dlgFile.m_ofn.lpstrFile	= chBuffer;
	dlgFile.m_ofn.nMaxFile	= iSize;

	int iRet = dlgFile.DoModal();
	if (IDOK == iRet)
	{
		// count files first
		int iCount = 0;
		_TCHAR ch = 0;
		for (int i=0 ; i<iSize; i++)
		{
			ch = chBuffer[i];
			if (ch == 0)
			{
				iCount++;
				if (chBuffer[i+1] == 0)
				{
					break;
				}
			}
		}
		if (iCount > 0)
		{
			if (iCount > 1)
			{
				// decrease for DIR
				iCount--;
			}
			TRACE(_T("Add %i glossaries\n"), iCount);

			POSITION pos = dlgFile.GetStartPosition();

			m_dlgInfo.SetGlossaryCount(iCount);
			while (pos != NULL)
			{
				TRACE(_T("Glossaries still to add %i\n"), iCount--);
				CString strPath = dlgFile.GetNextPathName(pos);
				int nFind = strPath.ReverseFind('\\');
				if (nFind == -1) nFind = NULL;
				m_dlgInfo.SetFileName(strPath.Mid(nFind));
				AddGlossary(strPath, bGerman);
			}

			BOOL bModified = 0;
			if (m_mapAutomatic.GetCount())
			{
				CTranslateString::gm_hProgress = m_dlgInfo.m_cTranslate.m_hWnd;
				CTranslateString::gm_bReplace  = m_dlgInfo.m_bReplace;
				bModified = m_TSArrayArray.AutomaticTranslate(m_mapAutomatic);
				CTranslateString::gm_bReplace  = TRUE;
				CTranslateString::gm_hProgress = NULL;
			}
			if (AfxGetApp()->m_pMainWnd)
			{
				AfxGetApp()->m_pMainWnd->PostMessage(WM_USER, UPDATE_OPENED_DOC, bModified);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::AddGlossary(const CString& sPathname, BOOL bGerman)
{
#ifdef _UNICODE
	CStdioFileU file;
#else
	CStdioFile file;
#endif
	CString sLine;

	if (file.Open(sPathname,CFile::modeRead|CFile::shareDenyNone))
	{
#ifdef _UNICODE
		WORD wTemp;
		file.Read(&wTemp, sizeof(WORD));
		if (wTemp == TEXTIO_UNICODE)
		{
			file.SetFileUnicode();
			// real file starts after unicode prefix!
			file.Seek(2, FILE_BEGIN);
		}
		else
		{
			file.SetFileMCBS(theApp.m_wCodePage);
			file.Seek(0, FILE_BEGIN);
		}
#endif
		int iCount = 0;
		CString sSeperator = ',';
		while (file.ReadString(sLine))
		{
			if (sLine.Find(_T("This glossary is intellectual property of Microsoft Corporation.")) != -1)
			{
				TRACE(_T("DUMMY\n"));
				// ignore it
			}
			else
			{
				if (iCount == 0)
				{
					// Check for effective seperator
					int iComma = 0;
					int iSemicolon = 0;
					int iIndex = 0;
					while(   (iIndex < sLine.GetLength())
						  && (iIndex != -1)
						  )

					{
						iIndex = sLine.Find(',', iIndex);
						if (iIndex != -1)
						{
							iComma++;
							iIndex++;
						}
					}
					iIndex = 0;
					while(   (iIndex < sLine.GetLength())
						  && (iIndex != -1)
						  )

					{
						iIndex = sLine.Find(';', iIndex);
						if (iIndex != -1)
						{
							iSemicolon++;
							iIndex++;
						}
					}
					if (iSemicolon > iComma)
					{
						sSeperator = ';';
					}
				}
				iCount++;
				m_dlgInfo.SetTextCount(iCount);
				if (iCount == 19722)
				{
					iCount = iCount;
				}
				
				AddAutomaticString(sLine, bGerman, sSeperator);
				while (m_dlgInfo.m_bStop)
				{
					if (m_dlgInfo.m_bStep)
					{
						m_dlgInfo.m_bStep = FALSE;
						break;
					}
					Sleep(10);
				}

				if (m_dlgInfo.m_bCancel)
				{
					break;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::AddGlossaryFromTxtFile(const CString& sPathname)
{
#ifdef _UNICODE
	CStdioFileU file;
#else
	CStdioFile file;
#endif
	CString sLine;

	if (file.Open(sPathname,CFile::modeRead|CFile::shareDenyNone))
	{
#ifdef _UNICODE
		WORD wTemp;
		file.Read(&wTemp, sizeof(WORD));
		if (wTemp == TEXTIO_UNICODE)
		{
			file.SetFileUnicode();
		}
		else
		{
			file.SetFileMCBS(theApp.m_wCodePage);
		}
		file.Seek(0, FILE_BEGIN);
#endif
		int iCount = 0;
		BOOL bFoundOriginal = FALSE;
		BOOL bFoundTranslation = FALSE;
		CString sOriginal, sTranslation;
		int iOriginalSize = _tcsclen(szOriginal);
		int iTranslationSize = _tcsclen(szTranslation);
		int iIndex = -1;
		while (file.ReadString(sLine))
		{
			if (bFoundOriginal == FALSE)
			{
				if (sLine.Left(iOriginalSize) == szOriginal)
				{
					bFoundOriginal = TRUE;
					sOriginal = sLine.Mid(iOriginalSize);
					iIndex = sOriginal.Find(':');
					if (iIndex != -1)
					{
						sOriginal = sOriginal.Mid(iIndex+1);
					}
					sOriginal.TrimLeft();
					sOriginal.TrimRight();
				}
			}
			else
			{
				if (sLine.Left(iTranslationSize) == szTranslation)
				{
					bFoundTranslation = TRUE;
					sTranslation = sLine.Mid(iTranslationSize);
					iIndex = sTranslation.Find(':');
					if (iIndex != -1)
					{
						sTranslation = sTranslation.Mid(iIndex+1);
					}
					sTranslation.TrimLeft();
					sTranslation.TrimRight();
				}
			}

			if (bFoundTranslation && bFoundTranslation)
			{
				bFoundOriginal = FALSE;
				bFoundTranslation = FALSE;
				iCount++;
				m_dlgInfo.SetTextCount(iCount);
				if (!sOriginal.IsEmpty() && !sTranslation.IsEmpty())
				{
					m_mapAutomatic.SetAt(sOriginal,sTranslation);
				}
				else if (sOriginal.IsEmpty() && sTranslation.IsEmpty())
				{
					// Well, not registered as error
				}
				else
				{
					// May be error, but may be other text
//					WK_TRACE_ERROR(_T("Add glossary string\n   Original: %s\n   Translate: %s\n"),
//									sOriginal, sTranslation);
				}
			}
			
			while (m_dlgInfo.m_bStop)
			{
				if (m_dlgInfo.m_bStep)
				{
					m_dlgInfo.m_bStep = FALSE;
					break;
				}
				Sleep(10);
			}
			if (m_dlgInfo.m_bCancel)
			{
				break;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::AddAutomaticString(const CString& sLine, BOOL bGerman, const CString& sSeperator)
{
	CString sFirstEntry;
	CString sSecondEntry;
	int p1,p2,p3;

	if (sLine.IsEmpty())
	{
		return;
	}

	if ('\"'==sLine.GetAt(0))
	{
		// TODO
		BOOL bLoop = TRUE;
		int iQuote = 1;
		p1 = 1;
		while (bLoop)
		{
			int i = p1;
			p1 = sLine.Find(_T("\"") + sSeperator, p1);
			if (p1!=-1)
			{
				while (i <= p1)
				{
					i = sLine.Find('"', i);
					if (i!=-1)
					{
						iQuote++;
					}
					else
					{
						i = p1;
					}
					i++;
				}
				if (iQuote%2 == 0)
				{
					sFirstEntry = sLine.Mid(1,p1-1);
					bLoop = FALSE;
				}
			}
			else
			{
				bLoop = FALSE;
			}
			p1++;
		}
	}
	else
	{
		p1 = sLine.Find(sSeperator);
		if (p1!=-1)
		{
			sFirstEntry = sLine.Left(p1);
		}
	}
	
	if (!sFirstEntry.IsEmpty())
	{
		p2 = sLine.Find(sSeperator, p1+1);
		if (p2!=-1)
		{
			if ('\"'==sLine.GetAt(p2+1))
			{
				// TODO
				BOOL bLoop = TRUE;
				int iQuote = 1;
				p3 = p2+2;
				while (bLoop)
				{
					int i = p3;
					p3 = sLine.Find(_T("\"") + sSeperator, p3);
					if (p3!=-1)
					{
						while (i <= p3)
						{
							i = sLine.Find('"', i);
							if (i!=-1)
							{
								iQuote++;
							}
							else
							{
								i = p3;
							}
							i++;
						}
						if (iQuote%2 == 0)
						{
							sSecondEntry = sLine.Mid(p2+2,p3-p2-2);
							bLoop = FALSE;
						}
					}
					else
					{
						bLoop = FALSE;
					}
					p3++;
				}
			}
			else
			{
				p3 = sLine.Find(sSeperator,p2+1);
				if (p3!=-1)
				{
					sSecondEntry = sLine.Mid(p2+1,p3-p2-1);
				}
				else
				{
					// no more seperators, ist the whole rest
					sSecondEntry = sLine.Mid(p2+1);
				}
			}
		}
	}

	CString sOriginal, sTranslation;
	if(bGerman)
	{
		sOriginal = sSecondEntry;
		sTranslation = sFirstEntry;
	}
	else
	{
		sOriginal = sFirstEntry;
		sTranslation = sSecondEntry;
	}

	if (!sOriginal.IsEmpty() && !sTranslation.IsEmpty())
	{
		m_dlgInfo.m_cOriginalText.SetWindowText(sOriginal);
		m_dlgInfo.m_cTranslatedText.SetWindowText(sTranslation);
		m_mapAutomatic.SetAt(sOriginal,sTranslation);
		// Trim whitespaces
		CString sOrig = sOriginal;
		sOriginal.TrimLeft(' ');
		sOriginal.TrimRight(' ');
		if (sOrig != sOriginal)
		{
			sTranslation.TrimLeft(' ');
			sTranslation.TrimRight(' ');
			m_dlgInfo.m_cOriginalText.SetWindowText(sOriginal);
			m_dlgInfo.m_cTranslatedText.SetWindowText(sTranslation);
			m_mapAutomatic.SetAt(sOriginal,sTranslation);
		}

	}
	else if (sOriginal.IsEmpty() && sTranslation.IsEmpty())
	{
		// Well, not registered as error
	}
	else
	{
		// May be error, but may be other text
//		WK_TRACE_ERROR(_T("Add glossary string\n   Original: %s\n   Translate: %s\n"),
//						sOriginal, sTranslation);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnGlossaryExportDeu() 
{
	theApp.OnGlossarySelectCodepage();
	ExportGlossary(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnGlossaryExport() 
{
	theApp.OnGlossarySelectCodepage();
	ExportGlossary(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::ExportGlossary(BOOL bGerman) 
{
	CFileDialog dlg(FALSE, _T("*.csv"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV Files(*.csv)|*.csv"), AfxGetMainWnd());

	if (IDOK==dlg.DoModal())
	{
#ifdef _UNICODE
		CStdioFileU file;
		if (theApp.m_wCodePage == CP_UNICODE)
		{
			file.SetFileUnicode();
		}
		else
		{
			file.SetFileMCBS(theApp.m_wCodePage);
		}
#else
		CStdioFile file;
#endif
		CString sLine;
		CString rKey,rValue;
		POSITION pos;

		pos = m_mapAutomatic.GetStartPosition();

		if (file.Open(dlg.GetPathName(), CFile::modeCreate|CFile::modeWrite))
		{
			for( pos = m_mapAutomatic.GetStartPosition(); pos != NULL; )   
			{
				m_mapAutomatic.GetNextAssoc(pos,rKey,rValue);
				if (pos)
				{
					if (bGerman)
					{
						sLine = _T("\"") + rValue + _T("\",,\"") + rKey + _T("\",\n");
					}
					else
					{
						sLine = _T("\"") + rKey+ _T("\",,\"") + rValue  + _T("\",\n");
					}
					file.WriteString(sLine);
				}
			}
			CString sResult;
			sResult.Format(_T("%d automatic Strings exported"),m_mapAutomatic.GetCount());
			AfxMessageBox(sResult);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnFileSaveAsGlossary() 
{
	FileSaveAsGlossary(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnFileSaveAsGlossaryDeu() 
{
	FileSaveAsGlossary(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::FileSaveAsGlossary(BOOL bGerman) 
{
	CFileDialog	dlg(FALSE, _T("*.csv"),NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("Glossary (*.csv)|*.csv"), AfxGetMainWnd());

	theApp.OnGlossarySelectCodepage();
	if (IDOK==dlg.DoModal())
	{
		CString sPathname = dlg.GetPathName();
		m_TSArrayArray.ExportAsGlossary(sPathname, bGerman);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnFileSaveTextOnly() 
{
	CFileDialog	dlg(FALSE, _T("*.txt"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("Text (*.txt)|*.txt"), AfxGetMainWnd());

	theApp.OnGlossarySelectCodepage();
	if (IDOK==dlg.DoModal())
	{
		CString sPathname = dlg.GetPathName();
		m_TSArrayArray.ExportAsTextOnly(sPathname);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnFileSaveOriginalLengthComment() 
{
	CFileDialog	dlg(FALSE, _T("*.txt"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("Text (*.txt)|*.txt"), AfxGetMainWnd());

	theApp.OnGlossarySelectCodepage();
	if (IDOK==dlg.DoModal())
	{
		CString sPathname = dlg.GetPathName();
		m_TSArrayArray.ExportAsOriginalLengthComment(sPathname);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnGlossaryImportTxtFile() 
{
	CFileDialog	dlg(TRUE, _T("*.txt"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("Text (*.txt)|*.txt"), AfxGetMainWnd());

	theApp.OnGlossarySelectCodepage();
	if (IDOK==dlg.DoModal())
	{
		if (m_hThread == NULL)
		{
			m_nWhat = ADDGLOSSARYFROMTXTFILE;
			m_sTempText = dlg.GetPathName();
			m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &m_nThreadID);
			m_dlgInfo.DoModal();
		}

		BOOL bModified = FALSE;
		if (m_mapAutomatic.GetCount())
		{
			CTranslateString::gm_bReplace  = m_dlgInfo.m_bReplace;
			bModified |= m_TSArrayArray.AutomaticTranslate(m_mapAutomatic);
			CTranslateString::gm_bReplace  = TRUE;
		}

		if (bModified)
		{
			UpdateAllViews(NULL, UPDATE_OPENED_DOC, (CObject*)&m_TSArrayArray);
			SetModifiedFlag(TRUE);
		}
		CString sResult;
		sResult.Format(IDP_GLOSSARY_ADDED ,m_mapAutomatic.GetCount());
		AfxMessageBox(sResult);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWkTranslatorDoc::HasData()
{
	return (m_TSArrayArray.GetSize() > 0);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnEnableEditAcc() 
{
	m_bEnableEdit = !m_bEnableEdit;
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnEnableEdit() 
{
	m_bEnableEdit = !m_bEnableEdit;
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::OnUpdateEnableEdit(CCmdUI* pCmdUI) 
{
#ifdef _DEBUG
	pCmdUI->Enable(TRUE);
#else
	pCmdUI->Enable(FALSE);
#endif
	pCmdUI->SetCheck(m_bEnableEdit);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDocument::SetPathName(lpszPathName, bAddToMRU);
	SetTitle(lpszPathName);
}
/////////////////////////////////////////////////////////////////////////////
CString CWkTranslatorDoc::GetAppDir()
{
	CString sDir;
	DWORD dwChars = GetModuleFileName(NULL, sDir.GetBuffer(_MAX_PATH), _MAX_PATH);
	// if full buffer is used, path may be truncated
	if (dwChars < _MAX_PATH)
	{
		int i = sDir.ReverseFind('\\');
		if (i != -1)
		{
			sDir.Left(i);
		}
	}
	return sDir;
}

UINT __stdcall CWkTranslatorDoc::ThreadProc(void* lParam)
{
	CWkTranslatorDoc *pThis = (CWkTranslatorDoc*)lParam;
	switch(pThis->m_nWhat)
	{
		case IMPORT_CLOSSARY:
		 pThis->ImportGlossary(FALSE);
		 break;
		case IMPORT_CLOSSARY_GER:
		 pThis->ImportGlossary(TRUE);
		 break;
		case ADDGLOSSARYFROMTXTFILE:
		 pThis->AddGlossaryFromTxtFile(pThis->m_sTempText);
		 break;
	}
	pThis->m_hThread = NULL;
	pThis->m_nWhat = 0;
	pThis->m_nThreadID = 0;
	pThis->m_dlgInfo.PostMessage(WM_COMMAND, IDOK, 0);

	return 0;
}

LRESULT CWkTranslatorDoc::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	if (wParam==UPDATE_OPENED_DOC)
	{
		if (lParam)
		{
			UpdateAllViews(NULL, UPDATE_OPENED_DOC, (CObject*)&m_TSArrayArray);
			SetModifiedFlag(TRUE);
		}
		CString sResult;
		sResult.Format(IDP_GLOSSARY_ADDED ,m_mapAutomatic.GetCount(), lParam);
		AfxMessageBox(sResult);
	}
	return 0;
}
