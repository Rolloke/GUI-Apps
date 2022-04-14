// H263Child.cpp : implementation file
//

#include "stdafx.h"
#include "h263play.h"
#include "H263Child.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CH263Child

IMPLEMENT_DYNCREATE(CH263Child, CMDIChildWnd)

CH263Child::CH263Child()
{
}

CH263Child::~CH263Child()
{
}


BEGIN_MESSAGE_MAP(CH263Child, CMDIChildWnd)
	//{{AFX_MSG_MAP(CH263Child)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CH263Child message handlers

BOOL CH263Child::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CMDIChildWnd::PreCreateWindow(cs);
}
