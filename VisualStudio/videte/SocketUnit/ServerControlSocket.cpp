/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ServerControlSocket.cpp $
// ARCHIVE:		$Archive: /Project/SocketUnit/ServerControlSocket.cpp $
// CHECKIN:		$Date: 24.07.06 16:46 $
// VERSION:		$Revision: 56 $
// LAST CHANGE:	$Modtime: 24.07.06 15:32 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
 
#include "stdafx.h"

#include "ServerControlSocket.h"
#include "SocketUnitDoc.h"
#include "SocketUnit.h"
#include "CipcPipeSocket.h"
#include "DataSocket.h"
#include "ConnectionThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CServerControlSocket::CServerControlSocket(
					CSocketUnitDoc *pDoc,
					LPCTSTR shmName
					)
	: CIPCServerControl ( shmName, TRUE)
{
	m_pDoc = pDoc;
	m_pConnectionThread = NULL;
	SetTimeoutCmdReceive(60*1000);
	SetConnectRetryTime(50);
}

/////////////////////////////////////////////////////////////////////////////

CServerControlSocket::~CServerControlSocket()
{
}   

/////////////////////////////////////////////////////////////////////////////

BOOL CServerControlSocket::CheckIfClientIsStillListening(CConnectionThread* pOldThread)
{
	BOOL bRet = FALSE;
	m_csConnection.Lock();
	if (pOldThread == m_pConnectionThread)
	{
		bRet = TRUE;
	}
	m_csConnection.Unlock();
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////

void CServerControlSocket::OnReadChannelFound()
{
	CIPC::OnReadChannelFound();
}

/////////////////////////////////////////////////////////////////////////////

void CServerControlSocket::OnRequestDisconnect()
{
	// if client has cancelled, can we cancel the fetch?
	// gf yes, indirect via CheckIfClientIsStillListening()
	TRACE(_T("ServerControl: Client has disconnected\n"));
	m_csConnection.Lock();
	m_pConnectionThread = NULL;
	m_csConnection.Unlock();
}

/////////////////////////////////////////////////////////////////////////////

void CServerControlSocket::CreateConnection(const CConnectionRecord &cIn,
											CIPCType cipcType
											)
{
	CConnectionRecord c(cIn);	// local copy, might need a modification in case of routing
	CString sIPAdapter;
	if (MyGetApp()->GetAllowOutgoingCalls()==FALSE) 
	{
		// no outgoing calls at all!
		WK_TRACE(_T("Outgoing call denied\n"));

		CString sMessage;
		sMessage.LoadString(IDS_NO_OUTGOING);

		DWORD dwCmd = SRV_CONTROL_REQUEST_INPUT_CONNECTION;
		switch (cipcType)
		{
			case CIPC_INPUT_CLIENT:
					dwCmd = SRV_CONTROL_REQUEST_INPUT_CONNECTION;
				break;
			case CIPC_OUTPUT_CLIENT:
					dwCmd = SRV_CONTROL_REQUEST_OUTPUT_CONNECTION;
				break;
			case CIPC_DATABASE_CLIENT:
					dwCmd = SRV_CONTROL_REQUEST_DB_CONNECTION;
				break;
			case CIPC_ALARM_SERVER:
					dwCmd = SRV_CONTROL_REQUEST_ALARM_CONNECTION;
				break;
			case CIPC_AUDIO_CLIENT:
					dwCmd = SRV_CONTROL_REQUEST_AUDIO_CONNECTION;
				break;
		}
		DoIndicateError(dwCmd,
						SECID_NO_ID, 
						CIPC_ERROR_UNABLE_TO_CONNECT,
						0,
						sMessage
						);
	}
	else
	{
		CString sHostName = c.GetDestinationHost();

		// check for path
		int ix = sHostName.Find(ROUTING_CHAR);
		if (ix != -1) 
		{
			// a routing address
			// don't touch Destination host now
			// the receiver will do that
			// but calc the next hostname
			sHostName = sHostName.Left(ix);
		}

		sHostName.TrimLeft();
		// drop leading tcp:
		if (sHostName.Find(_T("tcp:"))!=-1)
		{
			sHostName = sHostName.Mid(4);
		}
		else
		{
			WK_TRACE_ERROR(_T("Missing leading tcp: in '%s'\n"), LPCTSTR(sHostName));
		}
// gf Warum nur bei DTS?
#ifdef _DTS
		sIPAdapter = ResolveIPAdapter(sHostName);
#endif
	
		m_csConnection.Lock();
		m_pConnectionThread = new CConnectionThread(m_pDoc,
													cipcType,
													sHostName,
													sIPAdapter,
													c
													);
		m_pDoc->AddNewThread(m_pConnectionThread);
		
		m_pConnectionThread->CreateThread();	// activate the thread
		m_csConnection.Unlock();

// This waiting while not init may last up to 1 min(!) if remote host is not available!
// Blocks the whole ServerControl!
// And does NOTHING afterwards - USELESS!
/*		
		int iNumRetries=0;
		while (!(m_pConnectionThread->ThreadsInitDone()) && (iNumRetries<4000))
		{
			Sleep(10);	// max 1000 retries
			iNumRetries++;
		}
		if (iNumRetries<4000)
		{
			
		}
		else
		{
			// FAILED
			WK_TRACE_ERROR(_T("Failed to start connection thread\n"));
			m_pThread = NULL;
		}
*/
	}
}

/////////////////////////////////////////////////////////////////////////////

void CServerControlSocket::OnRequestInputConnection(const CConnectionRecord &c)
{
	CreateConnection(c,CIPC_INPUT_CLIENT);
	// client pipe is created later on, see Protocol
}

/////////////////////////////////////////////////////////////////////////////

void CServerControlSocket::OnRequestOutputConnection(const CConnectionRecord &c)
{
	CreateConnection(c,CIPC_OUTPUT_CLIENT);
}

/////////////////////////////////////////////////////////////////////////////
void CServerControlSocket::OnRequestAudioConnection(const CConnectionRecord &c)
{
	CreateConnection(c,CIPC_AUDIO_CLIENT);
}
//////////////////////////////////////////////////////////////////////////

void CServerControlSocket::OnRequestDataBaseConnection(const CConnectionRecord &c)
{
	CreateConnection(c,CIPC_DATABASE_CLIENT);
}

/////////////////////////////////////////////////////////////////////////////

void CServerControlSocket::OnRequestAlarmConnection(const CConnectionRecord &c)
{
	CreateConnection(c,CIPC_ALARM_SERVER);
}

/////////////////////////////////////////////////////////////////////////////

void CServerControlSocket::OnRequestServerReset(const CConnectionRecord &c)
{
	// NOT YET check permission/user/password
	WK_TRACE(_T("Reset request received. Shutting down....\n"));
	DoConfirmServerReset();
	Sleep(200);	// give the confirm a chance

	// now end our app save
	// CAVEAT can't use AfxGetMainWnd here, since we are in the CIPC worker thread
	// but m_pMainWnd works fine :-)
	CWnd* pWnd = AfxGetApp()->m_pMainWnd;
	if (pWnd && pWnd->m_hWnd && ::IsWindow(pWnd->m_hWnd))
	{
		pWnd->PostMessage(WM_COMMAND,ID_APP_EXIT);
	}
	else
	{
		WK_TRACE(_T(" no main wnd found to post something\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////////////

CString CServerControlSocket::ResolveIPAdapter(CString sRemoteIP)
{
	CString sAdapterIP;
	int iNrOfIP=0;

	CIPInterface::m_sAdapterIP=_T("");
	CIPInterface::m_iNrOfConnectingInterface=0;
	CIPInterface::m_eConnected.ResetEvent();
	
	//1. Eintrag schon in einem IP-Book vorhanden?
#ifdef _DTS
	CIPBook::m_MapServerList.Lookup(sRemoteIP,sAdapterIP);
#endif
	//2. Wenn nein, dann über jeden vorhandenen IP-Adapter ein connect versuchen.

	if(sAdapterIP==_T(""))
	{
		WK_TRACE(_T("[CServerControlSocket::ResolveIPAdapter]Remote Station steht nicht im IPBook.\n"));
		WK_TRACE(_T("[CServerControlSocket::ResolveIPAdapter]Verbindung wird geortet...\n"));
		CIPInterface* pIPInterface[10];

		for (int i=0;i<10;i++)
		{
			pIPInterface[i] = NULL;
		}

		GetLocalIPList(iNrOfIP);

		WK_TRACE(_T("%d local IP Adapters found\n"),iNrOfIP);
		if (iNrOfIP==1)
		{
			sAdapterIP = inet_ntoa(*(struct in_addr*)&m_lLocalIp[0]);
			WK_TRACE(_T("only one adapter found use it <%s>\n"),sAdapterIP);
		}
		else
		{				
			for(int iN=0; iN<iNrOfIP; iN++)
			{								//Hier wird auf jedem IP-Adapter ein Verbindungsthread gestartet
				CString sAIP =  inet_ntoa(*(struct in_addr*)&m_lLocalIp[iN]);
				WK_TRACE(_T("TRY Connection on CIPCInterface %d. <%s>\n"),iN,sAIP);
				pIPInterface[iN] = new CIPInterface(sAIP);
				pIPInterface[iN]->TryToConnect(sRemoteIP);
			}
											//Wir warten bis einer der Adapter eine Verbindung herstellen konnte.
											//maximal jedoch 40s
			DWORD dwRet=WaitForSingleObject(CIPInterface::m_eConnected,40000);
			if(dwRet==WAIT_OBJECT_0)
			{					
				sAdapterIP = CIPInterface::m_sAdapterIP;
				if(sAdapterIP!=_T(""))
				{							//Wir tragen die gefundene IP Adresse in der Serverliste ein
	#ifdef _DTS
					CIPBook::m_MapServerList.SetAt(sRemoteIP,sAdapterIP);
	#endif
					WK_TRACE(_T("[CServerControlSocket::ResolveIPAdapter]Verbindung geortet...\n"));
				}
				else
				{
					WK_TRACE(_T("[CServerControlSocket::ResolveIPAdapter]Verbindung nicht geortet...\n"));
				}
			}
			else
			{
				WK_TRACE(_T("[CServerControlSocket::ResolveIPAdapter]Verbindung nicht geortet...\n"));
			}
		}
		for(int iN=0; iN<iNrOfIP; iN++)
		{ 
			WK_DELETE(pIPInterface[iN]);
		}
	}

	return sAdapterIP;
}

/////////////////////////////////////////////////////////////////////////////////////

void CServerControlSocket::GetLocalIPList(int &iNrOfIP)
{
	char szHostName[128];
	CWK_String str;
	
	if( gethostname(szHostName, 128) == 0 )
	{
		// Get host adresses
		struct hostent * pHost;
		
		pHost = gethostbyname(szHostName);
		
		for(int i = 0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ )
		{
			str.Empty();
			for(int j = 0; j < pHost->h_length; j++ )
			{
				CString addr;
				
				if( j > 0 )
					str += _T(".");
				
				addr.Format(_T("%u"), (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j]);
				str += addr;
			}
			m_lLocalIp[i]=inet_addr(str);
		}
		iNrOfIP=i;
	}
}


