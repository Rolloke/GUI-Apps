#if !defined(AFX_WINSOCKET_H__13791303_A038_4547_B815_B4E8C0E080DB__INCLUDED_)
#define AFX_WINSOCKET_H__13791303_A038_4547_B815_B4E8C0E080DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinSocket.h : header file
//
#include <afxmt.h>
#include <afxtempl.h>



/*

 * Options for use with [gs]etsockopt at the IP level.

	IP_OPTIONS                 set/get IP per-packet options    
	IP_MULTICAST_IF            set/get IP multicast interface   
	IP_MULTICAST_TTL           set/get IP multicast timetolive  
	IP_MULTICAST_LOOP          set/get IP multicast loopback    
	IP_ADD_MEMBERSHIP          add  an IP group membership      
	IP_DROP_MEMBERSHIP         drop an IP group membership      
	IP_TTL                     set/get IP Time To Live          
	IP_TOS                     set/get IP Type Of Service       
	IP_DONTFRAGMENT            set/get IP Don't Fragment flag   

	IP_DEFAULT_MULTICAST_TTL   normally limit m'casts to 1 hop  
	IP_DEFAULT_MULTICAST_LOOP  normally hear sends if a member  
	IP_MAX_MEMBERSHIPS         per socket; must fit in one mbuf 

 * Option flags per-socket. Level: SOL_SOCKET
 
	SO_DEBUG                   turn on debugging info recording 
	SO_ACCEPTCONN              socket has had listen() 
	SO_REUSEADDR               allow local address reuse 
	SO_KEEPALIVE               keep connections alive 
	SO_DONTROUTE               just use interface addresses 
	SO_BROADCAST               permit sending of broadcast msgs 
	SO_USELOOPBACK             bypass hardware when possible 
	SO_LINGER                  linger on close if data present 
	SO_OOBINLINE               leave received OOB data in line 

    SO_DONTLINGER   (int)(~SO_LINGER)

 * Additional options.

    SO_SNDBUF					send buffer size 
    SO_RCVBUF					receive buffer size
    SO_SNDLOWAT					send low-water mark 
    SO_RCVLOWAT					receive low-water mark 
    SO_SNDTIMEO					send timeout 
    SO_RCVTIMEO					receive timeout 
    SO_ERROR					get error status and clear 
    SO_TYPE						get socket type

 * WinSock 2 extension -- new options

	SO_GROUP_ID					ID of a socket group 
	SO_GROUP_PRIORITY			the relative priority within a group
	SO_MAX_MSG_SIZE				maximum message size 
	SO_PROTOCOL_INFOA			WSAPROTOCOL_INFOA structure 
	SO_PROTOCOL_INFOW			WSAPROTOCOL_INFOW structure 
	
	UNICODE
	SO_PROTOCOL_INFO			SO_PROTOCOL_INFOW
	SO_PROTOCOL_INFO			SO_PROTOCOL_INFOA

 * UNICODE
	PVD_CONFIG               configuration info for service provider */



#define TM_MESSAGE				WM_USER + 1
#define TWM_SOCKET_CREATED		WM_USER + 2
#define TWM_NEW_CONNECTION		WM_USER + 3
#define TWM_NEW_DATA			WM_USER + 4	 
#define TWM_LISTENING			WM_USER + 5	 
#define TWM_CONNECTION_CLOSED	WM_USER + 6
#define TWM_STOP_LISTEN			WM_USER + 7	 
#define TWM_CONNECTED		    WM_USER + 8
#define TWM_SOCKET_ERROR		WM_USER + 9
#define TWM_DISCONNECTED        WM_USER + 10
#define TWM_BANDWIDTH			WM_USER + 11
#define TWM_SOCKET_BIND			WM_USER + 12
#define TWM_SOCKET_CLOSED		WM_USER + 13
#define TWM_MULTICAST_DROPPED   WM_USER + 14
#define TWM_MULTIPLEX_DROPPED	WM_USER + 15
#define TWM_MULTICAST_JOINED	WM_USER + 16
#define TWM_MULTIPLEX_ADDED     WM_USER + 17
#define TWM_DATA_SEND			WM_USER + 18
#define TWM_SENDING				WM_USER + 19
#define TWM_RECEIVING			WM_USER + 20
#define TWM_REUSED				WM_USER + 21
#define TWM_PINGRETURN			WM_USER + 25

#define M_CREATE_SOCKET			  1
#define M_LISTEN				  3

#define M_ON_SEND				  8
#define M_ATTACH_HANDLE			  9
#define M_RECEIVE				 10
#define M_ON_ACCEPT				 11
#define M_CLOSE_SOCKET			 12  
#define M_DESTROY_SOCKET		 13
#define M_SEND_DATA				 14

#define M_JOIN_MULTICAST		 16
#define M_DROP_MULTICAST		 17
#define M_DUPLEX_MODE			 18
#define M_REUSE_ADDR			 19
#define M_BIND_SOCKET			 20 
#define M_ADD_MULTIPLEX_MEMBER   21
#define M_DROP_MULTIPLEX_MEMBER  22
#define M_CONNECT				 23
#define M_DISCONNECT			 24
#define M_PING					 25

#define DUPLEX_SEND    (BYTE)0x01
#define DUPLEX_RECEIVE (BYTE)0x02
#define DUPLEX_BOTH    (BYTE)0x03

#define UDP			   (BYTE)0x01
#define TCP			   (BYTE)0x02
#define FLAG_BROADCAST (BYTE)0x03
#define FLAG_MULTIPLEX (BYTE)0x04
#define FLAG_NORMAL	   (BYTE)0x05
#define FLAG_MULTICAST (BYTE)0x06
#define FLAG_BANDWIDTH (BYTE)0x07
#define RAW			   (BYTE)0x08
#define FLAG_PING	   (BYTE)0x09

#define MAX_PENDING_CONNECTS  5



//#define RECEIVE_BUFFER_SIZE   10000


typedef struct DATAGRAM {

	DWORD  dwWinSockAdress;				//Adresse des sendenen WinsocketThreads
	struct sockaddr_in  SenderAddr;		//Absender Adresse
	struct sockaddr_in  ReceiverAddr;	//Empfänger Adresse

	BYTE* bDataBuffer;					//Empfangene oder zu sendende Daten
	UINT  uDataBufferSize;				//Grösse des Datenpuffers in Byte

	LPSYSTEMTIME pSystemTimeStamp;		//Zeitstempel des Datenpuffers (Eingangszeit)
	BYTE         bFlag;					//Flag für interne Verarbeitung
	DWORD	     iValue;
} Datagram;


inline void DELETE_DATAGRAM(DATAGRAM *pDatagram)
{
   delete pDatagram->bDataBuffer;
   delete pDatagram->pSystemTimeStamp;

   delete pDatagram;
   pDatagram =NULL;
}

inline void INIT_DATAGRAM(DATAGRAM *pDatagram)
{
	pDatagram->bDataBuffer     = NULL;
	pDatagram->uDataBufferSize = 0L;
	pDatagram->dwWinSockAdress = 0L;
	pDatagram->pSystemTimeStamp= NULL;
	pDatagram->bFlag		   = 0;
	pDatagram->iValue		   = 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CWinSocket thread


class CWinSocket : public CWinThread
{

	DECLARE_DYNCREATE(CWinSocket)

//Construction/Destruction

	CWinSocket(CWinThread  *pMessageReceiver = NULL,CWnd *pWndMessageReceiver = NULL); 
	virtual ~CWinSocket();


// Operations


/////////////////////////////////////////////////////////////////////////////////
// "SocketBlackbox" Interface INPUT											   //
//																			   //
//																			   //
//																			   //
public:


   	void Create(int iType,int iProtocol = 0);
	void ReuseAddr(UINT uPort);
	void Bind(CString sIP,UINT uPort);
	void Connect(CString sRemoteIP, UINT uRemotePort);
	void Disconnect();
	void RequestBandwidth(CString sIP, UINT uPort);
	void CloseSocket();
	void Listen();
	void SetDuplexMode(BOOL bSend, BOOL bRecv);

	void AddMultiplexMember(CString sMultiplexMemberIP, UINT uMultiplexPort);
	void DropMultiplexMember(CString sMultiplexMemberIP, UINT uMultiplexMemberPort);

	void JoinMulticastGroup(CString sMulticastIP);
	void DropFromMulticastGroup(CString sMulticastIP);

	void SetReceiver(CWinThread *pWinThread=NULL, CWnd* pWnd=NULL);

	void Send(DATAGRAM *pDatagram, 
		      CString sRecvIP="",
			  UINT uRecvPort=0,
			  BYTE bWayOfSendFlag=FLAG_NORMAL,
			  int iTTL=1);

	void Ping(CString sDestIP, UINT uDestPort,int iTimeout=10000/*ms*/);	

	CString GetHostName();
	UINT  GetLocalPort();
	LONG* GetLocalIP(int &iNrOfIP);

//																			   //
//																			   //
// "SocketBlackbox" Interface OUTPUT										   //
//																			   //
//																			   //

protected:
				 //Socket ist im Listen Modus auf Port uPort
	virtual void OnListening(CWinSocket *pSocket, UINT uPort);
				 //Socket wurde erfolgreich an IP und Port gebunden
	virtual void OnSocketBind();
				 //Socketobject wurde erfolgreich angelegt
	virtual void OnSocketCreated();
				 //Socket wurde geschlossen	über API closesocket
	virtual void OnSocketClosed(CWinSocket* pSocket);
				 //Multiplex Mitglied wurde aus Grupper 
	virtual void OnMultiplexMembershipDropped(CString sMulticastIP, int iMulticastPort);
	virtual void OnMulticastMembershipDropped(CString sMulticastIP, int iMulticastPort);
	virtual void OnMulticastGroupJoined(CString sMulticastIP, int iMulticastPort);
	virtual void OnMultiplexMemberAdded(CString sMulticastIP, int iMulticastPort);
	virtual void OnConfirmBandwidth(CString sRemoteIP, int iThroughput);
	virtual void OnDataSend(DATAGRAM* pDatagram);
	virtual void OnNewData(DATAGRAM* pDatagram);
	virtual void OnConnectionClosed(CWinSocket* pSocket);
	virtual void OnSending(CWinSocket* pSocket);
	virtual void OnReceiving(CWinSocket* pSocket);
	virtual void OnPing(CWinSocket* pSocket, DATAGRAM* pDatagram);
	virtual void OnReused(int iPort);  

//																			//
//																			//
//																			//
//																			//
//////////////////////////////////////////////////////////////////////////////
			 ////											  ////
		     ////											  ////
			 ////											  ////
//////////////////////////////////////////////////////////////////////////////
// "SocketBlackbox"															//
//																			//
//																			//
//  																		//
private:

	int SendTCPPing(DATAGRAM *pDatagram);
	int SendTCPBandwidth(DATAGRAM *pDatagram);
	int SendUDPPing(DATAGRAM *pDatagram);
	int SendUDPBandwidth(DATAGRAM *pDatagram);
	int SendUDPNormal(DATAGRAM *pDatagram);
	int SendUDPMulticast(DATAGRAM *pDatagram);
	int SendUDPMultiplex(DATAGRAM *pDatagram);
	int SendUDPBroadcast(DATAGRAM *pDatagram);
	void SendOverTCP(DATAGRAM *pDatagram);
	int SendOverUDP(DATAGRAM *pDatagram);

	BOOL InitInstance();
	int ExitInstance();

	void CancelBlockingCall(BOOL bSends=FALSE, BOOL bReceives=FALSE);
	void Attach(SOCKET hSocket);
	void ExitThread();

	static UINT AcceptThreadFunc(LPVOID pWinSockObject);
	static UINT ReceiveThreadFunc(LPVOID pWinSockObject);
	static UINT SendThreadFunc(LPVOID pWinSockObject);
	static UINT PingThreadFunc(LPVOID pWinSockObject);

	afx_msg void OnInternMessage(UINT uData,LONG lMessage);
	afx_msg void OnInternSocketBind(UINT uParam, LONG lParam);
	afx_msg void OnInternSocketCreated(UINT uParam, LONG lParam);
	afx_msg void OnInternSocketClosed(UINT uParam, LONG lParam);
	afx_msg void OnInternMulticastMembershipDropped(UINT uParam, LONG lParam);
	afx_msg void OnInternMultiplexMembershipDropped(UINT uParam, LONG lParam);
	afx_msg void OnInternMulticastGroupJoined(UINT uParam, LONG lParam);
	afx_msg void OnInternMultiplexMemberAdded(UINT uParam, LONG lParam);
	afx_msg void OnInternConfirmBandwidth(UINT uParam, LONG lParam);
	afx_msg void OnInternDataSend(UINT uParam, LONG lParam);
	afx_msg void OnInternNewData(UINT uParam, LONG lParam);
	afx_msg void OnInternConnectionClosed(UINT uParam, LONG lParam);
	afx_msg void OnInternSending(UINT uParam, LONG lParam);
	afx_msg void OnInternReceiving(UINT uParam, LONG lParam);
	afx_msg void OnInternReused(UINT uParam, LONG lParam);
	afx_msg void OnInternPingReturn(UINT uParam, LONG lParam);
	afx_msg void OnInternListening(UINT uParam, LONG lParam);

	DECLARE_MESSAGE_MAP()

	void OnInternPing();
	void OnInternDisconnect();
	void OnInternConnect();
	void OnInternDropMultiplexMember(unsigned long lInetAddr, UINT uPort);
	void OnInternAddMultiplexMember(unsigned long lInetAddr, UINT uPort);
	void OnInternReuseAddr(UINT uPort);
	void OnInternBind(UINT uPort);
	void OnInternSetMode(UINT uMode);
	void OnInternDropFromMulticastGroup(long iaddr);
	void OnInternJoinMulticastGroup(unsigned long iaddr);
	void OnInternCreate(UINT uType, int iProtocol);
	void OnInternListen();	 
	void OnInternAccept(SOCKET hSocket);
	void OnInternAttachHandle(SOCKET hSockethandle);
	void OnInternStopListen();
	void OnInternCloseSocket();
	void OnInternSend(DATAGRAM *pDatagram);

	void PostError(int iErrorcode);
	void Post(UINT uMessage, UINT uData, LONG lData);

	DWORD GetTicks();
	int m_iPingTimeout;
	BYTE* m_pCryptData;
	BYTE* GetEncryptedDatagram();

	static UINT ConnectThreadFunc(LPVOID pWinSockObject);
	int PeekInBuffer();
	CString GetIPByHostname(CString sName);	

	BOOL m_bSocketIsBind;

	CEvent m_eStopSendThread;
	CEvent m_eStopReceiveThread;
	CEvent m_eStopPingThread;

	void StopThreads();
	
	struct sockaddr_in m_MCastDestinationAdr;


	UINT m_uSTREAMDataLen;
	BYTE *m_pbSTREAMData;

	UINT m_uMaxSendBufferSize;	//Beim Erreichen dieser Grösse wird der Buffer an die nächste Schicht weitergeleitet
	UINT m_uMaxRecvBufferSize;
	UINT m_uMaxUDSize;			//Maximale Grösse eines Datagrams im UDP
	UINT GetMaxMessageSize();

	int GetSendBufferSize();
	int GetRecvBufferSize();


	CCriticalSection m_CritSec;
	

	CList <DATAGRAM*,DATAGRAM*> m_ListDatagram;			 
	CList <struct sockaddr_in*,struct sockaddr_in*> m_ListMultiplexMember;
	LONG m_lLocalIp[10];

	struct sockaddr_in *m_LocalAddr;
	struct sockaddr_in m_RemoteConnectedAddr;
	struct sockaddr_in m_PingDestAddr;

	CWinThread* m_pAcceptThread;
	CWinThread* m_pReceiveThread;
	CWinThread* m_pSendThread;
	CWinThread* m_pConnectThread;
	CWinThread* m_pPingThread;

	CWinThread* m_pMessageReceiver;
	CWnd*       m_pWndMessageReceiver;
	
	int m_iSocketType;
	SOCKET m_hSocket;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINSOCKET_H__13791303_A038_4547_B815_B4E8C0E080DB__INCLUDED_)
