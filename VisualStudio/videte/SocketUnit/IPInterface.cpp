// IPInterface.cpp: implementation of the CIPInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPInterface.h"
#include "SocketUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

   int  CIPInterface::m_iNrOfConnectingInterface=0;
CEvent  CIPInterface::m_eConnected;
CString CIPInterface::m_sAdapterIP;

CIPInterface::CIPInterface(CString sAdapterIP)
{
	m_sProbeAdapterIP= sAdapterIP;
	m_bIPReachable   = FALSE;
	m_bConnecting    = FALSE;
	m_hSocket		 = NULL;
	m_pThread		 = NULL;
}

/////////////////////////////////////////////////////////////////////

CIPInterface::~CIPInterface()
{
	Sleep(50);
	if(m_bConnecting || m_hSocket)
	{
		closesocket(m_hSocket);
		if(m_pThread)
		{
			WaitForSingleObject(m_pThread->m_hThread,5000);
		}	
	}


	WK_DELETE(m_pThread);
}

/////////////////////////////////////////////////////////////////////

void CIPInterface::TryToConnect(CString sRemoteIP)
{
	m_sRemoteIP = sRemoteIP;
	m_iNrOfConnectingInterface++;
	m_pThread = AfxBeginThread(ConnectThread,this,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	if(m_pThread)
	{
		m_pThread->m_bAutoDelete=FALSE;
		m_pThread->ResumeThread();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
UINT CIPInterface::ConnectThread(LPVOID ThreadObject)
{
	CIPInterface *IPInterface = (CIPInterface*)ThreadObject;

	IPInterface->m_hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(IPInterface->m_hSocket == INVALID_SOCKET)
	{
		IPInterface->m_bIPReachable=FALSE;
	}
	else
	{
		int iErr;
		sockaddr_in local_sin;
		CWK_String sProbeAdapterIP = IPInterface->m_sProbeAdapterIP;
		ZeroMemory(&local_sin,sizeof(local_sin));
		local_sin.sin_family      = AF_INET;
		local_sin.sin_port        = htons(0);
		local_sin.sin_addr.s_addr = (inet_addr(sProbeAdapterIP));

		iErr = bind(IPInterface->m_hSocket,(struct sockaddr*)&local_sin,sizeof(struct sockaddr));

		if(iErr == SOCKET_ERROR)
		{
			WK_TRACE_ERROR(_T("cannot bind socket %s %s\n"),IPInterface->m_sProbeAdapterIP,WSAGetLastErrorString());
			IPInterface->m_bIPReachable=FALSE;
		}
		else
		{
			struct sockaddr_in	stAddr;
			CWK_String sRemoteIP = IPInterface->m_sRemoteIP;
			ZeroMemory(&stAddr,sizeof(stAddr));
			stAddr.sin_family= AF_INET;
			stAddr.sin_addr.s_addr = inet_addr(sRemoteIP);
			stAddr.sin_port  = htons(1927);
			IPInterface->m_bConnecting = TRUE;
			WK_TRACE(_T("TRY TO CONNECT ON %s TO %s\n"),IPInterface->m_sProbeAdapterIP,IPInterface->m_sRemoteIP);
			iErr = connect(IPInterface->m_hSocket,(struct sockaddr*)&stAddr,sizeof(struct sockaddr));
			if(iErr==0)
			{
				IPInterface->m_bIPReachable=TRUE;
				IPInterface->m_iNrOfConnectingInterface=0;
				shutdown(IPInterface->m_hSocket,0x02);
				closesocket(IPInterface->m_hSocket);
				IPInterface->m_hSocket = NULL;
			}
			else 
			{
				if (iErr == SOCKET_ERROR)
				{
					WK_TRACE(_T("WSA ERROR %s during try to connect from %s to %s\n"),
						WSAGetLastErrorString(),IPInterface->m_sProbeAdapterIP,IPInterface->m_sRemoteIP);
				}
				if(IPInterface->m_iNrOfConnectingInterface)
				{
					IPInterface->m_iNrOfConnectingInterface--;
				}
				IPInterface->m_bIPReachable=FALSE;
			}
		}
	}
	IPInterface->m_bConnecting=FALSE;
	if(IPInterface->m_bIPReachable)
	{
		if(IPInterface->m_sAdapterIP==_T(""))
		{
			IPInterface->m_sAdapterIP=IPInterface->m_sProbeAdapterIP;
			IPInterface->m_eConnected.SetEvent();
		}
	}
	if(IPInterface->m_iNrOfConnectingInterface <= 0)
	{
		IPInterface->m_eConnected.SetEvent();
	}
	return 0;
}

