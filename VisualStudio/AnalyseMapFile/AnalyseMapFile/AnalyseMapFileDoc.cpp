// AnalyseMapFileDoc.cpp : Implementierung der Klasse CAnalyseMapFileDoc
//

#include "stdafx.h"
#include "AnalyseMapFile.h"

#include "AnalyseMapFileDoc.h"
#include ".\analysemapfiledoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

// class CUpdateDocumentContentHint
IMPLEMENT_DYNAMIC(CUpdateDocumentContentHint, CObject)
CUpdateDocumentContentHint::CUpdateDocumentContentHint(int nItems, LPCTSTR sPath)
{
	m_nItems = nItems;
	m_sPath  = sPath;
}

// class CFunctionParamList
CFunctionParamList::CFunctionParamList()
{
	m_pFS = NULL;
}

CFunctionParamList::~CFunctionParamList()
{
	if (m_pFS)
	{
		delete m_pFS;
	}
	DeleteContent();
}

void CFunctionParamList::DeleteContent()
{
	while (GetCount())
	{
		delete RemoveHead();
	};
}

BOOL CFunctionParamList::IsMapFile()
{
	return (m_sMapFile.Mid(m_sMapFile.GetLength()-3).CompareNoCase(_T("map")) == 0) ? TRUE : FALSE;
}

BOOL CFunctionParamList::IsDllFile()
{
	return (m_sMapFile.Mid(m_sMapFile.GetLength()-3).CompareNoCase(_T("dll")) == 0) ? TRUE : FALSE;
}


// class CAnalyseMapFileDoc

IMPLEMENT_DYNCREATE(CAnalyseMapFileDoc, CDocument)

BEGIN_MESSAGE_MAP(CAnalyseMapFileDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_ADDRESS, OnUpdateEditFindAddress)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_FUNCTION, OnUpdateEditFindFunction)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
END_MESSAGE_MAP()

#define LINE_NO_SEPARATOR_LINE		_T("Line numbers for")
#define COLUMN1 _T("Address")
#define COLUMN2 _T("Publics by Value")
#define COLUMN3 _T("Rva+Base")
#define COLUMN4 _T("Lib:Object")


/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileDoc Erstellung/Zerstörung
CAnalyseMapFileDoc::CAnalyseMapFileDoc()
{
	m_nSelected = -1;
}
/////////////////////////////////////////////////////////////////////////////
CAnalyseMapFileDoc::~CAnalyseMapFileDoc()
{
	while (m_FunctionParamMap.GetCount())
	{
		delete m_FunctionParamMap.RemoveHead();
	};
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileDoc Serialisierung
void CAnalyseMapFileDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: Hier Code zum Speichern einfügen
	}
	else if (GetFunctionParam().IsMapFile())
	{
		CString str;
		int nSection = 0;
		int nFind;
		int nAddressLen = 0;
		while (ar.ReadString(str))
		{
			str.TrimLeft();
			if (nSection == 0)
			{
				nFind = str.Find(COLUMN3);
				if (nFind != -1)
				{
					nSection = 1;
				}
			}
			else if (nSection == 1)
			{
				if (str.GetLength() > 5)
				{
					if (str.Find(LINE_NO_SEPARATOR_LINE) != -1)
					{
						nSection = 2;
						continue;
					}
					else if (nAddressLen == 0)
					{
						nAddressLen = str.Find(_T(" "));
					}
					FunctionParam*pFP = new FunctionParam;
					pFP->sAddress = str.Left(nAddressLen);
					pFP->sFunction = str.Mid(nAddressLen+1);
					pFP->sFunction.TrimLeft();
					nFind = pFP->sFunction.Find(_T(" "));
					if (nFind != -1)
					{
						str = pFP->sFunction;
						pFP->sFunction = pFP->sFunction.Left(nFind);
						pFP->sRVApBase = str.Mid(nFind);
						pFP->sRVApBase.TrimLeft();
						str = pFP->sRVApBase;
						pFP->sRVApBase = pFP->sRVApBase.Left(8);
						nFind = str.ReverseFind(_T(' '));
						if (nFind != -1)
						{
							pFP->sObjectFile = str.Mid(nFind);
						}
					}
					GetFunctionParam().AddTail(pFP);
				}
			}
			else if (nSection == 2)
			{
				break;
			}
		};
		CUpdateDocumentContentHint Hint((int)GetFunctionParam().GetCount(), GetFunctionParam().m_sMapFile);
		UpdateAllViews(NULL, UPDATE_DOCUMENT_CONTENT, &Hint);
	}
	else if (GetFunctionParam().IsDllFile())
	{
		if (GetFunctionParam().m_pFS == NULL)
		{
			GetFunctionParam().m_pFS = new FindSymbols;
		}
		FindSymbols *pFS = GetFunctionParam().m_pFS;
		pFS->SetModule(ar.GetFile()->GetFileName());
		BOOL bResult = theApp.FindModule(*pFS);
		if (pFS->dwModuleSize == 0 && !pFS->sModule.IsEmpty())
		{
			theApp.AddModule(*pFS);
		}
		m_nOrdinal = 1;
		SymEnumerateSymbols64(GetCurrentProcess(), pFS->dwBaseAddress, EnumSymbolsProc, (void*)this);
		CUpdateDocumentContentHint Hint((int)GetFunctionParam().GetCount(), GetFunctionParam().m_sMapFile);
		UpdateAllViews(NULL, UPDATE_DOCUMENT_CONTENT, &Hint);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CAnalyseMapFileDoc::EnumSymbolsProc(PCSTR SymbolName, DWORD64 SymbolAddress, ULONG SymbolSize, PVOID UserContext)
{
	CAnalyseMapFileDoc*pThis = (CAnalyseMapFileDoc*) UserContext;
	FunctionParam*pFP = new FunctionParam;
	pFP->sAddress.Format(_T("%08d"), pThis->m_nOrdinal++);
	pFP->sFunction = SymbolName;
	pFP->sObjectFile = theApp.DWORD64ToString(SymbolSize);
	pFP->sRVApBase = theApp.DWORD64ToString(SymbolAddress);
	pThis->GetFunctionParam().AddTail(pFP);
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileDoc Diagnose
#ifdef _DEBUG
void CAnalyseMapFileDoc::AssertValid() const
{
	CDocument::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileDoc-Befehle
void CAnalyseMapFileDoc::OnUpdateEditFindAddress(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFunctionParam().GetCount() > 1);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileDoc::OnUpdateEditFindFunction(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetFunctionParam().GetCount() > 1);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileDoc::DeleteContents()
{
	GetFunctionParam().DeleteContent();
	CDocument::DeleteContents();
}
/////////////////////////////////////////////////////////////////////////////
CFunctionParamList& CAnalyseMapFileDoc::GetFunctionParam()
{
	CFunctionParamList*pList = &m_Dummy;
	POSITION pos = m_FunctionParamMap.FindIndex(m_nSelected);
	if (pos)
	{
		pList = m_FunctionParamMap.GetAt(pos);
	}
	return *pList;
}
/////////////////////////////////////////////////////////////////////////////
int CAnalyseMapFileDoc::GetFiles()
{
	return (int) m_FunctionParamMap.GetCount();
}
/////////////////////////////////////////////////////////////////////////////
int CAnalyseMapFileDoc::GetSelectedFile()
{
	return m_nSelected;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CFunctionParamList*pList = &m_Dummy;
	POSITION pos = m_FunctionParamMap.GetHeadPosition();

	if (!SelectFile(lpszPathName))
	{
		AddMapFile();
	}

	GetFunctionParam().m_sMapFile = lpszPathName;
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileDoc::AddMapFile()
{
	m_FunctionParamMap.AddTail(new CFunctionParamList());
	m_nSelected = ((int)m_FunctionParamMap.GetCount())-1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileDoc::SelectFile(LPCTSTR sFile, BOOL bUpdate/*=FALSE*/)
{
	CFunctionParamList*pList = &m_Dummy;
	POSITION pos = m_FunctionParamMap.GetHeadPosition();
	int nFile = 0;
	while (pos)
	{
		pList = m_FunctionParamMap.GetNext(pos);
		if (pList->m_sMapFile == sFile)
		{
			m_nSelected = nFile;
			if (bUpdate)
			{
				CUpdateDocumentContentHint Hint((int)GetFunctionParam().GetCount(), GetFunctionParam().m_sMapFile);
				UpdateAllViews(NULL, UPDATE_DOCUMENT_CONTENT, &Hint);
			}
			return TRUE;
		}
		nFile++;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileDoc::SetSelectedFile(int nSelected, BOOL bUpdate/*=FALSE*/)
{
	if (nSelected >= 0 && nSelected < GetFiles())
	{
		m_nSelected = nSelected;
		if (bUpdate)
		{
			CUpdateDocumentContentHint Hint((int)GetFunctionParam().GetCount(), GetFunctionParam().m_sMapFile);
			UpdateAllViews(NULL, UPDATE_DOCUMENT_CONTENT, &Hint);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileDoc::OnFileClose()
{
	POSITION pos = m_FunctionParamMap.FindIndex(m_nSelected);
	if (pos)
	{
		CFunctionParamList*pList = m_FunctionParamMap.GetAt(pos);
		if (pList)
		{
			m_nSelected = -1;
			m_FunctionParamMap.RemoveAt(pos);
			CUpdateDocumentContentHint Hint(0, pList->m_sMapFile);
			UpdateAllViews(NULL, UPDATE_DOCUMENT_CONTENT, &Hint);
			delete pList;
			SetSelectedFile(0, TRUE); 
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileDoc::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nSelected >= 0 && m_nSelected < m_FunctionParamMap.GetCount());
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	CDocument::SetPathName(lpszPathName, bAddToMRU);
	m_strPathName.Empty(); // Allow to reload it!
}
/////////////////////////////////////////////////////////////////////////////
