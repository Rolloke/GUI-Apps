// ConnectSocket.h: interface for the CConnectSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTSOCKET_H__7F4E4641_A479_11D2_A9C3_004005A11E32__INCLUDED_)
#define AFX_CONNECTSOCKET_H__7F4E4641_A479_11D2_A9C3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HHLASocket.h"

class CRemoteControlDlg;
class CConnectSocket : public CHHLASocket  
{
public:
	CConnectSocket(CRemoteControlDlg* pWnd, const CString &sIPAddr, int nPort, DWORD dwConnectTimeOut);
	virtual ~CConnectSocket();
	int WaitForWorkport(DWORD dwTimeOut);

protected:
	virtual BOOL OnReceivePacketType00(WORD wStatusCode, WORD wTimeOut, int nWorkerPort);
	virtual void OnClose(int nErrorCode);

protected:
	HANDLE				m_hGetWorkerPort;				// Field Encoded Event
	int					m_nWorkerPort;
	CRemoteControlDlg*	m_pWnd;
};

#endif // !defined(AFX_CONNECTSOCKET_H__7F4E4641_A479_11D2_A9C3_004005A11E32__INCLUDED_)
