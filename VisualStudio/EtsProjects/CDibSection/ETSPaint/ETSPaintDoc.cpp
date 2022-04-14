// ETSPaintDoc.cpp : Implementierung der Klasse CETSPaintDoc
//

#include "stdafx.h"
#include "ETSPaint.h"

#include "ETSPaintDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CETSPaintDoc

IMPLEMENT_DYNCREATE(CETSPaintDoc, CDocument)

BEGIN_MESSAGE_MAP(CETSPaintDoc, CDocument)
	//{{AFX_MSG_MAP(CETSPaintDoc)
	ON_COMMAND(ID_EDIT_CONVERT, OnEditConvert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CETSPaintDoc Konstruktion/Destruktion

CETSPaintDoc::CETSPaintDoc()
{
	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen
   m_Dibsection.LoadFromResource(AfxGetInstanceHandle(), IDB_BITMAP1);
}

CETSPaintDoc::~CETSPaintDoc()
{
}

BOOL CETSPaintDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CETSPaintDoc Serialisierung

void CETSPaintDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
      m_Dibsection.SaveInFile((HANDLE)ar.GetFile()->m_hFile);
	}
	else
	{
      m_Dibsection.LoadFromFile((HANDLE)ar.GetFile()->m_hFile);
      m_Dibsection.InitPalette();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CETSPaintDoc Diagnose

#ifdef _DEBUG
void CETSPaintDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CETSPaintDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CETSPaintDoc Befehle

void CETSPaintDoc::DeleteContents() 
{
   m_Dibsection.Destroy();
	CDocument::DeleteContents();
}

void CETSPaintDoc::OnEditConvert() 
{
   if (m_Dibsection.GetBitmapInfo())
   {
      int nConvert = 16;
      m_Dibsection.ConvertBitCount(nConvert);
   }
}
