// DVProcessView.cpp : implementation of the CDVProcessView class
//

#include "stdafx.h"
#include "DVProcess.h"

#include "DVProcessDoc.h"
#include "DVProcessView.h"

#include "Camera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDVProcessView

IMPLEMENT_DYNCREATE(CDVProcessView, CView)

BEGIN_MESSAGE_MAP(CDVProcessView, CView)
	//{{AFX_MSG_MAP(CDVProcessView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVProcessView construction/destruction

CDVProcessView::CDVProcessView()
{
	m_stStart.GetLocalTime();
}

CDVProcessView::~CDVProcessView()
{
}

BOOL CDVProcessView::PreCreateWindow(CREATESTRUCT& cs)
{
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDVProcessView drawing

void CDVProcessView::OnDraw(CDC* pDC)
{
	CFont font;
	CFont* pOldFont;

	font.CreatePointFont(8*10,_T("Arial"),pDC);

	pOldFont = pDC->SelectObject(&font);

	if (!theApp.IsResetting())
	{
		int o = 5;
		int x = 0;
		int y = 5;
		int y0 = 5;
		int i;
		CSize s;
		CString f;

		o += x + 10;
		y = 5;
		x = 0;

		f = _T("Start: ") + m_stStart.GetDateTime();
		s = pDC->GetOutputTextExtent(f);
		pDC->TextOut(o,y,f);
		y0 = y + s.cy + 5;

		f = CString(NameOfEnum(theApp.GetSettings().GetResolution()));
		f += _T(" , ");
		f += theApp.GetSettings().GetMPEG4() ? _T("MPEG-4") : _T("JPEG");
		f += _T(" , ");
		f += theApp.GetSettings().UseAVCodec() ? _T("AV Codec") : _T("Emuzed/Intel");
		s = pDC->GetOutputTextExtent(f);
		pDC->TextOut(o,y0,f);
		y0 += s.cy + 5;

		f = _T("Camera/Relais:");
		y = y0;
		s = pDC->GetOutputTextExtent(f);
		pDC->TextOut(o,y,f);
		y += s.cy + 5;
		if (s.cx > x)
		{
			x = s.cx;
		}
		CString g;
		for (i=0;i<theApp.GetOutputGroups().GetSize();i++)
		{
			COutputGroup* pGroup = theApp.GetOutputGroups().GetAtFast(i);
			for (int j=0; j<pGroup->GetSize();j++)
			{
				COutput* pOutput = pGroup->GetAtFast(j);
				if (pOutput->IsActive())
				{
					f = g + pOutput->Format();
					s = pDC->GetOutputTextExtent(f);
					pDC->TextOut(o,y,f);
					y += s.cy + 5;
					if (s.cx > x)
					{
						x = s.cx;
					}
				}
			}
		}

		o += x + 10;
		y = 5;
		x = 0;

		f = _T("Melder:");
		y = y0;
		s = pDC->GetOutputTextExtent(f);
		pDC->TextOut(o,y,f);
		y += s.cy + 5;
		if (s.cx > x)
		{
			x = s.cx;
		}
		for (i=0;i<theApp.GetInputGroups().GetSize();i++)
		{
			CInputGroup* pGroup = theApp.GetInputGroups().GetAtFast(i);
			for (int j=0;j<pGroup->GetSize();j++)
			{
				CInput* pInput = pGroup->GetAtFast(j);
				if (pInput->IsOn())
				{
					f = pInput->Format();
					s = pDC->GetOutputTextExtent(f);
					pDC->TextOut(o,y,f);
					y += s.cy + 5;
					if (s.cx > x)
					{
						x = s.cx;
					}
				}
			}
		}

		o += x + 10;
		y = 5;
		x = 0;

		f = _T("Audio:");
		y = y0;
		s = pDC->GetOutputTextExtent(f);
		pDC->TextOut(o,y,f);
		y += s.cy + 5;
		if (s.cx > x)
		{
			x = s.cx;
		}
		for (i=0;i<theApp.GetAudioGroups().GetSize();i++)
		{
			CIPCAudioServer* pGroup = theApp.GetAudioGroups().GetAtFast(i);
			for (int j=0;j<pGroup->GetSize();j++)
			{
				CMedia* pMedia = pGroup->GetAtFast(j);
				if (pMedia)
				{
					f = pMedia->Format();
					s = pDC->GetOutputTextExtent(f);
					pDC->TextOut(o,y,f);
					y += s.cy + 5;
					if (s.cx > x)
					{
						x = s.cx;
					}
				}
			}
		}	
	}
	else
	{
		pDC->TextOut(5,5,_T("resetting"));
	}


	pDC->SelectObject(pOldFont);

	font.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CDVProcessView diagnostics

#ifdef _DEBUG
void CDVProcessView::AssertValid() const
{
	CView::AssertValid();
}

void CDVProcessView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDVProcessDoc* CDVProcessView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDVProcessDoc)));
	return (CDVProcessDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDVProcessView message handlers
