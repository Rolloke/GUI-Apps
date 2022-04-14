/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: LogViewDoc.cpp $
// ARCHIVE:		$Archive: /Project/Tools/LogView/LogViewDoc.cpp $
// CHECKIN:		$Date: 3/15/06 10:33a $
// VERSION:		$Revision: 45 $
// LAST CHANGE:	$Modtime: 3/15/06 10:23a $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogView.h"
#include "Mainfrm.h"
#include "LogViewDoc.h"
#include "CntrItem.h"

#include "DlgLoadCompressed.h"
#include "LogViewView.h"
#include ".\logviewdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FIRST_FILE 2
/////////////////////////////////////////////////////////////////////////////
// CLogViewDoc
IMPLEMENT_DYNCREATE(CLogViewDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CLogViewDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CLogViewDoc)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, CRichEditDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, CRichEditDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, CRichEditDoc::OnUpdateObjectVerbMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SEARCH_IN_COMPRESSED, OnUpdateEditSearchInCompressed)
	ON_COMMAND(ID_VIEW_PREV_COMPRESSED, OnViewPrevCompressed)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREV_COMPRESSED, OnUpdateViewPrevCompressed)
	ON_COMMAND(ID_VIEW_NEXT_COMPRESSED, OnViewNextCompressed)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT_COMPRESSED, OnUpdateViewNextCompressed)
	ON_COMMAND(ID_EDIT_STOP_SEARCH_IN_COMPRESSED, OnEditStopSearchInCompressed)
	ON_UPDATE_COMMAND_UI(ID_EDIT_STOP_SEARCH_IN_COMPRESSED, OnUpdateEditStopSearchInCompressed)
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogViewDoc construction/destruction

/////////////////////////////////////////////////////////////////////////////
CLogViewDoc::CLogViewDoc()
{
	m_bDecompress = FALSE;
	m_pSearchThread = NULL;
	m_bRun = FALSE;

	m_sTempFileName = GetLogPath();
	m_sFileName		= _T("");
}

/////////////////////////////////////////////////////////////////////////////
CLogViewDoc::~CLogViewDoc()
{
	if (m_pSearchThread)
	{
		TerminateThread(m_pSearchThread->m_hThread, 1);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;


	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
CRichEditCntrItem* CLogViewDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return new CLogViewCntrItem(preo, (CLogViewDoc*) this);
}



/////////////////////////////////////////////////////////////////////////////
// CLogViewDoc serialization

/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::Serialize(CArchive& ar)
{
//	if (ar.IsStoring())
//	{
//	}
//	else
//	{
//	}

	// set CRichEditDoc::m_bRTF = FALSE if you are serializing as text
	m_bRTF = FALSE;
	if (theApp.DoReadUnicode())
	{
		((CLogViewView*)GetView())->Stream(ar, FALSE);
	}
	else
	{
		CRichEditDoc::Serialize(ar);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CLogViewDoc diagnostics

#ifdef _DEBUG
void CLogViewDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CLogViewDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::IsModified()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	TCHAR* pszTempName	= _ttempnam(lpszPathName, _T("TmpLog"));
	int nLen = _MAX_PATH*2;
	m_sTempFileName		= pszTempName;
	int nBackSlash = m_sTempFileName.ReverseFind(_T('\\'));
	m_sTempFileName = m_sTempFileName.Left(nBackSlash);
	LPTSTR sBuffer = m_sTempFileName.GetBufferSetLength(nLen);
	WK_GetLongPathName(sBuffer, sBuffer, nLen);
	m_sTempFileName.ReleaseBuffer();
	m_sTempFileName += &pszTempName[nBackSlash];
	free(pszTempName);

	m_sFileName			= lpszPathName;

	CStringArray	FileNames;

	CStringArray upFiles;
	CFileFind fileFinder;
	CString sPattern;
	CString sPath;
	CString sBasename;
	BOOL bAppendFile = FALSE;

	WK_SplitPath(m_sFileName, sPath, sBasename);

	CWnd *pWnd = theApp.GetMainWnd();
	ASSERT_KINDOF(CMainFrame, pWnd);
	
	// Ist das ausgewählte File ein UP-File? 
	if (!((CMainFrame*)pWnd)->IsInOnTimer()	// auto update is timer controlled
		&& m_sFileName.Find(_T("UP"))==-1)
	{
		// Existieren zu diesem File UP-Files?
		sPattern.Format(_T("%sUP%s*"),sPath,sBasename);
		BOOL bFoundSome = fileFinder.FindFile(sPattern);
		while (bFoundSome)
		{
			bFoundSome = fileFinder.FindNextFile();
			CString sFullUpFilePath = sPath + fileFinder.GetFileName();
			FileNames.Add(sFullUpFilePath);
			bAppendFile = TRUE;
		}
		if (FileNames.GetCount())
		{
			QSortStrings(FileNames);//, TRUE);
			CDlgLoadCompressed dlg;
			dlg.m_pFiles = &FileNames;
			dlg.DoModal();
		}
	}

	// Ausgewähltes File in die Liste hängen.
	FileNames.Add(m_sFileName);

	// Lade alle Files der Liste
	LoadList(FileNames);
	
	// Temporäres Logfile öffnen
	if (!CRichEditDoc::OnOpenDocument(m_sTempFileName))
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::LoadList(const CStringArray &FileNames)
{
	int nI, nSize = FileNames.GetSize();
	if (nSize == 1)
	{
		return ReadFile(FileNames[0], m_sTempFileName, FALSE);
	}
	for (nI = 0; nI < nSize; nI++)
	{
		if (!ReadFile(FileNames[nI], m_sTempFileName, nI != 0 ? TRUE : FIRST_FILE))
			return FALSE;

	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::ReadFile(const CString &sFN, CString &sTempFN, BOOL bAppend)
{
	BOOL  bRet  = FALSE;
	BYTE* pData = NULL;
	CFile FileWrite;
	CString sFileName = sFN;
	CString sTempFileName = sTempFN;
	CString sTempLgzFileName;

	((CMainFrame*)theApp.m_pMainWnd)->SetStatusText(sFN);

	TRY
	{
		m_bDecompress = FALSE;
		// Komprimierte .lgz-Datei?
		if (sFileName.Find(_T(".lgz")) != -1 || sFileName.Find(_T(".LGZ")) != -1)
		{
			// Temporären Dateinamen für das dekomprimierte File erzeugen.
			TCHAR* pszTempName = _ttempnam(sFileName, _T("TmpLgz"));
			int nLen = _MAX_PATH*2;
			sTempLgzFileName		= pszTempName;
			int nBackSlash = sTempLgzFileName.ReverseFind(_T('\\'));
			sTempLgzFileName = sTempLgzFileName.Left(nBackSlash);
			LPTSTR sBuffer = sTempLgzFileName.GetBufferSetLength(nLen);
			WK_GetLongPathName(sBuffer, sBuffer, nLen);
			sTempLgzFileName.ReleaseBuffer();
			sTempLgzFileName += &pszTempName[nBackSlash];
			free(pszTempName);

			// Datei dekomprimieren
			m_bDecompress = DecompressLGZFile(sFileName, sTempLgzFileName);
			sFileName = sTempLgzFileName;
		}

		if (theApp.DoReadUnicode() && bAppend == FALSE)
		{
			sTempFN = sFileName;
			return 2;
		}

		CFile FileRead(sFileName, CFile::modeRead | CFile::shareDenyNone);

		DWORD dwLen = (DWORD)FileRead.GetLength();
		int nStart = 0;
		pData = new BYTE[dwLen];

		if (!pData)
			return FALSE;

		// File laden
		FileRead.Read(pData, dwLen);
		FileRead.Close();

		// Das dekomprimierte temporäre File löschen
		if (m_bDecompress)
		{
			CFile::Remove(sTempLgzFileName);
		}

		// Decodieren
		if (theApp.DoReadUnicode())
		{
			if (pData[0] == 0xff && pData[1] == 0xff && bAppend != FIRST_FILE)
			{
				nStart = 4;
			}
		}
		else
		{
			if (pData[0] == 0xff && pData[1] == 0xff)
			{
				nStart = 4;
			}
			Decode(&pData[nStart], dwLen-nStart);
		}

		// Soll die dekodierte an eine bestehende Datei angehängt werden?
		if (bAppend == TRUE)	// rke: only TRUE means append
		{
			FileWrite.Open(sTempFileName, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
			FileWrite.SeekToEnd();
		}
		else // Values: (FALSE, FIRST_FILE)
		{
			FileWrite.Open(sTempFileName, CFile::modeWrite | CFile::modeCreate);
		}

		// Temporäre dekodierte Logdatei schreiben.
		FileWrite.Write(&pData[nStart], dwLen-nStart);

		FileWrite.Close();

		WK_DELETE(pData);
		bRet = TRUE;
	}
	CATCH( CFileException, e )
	{
		WK_DELETE(pData);
		bRet = FALSE;
	}
	END_CATCH

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::OnCloseDocument() 
{
	CString sTempFileName;
	if (m_sFileName != m_sTempFileName) // Save the name before destroying Document.
	{
		sTempFileName = m_sTempFileName;
	}
	if (m_sFileName.Find(TEMPORARY_LOGFILE) != -1)
	{
		sTempFileName = m_sTempFileName;
	}


	if (m_pSearchThread)
	{
		TerminateThread(m_pSearchThread->m_hThread, 1);
	}

	CRichEditDoc::OnCloseDocument();
	if (!sTempFileName.IsEmpty())
	{
		TRY								
		{
			CFile::Remove(sTempFileName);
		}
		CATCH( CFileException, e )
		{
			
		}
		END_CATCH
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::DecompressLGZFile(const CString & sName, const CString &sNew)
{
	BOOL bReturn = FALSE;
	CString sNewNameLog = sNew+_T(".log");
	CString sNewNameLgz = sNew+_T(".lgz");
	
	if (CopyFile(sName,sNewNameLgz,TRUE))
	{
		if (WK_DecompressLogFile(sNewNameLgz))
		{
			CFile::Rename(sNewNameLog, sNew);
			bReturn = TRUE;
		}
		else
		{
			CString sMsg;
			sMsg.Format(_T("Could not decompress file (%s)\n"), sNewNameLgz);
			AfxMessageBox(sMsg,MB_OK);
		}
	}
	else
	{
		CString sMsg;
		sMsg.Format(_T("Could not create tmp file (%s)\n"),GetLastErrorString());
		AfxMessageBox(sMsg,MB_OK);
	}
	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	
//	return CRichEditDoc::OnSaveDocument(lpszPathName);
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::Refresh() 
{
	DeleteContents();
	if (m_sTempFileName != m_sFileName)
	{
		TRY								
		{
			CFile::Remove(m_sTempFileName);
		}
		CATCH( CFileException, e )
		{
			
		}
		END_CATCH
	}
	OnOpenDocument(m_sFileName);
}
/////////////////////////////////////////////////////////////////////////////
int __cdecl CLogViewDoc::CompareCStringRef(const void *p1, const void *p2)
{
	CString *pS1 = (CString*)p1;
	CString *pS2 = (CString*)p2;
	return _tcscmp(*pS1, *pS2);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::QSortStrings(CStringArray &ar)
{
	qsort(ar.GetData(), ar.GetSize(), sizeof(CString&), CompareCStringRef);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::GetCompressedFiles(CString &sPath, CStringArray *par)
{
	CString sBasename, sPattern;
	WK_SplitPath(m_sFileName, sPath, sBasename);
	CFileFind fileFinder;
	sPattern.Format(_T("%sUP%s*"), sPath, sBasename);
	BOOL bFoundSome = fileFinder.FindFile(sPattern);
	while (bFoundSome)
	{
		if (par)
		{
			bFoundSome = fileFinder.FindNextFile();
			par->Add(fileFinder.GetFileName());
		}
		else
		{
			break;
		}
	}
	
	if (par)
	{
		QSortStrings(*par);
		return par->GetCount();
	}
	return bFoundSome;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::StartSearchInCompressedThread(CLogViewView *pView)
{
#ifdef _THREAD_SEARCH
	void** pnArgs = (void**)new BYTE[sizeof(void*)*2];
	pnArgs[0] = this;
	pnArgs[1] = pView;
	m_pSearchThread = AfxBeginThread(SearchThread, (LPVOID)pnArgs);
	m_pSearchThread->m_bAutoDelete = TRUE;
#else
	OnEditSearchInCompressed((CLogViewView*)pView);
#endif
}
/////////////////////////////////////////////////////////////////////////////
UINT CLogViewDoc::SearchThread(LPVOID pParam )
{
	void** pnArgs = (void**)pParam;
	CLogViewDoc *pThis = (CLogViewDoc*) pnArgs[0];
	CLogViewView*pView = (CLogViewView*) pnArgs[1];
	delete (BYTE*)pnArgs;
	pThis->m_bRun = TRUE;
	pThis->OnEditSearchInCompressed(pView);

	pView->EndSearchInCompressedThread();

	pThis->m_pSearchThread = NULL;
	pThis->m_bRun = FALSE;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::IsSearchThreadRunning()
{
	return m_pSearchThread != NULL && m_bRun ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::StopSearchThread()
{
	m_bRun = FALSE;
	if (m_pSearchThread)
	{
		WaitForSingleObject(m_pSearchThread->m_hThread, 100);
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDIT_STOP_SEARCH_IN_COMPRESSED);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::LoadDocumentThreadSave(LPCTSTR lpszPathName)
{
	SetModifiedFlag();  // dirty during de-serialize
	CRichEditView*pView = GetView();
	CFile file;
	if (file.Open(lpszPathName, CFile::modeRead|CFile::shareDenyNone) && file.GetLength())
	{
		CArchive loadArchive(&file, CArchive::load | CArchive::bNoFlushOnDelete);
		loadArchive.m_pDocument = this;
		loadArchive.m_bForceFlat = FALSE;
		((CLogViewView*)pView)->DeleteContentThreadSave();

		TRY
		{
			pView->Stream(loadArchive, FALSE);     // load me
			loadArchive.Close();
		}
		CATCH_ALL(e)
		{
			delete e;
			return ;
		}
		END_CATCH_ALL
		pView->GetRichEditCtrl().SetModify(FALSE);
	}
	SetModifiedFlag(FALSE);     // start off with unmodified
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::OnEditSearchInCompressed(CLogViewView*pView)
{
	CString sPath;
	CStringArray ar;
	GetCompressedFiles(sPath, &ar);

	int nCount = ar.GetCount();
	int nCurrent = -1;
	if (   pView->m_nCurrentItem != -1 
		&& IsBetween(pView->m_nCurrentItem, 0, nCount-1))
	{
		nCurrent = pView->m_nCurrentItem;
	}
	if (nCurrent == -1)
	{
		nCurrent = pView->m_bNext ? 0 : nCount -1;
	}

#ifdef _THREAD_SEARCH
	while (m_bRun)
#else
	while (TRUE)
#endif
	{
		CString sFullUpFilePath = sPath + ar.GetAt(nCurrent);
		if (m_sCurrent.IsEmpty())
		{
			ReadFile(sFullUpFilePath, m_sTempFileName, FALSE);
#ifdef _THREAD_SEARCH
			LoadDocumentThreadSave(m_sTempFileName);
#else
			CDocument::OnOpenDocument(m_sTempFileName);
#endif
			pView->m_bFirstSearch = FALSE;
			if (pView->m_bNext)
			{
				pView->m_lInitialSearchPos = 0;
			}
			else
			{
				pView->m_lInitialSearchPos = pView->GetRichEditCtrl().GetTextLength();
			}
		}
		else
		{
			ASSERT(m_sCurrent == sFullUpFilePath);
		}
		if (!m_bRun)
		{
			return;
		}
#ifdef _THREAD_SEARCH
		BOOL bResult = FALSE;
		pView->SendMessage(WM_USER, (WPARAM)MAKELONG(WM_COMMAND, ID_EDIT_REPEAT), (LPARAM)&bResult);
		if (bResult)
#else
		if (pView->OnEditRepeat())
#endif
		{
			pView->m_nCurrentItem = nCurrent;
			m_sCurrent = sFullUpFilePath;
			return;
		}
		else
		{
			m_sCurrent.Empty();
		}
		if (pView->m_bNext)
		{
			nCurrent++;
			if (nCurrent>=nCount)
			{
				break;
			}
		}
		else
		{
			nCurrent --;
			if (nCurrent<0)
			{
				break;
			}
		}
	}
	pView->m_nCurrentItem = -1;
	m_sCurrent.Empty();
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::OnEditStopSearchInCompressed()
{
	StopSearchThread();
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::OnUpdateEditStopSearchInCompressed(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSearchThread != NULL);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewDoc::SelectCompressedFileByTime(CLogViewView*pView, int nDay, int nSec)
{
	if (pView->IsTimeAvailable(nDay, nSec))	// erst mal das aktuelle
	{
		return TRUE;
	}
	CString sPath, sFile, sFullUpFilePath, sOldCurrent = m_sCurrent;
	CStringArray ar;
	GetCompressedFiles(sPath, &ar);
	if (!sOldCurrent.IsEmpty())				// die aktuelle komprimierte Datei sichern
	{
		sOldCurrent = sOldCurrent.Mid(sPath.GetLength());
	}

	int i, nFind, nCount = ar.GetCount();
	for (i=nCount-1; i>=0; i--)				// von hinten suchen,
	{										// da immer das letzte datum im Namen steht
		sFile = ar.GetAt(i);
		if (!sOldCurrent.IsEmpty() && sOldCurrent.CompareNoCase(sFile) == 0)
		{									// did akltuelle Datei
			continue;						// nicht noch einmal laden
		}
		nFind = sFile.Find(_T("_"));
		if (nFind != -1)
		{
			int nYear=0, nMonth=0, nSearchDay=0;
			CString sTemp = sFile.Mid(nFind+1, 8);
			if (_stscanf(sTemp, _T("%04d%02d%02d"), &nYear, &nMonth, &nSearchDay) == 3)
			{
				nSearchDay = (nYear - 2000) * 365 + nMonth * 30 + nSearchDay;
				if (nDay <= nSearchDay)
				{
					CString sFullUpFilePath = sPath + sFile;
					ReadFile(sFullUpFilePath, m_sTempFileName, FALSE);
					CRichEditDoc::OnOpenDocument(m_sTempFileName);
					m_sCurrent = sFullUpFilePath;
					if (pView->IsTimeAvailable(nDay, nSec))
					{
						return TRUE;
					}
				}
			}
			nYear = 0;
		}
	}
	if (!sOldCurrent.IsEmpty())	// wa die Originaldatei noch nicht geladen?
	{
		ReadFile(m_sFileName, m_sTempFileName, FALSE);
		CRichEditDoc::OnOpenDocument(m_sTempFileName);
		if (pView->IsTimeAvailable(nDay, nSec))
		{
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::OnUpdateEditSearchInCompressed(CCmdUI *pCmdUI)
{
	if (m_sFileName.Find(_T("UP"))==-1 && m_pSearchThread == NULL)
	{
		CString sPath;
		pCmdUI->Enable(GetCompressedFiles(sPath, NULL));
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::OnViewPrevCompressed()
{
	if (m_sTempFileName.Find(_T("\\Tmp")) != -1)
	{
		CFile::Remove(m_sTempFileName);
	}
	CString sPath;
	CStringArray ar;
	GetCompressedFiles(sPath, &ar);
	CString sPath2, sCurrent;
	WK_SplitPath(m_sCurrent, sPath2, sCurrent);
	m_sCurrent.Empty();

	int nFind = FindString(ar, sCurrent, FALSE, TRUE);
	if (nFind == -1)
	{
		nFind  = ar.GetCount()-1;
	}
	else if (nFind > 0)
	{
		nFind--;
	}
	else
	{
		m_sCurrent = m_sFileName;
	}

	if (m_sCurrent.IsEmpty())
	{
		m_sCurrent = sPath + ar.GetAt(nFind);
	}
	ReadFile(m_sCurrent, m_sTempFileName, FALSE);
	CRichEditDoc::OnOpenDocument(m_sTempFileName);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::OnUpdateViewPrevCompressed(CCmdUI *pCmdUI)
{
	OnUpdateEditSearchInCompressed(pCmdUI);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::OnViewNextCompressed()
{
	if (m_sTempFileName.Find(_T("\\Tmp")) != -1)
	{
		CFile::Remove(m_sTempFileName);
	}
	CString sPath;
	CStringArray ar;
	GetCompressedFiles(sPath, &ar);
	CString sPath2, sCurrent;
	WK_SplitPath(m_sCurrent, sPath2, sCurrent);
	m_sCurrent.Empty();

	int nFind = FindString(ar, sCurrent, FALSE, TRUE);
	if (nFind == -1)
	{
		nFind  = 0;
	}
	else if (nFind < ar.GetCount()-1)
	{
		nFind++;
	}
	else
	{
		m_sCurrent = m_sFileName;
	}

	if (m_sCurrent.IsEmpty())
	{
		m_sCurrent = sPath + ar.GetAt(nFind);
	}
	ReadFile(m_sCurrent, m_sTempFileName, FALSE);
	CRichEditDoc::OnOpenDocument(m_sTempFileName);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::OnUpdateViewNextCompressed(CCmdUI *pCmdUI)
{
	OnUpdateEditSearchInCompressed(pCmdUI);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewDoc::DeleteContents()
{
	CRichEditView* pView = CRichEditDoc::GetView();
	if (pView && pView->IsKindOf(RUNTIME_CLASS(CLogViewView)))
	{
		pView->SendMessage(WM_COMMAND, ID_VIEW_DELETE_BOOKMARKS);
	}

	CRichEditDoc::DeleteContents();
}
/////////////////////////////////////////////////////////////////////////////
CFile* CLogViewDoc::GetFile(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	nOpenFlags &= ~CFile::shareDenyWrite;
	nOpenFlags |= CFile::shareDenyNone;
	return CDocument::GetFile(lpszFileName, nOpenFlags, pError);
}
