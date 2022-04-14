// SrvrItem.cpp : Implementierung der Klasse COleServerSrvrItem
//

#include "stdafx.h"
#ifdef ETS_OLE_SERVER

#include "CARA2DV.h"
#include "CARA2DVDoc.h"
#include "CARA2DVView.h"
#include "SrvrItem.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COleServerSrvrItem Implementierung



IMPLEMENT_DYNAMIC(COleServerSrvrItem, COleServerItem)

COleServerSrvrItem::COleServerSrvrItem(CCARA2DVDoc* pContainerDoc)
	: COleServerItem(pContainerDoc, TRUE)
{
   m_sizeExtent = CSize(3000, 3000);
	m_strItemName.LoadString(AFX_IDS_APP_TITLE);
}

COleServerSrvrItem::~COleServerSrvrItem()
{
	// ZU ERLEDIGEN: Hier Bereinigungscode hinzufügen
}

void COleServerSrvrItem::Serialize(CArchive& ar)
{
   BEGIN("COleServerSrvrItem::Serialize");
	// COleServerSrvrItem::Serialize wird automatisch aufgerufen, wenn
	//  das Element in die Zwischenablage kopiert wird. Dies kann automatisch 
	//  über die OLE-Rückruffunktion OnGetClipboardData geschehen. Ein Standardwert für
	//  das eingebundene Element dient einfach zur Delegierung der Serialisierungsfunktion des 
	//  Dokuments. Wenn Sie Verweise unterstützen, möchten Sie vielleicht nur einen Teil des
	//  Dokuments serialisieren.

	if (!IsLinkedItem())
	{
		CCARA2DVDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
//      pDoc->Serialize(ar);
      if (pDoc->m_bOleChanged)
      {
         pDoc->GetLabelContainer()->ResetStates();
         CCARA2DVView*pView = (CCARA2DVView*) pDoc->GetCaraView();
         pView->ResetPickStatesQuiet();
         pDoc->UpdateAllItems(NULL);
         pDoc->m_bOleChanged = false;
      }

      CLabelContainer  *pplots = pDoc->GetLabelContainer();
      pplots->Serialize(ar);
	}
}

BOOL COleServerSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
   BEGIN("COleServerSrvrItem::OnGetExtent");
	// Die meisten Anwendungen (wie auch diese) unterstützen als einzigen Aspekt das Zeichnen
	//  des Elementinhalts. Wollen Sie andere Aspekte unterstützen, wie z.B.
	//  DVASPECT_THUMBNAIL (durch Überladen von OnDrawEx), so sollte diese
	//  Implementierung von OnGetExtent dahingehend modifiziert werden, dass sie
	//  zusätzliche Aspekte verarbeiten kann.

	if (dwDrawAspect != DVASPECT_CONTENT)
   {
      BOOL bReturn = COleServerItem::OnGetExtent(dwDrawAspect, rSize);
   }

	// COleServerSrvrItem::OnGetExtent wird aufgerufen, um das Extent in 
	//  HIMETRIC-Einheiten des gesamten Elements zu ermitteln. Die Standardimplementierung
	//  liefert hier einfach eine fest programmierte Einheitenanzahl zurück.
   rSize.cx = MulDiv(m_sizeExtent.cx, 6125, 7219);
   rSize.cy = MulDiv(m_sizeExtent.cy, 6125, 7219);
   REPORT("DVASPECT_CONTENT %d, %d", rSize.cx, rSize.cy);

	return TRUE;
}

BOOL COleServerSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
   BEGIN("COleServerSrvrItem::OnDraw");
	UNREFERENCED_PARAMETER(rSize);

	CCARA2DVDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   CLabelContainer  *pplots = pDoc->GetLabelContainer();

	pDC->SetMapMode(MM_LOMETRIC);
   rSize = CSize(0,0);
   pDC->SaveDC();
   pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), pDC, COleServerSrvrItem::DrawLabel);
   pDC->RestoreDC(-1);
	return TRUE;
}

BOOL COleServerSrvrItem::OnDrawEx( CDC* pDC, DVASPECT nDrawAspect, CSize& rSize)
{
   BEGIN("COleServerSrvrItem::OnDrawEx");
   REPORT("%d", nDrawAspect);
   if (nDrawAspect == DVASPECT_DOCPRINT)
   {
      return OnDraw(pDC, rSize);
   }
   else return COleServerItem::OnDrawEx(pDC, nDrawAspect, rSize);
}

int COleServerSrvrItem::DrawLabel(CLabel *pl, void *p)
{
   CDC* pDC = (CDC*)p;
   pl->Draw(pDC);
   return 0;
}

BOOL COleServerSrvrItem::OnQueryUpdateItems( )
{
   BEGIN("COleServerSrvrItem::OnQueryUpdateItems");
   return COleServerItem::OnQueryUpdateItems();
}

void COleServerSrvrItem::OnUpdateItems( )
{
   BEGIN("COleServerSrvrItem::OnUpdateItems");
   COleServerItem::OnUpdateItems();
}


/////////////////////////////////////////////////////////////////////////////
// COleServerSrvrItem Diagnose

#ifdef _DEBUG
void COleServerSrvrItem::AssertValid() const
{
	COleServerItem::AssertValid();
}

void COleServerSrvrItem::Dump(CDumpContext& dc) const
{
	COleServerItem::Dump(dc);
}
#endif

#endif // ETS_OLE_SERVER

/////////////////////////////////////////////////////////////////////////////
