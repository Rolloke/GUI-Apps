
#include "stdafx.h"
#include "HiddenCapiWnd.h"
#include "CapiThread.h"
#include "ISDNConnection.h"
#include "Net_Messages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHiddenCapiWnd
CHiddenCapiWnd::CHiddenCapiWnd(CCapiThread *pThread)
{
	m_pThread = pThread;
}

CHiddenCapiWnd::~CHiddenCapiWnd()
{
	m_pThread->m_pCapiWnd = NULL;
}


BEGIN_MESSAGE_MAP(CHiddenCapiWnd, CWnd)
	//{{AFX_MSG_MAP(CHiddenCapiWnd)
	ON_MESSAGE(NET_CAPI_MSG, OnCapiMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHiddenCapiWnd message handlers
void CHiddenCapiWnd::PostNcDestroy()
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
long CHiddenCapiWnd::OnCapiMessage(WPARAM wParam, LPARAM lParam)
{
	return m_pThread->m_pConnection->ReceiveCapiMessage(wParam, lParam);
}
