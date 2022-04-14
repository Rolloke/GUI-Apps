// IPInterface.h: interface for the CIPInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPINTERFACE_H__4294D3EE_366D_4C6A_9DB9_E5FDC6B9CDBE__INCLUDED_)
#define AFX_IPINTERFACE_H__4294D3EE_366D_4C6A_9DB9_E5FDC6B9CDBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPInterface  
{
public:

	CIPInterface(CString sAdapterIP);
	virtual ~CIPInterface();
	static UINT ConnectThread(LPVOID ThreadObject);
	void TryToConnect(CString sRemoteIP);

	static int     m_iNrOfConnectingInterface;
	static CEvent  m_eConnected;
	static CString m_sAdapterIP;

private:

	CWinThread* m_pThread;
	SOCKET m_hSocket;
	BOOL m_bIPReachable;
	BOOL m_bConnecting;	
	CEvent  m_eConnectProcessReady;
	CString m_sProbeAdapterIP;
	CString m_sRemoteIP;
};

#endif // !defined(AFX_IPINTERFACE_H__4294D3EE_366D_4C6A_9DB9_E5FDC6B9CDBE__INCLUDED_)
