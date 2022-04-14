
#include "stdafx.h"
#include "ControlSocket.h"
#include "SocketUnitDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef _DEBUG
void CControlSocket::AssertValid() const
{
	CSocket::AssertValid();
}

void CControlSocket::Dump(CDumpContext& dc) const
{
	CSocket::Dump(dc);
}
#endif //_DEBUG

IMPLEMENT_DYNAMIC(CControlSocket, CSocket)


/////////////////////////////////////////////////////////////////////////////

CControlSocket::CControlSocket(CSocketUnitDoc* pDoc)
{
	m_pDoc = pDoc;
}

/////////////////////////////////////////////////////////////////////////////////////

void CControlSocket::OnAccept(int nErrorCode)
{
	WK_TRACE(_T("CControlSocket::OnAccept\n"));
  	// control sockets are in the main thread!
	m_pDoc->ProcessPendingAccept(this);
	CSocket::OnAccept(nErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
// CSocket Implementation

CControlSocket::~CControlSocket()
{
}

