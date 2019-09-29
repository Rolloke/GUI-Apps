// AnalyseMapFileView.cpp : Implementierung der Klasse CAnalyseMapFileView
//

#include "stdafx.h"
#include "AnalyseMapFile.h"

#include "AnalyseMapFileDoc.h"
#include "AnalyseMapFileView.h"
#include "MainFrm.h"
#include "findAddress.h"
#include "CallStackView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LIST_COLUMN_NUMBER		0
#define LIST_COLUMN_FUNCTION	1
#define LIST_COLUMN_ADDRESS		2
#define LIST_COLUMN_OBJ_DLL		3

/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileView
IMPLEMENT_DYNCREATE(CAnalyseMapFileView, CListView)

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAnalyseMapFileView, CListView)
	ON_WM_STYLECHANGED()
	// Standarddruckbefehle
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
//	ON_COMMAND(ID_EDIT_SEARCH_IN_MODULES, OnEditSearchInModules)
ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileView Erstellung/Zerstörung
CAnalyseMapFileView::CAnalyseMapFileView()
{
	m_nLastPos = -1;
}
/////////////////////////////////////////////////////////////////////////////
CAnalyseMapFileView::~CAnalyseMapFileView()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_OWNERDATA | LVS_SINGLESEL | LVS_REPORT |  LVS_SHOWSELALWAYS;
//	cs.dwExStyle |= (LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	return CListView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CListView::OnPrepareDC(pDC, pInfo);
	pDC->SetMapMode(MM_ISOTROPIC);
	CRect rc;
	// TODO! RKE: Unvollständig aber es ist etwas zu sehen
	CRect rcPage(pInfo->m_rectDraw);
	CSize szPage(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
//	GetListCtrl().GetClientRect(&rc);
	rc.left = rc.top = 0;
	rc.right = 0;
	rc.right += GetListCtrl().GetColumnWidth(LIST_COLUMN_NUMBER);
	rc.right += GetListCtrl().GetColumnWidth(LIST_COLUMN_FUNCTION);
	rc.right += GetListCtrl().GetColumnWidth(LIST_COLUMN_ADDRESS);
	rc.right += GetListCtrl().GetColumnWidth(LIST_COLUMN_OBJ_DLL);

	rc.bottom = MulDiv(rc.right, szPage.cy, szPage.cx);

	pDC->SetWindowExt(rc.Size());
	pDC->SetViewportExt(szPage);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnDraw(CDC* pDC)
{
	CAnalyseMapFileDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDC->IsPrinting())
	{
		pDC->SaveDC();
		pDC->SelectObject(GetListCtrl().GetFont());
		GetListCtrl().SendMessage(WM_PRINTCLIENT, (WPARAM)pDC->m_hDC, PRF_CLIENT);
		pDC->RestoreDC(-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
}
/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileView drucken
BOOL CAnalyseMapFileView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Bereinigung nach dem Drucken einfügen
}
/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileView Diagnose
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CAnalyseMapFileView::AssertValid() const
{
	CListView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CAnalyseMapFileDoc* CAnalyseMapFileView::GetDocument() const // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnalyseMapFileDoc)));
	return (CAnalyseMapFileDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileView Meldungshandler
void CAnalyseMapFileView::OnStyleChanged(int /*nStyleType*/, LPSTYLESTRUCT /*lpStyleStruct*/)
{
	
	Default();
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{
	if (lHint == UPDATE_DOCUMENT_CONTENT)
	{
		ASSERT_KINDOF(CUpdateDocumentContentHint, pHint);
		CUpdateDocumentContentHint *pUDCHint = (CUpdateDocumentContentHint*) pHint;
		GetListCtrl().SetItemCount(pUDCHint->m_nItems);
		GetListCtrl().SetColumnWidth(0, LVSCW_AUTOSIZE);
		GetListCtrl().SetColumnWidth(1, LVSCW_AUTOSIZE);
		GetListCtrl().SetColumnWidth(2, LVSCW_AUTOSIZE);
		GetListCtrl().SetColumnWidth(3, LVSCW_AUTOSIZE);
		CWnd *pWnd = AfxGetMainWnd();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
		{
			((CMainFrame*)pWnd)->SetActiveView(this);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	POSITION pos = GetDocument()->GetFunctionParam().FindIndex(pDispInfo->item.iItem);
	if (pos)
	{
		FunctionParam *pFP = (FunctionParam*)GetDocument()->GetFunctionParam().GetAt(pos);
		if (pFP && pDispInfo->item.mask & LVIF_TEXT)
		{
			if (pDispInfo->item.iSubItem == LIST_COLUMN_NUMBER)
			{
				pDispInfo->item.pszText = (LPTSTR)(LPCTSTR)pFP->sAddress;
				pDispInfo->item.cchTextMax = pFP->sAddress.GetLength();
			}
			else if (pDispInfo->item.iSubItem == LIST_COLUMN_FUNCTION)
			{
				pDispInfo->item.pszText = (LPTSTR)(LPCTSTR)pFP->sFunction;
				pDispInfo->item.cchTextMax = pFP->sFunction.GetLength();
			}
			else if (pDispInfo->item.iSubItem == LIST_COLUMN_ADDRESS)
			{
				pDispInfo->item.pszText = (LPTSTR)(LPCTSTR)pFP->sRVApBase;
				pDispInfo->item.cchTextMax = pFP->sRVApBase.GetLength();
			}
			else if (pDispInfo->item.iSubItem == LIST_COLUMN_OBJ_DLL)
			{
				pDispInfo->item.pszText = (LPTSTR)(LPCTSTR)pFP->sObjectFile;
				pDispInfo->item.cchTextMax = pFP->sObjectFile.GetLength();
			}
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
int CAnalyseMapFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetListCtrl().SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	GetListCtrl().InsertColumn(LIST_COLUMN_NUMBER, _T("Nummer"));
	GetListCtrl().InsertColumn(LIST_COLUMN_FUNCTION, _T("Funktion"));
	GetListCtrl().InsertColumn(LIST_COLUMN_ADDRESS, _T("Adresse"));
	GetListCtrl().InsertColumn(LIST_COLUMN_OBJ_DLL, _T(".Obj- / .Dll- Datei"));

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnEditFindAddress()
{
	CFindAddress dlg;
	dlg.m_sSearchString = m_sSearchAddress;
	if (dlg.DoModal() == IDOK)
	{
		DWORD64 dwAddress, dwFind;
		BOOL bNeg;
		LONGLONG lMin = -1, lDiff;
		int nPos, nFound = -1, nFoundSel = -1;
		int nSel, nFiles = GetDocument()->GetFiles();
		int nOldSelected = GetDocument()->GetSelectedFile();
		m_sSearchAddress = dlg.m_sSearchString;
		theApp.GetMainFrame()->SetPaneText(IDS_SEARCH_ADDRESS, m_sSearchAddress);
		
		dwAddress = theApp.StringToDWORD64(m_sSearchAddress);
		m_sSearchAddress = theApp.DWORD64ToString(dwAddress);

		for (nSel=0; nSel<nFiles; nSel++)
		{
			GetDocument()->SetSelectedFile(nSel, FALSE);
			POSITION pos = GetDocument()->GetFunctionParam().GetHeadPosition();
			nPos = 0;
			while (pos)
			{
				FunctionParam *pFP = (FunctionParam*)GetDocument()->GetFunctionParam().GetNext(pos);
				dwFind = (DWORD)theApp.StringToDWORD64(CString(pFP->sRVApBase), FORCE_HEX);
				lDiff =  dwAddress - dwFind;
				bNeg  = lDiff < 0;
				if (bNeg)
				{
					lDiff = -lDiff;
				}
				if (lMin == -1)
				{
					lMin = lDiff;
				}
				else if (lDiff < lMin)
				{
					lMin = lDiff;
					nFound = nPos;
					nFoundSel = nSel;
					if (bNeg)
					{
						nFound--;
					}
				}
				nPos++;
			}
		}
		if (nFound != -1)
		{
			GetDocument()->SetSelectedFile(nFoundSel, nFoundSel != nOldSelected);
			GetListCtrl().SetItemState(nFound, LVIS_SELECTED, LVIS_SELECTED);
			GetListCtrl().EnsureVisible(nFound, FALSE);
			GetListCtrl().SetSelectionMark(nFound);
			CString sFoundOffset = theApp.DWORD64ToString(lMin);
			theApp.GetMainFrame()->SetPaneText(IDS_FOUND_OFFSET, sFoundOffset);
			m_sSearchFunction = GetListCtrl().GetItemText(nFound, LIST_COLUMN_FUNCTION);
			theApp.GetMainFrame()->SetPaneText(IDS_SEARCH_FUNCTION, m_sSearchFunction);
		}
		else
		{
			GetDocument()->SetSelectedFile(nOldSelected, FALSE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnEditFindFunction()
{
	CFindAddress dlg;
	dlg.m_sTitle = _T("Funktion suchen");
	dlg.m_sSearchString = m_sSearchFunction;
	if (dlg.DoModal() == IDOK)
	{
		if (m_sSearchFunction != dlg.m_sSearchString)
		{
			m_nLastPos = -1;
		}
		m_sSearchFunction = dlg.m_sSearchString;
		theApp.GetMainFrame()->SetPaneText(IDS_SEARCH_FUNCTION, m_sSearchFunction);
		OnEditFindNextFnc();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnEditFindNextFnc()
{
	if (!m_sSearchFunction.IsEmpty())
	{
		int nPos = 0, nFound = -1;
		POSITION pos = NULL;
		if (m_nLastPos != -1)
		{
			pos = GetDocument()->GetFunctionParam().FindIndex(m_nLastPos);
			if (pos)
			{
				GetDocument()->GetFunctionParam().GetNext(pos);
				nPos = m_nLastPos+1;
			}
		}
		if (pos == NULL)
		{
			pos = GetDocument()->GetFunctionParam().GetHeadPosition();
			m_nLastPos = 0;
		}
		while (pos)
		{
			FunctionParam *pFP = (FunctionParam*)GetDocument()->GetFunctionParam().GetNext(pos);
			if (pFP->sFunction.Find(m_sSearchFunction) != -1)
			{
				nFound = m_nLastPos = nPos;
				break;
			}
			nPos++;
		}
		if (nFound != -1)
		{
			GetListCtrl().SetItemState(nFound, LVIS_SELECTED, LVIS_SELECTED);
			GetListCtrl().EnsureVisible(nFound, FALSE);
			GetListCtrl().SetSelectionMark(nFound);
		}
		else
		{
			AfxMessageBox(_T("Keinen weiteren Eintrag gefunden!"), MB_OK|MB_ICONINFORMATION);
			m_nLastPos = 0;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnUpdateEditFindNextFnc(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_sSearchFunction.IsEmpty() && m_nLastPos != -1);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_nLastPos = GetListCtrl().GetSelectionMark();
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_nLastPos != -1)
	{
		CString sMsg, sF = GetListCtrl().GetItemText(m_nLastPos, LIST_COLUMN_FUNCTION);
		int nF, nC, nP;
		sF.Replace(_T("?"), _T(""));
		sF.Replace(_T("$"), _T(""));
		sF.Replace(_T("_imp_"), _T("")); 
		
		nF = sF.Find(_T("@"));
		if (nF != -1 && sF.GetAt(nF+1) != _T('@'))
		{
			nC = sF.Find(_T("@@"), nF+1);
			if (nC != -1)
			{
				CString sFunc = sF.Left(nF);
				nF++;
				CString sClass =sF.Mid(nF, nC-nF);
				sMsg.Format(_T("%s::%s(..)"), sClass, sFunc);
				nP = sF.Find(_T("AEH"), nC+2);
				if (nP != -1)
				{
					sMsg = _T("int ") + sMsg;
				}
				else
				{
					nP = sF.Find(_T("AEX"), nC+2);
					if (nP != -1)
					{
						sMsg = _T("void ") + sMsg;
					}
				}
			}
		}
		else
		{
			nF = sF.Find(_T("@"));
			if (nF != -1)
			{
				sMsg = sF.Left(nF) + _T("(..)");
			}
			else
			{
				sMsg = sF;
			}
		}
		if (sMsg.IsEmpty())
		{
			sMsg = GetListCtrl().GetItemText(m_nLastPos, LIST_COLUMN_FUNCTION);
		}

		m_sSearchFunction = sF;
		theApp.GetMainFrame()->SetPaneText(IDS_SEARCH_FUNCTION, m_sSearchFunction);
		if (AfxMessageBox(sMsg, MB_ICONINFORMATION|MB_OKCANCEL) == IDOK)
		{
			CString sAddr = GetListCtrl().GetItemText(m_nLastPos, LIST_COLUMN_ADDRESS);
			theApp.GetMainFrame()->GetCallStackPane()->InsertEntry(sAddr, sMsg);
		}

	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
