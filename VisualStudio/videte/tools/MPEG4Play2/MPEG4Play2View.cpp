// MPEG4Play2View.cpp : Implementierung der Klasse CMPEG4Play2View
//

#include "stdafx.h"
#include "MPEG4Play2.h"

#include "MPEG4Play2Doc.h"
#include "MPEG4Play2View.h"

#include "PanelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMPEG4Play2View

IMPLEMENT_DYNCREATE(CMPEG4Play2View, CView)

BEGIN_MESSAGE_MAP(CMPEG4Play2View, CView)
	// Standarddruckbefehle
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMPEG4Play2View Erstellung/Zerstörung

CMPEG4Play2View::CMPEG4Play2View()
{
}

CMPEG4Play2View::~CMPEG4Play2View()
{
}

BOOL CMPEG4Play2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CView::PreCreateWindow(cs);
}

// CMPEG4Play2View-Zeichnung
void CMPEG4Play2View::OnDraw(CDC* pDC)
{
	CMPEG4Play2Doc* pDoc = GetDocument();
	//CMPEG4Play2Doc* pDoc = (CMPEG4Play2Doc*)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveDocument();
	//ASSERT_VALID(pDoc);

	if(pDoc->GetMPEG4Frame())
	{
		if(pDC)
		{
			CRect rect, r2;
			CString strFrameType;
			CString strRect;
			
			GetClientRect(rect);

			if(pDoc->IsIFrame())
				strFrameType = _T("I-Frame");
			else
				strFrameType = _T("P-Frame");

			strRect.Format(_T("%dx%d|"), rect.Width(), rect.Height());

			//pDoc->GetMPEG4Decoder()->SetOSDText1(0, 0, strRect+strFrameType);
			pDoc->GetMPEG4Decoder()->OnDraw(pDC, rect, r2);
		}
	}
	else
	{
		////CPaintDC dc( this ); // Device context for painting

		//CBitmap *poldbmp;
		//CDC memdc;

		//BITMAP bitmap;

		//m_bmpLogo.GetBitmap(&bitmap);

		//// Load the bitmap resource
		////bmp.LoadBitmap( IDB_CORPLOGO );

		//// Create a compatible memory DC
		////memdc.CreateCompatibleDC( &dc );
		//memdc.CreateCompatibleDC( pDC );

		//// Select the bitmap into the DC
		//poldbmp = memdc.SelectObject( &m_bmpLogo );

		//// Copy (BitBlt) bitmap from memory DC to screen DC
		////pDC->BitBlt( 0, 0, 472, 358, &memdc, 0, 0, SRCCOPY );
		//pDC->BitBlt( 0,
		//	         0,
		//			 bitmap.bmWidth,
		//			 bitmap.bmHeight,
		//			 &memdc,
		//			 0,
		//			 0,
		//			 SRCCOPY );

		//TRACE(_T("%d   %d\n"),m_bmpLogo.GetBitmapDimension().cx, m_bmpLogo.GetBitmapDimension().cx);

		//memdc.SelectObject( poldbmp );
	}
}



// CMPEG4Play2View drucken

BOOL CMPEG4Play2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CMPEG4Play2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}

void CMPEG4Play2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Bereinigung nach dem Drucken einfügen
}


// CMPEG4Play2View Diagnose

#ifdef _DEBUG
void CMPEG4Play2View::AssertValid() const
{
	CView::AssertValid();
}

void CMPEG4Play2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMPEG4Play2Doc* CMPEG4Play2View::GetDocument() const // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMPEG4Play2Doc)));
	return (CMPEG4Play2Doc*)m_pDocument;
}
#endif //_DEBUG


// CMPEG4Play2View Meldungshandler

void CMPEG4Play2View::OnInitialUpdate(void)
{
	CSize size;

	if(GetDocument()->GetFile())
	{
		size = GetDocument()->GetMPEG4Decoder()->GetImageDims();

		// Deutet das Seitenverhältnis auf ein Halbbild hin?
		if ((float)size.cx / (float)size.cy > 2.0)
		{
			size.cy *= 2;
		}
	}
	else
	{
		//BITMAP bitmap;

		//m_bmpLogo.GetBitmap(&bitmap);

		//nWidth  = bitmap.bmWidth;
		//nHeight = bitmap.bmHeight;

		size.cx  = 200;
		size.cy = 100;
	}

	CWnd* pMainFrame = AfxGetMainWnd();
	if (pMainFrame)
	{
		CRect rcClient(0,0,size.cx, size.cy);
		//AdjustWindowRectEx(&rcClient, WS_THICKFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU, TRUE, 0);
		AdjustWindowRectEx(&rcClient, WS_THICKFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU, FALSE, 0);
		pMainFrame->SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOMOVE);
		//pMainFrame->SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height(), SWP_SHOWWINDOW);
	}
}
