
// LeftView.cpp: Implementierung der Klasse CLeftView
//

#include "stdafx.h"
#include "AnyFileViewer.h"

#include "AnyFileViewerDoc.h"
#include "LeftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	// Standarddruckbefehle
	ON_COMMAND(ID_FILE_PRINT, &CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTreeView::OnFilePrintPreview)
	ON_COMMAND(ID_VIEW_UPDATE, &CLeftView::OnViewUpdate)
END_MESSAGE_MAP()


// CLeftView-Erstellung/Zerstörung

CLeftView::CLeftView()
{
	// TODO: Hier Code zur Konstruktion einfügen
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie die Fensterklasse oder die Stile hier, indem Sie CREATESTRUCT ändern

	return CTreeView::PreCreateWindow(cs);
}


// CLeftView drucken

BOOL CLeftView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CLeftView::OnDraw(CDC* /*pDC*/)
{
	CAnyFileViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: Code zum Zeichnen der nativen Daten hinzufügen
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
	CTreeView::OnInitialUpdate();

	// TODO: Sie können Elemente in Ihre TreeView eintragen, indem Sie über
	//  einen Aufruf von GetTreeCtrl() direkt auf die Baumsteuerung zugreifen.
}


// CLeftView-Diagnose

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CAnyFileViewerDoc* CLeftView::GetDocument() // Nichtdebugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnyFileViewerDoc)));
	return (CAnyFileViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView-Meldungshandler


void CLeftView::OnViewUpdate()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
}
