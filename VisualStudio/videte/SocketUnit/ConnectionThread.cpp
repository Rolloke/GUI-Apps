
#include "stdafx.h"

#include "ConnectionThread.h"
#include "CIPCPipeSocket.h"
#include "CIPCExtraMemory.h"
#include "DataSocket.h"
#include "SocketUnit.h"

#include "CIPCOutput.h"
#include "CIPCServerControlClientSide.h"
#include "SocketUnitDoc.h"

#include "ServerControlSocket.h"	// to be able to create bubble
#include "CIPCOutput.h" // for sync cmd

#include "resource.h"
#include "ConnectionThreadWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


WORD CConnectionThread::m_wTheIdCounter=1;
WORD CConnectionThread::m_wThePipeIdCounter=1;
DWORD CConnectionThread::m_dwTheSyncRetryTime=30000;


int CConnectionThread::m_nOutgoingCall=0;	// Zählt ausgehende Rufe
int CConnectionThread::m_nIncomingCall=0;	// Zählt einkommende Rufe
CPtrList CConnectionThread::m_ConnectedHosts;

// 1 first version with version check (for incoming calls)
int CConnectionThread::m_iTheVersion=1;


BEGIN_MESSAGE_MAP(CConnectionThread, CWinThread)
	//{{AFX_MSG_MAP(CConnectionThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////

void CConnectionThread::Init()
{

	m_bInitDone=FALSE;
	m_bAutoDelete = TRUE;

	m_hSocket=NULL;
	// m_sHostName;
	m_wId = m_wTheIdCounter++;
	m_dwLastSyncRunningTime=0;
	m_dwSyncSendTime = 0;
	m_dwDynamicSyncTimeout = 10000;
	m_pDoc=NULL;
	
	m_pPipe=NULL;
	m_pPipe2=NULL;

	m_pSocket=NULL;

	m_pipeType = CIPC_TYPE_INVALID;
	m_state = CTS_CREATED;
	//
	m_wPipeId1 = 0;
	m_wPipeId2 = 0;
	//
	m_dwLastSyncCheck = GetTickCount();
	
	m_lRemoteIP = 0;
	//m_sendQueue.SetAutoDelete(TRUE);
	m_iMaxSendQueue = 0;
	m_ConnectionState = CONNECTION_INIT;
	m_iSyncSocketState= INIT;
	m_iSyncRetryCounter=0;
}

/////////////////////////////////////////////////////////////////////////////

CConnectionThread::CConnectionThread(CSocketUnitDoc *pDoc,
						CIPCType ctype,
						LPCTSTR hostName,
						CString sIPAdapter,
						const CConnectionRecord &c
						)
{
	// called from OnRequestXXX
	m_sAdapterIP = sIPAdapter;
	Init();

	m_connectionRecord = c;	// also stores the Alarm SharedMemoryNames

	m_pDoc = pDoc;
	m_pipeType = ctype;
	m_sHostName = hostName;

	m_bActivePeer = TRUE;
}

/////////////////////////////////////////////////////////////////////////////

CConnectionThread::CConnectionThread(CSocketUnitDoc *pDoc,
									 SOCKET hSocket 
									 )
{
	// called from OnAccept......
	Init();

	m_pDoc = pDoc;
	m_hSocket = hSocket; // handle was created in the main thread
	m_bActivePeer = FALSE;
}

/////////////////////////////////////////////////////////////////////////////

CConnectionThread::~CConnectionThread()
{
	WK_DELETE(m_pPipe);
	WK_DELETE(m_pPipe2);
	WK_DELETE(m_pSocket);

	m_sendQueue.SafeDeleteAll();
}

/////////////////////////////////////////////////////////////////////////////

int CConnectionThread::ExitInstance()
{
	CString sMsg;
	CString sRemoteName(m_sHostName);	// /use as default, in case socket already gone
	CString sPipeType = UserNameOfEnum(m_pipeType);


	if (m_state == CTS_CONNECTED_ACTIVE_UNCONFIRMED  )
	{
		sPipeType +=_T(",Unconfirmed");
	}

	HandleVirtualAlarm(FALSE);

	CString sLogMsg;
	if (m_bActivePeer)
	{
		sLogMsg.Format(_T("Connection|Outgoing|%s"), LPCTSTR(m_sHostName));
	}
	else
	{
		sLogMsg.Format(_T("Connection|Incoming|%s"), LPCTSTR(m_sHostName));
	}

	WK_STAT_LOG(_T("Reset"),0,sLogMsg);

	WK_DELETE(m_pSocket);
	WK_DELETE(m_pPipe);
	WK_DELETE(m_pPipe2);

	WK_TRACE(_T("Disconnect (%s) connected to %s\n"), LPCTSTR(sPipeType), LPCTSTR(sRemoteName));
	m_state = CTS_TERMINATING;
	m_pDoc->RemoveThread(this);

	TRACE(_T("CConnectionThread::ExitInstance(%s-%s)\n"),m_sHostName,NameOfEnum(m_pipeType));

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CConnectionThread::InitInstance()
{
	TRACE(_T("CConnectionThread::InitInstance(%s-%s)\n"),m_sHostName,NameOfEnum(m_pipeType));
	m_pMainWnd = new CConnectionThreadWnd(this);
	CRect rect;
	CString sWndClass = AfxRegisterWndClass(0);
	if (!m_pMainWnd->CreateEx(0,sWndClass,_T("SocketWnd"),WS_OVERLAPPED,rect,NULL,0))
	{
		return FALSE;
	}
	// two cases: 
	//		- activated by ServerControl, 
	//		  needs to Connect() to a remote server
	//		- activated by OnAccept(), m_hSocket set
	//		  needs to FetchXXX connection from the local server
	//		  and has to Attach to the socket
	if (m_bActivePeer) 
	{
		if (   m_pipeType == CIPC_INPUT_CLIENT
			|| m_pipeType == CIPC_OUTPUT_CLIENT
			|| m_pipeType == CIPC_DATABASE_CLIENT
			|| m_pipeType == CIPC_ALARM_SERVER

			|| m_pipeType == CIPC_AUDIO_CLIENT
			|| m_pipeType == CIPC_AUDIO_SERVER
			) 
		{
			m_state           = CTS_ACTIVATED_ACTIVE;
			BOOL bConnected   = FALSE;
			BOOL bCreated     = FALSE;


			m_sThreadName = m_sHostName + _T("-");
			switch (m_pipeType) 
			{
			case CIPC_INPUT_CLIENT:
			case CIPC_INPUT_SERVER:
				m_sThreadName += _T("INPUT");
				break;
			case CIPC_OUTPUT_CLIENT:
			case CIPC_OUTPUT_SERVER:
				m_sThreadName += _T("OUTPUT");
				break;
			case CIPC_DATABASE_CLIENT:
			case CIPC_DATABASE_SERVER:
				m_sThreadName += _T("DATABASE");
				break;
			case CIPC_AUDIO_CLIENT:
			case CIPC_AUDIO_SERVER:
				m_sThreadName += _T("AUDIO");
				break;
			case CIPC_ALARM_CLIENT:
			case CIPC_ALARM_SERVER:
				m_sThreadName += _T("ALARM");
				break;
			}
			XTIB::SetThreadName(m_sThreadName);
			m_dwThreadID = GetCurrentThreadId();


			CString msg;
			CDataSocket *pSocket = new CDataSocket(this);
#ifdef _DTS			

			if(m_sAdapterIP==_T(""))
			{
				bCreated=FALSE;
				WK_TRACE_ERROR(_T("ID:%u Outgoing socket not created because of empty adapter IP\n"), m_wId);
			}
			else
			{
				bCreated = pSocket->Create(0,SOCK_STREAM,m_sAdapterIP);
			}
#else
			bCreated = pSocket->Create(/*0,SOCK_STREAM,NULL*/);	
#endif
			if(bCreated)
			{
				pSocket->SetSocketOpts();
#ifdef _DTS
			    bConnected = pSocket->Connect(m_sHostName, VIDETE_CIPC_PORT);
#else
				bConnected = pSocket->Connect(m_sHostName, VIDETE_CIPC_PORT);
				// Skip next try, if request is cancelled
				if(!bConnected &&
					GetDoc()->m_pCipcControl->CheckIfClientIsStillListening(this)
					)
				{
					WK_TRACE_WARNING(_T("Try to connect with old port\n"));
					bConnected = pSocket->Connect(m_sHostName, VIDETE_OLD_PORT);	
				}
#endif

				if (bConnected) 
				{
					m_pSocket = pSocket;
					m_state = CTS_CONNECTED_ACTIVE;
					msg.Format(_T("Connected (%s)--> %s on AdapterIP(%s)"),
								LPCTSTR(m_pSocket->GetPipeTypeString()),
								m_pSocket->GetRemoteName(),
								m_sAdapterIP
								);
					WK_TRACE(_T("%s\n"),msg);
				} 
				else 
				{	// zuallererst GetLastErrorString() aufrufen!
					// if (GetLastError() == WSAEINVAL) nur als Gedaechtnisstuetze
					msg.Format(_T("FAILED to connect to %s on AdapterIP(%s)\n%s\n"),
									m_sHostName, m_sAdapterIP, GetLastErrorString());
					WK_TRACE(msg);
					SendErrorIndicationToClient(CIPC_ERROR_UNABLE_TO_CONNECT,0,msg);
#ifdef _DTS
					CIPBook::m_MapServerList.RemoveKey(m_sHostName);
#endif
					WK_DELETE(pSocket);
					Sleep(10);	// before terminating
					DoTerminate();
				}
			}
			else // bCreated
			{
				WK_TRACE_ERROR(_T("ID:%u Failed to Create() outgoing socket\n"), m_wId);
				WK_DELETE(pSocket);
				msg.Format(_T("FAILED to connect to %s on AdapterIP(%s)\n"), m_sHostName, m_sAdapterIP);
				WK_TRACE_ERROR(msg);
				SendErrorIndicationToClient(CIPC_ERROR_UNABLE_TO_CONNECT,0,msg);
				Sleep(100);	// before terminating
				DoTerminate();				
			}
		} 
		else 
		{
			WK_TRACE_ERROR(_T("ID:%u Invalid cipcType %d for activePeer\n"), m_wId, m_pipeType);
		}
	} 
	else 
	{	// passive peer follows here
	
		// attach socket to this thread
		m_pSocket = new CDataSocket(m_pPipe, this);	// OOPS args
		if(m_pSocket)
		{
			if(m_pSocket->Attach(m_hSocket))
			{
				m_sHostName = m_pSocket->GetRemoteName();
				m_sThreadName = m_sHostName;
				XTIB::SetThreadName(m_sThreadName);
				m_state = CTS_CONNECTED_PASSIVE;
				m_pSocket->SetSocketOpts();
			}
			else
			{
				DoTerminate();
			}
		}
		else
		{
			DoTerminate();
		}
	}

	CString sLogMsg;
	if (m_bActivePeer) 
	{
		sLogMsg.Format(_T("Connection|Outgoing|%s"), LPCTSTR(m_sHostName));
	} 
	else 
	{
		sLogMsg.Format(_T("Connection|Incoming|%s"), LPCTSTR(m_sHostName));
	}

	WK_STAT_LOG(_T("Reset"),1,sLogMsg);

	m_bInitDone = TRUE;

	return TRUE;
} // end of InitInstance

/*
@MD
*/
/////////////////////////////////////////////////////////////////////////////

void CConnectionThread::HandleRemoteFetchResult(CIPCFetchResult &remoteFetchResult,
											WORD wId1, WORD wId2)
{
	BOOL bRemoteFetchOkay = remoteFetchResult.IsOkay();
	TRACE(_T("FetchResult %s\n"),GetHostName());
#ifdef _UNICODE
	WORD wCodePage = remoteFetchResult.GetCodePage();
	TRACE("Remote CodePage %u\n", wCodePage);
#endif

	// create a client connection
	CString sShmName,sStyle;

	if (m_pipeType==CIPC_ALARM_SERVER) 
	{
		// and again the dual pipe, now the shared memory names
		// are still in the connection record
		if (bRemoteFetchOkay)
		{
			m_pPipe = new CIPCPipeSocket(this, 
							m_connectionRecord.GetInputShm(), 
							CIPC_INPUT_SERVER,
							FALSE,
							0,	// options ignored for inputs
							m_wPipeId1,
							m_sHostName
							);
#ifdef _UNICODE
			m_pPipe->SetCodePage(wCodePage);
#endif
			m_pPipe->StartThread(TRUE);
			m_pPipe->m_pServerSocket = m_pSocket;	// OOPS
			m_pSocket->SetCIPCPipeSocket(m_pPipe);
			m_pPipe->SetRemoteId(wId1);
			GetDoc()->m_ServerPipes.Add(m_pPipe);

			// OUTPUT
			m_pPipe2 = new CIPCPipeSocket(this, 
							m_connectionRecord.GetOutputShm(), 
							CIPC_OUTPUT_SERVER,
							FALSE,
							0,	// options ignored for inputs
							m_wPipeId2,
							m_sHostName
							);
#ifdef _UNICODE
			m_pPipe2->SetCodePage(wCodePage);
#endif
			m_pPipe2->StartThread(TRUE);
			m_pPipe2->m_pServerSocket = m_pSocket;	// OOPS
			m_pSocket->SetCIPCPipeSocket(m_pPipe2);
			m_pPipe2->SetRemoteId(wId2);
			GetDoc()->m_ServerPipes.Add(m_pPipe2);
		}
		else
		{
			// remote fetch failed, error indication is below
		}

	}
	else
	{	// plain connections
		if (m_pipeType == CIPC_INPUT_CLIENT) sStyle=_T("Input");
		else if (m_pipeType == CIPC_DATABASE_CLIENT) sStyle=_T("Database");
		else sStyle=_T("Output");
		sShmName.Format(_T("Socket%s%d"),sStyle,m_wId);

		if (bRemoteFetchOkay)
		{
			m_pPipe = new CIPCPipeSocket(this, 
							sShmName, 
							m_pipeType,
							TRUE,
							0,	// options ignored for inputs
							m_wPipeId1,	// OOPS OHOH m_wThePipeIdCounter++,
							m_sHostName
							);
#ifdef _UNICODE
			m_pPipe->SetCodePage(wCodePage);
#endif
			m_pPipe->StartThread(TRUE);
			m_pPipe->m_pDataSocket = m_pSocket;	// OOPS
			m_pSocket->SetCIPCPipeSocket(m_pPipe);
			m_pPipe->SetRemoteId(wId1);
			GetDoc()->m_ClientPipes.Add(m_pPipe);
			// set timeout for client connect to 5s
			m_pPipe->SetNotConnectedTimeout(5*1000);
		}
		else
		{
			// remote fetch failed
		}
	}	// end of plain pipe

	if (m_pPipe)
	{
		switch (m_pipeType) 
		{
			case CIPC_INPUT_CLIENT:
				GetDoc()->m_pCipcControl->DoConfirmInputConnection(
									CSecID(0x1234,m_pPipe->GetId()),
									m_pPipe->GetShmName(),
									remoteFetchResult.GetErrorMessage(),
									remoteFetchResult.GetOptions()
#ifdef _UNICODE
									, MAKELONG(wCodePage, 0)
#endif									
								);
			break;
			case CIPC_OUTPUT_CLIENT:
				GetDoc()->m_pCipcControl->DoConfirmOutputConnection(
									CSecID(0x1234,m_pPipe->GetId()),
									m_pPipe->GetShmName(),
									remoteFetchResult.GetErrorMessage(),
									remoteFetchResult.GetOptions()
#ifdef _UNICODE
									, MAKELONG(wCodePage, 0)
#endif									
								);
			break;
			case CIPC_DATABASE_CLIENT:
				GetDoc()->m_pCipcControl->DoConfirmDataBaseConnection(
									CSecID(0x1234,m_pPipe->GetId()),
									m_pPipe->GetShmName(),
									remoteFetchResult.GetErrorMessage(),
									remoteFetchResult.GetOptions()
#ifdef _UNICODE
									, MAKELONG(wCodePage, 0)
#endif									
								);
			break;
			case CIPC_ALARM_SERVER:
				if (m_pPipe && m_pPipe2)
				{
					CString sBitrate = remoteFetchResult.GetBitrate();
					DWORD dwRemoteBitrate = 0;
					if (1==_stscanf(sBitrate,_T("%d"),&dwRemoteBitrate))
					{
						if (dwRemoteBitrate>theApp.GetSelectedBitrate())
						{
							sBitrate.Format(_T("%d"),theApp.GetSelectedBitrate());
						}
					}

					GetDoc()->m_pCipcControl->DoConfirmAlarmConnection(
									m_pPipe->GetShmName(),
									m_pPipe2->GetShmName(),
									sBitrate,
									remoteFetchResult.GetServerVersion(),
									remoteFetchResult.GetOptions()
#ifdef _UNICODE
									, MAKELONG(wCodePage, 0)
#endif									
								);
				}
				else
				{
					// NOT YET DoIndicateError, but m_pPipe is checked
					// so it's NOT YET error indication if pipe2 failed
				}
				break;
			case CIPC_AUDIO_CLIENT:
				GetDoc()->m_pCipcControl->DoConfirmAudioConnection(
									CSecID(0x1234,m_pPipe->GetId()),
									m_pPipe->GetShmName(),
									remoteFetchResult.GetErrorMessage(),
									remoteFetchResult.GetOptions()
#ifdef _UNICODE
									, MAKELONG(wCodePage, 0)
#endif									
								);
			break;
			default:
				WK_TRACE(_T("ID:%u internal error, wrong pipe type %d/%s in confirm\n"),
					m_wId,
					m_pipeType,
					NameOfEnum(m_pipeType)
					);
		}	// end of switch(m_pipeType) for Fetch confirmation
		m_state = CTS_CONNECTED;
		HandleVirtualAlarm(TRUE);
	}
	else
	{
		// no pipe was created, due to failed remote fetchResult
		// NOT YET forward the remote error code
		SendErrorIndicationToClient(
			remoteFetchResult.GetError(),
			remoteFetchResult.GetErrorCode(),
			remoteFetchResult.GetErrorMessage()
			);

		DoTerminate();
	}
}

/////////////////////////////////////////////////////////////////////////////

void CConnectionThread::SendErrorIndicationToClient(CIPCError errorCode,
													DWORD dwUnitCode,
													const CString &sErrorMessage
													)
{
	CServerControlSocket* pControl = GetDoc()->m_pCipcControl;
	if (pControl)
	{
		if (pControl->CheckIfClientIsStillListening(this))
		{
			// NOT YET check requestNumber
			DWORD dwCmd = SRV_CONTROL_REQUEST_INPUT_CONNECTION;
			switch (m_pipeType)
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
				default:
					WK_TRACE(_T("ID:%u Invalid pipe Type %d\n"), m_wId,m_pipeType);
			}
			pControl->DoIndicateError(dwCmd,
									  SECID_NO_ID, 
									  errorCode,
									  dwUnitCode,
									  sErrorMessage
									  );
		}
	}
}
/*
@MD
@REMARK no return value, since the calling socket can't make any use of it.
*/

/////////////////////////////////////////////////////////////////////////////

void CConnectionThread::HandleCmd(WORD wId1, WORD wId2,
								  DWORD dwDataLen, const void *pData,
								  DWORD dwCmd,DWORD dwParam1, DWORD dwParam2, 
								  DWORD dwParam3, DWORD dwParam4
								  )
{


#ifdef _DEBUG
	if (m_state!=CTS_CONNECTED) 
	{
		WK_TRACE(_T("ID:%u HandleCmd in state %d\n"), m_wId,m_state);
	}

#endif

	switch (m_state) 
	{
		case CTS_CONNECTED_ACTIVE_UNCONFIRMED:
		{
			if (pData) 
			{
				if (dwDataLen < 102*1024) 
				{
					TRACE(_T("received %s\n"),CIPC::NameOfCmd(dwCmd));
					CIPCExtraMemory tmpBubble;
					if (tmpBubble.Create(GetDoc()->m_pCipcControl,dwDataLen,pData))
					{
						CIPCFetchResult remoteFetchResult(tmpBubble);
						WK_TRACE(_T("incoming fetch result <%s><%s><%s>\n"),
							NameOfEnum(remoteFetchResult.GetError()),
							remoteFetchResult.GetErrorMessage(),
							GetHostName());
						HandleRemoteFetchResult(remoteFetchResult,wId1,wId2);
					}
					else
					{
						SendErrorIndicationToClient(CIPC_ERROR_UNABLE_TO_CONNECT,0,
													_T("Cannot create CIPCExtraMemory"));
					}
				} 
				else 
				{
					CString sMsg;
					sMsg.Format(_T("ID:%u Invalid dataLength from remote fetch result !?"), m_wId);
					WK_TRACE_ERROR(_T("%s\n"),sMsg);
					WK_TRACE(_T("received %s\n"),CIPC::NameOfCmd(dwCmd));
					SendErrorIndicationToClient(CIPC_ERROR_UNABLE_TO_CONNECT,0,
												sMsg);
				}
			}
			else 
			{
				// no bubble no remoteFetchResult
				CString sMsg;
				sMsg.Format(_T("ID:%u No data from remote fetch result !?"), m_wId);
				WK_TRACE_ERROR(_T("%s\n"), sMsg);
				WK_TRACE(_T("received %s\n"),CIPC::NameOfCmd(dwCmd));
				SendErrorIndicationToClient(CIPC_ERROR_UNABLE_TO_CONNECT,0,sMsg);

			}

		} // end of connected ACTIVE_UNCONFIRMED
		break;
		case CTS_CONNECTED_PASSIVE:
		{
			// should be a ConnectionRecord
			// NOT YET more checks on memory etc.
			if (dwDataLen<104*1024) 
			{
				CIPCExtraMemory tmpBubble;
				if (tmpBubble.Create(GetDoc()->m_pCipcControl, dwDataLen, pData))
				{
					CIPCCmdRecord cmdRecord(&tmpBubble,
											dwCmd,
											dwParam1, dwParam2, dwParam3, dwParam4);
					CConnectionRecord connectionInfo(cmdRecord);
					
					HandleIncomingConnectionRecord(connectionInfo,wId1,wId2);
				}
			} 
		}
		break;
		case CTS_CONNECTED:
		// plain pipe function, once fully connected
		// or the special keepAlive syncs
		if (dwCmd == CIPC_REQUEST_SYNC && dwParam1==5678) 
		{ 
			// but should be the type (next param)
									//RM
			//WK_TRACE(_T("%u hat CIPC_REQUEST_SYNC erhalten\n"),m_wId);
			if (m_pPipe && m_pSocket) 
			{
				AddToSendQueue(new CSocketCmdRecord(m_wPipeId1,
													m_wPipeId2,
													CIPC_CONFIRM_SYNC,
													5678,	// OOPS undocumented but should be the type (next param)
													0,0,0,NULL),TRUE);

				if (theApp.m_bTraceSync) 
				{
					CString sMsg;
					sMsg.Format(_T("SyncConfirmationSendTo|%s|%s"),m_sHostName,
						NameOfEnum(GetCIPCType())
						);
					WK_STAT_PEAK(_T("Reset"),1,sMsg);
				}

			}
		} 
		else if ( dwCmd == CIPC_CONFIRM_SYNC && dwParam1==5678) 
		{ // but should be the type (next param)
			
			CheckSyncRunningTime();
			m_iSyncSocketState=INIT;
			m_iSyncRetryCounter=0;
			m_ConnectionState = CONNECTION_OK;
			ResetSyncTimer();

			//if(m_dwLastSyncRunningTime <  GetTickCount()-m_dwSyncSendTime)
			{
				m_dwLastSyncRunningTime = GetTickCount()-m_dwSyncSendTime;
				TRACE_SYNC(_T("SYNC running timepeak"),m_dwLastSyncRunningTime);
			}
			

			if (theApp.m_bTraceSync)
			{
				CString sMsg;
				sMsg.Format(_T("SyncConfirmedFrom|%s|%s"),m_sHostName,
					NameOfEnum(GetCIPCType())
					);
				WK_STAT_PEAK(_T("Reset"),1,sMsg);
			}
		} 
		else if (m_pPipe)
		{
			if (m_pPipe->GetRemoteId()==wId1 && m_pPipe->GetId()==wId2)
			{
				m_pPipe->PushCmd(dwDataLen, pData,
						dwCmd,
						dwParam1, dwParam2, dwParam3, dwParam4
						);
			} 
			else if (m_pPipe2 &&
						m_pPipe2->GetRemoteId()==wId1 && m_pPipe2->GetId()==wId2) 
			{
				m_pPipe2->PushCmd(dwDataLen, pData,
							  	  dwCmd,
								  dwParam1, dwParam2, dwParam3, dwParam4
								 );

			} 
			else 
			{
				if (m_pPipe2==NULL)
				{
				    WK_TRACE_ERROR(_T("ID:%u PipeID mismatch local %d/%d!= %d/%d\n"),
					m_wId,
					m_pPipe->GetId(),
					m_pPipe->GetRemoteId(),
					wId1, wId2
					);
					OnClose(0);
				}
				else
				{
					WK_TRACE_ERROR(_T("ID:%u PipeID mismatch local %d/%d, %d/%d!= %d/%d\n"),
					m_wId,
					m_pPipe->GetId(),
					m_pPipe->GetRemoteId(),
					m_pPipe2->GetId(),
					m_pPipe2->GetRemoteId(),
					wId1, wId2
					);
					OnClose(0);
				}

			}
		}
		else 
		{
			OnClose(0);
		}
		break;
	default:
		WK_TRACE(_T("ID:%u NOT YET in CConnectionThread::HandleCmd\n"),	m_wId);
	} // end of switch(m_state);

}
/////////////////////////////////////////////////////////////////////////////
void CConnectionThread::AddToSendQueue(CSocketCmdRecord *pSendRecord, BOOL bAddTail)
{
	if (pSendRecord)
	{
		if (m_sendQueue.GetCount()<50) 
		{
			m_sendQueue.Lock();
			if (bAddTail)
			{
				m_sendQueue.AddTail(pSendRecord);
			}
			else
			{
				m_sendQueue.AddHead(pSendRecord);
			}
			if (m_sendQueue.GetCount()>m_iMaxSendQueue)
			{
				m_iMaxSendQueue = m_sendQueue.GetCount();
			}
			m_sendQueue.Unlock();
			if (WK_IS_WINDOW(m_pMainWnd))
			{
				m_pMainWnd->PostMessage(WM_USER,WPARAM_SEND);
			}
		} 
		else 
		{
			delete pSendRecord;
			WK_TRACE(_T("Queue overflow! Already have %d records in queue, data dropped.\n"),m_sendQueue.GetCount());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CConnectionThread::SendConnectionRecord()
{
	CConnectionRecord c(m_connectionRecord);
	// OOPS address or IDIPName ?
#ifdef _DTS
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
#else
	CWK_Profile wkp;
#endif
	CHostArray hosts;
	hosts.Load(wkp);
	if (c.GetSourceHost().IsEmpty())
	{
		c.SetSourceHost(hosts.GetLocalHostName() );
	}
	c.SetFieldValue(CRF_IPADDRESS,theApp.GetIPAdress());

	CWK_Dongle dongle;
	if (dongle.GetProductCode()!=IPC_DTS_IP_RECEIVER)
	{
		CString sBitrate;
		sBitrate.Format(_T("%d"),theApp.GetSelectedBitrate());
		c.SetFieldValue(CRF_BITRATE,sBitrate);
	}

	CString sExtraInfo;
	// extraInfo in InputShm is the (local/sender) pipeType
	sExtraInfo.Format(_T("%d"),(int)m_pipeType);
	c.SetInputShm(sExtraInfo);

	m_wPipeId1 = m_wThePipeIdCounter++;

	sExtraInfo.Format(_T("%d"),(int)m_pipeType);
	c.SetInputShm(sExtraInfo);

	CString sVersion;
	sVersion.Format(_T("%d"),m_iTheVersion);
	c.SetOutputShm(sVersion);

	if (m_pSocket)
	{
		CIPCCmdRecord *pCmdRecord = c.CreateCmdRecord(
				GetDoc()->m_pCipcControl,
				SRV_CONTROL_REQUEST_PIPE
				);
		if (pCmdRecord)
		{
			AddToSendQueue(new CSocketCmdRecord( 
								m_wPipeId1,m_wPipeId2,
								pCmdRecord->m_dwCmd,
								pCmdRecord->m_dwParam1, 
								pCmdRecord->m_dwParam2,
								pCmdRecord->m_dwParam3, 
								pCmdRecord->m_dwParam4,
								pCmdRecord->m_pExtraMemory
								),TRUE);
		}
		else
		{
		}
		WK_DELETE(pCmdRecord);
	}
	else
	{
	}
	hosts.DeleteAll();
}

// The following versions are on the run
// 0 or no version
// 1

/////////////////////////////////////////////////////////////////////////////

BOOL CConnectionThread::CheckIncomingVersion(const CString &sVersion)
{
	int iValue=0;
	
	if (sVersion.GetLength())
	{
		iValue = _ttoi(sVersion);
	}

	if (iValue>=1)
	{
		return TRUE;
	}
	else
	{
		CString sMsg;
		sMsg.Format(_T("Version mismatch (local %d, remote %d)"),
			m_iTheVersion, 
			iValue);

		WK_TRACE(_T("%s\n"),sMsg);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CConnectionThread::HandleIncomingConnectionRecord(const CConnectionRecord &connectionInfoIn,
													   WORD wId1,
													   WORD wId2
													   )
{
	CConnectionRecord connectionInfo(connectionInfoIn);
/*
	CStringArray sa;
	connectionInfo.GetAllFields(sa);
	for (int i=0;i<sa.GetSize();i++)
	{
		TRACE(_T("%d, %s\n"),i,sa.GetAt(i));
	}
*/
	BOOL bFetch = TRUE;

#ifdef _DEBUG
	WK_TRACE(_T("ID:%u Received connection request %s/%s\n"),
		m_wId,
		connectionInfo.GetPermission(),
		connectionInfo.GetUser()
		);
#endif
	
	CIPCFetchResult fetchResult;
	
	if (CheckIncomingVersion(connectionInfo.GetOutputShm())==FALSE) 
	{
		fetchResult = CIPCFetchResult(_T("No pipe created"),
									  CIPC_ERROR_UNABLE_TO_CONNECT,
									  6,
									  _T(""),
									  0,
									  CSecID(SECID_NO_ID)
#ifdef _UNICODE
									  , 0
#endif
									 );
		
	} 
	else 
	{
		CIPCType remotePipeType = CIPC_TYPE_INVALID;
		CIPCType localPipeType  = CIPC_TYPE_INVALID;
		if (connectionInfo.GetInputShm().GetLength()) 
		{
			DWORD dwInfo=0;
			int iNumRead = _stscanf(connectionInfo.GetInputShm(),_T("%d"),&dwInfo);
			if (iNumRead==1)
			{
				remotePipeType = (CIPCType) dwInfo;
				switch (remotePipeType)
				{
					case CIPC_INPUT_CLIENT: 
						localPipeType = CIPC_INPUT_SERVER;
						break;
					case CIPC_OUTPUT_CLIENT: 
						localPipeType = CIPC_OUTPUT_SERVER;
						break;
					case CIPC_DATABASE_CLIENT: 
						localPipeType = CIPC_DATABASE_SERVER;
						break;
					case CIPC_ALARM_SERVER:
						localPipeType = CIPC_ALARM_CLIENT;
						break;
					case CIPC_AUDIO_CLIENT: 
						localPipeType = CIPC_AUDIO_SERVER;
						break;
					default:
						WK_TRACE(_T("ID:%u inalid remotePipe type %s\n"),
								m_wId, NameOfEnum(remotePipeType));
				}
			}
			else
			{
			}
		} 
		else
		{

			WK_TRACE(_T("Extra info missing in PipeRequest!\n"));
			OnClose(0);
		}
		
		// check for correct permission
		CString sDestination= connectionInfo.GetDestinationHost();
		WK_TRACE(_T("ID:%u Destination is %s\n"), m_wId,sDestination);	// OBSOLETE
		if (sDestination.GetLength()) 
		{
			// OOPS drop leading own name, or already done before
		}
		else
		{
			connectionInfo.SetDestinationHost(LOCAL_LOOP_BACK); // NOT YET routing
		}
		int ix = sDestination.Find(ROUTING_CHAR);
		
		if (ix != -1) 
		{
			// is routing allowed ???
			CWK_Dongle dongle;
			if (FALSE==dongle.AllowRouting())
			{
				CString sDenyError;
				sDenyError.LoadString(IDS_NO_ROUTING);
				// no outgoing calls at all!
				WK_TRACE(_T("Routing Call denied from %s\n"),connectionInfoIn.GetSourceHost());
				fetchResult = CIPCFetchResult(_T("No pipe created"),
											  CIPC_ERROR_UNABLE_TO_CONNECT,
											  6,
											  sDenyError,
											  0,
											  CSecID(SECID_NO_ID)
#ifdef _UNICODE
											  , 0
#endif
											 );
				bFetch = FALSE;
			}
			else
			{
				CString sMyName;
				CString sNextHost;
				sMyName = sDestination.Left(ix-1);
				sNextHost = sDestination.Mid(ix+1);
				connectionInfo.SetDestinationHost(sNextHost);
				connectionInfo.SetSourceHost(connectionInfoIn.GetSourceHost());
				WK_TRACE(_T("routing call to %s\n"),sNextHost);
			}
		} 
		else 
		{
			connectionInfo.SetDestinationHost(LOCAL_LOOP_BACK);
		}
		
		// do the Fetch....
		CString sSharedMemory;
		if (bFetch)
		{
			
			BOOL bAccessGranted = CheckPermission(connectionInfo, fetchResult);
			CIPCFetch fetch;
			CString sBitrate;
			sBitrate.Format(_T("%d"),theApp.GetSelectedBitrate());
			
			if (bAccessGranted) 
			{
				switch (localPipeType) 
				{
					case CIPC_INPUT_SERVER:
						fetchResult = fetch.FetchInput(connectionInfo);
						sSharedMemory = fetchResult.GetShmName();
						break;
					case CIPC_OUTPUT_SERVER:
						fetchResult = fetch.FetchOutput(connectionInfo);
						sSharedMemory = fetchResult.GetShmName();
						break;
					case CIPC_AUDIO_SERVER:
						fetchResult = fetch.FetchAudio(connectionInfo);
						sSharedMemory = fetchResult.GetShmName();
						break;
					case CIPC_DATABASE_SERVER:
						fetchResult = fetch.FetchDatabase(connectionInfo);
						sSharedMemory = fetchResult.GetShmName();
						break;
					case CIPC_ALARM_CLIENT:
						// needs to create two pipes before doing the fetch
						m_wPipeId1 = m_wThePipeIdCounter++;
						m_wPipeId2 = m_wThePipeIdCounter++;	// OOPS not always increment
						m_pipeType = localPipeType;
						
						// INPUT PIPE
						sSharedMemory.Format(_T("SocketAlarmClientInput%d_%d"),
							m_wPipeId1, GetTickCount()
							);
						// create the pipe
						m_pPipe = new CIPCPipeSocket(this, 
							sSharedMemory, 
							CIPC_INPUT_SERVER,
							TRUE,
							fetchResult.GetOptions(),
							m_wPipeId1
							);
#ifdef _UNICODE
						m_pPipe->SetCodePage(fetchResult.GetCodePage());
#endif
						/// criss cross
						m_pPipe->m_pServerSocket = m_pSocket;
						m_pPipe->StartThread(TRUE);
						m_pDoc->m_ClientPipes.Add(m_pPipe);
						m_pSocket->SetCIPCPipeSocket(m_pPipe);
						m_pPipe->SetRemoteId(wId1);
						
						// OUTPUT PIPE
						sSharedMemory.Format(_T("SocketAlarmClientOutput%d_%d"),
							m_wPipeId2, GetTickCount()
							);
						// create the pipe
						m_pPipe2 = new CIPCPipeSocket(this, 
							sSharedMemory, 
							CIPC_OUTPUT_SERVER,
							TRUE,
							fetchResult.GetOptions(),
							m_wPipeId2
							);
#ifdef _UNICODE
						m_pPipe2->SetCodePage(fetchResult.GetCodePage());
#endif
						/// criss cross
						m_pPipe2->m_pServerSocket = m_pSocket;
						m_pPipe2->StartThread(TRUE);
						m_pDoc->m_ClientPipes.Add(m_pPipe2);
						m_pSocket->SetCIPCPipeSocket(m_pPipe2);
						m_pPipe2->SetRemoteId(wId2);
						
						connectionInfo.SetInputShm(m_pPipe->GetShmName());
						connectionInfo.SetOutputShm(m_pPipe2->GetShmName());
						TRACE(_T("ConnectionThread %s\n"), m_pPipe->GetShmName());
						TRACE(_T("ConnectionThread %s\n"), m_pPipe2->GetShmName());

						{
							CStringArray sa;
							connectionInfo.GetAllFields(sa);
							int i,d;

							d = sa.GetSize();
							WK_TRACE(_T("%d extra parameters\n"),d);
							for (i=0;i<d;i++)
							{
								if (-1 == sa[i].Find(CSD_PIN))
								{
									TRACE(_T("param%d = <%s>\n"),i,sa[i]);
								}
							}
						}

						if (connectionInfo.GetDestinationHost()==LOCAL_LOOP_BACK)
						{
							CString sTemp;
							if (connectionInfo.GetFieldValue(CRF_BITRATE,sTemp))
							{
								// ist eine Routing Verbindung
								DWORD dwRemoteBitrate = 0;
								if (1==_stscanf(sTemp,_T("%d"),&dwRemoteBitrate))
								{
									m_pPipe->m_dwBitrate = dwRemoteBitrate;
									m_pPipe2->m_dwBitrate = dwRemoteBitrate;
								}
							}
							else
							{
								connectionInfo.SetFieldValue(CRF_BITRATE,sBitrate);
							}

#ifdef _UNICODE
							if (WK_IS_RUNNING(WK_APP_NAME_IDIP_CLIENT)==FALSE) 
#else
							if (WK_IS_RUNNING(WK_APP_NAME_EXPLORER)==FALSE) 
#endif
							{
								if (CIPCServerControlClientSide::StartVision()) 
								{
									Sleep(500);	// already made a WaitForInputIdle
								} 
								else 
								{
									// failed to start
								}
							}
						}
						
						fetchResult = fetch.FetchAlarmConnection(connectionInfo);
						if (fetchResult.IsOkay())
						{
							CIPCFetchResult nfr(sBitrate,
												fetchResult.GetError(),
												fetchResult.GetErrorCode(),
												fetchResult.GetErrorMessage(),
												fetchResult.GetOptions(),
												fetchResult.GetAssignedID()
#ifdef _UNICODE
												, fetchResult.GetCodePage()
#endif
												);
							fetchResult = nfr;
						}
						break;
					default:
						WK_TRACE_ERROR(_T("ID:%u Unsupported/invalid pipeType %d\n"), m_wId,(int)localPipeType);
				}	// end of switch local pipe type
			}
			else
			{
				// access denied
			}
		} // bFetch by Routing Dongle!!
		
		if ( fetchResult.IsOkay() ) 
		{
			m_pipeType = localPipeType;
			WK_TRACE(_T("Accepted (%s) from %s\n"), LPCTSTR(UserNameOfEnum(m_pipeType)), LPCTSTR(m_pSocket->GetRemoteName()));
			
			if (localPipeType != CIPC_ALARM_CLIENT) 
			{
				m_wPipeId1 = m_wThePipeIdCounter++;
				m_wPipeId2 = m_wThePipeIdCounter++;	// OOPS not always increment
				
				// create the pipe
				m_pPipe = new CIPCPipeSocket(this, 
					sSharedMemory, 
					m_pipeType,
					FALSE,
					fetchResult.GetOptions(),
					m_wPipeId1
					);
#ifdef _UNICODE
				m_pPipe->SetCodePage(fetchResult.GetCodePage());
#endif
				/// criss cross
				m_pPipe->m_pServerSocket = m_pSocket;
				m_pPipe->StartThread(TRUE);
				m_pDoc->m_ServerPipes.Add(m_pPipe);
				m_pSocket->SetCIPCPipeSocket(m_pPipe);
				m_pPipe->SetRemoteId(wId1);
			} 
			else 
			{
				//  ALARM stuff already handled above 
				// always set the alarm, even if the following Fetch fails.
			}
		} 
		else if (bFetch)
		{
			WK_TRACE(_T("ID:%u FetchXXX failed\n"), m_wId);
			// CAVEAT to no close the socket here, it should still send the answer
			// which is done below
		}
	} // end of versions match
	
	// send confirmation or error indication
	CIPCExtraMemory *pAnswerBubble = fetchResult.CreateBubble(GetDoc()->m_pCipcControl);
	WK_TRACE(_T("adding FetchResult to send queue <%s><%s>\n"),
		NameOfEnum(fetchResult.GetError()), fetchResult.GetErrorMessage());
	
	AddToSendQueue(new CSocketCmdRecord(m_wPipeId1,m_wPipeId2,SRV_CONTROL_CONFIRM_PIPE,0,0,0,0, pAnswerBubble),TRUE);
	if (m_pPipe)
	{
		m_state = CTS_CONNECTED;	
		HandleVirtualAlarm(TRUE);
	}
	else
	{
		m_state = CTS_REJECTED;
	}
	
	WK_DELETE(pAnswerBubble);
}
/////////////////////////////////////////////////////////////////////////
void CConnectionThread::OnSend()
{
	if (m_state == CTS_CONNECTED_ACTIVE)
	{
		WK_TRACE(_T("SendConnectionRecord %s\n"),GetHostName());
		SendConnectionRecord();
		m_state = CTS_CONNECTED_ACTIVE_UNCONFIRMED;
	}
}
/////////////////////////////////////////////////////////////////////////
void CConnectionThread::OnClose(int iError)
{
	if (m_pPipe) 
	{
		m_pPipe->OnClose(iError);
	}
	if (m_pPipe2) 
	{
		m_pPipe2->OnClose(iError);
	}
	DoTerminate();
}
/////////////////////////////////////////////////////////////////////////
void CConnectionThread::OnRequestDisconnect()
{
	if (WK_IS_WINDOW(m_pMainWnd))
	{
		m_pMainWnd->PostMessage(WM_USER,WPARAM_DISCONNECT);
	}
}
/////////////////////////////////////////////////////////////////////////
void CConnectionThread::CloseConnection()
{
	if (m_pPipe)
	{
		m_pPipe->StopThread();
	}
	if (m_pPipe2)
	{
		m_pPipe2->StopThread();
	}
	if (m_pSocket)
	{
		m_pSocket->Close();
	}

	DoTerminate();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CConnectionThread::CheckPermission(CConnectionRecord &connectIn,
										CIPCFetchResult &fetchResult
										)
{
	BOOL bAccessGranted=FALSE;

#ifdef _DTS
	bAccessGranted = TRUE;
#else
	CIPCError denyError= CIPC_ERROR_INVALID_PERMISSIONS;
	int iDenyErrorCode = 0;
	CString sDenyErrorMessage;
	sDenyErrorMessage.LoadString(IDS_INCOMMING_PERMISSION_INVALID);

	CWK_Profile wkp;
	CHostArray hosts;
	hosts.Load(wkp);
	CString sOwnName = hosts.GetLocalHostName();

	TRACE(_T("DEBUG Incoming permission %s[%s]\n"),
				connectIn.GetPermission(),
				connectIn.GetPassword() 
		 );

	// empty permission
	// find user
	CSecID idPermission;
	if (connectIn.GetPermission().IsEmpty())
	{
		CUserArray users;
		users.Load(wkp);
		CUser* pUser = NULL;
		for (int i=0;i<users.GetSize();i++)
		{
			pUser = users.GetAtFast(i);
			if (   pUser
				&& pUser->GetName() == connectIn.GetUser()
				&& pUser->GetPassword() == connectIn.GetPassword())
			{
				idPermission = pUser->GetPermissionID();
				break;
			}
		}
	}

	CPermissionArray permissions;
	permissions.Load(wkp);
	const CPermission* pPermission = NULL;
	if (idPermission.IsPermissionID())
	{
		pPermission = permissions.GetPermission(idPermission);
		connectIn.SetPassword(pPermission->GetPassword());
		connectIn.SetPermission(pPermission->GetName());
	}
	else
	{
		pPermission = permissions.CheckPermission(connectIn);
	}
	if (pPermission)
	{
		bAccessGranted = hosts.CheckHost(connectIn.GetSourceHost(), pPermission);
		if (bAccessGranted)
		{
			// fine, access granted 
			denyError = CIPC_ERROR_OKAY;
			iDenyErrorCode = 0;
			sDenyErrorMessage = _T("");
		}
		else
		{
			// host unknown, access denied
			WK_TRACE(_T("ID:%u Invalid access host '%s', permission '%s'\n"),
							m_wId,
							connectIn.GetSourceHost(), 
							connectIn.GetPermission()
					);
			denyError = CIPC_ERROR_INVALID_PERMISSIONS;
			iDenyErrorCode = 2;
			sDenyErrorMessage.LoadString(IDS_INCOMMING_PERMISSION_INVALID);
		}
	}
	else
	{
		// an invalid permission
		WK_TRACE(_T("ID:%u Invalid access unknown permission or wrong password for '%s'\n"),
				m_wId,
				connectIn.GetPermission()
				);
		denyError = CIPC_ERROR_INVALID_PERMISSIONS;
		iDenyErrorCode = 1;
		sDenyErrorMessage.LoadString(IDS_INCOMMING_PERMISSION_INVALID);
	}

	if (bAccessGranted==FALSE)
	{
		WK_TRACE(_T("Access denied\n"));
		// acess denied fill fetchResult
		CIPCFetchResult tmpResult(sOwnName,	// hidden in shmName
								  denyError,
								  iDenyErrorCode,
								  sDenyErrorMessage,
								  0,
								  SECID_NO_ID
#ifdef _UNICODE
								  , CODEPAGE_UNICODE
#endif
								  );
		fetchResult=tmpResult;
	}
	hosts.DeleteAll();
#endif
	return bAccessGranted;
}

//////////////////////////////////////////////////////////////////////////////////////

BOOL CConnectionThread::HandleVirtualAlarm(BOOL bConnect)
{
// Beim DTS-System soll jeder Call protokoliert werden!
#if _DTS
	BOOL bFoundHostName = FALSE;
	POSITION pos = NULL;

	if (!m_bActivePeer)	// Incomming call
	{
		if (bConnect)	// Connect	
		{
			// Hat der Host schon eine aktive Verbindung?
			for (int i = 0; i < m_ConnectedHosts.GetCount(); i++)
			{
				POSITION pos = m_ConnectedHosts.FindIndex(i);
				if (pos)
				{
					CString sHost = *(CString*)m_ConnectedHosts.GetAt(pos);
					if (GetHostName().Find(sHost) != -1)
					{
						bFoundHostName = TRUE;
						break;
					}
				}
			}

			// Nur bei der ersten Verbindung einen virtuellen Alarm auslösen.
			if (!bFoundHostName)
			{
				WK_TRACE(_T("ID:%u ### Connect to %s\n"), m_wId, GetHostName());
				m_ConnectedHosts.AddHead(new CString(GetHostName()));

				// Virtuellen Online-Alarm auslösen...
				CString sOnlineMsg;
				sOnlineMsg.Format(IDS_INCOMING_CALL,GetHostName());
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_INCOMING_CALL, sOnlineMsg,0,1);	
				runtimeError.Send();
				Sleep(50);
			}
		}
		else	// Disconnect
		{
			// Host in der Verbindungsliste suchen und austragen.
			for (int i = 0; i < m_ConnectedHosts.GetCount(); i++)
			{
				pos = m_ConnectedHosts.FindIndex(i);
				if (pos)
				{
					CString sHost = *(CString*)m_ConnectedHosts.GetAt(pos);

					if (GetHostName().Find(sHost) != -1)
					{
						WK_TRACE(_T("ID:%u ### Disconnect %s\n"), m_wId, GetHostName());
						WK_DELETE(m_ConnectedHosts.GetAt(pos));
						m_ConnectedHosts.RemoveAt(pos);
						bFoundHostName = TRUE;
						break;
					}
				}
			}
			if (bFoundHostName)
			{
				// Virtuellen Online-Alarm auslösen...
				CString sOnlineMsg;
				sOnlineMsg.Format(IDS_CLOSE_INCOMING_CALL,GetHostName());
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_INCOMING_CALL, sOnlineMsg,0,0);	
				runtimeError.Send();
				Sleep(50);
			}
		}
	}
#else
	if (bConnect)	// Connect
	{
		if (m_bActivePeer)	// Outgoing
		{
			if (m_nOutgoingCall == 0)
			{
				// Virtuellen Online-Alarm auslösen...
				CString sOnlineMsg;
				sOnlineMsg.Format(IDS_OUTGOING_CALL,GetHostName());
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_OUTGOING_CALL, sOnlineMsg,0,1);	
				runtimeError.Send();
				Sleep(50);
			}
			m_nOutgoingCall++;
		}
		else	// Incoming
		{
			if (m_nIncomingCall == 0)
			{
				CString sOnlineMsg;
				sOnlineMsg.Format(IDS_INCOMING_CALL,GetHostName());
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_INCOMING_CALL, sOnlineMsg,0,1);	
				runtimeError.Send();
				Sleep(50);
			}
			m_nIncomingCall++;
		}
	}
	else	// Disconnect
	{
		if (m_bActivePeer)	// Outgoing
		{
			m_nOutgoingCall--;
			m_nOutgoingCall = max(m_nOutgoingCall, 0);

			if (m_nOutgoingCall == 0)
			{
				// Virtuellen Offline-Alarm auslösen...
				CString sOnlineMsg;
				sOnlineMsg.Format(IDS_CLOSE_OUTGOING_CALL,GetHostName());
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_OUTGOING_CALL, sOnlineMsg,0,0);	
				runtimeError.Send();
				Sleep(50);
			}
		}
		else	// Incoming
		{
			m_nIncomingCall--;
			m_nIncomingCall = max(m_nIncomingCall, 0);

			if (m_nIncomingCall == 0)
			{
				// Virtuellen Offline-Alarm auslösen...
				CString sOnlineMsg;
				sOnlineMsg.Format(IDS_CLOSE_INCOMING_CALL,GetHostName());
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_INCOMING_CALL, sOnlineMsg,0,0);	
				runtimeError.Send();
				Sleep(50);
			}
		}

		if (m_state == CTS_REJECTED)	// Reject 
		{
			UINT nID;
			if (m_bActivePeer)	// Outgoing
			{
				nID = IDS_REJECT_OUTGOING_CALL;
			}
			else
			{
				nID = IDS_REJECT_INCOMING_CALL;
			}

			// Virtuellen Reject-Alarm auslösen...
			CString sOnlineMsg;
			sOnlineMsg.Format(nID,GetHostName());
			CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_REJECT, sOnlineMsg,0,1);	
			runtimeError.Send();
			Sleep(50);
			CWK_RunTimeError runtimeError2(REL_MESSAGE, RTE_REJECT, sOnlineMsg,0,0);	
			runtimeError2.Send();
			Sleep(50);
		}
		else
		{

		}
	}
#endif
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CConnectionThread::ResetSyncTimer()
{
	m_dwLastSyncCheck         = GetTickCount();
	m_ConnectionState         = CONNECTION_OK;
	//m_dwSyncSendTime		  = m_dwLastSyncCheck;
}
/////////////////////////////////////////////////////////////////////////////
void CConnectionThread::Send()
{
	if (   m_pSocket
		&& m_pSocket->CanSend())
	{
		m_pSocket->DoSend();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CConnectionThread::DoTerminate()
{
	if (WK_IS_WINDOW(m_pMainWnd))
	{
		m_pMainWnd->PostMessage(WM_CLOSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CConnectionThread::ThreadsInitDone()
{
	return m_bInitDone;
}
/////////////////////////////////////////////////////////////////////////////
void CConnectionThread::Sync()
{
		// sync action...

	if((m_ConnectionState!=CONNECTION_OK) && 
	   (m_ConnectionState!=CONNECTION_CHECK) &&
	   (m_ConnectionState!=CONNECTION_INIT))
	{
		return;
	}

	DWORD dwCurrentTime=GetTickCount();	 

	if (m_ConnectionState==CONNECTION_CHECK) //Versendetes Sync wurde noch nicht bestätigt
	{								  
		if((dwCurrentTime - m_dwSyncSendTime)>(m_dwTheSyncRetryTime*2))
		{
			CString sMsg;

			sMsg.Format(IDS_NO_SYNC_RECEIVED, (m_dwTheSyncRetryTime*2)/1000);
			sMsg += _T("[");
			sMsg += m_sHostName;
			sMsg += _T("]");

			TRACE_SYNC(sMsg);
						
			CWK_RunTimeError runtimeError(WAI_SOCKET_UNIT, REL_MESSAGE, RTE_SU_SYNC_FAIL, sMsg);
			runtimeError.Send();

			if (theApp.m_bTraceSync)
			{
				CString sMsg;
				sMsg.Format(_T("SyncFailed|%s|%s"),m_sHostName,NameOfEnum(GetCIPCType()));
				WK_STAT_PEAK(_T("Reset"),1,sMsg);
			}
			m_ConnectionState=CONNECTION_ERROR;
			TRACE_SYNC(_T("SYNC TIMEOUT"),dwCurrentTime - m_dwSyncSendTime);
			DoTerminate();
		}
	}
	else if (m_ConnectionState==CONNECTION_OK || m_ConnectionState==CONNECTION_INIT)
	{
					// no outstanding sync, so send one
		
		if ((dwCurrentTime - m_dwLastSyncCheck)>=m_dwTheSyncRetryTime)
		{			
			m_dwSyncSendTime = GetTickCount();
			AddToSendQueue(new CSocketCmdRecord(m_wPipeId1,m_wPipeId2,CIPC_REQUEST_SYNC,5678,0,0,0,NULL),FALSE);

			if (theApp.m_bTraceSync)
			{
				CString sMsg;
				sMsg.Format(_T("SyncRequestSendTo|%s|%s"),m_sHostName,NameOfEnum(GetCIPCType()));
				WK_STAT_PEAK(_T("Reset"),1,sMsg);
			}
			m_ConnectionState = CONNECTION_CHECK; 
			m_iSyncRetryCounter++;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////

void CConnectionThread::CheckSyncRunningTime()
{ 
	DWORD dwTime = GetTickCount();
	m_dwDynamicSyncTimeout = (dwTime-m_dwSyncSendTime) + 5000;

	if(m_dwDynamicSyncTimeout<30000)
	{
		m_dwDynamicSyncTimeout = 30000;
	}
	else
	{
		TRACE_SYNC(_T("CHANGE SYNC TIMEOUT TO"),m_dwDynamicSyncTimeout);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//Nur für statistische Zwecke
void CConnectionThread::TRACE_SYNC(CString sMessage,DWORD dwValue)
{
/*	CString sPipeType; 

	CIPCType pipeType = m_pPipe->GetCIPCType();

				if(pipeType == CIPC_INPUT_CLIENT)
				{
					sPipeType = _T("CIPC_INPUT_CLIENT");	
				}
				else if(pipeType == CIPC_OUTPUT_CLIENT)
				{
					sPipeType = _T("CIPC_OUTPUT_CLIENT");
				}
				else if(pipeType == CIPC_DATABASE_CLIENT)			
				{
					sPipeType = _T("CIPC_DATABASE_CLIENT");
				}
				else if(pipeType == CIPC_AUDIO_CLIENT)
				{
					sPipeType = _T("CIPC_AUDIO_CLIENT");
				}
	if(dwValue)
	{	
		if(sMessage.Left(4)==_T("SYNC"))
		{
		  //WK_STAT_PEAK(_T("SYNCPEAK"),dwValue,sPipeType);

		}
		WK_TRACE(_T("Remote IP %s : %s : %s : %i\n"),m_pSocket->GetRemoteName(),sPipeType,sMessage,dwValue);
	}
	else
	{
		WK_TRACE(_T("Remote IP %s : %s : %s\n"),m_pSocket->GetRemoteName(),sPipeType,sMessage);
	}
 */ 
}
////////////////////////////////////////////////////////////////////////////////////
void CConnectionThread::ResetSync()
{
	ResetSyncTimer();
	m_iSyncSocketState=INIT;
	m_iSyncRetryCounter=0;
	m_ConnectionState = CONNECTION_OK;
}
////////////////////////////////////////////////////////////////////////////////////
CString CConnectionThread::Format()
{
	CString s;

	s = m_sHostName;
	if (m_pPipe)
	{
		s += CString(_T(" ")) + m_pPipe->GetShmName();
	}
	if (m_pPipe2)
	{
		s += CString(_T(" ")) + m_pPipe2->GetShmName();
	}

	return s;
}
