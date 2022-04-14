// JpegChild.cpp : implementation file
//

#include "stdafx.h"
#include "h263play.h"
#include "JpegChild.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJpegChild

IMPLEMENT_DYNCREATE(CJpegChild, CMDIChildWnd)

CJpegChild::CJpegChild()
{
}

CJpegChild::~CJpegChild()
{
}


BEGIN_MESSAGE_MAP(CJpegChild, CMDIChildWnd)
	//{{AFX_MSG_MAP(CJpegChild)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJpegChild message handlers

BOOL CJpegChild::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~(WS_THICKFRAME|WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	
	return CMDIChildWnd::PreCreateWindow(cs);
}
