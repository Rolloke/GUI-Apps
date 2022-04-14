// MenuToolTipView.cpp : Implementierung der Klasse CMenuToolTipView
//

#include "stdafx.h"
#include "MenuToolTip.h"

#include "MenuToolTipDoc.h"
#include "MenuToolTipView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMenuToolTipView

IMPLEMENT_DYNCREATE(CMenuToolTipView, CView)

BEGIN_MESSAGE_MAP(CMenuToolTipView, CView)
	// Standarddruckbefehle
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMenuToolTipView Erstellung/Zerstörung

CMenuToolTipView::CMenuToolTipView()
{
	// TODO: Hier Code zum Erstellen einfügen

}

CMenuToolTipView::~CMenuToolTipView()
{
}

BOOL CMenuToolTipView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CView::PreCreateWindow(cs);
}

// CMenuToolTipView-Zeichnung

void CMenuToolTipView::OnDraw(CDC* /*pDC*/)
{
	CMenuToolTipDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: Code zum Zeichnen der systemeigenen Daten hinzufügen
}


// CMenuToolTipView drucken

BOOL CMenuToolTipView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CMenuToolTipView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}

void CMenuToolTipView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Bereinigung nach dem Drucken einfügen
}


// CMenuToolTipView Diagnose

#ifdef _DEBUG
void CMenuToolTipView::AssertValid() const
{
	CView::AssertValid();
}

void CMenuToolTipView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMenuToolTipDoc* CMenuToolTipView::GetDocument() const // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMenuToolTipDoc)));
	return (CMenuToolTipDoc*)m_pDocument;
}
#endif //_DEBUG


// CMenuToolTipView Meldungshandler
