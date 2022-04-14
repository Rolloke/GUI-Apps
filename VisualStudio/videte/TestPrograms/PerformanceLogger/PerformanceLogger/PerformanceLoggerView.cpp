// PerformanceLoggerView.cpp : Implementierung der Klasse CPerformanceLoggerView
//

#include "stdafx.h"
#include "PerformanceLogger.h"

#include "PerformanceLoggerDoc.h"
#include "PerformanceLoggerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPerformanceLoggerView

IMPLEMENT_DYNCREATE(CPerformanceLoggerView, CView)

BEGIN_MESSAGE_MAP(CPerformanceLoggerView, CView)
	// Standarddruckbefehle
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CPerformanceLoggerView Erstellung/Zerstörung

CPerformanceLoggerView::CPerformanceLoggerView()
{
	// TODO: Hier Code zum Erstellen einfügen

}

CPerformanceLoggerView::~CPerformanceLoggerView()
{
}

BOOL CPerformanceLoggerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CView::PreCreateWindow(cs);
}

// CPerformanceLoggerView-Zeichnung

void CPerformanceLoggerView::OnDraw(CDC* /*pDC*/)
{
	CPerformanceLoggerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: Code zum Zeichnen der systemeigenen Daten hinzufügen
}


// CPerformanceLoggerView drucken

BOOL CPerformanceLoggerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CPerformanceLoggerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}

void CPerformanceLoggerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Bereinigung nach dem Drucken einfügen
}


// CPerformanceLoggerView Diagnose

#ifdef _DEBUG
void CPerformanceLoggerView::AssertValid() const
{
	CView::AssertValid();
}

void CPerformanceLoggerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPerformanceLoggerDoc* CPerformanceLoggerView::GetDocument() const // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPerformanceLoggerDoc)));
	return (CPerformanceLoggerDoc*)m_pDocument;
}
#endif //_DEBUG


// CPerformanceLoggerView Meldungshandler
