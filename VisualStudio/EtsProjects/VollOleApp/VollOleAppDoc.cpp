// VollOleAppDoc.cpp : Implementierung der Klasse CVollOleAppDoc
//

#include "stdafx.h"
#include "VollOleApp.h"

#include "VollOleAppDoc.h"
#include "CntrItem.h"
#include "SrvrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppDoc

IMPLEMENT_DYNCREATE(CVollOleAppDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CVollOleAppDoc, COleServerDoc)
	//{{AFX_MSG_MAP(CVollOleAppDoc)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
	// Zulassen der Standardimplementierung für OLE-Container
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleServerDoc::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleServerDoc::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleServerDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, COleServerDoc::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleServerDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleServerDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, COleServerDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CVollOleAppDoc, COleServerDoc)
	//{{AFX_DISPATCH_MAP(CVollOleAppDoc)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//      Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Hinweis: Wir stellen Unterstützung für IID_IVollOleApp zur Verfügung, um typsicheres Binden 
//  von VBA zu unterstützen. Diese IID muss mit der GUID übereinstimmen, die bei der 
//  Disp-Schnittstelle in der .ODL-Datei angegeben ist.

// {E1E8E842-3E20-11D4-B6EC-0000B458D891}
static const IID IID_IVollOleApp =
{ 0xe1e8e842, 0x3e20, 0x11d4, { 0xb6, 0xec, 0x0, 0x0, 0xb4, 0x58, 0xd8, 0x91 } };

BEGIN_INTERFACE_MAP(CVollOleAppDoc, COleServerDoc)
	INTERFACE_PART(CVollOleAppDoc, IID_IVollOleApp, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppDoc Konstruktion/Destruktion

CVollOleAppDoc::CVollOleAppDoc()
{
	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen

	EnableAutomation();

	AfxOleLockApp();
}

CVollOleAppDoc::~CVollOleAppDoc()
{
	AfxOleUnlockApp();
}

BOOL CVollOleAppDoc::OnNewDocument()
{
	if (!COleServerDoc::OnNewDocument())
		return FALSE;

	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppDoc Server-Implementierung

COleServerItem* CVollOleAppDoc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem wird automatisch aufgerufen, um COleServerItem zu erhalten,
	//  das mit dem Dokument verknüpft ist. Die Funktion wird nur bei Bedarf aufgerufen.

	CVollOleAppSrvrItem* pItem = new CVollOleAppSrvrItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppDoc Implementierung des aktiven Dokument-Servers

CDocObjectServer *CVollOleAppDoc::GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite)
{
	return new CDocObjectServer(this, pDocSite);
}



/////////////////////////////////////////////////////////////////////////////
// CVollOleAppDoc Serialisierung

void CVollOleAppDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// ZU ERLEDIGEN: Hier Code zum Speichern einfügen
	}
	else
	{
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
	}

	// Ein Aufruf der Basisklasse COleServerDoc ermöglicht die Serialisierung
	//  des Objekts COleClientItem des Container-Dokuments.
	COleServerDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppDoc Diagnose

#ifdef _DEBUG
void CVollOleAppDoc::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void CVollOleAppDoc::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppDoc Befehle
