// MenuToolTipDoc.cpp : Implementierung der Klasse CMenuToolTipDoc
//

#include "stdafx.h"
#include "MenuToolTip.h"

#include "MenuToolTipDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMenuToolTipDoc

IMPLEMENT_DYNCREATE(CMenuToolTipDoc, CDocument)

BEGIN_MESSAGE_MAP(CMenuToolTipDoc, CDocument)
END_MESSAGE_MAP()


// CMenuToolTipDoc Erstellung/Zerstörung

CMenuToolTipDoc::CMenuToolTipDoc()
{
	// TODO: Hier Code für One-Time-Konstruktion einfügen

}

CMenuToolTipDoc::~CMenuToolTipDoc()
{
}

BOOL CMenuToolTipDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}




// CMenuToolTipDoc Serialisierung

void CMenuToolTipDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: Hier Code zum Speichern einfügen
	}
	else
	{
		// TODO: Hier Code zum Laden einfügen
	}
}


// CMenuToolTipDoc Diagnose

#ifdef _DEBUG
void CMenuToolTipDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMenuToolTipDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMenuToolTipDoc-Befehle
