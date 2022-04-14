// SocketUnit.cpp : Defines the class behaviors for the application.
//


#include "stdafx.h"

#include "SocketUnit.h"

#include "MainFrm.h"
#include "SocketUnitDoc.h"
#include "SocketUnitView.h"
#include "SocketUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define BEEPONINCOMING		_T("BeepOnIncoming")
#define DOLISTEN			_T("DoListen")
#define ENABLED				_T("enabled")
#define DISABLED			_T("disabled")
#define ALLOWED				_T("allowed")
#define ALLOWOUTGOINGCALLS	_T("AllowOutgoingCalls")
#define SYNCRETRYTIMEMS		_T("SyncRetryTimeMS")
#define DEFAULTBITRATE		_T("DefaultBitrate")
#define USEDNS				_T("UseDNS")
#define CACHEKNOWNNAMES		_T("CacheKnownNames")
#define CACHEUNKNOWNNAMES	_T("CacheUnknownNames")
#define ALLOWANYHOST		_T("AllowAnyHost")
#define DENYNOHOST			_T("DenyNoHost")
#define NUMBEROFALLOW		_T("NumberOfAllow")
#define NUMBEROFDENY		_T("NumberOfDeny")

static BOOL DoesSupportTCPIP();	// forward declaration

// static void CalcBitCount();

static CSingleDocTemplate* pDocTemplate=NULL;

/////////////////////////////////////////////////////////////////////////////
// CSocketUnitApp

BEGIN_MESSAGE_MAP(CSocketUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CSocketUnitApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_CLOSE_ALL, OnCloseAll)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSocketUnitApp construction

CSocketUnitApp::CSocketUnitApp()
{
#ifdef _DTS
	m_bDoBeepOnIncoming = FALSE;
#else
	m_bDoBeepOnIncoming = TRUE;
#endif
	m_bDoListen = TRUE;

	m_dwSelectedBitrate = 1024 * 1024;
	m_bAllowAnyHost = TRUE;
	m_bDenyNoHost = FALSE;
	m_bTraceSync = FALSE;
	m_bAllowOutgoingCalls = TRUE;
	m_bIsDemo = FALSE;
	m_bIsDTSTransmitter = FALSE;
	m_pSendRate = NULL;
	m_pReceiveRate = NULL;
}

CSocketUnitApp::~CSocketUnitApp()
{
	WK_DELETE(m_pSendRate);
	WK_DELETE(m_pReceiveRate);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSocketUnitApp object

CSocketUnitApp theApp;

CString GetLocalIP()
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
	
			int j;
			
			for( j = 0; j < pHost->h_length; j++ )
			{
				CString addr;
				
				if( j > 0 )
					str += _T(".");
				
				addr.Format(_T("%u"),(unsigned int)((unsigned char*)pHost->h_addr_list[i])[j]);
				str += addr;
			}
		}
	}
	return str;

}
/////////////////////////////////////////////////////////////////////////////
// CSocketUnitApp initialization
BOOL CSocketUnitApp::InitInstance()
{
//	PrintPSC();
//	CalcBitCount();

	XTIB::SetThreadName(_T("SocketUnitApp"));

	InitDebugger(_T("SocketUnit.log"), WAI_SOCKET_UNIT);
	CWK_RunTimeError::SetDefaultID(WAI_SOCKET_UNIT);	// sets the static member


	WSADATA socketInfo;
	if (!AfxSocketInit(&socketInfo))
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	/*
	struct WSAData {
    WORD             wVersion;
    WORD             wHighVersion;
    char             szDescription[WSADESCRIPTION_LEN+1];
    char             szSystemStatus[WSASYSSTATUS_LEN+1];
    unsigned short   iMaxSockets;
    unsigned short   iMaxUdpDg;
    char FAR *       lpVendorInfo;
	}
	*/

	WK_TRACE(_T("SocketInfo: Version %d/%d\n\tmaxSockets:%d\n\tmaxUDP:%d\n\tStatus:%s\n\tDescription:%s\n"),
		socketInfo.wHighVersion,
		socketInfo.wVersion,
		socketInfo.iMaxSockets,
		socketInfo.iMaxUdpDg,
		CString(socketInfo.szSystemStatus),
		CString(socketInfo.szDescription)
		);

	if (WK_ALONE(WK_APP_NAME_SOCKETS)==FALSE) return FALSE;

	CString sTitle = COemGuiApi::GetApplicationName(WAI_SOCKET_UNIT);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

	// OBSOLETE WK_STAT_TICK_COUNT = GetTickCount();

	WK_STAT_LOG(_T("Reset"),1,_T("Active"));
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER >= 0x0710
	SetLanguageParameters(0, 0);
#else
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif // _AFXDLL
#endif // _MFC_VER >= 0x0710

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	WNDCLASS  wndclass ;

	// register window class
	wndclass.style =         0;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_SOCKETS;

	AfxRegisterClass(&wndclass);
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	wndclass.lpszClassName = _T("SocketDummy");
	AfxRegisterClass(&wndclass);

	// <HEDUTEST>
	WORD wVersionRequested;  
	WSADATA wsaData; 
	int err; 
	wVersionRequested = MAKEWORD(1, 1); 
 
	err = WSAStartup(wVersionRequested, &wsaData); 
 
	if (err != 0) 
	{
	}
	else
	{
		// fine
		TRACE(_T("WSAStartUp fine\n"));
	}
	// </HEDUTEST>

	// dongle check via applicationId
	CWK_Dongle dongle(WAI_SOCKET_UNIT);
	if (dongle.IsExeOkay()==FALSE) {	// already sends RunTimeError 
		return FALSE;
	}
	m_bIsDemo = dongle.IsDemo();
	m_bIsDTSTransmitter = dongle.GetProductCode() == IPC_DTS;
	switch(dongle.GetProductCode())
	{
		case IPC_DTS_RECEIVER:
		case IPC_DTS:
		case IPC_DTS_IP_RECEIVER:
		{
			CWK_Profile wkp;
			wkp.WriteString(_T("Version"), _T("Number"), COemGuiApi::GetSoftwareVersion());
		}break;
	}

	CWK_Profile wkp;
	if (DoesSupportTCPIP()) 
	{
		WK_TRACE(_T("Found suitable TCP/IP protocol support\n"));
		char str[_MAX_PATH];
		gethostname(str,_MAX_PATH);
		CString s(str);
		WK_TRACE(_T("Host Name is %s\n"),s);
		m_sIPAddress = GetLocalIP();
		WK_TRACE(_T("IP Adress is %s\n"), m_sIPAddress);

		if (m_bIsDTSTransmitter)
		{
			CString sLocalIP(m_sIPAddress);
			wkp.Encode(sLocalIP);
#ifdef _UNICODE
			wkp.WriteString(REGKEY_SOCKETUNIT, _T("OwnNumber"), sLocalIP, TRUE);
#else
			wkp.WriteString(REGKEY_SOCKETUNIT, _T("OwnNumber"), sLocalIP);
#endif			
		}
	} 
	else 
	{
		CString sLangMsg;
		if (sLangMsg.LoadString(IDS_NO_TCP_IP_PROTOCOL)==FALSE) {
			sLangMsg=_T("Could not find suitable TCP/IP protocol support");
		}

		CWK_RunTimeError runtimeError(WAI_SOCKET_UNIT, REL_CANNOT_RUN, RTE_CONFIGURATION, sLangMsg);
		runtimeError.Send();

		WK_TRACE(_T("Could not find suitable TCP/IP protocol support\n"));
		Sleep(500);	// let the DebuggerThread print the error message

		CloseDebugger();
		return FALSE;
	}


	ReadRegistrySettings();

	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSocketUnitDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSocketUnitView));
	AddDocTemplate(pDocTemplate);


	// do not touch
	if (m_nCmdShow!=0) // do not touch if already hidden
	{	
		m_nCmdShow = SW_HIDE;
	}

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	return TRUE;
}


// App command to run the dialog
void CSocketUnitApp::OnAppAbout()
{
	COemGuiApi::AboutDialog(this->m_pMainWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CSocketUnitApp commands

int CSocketUnitApp::ExitInstance() 
{
	CWK_DNS::Reset();

	if (pDocTemplate) {	// might be NULL, DongleCheck)

		POSITION pos = pDocTemplate->GetFirstDocPosition();
		while (pos != NULL)
		{
			CSocketUnitDoc* pDoc= (CSocketUnitDoc*)pDocTemplate->GetNextDoc(pos);
			if (pDoc) {
				pDoc->OnCloseDocument();
			}

		}
	}
	WK_STAT_LOG(_T("Reset"),0,_T("Active"));

	CloseDebugger();

	return CWinApp::ExitInstance();
}

#include "nspapi.h"

static BOOL DoesSupportTCPIP()
{
	const int iMaxProtocols = 500;
	INT buffer[iMaxProtocols];
	const PROTOCOL_INFO * pProtocols = (PROTOCOL_INFO*) buffer;	// CAST
	DWORD dwNumBytesUsed= iMaxProtocols*sizeof(INT);;
    int iNumFound = EnumProtocols( NULL, buffer, & dwNumBytesUsed ); 
	if (iNumFound < 0) {
		WK_TRACE_ERROR(_T("error in protocol check!\n"));
		WK_TRACE_ERROR(_T("Might be a non-Microsoft socket dll.\n"));
		WK_TRACE_ERROR(_T("Assuming there is TCP/IP available.\n"));
		return TRUE;	// <<< EXIT >>>
	} else if (iNumFound==0) {
		WK_TRACE_ERROR(_T("no protocol found in protocol check\n"));
	} else {
		WK_TRACE(_T("Found %d protocols, checking for suitable TCP/IP....\n"), iNumFound);
		for (int i=0;i<iNumFound;i++) {
		    const PROTOCOL_INFO *pOneInfo = &(pProtocols[i]);
			if (pOneInfo->iProtocol==IPPROTO_TCP) {
					if ( (pOneInfo->dwServiceFlags & XP_GUARANTEED_DELIVERY) ) {
						WK_TRACE(_T("protocol has XP_GUARANTEED_DELIVERY\n"));
					}
					if ( (pOneInfo->dwServiceFlags & XP_GUARANTEED_ORDER) ) {
						WK_TRACE(_T("protocol has XP_GUARANTEED_ORDER\n"));
					}

					 if ( (pOneInfo->dwServiceFlags & XP_GUARANTEED_DELIVERY)
					&&  (pOneInfo->dwServiceFlags &  XP_GUARANTEED_ORDER)) { 
						 // fine
						 return TRUE;	// <<< EXIT >>>
					}

			}
		}	// end of loop over protocols
		// NOT YET some fine tracemessages about the found protocols
	}	// end of some protocols found

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////
void CSocketUnitApp::ReadRegistrySettings()
{
	CString sMsg;
	CWK_Profile wkp;

	m_bDoBeepOnIncoming = wkp.GetInt(REGKEY_SOCKETUNIT,BEEPONINCOMING,m_bDoBeepOnIncoming);
	sMsg = _T("MessageBeep for incoming calls ");
	if (m_bDoBeepOnIncoming) {
		sMsg += ENABLED;
	} else {
		sMsg += DISABLED;
	}
	WK_TRACE(_T("%s.\n"), LPCTSTR(sMsg));

	m_bDoListen= wkp.GetInt(REGKEY_SOCKETUNIT,DOLISTEN,TRUE);
	sMsg=_T("Listen ");

	if (m_bDoListen) {
		sMsg += ENABLED;
	} else {
		sMsg += DISABLED;
	}
	WK_TRACE(_T("%s.\n"), LPCTSTR(sMsg));

	m_bAllowOutgoingCalls= wkp.GetInt(REGKEY_SOCKETUNIT,ALLOWOUTGOINGCALLS,TRUE);
	sMsg = _T("Outgoing calls ");

	if (m_bAllowOutgoingCalls) {
		sMsg += ALLOWED;
	} else {
		sMsg += DISABLED;
	}
	WK_TRACE(_T("%s.\n"), LPCTSTR(sMsg));

	CConnectionThread::m_dwTheSyncRetryTime= wkp.GetInt(REGKEY_SOCKETUNIT,SYNCRETRYTIMEMS,30000);

	if (CConnectionThread::m_dwTheSyncRetryTime<500) {
		WK_TRACE(_T("Minimum SyncRetryTime is 500 ms, adjusting %d\n"),
			CConnectionThread::m_dwTheSyncRetryTime
			);
	}
	WK_TRACE(_T("SyncRetryTime %d ms.\n"),CConnectionThread::m_dwTheSyncRetryTime);


	if (m_bIsDemo)
	{
		m_dwSelectedBitrate = 64;
	}
	else
	{
		m_dwSelectedBitrate = wkp.GetInt(REGKEY_SOCKETUNIT,DEFAULTBITRATE,256);
	}
	WK_TRACE(_T("Default selected bitrate is %d\n"),m_dwSelectedBitrate,
			m_dwSelectedBitrate
			);
	m_dwSelectedBitrate *= 1024;


	// DNS
	CWK_DNS::m_bUseDNS = wkp.GetInt(REGKEY_SOCKETUNIT,USEDNS,FALSE);
	CWK_DNS::m_bDoCacheKnownNames = wkp.GetInt(REGKEY_SOCKETUNIT,CACHEKNOWNNAMES,TRUE);
	CWK_DNS::m_bDoCacheUnknownNames= wkp.GetInt(REGKEY_SOCKETUNIT,CACHEUNKNOWNNAMES,TRUE);

	m_bAllowAnyHost = wkp.GetInt(REGKEY_SOCKETUNIT,ALLOWANYHOST,TRUE);

	if (m_bAllowAnyHost) {
		WK_TRACE(_T("Allow any host.\n"));
	} else {
		WK_TRACE(_T("Allow hosts by filter rules.\n"));
	}

	m_bDenyNoHost = wkp.GetInt(REGKEY_SOCKETUNIT,DENYNOHOST,TRUE);
	if (m_bDenyNoHost) {
		WK_TRACE(_T("Deny no host.\n"));
	} else {
		WK_TRACE(_T("Deny hosts by filter rules.\n"));
	}

	m_bTraceSync = wkp.GetInt(REGKEY_SOCKETUNIT,_T("TraceSync"),FALSE);
	if (m_bTraceSync ) {
		WK_TRACE(_T("Trace Sync.\n"));
		SET_WK_STAT_TICK_COUNT(1);
	} else {
		// no message if disabled
	}

	int nStatisticValues = wkp.GetInt(SOCKET_UNIT_STATISTICS, STAT_VALUES, -1);
	if (nStatisticValues==-1)
	{
		wkp.WriteInt(SOCKET_UNIT_STATISTICS, STAT_VALUES, 0);
		wkp.WriteInt(SOCKET_UNIT_STATISTICS, STAT_TIMER, 1000);
		wkp.WriteInt(SOCKET_UNIT_STATISTICS, STAT_MAXIMUM, 100); // MBits
	}
	else if (nStatisticValues)
	{
		m_pSendRate = new CStatistics(nStatisticValues);
		m_pReceiveRate = new CStatistics(nStatisticValues);
	}

	m_saAllow.RemoveAll();
	m_saDeny.RemoveAll();
	int i,c;
	CString sKey,sValue;

	c = wkp.GetInt(REGKEY_ALLOW,NUMBEROFALLOW,0);
	for (i=0;i<c;i++)
	{
		sKey.Format(_T("Allow%d"),i);
		sValue = wkp.GetString(REGKEY_ALLOW,sKey,_T(""));
		if (!sValue.IsEmpty())
		{
			m_saAllow.Add(sValue);
			WK_TRACE(_T("Allow rule[%2d]:%s\n"),m_saAllow.GetSize(),
				LPCTSTR(sValue));
		}
	}
	c = wkp.GetInt(REGKEY_DENY,NUMBEROFDENY,0);
	for (i=0;i<c;i++)
	{
		sKey.Format(_T("Deny%d"),i);
		sValue = wkp.GetString(REGKEY_DENY,sKey,_T(""));
		if (!sValue.IsEmpty())
		{
			m_saDeny.Add(sValue);
			WK_TRACE(_T("Deny  rule[%2d]:%s\n"),m_saDeny.GetSize(),
				LPCTSTR(sValue));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
void CSocketUnitApp::WriteRegistrySettings()
{
	CWK_Profile wkp;

	wkp.WriteInt(REGKEY_SOCKETUNIT,BEEPONINCOMING,m_bDoBeepOnIncoming);
	wkp.WriteInt(REGKEY_SOCKETUNIT,DOLISTEN,m_bDoListen);
	wkp.WriteInt(REGKEY_SOCKETUNIT,ALLOWOUTGOINGCALLS,m_bAllowOutgoingCalls);
	wkp.WriteInt(REGKEY_SOCKETUNIT,SYNCRETRYTIMEMS,CConnectionThread::m_dwTheSyncRetryTime);
	// DNS
	wkp.WriteInt(REGKEY_SOCKETUNIT,USEDNS,CWK_DNS::m_bUseDNS );
	wkp.WriteInt(REGKEY_SOCKETUNIT,CACHEKNOWNNAMES,CWK_DNS::m_bDoCacheKnownNames);
	wkp.WriteInt(REGKEY_SOCKETUNIT,CACHEUNKNOWNNAMES,CWK_DNS::m_bDoCacheUnknownNames);

	wkp.WriteInt(REGKEY_SOCKETUNIT,DEFAULTBITRATE,m_dwSelectedBitrate/1024 );


	// access control
	wkp.WriteInt(REGKEY_SOCKETUNIT,ALLOWANYHOST,m_bAllowAnyHost);
	wkp.WriteInt(REGKEY_SOCKETUNIT,DENYNOHOST,m_bDenyNoHost);
	int i,c;
	CString sKey,sValue;

	c = m_saAllow.GetSize();
	wkp.WriteInt(REGKEY_ALLOW,NUMBEROFALLOW,c);
	for (i=0;i<c;i++)
	{
		sKey.Format(_T("Allow%d"),i);
		wkp.WriteString(REGKEY_ALLOW,sKey,m_saAllow[i]);
	}
	c = m_saDeny.GetSize();
	wkp.WriteInt(REGKEY_DENY,NUMBEROFDENY,c);
	for (i=0;i<c;i++)
	{
		sKey.Format(_T("Deny%d"),i);
		wkp.WriteString(REGKEY_DENY,sKey,m_saDeny[i]);
	}
}
/////////////////////////////////////////////////////////////////////////////////////
BOOL CSocketUnitApp::DoLoginDialog()
{
	BOOL bRet=FALSE;
	CString sUser,sPassword;
	CWK_Profile wkp;

	COEMLoginDialog dlg;

	if (IDOK==dlg.DoModal())	  
	{
		bRet = TRUE;
	} 
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////////////
void CSocketUnitApp::OnCloseAll() 
{
	if (DoLoginDialog()==FALSE) {
		return;	// >>> EXIT >>>
	}

	POSITION pos = pDocTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CSocketUnitDoc* pDoc= (CSocketUnitDoc*)pDocTemplate->GetNextDoc(pos);
		if (pDoc) {
			pDoc->CloseAllConnections();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
BOOL CSocketUnitApp::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CSocketUnitDoc* pDoc= (CSocketUnitDoc*)pDocTemplate->GetNextDoc(pos);
		if (pDoc) 
		{
			pDoc->CheckConnections();
		}
		Sleep(10);
	}
	
	//return CWinApp::OnIdle(lCount);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////
BOOL CSocketUnitApp::GetStatistic(double& dSentMBperSec, double& dReceiveMBperSec, int& iSendQueue)
{
	if (m_pSendRate && m_pReceiveRate)
	{
		dSentMBperSec = m_pSendRate->GetMBperSecond();
		dReceiveMBperSec = m_pReceiveRate->GetMBperSecond();
		DWORD dwTick = GetTickCount();
		DWORD dwSpan = GetTimeSpan(m_pSendRate->GetLastTick(), dwTick);
		int i;
		if (dwSpan > 1000)
		{ 
			for (i=0; i<m_pSendRate->GetNumValues(); i++)
			{
				m_pSendRate->AddValue(0);
			}
		}
		dwSpan = GetTimeSpan(m_pReceiveRate->GetLastTick(), dwTick);
		if (dwSpan > 1000)
		{
			for (i=0; i<m_pSendRate->GetNumValues(); i++)
			{
				m_pReceiveRate->AddValue(0);
			}
		}
		iSendQueue = 0;
		CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
		if (m_pMainWnd)
		{
			CSocketUnitDoc* pDoc = (CSocketUnitDoc*)pMainFrame->GetActiveDocument();
			if (pDoc)
			{
				CConnectionThreads& threads = pDoc->GetConnectionThreads();
				for (int i=0;i<threads.GetSize();i++)
				{
					CConnectionThread* pThread = threads.GetAt(i);
					if (pThread)
					{
						iSendQueue += pThread->m_sendQueue.GetCount();
					}
				}
			}
		}
		return TRUE;
	}

	return FALSE;
}

#if _MFC_VER >= 0x0710
int CSocketUnitApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
	// TODO! RKE: extract resource for multilang
	// m_hLangResourceDLL may be initialized in CWinApp::InitInstance()
	// Do not call InitInstance() of the base class
//	CLoadResourceDll ResourceDll;
//	m_hLangResourceDLL = ResourceDll.DetachResourceDllHandle();
//	if (m_hLangResourceDLL == NULL)
//	{
//		WK_TRACE(_T("Did not find any ResourceDLL\n"));
//		ASSERT(FALSE);
//		return FALSE;
//	}
//	AfxSetResourceHandle(m_hLangResourceDLL);

	if (uCodePage == 0)
	{
		uCodePage = _ttoi(COemGuiApi::GetCodePage());
	}
	else
	{
		COemGuiApi::ChangeLanguageResource();
	}

//  Set the CodePage for MBCS conversion, if necessary
	CWK_String::SetCodePage(uCodePage);
	CWK_Profile wkp;
	wkp.SetCodePage(CWK_String::GetCodePage());

//	if (dwCPbits == 0)
//	{
//		dwCPbits = COemGuiApi::GetCodePageBits();
//	}
//	if (dwCPbits)
//	{
//		SetFontFaceNamesFromCodePageBits(dwCPbits);
//	}
//	else
//	{
//		CSkinDialog::DoUseGlobalFonts(FALSE);
//	}
	return 0;
}
#endif