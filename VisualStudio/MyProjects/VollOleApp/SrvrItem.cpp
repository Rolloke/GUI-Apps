// SrvrItem.cpp : Implementierung der Klasse CVollOleAppSrvrItem
//

#include "stdafx.h"
#include "VollOleApp.h"

#include "VollOleAppDoc.h"
#include "SrvrItem.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppSrvrItem Implementierung

IMPLEMENT_DYNAMIC(CVollOleAppSrvrItem, CDocObjectServerItem)

CVollOleAppSrvrItem::CVollOleAppSrvrItem(CVollOleAppDoc* pContainerDoc)
	: CDocObjectServerItem(pContainerDoc, TRUE)
{
	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen
	//  (z.B. Hinzufügen von zusätzlichen Zwischenablageformaten zur Datenquelle des Elements)
}

CVollOleAppSrvrItem::~CVollOleAppSrvrItem()
{
	// ZU ERLEDIGEN: Hier Bereinigungscode hinzufügen
}

void CVollOleAppSrvrItem::Serialize(CArchive& ar)
{
	// CVollOleAppSrvrItem::Serialize wird automatisch aufgerufen, wenn
	//  das Element in die Zwischenablage kopiert wird. Dies kann automatisch 
	//  über die OLE-Rückruffunktion OnGetClipboardData geschehen. Ein Standardwert für
	//  das eingebundene Element dient einfach zur Delegierung der Serialisierungsfunktion des 
	//  Dokuments. Wenn Sie Verweise unterstützen, möchten Sie vielleicht nur einen Teil des
	//  Dokuments serialisieren.

	if (!IsLinkedItem())
	{
		CVollOleAppDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->Serialize(ar);
	}
}

BOOL CVollOleAppSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
	// Die meisten Anwendungen (wie auch diese) unterstützen als einzigen Aspekt das Zeichnen
	//  des Elementinhalts. Wollen Sie andere Aspekte unterstützen, wie z.B.
	//  DVASPECT_THUMBNAIL (durch Überladen von OnDrawEx), so sollte diese
	//  Implementierung von OnGetExtent dahingehend modifiziert werden, dass sie
	//  zusätzliche Aspekte verarbeiten kann.

	if (dwDrawAspect != DVASPECT_CONTENT)
		return CDocObjectServerItem::OnGetExtent(dwDrawAspect, rSize);

	// CVollOleAppSrvrItem::OnGetExtent wird aufgerufen, um das Extent in 
	//  HIMETRIC-Einheiten des gesamten Elements zu ermitteln. Die Standardimplementierung
	//  liefert hier einfach eine fest programmierte Einheitenanzahl zurück.

	CVollOleAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// ZU ERLEDIGEN: Ersetzen Sie diese willkürlich gewählte Größe

	rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC Einheiten

	return TRUE;
}

BOOL CVollOleAppSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
	// Entfernen Sie dies, wenn Sie rSize verwenden
	UNREFERENCED_PARAMETER(rSize);

	CVollOleAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// ZU ERLEDIGEN: Setzen Sie Mapping-Modus und Extent
	//  (Das Extent stimmt üblicherweise mit der von OnGetExtent zurückgelieferten Größe überein)
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0,0);
	pDC->SetWindowExt(3000, 3000);

	// ZU ERLEDIGEN: Hier Code zum Zeichnen einfügen. Füllen Sie wahlweise das HIMETRIC-Extent aus.
	//  Alle Zeichenoperationen finden innerhalb des Metadatei-Gerätekontexts (pDC) statt.

	// ZU ERLEDIGEN: Auch eingebundene CVollOleAppCntrItem-Objekte zeichnen.

	// Der folgende Code zeichnet das erste Element an einer willkürlich gewählten Position.

	// ZU ERLEDIGEN: Entfernen Sie diesen Code, sobald Ihre richtige Zeichenroutine vollständig ist

	POSITION pos = pDoc->GetStartPosition();
	CVollOleAppCntrItem* pItem = (CVollOleAppCntrItem*)pDoc->GetNextClientItem(pos);
	if (pItem != NULL)
		pItem->Draw(pDC, CRect(10, 10, 1010, 1010));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppSrvrItem Diagnose

#ifdef _DEBUG
void CVollOleAppSrvrItem::AssertValid() const
{
	CDocObjectServerItem::AssertValid();
}

void CVollOleAppSrvrItem::Dump(CDumpContext& dc) const
{
	CDocObjectServerItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
