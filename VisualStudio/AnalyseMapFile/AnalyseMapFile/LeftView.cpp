// LeftView.cpp : Implementierung der Klasse CLeftView
//

#include "stdafx.h"
#include "AnalyseMapFile.h"

#include "AnalyseMapFileDoc.h"
#include "LeftView.h"
#include ".\leftview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CListView)

BEGIN_MESSAGE_MAP(CLeftView, CListView)
	// Standarddruckbefehle
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
END_MESSAGE_MAP()


// CLeftView Erstellung/Zerstörung

CLeftView::CLeftView()
{
	// TODO: Hier Code zum Erstellen einfügen
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie die Fensterklasse oder die Stile hier, indem Sie CREATESTRUCT ändern
	cs.style |= LVS_SINGLESEL | LVS_REPORT |  LVS_SHOWSELALWAYS;

	return CListView::PreCreateWindow(cs);
}


// CLeftView drucken

BOOL CLeftView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CLeftView::OnDraw(CDC* /*pDC*/)
{
	CAnalyseMapFileDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: Code zum Zeichnen der systemeigenen Daten hinzufügen
}

void CLeftView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}

void CLeftView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Bereinigung nach dem Drucken einfügen
}

void CLeftView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: Sie können Elemente in die Strukturansicht eintragen, indem Sie über
	//  einen Aufruf von GetTreeCtrl() direkt auf die Struktursteuerung zugreifen.
}


// CLeftView Diagnose

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CListView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CAnalyseMapFileDoc* CLeftView::GetDocument() // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnalyseMapFileDoc)));
	return (CAnalyseMapFileDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView Meldungshandler

int CLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetListCtrl().SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	GetListCtrl().InsertColumn(0, _T("Pfad"));
	GetListCtrl().InsertColumn(1, _T("Datei"));
	int nOrder[2] = {1, 0};
	GetListCtrl().SetColumnOrderArray(2, nOrder);

	return 0;
}

void CLeftView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{
	if (lHint == UPDATE_DOCUMENT_CONTENT)
	{
		ASSERT_KINDOF(CUpdateDocumentContentHint, pHint);
		CUpdateDocumentContentHint *pUDCHint = (CUpdateDocumentContentHint*) pHint;
		LVFINDINFO info;
		info.flags = LVFI_PARTIAL|LVFI_STRING;
		info.psz = pUDCHint->m_sPath;
		int nIndex = GetListCtrl().FindItem(&info);
		if (nIndex == -1)
		{
			nIndex = GetListCtrl().GetItemCount();
			GetListCtrl().InsertItem(nIndex, pUDCHint->m_sPath);
			int nFind = pUDCHint->m_sPath.ReverseFind('\\');
			CString sFile;
			if (nFind != -1)
			{
				sFile = pUDCHint->m_sPath.Mid(nFind+1);
			}
			else
			{
				sFile = pUDCHint->m_sPath;
			}
			GetListCtrl().SetItemText(nIndex, 1, sFile);
			GetListCtrl().SetColumnWidth(0, LVSCW_AUTOSIZE);
			GetListCtrl().SetColumnWidth(1, LVSCW_AUTOSIZE);
		}
		else
		{
			if (pUDCHint->m_nItems == 0)
			{
				GetListCtrl().DeleteItem(nIndex);
				GetListCtrl().SetColumnWidth(0, LVSCW_AUTOSIZE);
				GetListCtrl().SetColumnWidth(1, LVSCW_AUTOSIZE);
			}
			else
			{
				GetListCtrl().SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
				GetListCtrl().EnsureVisible(nIndex, FALSE);
				GetListCtrl().SetSelectionMark(nIndex);
			}
		}
	}
}

void CLeftView::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nItem = GetListCtrl().GetSelectionMark();
	if (nItem != -1)
	{
		GetDocument()->SelectFile(GetListCtrl().GetItemText(nItem, 0), TRUE);
	}
	*pResult = 0;
}
