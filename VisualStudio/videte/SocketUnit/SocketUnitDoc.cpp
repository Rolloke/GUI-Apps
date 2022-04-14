// SocketUnitDoc.cpp : implementation of the CSocketUnitDoc class
//

#include "stdafx.h"
#include "SocketUnit.h"
#include "SocketUtil.h"

#include "SocketUnitDoc.h"
#include "SocketUnitView.h"
#include "ControlSocket.h"
#include "DataSocket.h"
#include "CipcServerControlClientSide.h"
#include "ServerControlSocket.h"
#include "CipcPipeSocket.h"
#include "ConnectionThread.h"

#include "WK_Version.h"
#include "WK_Names.h"
//#include "WKSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSocketUnitDoc
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSocketUnitDoc, CDocument)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSocketUnitDoc, CDocument)
	//{{AFX_MSG_MAP(CSocketUnitDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSocketUnitDoc construction/destruction
/////////////////////////////////////////////////////////////////////////////
CSocketUnitDoc::CSocketUnitDoc()
{
	m_dwLastReconnectTry=0;
	m_pCipcControl=NULL;
	m_pControlSocket=NULL;

	WK_TRACE(_T("Default bitrate %d KB\n"),theApp.GetSelectedBitrate()/1024);
}
/////////////////////////////////////////////////////////////////////////////
void CSocketUnitDoc::StopAllThreads()
{
	BOOL bAllDone=FALSE;
	int iNumRetries=0;

	while (bAllDone==FALSE) 
	{
		m_threads.Lock();
		if ( m_threads.GetSize() ) 
		{
			m_threads[0]->OnRequestDisconnect();
			
			m_threads.Unlock();
			Sleep(50);
		} else {
			m_threads.Unlock();
			bAllDone=TRUE;
		}
		iNumRetries++;
		if (iNumRetries>100) 
		{
			WK_TRACE_ERROR(_T("QUIT FAILED\n"));
			bAllDone=TRUE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CSocketUnitDoc::~CSocketUnitDoc()
{
	/*
	if (m_threads.GetSize()) {
		WK_TRACE(_T("Still have %d connection threads...\n"),m_threads.GetSize());
	}

	StopAllThreads();


	WK_DELETE(m_pControlSocket);	// OOPS
	WK_DELETE(m_pControlSocketOld);

	WK_DELETE(m_pCipcControl);*/
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
/*
static void DoTest()
{
	TRACE(_T("DoTest...\n"));
	// TEST
	CString sQuery;
	CString sResult;

	sQuery=_T("134.245.76.77"); sResult = CWK_DNS::LookUp(sQuery);
	TRACE(_T("::WK_DNSLookUp(%s)->%s\n"), sQuery, sResult);

	sQuery=_T("hermelin"); sResult = CWK_DNS::LookUp(sQuery);
	TRACE(_T("::WK_DNSLookUp(%s)->%s\n"), sQuery, sResult);
	sQuery=_T("hermelin.wk.de"); sResult = CWK_DNS::LookUp(sQuery);
	TRACE(_T("::WK_DNSLookUp(%s)->%s\n"), sQuery, sResult);
	sQuery=_T("foo"); sResult = CWK_DNS::LookUp(sQuery);
	TRACE(_T("::WK_DNSLookUp(%s)->%s\n"), sQuery, sResult);
	sQuery=_T("hermelinAlt"); sResult = CWK_DNS::LookUp(sQuery);
	TRACE(_T("::WK_DNSLookUp(%s)->%s\n"), sQuery, sResult);
	sQuery=_T("hermelinAlt.wk.de"); sResult = CWK_DNS::LookUp(sQuery);
	TRACE(_T("::WK_DNSLookUp(%s)->%s\n"), sQuery, sResult);
	sQuery=_T("134.245.76.78"); sResult = CWK_DNS::LookUp(sQuery);
	TRACE(_T("::WK_DNSLookUp(%s)->%s\n"), sQuery, sResult);
	TRACE(_T("DoTest done.\n"));
}
 */
#endif
/////////////////////////////////////////////////////////////////////////////
BOOL CSocketUnitDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) {
		return FALSE;
	}

	BOOL okay=FALSE;

	// DoTest();

	m_pCipcControl = new CServerControlSocket(this, WK_SMSocketControl);
	m_pCipcControl->StartThread(TRUE);

	if (CWK_DNS::m_bUseDNS) {
		WK_TRACE(_T("DomainNameService (DNS) enabled\n"));
	} else {
		WK_TRACE(_T("DomainNameService (DNS) not enabled\n"));
	}


	const int iMaxHostLen=500;
	char szLocalHostName[iMaxHostLen];
	int iResult = gethostname (szLocalHostName, iMaxHostLen);
	switch (iResult) 
	{
	case 0:
		WK_TRACE(_T("Local TCP/IP hostname is '%s'\n"), CString(szLocalHostName));
		break;
	case WSAENETDOWN:
		WK_TRACE(_T("Network down !?\n")); 
		break;
	default:
		WK_TRACE(_T("Failed to get local hostname\n"));
		break;
	};

	if (theApp.IsListeningEnabled()) 
	{
		m_pControlSocket = new CControlSocket(this);
		if (m_pControlSocket)
		{
			if (m_pControlSocket->Create(VIDETE_CIPC_PORT,SOCK_STREAM))
			{
				m_pControlSocket->AsyncSelect(FD_READ | FD_ACCEPT | FD_CLOSE );	
				if (m_pControlSocket->Listen()) 
				{
					WK_TRACE(_T("Listen OK on PORT %d\n"),VIDETE_CIPC_PORT);
					okay = TRUE;
				} 
				else 
				{
					WK_TRACE_ERROR(_T("listen failed %d,%s\n"),VIDETE_CIPC_PORT,GetLastErrorString());
					okay=FALSE;
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("Failed to create listen port. %s\n"),GetLastErrorString());
			}
		} 
		else 
		{
			WK_TRACE_ERROR(_T("Failed to create listen port.\n"));
		}

		if (okay==FALSE) 
		{
			return FALSE;	// <<< EXIT >>>
		}
	} 
	else 
	{
		WK_TRACE(_T("Listen disabled\n"));
		okay=TRUE;
	}

	if (!theApp.GetAllowOutgoingCalls()) 
	{
		WK_TRACE(_T("Outgoing calls disabled\n"));
	}

	return okay;
}
/////////////////////////////////////////////////////////////////////////////
// CSocketUnitDoc diagnostics
#ifdef _DEBUG
void CSocketUnitDoc::AssertValid() const
{
	CDocument::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CSocketUnitDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CSocketUnitDoc::ProcessPendingAccept(CControlSocket *pControl)
{
	
	// OOPS unused pConnection ?
	// OOPS port number not used ?
	CDataSocket* pSocket = new CDataSocket(NULL);

	if (pControl->Accept(*pSocket))
	{
		CString sName,sPeerName;

		sName	  = pSocket->GetName();
		sPeerName = pSocket->GetRemoteName();

		if (!ScanHostList(pSocket))
		{
			WK_TRACE(_T("Denied. <%s> is not in the hostlist\n"), sPeerName);
			pSocket->Detach();
			WK_DELETE(pSocket);
			// TODO! RKE: implement RuntimeError for rejected call
			return;
		}

		CString msg;
		msg.Format(IDS_INCOMING_CALL,sPeerName);
		WK_TRACE(_T("%s\n"),msg);

		if (MyGetApp()->IsBeepOnIncomingEnabled()) 
		{
			MessageBeep((DWORD)-1);
		}

		SOCKET hSocket = pSocket->Detach();
		WK_DELETE(pSocket);

		CConnectionThread *pThread;
		pThread = new CConnectionThread(this,hSocket);

		AddNewThread(pThread);
		// pre 080199 m_threads.Lock(); 	m_threads.Add(pThread); 	m_threads.Unlock();
		if(!pThread->CreateThread())
		{
			WK_TRACE(_T("CouldnIt Create Connection Thread\n"));
			WK_DELETE(pSocket);
			return;
		}

		int iNumRetries=0;
		while (!(pThread->ThreadsInitDone()) && iNumRetries<1000) 
		{
			Sleep(50);	// wait for ConnectionThread to start, max 1000 retries
			iNumRetries++;

		}
		if (iNumRetries<1000)  // OKAY
		{
			if (iNumRetries>10)
			{
				WK_TRACE(_T("Waited %d ms for connectionThread (in accept)\n"),iNumRetries*100);
			}
		} 
		else // FAILED
		{
			
			WK_TRACE(_T("Failed to create connection thread (in accept)\n"));
		}
	} 
	else // NOT YET check error
	{
		WK_TRACE(_T("OnAccept failed %s\n"),GetLastErrorString());
		delete pSocket;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSocketUnitDoc::SaveModified( )
{
	return TRUE;	// it's safe to close
}
/////////////////////////////////////////////////////////////////////////////
void CSocketUnitDoc::CheckConnections() 
{
	CIPCPipeSocket* pPipe;
	m_csPipes.Lock();
	for (int i=0 ; i<m_ClientPipes.GetSize() ; i++) 
	{
		pPipe = m_ClientPipes.GetAt(i);
		if (pPipe->IsTimedOut() && !pPipe->GetIsMarkedForDelete())
		{
			WK_TRACE(_T("Pending Pipe remove %s %s 0x%x\n"),NameOfEnum(pPipe->GetCIPCType()), pPipe->GetShmName(), pPipe->GetId() );
			pPipe->Disconnect();
		}
	}
	m_csPipes.Unlock();

	m_threads.Lock();
	for (int i=0;i<m_threads.GetSize();i++)
	{
		CConnectionThread* pThread = m_threads.GetAtFast(i);
		pThread->Sync();
	}

	m_threads.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CSocketUnitDoc::RemoveMe(CIPCPipeSocket *pPipe)
{
	if (pPipe==NULL) {
		return;
	}

	int i;
	m_csPipes.Lock();


	for (i=0;i<m_ClientPipes.GetSize();i++) {
		if (m_ClientPipes[i]==pPipe) {
			m_ClientPipes.RemoveAt(i);
			m_csPipes.Unlock();
			return;	// EXIT
		}
	}
	for (i=0;i<m_ServerPipes.GetSize();i++) {
		if (m_ServerPipes[i]==pPipe) {
			m_ServerPipes.RemoveAt(i);
			m_csPipes.Unlock();
			return;	// EXIT
		}
	}
	// OOPS still called twice TRACE(_T("RemoveMe pipe NOT FOUND!?\n"));
	m_csPipes.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CSocketUnitDoc::CloseAllConnections() 
{
	WK_TRACE(_T("Closing all connections\n"));
	StopAllThreads();
}
/////////////////////////////////////////////////////////////////////////////
void CSocketUnitDoc::AddNewThread(CConnectionThread *pThread)
{
	m_threads.Lock();
	if (pThread) {
		m_threads.Add(pThread);
	}

	m_threads.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CSocketUnitDoc::RemoveThread(CConnectionThread *pThread)
{
	m_threads.Lock();
	m_threads.Remove(pThread);
	m_threads.Unlock();
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CSocketUnitDoc::ScanHostList(CDataSocket* pSocket)
{
	CWK_Profile prof;
	CWK_String sIP;
	CString sLineNr;
	CString sCallingIP;
	CString sCallingName;

	int nI = 0;
	BOOL bFoundInList = FALSE;

	if (!pSocket)
	{
		return bFoundInList;
	}

	// Soll die Rufnummer in der Hostliste gesucht werden?
	if (prof.GetInt(_T("SocketUnit"), _T("UseHostList"), 0) == 0)
		return TRUE;

	// IPAdresse holen
	UINT nPort;
	pSocket->GetPeerName(sCallingIP, nPort);
	sCallingName = pSocket->GetRemoteName();
	sCallingName.MakeLower();
	TRACE(_T("Calling party: %s, %s, DNS:%d\n"), sCallingIP, sCallingName, CWK_DNS::m_bUseDNS);

	// Wenn keine IP-Adresse übermittelt wurde, auch keine Suche starten
	if (sCallingIP.IsEmpty())
	{
		return TRUE;
	}

	// Nummer in der Hostliste suchen.
	do
	{
		sLineNr.Format(_T("%05d"), nI++);
		sIP.CleanUp();	// cleanup asscii string before assignement
		sIP	= prof.GetString(_T("SocketUnit\\HostList"), sLineNr, NULL);
		if (sIP.IsEmpty())
		{
			break;
		}
		if (CWK_DNS::m_bUseDNS)
		{
			DWORD dwAddr = inet_addr(sIP);
			if (dwAddr == INADDR_NONE)
			{
				sIP.MakeLower();
				if (sCallingName.Find(sIP) != -1)
				{
					bFoundInList = TRUE;
					break;
				}
				TRACE(_T("%s\n"), LPCTSTR(sIP));
				struct hostent FAR *pHost = NULL;
				pHost = gethostbyname(sIP);
				if (pHost && pHost->h_addrtype == AF_INET && pHost->h_length == 4)
				{
					in_addr ia;
					ia.S_un.S_addr = *((DWORD*)*((DWORD*)pHost->h_addr_list));
					CString s(inet_ntoa(ia));
					sIP.CleanUp();	// cleanup asscii string before assignement
					sIP = s;
					TRACE(_T("%s\n"), LPCTSTR(sIP));
				}
			}
		}
		// Nummer gefunden?
		if (sIP == sCallingIP)
		{
			bFoundInList = TRUE;
			break;
		}
	
	}while (!bFoundInList);

	return bFoundInList;
}
/////////////////////////////////////////////////////////////////////////////////////
void CSocketUnitDoc::PreCloseFrame( CFrameWnd* pFrame )
{
	if (m_threads.GetSize()) {
		WK_TRACE(_T("Still have %d connection threads...\n"),m_threads.GetSize());
	}

	StopAllThreads();


	WK_DELETE(m_pControlSocket);	// OOPS
	WK_DELETE(m_pCipcControl);
}
