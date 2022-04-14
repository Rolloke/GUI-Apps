// PerformanceLoggerDoc.cpp : Implementierung der Klasse CPerformanceLoggerDoc
//

#include "stdafx.h"
#include "PerformanceLogger.h"

#include "PerformanceLoggerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPerformanceLoggerDoc

IMPLEMENT_DYNCREATE(CPerformanceLoggerDoc, CDocument)

BEGIN_MESSAGE_MAP(CPerformanceLoggerDoc, CDocument)
END_MESSAGE_MAP()


// CPerformanceLoggerDoc Erstellung/Zerstörung

CPerformanceLoggerDoc::CPerformanceLoggerDoc()
{
	// TODO: Hier Code für One-Time-Konstruktion einfügen

}

CPerformanceLoggerDoc::~CPerformanceLoggerDoc()
{
}

BOOL CPerformanceLoggerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}




// CPerformanceLoggerDoc Serialisierung

void CPerformanceLoggerDoc::Serialize(CArchive& ar)
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


// CPerformanceLoggerDoc Diagnose

#ifdef _DEBUG
void CPerformanceLoggerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPerformanceLoggerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPerformanceLoggerDoc-Befehle
