// HtmlViewView.cpp : implementation of the CHtmlViewView class
//

#include "stdafx.h"
//#include "HtmlView.h"
#include "LogView.h"

#include "HtmlViewDoc.h"
#include "HtmlViewView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewView

IMPLEMENT_DYNCREATE(CHtmlViewView, CHtmlView)

BEGIN_MESSAGE_MAP(CHtmlViewView, CHtmlView)
	//{{AFX_MSG_MAP(CHtmlViewView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CHtmlView::OnFilePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CHtmlViewView::CHtmlViewView()
{

}

/////////////////////////////////////////////////////////////////////////////
CHtmlViewView::~CHtmlViewView()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHtmlViewView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CHtmlView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
void CHtmlViewView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
void CHtmlViewView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	CHtmlViewDoc* pDoc = GetDocument();				 
	ASSERT_VALID(pDoc);

	Navigate2(pDoc->GetPathName(),NULL,NULL);}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewView diagnostics
#ifdef _DEBUG
void CHtmlViewView::AssertValid() const
{
	CHtmlView::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CHtmlViewView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

/////////////////////////////////////////////////////////////////////////////
CHtmlViewDoc* CHtmlViewView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHtmlViewDoc)));
	return (CHtmlViewDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewView message handlers

/////////////////////////////////////////////////////////////////////////////
void CHtmlViewView::OnTitleChange(LPCTSTR lpszText) 
{
	CHtmlViewDoc* pDoc = GetDocument();				 
	ASSERT_VALID(pDoc);
	
	pDoc->SetTitle(lpszText);

	CHtmlView::OnTitleChange(lpszText);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHtmlViewView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// create the view window itself
	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName,
				dwStyle, rect, pParentWnd,  nID, pContext))
	{
		return FALSE;
	}

	// Verhindert das Abschalten des Contextmenüs
//	AfxEnableControlContainer();

	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, lpszWindowName,
				WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}

	return TRUE;
}
