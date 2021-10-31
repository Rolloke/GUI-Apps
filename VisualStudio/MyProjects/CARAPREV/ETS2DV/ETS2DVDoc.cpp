// ETS2DVDoc.cpp : Implementierung der Klasse CETS2DVDoc
//

#include "stdafx.h"
#include "ETS2DV.h"

#include "ETS2DVDoc.h"
#include "SrvrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CETS2DVDoc

IMPLEMENT_DYNCREATE(CETS2DVDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CETS2DVDoc, COleServerDoc)
	//{{AFX_MSG_MAP(CETS2DVDoc)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CETS2DVDoc, COleServerDoc)
	//{{AFX_DISPATCH_MAP(CETS2DVDoc)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//      Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Hinweis: Wir stellen Unterstützung für IID_IETS2DV zur Verfügung, um typsicheres Binden 
//  von VBA zu unterstützen. Diese IID muss mit der GUID übereinstimmen, die bei der 
//  Disp-Schnittstelle in der .ODL-Datei angegeben ist.

// {E217CE62-3215-11D4-B6EC-0000B458D891}
static const IID IID_IETS2DV =
{ 0xe217ce62, 0x3215, 0x11d4, { 0xb6, 0xec, 0x0, 0x0, 0xb4, 0x58, 0xd8, 0x91 } };

BEGIN_INTERFACE_MAP(CETS2DVDoc, COleServerDoc)
	INTERFACE_PART(CETS2DVDoc, IID_IETS2DV, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CETS2DVDoc Konstruktion/Destruktion

CETS2DVDoc::CETS2DVDoc()
{
	// Verwenden Sie OLE-Compound-Dateien
	EnableCompoundFile();

	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen

	EnableAutomation();

	AfxOleLockApp();
}

CETS2DVDoc::~CETS2DVDoc()
{
	AfxOleUnlockApp();
}

BOOL CETS2DVDoc::OnNewDocument()
{
	if (!COleServerDoc::OnNewDocument())
		return FALSE;

	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CETS2DVDoc Server-Implementierung

COleServerItem* CETS2DVDoc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem wird automatisch aufgerufen, um COleServerItem zu erhalten,
	//  das mit dem Dokument verknüpft ist. Die Funktion wird nur bei Bedarf aufgerufen.

	CETS2DVSrvrItem* pItem = new CETS2DVSrvrItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}



/////////////////////////////////////////////////////////////////////////////
// CETS2DVDoc Serialisierung

void CETS2DVDoc::Serialize(CArchive& ar)
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
// CETS2DVDoc Diagnose

#ifdef _DEBUG
void CETS2DVDoc::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void CETS2DVDoc::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CETS2DVDoc Befehle
