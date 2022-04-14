// WinSocket.cpp : implementation file
//

#include "stdafx.h"
#include "WinSocket.h"
#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CWinSocket

IMPLEMENT_DYNCREATE(CWinSocket, CWinThread)

CWinSocket::CWinSocket(CWinThread  *pMessageReceiver,CWnd *pWndMessageReceiver)
{

	m_bAutoDelete = TRUE;
	m_hSocket     = NULL;

	m_iSocketType         = -1;
    m_pMessageReceiver    = pMessageReceiver;
	m_pWndMessageReceiver = pWndMessageReceiver;
	m_bSocketIsBind       = FALSE;
	
	m_pAcceptThread       = NULL;
	m_pReceiveThread      = NULL;
	m_pSendThread         = NULL;
	m_pConnectThread	  = NULL;
	m_pPingThread		  = NULL;
	m_pCryptData		  = NULL;

	m_uMaxRecvBufferSize  = 0L;
	m_uMaxUDSize          = 0L;
	m_uMaxSendBufferSize  = 0L;

	m_uSTREAMDataLen	  = 0L;
	m_pbSTREAMData        = NULL;
	m_LocalAddr           = NULL;

	m_iPingTimeout        = 0;
	
	CreateThread();
}

/////////////////////////////////////////////////////////////////////////////////

CWinSocket::~CWinSocket()
{
	if(m_LocalAddr)
		delete m_LocalAddr;
   ExitThread();

}

/////////////////////////////////////////////////////////////////////////////////

BOOL CWinSocket::InitInstance()
{
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////

int CWinSocket::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CWinSocket, CWinThread)
	//{{AFX_MSG_MAP(CWinSocket)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(TWM_SOCKET_CREATED,OnInternSocketCreated)
	ON_THREAD_MESSAGE(TM_MESSAGE,OnInternMessage)	  
	ON_THREAD_MESSAGE(TWM_SOCKET_BIND,OnInternSocketBind)
	ON_THREAD_MESSAGE(TWM_SOCKET_CLOSED,OnInternSocketClosed)
	ON_THREAD_MESSAGE(TWM_MULTICAST_DROPPED,OnInternMulticastMembershipDropped)
	ON_THREAD_MESSAGE(TWM_MULTIPLEX_DROPPED,OnInternMultiplexMembershipDropped)
	ON_THREAD_MESSAGE(TWM_MULTICAST_JOINED,OnInternMulticastGroupJoined)
	ON_THREAD_MESSAGE(TWM_MULTIPLEX_ADDED,OnInternMultiplexMemberAdded)
	ON_THREAD_MESSAGE(TWM_BANDWIDTH,OnInternConfirmBandwidth)
	ON_THREAD_MESSAGE(TWM_DATA_SEND,OnInternDataSend)
	ON_THREAD_MESSAGE(TWM_NEW_DATA,OnInternNewData)
	ON_THREAD_MESSAGE(TWM_CONNECTION_CLOSED,OnInternConnectionClosed)
	ON_THREAD_MESSAGE(TWM_SENDING,OnInternSending)
	ON_THREAD_MESSAGE(TWM_RECEIVING,OnInternReceiving)
	ON_THREAD_MESSAGE(TWM_REUSED,OnInternReused)
	ON_THREAD_MESSAGE(TWM_PINGRETURN,OnInternPingReturn)
	ON_THREAD_MESSAGE(TWM_LISTENING,OnInternListening)


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinSocket message handlers

void CWinSocket::Create(int iType, int iProtocol)
{

	PostThreadMessage(TM_MESSAGE,
					  (UINT)iType,
					  MAKELONG(M_CREATE_SOCKET,iProtocol));
}

/////////////////////////////////////////////////////////////////////////////////
/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981, taken from the BSD file netinet/in.h.
 */
/*
 * Protocols
 *
	 IPPROTO_IP           dummy for IP 
	 IPPROTO_ICMP         control message protocol 
	 IPPROTO_IGMP         group management protocol
	 IPPROTO_GGP          gateway^2 (deprecated)
	 IPPROTO_TCP          tcp 
	 IPPROTO_PUP          pup
	 IPPROTO_UDP          user datagram protocol
	 IPPROTO_IDP          xns idp
	 IPPROTO_ND           UNOFFICIAL net disk proto
	 IPPROTO_RAW          raw IP packet 
	 IPPROTO_MAX           
 */


////////////////////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternCreate(UINT uType, int iProtocol)
{
	int iType, iPType;

	if(uType == UDP)
	{
		iType     = SOCK_DGRAM;	
		if(iProtocol)
		{
			iPType = iProtocol;
		}
		else
		{
			iPType = IPPROTO_UDP;
		}
	}

	else if(uType == TCP)
	{
		iType     = SOCK_STREAM;
		if(iProtocol)
		{
			iPType = iProtocol;
		}
		else
		{
			iPType = IPPROTO_TCP;
		}
	}
	else if(uType == RAW)
	{
		iType     = SOCK_RAW;
		if(iProtocol)
		{
			iPType = iProtocol;
		}
		else
		{
			iPType = IPPROTO_RAW;
		}
	}
	else
	{
		TRACE("WinSocket::OnCreate   Unknown Sockettype\n");
	}

	m_hSocket = socket(AF_INET,iType,iPType);

	if(m_hSocket == INVALID_SOCKET)
	{
		TRACE("WinSocket::OnCreate :");
		PostError(WSAGetLastError());
	}
	else
	{

		if(uType==UDP)
		{
			m_uMaxUDSize=GetMaxMessageSize();
		}
		m_uMaxRecvBufferSize = GetRecvBufferSize();
		m_uMaxSendBufferSize = GetSendBufferSize();

		m_iSocketType = uType;

		Post(TWM_SOCKET_CREATED,(UINT)this,(LONG)uType);
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::Listen()
{
	if(m_iSocketType == UDP)
	{
		//TODO

		return;
	}
	else if(m_hSocket)
	{
		PostThreadMessage(TM_MESSAGE,
			              0,
						  MAKELONG(M_LISTEN,0));
	}
	else
	{
		TRACE("WinSocket::Listen  No Sockethandle\n");
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternListen()
{
	int iErr = 0;

	if(m_bSocketIsBind)
	{
		iErr = listen(m_hSocket, MAX_PENDING_CONNECTS);

		if(iErr == SOCKET_ERROR)
		{
			TRACE("WinSocket::OnListen :");
			PostError(WSAGetLastError());
		}
		else
		{
			m_pAcceptThread = AfxBeginThread(AcceptThreadFunc,this,THREAD_PRIORITY_NORMAL);

			Post(TWM_LISTENING,(UINT)this,(LONG)m_LocalAddr->sin_port);
		}
	}
	else
	{
		TRACE("WinSocket::OnListen   Try to listen without bind\n");
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternAccept(SOCKET hClientSocket)
{
	CWinSocket *pWinSocket;
	
	if(m_pMessageReceiver)
	{
		pWinSocket = new CWinSocket(m_pMessageReceiver,NULL);
		pWinSocket->Create(TCP);
	}
	else if(m_pWndMessageReceiver)
	{
		pWinSocket = new CWinSocket(NULL,m_pWndMessageReceiver);
		pWinSocket->Create(TCP);
	}

	pWinSocket->PostThreadMessage(TM_MESSAGE,
								  (UINT)hClientSocket,
								  MAKELONG(M_ATTACH_HANDLE,0));
 }

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::ExitThread()
{
	if(m_hSocket)
	{
		CancelBlockingCall(TRUE,TRUE);
		if(closesocket(m_hSocket)==SOCKET_ERROR)
		{
			TRACE("CWinSocket::ExitThread :");
			PostError(WSAGetLastError());

		}
	}
	if(m_pReceiveThread)
	{
		delete m_pReceiveThread;
		m_pReceiveThread=NULL;
	}
	if(m_pAcceptThread)
	{
		delete m_pAcceptThread;
		m_pAcceptThread=NULL;
	}
	if(m_pSendThread)
	{
		delete m_pSendThread;
		m_pSendThread=NULL;
	}
	if(m_pConnectThread)
	{
		delete m_pConnectThread;
		m_pConnectThread=NULL;
	}
	if(m_pPingThread)
	{
		delete m_pPingThread;
		m_pPingThread=NULL;
	}

	::PostQuitMessage(0);
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::Attach(SOCKET hSocket)
{

}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternAttachHandle(SOCKET hSockethandle)
{

	int ierr;

	if(m_hSocket)			//Existiert schon
	{
		TRACE("CWinSocket::OnAttachHandle  Handle already exists\n");
	}
	else
	{
	   m_hSocket = hSockethandle;
	   struct sockaddr_in Sockaddr;
	   int Sockaddr_len = sizeof(Sockaddr);

	   if(m_iSocketType == TCP)
	   {
		   ierr = getpeername(m_hSocket,(struct sockaddr*)&Sockaddr,&Sockaddr_len);

		   if(ierr == SOCKET_ERROR)
		   {
    		   TRACE("CWinSocket::OnAttachHandle getpeername:");
			   PostError(WSAGetLastError());
		   }
		   else
		   {
				m_RemoteConnectedAddr = Sockaddr;
		   }
	   }
	   ierr = getsockname(m_hSocket,(struct sockaddr*)&Sockaddr,&Sockaddr_len);

   	   if(ierr == SOCKET_ERROR)
	   {
  		   TRACE("CWinSocket::OnAttachHandle getsockname:");
		   PostError(WSAGetLastError());
	   }
	   else
	   {
			m_uMaxRecvBufferSize = GetRecvBufferSize();
			m_uMaxSendBufferSize = GetSendBufferSize();
	
			Post(TWM_NEW_CONNECTION,(UINT)this,(LONG)m_iSocketType);
	   } 
    }
}

/////////////////////////////////////////////////////////////////////////////////

UINT CWinSocket::AcceptThreadFunc(LPVOID pWinSockObject)
{
	CWinSocket *pWinSocket = (CWinSocket*)pWinSockObject;
	
	struct sockaddr_in  accept_sin;
	int    accept_sin_len       = sizeof(accept_sin);

	pWinSocket->Post(TWM_LISTENING,(UINT)pWinSocket,(LONG)pWinSocket->m_LocalAddr->sin_port);

	while(1)
	{
		SOCKET ClientSocket = accept(pWinSocket->m_hSocket,
									 (struct sockaddr*) &accept_sin,
									 &accept_sin_len);

		if(ClientSocket==INVALID_SOCKET)
		{
			   int iError=WSAGetLastError();

			   if(iError == 10004)					//CancelBlockingCall
			   {
	     		   TRACE("CWinSocket::AcceptThreadFunc:");
				   pWinSocket->PostError(WSAGetLastError());
			   }
			   return 0;
		}
		else
		{
			((CWinSocket*)pWinSockObject)->PostThreadMessage(TM_MESSAGE,
														     (UINT)ClientSocket,
															 MAKELONG(M_ON_ACCEPT,0));
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::CancelBlockingCall(BOOL bSends, BOOL bReceives)
{

   int iRet;

   if(bSends)
   {
		iRet = shutdown(m_hSocket,0x01);

		if(iRet == SOCKET_ERROR )
		{
  		   TRACE("CWinSocket::CancelBlockingCall:");
		   PostError(WSAGetLastError());
		}
   }
   if(bReceives)
   {
		iRet = shutdown(m_hSocket,0x00);
   
		if(iRet == SOCKET_ERROR )
		{
  		   TRACE("CWinSocket::CancelBlockingCall:");
		   PostError(WSAGetLastError());
		}
   }
}

/////////////////////////////////////////////////////////////////////////////////


void CWinSocket::OnInternStopListen()
{
    int ierr = closesocket(m_hSocket);

	if(ierr == SOCKET_ERROR)
	{
	   TRACE("CWinSocket::OnStopListen:");
	   PostError(WSAGetLastError());
	}
	m_hSocket = NULL;
}

/////////////////////////////////////////////////////////////////////////////////

UINT CWinSocket::ReceiveThreadFunc(LPVOID pWinSockObject)
{

   	CWinSocket *pWinSocket = (CWinSocket*)pWinSockObject;
	int  iErr ,iSize;
	int  iBufferSize  = 0;
	BYTE *pRecvBuffer = NULL;
	char *cbuf = NULL;

	struct sockaddr_in RemoteConnectedAddr;
	struct sockaddr_in from;
	int fromlen = sizeof(from);   

	struct timeval timeout;
	DWORD dwRecvTime = 0L;
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	fd_set sockset;

	TRACE("CWinSocket: ReceiveThread started\n");

	while(1)
	{
		DWORD dwRet = WaitForSingleObject(pWinSocket->m_eStopReceiveThread,10);
		if(dwRet == WAIT_OBJECT_0)
		{
			TRACE("WinSocket ReceiveThread beendet\n");
			return 0;
		}
		FD_ZERO(&sockset); 
		FD_SET(pWinSocket->m_hSocket, &sockset);

		if(select(0,&sockset,NULL,NULL,&timeout))
		{
			if(pWinSocket->m_hSocket)
			{
				
		//--------------- UDP ----------------------------------------------

				if(pWinSocket->m_iSocketType == UDP)
				{//ok

					iSize = pWinSocket->m_uMaxUDSize;

					cbuf=new char[iSize];

					iErr = recvfrom(pWinSocket->m_hSocket,
									cbuf,
									iSize
									,0
									,(struct sockaddr*)&from,
									&fromlen);

					dwRecvTime = GetTickCount();
					RemoteConnectedAddr = from;
				}

		//--------------- TCP ----------------------------------------------

				else if(pWinSocket->m_iSocketType == TCP)
				{
					iSize = pWinSocket->m_uMaxRecvBufferSize;
					cbuf=new char[iSize];

					iErr = recv(pWinSocket->m_hSocket,
								cbuf,
								iSize
								,0);

					RemoteConnectedAddr  = pWinSocket->m_RemoteConnectedAddr;
				}
			}
			else
			{
				//TODO
				return 0;
			}
			if(iErr == SOCKET_ERROR)
			{
				//TODO 
				TRACE("CWinSocket::ReceiveThreadFunc:");
				pWinSocket->PostError(WSAGetLastError());
				return 0;
			}
			else if(iErr == 0)
			{
				//Verbindung wurde Remote beendet
				pWinSocket->Post(TWM_CONNECTION_CLOSED,(UINT)pWinSocket,NULL);

				return 0;
			}
			else
			{
				//Alles Ok
				if(iErr>0)
				{
				pRecvBuffer=new BYTE[iErr];
				CopyMemory(pRecvBuffer,(BYTE*)cbuf,iErr);
				if(cbuf)
				{
					delete cbuf;
					cbuf = NULL;
				}
				else
				{
				 
				}
				CString sMessage = (const char*)(BYTE*)pRecvBuffer;
				if(sMessage.Left(3)=="#B:")
				{
				   DWORD dwTime   =*(DWORD*)(((BYTE*)pRecvBuffer)+3);
				   DWORD dwActTime = GetTickCount();

				   DWORD dwTimeToRun = (dwActTime-dwTime)/2;
				   float fTime = (float)(dwTimeToRun);		//Byte/sec
				   						
				   int iBit = int((iErr / fTime)*1000);

				   pWinSocket->Post(TWM_BANDWIDTH,(UINT)iBit,(LONG)RemoteConnectedAddr.sin_addr.S_un.S_addr);

				   delete pRecvBuffer;
				   delete cbuf;
				}							   //UDP
				else if(sMessage.Left(3)=="_B:")
				{
					DATAGRAM *pDatagram        = new DATAGRAM;
					INIT_DATAGRAM(pDatagram);
						
					pDatagram->bDataBuffer       = (BYTE*)pRecvBuffer;
					pDatagram->uDataBufferSize   = (UINT)iErr;
					pDatagram->bFlag			 = FLAG_BANDWIDTH;
					CString sIP;  
					*pRecvBuffer='#';
					sIP.Format("%i.%i.%i.%i",
						       RemoteConnectedAddr.sin_addr.S_un.S_un_b.s_b1,
							   RemoteConnectedAddr.sin_addr.S_un.S_un_b.s_b2,
							   RemoteConnectedAddr.sin_addr.S_un.S_un_b.s_b3,
							   RemoteConnectedAddr.sin_addr.S_un.S_un_b.s_b4);

					UINT iPort=  htons(RemoteConnectedAddr.sin_port);

					pDatagram->iValue=dwRecvTime;
					pWinSocket->Send(pDatagram,sIP,iPort,FLAG_BANDWIDTH);
				}								//TCP
				else if(sMessage.Left(3)=="_B:")
				{

				}
				else
				{
					LPSYSTEMTIME pSystemtime = new SYSTEMTIME;

					GetSystemTime(pSystemtime);

					//Datagram erzeugen
					
					DATAGRAM *pDatagram        = new DATAGRAM;
					INIT_DATAGRAM(pDatagram);

					pDatagram->bDataBuffer       = (BYTE*)pRecvBuffer;
					pDatagram->uDataBufferSize   = (UINT)iErr;
					pDatagram->SenderAddr        = RemoteConnectedAddr;
					pDatagram->ReceiverAddr      = *pWinSocket->m_LocalAddr;
					pDatagram->pSystemTimeStamp  = pSystemtime;
					pDatagram->dwWinSockAdress   = (DWORD)pWinSockObject;
				
					pWinSocket->Post(TWM_NEW_DATA,(UINT)pDatagram,NULL);
				 }
				}
			}
		}
		else
		{
	  		//Sleep(10);
		}
		Sleep(10);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////


int CWinSocket::PeekInBuffer()
{

  int iSize=0;

  if(m_iSocketType==UDP)
  {
	iSize = m_uMaxUDSize;
  }
  else
  {
	iSize = m_uMaxRecvBufferSize;
  }

  char *cbuf=new char[iSize];
  int  iErr;

  if(m_hSocket)
  {
		iErr = recv(m_hSocket,cbuf,iSize,MSG_PEEK);   	
		delete cbuf;
		if(iErr != SOCKET_ERROR)
		{
			return iErr;
		}
		else
		{
			TRACE("CWinSocket::PeekInBuffer:");
			PostError(WSAGetLastError());
			return -1;
		}
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternMessage(UINT uData,LONG lMessage)
{
   WORD wMessage = LOWORD(lMessage);
   WORD wData    = HIWORD(lMessage);

   switch (wMessage)
	{

	   case M_CREATE_SOCKET:
		   OnInternCreate(uData,wData);
		   break;

	   case M_LISTEN:
		   OnInternListen();
		   break;

	   case M_ON_ACCEPT:
		   OnInternAccept((SOCKET)uData);
		   break;

	   case M_ON_SEND:
		   break;

	   case M_ATTACH_HANDLE:
		   OnInternAttachHandle((SOCKET)uData);
		   break;

	   case M_CLOSE_SOCKET:
		   OnInternCloseSocket();
		   break;

	   case M_DESTROY_SOCKET:
		   ExitThread();
		   break;

	   case M_SEND_DATA:
		   OnInternSend((DATAGRAM*)uData);
		   break; 

	   case M_JOIN_MULTICAST:
		   OnInternJoinMulticastGroup((unsigned long)uData);
		   break;

	   case M_DROP_MULTICAST:
		   OnInternDropFromMulticastGroup(uData);
		   break;

	   case M_DUPLEX_MODE:
		   OnInternSetMode(uData);
		   break; 

	   case M_REUSE_ADDR:
		   OnInternReuseAddr(uData);
		   break;

	   case M_BIND_SOCKET:
		   OnInternBind(uData);
		   break;

	   case M_ADD_MULTIPLEX_MEMBER:
		   OnInternAddMultiplexMember((long)uData,(UINT)HIWORD(lMessage));
		   break;

	   case M_DROP_MULTIPLEX_MEMBER:
		   OnInternDropMultiplexMember((long)uData,(UINT)HIWORD(lMessage));
		   break;

	   case M_CONNECT:
		   OnInternConnect();
		   break;

	   case M_DISCONNECT:
		   OnInternDisconnect();
		   break;

	   case M_PING:
		   OnInternPing();
		   break;
	}
}


/////////////////////////////////////////////////////////////////////////////////


void CWinSocket::CloseSocket()
{
	
	PostThreadMessage(TM_MESSAGE,
		              0,
					  MAKELONG(M_CLOSE_SOCKET,0));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternCloseSocket()
{
	int iErr;

	CancelBlockingCall(TRUE,TRUE);
	StopThreads();

	iErr=closesocket(m_hSocket);

	if(iErr == SOCKET_ERROR)
	{
		TRACE("CWinSocket::OnCloseSocket:");
		PostError(WSAGetLastError());
	}
	m_hSocket=NULL;

	Post(TWM_SOCKET_CLOSED,(UINT)this,0);

}


/////////////////////////////////////////////////////////////////////////////////

UINT CWinSocket::SendThreadFunc(LPVOID pWinSockObject)
{

	CWinSocket *pWinSocket = (CWinSocket*)pWinSockObject;
	int         iError       = 0,
				iMember    = 0;
	
	struct timeval timeout;
	DATAGRAM *pDatagram;
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	fd_set sockset;

	TRACE("CWinSocket: SendThread started\n");
	while(1)
	{

		FD_ZERO(&sockset); 
		FD_SET(pWinSocket->m_hSocket, &sockset);

		if(select(0,NULL,&sockset,NULL,&timeout))
		{

			//------------------------------------------------------------------------
			
			if(pWinSocket->m_iSocketType == UDP)	//Datagramweise auslesen und senden
			{
				pWinSocket->m_CritSec.Lock();
				
				if(!pWinSocket->m_ListDatagram.IsEmpty())
				{
					pDatagram = pWinSocket->m_ListDatagram.RemoveHead();
					if(pDatagram->uDataBufferSize > pWinSocket->m_uMaxUDSize)
					{
						pDatagram->uDataBufferSize=pWinSocket->m_uMaxUDSize;
					}
	   				iError = pWinSocket->SendOverUDP(pDatagram);
				}
				pWinSocket->m_CritSec.Unlock();	
			}

/*	if(iError == SOCKET_ERROR)
	{
		TRACE("CWinSocket::SendThreadFunc:");
		pWinSocket->PostError(WSAGetLastError());
		return 0;
	}
	}
	else
	{
		pWinSocket->m_CritSec.Unlock();	
	}
	}
*/			

	//------------------------------------------------------------------------
			
			else if(pWinSocket->m_iSocketType == TCP)	//Streaming
			{  

				if(pWinSocket->m_uSTREAMDataLen > 0)
				{
					BYTE *pbHelpBuffer = NULL, 
						 *pbNewBuffer  = NULL;
					
					UINT uHelpLen = 0;
					
					pWinSocket->m_CritSec.Lock();
					
					if(pWinSocket->m_uMaxSendBufferSize < pWinSocket->m_uSTREAMDataLen)
					{
						pbHelpBuffer = new BYTE[pWinSocket->m_uMaxSendBufferSize];
						
						CopyMemory(pbHelpBuffer,
									pWinSocket->m_pbSTREAMData,
									pWinSocket->m_uMaxSendBufferSize);
						
						uHelpLen = pWinSocket->m_uMaxSendBufferSize;
						
						pbNewBuffer = new BYTE[pWinSocket->m_uSTREAMDataLen-
												pWinSocket->m_uMaxSendBufferSize];
						CopyMemory(pbNewBuffer,
									pWinSocket->m_pbSTREAMData+pWinSocket->m_uMaxSendBufferSize,
									pWinSocket->m_uSTREAMDataLen-pWinSocket->m_uMaxSendBufferSize);
						
						pWinSocket->m_uSTREAMDataLen = pWinSocket->m_uSTREAMDataLen -
													   pWinSocket->m_uMaxSendBufferSize;
						
						delete(pWinSocket->m_pbSTREAMData);
						pWinSocket->m_pbSTREAMData = pbNewBuffer;
					}
					else
					{
						pbHelpBuffer = new BYTE[pWinSocket->m_uSTREAMDataLen];
						CopyMemory(pbHelpBuffer,
									pWinSocket->m_pbSTREAMData,
									pWinSocket->m_uSTREAMDataLen);
						
						uHelpLen = pWinSocket->m_uSTREAMDataLen;
						delete(pWinSocket->m_pbSTREAMData);
						pWinSocket->m_pbSTREAMData   = NULL;
						pWinSocket->m_uSTREAMDataLen = 0;
					}
					pWinSocket->m_CritSec.Unlock();
					
					iError = send(pWinSocket->m_hSocket,
								(const char*)pbHelpBuffer,
								uHelpLen,
								0);
					
					delete pbHelpBuffer;
					pbHelpBuffer = NULL;
					uHelpLen     = 0;
					
					if(iError == SOCKET_ERROR)
					{
						TRACE("CWinSocket::SendThreadFunc:");
						pWinSocket->PostError(WSAGetLastError());
						return 0;
					}
					else
					{
						/*
						Post(TWM_DATA_SEND,(UINT)pDatagram,0);
						*/
					}

				}
			}
			else
			{
				return 0;
			}
			
			DWORD dwRet = WaitForSingleObject(pWinSocket->m_eStopSendThread,0);
			
			if(dwRet == WAIT_OBJECT_0)
			{
				TRACE("WinSocket SendThread beendet\n");
				return 0;
			}
			
		}
		Sleep(10);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////

UINT CWinSocket::GetMaxMessageSize()
{

	int iSize = sizeof(unsigned int);
	unsigned int uMsgSize=0;
	int iErr;

	iErr=
	getsockopt(m_hSocket,
		       SOL_SOCKET,
			   0x2003,
			   (char*)&uMsgSize,
			   &iSize);
	
	if(iErr == SOCKET_ERROR)
	{
      TRACE("CWinSocket::GetMaxMessageSize:");
	  PostError(WSAGetLastError());
	  return -1;
	}

  	 return uMsgSize;
}

/////////////////////////////////////////////////////////////////////////////////

int CWinSocket::GetRecvBufferSize()
{
	int iSize = sizeof(unsigned int);
	unsigned int uMsgSize=0;
	int iErr;

	iErr=
	getsockopt(m_hSocket,
		       SOL_SOCKET,
			   SO_RCVBUF,
			   (char*)&uMsgSize,
			   &iSize);
	
	if(iErr == SOCKET_ERROR)
	{
      TRACE("CWinSocket::GetRecvBufferSize:");
	  PostError(WSAGetLastError());
	}
	return uMsgSize;
}

/////////////////////////////////////////////////////////////////////////////////

int CWinSocket::GetSendBufferSize()
{
	int iSize = sizeof(unsigned int);
	unsigned int uMsgSize=0;
	int iErr;

	iErr = getsockopt(m_hSocket,
					  SOL_SOCKET,
					  SO_SNDBUF,
					  (char*)&uMsgSize,
					  &iSize);
	
	if(iErr == SOCKET_ERROR)
	{
      TRACE("CWinSocket::GetSendBufferSize:");
	  PostError(WSAGetLastError());
	  return -1;
	}
	return uMsgSize;
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternSend(DATAGRAM *pDatagram)
{
	UINT uOverallLen=0;
	BYTE *pbHelpBuffer=NULL;
	int iError;

	if(m_iSocketType==UDP)
	{
		m_CritSec.Lock();
		m_ListDatagram.AddTail(pDatagram);
		m_CritSec.Unlock();
	}
	else if(m_iSocketType==TCP)
	{
		if(pDatagram->bFlag == FLAG_BANDWIDTH)
		{
			iError = SendTCPBandwidth(pDatagram);
		}
		else if(pDatagram->bFlag == FLAG_PING)
		{
			iError = SendTCPPing(pDatagram);
		}
		else
		{
			m_CritSec.Lock();

			if(m_pbSTREAMData)
			{
				if(pDatagram->bDataBuffer)
				{
					uOverallLen = m_uSTREAMDataLen + pDatagram->uDataBufferSize;
					pbHelpBuffer=new BYTE[uOverallLen];
					CopyMemory(pbHelpBuffer,m_pbSTREAMData,m_uSTREAMDataLen);
					CopyMemory(pbHelpBuffer+pDatagram->uDataBufferSize,pDatagram->bDataBuffer,pDatagram->uDataBufferSize);
					m_uSTREAMDataLen = uOverallLen;
					delete(m_pbSTREAMData);
					m_pbSTREAMData = pbHelpBuffer;
				}
			}
			else
			{
				m_pbSTREAMData=new BYTE[pDatagram->uDataBufferSize];
				CopyMemory(m_pbSTREAMData,pDatagram->bDataBuffer,pDatagram->uDataBufferSize);
				m_uSTREAMDataLen=pDatagram->uDataBufferSize;
			}
			m_CritSec.Unlock();
			DELETE_DATAGRAM(pDatagram);
		}
	}
	else
	{
		DELETE_DATAGRAM(pDatagram);
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::Send(DATAGRAM *pDatagram, CString sRecvIP, UINT uRecvPort,BYTE	bWayOfSendFlag,int iTTL)
{
	 if(!pDatagram)
	 {
	   return;
	 }
	 unsigned long IP = inet_addr(sRecvIP);
	 CString sIP;

	 if(IP==INADDR_NONE) //keine IP adresse
	 {
		 //GetHostbyName;
		sIP = GetIPByHostname(sRecvIP);
		sRecvIP=sIP;
	 }
	 if(m_iSocketType == UDP)
	   {
		   if(bWayOfSendFlag == FLAG_MULTICAST)
		   {
		  		pDatagram->ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(sRecvIP);
				pDatagram->ReceiverAddr.sin_port             = htons(uRecvPort);
				pDatagram->ReceiverAddr.sin_family           = AF_INET;	 
				pDatagram->bFlag						     = FLAG_MULTICAST;
				pDatagram->iValue							 = iTTL;
		   }
		   else if(bWayOfSendFlag == FLAG_BROADCAST)
		   {
		  		pDatagram->ReceiverAddr.sin_addr.S_un.S_addr = htonl ( INADDR_BROADCAST );
				pDatagram->ReceiverAddr.sin_port             = htons(uRecvPort);
				pDatagram->ReceiverAddr.sin_family           = AF_INET;	
				pDatagram->bFlag						     = FLAG_BROADCAST;
		   }
		   else if(bWayOfSendFlag == FLAG_MULTIPLEX)
		   {
				pDatagram->bFlag						     = FLAG_MULTIPLEX;
		   }
		   else
		   {
			   if(sRecvIP=="")
			   {
					//TODO
				   DELETE_DATAGRAM(pDatagram);
				   return;
			   }
	  		   pDatagram->ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(sRecvIP);
			   pDatagram->ReceiverAddr.sin_port             = htons(uRecvPort);
			   pDatagram->ReceiverAddr.sin_family           = AF_INET;
			   pDatagram->bFlag						        = FLAG_NORMAL;
		   }
		}
	   else if(m_iSocketType == TCP)
	   {

	   }
	   else
	   {
		   //TODO
		   return;
	   }
	   PostThreadMessage(TM_MESSAGE,
						 (UINT)pDatagram,
						 MAKELONG(M_SEND_DATA,0));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::JoinMulticastGroup(CString sMulticastIP)
{

	PostThreadMessage(TM_MESSAGE,
		              (UINT)inet_addr(sMulticastIP),
					  MAKELONG(M_JOIN_MULTICAST,0));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternJoinMulticastGroup(unsigned long iaddr)
{

	struct ip_mreq     mreq;

	int iErr;

	if(!m_hSocket)
	{
      TRACE("CWinSocket::OnJoinMulticastGroup: No Handle");
	  return;
	}
	if(!m_bSocketIsBind)
	{
		//TODO
        TRACE("CWinSocket::OnJoinMulticastGroup: Socket is not bind");
		return;
	}
	mreq.imr_multiaddr.S_un.S_addr = iaddr;
	mreq.imr_interface.S_un.S_addr = INADDR_ANY;

	iErr = setsockopt(m_hSocket,
		              IPPROTO_IP,
					  IP_ADD_MEMBERSHIP,
					  (char FAR*)&mreq,
					  sizeof(mreq));

	if(iErr == SOCKET_ERROR)
	{
      TRACE("CWinSocket::OnJoinMulticastGroup: setsockopt");
	  PostError(WSAGetLastError());
	}
	else
	{
		Post(TWM_MULTICAST_JOINED,0,(LONG)iaddr);
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::DropFromMulticastGroup(CString sMulticastIP)
{
	PostThreadMessage(TM_MESSAGE,
		              (UINT)inet_addr(sMulticastIP),
					  MAKELONG(M_DROP_MULTICAST,0));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternDropFromMulticastGroup(long iaddr)
{

	struct ip_mreq mreq;

	int iErr;

	if(!m_hSocket)
	{
      TRACE("CWinSocket::OnDropFromMulticastGroup: No Socket Handle");
      return;
	}

	mreq.imr_multiaddr.S_un.S_addr = iaddr;
	mreq.imr_interface.S_un.S_addr = INADDR_ANY;

	iErr = setsockopt(m_hSocket,
		              IPPROTO_IP,
					  IP_DROP_MEMBERSHIP,
					  (char *)&mreq,
					  sizeof(mreq));

	if(iErr == SOCKET_ERROR)
	{
	      TRACE("CWinSocket::OnDropFromMulticastGroup: setsockopt ");
		  PostError(WSAGetLastError());
		  return;
	}
	else
	{
		Post(TWM_MULTICAST_DROPPED,0,(LONG)iaddr);
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::SetDuplexMode(BOOL bSend, BOOL bRecv)
{

 	BYTE bMode=0x00;
	
	if(bSend)
		bMode = 0x01;
	if(bRecv)
		bMode = 0x02;

	if(bSend && bRecv)
		bMode = 0x03;

	if(m_hSocket)
	{
		PostThreadMessage(TM_MESSAGE,
			              (LONG)bMode,
						  MAKELONG(M_DUPLEX_MODE,0));
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternSetMode(UINT uMode)
{
	//SEND    = 1
	//RECEIVE = 2
	//BOTH    = 3

	if(m_iSocketType == UDP)
	{
		if(!m_bSocketIsBind)
		{
			if(!m_bSocketIsBind)
			{
				TRACE("CWinSocket::OnSetMode: Socket is not bind ");
				return;
			}
		}
	}
	switch(uMode)
	{
		case 0:									//Nichts
			if(m_pReceiveThread)
			{
				shutdown(m_hSocket,0x00);
			}
			if(m_pSendThread)
			{
				shutdown(m_hSocket,0x01);
			}
			break;

		case 1:									//Nur Senden		HALFDUPLEX
			if(m_pReceiveThread)
			{
				shutdown(m_hSocket,0x00);
				delete(m_pReceiveThread);
				m_pReceiveThread=NULL;
			}
			if(m_hSocket)
			{
				if(!m_pSendThread)
				{
 					m_pSendThread = AfxBeginThread(SendThreadFunc,
												   this,
												   THREAD_PRIORITY_NORMAL);
					m_pSendThread->m_bAutoDelete=TRUE;
				}
			}
			break;

		case 2:									//Nur Empfangen	    HALFDUPLEX
			if(m_pSendThread)
			{
				shutdown(m_hSocket,0x01);
				delete(m_pSendThread);
				m_pSendThread=NULL;
			}
			if(m_pReceiveThread)
			{
				shutdown(m_hSocket,0x00);
				delete(m_pReceiveThread);
				m_pReceiveThread=NULL;
			}

			if(m_hSocket)
			{
				if(!m_pReceiveThread)
				{
 					m_pReceiveThread = AfxBeginThread(ReceiveThreadFunc,
													  this,
													  THREAD_PRIORITY_NORMAL);
					m_pReceiveThread->m_bAutoDelete=TRUE;
				}
			}
			break;

		case 3:									//Senden und Empfangen FULLDUPLEX
			if(m_hSocket)
			{
				if(m_pSendThread)
				{
					shutdown(m_hSocket,0x01);
					delete(m_pSendThread);
					m_pSendThread=NULL;
				}
				if(!m_pSendThread)
				{
 					m_pSendThread = AfxBeginThread(SendThreadFunc,
												   this,
												   THREAD_PRIORITY_NORMAL);
					m_pSendThread->m_bAutoDelete=TRUE;
				}
			}
			if(m_hSocket)
			{
				if(m_pReceiveThread)
				{
					shutdown(m_hSocket,0x00);
					delete(m_pReceiveThread);
					m_pReceiveThread=NULL;
				}
				if(!m_pReceiveThread)
				{
 					m_pReceiveThread = AfxBeginThread(ReceiveThreadFunc,
													  this,
													  THREAD_PRIORITY_NORMAL);
					m_pReceiveThread->m_bAutoDelete=TRUE;
				}
			}
			break;
	}
	if(m_pReceiveThread)
	{
		Post(TWM_RECEIVING,(UINT)this,0);
	}
	if(m_pSendThread)
	{
		Post(TWM_SENDING,(UINT)this,0);
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::StopThreads()
{
	m_eStopSendThread.SetEvent();
	
	if(m_pSendThread)
	{
		DWORD dwRet = WaitForSingleObject(m_pSendThread->m_hThread,5000);
	}
	m_pSendThread=NULL;

	m_eStopReceiveThread.SetEvent();

	if(m_pReceiveThread)
	{
		DWORD dwRet = WaitForSingleObject(m_pReceiveThread->m_hThread,5000);
	}
	m_pReceiveThread=NULL;
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::SetReceiver(CWinThread *pWinThread, CWnd *pWnd)
{
	m_pMessageReceiver    = pWinThread;
	m_pWndMessageReceiver = pWnd;
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternBind(UINT uData)
{
	SOCKADDR_IN *local_sin = (SOCKADDR_IN *)uData;

	int iErr;

	iErr = bind(m_hSocket,(struct sockaddr*)local_sin,sizeof(struct sockaddr));

	m_LocalAddr  = local_sin;

	if(iErr == SOCKET_ERROR)
	{
      TRACE("CWinSocket::OnBind ");
	  PostError(WSAGetLastError());
      m_bSocketIsBind=FALSE;
	  return;
	}
	else
	{
		Post(TWM_SOCKET_BIND,0,0);
		m_bSocketIsBind=TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::ReuseAddr(UINT uPort)
{
	PostThreadMessage(TM_MESSAGE,
		              uPort,
					  MAKELONG(M_REUSE_ADDR,0));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternReuseAddr(UINT uPort)
{
	int iErr;

	iErr = setsockopt(m_hSocket,
			          SOL_SOCKET,
					  SO_REUSEADDR,
					  (char*)&uPort,
					  sizeof(int));

	if(iErr == SOCKET_ERROR)
	{
      TRACE("CWinSocket::OnReuseAddr ");
	  PostError(WSAGetLastError());
	  return;
	}
	else
	{
		Post(TWM_REUSED,(UINT)uPort,0);
	}
}

/////////////////////////////////////////////////////////////////////////////////

CString CWinSocket::GetIPByHostname(CString sName)
{
	CString sIP;
	char *cp;
	BYTE b1,b2,b3,b4;

	struct hostent *phost = gethostbyname(sName);
	if(!phost)
	{
	  return "";
	}
	cp = phost->h_addr_list[0];
  
	b1 = *(BYTE*)cp;
	b2 = *((BYTE*)(++cp));
	b3 = *((BYTE*)(++cp));
	b4 = *((BYTE*)(++cp));

	sIP.Format("%i.%i.%i.%i",b1,b2,b3,b4);
	return sIP; 
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::Bind(CString sIP,UINT uPort)
{

	SOCKADDR_IN *local_sin= new SOCKADDR_IN;

	local_sin->sin_family      = AF_INET;
	local_sin->sin_port        = htons(uPort);
	local_sin->sin_addr.s_addr = (inet_addr(sIP));

	PostThreadMessage(TM_MESSAGE,
		              (UINT)local_sin,
					  MAKELONG(M_BIND_SOCKET,0));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::AddMultiplexMember(CString sMultiplexMemberIP, UINT uMultiplexMemberPort)
{
	PostThreadMessage(TM_MESSAGE,
		              (UINT)inet_addr(sMultiplexMemberIP),
					  MAKELONG(M_ADD_MULTIPLEX_MEMBER,uMultiplexMemberPort));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternAddMultiplexMember(unsigned long lInetAddr, UINT uPort)
{
	struct sockaddr_in* pSock_Addr = new struct sockaddr_in;

	pSock_Addr->sin_addr.S_un.S_addr = lInetAddr;
	pSock_Addr->sin_port             = htons(uPort);

	m_CritSec.Lock();
	m_ListMultiplexMember.AddTail(pSock_Addr);
	m_CritSec.Unlock();

	Post(TWM_MULTIPLEX_ADDED,0,(LONG)pSock_Addr);
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::DropMultiplexMember(CString sMultiplexMemberIP, UINT uMultiplexMemberPort)
{
	PostThreadMessage(TM_MESSAGE,
		              (UINT)inet_addr(sMultiplexMemberIP),
					  MAKELONG(M_DROP_MULTIPLEX_MEMBER,uMultiplexMemberPort));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternDropMultiplexMember(unsigned long lInetAddr, UINT uPort)
{

	POSITION posA;
	struct sockaddr_in *pAddr;

	posA = m_ListMultiplexMember.GetHeadPosition();

	while(posA)
	{
		pAddr = m_ListMultiplexMember.GetNext(posA);

		if(pAddr->sin_addr.S_un.S_addr == lInetAddr)
		{
			if(pAddr->sin_port == htons(uPort))
			{
			    delete pAddr;
			    m_ListMultiplexMember.RemoveAt(posA);  

				Post(TWM_MULTIPLEX_DROPPED,0,(LONG)lInetAddr);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////

UINT CWinSocket::ConnectThreadFunc(LPVOID pWinSockObject)
{

	CWinSocket *pWinSocket = (CWinSocket*)pWinSockObject;
	

	int iErr;

	iErr = connect(pWinSocket->m_hSocket,                          
				   (struct sockaddr*)&(pWinSocket->m_RemoteConnectedAddr),
  				   sizeof(struct sockaddr)                        
				  );

	if(iErr == SOCKET_ERROR)
	{
      TRACE("CWinSocket::ConnectThreadFunc ");
	  pWinSocket->PostError(WSAGetLastError());
	}
	else
	{
		pWinSocket->Post(TWM_CONNECTED,(UINT)pWinSocket,(LONG)pWinSocket->m_RemoteConnectedAddr.sin_addr.S_un.S_addr);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::Connect(CString sRemoteIP, UINT uRemotePort)
{
	unsigned long lIP = inet_addr(sRemoteIP);

	CString sIP;

	if(lIP==INADDR_NONE) //keine gültige IP adresse
	{
		sIP = GetIPByHostname(sRemoteIP);
		//TODO error
		lIP = inet_addr(sRemoteIP);
		m_RemoteConnectedAddr.sin_addr.S_un.S_addr = lIP;
		m_RemoteConnectedAddr.sin_port = htons(uRemotePort);
		m_RemoteConnectedAddr.sin_family=AF_INET;
	}
	PostThreadMessage(TM_MESSAGE,
					  0,
					  MAKELONG(M_CONNECT,0));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternConnect()
{
	//TODO error 
	if(!m_pConnectThread)
	{
 		m_pConnectThread = AfxBeginThread(ConnectThreadFunc,
										  this,
										  THREAD_PRIORITY_NORMAL);
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::PostError(int iErrorcode)
{
	Post(TWM_SOCKET_ERROR,(UINT)this,(LONG)iErrorcode);

	TRACE("-> CWinSocket Error, Errorcode: %i\n",iErrorcode);
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::Disconnect()
{
	PostThreadMessage(TM_MESSAGE,
		              0,
					  MAKELONG(M_DISCONNECT,0));
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternDisconnect()
{

	int iErr;

	m_RemoteConnectedAddr.sin_addr.S_un.S_addr = 0;
	m_RemoteConnectedAddr.sin_port             = 0;
	m_RemoteConnectedAddr.sin_family           = 0;

	iErr = connect(m_hSocket,                          
				   (struct sockaddr*)&(m_RemoteConnectedAddr),
  				   sizeof(struct sockaddr)                        
				  );

	if(iErr == SOCKET_ERROR)
	{
      TRACE("CWinSocket::OnDisconnect ");
	  PostError(WSAGetLastError());
	}
	else
	{
		Post(TWM_DISCONNECTED,(UINT)this,0);
	}
}

/////////////////////////////////////////////////////////////////////////////////

void CWinSocket::Ping(CString sDestIP, UINT uDestPort, int iTimeout)
{

	unsigned long lIP = inet_addr(sDestIP);
	m_iPingTimeout=iTimeout;

	CString sIP;

	if(lIP==INADDR_NONE) //keine gültige IP adresse
	{
		sIP = GetIPByHostname(sDestIP);
		//TODO error
		lIP = inet_addr(sDestIP);	

		m_PingDestAddr.sin_addr.S_un.S_addr = lIP;
		m_PingDestAddr.sin_port = htons(uDestPort);
		m_PingDestAddr.sin_family=AF_INET;
	}
	
	PostThreadMessage(TM_MESSAGE,
					  0,
					  MAKELONG(M_PING,0));
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternPing()
{
	DATAGRAM *pDatagram = new DATAGRAM;

	INIT_DATAGRAM(pDatagram);
   	pDatagram->bFlag=FLAG_PING;
  
	m_pPingThread = AfxBeginThread(PingThreadFunc,this,THREAD_PRIORITY_NORMAL);
	OnInternSend(pDatagram);
}

/////////////////////////////////////////////////////////////////////////////

UINT CWinSocket::PingThreadFunc(LPVOID pWinSockObject)
{
	
	CWinSocket *pWinSocket = (CWinSocket*)pWinSockObject;

	DWORD dwTick = GetTickCount();
	DWORD dwActualTick = 0L;

	while(1)
	{
		DWORD dwRet = WaitForSingleObject(pWinSocket->m_eStopPingThread,10);
		if(dwRet == WAIT_OBJECT_0)
		{
			TRACE("WinSocket PingThread beendet\n");
			return 0;
		}
		dwActualTick=GetTickCount();

		if((int)(dwActualTick-dwTick)>(pWinSocket->m_iPingTimeout))
		{
			pWinSocket->Post(TWM_PINGRETURN,(UINT)pWinSocket,NULL);
		}
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////

LONG* CWinSocket::GetLocalIP(int &iNrOfIP)
{
	char szHostName[128];
	CString str;

	if( gethostname(szHostName, 128) == 0 )
	{
		// Get host adresses
		struct hostent * pHost;
		int i;
		
		pHost = gethostbyname(szHostName);
		
		for( i = 0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ )
		{
			str.Empty();
			int j;
			
			for( j = 0; j < pHost->h_length; j++ )
			{
				CString addr;
				
				if( j > 0 )
					str += ".";
				
				addr.Format("%u", 
							(unsigned int)((unsigned char*)pHost->h_addr_list[i])[j]);
				str += addr;
			}

			m_lLocalIp[i]=inet_addr((const char*)str);
		}
		iNrOfIP=i;
	}
	return m_lLocalIp;
}

/////////////////////////////////////////////////////////////////////////////

UINT CWinSocket::GetLocalPort()
{
    struct sockaddr_in Sockaddr;
    int Sockaddr_len = sizeof(Sockaddr);
    int ierr;

    ierr = getsockname(m_hSocket,(struct sockaddr*)&Sockaddr,&Sockaddr_len);
	
    int iport = htons(Sockaddr.sin_port);
    return iport;
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::RequestBandwidth(CString sIP, UINT uPort)
{
	 unsigned long IP = inet_addr(sIP);
	 CString snIP;


	 DATAGRAM *pDatagram = new DATAGRAM;
	 INIT_DATAGRAM(pDatagram);


	 if(IP==INADDR_NONE) //keine IP adresse
	 {
		 //GetHostbyName;
		snIP = GetIPByHostname(sIP);
		sIP=snIP;

	 }

	 pDatagram->ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(sIP);
	 pDatagram->ReceiverAddr.sin_port             = htons(uPort);
	 pDatagram->ReceiverAddr.sin_family           = AF_INET;
	 pDatagram->bFlag							  = FLAG_BANDWIDTH;

	 BYTE* pcDestinationBuffer;

	 pDatagram->bDataBuffer     = new BYTE[4096];
	 pDatagram->uDataBufferSize = 4096;
	
	 pcDestinationBuffer=GetEncryptedDatagram();

	 CopyMemory(pDatagram->bDataBuffer,pcDestinationBuffer,4096);
	 pDatagram->uDataBufferSize=4096;
		
	 PostThreadMessage(TM_MESSAGE,
					  (UINT)pDatagram,
					  MAKELONG(M_SEND_DATA,0));
}

/////////////////////////////////////////////////////////////////////////////

BYTE* CWinSocket::GetEncryptedDatagram()
{

	if(m_pCryptData)
		return m_pCryptData;

	HRSRC hRc = FindResource(NULL, MAKEINTRESOURCE(IDR_CRYPTDATA), "CRYPT");
	if (hRc)
		{
			HGLOBAL hResource	= LoadResource(NULL, hRc);
			if (hResource)
			{
				m_pCryptData= (BYTE*)LockResource(hResource);
				return m_pCryptData;
			}
			else;
		//		TRACE("CryptData konnte nicht geladen werden\n");
		}
		else;
		//	TRACE("CryptData konnte nicht geladen werden\n");
 return NULL;
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternSocketCreated(UINT uParam, LONG lParam)
{
   OnSocketCreated();
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternSocketBind(UINT uParam, LONG lParam)
{
	OnSocketBind();
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternSocketClosed(UINT uParam, LONG lParam)
{
	OnSocketClosed(this);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternMulticastMembershipDropped(UINT uParam, LONG lParam)
{
	CString sIP;
	int iPort=0;

	//TODO
	OnMulticastMembershipDropped(sIP,iPort);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternMultiplexMembershipDropped(UINT uParam, LONG lParam)
{
	CString sIP;
	int iPort=0;

	//TODO
	OnMultiplexMembershipDropped(sIP,iPort);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternMulticastGroupJoined(UINT uParam, LONG lParam)
{
	CString sIP;
	int iPort=0;

	//TODO
	OnMulticastGroupJoined(sIP,iPort);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternListening(UINT uParam, LONG lParam)
{
	 CWinSocket* pSocket = (CWinSocket*)uParam;
	 UINT uPort = (UINT)lParam;
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternMultiplexMemberAdded(UINT uParam, LONG lParam)
{
	CString sIP;
	int iPort=0;

	//TODO
	OnMultiplexMemberAdded(sIP,iPort);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternConfirmBandwidth(UINT uParam, LONG lParam)
{
	CString sIP;
	int iThroughput=(int)uParam;
	//uParam int Byte/sec
	//lParam long iaddr
	//TODO
	sIP=inet_ntoa(*(struct in_addr*)&lParam);

	OnConfirmBandwidth(sIP,iThroughput);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternDataSend(UINT uParam, LONG lParam)
{
	DATAGRAM *pDatagram=(DATAGRAM*)uParam;

	OnDataSend(pDatagram);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternNewData(UINT uParam, LONG lParam)
{
	DATAGRAM *pDatagram=(DATAGRAM*)uParam;

	OnNewData(pDatagram);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternConnectionClosed(UINT uParam, LONG lParam)
{
	CWinSocket* pSocket = (CWinSocket*)uParam;

	OnConnectionClosed(pSocket);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternSending(UINT uParam, LONG lParam)
{
	CWinSocket* pSocket = (CWinSocket*)uParam;
	OnSending(pSocket);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternReceiving(UINT uParam, LONG lParam)
{
	CWinSocket* pSocket = (CWinSocket*)uParam;
	OnReceiving(pSocket);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternReused(UINT uParam, LONG lParam)
{
	int iPort = (int)uParam;
	OnReused(iPort);
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnInternPingReturn(UINT uParam, LONG lParam)
{
	CWinSocket* pSocket = (CWinSocket*)uParam;
	DATAGRAM *pDatagram=(DATAGRAM*)lParam;
	OnPing(pSocket,pDatagram);
}

///////////////////////////////////////////////////////////////////////////

void CWinSocket::OnSocketCreated()
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnSocketBind()
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnSocketClosed(CWinSocket* pSocket)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnMulticastMembershipDropped(CString sMulticastIP, int iMulticastPort)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnMultiplexMembershipDropped(CString sMulticastIP, int iMulticastPort)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnMulticastGroupJoined(CString sMulticastIP, int iMulticastPort)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnMultiplexMemberAdded(CString sMulticastIP, int iMulticastPort)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnConfirmBandwidth(CString sRemoteIP, int iThroughput)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnDataSend(DATAGRAM* pDatagram)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnNewData(DATAGRAM* pDatagram)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnConnectionClosed(CWinSocket* pSocket)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnSending(CWinSocket* pSocket)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnReceiving(CWinSocket* pSocket)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnReused(int iPort)
{

}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::OnPing(CWinSocket* pSocket, DATAGRAM* pDatagram)
{

}

/////////////////////////////////////////////////////////////////////////////

CString CWinSocket::GetHostName()
{

	char szHostName[128];

	if( gethostname(szHostName, 128) == 0 )
		return ((const char*)szHostName);

	return "NULL";
}

/////////////////////////////////////////////////////////////////////////////

void CWinSocket::Post(UINT uMessage, UINT uData, LONG lData)
{
   if(m_pMessageReceiver)
   {
		m_pMessageReceiver->PostThreadMessage(uMessage,uData,lData);
   }
   else if(m_pWndMessageReceiver)
   {
		m_pWndMessageReceiver->PostMessage(uMessage,(WPARAM)uData,(LPARAM)lData);
   }
   else
   {
   	    PostThreadMessage(uMessage,uData,lData);
   }

}



void CWinSocket::OnListening(CWinSocket *pSocket, UINT uPort)
{

}

int CWinSocket::SendOverUDP(DATAGRAM *pDatagram)
{
	int iError=0;
					//An Broadcastadressen senden
	if(pDatagram->bFlag == FLAG_BROADCAST)
	{
		iError = SendUDPBroadcast(pDatagram);
	}
					//An Multiplex Member senden
	else if(pDatagram->bFlag == FLAG_MULTIPLEX)
	{
		iError = SendUDPMultiplex(pDatagram);
	}
					//An Multicast Member senden	
	else if(pDatagram->bFlag == FLAG_MULTICAST)
	{
		iError = SendUDPMulticast(pDatagram);
	}
					//"Normal" senden
	else if(pDatagram->bFlag == FLAG_NORMAL)
	{
		iError = SendUDPNormal(pDatagram);					
	}
					//Bandbreitenanforderung senden
	else if(pDatagram->bFlag == FLAG_BANDWIDTH)
	{
		iError = SendUDPBandwidth(pDatagram);
	}
					//Pingrequest senden
	else if(pDatagram->bFlag == FLAG_PING)
	{
		iError = SendUDPPing(pDatagram);
	}

	return iError;
}

//------------------------------------------------------------------------

void CWinSocket::SendOverTCP(DATAGRAM *pDatagram)
{

}

//------------------------------------------------------------------------

int CWinSocket::SendUDPBroadcast(DATAGRAM *pDatagram)
{
	int iErr = 0;

	BOOL fBroadcast = TRUE;
					
	iErr = setsockopt (m_hSocket, 
					   SOL_SOCKET, 
					   SO_BROADCAST,
					   (CHAR *) &fBroadcast, 
					   sizeof ( BOOL ) 
					   );
						
	iErr = sendto(m_hSocket,
				  (const char*)pDatagram->bDataBuffer,
				  pDatagram->uDataBufferSize,                         
				  0,								//!MSG_DONTROUTE                    
				  (SOCKADDR *)&(pDatagram->ReceiverAddr),
				  sizeof ( SOCKADDR_IN )
				   );

	fBroadcast = FALSE;
						
	iErr = setsockopt ( m_hSocket, 
						SOL_SOCKET, 
						SO_BROADCAST,
						(CHAR *) &fBroadcast, 
						sizeof ( BOOL ) 
						);

	if(iErr != SOCKET_ERROR)
	{
		Post(TWM_DATA_SEND,(UINT)pDatagram,0);
	}

	return iErr;
}

//------------------------------------------------------------------------

int CWinSocket::SendUDPMultiplex(DATAGRAM *pDatagram)
{
	int iErr = 0;
	int iMember;

	if(!(m_ListMultiplexMember.IsEmpty()))
	{
		BOOL bSendError=FALSE;

		for(iMember = 1;iMember < m_ListMultiplexMember.GetCount();iMember ++)
		{
			POSITION pos = m_ListMultiplexMember.FindIndex(iMember-1);
			struct sockaddr_in *pAddr = m_ListMultiplexMember.GetAt(pos);

			iErr = sendto(m_hSocket,
						  (const char*)pDatagram->bDataBuffer,
						   pDatagram->uDataBufferSize,                         
						   0,								//!MSG_DONTROUTE                    
						   (struct sockaddr*)pAddr,
						   sizeof(*pAddr)
						   );
			if(iErr==SOCKET_ERROR)
			{
				 bSendError = TRUE;
			}
		}
		if(bSendError)
		{
			iErr=SOCKET_ERROR;
		}
		if(iErr != SOCKET_ERROR)
		{
			Post(TWM_DATA_SEND,(UINT)pDatagram,0);
		}
	}
	return iErr;
}

//------------------------------------------------------------------------

int CWinSocket::SendUDPMulticast(DATAGRAM *pDatagram)
{
	int iErr = 0;

	iErr = setsockopt(m_hSocket,
					  IPPROTO_IP,
					  IP_MULTICAST_TTL,
					  (char*)&pDatagram->iValue,
					  sizeof(int));

	iErr = sendto(m_hSocket,
				  (const char*)pDatagram->bDataBuffer,
				  pDatagram->uDataBufferSize,                         
				  0,								//!MSG_DONTROUTE                    
				  (struct sockaddr*)&(pDatagram->ReceiverAddr),
				  sizeof(pDatagram->ReceiverAddr)
				 );
	if(iErr != SOCKET_ERROR)
	{
		Post(TWM_DATA_SEND,(UINT)pDatagram,0);
	}

	return iErr;
}

//------------------------------------------------------------------------

int CWinSocket::SendUDPNormal(DATAGRAM *pDatagram)
{
	int iErr = 0;
	
	iErr = sendto(m_hSocket,
				  (const char*)pDatagram->bDataBuffer,
				  pDatagram->uDataBufferSize,                         
				  0,								//!MSG_DONTROUTE                    
				  (struct sockaddr*)&(pDatagram->ReceiverAddr),
				  sizeof(pDatagram->ReceiverAddr)
				  );

	if(iErr != SOCKET_ERROR)
	{
		Post(TWM_DATA_SEND,(UINT)pDatagram,0);
	}

	return iErr;
}

//------------------------------------------------------------------------

int CWinSocket::SendUDPBandwidth(DATAGRAM *pDatagram)
{
	int iErr = 0;

	if(((CString)(const char*)pDatagram->bDataBuffer).Left(3)=="#B:")
	{
	    //DWORD dwTime   =*(DWORD*)(((BYTE*)pDatagram->bDataBuffer)+11);
		//dwTime = dwTime	- (GetTickCount()-pDatagram->iValue);
		//*(DWORD*)(((BYTE*)pDatagram->bDataBuffer)+11)=dwTime;
	}
	else
	{
		CString sMessage;
		CopyMemory(pDatagram->bDataBuffer,(BYTE*)(const char*)"_B:",3);
		DWORD dwtime;
		dwtime = GetTickCount();
		*(DWORD*)(((BYTE*)pDatagram->bDataBuffer)+3)=dwtime;
	}

	iErr = sendto(m_hSocket,
				  (const char*)pDatagram->bDataBuffer,
				  pDatagram->uDataBufferSize,                         
				  0,								//!MSG_DONTROUTE                    
				  (struct sockaddr*)&(pDatagram->ReceiverAddr),
				  sizeof(pDatagram->ReceiverAddr)
				  );

	DELETE_DATAGRAM(pDatagram);
	
	return iErr;
}

//------------------------------------------------------------------------

int CWinSocket::SendUDPPing(DATAGRAM *pDatagram)
{
	CString sMessage;

	int iErr = 0;

	DWORD dwtime = GetTickCount();
	sMessage.Format("PING %l",dwtime);
	CopyMemory(pDatagram->bDataBuffer,sMessage,sMessage.GetLength()+1);


	iErr = sendto(m_hSocket,
				  (const char*)pDatagram->bDataBuffer,
				  pDatagram->uDataBufferSize,                         
				  0,								//!MSG_DONTROUTE                    
				  (struct sockaddr*)&(pDatagram->ReceiverAddr),
				  sizeof(pDatagram->ReceiverAddr)
				  );

	DELETE_DATAGRAM(pDatagram);

	return iErr;
}

/////////////////////////////////////////////////////////////////////////////

int CWinSocket::SendTCPBandwidth(DATAGRAM *pDatagram)
{

	int iErr = 0;

	CString sMessage;
	CopyMemory(pDatagram->bDataBuffer,(BYTE*)(const char*)"_B:",3);//markiert Anfang
	CopyMemory(pDatagram->bDataBuffer,(BYTE*)(const char*)":B_",3);//markiert Ende

	DWORD dwtime = GetTickCount();

	*(DWORD*)(((BYTE*)pDatagram->bDataBuffer)+3)=dwtime;


	iErr = send(m_hSocket,
			   (const char*)pDatagram->bDataBuffer,
			    pDatagram->uDataBufferSize,                         
			    0
			   );

	DELETE_DATAGRAM(pDatagram);
	
	return iErr;
}

/////////////////////////////////////////////////////////////////////////////

int CWinSocket::SendTCPPing(DATAGRAM *pDatagram)
{
  return 0;
}
