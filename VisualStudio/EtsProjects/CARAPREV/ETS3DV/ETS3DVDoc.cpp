// ETS3DVDoc.cpp : Implementierung der Klasse CETS3DVDoc
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
// CETS3DVDoc

IMPLEMENT_DYNCREATE(CETS3DVDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CETS3DVDoc, COleServerDoc)
	//{{AFX_MSG_MAP(CETS3DVDoc)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CETS3DVDoc Konstruktion/Destruktion

CETS3DVDoc::CETS3DVDoc()
{
	// Verwenden Sie OLE-Compound-Dateien
	EnableCompoundFile();

	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen

}

CETS3DVDoc::~CETS3DVDoc()
{
}

BOOL CETS3DVDoc::OnNewDocument()
{
	if (!COleServerDoc::OnNewDocument())
		return FALSE;

	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CETS3DVDoc Server-Implementierung

COleServerItem* CETS3DVDoc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem wird automatisch aufgerufen, um COleServerItem zu erhalten,
	//  das mit dem Dokument verknüpft ist. Die Funktion wird nur bei Bedarf aufgerufen.

	CETS3DVSrvrItem* pItem = new CETS3DVSrvrItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}

/////////////////////////////////////////////////////////////////////////////
// CETS3DVDoc Implementierung des aktiven Dokument-Servers

CDocObjectServer *CETS3DVDoc::GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite)
{
	return new CDocObjectServer(this, pDocSite);
}



/////////////////////////////////////////////////////////////////////////////
// CETS3DVDoc Serialisierung

void CETS3DVDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// ZU ERLEDIGEN: Hier Code zum Speichern einfügen
	}
	else
	{
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
	}
}

/////////////////////////////////////////////////////////////////////////////
// CETS3DVDoc Diagnose

#ifdef _DEBUG
void CETS3DVDoc::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void CETS3DVDoc::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CETS3DVDoc Befehle
