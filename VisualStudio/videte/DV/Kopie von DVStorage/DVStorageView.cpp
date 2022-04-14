// DVStorageView.cpp : implementation of the CDVStorageView class
//

#include "stdafx.h"
#include "DVStorage.h"

#include "DVStorageDoc.h"
#include "DVStorageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDVStorageView

IMPLEMENT_DYNCREATE(CDVStorageView, CView)

BEGIN_MESSAGE_MAP(CDVStorageView, CView)
	//{{AFX_MSG_MAP(CDVStorageView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVStorageView construction/destruction

CDVStorageView::CDVStorageView()
{
	// add construction code here
	m_stStart.GetLocalTime();

}

CDVStorageView::~CDVStorageView()
{
}

BOOL CDVStorageView::PreCreateWindow(CREATESTRUCT& cs)
{
	//  Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDVStorageView drawing

void CDVStorageView::OnDraw(CDC* pDC)
{
	CFont font;
	CFont* pOldFont;

	font.CreatePointFont(10*10,_T("Courier New"),pDC);

	pOldFont = pDC->SelectObject(&font);

	if (theApp.IsResetting())
	{
		if (theApp.IsFastResetting())
		{
			pDC->TextOut(5,5,_T("fast resetting"));
		}
		else
		{
			pDC->TextOut(5,5,_T("resetting"));
		}
	}
	else
	{
		CSize s;
		int h = 5;
		s = pDC->TabbedTextOut(5,h,_T("STORING"),0,NULL,0);
		h += s.cy;
		if (theApp.GetNoFrag())
		{
			s = pDC->TabbedTextOut(5,h,_T("NO FRAG"),0,NULL,0);
			h += s.cy;
		}

#ifdef _DEBUG
		CAutoCritSec acs(&theApp.m_Archives.m_cs);
		CString f;
		f = _T("Start: ") + m_stStart.GetDateTime();
		s = pDC->TabbedTextOut(5,h,f,0,NULL,0);
		h += s.cy + 5;

		for (int i=0;i<theApp.m_Archives.GetSize();i++)
		{
			f = theApp.m_Archives.GetAt(i)->Format();
			s = pDC->GetOutputTextExtent(f);
			pDC->TextOut(5,h,f);
			h += s.cy + 5;
		}
		acs.Unlock();
		CIPCDrive* pDrive;
		CAutoCritSec acsD(&theApp.m_Drives.m_cs);
		for (i=0;i<theApp.m_Drives.GetSize();i++)
		{
			pDrive = theApp.m_Drives.GetAt(i);
			if (   pDrive
				&& (   pDrive->IsDatabase()
				    || pDrive->IsWriteBackup()
					|| pDrive->IsReadBackup()
				   )
				)
			{
				f = theApp.m_Drives.GetAt(i)->Format();
				s = pDC->TabbedTextOut(5,h,f,0,NULL,0);
				h += s.cy + 5;
			}
		}
		acsD.Unlock();
#endif
	}

	pDC->SelectObject(pOldFont);

	font.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CDVStorageView diagnostics

#ifdef _DEBUG
void CDVStorageView::AssertValid() const
{
	CView::AssertValid();
}

void CDVStorageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDVStorageDoc* CDVStorageView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDVStorageDoc)));
	return (CDVStorageDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDVStorageView message handlers
