// ETS3DVView.cpp : Implementierung der Klasse CETS3DVView
//

#include "stdafx.h"
#include "ETS3DV.h"

#include "ETS3DVDoc.h"
#include "ETS3DVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CETS3DVView

IMPLEMENT_DYNCREATE(CETS3DVView, CView)

BEGIN_MESSAGE_MAP(CETS3DVView, CView)
	//{{AFX_MSG_MAP(CETS3DVView)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CETS3DVView Konstruktion/Destruktion

CETS3DVView::CETS3DVView()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen,

}

CETS3DVView::~CETS3DVView()
{
}

BOOL CETS3DVView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CETS3DVView Zeichnen

void CETS3DVView::OnDraw(CDC* pDC)
{
	CETS3DVDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// ZU ERLEDIGEN: Hier Code zum Zeichnen der ursprünglichen Daten hinzufügen
}

/////////////////////////////////////////////////////////////////////////////
// CETS3DVView Drucken

BOOL CETS3DVView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CETS3DVView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}

void CETS3DVView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Hier Bereinigungsarbeiten nach dem Drucken einfügen
}

/////////////////////////////////////////////////////////////////////////////
// OLE-Server-Unterstützung

// Der folgende Befehls-Handler stellt die Standardtastatur als
//  Benutzerschnittstelle zum Abbruch der direkten Bearbeitungssitzung zur Verfügung. Hier
//  verursacht der Container (nicht der Server) die Deaktivierung.
void CETS3DVView::OnCancelEditSrvr()
{
	GetDocument()->OnDeactivateUI(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CETS3DVView Diagnose

#ifdef _DEBUG
void CETS3DVView::AssertValid() const
{
	CView::AssertValid();
}

void CETS3DVView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CETS3DVDoc* CETS3DVView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CETS3DVDoc)));
	return (CETS3DVDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CETS3DVView Nachrichten-Handler
