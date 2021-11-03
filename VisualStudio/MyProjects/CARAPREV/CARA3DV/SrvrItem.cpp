// SrvrItem.cpp : Implementierung der Klasse COleServerSrvrItem
//

#include "stdafx.h"
#ifdef ETS_OLE_SERVER

#include "CARA3DV.h"
#include "CARA3DVDoc.h"
#include "SrvrItem.h"
#include "IpFrame.h"
#include "CARA3DVView.h"

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

COleServerSrvrItem::COleServerSrvrItem(CCARA3DVDoc* pContainerDoc)
	: COleServerItem(pContainerDoc, TRUE)
{
   BEGIN("COleServerSrvrItem::COleServerSrvrItem");
   m_sizeExtent = CSize(3000, 3000);
	m_strItemName.LoadString(AFX_IDS_APP_TITLE);
}

COleServerSrvrItem::~COleServerSrvrItem()
{
   BEGIN("COleServerSrvrItem::~COleServerSrvrItem");
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
		CCARA3DVDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
      pDoc->Serialize(ar);
	}
}

BOOL COleServerSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
   BEGIN("COleServerSrvrItem::OnGetExtent");

	if (dwDrawAspect != DVASPECT_CONTENT)
   {
      BOOL bReturn = COleServerItem::OnGetExtent(dwDrawAspect, rSize);
      REPORT("DVASPECT_CONTENT %d, %d", rSize.cx, rSize.cy);
   }

   rSize = m_sizeExtent;

	return TRUE;
}

BOOL COleServerSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
   BEGIN("COleServerSrvrItem::OnDraw");
   ETS3DGL_Draw draw;
   CCARA3DVApp *pApp = (CCARA3DVApp*)AfxGetApp();
   CCARA3DVView *pC3dv = NULL;
	CCARA3DVDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   if (!pDoc->m_p3DObject)
      return true;

   ZeroMemory(&draw, sizeof(ETS3DGL_Draw));
   if (pApp->m_pActiveWnd && pApp->m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CInPlaceFrame)))
   {
      CInPlaceFrame *pIPFrame = (CInPlaceFrame*)pApp->m_pActiveWnd;
      CView *pView = pIPFrame->GetActiveView();
      if (pView && pView->IsWindowVisible() && pView->IsKindOf(RUNTIME_CLASS(CCARA3DVView)))
      {
         pC3dv = (CCARA3DVView*) pView;
	      pDC->SetMapMode(MM_HIMETRIC);
         rSize = CSize(0,0);
         pDC->SaveDC();
         draw.hDC = pDC->m_hDC;
         CSize size(m_sizeExtent);
         draw.rcFrame.right  = m_sizeExtent.cx;
         draw.rcFrame.bottom = -m_sizeExtent.cy;
         LRESULT lResult = pC3dv->SendMsgToGLView(WM_COPY_FRAME_BUFFER, COPY_TO_HDC, (LPARAM)&draw);
         pDC->RestoreDC(-1);
      }
   }
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
