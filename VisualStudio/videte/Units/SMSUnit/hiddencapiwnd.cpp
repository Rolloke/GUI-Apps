
#include "stdafx.h"
#include "HiddenCapiWnd.h"
#include "CapiSMS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHiddenCapiWnd

CHiddenCapiWnd::CHiddenCapiWnd(CCapiSMS* pConnection)
{
	m_pConnection = pConnection;
}

CHiddenCapiWnd::~CHiddenCapiWnd()
{
}


BEGIN_MESSAGE_MAP(CHiddenCapiWnd, CWnd)
	//{{AFX_MSG_MAP(CHiddenCapiWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(NET_CAPI_MSG, OnCapiMessage)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHiddenCapiWnd message handlers

/////////////////////////////////////////////////////////////////////////////
long CHiddenCapiWnd::OnCapiMessage(WPARAM wParam, LPARAM lParam)
{
	return m_pConnection->ReceiveCapiMessage(wParam, lParam);
}
