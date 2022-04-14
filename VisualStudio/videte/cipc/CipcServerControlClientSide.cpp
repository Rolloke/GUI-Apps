/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcServerControlClientSide.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CipcServerControlClientSide.cpp $
// CHECKIN:		$Date: 4.11.04 15:22 $
// VERSION:		$Revision: 104 $
// LAST CHANGE:	$Modtime: 4.11.04 13:59 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdcipc.h"

#include "IPCFetch.h"
#include "CipcServerControlClientSide.h"
#include "WK_Names.h"

#include "wkclasses\wk_wincpp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const _TCHAR* NameOfEnum(ServerControlGroup scg)
{
	switch (scg) 
	{
		NAME_OF_ENUM(SC_NONE);
		NAME_OF_ENUM(SC_INPUT);
		NAME_OF_ENUM(SC_OUTPUT);
		NAME_OF_ENUM(SC_DATABASE);
		NAME_OF_ENUM(SC_CLIENT);
		NAME_OF_ENUM(SC_RESET_SERVER);
		NAME_OF_ENUM(SC_FILE_UPDATE);
		NAME_OF_ENUM(SC_DATA_CARRIER);
		NAME_OF_ENUM(SC_AUDIO);
		default:
			return _T("invalid ServerControlGroup");
	}
}

/*{CIPCServerControlClientSide|Overview,CIPCServerControlClientSide}
 {members|CIPCServerControlClientSide},
{CIPCServerControl} 
*/
void CIPCServerControlClientSide::Init()
{
	// m_evResult
	m_bCancel = FALSE;
	m_dwOptions=0;
	m_group=SC_NONE;
	// m_connection

	m_iNumWriteChannelRetries = 10;

	m_pData     = NULL;
	m_dwDataLen = 0;

#ifdef _UNICODE
	// CIPCServerControlClientSide sends initial request for connections
	// They have to be coded in ANSI for compatibility with older systems
	m_wCodePage = (WORD)CWK_String::GetCodePage();
#endif
}

/*Function: NOT YET DOCUMENTED */
CIPCServerControlClientSide::CIPCServerControlClientSide(
					ServerControlGroup g,
					LPCTSTR unitName,
					const CConnectionRecord &c
					)
					: CIPCServerControl(unitName, FALSE)
{
	Init ();

	m_group=g;
	m_connection = c;
	m_dwOptions=c.GetOptions();
}

/*Function: NOT YET DOCUMENTED */
CIPCServerControlClientSide::CIPCServerControlClientSide(
				LPCTSTR sShmName,
				int iDestination,
				const CString &sFileName,
				const void *pData,
				DWORD dwDataLen,
				BOOL bNeedsReboot
				) : CIPCServerControl(sShmName,FALSE)
{
	Init();

	m_group = SC_FILE_UPDATE;
	m_iDestination = iDestination;
	m_sFileName  = sFileName;
	m_pData = new BYTE[dwDataLen];
	CopyMemory(m_pData,pData,dwDataLen);
	m_dwDataLen = dwDataLen;
	m_bNeedsReboot = bNeedsReboot;
}

/*Function: NOT YET DOCUMENTED */
CIPCServerControlClientSide::~CIPCServerControlClientSide()
{
	WK_DELETE(m_pData);
}
//
void CIPCServerControlClientSide::OnReadChannelFound()
{
	if (!m_bCancel)
	{
		DoRequestConnection();
	}
}

void CIPCServerControlClientSide::OnIndicateError(DWORD dwCmd, 
												  CSecID id, 
												  CIPCError cipcError, 
												  int iErrorCode,
												  const CString &sErrorMessage)
{
	// NOT YET
	m_frResult = CIPCFetchResult(_T(""),
								 cipcError,
								 iErrorCode,
								 sErrorMessage,
								 0,
								 SECID_NO_ID
#ifdef _UNICODE
									, m_wCodePage
#endif
								 );
	m_evResult.SetEvent();
	WK_TRACE(_T("Connect failed %s %s/%d/%s\n"), NameOfEnum(m_group), 
										     NameOfEnum(cipcError), 
											 iErrorCode, 
											 LPCTSTR(sErrorMessage));
}

BOOL CIPCServerControlClientSide::OnRetryWriteChannel()
{
	// NOT YET
	m_frResult = CIPCFetchResult(_T(""),
								 CIPC_ERROR_UNABLE_TO_CONNECT,
								 5,
								 _T("CIPC Control Busy"),
								 0,
								 SECID_NO_ID
#ifdef _UNICODE
								 , m_wCodePage
#endif
								 );

	if (m_iNumWriteChannelRetries>0) 
	{
		m_iNumWriteChannelRetries--;
		int iRandom = (100*rand()) / RAND_MAX;
		Sleep(50+iRandom);
		WK_TRACE(_T("Busy, waiting %d\n"), 50+iRandom);
	} 
	else 
	{
		m_evResult.SetEvent();
		WK_TRACE(_T("Connect: busy\n"));
	}

	return FALSE;
}

void CIPCServerControlClientSide::Cancel()
{
	m_bCancel = TRUE;

	if (GetIPCState() == CIPC_STATE_CONNECTED)
	{
		DelayedDelete();
	}
}

void CIPCServerControlClientSide::OnConfirmConnection()
{
	if (!m_bCancel)
	{
		// send message
		switch (m_group)
		{
		case SC_INPUT:
			DoRequestInputConnection(m_connection);
			break;
		case SC_OUTPUT:
			DoRequestOutputConnection(m_connection);
			break;
		case SC_DATABASE:
			DoRequestDataBaseConnection(m_connection);
			break;
		case SC_RESET_SERVER:
			DoRequestServerReset(m_connection);
			break;
		case SC_CLIENT:
			DoRequestAlarmConnection(m_connection);
			break;
		case SC_FILE_UPDATE:
			DoRequestFileUpdate(m_iDestination,
								m_sFileName,
								m_pData, m_dwDataLen,
								m_bNeedsReboot);
			break;
		case SC_DATA_CARRIER:
			DoRequestDCConnection(m_connection);
			break;
		case SC_AUDIO:
			DoRequestAudioConnection(m_connection);
			break;
		default:
			WK_TRACE_ERROR(_T("ServerControlClientSide: internal error, wrong group %d\n"),
				m_group);
			break;
		}
		// the response will come back in OnConfirmConnect
	}
}
void CIPCServerControlClientSide::OnConfirmInputConnection(CSecID assignedID,
														  const CString& sShmName,
														  const CString& sVersion,
														  DWORD dwOptions
#ifdef _UNICODE
														  , WORD wCodePage
#endif
														  )
{
	m_frResult = CIPCFetchResult(sShmName, 
							   CIPC_ERROR_OKAY, 
							   0,
							   sVersion,
							   dwOptions,
							   assignedID
#ifdef _UNICODE
							   , wCodePage
#endif
								);
	m_evResult.SetEvent();
}
void  CIPCServerControlClientSide::OnConfirmOutputConnection(CSecID assignedID,
															  const CString& sShmName,
															  const CString& sVersion,
															  DWORD dwOptions
#ifdef _UNICODE
															  , WORD wCodePage
#endif
															  )
{
	m_frResult = CIPCFetchResult(sShmName, 
							   CIPC_ERROR_OKAY, 
							   0,
							   sVersion,
							   dwOptions,
							   assignedID
#ifdef _UNICODE
							   , wCodePage
#endif
								);
	m_evResult.SetEvent();
}

void  CIPCServerControlClientSide::OnConfirmAudioConnection(CSecID assignedID,
															  const CString& sShmName,
															  const CString& sVersion,
															  DWORD dwOptions
#ifdef _UNICODE
															  , WORD wCodePage
#endif
															  )
{
	m_frResult = CIPCFetchResult(sShmName, 
							   CIPC_ERROR_OKAY, 
							   0,
							   sVersion,
							   dwOptions,
							   assignedID
#ifdef _UNICODE
							   , wCodePage
#endif
								);
	m_evResult.SetEvent();
}
void  CIPCServerControlClientSide::OnConfirmDataBaseConnection(CSecID assignedID,
															  const CString& sShmName,
															  const CString& sVersion,
															  DWORD dwOptions
#ifdef _UNICODE
															  , WORD wCodePage
#endif
															  )
{
	m_frResult = CIPCFetchResult(sShmName, 
							   CIPC_ERROR_OKAY, 
							   0,
							   sVersion,
							   dwOptions,
							   assignedID
#ifdef _UNICODE
							   , wCodePage
#endif
								);
	m_evResult.SetEvent();
}
void CIPCServerControlClientSide::OnConfirmDCConnection(const CString &shmNameDC,
							 						   const CString &sVersion,
													   DWORD dwOptions
#ifdef _UNICODE
													   , WORD wCodePage
#endif
													   )
{
	m_frResult = CIPCFetchResult(shmNameDC, 
							   CIPC_ERROR_OKAY, 
							   0,
							   sVersion,
							   dwOptions,
							   SECID_NO_ID
#ifdef _UNICODE
							   , wCodePage
#endif
								);
	m_evResult.SetEvent();
}
void  CIPCServerControlClientSide::OnConfirmServerReset()
{
	m_frResult = CIPCFetchResult(_T(""),
								 CIPC_ERROR_OKAY,
								 0,
								 _T(""),
								 0,
								 SECID_NO_ID
#ifdef _UNICODE
								 , m_wCodePage
#endif
								 );
	m_evResult.SetEvent();
}
void CIPCServerControlClientSide::OnConfirmAlarmConnection(const CString &shmNameInput,
														   const CString &shmNameOutput,
														   const CString &sBitrate,
				 										   const CString &sVersion,
														   DWORD dwOptions
#ifdef _UNICODE
														   , WORD wCodePage
#endif
														   )
{
	m_frResult = CIPCFetchResult(sBitrate,
								 CIPC_ERROR_OKAY,
								 0,
								 sVersion,
								 dwOptions,
								 SECID_NO_ID
#ifdef _UNICODE
								 , wCodePage
#endif
								 );
	m_evResult.SetEvent();
}

void CIPCServerControlClientSide::OnConfirmFileUpdate(const CString &sFileName)
{
	m_evResult.SetEvent();
}



void CIPCServerControlClientSide::CalcRemoteNames(
					const CString &sRemoteIn,
					CString & sUnitName, 
					CString & sModulName,
					ServerControlGroup group)
{
	CString sTcpTmp;

	// new reset style, allow direct acces to reset different servers
	if (group==SC_RESET_SERVER) 
	{
		if (sRemoteIn==WK_APP_NAME_SERVER) 
		{
			sUnitName = WK_SMServerControl;
			sModulName = WK_APP_NAME_SERVER;
		} 
		else if (sRemoteIn==WK_APP_NAME_LAUNCHER) 
		{
			sUnitName = SM_LAUNCHER;
			sModulName = WK_APP_NAME_LAUNCHER;
		} 
		else if (sRemoteIn==WK_APP_NAME_DB_SERVER) 
		{
			sUnitName = WK_SMDatabaseControl;
			sModulName = WK_APP_NAME_DB_SERVER;
		} 
		else if (sRemoteIn==WK_APP_NAME_SOCKETS) 
		{
			sUnitName = WK_SMSocketControl;
			sModulName = WK_APP_NAME_SOCKETS;
		} 
		else if (sRemoteIn==WK_APP_NAME_ISDN) 
		{
			sUnitName = WK_SMServerControlIsdn;
			sModulName = WK_APP_NAME_ISDN;
		} 
		else if (sRemoteIn==WK_APP_NAME_ISDN2) 
		{
			sUnitName = WK_SMServerControlIsdn2;
			sModulName = WK_APP_NAME_ISDN2;
		} 
		else if (sRemoteIn==WK_APP_NAME_PTUNIT) 
		{
			sUnitName = WK_SMServerControlPT;
			sModulName = WK_APP_NAME_PTUNIT;
		} 
		else if (sRemoteIn==WK_APP_NAME_PTUNIT2) 
		{
			sUnitName = WK_SMServerControlPT2;
			sModulName = WK_APP_NAME_PTUNIT2;
		} 
		else if (sRemoteIn==WK_APPNAME_TOBSUNIT) 
		{
			sUnitName = WK_SMServerControlTOBS;
			sModulName = WK_APPNAME_TOBSUNIT;
		} 
		else if (sRemoteIn==WK_APPNAME_FAXUNIT) 
		{
			sUnitName = WK_SMServerControlFAX;
			sModulName = WK_APPNAME_FAXUNIT;
		} 
		else if (sRemoteIn==WK_APPNAME_EMAILUNIT) 
		{
			sUnitName = WK_SMServerControlEMail;
			sModulName = WK_APPNAME_EMAILUNIT;
		} 
		else if (sRemoteIn==WK_APPNAME_SMSUNIT) 
		{
			sUnitName = WK_SMServerControlSMS;
			sModulName = WK_APPNAME_SMSUNIT;
		}
		else if (sRemoteIn==WK_APP_NAME_IDIP_CLIENT)
		{
			sUnitName = WK_SMClientControl;
			sModulName = WK_APP_NAME_IDIP_CLIENT;
		}
		else {
			// old style, just the server
			sUnitName = WK_SMServerControl;
			sModulName = WK_APP_NAME_SERVER;
		}
	} 
	else 
	{
		// no server reset
		if (sRemoteIn==LOCAL_LOOP_BACK) 
		{
			switch (group) 
			{
			case SC_INPUT:
			case SC_OUTPUT:
			case SC_AUDIO:
			case SC_RESET_SERVER:
				sUnitName = WK_SMServerControl;
				sModulName = WK_APP_NAME_SERVER;
				break;
			case SC_DATABASE:
				sUnitName = WK_SMDatabaseControl;
				sModulName = WK_APP_NAME_DB_SERVER;
				break;
			case SC_CLIENT:
				sUnitName = WK_SMClientControl;
				{
					CWK_Dongle dongle;
					if (   dongle.GetProductCode() == IPC_DTS
						|| dongle.GetProductCode() == IPC_DTS_IP_RECEIVER)
					{
						sModulName = WK_APP_NAME_DVCLIENT;
					}
					else
					{
#ifdef _UNICODE
						sModulName = WK_APP_NAME_IDIP_CLIENT;
#else
						sModulName = WK_APP_NAME_EXPLORER;
#endif
					}
				}
				break;
			case SC_DATA_CARRIER:
				sUnitName = WK_SMServerControlACDC;
				sModulName = WK_APP_NAME_ACDC;
				break;
			default:
				TRACE(_T("CalcRemoteName INVALID group %d\n"),group);
			}
		} 
		else 
		{
			// here we only need the first/next host to determine
			// which unit will handle the call
			// the units will do the real cut hostA!hostB
			CString sNextHost(sRemoteIn);
			sNextHost.TrimLeft();
			
			int nx = sNextHost.Find(ROUTING_CHAR);	
			if (nx != -1) 
			{		// is there a path ?
				sNextHost = sNextHost.Left(nx);	// up to the first !
			}

			if (sNextHost.Find(_T("tcp:"))!=-1) 
			{
				sUnitName = WK_SMSocketControl;
				sModulName = WK_APP_NAME_SOCKETS;
			}
			else if (sNextHost.Find(_T("dun:"))!=-1) 
			{
				sUnitName = WK_SMSocketControl;
				sModulName = WK_APP_NAME_SOCKETS;
			}
			else if (sNextHost.Find(_T("sms:"))!=-1) 
			{
				sUnitName = WK_SMServerControlSMS;
				sModulName = WK_APPNAME_SMSUNIT;
			}
			else if (sNextHost.Find(_T("mailto:"))!=-1) {
				sUnitName = WK_SMServerControlEMail;
				sModulName = WK_APPNAME_EMAILUNIT;
			}
			else if (sNextHost.Find(_T("fax:"))!=-1) {
				sUnitName = WK_SMServerControlFAX;
				sModulName = WK_APPNAME_FAXUNIT;
			}
			else if (sNextHost.Find(_T("vcs:"))!=-1) {
				sUnitName = WK_SMServerControlVcs;
				sModulName = WK_APPNAME_VCSUNIT;
			}
			else if (sNextHost.Find(_T("pres:"))!=-1) {
				sUnitName = WK_SMServerControlPres;
				sModulName = WK_APPNAME_PRESUNIT;
			}
			else if (sNextHost.Find(_T("tobs:"))!=-1) {
				sUnitName = WK_SMServerControlTOBS;
				sModulName = WK_APPNAME_TOBSUNIT;
			}else {
				sUnitName = WK_SMServerControlIsdn;
				sModulName = WK_APP_NAME_ISDN;
			}
		}
	}
}


/*{OldFetchInputConnection}
CString FetchInputConnection(
 LPCTSTR remote,	// NULL, or remote address
 {CSecID} &assignedID		// id on the server side
 DWORD dwTimeout //timeout used by client
)

 The result contains the name of the shared memory to use
for a client connection. The strings is empty, if the request failed.

FetchInputConnection creates a CIPCServerControlClientSide
and handles the communication to request an Input-client connection
from a server or link unit. 

 {FetchOutputConnection},{FetchDataBaseConnection},
{FetchAlarmConnection}
*/

void CIPCServerControlClientSide::ModuleNotRunning(CIPCFetchResult &result, const CString &sModule)
{
	CString sMsg;
	sMsg.Format(_T("module %s not running"), LPCTSTR(sModule)),

	WK_TRACE(_T("%s\n"), LPCTSTR(sMsg));

	result = CIPCFetchResult(_T(""),
							 CIPC_ERROR_UNABLE_TO_CONNECT,
							 1,
							 sMsg,
							 0,
							 SECID_NO_ID
#ifdef _UNICODE
							 , 0
#endif
);

}

CIPCFetchResult NewFetchInputConnection(const CConnectionRecord &cIn,
										int iNumWriteChannelRetries
										)
{
	CIPCFetch fetch;
	return fetch.FetchInput(cIn);
}
/*{FetchOutputConnection}
CString FetchOutputConnection(
 LPCTSTR remote,	// NULL, or remote address
 DWORD  dwOptions,		// see {options|SCO_XXX}
 DWORD &dwOptionsReturned,	// what the server returned
 {CSecID} &assignedID		// id on the server side
 DWORD dwTimeout //timeout used by client
)

 The result contains the name of the shared memory to use
for a clinet connection. The strings is empty, if the request failed.

FetchOutputConnection creates a CIPCServerControlClientSide
and handles the communication to request an output-client connection
from a server or link unit. 

 {FetchInputConnection},{FetchDataBaseConnection},
{FetchAlarmConnection}
*/
CString FetchOutputConnection(const CConnectionRecord &c,
							  DWORD &dwOptionsReturned,
							  CSecID &assignedID)
{
	CIPCFetchResult result = NewFetchOutputConnection(c);
	assignedID = result.GetAssignedID();
	dwOptionsReturned = result.GetOptions();
	return result.GetShmName();
}

CIPCFetchResult NewFetchOutputConnection(  const CConnectionRecord &cIn,
										 int iNumWriteChannelRetries
								)
{
	CIPCFetch fetch;
	return fetch.FetchOutput(cIn);
}


/*{FetchDataBaseConnection}
CString FetchDataBaseConnection(
 LPCTSTR remote,	// NULL, or remote address
 {CSecID} &assignedID		// id on the server side
 DWORD dwTimeout //timeout used by client
)

 The result contains the name of the shared memory to use
for a client connection. The strings is empty, if the request failed.

FetchDatabaseConnection creates a CIPCServerControlClientSide
and handles the communication to request an Database-client connection
from a server or link unit. 

 {FetchInputConnection},{FetchDataBaseConnection},
{FetchAlarmConnection}
*/

CIPCFetchResult NewFetchDataBaseConnection(const CConnectionRecord &cIn,
										   int iNumWriteChannelRetries
										   )
{
	CIPCFetch fetch;
	return fetch.FetchDatabase(cIn);
}

//////////////////////////////////////////////////////////

BOOL CIPCServerControlClientSide::FetchServerReset (const CConnectionRecord &c)
{
	CIPCFetch fetch;
	return fetch.FetchServerReset(c);
}

/*{FetchAlarmConnection}
CString FetchAlarmConnection(
 LPCTSTR remote,	// NULL, or remote address
 const CString &shmInput
 const CString &shmOutput
 CSecID camID
 DWORD dwTimeout //timeout used by client
)

 {FetchInputConnection},{FetchDataBaseConnection},
{FetchOutputConnection}
*/

BOOL CIPCServerControlClientSide::FetchAlarmConnection( const CConnectionRecord &cIn )
{
	CIPCFetch fetch;
	return fetch.FetchAlarmConnection(cIn).IsOkay();
}
BOOL CIPCServerControlClientSide::StartVision()
{
	WK_TRACE(_T("trying to launch vision for alarm\n"));

	BOOL bProcess = FALSE;
	DWORD dwStartTime = 0;
	DWORD dwIdleTime = 0;

	// use standard login
	CWK_Profile wkp;
	CUserArray users;
	users.Load(wkp);
	
	CString sPassword = _T("notfound");
	CWK_Dongle dongle;
	InternalProductCode ipc = dongle.GetProductCode();
	if (   ipc == IPC_DTS_RECEIVER
		|| ipc == IPC_TOBS_RECEIVER
		|| ipc == IPC_DTS_IP_RECEIVER
		)
	{
		sPassword.Empty();
	}
	else
	{
		BOOL bFound=FALSE;
		for (int i=0;bFound==FALSE && i<users.GetSize();i++) 
		{
			const CUser &oneUser = *(users[i]);
			CString sName = oneUser.GetName();	// sigh returns const char *
			if (sName==_T("Alarm")) 
			{
				sPassword = oneUser.GetPassword();	// sigh returns const char *
				bFound = TRUE;
			}
		}// end of loop over users
		
		if (bFound==FALSE) 
		{
			WK_TRACE(_T("StandardAlarmUser not found, can't activate alarm client\n"));
			return FALSE;	// <<< EXIT >>>
		}
	}

	// EXmembers
	CStartupInfo		startUpInfo;
	CProcessInformation prozessInformation;

    startUpInfo.dwFlags	      = STARTF_USESHOWWINDOW;
    startUpInfo.wShowWindow   = SW_SHOWNORMAL;

	CString sExePathName;
	
	if (ipc == IPC_DTS_IP_RECEIVER)
	{
		sExePathName = _T("DVClient.exe");
	}
	else
	{
#ifdef _UNICODE
		sExePathName = wkp.GetString(_T("SecurityLauncher\\Modules"), WK_APP_NAME_IDIP_CLIENT, _T("IdipClient.exe"));
#else
		sExePathName = wkp.GetString(_T("SecurityLauncher\\Modules"), WK_APP_NAME_EXPLORER, _T("Vision.exe"));
#endif
	}

	CString sWorkingDirectory = _T(".");
	dwStartTime = GetTickCount();

	CString sCommandLine;
	if (sPassword.IsEmpty())
	{
		sCommandLine.Format(_T("%s Alarm"),sExePathName);
	}
	else
	{
		sCommandLine.Format(_T("%s Alarm %s"),sExePathName,sPassword);
	}
	// OOPS NT might be different
	bProcess = CreateProcess(NULL, /// (LPCSTR)sExePathName,	// pointer to name of executable module 
							(LPTSTR)sCommandLine.GetBuffer(0),	// pointer to command line string
							(LPSECURITY_ATTRIBUTES)NULL,	// pointer to process security attributes 
							(LPSECURITY_ATTRIBUTES)NULL,	// pointer to thread security attributes 
							FALSE,	// handle inheritance flag 
							NORMAL_PRIORITY_CLASS,//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
							(LPVOID)NULL,	// pointer to new environment block 
							(LPCTSTR)sWorkingDirectory,	// pointer to current directory name 
							(LPSTARTUPINFO)&startUpInfo,	// pointer to STARTUPINFO 
							(LPPROCESS_INFORMATION)&prozessInformation 	// pointer to PROCESS_INFORMATION  
							);

	sCommandLine.ReleaseBuffer();

	if (bProcess) 
	{
		DWORD rWFII;
		rWFII=WaitForInputIdle(prozessInformation.hProcess,dwStartTime);
		if (rWFII==0)
		{
			dwIdleTime = GetTimeSpan(dwStartTime);	
			WK_TRACE(_T("started %d ms\n"), dwIdleTime);
		}
		else if(rWFII==WAIT_TIMEOUT)
		{
			WK_TRACE(_T("wait for input idle timeout %d\n"), dwStartTime);
		}
	} 
	else 
	{
		WK_TRACE(_T("could not start\n"));
	}
	
	return bProcess;
}



BOOL CIPCServerControlClientSide::FetchUpdate(int iDestination,
												const CString &sFileName,
												const void *pData,
												DWORD dwDataLen,
												BOOL bNeedsReboot
											 )
{
	CIPCFetch fetch;
	return fetch.FetchUpdate(iDestination,sFileName,pData,dwDataLen,bNeedsReboot);
}

