// SocketConnection.cpp : implementation file
//

#include "stdafx.h"
#include "emailunit.h"
#include "SocketConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSocketConnection

IMPLEMENT_DYNCREATE(CSocketConnection, CWinThread)

CSocketConnection::CSocketConnection()
{
}

CSocketConnection::~CSocketConnection()
{
}

BOOL CSocketConnection::InitInstance()
{
	
	m_sSocket.Attach(m_hSocketHandle);
	return TRUE;
}

int CSocketConnection::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSocketConnection, CWinThread)
	//{{AFX_MSG_MAP(CSocketConnection)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSocketConnection message handlers
