// ConnectionThreadWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SocketUnit.h"
#include "ConnectionThreadWnd.h"
#include "ConnectionThread.h"
#include ".\connectionthreadwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CConnectionThreadWnd

IMPLEMENT_DYNAMIC(CConnectionThreadWnd, CWnd)
CConnectionThreadWnd::CConnectionThreadWnd(CConnectionThread* pConnectionThread)
{
	m_pConnectionThread = pConnectionThread;
}
//////////////////////////////////////////////////////////////////////////////////
CConnectionThreadWnd::~CConnectionThreadWnd()
{
}
//////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CConnectionThreadWnd, CWnd)
	ON_MESSAGE(WM_USER,OnUser)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////////////
// CConnectionThreadWnd message handlers
LRESULT CConnectionThreadWnd::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) 
	{
	case WPARAM_SEND:
		m_pConnectionThread->Send();
		break;
	case WPARAM_DISCONNECT:
		m_pConnectionThread->CloseConnection();
		break;
	}
	return 0L;
}
//////////////////////////////////////////////////////////////////////////////////
void CConnectionThreadWnd::PostNcDestroy()
{
	delete this;
}
