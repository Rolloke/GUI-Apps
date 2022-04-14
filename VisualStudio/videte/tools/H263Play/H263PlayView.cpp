// H263PlayView.cpp : implementation of the CH263PlayView class
//

#include "stdafx.h"
#include "H263Play.h"

#include "H263PlayDoc.h"
#include "H263PlayView.h"

#include "cdjpeg\JpegEncoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CH263PlayView

IMPLEMENT_DYNCREATE(CH263PlayView, CView)

BEGIN_MESSAGE_MAP(CH263PlayView, CView)
	//{{AFX_MSG_MAP(CH263PlayView)
	ON_COMMAND(ID_PLAY, OnPlay)
	ON_WM_DESTROY()
	ON_COMMAND(ID_DRAWFRAME, OnDrawframe)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_H263_FIT, OnH263Fit)
	ON_UPDATE_COMMAND_UI(ID_H263_FIT, OnUpdateH263Fit)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_DECODE_VIDEO, OnDecode)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CH263PlayView construction/destruction

CH263PlayView::CH263PlayView()
{
	// TODO: add construction code here
}

CH263PlayView::~CH263PlayView()
{
}

BOOL CH263PlayView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CH263PlayView drawing

void CH263PlayView::OnDraw(CDC* pDC)
{
	CH263PlayDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	CRect rect;
	GetClientRect(rect);
	m_H263.OnDraw(pDC,rect);
}

/////////////////////////////////////////////////////////////////////////////
// CH263PlayView printing

BOOL CH263PlayView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CH263PlayView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CH263PlayView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CH263PlayView diagnostics

#ifdef _DEBUG
void CH263PlayView::AssertValid() const
{
	CView::AssertValid();
}

void CH263PlayView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CH263PlayDoc* CH263PlayView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CH263PlayDoc)));
	return (CH263PlayDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
void CH263PlayView::OnPlay() 
{
	CH263PlayDoc* pDoc = GetDocument();

	if (pDoc->GetBuffer())
	{
		m_H263.Decode(pDoc->GetBuffer(),pDoc->GetBufferLen());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CH263PlayView::OnDecode() 
{
	Invalidate(FALSE);
	UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CH263PlayView::OnDestroy() 
{
	CView::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CH263PlayView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	m_H263.SetCaps(H263_SINGLE_TREAD,TRUE);
	m_H263.SetCaps(H263_FPS,TRUE);
	m_H263.Init(this,WM_COMMAND,ID_DECODE_VIDEO,0);

	CSize s(352,288);
	CRect vr(0,0,s.cx,s.cy);
	CRect mr;
	GetParent()->GetClientRect(mr);
	mr.right = mr.left + s.cx;
	mr.bottom = mr.top + s.cy;
	GetParent()->MoveWindow(mr,TRUE);
	MoveWindow(vr,TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CH263PlayView::OnDrawframe() 
{
	CRect rect;
	GetClientRect(rect);
	InvalidateRect(rect,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CH263PlayView::OnFileSaveAs() 
{
	CString sFilter;
	sFilter.LoadString(IDS_SAVE_FILTER);

	CFileDialog dlg(FALSE,"bmp",NULL,OFN_HIDEREADONLY,sFilter,this);

	if (dlg.DoModal())
	{
		CString sExt = dlg.GetFileExt();
		if (0==sExt.CompareNoCase("bmp"))
		{
			m_H263.SaveAsBMP(dlg.GetPathName());
		}
		else if (0==sExt.CompareNoCase("jpg"))
		{
			char szTempPathName[_MAX_PATH];
			char szTempFileName[_MAX_PATH];
			GetTempPath(_MAX_PATH,szTempPathName);
			GetTempFileName(szTempPathName,"263",0,szTempFileName);
			m_H263.SaveAsBMP(szTempFileName);
			CJpegEncoder e;
			e.SetSource(szTempFileName);
			e.SetDestination(dlg.GetPathName());
			e.Encode();
			DeleteFile(szTempFileName);
		}
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CH263PlayView::OnFileSave() 
{
	OnFileSaveAs();
}
/////////////////////////////////////////////////////////////////////////////
void CH263PlayView::OnH263Fit() 
{
//	m_H263.SetFit(!m_H263.GetFit());
}
/////////////////////////////////////////////////////////////////////////////
void CH263PlayView::OnUpdateH263Fit(CCmdUI* pCmdUI) 
{
//	pCmdUI->SetCheck(m_H263.GetFit());
}
