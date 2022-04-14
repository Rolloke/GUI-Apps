// VollOleAppView.cpp : Implementierung der Klasse CVollOleAppView
//

#include "stdafx.h"
#include "VollOleApp.h"

#include "VollOleAppDoc.h"
#include "CntrItem.h"
#include "VollOleAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppView

IMPLEMENT_DYNCREATE(CVollOleAppView, CView)

BEGIN_MESSAGE_MAP(CVollOleAppView, CView)
	//{{AFX_MSG_MAP(CVollOleAppView)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
	ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppView Konstruktion/Destruktion

CVollOleAppView::CVollOleAppView()
{
	m_pSelection = NULL;
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen,

}

CVollOleAppView::~CVollOleAppView()
{
}

BOOL CVollOleAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppView Zeichnen

void CVollOleAppView::OnDraw(CDC* pDC)
{
	CVollOleAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// ZU ERLEDIGEN: Hier Code zum Zeichnen der ursprünglichen Daten hinzufügen
}

void CVollOleAppView::OnInitialUpdate()
{
	CView::OnInitialUpdate();


	// ZU ERLEDIGEN: Entfernen Sie diesen Code, sobald der endgültige Code für das Auswahlmodell geschrieben ist
	m_pSelection = NULL;    // Auswahl initialisieren

	//Active documents should always be activated
	COleDocument* pDoc = (COleDocument*) GetDocument();
	if (pDoc != NULL)
	{
		// activate the first one
		POSITION posItem = pDoc->GetStartPosition();
		if (posItem != NULL)
		{
			CDocItem* pItem = pDoc->GetNextItem(posItem);

			// only if it's an Active document
			COleDocObjectItem *pDocObjectItem =
				DYNAMIC_DOWNCAST(COleDocObjectItem, pItem);

			if (pDocObjectItem != NULL)
			{
				pDocObjectItem->DoVerb(OLEIVERB_SHOW, this);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppView Drucken

BOOL CVollOleAppView::OnPreparePrinting(CPrintInfo* pInfo)
{
	if (!CView::DoPreparePrinting(pInfo))
		return FALSE;
	
	if (!COleDocObjectItem::OnPreparePrinting(this, pInfo))
		return FALSE;

	return TRUE;
}

void CVollOleAppView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}

void CVollOleAppView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Hier Bereinigungsarbeiten nach dem Drucken einfügen
}

void CVollOleAppView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// ZU ERLEDIGEN: Benutzerdefinierten Code zum Ausdrucken hier einfügen
	if(pInfo->m_bDocObject)
		COleDocObjectItem::OnPrint(this, pInfo, TRUE);
	else
		CView::OnPrint(pDC, pInfo);
}

void CVollOleAppView::OnDestroy()
{
	// Das Element bei Destruktion deaktivieren; dies ist wichtig,
	// wenn eine Teilansicht (Splitter View) verwendet wird.
   CView::OnDestroy();
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
}


/////////////////////////////////////////////////////////////////////////////
// OLE-Client-Unterstützung und -Befehle

BOOL CVollOleAppView::IsSelected(const CObject* pDocItem) const
{
	// Die nachfolgende Implementierung ist angemessen, wenn sich Ihre Auswahl nur aus
	//  CVollOleAppCntrItem-Objekten zusammensetzt. Zur Bearbeitung unterschiedlicher 
	//  Auswahlmechanismen sollte die hier gegebene Implementierung ersetzt werden.

	// ZU ERLEDIGEN: Implementieren Sie diese Funktion, die auf ein ausgewähltes OLE-Client-Element testet

	return pDocItem == m_pSelection;
}

void CVollOleAppView::OnInsertObject()
{
	// Standarddialogfeld zum Einfügen von Objekten aufrufen, um Infos abzufragen
	//  für neues CVollOleAppCntrItem-Objekt.
	COleInsertDialog dlg;
	if (dlg.DoModal(COleInsertDialog::DocObjectsOnly) != IDOK)
		return;

	BeginWaitCursor();

	CVollOleAppCntrItem* pItem = NULL;
	TRY
	{
		// Neues, mit diesem Dokument verbundenes Element erzeugen.
		CVollOleAppDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new CVollOleAppCntrItem(pDoc);
		ASSERT_VALID(pItem);

		// Element mit Dialogfelddaten initialisieren.
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  // Beliebige Ausnahme erzeugen
		ASSERT_VALID(pItem);

		pItem->DoVerb(OLEIVERB_SHOW, this);

		ASSERT_VALID(pItem);

		// Die Größe wird hier willkürlich auf die Größe
		//  des zuletzt eingefügten Elements gesetzt.

		// ZU ERLEDIGEN: Implementieren Sie die Auswahl erneut in einer für Ihre Anwendung geeigneten Weise

		m_pSelection = pItem;   // set selection to last inserted item
		pDoc->UpdateAllViews(NULL);
	}
	CATCH(CException, e)
	{
		if (pItem != NULL)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH

	EndWaitCursor();
}

// Der folgende Befehls-Handler stellt die Standardtastatur als
//  Benutzerschnittstelle zum Abbruch der direkten Bearbeitungssitzung zur Verfügung. Hier
//  verursacht der Container (nicht der Server) die Deaktivierung.
void CVollOleAppView::OnCancelEditCntr()
{
	// Schließen aller direkt aktiven Elemente dieser Ansicht.
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// Für einen Container müssen OnSetFocus und OnSize speziell gehandhabt werden,
//  wenn ein Objekt direkt bearbeitet wird.
void CVollOleAppView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// dieses Element muss den Fokus erhalten, wenn es sich in der gleichen Ansicht befindet
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();   // kein Aufruf der Basisklasse
			return;
		}
	}

	CView::OnSetFocus(pOldWnd);
}

void CVollOleAppView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
		pActiveItem->SetItemRects();
}

/////////////////////////////////////////////////////////////////////////////
// OLE-Server-Unterstützung

// Der folgende Befehls-Handler stellt die Standardtastatur als
//  Benutzerschnittstelle zum Abbruch der direkten Bearbeitungssitzung zur Verfügung. Hier
//  verursacht der Container (nicht der Server) die Deaktivierung.
void CVollOleAppView::OnCancelEditSrvr()
{
	GetDocument()->OnDeactivateUI(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppView Diagnose

#ifdef _DEBUG
void CVollOleAppView::AssertValid() const
{
	CView::AssertValid();
}

void CVollOleAppView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVollOleAppDoc* CVollOleAppView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVollOleAppDoc)));
	return (CVollOleAppDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppView Nachrichten-Handler
