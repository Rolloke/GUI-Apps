// SrvrItem.cpp : Implementierung der Klasse CETS3DVSrvrItem
//

#include "stdafx.h"
#include "ETS3DV.h"

#include "ETS3DVDoc.h"
#include "SrvrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CETS3DVSrvrItem Implementierung

IMPLEMENT_DYNAMIC(CETS3DVSrvrItem, CDocObjectServerItem)

CETS3DVSrvrItem::CETS3DVSrvrItem(CETS3DVDoc* pContainerDoc)
	: CDocObjectServerItem(pContainerDoc, TRUE)
{
	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen
	//  (z.B. Hinzufügen von zusätzlichen Zwischenablageformaten zur Datenquelle des Elements)
}

CETS3DVSrvrItem::~CETS3DVSrvrItem()
{
	// ZU ERLEDIGEN: Hier Bereinigungscode hinzufügen
}

void CETS3DVSrvrItem::Serialize(CArchive& ar)
{
	// CETS3DVSrvrItem::Serialize wird automatisch aufgerufen, wenn
	//  das Element in die Zwischenablage kopiert wird. Dies kann automatisch 
	//  über die OLE-Rückruffunktion OnGetClipboardData geschehen. Ein Standardwert für
	//  das eingebundene Element dient einfach zur Delegierung der Serialisierungsfunktion des 
	//  Dokuments. Wenn Sie Verweise unterstützen, möchten Sie vielleicht nur einen Teil des
	//  Dokuments serialisieren.

	if (!IsLinkedItem())
	{
		CETS3DVDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->Serialize(ar);
	}
}

BOOL CETS3DVSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
	// Die meisten Anwendungen (wie auch diese) unterstützen als einzigen Aspekt das Zeichnen
	//  des Elementinhalts. Wollen Sie andere Aspekte unterstützen, wie z.B.
	//  DVASPECT_THUMBNAIL (durch Überladen von OnDrawEx), so sollte diese
	//  Implementierung von OnGetExtent dahingehend modifiziert werden, dass sie
	//  zusätzliche Aspekte verarbeiten kann.

	if (dwDrawAspect != DVASPECT_CONTENT)
		return CDocObjectServerItem::OnGetExtent(dwDrawAspect, rSize);

	// CETS3DVSrvrItem::OnGetExtent wird aufgerufen, um das Extent in 
	//  HIMETRIC-Einheiten des gesamten Elements zu ermitteln. Die Standardimplementierung
	//  liefert hier einfach eine fest programmierte Einheitenanzahl zurück.

	CETS3DVDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// ZU ERLEDIGEN: Ersetzen Sie diese willkürlich gewählte Größe

	rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC Einheiten

	return TRUE;
}

BOOL CETS3DVSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
	// Entfernen Sie dies, wenn Sie rSize verwenden
	UNREFERENCED_PARAMETER(rSize);

	CETS3DVDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// ZU ERLEDIGEN: Setzen Sie Mapping-Modus und Extent
	//  (Das Extent stimmt üblicherweise mit der von OnGetExtent zurückgelieferten Größe überein)
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0,0);
	pDC->SetWindowExt(3000, 3000);

	// ZU ERLEDIGEN: Hier Code zum Zeichnen einfügen. Füllen Sie wahlweise das HIMETRIC-Extent aus.
	//  Alle Zeichenoperationen finden innerhalb des Metadatei-Gerätekontexts (pDC) statt.

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CETS3DVSrvrItem Diagnose

#ifdef _DEBUG
void CETS3DVSrvrItem::AssertValid() const
{
	CDocObjectServerItem::AssertValid();
}

void CETS3DVSrvrItem::Dump(CDumpContext& dc) const
{
	CDocObjectServerItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
