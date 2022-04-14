// ConnectSocket.cpp: implementation of the CConnectSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RemoteControl.h"
#include "ConnectSocket.h"
#include "RemoteControlDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
CConnectSocket::CConnectSocket(CRemoteControlDlg *pWnd, const CString &sIPAddr, int nPort, DWORD dwConnectTimeOut) : 
				CHHLASocket(sIPAddr, nPort, dwConnectTimeOut)
{
	m_nWorkerPort		= 0;
	m_pWnd				= pWnd;
	m_hGetWorkerPort	= CreateEvent(0, FALSE, FALSE, NULL);
	if (!m_hGetWorkerPort)
		WK_TRACE_ERROR(_T("Create m_hGetWorkerPort failed\n"));

}

/////////////////////////////////////////////////////////////////////////////
CConnectSocket::~CConnectSocket()
{
	if (m_hGetWorkerPort)
	{
		CloseHandle(m_hGetWorkerPort);
		m_hGetWorkerPort = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CConnectSocket::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_pWnd && IsWindow(m_pWnd->m_hWnd))
		m_pWnd->PostMessage(WM_SOCKET_CLOSED, 0, 0);

	CSocket::OnClose(nErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CConnectSocket::OnReceivePacketType00(WORD wStatusCode, WORD wTimeOut, int nWorkerPort)
{
	WK_TRACE(_T("--------------------------->\n"));
	WK_TRACE(_T("ConnectSocket: PacketType:00\n"));
	WK_TRACE(_T("\tStatusCode:%u\n"),  wStatusCode);
	WK_TRACE(_T("\tTimeOut...:%u\n"),  wTimeOut);
	WK_TRACE(_T("\tWorkerPort:%d\n"), nWorkerPort);
	WK_TRACE(_T("<---------------------------\n"));

	if (wStatusCode == 98)
	{
		m_nWorkerPort = nWorkerPort;
		SetEvent(m_hGetWorkerPort);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CConnectSocket::WaitForWorkport(DWORD dwTimeout)
{
	HANDLE hEvent[1];
	
	hEvent[0]	= m_hGetWorkerPort;
	DWORD dwTC	= GetTickCount();

	// Warte 'dwTimeout' Millisekunden auf den Workerport
	do
	{
		OnReceive(0);	// Socket bedienen.

		switch (WaitForMultipleObjects(1, hEvent, FALSE, 200))
		{
			case WAIT_OBJECT_0:			  
				return m_nWorkerPort;
		}
	}
	while (GetTickCount() < dwTC + dwTimeout);

	return 0;
}

