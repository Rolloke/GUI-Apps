// MPEG4PlayView.cpp : implementation of the CMPEG4PlayView class
//

#include "stdafx.h"
#include "MPEG4Play.h"

#include "MPEG4PlayDoc.h"
#include "MPEG4PlayView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayView

IMPLEMENT_DYNCREATE(CMPEG4PlayView, CView)

BEGIN_MESSAGE_MAP(CMPEG4PlayView, CView)
	//{{AFX_MSG_MAP(CMPEG4PlayView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_DECODE_VIDEO, OnDecode)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayView construction/destruction

CMPEG4PlayView::CMPEG4PlayView()
{
	// TODO: add construction code here
}

CMPEG4PlayView::~CMPEG4PlayView()
{
}

BOOL CMPEG4PlayView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayView drawing

void CMPEG4PlayView::OnDraw(CDC* pDC)
{
	CMPEG4PlayDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here

	// MPEG4-File dekodieren und abspielen
	if(pDoc->GetMPEG4Decoder() != NULL)
	{
		CRect rect;
		GetClientRect(rect);

		pDoc->GetMPEG4Decoder()->OnDraw(pDC, rect);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayView diagnostics

#ifdef _DEBUG
void CMPEG4PlayView::AssertValid() const
{
	CView::AssertValid();
}

void CMPEG4PlayView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMPEG4PlayDoc* CMPEG4PlayView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMPEG4PlayDoc)));
	return (CMPEG4PlayDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayView message handlers

void CMPEG4PlayView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	// MPEG4Decoder initialisieren
	if(GetDocument()->GetMPEG4Decoder())
	{
		if(!GetDocument()->GetMPEG4Decoder()->Init(this,WM_COMMAND,ID_DECODE_VIDEO,0))
		{
			TRACE(_T("MPEG4Decoder-Init() fehlgeschlagen\n"));
		}
		else
		{
			int   nWidth  = GetDocument()->GetMPEG4Decoder()->GetStreamInfo().width;
			int   nHeight = GetDocument()->GetMPEG4Decoder()->GetStreamInfo().height;
			CWnd* pMainFrame = AfxGetMainWnd();
			if (pMainFrame)
			{
				CRect rcClient(0,0,nWidth, nHeight);
				AdjustWindowRectEx(&rcClient, WS_THICKFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU, TRUE, 0);		
				pMainFrame->SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOMOVE);
			}
		}
	}

	// MPEG4Encoder initialisieren
	if(GetDocument()->GetMPEG4Encoder())
	{
		if(!GetDocument()->GetMPEG4Encoder()->Init(0,0,0))
		{
			TRACE(_T("MPEG4Encoder-Init() fehlgeschlagen\n"));
		}
//		else
//		{
//			MoveWindow(0,
//                       0,
//                       GetDocument()->GetMPEG4Encoder()->GetStreamInfo().width+4,
//                       GetDocument()->GetMPEG4Encoder()->GetStreamInfo().height+4);
//		}
	}
}


/////////////////////////////////////////////////////////////////////////////
void CMPEG4PlayView::OnDecode() 
{
	Invalidate(FALSE);
	UpdateWindow();
}

